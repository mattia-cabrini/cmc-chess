#!/bin/bash

find "$1" -type f -name "*.txt" | while read fp; do
	echo -n "Testing $fp"
	./cmc-chess < "$fp" &> /dev/null

	if [[ $? -eq 0 ]]; then
		echo " ...OK"
	else
		echo " ...FAILED with code $?"
		exit
	fi
done
