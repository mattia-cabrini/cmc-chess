/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include "game_msg.h"
#include "game_io.h"

#include <stdarg.h>
#include <stddef.h>

void game_msg_append(game_msg_p E, const char* str)
{
#if GAME_MSG_BUFFER_TYPE == GAME_MSG_BUFFER_TOTAL
    while (E->cur < E->buf + sizeof(E->buf) - 1 && (*(E->cur++) = *str++))
        ;

    if (E->cur == E->buf + sizeof(E->buf) - 1)
        *E->cur = '\0';
    else
        --E->cur; /* Next time \0 will be overwritten */
#elif GAME_MSG_BUFFER_TYPE == GAME_MSG_BUFFER_NONE
    (void)E;
    game_io_puts(str);
#else
#error "GAME_MSG_BUFFER_TYPE has got an illegal value"
#endif
}

void game_msg_flush(game_msg_p E)
{
#if GAME_MSG_BUFFER_TYPE == GAME_MSG_BUFFER_TOTAL
    game_io_puts(E->buf);
    game_msg_clear(E);
#elif GAME_MSG_BUFFER_TYPE == GAME_MSG_BUFFER_NONE
    (void)E;
#else
#error "GAME_MSG_BUFFER_TYPE has got an illegal value"
#endif
}

void game_msg_clear(game_msg_p E)
{
#if GAME_MSG_BUFFER_TYPE == GAME_MSG_BUFFER_TOTAL
    E->buf[0] = '\0';
    E->cur    = E->buf;
#elif GAME_MSG_BUFFER_TYPE == GAME_MSG_BUFFER_NONE
    (void)E;
#else
#error "GAME_MSG_BUFFER_TYPE has got an illegal value"
#endif
}

void game_msg_vappend(game_msg_p E, ...)
{
    const char* strx;
    va_list     args;

    va_start(args, E);
    while ((strx = va_arg(args, const char*)) != NULL)
        game_msg_append(E, strx);

    va_end(args);
}

void game_msg_init(game_msg_p E) { game_msg_clear(E); }
