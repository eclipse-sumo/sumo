#ifndef ROHelper_h
#define ROHelper_h

#include <functional>
#include "ROVehicle.h"

class ROHelper {
public:
    class VehicleByDepartureComperator : public std::less<ROVehicle*> {
    public:
        explicit VehicleByDepartureComperator() { }
        ~VehicleByDepartureComperator() { }
        bool operator()(ROVehicle *veh1, ROVehicle *veh2) const {
            return veh1->getDepartureTime()<veh2->getDepartureTime();
        }
    };
};

#endif

