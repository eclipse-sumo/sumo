set(netedit_demandelements_SRCS
   GNERouteHandler.h
   GNERouteHandler.cpp
   GNEDemandElement.h
   GNEDemandElement.cpp
   GNERoute.h
   GNERoute.cpp
   GNEVehicleType.h
   GNEVehicleType.cpp
   GNEVehicle.h
   GNEVehicle.cpp
   GNEStop.h
   GNEStop.cpp
   GNEWalk.h
   GNEWalk.cpp
   GNERide.h
   GNERide.cpp
   GNEPersonTrip.h
   GNEPersonTrip.cpp
   GNEPerson.h
   GNEPerson.cpp
   )

add_library(netedit_demandelements STATIC ${netedit_demandelements_SRCS})
set_property(TARGET netedit_demandelements PROPERTY PROJECT_LABEL "z_netedit_demandelements")
