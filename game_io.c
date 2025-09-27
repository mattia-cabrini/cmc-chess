/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include "game_io.h"

#include <stdarg.h>
#include <stdio.h>

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
    return fgets(str, str_length, stdin) != NULL;
}
