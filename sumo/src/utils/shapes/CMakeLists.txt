set(utils_shapes_STAT_SRCS
   PointOfInterest.h
   Shape.cpp
   Shape.h
   Polygon.cpp
   Polygon.h
   ShapeHandler.cpp
   ShapeHandler.h
   ShapeContainer.cpp
   ShapeContainer.h
)

add_library(utils_shapes STATIC ${utils_shapes_STAT_SRCS})
