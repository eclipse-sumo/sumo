#include "NIVissimVehicleType.h"

NIVissimVehicleType::DictType NIVissimVehicleType::myDict;

NIVissimVehicleType::NIVissimVehicleType(int id,
        const std::string &name, const std::string &category, double length,
        const RGBColor &color, double amax, double dmax)
    : myID(id), myName(name), myCategory(category), myLength(length),
    myColor(color), myAMax(amax), myDMax(dmax)
{
}


NIVissimVehicleType::~NIVissimVehicleType()
{
}



bool
NIVissimVehicleType::dictionary(int id,
    const std::string &name, const std::string &category, double length,
    const RGBColor &color, double amax, double dmax)
{
    NIVissimVehicleType *o = new NIVissimVehicleType(id, name, category,
        length, color, amax, dmax);
    if(!dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}


bool
NIVissimVehicleType::dictionary(int id, NIVissimVehicleType *o)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        myDict[id] = o;
        return true;
    }
    return false;
}


NIVissimVehicleType *
NIVissimVehicleType::dictionary(int id)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}

void
NIVissimVehicleType::clearDict()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}




