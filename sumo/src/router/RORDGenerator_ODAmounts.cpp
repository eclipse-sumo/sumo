/****************************************************************************/
/// @file    RORDGenerator_ODAmounts.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 21 Jan 2004
/// @version $Id$
///
// Class for loading trip amount definitions and route generation
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

#include <cassert>
#include <string>
#include <algorithm>
#include <utils/options/OptionsCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/RandHelper.h>
#include <utils/common/StringUtils.h>
#include "RORouteDef.h"
#include "RONet.h"
#include "RORouteDef_OrigDest.h"
#include "RORDGenerator_ODAmounts.h"
#include "ROVehicle.h"
#include "RORouteDef_Complete.h"
#include "ROAbstractRouteDefLoader.h"
#include <utils/xml/SUMOVehicleParserHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * RORDGenerator_ODAmounts::FlowDef - methods
 * ----------------------------------------------------------------------- */
RORDGenerator_ODAmounts::FlowDef::FlowDef(ROVehicle *vehicle,
        SUMOVTypeParameter *type,
        RORouteDef *route,
        SUMOTime intBegin,
        SUMOTime intEnd,
        unsigned int vehicles2insert,
        bool randomize)
        : myVehicle(vehicle), myVehicleType(type), myRoute(route),
        myIntervalBegin(intBegin), myIntervalEnd(intEnd),
        myVehicle2InsertNumber(vehicles2insert), myInserted(0), myRandom(randomize) {
    assert(myIntervalBegin<myIntervalEnd);
    if (myRandom) {
        SUMOTime period = myIntervalEnd - myIntervalBegin;
        myDepartures.reserve(myVehicle2InsertNumber);
        for (size_t i=0; i<myVehicle2InsertNumber; ++i) {
            SUMOTime departure = ((int)(RandHelper::rand(period) / DELTA_T)) * DELTA_T;
            myDepartures.push_back(departure);
        }
        sort(myDepartures.begin(), myDepartures.end());
        reverse(myDepartures.begin(), myDepartures.end());
    }
}


RORDGenerator_ODAmounts::FlowDef::~FlowDef() {
    delete myVehicle;
}


bool
RORDGenerator_ODAmounts::FlowDef::applicableForTime(SUMOTime t) const {
    return myIntervalBegin<=t&&myIntervalEnd>t;
}


void
RORDGenerator_ODAmounts::FlowDef::addRoutes(RONet &net, SUMOTime t) {
    assert(myIntervalBegin<=t&&myIntervalEnd>=t);
    //
    if (!myRandom) {
        unsigned int absPerEachStep = myVehicle2InsertNumber / ((myIntervalEnd-myIntervalBegin) / DELTA_T);
        for (unsigned int i=0; i<absPerEachStep; i++) {
            addSingleRoute(net, t);
        }
        // fraction
        SUMOReal toInsert = (SUMOReal) myVehicle2InsertNumber / (SUMOReal)(myIntervalEnd-myIntervalBegin) * (SUMOReal)(t-myIntervalBegin+(SUMOReal)DELTA_T/2.);
        if (toInsert>myInserted) {
            addSingleRoute(net, t);
        }
    } else {
        while (myDepartures.size()>0&&*(myDepartures.end()-1)<t+DELTA_T) {
            addSingleRoute(net, t);
            myDepartures.pop_back();
        }
    }
}


void
RORDGenerator_ODAmounts::FlowDef::addSingleRoute(RONet &net, SUMOTime t) {
    std::string id = myVehicle->getID() + "_" + toString<unsigned int>(myInserted);
    RORouteDef *rd = myRoute->copy(id);
    net.addRouteDef(rd);
    ROVehicle *veh = myVehicle->copy(id, t, rd);
    net.addVehicle(id, veh);
    ++myInserted;
}


SUMOTime
RORDGenerator_ODAmounts::FlowDef::getIntervalEnd() const {
    return myIntervalEnd;
}


/* -------------------------------------------------------------------------
 * RORDGenerator_ODAmounts - methods
 * ----------------------------------------------------------------------- */
RORDGenerator_ODAmounts::RORDGenerator_ODAmounts(RONet &net,
        SUMOTime begin,
        SUMOTime end,
        bool emptyDestinationsAllowed,
        bool randomize,
        const std::string &fileName) throw(ProcessError)
        : RORDLoader_TripDefs(net, begin, end, emptyDestinationsAllowed, false, fileName),
        myRandom(randomize) {
    // read the complete file on initialisation
    myParser->parseReset(myToken);
    myParser->parse(getFileName().c_str());
    myDepartureTime = myBegin;
}


RORDGenerator_ODAmounts::~RORDGenerator_ODAmounts() throw() {
    for (FlowDefV::const_iterator i=myFlows.begin(); i!=myFlows.end(); i++) {
        delete(*i);
    }
}


bool
RORDGenerator_ODAmounts::readRoutesAtLeastUntil(SUMOTime until, bool skipping) throw(ProcessError) {
    UNUSED_PARAMETER(skipping);
    // skip routes before begin
    if (until<myBegin) {
        myDepartureTime = until;
        return true;
    }
    // build route definitions for the given timestep
    buildRoutes(until);
    return true;
}


void
RORDGenerator_ODAmounts::buildRoutes(SUMOTime until) throw() {
    SUMOTime t;
    for (t=myDepartureTime; t<until+1; t+=DELTA_T) {
        buildForTimeStep(t);
    }
    myDepartureTime = t;
}


