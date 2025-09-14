/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef CMC_CHESS_UTIL_H
#define CMC_CHESS_UTIL_H

#include "board.h"

extern void clear(void);

extern int streq_ci(const char* str1, const char* str2);

/* If read QUIT (case insensitive) exit; If err exit */
extern void read_command(char* comm, size_t n);

#endif /* CMC_CHESS_UTIL_H */
