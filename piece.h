/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef CMC_CHESS_PIECE_H
#define CMC_CHESS_PIECE_H

#include "int.h"

typedef myint8_t piece_t;
typedef piece_t  turn_t;

/* Using defines bacause const cannot be used in array static initialization */
#define cpEEMPTY ((piece_t)0)
#define cpWPAWN ((piece_t)1)
#define cpWROOK ((piece_t)2)
#define cpWKNIGHT ((piece_t)3)
#define cpWBISHOP ((piece_t)4)
#define cpWQUEEN ((piece_t)5)
#define cpWKING ((piece_t)6)
#define cpBPAWN ((piece_t)(-1))
#define cpBROOK ((piece_t)(-2))
#define cpBKNIGHT ((piece_t)(-3))
#define cpBBISHOP ((piece_t)(-4))
#define cpBQUEEN ((piece_t)(-5))
#define cpBKING ((piece_t)(-6))

/* cpWTURN is always equal to ~cpBTURN */
extern const turn_t cpWTURN;
extern const turn_t cpBTURN;

extern const char* cpWHITES;
extern const char* cpBLACKS;

extern char    piece_to_char(piece_t p);
extern piece_t piece_from_char(char p, turn_t t);

#endif /* CMC_CHESS_PIECE_H */