void
RORDGenerator_ODAmounts::buildForTimeStep(SUMOTime time) throw() {
    if (time<myBegin||time>=myEnd) {
        return;
    }
    myEnded = true;
    for (FlowDefV::const_iterator i=myFlows.begin(); i!=myFlows.end(); i++) {
        FlowDef *fd = *i;
        // skip flow definitions not valid for the current time
        if (fd->applicableForTime(time)) {
            fd->addRoutes(myNet, time);
        }
        // check whether any further exists
        if (fd->getIntervalEnd()>time) {
            myEnded = false;
        }
    }
}


void
RORDGenerator_ODAmounts::myStartElement(SumoXMLTag element,
                                        const SUMOSAXAttributes &attrs) throw(ProcessError) {
    RORDLoader_TripDefs::myStartElement(element, attrs);
    if (element == SUMO_TAG_FLOW) {
        parseFlowAmountDef(attrs);
    } else if (element == SUMO_TAG_INTERVAL) {
        parseInterval(attrs);
    }
}


void
RORDGenerator_ODAmounts::parseFlowAmountDef(const SUMOSAXAttributes &attrs) throw(ProcessError) {
    // get the vehicle id, the edges, the speed and position and
    //  the departure time and other information
    std::string id = getVehicleID(attrs);
    if (myKnownIDs.find(id)!=myKnownIDs.end()) {
        throw ProcessError("The id '" + id + "' appears twice within the flow descriptions.'");
    }
    myKnownIDs.insert(id); // !!! a local storage is not save
    myBeginEdge = getEdge(attrs, "origin", SUMO_ATTR_FROM, id, false);
    myEndEdge = getEdge(attrs, "destination",
                        SUMO_ATTR_TO, id, myEmptyDestinationsAllowed);
    try {
        myParameter = SUMOVehicleParserHelper::parseVehicleAttributes(attrs, true, true);
    } catch (ProcessError &e) {
        throw ProcessError(StringUtils::replace(e.what(), "''", id.c_str()));
    }
    myParameter->id = id;
    bool ok = true;
    myIntervalBegin = attrs.getOptSUMOTimeReporting(SUMO_ATTR_BEGIN, "flow", id.c_str(), ok, myUpperIntervalBegin);
    myIntervalEnd = attrs.getOptSUMOTimeReporting(SUMO_ATTR_END, "flow", id.c_str(), ok, myUpperIntervalEnd);
    myVehicle2InsertNumber = attrs.getIntReporting(SUMO_ATTR_NO, "flow", id.c_str(), ok); // !!! no real error handling
    if (!ok) {
        throw ProcessError();
    }
    if (myIntervalEnd<=myIntervalBegin) {
        throw ProcessError("The interval must be larger than 0.\n The current values are: begin=" + toString<unsigned int>(myIntervalBegin) + " end=" + toString<unsigned int>(myIntervalEnd));
    }
}


void
RORDGenerator_ODAmounts::parseInterval(const SUMOSAXAttributes &attrs) {
    bool ok = true;
    myUpperIntervalBegin = attrs.getOptSUMOTimeReporting(SUMO_ATTR_BEGIN, "interval", 0, ok, -1); // !!!really optional ?
    myUpperIntervalEnd = attrs.getOptSUMOTimeReporting(SUMO_ATTR_END, "interval", 0, ok, -1); // !!!really optional ?
}


void
RORDGenerator_ODAmounts::myEndElement(SumoXMLTag element) throw(ProcessError) {
    RORDLoader_TripDefs::myEndElement(element);
    if (element == SUMO_TAG_FLOW) {
        myEndFlowAmountDef();
    } else if (element == SUMO_TAG_INTERVAL) {
        myEndInterval();
    }
}


void
RORDGenerator_ODAmounts::myEndInterval() {
    myUpperIntervalBegin = 0; // !!! was -1
    myUpperIntervalEnd = 0; // !!! was: -1
}


void
RORDGenerator_ODAmounts::myEndFlowAmountDef() {
    if (!MsgHandler::getErrorInstance()->wasInformed()) {

        if (myIntervalEnd<myBegin) {
            return;
        }
        // add the vehicle type, the vehicle and the route to the net
        RGBColor *col = myParameter->wasSet(VEHPARS_COLOR_SET) ? new RGBColor(myParameter->color) : 0;
        RORouteDef *route = new RORouteDef_OrigDest(myParameter->id, col, myBeginEdge, myEndEdge);//!!! set double in route def and flowdef?
        SUMOVTypeParameter *type = myNet.getVehicleTypeSecure(myParameter->vtypeid);
        // check whether any errors occured
        if (MsgHandler::getErrorInstance()->wasInformed()) {
            return;
        }
        // build the vehicle
        if (myNet.addRouteDef(route)) {
            myNextRouteRead = true;
            ROVehicle *vehicle = new ROVehicle(*myParameter, route, type);
            // add to the container
            FlowDef *fd = new FlowDef(vehicle, type, route, myIntervalBegin, myIntervalEnd, myVehicle2InsertNumber, myRandom);
            myFlows.push_back(fd);
        } else {
            MsgHandler::getErrorInstance()->inform("The vehicle '" + myParameter->id + "' occurs at least twice.");
            delete route;
        }
        delete myParameter;
        myParameter = 0;
    }
}


/****************************************************************************/

