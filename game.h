/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef CMC_CHESS_GAME_H
#define CMC_CHESS_GAME_H

#include "board.h"

#define GAME_COMMAND_LENGTH 256

enum
{
    GX_UNKNOWN,

    /* Dot Command */
    GD_DUMP,
    GD_RESTORE,

    /* Play */
    GP_MOVE
};

typedef struct game_t
{
    struct board_t board;
    char           comm_buf[GAME_COMMAND_LENGTH];

    const char* done;
    char        err[GAME_COMMAND_LENGTH];
    turn_t      turn;

    int           comm_type;
    struct move_t comm_move;
}* game_p;

extern const char* GAME_DONE_COULD_NOT_READ_STDIN;
extern const char* GAME_DONE_COMM_QUIT;

extern void game_init(game_p);
extern void game_run(game_p);

#endif /* CMC_CHESS_GAME_H */
