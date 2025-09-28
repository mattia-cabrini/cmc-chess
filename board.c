/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include "board.h"
#include "int.h"
#include "util.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

/* Data needed to restore a board from a simulated move */
typedef struct simul_restore_t
{
    piece_t o_dst;
}* simul_restore_p;

const piece_t DEFAULT_BOARD[] = {
    cpBROOK,  cpBKNIGHT, cpBBISHOP, cpBKING,  cpBQUEEN, cpBBISHOP, cpBKNIGHT,
    cpBROOK,  cpBPAWN,   cpBPAWN,   cpBPAWN,  cpBPAWN,  cpBPAWN,   cpBPAWN,
    cpBPAWN,  cpBPAWN,   cpEEMPTY,  cpEEMPTY, cpEEMPTY, cpEEMPTY,  cpEEMPTY,
    cpEEMPTY, cpEEMPTY,  cpEEMPTY,  cpEEMPTY, cpEEMPTY, cpEEMPTY,  cpEEMPTY,
    cpEEMPTY, cpEEMPTY,  cpEEMPTY,  cpEEMPTY, cpEEMPTY, cpEEMPTY,  cpEEMPTY,
    cpEEMPTY, cpEEMPTY,  cpEEMPTY,  cpEEMPTY, cpEEMPTY, cpEEMPTY,  cpEEMPTY,
    cpEEMPTY, cpEEMPTY,  cpEEMPTY,  cpEEMPTY, cpEEMPTY, cpEEMPTY,  cpWPAWN,
    cpWPAWN,  cpWPAWN,   cpWPAWN,   cpWPAWN,  cpWPAWN,  cpWPAWN,   cpWPAWN,
    cpWROOK,  cpWKNIGHT, cpWBISHOP, cpWKING,  cpWQUEEN, cpWBISHOP, cpWKNIGHT,
    cpWROOK
};

const char* ILLEGAL_MOVE_FROM_OUT_OF_BOUND =
    "source coordinates are out of bound";
const char* ILLEGAL_MOVE_TO_OUT_OF_BOUND =
    "destination coordinates are out of bound";
const char* ILLEGAL_MOVE_FROM_IS_EMPTY = "source is empty";
const char* ILLEGAL_MOVE_FMT           = "invalid format";
const char* ILLEGAL_MOVE_NOT_YOUR_TURN = "not your turn";
const char* ILLEGAL_MOVE_NO_MOVE       = "not a move";
const char* ILLEGAL_MOVE_TAKE_OVER_SELF =
    "do you want to take over your own pieces? Crazy";
const char* ILLEGAL_MOVE_NOT_IMPLEMENTED_YET = "not implemented, yet";
const char* ILLEGAL_MOVE_CHECK =
    "cannot move! Do you want to get under check? Crazy";
const char* ILLEGAL_MOVE_PAWN_MORPH = "pawn should morph into something when "
                                      "reaching the other side of the board";

const char* ILLEGAL_MOVE_PAWN_DESC =
    "pawn can only go forward:\n - By two position, if still in default "
    "position;\n - By one position, if not in default position.\n\nA pawn can "
    "only take over by one diagonal position.\n";
const char* ILLEGAL_MOVE_ROOK_DESC =
    "rook can only move horizontally or vertically without jumping pieces";
const char* ILLEGAL_MOVE_KNIGHT_DESC =
    "knight can only move by one vertical and two horizontal OR two vertical "
    "and one horizontal";
const char* ILLEGAL_MOVE_BISHOP_DESC =
    "bishop can only move on a diagonal line, without jumping pieces";
const char* ILLEGAL_MOVE_QUEEN_DESC =
    "queen can move on a diagonal line, a horizontal line or a vertical line, "
    "but without jumping pieces";
const char* ILLEGAL_MOVE_KING_DESC =
    "King can only move one position and cannot take over";

static const char* board_is_illegal_PAWN_move(board_p B, move_p M);
static const char* board_is_illegal_ROOK_move(board_p B, move_p M);
static const char* board_is_illegal_KNIGHT_move(board_p B, move_p M);
static const char* board_is_illegal_BISHOP_move(board_p B, move_p M);
static const char* board_is_illegal_QUEEN_move(board_p B, move_p M);
static const char* board_is_illegal_KING_move(board_p B, move_p M);

/* Check and return any viable straight move by increments of incr.
 * incr should be R/C=-1/-1, for diagonal move going TL, R/C=1/-1, going BL, ...
 */
