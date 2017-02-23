message("entering vehicle")

set(vehicle_files
	AStarRouter.h
	CarEdge.h
	CHRouter.h CHRouterWrapper.h
	DijkstraRouterTT.h DijkstraRouterEffort.h
	IntermodalEdge.h IntermodalNetwork.h IntermodalRouter.h
	GawronCalculator.h LogitCalculator.h RouteCostCalculator.h
	PedestrianRouter.h SUMOAbstractRouter.h
	SPTree.h SUMOVehicle.h
	SUMOVehicleParameter.cpp SUMOVehicleParameter.h
	SUMOVTypeParameter.cpp SUMOVTypeParameter.h)

add_library(utvehicle STATIC ${vehicle_files})
