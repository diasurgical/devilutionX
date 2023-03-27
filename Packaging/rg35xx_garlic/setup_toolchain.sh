#!/bin/sh

main() {
	install_dependencies
	if ! [ -d "/opt/miyoo" ]; then #Can't refresh this as it's possible this is being built in the needed container already (container-ception makes things complicated)
		install_toolchain
	fi
}

install_toolchain() {

	TOOLCHAIN_GIT="https://github.com/edemirkan/rg35xx-toolchain.git"
	TOOLCHAIN_DOCKER_DIR="rg35xx-toolchain"
	TOOLCHAIN_NAME=aveferrum/rg35xx-toolchain

	rm -rf $TOOLCHAIN_DOCKER_DIR
	mkdir $TOOLCHAIN_DOCKER_DIR
	
	cd $TOOLCHAIN_DOCKER_DIR
	git clone $TOOLCHAIN_GIT .

	make shell
	CONTAINER_NAME=$(shell docker ps -f "ancestor=$(TOOLCHAIN_NAME)" --format "{{.Names}}")

	#Extract tool chain from container
	docker cp $CONTAINER_NAME:/opt/miyoo /opt/
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
		gettext \
		git \
		libncurses5-dev \
		make \
		rsync \
		scons \
		tree \
		unzip \
		wget \
		zip \
		docker.io
}

main
