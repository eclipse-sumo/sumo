---
title: Profiling SUMO
---

# Introduction

Different approaches can be used to profile SUMO. For the C++ part we traditionally employed
gprof. Newer approaches use either the perf Linux tools or valgrind. All these tools have
only been tried on Linux.

For python the tool to use is cProfile.

# Profiling C++

This is mostly a summary of https://stackoverflow.com/questions/375913/how-do-i-profile-c-code-running-on-linux/60265409#60265409

## perf

This is the preferred tool. It is fast does not need a special build and the flamegraphs look nice. The only downside is the nasty setup messing with the sysctl.

The process is as follows:

- Install perf tools `sudo apt install linux-tools-$(uname -r)`.
- Run `perf record` to check whether you are actually allowed to record kernel events (you might need to abort the call with <kbd>Ctrl</kbd> + <kbd>c</kbd>).
  - If not and you want to enable it permanently do `sudo sh -c 'echo "kernel.perf_event_paranoid = -1\nkernel.kptr_restrict = 0" >> /etc/sysctl.conf'`.
- Run `perf record --call-graph dwarf sumo -c test.sumocfg`. This will generate a file named perf.data.
- Inspect interactively with `perf report`.
- Optionally do a flamegraph (use a web browser for the resulting svg)
  - `git clone https://github.com/brendangregg/FlameGraph`
  - `perf script | FlameGraph/stackcollapse-perf.pl | FlameGraph/flamegraph.pl > flamegraph.svg`

## gprof

This is second best if you do not want to or cannot use the method above. It requires recompilation of sumo but usually you don't need root access
because most of the time the tooling (except for the visual part) is already installed with the compiler.

The process is as follows:

- Install gprof `sudo apt install binutils`.
- Enable code instrumentation either by choosing the "Profiling" build variant in VS Code or by enabling `-DPROFILING=ON` when running cmake. This will by default add a suffix `P` to all executable names.
- Run your code e.g. `$SUMO_HOME/bin/sumoP -c test.sumocfg`. This will generate a file named gmon.out.
- Generate textual output e.g. `gprof $SUMO_HOME/bin/sumoP gmon.out > gprof.log`.
- Optionally use a visual tool
  - `python3 -m pip install gprof2dot`.
  - `gprof2dot < gprof.log > gprof.dot`.
  - `sudo apt install xdot`
  - `xdot gprof.dot`

## valgrind

TBD

# Python profiling

TBD
