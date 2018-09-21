#Options:
option(CHECK_OPTIONAL_LIBS "Try to download / enable all optional libraries" true)
option(MULTITHREADED_BUILD "Use all available cores for building" true)

# Set a default build type if none was specified
# you may use -DCMAKE_BUILD_TYPE:STRING=Debug from the command line
set(default_build_type "Release")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

project(SUMO)
set(PACKAGE_VERSION "git")
cmake_minimum_required(VERSION 3.1)
# do not expand quoted variables in if statements
cmake_policy(SET CMP0054 NEW)

set(CMAKE_COLOR_MAKEFILE ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/build/cmake_modules/")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
        "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic -Wextra")
elseif (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /W3")
    # set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W4 /WX /EHsc")
    # enabling /WX is not possible due to warnings in external headers
    # /Wall brings MSVC to complete halt
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W4 /EHsc")
    if (MULTITHREADED_BUILD)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    endif ()
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined,address,integer,unsigned-integer-overflow -fno-omit-frame-pointer -fsanitize-blacklist=${CMAKE_SOURCE_DIR}/build/clang_sanitize_blacklist.txt")
endif ()

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
message(STATUS "    Compiler: " ${CMAKE_CXX_COMPILER_ID})
if (MSVC)
    message(STATUS "    MSVC: " ${MSVC_VERSION})
endif ()
if (CMAKE_GENERATOR MATCHES Xcode)
    message(STATUS "    Xcode: " ${XCODE_VERSION})
endif ()
message(STATUS "")

set(ENABLED_FEATURES "${CMAKE_HOST_SYSTEM}")

#set(Python_ADDITIONAL_VERSIONS 3.7 3.6 3.5 3.4 3.3 2.7) # it may help in finding the correct python for libsumo
find_package(PythonInterp REQUIRED)
message(STATUS "Found Python: " ${PYTHON_EXECUTABLE})

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

#  Specifically define variable WIN32 for compilations under windows (due an error in Shawn)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND WIN32)
    option(USE_MINGW_64BITS "Use 64 bits libraries for the compilation with MinGW" true)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWIN32 -D_WIN32_WINNT=0x0501")
endif ()

# Check if libraries have to be found, depending on SUMO_LIBRARIES
set(SUMO_LIBRARIES "$ENV{SUMO_LIBRARIES}" CACHE PATH "Location of SUMOLibraries dependencies")
if (SUMO_LIBRARIES)
    # Special option for MinGW32
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND WIN32)
        if(USE_MINGW_64BITS)
            message(STATUS "Using 64 bit libraries from SUMO_LIBRARIES placed in " ${SUMO_LIBRARIES} " for MinGW compilation")
            set(LIB_VERSION "_64")
        else()
            message(STATUS "Using 32 bit libraries from SUMO_LIBRARIES placed in " ${SUMO_LIBRARIES} " for MinGW compilation")
            set(CHECK_OPTIONAL_LIBS false)
        endif()
    elseif(${CMAKE_MODULE_LINKER_FLAGS} STREQUAL "/machine:x64")
        message(STATUS "Using 64 bit libraries from SUMO_LIBRARIES placed in " ${SUMO_LIBRARIES})
        set(LIB_VERSION "_64")
    else()
        message(STATUS "Using 32 bit libraries from SUMO_LIBRARIES placed in " ${SUMO_LIBRARIES})
        set(CHECK_OPTIONAL_LIBS false)
    endif()
    # TODO: Use file(GLOB ...) to avoid explicit versions
    # set Xerces
    if(${MSVC_VERSION} AND ${MSVC_VERSION} STREQUAL "1800")
        set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${SUMO_LIBRARIES}/msvc12/xerces-c-3.2.0${LIB_VERSION}")
    else()
        set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${SUMO_LIBRARIES}/xerces-c-3.2.0${LIB_VERSION}")
    endif()
    # set Proj
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${SUMO_LIBRARIES}/proj_gdal-1911${LIB_VERSION}")
    # set FOX
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${SUMO_LIBRARIES}/fox-1.6.55${LIB_VERSION}")
    # set GoogleTest only if compiler isn't mingw32
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND WIN32)
        if(USE_MINGW_64BITS)
            # Prevent GoogleTest from overriding our compiler/linker options
            set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
            add_subdirectory("${SUMO_LIBRARIES}/googletest-1.8.0/googletest"
                             "${CMAKE_BINARY_DIR}/googletest-build")
            include_directories("${SUMO_LIBRARIES}/googletest-1.8.0/googletest/include")
            set(GTEST_BOTH_LIBRARIES "gtest" "gtest_main")
            set(GTEST_FOUND true)
        else()
            message(STATUS "Disabled Google Test in Mingw32")
        endif()
    endif()
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

    find_package(SWIG 3.0)
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

