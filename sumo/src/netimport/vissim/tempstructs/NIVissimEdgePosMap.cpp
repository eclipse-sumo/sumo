#include <map>
#include "NIVissimEdgePosMap.h"


NIVissimEdgePosMap::NIVissimEdgePosMap()
{
}


NIVissimEdgePosMap::~NIVissimEdgePosMap()
{
}


void
NIVissimEdgePosMap::add(int edgeid, double pos)
{
    add(edgeid, pos, pos);
}


void
NIVissimEdgePosMap::add(int edgeid, double from, double to)
{
    if(from>to) {
        double tmp = from;
        from = to;
        to = tmp;
    }
    ContType::iterator i=myCont.find(edgeid);
    if(i==myCont.end()) {
        myCont[edgeid] = Range(from, to);
    } else {
        double pfrom = (*i).second.first;
        double pto = (*i).second.second;
        if(pfrom<from) {
            from = pfrom;
        }
        if(pto>to) {
            to = pto;
        }
        myCont[edgeid] = Range(from, to);
    }
}


void
NIVissimEdgePosMap::join(NIVissimEdgePosMap &with)
{
    for(ContType::iterator i=with.myCont.begin(); i!=with.myCont.end(); i++) {
        add((*i).first, (*i).second.first, (*i).second.second);
    }
}


