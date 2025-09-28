/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef CMC_CHESS_GAME_IO_H
#define CMC_CHESS_GAME_IO_H

#include <stdio.h>

/* Control */
extern void game_io_init(void);
extern void game_io_set_stream_in(FILE* fp);

/* Out */
extern void game_io_printf(const char* fmt, ...)
    __attribute__((format(printf, 1, 2)));
extern void game_io_putc(int);
extern void game_io_puts(const char*);

/* In */
extern int game_io_gets(char* str, int str_length);

#endif /* CMC_CHESS_GAME_IO_H */
