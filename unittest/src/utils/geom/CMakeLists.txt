add_executable(testgeom
        BoundaryTest.cpp
        GeoConvHelperTest.cpp
        PositionVectorTest.cpp
        GeomHelperTest.cpp
        )

if (NOT APPLE)
    target_link_libraries(testgeom -Wl,--start-group ${commonlibs} -Wl,--end-group ${GTEST_BOTH_LIBRARIES} pthread)
else ()
    target_link_libraries(testgeom ${commonlibs} ${GTEST_BOTH_LIBRARIES} pthread)
endif ()