/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <sys/stat.h>

#include "board.h"
#include "game.h"
#include "game_assert.h"
#include "game_history.h"
#include "game_io.h"
#include "util.h"

static void game_refresh(game_p G);

static void game_read_command(game_p G);
static void game_decode_command(game_p G);
static void game_next_turn(game_p G);

static void game_comm_play_move(game_p G);

static void game_comm_dot_new(game_p G);
static void game_comm_dot_dump(game_p G);
static void game_comm_dot_restore(game_p G);
static void game_comm_dot_noclear(game_p G);
static void game_comm_dot_save(game_p G, int force);
static void game_comm_dot_comment(game_p G);

static void game_comm_eq_clear(game_p G);
static void game_comm_eq_set(game_p G);
static void game_comm_eq_assert(game_p G);

static void game_comm_qm_list(game_p G);

const char* GAME_DONE_COULD_NOT_READ_STDIN = "could not read stdin";
const char* GAME_DONE_COMM_QUIT            = "closed by user";
const char* GAME_DONE_ASSERT_FAILED        = "assert failed";
const char* GAME_DONE_ASSERT_PARSE         = "could not parse assert";

void game_init(game_p G)
{
    memset(G, 0, sizeof(struct game_t));
    game_msg_init(&G->message);
    G->turn      = cpWTURN;
    G->comm_type = GX_UNKNOWN;

    G->opts |= GOPT_CLEAR;
    G->opts &= ~GOPT_IN_LOAD;

    board_init(&G->board);
}

void game_run(game_p G)
{
    history_init();

    while (G->done == NULL)
    {
        if (G->opts & GOPT_CLEAR)
            clear();

        game_refresh(G);

        do
        {
            game_io_printf("Command: ");

            game_read_command(G);
            game_decode_command(G);

            if (G->done != NULL)
                return;

            if (G->comm_type == GX_UNKNOWN)
                game_io_printf("What did you just say?\n");
        } while (G->comm_type == GX_UNKNOWN);

        switch (G->comm_type)
        {
        case GD_NEW:
            game_comm_dot_new(G);
            break;
        case GD_DUMP:
            game_comm_dot_dump(G);
            break;
        case GD_RESTORE:
            game_comm_dot_restore(G);
            break;
        case GD_NOCLEAR:
            game_comm_dot_noclear(G);
            break;
        case GD_SAVE:
            game_comm_dot_save(G, 0);
            break;
        case GD_SAVE_FORCE:
            game_comm_dot_save(G, 1);
            break;
        case GD_COMMENT:
            if (G->opts & GOPT_IN_LOAD)
                game_comm_dot_comment(G);
            break;
        case GQ_LIST:
            game_comm_qm_list(G);
            break;
        case GE_CLEAR:
            game_comm_eq_clear(G);
            break;
        case GE_SET:
            game_comm_eq_set(G);
            break;
        case GE_ASSERT:
            game_comm_eq_assert(G);
            break;
        case GP_MOVE:
            game_comm_play_move(G);
            break;
        case GX_IGNORE:
            break;
        }
    }

    history_close();
}

