#include <string>
#include <map>
#include "NIVissimSource.h"

NIVissimSource::DictType NIVissimSource::myDict;

NIVissimSource::NIVissimSource(const std::string &id, const std::string &name,
                               const std::string &edgeid, double q,
                               bool exact, int vehicle_combination,
                               double beg, double end)
    : myID(id), myName(name), myEdgeID(edgeid), myQ(q), myExact(exact),
        myVehicleCombination(vehicle_combination),
        myTimeBeg(beg), myTimeEnd(end)
{
}


NIVissimSource::~NIVissimSource()
{
}


bool
NIVissimSource::dictionary(const std::string &id, const std::string &name,
                           const std::string &edgeid, double q, bool exact,
                           int vehicle_combination, double beg, double end)
{
    NIVissimSource *o = new NIVissimSource(id, name, edgeid, q, exact,
        vehicle_combination, beg, end);
    if(!dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}


bool
NIVissimSource::dictionary(const std::string &id, NIVissimSource *o)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        myDict[id] = o;
        return true;
    }
    return false;
}


NIVissimSource *
NIVissimSource::dictionary(const std::string &id)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}


void
NIVissimSource::clearDict()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}



