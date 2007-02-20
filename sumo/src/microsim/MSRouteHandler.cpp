/****************************************************************************/
/// @file    MSRouteHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Parser and container for routes during their loading
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

#include <string>
#include <map>
#include <vector>
#include <microsim/MSRoute.h>
#include <microsim/MSEdge.h>
#include <microsim/MSVehicleType.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSEdge.h>
#include <microsim/MSEmitControl.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSLane.h>
#include "MSRouteHandler.h"
#include <utils/xml/XMLBuildingExceptions.h>
#include <utils/sumoxml/SUMOSAXHandler.h>
#include <utils/sumoxml/SUMOXMLDefinitions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/gfx/RGBColor.h>
#include <utils/gfx/GfxConvHelper.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>
#include "MSNet.h"

#include <microsim/trigger/MSTriggerControl.h>
#include <microsim/trigger/MSBusStop.h>
#include <microsim/MSGlobals.h>

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
MSRouteHandler::MSRouteHandler(const std::string &file,
                               MSVehicleControl &vc,
                               bool addVehiclesDirectly,
                               bool wantsVehicleColor,
                               int incDUABase,
                               int incDUAStage)
        : SUMOSAXHandler("sumo-network/routes", file),
        myVehicleControl(vc),
        myLastDepart(0), myLastReadVehicle(0),
        myAddVehiclesDirectly(addVehiclesDirectly),
        myWantVehicleColor(wantsVehicleColor),
        myCurrentEmbeddedRoute(0),
        myAmUsingIncrementalDUA(incDUAStage>0),
        myRunningVehicleNumber(0),
        myIncrementalBase(incDUABase),
        myIncrementalStage(incDUAStage)
{
    myActiveRoute.reserve(100);
}


MSRouteHandler::~MSRouteHandler()
{}


SUMOTime
MSRouteHandler::getLastDepart() const
{
    return myLastDepart;
}


MSVehicle *
MSRouteHandler::retrieveLastReadVehicle()
{
    MSVehicle *v = myLastReadVehicle;
    myLastReadVehicle = 0;
    return v;
}


void
MSRouteHandler::myStartElement(int element, const std::string &name,
                               const Attributes &attrs)
{
    switch (element) {
    case SUMO_TAG_VEHICLE:
        openVehicle(*this, attrs, myWantVehicleColor);
        break;
    case SUMO_TAG_VTYPE:
        addVehicleType(attrs);
        break;
    case SUMO_TAG_ROUTE:
        openRoute(attrs);
        break;
    default:
        break;
    }

    if (name=="stop") { // !!! make an int out of this
        MSVehicle::Stop stop;
        stop.lane = 0;
        stop.busstop = 0;
        // try to parse the assigne bus stop
        string bus_stop = getStringSecure(attrs, "bus_stop", "");
        if (bus_stop!="") {
            // ok, we have obviously a bus stop
            MSBusStop *bs =
                (MSBusStop*) MSNet::getInstance()->getTriggerControl().getTrigger(bus_stop);
            if (bs!=0) {
                const MSLane &l = bs->getLane();
                stop.lane = &((MSLane &) l);
                stop.busstop = bs;
                stop.pos = bs->getEndLanePosition();
            } else {
                MsgHandler::getErrorInstance()->inform("The bus stop '" + bus_stop + "' is not known.");
                return;
            }
        } else {
            // no, the lane and the position should be given
            // get the lane
            string laneS = getStringSecure(attrs, SUMO_ATTR_LANE, "");
            if (laneS!="") {
                MSLane *l = MSLane::dictionary(laneS);
                if (l==0) {
                    MsgHandler::getErrorInstance()->inform("The lane '" + laneS + "' for a stop is not known.");
                    return;
                }
                stop.lane = l;
            } else {
                MsgHandler::getErrorInstance()->inform("A stop must be placed on a bus stop or a lane.");
                return;
            }
            // get the position
            try {
                stop.pos = getFloat(attrs, SUMO_ATTR_POS);
            } catch (EmptyData&) {
                MsgHandler::getErrorInstance()->inform("The position of a stop is not defined.");
                return;
            } catch (NumberFormatException&) {
                MsgHandler::getErrorInstance()->inform("The position of a stop is not numeric.");
                return;
            }
        }

        // get the standing duration
        try {
            stop.duration = (SUMOTime) getFloat(attrs, "duration"); // time-parser
        } catch (EmptyData&) {
            MsgHandler::getErrorInstance()->inform("The duration of a stop is not defined.");
            return;
        } catch (NumberFormatException&) {
            MsgHandler::getErrorInstance()->inform("The duration of a stop is not numeric.");
            return;
        }
        stop.reached = false;
        myVehicleStops.push_back(stop);
    }
}


