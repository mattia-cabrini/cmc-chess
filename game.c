/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include <stdio.h>
#include <string.h>

#include "board.h"
#include "game.h"
#include "util.h"

static void game_read_command(game_p G);
static void game_decode_command(game_p G);
static void game_next_turn(game_p G);
static void game_comm_move(game_p G);

const char* GAME_DONE_COULD_NOT_READ_STDIN = "could not read stdin";
const char* GAME_DONE_COMM_QUIT            = "closed by user";

void game_init(game_p G)
{
    memset(G, 0, sizeof(struct game_t));
    G->turn      = cpWTURN;
    G->comm_type = UNKNOWN;
    board_init(&G->board);
}

void game_run(game_p G)
{
    while (G->done == NULL)
    {
        /* clear(); */
        board_print(&G->board);

        do
        {
            printf("Command: ");

            game_read_command(G);
            game_decode_command(G);

            if (G->done != NULL)
                return;

            if (G->comm_type == UNKNOWN)
                printf("What did you just say?\n");
        } while (G->comm_type == UNKNOWN);

        switch (G->comm_type)
        {
        case MOVE:
            game_comm_move(G);
        }
    }
}

static void game_read_command(game_p G)
{
    size_t len;
    size_t first_nb;
    size_t cur;
    char*  control;

    /* Does not take into account the terminal buffer */
    fflush(stdin);
    control = fgets(G->comm_buf, sizeof(G->comm_buf), stdin);

    if (control == NULL)
    {
        G->done = GAME_DONE_COULD_NOT_READ_STDIN;
        return;
    }

#ifdef DEBUG
    printf("DEBUG comm: `%s`\n", G->comm_buf);
#endif

    /* Remove trailing \n or \r */
    for (len = strlen(G->comm_buf);; --len)
    {
        if (G->comm_buf[len - 1] == '\r' || G->comm_buf[len - 1] == '\n')
            G->comm_buf[len - 1] = '\0';
        else
            break;

        if (len == 1)
            break;
    }

    len = strlen(G->comm_buf);
    for (first_nb = 0; G->comm_buf[first_nb]; ++first_nb)
    {
        if (G->comm_buf[first_nb] == ' ' || G->comm_buf[first_nb] == '\t')
            continue;

        break;
    }

    for (cur = 0; cur <= len - first_nb; ++cur)
        G->comm_buf[cur] = G->comm_buf[cur + first_nb];

#ifdef DEBUG
    printf("DEBUG comm after trim: `%s`\n", G->comm_buf);
#endif
}

static void game_decode_command(game_p G)
{
    if (streq_ci(G->comm_buf, "quit"))
    {
        G->done = GAME_DONE_COMM_QUIT;
        return;
    }

    if (strlen(G->comm_buf) == 4 && G->comm_buf[0] != '?')
    {
        move_init(&G->comm_move, G->comm_buf, sizeof(G->comm_buf));
        G->comm_type = MOVE;
        return;
    }

    G->comm_type = UNKNOWN;
}

static void game_next_turn(game_p G) { G->turn = (turn_t)~G->turn; }

static void game_comm_move(game_p G)
{
    char           buf[16];
    const char*    illegal_move;
    struct coord_t whence_check;

    illegal_move =
        board_check_move(&G->board, &G->comm_move, G->turn, &whence_check);
    if (illegal_move != NULL)
    {
        printf("Illegal move: %s\n", illegal_move);
        G->comm_buf[0] = '\0';
    }
    else
    {
        board_exec(&G->board, &G->comm_move);
        game_next_turn(G);
    }

    if (illegal_move == ILLEGAL_MOVE_CHECK)
    {
        coord_to_str(&whence_check, buf, sizeof(buf));
        printf("%s would take over the King\n", buf);
    }
}
