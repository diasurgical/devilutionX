# Profiling on Linux

If you're trying to make DevilutionX run faster or use less memory, profiling can be very helpful.

## gperftools

[gperftools] is a library that provides a heap profiler and a CPU profiler.

To install gperftools on Debian and Ubuntu, simply run:

```bash
sudo apt install libgoogle-perftools-dev
```

You may also want to install debugging symbols for SDL2:

```bash
sudo apt install libsdl2-dev-dbgsym
```

gperftools by default only comes with a basic visualizer.
[pprof](https://github.com/google/pprof), also from Google, is a more fully-featured profile visualizer
that provides an interactive web server with a flame graph, source annotation, etc.

To install pprof, run:

```bash
go install github.com/google/pprof@latest
```

## CPU profiling with gperftools

```bash
cmake -S. -Bbuild-gperf -DCMAKE_BUILD_TYPE=RelWithDebInfo -DGPERF=ON -DBUILD_TESTING=ON
```

Timedemo:

```bash
tools/build_and_run_benchmark.py --gperf devilutionx -- --diablo --spawn --lang en --demo 0 --timedemo
```

Individual benchmarks (built when `BUILD_TESTING` is `ON`):

```bash
tools/build_and_run_benchmark.py --gperf clx_render_benchmark
```

## Heap profiling with gperftools

Heap profiling produces a graph of all heap allocations that are alive between two points
in a program.

DevilutionX has built-in support for heap allocation via [gperftools].

Then, configure and build DevilutionX with the GPERF option:

```bash
cmake -S. -Bbuild-gperf -DCMAKE_BUILD_TYPE=RelWithDebInfo -DGPERF=ON -DGPERF_HEAP_FIRST_GAME_ITERATION=ON
cmake --build build-gperf -j $(nproc)
```

The `GPERF_HEAP_FIRST_GAME_ITERATION` option will make DevilutionX dump the heap profile of the first game
iteration.

Start DevilutionX and load a game:

```bash
build-gperf/devilutionx
```

Heap profile data will be generated at `main.0001.heap`.

To inspect the profile, run:

```bash
google-pprof --web build-gperf/devilutionx main.0001.heap
```

See [gperftools heap profiling documentation] for more information.

[gperftools]: https://github.com/gperftools/gperftools/wiki

[gperftools heap profiling documentation]: https://gperftools.github.io/gperftools/heapprofile.html
