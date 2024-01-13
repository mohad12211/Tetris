#include <raylib.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

static void UpdateDrawFrame(void);

#include "game.h"

int main(void) {
  SetTraceLogLevel(LOG_WARNING);

  InitAudioDevice();
  InitWindow(WIDTH, HEIGHT, "Tetris");
  GameInit();

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    UpdateDrawFrame();
  }
#endif

  GameCleanup();
  CloseWindow();
  CloseAudioDevice();

  return 0;
}

static void UpdateDrawFrame(void) {
  GameUpdate();
  GameDraw();
  DrawFPS(5, 5);
}
