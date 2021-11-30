/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2021 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    RORoute.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Yun-Pang Floetteroed
/// @date    Sept 2002
///
// A complete router's route
/****************************************************************************/
#include <config.h>

#include <string>
#include <iostream>
#include <utils/common/Named.h>
#include <utils/common/StringUtils.h>
#include <utils/common/StdDefs.h>
#include "ROEdge.h"
#include "RORoute.h"
#include "ROHelper.h"
#include <utils/iodevices/OutputDevice.h>


// ===========================================================================
// method definitions
// ===========================================================================
RORoute::RORoute(const std::string& id, double costs, double prop,
                 const ConstROEdgeVector& route,
                 const RGBColor* const color,
                 const std::vector<SUMOVehicleParameter::Stop>& stops)
    : Named(StringUtils::convertUmlaute(id)), myCosts(costs),
      myProbability(prop), myRoute(route), myColor(color), myStops(stops) {}

RORoute::RORoute(const std::string& id, const ConstROEdgeVector& route)
    : Named(StringUtils::convertUmlaute(id)), myCosts(0.0),
      myProbability(0.0), myRoute(route), myColor(nullptr), myStops() {}

RORoute::RORoute(const RORoute& src)
    : Named(src.myID), myCosts(src.myCosts),
      myProbability(src.myProbability), myRoute(src.myRoute), myColor(nullptr) {
    if (src.myColor != nullptr) {
        myColor = new RGBColor(*src.myColor);
    }
}


RORoute::~RORoute() {
    delete myColor;
}


void
RORoute::setCosts(double costs) {
    myCosts = costs;
}


void
RORoute::setProbability(double prob) {
    myProbability = prob;
}


void
RORoute::recheckForLoops(const ConstROEdgeVector& mandatory) {
    ROHelper::recheckForLoops(myRoute, mandatory);
}

void
RORoute::addProbability(double prob) {
    myProbability += prob;
}


ConstROEdgeVector
RORoute::getNormalEdges() const {
    ConstROEdgeVector tempRoute;
    for (const ROEdge* roe : myRoute) {
        if (!roe->isInternal() && !roe->isTazConnector()) {
            tempRoute.push_back(roe);
        }
    }
    return tempRoute;
}


OutputDevice&
RORoute::writeXMLDefinition(OutputDevice& dev, const ROVehicle* const veh,
                            const bool withCosts,
                            const bool withProb,
                            const bool withExitTimes,
                            const bool withLength,
                            const std::string& id) const {
    dev.openTag(SUMO_TAG_ROUTE);
    if (id != "") {
        dev.writeAttr(SUMO_ATTR_ID, id);
    }
    if (withCosts) {
        dev.writeAttr(SUMO_ATTR_COST, myCosts);
        dev.setPrecision(8);
    }
    if (withProb) {
        dev.writeAttr(SUMO_ATTR_PROB, myProbability);
        dev.setPrecision();
    }
    if (myColor != nullptr) {
        dev.writeAttr(SUMO_ATTR_COLOR, *myColor);
    }
    dev.writeAttr(SUMO_ATTR_EDGES, getNormalEdges());
    if (withExitTimes) {
        std::vector<double> exitTimes;
        double time = STEPS2TIME(veh->getDepartureTime());
        for (const ROEdge* const roe : myRoute) {
            time += roe->getTravelTime(veh, time);
            if (!roe->isInternal() && !roe->isTazConnector()) {
                exitTimes.push_back(time);
            }
        }
        dev.writeAttr("exitTimes", exitTimes);
    }
    if (withLength) {
        double length = 0.;
        for (const ROEdge* const roe : myRoute) {
            length += roe->getLength();
        }
        dev.writeAttr("routeLength", length);
    }
    dev.closeTag();
    return dev;
}


/****************************************************************************/
