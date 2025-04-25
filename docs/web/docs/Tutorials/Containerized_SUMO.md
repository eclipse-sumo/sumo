---
title: Containerized SUMO
---

This tutorial shows how to use SUMO to run simulations inside a docker container.

## Introduction

This tutorial demonstrates how to use SUMO for running simulations without installing the software on your local machine.

It is also possible to run the GUI applications from the docker container.
The way to do this is highly dependent on your individual setup.
We try [in a dedicated tutorial](Containerized_SUMO_GUI.md) to give advice on how to do this.

## Getting the docker image

You can either get the latest docker image from the SUMO docker registry by using

```shell
  docker pull ghcr.io/eclipse-sumo/sumo:latest
```

or build your own local version of the image by checking out the [SUMO repository](https://github.com/eclipse-sumo/sumo) and executing:

```shell
  cd build_config/docker
  docker build -t ghcr.io/eclipse-sumo/sumo:latest -f Dockerfile.ubuntu.git .
```

## Creating simulation files

For this tutorial we use the simulation files from the [Hello SUMO Tutorial](Hello_SUMO.md).
Check out that tutorial to learn about the purpose of the files.

Create a directory called `hello_sumo_docker` and in it create 3 files:

#### `hello.net.xml`

```xml
<?xml version="1.0" encoding="UTF-8"?>
<net version="1.20" junctionCornerDetail="5" limitTurnSpeed="5.50" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
     xsi:noNamespaceSchemaLocation="http://sumo.dlr.de/xsd/net_file.xsd">
    <location netOffset="250.00,0.00" convBoundary="0.00,0.00,501.00,0.00" origBoundary="-250.00,0.00,251.00,0.00"
              projParameter="!"/>
    <edge id=":2_0" function="internal">
        <lane id=":2_0_0" index="0" speed="13.89" length="0.10" shape="500.00,-1.60 500.00,-1.60"/>
    </edge>
    <edge id="1to2" from="1" to="2" priority="-1">
        <lane id="1to2_0" index="0" speed="13.89" length="500.00" shape="0.00,-1.60 500.00,-1.60"/>
    </edge>
    <edge id="out" from="2" to="3" priority="-1">
        <lane id="out_0" index="0" speed="13.89" length="1.00" shape="500.00,-1.60 501.00,-1.60"/>
    </edge>

    <junction id="1" type="dead_end" x="0.00" y="0.00" incLanes="" intLanes="" shape="0.00,0.00 0.00,-3.20"/>
    <junction id="2" type="priority" x="500.00" y="0.00" incLanes="1to2_0" intLanes=":2_0_0"
              shape="500.00,0.00 500.00,-3.20 500.00,0.00">
        <request index="0" response="0" foes="0" cont="0"/>
    </junction>
    <junction id="3" type="dead_end" x="501.00" y="0.00" incLanes="out_0" intLanes="" shape="501.00,-3.20 501.00,0.00"/>

    <connection from="1to2" to="out" fromLane="0" toLane="0" via=":2_0_0" dir="s" state="M"/>
    <connection from=":2_0" to="out" fromLane="0" toLane="0" dir="s" state="M"/>
</net>
```

#### `hello.rou.xml`

```xml
<routes>
    <vType accel="1.0" decel="5.0" id="Car" length="2.0" maxSpeed="100.0" sigma="0.0"/>
    <route id="route0" edges="1to2 out"/>
    <vehicle depart="1" id="veh0" route="route0" type="Car"/>
</routes>
```

#### `hello.sumocfg`

```xml

<configuration>
    <input>
        <net-file value="hello.net.xml"/>
        <route-files value="hello.rou.xml"/>
    </input>
    <time>
        <begin value="0"/>
        <end value="10000"/>
    </time>
</configuration>
```

## Running the simulation

To run the simulation start a new container from the image and mount the directory in there with the `-v`-Option.
Add `--full-output result.xml` to the SUMO command to get the result of the simulation.
The command to start the container might look like this:

```shell
  # Replace this with the location where your simulation files are located.
  export SIMULATION_FILES_DIR=$PWD

  docker run \
      --rm \
      -v $SIMULATION_FILES_DIR:$SIMULATION_FILES_DIR \
      ghcr.io/eclipse-sumo/sumo:latest \
      sumo --configuration-file $SIMULATION_FILES_DIR/hello.sumocfg --full-output $SIMULATION_FILES_DIR/result.xml
```

This will run the simulation specified in `hello.sumocfg`.
After that, a file named `result.xml` will have been created, which contains the output of the simulation.

## Further Reading

You can try more of the [Tutorials](index.md) with the docker image.
