#!/usr/bin/env bash
# See https://google.github.io/benchmark/reducing_variance.html
set -x

sudo cpupower frequency-set --governor performance 1>/dev/null
echo 0 | sudo tee /sys/devices/system/cpu/cpufreq/boost 1>/dev/null

taskset -c 0 "$@"
{ set +x; } 2> /dev/null
result=$?
set -x

sudo cpupower frequency-set --governor ondemand 1>/dev/null
echo 1 | sudo tee /sys/devices/system/cpu/cpufreq/boost 1>/dev/null

exit "$result"
