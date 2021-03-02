---
title: Libtraci
---

# Libtraci

The main way to interact with a running simulation is
[TraCI](TraCI.md) which gives the complete flexibility of doing
cross-platform, cross-language, and networked interaction with
[sumo](sumo.md) acting as a server.
To allow coupling with client code, bindings must be provided for each client
language. Historically, this has led to a larger number of client
implementations with varying levels of API completeness and only the python
client was kept up-to-date by the core sumo development team.

The avoid this problem, Libtraci is provided as a SWIG compatible C++ client
library that is fully compatible with [Libsumo](Libsumo.md). It even uses the
same header files as Libsumo.

- C++ interface based on static functions and a few simple wrapper
  classes for results which can be linked directly to the client code
- Pre-built language bindings for Java and Python (using
  [SWIG](http://www.swig.org/))
- Support for other programming languages via
  [SWIG](http://www.swig.org/)

Unlike Libsumo, Libtraci allows

- multple clients
- running with [sumo-gui](sumo-gui.md)

# Limitations

The following things currently do not work (or work differently than with the TraCI Python client):

- subscriptions that require additional arguments (except for *vehicle.getLeader*)
- stricter type checking
  - the TraCI client sometimes accepts any iterable object where Libsumo wants a list
  - TraCI client may accept any object where Libsumo needs a boolean value
- using traci.init or traci.connect is not possible (you always need to use libsumo.start)
- with traci every TraCIException will generate a message on stderr, Libsumo does not generate this message

# Building it

It currently requires cmake and swig being installed together with the
developer packages for Python (and Java if needed), for Windows see
[Installing/Windows_CMake](Installing/Windows_CMake.md). You
need to (re-)compile sumo yourself under Windows following the remarks
above, under Linux see [Installing/Linux_Build](Installing/Linux_Build.md)
(it is probably just a matter of calling cmake and
make again if you previously did a build without swig).
For the python bindings you will get a libsumo.py and a
_libsumo.so (or .pyd on Windows). If you place them somewhere on your
python path you should be able to use them as described below.

# Using libtraci

## Python

!!! note
    There is no advantage in using libtraci instead of the standard python traci
    library. The method below is mostly used for testing libtraci.

```
import libtraci
libtraci.start(["sumo", "-c", "test.sumocfg"])
libtraci.simulationStep()
```

Existing traci scripts can mostly be reused by calling

```
import libtraci as traci
```

In case you have a lot of scripts you can also set the environment
variable `LIBSUMO_AS_TRACI` to a non empty value which will trigger the
import as above.

## C++

### Example Code (test.cpp)

```
#include <iostream>
#define LIBTRACI
#include <libsumo/Simulation.h>

using namespace libtraci;

int main(int argc, char* argv[]) {
    Simulation::start({"sumo", "-n", "net.net.xml"});
    for (int i = 0; i < 5; i++) {
        Simulation::step();
    }
    Simulation::close();
}
```

### compiling on Linux (make sure SUMO_HOME is set and sumo has been built)

```
g++ -o test -std=c++11 -I$SUMO_HOME/build/cmake-build/src  -I$SUMO_HOME/src test.cpp -L$SUMO_HOME/bin -ltracicpp
```

### running on Linux
```
LD_LIBRARY_PATH=$SUMO_HOME/bin ./test
```
