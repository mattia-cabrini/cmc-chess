/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include "game_io.h"
#include "exit_codes.h"
#include "util.h"

#include <stdarg.h>
#include <stdio.h>

static FILE* game_io_stream_in = NULL;

void game_io_init(void) { game_io_stream_in = stdin; }

void game_io_printf(const char* fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

void game_io_putc(int ch) { fputc(ch, stdout); }

void game_io_puts(const char* str) { fputs(str, stdout); }

int game_io_gets(char* str, int str_length)
{
    assert_fatal(game_io_stream_in != NULL, CHESS_GAME_IO_NOT_INIT);

    str = fgets(str, str_length, game_io_stream_in);

    if (game_io_stream_in != stdin && feof(game_io_stream_in))
        game_io_stream_in = stdin; /* File terminated; back to stdin */

    return str != NULL;
}

void game_io_set_stream_in(FILE* fp)
{
    if (fp != NULL)
        game_io_stream_in = fp;
    else
        game_io_stream_in = stdin;
}
