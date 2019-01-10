add_executable(testcommon
        StringTokenizerTest.cpp
        FileHelpersTest.cpp
        StringUtilsTest.cpp
        TplConvertTest.cpp
        RGBColorTest.cpp
        ValueTimeLineTest.cpp
        )
add_test(NAME testcommon COMMAND $<TARGET_FILE:testcommon>)
set_target_properties(testcommon PROPERTIES OUTPUT_NAME_DEBUG testcommonD)

if (MSVC)
    target_link_libraries(testcommon ${commonlibs} ${GTEST_BOTH_LIBRARIES})
else ()
    target_link_libraries(testcommon -Wl,--start-group ${commonlibs} -Wl,--end-group ${GTEST_BOTH_LIBRARIES})
endif ()
