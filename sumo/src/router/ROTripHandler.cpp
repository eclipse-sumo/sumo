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
// Revision 1.5  2003/06/18 11:20:54  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.4  2003/04/09 15:39:11  dkrajzew
// router debugging & extension: no routing over sources, random routes added
//
// Revision 1.3  2003/03/20 16:39:17  dkrajzew
// periodical car emission implemented; windows eol removed
//
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
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>
#include "RORouteDef.h"
#include "RONet.h"
#include "ROOrigDestRouteDef.h"
#include "ROTripHandler.h"
#include "RORouteDefList.h"
#include "ROVehicle.h"
#include "RORunningVehicle.h"
#include "ROCompleteRouteDef.h"
#include "ROTypedRoutesLoader.h"

using namespace std;

ROTripHandler::ROTripHandler(RONet &net,
                                     const std::string &fileName)
    : ROTypedXMLRoutesLoader(net, fileName)
{
}


ROTripHandler::~ROTripHandler()
{
}


void ROTripHandler::myStartElement(int element, const std::string &name,
                                      const Attributes &attrs)
{
    if(element==SUMO_TAG_TRIPDEF) {
        // get the vehicle id, the edges, the speed and position and
        //  the departure time and other information
        myID = getVehicleID(attrs);
        myBeginEdge = getEdge(attrs, "origin", SUMO_ATTR_FROM, myID);
        myEndEdge = getEdge(attrs, "destination", SUMO_ATTR_TO, myID);
        myType = getVehicleType(attrs);
        myPos = getOptionalFloat(attrs, "pos", SUMO_ATTR_POS, myID);
        mySpeed = getOptionalFloat(attrs, "speed", SUMO_ATTR_SPEED, myID);
        myDepartureTime = getDepartureTime(attrs, myID);
        myPeriodTime = getPeriod(attrs, myID);
        myNumberOfRepetitions = getRepetitionNumber(attrs, myID);
        myLane = getLane(attrs);
        // recheck attributes
        if(myBeginEdge==0||myEndEdge==0||myDepartureTime<0) {
            if(myDepartureTime<0) {
                MsgHandler::getErrorInstance()->inform("The departure time must be positive.");
            }
            return;
        }
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
                           AttrEnum which, const string &vid)
{
    ROEdge *e = 0;
    string id;
    try {
        id = getString(attrs, which);
        e = _net.getEdge(id);
        if(e!=0) {
            return e;
        }
    } catch(EmptyData) {
        MsgHandler::getErrorInstance()->inform(string("Missing ") +
            purpose + string(" edge in description of a route."));
    }
    if(e==0) {
        MsgHandler::getErrorInstance()->inform(string("The edge '") +
            id + string("' is not known."));
    }
    if(vid.length()!=0) {
        MsgHandler::getErrorInstance()->inform(string(" Vehicle id='") + vid + string("'."));
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
        MsgHandler::getErrorInstance()->inform(string("The value of '") + name +
            string("' should be numeric but is not."));
        if(place.length()!=0)
            MsgHandler::getErrorInstance()->inform(string(" Route id='") + place + string("'"));
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
        MsgHandler::getErrorInstance()->inform("Missing departure time in description of a route.");
        if(id.length()!=0)
            MsgHandler::getErrorInstance()->inform(string(" Vehicle id='") + id + string("'."));
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform("The value of the departure time should be numeric but is not.");
        if(id.length()!=0)
            MsgHandler::getErrorInstance()->inform(string(" Route id='") + id + string("'"));
    }
    return -1;
}


int
ROTripHandler::getPeriod(const Attributes &attrs,
                         const std::string &id)
{
    // get the repetition period
    try {
        return getInt(attrs, SUMO_ATTR_PERIOD);
    } catch(EmptyData) {
        return -1;
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform("The value of the period should be numeric but is not.");
        if(id.length()!=0)
            MsgHandler::getErrorInstance()->inform(string(" Route id='") + id + string("'"));
    }
    return -1;
}


int
ROTripHandler::getRepetitionNumber(const Attributes &attrs,
                                   const std::string &id)
{
    // get the repetition period
    try {
        return getInt(attrs, SUMO_ATTR_REPNUMBER);
    } catch(EmptyData) {
        return -1;
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform("The number of cars that shall be emitted with the same parameter must be numeric.");
        if(id.length()!=0)
            MsgHandler::getErrorInstance()->inform(string(" Route id='") + id + string("'"));
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
    if(element==SUMO_TAG_TRIPDEF) {
        StringTokenizer st(chars);
        myEdges.clear();
        while(st.hasNext()) {
            string id = st.next();
            ROEdge *edge = _net.getEdge(id);
            if(edge==0) {
                MsgHandler::getErrorInstance()->inform(
                    string("Could not find edge '") + id
                    + string("' wihtin route '") + myID + string("'."));
                return;
            }
            myEdges.add(edge);
        }
    }
}


void
ROTripHandler::myEndElement(int element, const std::string &name)
{
    if(element==SUMO_TAG_TRIPDEF&&!MsgHandler::getErrorInstance()->wasInformed()) {
        // add the vehicle type, the vehicle and the route to the net
        RORouteDef *route = 0;
        if(myEdges.size()==0) {
            route = new ROOrigDestRouteDef(myID, myBeginEdge, myEndEdge);
        } else {
            route = new ROCompleteRouteDef(myID, myEdges);
        }
        ROVehicleType *type = _net.getVehicleTypeSecure(myType);
        if(myPos>=0||mySpeed>=0) {
            _net.addVehicle(myID,
                new RORunningVehicle(myID, route, myDepartureTime,
                    type, myLane, myPos, mySpeed, myPeriodTime,
                    myNumberOfRepetitions));
        } else {
            _net.addVehicle(myID,
                new ROVehicle(myID, route, myDepartureTime,
                    type, myPeriodTime, myNumberOfRepetitions));
        }
        _net.addRouteDef(route);
        _nextRouteRead = true;
        _currentTimeStep = myDepartureTime;
    }
}





ROTypedRoutesLoader *
ROTripHandler::getAssignedDuplicate(const std::string &file) const
{
    return new ROTripHandler(_net, file);
}


std::string
ROTripHandler::getDataName() const
{
    return "XML-route definitions";
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "ROTripHandler.icc"
//#endif

// Local Variables:
// mode:C++
// End:


