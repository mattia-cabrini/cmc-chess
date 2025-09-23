/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include "game_assert.h"
#include "board.h"
#include "int.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>

#define MAX_ATTR_NAME_LENGTH 16

/* Read Assert Kind
 * - Move str to first not blank character;
 * - Checks str against all possible values and set A->kind.
 *
 * If all goes well return a pointer to the next char to parse.
 * An error condition can be detected by checking A->kind against
 * ASSERT_KIND_UNKNOWN.
 */
static const char* parse_kind_string(game_assert_p A, const char* str);

/* Read Attribute Name
 * - Move str to first not blank character;
 * - Read the attribute name. That is a contiguous block of not blank characters
 *   of maximum size MAX_ATTR_NAME_LENGTH (including NUL terminator). Parsing
 *   goes on until it reaches '=';
 * - If the assert kind as string is longer than MAX_ATTR_NAME_LENGTH, return
 *   NULL for it is an error condition.
 *
 * If all goes well return a pointer to the next char to parse.
 */
static const char*
parse_attribute_name(const char* str, char* attr_name, size_t attr_name_n);

/* Read An Integer
 * - Move str to the first not blank character;
 * - Read a piece, turn or a boolean. That is at most two characters in the
 *   form `[-]\d`:
 *   - If the first character is '-' the parser reads two characters.
 *   - Otherwise it reads one character.
 * - Return NULL if the first char is NUL;
 * - Return NULL if the first char is '-' and the second character is NUL;
 *
 * If all goes well return a pointer to the next char to parse.
 */
static const char* parse_int(int* n, const char* str);

/* Read Coordidates
 * - Move str to first not blank character;
 * - Read the coordinates. That is exactly two characters `[A-G][1-8]`.
 * - Return NULL if there are not two characters available;
 * - Does not check if read coordinates does not actually translate into a board
 *   cell.
 *
 * If all goes well return a pointer to the next char to parse.
 */
static const char* parse_coord(coord_p A, const char* str);

void game_assert_parse(
    game_assert_p A, const char* str, char* err, size_t err_length
)
{
    char attr_name[MAX_ATTR_NAME_LENGTH];
    int  tmp; /* For piece and turn */

    A->src.row    = -1;
    A->dst.row    = -1;
    A->whence.row = -1;

    A->rev        = 0;

    str           = parse_kind_string(A, str);
    if (A->kind == ASSERT_KIND_UNKNOWN)
    {
        strncpy(err, "could not parse assert kind", err_length);
        return;
    }

    for (; str != NULL;)
    {
        str = parse_attribute_name(str, attr_name, sizeof(attr_name));
        if (str == NULL)
        {
            strncpy(err, "could not read attribute name", err_length);
            break;
        }

        if (!*str)
            break;

        if (streq_ci(attr_name, "src"))
        {
            str = parse_coord(&A->src, str);
            if (str == NULL)
                strncpy(err, "could not read source", err_length);
        }
        else if (streq_ci(attr_name, "dst"))
        {
            str = parse_coord(&A->dst, str);
            if (str == NULL)
                strncpy(err, "could not read destination", err_length);
        }
        else if (streq_ci(attr_name, "whence"))
        {
            str = parse_coord(&A->whence, str);
            if (str == NULL)
                strncpy(err, "could not read destination", err_length);
        }
        else if (streq_ci(attr_name, "piece"))
        {
            str = parse_int(&tmp, str);

            if (str != NULL)
            {
                A->piece = (piece_t)tmp;
                if (piece_to_char(A->piece) == 'E')
                    str = NULL;
            }

            if (str == NULL)
                strncpy(err, "could not read piece", err_length);
        }
        else if (streq_ci(attr_name, "turn"))
        {
            str = parse_int(&tmp, str);
            if (str == NULL)
                strncpy(err, "could not read turn", err_length);
            else
                A->turn = (turn_t)tmp;
        }
        else if (streq_ci(attr_name, "rev"))
        {
            str = parse_int(&tmp, str);
            if (str == NULL)
                strncpy(err, "could not read turn", err_length);
            else
                A->rev = (turn_t)tmp;
        }
    }

    if (str == NULL)
        A->kind = ASSERT_KIND_UNKNOWN;
}

static const char* parse_kind_string(game_assert_p A, const char* str)
{
    str = move_to_not_blank(str);

    if (strneq_ci(str, "piece-can-move", 14))
    {
        A->kind = ASSERT_KIND_PIECE_CAN_MOVE;
        str += 14;
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
    else if (strneq_ci(str, "check", 5))
    {
        A->kind = ASSERT_KIND_CHECK;
        str += 5;
    }
    else
    {
        A->kind = ASSERT_KIND_UNKNOWN;
        return NULL;
    }

    return str;
}

static const char*
parse_attribute_name(const char* str, char* attr_name, size_t attr_name_n)
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

static const char* parse_int(int* n, const char* str)
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

    *n         = (piece_t)atoi(buf);

    return str;
}

static const char* parse_coord(coord_p C, const char* str)
{
    char buf[3];

    str = move_to_not_blank(str);

    if (!*str)
        return NULL; /* Err */
    buf[0] = *(str++);

    if (!*str)
        return NULL; /* Err */
    buf[1] = *(str++);
    buf[2] = '\0';

    coord_init_by_str(C, buf);

    return str;
}
