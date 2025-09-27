/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include "game_msg.h"

#include <stdarg.h>
#include <stdio.h>

void game_msg_append(game_msg_p E, const char* str)
{
    while (E->cur < E->buf + sizeof(E->buf) - 1 && (*(E->cur++) = *str++))
        ;

    if (E->cur == E->buf + sizeof(E->buf) - 1)
        *E->cur = '\0';
    else
        --E->cur; /* Next time \0 will be overwritten */
}

void game_msg_flush(game_msg_p E)
{
    puts(E->buf);
    game_msg_clear(E);
}

void game_msg_clear(game_msg_p E)
{
    E->buf[0] = '\0';
    E->cur    = E->buf;
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
