/****************************************************************************/
/// @file    MSDevice_Tripinfo.cpp
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
#include "MSDevice_Tripinfo.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice_Tripinfo::buildVehicleDevices(SUMOVehicle &v, std::vector<MSDevice*> &into) throw() {
    if (OptionsCont::getOptions().isSet("tripinfo-output")) {
        MSDevice_Tripinfo* device = new MSDevice_Tripinfo(v, "tripinfo_" + v.getID());
        into.push_back(device);
    }
}


// ---------------------------------------------------------------------------
// MSDevice_Tripinfo-methods
// ---------------------------------------------------------------------------
MSDevice_Tripinfo::MSDevice_Tripinfo(SUMOVehicle &holder, const std::string &id) throw()
        : MSDevice(holder, id), myDepartLane(""), myDepartPos(-1), myDepartSpeed(-1),
          myWaitingSteps(0), myArrivalTime(-1), myArrivalLane(""), myArrivalPos(-1), myArrivalSpeed(-1) {
}


MSDevice_Tripinfo::~MSDevice_Tripinfo() throw() {
}


bool
MSDevice_Tripinfo::notifyMove(SUMOVehicle& veh, SUMOReal oldPos,
              SUMOReal newPos, SUMOReal newSpeed) throw() {
    if (newSpeed<=0.1) {
        myWaitingSteps++;
    }
    return true;
}


bool
MSDevice_Tripinfo::notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification reason) throw() {
    if (reason == MSMoveReminder::NOTIFICATION_DEPARTED) {
#ifdef HAVE_MESOSIM
    if (!MSGlobals::gUseMesoSim) {
#endif
        myDepartLane = static_cast<MSVehicle&>(veh).getLane().getID();
#ifdef HAVE_MESOSIM
    }
#endif
        myDepartPos = veh.getPositionOnLane();
        myDepartSpeed = veh.getSpeed();
    }
    return true;
}


bool
MSDevice_Tripinfo::notifyLeave(SUMOVehicle& veh, SUMOReal lastPos,
                               MSMoveReminder::Notification reason) throw() {
    if (reason >= MSMoveReminder::NOTIFICATION_ARRIVED) {
        myArrivalTime = MSNet::getInstance()->getCurrentTimeStep();
#ifdef HAVE_MESOSIM
    if (!MSGlobals::gUseMesoSim) {
#endif
        myArrivalLane = static_cast<MSVehicle&>(veh).getLane().getID();
#ifdef HAVE_MESOSIM
    }
#endif
        myArrivalPos = myHolder.getPositionOnLane();
        myArrivalSpeed = veh.getSpeed();
    }
    return true;
}


void
MSDevice_Tripinfo::generateOutput() const throw(IOError) {
    SUMOReal routeLength = myHolder.getRoute().getLength();
    // write
    OutputDevice &os = OutputDevice::getDeviceByOption("tripinfo-output");
    os.openTag("tripinfo") << " id=\"" << myHolder.getID() << "\" ";
    routeLength -= myDepartPos;
    os << "depart=\"" << time2string(myHolder.getDeparture()) << "\" "
       << "departLane=\"" << myDepartLane << "\" "
       << "departPos=\"" << myDepartPos << "\" "
       << "departSpeed=\"" << myDepartSpeed << "\" "
       << "departDelay=\"" << time2string(myHolder.getDeparture() - myHolder.getParameter().depart) << "\" ";
    if (myArrivalLane != "") {
        routeLength -= MSLane::dictionary(myArrivalLane)->getLength() - myArrivalPos;
    }
    os << "arrival=\"" << time2string(myArrivalTime) << "\" "
       << "arrivalLane=\"" << myArrivalLane << "\" "
       << "arrivalPos=\"" << myArrivalPos << "\" "
       << "arrivalSpeed=\"" << myArrivalSpeed << "\" "
       << "duration=\"" << time2string(myArrivalTime - myHolder.getDeparture()) << "\" "
       << "routeLength=\"" << routeLength << "\" "
       << "waitSteps=\"" << myWaitingSteps << "\" "
       << "rerouteNo=\"" << myHolder.getNumberReroutes();
    const std::vector<MSDevice*> &devices = myHolder.getDevices();
    std::ostringstream str;
    for (std::vector<MSDevice*>::const_iterator i=devices.begin(); i!=devices.end(); ++i) {
        if (i!=devices.begin()) {
            str << ' ';
        }
        str << (*i)->getID();
    }
    os << "\" devices=\"" << str.str()
       << "\" vtype=\"" << myHolder.getVehicleType().getID()
       << "\" vaporized=\"" << (myHolder.getEdge() == *(myHolder.getRoute().end() - 1) ? "" : "0")
       << "\"";
    if (devices.size() > 1) {
        os << ">\n";
    }
}



/****************************************************************************/

