---
title: Maven
---

The Eclipse SUMO project provides the jar files for `libsumo` and `libtraci` in a public Eclipse Nexus instance.
The Nexus instance is available [here](https://repo.eclipse.org). They can be used in `maven` builds.
More information on the repository hosted by the Eclipse Foundation can be found [here](https://wiki.eclipse.org/Services/Nexus).

## Pulling Artifacts

You can decide, if you want to use *release* or *snapshot* builds of `libsumo` or `libtraci`.

 - Release builds: https://repo.eclipse.org/content/repositories/sumo-releases/
 - Snapshot builds: https://repo.eclipse.org/content/repositories/sumo-snapshots/

In order to use those repositories in your `pom.xml` file, you just have to add one of the repositories to your configuration inside the `<project>` element.

```xml
<repositories>
    <repository>
        <id>eclipse-sumo</id>
        <name>Public Eclipse SUMO Nexus</name>
        <url>https://repo.eclipse.org/content/repositories/sumo-releases/</url>
    </repository>
</repositories>
```
or
```xml
<repositories>
    <repository>
        <id>eclipse-sumo</id>
        <name>Public Eclipse SUMO Nexus</name>
        <url>https://repo.eclipse.org/content/repositories/sumo-snapshots/</url>
    </repository>
</repositories>
```
You are then able to reference `libsumo` or `libtraci` as a dependency in our `pom.xml` file.
```xml
<dependencies>
   ...
   <dependency>
      <groupId>org.eclipse.sumo</groupId>
      <artifactId>libsumo</artifactId>
      <version>1.10.0-SNAPSHOT</version>
   </dependency>
   ...
   <dependency>
      <groupId>org.eclipse.sumo</groupId>
      <artifactId>libtraci</artifactId>
      <version>1.10.0-SNAPSHOT</version>
   </dependency>
   ...
</dependencies>
```

## Suppressing the Console Window on Windows

On Windows, `org.eclipse.sumo.libtraci.Simulation.start(...)` delegates process creation to C++ `_popen`, which opens a `cmd.exe` console window even when the host Java application runs under `javaw`. Use either of two approaches to run headless simulations without a console window:

1. **Run in-process with `libsumo`**: Switch to `org.eclipse.sumo.libsumo.Simulation` under `javaw`, which executes SUMO inside the JVM process without spawning a child process.
2. **Spawn `sumo` via Java `ProcessBuilder` and attach `libtraci`**: Launch `sumo --remote-port <PORT>` from Java (`new ProcessBuilder("sumo", "-c", "run.sumocfg", "--remote-port", "8813").redirectErrorStream(true).start()`) under `javaw` and connect with `org.eclipse.sumo.libtraci.Simulation.init(8813)`.
