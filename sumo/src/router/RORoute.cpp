#include <string>
#include <iostream>
#include <utils/common/Named.h>
#include "ROEdgeVector.h"
#include "ROEdge.h"
#include "RORoute.h"

using namespace std;

RORoute::RORoute(const std::string &id, double costs, double prop,
                 const ROEdgeVector &route)
	: Named(id), _costs(costs), _propability(prop), _route(route)
{
}


RORoute::~RORoute()
{
}


void
RORoute::add(ROEdge *edge)
{
	_route.add(edge);
}


void
RORoute::xmlOut(std::ostream &os) const
{
    os << "<route id=\"" << _id << "\">";
//    os << " references=\"" << _references << "\">";
    os << _route;
    os << "</route>" << endl;
}

void
RORoute::xmlOutEdges(std::ostream &os) const
{
    os << _route;
}


double 
RORoute::getCosts() const
{
    return _costs;
}


void 
RORoute::setCosts(double costs)
{
    _costs = costs;
}


double 
RORoute::recomputeCosts(long begin) const
{
    return _route.recomputeCosts(begin);
}


ROEdge *
RORoute::getFirst() const
{
    return _route.getFirst();
}


ROEdge *
RORoute::getLast() const
{
    return _route.getLast();
}



double 
RORoute::getPropability() const
{
    return _propability;
}


bool 
RORoute::equals(RORoute *ro) const
{
    return _route.equals(ro->_route);
}


size_t 
RORoute::size() const
{
    return _route.size();
}


void 
RORoute::setPropability(double prop)
{
    _propability = prop;
}


