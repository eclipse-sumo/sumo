set(netedit_demandelements_SRCS
   GNEDemandHandler.h
   GNEDemandHandler.cpp
   GNEDemandElement.h
   GNEDemandElement.cpp
   GNERoute.h
   GNERoute.cpp
   GNEVehicleType.h
   GNEVehicleType.cpp
   GNEFlow.h
   GNEFlow.cpp
   GNETrip.h
   GNETrip.cpp
   )

add_library(netedit_demandelements STATIC ${netedit_demandelements_SRCS})
set_property(TARGET netedit_demandelements PROPERTY PROJECT_LABEL "z_netedit_demandelements")
