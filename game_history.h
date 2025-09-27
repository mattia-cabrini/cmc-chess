/* Copyright (c) 2025 Mattia Cabrini      */
/* SPDX-License-Identifier: AGPL-3.0-only */

#ifndef CMC_CHESS_HISTORY_H
#define CMC_CHESS_HISTORY_H

extern void history_init(void);

/* Prints a new line only if str does not end with '\n'; If it does, that is
 * considered as the desired new line. */
extern int history_println(char* str);

/* If fname already exists, history_mv fails and errno is set to EEXIST */
extern int history_mv(const char* fname);

extern void history_close(void);

#endif /* CMC_CHESS_HISTORY_H */
