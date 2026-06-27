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

# @file    SUMOCPack.cmake
# @author  Michael Behrisch
# @date    2026-06-27
#
# Configures CPack to build a Zip and a Windows MSI installer for SUMO using the
# built-in WIX generator.

if (MSVC)
    set(CPACK_GENERATOR "WIX;ZIP")
else ()
    set(CPACK_GENERATOR "ZIP")
endif ()

# ---------------------------------------------------------------------------
# Version handling
# ---------------------------------------------------------------------------
# CPack/WIX requires a strict W.X.Y[.Z] numeric version. The repository
# default `PACKAGE_VERSION=git` is only good for human-readable paths,
# so derive a numeric version from `tools/build_config/version.py` and
# fall back to a sensible default if the helper cannot be executed.
set(SUMO_CPACK_VERSION "0.0.0")
if (Python_EXECUTABLE)
    execute_process(
        COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/build_config/version.py --pep440 -
        OUTPUT_VARIABLE _version_pep440
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE _version_pep440_res)
    if (_version_pep440_res EQUAL 0)
        if (_version_pep440 MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)$")
            set(SUMO_CPACK_VERSION "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}")
        elseif (_version_pep440 MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)\\.post([0-9]+)")
            set(SUMO_CPACK_VERSION "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}.${CMAKE_MATCH_4}")
        endif ()
    endif ()
endif ()

# ---------------------------------------------------------------------------
# Common package metadata
# ---------------------------------------------------------------------------
set(CPACK_PACKAGE_NAME              "SUMO")
set(CPACK_PACKAGE_VENDOR            "Eclipse")
set(CPACK_PACKAGE_CONTACT           "sumo-dev@eclipse.org")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Simulation of Urban MObility")
set(CPACK_PACKAGE_HOMEPAGE_URL      "https://eclipse.dev/sumo")
set(CPACK_PACKAGE_VERSION           "${SUMO_CPACK_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Eclipse/Sumo")
set(CPACK_PACKAGE_FILE_NAME         "sumo-${SUMO_CPACK_VERSION}")

# License file used by the installer. Can be overridden from the command
# line with `-DCPACK_RESOURCE_FILE_LICENSE=...` (the daily build uses this
# to swap in the GPL license for the `extra` flavour).
if (NOT CPACK_RESOURCE_FILE_LICENSE)
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/build_config/wix/License.rtf")
endif ()

# ---------------------------------------------------------------------------
# Components → features
# ---------------------------------------------------------------------------
set(CPACK_COMPONENTS_ALL
    runtime         # executables, python tools, libsumo/libtraci C++ libs
    dll             # Windows runtime DLLs
    data            # XML schemata, edge type maps
    docs            # user/python/java/tutorial/examples docs
    include_files   # C++ headers, sources zip
    tools           # Python tools
)
set(CPACK_COMPONENT_RUNTIME_DISPLAY_NAME       "Main program")
set(CPACK_COMPONENT_RUNTIME_DESCRIPTION        "SUMO binaries and libsumo / libtraci C++ dlls.")
set(CPACK_COMPONENT_RUNTIME_REQUIRED           TRUE)
set(CPACK_COMPONENT_DLL_DISPLAY_NAME           "Runtime libraries")
set(CPACK_COMPONENT_DLL_DESCRIPTION            "Required Windows DLLs.")
set(CPACK_COMPONENT_DLL_REQUIRED               TRUE)
set(CPACK_COMPONENT_DLL_HIDDEN                 TRUE)
set(CPACK_COMPONENT_DATA_DISPLAY_NAME          "Data")
set(CPACK_COMPONENT_DATA_DESCRIPTION           "Additional data like XML schemata and edge type maps.")
set(CPACK_COMPONENT_DOCS_DISPLAY_NAME          "Documentation")
set(CPACK_COMPONENT_DOCS_DESCRIPTION           "User, Python and Java documentation, tutorials and examples.")
set(CPACK_COMPONENT_INCLUDE_FILES_DISPLAY_NAME "Includes")
set(CPACK_COMPONENT_INCLUDE_FILES_DESCRIPTION  "C++ header files for libsumo / libtraci.")
set(CPACK_COMPONENT_TOOLS_DISPLAY_NAME         "Tools")
set(CPACK_COMPONENT_TOOLS_DESCRIPTION          "Additional Python tooling.")

# ---------------------------------------------------------------------------
# WIX-specific configuration
# ---------------------------------------------------------------------------
# Stable upgrade GUID (matches the value previously used by sumo.wxs so
# that the new installer upgrades existing SUMO installations cleanly).
set(CPACK_WIX_UPGRADE_GUID             "A764BC4F-2B15-11E1-9E7E-028037EC0200")
set(CPACK_WIX_PRODUCT_ICON             "${CMAKE_SOURCE_DIR}/data/logo/sumo-application-icon.ico")
set(CPACK_WIX_UI_BANNER                "${CMAKE_SOURCE_DIR}/build_config/wix/bannrbmp.bmp")
set(CPACK_WIX_UI_DIALOG                "${CMAKE_SOURCE_DIR}/build_config/wix/dlgbmp.bmp")
set(CPACK_WIX_PROGRAM_MENU_FOLDER      "SUMO")
set(CPACK_WIX_ROOT_FEATURE_TITLE       "SUMO ${SUMO_CPACK_VERSION}")
set(CPACK_WIX_ROOT_FEATURE_DESCRIPTION "Simulation of Urban MObility")
set(CPACK_WIX_PROPERTY_ARPCOMMENTS     "Simulation of Urban MObility")
set(CPACK_WIX_PROPERTY_ARPURLINFOABOUT "https://eclipse.dev/sumo")
set(CPACK_WIX_PROPERTY_ARPHELPLINK     "https://sumo.dlr.de/docs/")

# Provide extra WiX sources for things CPack cannot express natively:
#   * SUMO_HOME / PATH / PYTHONPATH environment variables
#   * .sumocfg file association handled by sumo-gui.exe
# and a patch file that wires the resulting ComponentGroup into the
# main feature.
set(CPACK_WIX_EXTRA_SOURCES "${CMAKE_SOURCE_DIR}/build_config/wix/cpack_extras.wxs")
set(CPACK_WIX_PATCH_FILE    "${CMAKE_SOURCE_DIR}/build_config/wix/cpack_patch.xml")
set(CPACK_WIX_EXTENSIONS    "WixUtilExtension")

# Desktop / start-menu shortcuts created automatically by CPack-WIX.
# Format: <executable-without-extension>;<label>
set(CPACK_PACKAGE_EXECUTABLES
    "sumo-gui"  "SUMO GUI"
    "netedit"   "SUMO netedit")

include(CPack)
