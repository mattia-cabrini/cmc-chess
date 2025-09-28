/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include "piece.h"

const turn_t cpWTURN = 0x1;
const turn_t cpBTURN = ~0x1;

const char* cpWHITES = ".PRNBQK";
const char* cpBLACKS = ".prnbqk";

char piece_to_char(piece_t p)
{
    int pint = (int)p;

    if (p > 6 || p < -6)
        return 'E';
    if (p > 0)
        return cpWHITES[pint];
    else
        return cpBLACKS[-pint];
}

piece_t piece_from_char(char p, turn_t t)
{
    switch (p & 0xDF)
    {
    case 'P':
        return t == cpWTURN ? cpWPAWN : cpBPAWN;
    case 'R':
        return t == cpWTURN ? cpWROOK : cpBROOK;
    case 'N':
        return t == cpWTURN ? cpWKNIGHT : cpBKNIGHT;
    case 'B':
        return t == cpWTURN ? cpWBISHOP : cpBBISHOP;
    case 'Q':
        return t == cpWTURN ? cpWQUEEN : cpBQUEEN;
    case 'K':
        return t == cpWTURN ? cpWKING : cpBKING;
    }

    return cpEEMPTY;
}
