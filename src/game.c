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
static void GameUpdateMusic(void);
static void GameHandleInput(void);
static int GameGetFullRowsCount(void);

static const int scoringTable[4] = {40, 100, 300, 1200};
static const float fallingSpeedTable[30] = {0.800f, 0.715f, 0.632f, 0.549f, 0.466f, 0.383f, 0.300f, 0.216f, 0.133f, 0.100f,
                                            0.083f, 0.083f, 0.083f, 0.067f, 0.067f, 0.067f, 0.050f, 0.050f, 0.050f, 0.033f,
                                            0.033f, 0.033f, 0.033f, 0.033f, 0.033f, 0.033f, 0.033f, 0.033f, 0.033f, 0.016f};
static GameState state = {0};

// TODO: add max score
void GameUpdate(void) {
  switch (state.screenState) {
  case SCREEN_START: {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      const float levelBoxSpacing = BLOCK_LEN / 4.0f;
      const float levelBoxLen = BLOCK_LEN * 1.5f;
      const float totalWidth = 10.0f * levelBoxLen + 9.0f * levelBoxSpacing;
      const int chosenLevel = (int)((GetMouseX() - ((WIDTH - totalWidth) / 2.0f)) / (levelBoxSpacing + levelBoxLen));
      const Rectangle levelBox = {(WIDTH - totalWidth) / 2.0f + chosenLevel * (levelBoxLen + levelBoxSpacing), HEIGHT / 2.0f, levelBoxLen,
                                  levelBoxLen};
      if (chosenLevel >= 0 && chosenLevel <= 9 && CheckCollisionPointRec(GetMousePosition(), levelBox)) {
        state.screenState = SCREEN_PLAY;
        state.startingLevel = chosenLevel;
        state.currentLevel = chosenLevel;
        if (IsKeyDown(KEY_X)) {
          state.startingLevel += 10;
          state.currentLevel += 10;
        }
      }
    }
    break;
  }
  case SCREEN_PLAY: {
    // State input Controls
    if (IsKeyPressed(KEY_R)) {
      GameReset();
      break;
    }
    if (IsKeyPressed(KEY_SPACE)) {
      state.isPaused = !state.isPaused;
    }
    if (IsKeyPressed(KEY_M)) {
      state.isMusicPaused = !state.isMusicPaused;
    }
    if (state.isPaused) {
      break;
    }

    GameUpdateMusic();
    const float dt = GetFrameTime();
    const float fallingSpeed = fallingSpeedTable[MIN(state.currentLevel, 29)];

    if (state.ARETimer <= 0.0f) {
      GameHandleInput();
    }

    if (state.fallingTimer < fallingSpeed) {
      // FIXME: should this line be inside the condition or before it?
      state.fallingTimer += dt;
      break;
    }

    // Dropping Logic
    const bool isDropped = PieceMoveDown(&state.currentPiece, state.board);
    if (isDropped) {
      state.fallingTimer = 0.0f;
      break;
    }

    // Locking Logic
    int lockRow = -1;
    for (int i = 0; i < 4; i++) {
      const PieceConfiguration *blocks = &state.currentPiece.tetromino->rotations[state.currentPiece.rotationIndex];
      const Vector2 blockPosition = Vector2Add(blocks->points[i], state.currentPiece.position);
      lockRow = MAX(lockRow, (int)blockPosition.y);
    }
    const float AREDelay = (int)(((((ROWS - lockRow - 1) + 2) / 4) * 2 + 10)) / 60.0f;
    if (state.ARETimer < AREDelay) {
      state.ARETimer += dt;
      break;
    }

    // Clear rows and update score and generate next piece
    for (int i = 0; i < 4; i++) {
      const PieceConfiguration *blocks = &state.currentPiece.tetromino->rotations[state.currentPiece.rotationIndex];
      Vector2 blockPosition = Vector2Add(blocks->points[i], state.currentPiece.position);
      state.board[(int)blockPosition.y][(int)blockPosition.x] = (Block){state.currentPiece.tetromino->shapeType, true};
    }

    int fullRowsCount = GameGetFullRowsCount();
    if (state.animationTimer <= 0.5f && fullRowsCount > 0) {
      if (FloatEquals(state.animationTimer, 0)) {
        PlaySound(state.sounds[SOUND_LINECLEAR]);
      }
      state.animationTimer += dt;
      break;
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

    // Update score and lines cleared
    if (fullRowsCount > 0) {
      state.linesCleared += fullRowsCount;
      const int transitionPoint = (state.startingLevel + 1) * 10;
      if (state.linesCleared >= transitionPoint) {
        state.currentLevel = (state.linesCleared - transitionPoint) / 10 + state.startingLevel + 1;
      }
      state.score += scoringTable[fullRowsCount - 1] * (state.currentLevel + 1);
    }

    // Check if player lost
    for (int i = 0; i < 4; i++) {
      const PieceConfiguration *blocks = &state.nextPiece.tetromino->rotations[state.nextPiece.rotationIndex];
      const Vector2 blockPosition = Vector2Add(blocks->points[i], INITIAL_BOARD_POSITION);
      if (state.board[(int)blockPosition.y][(int)blockPosition.x].occupied) {
        PlaySound(state.sounds[SOUND_GAMEOVER]);
        state.screenState = SCREEN_GAMEOVER;
        break;
      }
    }

    // Generate next piece
    state.score += MAX(0, state.softDropCounter - 1);
    state.softDropCounter = 0;
    state.fallingTimer = 0.0f;
    state.ARETimer = 0.0f;
    state.animationTimer = 0.0f;
    state.currentPiece = state.nextPiece;
    state.currentPiece.position = INITIAL_BOARD_POSITION;
    state.nextPiece = PieceGetRandom(state.currentPiece.tetromino);
    state.keyTimers[KEY_DOWN_TIMER] = KEY_DOWN_TIMER_SPEED + 1.0f;
    break;
  }
  case SCREEN_GAMEOVER:
    if (IsKeyPressed(KEY_R)) {
      GameReset();
    }
    break;
  }
}

