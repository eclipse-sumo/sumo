/****************************************************************************/
/// @file    MSRouteHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Parser and container for routes during their loading
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include <map>
#include <vector>
#include <microsim/MSRoute.h>
#include <microsim/MSEdge.h>
#include <microsim/MSJunction.h>
#include <microsim/MSVehicleType.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSLane.h>
#include "MSRouteHandler.h"
#include "MSPersonControl.h"
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/options/OptionsCont.h>
#include "MSNet.h"

#include <microsim/trigger/MSBusStop.h>
#include <microsim/MSGlobals.h>
#include <utils/xml/SUMOVehicleParserHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static members
// ===========================================================================
MTRand MSRouteHandler::myParsingRNG;


// ===========================================================================
// method definitions
// ===========================================================================
MSRouteHandler::MSRouteHandler(const std::string& file,
                               bool addVehiclesDirectly) :
    SUMORouteHandler(file),
    myActivePlan(0),
    myAddVehiclesDirectly(addVehiclesDirectly),
    myCurrentVTypeDistribution(0),
    myCurrentRouteDistribution(0) {
    myActiveRoute.reserve(100);
}


MSRouteHandler::~MSRouteHandler() {
}


void
MSRouteHandler::myStartElement(int element,
                               const SUMOSAXAttributes& attrs) {
    SUMORouteHandler::myStartElement(element, attrs);
    switch (element) {
        case SUMO_TAG_PERSON:
            myActivePlan = new MSPerson::MSPersonPlan();
            break;
        case SUMO_TAG_RIDE: {
            const std::string pid = myVehicleParameter->id;
            bool ok = true;
            MSEdge* from = 0;
            const std::string desc = attrs.get<std::string>(SUMO_ATTR_LINES, pid.c_str(), ok);
            StringTokenizer st(desc);
            std::string bsID = attrs.getOpt<std::string>(SUMO_ATTR_BUS_STOP, 0, ok, "");
            MSBusStop* bs = 0;
            if (bsID != "") {
                bs = MSNet::getInstance()->getBusStop(bsID);
                if (bs == 0) {
                    throw ProcessError("Unknown bus stop '" + bsID + "' for person '" + myVehicleParameter->id + "'.");
                }
            }
            if (attrs.hasAttribute(SUMO_ATTR_FROM)) {
                const std::string fromID = attrs.get<std::string>(SUMO_ATTR_FROM, pid.c_str(), ok);
                from = MSEdge::dictionary(fromID);
                if (from == 0) {
                    throw ProcessError("The from edge '" + fromID + "' within a ride of person '" + pid + "' is not known.");
                }
                if (!myActivePlan->empty() && &myActivePlan->back()->getDestination() != from) {
                    throw ProcessError("Disconnected plan for person '" + myVehicleParameter->id + "' (" + fromID + "!=" + myActivePlan->back()->getDestination().getID() + ").");
                }
                if (myActivePlan->empty()) {
                    myActivePlan->push_back(new MSPerson::MSPersonStage_Waiting(
                                                *from, -1, myVehicleParameter->depart, myVehicleParameter->departPos, "start"));
                }
            } else if (myActivePlan->empty()) {
                throw ProcessError("The start edge within for person '" + pid + "' is not known.");
            }
            const std::string toID = attrs.get<std::string>(SUMO_ATTR_TO, pid.c_str(), ok);
            MSEdge* to = MSEdge::dictionary(toID);
            if (to == 0) {
                throw ProcessError("The to edge '" + toID + "' within a ride of person '" + pid + "' is not known.");
            }
            myActivePlan->push_back(new MSPerson::MSPersonStage_Driving(*to, bs, st.getVector()));
            break;
        }
        case SUMO_TAG_WALK: {
            myActiveRoute.clear();
            bool ok = true;
            SUMOReal departPos = attrs.getOpt<SUMOReal>(SUMO_ATTR_DEPARTPOS, myVehicleParameter->id.c_str(), ok, 0);
            SUMOReal arrivalPos = attrs.getOpt<SUMOReal>(SUMO_ATTR_ARRIVALPOS, myVehicleParameter->id.c_str(), ok, -NUMERICAL_EPS);
            const SUMOTime duration = attrs.getOptSUMOTimeReporting(SUMO_ATTR_DURATION, 0, ok, -1);
            if (attrs.hasAttribute(SUMO_ATTR_DURATION) && duration <= 0) {
                throw ProcessError("Non-positive walking duration for  '" + myVehicleParameter->id + "'.");
            }
            SUMOReal speed = DEFAULT_PEDESTRIAN_SPEED;
            const MSVehicleType* vtype = MSNet::getInstance()->getVehicleControl().getVType(myVehicleParameter->vtypeid, &myParsingRNG);
            // need to check for explicitly set speed since we might have // DEFAULT_VEHTYPE
            if (vtype != 0 && vtype->wasSet(VTYPEPARS_MAXSPEED_SET)) {
                speed = vtype->getMaxSpeed();
            }
            speed = attrs.getOpt<SUMOReal>(SUMO_ATTR_SPEED, 0, ok, speed);
            if (speed <= 0) {
                throw ProcessError("Non-positive walking speed for  '" + myVehicleParameter->id + "'.");
            }
            std::string bsID = attrs.getOpt<std::string>(SUMO_ATTR_BUS_STOP, 0, ok, "");
            MSBusStop* bs = 0;
            if (bsID != "") {
                bs = MSNet::getInstance()->getBusStop(bsID);
                if (bs == 0) {
                    throw ProcessError("Unknown bus stop '" + bsID + "' for person '" + myVehicleParameter->id + "'.");
                }
                arrivalPos = bs->getEndLanePosition();
            }
            if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
                MSEdge::parseEdgesList(attrs.get<std::string>(SUMO_ATTR_EDGES, myVehicleParameter->id.c_str(), ok), myActiveRoute, myActiveRouteID);
            } else {
                if (attrs.hasAttribute(SUMO_ATTR_FROM) && attrs.hasAttribute(SUMO_ATTR_TO)) {
                    const std::string fromID = attrs.get<std::string>(SUMO_ATTR_FROM, myVehicleParameter->id.c_str(), ok);
                    MSEdge* from = MSEdge::dictionary(fromID);
                    if (from == 0) {
                        throw ProcessError("The from edge '" + fromID + "' within a walk of person '" + myVehicleParameter->id + "' is not known.");
                    }
                    const std::string toID = attrs.get<std::string>(SUMO_ATTR_TO, myVehicleParameter->id.c_str(), ok);
                    MSEdge* to = MSEdge::dictionary(toID);
                    if (to == 0) {
                        throw ProcessError("The to edge '" + toID + "' within a walk of person '" + myVehicleParameter->id + "' is not known.");
                    }
                    MSNet::getInstance()->getPedestrianRouter().compute(from, to,
                            SUMOVehicleParameter::interpretEdgePos(departPos, from->getLength(), SUMO_ATTR_DEPARTPOS, "person walking from " + from->getID()),
                            SUMOVehicleParameter::interpretEdgePos(arrivalPos, to->getLength(), SUMO_ATTR_ARRIVALPOS, "person walking to " + to->getID()),
                            speed, 0, 0, myActiveRoute);
                    if (myActiveRoute.empty()) {
                        const std::string error = "No connection found between '" + from->getID() + "' and '" + to->getID() + "' for person '" + myVehicleParameter->id + "'.";
                        if (OptionsCont::getOptions().getBool("ignore-route-errors")) {
                            myActiveRoute.push_back(from);
                            // XXX
                            //myActiveRoute.push_back(to);
                            //WRITE_WARNING(error);
                        } else {
                            WRITE_ERROR(error);
                        }
                    }
                    //std::cout << myVehicleParameter->id << " edges=" << toString(myActiveRoute) << "\n";
                }
            }
            if (myActiveRoute.empty()) {
                throw ProcessError("No edges to walk for person '" + myVehicleParameter->id + "'.");
            }
            if (!myActivePlan->empty() && &myActivePlan->back()->getDestination() != myActiveRoute.front()) {
                throw ProcessError("Disconnected plan for person '" + myVehicleParameter->id + "' (" + myActiveRoute.front()->getID() + "!=" + myActivePlan->back()->getDestination().getID() + ").");
            }
            if (myActivePlan->empty()) {
                myActivePlan->push_back(new MSPerson::MSPersonStage_Waiting(
                                            *myActiveRoute.front(), -1, myVehicleParameter->depart, departPos, "start"));
            }
            myActivePlan->push_back(new MSPerson::MSPersonStage_Walking(myActiveRoute, bs, duration, speed, departPos, arrivalPos));
            myActiveRoute.clear();
            break;
        }
        case SUMO_TAG_FLOW:
            if (attrs.hasAttribute(SUMO_ATTR_FROM) && attrs.hasAttribute(SUMO_ATTR_TO)) {
                myActiveRouteID = "!" + myVehicleParameter->id;
                bool ok = true;
                MSEdge::parseEdgesList(attrs.get<std::string>(SUMO_ATTR_FROM, myVehicleParameter->id.c_str(), ok),
                                       myActiveRoute, "for vehicle '" + myVehicleParameter->id + "'");
                MSEdge::parseEdgesList(attrs.get<std::string>(SUMO_ATTR_TO, myVehicleParameter->id.c_str(), ok),
                                       myActiveRoute, "for vehicle '" + myVehicleParameter->id + "'");
                closeRoute(true);
                myVehicleParameter->setParameter |= VEHPARS_FORCE_REROUTE;
            }
            break;
        case SUMO_TAG_TRIP: {
            bool ok = true;
            if (attrs.hasAttribute(SUMO_ATTR_FROM) || !myVehicleParameter->wasSet(VEHPARS_TAZ_SET)) {
                MSEdge::parseEdgesList(attrs.get<std::string>(SUMO_ATTR_FROM, myVehicleParameter->id.c_str(), ok),
                                       myActiveRoute, "for vehicle '" + myVehicleParameter->id + "'");
                MSEdge::parseEdgesList(attrs.get<std::string>(SUMO_ATTR_TO, myVehicleParameter->id.c_str(), ok),
                                       myActiveRoute, "for vehicle '" + myVehicleParameter->id + "'");
            } else {
                const MSEdge* fromTaz = MSEdge::dictionary(myVehicleParameter->fromTaz + "-source");
                if (fromTaz == 0) {
                    WRITE_ERROR("Source district '" + myVehicleParameter->fromTaz + "' not known for '" + myVehicleParameter->id + "'!");
                } else if (fromTaz->getNumSuccessors() == 0) {
                    WRITE_ERROR("Source district '" + myVehicleParameter->fromTaz + "' has no outgoing edges for '" + myVehicleParameter->id + "'!");
                } else {
                    myActiveRoute.push_back(fromTaz->getSuccessor(0));
                }
            }
            closeRoute(true);
            closeVehicle();
        }
        break;
        default:
            break;
    }
    // parse embedded vtype information
    if (myCurrentVType != 0 && element != SUMO_TAG_VTYPE && element != SUMO_TAG_PARAM) {
        SUMOVehicleParserHelper::parseVTypeEmbedded(*myCurrentVType, element, attrs);
        return;
    }
}


