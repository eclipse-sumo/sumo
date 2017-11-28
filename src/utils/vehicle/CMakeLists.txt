set(utils_vehicle_STAT_SRCS
   AStarRouter.h
   CarEdge.h
   CHBuilder.h
   CHRouter.h
   CHRouterWrapper.h
   DijkstraRouter.h
   IntermodalEdge.h
   IntermodalNetwork.h
   IntermodalRouter.h
   GawronCalculator.h
   LogitCalculator.h
   RouteCostCalculator.h
   PedestrianRouter.h
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
