#ifndef ROVehicleCont_h
#define ROVehicleCont_h

#include <string>
#include <map>
#include <queue>
#include <vector>
#include <utils/common/NamedObjectCont.h>
#include "ROHelper.h"

class ROVehicle;

class ROVehicleCont : public NamedObjectCont<ROVehicle*> {
private:
    std::priority_queue<ROVehicle*, 
        std::vector<ROVehicle*>, 
        ROHelper::VehicleByDepartureComperator> _sorted;
public:
    ROVehicleCont();
    ~ROVehicleCont();
    std::priority_queue<ROVehicle*, 
        std::vector<ROVehicle*>, 
        ROHelper::VehicleByDepartureComperator> &sort();
private:
    /// we made the copy constructor invalid
    ROVehicleCont(const ROVehicleCont &src);
    /// we made the assignment operator invalid
    ROVehicleCont &operator=(const ROVehicleCont &src);
};

#endif

