/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"

void clear(void);

int main(int argc, char** argv)
{
    struct chess_board_t board;
    char                 move[16];
    int                  rF, cF, rT, cT;

    (void)argc;
    (void)argv;

    chess_board_init(&board);

    for (;;)
    {
        /* clear(); */
        chess_board_print(&board);

        for (move[0] = 0; strlen(move) < 4;)
        {
            printf("Command: ");

            if (scanf("%s", move) != 1)
                exit(1);

            if (strlen(move) < 4)
                printf("???\n");
        }

        chess_board_coord_to_idx(move, &rF, &cF);
        chess_board_coord_to_idx(move + 2, &rT, &cT);

        printf("From r-c=c%d-%d to r-c=%d-%d", rF, cF, rT, cT);

        printf(
            "From: %c\n",
            chess_piece_to_char(chess_board_get_at(&board, rF, cF))
        );
        printf(
            "To: %c\n", chess_piece_to_char(chess_board_get_at(&board, rT, cT))
        );
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
