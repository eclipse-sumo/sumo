#!/bin/bash
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    cibuild_wheels.sh
# @author  Michael Behrisch
# @date    2026-01-06

# This script builds the python wheels using cibuildwheel.
# It understands one parameter which is the docker image to use.
# If it is provided, the docker image needs to contain all dependencies and the build should work
# without access to the packages of the distribution (but still needs pypi).

platform="auto"
if [[ $# -ge 1 && "$1" != "local" ]] ; then
    export CIBW_MANYLINUX_X86_64_IMAGE=$1
    if [[ "$(uname)" == "Linux" ]] ; then
        # on Linux we run isolated
        export CIBW_BEFORE_ALL=""
        export CIBW_ENVIRONMENT_PASS_LINUX="HTTP_PROXY HTTPS_PROXY"
    else
        platform="linux"
    fi
fi
cd $(dirname $0)/../..
tools/build_config/version.py --pep440 build_config/pyproject/sumolib.toml pyproject.toml
python -m build -o wheelhouse
tools/build_config/version.py --pep440 build_config/pyproject/traci.toml pyproject.toml
python -m build -o wheelhouse
tools/build_config/version.py --pep440 build_config/pyproject/eclipse-sumo.toml pyproject.toml
pipx run cibuildwheel --platform $platform
tools/build_config/version.py --pep440 build_config/pyproject/sumo-data.toml pyproject.toml
pipx run cibuildwheel --platform $platform
tools/build_config/version.py --pep440 build_config/pyproject/libsumo.toml pyproject.toml
pipx run cibuildwheel --platform $platform