static size_t board_list_ANY_STRAIGHT_moves(
    board_p, coord_p src, coord_p incr, coord_p dst, size_t n
);
static int board_can_move_nc(board_p B, coord_p src, coord_p king);

/* These functions tell any possible move by source BUT they do not check
 * whether or not a move might result in a check
 */
static size_t
board_list_PAWN_moves(board_p, coord_p src, coord_p dst, size_t n);
static size_t
board_list_ROOK_moves(board_p, coord_p src, coord_p dst, size_t n);
static size_t
board_list_BISHOP_moves(board_p, coord_p src, coord_p dst, size_t n);
static size_t
board_list_QUEEN_moves(board_p, coord_p src, coord_p dst, size_t n);
static size_t
board_list_KNIGHT_moves(board_p, coord_p src, coord_p dst, size_t n);
static size_t
board_list_KING_moves(board_p, coord_p src, coord_p dst, size_t n);

static const char* board_check_move_direction(board_p B, move_p M, turn_t turn);

static const char* board_colour(coord_p C);

/* Initialize R and simulate src->dst on B
 *
 * WARNING
 * No check on src->dst validity.
 */
static void
board_simulation_do(board_p B, simul_restore_p R, coord_p src, coord_p dst);

/* Restore B effectively ending a simulation, using R info */
static void
board_simulation_undo(board_p B, simul_restore_p R, coord_p src, coord_p dst);

piece_t board_get_at(board_p B, coord_p C)
{
    return B->board[8 * C->row + C->col];
}

void board_set_at(board_p B, coord_p C, piece_t p)
{
    B->board[8 * C->row + C->col] = p;
}

void board_init(board_p B)
{
    memcpy(B->board, DEFAULT_BOARD, sizeof(DEFAULT_BOARD));

    B->wking.row = 7;
    B->wking.col = 3;
    B->bking.row = 0;
    B->bking.col = 3;
}

void board_print(board_p B)
{
    struct coord_t coord;

    printf("\n    ");
    for (coord.col = 0; coord.col < 8; ++coord.col)
        printf("%c ", 'A' + coord.col);
    printf("\n");

    for (coord.row = 0; coord.row < 8; ++coord.row)
    {
        printf("\n%d   ", 8 - coord.row);
        for (coord.col = 0; coord.col < 8; ++coord.col)
        {
            piece_t p = board_get_at(B, &coord);
            printf("%s%c\x1b[0m ", board_colour(&coord), piece_to_char(p));
        }
    }

    printf("\n\n");
}

static const char* board_check_move_direction(board_p B, move_p M, turn_t turn)
{
    piece_t source;
    piece_t dest;

    if (M->source.row == -1)
        return ILLEGAL_MOVE_FMT;

    if (M->offset.row == 0 && M->offset.col == 0)
        return ILLEGAL_MOVE_NO_MOVE;

    if (board_coord_out_of_bound(&M->source))
        return ILLEGAL_MOVE_FROM_OUT_OF_BOUND;

    if (board_coord_out_of_bound(&M->dest))
        return ILLEGAL_MOVE_TO_OUT_OF_BOUND;

    if (board_get_at(B, &M->source) == cpEEMPTY)
        return ILLEGAL_MOVE_FROM_IS_EMPTY;

    source = board_get_at(B, &M->source);

    /* True is source and turn does not have the same sign bit */
    if ((source ^ turn) < 0)
        return ILLEGAL_MOVE_NOT_YOUR_TURN;

    dest = board_get_at(B, &M->dest);

    /* True if dest is not empty and does have the same sign bit as source */
    if (dest != cpEEMPTY && (source ^ dest) > 0)
        return ILLEGAL_MOVE_TAKE_OVER_SELF;

    switch (source)
    {
    case cpBPAWN:
    case cpWPAWN:
        return board_is_illegal_PAWN_move(B, M);
    case cpBROOK:
    case cpWROOK:
        return board_is_illegal_ROOK_move(B, M);
    case cpBKNIGHT:
    case cpWKNIGHT:
        return board_is_illegal_KNIGHT_move(B, M);
    case cpBBISHOP:
    case cpWBISHOP:
        return board_is_illegal_BISHOP_move(B, M);
    case cpBKING:
    case cpWKING:
        return board_is_illegal_KING_move(B, M);
    case cpBQUEEN:
    case cpWQUEEN:
        return board_is_illegal_QUEEN_move(B, M);
    }

    return NULL;
}

