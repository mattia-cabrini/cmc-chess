/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "board.h"
#include "game.h"
#include "util.h"

static void game_refresh(game_p G);

static void game_msg_init(game_msg_p E);
static void game_msg_append(game_msg_p E, const char* str);
static void game_msg_vappend(game_msg_p E, ...);
static void game_msg_flush(game_msg_p E);
static void game_msg_clear(game_msg_p E);

static void game_read_command(game_p G);
static void game_decode_command(game_p G);
static void game_next_turn(game_p G);

static void game_comm_play_move(game_p G);
static void game_comm_dot_dump(game_p G);
static void game_comm_dot_restore(game_p G);
static void game_comm_qm_list(game_p G);

const char* GAME_DONE_COULD_NOT_READ_STDIN = "could not read stdin";
const char* GAME_DONE_COMM_QUIT            = "closed by user";

void game_init(game_p G)
{
    memset(G, 0, sizeof(struct game_t));
    game_msg_init(&G->message);
    G->turn      = cpWTURN;
    G->comm_type = GX_UNKNOWN;
    board_init(&G->board);
}

void game_run(game_p G)
{
    while (G->done == NULL)
    {
        clear();

        game_refresh(G);

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
        case GQ_LIST:
            game_comm_qm_list(G);
            break;
        case GP_MOVE:
            game_comm_play_move(G);
            break;
        }
    }
}

static void game_read_command(game_p G)
{
    char* control;

    /* Does not take into account the terminal buffer */
    fflush(stdin);
    control = fgets(G->comm_buf, sizeof(G->comm_buf), stdin);

    if (control == NULL)
    {
        G->done = GAME_DONE_COULD_NOT_READ_STDIN;
        return;
    }

    /* Remove trailing \n or \r */
    trim_right(G->comm_buf);
    trim_left(G->comm_buf);
}

static void game_decode_command(game_p G)
{
    if (streq_ci(G->comm_buf, "quit"))
    {
        G->done = GAME_DONE_COMM_QUIT;
        return;
    }

    switch (G->comm_buf[0])
    {
    case '.':
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
        break;
    case '?':
        trim(G->comm_buf + 1);
        G->comm_type = GQ_LIST;
        return;
        break;
    default:
        if (strlen(G->comm_buf) == 4 && G->comm_buf[0] != '?')
        {
            move_init(&G->comm_move, G->comm_buf, sizeof(G->comm_buf));
            G->comm_type = GP_MOVE;
            return;
        }
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
        game_msg_vappend(
            &G->message, "Illegal move: ", illegal_move, "\n", NULL
        );
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
        game_msg_vappend(&G->message, buf, " would take over the King\n", NULL);
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
        game_msg_append(&G->message, "could not open file");
        return;
    }

    ok = board_dump(&G->board, fp);
    if (!ok)
    {
        game_msg_append(&G->message, "could not write board to file");
        return;
    }

    ok = (int)fwrite(&G->turn, 1, sizeof(G->turn), fp);
    if (ok != (int)sizeof(G->turn))
    {
        game_msg_append(&G->message, "could not write current turn to file");
        return;
    }

    game_msg_append(&G->message, "dump done");
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
        game_msg_append(&G->message, "could not open file");
        return;
    }

    ok = board_restore(&G->board, fp);
    if (!ok)
    {
        game_msg_append(&G->message, "could not read board from file");
        return;
    }

    ok = (int)fread(&G->turn, 1, sizeof(G->turn), fp);
    if (ok != (int)sizeof(G->turn))
    {
        game_msg_append(&G->message, "could not read turn from file");
        return;
    }

    game_msg_append(&G->message, "restore done");
    fclose(fp);
}

static void game_msg_append(game_msg_p E, const char* str)
{
    while (E->cur < E->buf + sizeof(E->buf) - 1 && (*(E->cur++) = *str++))
        ;

    if (E->cur == E->buf + sizeof(E->buf) - 1)
        *E->cur = '\0';
    else
        --E->cur; /* Next time \0 will be overwritten */
}

static void game_msg_flush(game_msg_p E)
{
    puts(E->buf);
    game_msg_clear(E);
}

static void game_msg_clear(game_msg_p E)
{
    E->buf[0] = '\0';
    E->cur    = E->buf;
}

static void game_msg_vappend(game_msg_p E, ...)
{
    const char* strx;
    va_list     args;

    va_start(args, E);
    while ((strx = va_arg(args, const char*)) != NULL)
        game_msg_append(E, strx);

    va_end(args);
}

static void game_msg_init(game_msg_p E) { game_msg_clear(E); }

static void game_comm_qm_list(game_p G)
{
    struct coord_t DST[GAME_MAX_MOVE_FOR_ONE_PIECE];
    struct coord_t src;
    int            cur;
    int            ncord;
    char           buf[3];

    piece_t        king_under_check;
    struct coord_t whence;

    coord_init_by_str(&src, G->comm_buf + 1);
    if (board_coord_out_of_bound(&src))
    {
        game_msg_append(&G->message, "Out of bound.\n");
        return;
    }

    ncord = board_list_moves(&G->board, &src, DST, GAME_MAX_MOVE_FOR_ONE_PIECE);

    if (ncord == 0)
        game_msg_vappend(&G->message, "! No move\n", NULL);
    else if (ncord == -1 || ncord > GAME_MAX_MOVE_FOR_ONE_PIECE)
        game_msg_vappend(&G->message, "! ERR\n", NULL);
    else
    {
        for (cur = 0; cur < ncord; ++cur)
        {
            coord_to_str(DST + cur, buf, sizeof(buf));

            king_under_check =
                board_under_check_part_w(&G->board, &src, DST + cur, &whence);
            if (king_under_check == cpEEMPTY)
            {
                game_msg_vappend(&G->message, "> ", buf, "\n", NULL);
            }
            else if ((king_under_check ^ G->turn) > 0)
            {
                game_msg_vappend(
                    &G->message,
                    "! ",
                    buf,
                    " NO GO would get under check by ",
                    NULL
                );
                coord_to_str(&whence, buf, sizeof(buf));
                game_msg_vappend(&G->message, buf, "\n", NULL);
            }
            else
            {
                game_msg_vappend(
                    &G->message, "> ", buf, " would check\n", NULL
                );
            }
        }
    }
}

static void game_refresh(game_p G)
{
    struct coord_t whence;
    char           buf[3];

    whence.row = -1;
    putchar('\n');

    board_under_check_part(&G->board, &G->board.wking, &whence);
    if (whence.row != -1)
    {
        coord_to_str(&whence, buf, sizeof(buf));
        printf("WHITE King is under check by %s!\n", buf);
    }
    else
    {
        board_under_check_part(&G->board, &G->board.bking, &whence);
        if (whence.row != -1)
        {
            coord_to_str(&whence, buf, sizeof(buf));
            printf("BLACK King is under check by %s!\n", buf);
        }
    }

    printf("It is %s turn.\n", G->turn == cpWTURN ? "WHITE" : "BLACK");

    board_print(&G->board);
    game_msg_flush(&G->message);
}
