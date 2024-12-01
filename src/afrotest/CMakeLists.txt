set(afrotest_SRCS
   afrotest_main.cpp
   "${CMAKE_CURRENT_SOURCE_DIR}/../duarouter/RODUAEdgeBuilder.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/../duarouter/RODUAEdgeBuilder.h"
   "${CMAKE_CURRENT_SOURCE_DIR}/../duarouter/RODUAFrame.cpp"
   "${CMAKE_CURRENT_SOURCE_DIR}/../duarouter/RODUAFrame.h"
   AFROTest.cpp
   AFROTest.h
)

add_executable(afrotest ${afrotest_SRCS})
set_target_properties(afrotest PROPERTIES OUTPUT_NAME afrotest${BINARY_SUFFIX})
set_target_properties(afrotest PROPERTIES OUTPUT_NAME_DEBUG afrotest${BINARY_SUFFIX}D)
target_link_libraries(afrotest router ${commonvehiclelibs} ${TCMALLOC_LIBRARY})
add_dependencies(afrotest generate-version-h install_dll)

install(TARGETS afrotest RUNTIME DESTINATION bin)
