
#########################################################
# Building of the libsumo-FMI2 library
set (libsumofmi2_SRCS
  fmi2Functions.c
  sumo2fmi_bridge.c
  libsumocpp2c.cpp
)

add_library(libsumofmi2 SHARED ${libsumofmi2_SRCS})
set_target_properties(libsumofmi2 PROPERTIES PREFIX "")
set_target_properties(libsumofmi2 PROPERTIES OUTPUT_NAME libsumofmi2${BINARY_SUFFIX})
set_target_properties(libsumofmi2 PROPERTIES OUTPUT_NAME_DEBUG libsumofmi2${BINARY_SUFFIX}D)
target_link_libraries(libsumofmi2 libsumocpp)

#########################################################
# Package the model description and shared libraries into an FMU

# The Library needs to go to a specific folder
# (binaries/win32 or binaries/win64 or ...)
# We need to determine the name for this folder

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BITS "64")
else()
    set(BITS "32")
endif()

if (WIN32)
    if (CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
        set(BITS "32")
    endif()
    if (CMAKE_GENERATOR_PLATFORM STREQUAL "x64")
        set(BITS "64")
    endif()
    set(PLATFORM_FOLDER_NAME "win${BITS}")
endif()

if (UNIX AND NOT APPLE)
    set(PLATFORM_FOLDER_NAME "linux${BITS}")
endif()

if (APPLE)
    set(PLATFORM_FOLDER_NAME "darwin${BITS}")
endif()

# Build the archive by default whenever FMI support is enabled.
add_custom_target(fmi ALL
    COMMAND "${Python_EXECUTABLE}" "${SUMO_TOOLS_DIR}/build_config/packageFMI.py"
        --output "$<TARGET_FILE_DIR:libsumofmi2>/sumo-fmi2$<$<CONFIG:Debug>:D>-${PLATFORM_FOLDER_NAME}.fmu"
        --platform "${PLATFORM_FOLDER_NAME}"
        --model-description "${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml"
        --fmi-library "$<TARGET_FILE:libsumofmi2>"
        --sumo-library "$<TARGET_FILE:libsumocpp>"
        "$<$<CONFIG:Debug>:${SUMO_LIBRARIES_DLL}>"
        "$<$<NOT:$<CONFIG:Debug>>:${SUMO_LIBRARIES_RELEASE_DLL}>"
    DEPENDS libsumofmi2 libsumocpp
    COMMAND_EXPAND_LISTS
    VERBATIM)
