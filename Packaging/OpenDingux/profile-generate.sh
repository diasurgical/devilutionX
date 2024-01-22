#!/bin/sh

set -x

SAVE_DIR="$(mktemp -d)"
cp "${PWD}/demo_0_reference_spawn_0_sv" "${SAVE_DIR}/"
cp "${PWD}/demo_0.dmo" "${SAVE_DIR}/"
cp -r "${PWD}/spawn_0_sv" "${SAVE_DIR}/"
rm -rf "${HOME}/devilutionx-profile"
mkdir -p "${HOME}/devilutionx-profile"
./devilutionx-from-disk.sh --diablo --spawn --demo 0 --timedemo --save-dir "$SAVE_DIR" --data-dir ~/.local/share/diasurgical/devilution
rm -rf "$SAVE_DIR"
