add_executable(testfoxtools
        FXWorkerThreadTest.cpp
        )
set_target_properties(testfoxtools PROPERTIES OUTPUT_NAME_DEBUG testfoxtoolsD)

if (NOT APPLE)
    target_link_libraries(testfoxtools ${FOX_LIBRARY} -Wl,--start-group ${commonlibs} -Wl,--end-group ${GTEST_BOTH_LIBRARIES})
else ()
    target_link_libraries(testfoxtools ${FOX_LIBRARY} ${commonlibs} ${GTEST_BOTH_LIBRARIES} pthread)
endif ()