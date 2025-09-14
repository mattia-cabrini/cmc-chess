/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include "board.h"
#include "int.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define TO_UPPER_MASK 0xDF

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

static void move_init_part(const char* str, coord_p C);

static const char* board_is_illegal_PAWN_move(board_p B, move_p M);
static const char* board_is_illegal_ROOK_move(board_p B, move_p M);
static const char* board_is_illegal_KNIGHT_move(board_p B, move_p M);
static const char* board_is_illegal_BISHOP_move(board_p B, move_p M);
static const char* board_is_illegal_QUEEN_move(board_p B, move_p M);
static const char* board_is_illegal_KING_move(board_p B, move_p M);

static const char* board_colour(coord_p C);

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
        printf("\n%d   ", coord.row + 1);
        for (coord.col = 0; coord.col < 8; ++coord.col)
        {
            piece_t p = board_get_at(B, &coord);
            printf("%s%c\x1b[0m ", board_colour(&coord), piece_to_char(p));
        }
    }

    printf("\n\n");
}

const char* board_check_move(board_p B, move_p M, turn_t turn)
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
                tmp.row = M->source.row + 1;
                tmp.col = M->source.col;
                ko      = (board_get_at(B, &tmp) != cpEEMPTY);
            }
        }
        else
        {
            /* Take over */
            ko = ko || (M->offset.row > 1);
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
                tmp.row = M->source.row - 1;
                tmp.col = M->source.col;
                ko      = (board_get_at(B, &tmp) != cpEEMPTY);
            }
        }
        else
        {
            /* Take over */
            ko = ko || (M->offset.row > 1);
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
    int* coord_cur; /* Coord cursor; When iterating on the board using ctmp, one
                       coordinate is constant (col or row) and the other one is
                       variable. This is a pointer to the latter  */
    int incr;       /* 1 is the move goes up, -1 if goes down */
    int actual_offset; /* The value of the actual offset */
    int move_bound;    /* Index of destination row or col */

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

    *coord_cur += incr;
    while ((actual_offset > 0 && *coord_cur < move_bound) ||
           (actual_offset < 0 && *coord_cur > move_bound))
    {
        if (board_get_at(B, &ctmp) != cpEEMPTY)
            /* Trying to jump a piece */
            return ILLEGAL_MOVE_ROOK_DESC;
        *coord_cur += incr;
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

    incr_r = M->offset.row > 0 ? 1 : -1;
    incr_c = M->offset.col > 0 ? 1 : -1;
    ctmp   = M->source;

    ctmp.row += incr_r;
    ctmp.col += incr_c;

    /* The condition could be based both on row and col, but since row and col
     * get incremented together, there is no point in checking both */
    while ((incr_r > 0 && ctmp.row < M->dest.row) ||
           (incr_r < 0 && ctmp.row > M->dest.row))
    {
        if (board_get_at(B, &ctmp) != cpEEMPTY)
            return ILLEGAL_MOVE_BISHOP_DESC;

        ctmp.row += incr_r;
        ctmp.col += incr_c;
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
    (void)B;
    (void)M;

    return ILLEGAL_MOVE_NOT_IMPLEMENTED_YET;
}

static void move_init_part(const char* str, coord_p C)
{
    C->col = (str[0] & TO_UPPER_MASK) - 'A';
    C->row = str[1] - '0' - 1;
}

void move_init(move_p M, const char* str, size_t n)
{
    const char* trim_str = str;

    if (n >= 4)
        for (trim_str = str; *trim_str == ' '; ++trim_str)
            ;

    /* trim_str + 3 is the last position that should be read to init M:
     * - 0, 1 for source;
     * - 2, 3 for dest.
     *
     * If that position exceeds str boundary (that is str + n - 1), then the
     * command is not vaild.
     */
    if (trim_str + 3 > str + n - 1)
    {
        M->source.row = M->source.col = -1;
        M->dest.row = M->dest.col = -1;
        return;
    }

    move_init_part(trim_str, &M->source);
    move_init_part(trim_str + 2, &M->dest);

    M->offset.row = M->dest.row - M->source.row;
    M->offset.col = M->dest.col - M->source.col;

    M->abs_offset.row =
        (unsigned int)(M->offset.row > 0 ? M->offset.row : -M->offset.row);
    M->abs_offset.col =
        (unsigned int)(M->offset.col > 0 ? M->offset.col : -M->offset.col);
}

int board_coord_out_of_bound(coord_p C)
{
    if (C->row < 0 || C->row > 7)
        return 1;
    if (C->col < 0 || C->col > 7)
        return 1;

    return 0;
}

void board_exec(board_p B, move_p M)
{
    piece_t tmp;

    tmp = board_get_at(B, &M->source);
    board_set_at(B, &M->source, cpEEMPTY);
    board_set_at(B, &M->dest, tmp);
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