void
MSRouteHandler::openVehicleTypeDistribution(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    myCurrentVTypeDistributionID = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (ok) {
        myCurrentVTypeDistribution = new RandomDistributor<MSVehicleType*>();
        if (attrs.hasAttribute(SUMO_ATTR_VTYPES)) {
            const std::string vTypes = attrs.get<std::string>(SUMO_ATTR_VTYPES, myCurrentVTypeDistributionID.c_str(), ok);
            StringTokenizer st(vTypes);
            while (st.hasNext()) {
                std::string vtypeID = st.next();
                MSVehicleType* type = MSNet::getInstance()->getVehicleControl().getVType(vtypeID, &myParsingRNG);
                if (type == 0) {
                    throw ProcessError("Unknown vtype '" + vtypeID + "' in distribution '" + myCurrentVTypeDistributionID + "'.");
                }
                myCurrentVTypeDistribution->add(type->getDefaultProbability(), type);
            }
        }
    }
}


void
MSRouteHandler::closeVehicleTypeDistribution() {
    if (myCurrentVTypeDistribution != 0) {
        if (MSGlobals::gStateLoaded && MSNet::getInstance()->getVehicleControl().hasVTypeDistribution(myCurrentVTypeDistributionID)) {
            delete myCurrentVTypeDistribution;
            return;
        }
        if (myCurrentVTypeDistribution->getOverallProb() == 0) {
            delete myCurrentVTypeDistribution;
            throw ProcessError("Vehicle type distribution '" + myCurrentVTypeDistributionID + "' is empty.");
        }
        if (!MSNet::getInstance()->getVehicleControl().addVTypeDistribution(myCurrentVTypeDistributionID, myCurrentVTypeDistribution)) {
            delete myCurrentVTypeDistribution;
            throw ProcessError("Another vehicle type (or distribution) with the id '" + myCurrentVTypeDistributionID + "' exists.");
        }
        myCurrentVTypeDistribution = 0;
    }
}


