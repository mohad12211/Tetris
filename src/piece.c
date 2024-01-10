#include "common.h"

const PieceType tetrominoes[] = {
    // I
    {SKYBLUE,
     {3, 0},
     {{{{0, 2}, {1, 2}, {2, 2}, {3, 2}}},
      {{{2, 0}, {2, 1}, {2, 2}, {2, 3}}},
      {{{0, 2}, {1, 2}, {2, 2}, {3, 2}}},
      {{{2, 0}, {2, 1}, {2, 2}, {2, 3}}}}},
    // // J
    // {BLUE, {{0, 0}, {1, 0}, {2, 0}, {2, 1}}},
    // // L
    // {ORANGE, {{0, 0}, {1, 0}, {2, 0}, {0, 1}}},
    // // O
    // {YELLOW, {{1, 0}, {1, 1}, {2, 0}, {2, 1}}},
    // // T
    // {PURPLE, {{0, 1}, {1, 1}, {2, 1}, {1, 2}}},
    // // S
    // {GREEN, {{1, 1}, {2, 1}, {0, 2}, {1, 2}}},
    // // Z
    // {RED, {{0, 0}, {1, 0}, {1, 1}, {2, 1}}},

};

void PieceDraw(Piece *piece, Vector2 screenPosition) {
  for (int i = 0; i < 4; i++) {
    const PieceConfiguration *blocks = &piece->tetromino->rotations[piece->rotationIndex];
    Vector2 blockPosition = Vector2Add(blocks->points[i], piece->position);
    Vector2 blockPositionOnScreen = Vector2Add(Vector2Scale(blockPosition, BLOCK_LEN), screenPosition);
    DrawRectangleV(blockPositionOnScreen, BLOCK_SIZE, piece->tetromino->color);
  }
}

void PieceRotateClockwise(Piece *piece) { piece->rotationIndex = (piece->rotationIndex + 1) % 4; }

void PieceRotateCounterClockwise(Piece *piece) { piece->rotationIndex = ((piece->rotationIndex - 1) + 4) % 4; }

void PieceMoveLeft(Piece *piece) {
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

void PieceMoveRight(Piece *piece) {
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

void PieceMoveDown(Piece *piece) { piece->position.y += 1; }
