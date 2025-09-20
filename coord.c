/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include "coord.h"
#include "int.h"
#include "util.h"

#include <stdio.h>

#define TO_UPPER_MASK 0xDF

void coord_init_by_str(coord_p C, const char* str)
{
    C->col = (myint8_t)((str[0] & TO_UPPER_MASK) - 'A');
    C->row = (myint8_t)(str[1] - '0' - 1);
}

void coord_to_str(coord_p C, char* buf, size_t n)
{
    if (n < 2)
        return;

    /* Not using board_coord_out_of_bound to avoid circular dependencies, it
     * will not be a problem on a normal board
     */
    if (C->row < 0 || C->row > 7 || C->col < 0 || C->col > 7)
    {
        buf[0] = '#';
        buf[1] = '#';
        return;
    }

    sprintf(buf, "%c%d", 'A' + C->col, C->row + 1);
}

int coord_eq(coord_p A, coord_p B)
{
    return A->row == B->row && A->col == B->col;
}
