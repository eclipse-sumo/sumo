//---------------------------------------------------------------------------//
//                        ROSUMOAltRoutesHandler.cpp -
//  A SAX-handler for SUMO-route-alternatives
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
// Revision 1.3  2003/03/03 15:22:35  dkrajzew
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
#include <utils/common/UtilExceptions.h>
#include <utils/common/SErrorHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include "RORouteDef.h"
#include "ROVehicle.h"
#include "RORouteAlternativesDef.h"
//#include "RORouteAlternative.h"
#include "ROVehicleType.h"
#include "ROVehicleType_Krauss.h"
#include "ROSUMOAltRoutesHandler.h"
#include "ROEdgeVector.h"
#include "RORoute.h"
#include "RONet.h"

using namespace std;


ROSUMOAltRoutesHandler::ROSUMOAltRoutesHandler(RONet &net,
                                               double gawronBeta,
                                               double gawronA,
                                               const std::string &file)
    : ROTypedXMLRoutesLoader(net, file), _currentAlternatives(0),
    _gawronBeta(gawronBeta), _gawronA(gawronA)
{
}


ROSUMOAltRoutesHandler::~ROSUMOAltRoutesHandler()
{
}


void ROSUMOAltRoutesHandler::myStartElement(int element,
                                            const std::string &name,
                                            const Attributes &attrs)
{
    switch(element) {
    case SUMO_TAG_ROUTEALT:
        startAlternative(attrs);
        break;
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
ROSUMOAltRoutesHandler::startRoute(const Attributes &attrs)
{
    if(_currentAlternatives==0) {
        SErrorHandler::add(
            "Route declaration without an alternatives container occured.");
        return;
    }
    // try to get the costs
    _cost = getFloatSecure(attrs, SUMO_ATTR_COST, -1);
    if(_cost<0) {
        SErrorHandler::add(
            string("Invalid cost in alternative for route '")
            + _currentAlternatives->getID() + string("'."));
        return;
    }
    // try to get the propability
    _prob = getFloatSecure(attrs, SUMO_ATTR_PROP, -1);
    if(_prob<0) {
        SErrorHandler::add(
            string("Invalid propability in alternative for route '")
            + _currentAlternatives->getID() + string("'."));
        return;
    }
}


void
ROSUMOAltRoutesHandler::startVehicle(const Attributes &attrs)
{
    // get the vehicle id
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
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
    _net.addVehicle(id, new ROVehicle(id, route, time, type, -1));
    _currentTimeStep = time;
    _nextRouteRead = true;
}


void
ROSUMOAltRoutesHandler::startVehType(const Attributes &attrs)
{
    // get the vehicle type id
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        SErrorHandler::add("Missing id in vtype.");
        return;
    }
    // get the other values
    float maxspeed = getFloatReporting(attrs, SUMO_ATTR_MAXSPEED, id,
        "maxspeed");
    float length = getFloatReporting(attrs, SUMO_ATTR_LENGTH, id,
        "length");
    float accel = getFloatReporting(attrs, SUMO_ATTR_ACCEL, id,
        "accel");
    float decel = getFloatReporting(attrs, SUMO_ATTR_DECEL, id,
        "decel");
    float sigma = getFloatReporting(attrs, SUMO_ATTR_SIGMA, id,
        "sigma");
    // build the vehicle type after checking
    //  by now, only vehicles using the krauss model are supported
    if(maxspeed>0&&length>0&&accel>0&&decel>0&&sigma>0) {
        _net.addVehicleType(
            new ROVehicleType_Krauss(id, accel, decel,
            sigma, length, maxspeed));
    }
}


float
ROSUMOAltRoutesHandler::getFloatReporting(const Attributes &attrs,
                                          AttrEnum attr,
                                          const std::string &id,
                                          const std::string &name)
{
    try {
        return getFloat(attrs, attr);
    } catch (EmptyData) {
        SErrorHandler::add(string("Missing ") + name + string(" in vehicle '") +
            id + string("'."));
    } catch (NumberFormatException) {
        SErrorHandler::add(name + string(" in vehicle '") +
            id + string("' is not numeric."));
    }
    return -1;
}



void ROSUMOAltRoutesHandler::myCharacters(int element,
                                          const std::string &name,
                                          const std::string &chars)
{
    // process routes only, all other elements do
    //  not have embedded characters
    if(element!=SUMO_TAG_ROUTE) {
        return;
    }
    // check whether the costs and the propability are valid
    if(_cost<0||_prob<0) {
        return; // !!! should not happen (no costs and no propability)
    }
    // build the list of edges
    ROEdgeVector *list = new ROEdgeVector();
    bool ok = true;
    StringTokenizer st(chars);
    while(ok&&st.hasNext()) { // !!! too slow !!!
        string id = st.next();
        ROEdge *edge = _net.getEdge(id);
        if(edge!=0) {
            list->add(edge);
        } else {
            SErrorHandler::add(
                string("The route '") + _currentAlternatives->getID() +
                string("' contains the unknown edge '") + id +
                string("'."));
            ok = false;
        }
    }
    if(ok) {
        _currentAlternatives->addLoadedAlternative(
            new RORoute(_currentAlternatives->getID(), _cost, _prob, *list));
    }
    delete list;
}


void
ROSUMOAltRoutesHandler::myEndElement(int element, const std::string &name)
{
    if(element==SUMO_TAG_ROUTEALT) {
        endAlternative();
    }
}


ROTypedRoutesLoader *
ROSUMOAltRoutesHandler::getAssignedDuplicate(const std::string &file) const
{
    return new ROSUMOAltRoutesHandler(_net, _gawronBeta, _gawronA, file);
}


std::string
ROSUMOAltRoutesHandler::getDataName() const {
    return "precomputed sumo route alternatives";
}


void
ROSUMOAltRoutesHandler::startAlternative(const Attributes &attrs)
{
    // try to get the id
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch (EmptyData) {
        SErrorHandler::add("Missing route alternative name.");
        return;
    }
    // try to get the index of the last element
    int index = getIntSecure(attrs, SUMO_ATTR_LAST, -1);
    if(index<0) {
        SErrorHandler::add(
            string("Missing or non-numeric index of a route alternative (id='")
            + id + string("'."));
        return;
    }
    // try to get the start time
/*    int time = getLongSecure(attrs, SUMO_ATTR_DEPART, -1);
    if(time<0) {
        SErrorHandler::add(
            string("Missing or non-numeric departure time of a route alternative (id='")
            + id + string("'."));
        return;
    }*/
    // !!!
    // build the alternative cont
    _currentAlternatives = new RORouteAlternativesDef(id, index,
        _gawronBeta, _gawronA);
}


void
ROSUMOAltRoutesHandler::endAlternative()
{
    _net.addRouteDef(_currentAlternatives);
    _currentAlternatives = 0;
}





/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "ROSUMOAltRoutesHandler.icc"
//#endif

// Local Variables:
// mode:C++
// End:


