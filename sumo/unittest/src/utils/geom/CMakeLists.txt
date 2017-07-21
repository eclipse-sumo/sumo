SET(testgeom_STAT_SRCS
   BoundaryTest.cpp
   GeoConvHelperTest.cpp
   PositionVectorTest.cpp
   GeomHelperTest.cpp
)

add_library(testgeom STATIC ${testgeom_STAT_SRCS})
