add_executable(testmicrosim
        MSEventControlTest.cpp
        MSCFModelTest.cpp
        )
set_target_properties(testmicrosim PROPERTIES OUTPUT_NAME_DEBUG testmicrosimD)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_link_libraries(testmicrosim -Wl,--start-group microsim microsim_actions microsim_devices microsim_cfmodels microsim_lcmodels microsim_pedestrians microsim_trigger microsim_traffic_lights mesosim traciserver libsumostatic netload microsim_output mesosim ${commonvehiclelibs} -Wl,--end-group ${GTEST_BOTH_LIBRARIES} ${GRPC_LIBS})
else ()
    target_link_libraries(testmicrosim microsim microsim_actions microsim_devices microsim_cfmodels microsim_lcmodels microsim_pedestrians microsim_trigger microsim_traffic_lights mesosim traciserver libsumostatic netload microsim_output mesosim ${commonvehiclelibs} ${GTEST_BOTH_LIBRARIES} ${GRPC_LIBS})
endif ()