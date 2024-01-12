#include <raylib.h>

#include "game.h"

int main(void) {
  SetTraceLogLevel(LOG_WARNING);
  SetTargetFPS(60);

  InitAudioDevice();
  InitWindow(WIDTH, HEIGHT, "Tetris");
  GameInit();

  while (!WindowShouldClose()) {
    GameUpdate();
    GameDraw();
  }

  GameCleanup();
  CloseWindow();
  CloseAudioDevice();

  return 0;
}
