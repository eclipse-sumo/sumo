#include <string>
#include <map>
#include "NIVissimVehicleClassVector.h"
#include "NIVissimTrafficDescription.h"

NIVissimTrafficDescription::DictType NIVissimTrafficDescription::myDict;

NIVissimTrafficDescription::NIVissimTrafficDescription(
    const std::string &id, const std::string &name,
    const NIVissimVehicleClassVector &vehicleTypes)
    : myID(id), myName(name), myVehicleTypes(vehicleTypes)
{
}


NIVissimTrafficDescription::~NIVissimTrafficDescription()
{
}


bool
NIVissimTrafficDescription::dictionary(const std::string &id,
        const std::string &name, const NIVissimVehicleClassVector &vehicleTypes)
{
    NIVissimTrafficDescription *o = new NIVissimTrafficDescription(id, name, vehicleTypes);
    if(!dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}


bool
NIVissimTrafficDescription::dictionary(const std::string &id, NIVissimTrafficDescription *o)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        myDict[id] = o;
        return true;
    }
    return false;
}


NIVissimTrafficDescription *
NIVissimTrafficDescription::dictionary(const std::string &id)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}



