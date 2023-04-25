#!/usr/bin/env bash
set -euo pipefail
set -x

sudo dpkg --add-architecture i386
sudo apt-get update
sudo apt-get install --ignore-hold -y \
  cmake g++-multilib git smpq gettext \
  libsdl2-dev:i386 libsdl2-image-dev:i386 libsodium-dev:i386 \
  libpng-dev:i386 libbz2-dev:i386 libfmt-dev:i386
