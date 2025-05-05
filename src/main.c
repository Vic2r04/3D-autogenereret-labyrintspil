#include "./headers/main.h"
#include "./headers/maze.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <math.h>
#include <stdint.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define pi 3.1415

#define WINDOW_HEIGHT 900
#define WINDOW_WIDTH 1200
#define MAZE_HEIGHT 11
#define MAZE_WIDTH 15
#define CELL_WIDTH 400
#define MAP_SCALE 12

#define PLAYER_SPEED 1200
#define FOV 1.0

uint64_t lasttick = 0;
uint64_t tick = 0;
double dt = 0;

cell maze[MAZE_HEIGHT][MAZE_WIDTH];

struct {
  SDL_Window *window;
  SDL_Texture *texture;
  SDL_Renderer *renderer;
  bool quit;
  int px;
  int py;
  float pa;
  int speed;
} state;

void currentPos(int *coord) {
  coord[0] = (int)(state.px / CELL_WIDTH);
  coord[1] = (int)(state.py / CELL_WIDTH);
}

int rayLength(int rx, int ry) { return (int)sqrtf(powf(rx, 2) + powf(ry, 2)); }

void castHoriWalls(int *coords, int pos[2], float angle) {
  int cy, cx, hoy, hox, i, hrx, hry;
  hrx = hry = i = 0;
  float atan;
  atan = -1 / SDL_tanf(angle);

  if (angle > pi) {
    hry = (pos[1] + 1) * CELL_WIDTH - state.py;
    hoy = CELL_WIDTH;
  } else {
    hry = (pos[1]) * CELL_WIDTH - state.py;
    hoy = -CELL_WIDTH;
  }
  hox = atan * hoy;
  hrx = hry * atan;

  while (i != MAZE_WIDTH && i != MAZE_HEIGHT) {

    cx = (int)(hrx + state.px) / CELL_WIDTH;
    cy = (int)(hry + state.py) / CELL_WIDTH;
    cy += -1;
    if (cx < 0 || cy < 0) {
      break;
    }
    if (cx >= 0 && cy >= 0 && cx < MAZE_WIDTH && cy < MAZE_HEIGHT &&
        maze[cy][cx].lower) {
      break;
    }
    hrx += hox;
    hry += hoy;
    i++;
  }

  coords[0] = hrx;
  coords[1] = hry;
}

void castVertWalls(int *coords, int pos[2], float angle) {
  int i, cx, cy, vry, vrx, vox, voy;

  float ntan = -SDL_tanf(angle);

  if (angle < pi / 2 || angle > 1.5 * pi) {
    vrx = (pos[0] + 1) * CELL_WIDTH - state.px;
    vox = CELL_WIDTH;
  } else {
    vrx = pos[0] * CELL_WIDTH - state.px;
    vox = -CELL_WIDTH;
  }
  voy = vox * ntan;
  vry = vrx * ntan;

  i = 0;
  while (i != MAZE_WIDTH && i != MAZE_HEIGHT) {
    cx = (int)(vrx + state.px) / CELL_WIDTH;
    cy = (int)(vry + state.py) / CELL_WIDTH;
    cx += -1;

    if (cx < 0 || cy < 0) {
      break;
    }
    if (cx >= 0 && cy >= 0 && cx < MAZE_WIDTH && cy < MAZE_HEIGHT &&
        maze[cy][cx].right) {
      break;
    }
    vrx += vox;
    vry += voy;
    i++;
  }

  coords[0] = vrx;
  coords[1] = vry;
}

void drawPlayer() {
  SDL_SetRenderDrawColor(state.renderer, 0x00, 0x00, 0xFF, 0xFF);
  int scaled_x = (state.px / (float)MAP_SCALE);
  int scaled_y = (state.py / (float)MAP_SCALE);

  SDL_FRect p_rect = {.x = scaled_x - 5, .y = scaled_y - 5, 10, 10};
  SDL_RenderFillRect(state.renderer, &p_rect);
}

