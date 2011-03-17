/****************************************************************************/
/// @file    MSRouteHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Parser and container for routes during their loading
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
#include <map>
#include <vector>
#include <microsim/MSRoute.h>
#include <microsim/MSEdge.h>
#include <microsim/MSVehicleType.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSEdge.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSLane.h>
#include "MSRouteHandler.h"
#include "MSPersonControl.h"
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/MsgHandler.h>
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
// method definitions
// ===========================================================================
MSRouteHandler::MSRouteHandler(const std::string &file,
                               bool addVehiclesDirectly)
        : SUMOSAXHandler(file), myVehicleParameter(0),
        myLastDepart(0), myAddVehiclesDirectly(addVehiclesDirectly),
        myRunningVehicleNumber(0),
        myCurrentVTypeDistribution(0),
        myCurrentRouteDistribution(0),
        myHaveWarned(false), myCurrentVType(0), myActivePlan(0) {
    myIncrementalBase = OptionsCont::getOptions().getInt("incremental-dua-base");
    myIncrementalStage = OptionsCont::getOptions().getInt("incremental-dua-step");
    myAmUsingIncrementalDUA = (myIncrementalStage>0);
    myActiveRoute.reserve(100);
}


MSRouteHandler::~MSRouteHandler() throw() {
}


SUMOTime
MSRouteHandler::getLastDepart() const {
    return myLastDepart;
}


bool
MSRouteHandler::checkLastDepart() {
    if (myVehicleParameter->departProcedure == DEPART_GIVEN) {
        if (!myAddVehiclesDirectly && myVehicleParameter->depart < myLastDepart) {
            WRITE_WARNING("Route file should be sorted by departure time, ignoring '" + myVehicleParameter->id + "'!");
            return false;
        }
        myLastDepart = myVehicleParameter->depart;
    }
    return true;
}


