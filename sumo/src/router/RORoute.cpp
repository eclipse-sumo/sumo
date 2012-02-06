/****************************************************************************/
/// @file    RORoute.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A complete router's route
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
RORoute::RORoute(const std::string& id, SUMOReal costs, SUMOReal prop,
                 const std::vector<const ROEdge*> &route,
                 const RGBColor* const color)
    : Named(StringUtils::convertUmlaute(id)), myCosts(costs),
      myProbability(prop), myRoute(route), myColor(color) {}


RORoute::RORoute(const RORoute& src)
    : Named(src.myID), myCosts(src.myCosts),
      myProbability(src.myProbability), myRoute(src.myRoute), myColor(0) {
    if (src.myColor != 0) {
        myColor = new RGBColor(*src.myColor);
    }
}


RORoute::~RORoute() {
    delete myColor;
}


void
RORoute::add(ROEdge* edge) {
    myRoute.push_back(edge);
}


void
RORoute::setCosts(SUMOReal costs) {
    myCosts = costs;
}


void
RORoute::setProbability(SUMOReal prob) {
    myProbability = prob;
}


void
RORoute::recheckForLoops() {
    ROHelper::recheckForLoops(myRoute);
}


OutputDevice&
RORoute::writeXMLDefinition(SUMOAbstractRouter<ROEdge, ROVehicle> &router,
                            OutputDevice& dev, const ROVehicle* const veh, bool asAlternatives, bool withExitTimes) const {
    UNUSED_PARAMETER(router);
    // (optional) alternatives header
    if (asAlternatives) {
        dev.openTag(SUMO_TAG_ROUTE_DISTRIBUTION).writeAttr(SUMO_ATTR_LAST, 0).closeOpener();
    }
    // the route
    dev.openTag(SUMO_TAG_ROUTE);
    if (asAlternatives) {
        dev.writeAttr(SUMO_ATTR_COST, myCosts);
        dev.setPrecision(8);
        dev.writeAttr(SUMO_ATTR_PROB, myProbability);
        dev.setPrecision();
    }
    if (myColor != 0) {
        dev.writeAttr(SUMO_ATTR_COLOR, *myColor);
    }
    dev.writeAttr(SUMO_ATTR_EDGES, myRoute);
    if (withExitTimes) {
        std::string exitTimes;
        SUMOReal time = STEPS2TIME(veh->getDepartureTime());
        for (std::vector<const ROEdge*>::const_iterator i = myRoute.begin(); i != myRoute.end(); ++i) {
            if (i != myRoute.begin()) {
                exitTimes += " ";
            }
            time += (*i)->getTravelTime(veh, time);
            exitTimes += toString(time);
        }
        dev.writeAttr("exitTimes", exitTimes);
    }
    dev.closeTag(true);
    // (optional) alternatives end
    if (asAlternatives) {
        dev.closeTag();
    }
    return dev;
}




/****************************************************************************/

