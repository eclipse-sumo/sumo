/****************************************************************************/
/// @file    RORoute.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A complete router's route
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
#include "ROEdge.h"
#include "RORoute.h"
#include "ROHelper.h"
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
                 const std::vector<const ROEdge*> &route,
                 const RGBColor &color) throw()
        : Named(StringUtils::convertUmlaute(id)), myCosts(costs),
        myProbability(prop), myRoute(route), myColor(color)
{}


RORoute::RORoute(const RORoute &src) throw()
    : Named(src.myID), myCosts(src.myCosts), 
    myProbability(src.myProbability), myRoute(src.myRoute),
    myColor(src.myColor)
{
}


RORoute::~RORoute() throw()
{}


void
RORoute::add(ROEdge *edge) throw()
{
    myRoute.push_back(edge);
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


void
RORoute::setCosts(SUMOReal costs) throw()
{
    myCosts = costs;
}


void
RORoute::setProbability(SUMOReal prob) throw()
{
    myProbability = prob;
}


void
RORoute::pruneFirst() throw()
{
    myRoute.erase(myRoute.begin());
}


void
RORoute::recheckForLoops() throw()
{
    ROHelper::recheckForLoops(myRoute);
}


OutputDevice &
RORoute::writeXMLDefinition(OutputDevice &dev, bool asAlternatives) const
{
    // (optional) alternatives header
    if(asAlternatives) {
        dev << "<routealt last=\"0\"";
        dev << ">\n         ";
    }
    // the route
    dev << "<route";
    if(asAlternatives) {
        dev << " cost=\"" << myCosts;
        dev << "\" probability=\"" << myProbability << "\"";
    }
    if(myColor!=RGBColor()) {
        dev << " color=\"" << myColor << "\"";
    }
    dev << ">" << myRoute << "</route>\n";
    // (optional) alternatives end
    if(asAlternatives) {
        dev << "      </routealt>\n";
    }
    return dev;
}




/****************************************************************************/

