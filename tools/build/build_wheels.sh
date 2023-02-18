#!/bin/bash
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2022 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    build_wheels.sh
# @author  Michael Behrisch
# @author  Robert Hilbrich
# @date    2019

# If we are behind a firewall we cannot install from the CentOS repos but we can use pip via the proxy
if test $# -ge 1; then
  export HTTPS_PROXY=$1
fi
if test -z $HTTPS_PROXY; then
    yum install -y epel-release
    yum-config-manager --add-repo=https://download.opensuse.org/repositories/science:/dlr/CentOS_7/
    yum install -y --nogpgcheck ccache libxerces-c-devel proj-devel fox16-devel bzip2-devel gl2ps-devel swig3
    pipx install -f patchelf==0.16.1.0  # see https://github.com/pypa/manylinux/issues/1421
fi

mkdir -p $HOME/.ccache
echo "hash_dir = false" >> $HOME/.ccache/ccache.conf
echo "base_dir = /github/workspace/_skbuild/linux-x86_64-3.8" >> $HOME/.ccache/ccache.conf
cp build/pyproject.toml .
py=/opt/python/cp38-cp38
$py/bin/python tools/build/version.py tools/build/setup-sumo.py ./setup.py
$py/bin/python -m build --wheel
mv dist/eclipse_sumo-* `echo dist/eclipse_sumo-* | sed 's/cp38-cp38/py2.py3-none/'`
auditwheel repair dist/eclipse_sumo*.whl
cp -a data tools/libsumo
for py in /opt/python/cp3[1789]*; do
    rm dist/*.whl
    pminor=`echo $py | sed 's,/opt/python/cp3,,;s/-.*//'`
    echo "base_dir = /github/workspace/_skbuild/linux-x86_64-3.${pminor}" >> $HOME/.ccache/ccache.conf
    $py/bin/python tools/build/version.py tools/build/setup-sumo.py ./setup.py
    $py/bin/python -m build --wheel
    $py/bin/python tools/build/version.py tools/build/setup-libsumo.py tools/setup.py
    $py/bin/python -m build --wheel tools -o dist
    $py/bin/python tools/build/version.py tools/build/setup-libtraci.py tools/setup.py
    $py/bin/python -m build --wheel tools -o dist
    auditwheel repair dist/libsumo*.whl
    auditwheel repair dist/libtraci*.whl
done
