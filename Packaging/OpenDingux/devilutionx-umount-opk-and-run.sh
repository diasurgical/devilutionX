#!/bin/sh

set -x
echo | sudo -S umount -l "$PWD"
exec "$@"
