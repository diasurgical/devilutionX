#!/usr/bin/env bash

set -euo pipefail

echo "Building for target: clockwork pi GameSH"

declare -r DIR="$(dirname "${BASH_SOURCE[0]}")"
cd "$DIR"
declare -r ABSDIR="$(pwd)"

usage() {
	echo "${BASH_SOURCE[0]} [--target /path/to/devliution/in/gameshell/menu] [--usage]"
	exit 1
}

POSITIONAL=()
while [[ $# -gt 0 ]]
do
key="$1"

case $key in
    -t|--target)
    TARGET="$2"
    shift # past argument
    shift # past value
    ;;
    --help|-h|--usage|-u)
    usage
    shift # past argument
    ;;
    *)    # unknown option
    POSITIONAL+=("$1") # save it in an array for later
    shift # past argument
    ;;
esac
done
set -- "${POSITIONAL[@]}" # restore positional parameters

install_deps() {
	sudo apt install -y cmake libsdl2-dev
}

main() {
	install_deps
	build
	install
}

build() {
	mkdir -p ../../build
	cd ../../build
	rm -f CMakeCache.txt

	cmake .. -DTARGET_PLATFORM=cpigamesh
	make -j $(getconf _NPROCESSORS_ONLN)
	cd -
}

install() {
	mkdir -p /home/cpi/games/devilutionX/bin
	cp /home/cpi/games/devilutionX/build/devilutionx /home/cpi/games/devilutionX/bin
	git rev-parse HEAD > /home/cpi/games/devilutionX/bin/devilutionx.rev

	if [ -z ${TARGET+x} ]; then
		local target_dir="25_devilutionX"
	else
		local target_dir=${TARGET#"/home/cpi/apps/Menu/"}
	fi

	local script_dir="/home/cpi/apps/Menu/$target_dir"

	local target_dir_base=`basename "$target_dir"`
	local target_dir_dir=`dirname "$target_dir"`
	local icon_name="${target_dir_dir}/${target_dir_base#*_}"

	local icon_dir="/home/cpi/launcher/skin/default/Menu/GameShell/${icon_name}.png"

	echo $target_dir
	echo $script_dir
	echo $target_dir_base
	echo $target_dir_dir
	echo $icon_name
	echo $icon_dir

	mkdir -p $script_dir
	cp __init__.py $script_dir
	cp Devilution.png $icon_dir
}

main
