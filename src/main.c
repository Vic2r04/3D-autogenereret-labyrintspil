#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define WINDOW_HEIGHT 480
#define WINDOW_WIDTH 640

struct {
  SDL_Window *window;
  SDL_Texture *texture;
  SDL_Renderer *renderer;
  u_int32_t pixels[WINDOW_WIDTH * WINDOW_HEIGHT];
  bool quit;
} state;

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

  return SDL_APP_CONTINUE; /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
  }
  return SDL_APP_CONTINUE; /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate) {
  for (int p = 0; p < WINDOW_WIDTH * WINDOW_HEIGHT; p++) {
    if (p > WINDOW_WIDTH * WINDOW_HEIGHT / 2) {
      state.pixels[p] = (u_int32_t)0xFF000000;
    } else {
      state.pixels[p] = (u_int32_t)0xFFFF0000;
    }
  }
  SDL_UpdateTexture(state.texture, NULL, state.pixels, WINDOW_WIDTH * 4);
  SDL_RenderTextureRotated(state.renderer, state.texture, NULL, NULL, 0, 0,
                           SDL_FLIP_VERTICAL);

  SDL_RenderPresent(state.renderer); /* put it all on the screen! */
  return SDL_APP_CONTINUE;           /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  /* SDL will clean up the window/renderer for us. */
}
