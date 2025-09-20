/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include "game_assert.h"
#include "board.h"
#include "int.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>

#define MAX_ATTR_NAME_LENGTH 16

static const char*
game_assert_parse_kind_string(game_assert_p A, const char* str);
static const char* game_assert_parse_attribute_name(
    const char* str, char* attr_name, size_t attr_name_n
);
static const char* game_assert_parse_piece(game_assert_p A, const char* str);

void game_assert_parse(
    game_assert_p A, const char* str, char* err, size_t err_length
)
{
    char attr_name[MAX_ATTR_NAME_LENGTH];

    strncpy(err, "not implemented, yet", err_length);

    str = game_assert_parse_kind_string(A, str);
    if (A->kind == ASSERT_KIND_UNKNOWN)
    {
        strncpy(err, "could not parse assert kind", err_length);
        return;
    }

    for (; str != NULL;)
    {
        str =
            game_assert_parse_attribute_name(str, attr_name, sizeof(attr_name));
        if (str == NULL)
        {
            A->kind = ASSERT_KIND_UNKNOWN;
            strncpy(err, "could not read attribute name", err_length);
            return;
        }

        if (!*str)
            break;

        if (streq_ci(attr_name, "src"))
        {
            A->kind = ASSERT_KIND_UNKNOWN;
            break;
        }
        else if (streq_ci(attr_name, "dst"))
        {
            A->kind = ASSERT_KIND_UNKNOWN;
            break;
        }
        else if (streq_ci(attr_name, "piece"))
        {
            str = game_assert_parse_piece(A, str);
            if (str == NULL)
            {
                A->kind = ASSERT_KIND_UNKNOWN;
                strncpy(err, "could not read piece", err_length);
            }
        }
    }
}

static const char*
game_assert_parse_kind_string(game_assert_p A, const char* str)
{
    str = move_to_not_blank(str);

    if (strneq_ci(str, "check", 5))
    {
        A->kind = ASSERT_KIND_CHECK;
        str += 5;
    }
    else if (strneq_ci(str, "checkmate", 9))
    {
        A->kind = ASSERT_KIND_CHECKMATE;
        str += 9;
    }
    else if (strneq_ci(str, "piece-is", 8))
    {
        A->kind = ASSERT_KIND_PIECE_IS;
        str += 8;
    }
    else if (strneq_ci(str, "piece-can-move", 14))
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

static const char* game_assert_parse_attribute_name(
    const char* str, char* attr_name, size_t attr_name_n
)
{
    str = move_to_not_blank(str);

    if (!*str)
        return str;

    for (; attr_name_n > 1 && *str && *str != '='; --attr_name_n)
    {
        *attr_name = *str;
        ++str;
        ++attr_name;
    }

    *attr_name = '\0';
    if (*str == '=')
        return ++str;

    /* Name too long */
    return NULL;
}

static const char* game_assert_parse_piece(game_assert_p A, const char* str)
{
    char   buf[3] = {0, 0, 0};
    size_t cur;

    cur = 0;
    str = move_to_not_blank(str);

    if (*str == '\0')
        return NULL; /* Empty */

    if (*str == '-')
        buf[cur++] = *(str++);

    buf[cur++] = *(str++);

    A->piece   = (piece_t)atoi(buf);

    if (piece_to_char(A->piece) == 'E')
        return NULL; /* Invalid piece */

    return str;
}