void
MSRouteHandler::addVehicleType(const Attributes &attrs)
{
    // !!! unsecure
    try {
        string id = getString(attrs, SUMO_ATTR_ID);
        try {
            addParsedVehicleType(id,
                                 getFloat(attrs, SUMO_ATTR_LENGTH),
                                 getFloat(attrs, SUMO_ATTR_MAXSPEED),
                                 getFloat(attrs, SUMO_ATTR_ACCEL),
                                 getFloat(attrs, SUMO_ATTR_DECEL),
                                 getFloat(attrs, SUMO_ATTR_SIGMA),
                                 parseVehicleClass(*this, attrs, "vehicle", id));
        } catch (XMLIdAlreadyUsedException &e) {
            MsgHandler::getErrorInstance()->inform(e.getMessage("vehicletype", id));
        } catch (EmptyData) {
            MsgHandler::getErrorInstance()->inform("Error in description: missing attribute in a vehicletype-object.");
        } catch (NumberFormatException) {
            MsgHandler::getErrorInstance()->inform("Error in description: one of an vehtype's attributes must be numeric but is not.");
        }
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Error in description: missing id of a vehicle-object.");
    }
}


void
MSRouteHandler::addParsedVehicleType(const string &id, const SUMOReal length,
                                     const SUMOReal maxspeed, const SUMOReal bmax,
                                     const SUMOReal dmax, const SUMOReal sigma,
                                     SUMOVehicleClass vclass)
{
    MSVehicleType *vtype = new MSVehicleType(id, length, maxspeed, bmax, dmax, sigma, vclass);
    if (!MSVehicleType::dictionary(id, vtype)) {
        delete vtype;
        if (!MSGlobals::gStateLoaded) {
            throw XMLIdAlreadyUsedException("VehicleType", id);
        }
    }
}


void
MSRouteHandler::openRoute(const Attributes &attrs)
{
    // get the id
    try {
        // check whether the id is really necessary
        if (myAmInEmbeddedMode) {
            // ok, a vehicle is wrapping the route,
            //  we may use this vehicle's id as default
            myActiveRouteID =
                getStringSecure(attrs, SUMO_ATTR_ID, "!" + myActiveVehicleID); // !!! document this
        } else {
            myActiveRouteID = getString(attrs, SUMO_ATTR_ID);
        }
    } catch (EmptyData) {
        MsgHandler::getErrorInstance()->inform("Error in description: missing id of a route-object.");
        return;
    }
    // get the information whether the route shall be deleted after
    // being passed
    bool multiReferenced = false;
    try {
        multiReferenced = getBool(attrs, SUMO_ATTR_MULTIR);
    } catch (...) {}
    m_IsMultiReferenced = multiReferenced;
}


// ----------------------------------


void
MSRouteHandler::myCharacters(int element, const std::string &name,
                             const std::string &chars)
{
    switch (element) {
    case SUMO_TAG_ROUTE:
        addRouteElements(name, chars);
        break;
    default:
        break;
    }
}


void
MSRouteHandler::addRouteElements(const std::string &name,
                                 const std::string &chars)
{
    StringTokenizer st(chars);
    if (st.size()==0) {
        MsgHandler::getErrorInstance()->inform("Empty route (" + name + ")");
        return;
    }
    MSEdge *edge = 0;
    while (st.hasNext()) {
        string set = st.next();
        edge = MSEdge::dictionary(set);
        // check whether the edge exists
        if (edge==0) {
            MsgHandler::getErrorInstance()->inform("The edge '" + set + "' within route '" + myActiveRouteID + "' is not known."
                                                       + "\n The route can not be build.");
            throw ProcessError();
        }
        myActiveRoute.push_back(edge);
    }
    // check whether the route is long enough
    if (myActiveRoute.size()<2) {
        MsgHandler::getErrorInstance()->inform("SUMO assumes each route to be at least two edges long ('" + name + "' has " + toString(myActiveRoute.size()) + ").");
        throw ProcessError();
    }
}


// ----------------------------------

void
MSRouteHandler::myEndElement(int element, const std::string &)
{
    switch (element) {
    case SUMO_TAG_ROUTE:
        try {
            closeRoute();
        } catch (XMLListEmptyException &e) {
            MsgHandler::getErrorInstance()->inform(e.getMessage("route", ""));
        } catch (XMLIdAlreadyUsedException &e) {
            MsgHandler::getErrorInstance()->inform(e.getMessage("route", ""));
        }
        break;
    case SUMO_TAG_VEHICLE:
        closeVehicle();
        break;
    }
}


