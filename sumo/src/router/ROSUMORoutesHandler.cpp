//---------------------------------------------------------------------------//
//                        ROSUMORoutesHandler.cpp -
//  A SAX-handler for SUMO-routes
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
// Revision 1.4  2003/03/20 16:39:17  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.3  2003/03/03 15:22:36  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:45:06  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include <string>
#include <utils/xml/GenericSAX2Handler.h>
#include <utils/xml/AttributesHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SErrorHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include "RORouteDef.h"
#include "ROCompleteRouteDef.h"
#include "ROVehicle.h"
#include "ROVehicleType.h"
#include "ROVehicleType_Krauss.h"
#include "ROSUMORoutesHandler.h"
#include "ROEdgeVector.h"
#include "RONet.h"

using namespace std;

ROSumoRoutesHandler::ROSumoRoutesHandler(RONet &net, const std::string &file)
    : ROTypedXMLRoutesLoader(net, file)
{
/*    _attrHandler.add(SUMO_ATTR_ID, "id");
    _attrHandler.add(SUMO_ATTR_MAXSPEED, "maxspeed");
    _attrHandler.add(SUMO_ATTR_LENGTH, "length");
    _attrHandler.add(SUMO_ATTR_ACCEL, "accel");
    _attrHandler.add(SUMO_ATTR_DECEL, "decel");
    _attrHandler.add(SUMO_ATTR_SIGMA, "sigma");
    _attrHandler.add(SUMO_ATTR_TYPE, "type");
    _attrHandler.add(SUMO_ATTR_DEPART, "depart");
    _attrHandler.add(SUMO_ATTR_ROUTE, "route");*/
}


ROSumoRoutesHandler::~ROSumoRoutesHandler()
{
}

void ROSumoRoutesHandler::myStartElement(int element, const std::string &name,
                                         const Attributes &attrs)
{
    switch(element) {
    case SUMO_TAG_ROUTE:
        startRoute(attrs);
        break;
    case SUMO_TAG_VEHICLE:
        startVehicle(attrs);
        break;
    case SUMO_TAG_VTYPE:
        startVehType(attrs);
        break;
    default:
        break;
    }
}


void
ROSumoRoutesHandler::startRoute(const Attributes &attrs)
{
    try {
        _currentRoute = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        _currentRoute = "";
        SErrorHandler::add("Missing id in route.");
    }
}


void
ROSumoRoutesHandler::startVehicle(const Attributes &attrs)
{
    // get the vehicle id
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        _currentRoute = "";
        SErrorHandler::add("Missing id in vehicle.");
        return;
    }
    // get vehicle type
    ROVehicleType *type = 0;
    try {
        string name = getString(attrs, SUMO_ATTR_TYPE);
        type = _net.getVehicleType(name);
        if(type==0) {
            SErrorHandler::add(string("The type of the vehicle '") +
                name + string("' is not known."));
        }
    } catch (EmptyData) {
        _currentRoute = "";
        if(id.length()!=0) {
            SErrorHandler::add(string("Missing type in vehicle '") +
                id + string("'."));
        }
    }
    // get the departure time
    long time = -1;
    try {
        time = getLong(attrs, SUMO_ATTR_DEPART);
    } catch (EmptyData) {
        if(id.length()!=0) {
            SErrorHandler::add(string("Missing departure time in vehicle '") +
                id + string("'."));
        }
    } catch (NumberFormatException) {
        if(id.length()!=0) {
            SErrorHandler::add(string("Non-numerical departure time in vehicle '") +
                id + string("'."));
        }
    }
    // get the route id
    RORouteDef *route;
    try {
        string name = getString(attrs, SUMO_ATTR_ROUTE);
        route = _net.getRouteDef(name);
        if(route==0) {
            SErrorHandler::add(string("The route of the vehicle '") +
                name + string("' is not known."));
            return;
        }
    } catch (EmptyData) {
        if(id.length()!=0) {
            SErrorHandler::add(string("Missing route in vehicle '") +
                id + string("'."));
        }
    }
    // build the vehicle
    // get further optional information
    int repOffset = getIntSecure(attrs, SUMO_ATTR_PERIOD, -1);
    int repNumber = getIntSecure(attrs, SUMO_ATTR_REPNUMBER, -1);
    _net.addVehicle(id, new ROVehicle(id, route, time, type,
        repOffset, repNumber));
    _currentTimeStep = time;
}


void
ROSumoRoutesHandler::startVehType(const Attributes &attrs)
{
    // get the vehicle type id
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        _currentRoute = "";
        SErrorHandler::add("Missing id in vtype.");
        return;
    }
    // get the other values
    float maxspeed = getFloatReporting(attrs, SUMO_ATTR_MAXSPEED, id, "maxspeed");
    float length = getFloatReporting(attrs, SUMO_ATTR_LENGTH, id, "length");
    float accel = getFloatReporting(attrs, SUMO_ATTR_ACCEL, id, "accel");
    float decel = getFloatReporting(attrs, SUMO_ATTR_DECEL, id, "decel");
    float sigma = getFloatReporting(attrs, SUMO_ATTR_SIGMA, id, "sigma");
    // build the vehicle type after checking
    //  by now, only vehicles using the krauss model are supported
    if(maxspeed>0&&length>0&&accel>0&&decel>0&&sigma>0) {
        _net.addVehicleType(
            new ROVehicleType_Krauss(id, accel, decel, sigma, length, maxspeed));
    }
}


float
ROSumoRoutesHandler::getFloatReporting(const Attributes &attrs, AttrEnum attr,
                                       const std::string &id,
                                       const std::string &name)
{
    try {
        return getFloat(attrs, attr);
    } catch (EmptyData) {
        SErrorHandler::add(string("Missing ") + name + string(" in vehicle '") +
            id + string("'."));
    } catch (NumberFormatException) {
        SErrorHandler::add(name + string(" in vehicle '")
            + id + string("' is not numeric."));
    }
    return -1;
}



void ROSumoRoutesHandler::myCharacters(int element, const std::string &name,
                                       const std::string &chars)
{
    if(element==SUMO_TAG_ROUTE&&_currentRoute.length()!=0) {
        ROEdgeVector list;
        StringTokenizer st(chars);
        bool ok = st.size()>1;
        while(ok&&st.hasNext()) { // !!! too slow !!!
            string id = st.next();
            ROEdge *edge = _net.getEdge(id);
            if(edge!=0) {
                list.add(edge);
            } else {
                SErrorHandler::add(
                    string("The route '") + _currentRoute +
                    string("' contains the unknown edge '") + id +
                    string("'."));
                ok = false;
            }
        }
        if(ok) {
            ROCompleteRouteDef *route =
                new ROCompleteRouteDef(_currentRoute, list);
            _net.addRouteDef(route);
        } else {
            if(_currentRoute.length()>0) {
                SErrorHandler::add(
                    string("Something is wrong with route '") + _currentRoute
                    + string("'."));
            } else {
                SErrorHandler::add(
                    string("Invalid route occured."));
            }
        }
    }
}


void
ROSumoRoutesHandler::myEndElement(int element, const std::string &name)
{
    if(element==SUMO_TAG_ROUTE) {
        _currentRoute = "";
        _nextRouteRead = true;
    }
}

ROTypedRoutesLoader *
ROSumoRoutesHandler::getAssignedDuplicate(const std::string &file) const
{
    return new ROSumoRoutesHandler(_net, file);
}

std::string
ROSumoRoutesHandler::getDataName() const {
    return "precomputed sumo routes";
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "ROSUMORoutesHandler.icc"
//#endif

// Local Variables:
// mode:C++
// End:


