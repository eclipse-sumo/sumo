/****************************************************************************/
/// @file    RORouteHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Parser and container for routes during their loading
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
#include <map>
#include <vector>
#include <iostream>
#include <utils/iodevices/OutputDevice.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/options/OptionsCont.h>
#include <utils/xml/SUMOVehicleParserHelper.h>
#include <utils/iodevices/OutputDevice_String.h>
#include "RONet.h"
#include "RORouteHandler.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
RORouteHandler::RORouteHandler(RONet& net, const std::string& file,
                               const bool tryRepair) :
    SUMORouteHandler(file),
    myNet(net),
    myActivePlan(0),
    myTryRepair(tryRepair),
    myCurrentVTypeDistribution(0),
    myCurrentAlternatives(0) {
    myActiveRoute.reserve(100);
}


RORouteHandler::~RORouteHandler() {
}


void
RORouteHandler::myStartElement(int element,
                               const SUMOSAXAttributes& attrs) {
    SUMORouteHandler::myStartElement(element, attrs);
    switch (element) {
        case SUMO_TAG_PERSON:
            myActivePlan = new OutputDevice_String(false, 1);
            break;
        case SUMO_TAG_RIDE: {
            myActivePlan->openTag(SUMO_TAG_RIDE);
            (*myActivePlan) << attrs;
            myActivePlan->closeTag(true);
            break;
        }
        case SUMO_TAG_WALK: {
            myActivePlan->openTag(SUMO_TAG_WALK);
            (*myActivePlan) << attrs;
            myActivePlan->closeTag(true);
            break;
        }
        case SUMO_TAG_FLOW:
            if (attrs.hasAttribute(SUMO_ATTR_FROM) && attrs.hasAttribute(SUMO_ATTR_TO)) {
                myActiveRouteID = "!" + myVehicleParameter->id;
                bool ok = true;
                parseEdges(attrs.getStringReporting(SUMO_ATTR_FROM, myVehicleParameter->id.c_str(), ok),
                           myActiveRoute, "for vehicle '" + myVehicleParameter->id + "'");
                parseEdges(attrs.getStringReporting(SUMO_ATTR_TO, myVehicleParameter->id.c_str(), ok),
                           myActiveRoute, "for vehicle '" + myVehicleParameter->id + "'");
                closeRoute();
            }
            break;
        case SUMO_TAG_TRIP: {
            bool ok = true;
            if (attrs.hasAttribute(SUMO_ATTR_FROM) || !myVehicleParameter->wasSet(VEHPARS_TAZ_SET)) {
                parseEdges(attrs.getStringReporting(SUMO_ATTR_FROM, myVehicleParameter->id.c_str(), ok),
                           myActiveRoute, "for vehicle '" + myVehicleParameter->id + "'");
                parseEdges(attrs.getStringReporting(SUMO_ATTR_TO, myVehicleParameter->id.c_str(), ok),
                           myActiveRoute, "for vehicle '" + myVehicleParameter->id + "'");
            } else {
                const ROEdge* fromTaz = myNet.getEdge(myVehicleParameter->fromTaz + "-source");
                if (fromTaz == 0) {
                    WRITE_ERROR("Source district '" + myVehicleParameter->fromTaz + "' not known for '" + myVehicleParameter->id + "'!");
                } else if (fromTaz->getNoFollowing() == 0) {
                    WRITE_ERROR("Source district '" + myVehicleParameter->fromTaz + "' has no outgoing edges for '" + myVehicleParameter->id + "'!");
                } else {
                    myActiveRoute.push_back(fromTaz->getFollower(0));
                }
            }
            closeRoute();
            closeVehicle();
        }
        break;
        default:
            break;
    }
    // parse embedded vtype information
    if (myCurrentVType != 0 && element != SUMO_TAG_VTYPE) {
        SUMOVehicleParserHelper::parseVTypeEmbedded(*myCurrentVType, element, attrs);
        return;
    }
}


void
RORouteHandler::openVehicleTypeDistribution(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    myCurrentVTypeDistributionID = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    if (ok) {
        myCurrentVTypeDistribution = new RandomDistributor<SUMOVTypeParameter*>();
        if (attrs.hasAttribute(SUMO_ATTR_VTYPES)) {
            const std::string vTypes = attrs.getStringReporting(SUMO_ATTR_VTYPES, myCurrentVTypeDistributionID.c_str(), ok);
            StringTokenizer st(vTypes);
            while (st.hasNext()) {
                SUMOVTypeParameter* type = myNet.getVehicleTypeSecure(st.next());
                myCurrentVTypeDistribution->add(1., type);
            }
        }
    }
}


