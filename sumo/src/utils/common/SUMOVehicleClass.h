#ifndef SUMOVehicleClass_h
#define SUMOVehicleClass_h

#include <string>

enum SUMOVehicleClass {
	SVC_UNKNOWN = -1,

	SVC_PRIVATE = 0,

	SVC_PUBLIC_TRANSPORT_BUS,
	SVC_PUBLIC_TRANSPORT_LIGHTRAIL,
	SVC_PUBLIC_TRANSPORT_CITY_RAIL,
	SVC_PUBLIC_TRANSPORT_RAIL_SLOW,
	SVC_PUBLIC_TRANSPORT_RAIL_FAST,

	SVC_PUBLIC_EMERGENCY,

	SVC_PRIVATE_MOTORCYCLE,
	SVC_PRIVATE_BICYCLE,
	SVC_PRIVATE_PEDESTRIAN,

};

extern std::string getVehicleClassName(SUMOVehicleClass id);
extern SUMOVehicleClass getVehicleClassID(const std::string &name);

#endif
