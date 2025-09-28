/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include "exit_codes.h"

static const char* CHESS_OK_STR                  = "OK";
static const char* CHESS_GAME_ERROR_STR          = "Game error";
static const char* CHESS_ASSERT_FAILED_STR       = "Assert failed";
static const char* CHESS_ASSERT_PARSE_FAILED_STR = "Assert parse failed";
static const char* CHESS_COMMAND_NA_DEBUG_UNDEF_STR =
    "Command not available (!def DEBUG)";
static const char* CHESS_COMMAND_UNKNOWN_STR  = "Command unknown";
static const char* CHESS_GAME_IO_NOT_INIT_STR = "Game I/O not initialized";

const char* chess_error_str(int n)
{
    switch (n)
    {
    case CHESS_OK:
        return CHESS_OK_STR;
    case CHESS_GAME_ERROR:
        return CHESS_GAME_ERROR_STR;
    case CHESS_ASSERT_FAILED:
        return CHESS_ASSERT_FAILED_STR;
    case CHESS_ASSERT_PARSE_FAILED:
        return CHESS_ASSERT_PARSE_FAILED_STR;
    case CHESS_COMMAND_NA_DEBUG_UNDEF:
        return CHESS_COMMAND_NA_DEBUG_UNDEF_STR;
    case CHESS_COMMAND_UNKNOWN:
        return CHESS_COMMAND_UNKNOWN_STR;
    case CHESS_GAME_IO_NOT_INIT:
        return CHESS_GAME_IO_NOT_INIT_STR;

    default:
        return "FAILED";
    }
}
