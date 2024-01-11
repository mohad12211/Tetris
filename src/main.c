#include "common.h"
#include <raylib.h>
#include <raymath.h>

static void BoardDraw(Vector2 screenPosition);

GameState gameState = {0};

static void BoardDraw(Vector2 screenPosition) {
  for (int y = 0; y < ROWS; y++) {
    for (int x = 0; x < COLUMNS; x++) {
      if (gameState.board[y][x].occupied) {
        Vector2 blockPositionOnScreen = Vector2Add(Vector2Scale((Vector2){x, y}, BLOCK_LEN), screenPosition);
        DrawRectangleV(blockPositionOnScreen, BLOCK_SIZE, gameState.board[y][x].color);
      }
    }
  }
}

int main(void) {
  for (int i = 0; i < ROWS * COLUMNS; i++) {
    ((Block *)gameState.board)[i] = (Block){WHITE, false};
  }
  SetTraceLogLevel(LOG_WARNING);
  InitAudioDevice();
  SetTargetFPS(60);
  InitWindow(WIDTH, HEIGHT, "Tetris");
  Music music = LoadMusicStream("resources/Tetris_Theme_B_Orchestral_Cover.wav");
  SetMusicVolume(music, 0.05f);
  PlayMusicStream(music);

  float time = 0;
  Piece fallingPiece = PieceGetRandom();

  while (!WindowShouldClose()) {
    UpdateMusicStream(music);
    if (time >= GRAVITY_TIME) {
      bool reachedGround = PieceMoveDown(&fallingPiece);
      if (reachedGround) {
        for (int i = 0; i < 4; i++) {
          const PieceConfiguration *blocks = &fallingPiece.tetromino->rotations[fallingPiece.rotationIndex];
          Vector2 blockPosition = Vector2Add(blocks->points[i], fallingPiece.position);
          gameState.board[(int)blockPosition.y][(int)blockPosition.x] = (Block){fallingPiece.tetromino->color, true};
        }
        fallingPiece = PieceGetRandom();
      }
      time = 0;
    }

    BeginDrawing();
    ClearBackground(BLACK);

    // TODO: controls feel funky
    if (IsKeyPressedRepeat(KEY_X) || IsKeyPressed(KEY_X)) {
      PieceRotateClockwise(&fallingPiece);
    }
    if (IsKeyPressedRepeat(KEY_Z) || IsKeyPressed(KEY_Z)) {
      PieceRotateCounterClockwise(&fallingPiece);
    }
    if (IsKeyPressedRepeat(KEY_LEFT) || IsKeyPressed(KEY_LEFT)) {
      PieceMoveLeft(&fallingPiece);
    }
    if (IsKeyPressedRepeat(KEY_RIGHT) || IsKeyPressed(KEY_RIGHT)) {
      PieceMoveRight(&fallingPiece);
    }
    if (IsKeyPressedRepeat(KEY_DOWN) || IsKeyPressed(KEY_DOWN)) {
      time = GRAVITY_TIME;
    }

    Rectangle playfield = {(WIDTH - BLOCK_LEN * COLUMNS) / 2.0f, HEIGHT / 20.0f, BLOCK_LEN * COLUMNS, BLOCK_LEN * ROWS};
    // playfield without the buffer area
    Rectangle shownPlayfield = {playfield.x, playfield.y + BUFFER_AREA, playfield.width, playfield.height - BUFFER_AREA};
    BeginScissorMode(shownPlayfield.x, shownPlayfield.y, shownPlayfield.width, shownPlayfield.height);
    PieceDraw(&fallingPiece, (Vector2){playfield.x, playfield.y});
    BoardDraw((Vector2){playfield.x, playfield.y});
    DrawRectangleLinesEx(shownPlayfield, 2, GRAY);
    EndScissorMode();

    EndDrawing();
    time += GetFrameTime();
  }

  UnloadMusicStream(music);
  CloseWindow();
  CloseAudioDevice();
  return 0;
}
