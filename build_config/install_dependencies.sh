#!/bin/bash

SCRIPT_DIR=$(dirname $0)
# Check for macOS
if [[ "$(uname)" == "Darwin" ]]; then
    brew update && brew bundle --file=$SCRIPT_DIR/Brewfile --no-upgrade
    exit 0
fi

if [[ ! -f /etc/os-release ]]; then
    echo "Unknown OS and /etc/os-release not found"
    exit 1
fi
source /etc/os-release

# Determine Linux version
case "$ID" in
    ubuntu|debian)
        apt-get -y install $(cat $SCRIPT_DIR/build_req_deb.txt)
        ;;
    centos)
        if [[ "$VERSION_ID" == "7" ]]; then
            yum install -y epel-release
            yum-config-manager --add-repo=https://download.opensuse.org/repositories/science:/dlr/CentOS_7/
            yum install -y --nogpgcheck ccache libxerces-c-devel proj-devel fox16-devel bzip2-devel gl2ps-devel swig3 eigen3-devel
            yum install -y https://download.postgresql.org/pub/repos/yum/reporpms/EL-7-$(uname -m)/pgdg-redhat-repo-latest.noarch.rpm
            yum install -y geos311-devel
            yum install -y https://packages.apache.org/artifactory/arrow/centos/7/apache-arrow-release-latest.rpm
            yum install -y arrow-devel parquet-devel # For Apache Parquet
        else
            echo "CentOS version other than 7 detected: $VERSION_ID"
        fi
        ;;
    almalinux)
        # this is only tested with quay.io/pypa/manylinux_2_28_x86_64 and will probably not work with vanilla almalinux
        dnf install -y epel-release
        dnf install -y ccache xerces-c-devel proj-devel bzip2-devel gl2ps-devel swig gdal-devel eigen3-devel geos-devel
        # fox dependencies
        dnf install -y libX11-devel libXft-devel libXcursor-devel libXrandr-devel libXinerama-devel mesa-libGL-devel mesa-libGLU-devel freetype-devel fontconfig-devel libjpeg-turbo-devel libpng-devel
        # installing arrow / parquet
        dnf install -y https://packages.apache.org/artifactory/arrow/almalinux/$(echo $VERSION_ID | cut -f1 -d.)/apache-arrow-release-latest.rpm
        dnf install -y arrow-devel parquet-devel
        cd /opt
        # building fox from source
        curl -LO http://www.fox-toolkit.org/ftp/fox-1.6.59.tar.gz
        tar xf fox-1.6.59.tar.gz
        cd fox-1.6.59
        ./configure --disable-static --enable-shared
        make -j$(nproc)
        make install
        cd ..
        # building jupedsim from source
        curl -LO https://github.com/PedestrianDynamics/jupedsim/archive/refs/tags/v1.3.1.tar.gz
        tar xf v1.3.1.tar.gz
        cmake -B jupedsim-build -DCMAKE_BUILD_TYPE=Release jupedsim-1.3.1
        cmake --build jupedsim-build -j$(nproc)
        cmake --install jupedsim-build
        ;;
    *)
        echo "Unknown or unsupported OS: $ID"
        ;;
esac

# see https://github.com/pypa/manylinux/issues/1421
pipx install -f patchelf==0.16.1.0
