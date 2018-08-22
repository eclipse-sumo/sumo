set(utils_vehicle_STAT_SRCS
   AStarRouter.h
   AccessEdge.h
   CarEdge.h
   PedestrianEdge.h
   PublicTransportEdge.h
   StopEdge.h
   CHBuilder.h
   CHRouter.h
   CHRouterWrapper.h
   DijkstraRouter.h
   IntermodalEdge.h
   IntermodalNetwork.h
   IntermodalRouter.h
   IntermodalTrip.h
   EffortCalculator.h
   GawronCalculator.h
   LogitCalculator.h
   RouteCostCalculator.h
   PedestrianRouter.h
   RouterProvider.h
   SUMOAbstractRouter.h
   SPTree.h
   SUMOVehicle.h
   SUMOVehicleParameter.cpp
   SUMOVehicleParameter.h
   SUMOVTypeParameter.cpp
   SUMOVTypeParameter.h
)

add_library(utils_vehicle STATIC ${utils_vehicle_STAT_SRCS})
set_property(TARGET utils_vehicle PROPERTY PROJECT_LABEL "z_utils_vehicle")
