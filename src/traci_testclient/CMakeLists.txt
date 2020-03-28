set(TraCITestClient_SRCS
   tracitestclient_main.cpp
   TraCITestClient.cpp
   TraCITestClient.h
)

add_executable(TraCITestClient ${TraCITestClient_SRCS})
set_target_properties(TraCITestClient PROPERTIES OUTPUT_NAME TraCITestClient${BINARY_SUFFIX})
set_target_properties(TraCITestClient PROPERTIES OUTPUT_NAME_DEBUG TraCITestClient${BINARY_SUFFIX}D)
target_link_libraries(TraCITestClient utils_traci ${commonlibs})

add_executable(testlibsumo testlibsumo_main.cpp)
set_property(TARGET testlibsumo PROPERTY FOLDER "test_exe")
set_target_properties(testlibsumo PROPERTIES OUTPUT_NAME testlibsumo${BINARY_SUFFIX})
set_target_properties(testlibsumo PROPERTIES OUTPUT_NAME_DEBUG testlibsumo${BINARY_SUFFIX}D)
target_link_libraries(testlibsumo libsumoc)
add_test(libsumotest ${CMAKE_SOURCE_DIR}/bin/testlibsumo)

add_executable(testlibsumostatic testlibsumo_main.cpp)
set_property(TARGET testlibsumostatic PROPERTY FOLDER "test_exe")
set_target_properties(testlibsumostatic PROPERTIES OUTPUT_NAME testlibsumostatic${BINARY_SUFFIX})
set_target_properties(testlibsumostatic PROPERTIES OUTPUT_NAME_DEBUG testlibsumostatic${BINARY_SUFFIX}D)
target_link_libraries(testlibsumostatic libsumostatic ${sumolibs} traciserver libsumostatic microsim_cfmodels microsim_devices microsim_engine microsim_output microsim_transportables)
add_test(libsumostatictest ${CMAKE_SOURCE_DIR}/bin/testlibsumostatic)