void
MSRouteHandler::openRoute(const SUMOSAXAttributes& attrs) {
    // check whether the id is really necessary
    std::string rid;
    if (myCurrentRouteDistribution != 0) {
        myActiveRouteID = myCurrentRouteDistributionID + "#" + toString(myCurrentRouteDistribution->getProbs().size()); // !!! document this
        rid =  "distribution '" + myCurrentRouteDistributionID + "'";
    } else if (myVehicleParameter != 0) {
        // ok, a vehicle is wrapping the route,
        //  we may use this vehicle's id as default
        myActiveRouteID = "!" + myVehicleParameter->id; // !!! document this
        if (attrs.hasAttribute(SUMO_ATTR_ID)) {
            WRITE_WARNING("Ids of internal routes are ignored (vehicle '" + myVehicleParameter->id + "').");
        }
    } else {
        bool ok = true;
        myActiveRouteID = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok, false);
        if (!ok) {
            return;
        }
        rid = "'" + myActiveRouteID + "'";
    }
    if (myVehicleParameter != 0) { // have to do this here for nested route distributions
        rid =  "for vehicle '" + myVehicleParameter->id + "'";
    }
    bool ok = true;
    if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
        MSEdge::parseEdgesList(attrs.get<std::string>(SUMO_ATTR_EDGES, myActiveRouteID.c_str(), ok), myActiveRoute, rid);
    }
    myActiveRouteRefID = attrs.getOpt<std::string>(SUMO_ATTR_REFID, myActiveRouteID.c_str(), ok, "");
    if (myActiveRouteRefID != "" && MSRoute::dictionary(myActiveRouteRefID, &myParsingRNG) == 0) {
        WRITE_ERROR("Invalid reference to route '" + myActiveRouteRefID + "' in route " + rid + ".");
    }
    myActiveRouteProbability = attrs.getOpt<SUMOReal>(SUMO_ATTR_PROB, myActiveRouteID.c_str(), ok, DEFAULT_VEH_PROB);
    myActiveRouteColor = attrs.hasAttribute(SUMO_ATTR_COLOR) ? new RGBColor(attrs.get<RGBColor>(SUMO_ATTR_COLOR, myActiveRouteID.c_str(), ok)) : 0;
    myCurrentCosts = attrs.getOpt<SUMOReal>(SUMO_ATTR_COST, myActiveRouteID.c_str(), ok, -1);
    if (ok && myCurrentCosts != -1 && myCurrentCosts < 0) {
        WRITE_ERROR("Invalid cost for route '" + myActiveRouteID + "'.");
    }
}


