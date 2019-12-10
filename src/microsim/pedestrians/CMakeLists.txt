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

add_library(microsim_pedestrians STATIC ${microsim_pedestrians_STAT_SRCS} ${remote_model})
set_property(TARGET microsim_pedestrians PROPERTY PROJECT_LABEL "z_microsim_pedestrians")
