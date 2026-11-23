add_executable(testthreadpool ThreadPoolTest.cpp)
set_property(TARGET testthreadpool PROPERTY FOLDER "test_exe")
add_test(NAME testthreadpool COMMAND testthreadpool)
set_tests_properties(testthreadpool PROPERTIES TIMEOUT 60)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_link_libraries(testthreadpool pthread)
endif ()
