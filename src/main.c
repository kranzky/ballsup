#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include <raylib.h>
#include <raymath.h>

//==============================================================================

#define FOCAL_LENGTH 1000
#define NUM_ENTITIES 1000
//#define SHOW_SPLASH 1

typedef struct
{
  Vector3 position;
  double size;
  Color color;
} Entity;

//------------------------------------------------------------------------------

int compare(const void *a, const void *b)
{
  Entity* entity_a = (Entity *)a;
  Entity* entity_b = (Entity *)b;
  return ((entity_b->position.z + entity_b->size/2) - (entity_a->position.z + entity_a->size/2));
}

int main(int argc, char **argv)
{
  int width = 1280;
  int height = 720;
  Entity *entities = NULL;

  srand(time(NULL));

  InitWindow(width, height, "Balls Up!");
  InitAudioDevice();

  BeginDrawing();
  DrawText("LOADING DATA", 10, 10, 20, DARKBLUE);
  EndDrawing();

  Texture2D ball = LoadTexture("./res/ball.png");

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
        entities[i].position.x = x * 250;
        entities[i].position.y = y * 250;
        entities[i].position.z = z * 250;
      }
    }
  }

  float roll = 0.01;
  float pitch = 0.02;
  float yaw = 0.03;

  Quaternion q = QuaternionFromEuler(roll, pitch, yaw);

  Vector3 camera_pos = {0, 0, -2000};

#ifdef SHOW_SPLASH
  Texture2D app = LoadTexture("./res/APP_logo.png");
  Texture2D kb = LoadTexture("./res/Kranzky_Brothers_logo.png");
  Music music = LoadMusicStream("./res/balls.mod");
  PlayMusicStream(music);
  float time = GetTime();
#endif

  while (!WindowShouldClose())
  {
#ifdef SHOW_SPLASH
    UpdateMusicStream(music);
    if (GetTime() - time < 10.4)
    {
      Texture2D *tex = NULL;
      if (GetTime() - time < 5.2)
        tex = &app;
      else
        tex = &kb;
      src.width = tex->width;
      src.height = tex->height;
      dst.width = src.width;
      dst.height = src.height;
      dst.x = width / 2;
      dst.y = height / 2;
      origin.x = dst.width / 2;
      origin.y = dst.height / 2;
      BeginDrawing();
      ClearBackground(WHITE);
      DrawTexturePro(*tex, src, dst, origin, 0.0, WHITE);
      DrawFPS(10, 10);
      EndDrawing();
      continue;
    }
#endif
    if (IsGamepadAvailable(GAMEPAD_PLAYER1))
    {
      camera_pos.x += 10 * GetGamepadAxisMovement(GAMEPAD_PLAYER1, GAMEPAD_AXIS_LEFT_X);
      camera_pos.y += 10 * GetGamepadAxisMovement(GAMEPAD_PLAYER1, GAMEPAD_AXIS_LEFT_Y);
      camera_pos.z += 10 * GetGamepadAxisMovement(GAMEPAD_PLAYER1, GAMEPAD_AXIS_RIGHT_TRIGGER);
      camera_pos.z -= 10 * GetGamepadAxisMovement(GAMEPAD_PLAYER1, GAMEPAD_AXIS_LEFT_TRIGGER);
    }
    if (camera_pos.z > 10000)
      camera_pos.z = -20000;
    if (camera_pos.z < -25000)
      camera_pos.z = 10000;
    src.width = ball.width;
    src.height = ball.height;
    BeginDrawing();
    ClearBackground(BLACK);
    qsort(entities, NUM_ENTITIES, sizeof(Entity), compare);
    for (int i = 0; i < NUM_ENTITIES; ++i)
    {
      Entity *entity = &entities[i];
      entity->position = Vector3RotateByQuaternion(entity->position, q);
      double x = entity->position.x - camera_pos.x;
      double y = entity->position.y - camera_pos.y;
      double z = entity->position.z - camera_pos.z;
      if (z <= 1)
        continue;
      dst.width = FOCAL_LENGTH * entity->size / z;
      dst.height = dst.width;
      dst.x = FOCAL_LENGTH * x / z + width / 2;
      dst.y = FOCAL_LENGTH * y / z + height / 2;
      origin.x = dst.width / 2;
      origin.y = dst.height / 2;
      DrawTexturePro(ball, src, dst, origin, 0.0, entity->color);
    }
    DrawLine(0, height / 2, width, height / 2, GRAY);
    DrawLine(width / 2, 0, width / 2, height, GRAY);
    DrawFPS(10, 10);
    EndDrawing();
  }

  if (entities)
    free(entities);

#ifdef SHOW_SPLASH
  UnloadMusicStream(music);
#endif

  CloseAudioDevice();
  CloseWindow();

  return 0;
}
