#include <raylib.h>

#include "game.h"

int main(void) {
  SetTraceLogLevel(LOG_WARNING);
  SetTargetFPS(60);
  InitAudioDevice();
  InitWindow(WIDTH, HEIGHT, "Tetris");
  GameState gameState = {0};
  GameInit(&gameState);

  while (!WindowShouldClose()) {
    GameUpdate(&gameState);
    GameDraw(&gameState);
  }

  GameCleanup(&gameState);
  CloseWindow();
  CloseAudioDevice();
  return 0;
}
