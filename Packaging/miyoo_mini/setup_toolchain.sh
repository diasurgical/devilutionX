#!/bin/sh

main() {
	install_dependencies
	install_toolchain
}

install_toolchain() {
	TOOLCHAIN_VERSION=v0.0.2
	TOOLCHAIN_TAR="miyoomini-toolchain.tar.xz"

	TOOLCHAIN_ARCH=`uname -m`
	if [ "$TOOLCHAIN_ARCH" = "aarch64" ]; then
		TOOLCHAIN_REPO=miyoomini-toolchain-buildroot-aarch64
	else
		TOOLCHAIN_REPO=miyoomini-toolchain-buildroot
	fi

	TOOLCHAIN_URL="https://github.com/shauninman/$TOOLCHAIN_REPO/releases/download/$TOOLCHAIN_VERSION/$TOOLCHAIN_TAR"

	cd /opt
	wget "$TOOLCHAIN_URL"
	echo "extracting remote toolchain $TOOLCHAIN_VERSION ($TOOLCHAIN_ARCH)"

	tar xf "./$TOOLCHAIN_TAR"
	rm -rf "./$TOOLCHAIN_TAR"
}

install_dependencies() {
	apt-get -y update && apt-get -y install \
		bc \
		build-essential \
		bzip2 \
		bzr \
		cmake \
		cmake-curses-gui \
		cpio \
		git \
		libncurses5-dev \
		make \
		rsync \
		scons \
		tree \
		unzip \
		wget \
		zip
}

main
