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
    SUMOVehicleParameter pars;
    pars.departProcedure = DEPART_TRIGGERED;

    for (StringTokenizer st(vTypes); st.hasNext();) {
        const std::string vtypeid = st.next();
        SUMOVTypeParameter* type = net->getVehicleTypeSecure(vtypeid);
        if (type == 0) {
            throw InvalidArgument("The vehicle type '" + vtypeid + "' in a trip for person '" + getID() + "' is not known.");
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
ROPerson::PersonTrip::saveVehicles(OutputDevice& os, bool asAlternatives, OptionsCont& options) const {
    for (std::vector<ROVehicle*>::const_iterator it = myVehicles.begin(); it != myVehicles.end(); ++it) {
        (*it)->saveAsXML(os, asAlternatives, options);
    }
}


void
ROPerson::computeRoute(const RORouterProvider& provider,
                       const bool removeLoops, MsgHandler* errorHandler) {
    myRoutingSuccess = true;
    for (std::vector<PlanItem*>::iterator it = myPlan.begin(); it != myPlan.end(); ++it) {
        if ((*it)->needsRouting()) {
            PersonTrip* trip = static_cast<PersonTrip*>(*it);
            ConstROEdgeVector edges;
            std::vector<ROVehicle*>& vehicles = trip->getVehicles();
            if (vehicles.empty()) {
                provider.getPedestrianRouter().compute(trip->getOrigin(), trip->getDestination(), 0, 0, DEFAULT_PEDESTRIAN_SPEED, 0, 0, edges);
                if (edges.empty()) {
                    errorHandler->inform("No pedestrian route for trip in person '" + getID() + "'.");
                    myRoutingSuccess = false;
                } else {
                    trip->addTripItem(new Walk(edges));
                }
            } else {
                for (std::vector<ROVehicle*>::iterator v = vehicles.begin(); v != vehicles.end(); ) {
                    if (trip->isIntermodal()) {
                        std::vector<std::pair<std::string, ConstROEdgeVector> > result;
                        provider.getIntermodalRouter().compute(trip->getOrigin(), trip->getDestination(), 0, 0, DEFAULT_PEDESTRIAN_SPEED, *v, 0, result);
                        for (std::vector<std::pair<std::string, ConstROEdgeVector> >::const_iterator it = result.begin(); it != result.end(); ++it) {
                            const ConstROEdgeVector& edges = it->second;
                            if (!edges.empty()) {
                                const std::string& lines = it->first;
                                if (lines == "") {
                                    trip->addTripItem(new Walk(edges));
                                } else if (lines == (*v)->getID()) {
                                    trip->addTripItem(new Ride(edges.front(), edges.back(), (*v)->getID()));
                                    (*v)->getRouteDefinition()->addLoadedAlternative(new RORoute((*v)->getID() + "_RouteDef", edges));
                                } else {
                                    trip->addTripItem(new Ride(edges.front(), edges.back(), lines));
                                }
                            }
                        }
                    } else {
                        provider.getVehicleRouter().compute(trip->getOrigin(), trip->getDestination(), *v, 0, edges);
                        if (edges.empty()) {
                            errorHandler->inform("No route for trip in person '" + getID() + "'.");
                            myRoutingSuccess = false;
                            v = vehicles.erase(v);
                            continue;
                        }
                        trip->addTripItem(new Ride(edges.front(), edges.back(), (*v)->getID()));
                        (*v)->getRouteDefinition()->addLoadedAlternative(new RORoute((*v)->getID() + "_RouteDef", edges));
                    }
                    ++v;
                }
            }
        }
    }
}


void
ROPerson::saveAsXML(OutputDevice& os, bool asAlternatives, OptionsCont& options) const {
    // write the person's vehicles
    for (std::vector<PlanItem*>::const_iterator it = myPlan.begin(); it != myPlan.end(); ++it) {
        (*it)->saveVehicles(os, asAlternatives, options);
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

