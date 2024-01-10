#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>

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

typedef struct {
  Vector2 points[4];
} PieceConfiguration;

typedef struct {
  Color color;
  Vector2 spawnPosition;
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

extern const PieceType tetrominoes[];
extern Block board[ROWS][COLUMNS];

void PieceDraw(Piece *piece, Vector2 position);
void PieceRotateClockwise(Piece *piece);
void PieceRotateCounterClockwise(Piece *piece);
void PieceMoveLeft(Piece *piece);
void PieceMoveRight(Piece *piece);
void PieceMoveDown(Piece *piece);