void GameDraw(void) {
  BeginDrawing();
  ClearBackground(BLACK);
  switch (state.screenState) {
  case SCREEN_START: {
    const char *startText = "Select level to start";
    const Vector2 startTextMeasure = MeasureTextEx(GetFontDefault(), startText, 60.0f, 60.0f / 10.0f);
    DrawText(startText, (WIDTH - startTextMeasure.x) / 2.0f, HEIGHT / 2.5f, 60.0f, WHITE);
    const float levelBoxSpacing = BLOCK_LEN / 4.0f;
    const float levelBoxLen = BLOCK_LEN * 1.5f;
    const float totalWidth = 10.0f * levelBoxLen + 9.0f * levelBoxSpacing;
    for (int i = 0; i < 10; i++) {
      const Rectangle levelBox = {(WIDTH - totalWidth) / 2.0f + i * (levelBoxLen + levelBoxSpacing), HEIGHT / 2.0f, levelBoxLen,
                                  levelBoxLen};
      if (CheckCollisionPointRec(GetMousePosition(), levelBox)) {
        DrawRectangleRec(levelBox, ORANGE);
      }
      const char levelString[2] = {i + '0', 0};
      const Vector2 levelStringMeasure = MeasureTextEx(GetFontDefault(), levelString, 40.0f, 40.0f / 10.0f);
      DrawText(levelString, levelBox.x + (levelBox.width - levelStringMeasure.x) / 2.0f,
               levelBox.y + (levelBox.height - levelStringMeasure.y) / 2.0f, 40.0f, MAROON);
      DrawRectangleLinesEx(levelBox, 2.0f, GREEN);
    }
    break;
  }
  case SCREEN_GAMEOVER:
  case SCREEN_PLAY: {
    const Rectangle playfield = {(WIDTH - BLOCK_LEN * COLUMNS) / 2.0f, HEIGHT / 20.0f, BLOCK_LEN * COLUMNS + 5, BLOCK_LEN * ROWS + 2};
    // playfield without the buffer area
    const Rectangle shownPlayfield = {playfield.x, playfield.y + BUFFER_AREA, playfield.width, playfield.height - BUFFER_AREA};

    DrawRectangleLinesEx((Rectangle){shownPlayfield.x - 2, shownPlayfield.y - 2.0f, shownPlayfield.width + 4, shownPlayfield.height + 2.0f},
                         2, GRAY);
    BeginScissorMode(shownPlayfield.x, shownPlayfield.y, shownPlayfield.width, shownPlayfield.height);
    PieceDraw(&state.currentPiece, (Vector2){playfield.x, playfield.y});
    GameDrawBoard(state.board, (Vector2){playfield.x, playfield.y});
    EndScissorMode();

    const Rectangle nextPieceRect = {(WIDTH + shownPlayfield.width) / 2.0f + 3.0f, HEIGHT / 3.0f, BLOCK_LEN * 5.0f, BLOCK_LEN * 4.0f};
    PieceDraw(&state.nextPiece, (Vector2){nextPieceRect.x, nextPieceRect.y});
    DrawRectangleLinesEx(nextPieceRect, 2.0f, GRAY);

    const Rectangle linesCounterRect = {playfield.x - 2.0f, playfield.y, shownPlayfield.width + 4, 2.0f * BLOCK_LEN};
    DrawRectangleLinesEx(linesCounterRect, 2.0f, GRAY);
    const char *clearedLinesSting = TextFormat("LINES-%d", state.linesCleared);
    const Vector2 clearedLinesStringMeasure = MeasureTextEx(GetFontDefault(), clearedLinesSting, FONT_SIZE, FONT_SIZE / 10.0f);
    DrawText(clearedLinesSting, linesCounterRect.x + (linesCounterRect.width - clearedLinesStringMeasure.x) / 2.0f,
             linesCounterRect.y + (linesCounterRect.height - clearedLinesStringMeasure.y) / 2.0f, FONT_SIZE, WHITE);

    const Rectangle levelRect = {(WIDTH + shownPlayfield.width) / 2.0f + 3.0f, HEIGHT / 1.7f, BLOCK_LEN * 5.0f, BLOCK_LEN * 2.0 + 5.0f};
    DrawRectangleLinesEx(levelRect, 2.0f, GRAY);
    DrawText("LEVEL", levelRect.x + (levelRect.width - MeasureText("LEVEL", 40.0f)) / 2.0f, levelRect.y + 5.0f, 40.0f, WHITE);
    const char *currentLevelString = TextFormat("%d", state.currentLevel);
    DrawText(currentLevelString, levelRect.x + (levelRect.width - MeasureText(currentLevelString, 40.0f)) / 2.0f,
             levelRect.y + BLOCK_LEN + 5.0f, 40.0f, WHITE);

    const Rectangle scoreRect = {(WIDTH + shownPlayfield.width) / 2.0f + 3.0f, shownPlayfield.y - 2.0f, BLOCK_LEN * 6.0f,
                                 BLOCK_LEN * 2.0f + 5.0f};
    DrawRectangleLinesEx(scoreRect, 2, GRAY);
    DrawText("SCORE", scoreRect.x + (scoreRect.width - MeasureText("SCORE", 40.0f)) / 2.0f, scoreRect.y + 5.0f, 40.0f, WHITE);
    const char *scoreString = TextFormat("%09d", state.score);
    const Vector2 scoreStringMeasure = MeasureTextEx(GetFontDefault(), scoreString, 40.0f, 40.0f / 10.0f);
    DrawText(scoreString, scoreRect.x + (scoreRect.width - scoreStringMeasure.x) / 2.0f, scoreRect.y + BLOCK_LEN + 5.0f, 40.0f, WHITE);

    if (GameGetFullRowsCount() > 0) {
      for (int row = 0; row < ROWS; row++) {
        bool isFull = true;
        for (int column = 0; column < COLUMNS; column++) {
          if (!state.board[row][column].occupied) {
            isFull = false;
            break;
          }
        }
        if (isFull) {
          int w = ((int)(state.animationTimer * 10) + 1) * BLOCK_LEN;
          DrawRectangle(playfield.x + (5 * BLOCK_LEN - w), playfield.y + row * BLOCK_LEN, w, BLOCK_LEN, BLACK);
          DrawRectangle(playfield.x + 5 * BLOCK_LEN, playfield.y + row * BLOCK_LEN, w, BLOCK_LEN, BLACK);
        }
      }
    }

    // TODO: Maybe refactor this into its own function?
    if (state.screenState == SCREEN_PLAY) {
      break;
    }
    const char *gameoverString = "GAME OVER";
    const Vector2 gameoverStringMeasure = MeasureTextEx(GetFontDefault(), gameoverString, FONT_SIZE, FONT_SIZE / 10.0f);
    const char *tryAgainString = "Press R to try again";
    const Vector2 tryAgainStringMeasure = MeasureTextEx(GetFontDefault(), tryAgainString, 30.0f, 30.0 / 10.0f);

    const Rectangle gameoverTextRect = {playfield.x, playfield.y + playfield.height / 2.0f - BLOCK_LEN * 2.0f, BLOCK_LEN * COLUMNS,
                                        gameoverStringMeasure.y + tryAgainStringMeasure.y + 10.0f};
    DrawRectangleRec(gameoverTextRect, BLACK);
    DrawRectangleLinesEx(gameoverTextRect, 2.0f, GRAY);
    DrawText(gameoverString, gameoverTextRect.x + (gameoverTextRect.width - gameoverStringMeasure.x) / 2.0f, gameoverTextRect.y + 5.0f,
             FONT_SIZE, RED);
    DrawText(tryAgainString, gameoverTextRect.x + (gameoverTextRect.width - tryAgainStringMeasure.x) / 2.0f,
             gameoverTextRect.y + gameoverStringMeasure.y + 5.0f, 30.0f, WHITE);
    break;
  }
  }

  EndDrawing();
}

