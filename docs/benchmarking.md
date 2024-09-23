# Benchmarking

On Linux, use the `tools/linux_reduced_cpu_variance_run.sh` wrapper
to [reduce CPU variance](https://google.github.io/benchmark/reducing_variance.html).

Timedemo:

```bash
tools/linux_reduced_cpu_variance_run.sh tools/measure_timedemo_performance.py -n 5 --binary build-rel/devilutionx
```

Individual benchmarks (built when `BUILD_TESTING` is `ON`):

```bash
tools/build_and_run_benchmark.py clx_render_benchmark
```

You can pass arguments to the benchmark binary with `--`, e.g.:

```bash
tools/build_and_run_benchmark.py clx_render_benchmark -- --benchmark_repetitions=5
```

The `tools/build_and_run_benchmark.py` script basically does something like this:

```bash
{ [ -d build-reld ] || cmake -S. -Bbuild-reld -DCMAKE_BUILD_TYPE=RelWithDebInfo; } && \
cmake --build build-reld --target clx_render_benchmark && \
tools/linux_reduced_cpu_variance_run.sh build-reld/clx_render_benchmark
```

See `tools/build_and_run_benchmark.py --help` for more information.

You can also [profile](profiling-linux.md) your benchmarks.


## Comparing benchmark runs

You can use [compare.py from Google Benchmark](https://github.com/google/benchmark/blob/main/docs/tools.md) to compare 2 benchmarks.

First, install the tool:

```bash
git clone git@github.com:google/benchmark.git ~/google-benchmark
cd ~/google-benchmark/tools
pip3 install -r requirements.txt
cd -
```

Then, build the 2 binaries that you'd like to compare. For example:

```bash
BASELINE=master
BENCHMARK=dun_render_benchmark

git checkout "$BASELINE"
tools/build_and_run_benchmark.py -B "build-reld-${BASELINE}" --no-run "$BENCHMARK"
git checkout -

tools/build_and_run_benchmark.py --no-run "$BENCHMARK"

tools/linux_reduced_cpu_variance_run.sh ~/google-benchmark/tools/compare.py -a benchmarks \
  "build-reld-${BASELINE}/${BENCHMARK}" "build-reld/${BENCHMARK}" \
  --benchmark_repetitions=10
```
