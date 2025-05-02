#include "./headers/maze.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_surface.h>
#include <math.h>
#include <stdint.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define pi 3.1415

#define WINDOW_HEIGHT 480
#define WINDOW_WIDTH 640
#define MAZE_HEIGHT 11
#define MAZE_WIDTH 15
#define CELL_WIDTH 40

#define PLAYER_SPEED 30

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

void drawBackground() {
  for (int p = 0; p < WINDOW_WIDTH * WINDOW_HEIGHT; p++) {
    state.pixels[p] = (u_int32_t)0xFF000000;
  }
}

void castRays() {
  int length = 50;
  int end_y = cosf(state.pa) * length + state.py;
  int end_x = sinf(state.pa) * length + state.px;
  SDL_RenderLine(state.renderer, state.px, state.py, end_x, end_y);
}

void drawPlayer() {
  SDL_SetRenderDrawColor(state.renderer, 0x00, 0x00, 0xFF, 0xFF);
  SDL_FRect p_rect = {.x = state.px - 5, .y = state.py - 5, 10, 10};
  SDL_RenderFillRect(state.renderer, &p_rect);
  castRays();
}

void drawMap() {
  SDL_SetRenderDrawColor(state.renderer, 0xFF, 0x00, 0x00, 0xFF);

  for (int row = 0; row < MAZE_HEIGHT; row++) {
    for (int col = 0; col < MAZE_WIDTH; col++) {
      if (state.maze[row][col].right) {
        SDL_FRect square = {.x = (col + 1) * CELL_WIDTH,
                            .y = row * CELL_WIDTH,
                            .w = 5,
                            .h = CELL_WIDTH + 5};
        SDL_RenderFillRect(state.renderer, &square);
      }
      if (state.maze[row][col].lower) {
        SDL_FRect square = {.x = col * CELL_WIDTH,
                            .y = (row + 1) * CELL_WIDTH,
                            .w = CELL_WIDTH + 5,
                            .h = 5};
        SDL_RenderFillRect(state.renderer, &square);
      }
    }
  }
}

void currentPos(int *coord) {
  coord[0] = (int)(state.px / CELL_WIDTH);
  coord[1] = (int)(state.py / CELL_WIDTH);
}

void keyboardEvent() {
  const _Bool *key_state = SDL_GetKeyboardState(NULL);
  int dp = PLAYER_SPEED * dt;
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

  if (!SDL_CreateWindowAndRenderer("Maze game", 640, 480, 0, &state.window,
                                   &state.renderer)) {
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
      state.pa -= 0.06;
    }
    if (event->motion.xrel > 0) {
      state.pa += 0.06;
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
  dt = ((double)tick - (double)lasttick) / 100;
  drawBackground();
  SDL_UpdateTexture(state.texture, NULL, state.pixels, WINDOW_WIDTH * 4);
  SDL_RenderTextureRotated(state.renderer, state.texture, NULL, NULL, 0, 0,
                           SDL_FLIP_VERTICAL);
  drawMap();
  drawPlayer();

  SDL_RenderPresent(state.renderer);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  /* SDL will clean up the window/renderer for us. */
}
