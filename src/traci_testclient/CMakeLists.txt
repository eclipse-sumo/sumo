set(TraCITestClient_SRCS
   tracitestclient_main.cpp
   TraCITestClient.cpp
   TraCITestClient.h
)

add_executable(TraCITestClient ${TraCITestClient_SRCS})
set_target_properties(TraCITestClient PROPERTIES OUTPUT_NAME TraCITestClient${BINARY_SUFFIX})
set_target_properties(TraCITestClient PROPERTIES OUTPUT_NAME_DEBUG TraCITestClient${BINARY_SUFFIX}D)
target_link_libraries(TraCITestClient utils_traci foreign_tcpip)
if (WIN32)
    target_link_libraries(TraCITestClient ws2_32)
endif ()

add_executable(testlibsumo testlibsumo_main.cpp)
set_target_properties(testlibsumo PROPERTIES OUTPUT_NAME testlibsumo${BINARY_SUFFIX})
set_target_properties(testlibsumo PROPERTIES OUTPUT_NAME_DEBUG testlibsumo${BINARY_SUFFIX}D)
set_property(TARGET testlibsumo PROPERTY FOLDER "test_exe")
target_link_libraries(testlibsumo libsumocpp ${XercesC_LIBRARIES} ${ZLIB_LIBRARIES} ${PROJ_LIBRARY} ${TCMALLOC_LIBRARY})
add_test(NAME libsumotest COMMAND $<TARGET_FILE:testlibsumo>)

add_executable(testlibsumostatic testlibsumo_main.cpp)
set_target_properties(testlibsumostatic PROPERTIES OUTPUT_NAME testlibsumostatic${BINARY_SUFFIX})
set_target_properties(testlibsumostatic PROPERTIES OUTPUT_NAME_DEBUG testlibsumostatic${BINARY_SUFFIX}D)
set_property(TARGET testlibsumostatic PROPERTY FOLDER "test_exe")
if (FOX_FOUND)
    target_link_libraries(testlibsumostatic libsumoguistatic ${sumoguilibs} traciserver libsumostatic microsim_traffic_lights microsim_lcmodels microsim_cfmodels microsim_devices microsim_engine microsim_output microsim_transportables)
else ()
    target_link_libraries(testlibsumostatic libsumostatic ${sumolibs} traciserver libsumostatic microsim_traffic_lights microsim_lcmodels microsim_cfmodels microsim_devices microsim_engine microsim_output microsim_transportables)
endif()
add_test(NAME libsumostatictest COMMAND $<TARGET_FILE:testlibsumostatic>)

add_executable(testlibtraci testlibtraci_main.cpp)
set_target_properties(testlibtraci PROPERTIES OUTPUT_NAME testlibtraci${BINARY_SUFFIX})
set_target_properties(testlibtraci PROPERTIES OUTPUT_NAME_DEBUG testlibtraci${BINARY_SUFFIX}D)
set_property(TARGET testlibtraci PROPERTY FOLDER "test_exe")
target_link_libraries(testlibtraci libtracicpp)
add_test(NAME libtracitest COMMAND $<TARGET_FILE:testlibtraci> $<TARGET_FILE:sumo> -c ${CMAKE_SOURCE_DIR}/docs/examples/sumo/hokkaido/test.sumocfg)