const char* board_check_move(
    board_p B, move_p M, piece_t pawn_morph, turn_t turn, coord_p whence
)
{
    const char*    err_direction;
    piece_t        o_src;
    piece_t        o_dst;
    struct coord_t o_wking;
    struct coord_t o_bking;

    /* Check direction and fail */
    err_direction = board_check_move_direction(B, M, turn);
    if (err_direction != NULL)
        return err_direction;

    if (M->dest.row == 0 && board_get_at(B, &M->source) == cpWPAWN &&
        !(pawn_morph > 1 && pawn_morph < 6))
        return ILLEGAL_MOVE_PAWN_MORPH;

    if (M->dest.row == 7 && board_get_at(B, &M->source) == cpBPAWN &&
        !(pawn_morph < -1 && pawn_morph > -6))
        return ILLEGAL_MOVE_PAWN_MORPH;

    /* Save the original pieces and simulate execution */
    o_src = board_get_at(B, &M->source);
    o_dst = board_get_at(B, &M->dest);

    board_set_at(B, &M->source, cpEEMPTY);
    board_set_at(B, &M->dest, o_src);

    /* If it is turn white, update position for W king and check for check
     * ...           black  ...                 B ...
     *
     * Then restore W or B king position.
     * Whence is set to -1, -1 in order to determine if a check is detected.
     */

    whence->row = whence->col = -1;
    if (turn > 0)
    {
        o_wking = B->wking;
        if (o_src == cpWKING)
            B->wking = M->dest;

        board_under_check_part(B, &B->wking, whence);

        B->wking = o_wking;
    }
    else
    {
        o_bking = B->bking;
        if (o_src == cpBKING)
            B->bking = M->dest;

        board_under_check_part(B, &B->bking, whence);

        B->bking = o_bking;
    }

    /* Restore the board */
    board_set_at(B, &M->source, o_src);
    board_set_at(B, &M->dest, o_dst);

    /* If whence is changed a check has been detected */
    if (whence->row != -1)
        return ILLEGAL_MOVE_CHECK;

    return NULL;
}

static const char* board_is_illegal_PAWN_move(board_p B, move_p M)
{
    int            ko = 0;
    struct coord_t tmp;
    piece_t        source_piece;
    piece_t        dest_piece;

    source_piece = board_get_at(B, &M->source);
    dest_piece   = board_get_at(B, &M->dest);

    if (M->abs_offset.col > 1)
        return ILLEGAL_MOVE_PAWN_DESC;

    if (source_piece < 0)
    {
        /* Tying to go back */
        if (M->offset.row < 0)
            ko = 1;

        if (M->offset.col == 0)
        {
            /* Move forward */
            ko = ko || (M->source.row == 1 && M->offset.row > 2);
            ko = ko || (M->source.row != 1 && M->offset.row > 1);
            ko = ko || (dest_piece != cpEEMPTY);

            if (!ko && M->offset.row == 2)
            {
                tmp.row = (myint8_t)(M->source.row + 1);
                tmp.col = M->source.col;
                ko      = (board_get_at(B, &tmp) != cpEEMPTY);
            }
        }
        else
        {
            /* Take over */
            ko = ko || (M->abs_offset.row != 1);
            ko = ko || (dest_piece == cpEEMPTY);
        }
    }
    else
    {
        /* Tying to go back */
        if (M->offset.row > 0)
            ko = 1;

        if (M->offset.col == 0)
        {
            /* Move forward */
            ko = ko || (M->source.row == 6 && M->offset.row < -2);
            ko = ko || (M->source.row != 6 && M->offset.row < -1);
            ko = ko || (dest_piece != cpEEMPTY);

            if (!ko && M->offset.row == -2)
            {
                tmp.row = (myint8_t)(M->source.row - 1);
                tmp.col = M->source.col;
                ko      = (board_get_at(B, &tmp) != cpEEMPTY);
            }
        }
        else
        {
            /* Take over */
            ko = ko || (M->abs_offset.row != 1);
            ko = ko || (dest_piece == cpEEMPTY);
        }
    }

    if (ko)
        return ILLEGAL_MOVE_PAWN_DESC;

    return NULL;
}

