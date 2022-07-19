#!/bin/sh

progdir=`cd -- "$(dirname "$0")" >/dev/null 2>&1; pwd -P`
savedir="/mnt/SDCARD/Saves/CurrentProfile/saves/DevilutionX"
configdir="/mnt/SDCARD/Saves/CurrentProfile/config/DevilutionX"

cd $progdir
export LD_LIBRARY_PATH="$progdir/lib:$LD_LIBRARY_PATH"
SDL_HIDE_BATTERY=1 ./devilutionx --data-dir $progdir/FILES_HERE --save-dir $savedir --config-dir $configdir