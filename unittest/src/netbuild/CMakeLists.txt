add_executable(testnetbuild NBHeightMapperTest.cpp
        NBTrafficLightLogicTest.cpp)
set_target_properties(testnetbuild PROPERTIES OUTPUT_NAME_DEBUG testnetbuildD)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_link_libraries(testnetbuild netbuild ${GDAL_LIBRARY} -Wl,--start-group ${commonlibs} -Wl,--end-group ${GTEST_BOTH_LIBRARIES})
else ()
    target_link_libraries(testnetbuild netbuild ${GDAL_LIBRARY} ${commonlibs} ${GTEST_BOTH_LIBRARIES})
endif ()