void
MSRouteHandler::myEndElement(int element) {
    SUMORouteHandler::myEndElement(element);
    switch (element) {
        case SUMO_TAG_VTYPE: {
            MSVehicleType* vehType = MSVehicleType::build(*myCurrentVType);
            delete myCurrentVType;
            myCurrentVType = 0;
            if (!MSNet::getInstance()->getVehicleControl().addVType(vehType)) {
                const std::string id = vehType->getID();
                delete vehType;
                if (!MSGlobals::gStateLoaded) {
                    throw ProcessError("Another vehicle type (or distribution) with the id '" + id + "' exists.");
                }
            } else {
                if (myCurrentVTypeDistribution != 0) {
                    myCurrentVTypeDistribution->add(vehType->getDefaultProbability(), vehType);
                }
            }
        }
        break;
        default:
            break;
    }
}


void
MSRouteHandler::closeRoute(const bool /* mayBeDisconnected */) {
    if (myActiveRoute.size() == 0) {
        delete myActiveRouteColor;
        myActiveRouteColor = 0;
        if (myActiveRouteRefID != "" && myCurrentRouteDistribution != 0) {
            const MSRoute* route = MSRoute::dictionary(myActiveRouteRefID, &myParsingRNG);
            if (route != 0) {
                if (myCurrentRouteDistribution->add(myActiveRouteProbability, route)) {
                    route->addReference();
                }
            }
            myActiveRouteID = "";
            myActiveRouteRefID = "";
            return;
        }
        if (myVehicleParameter != 0) {
            throw ProcessError("Vehicle's '" + myVehicleParameter->id + "' route has no edges.");
        } else {
            throw ProcessError("Route '" + myActiveRouteID + "' has no edges.");
        }
    }
    MSRoute* route = new MSRoute(myActiveRouteID, myActiveRoute,
                                 myVehicleParameter == 0 || myVehicleParameter->repetitionNumber >= 1,
                                 myActiveRouteColor, myActiveRouteStops);
    route->setCosts(myCurrentCosts);
    myActiveRoute.clear();
    if (!MSRoute::dictionary(myActiveRouteID, route)) {
        delete route;
        if (!MSGlobals::gStateLoaded) {
            if (myVehicleParameter != 0) {
                if (MSNet::getInstance()->getVehicleControl().getVehicle(myVehicleParameter->id) == 0) {
                    throw ProcessError("Another route for vehicle '" + myVehicleParameter->id + "' exists.");
                } else {
                    throw ProcessError("A vehicle with id '" + myVehicleParameter->id + "' already exists.");
                }
            } else {
                throw ProcessError("Another route (or distribution) with the id '" + myActiveRouteID + "' exists.");
            }
        }
    } else {
        if (myCurrentRouteDistribution != 0) {
            if (myCurrentRouteDistribution->add(myActiveRouteProbability, route)) {
                route->addReference();
            }
        }
    }
    myActiveRouteID = "";
    myActiveRouteColor = 0;
    myActiveRouteStops.clear();
}


