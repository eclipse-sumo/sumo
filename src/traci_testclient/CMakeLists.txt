set(TraCITestClient_SRCS
   tracitestclient_main.cpp
   TraCITestClient.cpp
   TraCITestClient.h
)

add_executable(TraCITestClient ${TraCITestClient_SRCS})
set_target_properties(TraCITestClient PROPERTIES OUTPUT_NAME_DEBUG TraCITestClientD)
target_link_libraries(TraCITestClient utils_traci ${commonlibs})

add_executable(testlibsumo testlibsumo_main.cpp
    ../microsim/MSNet.cpp ../microsim/MSMoveReminder.cpp
    ../microsim/output/MSMeanData_Net.cpp ../microsim/output/MSRouteProbe.cpp
    ../microsim/output/MSInductLoop.cpp ../microsim/output/MSE2Collector.cpp ../microsim/output/MSE3Collector.cpp
    ../netload/NLBuilder.cpp)
set_target_properties(testlibsumo PROPERTIES OUTPUT_NAME_DEBUG testlibsumoD)
target_link_libraries(testlibsumo ${sumolibs})
