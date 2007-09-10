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
// included modules
// ===========================================================================
#ifdef _MSC_VER
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
#include <utils/iodevices/OutputDevice.h>

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
        : Named(StringUtils::convertUmlaute(id)), myCosts(costs),
        myProbability(prop), myRoute(route)
{}


RORoute::RORoute(const std::string &id, SUMOReal costs, SUMOReal prop,
                 const std::vector<const ROEdge*> &route)
        : Named(StringUtils::convertUmlaute(id)), myCosts(costs),
        myProbability(prop), myRoute(route)
{}


RORoute::~RORoute()
{}


void
RORoute::add(ROEdge *edge)
{
    myRoute.add(edge);
}


void
RORoute::xmlOut(OutputDevice &dev, bool isPeriodical) const
{
    dev << "   <route id=\"" << myID << "\"";
    if (isPeriodical) {
        dev << " multi_ref=\"x\"";
    }
    dev << ">";
    dev << myRoute;
    dev << "</route>\n";
}

void
RORoute::xmlOutEdges(OutputDevice &dev) const
{
    dev << myRoute;
}


SUMOReal
RORoute::getCosts() const
{
    return myCosts;
}


void
RORoute::setCosts(SUMOReal costs)
{
    myCosts = costs;
}


SUMOReal
RORoute::recomputeCosts(const ROVehicle *const v, SUMOTime begin) const
{
    return myRoute.recomputeCosts(v, begin);
}


const ROEdge *
RORoute::getFirst() const
{
    return myRoute.getFirst();
}


const ROEdge *
RORoute::getLast() const
{
    return myRoute.getLast();
}



SUMOReal
RORoute::getProbability() const
{
    return myProbability;
}


bool
RORoute::equals(RORoute *ro) const
{
    return myRoute.equals(ro->myRoute);
}


size_t
RORoute::size() const
{
    return myRoute.size();
}


void
RORoute::setProbability(SUMOReal prop)
{
    myProbability = prop;
}


void
RORoute::pruneFirst()
{
    myRoute.removeFirst();
}


const ROEdgeVector &
RORoute::getEdgeVector() const
{
    return myRoute;
}


void
RORoute::recheckForLoops()
{
    myRoute.recheckForLoops();
}



/****************************************************************************/

