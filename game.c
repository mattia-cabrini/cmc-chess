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

static void game_comm_play_move(game_p G);
static void game_comm_dot_dump(game_p G);
static void game_comm_dot_restore(game_p G);

const char* GAME_DONE_COULD_NOT_READ_STDIN = "could not read stdin";
const char* GAME_DONE_COMM_QUIT            = "closed by user";

void game_init(game_p G)
{
    memset(G, 0, sizeof(struct game_t));
    G->turn      = cpWTURN;
    G->comm_type = GX_UNKNOWN;
    board_init(&G->board);
}

void game_run(game_p G)
{
    while (G->done == NULL)
    {
        clear();

        if (G->err[0] != '\0')
        {
            printf("\n%s\n", G->err);
            G->err[0] = '\0';
        }

        board_print(&G->board);

        do
        {
            printf("Command: ");

            game_read_command(G);
            game_decode_command(G);

            if (G->done != NULL)
                return;

            if (G->comm_type == GX_UNKNOWN)
                printf("What did you just say?\n");
        } while (G->comm_type == GX_UNKNOWN);

        switch (G->comm_type)
        {
        case GD_DUMP:
            game_comm_dot_dump(G);
            break;
        case GD_RESTORE:
            game_comm_dot_restore(G);
            break;
        case GP_MOVE:
            game_comm_play_move(G);
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

    if (G->comm_buf[0] == '.')
    {
        if (strneq_ci(G->comm_buf + 1, "dump", 4))
        {
            G->comm_type = GD_DUMP;
            return;
        }
        if (strneq_ci(G->comm_buf + 1, "restore", 7))
        {
            G->comm_type = GD_RESTORE;
            return;
        }
    }

    if (strlen(G->comm_buf) == 4 && G->comm_buf[0] != '?')
    {
        move_init(&G->comm_move, G->comm_buf, sizeof(G->comm_buf));
        G->comm_type = GP_MOVE;
        return;
    }

    G->comm_type = GX_UNKNOWN;
}

static void game_next_turn(game_p G) { G->turn = (turn_t)~G->turn; }

static void game_comm_play_move(game_p G)
{
    char           buf[16];
    const char*    illegal_move;
    struct coord_t whence_check;

    illegal_move =
        board_check_move(&G->board, &G->comm_move, G->turn, &whence_check);
    if (illegal_move != NULL)
    {
        sprintf(G->err, "Illegal move: %s\n", illegal_move);
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
        sprintf(G->err, "%s would take over the King\n", buf);
    }
}

static void game_comm_dot_dump(game_p G)
{
    const char* fpath;
    size_t      spc;
    FILE*       fp;
    int         ok;

    for (spc = 0; G->comm_buf[spc] && G->comm_buf[spc] != ' '; ++spc)
        ;

    fpath = G->comm_buf + spc + 1;

    fp    = fopen(fpath, "w+");
    if (fp == NULL)
    {
        strncpy(G->err, "could not open file", sizeof(G->err));
        return;
    }

    ok = board_dump(&G->board, fp);
    if (!ok)
    {
        strncpy(G->err, "could not write board to file", sizeof(G->err));
        return;
    }

    ok = (int)fwrite(&G->turn, 1, sizeof(G->turn), fp);
    if (ok != (int)sizeof(G->turn))
    {
        strncpy(G->err, "could not write current turn to file", sizeof(G->err));
        return;
    }

    strncpy(G->err, "dump done", sizeof(G->err));
    fclose(fp);
}

static void game_comm_dot_restore(game_p G)
{
    const char* fpath;
    size_t      spc;
    FILE*       fp;
    int         ok;

    for (spc = 0; G->comm_buf[spc] && G->comm_buf[spc] != ' '; ++spc)
        ;

    fpath = G->comm_buf + spc + 1;

    fp    = fopen(fpath, "r");
    if (fp == NULL)
    {
        strncpy(G->err, "could not open file", sizeof(G->err));
        return;
    }

    ok = board_restore(&G->board, fp);
    if (!ok)
    {
        strncpy(G->err, "could not read board from file", sizeof(G->err));
        return;
    }

    ok = (int)fread(&G->turn, 1, sizeof(G->turn), fp);
    if (ok != (int)sizeof(G->turn))
    {
        strncpy(G->err, "could not read turn from file", sizeof(G->err));
        return;
    }

    strncpy(G->err, "restore done", sizeof(G->err));
    fclose(fp);
}
