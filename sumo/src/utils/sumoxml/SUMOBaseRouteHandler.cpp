/****************************************************************************/
/// @file    SUMOBaseRouteHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 12.12.2005
/// @version $Id$
///
// A base class for parsing vehicles
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/gfx/GfxConvHelper.h>
#include "SUMOBaseRouteHandler.h"
#include <utils/xml/XMLBuildingExceptions.h>


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
SUMOBaseRouteHandler::SUMOBaseRouteHandler()
        : myCurrentDepart(0), myAmInEmbeddedMode(false)
{}


SUMOBaseRouteHandler::~SUMOBaseRouteHandler()
{}


SUMOTime
SUMOBaseRouteHandler::getVehicleDepartureTime(SUMOSAXHandler &helper,
        const Attributes &attrs,
        const std::string &id)
{
    SUMOTime ret = -1;
    try {
        ret = helper.getInt(attrs, SUMO_ATTR_DEPART); // !!! getSUMOTime
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Missing departure time in vehicle '" + id + "'.");
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform("Non-numerical departure time in vehicle '" + id + "'.");
    }
    return ret;
}


RGBColor
SUMOBaseRouteHandler::parseColor(SUMOSAXHandler &helper,
                                 const Attributes &attrs,
                                 const std::string &type,
                                 const std::string &id)
{
    RGBColor col;
    try {
        col = GfxConvHelper::parseColor(helper.getStringSecure(attrs, SUMO_ATTR_COLOR, "-1,-1,-1"));
    } catch (NumberFormatException &) {
        MsgHandler::getErrorInstance()->inform("The color definition for " + type + " '" + id + "' is malicious.");
    }
    return col;
}


SUMOVehicleClass
SUMOBaseRouteHandler::parseVehicleClass(SUMOSAXHandler &helper,
                                        const Attributes &attrs,
                                        const std::string &type,
                                        const std::string &id)
{
    SUMOVehicleClass vclass = SVC_UNKNOWN;
    try {
        string vclassS = helper.getStringSecure(attrs, SUMO_ATTR_VCLASS, "");
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
SUMOBaseRouteHandler::openVehicle(SUMOSAXHandler &helper,
                                  const Attributes &attrs,
                                  bool wantsVehicleColor)
{
    myAmInEmbeddedMode = true;
    try {
        myActiveVehicleID = helper.getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Missing id of a vehicle-object.");
        return false;
    }

    // try to get some optional values
    // repetition values
    myRepOffset = helper.getIntSecure(attrs, SUMO_ATTR_PERIOD, -1);
    myRepNumber = helper.getIntSecure(attrs, SUMO_ATTR_REPNUMBER, -1);
    // the vehicle's route name
    myCurrentRouteName = helper.getStringSecure(attrs, SUMO_ATTR_ROUTE, "");
    if (wantsVehicleColor) {
        myCurrentVehicleColor =
            parseColor(helper, attrs, "vehicle", myActiveVehicleID);
    }


    // try to get the vehicle type
    try {
        myCurrentVType = helper.getStringSecure(attrs, SUMO_ATTR_TYPE, "");
        // now try to build the rest of the vehicle
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Missing vehicle type for vehicle '" + myActiveVehicleID + "'");
        return false;
    }
    // try to get the departure time
    myCurrentDepart = getVehicleDepartureTime(helper, attrs, myActiveVehicleID);
    if (myCurrentDepart==-1) {
        return false;
    }
    // everything's fine!?
    return true;
}


void
SUMOBaseRouteHandler::closeVehicle()
{
    myAmInEmbeddedMode = false;
}



/****************************************************************************/

