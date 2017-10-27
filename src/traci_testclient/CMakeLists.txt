set(TraCITestClient_SRCS
   tracitestclient_main.cpp
   TraCITestClient.cpp
   TraCITestClient.h
)

add_executable(TraCITestClient ${TraCITestClient_SRCS})

target_link_libraries(TraCITestClient utils_traci ${commonlibs})

install(TARGETS TraCITestClient DESTINATION bin)
