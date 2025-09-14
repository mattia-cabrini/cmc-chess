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

void trim_left(char* str)
{
    size_t len;
    size_t first_nb;
    size_t cur;
    int    br = 0;

#ifdef DEBUG
    printf("DEBUG trim_left before: `%s`\n", str);
#endif

    len = strlen(str);
    for (first_nb = 0; !br && str[first_nb]; ++first_nb)
    {
        switch (str[first_nb])
        {
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            br = 0;
            break;
        default:
            br = 1;
        }
    }

    --first_nb;
    for (cur = 0; cur <= len - first_nb; ++cur)
        str[cur] = str[cur + first_nb];

#ifdef DEBUG
    printf("DEBUG trim_left after: `%s`\n", str);
#endif
}

void trim_right(char* str)
{
    size_t len;

#ifdef DEBUG
    printf("DEBUG trim_right before: `%s`\n", str);
#endif

    for (len = strlen(str); len > 0; --len)
    {
        switch (str[len - 1])
        {
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            str[len - 1] = '\0';
            break;
        default:
            /* Combined with --len at for declaration will break the loop */
            len = 1;
        }
    }

#ifdef DEBUG
    printf("DEBUG trim_right after: `%s`\n", str);
#endif
}

void trim(char* str)
{
    trim_left(str);
    trim_right(str);
}
