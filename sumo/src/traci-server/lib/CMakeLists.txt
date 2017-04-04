set(traciserver_lib_STAT_SRCS
        TraCI.cpp
        TraCI.h
        TraCI_Junction.cpp
        TraCI_Junction.h
        TraCI_LaneAreaDetector.cpp
        TraCI_LaneAreaDetector.h
        TraCI_POI.cpp
        TraCI_POI.h
        TraCI_POI.cpp
        TraCI_Junction.h
        TraCI_Junction.cpp
        TraCI_Vehicle.h
        TraCI_Vehicle.cpp
        TraCI_LaneAreaDetector.h
        TraCI_LaneAreaDetector.cpp
        TraCI_Polygon.h
        TraCI_Polygon.cpp
        TraCI_Route.h
        TraCI_Route.cpp
        TraCI_VehicleType.cpp
        TraCI_VehicleType.h
        TraCI_InductionLoop.cpp
        TraCI_InductionLoop.h
        )

add_library(traciserver_lib STATIC ${traciserver_lib_STAT_SRCS})
