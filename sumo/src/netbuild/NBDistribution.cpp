#include "NBDistribution.h"

NBDistribution::TypedDistDict NBDistribution::myDict;

bool
NBDistribution::dictionary(const std::string &type, const std::string &id,
                           Distribution *d)
{
    TypedDistDict::iterator i=myDict.find(type);

    if(i==myDict.end()) {
        myDict[type][id] = d;
        return true;
    }
    DistDict &dict = (*i).second;
    DistDict::iterator j=dict.find(id);
    if(j==dict.end()) {
        myDict[type][id] = d;
        return true;
    }
    return false;
}


Distribution *
NBDistribution::dictionary(const std::string &type,
                           const std::string &id)
{
    TypedDistDict::iterator i=myDict.find(type);
    if(i==myDict.end()) {
        return 0;
    }
    DistDict &dict = (*i).second;
    DistDict::iterator j=dict.find(id);
    if(j==dict.end()) {
        return 0;
    }
    return (*j).second;
}


void 
NBDistribution::clear()
{
    for(TypedDistDict::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        DistDict &dict = (*i).second;
        for(DistDict::iterator j=dict.begin(); j!=dict.end(); j++) {
            delete (*j).second;
        }
    }
}
