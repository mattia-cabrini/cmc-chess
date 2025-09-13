/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef CMC_CHESS_BOARD_H
#define CMC_CHESS_BOARD_H

#include <stddef.h>

#include "int.h"
#include "piece.h"

typedef struct board_t
{
    piece_t board[64];
}* board_p;

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
}* move_p;

extern const char* ILLEGAL_MOVE_FROM_OUT_OF_BOUND;
extern const char* ILLEGAL_MOVE_TO_OUT_OF_BOUND;
extern const char* ILLEGAL_MOVE_FROM_IS_EMPTY;
extern const char* ILLEGAL_MOVE_FMT;
extern const char* ILLEGAL_MOVE_NOT_YOUR_TURN;

extern const char* ILLEGAL_MOVE_PAWN_DEF;
extern const char* ILLEGAL_MOVE_PAWN_1;
extern const char* ILLEGAL_MOVE_PAWN_FORWARD;
extern const char* ILLEGAL_MOVE_NOT_IMPLEMENTED_YET;

extern piece_t board_get_at(board_p B, int row, int col);
extern void    board_set_at(board_p B, int row, int col, piece_t p);
extern void    board_init(board_p B);
extern void    board_print(board_p B);

extern const char* board_check_move(board_p B, move_p M, turn_t turn);

/**
 * Unsafe
 */
extern void board_exec(board_p B, move_p M);

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
extern void move_init(move_p M, char* str, size_t n);
extern int  board_coord_out_of_bound(int r, int c);

#endif /* CMC_CHESS_BOARD_H */
