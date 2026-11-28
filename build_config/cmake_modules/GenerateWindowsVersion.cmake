# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at the following URL:
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

file(STRINGS "${VERSION_HEADER}" version_line LIMIT_COUNT 1 REGEX "^#define VERSION_STRING ")
set(numeric_version "0,0,0,0")
if (version_line MATCHES [=[^#define VERSION_STRING "v?([0-9]+)[._]([0-9]+)[._]([0-9]+)]=])
    string(REGEX REPLACE "(^|,)0+([0-9])" "\\1\\2" numeric_version
        "${CMAKE_MATCH_1},${CMAKE_MATCH_2},${CMAKE_MATCH_3},0")
endif ()
string(REPLACE "," "." version_string "${numeric_version}")
file(WRITE "${WINDOWS_VERSION_HEADER}"
    "#define SUMO_WINDOWS_VERSION ${numeric_version}\n"
    "#define SUMO_WINDOWS_VERSION_STRING \"${version_string}\"\n")
