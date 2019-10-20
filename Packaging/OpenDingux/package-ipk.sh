#!/usr/bin/env bash

set -euo pipefail

cd "$(dirname "${BASH_SOURCE[0]}")"

readonly OUT="${1:-../../build/devilutionx.ipk}"
readonly IN="${2:-../../build/devilutionx.dge}"

readonly PKG_TARGET=devilutionx
readonly TMP="tmp/${PKG_TARGET}"

pkg_control_get() {
  sed -n control -e 's/^.*'"$1"': //p'
}

readonly PKG_SECTION="$(pkg_control_get Section)"
readonly PKG_INSTALL_DIR="home/retrofw/${PKG_SECTION}/${PKG_TARGET}"
readonly PKG_LOCAL_DIR="home/retrofw/.local/share/diasurgical/devilution"
readonly PKG_MENU_LNK_OUT="home/retrofw/apps/gmenu2x/sections/${PKG_SECTION}/${PKG_TARGET}.lnk"

echo 1>&2 Packaging ${OUT} from ${TMP}...

set -x
rm -rf "${TMP}"
mkdir -p "${TMP}"

# data.tar.gz
mkdir -p "${TMP}/root/${PKG_INSTALL_DIR}" "${TMP}/root/${PKG_LOCAL_DIR}"
cp "$IN" "${TMP}/root/${PKG_INSTALL_DIR}/${PKG_TARGET}.dge"
cp ../resources/Diablo_32.png "${TMP}/root/${PKG_INSTALL_DIR}/devilutionx.png"
cp ../resources/CharisSILB.ttf ../resources/LICENSE.CharisSILB.txt "${TMP}/root/${PKG_INSTALL_DIR}"
cp devilutionx-retrofw.man.txt "${TMP}/root/${PKG_INSTALL_DIR}/devilutionx.man.txt"
cp diablo.ini "${TMP}/root/${PKG_LOCAL_DIR}/diablo.ini"
mkdir -p "${TMP}/root/$(dirname "$PKG_MENU_LNK_OUT")"
printf "%s\n" \
  "title=DevilutionX" \
  "description=$(pkg_control_get Description)" \
  "exec=/${PKG_INSTALL_DIR}/${PKG_TARGET}.dge" \
  > "${TMP}/root/${PKG_MENU_LNK_OUT}"
tar --owner=0 --group=0 -czvf "${TMP}/data.tar.gz" -C "${TMP}/root/" .

# control.tar.gz
sed -e "s/^Version:.*/Version: $(date +%Y%m%d)/" control > "${TMP}/control"
printf "%s\n" \
  "/$PKG_MENU_LNK_OUT" \
  "/${PKG_LOCAL_DIR}/diablo.ini" \
  >> ${TMP}/conffiles
tar --owner=0 --group=0 -czvf "${TMP}/control.tar.gz" -C "${TMP}/" control conffiles

printf '2.0\n' > "${TMP}/debian-binary"
ar r "$OUT" \
  "${TMP}/control.tar.gz" \
  "${TMP}/data.tar.gz" \
  "${TMP}/debian-binary"
