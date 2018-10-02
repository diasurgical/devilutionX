#!/bin/bash

#
# Build script to run inside a Docker container
#

set -e

cmake /src
make -j$(nproc)
