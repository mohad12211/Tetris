#include "common.h"
#include <raylib.h>
#include <raymath.h>

const PieceType tetrominoes[] = {
    // I
    {{0, 240, 240, 255},
     {{{{0, 2}, {1, 2}, {2, 2}, {3, 2}}},
      {{{2, 0}, {2, 1}, {2, 2}, {2, 3}}},
      {{{0, 2}, {1, 2}, {2, 2}, {3, 2}}},
      {{{2, 0}, {2, 1}, {2, 2}, {2, 3}}}}},
    // J
    {{0, 0, 240, 255},
     {{{{1, 2}, {2, 2}, {3, 2}, {3, 3}}},
      {{{1, 3}, {2, 1}, {2, 2}, {2, 3}}},
      {{{1, 2}, {2, 2}, {3, 2}, {1, 1}}},
      {{{3, 1}, {2, 1}, {2, 2}, {2, 3}}}}},
    // L
    {{240, 161, 0, 255},
     {{{{1, 2}, {2, 2}, {3, 2}, {1, 3}}},
      {{{1, 1}, {2, 1}, {2, 2}, {2, 3}}},
      {{{1, 2}, {2, 2}, {3, 2}, {3, 1}}},
      {{{3, 3}, {2, 1}, {2, 2}, {2, 3}}}}},
    // O
    {{240, 240, 0, 255},
     {{{{1, 2}, {1, 3}, {2, 2}, {2, 3}}},
      {{{1, 2}, {1, 3}, {2, 2}, {2, 3}}},
      {{{1, 2}, {1, 3}, {2, 2}, {2, 3}}},
      {{{1, 2}, {1, 3}, {2, 2}, {2, 3}}}}},
    // T
    {{162, 0, 240, 255},
     {{{{1, 2}, {2, 2}, {3, 2}, {2, 3}}},
      {{{2, 1}, {2, 2}, {2, 3}, {1, 2}}},
      {{{1, 2}, {2, 2}, {3, 2}, {2, 1}}},
      {{{2, 1}, {2, 2}, {2, 3}, {3, 2}}}}},
    // S
    {{0, 240, 0, 255},
     {{{{2, 2}, {2, 3}, {3, 2}, {1, 3}}},
      {{{2, 2}, {2, 1}, {3, 2}, {3, 3}}},
      {{{2, 2}, {2, 3}, {3, 2}, {1, 3}}},
      {{{2, 2}, {2, 1}, {3, 2}, {3, 3}}}}},
    // Z
    {{241, 0, 0, 255},
     {{{{2, 2}, {2, 3}, {3, 3}, {1, 2}}},
      {{{2, 3}, {2, 2}, {3, 1}, {3, 2}}},
      {{{2, 2}, {2, 3}, {3, 3}, {1, 2}}},
      {{{2, 3}, {2, 2}, {3, 1}, {3, 2}}}}},

};

void PieceDraw(Piece *piece, Vector2 screenPosition) {
  for (int i = 0; i < 4; i++) {
    const PieceConfiguration *blocks = &piece->tetromino->rotations[piece->rotationIndex];
    Vector2 blockPosition = Vector2Add(blocks->points[i], piece->position);
    Vector2 blockPositionOnScreen = Vector2Add(Vector2Scale(blockPosition, BLOCK_LEN), screenPosition);
    DrawRectangleV(blockPositionOnScreen, BLOCK_SIZE, piece->tetromino->color);
  }
}

void PieceRotateClockwise(Piece *piece) {
  for (int i = 0; i < 4; i++) {
    const PieceConfiguration *blocks = &piece->tetromino->rotations[(piece->rotationIndex + 1) % 4];
    Vector2 blockPosition = Vector2Add(blocks->points[i], piece->position);
    if (blockPosition.x < 0 || blockPosition.x >= COLUMNS || blockPosition.y >= ROWS ||
        gameState.board[(int)blockPosition.y][(int)blockPosition.x].occupied) {
      return;
    }
  }

  piece->rotationIndex = (piece->rotationIndex + 1) % 4;
}

void PieceRotateCounterClockwise(Piece *piece) {
  for (int i = 0; i < 4; i++) {
    const PieceConfiguration *blocks = &piece->tetromino->rotations[((piece->rotationIndex - 1) + 4) % 4];
    Vector2 blockPosition = Vector2Add(blocks->points[i], piece->position);
    if (blockPosition.x < 0 || blockPosition.x >= COLUMNS || blockPosition.y >= ROWS ||
        gameState.board[(int)blockPosition.y][(int)blockPosition.x].occupied) {
      return;
    }
  }
  piece->rotationIndex = ((piece->rotationIndex - 1) + 4) % 4;
}

void PieceMoveLeft(Piece *piece) {
  for (int i = 0; i < 4; i++) {
    const PieceConfiguration *blocks = &piece->tetromino->rotations[piece->rotationIndex];
    Vector2 blockPosition = Vector2Add(blocks->points[i], piece->position);
    blockPosition.x -= 1;
    if (blockPosition.x < 0 || gameState.board[(int)blockPosition.y][(int)blockPosition.x].occupied) {
      return;
    }
  }
  piece->position.x -= 1;
}

void PieceMoveRight(Piece *piece) {
  for (int i = 0; i < 4; i++) {
    const PieceConfiguration *blocks = &piece->tetromino->rotations[piece->rotationIndex];
    Vector2 blockPosition = Vector2Add(blocks->points[i], piece->position);
    blockPosition.x += 1;
    if (blockPosition.x >= COLUMNS || gameState.board[(int)blockPosition.y][(int)blockPosition.x].occupied) {
      return;
    }
  }
  piece->position.x += 1;
}

bool PieceMoveDown(Piece *piece) {
  for (int i = 0; i < 4; i++) {
    const PieceConfiguration *blocks = &piece->tetromino->rotations[piece->rotationIndex];
    Vector2 blockPosition = Vector2Add(blocks->points[i], piece->position);
    blockPosition.y += 1;
    if (blockPosition.y >= ROWS || gameState.board[(int)blockPosition.y][(int)blockPosition.x].occupied) {
      return true;
    }
  }
  piece->position.y += 1;
  return false;
}

Piece PieceGetRandom(void) {
  int randomIndex = GetRandomValue(0, 6);
  if (&tetrominoes[randomIndex] == gameState.previousPiece) {
    randomIndex = GetRandomValue(0, 6);
  }
  gameState.previousPiece = &tetrominoes[randomIndex];
  return (Piece){&tetrominoes[randomIndex], INITIAL_ROTATION, INITIAL_POSITION};
}
