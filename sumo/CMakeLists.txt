#Options:
# -DCMAKE_BUILD_TYPE:STRING=Release (default Debug)

option(BUILD_GRPC "Build grpc interface" OFF)

project(SUMO)
cmake_minimum_required(VERSION 3.1)
# do not expand quoted variables in if statements
cmake_policy(SET CMP0054 NEW)

set(CMAKE_COLOR_MAKEFILE ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/build/cmake_modules/")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
        "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(warnings "-Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    #  set(warnings "/W4 /WX /EHsc")
	# enabling /WX is not possible due to warnings in external headers
	# /Wall brings MSVC to complete halt
    set(warnings "/W4 /EHsc")
endif ()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${warnings}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${warnings}")

if (NOT CMAKE_BUILD_TYPE)
    #  set (CMAKE_BUILD_TYPE Release)
    set(CMAKE_BUILD_TYPE Debug)
endif (NOT CMAKE_BUILD_TYPE)
message(STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/bin")
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
if (NOT CMAKE_GENERATOR MATCHES "Xcode|Visual Studio")
    message(STATUS "    Configuration: " ${CMAKE_BUILD_TYPE})
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
        # SET(CMAKE_SYSTEM_NAME Darwin)
        # # Add MacPorts
        # INCLUDE_DIRECTORIES(/opt/local/include)

        # LINK_DIRECTORIES(/opt/local/lib)

    else ()
        # Recommendation, also add a "brew --prefix" custom command to detect a homebrew build environment
        execute_process(COMMAND brew --prefix RESULT_VARIABLE DETECT_BREW OUTPUT_VARIABLE BREW_PREFIX ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
        if (${DETECT_BREW} EQUAL 0)
            link_directories(${BREW_PREFIX}/lib)
            message(STATUS "Brew detected: ${BREW_PREFIX}")
        endif ()
    endif ()
endif (APPLE)

find_package(XercesC REQUIRED)
if (XercesC_FOUND)
    message(STATUS "Found xerces-c: " ${XercesC_VERSION})
    message(STATUS "including: " ${XercesC_INCLUDE_DIR})
	link_directories(${XercesC_LIBRARY})
    include_directories(${XercesC_INCLUDE_DIR})
    #set(libs "${libs}${XercesC_LIBRARY}")
endif (XercesC_FOUND)
#target_link_libraries ( ${PROJECT_NAME} ${XercesC_LIBRARY} )
#
find_package(Proj REQUIRED)
if (PROJ_FOUND)
    message(STATUS "including: " ${PROJ_INCLUDE_DIR})
    #set(libs "${libs}${PROJ_LIBRARY}")
    link_directories(${PROJ_LIBRARY})
    include_directories(${PROJ_INCLUDE_DIR})
    #target_link_libraries ( ${PROJECT_NAME} ${PROJ_LIBRARY} )
    set(HAVE_PROJ 1)
    set(ENABLED_FEATURES "${ENABLED_FEATURES} Proj")
endif (PROJ_FOUND)

find_package(GDAL REQUIRED)
if (GDAL_FOUND)
    message(STATUS "Found GDAL")
    #set(libs "${libs} GDAL")
    message(STATUS "including: " ${GDAL_INCLUDE_DIR})
    link_directories(${GDAL_LIBRARIES})
	include_directories(${GDAL_INCLUDE_DIR})
    #target_link_libraries ( ${PROJECT_NAME} ${GDAL_LIBRARY} )
    set(HAVE_GDAL 1)
    set(ENABLED_FEATURES "${ENABLED_FEATURES} GDAL")
endif (GDAL_FOUND)


find_package(FOX REQUIRED)
if (FOX_FOUND)
    message(STATUS "Found Fox Toolkit")
    include_directories(${FOX_INCLUDE_DIR})
    add_definitions(${FOX_CXX_FLAGS})
    set(HAVE_FOX 1)
    set(ENABLED_FEATURES "${ENABLED_FEATURES} GUI")
endif (FOX_FOUND)

find_package(X11)
if (X11_FOUND)
    link_directories(${X11_LIBRARY_DIR})
    include_directories(${X11_INCLUDE_DIR})
endif (X11_FOUND)
#find_package(OpenGL REQUIRED)

find_package(FFMPEG)
find_package(GTest)

#find_package(PythonLibs REQUIRED)
#if (PYTHONLIBS_FOUND)
#    message(STATUS "Python: " ${PYTHONLIBS_VERSION_STRING})
#    message(STATUS "Python include dirs:" ${PYTHON_INCLUDE_DIRS})
#    message(STATUS "Python libs:" ${PYTHON_LIBRARIES})
#    include_directories(${PYTHON_INCLUDE_DIRS})
#link_directories(${PYTHON_LIBRARIES})
#endif ()

if (APPLE)
    include_directories("/System/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7/")
    set(PYTHON_LIBRARIES "/System/Library/Frameworks/Python.framework/Versions/2.7/lib/libpython2.7.dylib")
else ()
    find_package(PythonLibs REQUIRED)
    if (PYTHONLIBS_FOUND)
        message(STATUS "Python: " ${PYTHONLIBS_VERSION_STRING})
        message(STATUS "Python include dirs:" ${PYTHON_INCLUDE_DIRS})
        message(STATUS "Python libs:" ${PYTHON_LIBRARIES})
        include_directories(${PYTHON_INCLUDE_DIRS})
    endif ()
endif ()

if (BUILD_GRPC)
    find_package(GRPC)
    if (GRPC_FOUND)
        find_package(Protobuf REQUIRED)
        add_definitions(-DBUILD_GRPC)
        add_definitions(${GRPC_DEFINITONS})
        link_directories(${PC_GRPC_LIBRARY_DIRS})
        set(GRPC_LIBS grpc++ grpc protobuf dl)
    endif (GRPC_FOUND)
endif (BUILD_GRPC)

if (APPLE AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(STATUS "configuring for OSX compiler flags")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -std=gnu++11")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -stdlib=libc++")
    SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -stdlib=libc++")
endif (APPLE AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

include_directories(${CMAKE_BINARY_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src)

set(commonlibs
        utils_distribution utils_shapes utils_options
        utils_xml utils_geom utils_common utils_importio utils_iodevices foreign_tcpip ${XercesC_LIBRARY_RELEASE} ${PROJ_LIBRARY})
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(commonlibs
            ${commonlibs} ws2_32)
endif ()
set(commonvehiclelibs
        utils_emissions foreign_phemlight utils_vehicle ${commonlibs} ${FOX_LIBRARY})

add_subdirectory(src)
if (GTEST_FOUND)
  add_subdirectory(unittest)
endif()

