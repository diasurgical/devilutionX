#!/usr/bin/env bash
set -euo pipefail

declare -r DIR="$(dirname "${BASH_SOURCE[0]}")"
cd "$DIR"
declare -r ABSDIR="$(pwd)"

declare -r BUILDROOT_DIR="${BUILDROOT_DIR:-$HOME/rs90-buildroot}"
declare -r SOURCE_CONFIG="${SOURCE_CONFIG:-od_rs90_defconfig}"

set -x

if ! [[ -d "$BUILDROOT_DIR" ]]; then
  mkdir -p "$BUILDROOT_DIR"
  git clone --depth 1 https://github.com/OpenDingux/buildroot.git "$BUILDROOT_DIR"
fi

cd "$BUILDROOT_DIR"
cat "configs/$SOURCE_CONFIG" "$ABSDIR/devilution_buildroot_config" \
  | sed -e "s|^BR2_HOST_DIR=.*|BR2_HOST_DIR=$BUILDROOT_DIR|" \
  > configs/devilution_buildroot_defconfig

make devilution_buildroot_defconfig
BR2_JLEVEL="$(nproc)" nice make
