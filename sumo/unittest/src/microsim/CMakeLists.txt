add_executable(testmicrosim
   MSEventControlTest.cpp
   MSCFModelTest.cpp
)
target_link_libraries(testmicrosim -Wl,--start-group microsim microsim_devices -Wl,--end-group microsim_cfmodels microsim_lcmodels microsim_pedestrians microsim_trigger microsim_traffic_lights mesosim traciserver traciserver_lib microsim_output mesosim -Wl,--start-group ${commonvehiclelibs} -Wl,--end-group ${GTEST_BOTH_LIBRARIES} pthread)
