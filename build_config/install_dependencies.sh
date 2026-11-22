#!/bin/bash
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2025-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    install_dependencies.sh
# @author  Michael Behrisch
# @date    2025-12-15

# This script installs all standard SUMO dependencies (without OpenSceneGraph and ffmpeg)
# on macOS and some standard Linux distros. It tries not to ask any questions and messes
# directly with your system so it is most useful in an isolated environment (container / CI).

FOX_VERSION=1.6.59
JUPEDSIM_VERSION=1.4.2

# Check for macOS
if [[ "$(uname)" == "Darwin" ]]; then
    ID="macOS"
else
    source /etc/os-release
fi

# network access to package mirrors is flaky, so retry a few times and stop if it keeps failing
retry() {
    for attempt in 1 2 3; do
        "$@" && return 0
        echo "Command failed (attempt $attempt): $*" >&2
        sleep $((attempt * 20))
    done
    echo "Giving up on: $*" >&2
    exit 1
}

SCRIPT_DIR=$(dirname $0)
case "$ID" in
    macOS)
        # with SKIP_BREW_UPDATE homebrew is only updated if the installation fails without it
        if [[ -z "$SKIP_BREW_UPDATE" ]] || ! brew bundle --file=$SCRIPT_DIR/Brewfile --no-upgrade; then
            retry brew update
            retry brew bundle --file=$SCRIPT_DIR/Brewfile --no-upgrade
        fi
        SUDO=sudo
        ;;
    ubuntu|debian)
        export DEBIAN_FRONTEND=noninteractive
        retry apt-get -qq update
        retry apt-get -y install curl $(cat $SCRIPT_DIR/build_req_deb.txt)
        apt-get install -y libboost-process-dev || true
        # Adding parquet support libraries
        retry curl -fLOs https://packages.apache.org/artifactory/arrow/$(lsb_release --id --short | tr 'A-Z' 'a-z')/apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb
        retry apt-get -y install ./apache-arrow-apt-source-latest-*.deb
        rm ./apache-arrow-apt-source-latest-*.deb
        retry apt-get -qq update
        PARQUET_VERSION=$(apt-cache policy libparquet-dev | grep -o '23[0-9.-]*' | head -1)
        retry apt-get -y install libarrow-dev=$PARQUET_VERSION libparquet-dev=$PARQUET_VERSION
        ;;
    centos)
        if [[ "$VERSION_ID" == "7" ]]; then
            # this is only tested with quay.io/pypa/manylinux2014_x86_64 and will probably not work with vanilla CentOS
            # GDAL cannot be added because the build fails with dependency problems with sqlite3
            retry yum install -y epel-release
            retry yum-config-manager --add-repo=https://download.opensuse.org/repositories/science:/dlr/CentOS_7/
            retry yum install -y --nogpgcheck ccache libxerces-c-devel proj-devel fox16-devel bzip2-devel gl2ps-devel swig3 eigen3-devel geos-devel
            retry yum install -y https://packages.apache.org/artifactory/arrow/centos/7/apache-arrow-release-latest.rpm
            retry yum install -y arrow-devel parquet-devel # For Apache Parquet
        else
            echo "CentOS version other than 7 detected: $VERSION_ID"
        fi
        ;;
    almalinux)
        # this is only tested with quay.io/pypa/manylinux_2_28_x86_64 and will probably not work with vanilla almalinux
        retry dnf install -y epel-release
        retry dnf -y update
        retry dnf install -y ccache xerces-c-devel proj-devel bzip2-devel gl2ps-devel swig gdal-devel eigen3-devel geos-devel
        # fox dependencies
        retry dnf install -y libX11-devel libXft-devel libXcursor-devel libXrandr-devel libXinerama-devel mesa-libGL-devel mesa-libGLU-devel freetype-devel fontconfig-devel libjpeg-turbo-devel libpng-devel
        # installing arrow / parquet
        retry dnf install -y https://packages.apache.org/artifactory/arrow/almalinux/$(echo $VERSION_ID | cut -f1 -d.)/apache-arrow-release-latest.rpm
        PARQUET_VERSION=$(dnf --showduplicates list parquet-devel | grep -o '23[0-9.-]*el[0-9]*' | tail -1)
        retry dnf install -y arrow-devel-$PARQUET_VERSION parquet-devel-$PARQUET_VERSION
        cd /opt
        # building fox from source
        if curl -LOs http://fox-toolkit.org/ftp/fox-$FOX_VERSION.tar.gz; then
            tar xf fox-$FOX_VERSION.tar.gz
            rm -f fox-$FOX_VERSION.tar.gz
        else
            FOX_VERSION=ed07da20d24279afc18951d2b0fbe6dc920a2660
            curl -LOs --proto =https https://github.com/franko/fox/archive/$FOX_VERSION.zip
            unzip $FOX_VERSION.zip
            rm -f $FOX_VERSION.zip
        fi
        cd fox-$FOX_VERSION
        ./configure --disable-static --enable-shared
        make -j$(nproc)
        make install
        cd ..
        rm -rf fox-$FOX_VERSION
        ;;
    *)
        echo "Unknown or unsupported OS: $ID"
        ;;
esac

# building jupedsim from source (can be skipped by setting SKIP_JUPEDSIM, e.g. if it is already installed)
if [[ -z "$SKIP_JUPEDSIM" ]]; then
    retry curl -fLOs --proto =https https://github.com/PedestrianDynamics/jupedsim/archive/refs/tags/v$JUPEDSIM_VERSION.tar.gz
    tar xf v$JUPEDSIM_VERSION.tar.gz
    cmake -B jupedsim-build -DCMAKE_BUILD_TYPE=Release jupedsim-$JUPEDSIM_VERSION
    cmake --build jupedsim-build -j2
    $SUDO cmake --install jupedsim-build
    rm -rf v$JUPEDSIM_VERSION.tar.gz jupedsim-$JUPEDSIM_VERSION jupedsim-build
fi

if [[ "$ID" != "macOS" ]]; then
    # see https://github.com/pypa/manylinux/issues/1421
    pipx uninstall patchelf
    pipx install -f patchelf==0.16.1.0
fi
