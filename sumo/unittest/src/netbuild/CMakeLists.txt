add_executable(testnetbuild NBHeightMapperTest.cpp
        NBTrafficLightLogicTest.cpp)

if (NOT APPLE)
    target_link_libraries(testnetbuild netbuild ${GDAL_LIBRARY} -Wl,--start-group ${commonlibs} -Wl,--end-group ${GTEST_BOTH_LIBRARIES} pthread)
else ()
    target_link_libraries(testnetbuild netbuild ${GDAL_LIBRARY} ${commonlibs} ${GTEST_BOTH_LIBRARIES} pthread)
endif ()

