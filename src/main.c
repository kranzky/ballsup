#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>

#include <raylib.h>

//==============================================================================

#define FOCAL_LENGTH 1000
#define NUM_ENTITIES 300

typedef struct
{
  double x;
  double y;
  double z;
  double size;
} Entity;

//------------------------------------------------------------------------------

int compare(const void * a, const void * b) {
   return (((Entity*)b)->z - ((Entity*)a)->z);
}

int main(int argc, char **argv)
{
  int width = 1280;
  int height = 720;
  Entity* entities = NULL;

  srand(time(NULL));
  
  InitWindow(width, height, "Balls Up!");
  SetTargetFPS(60);

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

  while (!WindowShouldClose())
  {
    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();
		camera.z += 100;
    if (camera.z > 10000)
      camera.z = -20000;
  }

  if (entities)
    free(entities);

  CloseWindow();

  return 0;
}
