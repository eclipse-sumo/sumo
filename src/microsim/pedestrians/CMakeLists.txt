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

if (GRPC_FOUND AND Protobuf_FOUND)
    if (MSVC)
        add_definitions(-D_WIN32_WINNT=0x0601)
    endif (MSVC)
    PROTOBUF_GENERATE_CPP(ProtoSources ProtoHeaders hybridsim.proto)
    PROTOBUF_GENERATE_GRPC_CPP(ProtoGRPCSources ProtoGRPCHeaders hybridsim.proto)
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    set(remote_model
            MSPModel_Remote.cpp MSPModel_Remote.h
            ${ProtoSources}
            ${ProtoGRPCSources}
            )
endif (GRPC_FOUND AND Protobuf_FOUND)

add_library(microsim_pedestrians STATIC ${microsim_pedestrians_STAT_SRCS} ${remote_model})
set_property(TARGET microsim_pedestrians PROPERTY PROJECT_LABEL "z_microsim_pedestrians")
