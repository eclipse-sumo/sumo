/***************************************************************************
                          SUMOBaseRouteHandler.cpp
                          A base class for parsing vehicles
                             -------------------
    project              : SUMO
    begin                : Mon, 12.12.2005
    copyright            : (C) 2005 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
     const char rcsid[] =
         "$Id$";
}
// $Log$
// Revision 1.2  2006/01/26 08:54:59  dkrajzew
// missing files added
//
// Revision 1.1  2006/01/09 13:38:36  dkrajzew
// debugging vehicle color usage
//
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

#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/gfx/GfxConvHelper.h>
#include "SUMOBaseRouteHandler.h"
#include <utils/xml/XMLBuildingExceptions.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
SUMOBaseRouteHandler::SUMOBaseRouteHandler()
    : myAmInEmbeddedMode(false), myCurrentDepart(0)
{
}


SUMOBaseRouteHandler::~SUMOBaseRouteHandler()
{
}


SUMOReal
SUMOBaseRouteHandler::getFloatReporting(SUMOSAXHandler &helper,
                                        const Attributes &attrs,
                                        AttrEnum attr,
                                        const std::string &id,
                                        const std::string &name)
{
    try {
        return helper.getFloat(attrs, attr);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(string("Missing ") + name + string(" in vehicle '") + id + string("'."));
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform(name + string(" in vehicle '") + id + string("' is not numeric."));
    }
    return -1;
}


SUMOTime
SUMOBaseRouteHandler::getVehicleDepartureTime(SUMOSAXHandler &helper,
                                              const Attributes &attrs,
                                              const std::string &id)
{
    SUMOTime ret = -1;
    try {
        ret = helper.getInt(attrs, SUMO_ATTR_DEPART); // !!! getSUMOTime
    } catch (EmptyData) {
        //!!!getErrorHandlerMarkInvalid()->
        MsgHandler::getErrorInstance()->inform("Missing departure time in vehicle '" + id + "'.");
    } catch (NumberFormatException) {
        //getErrorHandlerMarkInvalid()->
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
		if(vclassS=="") {
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
    if(wantsVehicleColor) {
        myCurrentVehicleColor =
            parseColor(helper, attrs, "vehicle", myActiveVehicleID);
    }


    // try to get the vehicle type
    try {
        myCurrentVType = helper.getString(attrs, SUMO_ATTR_TYPE);
    // now try to build the rest of the vehicle
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Missing vehicle type for vehicle '" + myActiveVehicleID + "'");
        return false;
    }
    // try to get the departure time
    myCurrentDepart = getVehicleDepartureTime(helper, attrs, myActiveVehicleID);
    if(myCurrentDepart==-1) {
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
