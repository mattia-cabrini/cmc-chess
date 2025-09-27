/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include <ctype.h>
#include <string.h>
#include <sys/types.h>

#include "game_io.h"
#include "util.h"

void clear(void)
{
#ifndef __AVR__
    size_t        i    = 0;
    unsigned char CV[] = {
        0x1b, 0x5b, 0x33, 0x4a, 0x1b, 0x5b, 0x48, 0x1b, 0x5b, 0x32, 0x4a
    };

    for (i = 0; i < sizeof(CV); ++i)
        game_io_putc(CV[i]);
#endif
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
    ssize_t len;
    ssize_t first_nb;
    ssize_t cur;
    int     br = 0;

#ifdef DEBUG
    game_io_printf("DEBUG trim_left before: `%s`\n", str);
#endif

    len = (ssize_t)strlen(str);
    for (first_nb = 0; !br && str[first_nb]; ++first_nb)
        if (str[first_nb] > 32)
            br = 1;

    --first_nb;
    if (first_nb >= 0)
        for (cur = 0; cur <= len - first_nb; ++cur)
            str[cur] = str[cur + first_nb];

#ifdef DEBUG
    game_io_printf("DEBUG trim_left after: `%s`\n", str);
#endif
}

void trim_right(char* str)
{
    size_t len;

#ifdef DEBUG
    game_io_printf("DEBUG trim_right before: `%s`\n", str);
#endif

    for (len = strlen(str); len > 0; --len)
        if (str[len - 1] <= 32)
            str[len - 1] = '\0';
        else
            /* Combined with --len at for declaration will break the loop */
            len = 1;

#ifdef DEBUG
    game_io_printf("DEBUG trim_right after: `%s`\n", str);
#endif
}

void trim(char* str)
{
    trim_left(str);
    trim_right(str);
}

const char* move_to_not_blank(const char* str)
{
    for (; *str && *str <= 32; ++str)
        ;

    return str;
}