static const char* board_is_illegal_ROOK_move(board_p B, move_p M)
{
    struct coord_t ctmp; /* Coord to iterate on */
    myint8_t*
        coord_cur; /* Coord cursor; When iterating on the board using ctmp, one
                      coordinate is constant (col or row) and the other one is
                      variable. This is a pointer to the latter */
    myint8_t incr; /* 1 is the move goes up, -1 if goes down */
    myint8_t actual_offset; /* The value of the actual offset */
    myint8_t move_bound;    /* Index of destination row or col */

    /* Moving both horizontally and vertically
     * No need to check if both are zero because it would be a non-move
     */
    if (M->offset.row != 0 && M->offset.col != 0)
        return ILLEGAL_MOVE_ROOK_DESC;

    if (M->offset.col != 0)
    {
        ctmp.row      = M->source.row;
        incr          = M->offset.col > 0 ? 1 : -1;
        coord_cur     = &ctmp.col;
        *coord_cur    = M->source.col;
        actual_offset = M->offset.col;
        move_bound    = M->dest.col;
    }
    else
    {
        ctmp.col      = M->source.col;
        incr          = M->offset.row > 0 ? 1 : -1;
        coord_cur     = &ctmp.row;
        *coord_cur    = M->source.row;
        actual_offset = M->offset.row;
        move_bound    = M->dest.row;
    }

    *coord_cur = (myint8_t)(*coord_cur + incr);
    while ((actual_offset > 0 && *coord_cur < move_bound) ||
           (actual_offset < 0 && *coord_cur > move_bound))
    {
        if (board_get_at(B, &ctmp) != cpEEMPTY)
            /* Trying to jump a piece */
            return ILLEGAL_MOVE_ROOK_DESC;
        *coord_cur = (myint8_t)(*coord_cur + incr);
    }

    return NULL;
}

static const char* board_is_illegal_KNIGHT_move(board_p B, move_p M)
{
    (void)B;

    if (M->abs_offset.row > 2 || M->abs_offset.row == 0)
        return ILLEGAL_MOVE_KNIGHT_DESC;

    if (M->abs_offset.col > 2 || M->abs_offset.col == 0)
        return ILLEGAL_MOVE_KNIGHT_DESC;

    if (M->abs_offset.row == 2 && M->abs_offset.col == 2)
        return ILLEGAL_MOVE_KNIGHT_DESC;

    if (M->abs_offset.row == 1 && M->abs_offset.col != 2)
        return ILLEGAL_MOVE_KNIGHT_DESC;

    if (M->abs_offset.col == 1 && M->abs_offset.row != 2)
        return ILLEGAL_MOVE_KNIGHT_DESC;

    return NULL;
}

static const char* board_is_illegal_BISHOP_move(board_p B, move_p M)
{
    struct coord_t ctmp;   /* Coord to iterate on */
    int            incr_r; /* Increment on row (1 or -1, based on M) */
    int            incr_c; /* Increment on col (1 or -1, based on M) */

    if (M->abs_offset.row != M->abs_offset.col)
        return ILLEGAL_MOVE_BISHOP_DESC;

    incr_r   = M->offset.row > 0 ? 1 : -1;
    incr_c   = M->offset.col > 0 ? 1 : -1;
    ctmp     = M->source;

    ctmp.row = (myint8_t)(ctmp.row + incr_r);
    ctmp.col = (myint8_t)(ctmp.col + incr_c);

    /* The condition could be based both on row and col, but since row and col
     * get incremented together, there is no point in checking both */
    while ((incr_r > 0 && ctmp.row < M->dest.row) ||
           (incr_r < 0 && ctmp.row > M->dest.row))
    {
        if (board_get_at(B, &ctmp) != cpEEMPTY)
            return ILLEGAL_MOVE_BISHOP_DESC;

        ctmp.row = (myint8_t)(ctmp.row + incr_r);
        ctmp.col = (myint8_t)(ctmp.col + incr_c);
    }

    return NULL;
}

static const char* board_is_illegal_QUEEN_move(board_p B, move_p M)
{
    const char* err;

    err = board_is_illegal_ROOK_move(B, M);
    if (err == NULL)
        return NULL;

    err = board_is_illegal_BISHOP_move(B, M);
    if (err == NULL)
        return NULL;

    return ILLEGAL_MOVE_QUEEN_DESC;
}

static const char* board_is_illegal_KING_move(board_p B, move_p M)
{
    if (M->abs_offset.row > 1)
        return ILLEGAL_MOVE_KING_DESC;

    if (M->abs_offset.col > 1)
        return ILLEGAL_MOVE_KING_DESC;

    if (board_get_at(B, &M->dest) != cpEEMPTY)
        return ILLEGAL_MOVE_KING_DESC;

    return NULL;
}