# set custom name and folder for ALL_BUILD and ZERO_CHECK in visual studio solutions
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

add_subdirectory(src)
if (GTEST_FOUND)
    enable_testing()
    add_subdirectory(unittest)
endif ()

# installation
install(DIRECTORY data/ DESTINATION share/sumo/data)
install(DIRECTORY tools/ DESTINATION share/sumo/tools
        PATTERN "calibration" EXCLUDE
        PATTERN "lisum" EXCLUDE
        PATTERN "sumolib4matlab/src" EXCLUDE
        PATTERN "traas" EXCLUDE
        PATTERN "traci4matlab/src" EXCLUDE
        PATTERN ".git" EXCLUDE
       )
install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ../../bin $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/sumo/bin)")

# custom targets
add_custom_target(examples DEPENDS sumo netconvert
    COMMAND ${PYTHON_EXECUTABLE} tools/extractTest.py -x -f tests/examples.txt -p docs/examples/runAll.py
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_test(exampletest ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/docs/examples/runAll.py)
find_package(Git)
if (GIT_FOUND)
    add_custom_target(dist DEPENDS examples
        COMMAND rm -rf sumo-${PACKAGE_VERSION} sumo-${PACKAGE_VERSION}.zip sumo-src-${PACKAGE_VERSION}.tar.gz
        COMMAND ${GIT_EXECUTABLE} archive --prefix sumo-${PACKAGE_VERSION}/ -o sumo-${PACKAGE_VERSION}.zip HEAD
        COMMAND unzip sumo-${PACKAGE_VERSION}.zip
        COMMAND cp -a docs/tutorial docs/examples sumo-${PACKAGE_VERSION}/docs
        COMMAND zip -r sumo-src-${PACKAGE_VERSION}.zip sumo-${PACKAGE_VERSION}
        COMMAND tar czf sumo-src-${PACKAGE_VERSION}.tar.gz sumo-${PACKAGE_VERSION}
        COMMAND rm -rf sumo-${PACKAGE_VERSION} sumo-${PACKAGE_VERSION}.zip
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    set_property(TARGET dist PROPERTY FOLDER "CMake") 
    set_property(TARGET dist PROPERTY EXCLUDE_FROM_DEFAULT_BUILD TRUE)

    add_custom_target(distcheck DEPENDS dist
        COMMAND rm -rf sumo-${PACKAGE_VERSION}
        COMMAND unzip sumo-src-${PACKAGE_VERSION}.zip
        COMMAND cd sumo-${PACKAGE_VERSION} && mkdir _cmake_build _cmake_install && cd _cmake_build
                && cmake -DCMAKE_INSTALL_PREFIX=../_cmake_install .. || (echo "ERROR: the cmake configuration failed." && false)
                && make -j8 || (echo "ERROR: the compilation failed." && false)
                && make test || (echo "ERROR: the test suite failed." && false)
                && make install || (echo "ERROR: the install target failed." && false)
        COMMAND rm -rf sumo-${PACKAGE_VERSION}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    set_property(TARGET distcheck PROPERTY FOLDER "CMake") 
    set_property(TARGET distcheck PROPERTY EXCLUDE_FROM_DEFAULT_BUILD TRUE)
endif ()
message(STATUS "Enabled features: ${ENABLED_FEATURES}")
