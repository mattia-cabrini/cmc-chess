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

void read_command(char* comm, size_t n)
{
    size_t len;

    /* Does not take into account the terminal buffer */
    fflush(stdin);
    comm = fgets(comm, (int)n, stdin);

    if (comm == NULL)
    {
        fprintf(stderr, "Could not read stdin: exit.\n");
        exit(1);
    }

    for (len = strlen(comm);; --len)
    {
        if (comm[len - 1] == '\r' || comm[len - 1] == '\n')
            comm[len - 1] = '\0';
        else
            break;

        if (len == 1)
            break;
    }

    if (streq_ci(comm, "quit"))
    {
        printf("Bye\n");
        exit(0);
    }
}
