/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include "board.h"

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

const char* ILLEGAL_MOVE_PAWN_DEF =
    "pawn in default position can only move forward by two rows or take over "
    "by one diagonal position";
const char* ILLEGAL_MOVE_PAWN_1 = "pawn can only move forward by one row or "
                                  "take over by one diagonal position";
const char* ILLEGAL_MOVE_PAWN_FORWARD = "pawn can only move forward";
const char* ILLEGAL_MOVE_PAWN_TAKE =
    "pawn can only take over by one diagonal position";
const char* ILLEGAL_MOVE_NOT_IMPLEMENTED_YET = "not implemented, yet";

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

const char* board_check_move(board_p B, move_p M)
{
    if (M->source.row == -1)
        return ILLEGAL_MOVE_FMT;

    if (board_coord_out_of_bound(M->source.row, M->source.col))
        return ILLEGAL_MOVE_FROM_OUT_OF_BOUND;

    if (board_coord_out_of_bound(M->dest.row, M->dest.col))
        return ILLEGAL_MOVE_TO_OUT_OF_BOUND;

    if (board_get_at(B, M->source.row, M->source.col) == cpEEMPTY)
        return ILLEGAL_MOVE_FROM_IS_EMPTY;

    switch (board_get_at(B, M->source.row, M->source.col))
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
    piece_t source_piece;
    piece_t dest_piece;

    source_piece = board_get_at(B, M->source.row, M->source.col);
    dest_piece   = board_get_at(B, M->dest.row, M->dest.col);

    if (M->source.col - M->dest.col > 1 || M->source.col - M->dest.col < -1)
        return ILLEGAL_MOVE_PAWN_DEF;

    if (source_piece < 0)
    {
        if (M->dest.row < M->source.row)
            return ILLEGAL_MOVE_PAWN_FORWARD;

        if (M->source.row == 1 && M->dest.row - M->source.row > 2)
            return ILLEGAL_MOVE_PAWN_DEF;

        if (M->source.row != 1 && M->dest.row - M->source.row > 1)
            return ILLEGAL_MOVE_PAWN_1;

        if (M->source.col - M->dest.col != 0 &&
            (dest_piece == cpEEMPTY || dest_piece < 0))
            return ILLEGAL_MOVE_PAWN_TAKE;
    }
    else
    {
        if (M->dest.row > M->source.row)
            return ILLEGAL_MOVE_PAWN_FORWARD;

        if (M->source.row == 6 && M->source.row - M->dest.row > 2)
            return ILLEGAL_MOVE_PAWN_DEF;

        if (M->source.row != 6 && M->source.row - M->dest.row > 1)
            return ILLEGAL_MOVE_PAWN_1;

        if (M->source.col - M->dest.col != 0 &&
            (dest_piece == cpEEMPTY || dest_piece > 0))
            return ILLEGAL_MOVE_PAWN_TAKE;
    }

    return NULL;
}

static const char* board_is_illegal_ROOK_move(board_p B, move_p M)
{
    (void)B;
    (void)M;

    return ILLEGAL_MOVE_NOT_IMPLEMENTED_YET;
}

static const char* board_is_illegal_KNIGHT_move(board_p B, move_p M)
{
    (void)B;
    (void)M;

    return ILLEGAL_MOVE_NOT_IMPLEMENTED_YET;
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
