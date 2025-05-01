#include "./headers/maze.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_surface.h>
#include <stdint.h>
#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define WINDOW_HEIGHT 480
#define WINDOW_WIDTH 640
#define MAZE_HEIGHT 11
#define MAZE_WIDTH 15
#define CELL_WIDTH 40

uint64_t lasttick = 0;
uint64_t tick = 0;
float dt = 0;

struct {
  SDL_Window *window;
  SDL_Texture *texture;
  SDL_Renderer *renderer;
  u_int32_t pixels[WINDOW_WIDTH * WINDOW_HEIGHT];
  bool quit;
  int px;
  int py;
  int pa;
  int speed;
  cell maze[MAZE_HEIGHT][MAZE_WIDTH];
} state;

void drawBackground() {
  for (int p = 0; p < WINDOW_WIDTH * WINDOW_HEIGHT; p++) {
    state.pixels[p] = (u_int32_t)0xFF000000;
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
  maze_height = MAZE_HEIGHT;
  maze_width = MAZE_WIDTH;
  initMaze(state.maze);
  genMaze(state.maze);
  printMaze(state.maze);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
  } else if (event->type == SDL_EVENT_KEY_DOWN) {
    if (event->key.key == SDLK_W) {
      state.py -= 10;
    }
    if (event->key.key == SDLK_S) {
      state.py += 10;
    }
    if (event->key.key == SDLK_A) {
      state.px -= 10;
    }
    if (event->key.key == SDLK_D) {
      state.px += 10;
    }
  }
  return SDL_APP_CONTINUE; /* carry on with the program! */
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  lasttick = tick;
  tick = SDL_GetTicks();
  dt = (tick - lasttick) / 1000.0;
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
