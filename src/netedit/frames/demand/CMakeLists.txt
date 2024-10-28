set(netedit_frames_demand_SRCS
   GNEDistributionFrame.h
   GNEDistributionFrame.cpp
   GNERouteFrame.h
   GNERouteFrame.cpp
   GNERouteDistributionFrame.h
   GNERouteDistributionFrame.cpp
   GNEStopFrame.h
   GNEStopFrame.cpp
   GNEPersonFrame.h
   GNEPersonFrame.cpp
   GNEPersonPlanFrame.h
   GNEPersonPlanFrame.cpp
   GNEContainerFrame.h
   GNEContainerFrame.cpp
   GNEContainerPlanFrame.h
   GNEContainerPlanFrame.cpp
   GNEVehicleFrame.h
   GNEVehicleFrame.cpp
   GNETypeFrame.h
   GNETypeFrame.cpp
   GNETypeDistributionFrame.h
   GNETypeDistributionFrame.cpp
   )

add_library(netedit_frames_demand STATIC ${netedit_frames_demand_SRCS})
set_property(TARGET netedit_frames_demand PROPERTY PROJECT_LABEL "z_netedit_frames_demand")
