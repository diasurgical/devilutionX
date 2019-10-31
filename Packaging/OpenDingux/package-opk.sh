#!/usr/bin/env bash

cd "$(dirname "${BASH_SOURCE[0]}")"

readonly OUT="${1:-../../build/devilutionx.opk}"
readonly IN="${2:-../../build/devilutionx.dge}"

readonly TMP="tmp/opk"

echo 1>&2 Packaging ${OUT}...

set -x
rm -rf "${TMP}"
mkdir -p "${TMP}"
cat > "${TMP}/default.gcw0.desktop" <<EOF
[Desktop Entry]
Name=DevilutionX
Comment=A port of DevilutionX for RetroFW.
Exec=devilutionx.dge
Terminal=false
Type=Application
StartupNotify=true
Icon=Diablo_32
Categories=games;
X-OD-Manual=readme.gcw0.txt
X-OD-NeedsDownscaling=true
X-OD-NeedsJoystick=true
EOF

mksquashfs \
  "${TMP}/default.gcw0.desktop" readme.gcw0.txt "$IN" \
  ../resources/Diablo_32.png ../resources/CharisSILB.ttf \
  "$OUT" \
  -all-root -no-xattrs -noappend -no-exports
