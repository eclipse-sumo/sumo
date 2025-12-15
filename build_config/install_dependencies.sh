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
        # For CentOS, check version for 7 specifically
        if [[ "$VERSION_ID" == "7" ]]; then
            yum install -y epel-release
            yum-config-manager --add-repo=https://download.opensuse.org/repositories/science:/dlr/CentOS_7/
            yum install -y --nogpgcheck ccache libxerces-c-devel proj-devel fox16-devel bzip2-devel gl2ps-devel swig3 eigen3-devel
            yum install -y https://download.postgresql.org/pub/repos/yum/reporpms/EL-7-$(uname -m)/pgdg-redhat-repo-latest.noarch.rpm
            yum install -y geos311-devel
            yum install -y https://packages.apache.org/artifactory/arrow/centos/7/apache-arrow-release-latest.rpm
            yum install -y arrow-devel parquet-devel # For Apache Parquet
            # see https://github.com/pypa/manylinux/issues/1421
            pipx install -f patchelf==0.16.1.0
        else
            echo "CentOS version other than 7 detected: $VERSION_ID"
        fi
        ;;
    *)
        echo "Unknown or unsupported OS: $ID"
        ;;
esac
