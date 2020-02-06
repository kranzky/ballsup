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
  Vector3 position;
  double size;
  Color color;
} Entity;

//------------------------------------------------------------------------------

int compare(const void *a, const void *b)
{
  return (((Entity *)b)->position.z - ((Entity *)a)->position.z);
}

int main(int argc, char **argv)
{
  int width = 1280;
  int height = 720;
  Entity *entities = NULL;

  srand(time(NULL));

  InitWindow(width, height, "Balls Up!");
  Texture2D ball = LoadTexture("res/ball.png");
  SetTargetFPS(60);

  Rectangle src = {0, 0, ball.width, ball.height};
  Vector2 origin = {100, 100};
  Rectangle dst = {width / 2, height / 2, 200, 200};

  entities = calloc(NUM_ENTITIES, sizeof(Entity));
  if (!entities)
    fprintf(stderr, "Allocate entities\n");
  else
  {
    for (int i = 0; i < NUM_ENTITIES; ++i)
    {
      Entity *entity = &entities[i];
      entity->size = 256; // rand() % 240 + 16;
      entity->position.x = rand() % 10000 - rand() % 10000;
      entity->position.y = rand() % 10000 - rand() % 10000;
      entity->position.z = rand() % 10000 - rand() % 10000;
      entity->color.r = rand() % 0xFF;
      entity->color.g = rand() % 0xFF;
      entity->color.b = rand() % 0xFF;
      entity->color.a = 0xFF;
    }
  }

  for (int z = -1; z <= 1; ++z)
  {
    for (int y = -1; y <= 1; ++y)
    {
      for (int x = -1; x <= 1; ++x)
      {
        int i = (x + 1) + 3 * (y + 1) + 9 * (1 - z);
        if (i >= NUM_ENTITIES)
          continue;
        entities[i].position.x = x * 256;
        entities[i].position.y = y * 256;
        entities[i].position.z = z * 256;
      }
    }
  }

  qsort(entities, NUM_ENTITIES, sizeof(Entity), compare);

  Vector3 camera = {0, 0, -20000};

  while (!WindowShouldClose())
  {
    BeginDrawing();
    ClearBackground(BLACK);
    for (int i = 0; i < NUM_ENTITIES; ++i)
    {
      Entity *entity = &entities[i];
      double d = entity->position.z - camera.z;
      if (d <= 1)
        continue;
      double x = entity->position.x - camera.x;
      double y = entity->position.y - camera.y;
      dst.width = FOCAL_LENGTH * entity->size / d;
      dst.height = dst.width;
      dst.x = FOCAL_LENGTH * x / d + width / 2;
      dst.y = FOCAL_LENGTH * y / d + height / 2;
      origin.x = dst.width / 2;
      origin.y = dst.height / 2;
      DrawTexturePro(ball, src, dst, origin, 0.0, entity->color);
    }
    DrawLine(0, height / 2, width, height / 2, GRAY);
    DrawLine(width / 2, 0, width / 2, height, GRAY);
    DrawFPS(10, 10);
    EndDrawing();
    camera.z += 50;
    camera.y += 1;
    if (camera.z > 10000)
      camera.z = -20000;
  }

  if (entities)
    free(entities);

  CloseWindow();

  return 0;
}
