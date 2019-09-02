---
title: Libsumo
permalink: /Libsumo/
---

# Libsumo

The main way to interact with a running simulation is
[TraCI](TraCI.md) which gives the complete flexibility of doing
cross-platform, cross-language, and networked interaction with
[SUMO](SUMO.md) acting as a server. One major drawback is the
communication overhead due to the protocol and the socket communication.
To have a more efficient coupling without the need for socket
communication, the TraCI API is provided as a C++ library with the
followin properties:

- C++ interface based on static functions and a few simple wrapper
  classes for results which can be linked directly to the client code
- Function signatures similar to [TraCI](TraCI.md)
- Pre-built language bindings for Java and Python (using
  [SWIG](http://www.swig.org/))
- Support for other programming languages via
  [SWIG](http://www.swig.org/)

# Limitations

The following things currently do not work:

- running with [SUMO-GUI](SUMO-GUI.md)
- subscriptions that require additional arguments
  (*vehicle.getLeader*)

# Downloading Libsumo

Libsumo is not part of the default package. It can be downloaded from
the extended package
[\[1\]](http://sumo.dlr.de/daily/sumo-msvc12extrax64-git.zip) as part of the
[nightly build](Downloads.md#nightly_snapshots).

# Building it

It currently requires cmake and swig being installed together with the
developer packages for Python (and Java if needed), for Windows see
[Installing/Windows_CMake](Installing/Windows_CMake.md). You
need to (re-)compile sumo yourself under Windows following the remarks
above, under Linux it is probably just a matter of calling cmake and
make. For the python bindings you will get a libsumo.py and a
_libsumo.so (or .pyd on Windows). If you place them somewhere on your
python path you should be able to use them like that:

# Using libsumo

## Python

```
import libsumo
libsumo.start(["-c", "test.sumocfg"])
libsumo.simulationStep()
```

Existing traci scripts can mostly be reused by calling

```
import libsumo as traci
```

In this case, it is not possible to use the *traci.connect* and
*traci.init* API functions. You must always use *traci.start*.