#!/usr/bin/env bash
# Installs the latest CMake for Ubuntu from the official Kitware repository
# https://apt.kitware.com/

set -euo pipefail
set -x

sudo apt-get update
sudo apt-get install gpg wget

wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | \
	gpg --dearmor - | sudo tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null

echo "deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ $(lsb_release -sc) main" | \
	sudo tee /etc/apt/sources.list.d/kitware.list >/dev/null

sudo rm /usr/share/keyrings/kitware-archive-keyring.gpg
sudo apt-get install kitware-archive-keyring
