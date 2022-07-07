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