void
MSRouteHandler::openRouteDistribution(const SUMOSAXAttributes& attrs) {
    // check whether the id is really necessary
    bool ok = true;
    if (myVehicleParameter != 0) {
        // ok, a vehicle is wrapping the route,
        //  we may use this vehicle's id as default
        myCurrentRouteDistributionID = "!" + myVehicleParameter->id; // !!! document this
    } else {
        myCurrentRouteDistributionID = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
        if (!ok) {
            return;
        }
    }
    myCurrentRouteDistribution = new RandomDistributor<const MSRoute*>();
    std::vector<SUMOReal> probs;
    if (attrs.hasAttribute(SUMO_ATTR_PROBS)) {
        bool ok = true;
        StringTokenizer st(attrs.get<std::string>(SUMO_ATTR_PROBS, myCurrentRouteDistributionID.c_str(), ok));
        while (st.hasNext()) {
            probs.push_back(TplConvert::_2SUMORealSec(st.next().c_str(), 1.0));
        }
    }
    if (attrs.hasAttribute(SUMO_ATTR_ROUTES)) {
        bool ok = true;
        StringTokenizer st(attrs.get<std::string>(SUMO_ATTR_ROUTES, myCurrentRouteDistributionID.c_str(), ok));
        size_t probIndex = 0;
        while (st.hasNext()) {
            std::string routeID = st.next();
            const MSRoute* route = MSRoute::dictionary(routeID, &myParsingRNG);
            if (route == 0) {
                throw ProcessError("Unknown route '" + routeID + "' in distribution '" + myCurrentRouteDistributionID + "'.");
            }
            const SUMOReal prob = (probs.size() > probIndex ? probs[probIndex] : 1.0);
            if (myCurrentRouteDistribution->add(prob, route, false)) {
                route->addReference();
            }
            probIndex++;
        }
        if (probs.size() > 0 && probIndex != probs.size()) {
            WRITE_WARNING("Got " + toString(probs.size()) + " probabilities for " + toString(probIndex) +
                          " routes in routeDistribution '" + myCurrentRouteDistributionID + "'");
        }
    }
}


