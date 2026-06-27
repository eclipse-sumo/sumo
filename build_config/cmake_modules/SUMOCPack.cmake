# SUMOCPack.cmake
#
# Configures CPack to build a Windows MSI installer for SUMO using the
# built-in WIX generator. This file replaces the previous custom
# `tools/build_config/wix.py` + hand-written `build_config/wix/*.wxs`
# workflow.
#
# The WIX generator is only activated for MSVC builds. On other
# platforms this module is a no-op (CPack is not invoked at all).

if (DEFINED SUMO_CPACK_INCLUDED)
    return()
endif ()
set(SUMO_CPACK_INCLUDED TRUE)

if (NOT MSVC)
    return()
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
        COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/build_config/version.py --pep440
        OUTPUT_VARIABLE _sumo_pep440
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE _sumo_pep440_res
        ERROR_QUIET)
    if (_sumo_pep440_res EQUAL 0 AND _sumo_pep440 MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)")
        set(SUMO_CPACK_VERSION "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}")
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
)
set(CPACK_COMPONENT_RUNTIME_DISPLAY_NAME       "Main program")
set(CPACK_COMPONENT_RUNTIME_DESCRIPTION        "Executables and Python tools.")
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
set(CPACK_COMPONENT_INCLUDE_FILES_DESCRIPTION  "C/C++ header files for libsumo / libtraci.")

# ---------------------------------------------------------------------------
# WIX-specific configuration
# ---------------------------------------------------------------------------
set(CPACK_GENERATOR "WIX;ZIP")

# Stable upgrade GUID (matches the value previously used by sumo.wxs so
# that the new installer upgrades existing SUMO installations cleanly).
set(CPACK_WIX_UPGRADE_GUID          "A764BC4F-2B15-11E1-9E7E-028037EC0200")
set(CPACK_WIX_PRODUCT_ICON          "${CMAKE_SOURCE_DIR}/build_config/wix/webWizard.ico")
set(CPACK_WIX_UI_BANNER             "${CMAKE_SOURCE_DIR}/build_config/wix/bannrbmp.bmp")
set(CPACK_WIX_UI_DIALOG             "${CMAKE_SOURCE_DIR}/build_config/wix/dlgbmp.bmp")
set(CPACK_WIX_PROGRAM_MENU_FOLDER   "SUMO")
set(CPACK_WIX_ROOT_FEATURE_TITLE    "SUMO ${SUMO_CPACK_VERSION}")
set(CPACK_WIX_ROOT_FEATURE_DESCRIPTION "Simulation of Urban MObility")
set(CPACK_WIX_PROPERTY_ARPCOMMENTS  "Simulation of Urban MObility")
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
    "sumo-gui"  "sumo-gui"
    "netedit"   "netedit")
set(CPACK_CREATE_DESKTOP_LINKS "sumo-gui;netedit")

include(CPack)