void
MSRouteHandler::closeRoute()
{
    int size = myActiveRoute.size();
    if (size==0) {
        throw XMLListEmptyException();
    }
    MSRoute *route = new MSRoute(myActiveRouteID, myActiveRoute, m_IsMultiReferenced);
    myActiveRoute.clear();
    if (!MSRoute::dictionary(myActiveRouteID, route)) {
        delete route;
        if (!MSGlobals::gStateLoaded) {
            throw XMLIdAlreadyUsedException("route", myActiveRouteID);
        } else {
            route = MSRoute::dictionary(myActiveRouteID);
        }
    }
    if (myAmInEmbeddedMode) {
        myCurrentEmbeddedRoute = route;
    }
}


void
MSRouteHandler::closeVehicle()
{
    SUMOBaseRouteHandler::closeVehicle();
    // get the vehicle's type
    MSVehicleType *vtype = MSVehicleType::dictionary(myCurrentVType);
    if (vtype==0) {
        MsgHandler::getErrorInstance()->inform("The vehicle type '" + myCurrentVType + "' for vehicle '" + myActiveVehicleID + "' is not known.");
        throw ProcessError();
    }
    // get the vehicle's route
    //  maybe it was explicitely assigned to the vehicle
    MSRoute *route = myCurrentEmbeddedRoute;
    if (route==0) {
        // if not, try via the (hopefully) given route-id
        route = MSRoute::dictionary(myCurrentRouteName);
    }
    if (route==0) {
        // nothing found? -> error
        MsgHandler::getErrorInstance()->inform("The route '" + myCurrentRouteName + "' for vehicle '" + myActiveVehicleID + "' is not known.");
        throw ProcessError();
    }

    // check whether the first edge is long enough for the vehicle
    const MSEdge *firstEdge = (*route)[0];
    if ((*firstEdge->getLanes())[0]->length()<=vtype->getLength()) {
        // the vehicle is too long -> report an error
        MsgHandler::getErrorInstance()->inform("Vehicle '" + myActiveVehicleID + "' is too long to start at '" + firstEdge->getID() + "'.");
        throw ProcessError();
    }

    // try to build the vehicle
    MSVehicle *vehicle = 0;
    if (myVehicleControl.getVehicle(myActiveVehicleID)==0) {
        // ok there was no other vehicle with the same id, yet
        // maybe we do not want this vehicle to be emitted due to using incremental dua
        bool add = true;
        if (myAmUsingIncrementalDUA) {
            if ((int)(myRunningVehicleNumber%myIncrementalBase)>=(int) myIncrementalStage) {
                add = false;
            }
            myRunningVehicleNumber++;
        }
        if (add) {
            vehicle =
                MSNet::getInstance()->getVehicleControl().buildVehicle(myActiveVehicleID,
                        route, myCurrentDepart, vtype, myRepNumber, myRepOffset);
            // check whether the color information shall be set
            if (myWantVehicleColor&&myCurrentVehicleColor!=RGBColor(-1,-1,-1)) {
                vehicle->setCORNColor(myCurrentVehicleColor.red(), myCurrentVehicleColor.green(), myCurrentVehicleColor.blue());
            }
            // add the vehicle to the vehicle control
            myVehicleControl.addVehicle(myActiveVehicleID, vehicle);
        }
    } else {
        // strange: another vehicle with the same id already exists
        if (!MSGlobals::gStateLoaded) {
            // and was not loaded while loading a simulation state
            // -> error
            throw XMLIdAlreadyUsedException("vehicle", myActiveVehicleID);
        } else {
            // ok, it seems to be loaded previously while loading a simulation state
            vehicle = 0;
        }
    }
    // check whether the vehicle shall be added directly to the network or
    //  shall stay in the internal buffer
    if (myAddVehiclesDirectly&&vehicle!=0) {
        MSNet::getInstance()->myEmitter->add(vehicle);
    } else {
        myLastReadVehicle = vehicle;
    }
    if (vehicle!=0) {
        for (std::vector<MSVehicle::Stop>::iterator i=myVehicleStops.begin(); i!=myVehicleStops.end(); ++i) {
            vehicle->addStop(*i);
        }
    }
    myVehicleStops.clear();
    myLastDepart = myCurrentDepart;
    myCurrentEmbeddedRoute = 0;
}



/****************************************************************************/

