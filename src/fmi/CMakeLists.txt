
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
# Building of the ZIP Archive with the model description
# and the static library for the FMU

# The Library needs to go to a specific folder
# (binaries/win32 or binaries/win64 or ...)
# We need to determine the name for this folder

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BITS "64")
else()
    set(BITS "32")
endif()

if(WIN32)
  if (CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
    set(BITS "32")
  endif()
  if (CMAKE_GENERATOR_PLATFORM STREQUAL "x64")
    set(BITS "64")
  endif()
  set(PLATFORM_FOLDER_NAME "win${BITS}")
endif()

if(UNIX AND NOT APPLE)
  set(PLATFORM_FOLDER_NAME "linux${BITS}")
endif()

if(APPLE)
  set(PLATFORM_FOLDER_NAME "darwin${BITS}")
endif()

# Set the temporary folder name to collect the contents for the ZIP archive
set(ZIP_BUILD_FOLDER_NAME "sumo-fmi2")

# The command and parameters for rm / remove_directory change in cmake
if (${CMAKE_VERSION} VERSION_GREATER 3.17.0)
  set(remove_command "rm" "-rf")
else()
  set(remove_command "remove_directory")
endif()

###########################################################
# Define the custom target FMI to have all of these files
# built (also as part of ALL - but of course this happens
# only when FMI is enabled)
add_custom_target(prepfmi
  # Remove the old directory (if exists) and create a new directory
  COMMAND ${CMAKE_COMMAND} -E ${remove_command} "${ZIP_BUILD_FOLDER_NAME}"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${ZIP_BUILD_FOLDER_NAME}"
  # Copy the XML file with the model description
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/modelDescription.xml" "${ZIP_BUILD_FOLDER_NAME}"
  # Create the folder for the binaries and copy the library
  COMMAND ${CMAKE_COMMAND} -E make_directory "${ZIP_BUILD_FOLDER_NAME}/binaries/${PLATFORM_FOLDER_NAME}/"
  COMMAND ${CMAKE_COMMAND} -E copy ${SUMO_LIBRARIES_RELEASE_DLL} $<TARGET_FILE:libsumofmi2> $<TARGET_FILE:libsumocpp> "${ZIP_BUILD_FOLDER_NAME}/binaries/${PLATFORM_FOLDER_NAME}/"
)
add_dependencies(prepfmi libsumofmi2)

# native zip seems to work better
FIND_PROGRAM(ZIP_EXECUTABLE zip)
if (ZIP_EXECUTABLE)
  add_custom_target(fmi ALL WORKING_DIRECTORY "${ZIP_BUILD_FOLDER_NAME}" COMMAND "${ZIP_EXECUTABLE}" -r "${CMAKE_SOURCE_DIR}/bin/${ZIP_BUILD_FOLDER_NAME}$<$<CONFIG:Debug>:D>-${PLATFORM_FOLDER_NAME}.fmu" .)
else()
  add_custom_target(fmi ALL COMMAND ${CMAKE_COMMAND} -E chdir "${ZIP_BUILD_FOLDER_NAME}" ${CMAKE_COMMAND} -E tar "cf" "${CMAKE_SOURCE_DIR}/bin/${ZIP_BUILD_FOLDER_NAME}$<$<CONFIG:Debug>:D>-${PLATFORM_FOLDER_NAME}.fmu" --format=zip ".")
endif()
add_dependencies(fmi prepfmi)
