add_executable(testgeom
        BoundaryTest.cpp
        GeoConvHelperTest.cpp
        PositionVectorTest.cpp
        GeomHelperTest.cpp
        )
set_target_properties(testgeom PROPERTIES OUTPUT_NAME_DEBUG testgeomD)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_link_libraries(testgeom -Wl,--start-group ${commonlibs} -Wl,--end-group ${GTEST_BOTH_LIBRARIES})
else ()
    target_link_libraries(testgeom ${commonlibs} ${GTEST_BOTH_LIBRARIES})
endif ()