add_executable(testmicrosim
        MSEventControlTest.cpp
        MSCFModelTest.cpp
        MSCFModel_IDMTest.cpp
        )
setTestProperties(testmicrosim microsim microsim_devices microsim_cfmodels microsim_lcmodels microsim_transportables mesosim traciserver libsumostatic netload microsim microsim_actions microsim_trigger microsim_traffic_lights microsim_output microsim_engine mesosim ${commonvehiclelibs})
