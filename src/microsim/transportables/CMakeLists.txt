if (HAVE_JUPEDSIM)
    set(microsim_transportables_JPS_SRCS
        MSPModel_JuPedSim.cpp
        MSPModel_JuPedSim.h
        )

    if (gRPC_FOUND)
        get_filename_component(jps_proto "./jupedsim.proto" ABSOLUTE)
        get_filename_component(jps_proto_path "${jps_proto}" PATH)
        set(jps_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/jupedsim.pb.cc")
        set(jps_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/jupedsim.pb.h")
        set(jps_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/jupedsim.grpc.pb.cc")
        set(jps_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/jupedsim.grpc.pb.h")
        # TODO this should probably be replaced by protobuf_generate_cpp
        add_custom_command(
            OUTPUT "${jps_proto_srcs}" "${jps_proto_hdrs}" "${jps_grpc_srcs}" "${jps_grpc_hdrs}"
            COMMAND ${_PROTOBUF_PROTOC}
            ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
                --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
                  -I "${jps_proto_path}"
                --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
                "${jps_proto}"
            DEPENDS "${jps_proto}")
        include_directories("${CMAKE_CURRENT_BINARY_DIR}")
        set(microsim_transportables_JPS_GRPC_SRCS
            MSPModel_JuPedSim_gRPC.cpp MSPModel_JuPedSim_gRPC.h
            ${jps_proto_srcs} ${jps_proto_hdrs} ${jps_grpc_srcs} ${jps_grpc_hdrs}
            )
    endif()
endif ()
set(microsim_transportables_STAT_SRCS
        MSPerson.cpp
        MSPerson.h
        MSPModel.cpp
        MSPModel.h
        MSPModel_Interacting.cpp
        MSPModel_Interacting.h
        MSPModel_NonInteracting.cpp
        MSPModel_NonInteracting.h
        MSPModel_Striping.cpp
        MSPModel_Striping.h
        MSStage.cpp
        MSStage.h
        MSStageMoving.cpp
        MSStageMoving.h
        MSStageDriving.cpp
        MSStageDriving.h
        MSStageTranship.cpp
        MSStageTranship.h
        MSStageTrip.cpp
        MSStageTrip.h
        MSStageWaiting.cpp
        MSStageWaiting.h
        MSStageWalking.cpp
        MSStageWalking.h
        MSTransportable.cpp
        MSTransportable.h
        MSTransportableControl.cpp
        MSTransportableControl.h
        ${microsim_transportables_JPS_SRCS}
        ${microsim_transportables_JPS_GRPC_SRCS}
        )

add_library(microsim_transportables STATIC ${microsim_transportables_STAT_SRCS} ${remote_model})
target_link_libraries(microsim_transportables
    PUBLIC $<$<BOOL:${HAVE_JUPEDSIM}>:jupedsim::jupedsim> ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF}
)
set_property(TARGET microsim_transportables PROPERTY PROJECT_LABEL "z_microsim_transportables")
