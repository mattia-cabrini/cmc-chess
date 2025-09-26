#!/bin/bash
# Copyright (c) 2025 Mattia Cabrini
# SPDX-License-Identifier: AGPL-3.0-only

ppath=patches

if [ ! -d ignore ]; then
	echo "Directory `ignore` not found; are you in the right directory?"
	exit 1
fi

if [ ! -d ignore/$ppath ]; then
	mkdir ignore/patches
	echo "Created directory ignore/patches"
fi

ppath="ignore/$ppath"
rm "$ppath"/*

echo -n "Subject: "
read subject

echo -n "To: "
read to

# cc stores multiple parameters.
# E.g. if mario@example.com and luca@example.com are in copy, cc will be set to:
# "--cc mario@example.com --cc luca@example.com " <- trailing SPC is not a typo.
cc=""
echo -n "CC: "
read ccpart
while [[ ! -z "$ccpart" ]]; do
	cc="$cc--cc $ccpart "

	echo -n "CC: "
	read ccpart
done

echo -n "SMTP Password: "
read -s smtpPassword

echo "Creating patch..."
git format-patch --subject-prefix="$subject" -s --cover-letter -o $ppath origin..HEAD

echo "Sending mail..."
git send-email --annotate --smtp-pass="$smtpPassword" $ppath/* --to $to $cc
