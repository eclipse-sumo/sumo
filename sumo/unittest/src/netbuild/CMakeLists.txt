add_executable(testnetbuild NBHeightMapperTest.cpp
   NBTrafficLightLogicTest.cpp)
target_link_libraries(testnetbuild netbuild ${GDAL_LIBRARY} -Wl,--start-group ${commonlibs} -Wl,--end-group ${GTEST_BOTH_LIBRARIES} pthread)
