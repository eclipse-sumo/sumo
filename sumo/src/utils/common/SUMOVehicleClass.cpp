#include "SUMOVehicleClass.h"
#include <string>


std::string
getVehicleClassName(SUMOVehicleClass id)
{
	switch(id) {
	case SVC_UNKNOWN:
		return "unknown";
	case SVC_PRIVATE:
		return "private";

	case SVC_PUBLIC_TRANSPORT_BUS:
		return "public_bus";
	case SVC_PUBLIC_TRANSPORT_LIGHTRAIL:
		return "public_lightrail";
	case SVC_PUBLIC_TRANSPORT_CITY_RAIL:
		return "public_cityrail";
	case SVC_PUBLIC_TRANSPORT_RAIL_SLOW:
		return "public_rail_slow";
	case SVC_PUBLIC_TRANSPORT_RAIL_FAST:
		return "public_rail_fast";

	case SVC_PUBLIC_EMERGENCY:
		return "public_emergency";

	case SVC_PRIVATE_MOTORCYCLE:
		return "private_motorcycle";
	case SVC_PRIVATE_BICYCLE:
		return "private_bicycle";
	case SVC_PRIVATE_PEDESTRIAN:
		return "pedestrian";
	default:
		throw 1;
	}
}


SUMOVehicleClass
getVehicleClassID(const std::string &name)
{
	if(name=="unknown") {
		return SVC_UNKNOWN;
	} else if(name=="private") {
		return SVC_PRIVATE;

	} else if(name=="public_bus") {
		return SVC_PUBLIC_TRANSPORT_BUS;
	} else if(name=="public_lightrail") {
		return SVC_PUBLIC_TRANSPORT_LIGHTRAIL;
	} else if(name=="public_cityrail") {
		return SVC_PUBLIC_TRANSPORT_CITY_RAIL;
	} else if(name=="public_rail_slow") {
		return SVC_PUBLIC_TRANSPORT_RAIL_SLOW;
	} else if(name=="public_rail_fast") {
		return SVC_PUBLIC_TRANSPORT_RAIL_FAST;

	} else if(name=="public_emergency") {
		return SVC_PUBLIC_EMERGENCY;

	} else if(name=="private_motorcycle") {
		return SVC_PRIVATE_MOTORCYCLE;
	} else if(name=="private_bicycle") {
		return SVC_PRIVATE_BICYCLE;
	} else if(name=="pedestrian") {
		return SVC_PRIVATE_PEDESTRIAN;
	}
	throw 1;
}
