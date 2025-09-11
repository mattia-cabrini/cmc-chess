/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef CMC_CHESS_BOARD_H
#define CMC_CHESS_BOARD_H

#include "piece.h"

typedef struct chess_board_t
{
    chess_piece_t board[64];
}* chess_board;

extern chess_piece_t chess_board_get_at(chess_board B, int row, int col);
extern void          chess_board_init(chess_board B);
extern void          chess_board_print(chess_board B);

#endif /* CMC_CHESS_BOARD_H */