int board_coord_out_of_bound(coord_p C)
{
    if (C->row < 0 || C->row > 7)
        return 1;
    if (C->col < 0 || C->col > 7)
        return 1;

    return 0;
}

void board_exec(board_p B, move_p M, piece_t pawn_morph)
{
    piece_t tmp;

    tmp = board_get_at(B, &M->source);
    board_set_at(B, &M->source, cpEEMPTY);

    if ((tmp == cpWPAWN && M->dest.row == 0) ||
        (tmp == cpBPAWN && M->dest.row == 7))
        board_set_at(B, &M->dest, pawn_morph);
    else
        board_set_at(B, &M->dest, tmp);

    if (tmp == cpWKING)
        B->wking = M->dest;

    if (tmp == cpBKING)
        B->bking = M->dest;
}

static const char* board_colour(coord_p C)
{
    /* I'm only interested in whether row and col are odd or even */
    int row = C->row & 1;
    int col = C->col & 1;

    if (row == col)
        /* Reset; Bold; Foreground Black; Background White */
        return "\x1b[0;1;30;47m";
    else
        /* Reset; Bold; Foreground White; Background Black */
        return "\x1b[0;1;40;37m";
}

void board_under_check_part(board_p B, coord_p king, coord_p whence)
{
    struct move_t mtmp;
    const char*   is_illegal_move;
    piece_t       src;
    piece_t       dst;

    mtmp.dest = *king;
    dst       = board_get_at(B, king);

    for (mtmp.source.row = 0; mtmp.source.row < 8; ++mtmp.source.row)
        for (mtmp.source.col = 0; mtmp.source.col < 8; ++mtmp.source.col)
        {
            /* Not a move */
            if (coord_eq(&mtmp.source, king))
                continue;

            /* Dest empty or same player */
            src = board_get_at(B, &mtmp.source);
            if (src == cpEEMPTY || (src ^ dst) > 0)
                continue;

            move_set_offset(&mtmp);

            is_illegal_move = board_check_move_direction(B, &mtmp, (turn_t)src);
            if (is_illegal_move == NULL)
            {
                *whence = mtmp.source;
                return;
            }
        }
}

int board_dump(board_p B, FILE* fp)
{
    size_t nw;

    nw = fwrite(B, 1, sizeof(struct board_t), fp);
    if (nw != sizeof(struct board_t))
        return 0;

    return 1;
}

int board_restore(board_p B, FILE* fp)
{
    size_t nr;

    nr = fread(B, 1, sizeof(struct board_t), fp);
    if (nr != sizeof(struct board_t))
        return 0;

    return 1;
}

static size_t
board_list_PAWN_moves(board_p B, coord_p src, coord_p dst, size_t n)
{
    piece_t        src_piece;
    struct coord_t possible_dst;
    myint8_t       incr;
    size_t         cur;

    cur = 0;
    assert_return(n > cur, cur);

    src_piece    = board_get_at(B, src);
    incr         = board_get_at(B, src) > 0 ? -1 : 1;
    possible_dst = *src;

    /* Advance by one only if destination is empty */
    possible_dst.row = (myint8_t)(possible_dst.row + incr);
    if (!board_coord_out_of_bound(&possible_dst) &&
        board_get_at(B, &possible_dst) == cpEEMPTY)
    {
        dst[cur++] = possible_dst;
        assert_return(n > cur, cur);
    }

    /* If PAWN is in initial position, let's check if it can advance by two */
    if ((src->row == 1 && incr == 1) || (src->row == 6 && incr == -1))
    {
        possible_dst.row = (myint8_t)(possible_dst.row + incr);
        if (!board_coord_out_of_bound(&possible_dst) &&
            board_get_at(B, &possible_dst) == cpEEMPTY)
        {
            dst[cur++] = possible_dst;
            assert_return(n > cur, cur);
        }

        /* Get back! */
        possible_dst.row = (myint8_t)(possible_dst.row - incr);
    }

    /* Now src is the original src + incr;
     * Let's see if PAWN can take over something
     */
    possible_dst.col = (myint8_t)(possible_dst.col + 1);
    if (!board_coord_out_of_bound(&possible_dst) &&
        board_get_at(B, &possible_dst) != cpEEMPTY &&
        (board_get_at(B, &possible_dst) ^ src_piece) < 0)
    {
        dst[cur++] = possible_dst;
        assert_return(n > cur, cur);
    }

    possible_dst.col = (myint8_t)(possible_dst.col - 2);
    if (!board_coord_out_of_bound(&possible_dst) &&
        board_get_at(B, &possible_dst) != cpEEMPTY &&
        (board_get_at(B, &possible_dst) ^ src_piece) < 0)
        dst[cur++] = possible_dst;

    return cur;
}

