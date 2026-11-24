# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# Select one SUMO catalog in the same order as the requested WiX cultures.
if (CPACK_GENERATOR STREQUAL "WIX")
    string(TOLOWER "${CPACK_WIX_CULTURES}" sumo_wix_cultures)
    string(REPLACE "," ";" sumo_wix_cultures "${sumo_wix_cultures}")
    foreach(culture IN LISTS sumo_wix_cultures)
        if (culture STREQUAL "de-de")
            set(sumo_wix_catalog "de-DE")
            break()
        elseif (culture STREQUAL "en-us")
            set(sumo_wix_catalog "en-US")
            break()
        endif ()
    endforeach()
    if (NOT sumo_wix_catalog)
        # Keep the requested stock UI and fall back to English SUMO strings.
        set(sumo_wix_catalog "en-US")
        if (CPACK_WIX_CULTURES)
            string(APPEND CPACK_WIX_CULTURES ",en-US")
        else ()
            set(CPACK_WIX_CULTURES "en-US")
        endif ()
    endif ()
    list(APPEND CPACK_WIX_LIGHT_EXTRA_FLAGS -loc "${CMAKE_CURRENT_LIST_DIR}/../wix/${sumo_wix_catalog}.wxl")
endif ()
