# set VERSION to empty string
cmake_policy(SET CMP0048 NEW)
# do not expand quoted variables in if statements
cmake_policy(SET CMP0054 NEW)
# Options
option(CHECK_OPTIONAL_LIBS "Try to download / enable all optional libraries (use only EPL clean libraries, if set to false)" true)
option(MULTITHREADED_BUILD "Use all available cores for building (applies to Visual Studio only)" true)
option(PROFILING "Enable output of profiling data (applies to gcc/clang builds only)" false)
option(PPROF "Link the pprof profiler library (applies to gcc/clang builds only)" false)
option(COVERAGE "Enable output of coverage data (applies to gcc/clang builds only)" false)
option(SUMO_UTILS "Enable generation of a shared library for the utility functions for option handling, XML parsing etc." false)
option(FMI "Enable generation of an FMI library for SUMO" false)
set(BINARY_SUFFIX "" CACHE STRING "Append the suffix to every generated binary")
set(COMPILE_DEFINITIONS "" CACHE STRING "Macros or defines to add when compiling")
set(CCACHE_SUPPORT ON CACHE BOOL "Enable ccache support if installed")
set(TCMALLOC ON CACHE BOOL "Use tcmalloc if installed")

# Set a default build type if none was specified
# you may use -DCMAKE_BUILD_TYPE:STRING=Debug from the command line
set(default_build_type "Release")

set(CMAKE_ORIGINAL_BUILD_TYPE "${CMAKE_BUILD_TYPE}")
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo")
endif ()

project(SUMO CXX C)
set(PACKAGE_VERSION "git")
cmake_minimum_required(VERSION 3.9)

# Check if libraries have to be found, depending on SUMO_LIBRARIES
set(SUMO_LIBRARIES "$ENV{SUMO_LIBRARIES}" CACHE PATH "Location of SUMOLibraries dependencies")

# some user place SUMOLibraries in the same SUMO folder
if (NOT SUMO_LIBRARIES AND EXISTS "${CMAKE_SOURCE_DIR}/../SUMOLibraries")
    set(SUMO_LIBRARIES "${CMAKE_SOURCE_DIR}/../SUMOLibraries")
endif ()

if (CCACHE_SUPPORT)
    find_program(CCACHE_PROGRAM ccache)
    file(GLOB SCCACHE_ROOT "${SUMO_LIBRARIES}/sccache-*")
    find_program(SCCACHE_PROGRAM sccache ${SCCACHE_ROOT})
    if (CCACHE_PROGRAM)
        message(STATUS "Found ccache: ${CCACHE_PROGRAM}")
        set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
        set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    elseif (SCCACHE_PROGRAM)
        message(STATUS "Found sccache: ${SCCACHE_PROGRAM}")
        set(CMAKE_C_COMPILER_LAUNCHER "${SCCACHE_PROGRAM}")
        set(CMAKE_CXX_COMPILER_LAUNCHER "${SCCACHE_PROGRAM}")
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
            string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
        elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
            string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
            string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
        endif()
    endif()
endif()

