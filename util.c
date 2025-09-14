/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

void clear(void)
{
    size_t        i    = 0;
    unsigned char CV[] = {
        0x1b, 0x5b, 0x33, 0x4a, 0x1b, 0x5b, 0x48, 0x1b, 0x5b, 0x32, 0x4a
    };

    for (i = 0; i < sizeof(CV); ++i)
        putc(CV[i], stdout);
}

int streq_ci(const char* str1, const char* str2)
{
    while (*str1 && *str2)
    {
        if (tolower(*str1) != tolower(*str2))
            return 0;

        ++str1;
        ++str2;
    }

    return *str1 == *str2;
}

int strneq_ci(const char* str1, const char* str2, size_t n)
{
    while (*str1 && *str2 && n)
    {
        if (tolower(*str1) != tolower(*str2))
            return 0;

        ++str1;
        ++str2;
        --n;
    }

    return n == 0 || *str1 == *str2;
}
