#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>

#include "piece.h"

static const PieceType tetrominoes[] = {
    // I
    {{{{{0, 2}, {1, 2}, {2, 2}, {3, 2}}},
      {{{2, 0}, {2, 1}, {2, 2}, {2, 3}}},
      {{{0, 2}, {1, 2}, {2, 2}, {3, 2}}},
      {{{2, 0}, {2, 1}, {2, 2}, {2, 3}}}},
     {0.5, -0.5},
     0},
    // J
    {{{{{1, 2}, {2, 2}, {3, 2}, {3, 3}}},
      {{{1, 3}, {2, 1}, {2, 2}, {2, 3}}},
      {{{1, 2}, {2, 2}, {3, 2}, {1, 1}}},
      {{{3, 1}, {2, 1}, {2, 2}, {2, 3}}}},
     {0, -1},
     2},
    // L
    {{{{{1, 2}, {2, 2}, {3, 2}, {1, 3}}},
      {{{1, 1}, {2, 1}, {2, 2}, {2, 3}}},
      {{{1, 2}, {2, 2}, {3, 2}, {3, 1}}},
      {{{3, 3}, {2, 1}, {2, 2}, {2, 3}}}},
     {0, -1},
     1},
    // O
    {{{{{1, 2}, {1, 3}, {2, 2}, {2, 3}}},
      {{{1, 2}, {1, 3}, {2, 2}, {2, 3}}},
      {{{1, 2}, {1, 3}, {2, 2}, {2, 3}}},
      {{{1, 2}, {1, 3}, {2, 2}, {2, 3}}}},
     {0.5, -1},
     0},
    // T
    {{{{{1, 2}, {2, 2}, {3, 2}, {2, 3}}},
      {{{2, 1}, {2, 2}, {2, 3}, {1, 2}}},
      {{{1, 2}, {2, 2}, {3, 2}, {2, 1}}},
      {{{2, 1}, {2, 2}, {2, 3}, {3, 2}}}},
     {0, -1},
     0},
    // S
    {{{{{2, 2}, {2, 3}, {3, 2}, {1, 3}}},
      {{{2, 2}, {2, 1}, {3, 2}, {3, 3}}},
      {{{2, 2}, {2, 3}, {3, 2}, {1, 3}}},
      {{{2, 2}, {2, 1}, {3, 2}, {3, 3}}}},
     {0, -1},
     2},
    // Z
    {{{{{2, 2}, {2, 3}, {3, 3}, {1, 2}}},
      {{{2, 3}, {2, 2}, {3, 1}, {3, 2}}},
      {{{2, 2}, {2, 3}, {3, 3}, {1, 2}}},
      {{{2, 3}, {2, 2}, {3, 1}, {3, 2}}}},
     {0, -1},
     1},
};

static const Color colorPalettes[10][2] = {{{0, 88, 248, 255}, {60, 188, 252, 255}},   {{0, 168, 0, 255}, {184, 248, 24, 255}},
                                           {{216, 0, 204, 255}, {248, 120, 248, 255}}, {{0, 88, 248, 255}, {88, 216, 84, 255}},
                                           {{228, 0, 88, 255}, {88, 248, 152, 255}},   {{88, 248, 152, 255}, {104, 136, 252, 255}},
                                           {{248, 56, 0, 255}, {124, 124, 124, 255}},  {{104, 68, 252, 255}, {110, 0, 64, 255}},
                                           {{0, 88, 248, 255}, {248, 56, 0, 255}},     {{248, 56, 0, 255}, {234, 158, 34, 255}}};

