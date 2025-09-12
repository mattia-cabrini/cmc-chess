/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"

void clear(void);

/* Return true is no error occurred while reading; false otherwise */
int read_command(char* comm, size_t n);

int main(int argc, char** argv)
{
    struct chess_board_t board;
    char                 comm[16];
    const char*          illegal_move;
    struct move_t        m;

    (void)argc;
    (void)argv;

    chess_board_init(&board);

    for (;;)
    {
        /* clear(); */
        chess_board_print(&board);

        for (comm[0] = 0; strlen(comm) < 4;)
        {
            printf("Command: ");

            if (!read_command(comm, sizeof(comm)))
                exit(1);

            if (strlen(comm) < 4)
                printf("???\n");

            move_init(comm, &m);

            illegal_move = chess_board_check_move(&board, &m);
            if (illegal_move != NULL)
            {
                printf("Illegal move: %s\n", illegal_move);
                comm[0] = '\0';
            }
            else
            {
                chess_board_exec(&board, &m);
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

int read_command(char* comm, size_t n)
{
    fflush(stdin);
    return fgets(comm, (int)n, stdin) != NULL;
}
