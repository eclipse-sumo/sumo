#include <string>
#include <utils/gfx/RGBColor.h>
#include <utils/common/IntVector.h>
#include "NIVissimVehTypeClass.h"


NIVissimVehTypeClass::DictType NIVissimVehTypeClass::myDict;

NIVissimVehTypeClass::NIVissimVehTypeClass(int id,
                                           const std::string &name,
                                           const RGBColor &color,
                                           IntVector &types)
    : myID(id), myName(name), myColor(color), myTypes(types)
{
}

NIVissimVehTypeClass::~NIVissimVehTypeClass()
{
}


bool
NIVissimVehTypeClass::dictionary(int id, const std::string &name,
                                 const RGBColor &color,
                                 IntVector &types)
{
    NIVissimVehTypeClass *o = new NIVissimVehTypeClass(id, name, color, types);
    if(!dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}




bool
NIVissimVehTypeClass::dictionary(int name, NIVissimVehTypeClass *o)
{
    DictType::iterator i=myDict.find(name);
    if(i==myDict.end()) {
        myDict[name] = o;
        return true;
    }
    return false;
}


NIVissimVehTypeClass *
NIVissimVehTypeClass::dictionary(int name)
{
    DictType::iterator i=myDict.find(name);
    if(i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}


void
NIVissimVehTypeClass::clearDict()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}


