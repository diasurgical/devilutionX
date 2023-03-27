#!/bin/sh
progdir=$(dirname "$0")/Diablo
cd $progdir
HOME=$progdir

./devilutionx
sync
