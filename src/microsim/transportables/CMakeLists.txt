if (HAVE_JUPEDSIM_LEGACY)
    set(microsim_transportables_JPS_SRCS
        MSPModel_JuPedSim_legacy.cpp
        MSPModel_JuPedSim_legacy.h
        )
endif()
if (HAVE_JUPEDSIM_GRPC)
    # gRPC is never used in Windows debug builds (single- and multi-config generators)
    set(jps_grpc_enabled "$<NOT:$<AND:$<BOOL:${WIN32}>,$<CONFIG:Debug>>>")
    set(microsim_transportables_JPS_GRPC_SRCS
        MSPModel_JuPedSim.h
        "$<${jps_grpc_enabled}:MSPModel_JuPedSim.cpp>"
        )
endif()
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
target_link_libraries(microsim_transportables PUBLIC
    $<$<BOOL:${HAVE_JUPEDSIM_LEGACY}>:jupedsim::jupedsim>
    $<$<AND:$<BOOL:${HAVE_JUPEDSIM_GRPC}>,${jps_grpc_enabled}>:gRPC::grpc++_unsecure protobuf::libprotobuf-lite>
)
# You can also use $<$<BOOL:${HAVE_JUPEDSIM_GRPC}>:gRPC::grpc++ protobuf::libprotobuf> above,
# if you need ssl communication or protobuf reflection.
# For the latter the jupedsim.proto file needs to be adapted as well.
set_property(TARGET microsim_transportables PROPERTY PROJECT_LABEL "z_microsim_transportables")

if (HAVE_JUPEDSIM_GRPC)
    get_filename_component(jps_python_dir "../../../tools/jupedsim_grpc" ABSOLUTE)
    set(jps_proto "${jps_python_dir}/jupedsim.proto")
    protobuf_generate(PROTOS "${jps_proto}" LANGUAGE cpp APPEND_PATH PROTOC_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}" OUT_VAR jps_cpp_generated)
    protobuf_generate(PROTOS "${jps_proto}" LANGUAGE grpc APPEND_PATH PROTOC_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}"
                      PLUGIN "protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>" GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
                      OUT_VAR jps_grpc_generated)
    foreach (jps_gen ${jps_cpp_generated} ${jps_grpc_generated})
        set_source_files_properties(${jps_gen} PROPERTIES GENERATED TRUE)
        target_sources(microsim_transportables PRIVATE "$<${jps_grpc_enabled}:${jps_gen}>")
    endforeach ()
    protobuf_generate(TARGET microsim_transportables PROTOS "${jps_proto}" LANGUAGE python APPEND_PATH PROTOC_OUT_DIR "${jps_python_dir}")
    protobuf_generate(TARGET microsim_transportables PROTOS "${jps_proto}" LANGUAGE grpc APPEND_PATH PROTOC_OUT_DIR "${jps_python_dir}"
                      PLUGIN "protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_python_plugin>" GENERATE_EXTENSIONS _pb2_grpc.py)
    include_directories("${CMAKE_CURRENT_BINARY_DIR}")
endif ()
