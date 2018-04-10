if(SWIG_FOUND)
    include(${SWIG_USE_FILE})
    set(CMAKE_SWIG_FLAGS "")
    set_source_files_properties(libsumo.i PROPERTIES CPLUSPLUS ON)

    find_package(PythonLibs)
    if(PYTHONLIBS_FOUND)
        include_directories(${PYTHON_INCLUDE_PATH})
        set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/python)
        swig_add_module(libsumo python libsumo.i)
        if (MSVC)
            swig_link_libraries(libsumo ${sumolibs} ${PYTHON_LIBRARIES})
        else()
            swig_link_libraries(libsumo -Wl,--whole-archive ${sumolibs} -Wl,--no-whole-archive ${PYTHON_LIBRARIES})
        endif()
    endif()

    find_package(Java)
    find_package(JNI)
    if(JAVA_FOUND AND JNI_FOUND)
        include(UseJava)
        include_directories(${JNI_INCLUDE_DIRS})
        set(CMAKE_SWIG_FLAGS -package de.dlr.ts.routingkernel.sumowrapper)
        set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/java)
        swig_add_module(libsumo java libsumo.i)
        if (MSVC)
            swig_link_libraries(libsumo ${sumolibs})
        else()
            swig_link_libraries(libsumo -Wl,--whole-archive ${sumolibs} -Wl,--no-whole-archive)
        endif()
    endif()
endif()

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
