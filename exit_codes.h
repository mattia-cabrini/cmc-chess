/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef CMC_CHESS_EXIT_CODES_H
#define CMC_CHESS_EXIT_CODES_H

enum
{
    CHESS_OK                     = 0,
    CHESS_GAME_ERROR             = 1,
    CHESS_ASSERT_FAILED          = 2,
    CHESS_ASSERT_PARSE_FAILED    = 3,
    CHESS_COMMAND_NA_DEBUG_UNDEF = 4,
    CHESS_COMMAND_UNKNOWN        = 5, /* argv[1] */
    CHESS_GAME_IO_NOT_INIT       = 6, /* argv[1] */

    ___cmc_chess_exit_codes_h_enum_sentinel
};

extern const char* chess_error_str(int n);

#endif /* CMC_CHESS_EXIT_CODES_H */