void
MSRouteHandler::closeRouteDistribution() {
    if (myCurrentRouteDistribution != 0) {
        const bool haveSameID = MSRoute::dictionary(myCurrentRouteDistributionID, &myParsingRNG) != 0;
        if (MSGlobals::gStateLoaded && haveSameID) {
            delete myCurrentRouteDistribution;
            return;
        }
        if (haveSameID) {
            delete myCurrentRouteDistribution;
            throw ProcessError("Another route (or distribution) with the id '" + myCurrentRouteDistributionID + "' exists.");
        }
        if (myCurrentRouteDistribution->getOverallProb() == 0) {
            delete myCurrentRouteDistribution;
            throw ProcessError("Route distribution '" + myCurrentRouteDistributionID + "' is empty.");
        }
        MSRoute::dictionary(myCurrentRouteDistributionID, myCurrentRouteDistribution, myVehicleParameter == 0);
        myCurrentRouteDistribution = 0;
    }
}


void
MSRouteHandler::closeVehicle() {
    // get nested route
    const MSRoute* route = MSRoute::dictionary("!" + myVehicleParameter->id, &myParsingRNG);
    MSVehicleControl& vehControl = MSNet::getInstance()->getVehicleControl();
    if (myVehicleParameter->departProcedure == DEPART_GIVEN) {
        // let's check whether this vehicle had to depart before the simulation starts
        if (!(myAddVehiclesDirectly || checkLastDepart()) || myVehicleParameter->depart < string2time(OptionsCont::getOptions().getString("begin"))) {
            if (route != 0) {
                route->addReference();
                route->release();
            }
            return;
        }
    }
    // get the vehicle's type
    MSVehicleType* vtype = 0;
    if (myVehicleParameter->vtypeid != "") {
        vtype = vehControl.getVType(myVehicleParameter->vtypeid, &myParsingRNG);
        if (vtype == 0) {
            throw ProcessError("The vehicle type '" + myVehicleParameter->vtypeid + "' for vehicle '" + myVehicleParameter->id + "' is not known.");
        }
    } else {
        // there should be one (at least the default one)
        vtype = vehControl.getVType(DEFAULT_VTYPE_ID, &myParsingRNG);
    }
    if (route == 0) {
        // if there is no nested route, try via the (hopefully) given route-id
        route = MSRoute::dictionary(myVehicleParameter->routeid, &myParsingRNG);
    }
    if (route == 0) {
        // nothing found? -> error
        if (myVehicleParameter->routeid != "") {
            throw ProcessError("The route '" + myVehicleParameter->routeid + "' for vehicle '" + myVehicleParameter->id + "' is not known.");
        } else {
            throw ProcessError("Vehicle '" + myVehicleParameter->id + "' has no route.");
        }
    }
    myActiveRouteID = "";

    // try to build the vehicle
    SUMOVehicle* vehicle = 0;
    if (vehControl.getVehicle(myVehicleParameter->id) == 0) {
        vehicle = vehControl.buildVehicle(myVehicleParameter, route, vtype, OptionsCont::getOptions().getBool("ignore-route-errors"));
        // maybe we do not want this vehicle to be inserted due to scaling
        unsigned int quota = vehControl.getQuota();
        if (quota > 0) {
            vehControl.addVehicle(myVehicleParameter->id, vehicle);
            if (myVehicleParameter->departProcedure == DEPART_TRIGGERED) {
                vehControl.addWaiting(*route->begin(), vehicle);
                vehControl.registerOneWaitingForPerson();
            } else {
                // !!! no scaling for triggered vehicles yet
                for (unsigned int i = 1; i < quota; i++) {
                    MSNet::getInstance()->getInsertionControl().add(vehicle);
                    SUMOVehicleParameter* newPars = new SUMOVehicleParameter(*myVehicleParameter);
                    newPars->id = myVehicleParameter->id + "." + toString(i);
                    vehicle = vehControl.buildVehicle(newPars, route, vtype, OptionsCont::getOptions().getBool("ignore-route-errors"));
                    vehControl.addVehicle(newPars->id, vehicle);
                }
            }
            registerLastDepart();
            myVehicleParameter = 0;
        } else {
            vehControl.deleteVehicle(vehicle, true);
            myVehicleParameter = 0;
            vehicle = 0;
        }
    } else {
        // strange: another vehicle with the same id already exists
        if (!MSGlobals::gStateLoaded) {
            // and was not loaded while loading a simulation state
            // -> error
            throw ProcessError("Another vehicle with the id '" + myVehicleParameter->id + "' exists.");
        } else {
            // ok, it seems to be loaded previously while loading a simulation state
            vehicle = 0;
        }
    }
    // check whether the vehicle shall be added directly to the network or
    //  shall stay in the internal buffer
    if (vehicle != 0) {
        if (vehicle->getParameter().departProcedure == DEPART_GIVEN) {
            MSNet::getInstance()->getInsertionControl().add(vehicle);
        }
    }
}


