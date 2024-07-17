#!/usr/bin/env bash
set -euo pipefail

SDL_BASE=https://raw.githubusercontent.com/libsdl-org/SDL/release-2.30.5
FILES=(
	HIDDevice.java
	HIDDeviceBLESteamController.java
	HIDDeviceManager.java
	HIDDeviceUSB.java
	SDL.java
	SDLActivity.java
	SDLAudioManager.java
	SDLControllerManager.java
	SDLSurface.java
)

for f in "${FILES[@]}"; do
	set -x
	curl -L -O -s "${SDL_BASE}/android-project/app/src/main/java/org/libsdl/app/${f}" \
		--output-dir android-project/app/src/main/java/org/libsdl/app/
	{ set +x; } 2> /dev/null
done
>&2 echo "Done. Remember to manually check for and sync changes in XML files, such as AndroidManifest.xml"
