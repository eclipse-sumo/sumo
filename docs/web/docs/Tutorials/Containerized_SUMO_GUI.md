---
title: Containerized SUMO GUI
---

This tutorial shows how to run the SUMO GUI from inside a docker container.

!!! caution
    SUMO can only be run from a Container using the X Window System.   
    If you are on Linux, make sure your Desktop Environment supports X11.   
    If you are on Windows, you need a separate X-Server (e.g. [VcXsrv](https://sourceforge.net/projects/vcxsrv)).

## Introduction

This tutorial demonstrates how to use the SUMO GUI without installing the software on your local machine.

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

## Starting the container

This section describes how to start the container.

It is only necessary to start a single container.
We will then launch the required applications from the command-line.

### Linux

This section assumes, you are either running X11 as your primary window system or a wayland compositor compatible with X11 (e.g. Weston).
SUMO does currently not work with Wayland and requires some compatibility with X11 (see also [flathub/org.eclipse.sumo#15](https://github.com/flathub/org.eclipse.sumo/issues/15)).

Most distributions fulfill this requirement by default, so if you are unsure about this, try proceeding.

To allow graphical applications to run from inside the docker container, we need to do two things:

- transfer the information which display to use with `-e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix:rw`
- transfer the information for authorizing into the container with `-e XAUTHORITY=$XAUTHORITY -v $XAUTHORITY:$XAUTHORITY`

We also need a directory to store our results persistent. In this example we are using `~/sumo-projs` for this.

So in total the command for starting the container looks like this:

```shell
mkdir -p ~/sumo-projs/
docker run \
    -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix:rw \
    -e XAUTHORITY=$XAUTHORITY -v $XAUTHORITY:$XAUTHORITY \
    -v ~/sumo-projs/:/sumo-projs \
    --workdir /sumo-projs \
    -it \
    ghcr.io/eclipse-sumo/sumo:latest bash
```

You will then have a shell, where you can start the GUI applications.
E.g. by typing `netedit` or `sumo-gui`.

### Windows

Start your X-Server (e.g. VcXsrv) and check the setting for "Disable access control" (by this, the setup gets a lot easier).
To then tell the applications in the container to use this X-Server, we add `-e DISPLAY=host.docker.internal:0` to the command-line.

We also need a directory to store our results persistent. In this example we are using `C:\sumo-projs` for this.

So in total the command for starting the container looks like this:

```shell
docker run \
    -e DISPLAY=host.docker.internal:0
    -v C:\sumo-projs:/sumo-projs \
    --workdir /sumo-projs \
    -it \
    ghcr.io/eclipse-sumo/sumo:latest bash
```

You will then have a shell, where you can start the GUI applications.
E.g. by typing `netedit` or `sumo-gui`.

## Creating and Running the Simulation

Follow the steps from [Hello SUMO](Hello_SUMO.md) using this GUI and save the resulting files in `/sumo-projs`, so they are saved in a persistent location.

## Further Reading

You can try more of the [Tutorials](index.md) with the docker image.
