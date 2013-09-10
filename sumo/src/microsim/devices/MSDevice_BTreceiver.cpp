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

#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/SUMOVehicle.h>
#include <utils/geom/Position.h>
#include <utils/geom/Line.h>
#include <utils/geom/GeomHelper.h>
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
// static member definitions
// ===========================================================================
NamedRTree MSDevice_BTreceiver::myLanesRTree;
bool MSDevice_BTreceiver::myWasInitialised = false;


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice_BTreceiver::insertOptions(OptionsCont& oc) {
    insertDefaultAssignmentOptions("btreceiver", "Communication", oc);

    oc.doRegister("device.btreceiver.range", new Option_Float(300));
    oc.addDescription("device.btreceiver.range", "Communication", "The range of the bt receiver");
    oc.doRegister("device.btreceiver.output", new Option_FileName());
    oc.addDescription("device.btreceiver.output", "Communication", "The file to write seen vehicles into");
}


void
MSDevice_BTreceiver::buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into) {
    OptionsCont& oc = OptionsCont::getOptions();
    if (equippedByDefaultAssignmentOptions(oc, "btreceiver", v)) {
        MSDevice_BTreceiver* device = new MSDevice_BTreceiver(v, "btreceiver_" + v.getID(), oc.getFloat("device.btreceiver.range"));
        into.push_back(device);
        if(!myWasInitialised) {
            MSLane::fill(myLanesRTree);
            myWasInitialised = true;
        }
    }
}


// ---------------------------------------------------------------------------
// MSDevice_BTreceiver-methods
// ---------------------------------------------------------------------------
MSDevice_BTreceiver::MSDevice_BTreceiver(SUMOVehicle& holder, const std::string& id,  SUMOReal range)
    : MSDevice(holder, id), myRange(range) {
    MSNet::getInstance()->addVehicleStateListener(this);
}


MSDevice_BTreceiver::~MSDevice_BTreceiver() {
    MSNet::getInstance()->removeVehicleStateListener(this);
}


// from http://blog.csharphelper.com/2010/03/28/determine-where-a-line-intersects-a-circle-in-c.aspx
int FindLineCircleIntersections(const Position &c, SUMOReal radius, const Position &p1, const Position &p2,
    Position &intersection1, Position &intersection2, SUMOReal &t1, SUMOReal &t2)
{
    //float dx, dy, A, B, C, det, t;

    SUMOReal dx = p2.x() - p1.x();
    SUMOReal dy = p2.y() - p1.y();

    SUMOReal A = dx * dx + dy * dy;
    SUMOReal B = 2 * (dx * (p1.x() - c.x()) + dy * (p1.y() - c.y()));
    SUMOReal C = (p1.x() - c.x()) * (p1.x() - c.x()) + (p1.y() - c.y()) * (p1.y() - c.y()) - radius * radius;

    SUMOReal det = B * B - 4 * A * C;
    if ((A <= 0.0000001) || (det < 0)) {
        // No real solutions.
        return 0;
    } else if (det == 0) {
        // One solution.
        t1 = -B / (2 * A);
        intersection1.set(p1.x() + t1 * dx, p1.y() + t1 * dy);
        return 1;
    } else {
        // Two solutions.
        t1 = (float)((-B + sqrt(det)) / (2 * A));
        intersection1.set(p1.x() + t1 * dx, p1.y() + t1 * dy);
        t2 = (float)((-B - sqrt(det)) / (2 * A));
        intersection2.set(p1.x() + t2 * dx, p1.y() + t2 * dy);
        return 2;
    }
}


void 
MSDevice_BTreceiver::enterRange(const MSVehicle &other, const Position &atPos, SUMOReal atOffset) {
    // add new vehicles to myCurrentlySeen
    if (myCurrentlySeen.find(other.getID()) == myCurrentlySeen.end()) {
        MeetingPoint mp(STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep()-DELTA_T)+atOffset,
            static_cast<MSVehicle&>(myHolder).getPosition(), myHolder.getSpeed(), atPos, other.getSpeed());
        SeenDevice* sd = new SeenDevice(mp);
        myCurrentlySeen[other.getID()] = sd;
    }
}


void 
MSDevice_BTreceiver::leaveRange(const MSVehicle &other, const Position &atPos, SUMOReal atOffset) {
    std::string id = other.getID();
    MeetingPoint mp(STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep()-DELTA_T)+atOffset,
        static_cast<MSVehicle&>(myHolder).getPosition(), myHolder.getSpeed(), atPos, other.getSpeed());
    myCurrentlySeen[id]->meetingEnd = mp;
    if (mySeen.find(id) == mySeen.end()) {
        mySeen[id] = std::vector<SeenDevice*>();
    }
    mySeen[id].push_back(myCurrentlySeen[id]);
    myCurrentlySeen.erase(myCurrentlySeen.find(id));
}


