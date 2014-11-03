/****************************************************************************/
/// @file    ROVehicle.cpp
/// @author  Daniel Krajzewicz
/// @author  Axel Wegener
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Sept 2002
/// @version $Id$
///
// A vehicle as used by router
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2014 DLR (http://www.dlr.de/) and contributors
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

#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <utils/vehicle/SUMOVTypeParameter.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <string>
#include <iostream>
#include "RORouteDef.h"
#include "ROVehicle.h"
#include "RORoute.h"
#include "ROHelper.h"
#include "RONet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
ROVehicle::ROVehicle(const SUMOVehicleParameter& pars,
                     RORouteDef* route, const SUMOVTypeParameter* type, const RONet* net)
    : myParameter(pars), myType(type), myRoute(route), myRoutingSuccess(false) {
    myParameter.stops.clear();
    if (route != 0) {
        for (std::vector<SUMOVehicleParameter::Stop>::const_iterator s = route->getFirstRoute()->getStops().begin(); s != route->getFirstRoute()->getStops().end(); ++s) {
            addStop(*s, net);
        }
    }
    for (std::vector<SUMOVehicleParameter::Stop>::const_iterator s = pars.stops.begin(); s != pars.stops.end(); ++s) {
        addStop(*s, net);
    }
}


void
ROVehicle::addStop(const SUMOVehicleParameter::Stop& stopPar, const RONet* net) {
    const ROEdge* stopEdge = net->getEdge(stopPar.lane.substr(0, stopPar.lane.rfind("_")));
    if (stopEdge == 0) {
        // warn here?
        return;
    }
    // where to insert the stop
    std::vector<SUMOVehicleParameter::Stop>::iterator iter = myParameter.stops.begin();
    std::vector<const ROEdge*>::iterator edgeIter = myStopEdges.begin();
    if (stopPar.index == STOP_INDEX_END || stopPar.index >= static_cast<int>(myParameter.stops.size())) {
        if (myParameter.stops.size() > 0) {
            iter = myParameter.stops.end();
            edgeIter = myStopEdges.end();
        }
    } else {
        if (stopPar.index == STOP_INDEX_FIT) {
            const std::vector<const ROEdge*> edges = myRoute->getFirstRoute()->getEdgeVector();
            std::vector<const ROEdge*>::const_iterator stopEdgeIt = std::find(edges.begin(), edges.end(), stopEdge);
            if (stopEdgeIt == edges.end()) {
                iter = myParameter.stops.end();
                edgeIter = myStopEdges.end();
            } else {
                while (iter != myParameter.stops.end()) {
                    if (edgeIter > stopEdgeIt || (edgeIter == stopEdgeIt && iter->endPos >= stopPar.endPos)) {
                        break;
                    }
                    ++iter;
                    ++edgeIter;
                }
            }
        } else {
            iter += stopPar.index;
            edgeIter += stopPar.index;
        }
    }
    myParameter.stops.insert(iter, stopPar);
    myStopEdges.insert(edgeIter, stopEdge);
}


ROVehicle::~ROVehicle() {}


void
ROVehicle::saveAllAsXML(OutputDevice& os, OutputDevice* const altos,
                        OutputDevice* const typeos, bool withExitTimes) const {
    // check whether the vehicle's type was saved before
    if (myType != 0 && !myType->saved) {
        // ... save if not
        if (typeos != 0) {
            myType->write(*typeos);
        } else {
            myType->write(os);
            if (altos != 0) {
                myType->write(*altos);
            }
        }
        myType->saved = true;
    }

    // write the vehicle (new style, with included routes)
    myParameter.write(os, OptionsCont::getOptions());
    if (altos != 0) {
        myParameter.write(*altos, OptionsCont::getOptions());
    }

    // save the route
    myRoute->writeXMLDefinition(os, this, false, withExitTimes);
    if (altos != 0) {
        myRoute->writeXMLDefinition(*altos, this, true, withExitTimes);
    }
    myParameter.writeStops(os);
    if (altos != 0) {
        myParameter.writeStops(*altos);
    }
    os.closeTag();
    if (altos != 0) {
        altos->closeTag();
    }
}


SUMOReal
ROVehicle::getMaxSpeed() const {
    return myType->maxSpeed;
}


/****************************************************************************/

