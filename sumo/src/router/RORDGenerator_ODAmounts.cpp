/****************************************************************************/
/// @file    RORDGenerator_ODAmounts.cpp
/// @author  Daniel Krajzewicz
/// @date    Wed, 21 Jan 2004
/// @version $Id$
///
// Class for loading trip amount definitions and route generation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
#include "RORouteDef.h"
#include "RONet.h"
#include "RORouteDef_OrigDest.h"
#include "RORDGenerator_ODAmounts.h"
#include "ROVehicle.h"
#include "ROVehicleBuilder.h"
#include "RORunningVehicle.h"
#include "RORouteDef_Complete.h"
#include "ROAbstractRouteDefLoader.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * RORDGenerator_ODAmounts::FlowDef - methods
 * ----------------------------------------------------------------------- */
RORDGenerator_ODAmounts::FlowDef::FlowDef(ROVehicle *vehicle,
        ROVehicleType *type,
        RORouteDef *route,
        SUMOTime intBegin,
        SUMOTime intEnd,
        unsigned int vehicles2Emit,
        bool randomize)
        : myVehicle(vehicle), myVehicleType(type), myRoute(route),
        myIntervalBegin(intBegin), myIntervalEnd(intEnd),
        myVehicle2EmitNumber(vehicles2Emit), myEmitted(0), myRandom(randomize)
{
    assert(myIntervalBegin<myIntervalEnd);
    if (myRandom) {
        SUMOTime period = myIntervalEnd - myIntervalBegin;
        myDepartures.reserve(myVehicle2EmitNumber);
        for (size_t i=0; i<myVehicle2EmitNumber; ++i) {
            SUMOTime departure = RandHelper::rand(period);
            myDepartures.push_back(departure);
        }
        sort(myDepartures.begin(), myDepartures.end());
        reverse(myDepartures.begin(), myDepartures.end());
    }
}


RORDGenerator_ODAmounts::FlowDef::~FlowDef()
{
    delete myVehicle;
}


bool
RORDGenerator_ODAmounts::FlowDef::applicableForTime(SUMOTime t) const
{
    return myIntervalBegin<=t&&myIntervalEnd>t;
}


void
RORDGenerator_ODAmounts::FlowDef::addRoutes(ROVehicleBuilder &vb,
        RONet &net,
        SUMOTime t)
{
    assert(myIntervalBegin<=t&&myIntervalEnd>=t);
    //
    if (!myRandom) {
        unsigned int absPerEachStep = myVehicle2EmitNumber / (myIntervalEnd-myIntervalBegin);
        for (unsigned int i=0; i<absPerEachStep; i++) {
            addSingleRoute(vb, net, t);
        }
        // fraction
        SUMOReal toEmit =
            (SUMOReal) myVehicle2EmitNumber
            / (SUMOReal)(myIntervalEnd-myIntervalBegin)
            * (SUMOReal)(t-myIntervalBegin);
        if (toEmit>myEmitted) {
            addSingleRoute(vb, net, t);
        }
    } else {
        while (myDepartures.size()>0&&*(myDepartures.end()-1)==t) {
            addSingleRoute(vb, net, t);
            myDepartures.pop_back();
        }
    }
}


void
RORDGenerator_ODAmounts::FlowDef::addSingleRoute(ROVehicleBuilder &vb,
        RONet &net,
        SUMOTime t)
{
    string id = myVehicle->getID() + "_" + toString<unsigned int>(myEmitted);
    RORouteDef *rd = myRoute->copy(id);
    net.addRouteDef(rd);
    ROVehicle *veh = myVehicle->copy(vb, id, t, rd);
    net.addVehicle(id, veh);
    myEmitted++;
}


SUMOTime
RORDGenerator_ODAmounts::FlowDef::getIntervalEnd() const
{
    return myIntervalEnd;
}


/* -------------------------------------------------------------------------
 * RORDGenerator_ODAmounts - methods
 * ----------------------------------------------------------------------- */
RORDGenerator_ODAmounts::RORDGenerator_ODAmounts(ROVehicleBuilder &vb,
        RONet &net,
        SUMOTime begin,
        SUMOTime end,
        bool emptyDestinationsAllowed,
        bool randomize,
        const std::string &fileName)
        : RORDLoader_TripDefs(vb, net, begin, end, emptyDestinationsAllowed, fileName),
        myEnded(false), myRandom(randomize)
{}


RORDGenerator_ODAmounts::~RORDGenerator_ODAmounts() throw()
{
    for (FlowDefV::const_iterator i=myFlows.begin(); i!=myFlows.end(); i++) {
        delete(*i);
    }
}


bool
RORDGenerator_ODAmounts::myReadRoutesAtLeastUntil(SUMOTime until)
{
    // skip routes before begin
    if (until<=myBegin) {
        myDepartureTime = until;
        return true;
    }
    // build route definitions for the given timestep
    buildRoutes(until);
    return true;
}


void
RORDGenerator_ODAmounts::buildRoutes(SUMOTime until)
{
    SUMOTime t;
    for (t=myDepartureTime; t<until+1; t++) {
        buildForTimeStep(t);
    }
    myDepartureTime = t;
}


void
RORDGenerator_ODAmounts::buildForTimeStep(SUMOTime time)
{
    if (time<myBegin||time>=myEnd) {
        return;
    }
    myEnded = true;
    for (FlowDefV::const_iterator i=myFlows.begin(); i!=myFlows.end(); i++) {
        FlowDef *fd = *i;
        // skip flow definitions not valid for the current time
        if (fd->applicableForTime(time)) {
            fd->addRoutes(myVehicleBuilder, myNet, time);
        }
        // check whether any further exists
        if (fd->getIntervalEnd()>time) {
            myEnded = false;
        }
    }
}


