//---------------------------------------------------------------------------//
//                        RORDGenerator_ODAmounts.cpp -
//      Class for loading trip amount definitions and route generation
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
// Revision 1.14  2006/01/09 12:00:58  dkrajzew
// debugging vehicle color usage
//
// Revision 1.13  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.12  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.11  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.10  2005/05/04 08:50:05  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.9  2005/02/17 10:33:40  dkrajzew
// code beautifying;
// Linux building patched;
// warnings removed;
// new configuration usage within guisim
//
// Revision 1.8  2004/11/23 10:25:52  dkrajzew
// debugging
//
// Revision 1.7  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT;
//  preparation of classes to be derived for an online-routing
//
// Revision 1.6  2004/02/16 13:47:07  dkrajzew
// Type-dependent loader/generator-"API" changed
//
// Revision 1.5  2004/02/10 07:16:05  dkrajzew
// removed some debug-variables
//
// Revision 1.4  2004/02/02 16:19:23  dkrajzew
// trying to catch up false user input
//  (using the same name for different routes)
//
// Revision 1.3  2004/01/27 08:45:00  dkrajzew
// given flow definitions an own tag
//
// Revision 1.2  2004/01/26 09:56:11  dkrajzew
// error handling corrected;
//  forgotten call of to interval end and trip building inserted :-)
//
// Revision 1.1  2004/01/26 08:02:27  dkrajzew
// loaders and route-def types are now renamed in an senseful way;
//  further changes in order to make both new routers work;
//  documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <cassert>
#include <string>
#include <utils/xml/AttributesHandler.h>
#include <utils/xml/GenericSAX2Handler.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
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

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


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
                                          SUMOTime intBegin,
                                          SUMOTime intEnd,
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
    unsigned int absPerEachStep = myVehicle2EmitNumber /
        (myIntervalEnd-myIntervalBegin);
    for(unsigned int i=0; i<absPerEachStep; i++) {
        addSingleRoute(vb, net, t);
    }
    // fraction
    SUMOReal toEmit =
        (SUMOReal) myVehicle2EmitNumber
        / (SUMOReal) (myIntervalEnd-myIntervalBegin)
        * (SUMOReal) (t-myIntervalBegin);
    if(toEmit>myEmitted) {
        addSingleRoute(vb, net, t);
    }
}


void
RORDGenerator_ODAmounts::FlowDef::addSingleRoute(ROVehicleBuilder &vb,
                                                 RONet &net,
                                                 SUMOTime t)
{
    string id = myVehicle->getID()
        + string("_") + toString<unsigned int>(myEmitted);
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
                                                 const std::string &fileName)
    : RORDLoader_TripDefs(vb, net, begin, end, emptyDestinationsAllowed, fileName),
    myEnded(false)
{
}


RORDGenerator_ODAmounts::~RORDGenerator_ODAmounts()
{
}


bool
RORDGenerator_ODAmounts::myReadRoutesAtLeastUntil(SUMOTime until)
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
RORDGenerator_ODAmounts::buildRoutes(SUMOTime until)
{
    SUMOTime t;
    for(t=myDepartureTime; t<until+1; t++) {
        buildForTimeStep(t);
    }
    myDepartureTime = t;
}


void
RORDGenerator_ODAmounts::buildForTimeStep(SUMOTime time)
{
    if(time<myBegin||time>=myEnd) {
        return;
    }
    myEnded = true;
    for(FlowDefV::const_iterator i=myFlows.begin(); i!=myFlows.end(); i++) {
        FlowDef *fd = *i;
        // skip flow definitions not valid for the current time
        if(fd->applicableForTime(time)) {
            fd->addRoutes(myVehicleBuilder, _net, time);
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
    RORDLoader_TripDefs::myStartElement(element, name, attrs);
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
    if(myKnownIDs.find(myID)!=myKnownIDs.end()) {
        MsgHandler::getErrorInstance()->inform(
            string("The id '") + myID
            + string("' appears twice within the flow descriptions.'"));
        return;
    }
    myKnownIDs.insert(myID); // !!! a local storage is not save
    myBeginEdge = getEdge(attrs, "origin", SUMO_ATTR_FROM, myID, false);
    myEndEdge = getEdge(attrs, "destination",
        SUMO_ATTR_TO, myID, myEmptyDestinationsAllowed);
    myType = getVehicleType(attrs);
    myPos = getOptionalFloat(attrs, "pos", SUMO_ATTR_POS, myID);
    mySpeed = getOptionalFloat(attrs, "speed", SUMO_ATTR_SPEED, myID);
    try {
        myIntervalBegin = getIntSecure(attrs, SUMO_ATTR_BEGIN, myUpperIntervalBegin);
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform(
            string("An interval begin is not numeric."));
        return;
    }
    try {
        myIntervalEnd = getIntSecure(attrs, SUMO_ATTR_END, myUpperIntervalEnd);
    } catch (NumberFormatException &) {
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
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform(
            string("An interval begin is not numeric."));
        return;
    }
    try {
        myUpperIntervalEnd = getIntSecure(attrs, SUMO_ATTR_END, -1);
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform(
            string("An interval end is not numeric."));
        return;
    }
}


void
RORDGenerator_ODAmounts::myCharacters(int element, const std::string &name,
                                      const std::string &chars)
{
    RORDLoader_TripDefs::myCharacters(element, name, chars);
}


void
RORDGenerator_ODAmounts::myEndElement(int element, const std::string &name)
{
    RORDLoader_TripDefs::myEndElement(element, name);
    switch(element) {
    case SUMO_TAG_FLOW:
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
    myUpperIntervalBegin = 0; // !!! was -1
    myUpperIntervalEnd = 0; // !!! was: -1
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
            vehicle = myVehicleBuilder.buildRunningVehicle(myID, route, myDepartureTime,
                type, myLane, (SUMOReal) myPos, (SUMOReal) mySpeed, myColor, -1, -1);
        } else {
            vehicle = myVehicleBuilder.buildVehicle(myID, route, myDepartureTime,
                type, myColor, -1, -1);
        }
        // add to the container
        FlowDef *fd =
            new FlowDef(vehicle, type, route,
                myIntervalBegin, myIntervalEnd, myVehicle2EmitNumber);
        myFlows.push_back(fd);
    }
}


std::string
RORDGenerator_ODAmounts::getDataName() const
{
    return "XML-flow definitions";
}


bool
RORDGenerator_ODAmounts::init(OptionsCont &options)
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


