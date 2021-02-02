#!/bin/bash
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2021 German Aerospace Center (DLR) and others.
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
# @date    2019
yum-config-manager --add-repo=https://download.opensuse.org/repositories/home:/behrisch/CentOS_7/
yum install -y --nogpgcheck cmake3 ccache xerces-c-devel proj-devel fox16-devel bzip2-devel gl2ps-devel swig3
/opt/python/cp38-cp38/bin/pip install scikit-build

mkdir -p $HOME/.ccache
echo "hash_dir = false" >> $HOME/.ccache/ccache.conf
echo "base_dir = /github/workspace/_skbuild/linux-x86_64-3.8" >> $HOME/.ccache/ccache.conf
/opt/python/cp38-cp38/bin/python tools/build/setup-sumo.py --cmake-executable cmake3 -j 8 bdist_wheel
mv dist/eclipse_sumo-* `echo dist/eclipse_sumo-* | sed 's/cp38-cp38/py2.py3-none/'`
auditwheel repair dist/eclipse_sumo*.whl
for py in /opt/python/*; do
    rm dist/*.whl
    $py/bin/pip install scikit-build
    pminor=`echo $py | sed 's,/opt/python/cp3,,;s/-.*//'`
    echo "base_dir = /github/workspace/_skbuild/linux-x86_64-3.${pminor}" >> $HOME/.ccache/ccache.conf
    $py/bin/python tools/build/setup-sumo.py --cmake-executable cmake3 -j 8 bdist_wheel
    $py/bin/python tools/build/setup-libsumo.py bdist_wheel
    $py/bin/python tools/build/setup-libtraci.py bdist_wheel
    auditwheel repair dist/libsumo*.whl
    auditwheel repair dist/libtraci*.whl
done
