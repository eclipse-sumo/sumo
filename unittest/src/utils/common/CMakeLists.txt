add_executable(testcommon
        StringTokenizerTest.cpp
        StringUtilsTest.cpp
        TplConvertTest.cpp
        RGBColorTest.cpp
        ValueTimeLineTest.cpp
        )

if (NOT APPLE)
    target_link_libraries(testcommon -Wl,--start-group ${commonlibs} -Wl,--end-group ${GTEST_BOTH_LIBRARIES} pthread)
else ()
    target_link_libraries(testcommon ${commonlibs} ${GTEST_BOTH_LIBRARIES} pthread)
endif ()