void
MSRouteHandler::closePerson() {
    if (myActivePlan->size() == 0) {
        throw ProcessError("Person '" + myVehicleParameter->id + "' has no plan.");
    }
    MSVehicleType* type = MSNet::getInstance()->getVehicleControl().getVType(myVehicleParameter->vtypeid, &myParsingRNG);
    if (type == 0) {
        throw ProcessError("The type '" + myVehicleParameter->vtypeid + "' for person '" + myVehicleParameter->id + "' is not known.");
    }
    MSPerson* person = MSNet::getInstance()->getPersonControl().buildPerson(myVehicleParameter, type, myActivePlan);
    // @todo: consider myScale?
    if (myAddVehiclesDirectly || checkLastDepart()) {
        if (MSNet::getInstance()->getPersonControl().add(myVehicleParameter->id, person)) {
            MSNet::getInstance()->getPersonControl().setDeparture(myVehicleParameter->depart, person);
            registerLastDepart();
        } else {
            delete person;
            throw ProcessError("Another person with the id '" + myVehicleParameter->id + "' exists.");
        }
    } else {
        // warning already given
        delete person;
    }
    myVehicleParameter = 0;
    myActivePlan = 0;
}


void
MSRouteHandler::closeFlow() {
    if (myVehicleParameter->repetitionNumber == 0) {
        return;
    }
    // let's check whether vehicles had to depart before the simulation starts
    myVehicleParameter->repetitionsDone = 0;
    if (myVehicleParameter->repetitionProbability < 0) {
        const SUMOTime offsetToBegin = string2time(OptionsCont::getOptions().getString("begin")) - myVehicleParameter->depart;
        while (myVehicleParameter->repetitionsDone * myVehicleParameter->repetitionOffset < offsetToBegin) {
            myVehicleParameter->repetitionsDone++;
            if (myVehicleParameter->repetitionsDone == myVehicleParameter->repetitionNumber) {
                return;
            }
        }
    }
    if (MSNet::getInstance()->getVehicleControl().getVType(myVehicleParameter->vtypeid, &myParsingRNG) == 0) {
        throw ProcessError("The vehicle type '" + myVehicleParameter->vtypeid + "' for flow '" + myVehicleParameter->id + "' is not known.");
    }
    if (MSRoute::dictionary("!" + myVehicleParameter->id, &myParsingRNG) == 0) {
        // if not, try via the (hopefully) given route-id
        if (MSRoute::dictionary(myVehicleParameter->routeid, &myParsingRNG) == 0) {
            if (myVehicleParameter->routeid != "") {
                throw ProcessError("The route '" + myVehicleParameter->routeid + "' for flow '" + myVehicleParameter->id + "' is not known.");
            } else {
                if (myVehicleParameter->wasSet(VEHPARS_TAZ_SET)) {
                    myVehicleParameter->setParameter |= VEHPARS_FORCE_REROUTE;
                    const MSEdge* fromTaz = MSEdge::dictionary(myVehicleParameter->fromTaz + "-source");
                    if (fromTaz == 0) {
                        WRITE_ERROR("Source district '" + myVehicleParameter->fromTaz + "' not known for '" + myVehicleParameter->id + "'!");
                    } else if (fromTaz->getNumSuccessors() == 0) {
                        WRITE_ERROR("Source district '" + myVehicleParameter->fromTaz + "' has no outgoing edges for '" + myVehicleParameter->id + "'!");
                    } else {
                        myActiveRoute.push_back(fromTaz->getSuccessor(0));
                    }
                    closeRoute(true);
                    myVehicleParameter->routeid = "!" + myVehicleParameter->id;
                } else {
                    throw ProcessError("Flow '" + myVehicleParameter->id + "' has no route.");
                }
            }
        }
    } else {
        myVehicleParameter->routeid = "!" + myVehicleParameter->id;
    }
    myActiveRouteID = "";

    // check whether the vehicle shall be added directly to the network or
    //  shall stay in the internal buffer
    if (myAddVehiclesDirectly || checkLastDepart()) {
        MSNet::getInstance()->getInsertionControl().add(myVehicleParameter);
        registerLastDepart();
    }
    myVehicleParameter = 0;
}


