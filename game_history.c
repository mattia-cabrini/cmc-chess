/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#define _POSIX_C_SOURCE 200809L
#include "game_history.h"
#include "util.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int         fd             = -1;
static const char* fname_template = "/tmp/cmc-chess-XXXXXX";
static char        fname[32];

void history_init(void)
{
    if (fd == -1)
    {
        memcpy(fname, fname_template, sizeof(fname));
        fd = mkstemp(fname);
    }
    else
    {
        close(fd);
        fd = creat(fname, 0600);
    }

    if (fd == -1)
    {
        fprintf(stderr, "Cannot stat temp file.\n");
        exit(1);
    }
}

int history_println(char* str)
{
    size_t  toadd = 0;
    size_t  len;
    ssize_t res;

    len = strlen(str);

    if (str[len - 1] != '\n')
    {
        str[len] = '\n';
        toadd    = 1;
    }

    res = write(fd, str, len + toadd);

    if (toadd == 1)
        str[len] = '\0';

    return res != -1;
}

int history_mv(const char* fname_to)
{
    int fddst;

    if (fd < 0)
    {
        errno = ENOENT;
        return 0;
    }

    fddst = creat(fname_to, 0664);
    if (fddst == -1)
        return 0;

    if (file_copy(fd, fddst))
        return 1;

    return close(fddst) != -1;
}

void history_close(void)
{
    if (fd < 0)
        return;

    close(fd);
    fd = -1;
}
