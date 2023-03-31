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
client was kept up-to-date by the core SUMO development team.

The avoid this problem, Libtraci is provided as a SWIG compatible C++ client
library that is fully compatible with [Libsumo](Libsumo.md). It even uses the
same header files as Libsumo.

- C++ interface based on static functions and a few simple wrapper
  classes for results which can be linked directly to the client code
- Pre-built language bindings for Java and Python (using
  [SWIG](http://www.swig.org/))
- Future support for other programming languages via SWIG

Unlike Libsumo, Libtraci allows

- multiple clients
- running with [sumo-gui](sumo-gui.md)

# Limitations

The following things currently do not work (or work differently than with the TraCI Python client):

- subscriptions that require additional arguments (except for *vehicle.getLeader*)
- stricter type checking
  - the pure Python TraCI client sometimes accepts any iterable object where Libtraci wants a list
  - pure Python may accept any object where Libtraci needs a boolean value or a string
- there is no cleanup / waiting for the started subprocess (sumo)

# Building and Installing it

The binary windows release already contains the readily compiled libtraci
for C++ and Java. For Python you should prefer the pure Python implementation
anyway (`pip install traci`) but if you must you can install it via `pip install libtraci`.
Only if your platform or language is not supported follow the steps below.

If swig and the developer packages for your target language (e.g. Python or Java)
are installed the build should be enabled by default.
For the Python bindings you will get a libtraci.py and a
_libtraci.so (or .pyd on Windows) in SUMO_HOME/tools/libtraci.
For Java the jar and .so (or .dlls) are placed in the bin dir.
Please add the bin or tools dir to your relevant search paths.
If you want to enable the experimental C# support, make sure that
you have `ENABLE_CS_BINDINGS` set in your cmake configuration.

# Using libtraci

## Python

!!! note
    There is no advantage in using libtraci instead of the standard python traci
    library. The method below is mostly used for testing libtraci.

```py
import libtraci
libtraci.start(["sumo", "-c", "test.sumocfg"])
libtraci.simulationStep()
libtraci.close()
```

Existing traci scripts can mostly be reused by calling

```py
import libtraci as traci
```

In case you have a lot of scripts you can also set the environment
variable `LIBTRACI_AS_TRACI` to a non empty value which will trigger the
import as above.

## C++

### Example Code (test.cpp)

```cpp
#include <iostream>
#include <libsumo/libtraci.h>

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
g++ -o test -std=c++11 -I$SUMO_HOME/src test.cpp -L$SUMO_HOME/bin -ltracicpp
```

### running on Linux

```
LD_LIBRARY_PATH=$SUMO_HOME/bin ./test
```

## Java

You might want to use the available [Maven package](Developer/Maven.md).

### Example Code (APITest.java)

```java
import org.eclipse.sumo.libtraci.*;

public class APITest {
    public static void main(String[] args) {
        System.loadLibrary("libtracijni");
        Simulation.start(new StringVector(new String[] {"sumo", "-n", "net.net.xml"}));
        for (int i = 0; i < 5; i++) {
            Simulation.step();
        }
        Simulation.close();
    }
}
```

See also the information on [casting subscription results](Libsumo.md#casting_subscription_results).

### compiling on Linux

make sure SUMO_HOME is set and the jar / so / dll files are available

```
javac -cp $SUMO_HOME/bin/libtraci-1.8.0-SNAPSHOT.jar APITest.java
```

### running on Linux

```
java -Djava.library.path=$SUMO_HOME/bin -cp $SUMO_HOME/bin/libtraci-1.8.0-SNAPSHOT.jar:. APITest
```

## Matlab

Please install the Python package using `pip install traci`. You can then use all commands inside your Matlab scripts
just as in Python by adding the `py.` prefix. (Please note that this is actually not using libtraci but the pure Python TraCI
implementation.)

```
py.traci.start(["sumo", "-c", "test.sumocfg"])
py.traci.simulationStep()
```
