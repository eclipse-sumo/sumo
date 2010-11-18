/****************************************************************************/
/// @file    MSDevice_Vehroutes.cpp
/// @author  Daniel Krajzewicz
/// @date    Fri, 30.01.2009
/// @version $Id$
///
// A device which collects info on the vehicle trip
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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

#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSDevice_Vehroutes.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member variables
// ===========================================================================
bool MSDevice_Vehroutes::mySaveExits = false;
bool MSDevice_Vehroutes::myLastRouteOnly = false;
bool MSDevice_Vehroutes::myWithTaz = false;
MSDevice_Vehroutes::StateListener MSDevice_Vehroutes::myStateListener;


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice_Vehroutes::init() throw() {
    if (OptionsCont::getOptions().isSet("vehroute-output")) {
        OutputDevice::createDeviceByOption("vehroute-output", "routes");
        mySaveExits = OptionsCont::getOptions().getBool("vehroute-output.exit-times");
        myLastRouteOnly = OptionsCont::getOptions().getBool("vehroute-output.last-route");
        myWithTaz = OptionsCont::getOptions().getBool("device.routing.with-taz");
        MSNet::getInstance()->addVehicleStateListener(&myStateListener);
    }
}


MSDevice_Vehroutes*
MSDevice_Vehroutes::buildVehicleDevices(SUMOVehicle &v, std::vector<MSDevice*> &into, unsigned int maxRoutes) throw() {
    if (maxRoutes < INT_MAX) {
        return new MSDevice_Vehroutes(v, "vehroute_" + v.getID(), maxRoutes);
    }
    if (OptionsCont::getOptions().isSet("vehroute-output")) {
        if (myLastRouteOnly) {
            maxRoutes = 0;
        }
        myStateListener.myDevices[&v] = new MSDevice_Vehroutes(v, "vehroute_" + v.getID(), maxRoutes);
        into.push_back(myStateListener.myDevices[&v]);
        return myStateListener.myDevices[&v];
    }
    return 0;
}


// ---------------------------------------------------------------------------
// MSDevice_Vehroutes::StateListener-methods
// ---------------------------------------------------------------------------
void
MSDevice_Vehroutes::StateListener::vehicleStateChanged(const SUMOVehicle * const vehicle, MSNet::VehicleState to) throw() {
    if (to == MSNet::VEHICLE_STATE_NEWROUTE) {
        myDevices[vehicle]->addRoute();
    }
}


// ---------------------------------------------------------------------------
// MSDevice_Vehroutes-methods
// ---------------------------------------------------------------------------
MSDevice_Vehroutes::MSDevice_Vehroutes(SUMOVehicle &holder, const std::string &id, unsigned int maxRoutes) throw()
    : MSDevice(holder, id), myCurrentRoute(&holder.getRoute()), myMaxRoutes(maxRoutes) {
    myCurrentRoute->addReference();
}


MSDevice_Vehroutes::~MSDevice_Vehroutes() throw() {
    for (std::vector<RouteReplaceInfo>::iterator i=myReplacedRoutes.begin(); i!=myReplacedRoutes.end(); ++i) {
        (*i).route->release();
    }
    myCurrentRoute->release();
    myStateListener.myDevices.erase(&myHolder);
}


bool
MSDevice_Vehroutes::notifyEnter(SUMOVehicle& veh, bool isEmit, bool isLaneChange) throw() {
    if (mySaveExits && !isEmit && !isLaneChange) {
        myExits.push_back(MSNet::getInstance()->getCurrentTimeStep());
    }
    return mySaveExits;
}


void
MSDevice_Vehroutes::writeXMLRoute(OutputDevice &os, int index) const {
    // check if a previous route shall be written
    os.openTag("route");
    if (index>=0) {
        assert((int) myReplacedRoutes.size()>index);
        // write edge on which the vehicle was when the route was valid
        os << " replacedOnEdge=\"" << myReplacedRoutes[index].edge->getID();
        // write the time at which the route was replaced
        os << "\" replacedAtTime=\"" << time2string(myReplacedRoutes[index].time) << "\" probability=\"0\" edges=\"";
        // get the route
        for (int i=0; i<index; ++i) {
            myReplacedRoutes[i].route->writeEdgeIDs(os, myReplacedRoutes[i].edge);
        }
        myReplacedRoutes[index].route->writeEdgeIDs(os);
    } else {
        os << " edges=\"";
        if (myHolder.getNumberReroutes() > 0) {
            int noReroutes = myHolder.getNumberReroutes();
            assert((int) myReplacedRoutes.size()==noReroutes);
            for (int i=0; i<noReroutes; ++i) {
                myReplacedRoutes[i].route->writeEdgeIDs(os, myReplacedRoutes[i].edge);
            }
        }
        myCurrentRoute->writeEdgeIDs(os);
        if (mySaveExits) {
            os << "\" exitTimes=\"";
            for (std::vector<SUMOTime>::const_iterator it = myExits.begin(); it != myExits.end(); ++it) {
                if (it != myExits.begin()) {
                    os << " ";
                }
                os << time2string(*it);
            }
        }
    }
    (os << "\"").closeTag(true);
}


void
MSDevice_Vehroutes::generateOutput() const throw(IOError) {
    OutputDevice& od = OutputDevice::getDeviceByOption("vehroute-output");
    od.openTag("vehicle")
        << " id=\"" << myHolder.getID() << "\" depart=\""
        << time2string(myHolder.getDeparture())
        << "\" arrival=\"" << time2string(MSNet::getInstance()->getCurrentTimeStep());
    if (OptionsCont::getOptions().getBool("device.routing.with-taz")) {
        od << "\" fromtaz=\"" << myHolder.getParameter().fromTaz << "\" totaz=\"" << myHolder.getParameter().toTaz;
    }
    od << "\">\n";
    if (myReplacedRoutes.size() > 0) {
        od.openTag("routeDistribution") << ">\n";
        for (unsigned int i=0; i<myReplacedRoutes.size(); ++i) {
            writeXMLRoute(od, i);
        }
    }
    writeXMLRoute(od);
    if (myReplacedRoutes.size() > 0) {
        od.closeTag();
    }
    od.closeTag();
    od << "\n";
}


const MSRoute*
MSDevice_Vehroutes::getRoute(int index) const {
    return myReplacedRoutes[index].route;
}


void
MSDevice_Vehroutes::addRoute() {
    if (myMaxRoutes > 0) {
        myReplacedRoutes.push_back(RouteReplaceInfo(myHolder.getEdge(), MSNet::getInstance()->getCurrentTimeStep(), myCurrentRoute));
        myCurrentRoute = &myHolder.getRoute();
        myCurrentRoute->addReference();
        if (myReplacedRoutes.size() > myMaxRoutes) {
            myReplacedRoutes.front().route->release();
            myReplacedRoutes.erase(myReplacedRoutes.begin());
        }
    }
}


/****************************************************************************/