void GameInit(void) {
  for (int i = 0; i < MUSIC_COUNT; i++) {
    state.music[i] = LoadMusicStream(TextFormat("resources/Music_%d.ogg", i + 1));
    if (!IsMusicReady(state.music[i])) {
      fprintf(stderr, "Coudln't load file: `resources/Music_%d.ogg`", i + 1);
      exit(1);
    }
    state.music[i].looping = false;
    SetMusicVolume(state.music[i], 0.05f);
  }
  for (int i = 0; i < SOUND_COUNT; i++) {
    state.sounds[i] = LoadSound(TextFormat("resources/Sound_%d.ogg", i + 1));
    if (!IsSoundReady(state.sounds[i])) {
      fprintf(stderr, "Coudln't load file: `resources/Sound_%d.ogg`", i + 1);
      exit(1);
    }
  }
  state.currentMusicIndex = 0;
  PlayMusicStream(state.music[state.currentMusicIndex]);
  GameReset();
}

void GameCleanup(void) {
  for (int i = 0; i < MUSIC_COUNT; i++) {
    UnloadMusicStream(state.music[i]);
  }
}

static void GameUpdateMusic(void) {
  Music currentMusic = state.music[state.currentMusicIndex];
  if (state.isMusicPaused) {
    return;
  }
  UpdateMusicStream(currentMusic);
  if (!IsMusicStreamPlaying(currentMusic)) {
    state.currentMusicIndex = (state.currentMusicIndex + 1) % MUSIC_COUNT;
    PlayMusicStream(state.music[state.currentMusicIndex]);
  }
}

