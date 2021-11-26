if (${CMAKE_VERSION} VERSION_GREATER 3.13.0)
    # UseSWIG generates standard target names
    cmake_policy(SET CMP0078 NEW)
endif()
if (${CMAKE_VERSION} VERSION_GREATER 3.14.0)
    # pass module name to swig compiler
    cmake_policy(SET CMP0086 NEW)
endif()
set(libtracistatic_STAT_SRCS
    Connection.cpp
    Connection.h
    Domain.h
    Edge.cpp
    GUI.cpp
    InductionLoop.cpp
    Junction.cpp
    Lane.cpp
    LaneArea.cpp
    MultiEntryExit.cpp
    Person.cpp
    POI.cpp
    Polygon.cpp
    Route.cpp
    Calibrator.cpp
    BusStop.cpp
    ParkingArea.cpp
    ChargingStation.cpp
    RouteProbe.cpp
    Rerouter.cpp
    VariableSpeedSign.cpp
    MeanData.cpp
    OverheadWire.cpp
    Simulation.cpp
    TrafficLight.cpp
    Vehicle.cpp
    VehicleType.cpp
)

add_library(libtracistatic STATIC ${libtracistatic_STAT_SRCS})
add_dependencies(libtracistatic generate-version-h install_dll)
set_property(TARGET libtracistatic PROPERTY FOLDER "libtraci")

if (MSVC)
    set(libtcpip foreign_tcpip ws2_32)
else()
    set(libtcpip foreign_tcpip)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)
