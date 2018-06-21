set(libsumostatic_STAT_SRCS
        Helper.cpp
        Helper.h
        InductionLoop.cpp
        InductionLoop.h
        Junction.cpp
        Junction.h
        Lane.cpp
        Lane.h
        LaneArea.cpp
        LaneArea.h
        MultiEntryExit.h
        MultiEntryExit.cpp
        Person.cpp
        Person.h
        POI.cpp
        POI.h
        Polygon.h
        Polygon.cpp
        Route.h
        Route.cpp
        Simulation.cpp
        Simulation.h
        Subscription.h
        TraCIDefs.h
        TrafficLight.cpp
        TrafficLight.h
        Vehicle.h
        Vehicle.cpp
        VehicleType.cpp
        VehicleType.h
        Edge.cpp Edge.h)

add_library(libsumostatic STATIC ${libsumostatic_STAT_SRCS})
set_property(TARGET libsumostatic PROPERTY PROJECT_LABEL "z_libsumostatic")

set(ENABLE_PYTHON_BINDINGS ON CACHE BOOL "Build Python Bindings")
set(ENABLE_JAVA_BINDINGS ON CACHE BOOL "Build Java Bindings")

if(SWIG_FOUND)
    include(${SWIG_USE_FILE})
    set(CMAKE_SWIG_FLAGS "")
    set_source_files_properties(libsumo.i PROPERTIES CPLUSPLUS ON)

    find_package(PythonLibs)
    if(ENABLE_PYTHON_BINDINGS)
        if(PYTHONLIBS_FOUND)
            include_directories(${PYTHON_INCLUDE_PATH})
            set(CMAKE_SWIG_OUTDIR ${CMAKE_SOURCE_DIR}/tools/libsumo)
            set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/tools/libsumo)
			set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/tools/libsumo)
			set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/tools/libsumo)
            if (${CMAKE_VERSION} VERSION_GREATER 3.8.0)
                SWIG_ADD_LIBRARY(libsumo LANGUAGE python SOURCES libsumo.i)
            else()
                SWIG_ADD_MODULE(libsumo python libsumo.i)
            endif()
            if (MSVC)
                # disable python module for the debug build because virtually no one has a python debug dll to link against
                set_property(TARGET ${SWIG_MODULE_libsumo_REAL_NAME} PROPERTY EXCLUDE_FROM_DEFAULT_BUILD_DEBUG TRUE)
                swig_link_libraries(libsumo ${sumolibs} ${PYTHON_LIBRARIES})
            else()
                swig_link_libraries(libsumo -Wl,--whole-archive ${sumolibs} -Wl,--no-whole-archive ${PYTHON_LIBRARIES})
            endif()
            configure_file(setup.py.in ${CMAKE_SWIG_OUTDIR}/setup.py)
            install(
                CODE "execute_process(COMMAND ${PYTHON_EXECUTABLE} ${SETUP_PY} install WORKING_DIRECTORY ${CMAKE_SWIG_OUTDIR})"
                COMPONENT pylibsumo)
        else()
            message(WARNING "ENABLE_PYTHON_BINDINGS is set but python libraries were not found.")
        endif()
    endif()

    if(ENABLE_JAVA_BINDINGS)
        find_package(Java)
        find_package(JNI)
        if(JAVA_FOUND AND JNI_FOUND)
            include(UseJava)
            include_directories(${JNI_INCLUDE_DIRS})
            set(CMAKE_SWIG_FLAGS -package org.eclipse.sumo.libsumo)
            set(CMAKE_SWIG_OUTDIR ${CMAKE_SOURCE_DIR}/tools/libsumo/java/org/eclipse/sumo/libsumo)
            set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/tools/libsumo/java)
			set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/tools/libsumo/java)
			set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/tools/libsumo/java)
            if (${CMAKE_VERSION} VERSION_GREATER 3.8.0)
                SWIG_ADD_LIBRARY(libsumo LANGUAGE java SOURCES libsumo.i)
            else()
                SWIG_ADD_MODULE(libsumo java libsumo.i)
            endif()
            if (MSVC)
                swig_link_libraries(libsumo ${sumolibs})
            else()
                set_source_files_properties(${swig_generated_file_fullname} PROPERTIES COMPILE_FLAGS "-Wno-strict-aliasing")
                swig_link_libraries(libsumo -Wl,--whole-archive ${sumolibs} -Wl,--no-whole-archive)
            endif()
    #       add_jar(libsumojni ${swig_extra_generated_files})
        else()
            message(WARNING "ENABLE_JAVA_BINDINGS is set but Java or JNI was not found.")
        endif()
    endif()
endif()
