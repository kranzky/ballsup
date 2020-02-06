#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>

#include "SDL_FontCache.h"

//==============================================================================

#define FOCAL_LENGTH 1000
#define NUM_ENTITIES 300

typedef struct
{
  double x;
  double y;
  double z;
  double size;
  Uint8 r;
  Uint8 g;
  Uint8 b;
} Entity;

//------------------------------------------------------------------------------

int compare(const void * a, const void * b) {
   return (((Entity*)b)->z - ((Entity*)a)->z);
}

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
//else if (!SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1"))
//  fprintf(stderr, "SDL_SetHint: SDL_HINT_RENDER_BATCHING\n");
//else if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
//  fprintf(stderr, "SDL_SetHint: SDL_HINT_RENDER_SCALE_QUALITY\n");
//else if (!SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1"))
//  fprintf(stderr, "SDL_SetHint: SDL_HINT_RENDER_VSYNC\n");
//else if (!SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "1"))
//  fprintf(stderr, "SDL_SetHint: SDL_HINT_VIDEO_HIGHDPI_DISABLED\n");
//else if (!SDL_SetHint(SDL_HINT_VIDEO_MAC_FULLSCREEN_SPACES, "0"))
//  fprintf(stderr, "SDL_SetHint: SDL_HINT_VIDEO_MAC_FULLSCREEN_SPACES\n");
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
      entity->size = 256; // rand() % 240 + 16;
      entity->x = rand() % 10000 - rand() % 10000;
      entity->y = rand() % 10000  - rand() % 10000;
      entity->z = rand() % 10000  - rand() % 10000;
      entity->r = rand() % 0xFF;
      entity->g = rand() % 0xFF;
      entity->b = rand() % 0xFF;
    }
  }

	for (int z=-1; z<= 1; ++z) {
		for (int y=-1; y<= 1; ++y) {
			for (int x=-1; x<= 1; ++x) {
				int i = (x+1) + 3*(y+1) + 9*(1-z);
				entities[i].x = x * 256;
				entities[i].y = y * 256;
				entities[i].z = z * 256;
			}
		}
	}

  qsort(entities, NUM_ENTITIES, sizeof(Entity), compare);

  Entity camera = {0, 0, -20000};

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
        double d = entity->z - camera.z;
        if (d <= 1)
          continue;
        double x = entity->x - camera.x;
        double y = entity->y - camera.y;
        dst.w = FOCAL_LENGTH * entity->size / d;
        dst.h = dst.w;
        dst.x = FOCAL_LENGTH * (x-entity->size/2) / d + width/2;
        dst.y = FOCAL_LENGTH * (y-entity->size/2) / d + height/2;
        if (SDL_SetTextureColorMod(ball, entity->r, entity->g, entity->b) < 0)
          fprintf(stderr, "SDL_SetTextureColorMod: %s\n", SDL_GetError());
        SDL_RenderCopy(screen, ball, &src, &dst);
      }
    }
    uint32_t delta = SDL_GetTicks() - epoch;
    if (delta < 20) {
      SDL_Delay(20 - delta);
      delta = 20;
    }
    snprintf(buffer, 16, "%3.0f", 1000.0f / delta);
    FC_Draw(font, screen, width - 64, 8, buffer);
    SDL_RenderPresent(screen);
    epoch = SDL_GetTicks();
		camera.z += 100;
    if (camera.z > 10000)
      camera.z = -20000;
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
