# TraCI API Versions

TraCI clients and servers exchange a protocol version during connection setup
(`CMD_GETVERSION`). The authoritative definition is `TRACI_VERSION` in
`src/libsumo/TraCIConstants.h`. The Python (`tools/traci/constants.py`) and Java
(`tools/contributed/traas/src/main/java/de/tudresden/sumo/config/Constants.java`)
constants are generated from it and should not be edited directly. See
[Control-related commands](Control-related_commands.md#command-0x00-get-version)
for the version handshake.

The version number is incremented when the protocol changes in a way that breaks
existing clients: removed or renamed variables and commands, changed units, or
changed semantics of an existing value. Adding new variables, commands or attributes
is backward compatible and does not require a version increment. A client and a server
should use the same version; no automatic negotiation takes place.

The table below lists the versions and the SUMO release in which each version was
introduced, together with a short summary of the incompatible change. Detailed
entries are available in the changelog of the respective release.

| TraCI version | SUMO release | Incompatible change |
| --- | --- | --- |
| 11 | [0.26.0](../ChangeLog/Changes_in_2016_releases.md#version-0260) | see release notes |
| 12 | [0.27.0](../ChangeLog/Changes_in_2016_releases.md#version-0270) | vehicle command changes |
| 13 | [0.28.0](../ChangeLog/Changes_in_2016_releases.md#version-0280) | see release notes |
| 14 | [0.29.0](../ChangeLog/Changes_in_2017_releases.md#version-0290) | binary format SBX version 2 (#2651) |
| 15 | [0.30.0](../ChangeLog/Changes_in_2017_releases.md#version-0300) | constants renamed (`CMD_SIMSTEP2` to `CMD_SIMSTEP`, `*AREAL_DETECTOR*` to `*LANEAREA*`, `*MULTI_ENTRY_EXIT_DETECTOR*` to `*MULTIENTRYEXIT*`) |
| 16 | [0.31.0](../ChangeLog/Changes_in_2017_releases.md#version-0310) | see release notes |
| 17 | [0.32.0](../ChangeLog/Changes_in_2017_releases.md#version-0320) | see release notes |
| 18 | [1.0.0](../ChangeLog/Changes_in_2018_releases.md#version-100) | times are consistently represented in seconds as double |
| 19 | [1.1.0](../ChangeLog/Changes_in_2018_releases.md#version-110) | see release notes |
| 20 | [1.2.0](../ChangeLog/Changes_in_2019_releases.md#version-120) | `trafficlight.setCompleteRedYellowGreeenDefinition` changed |
| 21 | [1.19.0](../ChangeLog/Changes_in_2023_releases.md#version-1190) | unit changes (#7277) |
| 22 | [1.23.0](../ChangeLog/Changes_in_2025_releases.md#version-1230) | Python client returns tuples instead of lists; removed `TL_EXTERNAL_STATE`; deprecated `simulation.getBusStop*` |
