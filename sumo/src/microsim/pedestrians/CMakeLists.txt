message("entering pedestrians")

set(microsimpeds_files
	MSPerson.cpp MSPerson.h
	MSPModel.cpp MSPModel.h 
	MSPModel_Striping.cpp MSPModel_Striping.h 
	MSPModel_NonInteracting.cpp MSPModel_NonInteracting.h)	



if(GRPC_FOUND)

	message(STATUS "generating protobuf files")
	set(GRPC_DIR "${CMAKE_SOURCE_DIR}/cmake_modules/")
	file(GLOB ProtoFiles "${CMAKE_SOURCE_DIR}/src/microsim/pedestrians/*.proto")
	PROTOBUF_GENERATE_CPP(ProtoSources ProtoHeaders ${ProtoFiles})
	PROTOBUF_GENERATE_GRPC_CPP(ProtoGRPCSources ProtoGRPCHeaders ${ProtoFiles})
	message(STATUS "generated the following files:
        ${ProtoSources}
        ${ProtoGRPCSources}
        ${ProtoHeaders}
        ${ProtoGRPCHeaders}")

    include_directories(${CMAKE_CURRENT_BINARY_DIR})

    set(microsimpeds_files ${microsimpeds_files} ${ProtoSources}
            ${ProtoGRPCSources} ${ProtoHeaders} ${ProtoGRPCHeaders}
            MSPModel_RemoteControlled.cpp MSPModel_RemoteControlled.h MSPGRPCClient.h MSPGRPCClient.cpp
            MSPGRCPState.h MSPGRCPState.cpp)


endif()


add_library(mspedestrians STATIC ${microsimpeds_files})
