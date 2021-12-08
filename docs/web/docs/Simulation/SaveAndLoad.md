---
title: SaveAndLoad
---

SUMO allows loading and saving simulation state since version 0.18.0.

# Saving

To save the state at specified times during the simulation add the
option **--save-state.times**

```
sumo --save-state.times TIME1,TIME2,TIME3
```

Alternatively, the option **--save-state.period** {{DT_TIME}} can be used to save states periodically.
The option **--save-state.period.keep N** can be used to remove all but the last **N** state files.

By default the state will be written to files named *<PREFIX\>_<TIME\><SUFFIX\>* where *<PREFIX\>* can be set via option **--save-state.prefix** (default *state*), *<TIME\>* is the simulation time and *<SUFFIX\>* is either one of *.xml.gz* or *.xml* as controlled by the option **--save-state.suffix** (default *.xml.gz*).

!!! note
    The file extension '.xml.gz' indicates gzip-compressed xml which can be used directly by all Sumo applications and can also be unzipped with standard file compression tools

When using the option **--save-state.times**, output files can also be specified by using the
option **--save-state.files**

```
sumo --save-state.files FILE1,FILE2,FILE3
```

to set the file names explicitly. In particular if a plain text xml-file
is desired as output (instead of the compressed format .xml.gz) the file names
should be given (with suffix .xml).

## Further options

- **--save-state.rng**: Save random number generator states
- **--save-state.transportables**: Save person and container states
- **--save-state.constraints**: Save rail signal constraints
- **--save-state.precision**: Set decimal precision when saving states

# Loading

The option **--load-state** {{DT_FILE}} is used to initialize the simulation with the given state
file. It is recommended to start the simulation with the same input
files that were used when saving the state file and set option **--begin** to the
time when the state was saved. Additional files (i.e. more vehicles) can
be added without problems.

When the option **--load-state.offset** is set, all times loaded from the simulation state will
be shifted by the given amount. This can be used to reset the simulation
second to 0 after midnight and still have the loaded vehicles function
properly.

The option **--load-state.remove-vehicles ID1,ID2,...** allows removing vehicles from a loaded state. This opens up
the possibility for re-positioning selected vehicles by inserting them
again with a *.rou.xml* file.

# Details

Vehicles that have not yet departed are not included in the simulation
state and must be loaded from input files. When loading vehicles from an
input file that are already present in the simulation state, these
vehicles are automatically ignored and loading proceeds without errors.

# Random number generators (RNG)
By default, the state of the [random number generators](Randomness.md) is not saved. Thus,
simulations will behave differently from the original after being reloaded from a state.

When setting option **--save-state.rng**, the state of all random number generators will be included in the state and restored upon loading. Saving this extra state costs about 500Kb. If this overhead is too high it can be reduced by setting a lower value of **--thread-rngs** (default 64). The value should be no lower than the number of threads used for routing or simulation (**--threads, **--device.rerouting.threads**).

# Know Issues

- flows cannot be loaded from a state file alone (the original route file must be loaded as well)
- The internal state of the laneChangeModel is not saved
- the internal state of the carFollowModel is not saved (not all models have this).

# Older Versions of SUMO

On versions before 0.18.0 a possible workaround is to run SUMO within a
virtual machine (VM) as the state of a VM can be saved at any time.
Tests with VirtualBox on Linux have shown that SUMO does not run
significantly slower in a VM as directly on the host if enough hardware
resources are assigned to the VM (for example if you run another program
simultaneously to communicate to SUMO via TraCI you should assign enough
CPU-cores to the VM).