void
MSRouteHandler::addStop(const SUMOSAXAttributes& attrs) {
    std::string errorSuffix;
    if (myActiveRouteID != "") {
        errorSuffix = " in route '" + myActiveRouteID + "'.";
    } else if (myActivePlan) {
        errorSuffix = " in person '" + myVehicleParameter->id + "'.";
    } else {
        errorSuffix = " in vehicle '" + myVehicleParameter->id + "'.";
    }
    SUMOVehicleParameter::Stop stop;
    bool ok = parseStop(stop, attrs, errorSuffix, MsgHandler::getErrorInstance());
    if (!ok) {
        return;
    }
    // try to parse the assigned bus stop
    if (stop.busstop != "") {
        // ok, we have obviously a bus stop
        MSBusStop* bs = MSNet::getInstance()->getBusStop(stop.busstop);
        if (bs != 0) {
            const MSLane& l = bs->getLane();
            stop.lane = l.getID();
            stop.endPos = bs->getEndLanePosition();
            stop.startPos = bs->getBeginLanePosition();
        } else {
            WRITE_ERROR("The bus stop '" + stop.busstop + "' is not known" + errorSuffix);
            return;
        }
    } else {
        // no, the lane and the position should be given
        // get the lane
        stop.lane = attrs.getOpt<std::string>(SUMO_ATTR_LANE, 0, ok, "");
        if (ok && stop.lane != "") {
            if (MSLane::dictionary(stop.lane) == 0) {
                WRITE_ERROR("The lane '" + stop.lane + "' for a stop is not known" + errorSuffix);
                return;
            }
        } else {
            WRITE_ERROR("A stop must be placed on a bus stop or a lane" + errorSuffix);
            return;
        }
        if (myActivePlan &&
                !myActivePlan->empty() &&
                &myActivePlan->back()->getDestination() != &MSLane::dictionary(stop.lane)->getEdge()) {
            throw ProcessError("Disconnected plan for person '" + myVehicleParameter->id + "' (" + MSLane::dictionary(stop.lane)->getEdge().getID() + "!=" + myActivePlan->back()->getDestination().getID() + ").");
        }
        if (myActivePlan && myActivePlan->empty()) {
            myActivePlan->push_back(new MSPerson::MSPersonStage_Waiting(
                                        MSLane::dictionary(stop.lane)->getEdge(), -1, myVehicleParameter->depart, myVehicleParameter->departPos, "start"));
        }
        stop.endPos = attrs.getOpt<SUMOReal>(SUMO_ATTR_ENDPOS, 0, ok, MSLane::dictionary(stop.lane)->getLength());
        if (attrs.hasAttribute(SUMO_ATTR_POSITION)) {
            WRITE_WARNING("Deprecated attribute 'pos' in description of stop" + errorSuffix);
            stop.endPos = attrs.getOpt<SUMOReal>(SUMO_ATTR_POSITION, 0, ok, stop.endPos);
        }
        stop.startPos = attrs.getOpt<SUMOReal>(SUMO_ATTR_STARTPOS, 0, ok, MAX2((SUMOReal)0., stop.endPos - 2 * POSITION_EPS));
        const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, 0, ok, false);
        if (!ok || !checkStopPos(stop.startPos, stop.endPos, MSLane::dictionary(stop.lane)->getLength(), POSITION_EPS, friendlyPos)) {
            WRITE_ERROR("Invalid start or end position for stop on lane '" + stop.lane + "'" + errorSuffix);
            return;
        }
    }
    if (myActiveRouteID != "") {
        myActiveRouteStops.push_back(stop);
    } else if (myActivePlan) {
        std::string actType = attrs.getOpt<std::string>(SUMO_ATTR_ACTTYPE, 0, ok, "waiting");
        myActivePlan->push_back(new MSPerson::MSPersonStage_Waiting(
                                    MSLane::dictionary(stop.lane)->getEdge(), stop.duration, stop.until, stop.startPos, actType));
    } else {
        myVehicleParameter->stops.push_back(stop);
    }
}


/****************************************************************************/
