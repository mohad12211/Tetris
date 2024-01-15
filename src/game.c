#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "piece.h"
#include "util.h"

static void GameDrawBoard(Block board[ROWS][COLUMNS], Vector2 screenPosition);
static void GameReset(void);

static const int scoringTable[4] = {40, 100, 300, 1200};
static const float fallingSpeedTable[30] = {0.800f, 0.715f, 0.632f, 0.549f, 0.466f, 0.383f, 0.300f, 0.216f, 0.133f, 0.100f,
                                            0.083f, 0.083f, 0.083f, 0.067f, 0.067f, 0.067f, 0.050f, 0.050f, 0.050f, 0.033f,
                                            0.033f, 0.033f, 0.033f, 0.033f, 0.033f, 0.033f, 0.033f, 0.033f, 0.033f, 0.016f};
static GameState state = {0};

void GameUpdate(void) {
  float time = GetFrameTime();
  switch (state.screenState) {
  case SCREEN_START: {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      int levelBoxSpacing = BLOCK_LEN / 4.0f;
      float levelBoxLen = BLOCK_LEN * 1.5f;
      int totalWidth = 10 * levelBoxLen + 9 * levelBoxSpacing;
      int chosenLevel = (GetMouseX() - ((WIDTH - totalWidth) / 2.0f)) / (levelBoxSpacing + levelBoxLen);
      Rectangle levelBox = {(WIDTH - totalWidth) / 2.0f + chosenLevel * (levelBoxLen + levelBoxSpacing), HEIGHT / 2.0f, levelBoxLen,
                            levelBoxLen};
      if (chosenLevel >= 0 && chosenLevel <= 9 && CheckCollisionPointRec(GetMousePosition(), levelBox)) {
        if (IsKeyDown(KEY_X)) {
          chosenLevel += 10;
        }
        state.screenState = SCREEN_PLAY;
        state.startingLevel = chosenLevel;
        state.currentLevel = chosenLevel;
        PlayMusicStream(state.music);
      }
    }
    break;
  }
  case SCREEN_PLAY: {
    // State Controls
    if (IsKeyPressed(KEY_R)) {
      GameReset();
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
      break;
    }

    // Game Controls
    UpdateMusicStream(state.music);
    if (IsKeyPressed(KEY_X)) {
      PieceRotateClockwise(&state.currentPiece, state.board);
    }
    if (IsKeyPressed(KEY_Z)) {
      PieceRotateCounterClockwise(&state.currentPiece, state.board);
    }
    if (IsKeyDown(KEY_LEFT)) {
      if (WithinHalf(state.keyTimers[KEY_LEFT_TIMER], KEY_TIMER_SPEED) || IsKeyPressed(KEY_LEFT)) {
        state.keyTimers[KEY_LEFT_TIMER] = 0;
        PieceMoveLeft(&state.currentPiece, state.board);
      } else if (state.keyTimers[KEY_LEFT_TIMER] < KEY_TIMER_SPEED) {
        state.keyTimers[KEY_LEFT_TIMER] += time;
      }
    } else {
      state.keyTimers[KEY_LEFT_TIMER] = 0;
    }
    if (IsKeyDown(KEY_RIGHT)) {
      if (WithinHalf(state.keyTimers[KEY_RIGHT_TIMER], KEY_TIMER_SPEED) || IsKeyPressed(KEY_RIGHT)) {
        state.keyTimers[KEY_RIGHT_TIMER] = 0;
        PieceMoveRight(&state.currentPiece, state.board);
      } else if (state.keyTimers[KEY_RIGHT_TIMER] < KEY_TIMER_SPEED) {
        state.keyTimers[KEY_RIGHT_TIMER] += time;
      }
    } else {
      state.keyTimers[KEY_RIGHT_TIMER] = 0;
    }

    const float fallingSpeed = fallingSpeedTable[MIN(state.currentLevel, 29)];
    if (IsKeyDown(KEY_DOWN)) {
      if (WithinHalf(state.keyTimers[KEY_DOWN_TIMER], KEY_DOWN_TIMER_SPEED) || IsKeyPressed(KEY_DOWN)) {
        state.fallingTimer = fallingSpeed;
        state.softDropCounter++;
        state.keyTimers[KEY_DOWN_TIMER] = 0;
      } else if (state.keyTimers[KEY_DOWN_TIMER] < KEY_DOWN_TIMER_SPEED) {
        state.keyTimers[KEY_DOWN_TIMER] += time;
      }
    } else {
      state.keyTimers[KEY_DOWN_TIMER] = 0;
      state.softDropCounter = 0;
    }

    if (state.fallingTimer < fallingSpeed) {
      // FIXME: should this line be inside the condition or before it?
      state.fallingTimer += time;
      break;
    }

    // Falling Logic
    state.fallingTimer = 0;
    bool reachedGround = PieceMoveDown(&state.currentPiece, state.board);
    if (!reachedGround) {
      break;
    }
    state.score += MAX(0, state.softDropCounter - 1);
    state.softDropCounter = 0;
    for (int i = 0; i < 4; i++) {
      const PieceConfiguration *blocks = &state.currentPiece.tetromino->rotations[state.currentPiece.rotationIndex];
      Vector2 blockPosition = Vector2Add(blocks->points[i], state.currentPiece.position);
      state.board[(int)blockPosition.y][(int)blockPosition.x] = (Block){state.currentPiece.tetromino->color, true};
    }
    // Clear full rows
    int clearedRows = 0;
    for (int row = 0; row < ROWS; row++) {
      bool isFull = true;
      for (int column = 0; column < COLUMNS; column++) {
        if (!state.board[row][column].occupied) {
          isFull = false;
          break;
        }
      }
      if (isFull) {
        clearedRows++;
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
    if (clearedRows > 0) {
      state.linesCleared += clearedRows;
      int transitionPoint = (state.startingLevel + 1) * 10;
      if (state.linesCleared >= transitionPoint) {
        state.currentLevel = (state.linesCleared - transitionPoint) / 10 + state.startingLevel + 1;
      }
      state.score += scoringTable[clearedRows - 1] * (state.currentLevel + 1);
    }
    // Check if player lost
    for (int i = 0; i < 4; i++) {
      const PieceConfiguration *blocks = &state.nextPiece.tetromino->rotations[state.nextPiece.rotationIndex];
      Vector2 blockPosition = Vector2Add(blocks->points[i], INITIAL_BOARD_POSITION);
      if (state.board[(int)blockPosition.y][(int)blockPosition.x].occupied) {
        GameReset();
        break;
      }
    }
    // Generate next piece
    state.currentPiece = state.nextPiece;
    state.currentPiece.position = INITIAL_BOARD_POSITION;
    state.nextPiece = PieceGetRandom(state.currentPiece.tetromino);
    state.keyTimers[KEY_DOWN_TIMER] = KEY_DOWN_TIMER_SPEED + 1;
    break;
  }
  }
}

void GameDraw(void) {
  BeginDrawing();
  ClearBackground(BLACK);
  switch (state.screenState) {
  case SCREEN_START: {
    const char *startText = "Select level to start";
    Vector2 startTextMeasure = MeasureTextEx(GetFontDefault(), startText, 60, 60 / 10.0f);
    DrawText(startText, (WIDTH - startTextMeasure.x) / 2.0f, HEIGHT / 2.5f, 60, WHITE);
    int levelBoxSpacing = BLOCK_LEN / 4.0f;
    float levelBoxLen = BLOCK_LEN * 1.5f;
    int totalWidth = 10 * levelBoxLen + 9 * levelBoxSpacing;
    for (int i = 0; i < 10; i++) {
      Rectangle levelBox = {(WIDTH - totalWidth) / 2.0f + i * (levelBoxLen + levelBoxSpacing), HEIGHT / 2.0f, levelBoxLen, levelBoxLen};
      if (CheckCollisionPointRec(GetMousePosition(), levelBox)) {
        DrawRectangleRec(levelBox, ORANGE);
      }
      char levelString[2] = {i + '0', 0};
      Vector2 levelStringMeasure = MeasureTextEx(GetFontDefault(), levelString, 40, 40 / 10.0f);
      DrawText(levelString, levelBox.x + (levelBox.width - levelStringMeasure.x) / 2.0f,
               levelBox.y + (levelBox.height - levelStringMeasure.y) / 2.0f, 40, MAROON);
      DrawRectangleLinesEx(levelBox, 2, GREEN);
    }
    break;
  }
  case SCREEN_PLAY: {
    Rectangle playfield = {(WIDTH - BLOCK_LEN * COLUMNS) / 2.0f, HEIGHT / 20.0f, BLOCK_LEN * COLUMNS, BLOCK_LEN * ROWS};
    // playfield without the buffer area
    Rectangle shownPlayfield = {playfield.x, playfield.y + BUFFER_AREA, playfield.width, playfield.height - BUFFER_AREA};

    BeginScissorMode(shownPlayfield.x, shownPlayfield.y, shownPlayfield.width, shownPlayfield.height);
    PieceDraw(&state.currentPiece, (Vector2){playfield.x, playfield.y});
    GameDrawBoard(state.board, (Vector2){playfield.x, playfield.y});
    DrawRectangleLinesEx(shownPlayfield, 2, GRAY);
    EndScissorMode();

    Rectangle nextPieceRect = {(WIDTH + BLOCK_LEN * COLUMNS) / 2.0f - 2.0f, HEIGHT / 3.0f, BLOCK_LEN * 5, BLOCK_LEN * 4};
    PieceDraw(&state.nextPiece, (Vector2){nextPieceRect.x, nextPieceRect.y});
    DrawRectangleLinesEx(nextPieceRect, 2, GRAY);

    Rectangle linesCounterRect = {playfield.x, playfield.y + 2.0f, BLOCK_LEN * COLUMNS, 2 * BLOCK_LEN};
    DrawRectangleLinesEx(linesCounterRect, 2, GRAY);
    char clearedLinesSting[64];
    snprintf(clearedLinesSting, 64, "LINES-%d", state.linesCleared);
    Vector2 clearedLinesStringMeasure = MeasureTextEx(GetFontDefault(), clearedLinesSting, FONT_SIZE, FONT_SIZE / 10.0f);
    DrawText(clearedLinesSting, linesCounterRect.x + (linesCounterRect.width - clearedLinesStringMeasure.x) / 2.0f,
             linesCounterRect.y + (linesCounterRect.height - clearedLinesStringMeasure.y) / 2.0f, FONT_SIZE, WHITE);

    Rectangle levelRect = {(WIDTH + BLOCK_LEN * COLUMNS) / 2.0f - 2.0f, HEIGHT / 1.7f, BLOCK_LEN * 5, BLOCK_LEN * 2 + 5.0f};
    DrawRectangleLinesEx(levelRect, 2, GRAY);
    DrawText("LEVEL", levelRect.x + (levelRect.width - MeasureText("LEVEL", 40.0f)) / 2.0f, levelRect.y + 5.0f, 40.0f, WHITE);
    char currentLevelString[64];
    snprintf(currentLevelString, 64, "%d", state.currentLevel);
    DrawText(currentLevelString, levelRect.x + (levelRect.width - MeasureText(currentLevelString, 40.0f)) / 2.0f,
             levelRect.y + BLOCK_LEN + 5.0f, 40.0f, WHITE);

    Rectangle scoreRect = {(WIDTH + BLOCK_LEN * COLUMNS) / 2.0f - 2.0f, shownPlayfield.y, BLOCK_LEN * 6, BLOCK_LEN * 2 + 5.0f};
    DrawRectangleLinesEx(scoreRect, 2, GRAY);
    DrawText("SCORE", scoreRect.x + (scoreRect.width - MeasureText("SCORE", 40.0f)) / 2.0f, scoreRect.y + 5.0f, 40.0f, WHITE);
    char scoreString[64];
    snprintf(scoreString, 64, "%09d", state.score);
    Vector2 scoreStringMeasure = MeasureTextEx(GetFontDefault(), scoreString, 40.0f, 40.0f / 10);
    DrawText(scoreString, scoreRect.x + (scoreRect.width - scoreStringMeasure.x) / 2.0f, scoreRect.y + BLOCK_LEN + 5.0f, 40.0f, WHITE);

    break;
  }
  }

  EndDrawing();
}

void GameInit(void) {
  // Music music = LoadMusicStream("resources/Tetris_Theme_B_Orchestral_Cover.wav");
  Music music = LoadMusicStream("resources/Tetris_Theme_B_Orchestral_Cover.wav");
  SetMusicVolume(music, 0.05f);

  state.music = music;
  GameReset();
}

void GameCleanup(void) { UnloadMusicStream(state.music); }

static void GameReset(void) {
  for (int i = 0; i < ROWS * COLUMNS; i++) {
    ((Block *)state.board)[i] = (Block){WHITE, false};
  }
  memset(state.keyTimers, 0, KEY_TIMERS_COUNT * sizeof(float));
  state.screenState = SCREEN_START;
  state.startingLevel = 0;
  state.currentLevel = 0;
  state.score = 0;
  state.isPaused = false;
  state.currentPiece = PieceGetRandom(NULL);
  state.currentPiece.position = INITIAL_BOARD_POSITION;
  state.nextPiece = PieceGetRandom(state.currentPiece.tetromino);
  state.fallingTimer = 0;
  state.linesCleared = 0;
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
