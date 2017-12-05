if(SWIG_FOUND)
    include(${SWIG_USE_FILE})
    set(CMAKE_SWIG_FLAGS "")
    set_source_files_properties(libsumo.i PROPERTIES CPLUSPLUS ON)

    find_package(PythonLibs)
    if(PYTHONLIBS_FOUND)
        include_directories(${PYTHON_INCLUDE_PATH})
        set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/python)
        swig_add_module(libsumo python libsumo.i)
        swig_link_libraries(libsumo -Wl,--whole-archive ${sumolibs} -Wl,--no-whole-archive ${PYTHON_LIBRARIES})
    endif()

    find_package(Java)
    find_package(JNI)
    if(JAVA_FOUND AND JNI_FOUND)
        include(UseJava)
        include_directories(${JNI_INCLUDE_DIRS})
        set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/java)
        swig_add_module(swig_traci_java java libsumo.i)
        swig_link_libraries(swig_traci_java -Wl,--whole-archive ${sumolibs} -Wl,--no-whole-archive)
    endif()
endif()

set(traciserver_lib_STAT_SRCS
        TraCI.cpp
        TraCI.h
        TraCI_InductionLoop.cpp
        TraCI_InductionLoop.h
        TraCI_Junction.cpp
        TraCI_Junction.h
        TraCI_Lane.cpp
        TraCI_Lane.h
        TraCI_LaneArea.cpp
        TraCI_LaneArea.h
        TraCI_MultiEntryExit.h
        TraCI_MultiEntryExit.cpp
        TraCI_Person.cpp
        TraCI_Person.h
        TraCI_POI.cpp
        TraCI_POI.h
        TraCI_Polygon.h
        TraCI_Polygon.cpp
        TraCI_Route.h
        TraCI_Route.cpp
        TraCI_Simulation.cpp
        TraCI_Simulation.h
        TraCI_TrafficLight.cpp
        TraCI_TrafficLight.h
        TraCI_Vehicle.h
        TraCI_Vehicle.cpp
        TraCI_VehicleType.cpp
        TraCI_VehicleType.h
        TraCI_Edge.cpp TraCI_Edge.h)

add_library(traciserver_lib STATIC ${traciserver_lib_STAT_SRCS})
set_property(TARGET traciserver_lib PROPERTY PROJECT_LABEL "z_traciserver_lib")
