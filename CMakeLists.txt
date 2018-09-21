#Options:
# -DCMAKE_BUILD_TYPE:STRING=Debug (default Release)

option(CHECK_OPTIONAL_LIBS "Try to download / enable all optional libraries" true)
option(MULTITHREADED_BUILD "Use all available cores for building" true)

project(SUMO)
cmake_minimum_required(VERSION 3.1)
# do not expand quoted variables in if statements
cmake_policy(SET CMP0054 NEW)

set(CMAKE_COLOR_MAKEFILE ON)

if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Release)
endif (NOT CMAKE_BUILD_TYPE)
message(STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/build/cmake_modules/")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
        "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(warnings "-Wall -pedantic -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif (MSVC)
    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
        set(warnings "/W3")
    else ()
        #  set(warnings "/W4 /WX /EHsc")
        # enabling /WX is not possible due to warnings in external headers
        # /Wall brings MSVC to complete halt
        set(warnings "/W4 /EHsc")
    endif ()
    if (MULTITHREADED_BUILD)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    endif ()
endif ()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${warnings}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${warnings}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
# force Visual Studio to leave out the Release / Debug dirs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)
# Debug messages
message(STATUS "CMAKE_BINARY_DIR: " ${CMAKE_BINARY_DIR})
message(STATUS "CMAKE_SOURCE_DIR: " ${CMAKE_SOURCE_DIR})
message(STATUS "")
message(STATUS "Platform: ")
message(STATUS "    Host: " ${CMAKE_HOST_SYSTEM_NAME} ${CMAKE_HOST_SYSTEM_VERSION} ${CMAKE_HOST_SYSTEM_PROCESSOR})
if (CMAKE_CROSSCOMPILING)
    message(STATUS "    Target: " ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION} ${CMAKE_SYSTEM_PROCESSOR})
endif ()
message(STATUS "    CMake: " ${CMAKE_VERSION})
message(STATUS "    CMake generator: " ${CMAKE_GENERATOR})
message(STATUS "    CMake build tool: " ${CMAKE_BUILD_TOOL})
if (MSVC)
    message(STATUS "    MSVC: " ${MSVC_VERSION})
endif ()
if (CMAKE_GENERATOR MATCHES Xcode)
    message(STATUS "    Xcode: " ${XCODE_VERSION})
endif ()
message(STATUS "")

set(ENABLED_FEATURES "${CMAKE_HOST_SYSTEM}")