set(CMAKE_COLOR_MAKEFILE ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/build/cmake_modules/")

set(ENABLED_FEATURES "${CMAKE_SYSTEM} ${CMAKE_SYSTEM_PROCESSOR} ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} ${CMAKE_BUILD_TYPE}")

if (COMPILE_DEFINITIONS)
    add_compile_definitions(${COMPILE_DEFINITIONS})
endif ()

# declare flags for compilers
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set (GNU_COMPILER True)
endif ()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set (CLANG_COMPILER True)
endif ()

# compiler specific flags
if (GNU_COMPILER OR CLANG_COMPILER)
    # set flags for clang in windows
    if (CLANG_COMPILER AND WIN32)
        # flags for clang in windows
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
        # disable debug build (due a problem with Runtime Libraries)
        set(CMAKE_CONFIGURATION_TYPES "Release")
        set(CMAKE_BUILD_TYPE "Release")
    else ()
        #flags for clang and gcc in linux
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -Wall -pedantic -Wextra")
    endif ()
    if (PROFILING)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
        set(ENABLED_FEATURES "${ENABLED_FEATURES} Profiling")
        set(BINARY_SUFFIX "${BINARY_SUFFIX}P")
    endif ()
    if (COVERAGE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -O0")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
        set(ENABLED_FEATURES "${ENABLED_FEATURES} Coverage")
    endif ()
elseif (MSVC)
    # enabling /WX is not possible due to warnings in external headers
    # /Wall brings MSVC 2013 to complete halt
    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        # this is a workaround to disable lots of warnings for replacing /W3 with /W4 in VS 2019
        # there is a policy for that starting with CMake 3.15
        string(REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else ()
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W4")
    endif ()
    if (MULTITHREADED_BUILD)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    endif ()
    # SWIG generates large obj files
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
    # exporting symbols for shared libraries needs to enabled explicitly
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    # add option for enabling console in release mode
    option(CONSOLE_RELEASE "Enable console in SUMO-GUI and NETEDIT in release (use only for debug purposes)" false)
endif ()

# special debug flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
if (CLANG_COMPILER)
    if (WIN32) 
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined,integer,unsigned-integer-overflow -fsanitize-blacklist=${CMAKE_SOURCE_DIR}/build/clang_sanitize_blacklist.txt")
        # set iterator de bug level in 0
        add_compile_definitions(_ITERATOR_DEBUG_LEVEL=0)
    else ()
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined,address,integer,unsigned-integer-overflow -fno-omit-frame-pointer -fsanitize-blacklist=${CMAKE_SOURCE_DIR}/build/clang_sanitize_blacklist.txt")
    endif ()
endif ()

# we need to build position independent code when generating a shared library
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
if (SUMO_UTILS)
    set(ENABLED_FEATURES "${ENABLED_FEATURES} SumoUtilsLibrary")
endif ()
if (FMI)
    set(ENABLED_FEATURES "${ENABLED_FEATURES} FMI")
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
message(STATUS "    Host: " ${CMAKE_HOST_SYSTEM} " " ${CMAKE_HOST_SYSTEM_PROCESSOR})
message(STATUS "    Target: " ${CMAKE_SYSTEM} " " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "    CMake: " ${CMAKE_VERSION})
message(STATUS "    CMake generator: " ${CMAKE_GENERATOR})
message(STATUS "    CMake build tool: " ${CMAKE_BUILD_TOOL})
message(STATUS "    Compiler: " ${CMAKE_CXX_COMPILER_ID} " " ${CMAKE_CXX_COMPILER_VERSION})
if (CMAKE_GENERATOR MATCHES Xcode)
    message(STATUS "    Xcode: " ${XCODE_VERSION})
endif ()
message(STATUS "")

#set(Python_ADDITIONAL_VERSIONS 3.7 3.6 3.5 3.4 3.3 2.7) # it may help in finding the correct python for libsumo
find_package(PythonInterp REQUIRED)
message(STATUS "Found Python: " ${PYTHON_EXECUTABLE})
execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import setuptools" RESULT_VARIABLE SETUPTOOLS_MISSING)

if (APPLE)
    # we know that openGL is deprecated for newer MacOS
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGL_SILENCE_DEPRECATION")
    # http://stackoverflow.com/questions/1487752/how-do-i-instruct-cmake-to-look-for-libraries-installed-by-macports
    link_directories(/opt/X11/lib)
    include_directories(SYSTEM /opt/X11/include)
    # Detect if the "port" command is valid on this system; if so, return full path
    execute_process(COMMAND which port RESULT_VARIABLE DETECT_MACPORTS OUTPUT_VARIABLE MACPORTS_PREFIX ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (${DETECT_MACPORTS} EQUAL 0)
        # MACPORTS_PREFIX contains now something like "/opt/local/bin/port", so we get the parent directory
        get_filename_component(MACPORTS_PREFIX ${MACPORTS_PREFIX} DIRECTORY)
        # "/opt/local" is where MacPorts lives, add `/lib` suffix and link
        link_directories(${MACPORTS_PREFIX}/../lib)
        include_directories(${MACPORTS_PREFIX}/../include)
        message(STATUS "Macports detected: ${MACPORTS_PREFIX}")
    endif ()

    # Recommendation, also add a "brew --prefix" custom command to detect a homebrew build environment
    execute_process(COMMAND brew --prefix RESULT_VARIABLE DETECT_BREW OUTPUT_VARIABLE BREW_PREFIX ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
    if (${DETECT_BREW} EQUAL 0)
        link_directories(${BREW_PREFIX}/lib)
        include_directories(${BREW_PREFIX}/include)
        message(STATUS "Brew detected: ${BREW_PREFIX}")
    endif ()
endif (APPLE)

#  Specifically define variable WIN32 for compilations under windows (due an error in Shawn)
if (GNU_COMPILER AND WIN32)
    option(USE_MINGW_64BITS "Use 64 bits libraries for the compilation with MinGW" true)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWIN32 -D_WIN32_WINNT=0x0501")
    set(MINGW32 1)
endif ()

# check if SUMOLibraries was found (Only in Windows)
if (SUMO_LIBRARIES AND WIN32)
    # set option for install debug and release runtime dlls
    option(INSTALL_DLL_RUNTIMES "Copy debug and release runtimes for MSVC" true)
    # Special option for MinGW32
    if ((GNU_COMPILER AND WIN32) OR "${CMAKE_MODULE_LINKER_FLAGS}" STREQUAL "/machine:x64")
        message(STATUS "Using 64 bit libraries from SUMO_LIBRARIES placed in " ${SUMO_LIBRARIES})
    else()
        message(STATUS "Using 32 bit libraries from SUMO_LIBRARIES placed in " ${SUMO_LIBRARIES})
        set(LIB_VERSION "/32bits")
        set(CHECK_OPTIONAL_LIBS false)
    endif ()
    file(GLOB XERCES_PATH "${SUMO_LIBRARIES}${LIB_VERSION}/xerces-c-3.?.?")
    file(GLOB ZLIB_PATH "${SUMO_LIBRARIES}${LIB_VERSION}/3rdPartyLibs/zlib-?.?.??")
    file(GLOB PROJ_PATH "${SUMO_LIBRARIES}${LIB_VERSION}/proj-?.?.?")
    file(GLOB FOX_PATH "${SUMO_LIBRARIES}${LIB_VERSION}/fox-?.?.??")
    file(GLOB FREETYPE_PATH "${SUMO_LIBRARIES}${LIB_VERSION}/freetype-2.??.?")
    file(GLOB EIGEN3_PATH "${SUMO_LIBRARIES}/eigen-3.?.?")
    # set all paths in prefix
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${EIGEN3_PATH};${XERCES_PATH};${ZLIB_PATH};${PROJ_PATH};${FOX_PATH};${FREETYPE_PATH}")
    # save in variable SUMO_LIBRARIES_DLL all paths to dll files
    file(GLOB SUMO_LIBRARIES_DLL "${XERCES_PATH}/bin/*.dll"
                                 "${PROJ_PATH}/bin/*.dll"
                                 "${FOX_PATH}/bin/*.dll"
                                 "${ZLIB_PATH}/bin/*.dll"
#                                 "${FREETYPE_PATH}/bin/*.dll"
                                 "${SUMO_LIBRARIES}${LIB_VERSION}/3rdPartyLibs/bzip2-*/bin/*.dll"
                                 "${SUMO_LIBRARIES}${LIB_VERSION}/3rdPartyLibs/curl-*/bin/*.dll"
                                 "${SUMO_LIBRARIES}${LIB_VERSION}/3rdPartyLibs/libpng-*/bin/*.dll"
                                 "${SUMO_LIBRARIES}${LIB_VERSION}/3rdPartyLibs/libssh2-*/bin/*.dll"
                                 "${SUMO_LIBRARIES}${LIB_VERSION}/3rdPartyLibs/openssl-*/*.dll"
                                 "${SUMO_LIBRARIES}${LIB_VERSION}/3rdPartyLibs/sqlite-*/*.dll"
                                 "${SUMO_LIBRARIES}${LIB_VERSION}/3rdPartyLibs/tiff-*/bin/*.dll")
    # check if MSVC dll runtimes have to be copied
    if (INSTALL_DLL_RUNTIMES)
        file(GLOB MSVC_DLLS "${SUMO_LIBRARIES}${LIB_VERSION}/runtimes/*.dll")
        list(APPEND SUMO_LIBRARIES_DLL ${MSVC_DLLS})
    endif(INSTALL_DLL_RUNTIMES)
    # declare flag for use google test
    set(USE_GOOGLETEST true)
    # check if use google test
    if (WIN32)
        if (GNU_COMPILER AND NOT USE_MINGW_64BITS)
            message(STATUS "Disabled Google Test in Mingw32")
            set(USE_GOOGLETEST false)
        elseif(CLANG_COMPILER)
            message(STATUS "Disabled Google Test in Clang (Windows)")
            set(USE_GOOGLETEST false)
        endif ()
    endif()
    if (USE_GOOGLETEST)
        file(GLOB GTEST_ROOT "${SUMO_LIBRARIES}${LIB_VERSION}/googletest-?.??.?")
        message(STATUS "Found Google test: " ${GTEST_ROOT})
        include_directories("${GTEST_ROOT}/include")
        # set both google test library
        set(GTEST_BOTH_LIBRARIES "${GTEST_ROOT}/lib/gtest.lib" "${GTEST_ROOT}/lib/gtest_main.lib")
        # set google test DLLs
        file(GLOB GTEST_DLL "${GTEST_ROOT}/bin/gtest*.dll")
        set(GTEST_FOUND true)
    endif ()
    file(GLOB SWIG_EXECUTABLE "${SUMO_LIBRARIES}/swigwin-*/swig.exe")
    file(GLOB MVN_EXECUTABLE "${SUMO_LIBRARIES}/apache-maven-*/bin/mvn.cmd")
    file(GLOB TEXTTEST_EXECUTABLE "${SUMO_LIBRARIES}/TextTest-*/texttest.exe")
else ()
    # for Linux and Mac only
    find_package(GTest)
endif ()

find_package(XercesC REQUIRED)
if (XercesC_FOUND)
    include_directories(SYSTEM ${XercesC_INCLUDE_DIRS})
endif (XercesC_FOUND)

find_package(Proj)
if (PROJ_FOUND)
    include_directories(SYSTEM ${PROJ_INCLUDE_DIR})
    set(ENABLED_FEATURES "${ENABLED_FEATURES} Proj")
endif (PROJ_FOUND)

find_package(FOX)
if (FOX_FOUND)
    include_directories(SYSTEM ${FOX_INCLUDE_DIR})
    add_definitions(${FOX_CXX_FLAGS})
    add_definitions(-DFLOAT_MATH_FUNCTIONS)
    set(HAVE_FOX 1)
    set(ENABLED_FEATURES "${ENABLED_FEATURES} GUI")
endif (FOX_FOUND)

find_package(Freetype)
if (FREETYPE_FOUND)
    include_directories(SYSTEM ${FREETYPE_INCLUDE_DIRS})
else (FREETYPE_FOUND)
    message(WARNING "FreeType cannot be found. If you are using SUMO libraries, update SUMO_LIBRARIES folder with git pull. Otherwise, specify manually FreeType include folder and libs.")
endif (FREETYPE_FOUND)

find_package(X11)
if (X11_FOUND)
    link_directories(${X11_LIBRARY_DIR})
    include_directories(SYSTEM ${X11_INCLUDE_DIR})
endif (X11_FOUND)

find_package(ZLIB)
if (ZLIB_FOUND)
    set(HAVE_ZLIB 1)
    link_directories(${ZLIB_LIBRARY_DIR})
    include_directories(SYSTEM ${ZLIB_INCLUDE_DIR})
endif ()

find_package(SWIG 3.0)
if (SWIG_FOUND)
    set(ENABLED_FEATURES "${ENABLED_FEATURES} SWIG")
endif ()

if (TCMALLOC)
    find_library(TCMALLOC_LIBRARY NAMES tcmalloc_minimal)
    if (TCMALLOC_LIBRARY)
        set(ENABLED_FEATURES "${ENABLED_FEATURES} tcmalloc")
    else ()
        set(TCMALLOC_LIBRARY "")
    endif ()
endif ()

if (CHECK_OPTIONAL_LIBS)
    file(GLOB GDAL_PATH "${SUMO_LIBRARIES}${LIB_VERSION}/gdal-?.?.?")
    file(GLOB FFMPEG_PATH "${SUMO_LIBRARIES}/FFMPEG-?.?.?")
    file(GLOB OSG_PATH "${SUMO_LIBRARIES}/OSG-?.?.?")
    file(GLOB GL2PS_PATH "${SUMO_LIBRARIES}${LIB_VERSION}/gl2ps-?.?.?")
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${GDAL_PATH};${FFMPEG_PATH};${OSG_PATH};${GL2PS_PATH}")
    file(GLOB SUMO_OPTIONAL_LIBRARIES_DLL "${GDAL_PATH}/bin/*.dll" "${FFMPEG_PATH}/bin/*.dll" "${GL2PS_PATH}/bin/*.dll")

    # GDAL (for geopositioning)
    find_package(GDAL)
    if (GDAL_FOUND)
        include_directories(SYSTEM ${GDAL_INCLUDE_DIR})
        set(HAVE_GDAL 1)
        set(ENABLED_FEATURES "${ENABLED_FEATURES} GDAL")
    else (GDAL_FOUND)
        set(GDAL_LIBRARY "")
    endif (GDAL_FOUND)

    # FFMPEG (for recording videos)
    find_package(FFMPEG)
    if (FFMPEG_FOUND)
        include_directories(SYSTEM ${FFMPEG_INCLUDE_DIR})
        set(HAVE_FFMPEG 1)
        set(ENABLED_FEATURES "${ENABLED_FEATURES} FFmpeg")
    endif ()

    # OSG (For 3D view)
    find_package(OpenSceneGraph 3.4.0 COMPONENTS osgGA osgViewer osgUtil osgDB osgText)
    if (OPENSCENEGRAPH_FOUND)
        include_directories(SYSTEM ${OPENSCENEGRAPH_INCLUDE_DIRS})
        set(HAVE_OSG 1)
        set(ENABLED_FEATURES "${ENABLED_FEATURES} OSG")
    endif ()

    # GL2PS (deprecated, will be changed by FreeType
    find_package(GL2PS)
    if (GL2PS_FOUND)
        include_directories(SYSTEM ${GL2PS_INCLUDE_DIR})
        set(HAVE_GL2PS 1)
        set(ENABLED_FEATURES "${ENABLED_FEATURES} GL2PS")
    endif (GL2PS_FOUND)

    # Eigen (for overhead wire / electric circuit simulation)
    find_package(Eigen3 3.2)
    if (EIGEN3_FOUND)
        include_directories(SYSTEM ${EIGEN3_INCLUDE_DIR})
        set(HAVE_EIGEN 1)  # see config.h.cmake for #cmakedefine
        set(ENABLED_FEATURES "${ENABLED_FEATURES} Eigen")
    endif (EIGEN3_FOUND)

    # set(JPS_FOUND 1)
    if (JPS_FOUND)
        add_library(jps SHARED IMPORTED) # or STATIC instead of SHARED
        set_target_properties(jps PROPERTIES IMPORTED_LOCATION "/tmp/libjps.so")
        include_directories(SYSTEM "/tmp/include")
        set(HAVE_JPS 1)
    endif (JPS_FOUND)

    # add optional libraries
    list(APPEND SUMO_LIBRARIES_DLL ${SUMO_OPTIONAL_LIBRARIES_DLL})
endif (CHECK_OPTIONAL_LIBS)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(commonlibs
        utils_distribution utils_handlers utils_shapes utils_options
        utils_xml utils_geom utils_common utils_importio utils_iodevices utils_traction_wire foreign_tcpip
        ${XercesC_LIBRARIES} ${ZLIB_LIBRARIES} ${PROJ_LIBRARY} ${TCMALLOC_LIBRARY})
if (MSVC)
    set(commonlibs ${commonlibs} ws2_32)
endif ()
if (PPROF)
    set(commonlibs ${commonlibs} profiler)
endif ()
set(commonvehiclelibs
        utils_emissions foreign_phemlight utils_vehicle ${commonlibs} ${FOX_LIBRARY})

# set custom name and folder for ALL_BUILD and ZERO_CHECK in visual studio solutions
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

# installation
if (MSVC)
    if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(CMAKE_INSTALL_PREFIX "sumo-${PACKAGE_VERSION}")
    endif()
else ()
    include(GNUInstallDirs)
endif ()
if (SKBUILD OR MSVC)
    set(DATA_PATH "")
else ()
    set(DATA_PATH "share/sumo/")
endif ()
install(DIRECTORY bin/ DESTINATION bin
        FILES_MATCHING PATTERN "*.dll"
        PATTERN "*d.dll" EXCLUDE
        PATTERN "gtest*.dll" EXCLUDE
        PATTERN "FOXDLLD-1.6.dll" EXCLUDE)
install(DIRECTORY data/ DESTINATION ${DATA_PATH}data)
install(DIRECTORY tools/ DESTINATION ${DATA_PATH}tools
        USE_SOURCE_PERMISSIONS
        PATTERN "build/sumo" EXCLUDE  # needed for the scikit build
        PATTERN "calibration" EXCLUDE
        PATTERN "lisum-core" EXCLUDE
        PATTERN "lisum-gui" EXCLUDE
        PATTERN "sumolib4matlab/src" EXCLUDE
        PATTERN "traas" EXCLUDE
        PATTERN "traci4matlab/src" EXCLUDE
        PATTERN "__pycache__" EXCLUDE
        PATTERN "*.pyc" EXCLUDE
        PATTERN "*.egg-info" EXCLUDE
        PATTERN ".git" EXCLUDE)
if (DATA_PATH)
    install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ../../bin \$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${DATA_PATH}bin)")
endif ()

install(EXPORT SUMOConfig DESTINATION ${DATA_PATH}cmake NAMESPACE SUMO::)

if (NOT ${SETUPTOOLS_MISSING})
    install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/build/setup-sumolib.py clean --all install --root=\$ENV{DESTDIR}/ --prefix=${CMAKE_INSTALL_PREFIX} --optimize=1)"
            COMPONENT pysumolib)
    install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/build/setup-traci.py clean --all install --root=\$ENV{DESTDIR}/ --prefix=${CMAKE_INSTALL_PREFIX} --optimize=1)"
            COMPONENT pytraci)
