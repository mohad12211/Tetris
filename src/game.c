#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdlib.h>

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
  if (state.isPaused) {
    return;
  }

  // Game Controls
  state.time += GetFrameTime();
  UpdateMusicStream(state.music);
  // TODO: controls feel funky
  if (IsKeyPressedRepeat(KEY_X) || IsKeyPressed(KEY_X)) {
    PieceRotateClockwise(&state.currentPiece, state.board);
  }
  if (IsKeyPressedRepeat(KEY_Z) || IsKeyPressed(KEY_Z)) {
    PieceRotateCounterClockwise(&state.currentPiece, state.board);
  }
  if (IsKeyPressedRepeat(KEY_LEFT) || IsKeyPressed(KEY_LEFT)) {
    PieceMoveLeft(&state.currentPiece, state.board);
  }
  if (IsKeyPressedRepeat(KEY_RIGHT) || IsKeyPressed(KEY_RIGHT)) {
    PieceMoveRight(&state.currentPiece, state.board);
  }
  if (IsKeyPressedRepeat(KEY_DOWN) || IsKeyPressed(KEY_DOWN)) {
    state.time = INITIAL_FALLING_TIME;
  }
  if (IsKeyPressed(KEY_UP) && state.isQOLMode) {
    while (!PieceMoveDown(&state.currentPiece, state.board))
      ;
    state.time = INITIAL_FALLING_TIME;
  }

  if (state.time < INITIAL_FALLING_TIME) {
    return;
  }
  // Falling Logic
  state.time = 0;
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
  state.isPaused = false;
  state.currentPiece = PieceGetRandom(NULL);
  state.currentPiece.position = INITIAL_BOARD_POSITION;
  state.nextPiece = PieceGetRandom(state.currentPiece.tetromino);
  state.time = 0;
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
