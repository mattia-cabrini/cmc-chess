/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef CMC_CHESS_GAME_H
#define CMC_CHESS_GAME_H

#include "board.h"
#include "game_msg.h"

#ifdef __AVR__
#define GAME_COMMAND_LENGTH 64
#else
#define GAME_COMMAND_LENGTH 256
#endif

enum
{
    GX_UNKNOWN,
    GX_IGNORE,

    /* Dot Command */
    GD_NEW,
    GD_DUMP,
    GD_RESTORE,

    /* Question Mark Command */
    GQ_LIST,

    /* Equal Command */
    GE_CLEAR,
    GE_SET,
    GE_ASSERT,

    /* Play */
    GP_MOVE
};

typedef struct game_t
{
    struct game_msg_t message;
    struct board_t    board;
    char              comm_buf[GAME_COMMAND_LENGTH];

    const char* done;
    turn_t      turn;

    int           comm_type;
    struct move_t comm_move;

    turn_t checkmate;
}* game_p;

extern const char* GAME_DONE_COULD_NOT_READ_STDIN;
extern const char* GAME_DONE_COMM_QUIT;
extern const char* GAME_DONE_ASSERT_FAILED;
extern const char* GAME_DONE_ASSERT_PARSE;

extern void game_init(game_p);
extern void game_run(game_p);

#endif /* CMC_CHESS_GAME_H */
