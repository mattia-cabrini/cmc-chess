/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include "game_assert.h"
#include "board.h"
#include "int.h"
#include "util.h"

#include <string.h>

static const char*
game_assert_parse_kind_string(game_assert_p A, const char* str);
static const char* move_to_not_blank(const char* str);

void game_assert_parse(
    game_assert_p A, const char* str, char* err, size_t err_length
)
{
    strncpy(err, "not implemented, yet", err_length);

    str = game_assert_parse_kind_string(A, str);

    if (A->kind == ASSERT_KIND_UNKNOWN)
    {
        strncpy(err, "could not parse assert kind", err_length);
        return;
    }

    A->kind = ASSERT_KIND_UNKNOWN; /* To signal an error */
}

static const char*
game_assert_parse_kind_string(game_assert_p A, const char* str)
{
    str = move_to_not_blank(str);

    if (streq_ci(str, "check"))
    {
        A->kind = ASSERT_KIND_CHECK;
        str += 5;
    }
    else if (streq_ci(str, "checkmate"))
    {
        A->kind = ASSERT_KIND_CHECKMATE;
        str += 9;
    }
    else if (streq_ci(str, "piece-is"))
    {
        A->kind = ASSERT_KIND_PIECE_IS;
        str += 8;
    }
    else if (streq_ci(str, "piece-can-move"))
    {
        A->kind = ASSERT_KIND_PIECE_CAN_MOVE;
        str += 14;
    }
    else
    {
        A->kind = ASSERT_KIND_UNKNOWN;
        return NULL;
    }

    return str;
}

static const char* move_to_not_blank(const char* str)
{
    for (; *str && *str <= 32; ++str)
        ;

    return str;
}
