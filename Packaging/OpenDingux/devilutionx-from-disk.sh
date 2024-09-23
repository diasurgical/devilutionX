#!/bin/sh

# Unpacks the mounted OPK to disk before running it
# in order to avoid the memory overhead of squashfs.

OPK_DIR="${PWD}"
STORAGE="$(grep mmcblk /proc/mounts | cut -d' ' -f2 || echo /media/data/local/home)"
UNPACK_DIR="${STORAGE}/devilutionx-opk-on-disk"

set -e
set -x

DO_COPY=1
if [ -f "${UNPACK_DIR}/devilutionx" ]; then
	INSTALLED_MD5="$(md5sum "${UNPACK_DIR}/devilutionx" | cut -d' ' -f1)"
	OPK_MD5="$(md5sum "${PWD}/devilutionx" | cut -d' ' -f1)"
	if [ "$INSTALLED_MD5" = "$OPK_MD5" ]; then
		DO_COPY=0
	fi
fi

if [ "$DO_COPY" = "1" ]; then
	rm -rf "$UNPACK_DIR"
	mkdir -p "$UNPACK_DIR"
	cp -rf "$OPK_DIR"/* "$UNPACK_DIR"
fi

exec "${UNPACK_DIR}/devilutionx-umount-opk-and-run.sh" "${UNPACK_DIR}/devilutionx" "$@"
