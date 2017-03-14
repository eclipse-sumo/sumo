message("entering eulerspiral")

set(eulerspiral_files
        euler.cpp euler.h
        BiArc.cpp BiArc.h
        angles.h)

add_library(foreigneulerspiral STATIC ${eulerspiral_files})