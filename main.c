/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include <stdio.h>

#include "game.h"

int main(int argc, char** argv)
{
    struct game_t game;

    (void)argc;
    (void)argv;

    game_init(&game);
    game_run(&game);

    if (game.done == GAME_DONE_COMM_QUIT)
    {
        printf("Bye\n");
    }
    else if (game.done == GAME_DONE_ASSERT_FAILED)
    {
        fprintf(stderr, "Error: %s.\n", game.done);
        return 2;
    }
    else if (game.done == GAME_DONE_ASSERT_PARSE)
    {
        fprintf(stderr, "Error: %s.\n", game.done);
        return 3;
    }
    else
    {
        fprintf(stderr, "Error: %s.\n", game.done);
        return 1;
    }

    return 0;
}
