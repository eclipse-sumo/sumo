add_executable(testnetbuild NBHeightMapperTest.cpp  NBTrafficLightLogicTest.cpp)
setTestProperties(testnetbuild netbuild ${GDAL_LIBRARY})
