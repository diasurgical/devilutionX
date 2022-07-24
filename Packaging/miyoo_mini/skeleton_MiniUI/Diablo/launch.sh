#!/bin/sh

cd "$(dirname "$0")"
HOME="$USERDATA_PATH"

if [ -f "DIABDAT.MPQ" ] || [ -f "spawn.mpq" ]; then
	./devilutionx
else
	show "okay.png"
	say "Missing DIABDAT.MPQ!"$'\n\n'"Please see readme.txt"$'\n'"in the Diablo folder"$'\n'"on your SD card."$'\n'
	confirm only
fi
