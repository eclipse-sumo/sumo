//---------------------------------------------------------------------------//
//                        ROTripHandler.cpp -
//  The basic class for route definition loading
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
// Revision 1.2  2003/03/03 15:22:36  dkrajzew
// debugging
//
// Revision 1.1  2003/02/07 10:45:06  dkrajzew
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
#include <utils/xml/AttributesHandler.h>
#include <utils/xml/GenericSAX2Handler.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SErrorHandler.h>
#include "RORouteDef.h"
#include "RONet.h"
#include "ROOrigDestRouteDef.h"
#include "ROTripHandler.h"
#include "RORouteDefList.h"
#include "ROVehicle.h"
#include "RORunningVehicle.h"
#include "ROTypedRoutesLoader.h"

using namespace std;

ROTripHandler::ROTripHandler(RONet &net,
                                     const std::string &fileName)
    : ROTypedXMLRoutesLoader(net, fileName)
{
/*    _attrHandler.add(SUMO_ATTR_id, "id");
    _attrHandler.add(SUMO_ATTR_time, "time");
    _attrHandler.add(SUMO_ATTR_from, "from");
    _attrHandler.add(SUMO_ATTR_to, "to");
    _attrHandler.add(SUMO_ATTR_type, "type");
    _attrHandler.add(SUMO_ATTR_speed, "speed");
    _attrHandler.add(SUMO_ATTR_pos, "pos");
    _attrHandler.add(SUMO_ATTR_lane, "lane");*/
}


ROTripHandler::~ROTripHandler()
{
}

void ROTripHandler::myStartElement(int element, const std::string &name,
                                      const Attributes &attrs)
{
    if(element==SUMO_TAG_TRIPDEF) {
        // get the vehicle id, the edges, the speed and position and
        //  the departure time
        string id = getVehicleID(attrs);
        ROEdge *from = getEdge(attrs, "origin", SUMO_ATTR_FROM, id);
        ROEdge *to = getEdge(attrs, "destination", SUMO_ATTR_TO, id);
        string typeID = getVehicleType(attrs);
        float pos = getOptionalFloat(attrs, "pos", SUMO_ATTR_POS, id);
        float speed = getOptionalFloat(attrs, "speed", SUMO_ATTR_SPEED, id);
        long time = getDepartureTime(attrs, id);
        long period = getPeriod(attrs, id);
        string lane = getLane(attrs);
        // recheck attributes
        if(from==0||to==0||time<0) {
/*            if(from==0) {
                SErrorHandler::add("The from edge is not known.");
            }
            if(to==0) {
                SErrorHandler::add("The to edge is not known.");
            }*/
            if(time<0) {
                SErrorHandler::add("The departure time must be positive.");
            }
            return;
        }
        // add the vehicle type, the vehicle and the route to the net
        RORouteDef *route = new ROOrigDestRouteDef(id, from, to);
        ROVehicleType *type = _net.getVehicleTypeSecure(typeID);
        if(pos>0||speed>0) {
            _net.addVehicle(id, new RORunningVehicle(id, route, time, type, lane, pos, speed, period));
        } else {
            _net.addVehicle(id, new ROVehicle(id, route, time, type, period));
        }
        _net.addRouteDef(route);
        _nextRouteRead = true;
        _currentTimeStep = time;
    }
}


std::string
ROTripHandler::getVehicleID(const Attributes &attrs)
{
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch(EmptyData) {
    }
    // get a valid vehicle id
    while(id.length()==0) {
        string tmp = _idSupplier.getNext();
        if(!_net.isKnownVehicleID(tmp))
            id = tmp;
    }
        // and save this vehicle id
    _net.addVehicleID(id);
    return id;
}


ROEdge *
ROTripHandler::getEdge(const Attributes &attrs, const std::string &purpose,
                           AttrEnum which, const string &id)
{
    try {
        string id = getString(attrs, which);
        return _net.getEdge(id);
    } catch(EmptyData) {
        SErrorHandler::add(string("Missing ") +
            purpose + string(" edge in description of a route."));
        if(id.length()!=0) {
            SErrorHandler::add(string(" Vehicle id='") + id + string("'."));
        }
    }
    return 0;
}

std::string
ROTripHandler::getVehicleType(const Attributes &attrs)
{
    try {
        return getString(attrs, SUMO_ATTR_TYPE);
    } catch(EmptyData) {
        return "SUMO_DEFAULTTYPE";
    }
}

float
ROTripHandler::getOptionalFloat(const Attributes &attrs,
                                    const std::string &name,
                                    AttrEnum which,
                                    const std::string &place)
{
    try {
        return getFloat(attrs, SUMO_ATTR_POS);
    } catch (EmptyData) {
    } catch (NumberFormatException) {
        SErrorHandler::add(string("The value of '") + name +
            string("' should be numeric but is not."));
        if(place.length()!=0)
            SErrorHandler::add(string(" Route id='") + place + string("'"));
    }
    return -1;
}


long
ROTripHandler::getDepartureTime(const Attributes &attrs,
                                    const std::string &id)
{
    // get the departure time
    try {
        return getLong(attrs, SUMO_ATTR_DEPART);
    } catch(EmptyData) {
        SErrorHandler::add("Missing departure time in description of a route.");
        if(id.length()!=0)
            SErrorHandler::add(string(" Vehicle id='") + id + string("'."));
    } catch (NumberFormatException) {
        SErrorHandler::add("The value of the departure time should be numeric but is not.");
        if(id.length()!=0)
            SErrorHandler::add(string(" Route id='") + id + string("'"));
    }
    return -1;
}


long
ROTripHandler::getPeriod(const Attributes &attrs,
                         const std::string &id)
{
    // get the departure time
    try {
        return getLong(attrs, SUMO_ATTR_PERIOD);
    } catch(EmptyData) {
        return -1;
    } catch (NumberFormatException) {
        SErrorHandler::add("The value of the period should be numeric but is not.");
        if(id.length()!=0)
            SErrorHandler::add(string(" Route id='") + id + string("'"));
    }
    return -1;
}


string
ROTripHandler::getLane(const Attributes &attrs)
{
    try {
        return getString(attrs, SUMO_ATTR_LANE);
    } catch (EmptyData) {
        return "";
    }
}


void
ROTripHandler::myCharacters(int element, const std::string &name,
                               const std::string &chars)
{
}


void
ROTripHandler::myEndElement(int element, const std::string &name)
{
}

ROTypedRoutesLoader *
ROTripHandler::getAssignedDuplicate(const std::string &file) const
{
    return new ROTripHandler(_net, file);
}

std::string
ROTripHandler::getDataName() const {
    return "XML-route definitions";
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "ROTripHandler.icc"
//#endif

// Local Variables:
// mode:C++
// End:


