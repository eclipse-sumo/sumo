#include <cassert>
#include "MSRoute.h"

using namespace std;

MSRoute::RouteDict MSRoute::myDict;

MSRoute::MSRoute(const std::string &id,	
				 const MSEdgeVector &edges,
                 bool multipleReferenced)
	: Named(id), _edges(edges), 
    _multipleReferenced(multipleReferenced)
{
}


MSRoute::~MSRoute()
{
}

MSRouteIterator
MSRoute::begin() const
{
	return _edges.begin();
}

MSRouteIterator
MSRoute::end() const 
{
	return _edges.end();
}

size_t 
MSRoute::size() const
{
	return _edges.size();
}


MSEdge *
MSRoute::getLastEdge() const
{
	return _edges[_edges.size()-1];
}

bool
MSRoute::dictionary(const string &id, MSRoute* route)
{
    RouteDict::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict.insert(RouteDict::value_type(id, route));
        return true;
    }
    return false;
}


MSRoute*
MSRoute::dictionary(const string &id)
{
    RouteDict::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}


void
MSRoute::clear()
{
    for(RouteDict::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}


void 
MSRoute::remove(const std::string &id)
{
    RouteDict::iterator i=myDict.find(id);
    assert(i!=myDict.end());
    delete (*i).second;
    myDict.erase(id);
}


bool
MSRoute::inFurtherUse() const 
{
    return _multipleReferenced;
}