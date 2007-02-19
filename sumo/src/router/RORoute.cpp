/****************************************************************************/
/// @file    RORoute.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A build route
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <utils/common/Named.h>
#include <utils/common/StringUtils.h>
#include "ROEdgeVector.h"
#include "ROEdge.h"
#include "RORoute.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
RORoute::RORoute(const std::string &id, SUMOReal costs, SUMOReal prop,
                 const ROEdgeVector &route)
        : Named(StringUtils::convertUmlaute(id)), _costs(costs),
        _probability(prop), _route(route)
{}


RORoute::RORoute(const std::string &id, SUMOReal costs, SUMOReal prop,
                 const std::vector<const ROEdge*> &route)
        : Named(StringUtils::convertUmlaute(id)), _costs(costs),
        _probability(prop), _route(route)
{}


RORoute::~RORoute()
{}


void
RORoute::add(ROEdge *edge)
{
    _route.add(edge);
}


void
RORoute::xmlOut(std::ostream &os, bool isPeriodical) const
{
    os << "   <route id=\"" << _id << "\"";
    if (isPeriodical) {
        os << " multi_ref=\"x\"";
    }
    os << ">";
    os << _route;
    os << "</route>" << endl;
}

void
RORoute::xmlOutEdges(std::ostream &os) const
{
    os << _route;
}


SUMOReal
RORoute::getCosts() const
{
    return _costs;
}


void
RORoute::setCosts(SUMOReal costs)
{
    _costs = costs;
}


SUMOReal
RORoute::recomputeCosts(const ROVehicle *const v, SUMOTime begin) const
{
    return _route.recomputeCosts(v, begin);
}


const ROEdge *
RORoute::getFirst() const
{
    return _route.getFirst();
}


const ROEdge *
RORoute::getLast() const
{
    return _route.getLast();
}



SUMOReal
RORoute::getProbability() const
{
    return _probability;
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
RORoute::setProbability(SUMOReal prop)
{
    _probability = prop;
}


void
RORoute::pruneFirst()
{
    _route.removeFirst();
}


const ROEdgeVector &
RORoute::getEdgeVector() const
{
    return _route;
}


void
RORoute::recheckForLoops()
{
    _route.recheckForLoops();
}



/****************************************************************************/

