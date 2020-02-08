#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include <raylib.h>

//==============================================================================

#define FOCAL_LENGTH 1000
#define NUM_ENTITIES 1000

typedef struct
{
  Vector3 position;
  double size;
  Color color;
  bool rotate;
} Entity;

//------------------------------------------------------------------------------

int compare(const void *a, const void *b)
{
  return (((Entity *)b)->position.z - ((Entity *)a)->position.z);
}

Vector3 Vector3RotateByQuaternion(Vector3 v, Quaternion q)
{
  Vector3 result = {0};

  result.x = v.x * (q.x * q.x + q.w * q.w - q.y * q.y - q.z * q.z) + v.y * (2 * q.x * q.y - 2 * q.w * q.z) + v.z * (2 * q.x * q.z + 2 * q.w * q.y);
  result.y = v.x * (2 * q.w * q.z + 2 * q.x * q.y) + v.y * (q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z) + v.z * (-2 * q.w * q.x + 2 * q.y * q.z);
  result.z = v.x * (-2 * q.w * q.y + 2 * q.x * q.z) + v.y * (2 * q.w * q.x + 2 * q.y * q.z) + v.z * (q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z);

  return result;
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
      entity->rotate = false;
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
        entities[i].position.x = x * 250;
        entities[i].position.y = y * 250;
        entities[i].position.z = z * 250;
        entities[i].rotate = true;
      }
    }
  }

  float roll = 0.01;
  float pitch = 0.02;
  float yaw = 0.03;

  Quaternion q = {0};

  float x0 = cosf(roll * 0.5f);
  float x1 = sinf(roll * 0.5f);
  float y0 = cosf(pitch * 0.5f);
  float y1 = sinf(pitch * 0.5f);
  float z0 = cosf(yaw * 0.5f);
  float z1 = sinf(yaw * 0.5f);

  q.x = x1 * y0 * z0 - x0 * y1 * z1;
  q.y = x0 * y1 * z0 + x1 * y0 * z1;
  q.z = x0 * y0 * z1 - x1 * y1 * z0;
  q.w = x0 * y0 * z0 + x1 * y1 * z1;

  Vector3 camera = {0, 0, -2000};

  while (!WindowShouldClose())
  {
    qsort(entities, NUM_ENTITIES, sizeof(Entity), compare);
    if (IsGamepadAvailable(GAMEPAD_PLAYER1))
    {
      camera.x += 10 * GetGamepadAxisMovement(GAMEPAD_PLAYER1, GAMEPAD_AXIS_LEFT_X);
      camera.y += 10 * GetGamepadAxisMovement(GAMEPAD_PLAYER1, GAMEPAD_AXIS_LEFT_Y);
      camera.z += 10 * GetGamepadAxisMovement(GAMEPAD_PLAYER1, GAMEPAD_AXIS_RIGHT_TRIGGER);
      camera.z -= 10 * GetGamepadAxisMovement(GAMEPAD_PLAYER1, GAMEPAD_AXIS_LEFT_TRIGGER);
    }
    if (camera.z > 10000)
      camera.z = -20000;
    if (camera.z < -25000)
      camera.z = 10000;
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
      if (entity->rotate)
      {
        entity->position = Vector3RotateByQuaternion(entity->position, q);
      }
      DrawTexturePro(ball, src, dst, origin, 0.0, entity->color);
    }
    DrawLine(0, height / 2, width, height / 2, GRAY);
    DrawLine(width / 2, 0, width / 2, height, GRAY);
    DrawFPS(10, 10);
    EndDrawing();
  }

  if (entities)
    free(entities);

  CloseWindow();

  return 0;
}
