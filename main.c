/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#include "game.h"

int main(int argc, char** argv)
{
    struct game_t game;

    (void)argc;
    (void)argv;

    game_init(&game);
    game_run(&game);

    return 0;
}
