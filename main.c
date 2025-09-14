/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "int.h"

void clear(void);

int streq_ci(const char* str1, const char* str2);

/* If read QUIT (case insensitive) exit; If err exit */
void read_command(char* comm, size_t n);

int main(int argc, char** argv)
{
    struct board_t board;
    char           comm[16];
    char           buf[16];
    const char*    illegal_move;
    struct move_t  move;
    turn_t         turn = cpWTURN;
    struct coord_t whence_check;

    (void)argc;
    (void)argv;

    board_init(&board);

    for (;;)
    {
        /* clear(); */
        board_print(&board);

        for (comm[0] = 0; strlen(comm) < 4;)
        {
            printf("Command: ");

            read_command(comm, sizeof(comm));

            if (strlen(comm) < 4)
                printf("???\n");

            move_init(&move, comm, sizeof(comm));

            illegal_move = board_check_move(&board, &move, turn, &whence_check);
            if (illegal_move != NULL)
            {
                printf("Illegal move: %s\n", illegal_move);
                comm[0] = '\0';
            }
            else
            {
                board_exec(&board, &move);
                turn = (turn_t)~turn;
            }

            if (illegal_move == ILLEGAL_MOVE_CHECK)
            {
                coord_to_str(&whence_check, buf, sizeof(buf));
                printf("%s would take over the King\n", buf);
            }
        }
    }

    exit(0);
}

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
