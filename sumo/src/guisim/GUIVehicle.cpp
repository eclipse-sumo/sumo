#include <vector>
#include <string>
#include <microsim/MSVehicle.h>
#include "GUIVehicle.h"

GUIVehicle::GUIVehicle()
    : MSVehicle("DUMMY", 0, 0, 0)
{
}


GUIVehicle::~GUIVehicle()
{
}


std::vector<std::string>
GUIVehicle::getNames()
{
    std::vector<std::string> ret;
    ret.reserve(MSVehicle::myDict.size());
    for(MSVehicle::DictType::iterator i=MSVehicle::myDict.begin();
        i!=MSVehicle::myDict.end(); i++) {
        ret.push_back((*i).first);
    }
    return ret;
}
