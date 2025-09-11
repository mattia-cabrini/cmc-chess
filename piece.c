/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include "piece.h"

const char* cpWHITES = ".PRNBQK";
const char* cpBLACKS = ".prnbqk";

char chess_piece_to_char(chess_piece_t p)
{
    int pint = (int)p;

    if (p > 6 || p < -6)
        return 'E';
    if (p > 0)
        return cpWHITES[pint];
    else
        return cpBLACKS[-pint];
}
