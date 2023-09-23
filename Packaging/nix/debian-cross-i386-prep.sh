#!/usr/bin/env bash
set -euo pipefail
set -x

PACKAGES=(
  cmake git smpq gettext
  libsdl2-dev:i386 libsdl2-image-dev:i386 libsodium-dev:i386
  libpng-dev:i386 libbz2-dev:i386 libfmt-dev:i386 libspeechd-dev:i386
)

if (( $# < 1 )) || [[ "$1" != --no-gcc ]]; then
  PACKAGES+=(g++-multilib)
fi

sudo dpkg --add-architecture i386
sudo apt-get update
sudo apt-get install --ignore-hold -y "${PACKAGES[@]}"

