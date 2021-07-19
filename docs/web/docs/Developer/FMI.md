---
title: FMI
---

# FMI

There is ongoing work towards building FMI 2 support for SUMO - especially libsumo.

## Goal

The initial goal is to build a prototype which is able to load a SUMO simulation via `libsumocpp`, run the simulation and provide the total amount of vehicles in the simulation as a scalar variable. 

## Architecture

The current architecture to build the functionality for a functional mockup unit (FMU) is as followed. The source code is located in the `src/fmi` folder.

```plantuml
@startuml
node "testlibsumofmi2.exe" {
   [testlibsumofmi2.c] as testlibsumofmi2
}


node "libsumofmi2.dll" {
   [libsumocpp2c.cpp] as libsumocpp2c
   [sumo2fmi_bridge.c] as sumo2fmi_bridge
   [fmi2Functions.c] as fmi2Functions
}

node "libsumocpp.dll" {
   [SUMO]
}

[fmi2Functions] --> [sumo2fmi_bridge]
[sumo2fmi_bridge] --> [libsumocpp2c]
[libsumocpp2c]  --> [SUMO]

note left of testlibsumofmi2.exe 
 test binary for libsumofmi2.dll
end note

note left of libsumofmi2.dll 
 Dynamic Library for FMI2, 
 will be distributed as ZIP file
 "sumo-fmi2.fmu"
 in binary format
end note

note left of [fmi2Functions]
 Implementation of 
 required FMI2 functions,
 C file
end note

note right of [sumo2fmi_bridge]
 Logic to build the bridge
 between the FMI2 functions
 and functions in libsumocpp;
 should be independent of FMI,
 C file
end note

note right of [libsumocpp2c]
 Wrapper for C++ functions 
 in libsumocpp to be called from 
 C functions in FMI, 
 C++ file
end note

note left of libsumocpp.dll
 Standard libsumocpp from SUMO, 
 shared library in C++
end note
@enduml
```

## FMI 2.0 Compliance Checking

Compliance with FMI 2.0 is checked with [FMU Compliance Checker](https://github.com/modelica-tools/FMUComplianceChecker). It can be installed after cloning according to the build instructions.

The validation can be triggered on macOS with

```
fmuCheck.darwin64 /PATH/TO/SUMO/FMI/sumo-fmi2.fmu 
```

On some macOS systems, the validation may fail with the following message: *Interceptors are not working*. This can be fixed with the following call:

```
DYLD_INSERT_LIBRARIES=/Library/Developer/CommandLineTools/usr/lib/clang/11.0.3/lib/darwin/libclang_rt.asan_osx_dynamic.dylib fmuCheck.darwin64 /PATH/TO/SUMO/FMI/sumo-fmi2.fmu 
```
