#!/usr/bin/env bash

set -euo pipefail

cd "$(dirname "${BASH_SOURCE[0]}")/../.."

source Packaging/OpenDingux/targets.sh

for target in "${VALID_TARGETS[@]}"; do
	Packaging/OpenDingux/build.sh "$target"
done
