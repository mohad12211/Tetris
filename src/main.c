#include "common.h"

Block board[ROWS][COLUMNS] = {0};

int main(void) {
  for (int i = 0; i < ROWS * COLUMNS; i++) {
    ((Block *)board)[i] = (Block){WHITE, false};
  }
  SetTraceLogLevel(LOG_WARNING);
  SetTargetFPS(60);
  InitWindow(WIDTH, HEIGHT, "Tetris");

  int i = 0;
  Piece I = {&tetrominoes[i], INITIAL_ROTATION, tetrominoes[i].spawnPosition};

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);

    if (IsKeyPressed(KEY_SPACE)) {
      i = (i + 1) % PIECE_COUNT;
    }

    if (IsKeyPressedRepeat(KEY_Z) || IsKeyPressed(KEY_X)) {
      PieceRotateClockwise(&I);
    }
    if (IsKeyPressedRepeat(KEY_Z) || IsKeyPressed(KEY_Z)) {
      PieceRotateCounterClockwise(&I);
    }

    if (IsKeyPressedRepeat(KEY_LEFT) || IsKeyPressed(KEY_LEFT)) {
      PieceMoveLeft(&I);
    }
    if (IsKeyPressedRepeat(KEY_RIGHT) || IsKeyPressed(KEY_RIGHT)) {
      PieceMoveRight(&I);
    }
    if (IsKeyPressedRepeat(KEY_DOWN) || IsKeyPressed(KEY_DOWN)) {
      PieceMoveDown(&I);
    }

    Rectangle playfield = {(WIDTH - BLOCK_LEN * COLUMNS) / 2.0f, HEIGHT / 20.0f, BLOCK_LEN * COLUMNS, BLOCK_LEN * ROWS};
    // playfield without the buffer area
    Rectangle shownPlayfield = {playfield.x, playfield.y + BUFFER_AREA, playfield.width, playfield.height - BUFFER_AREA};
    BeginScissorMode(shownPlayfield.x, shownPlayfield.y, shownPlayfield.width, shownPlayfield.height);
    PieceDraw(&I, (Vector2){playfield.x, playfield.y});
    DrawRectangleLinesEx(shownPlayfield, 2, GRAY);
    EndScissorMode();

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
