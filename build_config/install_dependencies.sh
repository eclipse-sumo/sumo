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
JUPEDSIM_VERSION=1.3.1

# Check for macOS
if [[ "$(uname)" == "Darwin" ]]; then
    ID="macOS"
else
    source /etc/os-release
fi

SCRIPT_DIR=$(dirname $0)
case "$ID" in
    macOS)
        brew update && brew bundle --file=$SCRIPT_DIR/Brewfile --no-upgrade
        SUDO=sudo
        ;;
    ubuntu|debian)
        export DEBIAN_FRONTEND=noninteractive
        apt-get -qq update
        apt-get -y install curl $(cat $SCRIPT_DIR/build_req_deb.txt)
        # Adding parquet support libraries
        curl -LO https://packages.apache.org/artifactory/arrow/$(lsb_release --id --short | tr 'A-Z' 'a-z')/apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb
        apt-get -y install ./apache-arrow-apt-source-latest-*.deb
        rm ./apache-arrow-apt-source-latest-*.deb
        apt-get -qq update
        PARQUET_VERSION=$(apt-cache policy libparquet-dev | grep -o '23[0-9.-]*' | head -1)
        sudo apt install -y -V libparquet-dev=$PARQUET_VERSION
        ;;
    centos)
        if [[ "$VERSION_ID" == "7" ]]; then
            # this is only tested with quay.io/pypa/manylinux2014_x86_64 and will probably not work with vanilla CentOS
            # GDAL cannot be added because the build fails with dependency problems with sqlite3
            yum install -y epel-release
            yum-config-manager --add-repo=https://download.opensuse.org/repositories/science:/dlr/CentOS_7/
            yum install -y --nogpgcheck ccache libxerces-c-devel proj-devel fox16-devel bzip2-devel gl2ps-devel swig3 eigen3-devel geos-devel
            yum install -y https://packages.apache.org/artifactory/arrow/centos/7/apache-arrow-release-latest.rpm
            yum install -y arrow-devel parquet-devel # For Apache Parquet
        else
            echo "CentOS version other than 7 detected: $VERSION_ID"
        fi
        ;;
    almalinux)
        # this is only tested with quay.io/pypa/manylinux_2_28_x86_64 and will probably not work with vanilla almalinux
        dnf install -y epel-release
        dnf -y update
        dnf install -y ccache xerces-c-devel proj-devel bzip2-devel gl2ps-devel swig gdal-devel eigen3-devel geos-devel
        # fox dependencies
        dnf install -y libX11-devel libXft-devel libXcursor-devel libXrandr-devel libXinerama-devel mesa-libGL-devel mesa-libGLU-devel freetype-devel fontconfig-devel libjpeg-turbo-devel libpng-devel
        # installing arrow / parquet
        dnf install -y https://packages.apache.org/artifactory/arrow/almalinux/$(echo $VERSION_ID | cut -f1 -d.)/apache-arrow-release-latest.rpm
        PARQUET_VERSION=$(dnf --showduplicates list parquet-devel | grep -o '23[0-9.-]*el[0-9]*' | tail -1)
        dnf install -y arrow-devel-$PARQUET_VERSION parquet-devel-$PARQUET_VERSION
        cd /opt
        # building fox from source
        curl -LO http://www.fox-toolkit.org/ftp/fox-$FOX_VERSION.tar.gz
        tar xf fox-$FOX_VERSION.tar.gz
        cd fox-$FOX_VERSION
        ./configure --disable-static --enable-shared
        make -j$(nproc)
        make install
        cd ..
        rm -rf fox-$FOX_VERSION.tar.gz fox-$FOX_VERSION
        ;;
    *)
        echo "Unknown or unsupported OS: $ID"
        ;;
esac

# building jupedsim from source
curl -LO https://github.com/PedestrianDynamics/jupedsim/archive/refs/tags/v$JUPEDSIM_VERSION.tar.gz
tar xf v$JUPEDSIM_VERSION.tar.gz
cmake -B jupedsim-build -DCMAKE_BUILD_TYPE=Release jupedsim-$JUPEDSIM_VERSION
cmake --build jupedsim-build -j2
$SUDO cmake --install jupedsim-build
rm -rf v$JUPEDSIM_VERSION.tar.gz jupedsim-$JUPEDSIM_VERSION jupedsim-build

if [[ "$ID" != "macOS" ]]; then
    # see https://github.com/pypa/manylinux/issues/1421
    pipx install -f patchelf==0.16.1.0
fi
