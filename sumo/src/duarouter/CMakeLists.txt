set(duarouter_SRCS
   duarouter_main.cpp
   RODUAEdgeBuilder.cpp
   RODUAEdgeBuilder.h
   RODUAFrame.cpp
   RODUAFrame.h
)

add_executable(duarouter ${duarouter_SRCS})
set_target_properties(duarouter PROPERTIES OUTPUT_NAME_DEBUG duarouterD)
target_link_libraries(duarouter router ${commonvehiclelibs})
