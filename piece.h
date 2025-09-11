/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef CMC_CHESS_PIECE_H
#define CMC_CHESS_PIECE_H

#include <limits.h>

/* C89 does not provide [u]intN_t types */
#ifndef int8_t
#if CHAR_BIT == 8
#define chess_piece_t char
#else
#error "No suitable 8 bit data type"
#endif
#endif

#define cpEEMPTY ((chess_piece_t)0)
#define cpWPAWN ((chess_piece_t)1)
#define cpWROOK ((chess_piece_t)2)
#define cpWKNIGHT ((chess_piece_t)3)
#define cpWBISHOP ((chess_piece_t)4)
#define cpWQUEEN ((chess_piece_t)5)
#define cpWKING ((chess_piece_t)6)
#define cpBPAWN ((chess_piece_t)(-1))
#define cpBROOK ((chess_piece_t)(-2))
#define cpBKNIGHT ((chess_piece_t)(-3))
#define cpBBISHOP ((chess_piece_t)(-4))
#define cpBQUEEN ((chess_piece_t)(-5))
#define cpBKING ((chess_piece_t)(-6))

extern const char* cpWHITES;
extern const char* cpBLACKS;

extern char chess_piece_to_char(chess_piece_t p);

#endif /* CMC_CHESS_PIECE_H */
