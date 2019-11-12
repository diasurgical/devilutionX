#!/usr/bin/env bash

cd "$(dirname "${BASH_SOURCE[0]}")"

readonly OUT="${1:-../../build/devilutionx.opk}"
readonly IN="${2:-../../build/devilutionx.dge}"

readonly TMP="tmp/opk"

echo 1>&2 Packaging ${OUT}...

# To run with SDL2 controller mappings for RG350:
# Exec=env SDL_GAMECONTROLLERCONFIG=190000006c696e6b6465762064657600,RG350,platform:Linux,x:b3,a:b0,b:b1,y:b2,back:b8,start:b9,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,dpup:h0.1,leftshoulder:b4,lefttrigger:b6,rightshoulder:b5,righttrigger:b7,leftstick:b10,rightstick:b11,leftx:a0,lefty:a1,rightx:a2,righty:a3, ./devilutionx.dge
#
# We use SDL1 instead as it's ~3x faster, likely because with SDL1 scaling is done in hardware.

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