void
RORouteHandler::closeVehicleTypeDistribution() {
    if (myCurrentVTypeDistribution != 0) {
        if (myCurrentVTypeDistribution->getOverallProb() == 0) {
            delete myCurrentVTypeDistribution;
            WRITE_ERROR("Vehicle type distribution '" + myCurrentVTypeDistributionID + "' is empty.");
        } else if (!myNet.addVTypeDistribution(myCurrentVTypeDistributionID, myCurrentVTypeDistribution)) {
            delete myCurrentVTypeDistribution;
            WRITE_ERROR("Another vehicle type (or distribution) with the id '" + myCurrentVTypeDistributionID + "' exists.");
        }
        myCurrentVTypeDistribution = 0;
    }
}


void
RORouteHandler::openRoute(const SUMOSAXAttributes& attrs) {
    // check whether the id is really necessary
    std::string rid;
    if (myCurrentAlternatives != 0) {
        myActiveRouteID = myCurrentAlternatives->getID();
        rid =  "distribution '" + myCurrentAlternatives->getID() + "'";
    } else if (myVehicleParameter != 0) {
        // ok, a vehicle is wrapping the route,
        //  we may use this vehicle's id as default
        myActiveRouteID = "!" + myVehicleParameter->id; // !!! document this
        if (attrs.hasAttribute(SUMO_ATTR_ID)) {
            WRITE_WARNING("Ids of internal routes are ignored (vehicle '" + myVehicleParameter->id + "').");
        }
    } else {
        bool ok = true;
        myActiveRouteID = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok, false);
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
        parseEdges(attrs.getStringReporting(SUMO_ATTR_EDGES, myActiveRouteID.c_str(), ok), myActiveRoute, rid);
    }
    myActiveRouteRefID = attrs.getOptStringReporting(SUMO_ATTR_REFID, myActiveRouteID.c_str(), ok, "");
    if (myActiveRouteRefID != "" && myNet.getRouteDef(myActiveRouteRefID) == 0) {
        WRITE_ERROR("Invalid reference to route '" + myActiveRouteRefID + "' in route " + rid + ".");
    }
    myActiveRouteProbability = attrs.getOptSUMORealReporting(SUMO_ATTR_PROB, myActiveRouteID.c_str(), ok, DEFAULT_VEH_PROB);
    myActiveRouteColor = attrs.hasAttribute(SUMO_ATTR_COLOR) ? new RGBColor(attrs.getColorReporting(myActiveRouteID.c_str(), ok)) : 0;
}


void
RORouteHandler::myEndElement(int element) {
    SUMORouteHandler::myEndElement(element);
    switch (element) {
        case SUMO_TAG_VTYPE: {
            if (myNet.addVehicleType(myCurrentVType)) {
                if (myCurrentVTypeDistribution != 0) {
                    myCurrentVTypeDistribution->add(myCurrentVType->defaultProbability, myCurrentVType);
                }
            }
            myCurrentVType = 0;
        }
        break;
        default:
            break;
    }
}


