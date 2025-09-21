#!/bin/bash

if [ -f "$1" ]; then
	./cmc-chess < "$1" > /dev/null
else
	echo "$1 is not a file"
	exit -1
fi
