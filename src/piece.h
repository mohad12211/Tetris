#ifndef PIECE_H
#define PIECE_H

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
#define INITIAL_POSITION ((Vector2){3, 0})
#define GRAVITY_TIME 1.0f

typedef struct {
  Vector2 points[4];
} PieceConfiguration;

typedef struct {
  Color color;
  PieceConfiguration rotations[4];
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

// extern const PieceType tetrominoes[];

void PieceDraw(Piece *piece, Vector2 position);
void PieceRotateClockwise(Piece *piece, Block board[ROWS][COLUMNS]);
void PieceRotateCounterClockwise(Piece *piece, Block board[ROWS][COLUMNS]);
void PieceMoveLeft(Piece *piece, Block board[ROWS][COLUMNS]);
void PieceMoveRight(Piece *piece, Block board[ROWS][COLUMNS]);
bool PieceMoveDown(Piece *piece, Block board[ROWS][COLUMNS]);
Piece PieceGetRandom(void);

#endif // PIECE_H
