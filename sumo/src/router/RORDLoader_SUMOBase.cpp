//---------------------------------------------------------------------------//
//                        RORDLoader_SUMOBase.cpp -
//      The base class for SUMO-native route handlers
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
// Revision 1.4  2005/02/17 10:33:40  dkrajzew
// code beautifying;
// Linux building patched;
// warnings removed;
// new configuration usage within guisim
//
// Revision 1.3  2004/12/16 12:26:52  dkrajzew
// debugging
//
// Revision 1.2  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived for an online-routing
//
// Revision 1.1  2004/01/26 08:02:27  dkrajzew
// loaders and route-def types are now renamed in an senseful way; further changes in order to make both new routers work; documentation added
//
// ------------------------------------------------
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "RORDLoader_SUMOBase.h"
#include "ROVehicleType.h"
#include "RORouteDef.h"
#include "RONet.h"
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/gfx/GfxConvHelper.h>
#include "ROVehicleType_Krauss.h"
#include "ROVehicleBuilder.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
RORDLoader_SUMOBase::RORDLoader_SUMOBase(ROVehicleBuilder &vb, RONet &net,
                                         unsigned int begin, unsigned int end,
                                         const std::string &dataName,
                                         const std::string &file)
    : ROTypedXMLRoutesLoader(vb, net, begin, end, file),
    myDataName(dataName), myDepartureTime(0)
{
}


RORDLoader_SUMOBase::~RORDLoader_SUMOBase()
{
}


float
RORDLoader_SUMOBase::getFloatReporting(const Attributes &attrs,
                                       AttrEnum attr,
                                       const std::string &id,
                                       const std::string &name)
{
    try {
        return getFloat(attrs, attr);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            string("Missing ") + name + string(" in vehicle '") +
            id + string("'."));
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform(
            name + string(" in vehicle '")
            + id + string("' is not numeric."));
    }
    return -1;
}


ROVehicleType*
RORDLoader_SUMOBase::getVehicleType(const Attributes &attrs,
                                  const std::string &id)
{
    ROVehicleType *type = 0;
    try {
        string name = getString(attrs, SUMO_ATTR_TYPE);
        type = _net.getVehicleType(name);
        if(type==0) {
            MsgHandler::getErrorInstance()->inform(string(
                "The type of the vehicle '")
                + name + string("' is not known."));
        }
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            string("Missing type in vehicle '")
            + id + string("'."));
    }
    return type;
}


void
RORDLoader_SUMOBase::getVehicleDepartureTime(const Attributes &attrs,
                                             const std::string &id)
{
    myDepartureTime = 0; // !!! was: -1
    try {
        myDepartureTime = getLong(attrs, SUMO_ATTR_DEPART);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            string("Missing departure time in vehicle '")
            + id + string("'."));
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform(
            string("Non-numerical departure time in vehicle '")
            + id + string("'."));
    }
}


RORouteDef *
RORDLoader_SUMOBase::getVehicleRoute(const Attributes &attrs,
                                     const std::string &id)
{
    RORouteDef *route = 0;
    try {
        string name = getString(attrs, SUMO_ATTR_ROUTE);
        route = _net.getRouteDef(name);
        if(route==0) {
            MsgHandler::getErrorInstance()->inform(
                string("The route of the vehicle '")
                + name + string("' is not known."));
            return 0;
        }
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform(
            string("Missing route in vehicle '")
            + id + string("'."));
    }
    return route;
}


RGBColor
RORDLoader_SUMOBase::parseColor(const Attributes &attrs,
                                const std::string &type,
                                const std::string &id)
{
    RGBColor col;
    try {
        col = GfxConvHelper::parseColor(getString(attrs, SUMO_ATTR_COLOR));
    } catch (EmptyData) {
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform(
            string("The color definition for ") + type + (" '") +
            id + string("' is malicious."));
    }
    return col;
}


std::string
RORDLoader_SUMOBase::getDataName() const
{
    return myDataName;
}


void
RORDLoader_SUMOBase::startVehicle(const Attributes &attrs)
{
    // get the vehicle id
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Missing id in vehicle.");
        return;
    }
    // get vehicle type
    ROVehicleType *type = getVehicleType(attrs, id);
    // get the departure time
    getVehicleDepartureTime(attrs, id);
    // get the route id
    RORouteDef *route = getVehicleRoute(attrs, id);
    // get the vehicle color
    RGBColor color = parseColor(attrs, "vehicle", id);
    // build the vehicle
    // get further optional information
    int repOffset = getIntSecure(attrs, SUMO_ATTR_PERIOD, -1);
    int repNumber = getIntSecure(attrs, SUMO_ATTR_REPNUMBER, -1);
    if(!MsgHandler::getErrorInstance()->wasInformed()) {
        if(myDepartureTime<myBegin||myDepartureTime>=myEnd) {
            _net.removeRouteSecure(route);
            // !!! was ist mit type?
            return;
        }
        _net.addVehicle(id, myVehicleBuilder.buildVehicle(
            id, route, myDepartureTime, type, color,
            repOffset, repNumber));
    }
}


void
RORDLoader_SUMOBase::startVehType(const Attributes &attrs)
{
    // get the vehicle type id
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Missing id in vtype.");
        return;
    }
    // get the other values
    float maxspeed = getFloatReporting(attrs, SUMO_ATTR_MAXSPEED,
        id, "maxspeed");
    float length = getFloatReporting(attrs, SUMO_ATTR_LENGTH,
        id, "length");
    float accel = getFloatReporting(attrs, SUMO_ATTR_ACCEL,
        id, "accel");
    float decel = getFloatReporting(attrs, SUMO_ATTR_DECEL,
        id, "decel");
    float sigma = getFloatReporting(attrs, SUMO_ATTR_SIGMA,
        id, "sigma");
    RGBColor color = parseColor(attrs, "vehicle type", id);
    // build the vehicle type after checking
    //  by now, only vehicles using the krauss model are supported
    if(maxspeed>0&&length>0&&accel>0&&decel>0&&sigma>0) {
        _net.addVehicleType(
            new ROVehicleType_Krauss(
                id, color, length, accel, decel, sigma, maxspeed));
    }
}


unsigned int
RORDLoader_SUMOBase::getCurrentTimeStep() const
{
    return myDepartureTime;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

