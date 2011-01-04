/****************************************************************************/
/// @file    RORoute.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A complete router's route
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/StdDefs.h>
#include "ROEdge.h"
#include "RORoute.h"
#include "ROHelper.h"
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
RORoute::RORoute(const std::string &id, SUMOReal costs, SUMOReal prop,
                 const std::vector<const ROEdge*> &route,
                 const RGBColor * const color) throw()
        : Named(StringUtils::convertUmlaute(id)), myCosts(costs),
        myProbability(prop), myRoute(route), myColor(color) {}


RORoute::RORoute(const RORoute &src) throw()
        : Named(src.myID), myCosts(src.myCosts),
        myProbability(src.myProbability), myRoute(src.myRoute), myColor(0) {
    if (src.myColor!=0) {
        myColor = new RGBColor(*src.myColor);
    }
}


RORoute::~RORoute() throw() {
    delete myColor;
}


void
RORoute::add(ROEdge *edge) throw() {
    myRoute.push_back(edge);
}


void
RORoute::setCosts(SUMOReal costs) throw() {
    myCosts = costs;
}


void
RORoute::setProbability(SUMOReal prob) throw() {
    myProbability = prob;
}


void
RORoute::recheckForLoops() throw() {
    ROHelper::recheckForLoops(myRoute);
}


OutputDevice &
RORoute::writeXMLDefinition(SUMOAbstractRouter<ROEdge,ROVehicle> &router,
                            OutputDevice &dev, const ROVehicle * const veh, bool asAlternatives, bool withExitTimes) const {
    UNUSED_PARAMETER(router);
    // (optional) alternatives header
    if (asAlternatives) {
        dev.openTag("routeDistribution") << " last=\"0\">\n";
    }
    // the route
    dev.openTag("route");
    if (asAlternatives) {
        dev << " cost=\"" << myCosts;
        dev.setPrecision(8);
        dev << "\" probability=\"" << myProbability << "\"";
        dev.setPrecision();
    }
    if (myColor!=0) {
        dev << " color=\"" << *myColor << "\"";
    }
    dev << " edges=\"" << myRoute;
    if (withExitTimes) {
        SUMOReal time = STEPS2TIME(veh->getDepartureTime());
        dev << "\" exitTimes=\"";
        std::vector<const ROEdge*>::const_iterator i = myRoute.begin();
        for (; i!=myRoute.end(); ++i) {
            if (i != myRoute.begin()) {
                dev << " ";
            }
            time += (*i)->getTravelTime(veh, (SUMOTime) time);
            dev << time;
        }
    }
    (dev << "\"").closeTag(true);
    // (optional) alternatives end
    if (asAlternatives) {
        dev.closeTag();
    }
    return dev;
}




/****************************************************************************/

