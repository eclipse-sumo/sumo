add_executable(testthreadpool
        ThreadPoolTest.cpp
        )
add_dependencies(testthreadpool install_test_dll)
set_property(TARGET testthreadpool PROPERTY FOLDER "test_exe")
add_test(testthreadpool ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/testthreadpool)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_link_libraries(testthreadpool pthread)
endif ()
