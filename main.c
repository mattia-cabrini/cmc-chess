/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include <stdio.h>

#include "board.h"
#include "coord.h"
#include "exit_codes.h"
#include "game.h"
#include "game_msg.h"
#include "util.h"

#ifdef DEBUG
static void meminfo(void);
#endif

/* Argv:
 * - 0: program name or path;
 * - 1: [meminfo]:
 *   - meminfo (ifdef DEBUG): print structs sizes.
 */
int main(int argc, char** argv)
{
    struct game_t game;

    if (argc > 1)
    {
        if (streq_ci(argv[1], "meminfo"))
        {
#ifdef DEBUG
            meminfo();
            return CHESS_OK;
#else
            fprintf(stderr, "`%s` not available: !def DEBUG.\n", argv[1]);
            return CHESS_COMMAND_NA_DEBUG_UNDEF;
#endif
        }
        else
        {
            fprintf(stderr, "`%s`: not valid command.\n", argv[1]);
            return CHESS_COMMAND_UNKNOWN;
        }
    }

    game_init(&game, GOPT_CLEAR);
    game_run(&game);

    if (game.done == GAME_DONE_COMM_QUIT)
    {
        printf("Bye\n");
    }
    else if (game.done == GAME_DONE_ASSERT_FAILED)
    {
        fprintf(stderr, "Error: %s.\n%s\n", game.done, game.comm_buf);
        return CHESS_ASSERT_FAILED;
    }
    else if (game.done == GAME_DONE_ASSERT_PARSE)
    {
        fprintf(stderr, "Error: %s.\n", game.done);
        return CHESS_ASSERT_PARSE_FAILED;
    }
    else
    {
        fprintf(stderr, "Error: %s.\n", game.done);
        return CHESS_GAME_ERROR;
    }

    return CHESS_OK;
}

#ifdef DEBUG
static void meminfo(void)
{
    coord_meminfo();
    putchar('\n');

    board_meminfo();
    putchar('\n');

    game_msg_meminfo();
    putchar('\n');

    game_assert_meminfo();
    putchar('\n');

    game_meminfo();
    putchar('\n');

    move_meminfo();
    putchar('\n');
}
#endif
