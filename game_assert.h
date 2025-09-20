/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef CMC_CHESS_GAME_ASSERT_H
#define CMC_CHESS_GAME_ASSERT_H

#include "board.h"
#include "int.h"

enum
{
    ASSERT_KIND_UNKNOWN, /* ??? */

    ASSERT_KIND_CHECK,         /* check */
    ASSERT_KIND_CHECKMATE,     /* checkmate */
    ASSERT_KIND_PIECE_IS,      /* piece-is */
    ASSERT_KIND_PIECE_CAN_MOVE /* piace-can-move */
};

typedef struct game_assert_t
{
    int            kind;
    piece_t        piece;
    struct coord_t src;
    struct coord_t dst;
}* game_assert_p;

extern void game_assert_parse(
    game_assert_p A, const char* str, char* err, size_t err_length
);

#endif /* CMC_CHESS_GAME_ASSERT_H */
