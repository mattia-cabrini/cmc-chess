/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef CMC_CHESS_GAME_MSG_H
#define CMC_CHESS_GAME_MSG_H

#ifdef __AVR__
#define GAME_MSG_LENGTH 64
#else
#define GAME_MSG_LENGTH 2048
#endif

#define GAME_MSG_BUFFER_NONE 1
#define GAME_MSG_BUFFER_TOTAL 2

#ifndef GAME_MSG_BUFFER_TYPE
#define GAME_MSG_BUFFER_TYPE 2
#endif

typedef struct game_msg_t
{
#if GAME_MSG_BUFFER_TYPE == GAME_MSG_BUFFER_TOTAL
    char  buf[GAME_MSG_LENGTH];
    char* cur; /* Index of next writable char */
#elif GAME_MSG_BUFFER_TYPE == GAME_MSG_BUFFER_NONE
    char nobuf;
#else
#error "GAME_MSG_BUFFER_TYPE has got an illegal value"
#endif
}* game_msg_p;

extern void game_msg_init(game_msg_p E);
extern void game_msg_append(game_msg_p E, const char* str);
extern void game_msg_vappend(game_msg_p E, ...);
extern void game_msg_flush(game_msg_p E);
extern void game_msg_clear(game_msg_p E);

#endif /* CMC_CHESS_GAME_MSG_H */
