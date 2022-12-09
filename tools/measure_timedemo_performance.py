#!/usr/bin/env python

import argparse
import re
import sys
import statistics
import subprocess
from typing import NamedTuple

_TIME_AND_FPS_REGEX = re.compile(rb'\d+ frames, (\d+(?:\.\d+)?) seconds: (\d+(?:\.\d+)?) fps')

class RunMetrics(NamedTuple):
	time: float
	fps: float

def measure(binary: str) -> RunMetrics:
	result: subprocess.CompletedProcess = subprocess.run(
		[binary, '--diablo', '--spawn', '--lang', 'en', '--demo', '0', '--timedemo'], capture_output=True)
	match = _TIME_AND_FPS_REGEX.search(result.stderr)
	if not match:
		raise Exception(f"Failed to parse output in:\n{result.stderr}")
	return RunMetrics(float(match.group(1)), float(match.group(2)))


def main():
	parser = argparse.ArgumentParser()
	parser.add_argument('--binary', help='Path to the devilutionx binary', required=True)
	parser.add_argument('-n', '--num-runs', type=int, default=16, metavar='N')
	args = parser.parse_args()

	num_runs = args.num_runs
	metrics = []
	for i in range(1, num_runs + 1):
		print(f"Run {i:>2} of {num_runs}: ", end='', file=sys.stderr, flush=True)
		run_metrics = measure(args.binary)
		print(f"\t{run_metrics.time:>5.2f} seconds\t{run_metrics.fps:>5.1f} FPS", file=sys.stderr, flush=True)
		metrics.append(run_metrics)

	mean = RunMetrics(statistics.mean(m.time for m in metrics), statistics.mean(m.fps for m in metrics))
	stdev = RunMetrics(statistics.stdev((m.time for m in metrics), mean.time), statistics.stdev((m.fps for m in metrics), mean.fps))
	print(f"{mean.time:.3f} ± {stdev.time:.3f} seconds, {mean.fps:.3f} ± {stdev.fps:.3f} FPS")

main()
