#include <string>
#include <utils/common/IntVector.h>
#include "NIVissimClosures.h"


NIVissimClosures::DictType NIVissimClosures::myDict;

NIVissimClosures::NIVissimClosures(const std::string &id,
                                   int from_node, int to_node,
                                   IntVector &overEdges)
    : myID(id), myFromNode(from_node), myToNode(to_node),
    myOverEdges(overEdges)
{
}


NIVissimClosures::~NIVissimClosures()
{
}


bool
NIVissimClosures::dictionary(const std::string &id,
                             int from_node, int to_node,
                             IntVector &overEdges)
{
    NIVissimClosures *o = new NIVissimClosures(id, from_node, to_node,
        overEdges);
    if(!dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}


bool
NIVissimClosures::dictionary(const std::string &name, NIVissimClosures *o)
{
    DictType::iterator i=myDict.find(name);
    if(i==myDict.end()) {
        myDict[name] = o;
        return true;
    }
    return false;
}


NIVissimClosures *
NIVissimClosures::dictionary(const std::string &name)
{
    DictType::iterator i=myDict.find(name);
    if(i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}



void
NIVissimClosures::clearDict()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}

