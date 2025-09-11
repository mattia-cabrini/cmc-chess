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

chess_piece_t chess_board_get_at(chess_board B, int row, int col)
{
    return B->board[8 * row + col];
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

int chess_board_is_legal_move(chess_board B, char* move)
{
    (void)B;
    (void)move;
    return 0;
}

void chess_board_coord_to_idx(char* coord, int* r, int* c)
{
    *c = (coord[0] & TO_UPPER_MASK) - 'A';
    *r = coord[1] - '0' - 1;
}
