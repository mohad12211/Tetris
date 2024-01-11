#ifndef GAME_H
#define GAME_H

#include <raylib.h>

#define WIDTH 1000
#define HEIGHT 1000
#define BLOCK_LEN 40
#define BLOCK_SIZE ((Vector2){BLOCK_LEN, BLOCK_LEN})
#define BUFFER_ROWS 2
#define BUFFER_AREA (BUFFER_ROWS * BLOCK_LEN)
#define PLAYFIELD_ROWS 20
#define ROWS (BUFFER_ROWS + PLAYFIELD_ROWS)
#define COLUMNS 10
#define PIECE_COUNT 7
#define INITIAL_ROTATION 0
#define INITIAL_BOARD_POSITION ((Vector2){3, 0})
#define GRAVITY_TIME 0.5f

typedef struct {
  Vector2 points[4];
} PieceConfiguration;

typedef struct {
  Color color;
  PieceConfiguration rotations[4];
  // the offset required so that the piece is centered when displayed on the "next piece" rectangle
  Vector2 displayOffset;
} PieceType;

typedef struct {
  const PieceType *tetromino;
  int rotationIndex;
  Vector2 position;
} Piece;

typedef struct {
  Color color;
  bool occupied;
} Block;

typedef struct {
  Block board[ROWS][COLUMNS];
  bool isPaused;
  bool isQOLMode;
  Piece currentPiece;
  Piece nextPiece;
  float time;
  Music music;
} GameState;

void GameCleanup(GameState *state);
void GameInit(GameState *state);
void GameUpdate(GameState *state);
void GameDraw(GameState *state);

#endif // GAME_H
