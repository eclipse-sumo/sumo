add_executable(testfoxtools
        FXWorkerThreadTest.cpp
        )
set_property(TARGET testfoxtools PROPERTY FOLDER "test_exe")
add_test(NAME testfoxtools COMMAND $<TARGET_FILE:testfoxtools>)
set_target_properties(testfoxtools PROPERTIES OUTPUT_NAME_DEBUG testfoxtoolsD)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_link_libraries(testfoxtools ${FOX_LIBRARY} -Wl,--start-group ${commonlibs} -Wl,--end-group ${GTEST_BOTH_LIBRARIES} -lpthread -lm)
else ()
    target_link_libraries(testfoxtools ${FOX_LIBRARY} ${commonlibs} ${GTEST_BOTH_LIBRARIES})
endif ()
