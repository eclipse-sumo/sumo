#include <map>
#include <utils/router/TextHelpers.h>
#include "ROEdgeVector.h"
#include "ROOrigDestRouteDef.h"
#include "RORouteSnippletCont.h"

using namespace std;

RORouteSnippletCont::RORouteSnippletCont()
{
}


RORouteSnippletCont::~RORouteSnippletCont()
{
}


bool
RORouteSnippletCont::add(const ROEdgeVector &item)
{
    ROEdge *from = item.getFirst();
    ROEdge *to = item.getLast();
    MapType::iterator i=_known.find(MapType::key_type(from, to));
    if(i==_known.end()) {
        _known.insert(
            MapType::value_type(std::pair<ROEdge*, ROEdge*>(from, to), item));
        return true;
    }
    return false;
}


const ROEdgeVector &
RORouteSnippletCont::get(ROEdge *from, ROEdge *to) const
{
    MapType::const_iterator i=_known.find(MapType::key_type(from, to));
    return (*i).second;
}

bool
RORouteSnippletCont::knows(ROEdge *from, ROEdge *to) const
{
    return false; // !!! should not be used until a validation of net statistics has been made
/*    MapType::const_iterator i=_known.find(MapType::key_type(from, to));
    return i!=_known.end();*/
}


