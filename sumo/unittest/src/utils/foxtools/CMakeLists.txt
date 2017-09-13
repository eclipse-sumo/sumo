add_executable(testfoxtools
        FXWorkerThreadTest.cpp
        )

if (NOT APPLE)
    target_link_libraries(testfoxtools ${FOX_LIBRARY} -Wl,--start-group ${commonlibs} -Wl,--end-group ${GTEST_BOTH_LIBRARIES} pthread)
else ()
    target_link_libraries(testfoxtools ${FOX_LIBRARY} ${commonlibs} ${GTEST_BOTH_LIBRARIES} pthread)
endif ()