static size_t board_list_ANY_STRAIGHT_moves(
    board_p B, coord_p src, coord_p incr, coord_p dst, size_t n
)
{
    struct coord_t candidate;
    piece_t        src_piece;
    size_t         cur;

    assert_return(n > 0, 0);
    cur           = 0;
    candidate     = *src;

    src_piece     = board_get_at(B, src);

    candidate.row = (myint8_t)(candidate.row + incr->row);
    candidate.col = (myint8_t)(candidate.col + incr->col);

    while (!board_coord_out_of_bound(&candidate) && cur < n)
    {
        if (board_get_at(B, &candidate) == cpEEMPTY)
        {
            dst[cur++] = candidate;
        }
        else if ((board_get_at(B, &candidate) ^ src_piece) < 0)
        {
            dst[cur++] = candidate;
            break;
        }
        else
        {
            break;
        }

        candidate.row = (myint8_t)(candidate.row + incr->row);
        candidate.col = (myint8_t)(candidate.col + incr->col);
    }

    return cur;
}

static size_t
board_list_ROOK_moves(board_p B, coord_p src, coord_p dst, size_t n)
{
    struct coord_t incr;
    size_t         cur;

    assert_return(n > 0, 0);
    cur      = 0;

    incr.row = 1;
    incr.col = 0;
    cur += board_list_ANY_STRAIGHT_moves(B, src, &incr, dst + cur, n - cur);

    incr.row = -1;
    incr.col = 0;
    cur += board_list_ANY_STRAIGHT_moves(B, src, &incr, dst + cur, n - cur);

    incr.row = 0;
    incr.col = 1;
    cur += board_list_ANY_STRAIGHT_moves(B, src, &incr, dst + cur, n - cur);

    incr.row = 0;
    incr.col = -1;
    cur += board_list_ANY_STRAIGHT_moves(B, src, &incr, dst + cur, n - cur);

    return cur;
}

static size_t
board_list_BISHOP_moves(board_p B, coord_p src, coord_p dst, size_t n)
{
    struct coord_t incr;
    size_t         cur;

    assert_return(n > 0, 0);
    cur      = 0;

    incr.row = 1;
    incr.col = 1;
    cur += board_list_ANY_STRAIGHT_moves(B, src, &incr, dst + cur, n - cur);

    incr.row = 1;
    incr.col = -1;
    cur += board_list_ANY_STRAIGHT_moves(B, src, &incr, dst + cur, n - cur);

    incr.row = -1;
    incr.col = 1;
    cur += board_list_ANY_STRAIGHT_moves(B, src, &incr, dst + cur, n - cur);

    incr.row = -1;
    incr.col = -1;
    cur += board_list_ANY_STRAIGHT_moves(B, src, &incr, dst + cur, n - cur);

    return cur;
}

static size_t
board_list_QUEEN_moves(board_p B, coord_p src, coord_p dst, size_t n)
{
    size_t cur;

    assert_return(n > 0, 0);
    cur = 0;

    cur += board_list_ROOK_moves(B, src, dst + cur, n - cur);
    cur += board_list_BISHOP_moves(B, src, dst + cur, n - cur);

    return cur;
}

