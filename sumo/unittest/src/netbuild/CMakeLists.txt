add_executable(testnetbuild NBHeightMapperTest.cpp
   NBTrafficLightLogicTest.cpp)
target_link_libraries(testnetbuild netbuild ${GDAL_LIBRARY} ${commonlibs} ${GTEST_BOTH_LIBRARIES} pthread)
