#ifndef PIECE_H
#define PIECE_H

#include "game.h"

void PieceDraw(const Piece *piece, const Vector2 screenPosition, int paletteIndex);
void PieceDrawGhost(Piece *piece, Vector2 screenPosition);
void PieceRotateClockwise(Piece *piece, const Block board[ROWS][COLUMNS]);
void PieceRotateCounterClockwise(Piece *piece, const Block board[ROWS][COLUMNS]);
void PieceMoveLeft(Piece *piece, const Block board[ROWS][COLUMNS]);
void PieceMoveRight(Piece *piece, const Block board[ROWS][COLUMNS]);
bool PieceMoveDown(Piece *piece, const Block board[ROWS][COLUMNS]);
Piece PieceGetRandom(const PieceType *previousPieceType);
void PieceDrawBlock(const Vector2 position, int paletteIndex, int shapeType);

#endif // PIECE_H
