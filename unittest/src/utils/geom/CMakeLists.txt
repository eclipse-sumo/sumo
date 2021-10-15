add_executable(testgeom
        BoundaryTest.cpp
        GeoConvHelperTest.cpp
        PositionVectorTest.cpp
        GeomHelperTest.cpp
        )
setTestProperties(testgeom utils_geom)
