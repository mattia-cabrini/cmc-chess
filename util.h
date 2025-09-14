/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef CMC_CHESS_UTIL_H
#define CMC_CHESS_UTIL_H

#include "board.h"

extern void clear(void);

extern int streq_ci(const char* str1, const char* str2);
extern int strneq_ci(const char* str1, const char* str2, size_t n);

extern void trim_left(char* str);
extern void trim_right(char* str);
extern void trim(char* str);

#endif /* CMC_CHESS_UTIL_H */
