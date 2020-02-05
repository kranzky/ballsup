#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>

#include "SDL_FontCache.h"

//==============================================================================

#define NUM_ENTITIES 1000

typedef struct
{
  int x;
  int y;
  int size;
  Uint8 r;
  Uint8 g;
  Uint8 b;
} Entity;

//------------------------------------------------------------------------------

int main(int argc, char **argv)
{
  bool ready = false;
  bool quit = false;
  int width = 1280;
  int height = 720;
  SDL_Window *window = NULL;
  SDL_Renderer *screen = NULL;
  SDL_Surface *surface = NULL;
  SDL_Texture *ball = NULL;
  FC_Font *font = NULL;
  Entity * entities = NULL;
  char buffer[16];

  srand(time(NULL));
  
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0)
    fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
  else if (SDL_GameControllerAddMappingsFromFile("res/gamecontrollerdb.txt") < 0)
    fprintf(stderr, "SDL_GameControllerAddMappingsFromFile: %s\n", SDL_GetError());
  else if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
    fprintf(stderr, "SDL_SetHint: SDL_HINT_RENDER_SCALE_QUALITY\n");
  else if (!SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1"))
    fprintf(stderr, "SDL_SetHint: SDL_HINT_RENDER_VSYNC\n");
  else if (!SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "1"))
    fprintf(stderr, "SDL_SetHint: SDL_HINT_VIDEO_HIGHDPI_DISABLED\n");
  else if (!SDL_SetHint(SDL_HINT_VIDEO_MAC_FULLSCREEN_SPACES, "0"))
    fprintf(stderr, "SDL_SetHint: SDL_HINT_VIDEO_MAC_FULLSCREEN_SPACES\n");
  else if (SDL_ShowCursor(SDL_DISABLE) < 0)
    fprintf(stderr, "SDL_ShowCursor: %s\n", SDL_GetError());
  else if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
    fprintf(stderr, "IMG_Init: %s\n", IMG_GetError());
  else if (TTF_Init() != 0)
    fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
  else
    ready = true;

  SDL_JoystickEventState(SDL_ENABLE);

  window = SDL_CreateWindow("Balls Up!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_GRABBED);

  if (!window)
    fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
  else
  {
    SDL_GetWindowSize(window, &width, &height);
    screen = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  }

  if (!screen)
    fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
  else
  {
    surface = IMG_Load("res/ball.png");
  }

  if (!surface)
    fprintf(stderr, "IMG_Load: %s\n", IMG_GetError());
  else
  {
    ball = SDL_CreateTextureFromSurface(screen, surface);
    SDL_FreeSurface(surface);
  }

  if (!ball)
    fprintf(stderr, "SDL_CreateTextureFromSurface: %s\n", IMG_GetError());
  else
    font = FC_CreateFont();

  if (!font)
    fprintf(stderr, "FC_CreateFont\n");
  else if (FC_LoadFont(font, screen, "res/hack_font.ttf", 15, (SDL_Colour){0xAA, 0xAA, 0xAA, SDL_ALPHA_OPAQUE}, TTF_STYLE_NORMAL) <= 0)
    fprintf(stderr, "FC_LoadFont\n");

  SDL_Rect src = (SDL_Rect){0, 0, 1249, 1249};
  SDL_Rect dst = (SDL_Rect){width/2 - 256, height/2 - 256, 512, 512};

  entities = calloc(NUM_ENTITIES, sizeof(Entity));
  if (!entities)
    fprintf(stderr, "Allocate entities\n");
  else {
    for (int i = 0; i < NUM_ENTITIES; ++i) {
      Entity *entity = &entities[i];
      entity->size = rand() % 500 + 12;
      entity->x = rand() % (width + entity->size * 2) - entity->size;
      entity->y = rand() % (height + entity->size * 2) - entity->size;
      entity->r = rand() % 0xFF;
      entity->g = rand() % 0xFF;
      entity->b = rand() % 0xFF;
    }
  }

  uint32_t epoch = SDL_GetTicks();
  while (!quit)
  {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0)
    {
      switch (event.type)
      {
        case SDL_QUIT:
          quit = true;
          break;
        case SDL_KEYDOWN:
          switch (event.key.keysym.sym)
          {
            case SDLK_ESCAPE:
              quit = true;
              break;
          }
          break;
      }
    }
    SDL_RenderClear(screen);
    if (entities) {
      for (int i = 0; i < NUM_ENTITIES; ++i) {
        Entity *entity = &entities[i];
        dst.w = entity->size;
        dst.h = dst.w;
        dst.x = entity->x;
        dst.y = entity->y;
        if (SDL_SetTextureColorMod(ball, entity->r, entity->g, entity->b) < 0)
          fprintf(stderr, "SDL_SetTextureColorMod: %s\n", SDL_GetError());
        entity->size += 1;
        entity->x -= 1;
        entity->y -= 1;
        SDL_RenderCopy(screen, ball, &src, &dst);
      }
    }
    uint32_t delta = SDL_GetTicks() - epoch;
    snprintf(buffer, 16, "%3.0f", 1000.0f / delta);
    FC_Draw(font, screen, width - 64, 8, buffer);
    SDL_RenderPresent(screen);
    if (delta < 20)
      SDL_Delay(20 - delta);
    epoch += delta;
  }

  if (entities)
    free(entities);
  if (ball)
    SDL_DestroyTexture(ball);
  if (screen)
    SDL_DestroyRenderer(screen);
  if (window)
    SDL_DestroyWindow(window);

  TTF_Quit();
  IMG_Quit();
  SDL_Quit();

  return 0;
}
