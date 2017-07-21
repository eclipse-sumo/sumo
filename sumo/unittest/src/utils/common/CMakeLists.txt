add_executable(testcommon
   StringTokenizerTest.cpp
   StringUtilsTest.cpp
   TplConvertTest.cpp
   RGBColorTest.cpp
   ValueTimeLineTest.cpp
)
target_link_libraries(testcommon -Wl,--start-group ${commonlibs} -Wl,--end-group ${GTEST_BOTH_LIBRARIES} pthread)
