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

# @file    wheels.sh
# @author  Michael Behrisch
# @date    2024-06-13

# Does the nightly git pull on the Linux / macOS server and then runs the wheel build
PREFIX=$1
LOG=$PREFIX/wheel.log

cd $PREFIX/sumo
git clean -f -x -d -q . &> $LOG || (echo "git clean failed"; tail -10 $LOG)
git pull >> $LOG 2>&1 || (echo "git pull failed"; tail -10 $LOG)
shift
for platform in $*; do
    ./tools/build_config/cibuild_wheels.sh $platform >> $LOG 2>&1;
done
for f in wheelhouse/*.whl; do
    # the credentials are in ~/.netrc
    curl -n -F file=@$f https://sumo.dlr.de/daily/upload/upload.php
done
