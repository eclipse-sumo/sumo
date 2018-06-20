set(TraCITestClient_SRCS
   tracitestclient_main.cpp
   TraCITestClient.cpp
   TraCITestClient.h
)

add_executable(TraCITestClient ${TraCITestClient_SRCS})
set_target_properties(TraCITestClient PROPERTIES OUTPUT_NAME_DEBUG TraCITestClientD)
target_link_libraries(TraCITestClient utils_traci ${commonlibs})

add_executable(testlibsumo testlibsumo_main.cpp
        microsim/MSMoveReminder.cpp microsim/MSMoveReminder.h)
set_target_properties(testlibsumo PROPERTIES OUTPUT_NAME_DEBUG testlibsumoD)
target_link_libraries(testlibsumo ${sumolibs})
