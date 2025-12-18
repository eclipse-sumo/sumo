#!/bin/bash
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# Bash script for the test run.
# Sets environment variables respecting SUMO_HOME and starts texttest.
# The script understands the option --gui to run sumo-gui instead of sumo
# and --debug to use the debug versions of the executables.
# If they are present, these two options need to come first,
# all remaining options are passed directly to texttest.

# to have reproducible (english) error messages and warnings
export LC_ALL=C
export LANG=C

if [[ "$1" == "--gui" ]]; then
  sumo_suffix="-gui"
  shift
fi
if [[ "$1" == "--debug" ]]; then
  suffix="D"
  sumo_suffix="${sumo_suffix}D"
  shift
fi
if [[ $(uname) == MINGW* || $(uname) == CYGWIN* ]]; then
  suffix="${suffix}.exe"
  sumo_suffix="${sumo_suffix}.exe"
fi

pushd $(dirname $0) > /dev/null
export TEXTTEST_HOME="$PWD"
shopt -s nullglob  # expand the pattern to an empty list if no env exists
for i in *env/bin/activate; do
  if [[ "$VIRTUAL_ENV" == "" ]]; then
    echo "Activating virtual environment $(dirname $(dirname $i))."
    source $i
  else
    echo "Virtual environment $VIRTUAL_ENV already active, ignoring $(dirname $(dirname $i))."
  fi
done
if [[ "$SUMO_HOME" == "" ]]; then
  cd ..
  export SUMO_HOME="$PWD"
fi
popd > /dev/null

# for clang sanitizer tests
export LSAN_OPTIONS=suppressions="$SUMO_HOME/build_config/clang_memleak_suppressions.txt,print_suppressions=0"
export UBSAN_OPTIONS=suppressions="$SUMO_HOME/build_config/clang_ubsan_suppressions.txt"

export ACTIVITYGEN_BINARY="$SUMO_HOME/bin/activitygen$suffix"
export DFROUTER_BINARY="$SUMO_HOME/bin/dfrouter$suffix"
export DUAROUTER_BINARY="$SUMO_HOME/bin/duarouter$suffix"
export JTRROUTER_BINARY="$SUMO_HOME/bin/jtrrouter$suffix"
export NETCONVERT_BINARY="$SUMO_HOME/bin/netconvert$suffix"
export NETEDIT_BINARY="$SUMO_HOME/bin/netedit$suffix"
export NETGENERATE_BINARY="$SUMO_HOME/bin/netgenerate$suffix"
export OD2TRIPS_BINARY="$SUMO_HOME/bin/od2trips$suffix"
export POLYCONVERT_BINARY="$SUMO_HOME/bin/polyconvert$suffix"
export SUMO_BINARY="$SUMO_HOME/bin/sumo$sumo_suffix"
export GUISIM_BINARY="$SUMO_HOME/bin/sumo-gui$suffix"
export MAROUTER_BINARY="$SUMO_HOME/bin/marouter$suffix"
export PYTHON="python"

texttest "$@"
