#include "./headers/maze.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
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
#define CELL_WIDTH 45

#define PLAYER_SPEED 300
#define FOV 1.5

uint64_t lasttick = 0;
uint64_t tick = 0;
double dt = 0;

struct {
  SDL_Window *window;
  SDL_Texture *texture;
  SDL_Renderer *renderer;
  u_int32_t pixels[WINDOW_WIDTH * WINDOW_HEIGHT];
  bool quit;
  int px;
  int py;
  float pa;
  int speed;
  cell maze[MAZE_HEIGHT][MAZE_WIDTH];
} state;

void currentPos(int *coord) {
  coord[0] = (int)(state.px / CELL_WIDTH);
  coord[1] = (int)(state.py / CELL_WIDTH);
}

void drawBackground() {
  for (int p = 0; p < WINDOW_WIDTH * WINDOW_HEIGHT; p++) {
    state.pixels[p] = (u_int32_t)0xFF000000;
  }
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
    if (cx >= 0 && cy >= 0 && cx < MAZE_WIDTH && cy < MAZE_HEIGHT &&
        state.maze[cy][cx].lower) {
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

    if (cx >= 0 && cy >= 0 && cx < MAZE_WIDTH && cy < MAZE_HEIGHT &&
        state.maze[cy][cx].right) {
      break;
    }
    vrx += vox;
    vry += voy;
    i++;
  }

  coords[0] = vrx;
  coords[1] = vry;
}

void draw3D(int *rays, int len) {}

void castRays() {
  SDL_SetRenderDrawColor(state.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  int pos[2];
  float angle;
  int ray_length;
  static float apc = FOV / WINDOW_WIDTH;
  currentPos(pos);

  for (int c = 0; c < WINDOW_WIDTH; c++) {

    angle = apc * c + (state.pa - FOV / 2.0);
    if (angle > 2 * pi) {
      angle -= 2 * pi;
    } else if (angle < 0) {
      angle += 2 * pi;
    }

    int h_coords[2];
    castHoriWalls(h_coords, pos, angle);
    int hlen = rayLength(h_coords[0], h_coords[1]);

    int v_coords[2];
    castVertWalls(v_coords, pos, angle);
    int vlen = rayLength(v_coords[0], v_coords[1]);

    if (hlen < vlen) {
      ray_length = hlen;
      SDL_RenderLine(state.renderer, state.px, state.py, h_coords[0] + state.px,
                     h_coords[1] + state.py);
    } else {
      ray_length = vlen;
      SDL_RenderLine(state.renderer, state.px, state.py, v_coords[0] + state.px,
                     v_coords[1] + state.py);
    }
    SDL_SetRenderDrawColor(state.renderer, 0xFF, 0x00, 0x00, 0xFF);
    int col_height;
    float ca = state.pa - angle;
    if (ca > 2 * pi) {
      ca -= 2 * pi;
    } else if (angle < 0) {
      ca += 2 * pi;
    }
    ray_length *= SDL_cosf(ca);
    col_height = (CELL_WIDTH * WINDOW_HEIGHT) / ray_length;
    SDL_FRect square = {.x = c,
                        .y = (int)(WINDOW_HEIGHT - col_height) / 2.0,
                        .w = 1,
                        .h = col_height};
    SDL_RenderFillRect(state.renderer, &square);
  }
}

void drawPlayer() {
  SDL_SetRenderDrawColor(state.renderer, 0x00, 0x00, 0xFF, 0xFF);
  SDL_FRect p_rect = {.x = state.px - 5, .y = state.py - 5, 10, 10};
  SDL_RenderFillRect(state.renderer, &p_rect);
}

void drawMap() {
  SDL_SetRenderDrawColor(state.renderer, 0xFF, 0x00, 0x00, 0xFF);

  for (int row = 0; row < MAZE_HEIGHT; row++) {
    for (int col = 0; col < MAZE_WIDTH; col++) {
      if (state.maze[row][col].right) {
        SDL_FRect square = {.x = (col + 1) * CELL_WIDTH,
                            .y = row * CELL_WIDTH,
                            .w = 1,
                            .h = CELL_WIDTH + 1};
        SDL_RenderFillRect(state.renderer, &square);
      }
      if (state.maze[row][col].lower) {
        SDL_FRect square = {.x = col * CELL_WIDTH,
                            .y = (row + 1) * CELL_WIDTH,
                            .w = CELL_WIDTH + 1,
                            .h = 1};
        SDL_RenderFillRect(state.renderer, &square);
      }
    }
  }
}

void keyboardEvent() {
  const _Bool *key_state = SDL_GetKeyboardState(NULL);
  int dp = ((double)PLAYER_SPEED * dt);
  int pos[2];
  currentPos(pos);
  cell current_cell = state.maze[pos[1]][pos[0]];
  cell upper_cell = state.maze[pos[1] - 1][pos[0]];
  cell left_cell = state.maze[pos[1]][pos[0] - 1];

  if (key_state[SDL_SCANCODE_W] &&
      !(state.py <= (CELL_WIDTH * (pos[1])) + 10 && upper_cell.lower)) {
    state.py -= dp;
  }
  if (key_state[SDL_SCANCODE_S] &&
      !(state.py >= (CELL_WIDTH * (pos[1] + 1)) - 5 && current_cell.lower)) {
    state.py += dp;
  }
  if (key_state[SDL_SCANCODE_A] &&
      !(state.px <= (CELL_WIDTH * (pos[0])) + 10 && left_cell.right)) {
    state.px -= dp;
  }
  if (key_state[SDL_SCANCODE_D] &&
      !(state.px >= (CELL_WIDTH * (pos[0] + 1)) - 5 && current_cell.right)) {
    state.px += dp;
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

  state.texture = SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_ABGR8888,
                                    SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH,
                                    WINDOW_HEIGHT);
  state.py = 10;
  maze_height = MAZE_HEIGHT;
  maze_width = MAZE_WIDTH;
  initMaze(state.maze);
  genMaze(state.maze);

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
    if (state.pa > pi * 2) {
      state.pa = pi * 2 - state.pa;
    } else if (state.pa < 0) {
      state.pa = pi * 2 + state.pa;
    }
  }
  return SDL_APP_CONTINUE; /* carry on with the program! */
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  keyboardEvent();
  lasttick = tick;
  tick = SDL_GetTicks();
  dt = ((double)tick - (double)lasttick) / (double)1000.0;
  // printf("fps: %f\n", 1 / dt);
  SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 255);
  SDL_RenderClear(state.renderer);
  SDL_UpdateTexture(state.texture, NULL, state.pixels, WINDOW_WIDTH * 4);
  SDL_RenderTextureRotated(state.renderer, state.texture, NULL, NULL, 0, 0,
                           SDL_FLIP_VERTICAL);
  drawMap();
  drawPlayer();
  castRays();

  SDL_RenderPresent(state.renderer);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  /* SDL will clean up the window/renderer for us. */
}
