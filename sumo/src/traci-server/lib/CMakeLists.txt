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