static void GameHandleInput(void) {
  const float dt = GetFrameTime();
  if (IsKeyPressed(KEY_X)) {
    PieceRotateClockwise(&state.currentPiece, state.board);
  }
  if (IsKeyPressed(KEY_Z)) {
    PieceRotateCounterClockwise(&state.currentPiece, state.board);
  }
  if (IsKeyDown(KEY_LEFT)) {
    if (WithinHalf(state.keyTimers[KEY_LEFT_TIMER], KEY_TIMER_SPEED) || IsKeyPressed(KEY_LEFT)) {
      state.keyTimers[KEY_LEFT_TIMER] = 0.0f;
      PieceMoveLeft(&state.currentPiece, state.board);
    } else if (state.keyTimers[KEY_LEFT_TIMER] < KEY_TIMER_SPEED) {
      state.keyTimers[KEY_LEFT_TIMER] += dt;
    }
  } else {
    state.keyTimers[KEY_LEFT_TIMER] = 0.0f;
  }
  if (IsKeyDown(KEY_RIGHT)) {
    if (WithinHalf(state.keyTimers[KEY_RIGHT_TIMER], KEY_TIMER_SPEED) || IsKeyPressed(KEY_RIGHT)) {
      state.keyTimers[KEY_RIGHT_TIMER] = 0.0f;
      PieceMoveRight(&state.currentPiece, state.board);
    } else if (state.keyTimers[KEY_RIGHT_TIMER] < KEY_TIMER_SPEED) {
      state.keyTimers[KEY_RIGHT_TIMER] += dt;
    }
  } else {
    state.keyTimers[KEY_RIGHT_TIMER] = 0.0f;
  }

  const float fallingSpeed = fallingSpeedTable[MIN(state.currentLevel, 29)];
  if (IsKeyDown(KEY_DOWN)) {
    if (WithinHalf(state.keyTimers[KEY_DOWN_TIMER], KEY_DOWN_TIMER_SPEED) || IsKeyPressed(KEY_DOWN)) {
      state.fallingTimer = fallingSpeed;
      state.softDropCounter++;
      state.keyTimers[KEY_DOWN_TIMER] = 0.0f;
    } else if (state.keyTimers[KEY_DOWN_TIMER] < KEY_DOWN_TIMER_SPEED) {
      state.keyTimers[KEY_DOWN_TIMER] += dt;
    }
  } else {
    state.keyTimers[KEY_DOWN_TIMER] = 0.0f;
    state.softDropCounter = 0;
  }
}

