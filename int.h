/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef CMC_CHESS_INT_H
#define CMC_CHESS_INT_H

#include <limits.h>

/* C89 does not provide [u]intN_t types */
#ifndef int8_t

#if CHAR_BIT == 8
typedef char          myint8_t;
typedef unsigned char myuint8_t;
#else
#error "No suitable 8 bit data type"
#endif

#endif

#endif /* CMC_CHESS_INT_H */
