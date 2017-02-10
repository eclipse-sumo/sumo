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
// Copyright (C) 2002-2017 DLR (http://www.dlr.de/) and contributors
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
                  const std::string& vTypes, const SUMOReal departPos, const SUMOReal arrivalPos, const std::string& busStop, SUMOReal walkFactor) {
    PersonTrip* trip = new PersonTrip(from, to, modeSet, departPos, arrivalPos, busStop, walkFactor);
    RONet* net = RONet::getInstance();
    SUMOVehicleParameter pars;
    pars.departProcedure = DEPART_TRIGGERED;

    for (StringTokenizer st(vTypes); st.hasNext();) {
        pars.vtypeid = st.next();
        pars.setParameter |= VEHPARS_VTYPE_SET;
        SUMOVTypeParameter* type = net->getVehicleTypeSecure(pars.vtypeid);
        if (type == 0) {
            delete trip;
            throw InvalidArgument("The vehicle type '" + pars.vtypeid + "' in a trip for person '" + getID() + "' is not known.");
        }
        pars.id = getID() + "_" + toString(trip->getVehicles().size());
        trip->addVehicle(new ROVehicle(pars, new RORouteDef("!" + pars.id, 0, false, false), type, net));
    }
    if ((modeSet & SVC_PASSENGER) != 0 && trip->getVehicles().empty()) {
        pars.id = getID() + "_0";
        trip->addVehicle(new ROVehicle(pars, new RORouteDef("!" + pars.id, 0, false, false), net->getVehicleTypeSecure(DEFAULT_VTYPE_ID), net));
    }
    myPlan.push_back(trip);
}


void
ROPerson::addRide(const ROEdge* const from, const ROEdge* const to, const std::string& lines, const std::string& destStop) {
    if (myPlan.empty() || myPlan.back()->isStop()) {
        myPlan.push_back(new PersonTrip());
    }
    myPlan.back()->addTripItem(new Ride(from, to, lines, destStop));
}


void
ROPerson::addWalk(const ConstROEdgeVector& edges, const SUMOReal duration, const SUMOReal speed, const SUMOReal departPos, const SUMOReal arrivalPos, const std::string& busStop) {
    if (myPlan.empty() || myPlan.back()->isStop()) {
        myPlan.push_back(new PersonTrip());
    }
    myPlan.back()->addTripItem(new Walk(edges, duration, speed, departPos, arrivalPos, busStop));
}


void
ROPerson::addStop(const SUMOVehicleParameter::Stop& stopPar, const ROEdge* const stopEdge) {
    myPlan.push_back(new Stop(stopPar, stopEdge));
}


void
ROPerson::Ride::saveAsXML(OutputDevice& os) const {
    os.openTag(SUMO_TAG_RIDE);
    if (from != 0) {
        os.writeAttr(SUMO_ATTR_FROM, from->getID());
    }
    if (to != 0) {
        os.writeAttr(SUMO_ATTR_TO, to->getID());
    }
    if (destStop != "") {
        os.writeAttr(SUMO_ATTR_BUS_STOP, destStop);
    }
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
    if (destStop != "") {
        os.writeAttr(SUMO_ATTR_BUS_STOP, destStop);
    }
    os.closeTag();
}


void
ROPerson::PersonTrip::saveVehicles(OutputDevice& os, OutputDevice* const typeos, bool asAlternatives, OptionsCont& options) const {
    for (std::vector<ROVehicle*>::const_iterator it = myVehicles.begin(); it != myVehicles.end(); ++it) {
        (*it)->saveAsXML(os, typeos, asAlternatives, options);
    }
}


bool
ROPerson::computeIntermodal(const RORouterProvider& provider, PersonTrip* const trip, const ROVehicle* const veh, MsgHandler* const errorHandler) {
    std::vector<ROIntermodalRouter::TripItem> result;
    provider.getIntermodalRouter().compute(trip->getOrigin(), trip->getDestination(), trip->getDepartPos(), trip->getArrivalPos(),
                                           myType->maxSpeed * trip->getWalkFactor(), veh, trip->getModes(), myParameter.depart, result);
    bool carUsed = false;
    for (std::vector<ROIntermodalRouter::TripItem>::const_iterator it = result.begin(); it != result.end(); ++it) {
        if (!it->edges.empty()) {
            if (it->line == "") {
                if (it + 1 == result.end() && !trip->hasBusStopDest()) {
                    trip->addTripItem(new Walk(it->edges));
                } else {
                    trip->addTripItem(new Walk(it->edges, it->destStop));
                }
            } else if (veh != 0 && it->line == veh->getID()) {
                trip->addTripItem(new Ride(it->edges.front(), it->edges.back(), veh->getID(), it->destStop));
                veh->getRouteDefinition()->addLoadedAlternative(new RORoute(veh->getID() + "_RouteDef", it->edges));
                carUsed = true;
            } else {
                trip->addTripItem(new Ride(0, 0, it->line, it->destStop));
            }
        }
    }
    if (result.empty()) {
        errorHandler->inform("No route for trip in person '" + getID() + "'.");
        myRoutingSuccess = false;
    }
    return carUsed;
}


void
ROPerson::computeRoute(const RORouterProvider& provider,
                       const bool /* removeLoops */, MsgHandler* errorHandler) {
    myRoutingSuccess = true;
    for (std::vector<PlanItem*>::iterator it = myPlan.begin(); it != myPlan.end(); ++it) {
        if ((*it)->needsRouting()) {
            PersonTrip* trip = static_cast<PersonTrip*>(*it);
            ConstROEdgeVector edges;
            std::vector<ROVehicle*>& vehicles = trip->getVehicles();
            if (vehicles.empty()) {
                computeIntermodal(provider, trip, 0, errorHandler);
            } else {
                for (std::vector<ROVehicle*>::iterator v = vehicles.begin(); v != vehicles.end();) {
                    if (!computeIntermodal(provider, trip, *v, errorHandler)) {
                        v = vehicles.erase(v);
                    } else {
                        ++v;
                    }
                }
            }
        }
    }
}


void
ROPerson::saveAsXML(OutputDevice& os, OutputDevice* const typeos, bool asAlternatives, OptionsCont& options) const {
    // write the person's vehicles
    for (std::vector<PlanItem*>::const_iterator it = myPlan.begin(); it != myPlan.end(); ++it) {
        (*it)->saveVehicles(os, typeos, asAlternatives, options);
    }

    if (typeos != 0  && myType != 0 && !myType->saved) {
        myType->write(*typeos);
        myType->saved = true;
    }
    if (myType != 0 && !myType->saved) {
        myType->write(os);
        myType->saved = asAlternatives;
    }

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

