set(microsim_pedestrians_STAT_SRCS
        MSPerson.cpp
        MSPerson.h
        MSPModel.cpp
        MSPModel.h
        MSPModel_Striping.cpp
        MSPModel_Striping.h
        MSPModel_NonInteracting.cpp
        MSPModel_NonInteracting.h
        )

if (BUILD_GRPC)
    #    file(GLOB ProtoFiles "${CMAKE_SOURCE_DIR}/hybrid/*.proto")
    file(GLOB ProtoFiles "hybridsim.proto")
    PROTOBUF_GENERATE_CPP(ProtoSources ProtoHeaders ${ProtoFiles})
    PROTOBUF_GENERATE_GRPC_CPP(ProtoGRPCSources ProtoGRPCHeaders ${ProtoFiles})
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    set(remote_model
            MSPModel_Remote.cpp MSPModel_Remote.h
            ${ProtoSources}
            ${ProtoGRPCSources}
            )
endif (BUILD_GRPC)

add_library(microsim_pedestrians STATIC ${microsim_pedestrians_STAT_SRCS} ${remote_model})
set_property(TARGET microsim_pedestrians PROPERTY PROJECT_LABEL "z_microsim_pedestrians")
