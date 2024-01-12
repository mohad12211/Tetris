#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "piece.h"

static void GameDrawBoard(Block board[ROWS][COLUMNS], Vector2 screenPosition);
static void GameReset(void);

static GameState state = {0};

void GameUpdate(void) {
  // State Controls
  if (IsKeyPressed(KEY_R)) {
    GameReset();
  }
  if (IsKeyPressed(KEY_E)) {
    state.isQOLMode = !state.isQOLMode;
  }
  if (IsKeyPressed(KEY_SPACE)) {
    state.isPaused = !state.isPaused;
  }
  if (IsKeyPressed(KEY_M)) {
    if (IsMusicStreamPlaying(state.music)) {
      PauseMusicStream(state.music);
    } else {
      ResumeMusicStream(state.music);
    }
  }
  if (state.isPaused) {
    return;
  }

  // Game Controls
  state.fallingTimer++;
  UpdateMusicStream(state.music);
  if (IsKeyPressed(KEY_X)) {
    PieceRotateClockwise(&state.currentPiece, state.board);
  }
  if (IsKeyPressed(KEY_Z)) {
    PieceRotateCounterClockwise(&state.currentPiece, state.board);
  }
  if (IsKeyDown(KEY_LEFT)) {
    if (state.keyTimers[KEY_LEFT_TIMER] < KEY_TIMER_SPEED && !IsKeyPressed(KEY_LEFT)) {
      state.keyTimers[KEY_LEFT_TIMER]++;
    } else {
      state.keyTimers[KEY_LEFT_TIMER] = 0;
      PieceMoveLeft(&state.currentPiece, state.board);
    }
  }
  if (IsKeyDown(KEY_RIGHT)) {
    if (state.keyTimers[KEY_RIGHT_TIMER] < KEY_TIMER_SPEED && !IsKeyPressed(KEY_RIGHT)) {
      state.keyTimers[KEY_RIGHT_TIMER]++;
    } else {
      state.keyTimers[KEY_RIGHT_TIMER] = 0;
      PieceMoveRight(&state.currentPiece, state.board);
    }
  }
  if (IsKeyDown(KEY_DOWN)) {
    if (state.keyTimers[KEY_DOWN_TIMER] < 1 && !IsKeyPressed(KEY_DOWN)) {
      state.keyTimers[KEY_DOWN_TIMER]++;
    } else {
      state.keyTimers[KEY_DOWN_TIMER] = 0;
      state.fallingTimer = FALLING_SPEED;
    }
  }
  if (IsKeyPressed(KEY_UP) && state.isQOLMode) {
    while (!PieceMoveDown(&state.currentPiece, state.board))
      ;
    state.fallingTimer = FALLING_SPEED;
  }

  if (state.fallingTimer < FALLING_SPEED) {
    return;
  }
  // Falling Logic
  state.fallingTimer = 0;
  bool reachedGround = PieceMoveDown(&state.currentPiece, state.board);
  if (!reachedGround) {
    return;
  }
  for (int i = 0; i < 4; i++) {
    const PieceConfiguration *blocks = &state.currentPiece.tetromino->rotations[state.currentPiece.rotationIndex];
    Vector2 blockPosition = Vector2Add(blocks->points[i], state.currentPiece.position);
    state.board[(int)blockPosition.y][(int)blockPosition.x] = (Block){state.currentPiece.tetromino->color, true};
  }
  // Clear full rows
  for (int row = 0; row < ROWS; row++) {
    bool isFull = true;
    for (int column = 0; column < COLUMNS; column++) {
      if (!state.board[row][column].occupied) {
        isFull = false;
        break;
      }
    }
    if (isFull) {
      state.linesCleared++;
      for (int column = 0; column < COLUMNS; column++) {
        state.board[row][column].occupied = false;
      }

      for (int rowAbove = row; rowAbove > 0; rowAbove--) {
        for (int column = 0; column < COLUMNS; column++) {
          state.board[rowAbove][column] = state.board[rowAbove - 1][column];
        }
      }
    }
  }
  // Check if player lost
  for (int i = 0; i < 4; i++) {
    const PieceConfiguration *blocks = &state.nextPiece.tetromino->rotations[state.nextPiece.rotationIndex];
    Vector2 blockPosition = Vector2Add(blocks->points[i], INITIAL_BOARD_POSITION);
    if (state.board[(int)blockPosition.y][(int)blockPosition.x].occupied) {
      GameReset();
      return;
    }
  }
  // Generate next piece
  state.currentPiece = state.nextPiece;
  state.currentPiece.position = INITIAL_BOARD_POSITION;
  state.nextPiece = PieceGetRandom(state.currentPiece.tetromino);
}

