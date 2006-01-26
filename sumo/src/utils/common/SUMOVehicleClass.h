#ifndef SUMOVehicleClass_h
#define SUMOVehicleClass_h

#include <string>

/**
 * Bits:
 * @arg 0-7:  vehicle ownership
 * @arg 8-23: vehicle size
 *
 * From NavTeq:
 * @arg [0] All
 * @arg [1] Passenger cars
 * @arg [2] High Occupancy Vehicle
 * @arg [3] Emergency Vehicle
 * @arg [4] Taxi
 * @arg [5] Public Bus
 * @arg [6] Delivery Truck
 * @arg [7] Transport Truck
 * @arg [8] Bicycle
 * @arg [9] Pedestrian
 */


enum SUMOVehicleClass {
	SVC_UNKNOWN = 0,

	//{ vehicle ownership
	/// private vehicles
	SVC_PRIVATE = 1,
	/// public transport vehicles
	SVC_PUBLIC_TRANSPORT = 2,
	/// public emergency vehicles
	SVC_PUBLIC_EMERGENCY = 4,
	/// authorities vehicles
	SVC_PUBLIC_AUTHORITY = 8,
	/// army vehicles
	SVC_PUBLIC_ARMY = 16,
	/// army vehicles
	SVC_VIP = 32,
	//}


	//{ vehicle size
	/// vehicle is a HOV
	SVC_HOV = 256,
	/// vehicle is a taxi
	SVC_TAXI = 512,
	/// vehicle is a bus
	SVC_BUS = 1024,
	/// vehicle is a small delivery vehicle
	SVC_DELIVERY = 2048,
	/// vehicle is a large tranposrt vehicle
	SVC_TRANSPORT = 4096,
	/// vehicle is a light rail
	SVC_LIGHTRAIL = 8192,
	/// vehicle is a city rail
	SVC_CITYRAIL = 16384,
	/// vehicle is a slow moving transport rail
	SVC_RAIL_SLOW = 32768,
	/// vehicle is a fast moving rail
	SVC_RAIL_FAST = 65536,

	/// vehicle is a motorcycle
	SVC_MOTORCYCLE = 131072,
	/// vehicle is a bicycle
	SVC_BICYCLE = 262144,
	/// is a pedestrian
	SVC_PEDESTRIAN = 524288,
	// }

};

extern std::string getVehicleClassName(SUMOVehicleClass id);
extern SUMOVehicleClass getVehicleClassID(const std::string &name);

#endif
