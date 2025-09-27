/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef CMC_CHESS_UTIL_H
#define CMC_CHESS_UTIL_H

#include <stdlib.h>

#define assert_return_void(cond)                                               \
    {                                                                          \
        if (!(cond))                                                           \
            return;                                                            \
    }
#define assert_return(cond, value)                                             \
    {                                                                          \
        if (!(cond))                                                           \
            return value;                                                      \
    }

#define assert_fatal(cond, n)                                                  \
    {                                                                          \
        if (!(cond))                                                           \
            exit(n);                                                           \
    }

extern void clear(void);

extern int         streq_ci(const char* str1, const char* str2);
extern int         strneq_ci(const char* str1, const char* str2, size_t n);
extern const char* move_to_not_blank(const char* str);

extern void trim_left(char* str);
extern void trim_right(char* str);
extern void trim(char* str);

extern int file_copy(int fdsrc, int fddst);

#endif /* CMC_CHESS_UTIL_H */
