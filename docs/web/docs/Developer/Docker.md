---
title: Docker
---

## Dockerized SUMO

Building and installing SUMO from source is not an easy task for beginner users.
Docker is a popular tool to solve this issue.
SUMO provides [Dockerfiles](https://github.com/eclipse-sumo/sumo/blob/main/build_config/docker) and [prebuilt Docker images](https://github.com/eclipse-sumo/sumo/pkgs/container/sumo/versions) to use.

### Available Dockerfiles

There are several Dockerfiles available in the SUMO repository.
To build them, check out [the SUMO repository](https://github.com/eclipse-sumo/sumo) and use the following command while in the `build_config/docker` directory of the repository:

```shell
    docker build -f {NAME_OF_DOCKERFILE} .
```

!!! note
    Ensure to include the dot at the end of the command.

The following Dockerfiles containing SUMO are available:

| Dockerfile                 | description                                                                                                                                                                                                                                                                                                                                                                              |
|----------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `Dockerfile.ubuntu.git`    | An image containing a complete SUMO installation (binaries, documentation, TraCI, ...) and [JuPedSim](https://www.jupedsim.org) based on Ubuntu Jammy (22.04). <br> When building this image, the latest version of SUMO from the repository is used. <br> This image is built nightly and uploaded to the container registry. <br> It is also built and tagged for each stable release. |
| `Dockerfile.fedora`        | An image with a SUMO installation based on the current Fedora development version (rawhide). <br> When building this image, the latest version of SUMO from the repository is used.                                                                                                                                                                                                      |
| `Dockerfile.ubuntu.latest` | An image based on Ubuntu Jammy (22.04) with SUMO installed from the APT package manager. <br> This image does not build SUMO from source, so it will always use a stable version.                                                                                                                                                                                                            |

The following Dockerfiles provide build environments:

| Dockerfile                 | Description                                                                                                                                 |
|----------------------------|---------------------------------------------------------------------------------------------------------------------------------------------|
| `Dockerfile.jenkins-build` | Environment for building SUMO on Jenkins. Installs all packages required to build SUMO, but does not contain any SUMO binaries.             |
| `Dockerfile.manylinux2014` | Build environment for creating [manylinux](https://github.com/pypa/manylinux) python wheels, adapted with additional preinstalled packages. |

### Available Images in the registry

SUMO publishes images to the [GitHub container registry](https://ghcr.io/eclipse-sumo/sumo), which can be pulled and
used immediately.
The published images are built from the `Dockerfile.ubuntu.git` Dockerfile.

To use any of the uploaded images, reference `ghcr.io/eclipse-sumo/sumo:${TAG}` (e.g. `docker pull ghcr.io/eclipse-sumo/sumo:main`).

The following tags are available:

| Tag                       | Description                                                                                                    |
|---------------------------|----------------------------------------------------------------------------------------------------------------|
| `main` `nightly`          | Nightly build containing the current version of SUMO from the repository.                                      |
| `vX_Y_Z` (e.g. `v1_21_0`) | A version of the image created alongside the respective release. Contains SUMO in the matching stable version. |
| `latest`                  | References the image of the latest stable release (e.g. `v1_21_0`)                                             |

### How to use dockerized SUMO

A complete example is available in [the tutorial on containerized SUMO](../Tutorials/Containerized_SUMO.md).

A quick example for running a prepared simulation looks like this:

```shell
   docker run \
      -v /path/to/your/simulation/data:/data \
      ghcr.io/eclipse-sumo/sumo:main \
      sumo /data/simulation.sumocfg
```

A use-case for this could be preparing the simulation data on a computer with SUMO installed and then running the simulation on a server without needing to install SUMO.

The container images also contains the graphical applications.
However, configuring them for use depends on your operating system and graphical environment (e.g., X11, Wayland).
For guidance, consult Docker's documentation on GUI application forwarding.
Therefore, we cannot provide specific instructions here.
