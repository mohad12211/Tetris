#include <raylib.h>
#include <raymath.h>

#include "piece.h"

static const PieceType tetrominoes[] = {
    // I
    {{0, 240, 240, 255},
     {{{{0, 2}, {1, 2}, {2, 2}, {3, 2}}},
      {{{2, 0}, {2, 1}, {2, 2}, {2, 3}}},
      {{{0, 2}, {1, 2}, {2, 2}, {3, 2}}},
      {{{2, 0}, {2, 1}, {2, 2}, {2, 3}}}},
     {0.5, -0.5}},
    // J
    {{0, 0, 240, 255},
     {{{{1, 2}, {2, 2}, {3, 2}, {3, 3}}},
      {{{1, 3}, {2, 1}, {2, 2}, {2, 3}}},
      {{{1, 2}, {2, 2}, {3, 2}, {1, 1}}},
      {{{3, 1}, {2, 1}, {2, 2}, {2, 3}}}},
     {0, -1}},
    // L
    {{240, 161, 0, 255},
     {{{{1, 2}, {2, 2}, {3, 2}, {1, 3}}},
      {{{1, 1}, {2, 1}, {2, 2}, {2, 3}}},
      {{{1, 2}, {2, 2}, {3, 2}, {3, 1}}},
      {{{3, 3}, {2, 1}, {2, 2}, {2, 3}}}},
     {0, -1}},
    // O
    {{240, 240, 0, 255},
     {{{{1, 2}, {1, 3}, {2, 2}, {2, 3}}},
      {{{1, 2}, {1, 3}, {2, 2}, {2, 3}}},
      {{{1, 2}, {1, 3}, {2, 2}, {2, 3}}},
      {{{1, 2}, {1, 3}, {2, 2}, {2, 3}}}},
     {0.5, -1}},
    // T
    {{162, 0, 240, 255},
     {{{{1, 2}, {2, 2}, {3, 2}, {2, 3}}},
      {{{2, 1}, {2, 2}, {2, 3}, {1, 2}}},
      {{{1, 2}, {2, 2}, {3, 2}, {2, 1}}},
      {{{2, 1}, {2, 2}, {2, 3}, {3, 2}}}},
     {0, -1}},
    // S
    {{0, 240, 0, 255},
     {{{{2, 2}, {2, 3}, {3, 2}, {1, 3}}},
      {{{2, 2}, {2, 1}, {3, 2}, {3, 3}}},
      {{{2, 2}, {2, 3}, {3, 2}, {1, 3}}},
      {{{2, 2}, {2, 1}, {3, 2}, {3, 3}}}},
     {0, -1}},
    // Z
    {{241, 0, 0, 255},
     {{{{2, 2}, {2, 3}, {3, 3}, {1, 2}}},
      {{{2, 3}, {2, 2}, {3, 1}, {3, 2}}},
      {{{2, 2}, {2, 3}, {3, 3}, {1, 2}}},
      {{{2, 3}, {2, 2}, {3, 1}, {3, 2}}}},
     {0, -1}},
};

void PieceDraw(Piece *piece, Vector2 screenPosition) {
  for (int i = 0; i < 4; i++) {
    const PieceConfiguration *blocks = &piece->tetromino->rotations[piece->rotationIndex];
    Vector2 blockPosition = Vector2Add(blocks->points[i], piece->position);
    Vector2 blockPositionOnScreen = Vector2Add(Vector2Scale(blockPosition, BLOCK_LEN), screenPosition);
    DrawRectangleV(blockPositionOnScreen, BLOCK_SIZE, piece->tetromino->color);
  }
}

void PieceRotateClockwise(Piece *piece, Block board[ROWS][COLUMNS]) {
  for (int i = 0; i < 4; i++) {
    const PieceConfiguration *blocks = &piece->tetromino->rotations[(piece->rotationIndex + 1) % 4];
    Vector2 blockPosition = Vector2Add(blocks->points[i], piece->position);
    if (blockPosition.x < 0 || blockPosition.x >= COLUMNS || blockPosition.y >= ROWS ||
        board[(int)blockPosition.y][(int)blockPosition.x].occupied) {
      return;
    }
  }

  piece->rotationIndex = (piece->rotationIndex + 1) % 4;
}

void PieceRotateCounterClockwise(Piece *piece, Block board[ROWS][COLUMNS]) {
  for (int i = 0; i < 4; i++) {
    const PieceConfiguration *blocks = &piece->tetromino->rotations[((piece->rotationIndex - 1) + 4) % 4];
    Vector2 blockPosition = Vector2Add(blocks->points[i], piece->position);
    if (blockPosition.x < 0 || blockPosition.x >= COLUMNS || blockPosition.y >= ROWS ||
        board[(int)blockPosition.y][(int)blockPosition.x].occupied) {
      return;
    }
  }
  piece->rotationIndex = ((piece->rotationIndex - 1) + 4) % 4;
}

void PieceMoveLeft(Piece *piece, Block board[ROWS][COLUMNS]) {
  for (int i = 0; i < 4; i++) {
    const PieceConfiguration *blocks = &piece->tetromino->rotations[piece->rotationIndex];
    Vector2 blockPosition = Vector2Add(blocks->points[i], piece->position);
    blockPosition.x -= 1;
    if (blockPosition.x < 0 || board[(int)blockPosition.y][(int)blockPosition.x].occupied) {
      return;
    }
  }
  piece->position.x -= 1;
}

void PieceMoveRight(Piece *piece, Block board[ROWS][COLUMNS]) {
  for (int i = 0; i < 4; i++) {
    const PieceConfiguration *blocks = &piece->tetromino->rotations[piece->rotationIndex];
    Vector2 blockPosition = Vector2Add(blocks->points[i], piece->position);
    blockPosition.x += 1;
    if (blockPosition.x >= COLUMNS || board[(int)blockPosition.y][(int)blockPosition.x].occupied) {
      return;
    }
  }
  piece->position.x += 1;
}

bool PieceMoveDown(Piece *piece, Block board[ROWS][COLUMNS]) {
  for (int i = 0; i < 4; i++) {
    const PieceConfiguration *blocks = &piece->tetromino->rotations[piece->rotationIndex];
    Vector2 blockPosition = Vector2Add(blocks->points[i], piece->position);
    blockPosition.y += 1;
    if (blockPosition.y >= ROWS || board[(int)blockPosition.y][(int)blockPosition.x].occupied) {
      return true;
    }
  }
  piece->position.y += 1;
  return false;
}

Piece PieceGetRandom(const PieceType *previousPieceType) {
  int randomIndex = GetRandomValue(0, 6);
  if (&tetrominoes[randomIndex] == previousPieceType) {
    randomIndex = GetRandomValue(0, 6);
  }
  return (Piece){&tetrominoes[randomIndex], INITIAL_ROTATION, tetrominoes[randomIndex].displayOffset};
}