void
RORDGenerator_ODAmounts::myStartElement(SumoXMLTag element,
                                        const Attributes &attrs) throw(ProcessError)
{
    RORDLoader_TripDefs::myStartElement(element, attrs);
    if (element == SUMO_TAG_FLOW) {
        parseFlowAmountDef(attrs);
    } else if (element == SUMO_TAG_INTERVAL) {
        parseInterval(attrs);
    }
}


void
RORDGenerator_ODAmounts::parseFlowAmountDef(const Attributes &attrs)
{
    // get the vehicle id, the edges, the speed and position and
    //  the departure time and other information
    myID = getVehicleID(attrs);
    if (myKnownIDs.find(myID)!=myKnownIDs.end()) {
        MsgHandler::getErrorInstance()->inform("The id '" + myID + "' appears twice within the flow descriptions.'");
        return;
    }
    myKnownIDs.insert(myID); // !!! a local storage is not save
    myBeginEdge = getEdge(attrs, "origin", SUMO_ATTR_FROM, myID, false);
    myEndEdge = getEdge(attrs, "destination",
                        SUMO_ATTR_TO, myID, myEmptyDestinationsAllowed);
    myType = getStringSecure(attrs, SUMO_ATTR_TYPE, "");
    myPos = getOptionalFloat(attrs, "pos", SUMO_ATTR_POSITION, myID);
    mySpeed = getOptionalFloat(attrs, "speed", SUMO_ATTR_SPEED, myID);
    try {
        myIntervalBegin = getIntSecure(attrs, SUMO_ATTR_BEGIN, myUpperIntervalBegin);
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("An interval begin is not numeric.");
        return;
    }
    try {
        myIntervalEnd = getIntSecure(attrs, SUMO_ATTR_END, myUpperIntervalEnd);
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("An interval end is not numeric.");
        return;
    }
    myVehicle2EmitNumber = getTime(attrs, SUMO_ATTR_NO, myID);
    if (myIntervalEnd<=myIntervalBegin) {
        MsgHandler::getErrorInstance()->inform("The interval must be larger than 0.\n The current values are: begin=" + toString<unsigned int>(myIntervalBegin) + " end=" + toString<unsigned int>(myIntervalEnd));
        return;
    }
    myPeriodTime = -1;
    myNumberOfRepetitions = -1;
    myLane = getLane(attrs);
    myColor = getStringSecure(attrs, SUMO_ATTR_COLOR, "");
}


void
RORDGenerator_ODAmounts::parseInterval(const Attributes &attrs)
{
    try {
        myUpperIntervalBegin = getIntSecure(attrs, SUMO_ATTR_BEGIN, -1);
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("An interval begin is not numeric.");
        return;
    }
    try {
        myUpperIntervalEnd = getIntSecure(attrs, SUMO_ATTR_END, -1);
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("An interval end is not numeric.");
        return;
    }
}


void
RORDGenerator_ODAmounts::myCharacters(SumoXMLTag element,
                                      const std::string &chars) throw(ProcessError)
{
    RORDLoader_TripDefs::myCharacters(element, chars);
}


void
RORDGenerator_ODAmounts::myEndElement(SumoXMLTag element) throw(ProcessError)
{
    RORDLoader_TripDefs::myEndElement(element);
    if (element == SUMO_TAG_FLOW) {
        myEndFlowAmountDef();
    } else if (element == SUMO_TAG_INTERVAL) {
        myEndInterval();
    }
}


void
RORDGenerator_ODAmounts::myEndInterval()
{
    myUpperIntervalBegin = 0; // !!! was -1
    myUpperIntervalEnd = 0; // !!! was: -1
}


void
RORDGenerator_ODAmounts::myEndFlowAmountDef()
{
    if (!MsgHandler::getErrorInstance()->wasInformed()) {

        if (myIntervalEnd<myBegin) {
            return;
        }
        // add the vehicle type, the vehicle and the route to the net
        RORouteDef *route = new RORouteDef_OrigDest(myID, myColor,
                myBeginEdge, myEndEdge);
        ROVehicleType *type = myNet.getVehicleTypeSecure(myType);
        // check whether any errors occured
        if (MsgHandler::getErrorInstance()->wasInformed()) {
            return;
        }
        // build the vehicle
        myNet.addRouteDef(route);
        myNextRouteRead = true;
        ROVehicle *vehicle = 0;
        if (myPos>=0||mySpeed>=0) {
            vehicle = myVehicleBuilder.buildRunningVehicle(myID, route, myDepartureTime,
                      type, myLane, (SUMOReal) myPos, (SUMOReal) mySpeed, myColor, -1, -1);
        } else {
            vehicle = myVehicleBuilder.buildVehicle(myID, route, myDepartureTime,
                                                    type, myColor, -1, -1);
        }
        // add to the container
        FlowDef *fd =
            new FlowDef(vehicle, type, route, myIntervalBegin, myIntervalEnd,
                        myVehicle2EmitNumber, myRandom);
        myFlows.push_back(fd);
    }
}


std::string
RORDGenerator_ODAmounts::getDataName() const
{
    return "XML-flow definitions";
}


bool
RORDGenerator_ODAmounts::init(OptionsCont &)
{
    // read in the file on initialisation
    myParser->parse(getFileName().c_str());
    myDepartureTime = myBegin;
    return true;
}


bool
RORDGenerator_ODAmounts::ended() const
{
    return myEnded;
}


void
RORDGenerator_ODAmounts::closeReading()
{}



/****************************************************************************/

