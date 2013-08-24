/****************************************************************************/
/// @file    MSDevice_BTreceiver.cpp
/// @author  Daniel Krajzewicz
/// @date    14.08.2013
/// @version $Id$
///
// A BT sender
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include <utils/common/TplConvert.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/SUMOVehicle.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSVehicle.h>
#include "MSDevice_Tripinfo.h"
#include "MSDevice_BTreceiver.h"

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
MSDevice_BTreceiver::insertOptions(OptionsCont& oc) {
    oc.addOptionSubTopic("Communication");

    insertDefaultAssignmentOptions("btreceiver", "Communication", oc);

    oc.doRegister("device.btreceiver.range", new Option_String("0", "TIME"));
    oc.addDescription("device.btreceiver.range", "Communication", "The period with which the vehicle shall be rerouted");
}


void
MSDevice_BTreceiver::buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into) {
    OptionsCont& oc = OptionsCont::getOptions();
    if (equippedByDefaultAssignmentOptions(oc, "btreceiver", v)) {
        MSDevice_BTreceiver* device = new MSDevice_BTreceiver(v, "btreceiver_" + v.getID(), oc.getFloat("device.btreceiver.range"));
        into.push_back(device);
    }
}


// ---------------------------------------------------------------------------
// MSDevice_BTreceiver-methods
// ---------------------------------------------------------------------------
MSDevice_BTreceiver::MSDevice_BTreceiver(SUMOVehicle& holder, const std::string& id,  SUMOReal range)
    : MSDevice(holder, id), myRange(range) {
}


MSDevice_BTreceiver::~MSDevice_BTreceiver() {
}


bool
MSDevice_BTreceiver::notifyMove(SUMOVehicle& veh, SUMOReal oldPos,
                                SUMOReal newPos, SUMOReal newSpeed) {
    Position p = static_cast<MSVehicle&>(veh).getPosition();
    // collect edges around
    std::set<std::string> tmp;
    Named::StoringVisitor sv(tmp);
    std::set<std::string> inRange;
    //!!!myObjects[CMD_GET_LANE_VARIABLE]->Search(cmin, cmax, sv);

    // check vehicles in range first;
    //  determine when they've entered range
    for (std::set<std::string>::const_iterator i = tmp.begin(); i != tmp.end(); ++i) {
        MSLane* l = MSLane::dictionary(*i);
        if (l == 0) {
            continue;
        }

        const MSLane::VehCont& vehs = l->getVehiclesSecure();
        for (MSLane::VehCont::const_iterator j = vehs.begin(); j != vehs.end(); ++j) {
            if (static_cast<MSVehicle&>(myHolder).getPosition().distanceTo((*j)->getPosition()) > myRange) {
                continue;
            }
            // save, we have to investigate vehicles we do not see anymore
            inRange.insert((*j)->getID());
            // add new vehicles to myCurrentlySeen
            if (myCurrentlySeen.find((*j)->getID()) == myCurrentlySeen.end()) {
                MeetingPoint mp(MSNet::getInstance()->getCurrentTimeStep(),
                                static_cast<MSVehicle&>(myHolder).getPosition(), myHolder.getSpeed(), (*j)->getPosition(), (*j)->getSpeed());
                SeenDevice* sd = new SeenDevice(mp);
                myCurrentlySeen[(*j)->getID()] = sd;
            }
        }
        l->releaseVehicles();
    }
    // check vehicles that are not longer in range
    //  set their range exit information
    for (std::map<std::string, SeenDevice*>::const_iterator i = myCurrentlySeen.begin(); i != myCurrentlySeen.end(); ++i) {
        if (inRange.find((*i).first) != inRange.end()) {
            continue;
        }
        MSVehicle* v = static_cast<MSVehicle*>(MSNet::getInstance()->getVehicleControl().getVehicle((*i).first));
        MeetingPoint mp(MSNet::getInstance()->getCurrentTimeStep(),
                        static_cast<MSVehicle&>(myHolder).getPosition(), myHolder.getSpeed(), v->getPosition(), v->getSpeed());
        myCurrentlySeen[(*i).first]->meetingEnd = mp;
        if (mySeen.find((*i).first) == mySeen.end()) {
            mySeen[(*i).first] = std::vector<SeenDevice*>();
        }
        mySeen[(*i).first].push_back(myCurrentlySeen[(*i).first]);
        myCurrentlySeen.erase(myCurrentlySeen.find((*i).first));
    }
    return true; // keep the device
}


void
MSDevice_BTreceiver::generateOutput() const {
}



/****************************************************************************/

