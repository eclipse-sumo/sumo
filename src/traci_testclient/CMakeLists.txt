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
set_target_properties(testlibsumo PROPERTIES OUTPUT_NAME testlibsumo${BINARY_SUFFIX})
set_target_properties(testlibsumo PROPERTIES OUTPUT_NAME_DEBUG testlibsumo${BINARY_SUFFIX}D)
target_link_libraries(testlibsumo ${sumolibs} traciserver microsim_cfmodels microsim_devices microsim_lcmodels microsim_output)