void PieceDrawBlock(Vector2 position, int paletteIndex, int shapeType) {
  const Color *colorPalette = colorPalettes[paletteIndex];
  position.x += 5;
  switch (shapeType) {
  case 0: {
    DrawRectangle(position.x, position.y, BLOCK_LEN - 5, BLOCK_LEN - 5, colorPalette[0]);
    DrawRectangle(position.x + 5, position.y + 5, BLOCK_LEN - 15, BLOCK_LEN - 15, WHITE);
    DrawRectangle(position.x, position.y, 5, 5, WHITE);
    break;
  }
  case 1: {
    DrawRectangle(position.x, position.y, BLOCK_LEN - 5, BLOCK_LEN - 5, colorPalette[1]);
    DrawRectangle(position.x, position.y, 5, 5, WHITE);
    DrawRectangle(position.x + 5, position.y + 5, 5 * 2, 5 * 2, WHITE);
    DrawRectangle(position.x + 5 * 2, position.y + 5 * 2, 5, 5, colorPalette[1]);
    break;
  }
  case 2: {
    DrawRectangle(position.x, position.y, BLOCK_LEN - 5, BLOCK_LEN - 5, colorPalette[0]);
    DrawRectangle(position.x, position.y, 5, 5, WHITE);
    DrawRectangle(position.x + 5, position.y + 5, 5 * 2, 5 * 2, WHITE);
    DrawRectangle(position.x + 5 * 2, position.y + 5 * 2, 5, 5, colorPalette[0]);
    break;
  }
  default: {
    fprintf(stderr, "Unknown block shape type: %d", shapeType);
    exit(1);
  }
  }
}

void PieceDraw(const Piece *piece, const Vector2 screenPosition, int paletteIndex) {
  for (int i = 0; i < 4; i++) {
    const PieceConfiguration *blocks = &piece->tetromino->rotations[piece->rotationIndex];
    const Vector2 blockPosition = Vector2Add(blocks->points[i], piece->position);
    const Vector2 blockPositionOnScreen = Vector2Add(Vector2Scale(blockPosition, BLOCK_LEN), screenPosition);
    PieceDrawBlock(blockPositionOnScreen, paletteIndex, piece->tetromino->shapeType);
  }
}

void PieceRotateClockwise(Piece *piece, const Block board[ROWS][COLUMNS]) {
  for (int i = 0; i < 4; i++) {
    const PieceConfiguration *blocks = &piece->tetromino->rotations[(piece->rotationIndex + 1) % 4];
    const Vector2 blockPosition = Vector2Add(blocks->points[i], piece->position);
    if (blockPosition.x < 0 || blockPosition.x >= COLUMNS || blockPosition.y >= ROWS ||
        board[(int)blockPosition.y][(int)blockPosition.x].occupied) {
      return;
    }
  }

  piece->rotationIndex = (piece->rotationIndex + 1) % 4;
}

void PieceRotateCounterClockwise(Piece *piece, const Block board[ROWS][COLUMNS]) {
  for (int i = 0; i < 4; i++) {
    const PieceConfiguration *blocks = &piece->tetromino->rotations[((piece->rotationIndex - 1) + 4) % 4];
    const Vector2 blockPosition = Vector2Add(blocks->points[i], piece->position);
    if (blockPosition.x < 0 || blockPosition.x >= COLUMNS || blockPosition.y >= ROWS ||
        board[(int)blockPosition.y][(int)blockPosition.x].occupied) {
      return;
    }
  }
  piece->rotationIndex = ((piece->rotationIndex - 1) + 4) % 4;
}

void PieceMoveLeft(Piece *piece, const Block board[ROWS][COLUMNS]) {
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

void PieceMoveRight(Piece *piece, const Block board[ROWS][COLUMNS]) {
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

bool PieceMoveDown(Piece *piece, const Block board[ROWS][COLUMNS]) {
  for (int i = 0; i < 4; i++) {
    const PieceConfiguration *blocks = &piece->tetromino->rotations[piece->rotationIndex];
    Vector2 blockPosition = Vector2Add(blocks->points[i], piece->position);
    blockPosition.y += 1;
    if (blockPosition.y >= ROWS || board[(int)blockPosition.y][(int)blockPosition.x].occupied) {
      return false;
    }
  }
  piece->position.y += 1;
  return true;
}

Piece PieceGetRandom(const PieceType *previousPieceType) {
  int randomIndex = GetRandomValue(0, 6);
  if (&tetrominoes[randomIndex] == previousPieceType) {
    randomIndex = GetRandomValue(0, 6);
  }
  return (Piece){&tetrominoes[randomIndex], tetrominoes[randomIndex].displayOffset, INITIAL_ROTATION};
}
