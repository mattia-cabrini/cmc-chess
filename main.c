/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"

int main(int argc, char** argv)
{
    struct chess_board_t board;

    (void)argc;
    (void)argv;

    chess_board_init(&board);
    chess_board_print(&board);

    exit(0);
}
