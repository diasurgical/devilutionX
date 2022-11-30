#!/bin/sh

set -x

SAVE_DIR="$(mktemp -d)"
ln -s "${PWD}/demo_0_reference_spawn_0.sv" "${SAVE_DIR}/"
ln -s "${PWD}/demo_0.dmo" "${SAVE_DIR}/"
cp "${PWD}/spawn_0.sv" "${SAVE_DIR}/"
./devilutionx --diablo --spawn --demo 0 --timedemo --save-dir "$SAVE_DIR" --data-dir ~/.local/share/diasurgical/devilution
rm -rf "$SAVE_DIR"