endif ()

if (SUMO_LIBRARIES AND WIN32)
    # filter release DLLs
    execute_process(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/build/filterDebugDLL.py ${SUMO_LIBRARIES_DLL}
                    OUTPUT_VARIABLE SUMO_LIBRARIES_RELEASE_DLL)
    # copy debug dlls
    foreach(DLL_PATH ${SUMO_LIBRARIES_DLL} ${GTEST_DLL})
        get_filename_component(DLL_BASENAME ${DLL_PATH} NAME)
        set(TARGET_DLL ${CMAKE_SOURCE_DIR}/bin/${DLL_BASENAME})
        list(FIND TARGET_ALL_DLLS ${TARGET_DLL} DLL_KNOWN)
        list(FIND SUMO_LIBRARIES_RELEASE_DLL ${DLL_PATH} IS_RELEASE_DLL)
        list(FIND GTEST_DLL ${DLL_PATH} IS_TEST_DLL)
        if (DLL_KNOWN EQUAL -1)
            add_custom_command(OUTPUT ${TARGET_DLL}
                               COMMAND ${CMAKE_COMMAND} -E copy ${DLL_PATH} ${TARGET_DLL}
                               DEPENDS ${DLL_PATH})
            if (IS_TEST_DLL GREATER -1)
                list(APPEND TARGET_TEST_DLLS ${TARGET_DLL})
            elseif (IS_RELEASE_DLL GREATER -1)
                list(APPEND TARGET_ALL_DLLS ${TARGET_DLL})
            else ()
                list(APPEND TARGET_ALL_DLLS $<$<CONFIG:Debug>:${TARGET_DLL}>)
            endif ()
        endif()
    endforeach(DLL_PATH)
    # copy OSG dlls
    if (HAVE_OSG)
        file(GLOB OSG_DLL "${OSG_PATH}/bin/*osg.dll")
        get_filename_component(OSG_DLL_BASENAME ${OSG_DLL} NAME)
        set(OSG_TARGET ${CMAKE_SOURCE_DIR}/bin/${OSG_DLL_BASENAME})
        add_custom_command(OUTPUT ${OSG_TARGET}
                           COMMAND ${CMAKE_COMMAND} -E copy_directory ${OSG_PATH}/bin ${CMAKE_SOURCE_DIR}/bin
                           DEPENDS ${OSG_DLL})
    endif (HAVE_OSG)
    # proj 7 needs to copy "share/proj" folder in SUMO_HOME/share
    set(PROJ_DATA ${CMAKE_SOURCE_DIR}/share/proj/proj.db)
    add_custom_command(OUTPUT ${PROJ_DATA}
                       COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJ_PATH}/share/proj ${CMAKE_SOURCE_DIR}/share/proj
                       DEPENDS ${PROJ_PATH}/share/proj/proj.db)
    install(DIRECTORY share/ DESTINATION ${DATA_PATH}share)