void
MSRouteHandler::myStartElement(SumoXMLTag element,
                               const SUMOSAXAttributes &attrs) throw(ProcessError) {
    switch (element) {
    case SUMO_TAG_VEHICLE:
        delete myVehicleParameter;
        myVehicleParameter = SUMOVehicleParserHelper::parseVehicleAttributes(attrs);
        break;
    case SUMO_TAG_PERSON:
        delete myVehicleParameter;
        myVehicleParameter = SUMOVehicleParserHelper::parseVehicleAttributes(attrs);
        myActivePlan = new MSPerson::MSPersonPlan();
        break;
    case SUMO_TAG_RIDE: {
        const std::string pid = myVehicleParameter->id;
        bool ok = true;
        MSEdge *from = 0;
        if (attrs.hasAttribute(SUMO_ATTR_FROM)) {
            const std::string fromID = attrs.getStringReporting(SUMO_ATTR_FROM, "ride", pid.c_str(), ok);
            from = MSEdge::dictionary(fromID);
            if (from==0) {
                throw ProcessError("The from edge '" + fromID + "' within a ride of person '" + pid + "' is not known.");
            }
            if (!myActivePlan->empty() && &myActivePlan->back()->getDestination() != from) {
                throw ProcessError("Disconnected plan for person '" + myVehicleParameter->id + "' (" + fromID + "!=" + myActivePlan->back()->getDestination().getID() + ").");
            }
            if (myActivePlan->empty()) {
                myActivePlan->push_back(new MSPerson::MSPersonStage_Waiting(*from, -1, myVehicleParameter->depart));
            }
        }
        const std::string toID = attrs.getStringReporting(SUMO_ATTR_TO, "ride", pid.c_str(), ok);
        MSEdge *to = MSEdge::dictionary(toID);
        if (to==0) {
            throw ProcessError("The to edge '" + toID + "' within a ride of person '" + pid + "' is not known.");
        }
        const std::string desc = attrs.getStringReporting(SUMO_ATTR_LINES, "ride", pid.c_str(), ok);
        StringTokenizer st(desc);
        myActivePlan->push_back(new MSPerson::MSPersonStage_Driving(*to, st.getVector()));
        break;
    }
    case SUMO_TAG_WALK: {
        myActiveRoute.clear();
        bool ok = true;
        MSEdge::parseEdgesList(attrs.getStringReporting(SUMO_ATTR_EDGES, "walk", myVehicleParameter->id.c_str(), ok), myActiveRoute, myActiveRouteID);
        if (myActiveRoute.empty()) {
            throw ProcessError("No edges to walk for person '" + myVehicleParameter->id + "'.");
        }
        if (!myActivePlan->empty() && &myActivePlan->back()->getDestination() != myActiveRoute.front()) {
            throw ProcessError("Disconnected plan for person '" + myVehicleParameter->id + "' (" + myActiveRoute.front()->getID() + "!=" + myActivePlan->back()->getDestination().getID() + ").");
        }
        if (myActivePlan->empty()) {
            myActivePlan->push_back(new MSPerson::MSPersonStage_Waiting(*myActiveRoute.front(), -1, myVehicleParameter->depart));
        }
        const SUMOTime duration = attrs.getOptSUMOTimeReporting(SUMO_ATTR_DURATION, "walk", 0, ok, -1);
        const SUMOReal speed = attrs.getOptSUMORealReporting(SUMO_ATTR_SPEED, "walk", 0, ok, -1);
        myActivePlan->push_back(new MSPerson::MSPersonStage_Walking(myActiveRoute, duration, speed));
        myActiveRoute.clear();
        break;
    }
    case SUMO_TAG_FLOW:
        delete myVehicleParameter;
        myVehicleParameter = SUMOVehicleParserHelper::parseFlowAttributes(attrs);
        if (attrs.hasAttribute(SUMO_ATTR_FROM) && attrs.hasAttribute(SUMO_ATTR_TO)) {
            myActiveRouteID = "!" + myVehicleParameter->id;
            bool ok = true;
            MSEdge::parseEdgesList(attrs.getStringReporting(SUMO_ATTR_FROM, "flow", myVehicleParameter->id.c_str(), ok),
                                   myActiveRoute, "for vehicle '" + myVehicleParameter->id + "'");
            MSEdge::parseEdgesList(attrs.getStringReporting(SUMO_ATTR_TO, "flow", myVehicleParameter->id.c_str(), ok),
                                   myActiveRoute, "for vehicle '" + myVehicleParameter->id + "'");
            closeRoute();
        }
        break;
    case SUMO_TAG_VTYPE:
        myCurrentVType = SUMOVehicleParserHelper::beginVTypeParsing(attrs);
        break;
    case SUMO_TAG_VTYPE_DISTRIBUTION:
        openVehicleTypeDistribution(attrs);
        break;
    case SUMO_TAG_ROUTE:
        openRoute(attrs);
        break;
    case SUMO_TAG_ROUTE_DISTRIBUTION:
        openRouteDistribution(attrs);
        break;
    case SUMO_TAG_STOP:
        addStop(attrs);
        break;
    case SUMO_TAG_TRIPDEF: {
        bool ok = true;
        myVehicleParameter = SUMOVehicleParserHelper::parseVehicleAttributes(attrs);
        myActiveRouteID = "!" + myVehicleParameter->id;
        if (attrs.hasAttribute(SUMO_ATTR_FROM) || !myVehicleParameter->wasSet(VEHPARS_TAZ_SET)) {
            MSEdge::parseEdgesList(attrs.getStringReporting(SUMO_ATTR_FROM, "tripdef", myVehicleParameter->id.c_str(), ok),
                                   myActiveRoute, "for vehicle '" + myVehicleParameter->id + "'");
            MSEdge::parseEdgesList(attrs.getStringReporting(SUMO_ATTR_TO, "tripdef", myVehicleParameter->id.c_str(), ok),
                                   myActiveRoute, "for vehicle '" + myVehicleParameter->id + "'");
        } else {
            const MSEdge* fromTaz = MSEdge::dictionary(myVehicleParameter->fromTaz+"-source");
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
    if (myCurrentVType!=0&&element!=SUMO_TAG_VTYPE) {
        SUMOVehicleParserHelper::parseVTypeEmbedded(*myCurrentVType, element, attrs);
        return;
    }
}


void
MSRouteHandler::openVehicleTypeDistribution(const SUMOSAXAttributes &attrs) {
    if (attrs.setIDFromAttributes("vtypeDistribution", myCurrentVTypeDistributionID)) {
        myCurrentVTypeDistribution = new RandomDistributor<MSVehicleType*>();
        if (attrs.hasAttribute(SUMO_ATTR_VTYPES)) {
            bool ok = true;
            StringTokenizer st(attrs.getStringReporting(SUMO_ATTR_VTYPES, "vtypeDistribution", myCurrentVTypeDistributionID.c_str(), ok));
            while (st.hasNext()) {
                std::string vtypeID = st.next();
                MSVehicleType *type = MSNet::getInstance()->getVehicleControl().getVType(vtypeID);
                if (type==0) {
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
        if (myCurrentVTypeDistribution->getOverallProb() == 0) {
            delete myCurrentVTypeDistribution;
            MsgHandler::getErrorInstance()->inform("Vehicle type distribution '" + myCurrentVTypeDistributionID + "' is empty.");
        } else if (!MSNet::getInstance()->getVehicleControl().addVTypeDistribution(myCurrentVTypeDistributionID, myCurrentVTypeDistribution)) {
            delete myCurrentVTypeDistribution;
            MsgHandler::getErrorInstance()->inform("Another vehicle type (or distribution) with the id '" + myCurrentVTypeDistributionID + "' exists.");
        }
        myCurrentVTypeDistribution = 0;
    }
}


void
MSRouteHandler::openRoute(const SUMOSAXAttributes &attrs) {
    // check whether the id is really necessary
    std::string rid;
    if (myCurrentRouteDistribution != 0) {
        myActiveRouteID = myCurrentRouteDistributionID + "#" + toString(myCurrentRouteDistribution->getProbs().size()); // !!! document this
        rid =  "distribution '" + myCurrentRouteDistributionID + "'";
    } else if (myVehicleParameter!=0) {
        // ok, a vehicle is wrapping the route,
        //  we may use this vehicle's id as default
        myActiveRouteID = "!" + myVehicleParameter->id; // !!! document this
    } else {
        bool ok = true;
        myActiveRouteID = attrs.getStringReporting(SUMO_ATTR_ID, "route", 0, ok, false);
        if (!ok) {
            return;
        }
        rid = "'" + myActiveRouteID + "'";
    }
    if (myVehicleParameter!=0) { // have to do this here for nested route distributions
        rid =  "for vehicle '" + myVehicleParameter->id + "'";
    }
    bool ok = true;
    if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
        MSEdge::parseEdgesList(attrs.getStringReporting(SUMO_ATTR_EDGES, "route", myActiveRouteID.c_str(), ok), myActiveRoute, rid);
    }
    myActiveRouteRefID = attrs.getOptStringReporting(SUMO_ATTR_REFID, "route", myActiveRouteID.c_str(), ok, "");
    if (myActiveRouteRefID != "" && MSRoute::dictionary(myActiveRouteRefID) == 0) {
        MsgHandler::getErrorInstance()->inform("Invalid reference to route '" + myActiveRouteRefID + "' in route " + rid + ".");
    }
    myActiveRouteProbability = attrs.getOptSUMORealReporting(SUMO_ATTR_PROB, "route", myActiveRouteID.c_str(), ok, DEFAULT_VEH_PROB);
    myActiveRouteColor = attrs.hasAttribute(SUMO_ATTR_COLOR) ? RGBColor::parseColorReporting(attrs.getString(SUMO_ATTR_COLOR), "route", myActiveRouteID.c_str(), true, ok) : RGBColor::getDefaultColor();
}


// ----------------------------------


void
MSRouteHandler::myCharacters(SumoXMLTag element,
                             const std::string &chars) throw(ProcessError) {
    switch (element) {
    case SUMO_TAG_ROUTE: {
        size_t len = chars.length();
        size_t beg = 0;
        while (beg<len&&chars[beg]<=32) {
            beg++;
        }
        if (beg<len) {
            if (!myHaveWarned) {
                MsgHandler::getWarningInstance()->inform("Defining routes as a nested string is deprecated, use the edges attribute instead.");
                myHaveWarned = true;
            }
            MSEdge::parseEdgesList(chars, myActiveRoute, myActiveRouteID);
        }
        break;
    }
    default:
        break;
    }
}


// ----------------------------------

void
MSRouteHandler::myEndElement(SumoXMLTag element) throw(ProcessError) {
    switch (element) {
    case SUMO_TAG_ROUTE:
        closeRoute();
        break;
    case SUMO_TAG_PERSON:
        closePerson();
        delete myVehicleParameter;
        myVehicleParameter = 0;
        break;
    case SUMO_TAG_VEHICLE:
        if (myVehicleParameter->repetitionNumber>0) {
            myVehicleParameter->repetitionNumber++; // for backwards compatibility
            // it is a flow, thus no break here
        } else {
            closeVehicle();
            delete myVehicleParameter;
            myVehicleParameter = 0;
            break;
        }
    case SUMO_TAG_FLOW:
        closeFlow();
        break;
    case SUMO_TAG_VTYPE_DISTRIBUTION:
        closeVehicleTypeDistribution();
        break;
    case SUMO_TAG_ROUTE_DISTRIBUTION:
        closeRouteDistribution();
        break;
    case SUMO_TAG_VTYPE: {
        SUMOVehicleParserHelper::closeVTypeParsing(*myCurrentVType);
        MSVehicleType *vehType = MSVehicleType::build(*myCurrentVType);
        delete myCurrentVType;
        myCurrentVType = 0;
        if (!MSNet::getInstance()->getVehicleControl().addVType(vehType)) {
            std::string id = vehType->getID();
            delete vehType;
#ifdef HAVE_MESOSIM
            if (!MSGlobals::gStateLoaded) {
#endif
                throw ProcessError("Another vehicle type (or distribution) with the id '" + id + "' exists.");
#ifdef HAVE_MESOSIM
            }
#endif
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
MSRouteHandler::closeRoute() throw(ProcessError) {
    if (myActiveRoute.size()==0) {
        if (myActiveRouteRefID != "" && myCurrentRouteDistribution != 0) {
            myCurrentRouteDistribution->add(myActiveRouteProbability, MSRoute::dictionary(myActiveRouteRefID));
            myActiveRouteID = "";
            myActiveRouteRefID = "";
            return;
        }
        if (myVehicleParameter!=0) {
            throw ProcessError("Vehicle's '" + myVehicleParameter->id + "' route has no edges.");
        } else {
            throw ProcessError("Route '" + myActiveRouteID + "' has no edges.");
        }
    }
    MSRoute *route = new MSRoute(myActiveRouteID, myActiveRoute,
                                 myVehicleParameter==0||myVehicleParameter->repetitionNumber>=1,
                                 myActiveRouteColor, myActiveRouteStops);
    myActiveRoute.clear();
    if (!MSRoute::dictionary(myActiveRouteID, route)) {
        delete route;
#ifdef HAVE_MESOSIM
        if (!MSGlobals::gStateLoaded) {
#endif
            if (myVehicleParameter!=0) {
                if (MSNet::getInstance()->getVehicleControl().getVehicle(myVehicleParameter->id)==0) {
                    throw ProcessError("Another route for vehicle '" + myVehicleParameter->id + "' exists.");
                } else {
                    throw ProcessError("A vehicle with id '" + myVehicleParameter->id + "' already exists.");
                }
            } else {
                throw ProcessError("Another route (or distribution) with the id '" + myActiveRouteID + "' exists.");
            }
#ifdef HAVE_MESOSIM
        }
#endif
    } else {
        if (myCurrentRouteDistribution != 0) {
            myCurrentRouteDistribution->add(myActiveRouteProbability, route);
        }
    }
    myActiveRouteID = "";
    myActiveRouteStops.clear();
}


void
MSRouteHandler::openRouteDistribution(const SUMOSAXAttributes &attrs) {
    // check whether the id is really necessary
    if (myVehicleParameter!=0) {
        // ok, a vehicle is wrapping the route,
        //  we may use this vehicle's id as default
        myCurrentRouteDistributionID = "!" + myVehicleParameter->id; // !!! document this
    } else {
        if (!attrs.setIDFromAttributes("routeDistribution", myCurrentRouteDistributionID)) {
            return;
        }
    }
    myCurrentRouteDistribution = new RandomDistributor<const MSRoute*>();
    if (attrs.hasAttribute(SUMO_ATTR_ROUTES)) {
        bool ok = true;
        StringTokenizer st(attrs.getStringReporting(SUMO_ATTR_ROUTES, "routeDistribution", myCurrentRouteDistributionID.c_str(), ok));
        while (st.hasNext()) {
            std::string routeID = st.next();
            const MSRoute *route = MSRoute::dictionary(routeID);
            if (route==0) {
                throw ProcessError("Unknown route '" + routeID + "' in distribution '" + myCurrentRouteDistributionID + "'.");
            }
            myCurrentRouteDistribution->add(1., route, false);
        }
    }
}


void
MSRouteHandler::closeRouteDistribution() {
    if (myCurrentRouteDistribution != 0) {
        if (myCurrentRouteDistribution->getOverallProb() == 0) {
            delete myCurrentRouteDistribution;
            MsgHandler::getErrorInstance()->inform("Route distribution '" + myCurrentRouteDistributionID + "' is empty.");
        } else if (!MSRoute::dictionary(myCurrentRouteDistributionID, myCurrentRouteDistribution)) {
            delete myCurrentRouteDistribution;
            MsgHandler::getErrorInstance()->inform("Another route (or distribution) with the id '" + myCurrentRouteDistributionID + "' exists.");
        }
        myCurrentRouteDistribution = 0;
    }
}


void
MSRouteHandler::closeVehicle() throw(ProcessError) {
    // get nested route
    const MSRoute *route = MSRoute::dictionary("!" + myVehicleParameter->id);
    if (myVehicleParameter->departProcedure == DEPART_GIVEN) {
        // let's check whether this vehicle had to depart before the simulation starts
        if (!checkLastDepart() || myVehicleParameter->depart<string2time(OptionsCont::getOptions().getString("begin"))) {
            if (route!=0) {
                route->addReference();
                route->release();
            }
            return;
        }
    }
    // get the vehicle's type
    MSVehicleType *vtype = 0;
    if (myVehicleParameter->vtypeid!="") {
        vtype = MSNet::getInstance()->getVehicleControl().getVType(myVehicleParameter->vtypeid);
        if (vtype==0) {
            throw ProcessError("The vehicle type '" + myVehicleParameter->vtypeid + "' for vehicle '" + myVehicleParameter->id + "' is not known.");
        }
    } else {
        // there should be one (at least the default one)
        vtype = MSNet::getInstance()->getVehicleControl().getVType();
    }
    if (route==0) {
        // if there is no nested route, try via the (hopefully) given route-id
        route = MSRoute::dictionary(myVehicleParameter->routeid);
    }
    if (route==0) {
        // nothing found? -> error
        if (myVehicleParameter->routeid!="") {
            throw ProcessError("The route '" + myVehicleParameter->routeid + "' for vehicle '" + myVehicleParameter->id + "' is not known.");
        } else {
            throw ProcessError("Vehicle '" + myVehicleParameter->id + "' has no route.");
        }
    }
    myActiveRouteID = "";

    // try to build the vehicle
    SUMOVehicle *vehicle = 0;
    if (MSNet::getInstance()->getVehicleControl().getVehicle(myVehicleParameter->id)==0) {
        // ok there was no other vehicle with the same id, yet
        // maybe we do not want this vehicle to be inserted due to using incremental dua
        bool add = true;
        if (myAmUsingIncrementalDUA) {
            if ((int)(myRunningVehicleNumber%myIncrementalBase)>=(int) myIncrementalStage) {
                add = false;
            }
            myRunningVehicleNumber++;
        }
        if (add) {
            vehicle = MSNet::getInstance()->getVehicleControl().buildVehicle(myVehicleParameter, route, vtype);
            // add the vehicle to the vehicle control
            MSNet::getInstance()->getVehicleControl().addVehicle(myVehicleParameter->id, vehicle);
            if (myVehicleParameter->departProcedure == DEPART_TRIGGERED) {
                MSNet::getInstance()->getVehicleControl().addWaiting(*route->begin(), vehicle);
                MSNet::getInstance()->getVehicleControl().registerOneWaitingForPerson();
            }
            myVehicleParameter = 0;
        }
    } else {
        // strange: another vehicle with the same id already exists
#ifdef HAVE_MESOSIM
        if (!MSGlobals::gStateLoaded) {
#endif
            // and was not loaded while loading a simulation state
            // -> error
            throw ProcessError("Another vehicle with the id '" + myVehicleParameter->id + "' exists.");
#ifdef HAVE_MESOSIM
        } else {
            // ok, it seems to be loaded previously while loading a simulation state
            vehicle = 0;
        }
#endif
    }
    // check whether the vehicle shall be added directly to the network or
    //  shall stay in the internal buffer
    if (vehicle!=0) {
        if (vehicle->getParameter().departProcedure == DEPART_GIVEN) {
            MSNet::getInstance()->getInsertionControl().add(vehicle);
        }
    }
}


void
MSRouteHandler::closePerson() throw(ProcessError) {
    if (myActivePlan->size()==0) {
        throw ProcessError("Person '" + myVehicleParameter->id + "' has no plan.");
    }
    MSPerson *person = new MSPerson(myVehicleParameter, myActivePlan);
    if (checkLastDepart() && MSNet::getInstance()->getPersonControl().add(myVehicleParameter->id, person)) {
        MSNet::getInstance()->getPersonControl().setArrival(myVehicleParameter->depart, person);
    } else {
        delete person;
    }
    myVehicleParameter = 0;
    myActivePlan = 0;
}


void
MSRouteHandler::closeFlow() throw(ProcessError) {
    // let's check whether vehicles had to depart before the simulation starts
    myVehicleParameter->repetitionsDone = 0;
    SUMOTime offsetToBegin = string2time(OptionsCont::getOptions().getString("begin")) - myVehicleParameter->depart;
    while (myVehicleParameter->repetitionsDone * myVehicleParameter->repetitionOffset < offsetToBegin) {
        myVehicleParameter->repetitionsDone++;
        if (myVehicleParameter->repetitionsDone == myVehicleParameter->repetitionNumber) {
            return;
        }
    }
    if (MSNet::getInstance()->getVehicleControl().getVType(myVehicleParameter->vtypeid)==0) {
        throw ProcessError("The vehicle type '" + myVehicleParameter->vtypeid + "' for vehicle '" + myVehicleParameter->id + "' is not known.");
    }
    if (MSRoute::dictionary("!" + myVehicleParameter->id)==0) {
        // if not, try via the (hopefully) given route-id
        if (MSRoute::dictionary(myVehicleParameter->routeid) == 0) {
            if (myVehicleParameter->routeid!="") {
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
    if (checkLastDepart()) {
        MSNet::getInstance()->getInsertionControl().add(myVehicleParameter);
    }
    myVehicleParameter = 0;
}

bool
MSRouteHandler::checkStopPos(SUMOReal &startPos, SUMOReal &endPos, const SUMOReal laneLength,
                             const SUMOReal minLength, const bool friendlyPos) {
    if (minLength > laneLength) {
        return false;
    }
    if (startPos < 0) {
        startPos += laneLength;
    }
    if (endPos < 0) {
        endPos += laneLength;
    }
    if (endPos < minLength || endPos > laneLength) {
        if (!friendlyPos) {
            return false;
        }
        if (endPos < minLength) {
            endPos = minLength;
        }
        if (endPos > laneLength) {
            endPos = laneLength;
        }
    }
    if (startPos < 0 || startPos > endPos - minLength) {
        if (!friendlyPos) {
            return false;
        }
        if (startPos < 0) {
            startPos = 0;
        }
        if (startPos > endPos - minLength) {
            startPos = endPos - minLength;
        }
    }
    return true;
}


void
MSRouteHandler::addStop(const SUMOSAXAttributes &attrs) throw(ProcessError) {
    bool ok = true;
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
    stop.busstop = attrs.getOptStringReporting(SUMO_ATTR_BUS_STOP, "stop", 0, ok, "");
    if (stop.busstop!="") {
        // ok, we have obviously a bus stop
        MSBusStop *bs = MSNet::getInstance()->getBusStop(stop.busstop);
        if (bs!=0) {
            const MSLane &l = bs->getLane();
            stop.lane = l.getID();
            stop.endPos = bs->getEndLanePosition();
            stop.startPos = bs->getBeginLanePosition();
        } else {
            MsgHandler::getErrorInstance()->inform("The bus stop '" + stop.busstop + "' is not known" + errorSuffix);
            return;
        }
    } else {
        // no, the lane and the position should be given
        // get the lane
        stop.lane = attrs.getOptStringReporting(SUMO_ATTR_LANE, "stop", 0, ok, "");
        if (ok && stop.lane!="") {
            if (MSLane::dictionary(stop.lane)==0) {
                MsgHandler::getErrorInstance()->inform("The lane '" + stop.lane + "' for a stop is not known" + errorSuffix);
                return;
            }
        } else {
            MsgHandler::getErrorInstance()->inform("A stop must be placed on a bus stop or a lane" + errorSuffix);
            return;
        }
        if (myActivePlan &&
                !myActivePlan->empty() &&
                &myActivePlan->back()->getDestination() != &MSLane::dictionary(stop.lane)->getEdge()) {
            throw ProcessError("Disconnected plan for person '" + myVehicleParameter->id + "' (" + MSLane::dictionary(stop.lane)->getEdge().getID() + "!=" + myActivePlan->back()->getDestination().getID() + ").");
        }
        stop.endPos = attrs.getOptSUMORealReporting(SUMO_ATTR_ENDPOS, "stop", 0, ok, MSLane::dictionary(stop.lane)->getLength());
        if (attrs.hasAttribute(SUMO_ATTR_POSITION)) {
            WRITE_WARNING("Deprecated attribute 'pos' in description of stop" + errorSuffix);
            stop.endPos = attrs.getOptSUMORealReporting(SUMO_ATTR_POSITION, "stop", 0, ok, stop.endPos);
        }
        stop.startPos = attrs.getOptSUMORealReporting(SUMO_ATTR_STARTPOS, "stop", 0, ok, stop.endPos - 2 * POSITION_EPS);
        if (!ok || !checkStopPos(stop.startPos, stop.endPos, MSLane::dictionary(stop.lane)->getLength(), POSITION_EPS,
                                 attrs.getOptBoolReporting(SUMO_ATTR_FRIENDLY_POS, "stop", 0, ok, false))) {
            MsgHandler::getErrorInstance()->inform("Invalid start or end position for stop" + errorSuffix);
            return;
        }
    }

    // get the standing duration
    if (!attrs.hasAttribute(SUMO_ATTR_DURATION) && !attrs.hasAttribute(SUMO_ATTR_UNTIL)) {
        stop.triggered = attrs.getOptBoolReporting(SUMO_ATTR_TRIGGERED, "stop", 0, ok, true);
        stop.duration = -1;
        stop.until = -1;
    } else {
        stop.duration = attrs.getOptSUMOTimeReporting(SUMO_ATTR_DURATION, "stop", 0, ok, -1);
        stop.until = attrs.getOptSUMOTimeReporting(SUMO_ATTR_UNTIL, "stop", 0, ok, -1);
        if (!ok || (stop.duration<0&&stop.until<0)) {
            MsgHandler::getErrorInstance()->inform("Invalid duration or end time is given for a stop" + errorSuffix);
            return;
        }
        stop.triggered = attrs.getOptBoolReporting(SUMO_ATTR_TRIGGERED, "stop", 0, ok, false);
    }
    stop.parking = attrs.getOptBoolReporting(SUMO_ATTR_PARKING, "stop", 0, ok, stop.triggered);
    if (!ok) {
        MsgHandler::getErrorInstance()->inform("Invalid bool for 'triggered' or 'parking' for stop" + errorSuffix);
        return;
    }
    const std::string idx = attrs.getOptStringReporting(SUMO_ATTR_INDEX, "stop", 0, ok, "end");
    if (idx == "end") {
        stop.index = STOP_INDEX_END;
    } else if (idx == "fit") {
        stop.index = STOP_INDEX_FIT;
    } else {
        stop.index = attrs.getIntReporting(SUMO_ATTR_INDEX, "stop", 0, ok);
        if (!ok || stop.index < 0) {
            MsgHandler::getErrorInstance()->inform("Invalid 'index' for stop" + errorSuffix);
            return;
        }
    }
    if (myActiveRouteID != "") {
        myActiveRouteStops.push_back(stop);
    } else if (myActivePlan) {
        myActivePlan->push_back(new MSPerson::MSPersonStage_Waiting(MSLane::dictionary(stop.lane)->getEdge(), stop.duration, stop.until));
    } else {
        myVehicleParameter->stops.push_back(stop);
    }
}


/****************************************************************************/