static size_t
board_list_KNIGHT_moves(board_p B, coord_p src, coord_p dst, size_t n)
{
    size_t         cur;
    struct coord_t candidate;
    piece_t        dst_piece;
    piece_t        src_piece;

    assert_return(n > 0, 0);

    cur       = 0;
    src_piece = board_get_at(B, src);

    /* Positions To Test
     *
     * Offset with respect to src
     * ROW -- +2 -2 -2 +2 // +1 -1 -1 +1
     * COL -- +1 +1 -1 -1 // +2 +2 -2 -2
     *
     * Move through the code
     * ROW -- +2 -4 +0 +4 // -1 -2 +0 +2
     * COL -- +1 +0 -2 -0 // +3 +0 -4 -0
     */

    candidate = *src;

    /* Using a MACRO because it is a lot of replicated code:
     * - It is ok to replicate it (the alternative would be a function in the
     * hope that the optimizer would embed it);
     * - It is reasonably simple.
     */
#define board_list_KNIGHT_moves_autogen_check(RR, CC)                          \
    {                                                                          \
        candidate.row = (myint8_t)(candidate.row + (RR));                      \
        candidate.col = (myint8_t)(candidate.col + (CC));                      \
        dst_piece     = board_get_at(B, &candidate);                           \
        if (!board_coord_out_of_bound(&candidate))                             \
        {                                                                      \
            if ((dst_piece == cpEEMPTY || (dst_piece ^ src_piece) < 0))        \
            {                                                                  \
                dst[cur++] = candidate;                                        \
                assert_return(n > cur, cur);                                   \
            }                                                                  \
        }                                                                      \
    }

    board_list_KNIGHT_moves_autogen_check(2, 1);
    board_list_KNIGHT_moves_autogen_check(-4, 0);
    board_list_KNIGHT_moves_autogen_check(0, -2);
    board_list_KNIGHT_moves_autogen_check(4, 0);

    board_list_KNIGHT_moves_autogen_check(-1, 3);
    board_list_KNIGHT_moves_autogen_check(-2, 0);
    board_list_KNIGHT_moves_autogen_check(0, -4);
    board_list_KNIGHT_moves_autogen_check(2, 0);

#undef board_list_KNIGHT_moves_autogen_check

    return cur;
}

static size_t
board_list_KING_moves(board_p B, coord_p src, coord_p dst, size_t n)
{
    size_t         cur;
    struct coord_t candidate;
    piece_t        dst_piece;
    piece_t        src_piece;

    assert_return(n > 0, 0);

    cur           = 0;
    src_piece     = board_get_at(B, src);

    candidate.row = (myint8_t)(src->row - 1);
    candidate.col = (myint8_t)(src->col - 1);

    do
    {
        dst_piece = board_get_at(B, &candidate);
        if (!board_coord_out_of_bound(&candidate))
        {
            if (dst_piece == cpEEMPTY || (dst_piece ^ src_piece) < 0)
            {
                dst[cur++] = candidate;
                assert_return(n > cur, cur);
            }
        }

        /* Increment Sequence
         * ROW -1 | -1 -1  0  1  1  1  0 | -1
         * COL -1 |  0  1  1  1  0 -1 -1 | -1
         */
        switch (candidate.row - src->row)
        {
        case -1:
            if (candidate.col <= src->col)
                candidate.col = (myint8_t)(candidate.col + 1);
            else
                candidate.row = src->row;
            break;
        case 0:
            candidate.row =
                (myint8_t)(candidate.row + candidate.col - src->col);
            break;
        case 1:
            if (candidate.col >= src->col)
                candidate.col = (myint8_t)(candidate.col - 1);
            else
                candidate.row = src->row;
            break;
        }
    } while (candidate.row != src->row - 1 || candidate.col != src->col - 1);

    return cur;
}

int board_list_moves(board_p B, coord_p src, coord_p dst, size_t n)
{
    piece_t src_piece;

    assert_return(n > 0, -1);

    src_piece = board_get_at(B, src);
    switch (src_piece)
    {
    case cpWPAWN:
    case cpBPAWN:
        return (int)board_list_PAWN_moves(B, src, dst, n);
    case cpWROOK:
    case cpBROOK:
        return (int)board_list_ROOK_moves(B, src, dst, n);
    case cpWBISHOP:
    case cpBBISHOP:
        return (int)board_list_BISHOP_moves(B, src, dst, n);
    case cpWQUEEN:
    case cpBQUEEN:
        return (int)board_list_QUEEN_moves(B, src, dst, n);
    case cpWKNIGHT:
    case cpBKNIGHT:
        return (int)board_list_KNIGHT_moves(B, src, dst, n);
    case cpWKING:
    case cpBKING:
        return (int)board_list_KING_moves(B, src, dst, n);
    case cpEEMPTY:
        return 0;
    }

    return -1;
}

static int board_can_move_nc(board_p B, coord_p src, coord_p king)
{
    struct coord_t DST[GAME_MAX_MOVES_FOR_ONE_PIECE];
    struct coord_t whence;
    piece_t        king_under_check;
    int            ncord;
    int            n_actual_cord;
    int            cur;

    ncord         = board_list_moves(B, src, DST, sizeof(DST));
    n_actual_cord = ncord;

    for (cur = 0; cur < ncord; ++cur)
    {
        king_under_check = board_under_check_part_w(B, src, DST + cur, &whence);

        if (king_under_check == board_get_at(B, king))
            --n_actual_cord;
    }

    return n_actual_cord > 0;
}

