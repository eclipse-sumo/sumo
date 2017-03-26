set(foreign_eulerspiral_STAT_SRCS
   angles.h
   BiArc.cpp
   BiArc.h
   euler.cpp
   euler.h
   points.h
)

add_library(foreign_eulerspiral STATIC ${foreign_eulerspiral_STAT_SRCS})
