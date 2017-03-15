message("entering lib")

set(tracilib_files
        TraCI.cpp TraCI.h
        TraCI_POI.cpp TraCI_POI.h)

add_library(tracilib STATIC ${tracilib_files})