add_library(libtracicpp SHARED ${libtracistatic_STAT_SRCS})
set_property(TARGET libtracicpp PROPERTY FOLDER "libtraci")
set_target_properties(libtracicpp PROPERTIES PREFIX "")
set_target_properties(libtracicpp PROPERTIES OUTPUT_NAME libtracicpp${BINARY_SUFFIX})
set_target_properties(libtracicpp PROPERTIES OUTPUT_NAME_DEBUG libtracicpp${BINARY_SUFFIX}D)
target_include_directories(libtracicpp PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>)
install(TARGETS libtracicpp EXPORT SUMOConfig
    DESTINATION bin
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
export(TARGETS libtracicpp NAMESPACE SUMO:: APPEND FILE "${CMAKE_BINARY_DIR}/SUMOConfig.cmake")

if (CLANG_COMPILER AND WIN32)
    target_link_libraries(libtracicpp PRIVATE -Wl,--whole-archive ${libtcpip} -Wl,--no-whole-archive)
elseif (MSVC OR APPLE)
    target_link_libraries(libtracicpp PRIVATE ${libtcpip})
else ()
    target_link_libraries(libtracicpp PRIVATE -Wl,--whole-archive ${libtcpip} -Wl,--no-whole-archive)
endif()

if(SWIG_FOUND)
    include(${SWIG_USE_FILE})
    set_source_files_properties(libtraci.i PROPERTIES CPLUSPLUS ON)
    file(GLOB libtraci_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/../libsumo/*.h")
    if(ENABLE_JAVA_BINDINGS)
        find_package(Java)
        find_package(JNI)
        if(JAVA_FOUND AND JNI_FOUND)
            include(UseJava)
            include_directories(${JNI_INCLUDE_DIRS})
            set(CMAKE_SWIG_FLAGS -package org.eclipse.sumo.libtraci)
            set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/java)
            set(SWIG_MODULE_libtracijni_EXTRA_DEPS ${libtraci_HEADERS} ../libsumo/libsumo_typemap.i)
            if (${CMAKE_VERSION} VERSION_GREATER 3.8.0)
                SWIG_ADD_LIBRARY(libtracijni LANGUAGE java SOURCES libtraci.i)
            else()
                SWIG_ADD_MODULE(libtracijni java libtraci.i)
            endif()
            if (MSVC OR APPLE)
                swig_link_libraries(libtracijni libtracistatic ${libtcpip})
            else()
                set_source_files_properties(${swig_generated_file_fullname} PROPERTIES COMPILE_FLAGS "-Wno-strict-aliasing")
                swig_link_libraries(libtracijni -Wl,--whole-archive libtracistatic ${libtcpip} -Wl,--no-whole-archive)
            endif()
            if (MVN_EXECUTABLE)
                ADD_CUSTOM_COMMAND(TARGET libtracijni POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory src/main/java/org/eclipse/sumo/libtraci
                    COMMAND ${CMAKE_COMMAND} -E copy_directory java src/main/java/org/eclipse/sumo/libtraci
                    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/build/pom.py libtraci
                    COMMAND ${MVN_COMMAND_PREFIX} ${MVN_EXECUTABLE} --batch-mode package source:jar
                )
            else()
                message(WARNING "Could not build libtraci jar, maven was not found.")
            endif()
            set_property(TARGET libtracijni PROPERTY FOLDER "libtraci")
        else()
            message(WARNING "ENABLE_JAVA_BINDINGS is set but Java or JNI was not found.")
        endif()
    endif()

    find_package(PythonLibs)
    if(ENABLE_PYTHON_BINDINGS)
        if (MSVC)
            # recheck that the platform of the generator and python matches
            execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import sys; print(sys.maxsize > 2**32)"
                            OUTPUT_VARIABLE IS_PYTHON64 OUTPUT_STRIP_TRAILING_WHITESPACE)
            if (${CMAKE_MODULE_LINKER_FLAGS} STREQUAL "/machine:x64")
                if (${IS_PYTHON64} STREQUAL "False")
                    message(STATUS "Did not find Python 64 bit. Please set PYTHON_EXECUTABLE, PYTHON_INCLUDE_DIR and PYTHON_LIBRARY manually.")
                    set(PYTHONLIBS_FOUND OFF)
                endif()
            else()
                if (${IS_PYTHON64} STREQUAL "True")
                    message(STATUS "Did not find Python 32 bit. Please set PYTHON_EXECUTABLE, PYTHON_INCLUDE_DIR and PYTHON_LIBRARY manually.")
                    set(PYTHONLIBS_FOUND OFF)
                endif()
            endif()
            if (NOT PYTHON_DEBUG_LIBRARY AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
                message(STATUS "Did not find Python debug library. Please reinstall your python and enable the python debug libraries in the installer.")
                set(PYTHONLIBS_FOUND OFF)
            endif()
        endif()
        if(PYTHONLIBS_FOUND)
            include_directories(${PYTHON_INCLUDE_PATH})
#                set(CMAKE_SWIG_FLAGS "")
            set(CMAKE_SWIG_FLAGS "-keyword") # is necessary for using keyword args
# other options which should be evaluated: -builtin, -modern, -extranative
            set(SWIG_OUTFILE_DIR ${CMAKE_CURRENT_BINARY_DIR})
            set(CMAKE_SWIG_OUTDIR ${CMAKE_SOURCE_DIR}/tools/libtraci)
            set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/tools/libtraci)
#            set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/tools/libtraci)
            set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/tools/libtraci)
            set(SWIG_MODULE_libtraci_EXTRA_DEPS ${libtraci_HEADERS} ../libsumo/libsumo_typemap.i)
            if (${CMAKE_VERSION} VERSION_GREATER 3.8.0)
                if (CLANG_COMPILER)
                    set_property(SOURCE libtraci.i PROPERTY GENERATED_COMPILE_OPTIONS "-Wno-deprecated-declarations")
                endif()
                SWIG_ADD_LIBRARY(libtraci LANGUAGE python SOURCES libtraci.i)
            else()
                SWIG_ADD_MODULE(libtraci python libtraci.i)
            endif()
            if (MSVC OR APPLE)
                set_property(TARGET ${SWIG_MODULE_libtraci_REAL_NAME} PROPERTY FOLDER "libtraci")
                swig_link_libraries(libtraci libtracistatic ${libtcpip} ${PYTHON_LIBRARIES})
            else()
                swig_link_libraries(libtraci -Wl,--whole-archive libtracistatic ${libtcpip} -Wl,--no-whole-archive)
            endif()
            if (NOT ${SETUPTOOLS_MISSING})
                install(
                    CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/build/setup-libtraci.py clean --all install --root=\$ENV{DESTDIR}/ --prefix=${CMAKE_INSTALL_PREFIX} --optimize=1)"
                    COMPONENT pylibtraci)
            endif()
        else()
            message(WARNING "ENABLE_PYTHON_BINDINGS is set but Python libraries were not found.")
        endif()
    endif()

endif()
