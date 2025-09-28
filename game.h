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
    GD_NOCLEAR,
    GD_SAVE,
    GD_SAVE_FORCE,
    GD_COMMENT, /* .. */
    GD_LOAD,

    /* Question Mark Command */
    GQ_LIST,

    /* Equal Command */
    GE_CLEAR,
    GE_SET,
    GE_ASSERT,

    /* Play */
    GP_MOVE
};

enum
{
    GOPT_CLEAR   = 1, /* 1: clear afet each command; 0: do not clear */
    GOPT_IN_LOAD = 2  /* 1: load in progress; 0: load not in progress */
};

typedef struct game_t
{
    struct game_msg_t message;
    char              comm_buf[GAME_COMMAND_LENGTH];
    struct board_t    board;

    int           comm_type;
    struct move_t comm_move;

    const char* done;

    int opts;

    turn_t turn;
    turn_t checkmate;

    piece_t pawn_morph;
}* game_p;

extern const char* GAME_DONE_COULD_NOT_READ_STDIN;
extern const char* GAME_DONE_COMM_QUIT;
extern const char* GAME_DONE_ASSERT_FAILED;
extern const char* GAME_DONE_ASSERT_PARSE;

extern void game_init(game_p G, int flags);
extern void game_run(game_p);

#ifdef DEBUG
extern void game_meminfo(void);
#endif

#endif /* CMC_CHESS_GAME_H */
