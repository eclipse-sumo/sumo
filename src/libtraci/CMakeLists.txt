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

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)
add_library(libtracistatic STATIC ${libtracistatic_STAT_SRCS})
add_dependencies(libtracistatic generate-version-h install_dll)
set_target_properties(libtracistatic PROPERTIES FOLDER "libtraci" PREFIX ""
    OUTPUT_NAME libtracistatic${BINARY_SUFFIX}
    OUTPUT_NAME_DEBUG libtracistatic${BINARY_SUFFIX}D)

if (WIN32)
    set(libtcpip foreign_tcpip ws2_32)
else()
    set(libtcpip foreign_tcpip)
endif()

add_library(libtracicpp SHARED ${libtracistatic_STAT_SRCS})
set_target_properties(libtracicpp PROPERTIES FOLDER "libtraci" PREFIX ""
    OUTPUT_NAME libtracicpp${BINARY_SUFFIX}
    OUTPUT_NAME_DEBUG libtracicpp${BINARY_SUFFIX}D)
target_include_directories(libtracicpp PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>)
install(TARGETS libtracicpp EXPORT SUMOConfig
    DESTINATION bin
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
export(TARGETS libtracicpp NAMESPACE SUMO:: APPEND FILE "${CMAKE_BINARY_DIR}/SUMOConfig.cmake")

if ((GNU_COMPILER OR CLANG_COMPILER) AND NOT APPLE)
    target_link_libraries(libtracicpp PRIVATE -Wl,--whole-archive ${libtcpip} -Wl,--no-whole-archive)
else ()
    target_link_libraries(libtracicpp PRIVATE ${libtcpip})
endif()

if (SWIG_FOUND)
    include(${SWIG_USE_FILE})
    set_source_files_properties(libtraci.i PROPERTIES CPLUSPLUS ON)
    file(GLOB libtraci_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/../libsumo/*.h")
    if (ENABLE_JAVA_BINDINGS)
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
            if ((GNU_COMPILER OR CLANG_COMPILER) AND NOT APPLE)
                set_source_files_properties(${swig_generated_file_fullname} PROPERTIES COMPILE_FLAGS "-Wno-strict-aliasing")
                swig_link_libraries(libtracijni -Wl,--whole-archive libtracistatic ${libtcpip} -Wl,--no-whole-archive)
            else()
                swig_link_libraries(libtracijni libtracistatic ${libtcpip})
            endif()
            if (MVN_EXECUTABLE)
                ADD_CUSTOM_COMMAND(TARGET libtracijni POST_BUILD
                    COMMAND ${PYTHON_EXECUTABLE} ${SUMO_TOOLS_DIR}/traci/rebuildConstants.py -o java/Constants.java -j org.eclipse.sumo.libtraci.Constants
                    COMMAND ${CMAKE_COMMAND} -E make_directory src/main/java/org/eclipse/sumo/libtraci
                    COMMAND ${CMAKE_COMMAND} -E copy_directory java src/main/java/org/eclipse/sumo/libtraci
                    COMMAND ${PYTHON_EXECUTABLE} ${SUMO_TOOLS_DIR}/build_config/pom.py libtraci
                    COMMAND ${MVN_COMMAND_PREFIX} ${MVN_EXECUTABLE} ${MVN_REPO} ${MVN_OPTS} --batch-mode package source:jar
                )
            else()
                message(WARNING "Could not build libtraci jar, maven was not found.")
            endif()
            set_target_properties(libtracijni PROPERTIES FOLDER "libtraci")
            install(TARGETS libtracijni DESTINATION bin LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
        else()
            message(WARNING "ENABLE_JAVA_BINDINGS is set but Java or JNI was not found.")
        endif()
    endif()

    if (ENABLE_CS_BINDINGS)
        set(CS_NAMESPACE Eclipse.Sumo.Libtraci)
        set(CMAKE_SWIG_FLAGS -namespace ${CS_NAMESPACE})
        set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/${CS_NAMESPACE})
        set(SWIG_MODULE_libtracics_EXTRA_DEPS ${libtraci_HEADERS} ../libsumo/libsumo_typemap.i)
        if (${CMAKE_VERSION} VERSION_GREATER 3.8.0)
            SWIG_ADD_LIBRARY(libtracics LANGUAGE CSharp SOURCES libtraci.i)
        else()
            SWIG_ADD_MODULE(libtracics CSharp libtraci.i)
        endif()
        if ((GNU_COMPILER OR CLANG_COMPILER) AND NOT APPLE)
            set_source_files_properties(${swig_generated_file_fullname} PROPERTIES COMPILE_FLAGS "-Wno-strict-aliasing")
            swig_link_libraries(libtracics -Wl,--whole-archive libtracistatic ${libtcpip} -Wl,--no-whole-archive)
        else()
            swig_link_libraries(libtracics libtracistatic ${libtcpip})
        endif()
        ADD_CUSTOM_COMMAND(TARGET libtracics POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E tar "cf" "${CMAKE_SOURCE_DIR}/bin/libtracics-sources.zip" --format=zip "${CS_NAMESPACE}"
        )
        set_target_properties(libtracics PROPERTIES FOLDER "libtraci" PREFIX "")
        install(TARGETS libtracics DESTINATION bin LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
    endif()

    if (ENABLE_PYTHON_BINDINGS AND PYTHON_INCLUDE_DIRS)
        include_directories(${PYTHON_INCLUDE_DIRS})
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
        if ((GNU_COMPILER OR CLANG_COMPILER) AND NOT APPLE)
            swig_link_libraries(libtraci -Wl,--whole-archive libtracistatic ${libtcpip} -Wl,--no-whole-archive)
        else()
            set_target_properties(${SWIG_MODULE_libtraci_REAL_NAME} PROPERTIES FOLDER "libtraci")
            if (SKBUILD OR ${CMAKE_VERSION} VERSION_LESS 3.14.0)
                swig_link_libraries(libtraci libtracistatic ${libtcpip} ${PYTHON_LIBRARIES})
            else()
                # using target fixes segfault in macos, swig_link_libraries is also deprecated
                target_link_libraries(libtraci libtracistatic ${libtcpip} Python::Module)
            endif()
        endif()
        if (WIN32)
            swig_link_libraries(libtraci ${PYTHON_LIBRARIES})
        endif()
        if (NOT ${PYTHON_SETUPTOOLS_MISSING})
            if (${PYTHON_BUILD_MISSING})
                install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${SUMO_TOOLS_DIR}/build_config/setup-libtraci.py clean --all install --root=\$ENV{DESTDIR}/ --prefix=${CMAKE_INSTALL_PREFIX} --optimize=1)"
                        COMPONENT pylibtraci)
            else ()
                install(CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ./build_config/version.py ./build_config/setup-libtraci.py ./setup.py COMMAND ${PYTHON_EXECUTABLE} -m pip install --root=\$ENV{DESTDIR}/ --prefix=${CMAKE_INSTALL_PREFIX} -f ${CMAKE_BINARY_DIR} . WORKING_DIRECTORY ${SUMO_TOOLS_DIR})"
                        COMPONENT pylibtraci)
            endif()
        endif()
    endif()

endif()
