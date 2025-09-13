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

static void move_init_part(char* str, int* r, int* c);

static const char* board_is_illegal_PAWN_move(board_p B, move_p M);
static const char* board_is_illegal_ROOK_move(board_p B, move_p M);
static const char* board_is_illegal_KNIGHT_move(board_p B, move_p M);
static const char* board_is_illegal_BISHOP_move(board_p B, move_p M);
static const char* board_is_illegal_QUEEN_move(board_p B, move_p M);
static const char* board_is_illegal_KING_move(board_p B, move_p M);

static const char* board_colour(int row, int col);

piece_t board_get_at(board_p B, int row, int col)
{
    return B->board[8 * row + col];
}

void board_set_at(board_p B, int row, int col, piece_t p)
{
    B->board[8 * row + col] = p;
}

void board_init(board_p B)
{
    memcpy(B->board, DEFAULT_BOARD, sizeof(DEFAULT_BOARD));
}

void board_print(board_p B)
{
    int r;
    int c;

    printf("\n    ");
    for (c = 0; c < 8; ++c)
        printf("%c ", 'A' + c);
    printf("\n");

    for (r = 0; r < 8; ++r)
    {
        printf("\n%d   ", r + 1);
        for (c = 0; c < 8; ++c)
        {
            piece_t p = board_get_at(B, r, c);
            printf("%s%c\x1b[0m ", board_colour(r, c), piece_to_char(p));
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

    if (board_coord_out_of_bound(M->source.row, M->source.col))
        return ILLEGAL_MOVE_FROM_OUT_OF_BOUND;

    if (board_coord_out_of_bound(M->dest.row, M->dest.col))
        return ILLEGAL_MOVE_TO_OUT_OF_BOUND;

    if (board_get_at(B, M->source.row, M->source.col) == cpEEMPTY)
        return ILLEGAL_MOVE_FROM_IS_EMPTY;

    source = board_get_at(B, M->source.row, M->source.col);

    /* True is source and turn does not have the same sign bit */
    if ((source ^ turn) < 0)
        return ILLEGAL_MOVE_NOT_YOUR_TURN;

    dest = board_get_at(B, M->dest.row, M->dest.col);

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
    int     ko = 0;
    piece_t source_piece;
    piece_t dest_piece;

    source_piece = board_get_at(B, M->source.row, M->source.col);
    dest_piece   = board_get_at(B, M->dest.row, M->dest.col);

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

            ko = ko || (M->offset.row == 2 &&
                        board_get_at(B, M->source.row + 1, M->source.col) !=
                            cpEEMPTY);
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

            ko = ko || (M->offset.row == 2 &&
                        board_get_at(B, M->source.row - 1, M->source.col) !=
                            cpEEMPTY);
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
    int cur;

    /* Moving both horizontally and vertically */
    if (M->offset.row != 0 && M->offset.col != 0)
        return ILLEGAL_MOVE_ROOK_DESC;

    if (M->offset.col > 0)
    {
        for (cur = M->source.col + 1; cur < M->dest.col; ++cur)
            if (board_get_at(B, M->source.row, cur) != cpEEMPTY)
                /* Trying to jump a piece */
                return ILLEGAL_MOVE_ROOK_DESC;
    }
    else if (M->offset.col < 0)
    {
        for (cur = M->source.col - 1; cur > M->dest.col; --cur)
            if (board_get_at(B, M->source.row, cur) != cpEEMPTY)
                /* Trying to jump a piece */
                return ILLEGAL_MOVE_ROOK_DESC;
    }
    else if (M->offset.row > 0)
    {
        for (cur = M->source.row + 1; cur < M->dest.row; ++cur)
            if (board_get_at(B, cur, M->source.col) != cpEEMPTY)
                /* Trying to jump a piece */
                return ILLEGAL_MOVE_ROOK_DESC;
    }
    else if (M->offset.row < 0)
    {
        for (cur = M->source.row - 1; cur > M->dest.row; --cur)
            if (board_get_at(B, cur, M->source.col) != cpEEMPTY)
                /* Trying to jump a piece */
                return ILLEGAL_MOVE_ROOK_DESC;
    }

    return NULL;
}

static const char* board_is_illegal_KNIGHT_move(board_p B, move_p M)
{
    (void)B;

    if (M->abs_offset.row == 1 && M->abs_offset.col != 2)
        return ILLEGAL_MOVE_KNIGHT_DESC;

    if (M->abs_offset.col == 1 && M->abs_offset.row != 2)
        return ILLEGAL_MOVE_KNIGHT_DESC;

    return NULL;
}

static const char* board_is_illegal_BISHOP_move(board_p B, move_p M)
{
    (void)B;
    (void)M;

    return ILLEGAL_MOVE_NOT_IMPLEMENTED_YET;
}

static const char* board_is_illegal_QUEEN_move(board_p B, move_p M)
{
    (void)B;
    (void)M;

    return ILLEGAL_MOVE_NOT_IMPLEMENTED_YET;
}

static const char* board_is_illegal_KING_move(board_p B, move_p M)
{
    (void)B;
    (void)M;

    return ILLEGAL_MOVE_NOT_IMPLEMENTED_YET;
}

static void move_init_part(char* str, int* r, int* c)
{
    *c = (str[0] & TO_UPPER_MASK) - 'A';
    *r = str[1] - '0' - 1;
}

void move_init(move_p M, char* str, size_t n)
{
    char* trim_str = str;

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

    move_init_part(trim_str, &M->source.row, &M->source.col);
    move_init_part(trim_str + 2, &M->dest.row, &M->dest.col);

    M->offset.row = M->dest.row - M->source.row;
    M->offset.col = M->dest.col - M->source.col;

    M->abs_offset.row =
        (unsigned int)(M->offset.row > 0 ? M->offset.row : -M->offset.row);
    M->abs_offset.col =
        (unsigned int)(M->offset.col > 0 ? M->offset.col : -M->offset.col);
}

int board_coord_out_of_bound(int r, int c)
{
    if (r < 0 || r > 7)
        return 1;
    if (c < 0 || c > 7)
        return 1;

    return 0;
}

void board_exec(board_p B, move_p M)
{
    piece_t tmp;

    tmp = board_get_at(B, M->source.row, M->source.col);
    board_set_at(B, M->source.row, M->source.col, cpEEMPTY);
    board_set_at(B, M->dest.row, M->dest.col, tmp);
}

static const char* board_colour(int row, int col)
{
    /* I'm only interested in whether row and col are odd or even */
    row &= 1;
    col &= 1;

    if (row == col)
        /* Reset; Bold; Foreground Black; Background White */
        return "\x1b[0;1;30;47m";
    else
        /* Reset; Bold; Foreground White; Background Black */
        return "\x1b[0;1;40;37m";
}
