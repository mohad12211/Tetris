#ifndef PIECE_H
#define PIECE_H

#include "game.h"

void PieceDraw(Piece *piece, Vector2 screenPosition);
void PieceDrawGhost(Piece *piece, Vector2 screenPosition);
void PieceRotateClockwise(Piece *piece, Block board[ROWS][COLUMNS]);
void PieceRotateCounterClockwise(Piece *piece, Block board[ROWS][COLUMNS]);
void PieceMoveLeft(Piece *piece, Block board[ROWS][COLUMNS]);
void PieceMoveRight(Piece *piece, Block board[ROWS][COLUMNS]);
bool PieceMoveDown(Piece *piece, Block board[ROWS][COLUMNS]);
Piece PieceGetRandom(const PieceType *previousPieceType);

#endif // PIECE_H