endif()
# set install dll targets, for non-Windows they depend on nothing
add_custom_target(install_dll DEPENDS ${TARGET_ALL_DLLS} ${OSG_TARGET} ${PROJ_DATA})
add_custom_target(install_test_dll DEPENDS ${TARGET_TEST_DLLS})
set_property(TARGET install_dll PROPERTY FOLDER "CMake")
set_property(TARGET install_test_dll PROPERTY FOLDER "CMake")

# java targets
find_program(MVN_EXECUTABLE mvn)
find_package(Java COMPONENTS Development)
if (MVN_EXECUTABLE AND Java_FOUND)
    if (NOT DEFINED ENV{JAVA_HOME})
        get_filename_component(JAVA_BIN ${Java_JAVAC_EXECUTABLE} DIRECTORY)
        get_filename_component(JAVA_HOME ${JAVA_BIN} DIRECTORY)
        set(MVN_COMMAND_PREFIX ${CMAKE_COMMAND} -E env JAVA_HOME=${JAVA_HOME})
    endif ()
    add_custom_target(traas
        COMMAND ${MVN_COMMAND_PREFIX} ${MVN_EXECUTABLE} --batch-mode -f tools/contributed/traas/pom.xml clean install
        COMMAND ${CMAKE_COMMAND} -E copy tools/contributed/traas/target/traas-1.1.jar bin/TraaS.jar
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    add_custom_target(lisum
        COMMAND ${MVN_COMMAND_PREFIX} ${MVN_EXECUTABLE} --batch-mode -f tools/contributed/lisum/pom.xml clean install
        COMMAND ${CMAKE_COMMAND} -E copy tools/contributed/lisum/lisum-core/target/lisum-core-1.0.2.jar bin/lisum-core.jar
        COMMAND ${CMAKE_COMMAND} -E copy tools/contributed/lisum/lisum-gui/target/lisum-gui-1.1.jar bin/lisum-gui.jar
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    add_dependencies(lisum traas)
    set(JAVA_TARGETS traas lisum)
    foreach (JAVAT ${JAVA_TARGETS})
        set_property(TARGET ${JAVAT} PROPERTY EXCLUDE_FROM_DEFAULT_BUILD TRUE)
        set_property(TARGET ${JAVAT} PROPERTY FOLDER "java")
    endforeach ()
endif ()

# doc targets
add_custom_target(doxygen
    COMMAND rm -rf docs/doxygen
    COMMAND mkdir docs/doxygen
    COMMAND doxygen sumo.doxyconf &> doxygen.log
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
set_property(TARGET doxygen PROPERTY EXCLUDE_FROM_DEFAULT_BUILD TRUE)
set_property(TARGET doxygen PROPERTY FOLDER "doc")

add_custom_target(userdoc
    COMMAND ../../tools/build/buildPyDoc.py -p ../pydoc -c
    COMMAND mkdocs build -d ../userdoc
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs/web)
set_property(TARGET userdoc PROPERTY EXCLUDE_FROM_DEFAULT_BUILD TRUE)
set_property(TARGET userdoc PROPERTY FOLDER "doc")

if (MVN_EXECUTABLE)
    add_custom_target(javadoc
        COMMAND rm -rf docs/javadoc
        COMMAND mkdir docs/javadoc
        COMMAND ${MVN_EXECUTABLE} --batch-mode -f tools/contributed/traas/pom.xml javadoc:javadoc
        COMMAND mv tools/contributed/traas/target/site/apidocs docs/javadoc/traas
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    set_property(TARGET javadoc PROPERTY EXCLUDE_FROM_DEFAULT_BUILD TRUE)
    set_property(TARGET javadoc PROPERTY FOLDER "doc")
    set(JAVADOC_TARGET javadoc)
endif ()

find_program(HELPMAN_EXECUTABLE help2man)
if (HELPMAN_EXECUTABLE)
    add_custom_target(man
        COMMAND rm -rf docs/man
        COMMAND mkdir docs/man
        COMMAND ${HELPMAN_EXECUTABLE} -N -n "A microscopic, multi-modal traffic simulation" bin/sumo > docs/man/sumo.1
        COMMAND ${HELPMAN_EXECUTABLE} -N -n "GUI version of the simulation SUMO" bin/sumo-gui > docs/man/sumo-gui.1
        COMMAND ${HELPMAN_EXECUTABLE} -N -n "Builds vehicle routes for SUMO using detector values" bin/dfrouter > docs/man/dfrouter.1
        COMMAND ${HELPMAN_EXECUTABLE} -N -n "Shortest path router and DUE computer for the microscopic traffic simulation SUMO" bin/duarouter > docs/man/duarouter.1
        COMMAND ${HELPMAN_EXECUTABLE} -N -n "Router for the microscopic traffic simulation SUMO based on junction turning ratios" bin/jtrrouter > docs/man/jtrrouter.1
        COMMAND ${HELPMAN_EXECUTABLE} -N -n "Import O/D-matrices and trips using macroscopic traffic assignment for SUMO" bin/marouter > docs/man/marouter.1
        COMMAND ${HELPMAN_EXECUTABLE} -N -n "Generates routes of persons throughout a day for the microscopic traffic simulation SUMO" bin/activitygen > docs/man/activitygen.1
        COMMAND ${HELPMAN_EXECUTABLE} -N -n "Importer of O/D-matrices for the traffic simulation SUMO" bin/od2trips > docs/man/od2trips.1
        COMMAND ${HELPMAN_EXECUTABLE} -N -n "Road network importer / builder for the traffic simulation SUMO" bin/netconvert > docs/man/netconvert.1
        COMMAND ${HELPMAN_EXECUTABLE} -N -n "Road network editor for the traffic simulation SUMO" bin/netedit > docs/man/netedit.1
        COMMAND ${HELPMAN_EXECUTABLE} -N -n "Road network generator for the microscopic traffic simulation SUMO" bin/netgenerate > docs/man/netgenerate.1
        COMMAND ${HELPMAN_EXECUTABLE} -N -n "Importer of polygons and POIs for the traffic simulation SUMO" bin/polyconvert > docs/man/polyconvert.1
        COMMAND ${HELPMAN_EXECUTABLE} -N -n "TraCITestClient for the traffic simulation SUMO" bin/TraCITestClient > docs/man/TraCITestClient.1
        COMMAND ${HELPMAN_EXECUTABLE} -N -n "Builds and writes an emissions map for SUMO\\\'s emission models" bin/emissionsMap > docs/man/emissionsMap.1
        COMMAND ${HELPMAN_EXECUTABLE} -N -n "Computes emissions by driving a time line using SUMO\\\'s emission models" bin/emissionsDrivingCycle > docs/man/emissionsDrivingCycle.1
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    set_property(TARGET man PROPERTY EXCLUDE_FROM_DEFAULT_BUILD TRUE)
    set_property(TARGET man PROPERTY FOLDER "doc")
    set(MAN_TARGET man)
endif ()

add_custom_target(doc)
add_dependencies(doc doxygen userdoc ${JAVADOC_TARGET} ${MAN_TARGET})
set_property(TARGET doc PROPERTY EXCLUDE_FROM_DEFAULT_BUILD TRUE)
set_property(TARGET doc PROPERTY FOLDER "doc")

# coverage targets
if (COVERAGE)
    find_program(LCOV_EXECUTABLE lcov)
    if (LCOV_EXECUTABLE)
        add_custom_target(lcov
            COMMAND rm -rf docs/lcov
            COMMAND mkdir docs/lcov
            COMMAND ${LCOV_EXECUTABLE} -d . --capture --output-file docs/lcov/lcov.info
            COMMAND ${LCOV_EXECUTABLE} --remove docs/lcov/lcov.info '/usr/*' --output-file docs/lcov/lcov.info
            COMMAND genhtml -o docs/lcov/html docs/lcov/lcov.info
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
        add_custom_target(lcov-reset
            COMMAND ${LCOV_EXECUTABLE} -d . -z
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    else ()
        message(WARNING "COVERAGE is enabled but lcov was not found.")
    endif ()
endif ()

# testing
enable_testing()
add_subdirectory(src)
if (GTEST_FOUND)
    add_subdirectory(unittest)
endif ()
if (TEXTTEST_EXECUTABLE AND EXISTS ${CMAKE_SOURCE_DIR}/tests/runCiTests.bat)
    add_test(texttest ${CMAKE_SOURCE_DIR}/tests/runCiTests.bat ${TEXTTEST_EXECUTABLE})
else ()
    find_program(TEXTTEST_FOUND "texttest")
    if (TEXTTEST_FOUND AND EXISTS ${CMAKE_SOURCE_DIR}/tests/runTests.sh)
        add_test(texttest ${CMAKE_SOURCE_DIR}/tests/runTests.sh -b ci -v ci)
    endif()
endif()

# example and dist targets
add_custom_target(examples
    COMMAND ${PYTHON_EXECUTABLE} tools/extractTest.py -x -f tests/examples.txt -p docs/examples/runAll.py
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
add_dependencies(examples sumo netconvert dfrouter duarouter jtrrouter)
set_property(TARGET examples PROPERTY EXCLUDE_FROM_DEFAULT_BUILD TRUE)
set_property(TARGET examples PROPERTY FOLDER "doc")

find_program(PYINSTALLER_FOUND "pyinstaller")
if (PYINSTALLER_FOUND AND EXISTS ${CMAKE_SOURCE_DIR}/tools/game/runner.spec)
    add_custom_target(game
        COMMAND pyinstaller --noconfirm ${CMAKE_SOURCE_DIR}/tools/game/runner.spec
        COMMAND ${CMAKE_COMMAND} -E chdir dist/runner ${CMAKE_COMMAND} -E tar "cf" "${CMAKE_BINARY_DIR}/sumo-game.zip" --format=zip "."
    )
endif()

add_test(exampletest ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/docs/examples/runAll.py)
find_package(Git)
if (GIT_FOUND)
    add_custom_target(dist
        COMMAND rm -rf sumo-${PACKAGE_VERSION} sumo-${PACKAGE_VERSION}.zip sumo-src-${PACKAGE_VERSION}.tar.gz sumo-src-${PACKAGE_VERSION}.zip sumo-all-${PACKAGE_VERSION}.tar.gz sumo-all-${PACKAGE_VERSION}.zip sumo-gui-macos-${PACKAGE_VERSION}.zip
        COMMAND ${GIT_EXECUTABLE} archive --prefix sumo-${PACKAGE_VERSION}/ -o sumo-${PACKAGE_VERSION}.zip HEAD
        COMMAND unzip -q sumo-${PACKAGE_VERSION}.zip
        COMMAND cp -a docs/tutorial docs/examples sumo-${PACKAGE_VERSION}/docs
        COMMAND find tools/contributed/saga/ tools/contributed/traci4matlab -type f | grep -v .git | xargs cp --parents --target-dir sumo-${PACKAGE_VERSION}
        COMMAND mkdir sumo-${PACKAGE_VERSION}/include
        COMMAND cp ${CMAKE_BINARY_DIR}/src/version.h sumo-${PACKAGE_VERSION}/include
        COMMAND zip -rq sumo-src-${PACKAGE_VERSION}.zip sumo-${PACKAGE_VERSION}
        COMMAND tar -czf sumo-src-${PACKAGE_VERSION}.tar.gz sumo-${PACKAGE_VERSION}
        COMMAND cp -a docs/userdoc docs/pydoc docs/javadoc docs/man sumo-${PACKAGE_VERSION}/docs
        COMMAND tar -czf sumo_${PACKAGE_VERSION}.orig.tar.gz --exclude "*.jar" sumo-${PACKAGE_VERSION}
        COMMAND mkdir sumo-${PACKAGE_VERSION}/sumo-gui-macos
        COMMAND cp -a build/macos/*/*.app sumo-${PACKAGE_VERSION}/sumo-gui-macos
        COMMAND zip -rq sumo-gui-macos-${PACKAGE_VERSION}.zip sumo-${PACKAGE_VERSION}/sumo-gui-macos/*.app
        COMMAND rm -rf sumo-${PACKAGE_VERSION} sumo-${PACKAGE_VERSION}.zip
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    add_dependencies(dist examples doc ${JAVA_TARGETS})
    set_property(TARGET dist PROPERTY FOLDER "CMake")
    set_property(TARGET dist PROPERTY EXCLUDE_FROM_DEFAULT_BUILD TRUE)

    add_custom_target(distcheck
        COMMAND rm -rf sumo-${PACKAGE_VERSION}
        COMMAND unzip -q sumo-src-${PACKAGE_VERSION}.zip
        COMMAND cd sumo-${PACKAGE_VERSION} && mkdir _cmake_build _cmake_install && cd _cmake_build
                && cmake -DCMAKE_INSTALL_PREFIX=../_cmake_install -DCHECK_OPTIONAL_LIBS=${CHECK_OPTIONAL_LIBS} .. || (echo "ERROR: the cmake configuration failed." && false)
                && make -j8 || (echo "ERROR: the compilation failed." && false)
                && make test || (echo "ERROR: the test suite failed." && false)
                && make install || (echo "ERROR: the install target failed." && false)
        COMMAND rm -rf sumo-${PACKAGE_VERSION}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    add_dependencies(distcheck dist)
    set_property(TARGET distcheck PROPERTY FOLDER "CMake")
    set_property(TARGET distcheck PROPERTY EXCLUDE_FROM_DEFAULT_BUILD TRUE)
endif ()
message(STATUS "Enabled features: ${ENABLED_FEATURES}")
