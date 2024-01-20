#ifndef PIECE_H
#define PIECE_H

#include "game.h"

void PieceDraw(const Piece *piece, const Vector2 screenPosition, int paletteIndex, float scale);
void PieceRotateClockwise(Piece *piece, const Block board[ROWS][COLUMNS]);
void PieceRotateCounterClockwise(Piece *piece, const Block board[ROWS][COLUMNS]);
void PieceMoveLeft(Piece *piece, const Block board[ROWS][COLUMNS]);
void PieceMoveRight(Piece *piece, const Block board[ROWS][COLUMNS]);
bool PieceMoveDown(Piece *piece, const Block board[ROWS][COLUMNS]);
Piece PieceGetRandom(const PieceType *previousPieceType);
void PieceDrawBlock(const Vector2 position, int paletteIndex, int shapeType, float scale);

#endif // PIECE_H
