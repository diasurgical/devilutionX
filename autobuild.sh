#!/usr/bin/env bash
#Simple script to properly build the OpenDingux version of DevilutionX.
BASEDIR="$(pwd)"
echo "Downloading OpenDingux toolchain..."
curl -L https://github.com/OpenDingux/buildroot/releases/download/od-2022.09.22/opendingux-gcw0-toolchain.2022-09-22.tar.xz -o gcw0-toolchain.tar.xz
echo "Creating /opt/gcw0-toolchain..."
sudo mkdir -p /opt/gcw0-toolchain
echo "Taking ownership of /opt/gcw0-toolchain..."
sudo chown -R "${USER}:" /opt/gcw0-toolchain
echo "Extracting GCW0 toolchain..."
tar -C /opt -xf gcw0-toolchain.tar.xz
echo "Cleaning up..."
rm gcw0-toolchain.tar.xz
cd /opt/gcw0-toolchain && ./relocate-sdk.sh
cd $BASEDIR
echo "Exporting TOOLCHAIN variable..."
export TOOLCHAIN=/opt/gcw0-toolchain
echo "Beginning build process..."
Packaging/OpenDingux/build.sh rg350
