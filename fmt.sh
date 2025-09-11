#!/bin/sh

fmt() {
	echo "Fmt $1"
	clang-format -i -style=file:clang-format "$1"
}

ls *.c | while read fp; do fmt "$fp"; done
ls *.h | while read fp; do fmt "$fp"; done
