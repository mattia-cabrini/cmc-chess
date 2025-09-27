/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef CMC_CHESS_MOVE_H
#define CMC_CHESS_MOVE_H

#include <stddef.h>

#include "coord.h"

typedef struct move_t
{
    struct coord_t     source;
    struct coord_t     dest;
    struct coord_t     offset;
    struct abs_coord_t abs_offset;
}* move_p;

/**
 * A move is a string like:
 *
 * FNTM:
 * - FN: coordinates from;
 * - TM: coordinates to.
 *
 * For example: "e2c4" means move e2 to c4.
 *
 * Return
 * NULL -> Legal move
 * *    -> ILLEGAL_MOVE_*
 */
extern void move_init(move_p M, const char* str, size_t n);
extern void move_set_offset(move_p M);

#ifdef DEBUG
extern void move_meminfo(void);
#endif

#endif /* CMC_CHESS_MOVE_H */