bool
MSDevice_BTreceiver::notifyMove(SUMOVehicle& veh, SUMOReal /* oldPos */,
                                SUMOReal /* newPos */, SUMOReal /* newSpeed */) {
    if(!myHolder.isOnRoad()) {    
        return true;
    }
    // collect edges around
    std::set<std::string> tmp;
    Named::StoringVisitor sv(tmp);
    Position egoPosition = static_cast<MSVehicle&>(veh).getPosition();
    SUMOReal angle = static_cast<MSVehicle&>(veh).getAngle() * M_PI / 180.;
    SUMOReal speed = static_cast<MSVehicle&>(veh).getSpeed();
    SUMOReal dist = SPEED2DIST(speed);
    Position egoP2 = egoPosition;
    Position egoP1(egoP2.x()-sin(angle)*dist, egoP2.y()-cos(angle)*dist);
    Position egoD = egoP2 - egoP1;
    Boundary b;
    b.add(egoPosition);
    b.grow(myRange + 500 / 3.6); // two vehicles passing at 250*2 km/h
    const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
    const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
    myLanesRTree.Search(cmin, cmax, sv);

    // check vehicles in range first;
    //  determine when they've entered range
    std::set<std::string> seen;
    for (std::set<std::string>::const_iterator i = tmp.begin(); i != tmp.end(); ++i) {
        MSLane* l = MSLane::dictionary(*i);
        if (l == 0) {
            continue;
        }

        const MSLane::VehCont& vehs = l->getVehiclesSecure();
        for (MSLane::VehCont::const_iterator j = vehs.begin(); j != vehs.end(); ++j) {
            if(static_cast<MSVehicle*>(*j)->getDevice(typeid(MSDevice_BTreceiver))==0) {
                // skipping not equipped vehicles
                continue;
            }
            
            // get the encountered vehicle's current and prior position (based on speed and direction)
            Position intersection1, intersection2;
            SUMOReal t1, t2;
            SUMOReal angle = (*j)->getAngle() * M_PI / 180.;
            SUMOReal speed = (*j)->getSpeed();
            SUMOReal dist = SPEED2DIST(speed);
            Position otherP2 = (*j)->getPosition();
            Position otherP1(otherP2.x()-sin(angle)*dist, otherP2.y()-cos(angle)*dist);
            Position otherD = otherP2 - otherP1;
            // let the other's current position be the one obtained by applying the relative direction vector to the initial position
            otherP2 = otherP1 + egoD - otherD;
            // find crossing points
            int count = FindLineCircleIntersections(egoPosition, myRange, otherP1, otherP2, intersection1, intersection2, t1, t2);
            intersection1 = otherP1 + otherD * t1;
            intersection2 = otherP1 + otherD * t2;
            if(count==0) {
                // no state change (is either still outside or still within the range)
                continue;
            }
            seen.insert((*j)->getID());
            if(myCurrentlySeen.find((*j)->getID())!=myCurrentlySeen.end()) {
                // vehicle was within range
                if(count==2) {
                    // should not happen
                    WRITE_WARNING("Vehicle '" + (*j)->getID() + "' was in range of '" + myHolder.getID() + "' but crosses the circle twice.");
                } else {
                    // vehicle leaves range
                    leaveRange(**j, intersection1, t1);
                }
            } else {
                // vehicle was outside range
                if(count==1) {
                    // entered only
                    enterRange(**j, intersection1, t1);
                } else {
                    enterRange(**j, intersection1, t1);
                    leaveRange(**j, intersection2, t2);
                }
            }
        }
        l->releaseVehicles();
    }
    // check vehicles that are not longer in range
    //  set their range exit information
    for (std::map<std::string, SeenDevice*>::const_iterator i = myCurrentlySeen.begin(); i != myCurrentlySeen.end(); ++i) {
        if (seen.find((*i).first) != seen.end()) {
            // skip those which are still visible
            continue;
        }
        WRITE_WARNING("Vehicle '" + (*i).first + "' was in range of '" + myHolder.getID() + "' and is not longer visible.");
        MSVehicle *v = static_cast<MSVehicle*>(MSNet::getInstance()->getVehicleControl().getVehicle((*i).first));
        leaveRange(*v, v->getPosition(), 1);
    }
    return true; // keep the device
}


void 
MSDevice_BTreceiver::vehicleStateChanged(const SUMOVehicle* const vehicle, MSNet::VehicleState to) {
    if(to!=MSNet::VEHICLE_STATE_STARTING_TELEPORT && to!=MSNet::VEHICLE_STATE_ARRIVED) {
        // want to know about vehicles leaving the network, only
        return;
    }
    std::string id = vehicle->getID();
    if(myCurrentlySeen.find(id)==myCurrentlySeen.end()) {
        // not seen before
        return;
    }
    const MSVehicle* v = static_cast<const MSVehicle*>(vehicle);
    leaveRange(*v, v->getPosition(), 1);
}


void
MSDevice_BTreceiver::generateOutput() const {
    if(!OptionsCont::getOptions().isSet("device.btreceiver.output")) {
        return;
    }
    OutputDevice& os = OutputDevice::getDeviceByOption("device.btreceiver.output");
    os.openTag("bt").writeAttr("id", myHolder.getID());
    for(std::map<std::string, std::vector<SeenDevice*> >::const_iterator i=mySeen.begin(); i!=mySeen.end(); ++i) {
        const std::vector<SeenDevice*> &sts = (*i).second;
        for(std::vector<SeenDevice*>::const_iterator j=sts.begin(); j!=sts.end(); ++j) {
            os.openTag("seen").writeAttr("id", (*i).first);
            os.writeAttr("tBeg", (*j)->meetingBegin.t)
                .writeAttr("observerPosBeg", (*j)->meetingBegin.observerPos).writeAttr("observerSpeedBeg", (*j)->meetingBegin.observerSpeed)
                .writeAttr("seenPosBeg", (*j)->meetingBegin.seenPos).writeAttr("seenSpeedBeg", (*j)->meetingBegin.seenSpeed);
            os.writeAttr("tEnd", (*j)->meetingEnd.t)
                .writeAttr("observerPosEnd", (*j)->meetingEnd.observerPos).writeAttr("observerSpeedEnd", (*j)->meetingEnd.observerSpeed)
                .writeAttr("seenPosEnd", (*j)->meetingEnd.seenPos).writeAttr("seenSpeedEnd", (*j)->meetingEnd.seenSpeed);
            os.closeTag();
        }
    }
    os.closeTag();
}



/****************************************************************************/

