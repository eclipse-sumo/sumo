#include <string>
#include <utils/common/NamedObjectCont.h>
#include <queue>
#include "ROVehicle.h"
#include "ROHelper.h"
#include "ROVehicleCont.h"

using namespace std;

ROVehicleCont::ROVehicleCont()
{
}


ROVehicleCont::~ROVehicleCont()
{
}


priority_queue<ROVehicle*, 
    std::vector<ROVehicle*>, ROHelper::VehicleByDepartureComperator> &
ROVehicleCont::sort() {
//    _sorted.reserve(_cont.size());
    for(myCont::iterator i=_cont.begin(); i!=_cont.end(); i++) {
        _sorted.push((*i).second);
    }
    return _sorted;
}



