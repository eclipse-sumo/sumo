message("entering lib")

set(tracilib_files
        TraCI.cpp TraCI.h
        TraCI_POI.cpp TraCI_POI.h
        TraCI_Polygon.cpp TraCI_Polygon.h
        TraCI_Junction.cpp TraCI_Junction.h
        TraCI_LaneAreaDetector.cpp TraCI_LaneAreaDetector.h
        TraCI_Vehicle.cpp TraCI_Vehicle.h)

add_library(tracilib STATIC ${tracilib_files})