static void game_read_command(game_p G)
{
    if (!game_io_gets(G->comm_buf, sizeof(G->comm_buf)))
    {
        G->done = GAME_DONE_COULD_NOT_READ_STDIN;
        return;
    }

    history_println(G->comm_buf);

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
    else if (streq_ci(G->comm_buf, ""))
    {
        G->comm_type = GX_IGNORE;
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
        if (strneq_ci(G->comm_buf + 1, "new", 3))
        {
            G->comm_type = GD_NEW;
            return;
        }
        if (strneq_ci(G->comm_buf + 1, "noclear", 7))
        {
            G->comm_type = GD_NOCLEAR;
            return;
        }
        if (strneq_ci(G->comm_buf + 1, "save!", 5))
        {
            G->comm_type = GD_SAVE_FORCE;
            return;
        }
        if (strneq_ci(G->comm_buf + 1, "save", 4))
        {
            G->comm_type = GD_SAVE;
            return;
        }
        if (strneq_ci(G->comm_buf + 1, ".", 1))
        {
            G->comm_type = GD_COMMENT;
            return;
        }
        break;

    case '=':
        if (strneq_ci(G->comm_buf + 1, "clear", 5))
        {
            G->comm_type = GE_CLEAR;
            return;
        }
        if (strneq_ci(G->comm_buf + 1, "set", 3))
        {
            G->comm_type = GE_SET;
            return;
        }
        if (strneq_ci(G->comm_buf + 1, "assert", 6))
        {
            G->comm_type = GE_ASSERT;
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

    if (G->checkmate != cpEEMPTY)
    {
        game_msg_vappend(
            &G->message,
            "It's checkmate, no good in trying to play pal. Stacce!",
            NULL
        );
        return;
    }

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

static void game_comm_dot_new(game_p G) { game_init(G); }

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

static void game_comm_dot_save(game_p G, int force)
{
    const char* fpath;
    char        mverr[64];
    size_t      spc;
    int         ok;
    struct stat st;

    for (spc = 0; G->comm_buf[spc] && G->comm_buf[spc] != ' '; ++spc)
        ;

    fpath = G->comm_buf + spc + 1;

    if (!force)
    {
        ok = stat(fpath, &st) == 0;
        if (ok)
        {
            game_msg_vappend(
                &G->message,
                "ERROR! COULD NOT SAVE! ",
                fpath,
                " already exists.\n",
                NULL
            );
            return;
        }
        else
        {
            if (errno != ENOENT)
            {
                strerror_r(errno, mverr, sizeof(mverr));
                game_msg_vappend(
                    &G->message, "ERROR! COULD NOT SAVE! ", mverr, "\n", NULL
                );
            }
        }
    }

    ok = history_mv(fpath);

    if (ok)
    {
        game_msg_vappend(&G->message, "saved to ", fpath, "\n", NULL);
        return;
    }

    strerror_r(errno, mverr, sizeof(mverr));
    game_msg_vappend(&G->message, "ERROR! COULD NOT SAVE! ", mverr, "\n", NULL);
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

static void game_comm_eq_clear(game_p G)
{
    struct coord_t src;

    for (src.row = 0; src.row < 8; ++src.row)
        for (src.col = 0; src.col < 8; ++src.col)
            board_set_at(&G->board, &src, cpEEMPTY);

    G->board.wking.row = -1;
    G->board.bking.row = -1;
}

static void game_comm_eq_set(game_p G)
{
    char*          comm_i;
    size_t         cur;
    char           buf_coord[3];
    int            piece;
    struct coord_t dst;

    comm_i = G->comm_buf + 4; /* Command starts at =set ... */

    while (*comm_i && *(comm_i++) == ' ')
        ;
    --comm_i;

    for (cur = 0; cur < 2 && *comm_i; ++cur)
        buf_coord[cur] = *(comm_i++);
    buf_coord[2] = '\0';

    if (sscanf(comm_i, "%d", &piece) != 1)
    {
        game_msg_append(&G->message, "bad format || could not read piece");
        return;
    }

    coord_init_by_str(&dst, buf_coord);

    if (board_coord_out_of_bound(&dst))
    {
        game_msg_append(&G->message, "bad format");
        return;
    }

    if (piece_to_char((piece_t)piece) == 'E')
    {
        game_msg_append(&G->message, "bad argument; unknown piece");
        return;
    }

    if (piece == cpWKING)
    {
        if (!board_coord_out_of_bound(&G->board.wking))
        {
            game_msg_append(
                &G->message,
                "White King has been moved (there can only be one King for "
                "each player)"
            );
            board_set_at(&G->board, &G->board.wking, cpEEMPTY);
        }

        G->board.wking = dst;
    }

    if (piece == cpBKING)
    {
        if (!board_coord_out_of_bound(&G->board.bking))
        {
            game_msg_append(
                &G->message,
                "Black King has been moved (there can only be one King for "
                "each player)"
            );
            board_set_at(&G->board, &G->board.bking, cpEEMPTY);
        }

        G->board.bking = dst;
    }

    if (piece == cpBKING)
        G->board.bking = dst;

    board_set_at(&G->board, &dst, (piece_t)piece);
}

static void game_comm_qm_list(game_p G)
{
    struct coord_t DST[GAME_MAX_MOVES_FOR_ONE_PIECE];
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

    ncord =
        board_list_moves(&G->board, &src, DST, GAME_MAX_MOVES_FOR_ONE_PIECE);

    if (ncord == 0)
        game_msg_vappend(&G->message, "! No move\n", NULL);
    else if (ncord == -1 || ncord > GAME_MAX_MOVES_FOR_ONE_PIECE)
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
        game_io_printf("WHITE King is under check by %s!\n", buf);
    }
    else
    {
        board_under_check_part(&G->board, &G->board.bking, &whence);
        if (whence.row != -1)
        {
            coord_to_str(&whence, buf, sizeof(buf));
            game_io_printf("BLACK King is under check by %s!\n", buf);
        }
    }

    if (board_under_check_mate_part(&G->board, &G->board.wking))
    {
        G->checkmate = cpWTURN;
        game_io_printf("IT'S CHECKMATE PAL!\n");
    }
    else if (board_under_check_mate_part(&G->board, &G->board.bking))
    {
        G->checkmate = cpBTURN;
        game_io_printf("IT'S CHECKMATE PAL!\n");
    }
    else
    {
        G->checkmate = cpEEMPTY;
    }

    if (G->checkmate == cpEEMPTY)
        game_io_printf(
            "It is %s turn.\n", G->turn == cpWTURN ? "WHITE" : "BLACK"
        );

    board_print(&G->board);
    game_msg_flush(&G->message);
}

static void game_comm_eq_assert(game_p G)
{
    struct game_assert_t A;
    char                 err[256];
    int                  positive_result;

    A.turn = G->turn;

    /* C->comm_buf + 7 = G->comm_buf + len of "=assert" */
    game_assert_parse(&A, G->comm_buf + 7, err, sizeof(err));

    if (A.kind == ASSERT_KIND_UNKNOWN)
    {
        game_msg_append(&G->message, err);
        G->done = GAME_DONE_ASSERT_PARSE;
        return;
    }

    positive_result = board_assert(&G->board, &A);
    if ((A.rev && positive_result) || (!A.rev && !positive_result))
    {
        G->done = GAME_DONE_ASSERT_FAILED;
    }
}

static void game_comm_dot_noclear(game_p G)
{
    if (G->opts & GOPT_CLEAR)
        G->opts &= ~GOPT_CLEAR;
    else
        G->opts |= GOPT_CLEAR;
}

static void game_comm_dot_comment(game_p G)
{
    game_msg_append(&G->message, G->comm_buf + 2);
}

#ifdef DEBUG
void game_meminfo(void)
{
    struct game_t T;

    printf("struct game_t: %lu\n", sizeof(T));
    printf(" message:      %lu\n", sizeof(T.message));
    printf(" comm_buf:     %lu\n", sizeof(T.comm_buf));
    printf(" board:        %lu\n", sizeof(T.board));
    printf(" comm_type:    %lu\n", sizeof(T.comm_type));
    printf(" comm_move:    %lu\n", sizeof(T.comm_move));
    printf(" done:         %lu\n", sizeof(T.done));
    printf(" opts:         %lu\n", sizeof(T.opts));
    printf(" turn:         %lu\n", sizeof(T.turn));
    printf(" checkmate:    %lu\n", sizeof(T.checkmate));
    printf(
        " ------------- %lu\n",
        sizeof(T.message) + sizeof(T.board) + sizeof(T.comm_buf) +
            sizeof(T.done) + sizeof(T.turn) + sizeof(T.comm_type) +
            sizeof(T.comm_move) + sizeof(T.checkmate) + sizeof(T.opts)
    );
}
#endif
