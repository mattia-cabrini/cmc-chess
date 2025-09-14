/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef CMC_CHESS_GAME_H
#define CMC_CHESS_GAME_H

#include "board.h"

typedef struct game_t
{
    struct board_t board;
}* game_p;

extern void game_init(game_p);
extern void game_run(game_p);

#endif /* CMC_CHESS_GAME_H */