void
RORouteHandler::closeRoute() {
    if (myActiveRoute.size() == 0) {
        if (myActiveRouteRefID != "" && myCurrentAlternatives != 0) {
            myCurrentAlternatives->addAlternativeDef(myNet.getRouteDef(myActiveRouteRefID));
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
    RORoute* route = new RORoute(myActiveRouteID, myCurrentCosts, myActiveRouteProbability, myActiveRoute,
                                 myActiveRouteColor);
    myActiveRoute.clear();
    if (myCurrentAlternatives == 0) {
        if (myNet.getRouteDef(myActiveRouteID) != 0) {
            delete route;
            if (myVehicleParameter != 0) {
                throw ProcessError("Another route for vehicle '" + myVehicleParameter->id + "' exists.");
            } else {
                throw ProcessError("Another route (or distribution) with the id '" + myActiveRouteID + "' exists.");
            }
        } else {
            myCurrentAlternatives = new RORouteDef(myActiveRouteID, 0, myTryRepair);
            myCurrentAlternatives->addLoadedAlternative(route);
            myNet.addRouteDef(myCurrentAlternatives);
            myCurrentAlternatives = 0;
        }
    } else {
        myCurrentAlternatives->addLoadedAlternative(route);
    }
    myActiveRouteID = "";
    myActiveRouteStops.clear();
}


void
RORouteHandler::openRouteDistribution(const SUMOSAXAttributes& attrs) {
    // check whether the id is really necessary
    bool ok = true;
    std::string id;
    if (myVehicleParameter != 0) {
        // ok, a vehicle is wrapping the route,
        //  we may use this vehicle's id as default
        id = "!" + myVehicleParameter->id; // !!! document this
    } else {
        id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
        if (!ok) {
            return;
        }
    }
    // try to get the index of the last element
    int index = attrs.getIntReporting(SUMO_ATTR_LAST, id.c_str(), ok);
    if (ok && index < 0) {
        WRITE_ERROR("Negative index of a route alternative (id='" + id + "').");
        return;
    }
    // build the alternative cont
    myCurrentAlternatives = new RORouteDef(id, index, false);
    if (attrs.hasAttribute(SUMO_ATTR_ROUTES)) {
        ok = true;
        StringTokenizer st(attrs.getStringReporting(SUMO_ATTR_ROUTES, id.c_str(), ok));
        while (st.hasNext()) {
            const std::string routeID = st.next();
            const RORouteDef* route = myNet.getRouteDef(routeID);
            if (route == 0) {
                throw ProcessError("Unknown route '" + routeID + "' in distribution '" + id + "'.");
            }
            myCurrentAlternatives->addAlternativeDef(route);
        }
    }
}


void
RORouteHandler::closeRouteDistribution() {
    if (myCurrentAlternatives != 0) {
        if (myCurrentAlternatives->getOverallProb() == 0) {
            WRITE_ERROR("Route distribution '" + myCurrentAlternatives->getID() + "' is empty.");
            delete myCurrentAlternatives;
        } else if (!myNet.addRouteDef(myCurrentAlternatives)) {
            WRITE_ERROR("Another route (or distribution) with the id '" + myCurrentAlternatives->getID() + "' exists.");
            delete myCurrentAlternatives;
        }
        myCurrentAlternatives = 0;
    }
}


void
RORouteHandler::closeVehicle() {
    // get the vehicle id
    if (myVehicleParameter->depart < string2time(OptionsCont::getOptions().getString("begin"))) {
        return;
    }
    // get vehicle type
    SUMOVTypeParameter* type = myNet.getVehicleTypeSecure(myVehicleParameter->vtypeid);
    // get the route
    RORouteDef* route = myNet.getRouteDef(myVehicleParameter->routeid);
    if (route == 0) {
        route = myNet.getRouteDef("!" + myVehicleParameter->id);
    }
    if (route == 0) {
        WRITE_ERROR("The route of the vehicle '" + myVehicleParameter->id + "' is not known.");
        return;
    }
    // build the vehicle
    if (!MsgHandler::getErrorInstance()->wasInformed()) {
        ROVehicle* veh = new ROVehicle(*myVehicleParameter, route, type);
        myNet.addVehicle(myVehicleParameter->id, veh);
    }
}


void
RORouteHandler::closePerson() {
    myPersonBuffer[myVehicleParameter->depart] = myActivePlan->getString();
    delete myVehicleParameter;
    myVehicleParameter = 0;
    delete myActivePlan;
    myActivePlan = 0;
}


void
RORouteHandler::closeFlow() {
    /*    // @todo: consider myScale?
        // let's check whether vehicles had to depart before the simulation starts
        myVehicleParameter->repetitionsDone = 0;
        SUMOTime offsetToBegin = string2time(OptionsCont::getOptions().getString("begin")) - myVehicleParameter->depart;
        while (myVehicleParameter->repetitionsDone * myVehicleParameter->repetitionOffset < offsetToBegin) {
            myVehicleParameter->repetitionsDone++;
            if (myVehicleParameter->repetitionsDone == myVehicleParameter->repetitionNumber) {
                return;
            }
        }
        if (MSNet::getInstance()->getVehicleControl().getVType(myVehicleParameter->vtypeid) == 0) {
            throw ProcessError("The vehicle type '" + myVehicleParameter->vtypeid + "' for vehicle '" + myVehicleParameter->id + "' is not known.");
        }
        if (MSRoute::dictionary("!" + myVehicleParameter->id) == 0) {
            // if not, try via the (hopefully) given route-id
            if (MSRoute::dictionary(myVehicleParameter->routeid) == 0) {
                if (myVehicleParameter->routeid != "") {
                    throw ProcessError("The route '" + myVehicleParameter->routeid + "' for vehicle '" + myVehicleParameter->id + "' is not known.");
                } else {
                    throw ProcessError("Vehicle '" + myVehicleParameter->id + "' has no route.");
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
        myVehicleParameter = 0;*/
}


void
RORouteHandler::addStop(const SUMOSAXAttributes& attrs) {
    /*bool ok = true;
    std::string errorSuffix;
    if (myActiveRouteID != "") {
        errorSuffix = " in route '" + myActiveRouteID + "'.";
    } else if (myActivePlan) {
        errorSuffix = " in person '" + myVehicleParameter->id + "'.";
    } else {
        errorSuffix = " in vehicle '" + myVehicleParameter->id + "'.";
    }
    SUMOVehicleParameter::Stop stop;
    // try to parse the assigned bus stop
    stop.busstop = attrs.getOptStringReporting(SUMO_ATTR_BUS_STOP, 0, ok, "");
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
        stop.lane = attrs.getOptStringReporting(SUMO_ATTR_LANE, 0, ok, "");
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
        stop.endPos = attrs.getOptSUMORealReporting(SUMO_ATTR_ENDPOS, 0, ok, MSLane::dictionary(stop.lane)->getLength());
        if (attrs.hasAttribute(SUMO_ATTR_POSITION)) {
            WRITE_WARNING("eprecated attribute 'pos' in description of stop" + errorSuffix);
            stop.endPos = attrs.getOptSUMORealReporting(SUMO_ATTR_POSITION, 0, ok, stop.endPos);
        }
        stop.startPos = attrs.getOptSUMORealReporting(SUMO_ATTR_STARTPOS, 0, ok, stop.endPos - 2 * POSITION_EPS);
        const bool friendlyPos = attrs.getOptBoolReporting(SUMO_ATTR_FRIENDLY_POS, 0, ok, false);
        if (!ok || !checkStopPos(stop.startPos, stop.endPos, MSLane::dictionary(stop.lane)->getLength(), POSITION_EPS, friendlyPos)) {
            WRITE_ERROR("Invalid start or end position for stop" + errorSuffix);
            return;
        }
    }

    // get the standing duration
    if (!attrs.hasAttribute(SUMO_ATTR_DURATION) && !attrs.hasAttribute(SUMO_ATTR_UNTIL)) {
        stop.triggered = attrs.getOptBoolReporting(SUMO_ATTR_TRIGGERED, 0, ok, true);
        stop.duration = -1;
        stop.until = -1;
    } else {
        stop.duration = attrs.getOptSUMOTimeReporting(SUMO_ATTR_DURATION, 0, ok, -1);
        stop.until = attrs.getOptSUMOTimeReporting(SUMO_ATTR_UNTIL, 0, ok, -1);
        if (!ok || (stop.duration < 0 && stop.until < 0)) {
            WRITE_ERROR("Invalid duration or end time is given for a stop" + errorSuffix);
            return;
        }
        stop.triggered = attrs.getOptBoolReporting(SUMO_ATTR_TRIGGERED, 0, ok, false);
    }
    stop.parking = attrs.getOptBoolReporting(SUMO_ATTR_PARKING, 0, ok, stop.triggered);
    if (!ok) {
        WRITE_ERROR("Invalid bool for 'triggered' or 'parking' for stop" + errorSuffix);
        return;
    }
    const std::string idx = attrs.getOptStringReporting(SUMO_ATTR_INDEX, 0, ok, "end");
    if (idx == "end") {
        stop.index = STOP_INDEX_END;
    } else if (idx == "fit") {
        stop.index = STOP_INDEX_FIT;
    } else {
        stop.index = attrs.getIntReporting(SUMO_ATTR_INDEX, 0, ok);
        if (!ok || stop.index < 0) {
            WRITE_ERROR("Invalid 'index' for stop" + errorSuffix);
            return;
        }
    }
    if (myActiveRouteID != "") {
        myActiveRouteStops.push_back(stop);
    } else if (myActivePlan) {
        myActivePlan->push_back(new MSPerson::MSPersonStage_Waiting(MSLane::dictionary(stop.lane)->getEdge(), stop.duration, stop.until));
    } else {
        myVehicleParameter->stops.push_back(stop);
    }*/
}


void
RORouteHandler::parseEdges(const std::string& desc, std::vector<const ROEdge*>& into,
                           const std::string& rid) {
    StringTokenizer st(desc);
    while (st.hasNext()) {
        const std::string id = st.next();
        const ROEdge* edge = myNet.getEdge(id);
        // check whether the edge exists
        if (edge == 0) {
            throw ProcessError("The edge '" + id + "' within the route " + rid + " is not known."
                               + "\n The route can not be build.");
        }
        into.push_back(edge);
    }
}


/****************************************************************************/
