/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include "move.h"
#include "coord.h"
#include "int.h"

void move_set_offset(move_p M)
{
    M->offset.row = (myint8_t)(M->dest.row - M->source.row);
    M->offset.col = (myint8_t)(M->dest.col - M->source.col);

    M->abs_offset.row =
        (myuint8_t)(M->offset.row > 0 ? M->offset.row : -M->offset.row);
    M->abs_offset.col =
        (myuint8_t)(M->offset.col > 0 ? M->offset.col : -M->offset.col);
}

void move_init(move_p M, const char* str, size_t n)
{
    const char* trim_str = str;

    if (n >= 4)
        for (trim_str = str; *trim_str == ' '; ++trim_str)
            ;

    /* trim_str + 3 is the last position that should be read to init M:
     * - 0, 1 for source;
     * - 2, 3 for dest.
     *
     * If that position exceeds str boundary (that is str + n - 1), then the
     * command is not vaild.
     */
    if (trim_str + 3 > str + n - 1)
    {
        M->source.row = M->source.col = -1;
        M->dest.row = M->dest.col = -1;
        return;
    }

    coord_init_by_str(&M->source, trim_str);
    coord_init_by_str(&M->dest, trim_str + 2);

    move_set_offset(M);
}
