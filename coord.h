/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef CMC_CHESS_COORD_H
#define CMC_CHESS_COORD_H

#include "int.h"

#include <stddef.h>

typedef struct coord_t
{
    myint8_t row;
    myint8_t col;
}* coord_p;

typedef struct abs_coord_t
{
    myuint8_t row;
    myuint8_t col;
}* abs_coord_p;

extern void coord_init_by_str(coord_p C, const char* str);
extern void coord_to_str(coord_p C, char* buf, size_t n);
extern int  coord_eq(coord_p A, coord_p B);

#endif /* CMC_CHESS_COORD_H */
