#include <vector>
#include <string>
#include <utility>
#include <utils/common/UtilExceptions.h>
#include <utils/common/Named.h>
#include "ODDistrict.h"

ODDistrict::ODDistrict(const std::string &id)
    : Named(id)
{
}

ODDistrict::~ODDistrict()
{
}


void
ODDistrict::addSource(const std::string &id, double weight)
{
    _sources.push_back(std::pair<std::string, double>(id, weight));
}


void
ODDistrict::addSink(const std::string &id, double weight)
{
    _sinks.push_back(std::pair<std::string, double>(id, weight));
}


std::string
ODDistrict::getRandomSource() const
{
    return getRandom(_sources);
}


std::string
ODDistrict::getRandomSink() const
{
    return getRandom(_sinks);
}


std::string
ODDistrict::getRandom(const WeightedEdgeIDCont &cont) const
{
    // check whether at least one elements exists
    if(cont.size()==0) {
        throw UnknownElement();
    }
    // compute which item to retrieve
    double val = rand() / RAND_MAX;
    // go through the list to retrieve the item
    for(WeightedEdgeIDCont::const_iterator i=cont.begin(); i!=cont.end(); i++) {
        val -= (*i).second;
        if(val<=0) {
            return (*i).first;
        }
    }
    // return the last item when no other was found
    //  !!! maybe a warning should be printed
    return (*(cont.end()-1)).first;
}
