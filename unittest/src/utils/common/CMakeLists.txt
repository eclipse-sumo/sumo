add_executable(testcommon
        StringTokenizerTest.cpp
        StringUtilsTest.cpp
        TplConvertTest.cpp
        RGBColorTest.cpp
        ValueTimeLineTest.cpp
        )
set_target_properties(testcommon PROPERTIES OUTPUT_NAME_DEBUG testcommonD)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_link_libraries(testcommon -Wl,--start-group ${commonlibs} -Wl,--end-group ${GTEST_BOTH_LIBRARIES})
else ()
    target_link_libraries(testcommon ${commonlibs} ${GTEST_BOTH_LIBRARIES})
endif ()