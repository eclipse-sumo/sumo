//---------------------------------------------------------------------------//
//                        RORDGenerator_ODAmounts.cpp -
//		Class for loading trip amount definitions and route generation
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Wed, 21 Jan 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.3  2004/01/27 08:45:00  dkrajzew
// given flow definitions an own tag
//
// Revision 1.2  2004/01/26 09:56:11  dkrajzew
// error handling corrected; forgotten call of to interval end and trip building inserted :-)
//
// Revision 1.1  2004/01/26 08:02:27  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <cassert>
#include <string>
#include <utils/xml/AttributesHandler.h>
#include <utils/xml/GenericSAX2Handler.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>
#include <utils/convert/ToString.h>
#include <utils/gfx/RGBColor.h>
#include <utils/gfx/GfxConvHelper.h>
#include "RORouteDef.h"
#include "RONet.h"
#include "RORouteDef_OrigDest.h"
#include "RORDGenerator_ODAmounts.h"
#include "RORouteDefList.h"
#include "ROVehicle.h"
#include "RORunningVehicle.h"
#include "RORouteDef_Complete.h"
#include "ROAbstractRouteDefLoader.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * RORDGenerator_ODAmounts::FlowDef - methods
 * ----------------------------------------------------------------------- */
RORDGenerator_ODAmounts::FlowDef::FlowDef(ROVehicle *vehicle,
                                          ROVehicleType *type,
                                          RORouteDef *route,
                                          unsigned int intBegin,
                                          unsigned int intEnd,
                                          unsigned int vehicles2Emit)
    : myVehicle(vehicle), myVehicleType(type), myRoute(route),
    myIntervalBegin(intBegin), myIntervalEnd(intEnd),
    myVehicle2EmitNumber(vehicles2Emit), myEmitted(0)
{
    assert(myIntervalBegin<myIntervalEnd);
}


RORDGenerator_ODAmounts::FlowDef::~FlowDef()
{
}


bool
RORDGenerator_ODAmounts::FlowDef::applicableForTime(unsigned int time) const
{
    return myIntervalBegin<=time&&myIntervalEnd>=time;
}


void
RORDGenerator_ODAmounts::FlowDef::addRoutes(RONet &net,
                                            unsigned int time)
{
    assert(myIntervalBegin<=time&&myIntervalEnd>=time);
    if(myVehicle->getID()=="3"&&time==18007) {
        int bla = 0;
    }
    //
    unsigned int absPerEachStep = myVehicle2EmitNumber /
        (myIntervalEnd-myIntervalBegin);
    for(unsigned int i=0; i<absPerEachStep; i++) {
        addSingleRoute(net, time);
    }
    // fraction
    double toEmit =
        (double) myVehicle2EmitNumber
        / (double) (myIntervalEnd-myIntervalBegin)
        * (double) (time-myIntervalBegin);
    if(toEmit>myEmitted) {
        addSingleRoute(net, time);
    }
}


void
RORDGenerator_ODAmounts::FlowDef::addSingleRoute(RONet &net,
                                                 unsigned int time)
{
    cout << time << ":" << myVehicle->getID() << endl;
    string id = myVehicle->getID()
        + string("_") + toString<unsigned int>(myEmitted);
    RORouteDef *rd = myRoute->copy(id);
    net.addRouteDef(rd);
    ROVehicle *veh = myVehicle->copy(id, time, rd);
    net.addVehicle(id, veh);
    myEmitted++;
}


unsigned int
RORDGenerator_ODAmounts::FlowDef::getIntervalEnd() const
{
    return myIntervalEnd;
}


/* -------------------------------------------------------------------------
 * RORDGenerator_ODAmounts - methods
 * ----------------------------------------------------------------------- */
RORDGenerator_ODAmounts::RORDGenerator_ODAmounts(RONet &net,
												 bool emptyDestinationsAllowed,
												 const std::string &fileName)
    : RORDLoader_TripDefs(net, emptyDestinationsAllowed, fileName),
    myEnded(false)
{
}


RORDGenerator_ODAmounts::~RORDGenerator_ODAmounts()
{
}


bool
RORDGenerator_ODAmounts::myReadRoutesAtLeastUntil(unsigned int until)
{
    // skip routes before begin
    if(until<=myBegin) {
        myDepartureTime = until;
        return true;
    }
    // build route definitions for the given timestep
    buildRoutes(until);
    return true;
}


void
RORDGenerator_ODAmounts::buildRoutes(unsigned int until)
{
    for(unsigned int time=myDepartureTime; time<until+1; time++) {
        buildForTimeStep(time);
    }
    myDepartureTime = time;
}


void
RORDGenerator_ODAmounts::buildForTimeStep(unsigned int time)
{
    myEnded = true;
    for(FlowDefV::const_iterator i=myFlows.begin(); i!=myFlows.end(); i++) {
        FlowDef *fd = *i;
        // skip flow definitions not valid for the current time
        if(fd->applicableForTime(time)) {
            fd->addRoutes(_net, time);
        }
        // check whether any further exists
        if(fd->getIntervalEnd()>time) {
            myEnded = false;
        }
    }
}


