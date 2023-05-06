#!/usr/bin/env bash
set -euo pipefail
set -x

BUILD_DIR="${1-build}"

mkdir -p "${BUILD_DIR}/package"
find "${BUILD_DIR}/_CPack_Packages/Linux/7Z/" -type f -name 'devilutionx' -exec cp "{}" "${BUILD_DIR}/devilutionx" \;
cp "${BUILD_DIR}/devilutionx" "${BUILD_DIR}/package/devilutionx"
cp "${BUILD_DIR}/devilutionx.mpq" "${BUILD_DIR}/package/devilutionx.mpq"

if which dpkg 2>/dev/null; then
	cp "${BUILD_DIR}/"devilutionx*.deb "${BUILD_DIR}/package/devilutionx.deb"
fi
if which rpmbuild; then
	cp "${BUILD_DIR}/"devilutionx*.rpm "${BUILD_DIR}/package/devilutionx.rpm"
fi

cp ./Packaging/nix/README.txt "${BUILD_DIR}/package/README.txt"
cp ./Packaging/resources/LICENSE.CC-BY.txt "${BUILD_DIR}/package/LICENSE.CC-BY.txt"
cp ./Packaging/resources/LICENSE.OFL.txt "${BUILD_DIR}/package/LICENSE.OFL.txt"
cd "${BUILD_DIR}/package/" && tar -cavf ../../devilutionx.tar.xz *
