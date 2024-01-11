#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>

#include "game.h"
#include "piece.h"

void GameUpdate(GameState *state) {
  if (IsKeyPressed(KEY_R)) {
    GameReset(state);
  }
  if (IsKeyPressed(KEY_SPACE)) {
    state->isPaused = !state->isPaused;
  }
  if (state->isPaused) {
    return;
  }
  state->time += GetFrameTime();
  UpdateMusicStream(state->music);
  if (state->time >= GRAVITY_TIME) {
    bool reachedGround = PieceMoveDown(&state->currentPiece, state->board);
    if (reachedGround) {
      for (int i = 0; i < 4; i++) {
        const PieceConfiguration *blocks = &state->currentPiece.tetromino->rotations[state->currentPiece.rotationIndex];
        Vector2 blockPosition = Vector2Add(blocks->points[i], state->currentPiece.position);
        state->board[(int)blockPosition.y][(int)blockPosition.x] = (Block){state->currentPiece.tetromino->color, true};
      }
      state->currentPiece = PieceGetRandom();
    }
    state->time = 0;
  }
  // TODO: controls feel funky
  if (IsKeyPressedRepeat(KEY_X) || IsKeyPressed(KEY_X)) {
    PieceRotateClockwise(&state->currentPiece, state->board);
  }
  if (IsKeyPressedRepeat(KEY_Z) || IsKeyPressed(KEY_Z)) {
    PieceRotateCounterClockwise(&state->currentPiece, state->board);
  }
  if (IsKeyPressedRepeat(KEY_LEFT) || IsKeyPressed(KEY_LEFT)) {
    PieceMoveLeft(&state->currentPiece, state->board);
  }
  if (IsKeyPressedRepeat(KEY_RIGHT) || IsKeyPressed(KEY_RIGHT)) {
    PieceMoveRight(&state->currentPiece, state->board);
  }
  if (IsKeyPressedRepeat(KEY_DOWN) || IsKeyPressed(KEY_DOWN)) {
    state->time = GRAVITY_TIME;
  }
}

void GameDraw(GameState *state) {
  BeginDrawing();
  ClearBackground(BLACK);
  Rectangle playfield = {(WIDTH - BLOCK_LEN * COLUMNS) / 2.0f, HEIGHT / 20.0f, BLOCK_LEN * COLUMNS, BLOCK_LEN * ROWS};
  // playfield without the buffer area
  Rectangle shownPlayfield = {playfield.x, playfield.y + BUFFER_AREA, playfield.width, playfield.height - BUFFER_AREA};
  BeginScissorMode(shownPlayfield.x, shownPlayfield.y, shownPlayfield.width, shownPlayfield.height);
  PieceDraw(&state->currentPiece, (Vector2){playfield.x, playfield.y});
  GameDrawBoard(state->board, (Vector2){playfield.x, playfield.y});
  DrawRectangleLinesEx(shownPlayfield, 2, GRAY);
  EndScissorMode();
  EndDrawing();
}

void GameDrawBoard(Block board[ROWS][COLUMNS], Vector2 screenPosition) {
  for (int y = 0; y < ROWS; y++) {
    for (int x = 0; x < COLUMNS; x++) {
      if (board[y][x].occupied) {
        Vector2 blockPositionOnScreen = Vector2Add(Vector2Scale((Vector2){x, y}, BLOCK_LEN), screenPosition);
        DrawRectangleV(blockPositionOnScreen, BLOCK_SIZE, board[y][x].color);
      }
    }
  }
}

void GameInit(GameState *state) {
  Music music = LoadMusicStream("resources/Tetris_Theme_B_Orchestral_Cover.wav");
  SetMusicVolume(music, 0.05f);
  PlayMusicStream(music);

  state->music = music;
  GameReset(state);
}

void GameReset(GameState *state) {
  for (int i = 0; i < ROWS * COLUMNS; i++) {
    ((Block *)state->board)[i] = (Block){WHITE, false};
  }
  state->isPaused = false;
  state->previousPieceType = NULL;
  state->currentPiece = PieceGetRandom();
  state->nextPiece = PieceGetRandom();
  state->time = 0;
  SeekMusicStream(state->music, 0.0f);
}

void GameCleanup(GameState *state) { UnloadMusicStream(state->music); }