void
RORDGenerator_ODAmounts::myStartElement(int element,
										const std::string &name,
										const Attributes &attrs)
{
    switch(element) {
    case SUMO_TAG_FLOW:
        parseFlowAmountDef(attrs);
        break;
    case SUMO_TAG_INTERVAL:
        parseInterval(attrs);
        break;
    }
}


void
RORDGenerator_ODAmounts::parseFlowAmountDef(const Attributes &attrs)
{
    // get the vehicle id, the edges, the speed and position and
    //  the departure time and other information
    myID = getVehicleID(attrs);
    myBeginEdge = getEdge(attrs, "origin", SUMO_ATTR_FROM, myID, false);
    myEndEdge = getEdge(attrs, "destination",
        SUMO_ATTR_TO, myID, myEmptyDestinationsAllowed);
    myType = getVehicleType(attrs);
    myPos = getOptionalFloat(attrs, "pos", SUMO_ATTR_POS, myID);
    mySpeed = getOptionalFloat(attrs, "speed", SUMO_ATTR_SPEED, myID);
    try {
        myIntervalBegin = getIntSecure(attrs, SUMO_ATTR_BEGIN, myUpperIntervalBegin);
    } catch (NumberFormatException &e) {
        MsgHandler::getErrorInstance()->inform(
            string("An interval begin is not numeric."));
        return;
    }
    try {
        myIntervalEnd = getIntSecure(attrs, SUMO_ATTR_END, myUpperIntervalEnd);
    } catch (NumberFormatException &e) {
        MsgHandler::getErrorInstance()->inform(
            string("An interval end is not numeric."));
        return;
    }
    myVehicle2EmitNumber = getTime(attrs, SUMO_ATTR_NO, myID);
    if(myIntervalEnd<=myIntervalBegin) {
        MsgHandler::getErrorInstance()->inform(
            string("The interval must be larger than 0."));
        MsgHandler::getErrorInstance()->inform(
            string("The current values are: begin=")
            + toString<unsigned int>(myIntervalBegin)
            + string(" end=")
            + toString<unsigned int>(myIntervalEnd));
        return;
    }
    myPeriodTime = -1;
    myNumberOfRepetitions = -1;
    myLane = getLane(attrs);
    myColor = getRGBColorReporting(attrs, myID);
}


void
RORDGenerator_ODAmounts::parseInterval(const Attributes &attrs)
{
    try {
        myUpperIntervalBegin = getIntSecure(attrs, SUMO_ATTR_BEGIN, -1);
    } catch (NumberFormatException &e) {
        MsgHandler::getErrorInstance()->inform(
            string("An interval begin is not numeric."));
        return;
    }
    try {
        myUpperIntervalEnd = getIntSecure(attrs, SUMO_ATTR_END, -1);
    } catch (NumberFormatException &e) {
        MsgHandler::getErrorInstance()->inform(
            string("An interval end is not numeric."));
        return;
    }
}


void
RORDGenerator_ODAmounts::myCharacters(int element, const std::string &name,
									  const std::string &chars)
{
}


void
RORDGenerator_ODAmounts::myEndElement(int element, const std::string &name)
{
    switch(element) {
    case SUMO_TAG_TRIPDEF:
        myEndFlowAmountDef();
        break;
    case SUMO_TAG_INTERVAL:
        myEndInterval();
        break;
    }
}


void
RORDGenerator_ODAmounts::myEndInterval()
{
    myUpperIntervalBegin = -1;
    myUpperIntervalEnd = -1;
}


void
RORDGenerator_ODAmounts::myEndFlowAmountDef()
{
    if(!MsgHandler::getErrorInstance()->wasInformed()) {

		if(myIntervalEnd<myBegin) {
			return;
		}
        // add the vehicle type, the vehicle and the route to the net
        RORouteDef *route = new RORouteDef_OrigDest(myID, myColor,
            myBeginEdge, myEndEdge);
        ROVehicleType *type = _net.getVehicleTypeSecure(myType);
        // check whether any errors occured
        if(MsgHandler::getErrorInstance()->wasInformed()) {
            return;
        }
        // build the vehicle
        _net.addRouteDef(route);
        _nextRouteRead = true;
        ROVehicle *vehicle = 0;
        if(myPos>=0||mySpeed>=0) {
            vehicle = new RORunningVehicle(myID, route, myDepartureTime,
                type, myLane, myPos, mySpeed, myColor, -1, -1);
        } else {
            vehicle = new ROVehicle(myID, route, myDepartureTime,
                type, myColor, -1, -1);
        }
        // add to the container
        FlowDef *fd =
            new FlowDef(vehicle, type, route,
                myIntervalBegin, myIntervalEnd, myVehicle2EmitNumber);
        myFlows.push_back(fd);
    }
}


ROAbstractRouteDefLoader *
RORDGenerator_ODAmounts::getAssignedDuplicate(const std::string &file) const
{
    return new RORDGenerator_ODAmounts(_net,
        myEmptyDestinationsAllowed, file);
}


std::string
RORDGenerator_ODAmounts::getDataName() const
{
    return "XML-trip amount definitions";
}


bool
RORDGenerator_ODAmounts::myInit(OptionsCont &options)
{
    // read in the file on initialisation
    _parser->parse(_file.c_str());
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
{
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