static void GameReset(void) {
  for (int i = 0; i < ROWS * COLUMNS; i++) {
    ((Block *)state.board)[i] = (Block){0, false};
  }
  for (int i = 0; i < KEY_TIMERS_COUNT; i++) {
    state.keyTimers[i] = 0.0f;
  }
  state.screenState = SCREEN_START;
  state.startingLevel = 0;
  state.currentLevel = 0;
  state.score = 0;
  state.isPaused = false;
  state.currentPiece = PieceGetRandom(NULL);
  state.currentPiece.position = INITIAL_BOARD_POSITION;
  state.nextPiece = PieceGetRandom(state.currentPiece.tetromino);
  state.fallingTimer = ENTRY_DELAY;
  state.linesCleared = 0;
  state.ARETimer = 0.0f;
  state.animationTimer = 0.0f;
  state.isMusicPaused = false;
  // SeekMusicStream(state.music[state.currentMusicIndex], 0.0f);
}

static void GameDrawBoard(Block board[ROWS][COLUMNS], Vector2 screenPosition) {
  for (int y = 0; y < ROWS; y++) {
    for (int x = 0; x < COLUMNS; x++) {
      if (board[y][x].occupied) {
        const Vector2 blockPositionOnScreen = Vector2Add(Vector2Scale((Vector2){x, y}, BLOCK_LEN), screenPosition);
        PieceDrawBlock(blockPositionOnScreen, 0, board[y][x].shapeType);
      }
    }
  }
}

static int GameGetFullRowsCount(void) {
  int clearedRows = 0;
  for (int row = 0; row < ROWS; row++) {
    for (int column = 0; column < COLUMNS; column++) {
      if (!state.board[row][column].occupied) {
        goto continue_outer;
      }
    }
    clearedRows++;
  continue_outer: {}
  }
  return clearedRows;
}
