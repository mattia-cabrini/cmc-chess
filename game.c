/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include <stdio.h>
#include <string.h>

#include "board.h"
#include "game.h"
#include "util.h"

void game_init(game_p G) { board_init(&G->board); }

void game_run(game_p G)
{
    char           comm[16];
    char           buf[16];
    const char*    illegal_move;
    struct move_t  move;
    turn_t         turn = cpWTURN;
    struct coord_t whence_check;

    for (;;)
    {
        /* clear(); */
        board_print(&G->board);

        for (comm[0] = 0; strlen(comm) < 4;)
        {
            printf("Command: ");

            read_command(comm, sizeof(comm));

            if (strlen(comm) < 4)
                printf("???\n");

            move_init(&move, comm, sizeof(comm));

            illegal_move =
                board_check_move(&G->board, &move, turn, &whence_check);
            if (illegal_move != NULL)
            {
                printf("Illegal move: %s\n", illegal_move);
                comm[0] = '\0';
            }
            else
            {
                board_exec(&G->board, &move);
                turn = (turn_t)~turn;
            }

            if (illegal_move == ILLEGAL_MOVE_CHECK)
            {
                coord_to_str(&whence_check, buf, sizeof(buf));
                printf("%s would take over the King\n", buf);
            }
        }
    }
}
