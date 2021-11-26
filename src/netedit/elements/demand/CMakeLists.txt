set(netedit_elements_demand_SRCS
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
   GNEPerson.h
   GNEPerson.cpp
   GNEPersonTrip.h
   GNEPersonTrip.cpp
   GNEWalk.h
   GNEWalk.cpp
   GNERide.h
   GNERide.cpp
   GNEContainer.h
   GNEContainer.cpp
   GNETranship.h
   GNETranship.cpp
   GNETransport.h
   GNETransport.cpp
   )

add_library(netedit_elements_demand STATIC ${netedit_elements_demand_SRCS})
set_property(TARGET netedit_elements_demand PROPERTY PROJECT_LABEL "z_netedit_elements_demand")
