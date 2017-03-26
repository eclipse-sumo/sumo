set(utils_geom_STAT_SRCS
   AbstractPoly.h
   Boundary.cpp
   Boundary.h
   Bresenham.cpp
   Bresenham.h
   GeomConvHelper.cpp
   GeomConvHelper.h
   GeoConvHelper.cpp
   GeoConvHelper.h
   GeomHelper.cpp
   GeomHelper.h
   Helper_ConvexHull.cpp
   Helper_ConvexHull.h
   Position.cpp
   Position.h
   PositionVector.cpp
   PositionVector.h
   bezier.cpp
   bezier.h
)

add_library(utils_geom STATIC ${utils_geom_STAT_SRCS})
