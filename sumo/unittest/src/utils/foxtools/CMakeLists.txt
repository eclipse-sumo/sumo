add_executable(testfoxtools
   FXWorkerThreadTest.cpp
)
target_link_libraries(testfoxtools ${FOX_LIBRARY} -Wl,--start-group ${commonlibs} -Wl,--end-group ${GTEST_BOTH_LIBRARIES} pthread)
