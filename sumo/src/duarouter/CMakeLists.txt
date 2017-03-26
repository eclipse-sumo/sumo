set(duarouter_SRCS
   duarouter_main.cpp
   RODUAEdgeBuilder.cpp
   RODUAEdgeBuilder.h
   RODUAFrame.cpp
   RODUAFrame.h
)

add_executable(duarouter ${duarouter_SRCS})

target_link_libraries(duarouter router ${commonvehiclelibs})

install(TARGETS duarouter DESTINATION bin)