void GameDraw(void) {
  BeginDrawing();
  ClearBackground(BLACK);
  Rectangle playfield = {(WIDTH - BLOCK_LEN * COLUMNS) / 2.0f, HEIGHT / 20.0f, BLOCK_LEN * COLUMNS, BLOCK_LEN * ROWS};
  // playfield without the buffer area
  Rectangle shownPlayfield = {playfield.x, playfield.y + BUFFER_AREA, playfield.width, playfield.height - BUFFER_AREA};
  BeginScissorMode(shownPlayfield.x, shownPlayfield.y, shownPlayfield.width, shownPlayfield.height);
  PieceDraw(&state.currentPiece, (Vector2){playfield.x, playfield.y});
  GameDrawBoard(state.board, (Vector2){playfield.x, playfield.y});
  DrawRectangleLinesEx(shownPlayfield, 2, GRAY);
  EndScissorMode();
  if (state.isQOLMode) {
    Piece ghostPiece = state.currentPiece;
    while (!PieceMoveDown(&ghostPiece, state.board))
      ;
    PieceDrawGhost(&ghostPiece, (Vector2){playfield.x, playfield.y});
  }
  Rectangle nextPieceRect = {(WIDTH + BLOCK_LEN * COLUMNS) / 2.0f - 2.0f, HEIGHT / 3.0f, BLOCK_LEN * 5, BLOCK_LEN * 4};
  PieceDraw(&state.nextPiece, (Vector2){nextPieceRect.x, nextPieceRect.y});
  DrawRectangleLinesEx(nextPieceRect, 2, GRAY);

  Rectangle linesCounterRect = {playfield.x, playfield.y + 2.0f, BLOCK_LEN * COLUMNS, 2 * BLOCK_LEN};
  DrawRectangleLinesEx(linesCounterRect, 2, GRAY);
  char text[64];
  snprintf(text, 64, "LINES-%d", state.linesCleared);
  Vector2 measure = MeasureTextEx(GetFontDefault(), text, FONT_SIZE, FONT_SIZE / 10.0f);
  DrawText(text, linesCounterRect.x + (linesCounterRect.width - measure.x) / 2.0f,
           linesCounterRect.y + (linesCounterRect.height - measure.y) / 2.0f + 4.0f, FONT_SIZE, WHITE);

  EndDrawing();
}

void GameInit(void) {
  Music music = LoadMusicStream("resources/Tetris_Theme_B_Orchestral_Cover.wav");
  SetMusicVolume(music, 0.05f);
  PlayMusicStream(music);

  state.music = music;
  GameReset();
}

void GameCleanup(void) { UnloadMusicStream(state.music); }

static void GameReset(void) {
  for (int i = 0; i < ROWS * COLUMNS; i++) {
    ((Block *)state.board)[i] = (Block){WHITE, false};
  }
  memset(state.keyTimers, 0, KEY_TIMERS_COUNT * sizeof(int));
  state.isPaused = false;
  state.currentPiece = PieceGetRandom(NULL);
  state.currentPiece.position = INITIAL_BOARD_POSITION;
  state.nextPiece = PieceGetRandom(state.currentPiece.tetromino);
  state.fallingTimer = 0;
  state.linesCleared = 0;
  state.isQOLMode = false;
  SeekMusicStream(state.music, 0.0f);
}

static void GameDrawBoard(Block board[ROWS][COLUMNS], Vector2 screenPosition) {
  for (int y = 0; y < ROWS; y++) {
    for (int x = 0; x < COLUMNS; x++) {
      if (board[y][x].occupied) {
        Vector2 blockPositionOnScreen = Vector2Add(Vector2Scale((Vector2){x, y}, BLOCK_LEN), screenPosition);
        DrawRectangleV(blockPositionOnScreen, BLOCK_SIZE, board[y][x].color);
      }
    }
  }
}
