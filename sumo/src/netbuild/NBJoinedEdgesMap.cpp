#include <cassert>
#include <iostream>
#include "NBJoinedEdgesMap.h"
#include <algorithm>

using namespace std;

NBJoinedEdgesMap gJoinedEdges;



NBJoinedEdgesMap::NBJoinedEdgesMap()
{
}


NBJoinedEdgesMap::~NBJoinedEdgesMap()
{
}


void
NBJoinedEdgesMap::init(const std::vector<std::string> &edgeNames)
{
    myMap.clear();
    for(std::vector<std::string>::const_iterator i=edgeNames.begin(); i!=edgeNames.end(); i++) {
        MappedEdgesVector e;
        e.push_back(*i);
        myMap[*i] = e;
    }
}


void
NBJoinedEdgesMap::appended(const std::string &to, const std::string &what)
{
    copy(myMap[what].begin(), myMap[what].end(),
        back_inserter(myMap[to]));
//    myMap[to].push_back(what);
    JoinedEdgesMap::iterator i = myMap.find(what);
    assert(i!=myMap.end());
    myMap.erase(i);
}


std::ostream &
operator<<(std::ostream &os, const NBJoinedEdgesMap &jemap)
{
    NBJoinedEdgesMap::JoinedEdgesMap::const_iterator i;
    for(i=jemap.myMap.begin(); i!=jemap.myMap.end(); ++i) {
        os << (*i).first << "\t";
        const NBJoinedEdgesMap::MappedEdgesVector &e = (*i).second;
        for(NBJoinedEdgesMap::MappedEdgesVector::const_iterator j=e.begin(); j!=e.end(); ++j) {
            os << (*j) << "\t";
        }
        os << endl;
    }
    return os;
}



