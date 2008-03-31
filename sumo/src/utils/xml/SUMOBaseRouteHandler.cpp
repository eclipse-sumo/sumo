/****************************************************************************/
/// @file    SUMOBaseRouteHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 12.12.2005
/// @version $Id$
///
// Base class for parsing vehicles
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

#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include "SUMOBaseRouteHandler.h"

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
SUMOBaseRouteHandler::SUMOBaseRouteHandler() throw()
        : myCurrentDepart(0), myAmInEmbeddedMode(false)
{}


SUMOBaseRouteHandler::~SUMOBaseRouteHandler() throw()
{}


SUMOTime
SUMOBaseRouteHandler::getVehicleDepartureTime(
        const SUMOSAXAttributes &attrs,
        const std::string &id) throw()
{
    SUMOTime ret = -1;
    try {
        ret = attrs.getInt(SUMO_ATTR_DEPART); // !!! getSUMOTime
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing departure time in vehicle '" + id + "'.");
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("Non-numerical departure time in vehicle '" + id + "'.");
    }
    return ret;
}


bool
SUMOBaseRouteHandler::parseVehicleColor(const SUMOSAXAttributes &) throw()
{
    return true;
}


SUMOVehicleClass
SUMOBaseRouteHandler::parseVehicleClass(const SUMOSAXAttributes &attrs,
                                        const std::string &type,
                                        const std::string &id) throw()
{
    SUMOVehicleClass vclass = SVC_UNKNOWN;
    try {
        string vclassS = attrs.getStringSecure(SUMO_ATTR_VCLASS, "");
        if (vclassS=="") {
            return vclass;
        }
        return getVehicleClassID(vclassS);
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("The class for " + type + " '" + id + "' is not known.");
    }
    return vclass;
}


bool
SUMOBaseRouteHandler::openVehicle(const SUMOSAXAttributes &attrs) throw()
{
    myAmInEmbeddedMode = true;
    // get the id, report an error if not given or empty...
    if(!attrs.setIDFromAttribues("vehicle", myActiveVehicleID)) {
        return false;
    }

    // try to get some optional values
    // repetition values
    myRepOffset = attrs.getIntSecure(SUMO_ATTR_PERIOD, -1);
    myRepNumber = attrs.getIntSecure(SUMO_ATTR_REPNUMBER, -1);
    // the vehicle's route name
    myCurrentRouteName = attrs.getStringSecure(SUMO_ATTR_ROUTE, "");
    if (!parseVehicleColor(attrs)) {
        MsgHandler::getErrorInstance()->inform("Incorrect color definition for vehicle " + myActiveVehicleID);
    }


    // try to get the vehicle type
    try {
        myCurrentVType = attrs.getStringSecure(SUMO_ATTR_TYPE, "");
        // now try to build the rest of the vehicle
    } catch (EmptyData &) {
        MsgHandler::getErrorInstance()->inform("Missing vehicle type name for vehicle '" + myActiveVehicleID + "'");
        return false;
    }
    // try to get the departure time
    myCurrentDepart = getVehicleDepartureTime(attrs, myActiveVehicleID);
    if (myCurrentDepart==-1) {
        return false;
    }
    // everything's fine!?
    return true;
}


bool
SUMOBaseRouteHandler::closeVehicle() throw(ProcessError)
{
    myAmInEmbeddedMode = false;
    return true;
}



/****************************************************************************/

