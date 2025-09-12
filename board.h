/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef CMC_CHESS_BOARD_H
#define CMC_CHESS_BOARD_H

#include "piece.h"

typedef struct chess_board_t
{
    chess_piece_t board[64];
}* chess_board;

typedef struct move_t
{
    struct
    {
        int row;
        int col;
    } source;
    struct
    {
        int row;
        int col;
    } dest;
}* move;

extern const char* ILLEGAL_MOVE_FROM_OUT_OF_BOUND;
extern const char* ILLEGAL_MOVE_TO_OUT_OF_BOUND;
extern const char* ILLEGAL_MOVE_FROM_IS_EMPTY;
extern const char* ILLEGAL_MOVE_PAWN_DEF;
extern const char* ILLEGAL_MOVE_PAWN_1;
extern const char* ILLEGAL_MOVE_PAWN_FORWARD;
extern const char* ILLEGAL_MOVE_PAWN_TAKE;
extern const char* ILLEGAL_MOVE_NOT_IMPLEMENTED_YET;

extern chess_piece_t chess_board_get_at(chess_board B, int row, int col);
extern void
chess_board_set_at(chess_board B, int row, int col, chess_piece_t p);
extern void chess_board_init(chess_board B);
extern void chess_board_print(chess_board B);

extern const char* chess_board_check_move(chess_board B, move M);

/**
 * Unsafe
 */
extern void chess_board_exec(chess_board B, move M);

/**
 * A move is a string like:
 *
 * FNTM:
 * - FN: coordinates from;
 * - TM: coordinates to.
 *
 * For example: "e2c4" means move e2 to c4.
 *
 * Return
 * NULL -> Legal move
 * *    -> ILLEGAL_MOVE_*
 */
extern void move_init(char* str, move M);
extern int  chess_board_coord_out_of_bound(int r, int c);

#endif /* CMC_CHESS_BOARD_H */
