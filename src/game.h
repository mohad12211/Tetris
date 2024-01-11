#ifndef GAME_H
#define GAME_H

#include "piece.h"

typedef struct {
  Block board[ROWS][COLUMNS];
  bool isPaused;
  Piece currentPiece;
  Piece nextPiece;
  float time;
  Music music;
} GameState;

void GameCleanup(GameState *state);
void GameReset(GameState *state);
void GameInit(GameState *state);
void GameUpdate(GameState *state);
void GameDraw(GameState *state);
void GameDrawBoard(Block board[ROWS][COLUMNS], Vector2 screenPosition);

#endif // GAME_H