piece_t
board_under_check_part_w(board_p B, coord_p src, coord_p dst, coord_p whence)
{
    struct simul_restore_t simulation_data;
    piece_t                res;

    res = cpEEMPTY;

    board_simulation_do(B, &simulation_data, src, dst);

    whence->row = -1;

    board_under_check_part(B, &B->wking, whence);

    if (whence->row == -1)
    {
        board_under_check_part(B, &B->bking, whence);

        if (whence->row != -1)
            res = cpBKING;
    }
    else
    {
        res = cpWKING;
    }

    board_simulation_undo(B, &simulation_data, src, dst);

    return res;
}

static void
board_simulation_do(board_p B, simul_restore_p R, coord_p src, coord_p dst)
{
    R->o_dst = board_get_at(B, dst);
    board_set_at(B, dst, board_get_at(B, src));
    board_set_at(B, src, cpEEMPTY);

    if (R->o_dst == cpWKING)
        B->wking.row = B->wking.col = -1;

    if (R->o_dst == cpBKING)
        B->bking.row = B->bking.col = -1;

    if (board_get_at(B, dst) == cpWKING)
        B->wking = *dst;

    if (board_get_at(B, dst) == cpBKING)
        B->bking = *dst;
}

static void
board_simulation_undo(board_p B, simul_restore_p R, coord_p src, coord_p dst)
{
    board_set_at(B, src, board_get_at(B, dst));
    board_set_at(B, dst, R->o_dst);

    if (R->o_dst == cpWKING)
        B->wking = *dst;

    if (R->o_dst == cpBKING)
        B->bking = *dst;

    if (board_get_at(B, src) == cpWKING)
        B->wking = *src;

    if (board_get_at(B, src) == cpBKING)
        B->bking = *src;
}

int board_under_check_mate_part(board_p B, coord_p king)
{
    int            res;
    struct coord_t src;

    res = 0;

    /* If the king is not on the board (custom game / learning / or debug) */
    if (board_coord_out_of_bound(king))
        return 0;

    for (src.row = 0; !res && src.row < 8; ++src.row)
        for (src.col = 0; !res && src.col < 8; ++src.col)
            if (board_get_at(B, &src) != cpEEMPTY &&
                (board_get_at(B, &src) ^ board_get_at(B, king)) >= 0)
                res = board_can_move_nc(B, &src, king);

    return !res;
}

int board_assert(board_p B, game_assert_p A)
{
    struct coord_t whence;
    struct move_t  M;
    const char*    err;

    switch (A->kind)
    {
    case ASSERT_KIND_CHECK:
        whence.row = -1;
        board_under_check_part(B, &A->src, &whence);
        if (whence.row != -1)
        {
            if (!board_coord_out_of_bound(&A->whence))
                return coord_eq(&A->whence, &whence);
            else
                return 1;
        }

        break;
    case ASSERT_KIND_CHECKMATE:
        if (board_under_check_mate_part(B, &A->src))
            return 1;
        break;
    case ASSERT_KIND_PIECE_IS:
        if (!board_coord_out_of_bound(&A->src) &&
            board_get_at(B, &A->src) == A->piece)
            return 1;
        break;
    case ASSERT_KIND_PIECE_CAN_MOVE:
        if (!board_coord_out_of_bound(&A->src) &&
            !board_coord_out_of_bound(&A->dst))
        {
            M.source = A->src;
            M.dest   = A->dst;

            move_set_offset(&M);

            err = board_check_move(B, &M, A->pawn_morph, A->turn, &whence);

            /* Can move: ok */
            if (err == NULL)
                return 1;

            /* Cannot move:
             * - If A->whence is set, check it against whence. The user is
             *   trying to asses if check detection is correct;
             * - If A->whence is not set, assert has failed.
             */
            else if (!board_coord_out_of_bound(&A->whence))
                return coord_eq(&A->whence, &whence);
        }
        break;
    }

    return 0;
}

#ifdef DEBUG
void board_meminfo(void)
{
    struct board_t T;

    printf("struct board_t: %lu\n", sizeof(T));
    printf(" board:         %lu\n", sizeof(T.board));
    printf(" wking:         %lu\n", sizeof(T.wking));
    printf(" bking:         %lu\n", sizeof(T.bking));
    printf(
        " -------------- %lu\n",
        sizeof(T.board) + sizeof(T.wking) + sizeof(T.bking)
    );
}
#endif
