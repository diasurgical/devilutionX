#!/bin/sh

progdir=`cd -- "$(dirname "$0")" >/dev/null 2>&1; pwd -P`
savedir="/mnt/SDCARD/Saves/CurrentProfile/saves/DevilutionX"
configdir="/mnt/SDCARD/Saves/CurrentProfile/config/DevilutionX"

cd $progdir

if [ -f "./FILES_HERE/DIABDAT.MPQ" ] || [ -f "./FILES_HERE/spawn.mpq" ]; then
	# Timer initialisation
	cd /mnt/SDCARD/App/PlayActivity
	./playActivity "init"

	export LD_LIBRARY_PATH="$progdir/lib:$LD_LIBRARY_PATH"
	SDL_HIDE_BATTERY=1 $progdir/devilutionx --data-dir $progdir/FILES_HERE --save-dir $savedir --config-dir $configdir
  
	# Timer registration
	cd /mnt/SDCARD/App/PlayActivity
	./playActivity "Diablo"
else
	cd "/mnt/SDCARD/Emu/PORTS/Binaries/missingFile"
	./infoPanel
fi
