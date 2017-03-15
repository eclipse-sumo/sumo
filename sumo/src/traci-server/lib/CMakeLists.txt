message("entering lib")

set(tracilib_files
        TraCI.cpp TraCI.h
        TraCI_POI.cpp TraCI_POI.h TraCI_Polygon.cpp TraCI_Polygon.h)

add_library(tracilib STATIC ${tracilib_files})
