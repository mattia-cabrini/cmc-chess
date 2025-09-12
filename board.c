/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include "board.h"

#include <stdio.h>
#include <string.h>

#define TO_UPPER_MASK 0xDF
const chess_piece_t DEFAULT_BOARD[] = {
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

static const char* chess_board_is_illegal_PAWN_move(chess_board B, move M);
static const char* chess_board_is_illegal_ROOK_move(chess_board B, move M);
static const char* chess_board_is_illegal_KNIGHT_move(chess_board B, move M);
static const char* chess_board_is_illegal_BISHOP_move(chess_board B, move M);
static const char* chess_board_is_illegal_QUEEN_move(chess_board B, move M);
static const char* chess_board_is_illegal_KING_move(chess_board B, move M);

chess_piece_t chess_board_get_at(chess_board B, int row, int col)
{
    return B->board[8 * row + col];
}

void chess_board_set_at(chess_board B, int row, int col, chess_piece_t p)
{
    B->board[8 * row + col] = p;
}

void chess_board_init(chess_board B)
{
    memcpy(B->board, DEFAULT_BOARD, sizeof(DEFAULT_BOARD));
}

void chess_board_print(chess_board B)
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
            chess_piece_t p = chess_board_get_at(B, r, c);
            printf("%c ", chess_piece_to_char(p));
        }
    }

    printf("\n\n");
}

const char* chess_board_check_move(chess_board B, move M)
{
    (void)B;

    if (chess_board_coord_out_of_bound(M->source.row, M->source.col))
        return ILLEGAL_MOVE_FROM_OUT_OF_BOUND;

    if (chess_board_coord_out_of_bound(M->dest.row, M->dest.col))
        return ILLEGAL_MOVE_TO_OUT_OF_BOUND;

    if (chess_board_get_at(B, M->source.row, M->source.col) == cpEEMPTY)
        return ILLEGAL_MOVE_FROM_IS_EMPTY;

    switch (chess_board_get_at(B, M->source.row, M->source.col))
    {
    case cpBPAWN:
    case cpWPAWN:
        return chess_board_is_illegal_PAWN_move(B, M);
    case cpBROOK:
    case cpWROOK:
        return chess_board_is_illegal_ROOK_move(B, M);
    case cpBKNIGHT:
    case cpWKNIGHT:
        return chess_board_is_illegal_KNIGHT_move(B, M);
    case cpBBISHOP:
    case cpWBISHOP:
        return chess_board_is_illegal_BISHOP_move(B, M);
    case cpBKING:
    case cpWKING:
        return chess_board_is_illegal_KING_move(B, M);
    case cpBQUEEN:
    case cpWQUEEN:
        return chess_board_is_illegal_QUEEN_move(B, M);
    }

    return NULL;
}

static const char* chess_board_is_illegal_PAWN_move(chess_board B, move M)
{
    chess_piece_t source_piece;
    chess_piece_t dest_piece;

    source_piece = chess_board_get_at(B, M->source.row, M->source.col);
    dest_piece   = chess_board_get_at(B, M->dest.row, M->dest.col);

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

static const char* chess_board_is_illegal_ROOK_move(chess_board B, move M)
{
    (void)B;
    (void)M;

    return ILLEGAL_MOVE_NOT_IMPLEMENTED_YET;
}

static const char* chess_board_is_illegal_KNIGHT_move(chess_board B, move M)
{
    (void)B;
    (void)M;

    return ILLEGAL_MOVE_NOT_IMPLEMENTED_YET;
}

static const char* chess_board_is_illegal_BISHOP_move(chess_board B, move M)
{
    (void)B;
    (void)M;

    return ILLEGAL_MOVE_NOT_IMPLEMENTED_YET;
}

static const char* chess_board_is_illegal_QUEEN_move(chess_board B, move M)
{
    (void)B;
    (void)M;

    return ILLEGAL_MOVE_NOT_IMPLEMENTED_YET;
}

static const char* chess_board_is_illegal_KING_move(chess_board B, move M)
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

void move_init(char* str, move M)
{
    move_init_part(str, &M->source.row, &M->source.col);
    move_init_part(str + 2, &M->dest.row, &M->dest.col);
}

int chess_board_coord_out_of_bound(int r, int c)
{
    if (r < 0 || r > 7)
        return 1;
    if (c < 0 || c > 7)
        return 1;

    return 0;
}

void chess_board_exec(chess_board B, move M)
{
    chess_piece_t tmp;

    tmp = chess_board_get_at(B, M->source.row, M->source.col);
    chess_board_set_at(B, M->source.row, M->source.col, cpEEMPTY);
    chess_board_set_at(B, M->dest.row, M->dest.col, tmp);
}
