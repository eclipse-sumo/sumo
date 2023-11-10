if (JPS_VERSION)
    set(microsim_transportables_JPS_SRCS
        MSPModel_JuPedSim.cpp
        MSPModel_JuPedSim.h
        )
endif ()
set(microsim_transportables_STAT_SRCS
        MSPerson.cpp
        MSPerson.h
        MSPModel.cpp
        MSPModel.h
        MSPModel_Striping.cpp
        MSPModel_Striping.h
        MSPModel_NonInteracting.cpp
        MSPModel_NonInteracting.h
        MSStage.cpp
        MSStage.h
        MSStageTrip.cpp
        MSStageTrip.h
        MSStageWaiting.cpp
        MSStageWaiting.h
        MSStageMoving.cpp
        MSStageMoving.h
        MSStageDriving.cpp
        MSStageDriving.h
        MSStageTranship.cpp
        MSStageTranship.h
        MSTransportable.cpp
        MSTransportable.h
        MSTransportableControl.cpp
        MSTransportableControl.h
        ${microsim_transportables_JPS_SRCS}
        )

add_library(microsim_transportables STATIC ${microsim_transportables_STAT_SRCS} ${remote_model})
target_link_libraries(microsim_transportables
    PUBLIC $<$<BOOL:${JPS_VERSION}>:jupedsim::jupedsim>
)
set_property(TARGET microsim_transportables PROPERTY PROJECT_LABEL "z_microsim_transportables")
