#!/bin/bash
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    wheels.sh
# @author  Michael Behrisch
# @date    2024-06-13

# Does the nightly git pull on the Linux / macOS server and then runs the wheel build
PREFIX=$1
PLATFORM=$2
LOG=$PREFIX/wheel.log
# to make sure we do not use the brew python
PYTHON=/usr/bin/python3

cd $PREFIX/sumo
git clean -f -x -d -q . &> $LOG ||(echo "git clean failed"; tail -10 $LOG)
git pull >> $LOG 2>&1 || (echo "git pull failed"; tail -10 $LOG)
rm -rf dist dist_native $LOG
if test "$3" == "local"; then
  cp build_config/pyproject.toml .
  $PYTHON ./tools/build_config/version.py tools/build_config/setup-sumo.py ./setup.py
  $PYTHON -m build --wheel >> $LOG 2>&1
  $PYTHON ./tools/build_config/version.py tools/build_config/setup-libsumo.py tools/setup.py
  $PYTHON -m build --wheel tools -o dist >> $LOG 2>&1
  $PYTHON -c 'import os,sys; v="cp%s%s"%sys.version_info[:2]; os.rename(sys.argv[1], sys.argv[1].replace("%s-%s"%(v,v), "py2.py3-none"))' dist/eclipse_sumo-*
  pushd tools >> $LOG
  $PYTHON ./build_config/version.py ./build_config/setup-sumolib.py ./setup.py
  $PYTHON -m build --wheel . -o ../dist >> $LOG 2>&1
  $PYTHON ./build_config/version.py ./build_config/setup-traci.py ./setup.py
  $PYTHON -m build --wheel . -o ../dist >> $LOG 2>&1
  popd >> $LOG
  mv dist dist_native  # just as backup
fi
# the docker script will create _skbuild, dist and wheelhouse dir owned by root but writable for everyone
# we only need wheelhouse, the rest is for inspecting if errors occur
docker run --rm -v $PWD:/opt/sumo --workdir /opt/sumo $PLATFORM tools/build_config/build_wheels.sh $HTTPS_PROXY v1.2.1 >> $LOG 2>&1
