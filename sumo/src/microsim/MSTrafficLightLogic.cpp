#include <string>
#include <map>
#include "MSTrafficLightLogic.h"


MSTrafficLightLogic::DictType MSTrafficLightLogic::_dict;

MSTrafficLightLogic::MSTrafficLightLogic(const std::string &id)
    : _id(id)
{
}


MSTrafficLightLogic::~MSTrafficLightLogic()
{
}


bool
MSTrafficLightLogic::dictionary(const std::string &name,
                                MSTrafficLightLogic *logic)
{
    if(_dict.find(name)==_dict.end()) {
        _dict[name] = logic;
        return true;
    }
    return false;
}


MSTrafficLightLogic *
MSTrafficLightLogic::dictionary(const std::string &name)
{
    DictType::iterator i = _dict.find(name);
    if(i==_dict.end()) {
        return 0;
    }
    return (*i).second;
}


