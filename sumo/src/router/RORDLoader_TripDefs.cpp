//---------------------------------------------------------------------------//
//                        RORDLoader_TripDefs.cpp -
//      The basic class for loading trip definitions
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
// Revision 1.10  2006/01/24 13:43:53  dkrajzew
// added vehicle classes to the routing modules
//
// Revision 1.9  2006/01/09 12:00:59  dkrajzew
// debugging vehicle color usage
//
// Revision 1.8  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.7  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.6  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/05/04 08:51:41  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.4  2004/11/23 10:25:52  dkrajzew
// debugging
//
// Revision 1.3  2004/07/02 09:39:41  dkrajzew
// debugging while working on INVENT; preparation of classes to be derived
//  for an online-routing
//
// Revision 1.2  2004/02/16 13:47:07  dkrajzew
// Type-dependent loader/generator-"API" changed
//
// Revision 1.1  2004/01/26 08:02:27  dkrajzew
// loaders and route-def types are now renamed in an senseful way;
//  further changes in order to make both new routers work;
//  documentation added
//
// ------------------------------------------------
// Revision 1.10  2003/12/12 12:33:01  dkrajzew
// error output corrected
//
// Revision 1.9  2003/11/20 13:11:17  dkrajzew
// some work on the code style
//
// Revision 1.8  2003/07/30 09:26:33  dkrajzew
// all vehicles, routes and vehicle types may now have specific colors
//
// Revision 1.7  2003/07/22 15:14:13  dkrajzew
// debugging (false vehicle length)
//
// Revision 1.6  2003/07/16 15:36:50  dkrajzew
// vehicles and routes may now have colors
//
// Revision 1.5  2003/06/18 11:20:54  dkrajzew
// new message and error processing: output to user may be a message, warning
//  or an error now; it is reported to a Singleton (MsgHandler);
// this handler puts it further to output instances.
// changes: no verbose-parameter needed; messages are exported to singleton
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

#include <string>
#include <utils/xml/AttributesHandler.h>
#include <utils/xml/GenericSAX2Handler.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>
#include <utils/gfx/RGBColor.h>
#include <utils/gfx/GfxConvHelper.h>
#include "RORouteDef.h"
#include "RONet.h"
#include "RORouteDef_OrigDest.h"
#include "RORDLoader_TripDefs.h"
#include "RORouteDefList.h"
#include "ROVehicle.h"
#include "RORunningVehicle.h"
#include "RORouteDef_Complete.h"
#include "ROAbstractRouteDefLoader.h"
#include "ROVehicleBuilder.h"
#include "ROVehicleType_Krauss.h"

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
RORDLoader_TripDefs::RORDLoader_TripDefs(ROVehicleBuilder &vb, RONet &net,
                                         SUMOTime begin, SUMOTime end,
                                         bool emptyDestinationsAllowed,
                                         const std::string &fileName)
    : ROTypedXMLRoutesLoader(vb, net, begin, end, fileName),
    myEmptyDestinationsAllowed(emptyDestinationsAllowed),
    myDepartureTime(0)
{
}


RORDLoader_TripDefs::~RORDLoader_TripDefs()
{
}


void
RORDLoader_TripDefs::myStartElement(int element, const std::string &name,
                                    const Attributes &attrs)
{
    // check whether a trip definition shall be parsed
    if(element==SUMO_TAG_TRIPDEF) {
        // get the vehicle id, the edges, the speed and position and
        //  the departure time and other information
        myID = getVehicleID(attrs);
        myDepartureTime = getTime(attrs, SUMO_ATTR_DEPART, myID);
        myBeginEdge = getEdge(attrs, "origin",
            SUMO_ATTR_FROM, myID, false);
        myEndEdge = getEdge(attrs, "destination",
            SUMO_ATTR_TO, myID, myEmptyDestinationsAllowed);
        myType = getVehicleType(attrs);
        myPos = getOptionalFloat(attrs, "pos", SUMO_ATTR_POS, myID);
        mySpeed = getOptionalFloat(attrs, "speed", SUMO_ATTR_SPEED, myID);
        myPeriodTime = getPeriod(attrs, myID);
        myNumberOfRepetitions = getRepetitionNumber(attrs, myID);
        myLane = getLane(attrs);
        myColor = getRGBColorReporting(attrs, myID);
        // recheck attributes
        if(myDepartureTime<0) {
            MsgHandler::getErrorInstance()->inform("The departure time must be positive.");
            return;
        }
    }
    // check whether a vehicle type shall be parsed
    if(element==SUMO_TAG_VTYPE) {
        // get and check the vtype-id
        string id = getStringSecure(attrs, SUMO_ATTR_ID, "");
        if(id=="") {
            MsgHandler::getErrorInstance()->inform("A vehicle type with an unknown id occured.");
            return;
        }
        // get the rest of the parameter
        try {
            SUMOReal a = getFloat(attrs, SUMO_ATTR_ACCEL);
            SUMOReal b = getFloat(attrs, SUMO_ATTR_DECEL);
            SUMOReal vmax = getFloat(attrs, SUMO_ATTR_MAXSPEED);
            SUMOReal length = getFloat(attrs, SUMO_ATTR_LENGTH);
            SUMOReal eps = getFloat(attrs, SUMO_ATTR_SIGMA);

			RGBColor col(-1,-1,-1);
            string colordef = getStringSecure(attrs, SUMO_ATTR_COLOR, "");
			if(colordef!="") {
				try {
					col = GfxConvHelper::parseColor(colordef);
				} catch (NumberFormatException &) {
					MsgHandler::getErrorInstance()->inform("The color information for vehicle type '" + id + "' is not numeric.");
				} catch (...) {
					MsgHandler::getErrorInstance()->inform("The color information for vehicle type '" + id + "' is malicious.");
				}
			}

			SUMOVehicleClass vclass = SVC_UNKNOWN;
			string classdef = getStringSecure(attrs, SUMO_ATTR_VCLASS, "");
			if(classdef!="") {
				try {
					vclass = getVehicleClassID(classdef);
				} catch (...) {
					MsgHandler::getErrorInstance()->inform("The vehicle class for vehicle type '" + id + "' is malicious.");
				}
			}
            ROVehicleType *vt = new ROVehicleType_Krauss(id, col, length, vclass, a, b, eps, vmax);
            _net.addVehicleType(vt);
        } catch (NumberFormatException&) {
            MsgHandler::getErrorInstance()->inform("One of the parameter for vehicle type '" + id + "' is not numeric.");
            return;
        } catch (EmptyData&) {
            MsgHandler::getErrorInstance()->inform("One of the parameter for vehicle type '" + id + "' is not given.");
            return;
        }
    }
}


