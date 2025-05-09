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

# @file    get_latest_pkg.sh
# @author  Robert Hilbrich
# @author  Michael Behrisch
# @date    2025-05-08

OUT_DIR=${1:-.}
BASE_URL="https://download.eclipse.org"
LATEST=$(curl -s $BASE_URL/sumo/daily/ | \
  grep -oE "href='(/sumo/daily/sumo[^']+\.pkg)'" | \
  sed "s/href='//;s/'//" | \
  sort -V | tail -n1)
curl --no-progress-meter --remote-time -o $OUT_DIR/$(basename $LATEST) $BASE_URL$LATEST
