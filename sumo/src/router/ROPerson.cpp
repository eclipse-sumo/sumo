/****************************************************************************/
/// @file    ROPerson.cpp
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
// Copyright (C) 2002-2015 DLR (http://www.dlr.de/) and contributors
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

#include <utils/common/StringTokenizer.h>
#include <utils/common/ToString.h>
#include <utils/common/TplConvert.h>
#include <utils/common/MsgHandler.h>
#include <utils/vehicle/SUMOVTypeParameter.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <string>
#include <iostream>
#include "RORouteDef.h"
#include "ROPerson.h"
#include "RORoute.h"
#include "ROVehicle.h"
#include "ROHelper.h"
#include "RONet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
ROPerson::ROPerson(const SUMOVehicleParameter& pars, const SUMOVTypeParameter* type)
    : RORoutable(pars, type) {
}


ROPerson::~ROPerson() {
    for (std::vector<PlanItem*>::const_iterator it = myPlan.begin(); it != myPlan.end(); ++it) {
        delete *it;
    }
}


void
ROPerson::addTrip(const ROEdge* const from, const ROEdge* const to, const SVCPermissions modeSet,
                  const std::string& vTypes, const std::string& busStop) {
    PersonTrip* trip = new PersonTrip(from, to, modeSet, busStop);
    RONet* net = RONet::getInstance();
    SUMOVehicleParameter* pars = new SUMOVehicleParameter();
    for (StringTokenizer st(vTypes); st.hasNext();) {
        const std::string vtypeid = st.next();
        SUMOVTypeParameter* type = net->getVehicleTypeSecure(vtypeid);
        if (type == 0) {
            throw InvalidArgument("The vehicle type '" + vtypeid + "' in a trip for person '" + getID() + "' is not known.");
        }
        trip->addVehicle(new ROVehicle(*pars, 0, type, net));
    }
    if ((modeSet & SVC_PASSENGER) != 0 && trip->getVehicles().empty()) {
        trip->addVehicle(new ROVehicle(*pars, 0, net->getVehicleTypeSecure(DEFAULT_VTYPE_ID), net));
    }
    myPlan.push_back(trip);
}

void
ROPerson::addRide(const ROEdge* const from, const ROEdge* const to, const std::string& lines) {
    if (myPlan.empty() || myPlan.back()->isStop()) {
        myPlan.push_back(new PersonTrip());
    }
    myPlan.back()->addTripItem(new Ride(from, to, lines));
}


void
ROPerson::addWalk(const SUMOReal duration, const SUMOReal speed, const ConstROEdgeVector& edges, const SUMOReal departPos, const SUMOReal arrivalPos, const std::string& busStop) {
    if (myPlan.empty() || myPlan.back()->isStop()) {
        myPlan.push_back(new PersonTrip());
    }
    myPlan.back()->addTripItem(new Walk(duration, speed, edges, departPos, arrivalPos, busStop));
}


void
ROPerson::addStop(const SUMOVehicleParameter::Stop& stopPar, const ROEdge* const stopEdge) {
    myPlan.push_back(new Stop(stopPar, stopEdge));
}


void
ROPerson::Ride::saveAsXML(OutputDevice& os) const {
    os.openTag(SUMO_TAG_RIDE);
    os.writeAttr(SUMO_ATTR_FROM, from->getID());
    os.writeAttr(SUMO_ATTR_TO, to->getID());
    os.writeAttr(SUMO_ATTR_LINES, lines);
    os.closeTag();
}


void
ROPerson::Walk::saveAsXML(OutputDevice& os) const {
    os.openTag(SUMO_TAG_WALK);
    if (dur > 0) {
        os.writeAttr(SUMO_ATTR_DURATION, dur);
    }
    if (v > 0) {
        os.writeAttr(SUMO_ATTR_SPEED, v);
    }
    os.writeAttr(SUMO_ATTR_EDGES, edges);
    if (dep != std::numeric_limits<SUMOReal>::infinity()) {
        os.writeAttr(SUMO_ATTR_DEPARTPOS, dep);
    }
    if (arr != std::numeric_limits<SUMOReal>::infinity()) {
        os.writeAttr(SUMO_ATTR_ARRIVALPOS, arr);
    }
    if (busStop != "") {
        os.writeAttr(SUMO_ATTR_BUS_STOP, busStop);
    }
    os.closeTag();
}


void
ROPerson::computeRoute(const RORouterProvider& provider,
                       const bool removeLoops, MsgHandler* errorHandler) {
    myRoutingSuccess = true;
    for (std::vector<PlanItem*>::iterator it = myPlan.begin(); it != myPlan.end(); ++it) {
        if ((*it)->needsRouting()) {
            PersonTrip* trip = static_cast<PersonTrip*>(*it);
            ConstROEdgeVector edges;
            const std::vector<ROVehicle*>& vehicles = trip->getVehicles();
            if (vehicles.empty()) {
                provider.getPedestrianRouter().compute(trip->getOrigin(), trip->getDestination(), 0, 0, DEFAULT_PEDESTRIAN_SPEED, 0, 0, edges);
                if (edges.empty()) {
                    errorHandler->inform("No connection found between '" + trip->getOrigin()->getID() + "' and '" + trip->getDestination()->getID() + "' for person '" + getID() + "'.");
                    myRoutingSuccess = false;
                } else {
                    trip->addTripItem(new Walk(edges));
                }
            } else {
                for (std::vector<ROVehicle*>::const_iterator v = vehicles.begin(); v != vehicles.end(); ++v) {
                    if (trip->isIntermodal()) {
                        provider.getIntermodalRouter().compute(trip->getOrigin(), trip->getDestination(), *v, 0, edges);
                    } else {
                        provider.getVehicleRouter().compute(trip->getOrigin(), trip->getDestination(), *v, 0, edges);
                    }
                }
            }
        }
    }
}


void
ROPerson::saveAsXML(OutputDevice& os, bool asAlternatives, OptionsCont& options) const {
    // write the person
    myParameter.write(os, options, SUMO_TAG_PERSON);

    for (std::vector<PlanItem*>::const_iterator it = myPlan.begin(); it != myPlan.end(); ++it) {
        (*it)->saveAsXML(os);
    }

    for (std::map<std::string, std::string>::const_iterator j = myParameter.getMap().begin(); j != myParameter.getMap().end(); ++j) {
        os.openTag(SUMO_TAG_PARAM);
        os.writeAttr(SUMO_ATTR_KEY, (*j).first);
        os.writeAttr(SUMO_ATTR_VALUE, (*j).second);
        os.closeTag();
    }
    os.closeTag();
}


/****************************************************************************/

