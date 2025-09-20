/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef CMC_CHESS_BOARD_H
#define CMC_CHESS_BOARD_H

#include <stddef.h>
#include <stdio.h>

#include "int.h"
#include "piece.h"

typedef struct coord_t
{
    myint8_t row;
    myint8_t col;
}* coord_p;

typedef struct abs_coord_t
{
    myuint8_t row;
    myuint8_t col;
}* abs_coord_p;

typedef struct board_t
{
    piece_t board[64];

    struct coord_t wking;
    struct coord_t bking;
}* board_p;

typedef struct move_t
{
    struct coord_t     source;
    struct coord_t     dest;
    struct coord_t     offset;
    struct abs_coord_t abs_offset;
}* move_p;

extern const char* ILLEGAL_MOVE_FROM_OUT_OF_BOUND;
extern const char* ILLEGAL_MOVE_TO_OUT_OF_BOUND;
extern const char* ILLEGAL_MOVE_FROM_IS_EMPTY;
extern const char* ILLEGAL_MOVE_FMT;
extern const char* ILLEGAL_MOVE_NOT_YOUR_TURN;
extern const char* ILLEGAL_MOVE_NOT_IMPLEMENTED_YET;
extern const char* ILLEGAL_MOVE_NO_MOVE;
extern const char* ILLEGAL_MOVE_TAKE_OVER_SELF;
extern const char* ILLEGAL_MOVE_CHECK;

extern const char* ILLEGAL_MOVE_PAWN_DESC;
extern const char* ILLEGAL_MOVE_ROOK_DESC;
extern const char* ILLEGAL_MOVE_KNIGHT_DESC;
extern const char* ILLEGAL_MOVE_BISHOP_DESC;
extern const char* ILLEGAL_MOVE_QUEEN_DESC;
extern const char* ILLEGAL_MOVE_KING_DESC;

extern piece_t board_get_at(board_p B, coord_p C);
extern void    board_set_at(board_p B, coord_p C, piece_t p);
extern void    board_init(board_p B);
extern void    board_print(board_p B);

/* If a check should occur, whence tells what piece would take over the king */
const char* board_check_move(board_p B, move_p M, turn_t turn, coord_p whence);

/**
 * Unsafe
 */
extern void board_exec(board_p B, move_p M);

/* Check if the parameter king is under check and sets whence.
 *
 * WARNING
 * Whence is set only if king is under check. It otherwise remains untouched.
 */
extern void board_under_check_part(board_p B, coord_p king, coord_p whence);

/* Check if the parameter king would be under check and sets whence.
 *
 * WARNING
 * - Whence is set if king is under check. It otherwise returns in an invalid
 *   state.
 * - No check is made to assess whether or not src->dst is a valid move.
 *
 * RETURN
 * The king that would be under check or cpEEMPTY.
 */
extern piece_t
board_under_check_part_w(board_p B, coord_p src, coord_p dst, coord_p whence);

extern int board_dump(board_p B, FILE* fp);
extern int board_restore(board_p B, FILE* fp);

extern int board_list_moves(board_p B, coord_p src, coord_p dst, size_t n);

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
extern void move_init(move_p M, const char* str, size_t n);
extern int  board_coord_out_of_bound(coord_p);

extern void coord_init_by_str(coord_p C, const char* str);
extern void coord_to_str(coord_p C, char* buf, size_t n);

#endif /* CMC_CHESS_BOARD_H */
