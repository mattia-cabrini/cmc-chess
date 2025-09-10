#!/bin/sh

SRC=$(echo *.c)
H=$(echo *.h)

clang-format -i -style=file:clang-format "$SRC" "$H"