void drawMap() {
  static int scaled_cell_width = CELL_WIDTH / MAP_SCALE;
  SDL_SetRenderDrawColor(state.renderer, 0x00, 0x00, 0x00, 0xFF);
  SDL_FRect mapBackground = {.x = 0,
                             .y = 0,
                             .w = scaled_cell_width * MAZE_WIDTH,
                             .h = scaled_cell_width * MAZE_HEIGHT};

  SDL_RenderFillRect(state.renderer, &mapBackground);
  SDL_SetRenderDrawColor(state.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  for (int row = 0; row < MAZE_HEIGHT; row++) {
    for (int col = 0; col < MAZE_WIDTH; col++) {
      if (maze[row][col].right) {
        SDL_FRect square = {.x = (col + 1) * scaled_cell_width,
                            .y = row * scaled_cell_width,
                            .w = 1,
                            .h = scaled_cell_width + 1};
        SDL_RenderFillRect(state.renderer, &square);
      }
      if (maze[row][col].lower) {
        SDL_FRect square = {.x = col * scaled_cell_width,
                            .y = (row + 1) * scaled_cell_width,
                            .w = scaled_cell_width + 1,
                            .h = 1};
        SDL_RenderFillRect(state.renderer, &square);
      }
    }
  }
}

void changeAngle(float *angle, float value) {
  *angle = value;
  if (*angle > 2 * pi) {
    *angle -= 2 * pi;
  } else if (*angle < 0) {
    *angle += 2 * pi;
  }
}

void castRays() {
  int pos[2];
  float angle;
  int ray_length;
  static float apc = FOV / WINDOW_WIDTH;
  currentPos(pos);

  for (int c = 0; c < WINDOW_WIDTH; c++) {
    changeAngle(&angle, apc * c + state.pa - FOV / 2.0);

    int h_coords[2];
    castHoriWalls(h_coords, pos, angle);
    int hlen = rayLength(h_coords[0], h_coords[1]);

    int v_coords[2];
    castVertWalls(v_coords, pos, angle);
    int vlen = rayLength(v_coords[0], v_coords[1]);

    if (hlen < vlen) {
      ray_length = hlen;
      SDL_SetRenderDrawColor(state.renderer, 0x00, 0xFF, 0x00, 0xFF);
    } else {
      ray_length = vlen;
      SDL_SetRenderDrawColor(state.renderer, 0x00, 0xAA, 0x00, 0xFF);
    }

    int col_height;
    float ca;
    changeAngle(&ca, state.pa - angle);
    ray_length = (int)((float)ray_length * SDL_cosf(ca));
    col_height = (CELL_WIDTH * WINDOW_HEIGHT) / ray_length;
    SDL_FRect square = {.x = c,
                        .y = (int)(WINDOW_HEIGHT - col_height) / 2.0,
                        .w = 1,
                        .h = col_height};
    SDL_RenderFillRect(state.renderer, &square);
  }
}

void keyboardEvent() {
  const _Bool *key_state = SDL_GetKeyboardState(NULL);
  float dx, dy;
  dx = dy = 0;

  if (key_state[SDL_SCANCODE_W]) {
    dx = SDL_cosf(2 * pi - state.pa);
    dy = SDL_sinf(2 * pi - state.pa);
  } else if (key_state[SDL_SCANCODE_S]) {
    dx = -SDL_cosf(2 * pi - state.pa);
    dy = -SDL_sinf(2 * pi - state.pa);
  } else {
    return;
  }

  int nx, ny;

  int dp = ((double)PLAYER_SPEED * dt);
  int pos[2];
  currentPos(pos);

  cell current_cell = maze[pos[1]][pos[0]];
  cell upper_cell = maze[pos[1] - 1][pos[0]];
  cell left_cell = maze[pos[1]][pos[0] - 1];

  nx = dx * dp + state.px;
  ny = dy * dp + state.py;

  if (dy < 0 && !(ny + dy - 100 <= (CELL_WIDTH * pos[1]) && upper_cell.lower) &&
      ny - 100 > 0) {
    state.py = ny;
  } else if (dy > 0 &&
             !(ny + 100 >= CELL_WIDTH * (pos[1] + 1) && current_cell.lower)) {
    state.py = ny;
  }
  if (dx < 0 && !(nx - 100 <= CELL_WIDTH * pos[0] && left_cell.right) &&
      nx - 100 > 0) {
    state.px = nx;
  } else if (dx > 0 &&
             !(nx + 100 >= CELL_WIDTH * (pos[0] + 1) && current_cell.right)) {
    state.px = nx;
  }
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  SDL_SetAppMetadata("Maze game", "1.0", "com.mazegame");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer("Maze game", WINDOW_WIDTH, WINDOW_HEIGHT, 0,
                                   &state.window, &state.renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_SetWindowRelativeMouseMode(state.window, 1);

  state.py = (int)(CELL_WIDTH / 2);
  state.px = (int)(CELL_WIDTH / 2);
  maze_height = MAZE_HEIGHT;
  maze_width = MAZE_WIDTH;
  initMaze(maze);
  genMaze(maze);
  printMaze(maze);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
  }
  if (event->type == SDL_EVENT_MOUSE_MOTION) {
    if (event->motion.xrel < 0) {
      state.pa -= 0.02;
    }
    if (event->motion.xrel > 0) {
      state.pa += 0.02;
    }
    changeAngle(&state.pa, state.pa);
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  keyboardEvent();
  lasttick = tick;
  tick = SDL_GetTicks();
  dt = ((double)tick - (double)lasttick) / (double)1000.0;
  // printf("fps: %f\n", 1 / dt);
  SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 255);
  SDL_RenderClear(state.renderer);
  SDL_RenderTextureRotated(state.renderer, state.texture, NULL, NULL, 0, 0,
                           SDL_FLIP_VERTICAL);
  castRays();
  drawMap();
  drawPlayer();

  SDL_RenderPresent(state.renderer);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  /* SDL will clean up the window/renderer for us. */
}