#http://stackoverflow.com/questions/1487752/how-do-i-instruct-cmake-to-look-for-libraries-installed-by-macports
if (APPLE)
    # Detect if the "port" command is valid on this system; if so, return full path
    execute_process(COMMAND which port RESULT_VARIABLE DETECT_MACPORTS OUTPUT_VARIABLE MACPORTS_PREFIX ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (${DETECT_MACPORTS} EQUAL 0)
        # "/opt/local/bin/port" doesn't have libs, so we get the parent directory
        get_filename_component(MACPORTS_PREFIX ${MACPORTS_PREFIX} DIRECTORY)
        # "/opt/local/bin" doesn't have libs, so we get the parent directory
        get_filename_component(MACPORTS_PREFIX ${MACPORTS_PREFIX} DIRECTORY)
        # "/opt/local" is where MacPorts lives, add `/lib` suffix and link
        link_directories(${MACPORTS_PREFIX}/lib)
        message(STATUS "Macports detected: ${MACPORTS_PREFIX}/lib")
    else ()
        # Recommendation, also add a "brew --prefix" custom command to detect a homebrew build environment
        execute_process(COMMAND brew --prefix RESULT_VARIABLE DETECT_BREW OUTPUT_VARIABLE BREW_PREFIX ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
        if (${DETECT_BREW} EQUAL 0)
            link_directories(${BREW_PREFIX}/lib)
            message(STATUS "Brew detected: ${BREW_PREFIX}")
        endif ()
    endif ()
endif (APPLE)

# Check if libraries have to be found, depending on SUMO_LIBRARIES
set(SUMO_LIBRARIES "$ENV{SUMO_LIBRARIES}" CACHE PATH "Location of SUMOLibraries dependencies")
if (SUMO_LIBRARIES)
    if(${CMAKE_MODULE_LINKER_FLAGS} STREQUAL "/machine:x64")
        message(STATUS "Using 64 bit libraries from SUMO_LIBRARIES placed in " ${SUMO_LIBRARIES})
        set(LIB_VERSION "_64")
    else()
        message(STATUS "Using 32 bit libraries from SUMO_LIBRARIES placed in " ${SUMO_LIBRARIES})
        set(CHECK_OPTIONAL_LIBS false)
    endif()
    # TODO: Use file(GLOB ...) to avoid explicit versions
    # set Xerces
    if(${MSVC_VERSION} STREQUAL "1800")
        set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${SUMO_LIBRARIES}/msvc12/xerces-c-3.2.0${LIB_VERSION}")
    else()
        set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${SUMO_LIBRARIES}/xerces-c-3.2.0${LIB_VERSION}")
    endif()
    # set Proj
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${SUMO_LIBRARIES}/proj_gdal-1911${LIB_VERSION}")
    # set FOX
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${SUMO_LIBRARIES}/fox-1.6.55${LIB_VERSION}")
    # set GoogleTest
    # Prevent GoogleTest from overriding our compiler/linker options
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    add_subdirectory("${SUMO_LIBRARIES}/googletest-1.8.0${LIB_VERSION}/googletest"
                     "${CMAKE_BINARY_DIR}/googletest-build")
    include_directories("${SUMO_LIBRARIES}/googletest-1.8.0${LIB_VERSION}/googletest/include")
    set(GTEST_BOTH_LIBRARIES "gtest" "gtest_main")
    set(GTEST_FOUND true)
    # set FFMPEG
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${SUMO_LIBRARIES}/FFMPEG-3.4.1${LIB_VERSION}")
    # set OSG
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${SUMO_LIBRARIES}/OSG-3.4.0${LIB_VERSION}")
    # set GL2PS
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${SUMO_LIBRARIES}/gl2ps-1.4.0${LIB_VERSION}")
    set(CMAKE_LIBRARY_PATH "${CMAKE_LIBRARY_PATH};${SUMO_LIBRARIES}/gl2ps-1.4.0${LIB_VERSION}/Release")
    # set SWIG
    set(SWIG_EXECUTABLE "${SUMO_LIBRARIES}/swigwin-3.0.12/swig.exe")
    # set GRPC
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${SUMO_LIBRARIES}/grpc-1.10.0${LIB_VERSION}")
    set(CMAKE_LIBRARY_PATH "${CMAKE_LIBRARY_PATH};${SUMO_LIBRARIES}/grpc-1.10.0${LIB_VERSION}/Debug")
    set(CMAKE_LIBRARY_PATH "${CMAKE_LIBRARY_PATH};${SUMO_LIBRARIES}/grpc-1.10.0${LIB_VERSION}/Release")
    set(GRPC_CPP_PLUGIN "${SUMO_LIBRARIES}/grpc-1.10.0${LIB_VERSION}/Release/grpc_cpp_plugin.exe")
    # set protobuf
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${SUMO_LIBRARIES}/3rdPartyLibs/protobuf-3.5.1${LIB_VERSION}")
    set(Protobuf_PROTOC_EXECUTABLE "${SUMO_LIBRARIES}/3rdPartyLibs/protobuf-3.5.1${LIB_VERSION}/cmake/Release/protoc.exe")
    set(Protobuf_LIBRARY_DEBUG "${CMAKE_LIBRARY_PATH};${SUMO_LIBRARIES}/protobuf-3.5.1${LIB_VERSION}/cmake/Debug/libprotobufd.lib")
    set(Protobuf_LIBRARY "${CMAKE_LIBRARY_PATH};${SUMO_LIBRARIES}/protobuf-3.5.1${LIB_VERSION}/cmake/Release/libprotobuf.lib")
else (SUMO_LIBRARIES)
    # for Linux and Mac only
    find_package(GTest)
endif (SUMO_LIBRARIES)

find_package(XercesC REQUIRED)
if (XercesC_FOUND)
    include_directories(${XercesC_INCLUDE_DIRS})
endif (XercesC_FOUND)

find_package(Proj)
if (PROJ_FOUND)
    include_directories(${PROJ_INCLUDE_DIR})
    set(HAVE_PROJ 1)
    set(ENABLED_FEATURES "${ENABLED_FEATURES} Proj")
endif (PROJ_FOUND)

find_package(FOX)
if (FOX_FOUND)
    if (FOX_INCLUDE_DIR)
        include_directories(${FOX_INCLUDE_DIR})
    endif (FOX_INCLUDE_DIR)
    add_definitions(${FOX_CXX_FLAGS})
    add_definitions(-DFLOAT_MATH_FUNCTIONS)
    set(HAVE_FOX 1)
    set(ENABLED_FEATURES "${ENABLED_FEATURES} GUI")
endif (FOX_FOUND)

find_package(X11)
if (X11_FOUND)
    link_directories(${X11_LIBRARY_DIR})
    include_directories(${X11_INCLUDE_DIR})
endif (X11_FOUND)

if (CHECK_OPTIONAL_LIBS)
    find_package(GDAL)
    if (GDAL_FOUND)
        include_directories(${GDAL_INCLUDE_DIR})
        set(HAVE_GDAL 1)
        set(ENABLED_FEATURES "${ENABLED_FEATURES} GDAL")
    endif (GDAL_FOUND)

    find_package(FFMPEG)
    if (FFMPEG_FOUND)
        include_directories(${FFMPEG_INCLUDE_DIR})
        set(HAVE_FFMPEG 1)
        set(ENABLED_FEATURES "${ENABLED_FEATURES} FFmpeg")
    endif ()

    find_package(OpenSceneGraph 3.4.0 COMPONENTS osgGA osgViewer osgUtil osgDB osgText)
    if (OPENSCENEGRAPH_FOUND)
        include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})
        set(HAVE_OSG 1)
        set(ENABLED_FEATURES "${ENABLED_FEATURES} OSG")
    endif ()

    find_package(GL2PS)
    if (GL2PS_FOUND)
        include_directories(${GL2PS_INCLUDE_DIR})
        set(HAVE_GL2PS 1)
        set(ENABLED_FEATURES "${ENABLED_FEATURES} GL2PS")
    endif (GL2PS_FOUND)

    find_package(SWIG)
    if (SWIG_FOUND)
        if (NOT MSVC)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
        endif ()
        set(ENABLED_FEATURES "${ENABLED_FEATURES} SWIG")
    endif ()

    if(NOT ${MSVC_VERSION} STREQUAL "1800")
        find_package(GRPC)
        find_package(Protobuf)
        if (GRPC_FOUND AND Protobuf_FOUND)
            include_directories(${Protobuf_INCLUDE_DIRS} ${GRPC_INCLUDE_DIRS})
            set(ENABLED_FEATURES "${ENABLED_FEATURES} GRPC")
        endif (GRPC_FOUND AND Protobuf_FOUND)
    endif()
endif (CHECK_OPTIONAL_LIBS)

include_directories(${CMAKE_BINARY_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src)

set(commonlibs
        utils_distribution utils_shapes utils_options
        utils_xml utils_geom utils_common utils_importio utils_iodevices foreign_tcpip ${XercesC_LIBRARIES} ${PROJ_LIBRARY})
if (MSVC)
    set(commonlibs ${commonlibs} ws2_32)
endif ()
set(commonvehiclelibs
        utils_emissions foreign_phemlight utils_vehicle ${commonlibs} ${FOX_LIBRARY})

add_subdirectory(src)
if (GTEST_FOUND)
    add_subdirectory(unittest)
endif ()

# set custom name and folder for ALL_BUILD and ZERO_CHECK in visual studio solutions
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

message(STATUS "Enabled features: ${ENABLED_FEATURES}")