std::string
RORDLoader_TripDefs::getVehicleID(const Attributes &attrs)
{
    string id;
    try {
        id = getString(attrs, SUMO_ATTR_ID);
    } catch(EmptyData) {
    }
    // get a valid vehicle id
    while(id.length()==0) {
        string tmp = _idSupplier.getNext();
        if(!_net.isKnownVehicleID(tmp)) {
            id = tmp;
        }
    }
        // and save this vehicle id
    _net.addVehicleID(id); // !!! what for?
    return id;
}


ROEdge *
RORDLoader_TripDefs::getEdge(const Attributes &attrs,
                             const std::string &purpose,
                             AttrEnum which, const string &vid,
                             bool emptyAllowed)
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
        if(!emptyAllowed) {
            MsgHandler::getErrorInstance()->inform(string("Missing ") +
                purpose + string(" edge in description of a route."));
        }
    }
    if(e==0) {
        if(!emptyAllowed) {
            MsgHandler::getErrorInstance()->inform(string("The edge '") +
                id + string("' is not known."));
        }
    }
    if(vid.length()!=0&&!emptyAllowed) {
        MsgHandler::getErrorInstance()->inform(string(" Vehicle id='") + vid + string("'."));
    }
    return 0;
}


std::string
RORDLoader_TripDefs::getVehicleType(const Attributes &attrs)
{
    try {
        return getString(attrs, SUMO_ATTR_TYPE);
    } catch(EmptyData) {
        return "!";// !!! make this is static const
    }
}


SUMOReal
RORDLoader_TripDefs::getOptionalFloat(const Attributes &attrs,
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
            MsgHandler::getErrorInstance()->inform(string(" Route id='") +
            place + string("')"));
    }
    return -1;
}


SUMOTime
RORDLoader_TripDefs::getTime(const Attributes &attrs, AttrEnum which,
                             const std::string &id)
{
    // get the departure time
    try {
        return getLong(attrs, which);
    } catch(EmptyData) {
        MsgHandler::getErrorInstance()->inform("Missing time in description of a route.");
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
RORDLoader_TripDefs::getPeriod(const Attributes &attrs,
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
RORDLoader_TripDefs::getRepetitionNumber(const Attributes &attrs,
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
RORDLoader_TripDefs::getLane(const Attributes &attrs)
{
    try {
        return getString(attrs, SUMO_ATTR_LANE);
    } catch (EmptyData) {
        return "";
    }
}


void
RORDLoader_TripDefs::myCharacters(int element, const std::string &name,
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
RORDLoader_TripDefs::myEndElement(int element, const std::string &name)
{
    if(element==SUMO_TAG_TRIPDEF &&
       !MsgHandler::getErrorInstance()->wasInformed()) {

        if(myDepartureTime<myBegin||myDepartureTime>=myEnd) {
            return;
        }
        RORouteDef *route = 0;
        if(myEdges.size()==0) {
            route = new RORouteDef_OrigDest(myID, myColor,
                myBeginEdge, myEndEdge);
        } else {
            route = new RORouteDef_Complete(myID, myColor,
                myEdges);
        }
        ROVehicleType *type = _net.getVehicleTypeSecure(myType);
        // check whether any errors occured
        if(MsgHandler::getErrorInstance()->wasInformed()) {
            return;
        }
        _net.addRouteDef(route);
        _nextRouteRead = true;
        // build the vehicle
        if(myPos>=0||mySpeed>=0) {
            _net.addVehicle(myID,
                myVehicleBuilder.buildRunningVehicle(
                myID, route, myDepartureTime,
                type, myLane, (SUMOReal) myPos, (SUMOReal) mySpeed, myColor, myPeriodTime,
                    myNumberOfRepetitions));
        } else {
            _net.addVehicle(myID,
                myVehicleBuilder.buildVehicle(
                myID, route, myDepartureTime,
                type, myColor, myPeriodTime, myNumberOfRepetitions));
        }
    }
}


std::string
RORDLoader_TripDefs::getDataName() const
{
    return "XML-trip definitions";
}


RGBColor
RORDLoader_TripDefs::getRGBColorReporting(const Attributes &attrs,
                                          const std::string &id)
{
    try {
        return GfxConvHelper::parseColor(getString(attrs, SUMO_ATTR_COLOR));
    } catch (EmptyData) {
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform(string("Color in vehicle '")
            + id + string("' is not numeric."));
    }
    return RGBColor(-1, -1, -1);
}



bool
RORDLoader_TripDefs::nextRouteRead()
{
    return _nextRouteRead;
}


void
RORDLoader_TripDefs::beginNextRoute()
{
    _nextRouteRead = false;
}


SUMOTime
RORDLoader_TripDefs::getCurrentTimeStep() const
{
    return myDepartureTime;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


