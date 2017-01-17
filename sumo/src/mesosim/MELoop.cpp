/****************************************************************************/
/// @file    MELoop.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, May 2005
/// @version $Id$
///
// The main mesocopic simulation loop
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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

#include <queue>
#include <vector>
#include <map>
#include <cmath>

#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/ToString.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/RandHelper.h>
#include "MELoop.h"
#include "MESegment.h"
#include "MEVehicle.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MELoop::MELoop(const SUMOTime recheckInterval) : myFullRecheckInterval(recheckInterval), myLinkRecheckInterval(TIME2STEPS(1)) {
}

MELoop::~MELoop() {
    for (std::vector<MESegment*>::const_iterator j = myEdges2FirstSegments.begin(); j != myEdges2FirstSegments.end(); ++j) {
        for (MESegment* s = *j; s != 0;) {
            MESegment* n = s->getNextSegment();
            delete s;
            s = n;
        }
    }
}


void
MELoop::simulate(SUMOTime tMax) {
    while (!myLeaderCars.empty()) {
        const SUMOTime time = myLeaderCars.begin()->first;
        assert(time > tMax - DELTA_T);
        if (time > tMax) {
            return;
        }
        std::vector<MEVehicle*> vehs = myLeaderCars[time];
        myLeaderCars.erase(time);
        for (std::vector<MEVehicle*>::const_iterator i = vehs.begin(); i != vehs.end(); ++i) {
            checkCar(*i);
            assert(myLeaderCars.empty() || myLeaderCars.begin()->first >= time);
        }
    }
}


bool
MELoop::changeSegment(MEVehicle* veh, SUMOTime leaveTime, MESegment* const toSegment, const bool ignoreLink) {
    MESegment* const onSegment = veh->getSegment();
    if (MESegment::isInvalid(toSegment)) {
        if (onSegment != 0) {
            onSegment->send(veh, toSegment, leaveTime);
        } else {
            WRITE_WARNING("Vehicle '" + veh->getID() + "' teleports beyond arrival edge '" + veh->getEdge()->getID() + "', time " + time2string(leaveTime) + ".");
        }
        veh->setSegment(toSegment); // signal arrival
        MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
        return true;
    }
    if (toSegment->hasSpaceFor(veh, leaveTime) && (ignoreLink || veh->mayProceed())) {
        if (onSegment != 0) {
            onSegment->send(veh, toSegment, leaveTime);
            toSegment->receive(veh, leaveTime, false, ignoreLink);
        } else {
            WRITE_WARNING("Vehicle '" + veh->getID() + "' ends teleporting on edge '" + toSegment->getEdge().getID()
                          + "':" + toString(toSegment->getIndex()) + ", time " + time2string(leaveTime) + ".");
            // this is not quite correct but suffices for interrogation by
            // subsequent methods (veh->getSpeed() needs segment != 0)
            veh->setSegment(myEdges2FirstSegments[veh->getEdge()->getNumericalID()]);
            toSegment->receive(veh, leaveTime, false, true);
        }
        return true;
    }
    return false;
}


void
MELoop::checkCar(MEVehicle* veh) {
    const SUMOTime leaveTime = veh->getEventTime();
    MESegment* const onSegment = veh->getSegment();
    MESegment* const toSegment = nextSegment(onSegment, veh);
    const bool teleporting = (onSegment == 0); // is the vehicle currently teleporting?
    if (changeSegment(veh, leaveTime, toSegment, teleporting)) {
        return;
    }
    if (MSGlobals::gTimeToGridlock > 0 && veh->getWaitingTime() > MSGlobals::gTimeToGridlock && !veh->isStopped()) {
        teleportVehicle(veh, toSegment);
        return;
    }
    if (veh->getBlockTime() == SUMOTime_MAX) {
        veh->setBlockTime(leaveTime);
    }
    if (leaveTime < toSegment->getEntryBlockTime()) {
        // receiving segment has recently received another vehicle
        veh->setEventTime(toSegment->getEntryBlockTime());
    } else if (toSegment->hasSpaceFor(veh, leaveTime) && !veh->mayProceed()) {
        // either the junction is blocked or the traffic light is red
        veh->setEventTime(leaveTime + MAX2(SUMOTime(1), myLinkRecheckInterval));
    } else {
        SUMOTime newEventTime = MAX3(toSegment->getEventTime() + 1, leaveTime + 1, leaveTime + myFullRecheckInterval);
        if (MSGlobals::gTimeToGridlock > 0) {
            // if teleporting is enabled, make sure we look at the vehicle when the the gridlock-time is up
            newEventTime = MIN2(newEventTime, veh->getBlockTime() + MSGlobals::gTimeToGridlock + 1);
        }
        veh->setEventTime(newEventTime);
    }
    addLeaderCar(veh, onSegment->getLink(veh));
}


void
MELoop::teleportVehicle(MEVehicle* veh, MESegment* const toSegment) {
    const SUMOTime leaveTime = veh->getEventTime();
    MESegment* const onSegment = veh->getSegment();
    const bool teleporting = (onSegment == 0); // is the vehicle already teleporting?
    // try to find a place on the current edge
    MESegment* teleSegment = toSegment->getNextSegment();
    while (teleSegment != 0 && !teleSegment->hasSpaceFor(veh, leaveTime)) {
        // @caution the time to get to the next segment here is ignored XXX
        teleSegment = teleSegment->getNextSegment();
    }
    if (teleSegment != 0) {
        if (!teleporting) {
            // we managed to teleport in a single jump
            WRITE_WARNING("Teleporting vehicle '" + veh->getID() + "'; waited too long, from edge '" + onSegment->getEdge().getID()
                          + "':" + toString(onSegment->getIndex())
                          + " to edge '" + teleSegment->getEdge().getID()
                          + "':" + toString(teleSegment->getIndex())
                          + ", time " + time2string(leaveTime) + ".");
            MSNet::getInstance()->getVehicleControl().registerTeleportJam();
        }
        changeSegment(veh, leaveTime, teleSegment, true);
        teleSegment->setEntryBlockTime(leaveTime); // teleports should not block normal flow
    } else {
        // teleport across the current edge and try insertion later
        if (!teleporting) {
            // announce start of multi-step teleport, arrival will be announced in changeSegment()
            WRITE_WARNING("Teleporting vehicle '" + veh->getID() + "'; waited too long, from edge '" + onSegment->getEdge().getID()
                          + "':" + toString(onSegment->getIndex()) + ", time " + time2string(leaveTime) + ".");
            MSNet::getInstance()->getVehicleControl().registerTeleportJam();
            // remove from current segment
            onSegment->send(veh, 0, leaveTime);
            // mark veh as teleporting
            veh->setSegment(0, 0);
        }
        // @caution microsim uses current travel time teleport duration
        const SUMOTime teleArrival = leaveTime + TIME2STEPS(veh->getEdge()->getLength() / veh->getEdge()->getSpeedLimit());
        const bool atDest = veh->moveRoutePointer();
        if (atDest) {
            // teleporting to end of route
            changeSegment(veh, teleArrival, 0, true);
        } else {
            veh->setEventTime(teleArrival);
            addLeaderCar(veh, 0);
            // teleporting vehicles must react to rerouters
            getSegmentForEdge(*veh->getEdge())->addReminders(veh);
            veh->activateReminders(MSMoveReminder::NOTIFICATION_JUNCTION);
        }
    }
}


void
MELoop::addLeaderCar(MEVehicle* veh, MSLink* link) {
    myLeaderCars[veh->getEventTime()].push_back(veh);
    setApproaching(veh, link);
}


void
MELoop::setApproaching(MEVehicle* veh, MSLink* link) {
    if (link != 0) {
        link->setApproaching(veh, veh->getEventTime() + (link->getState() == LINKSTATE_ALLWAY_STOP ?
                             (SUMOTime)RandHelper::rand((int)2) : 0), // tie braker
                             veh->getSpeed(), veh->getSpeed(), true,
                             veh->getEventTime(), veh->getSpeed(), veh->getWaitingTime(),
                             // @note: dist is not used by meso (getZipperSpeed is never called)
                             veh->getSegment()->getLength());
    }
}


void
MELoop::removeLeaderCar(MEVehicle* v) {
    std::vector<MEVehicle*>& cands = myLeaderCars[v->getEventTime()];
    cands.erase(find(cands.begin(), cands.end(), v));
}


MESegment*
MELoop::nextSegment(MESegment* s, MEVehicle* v) {
    if (s != 0) { // vehicle is not teleporting
        MESegment* next = s->getNextSegment();
        if (next != 0) {
            // ok, the street continues
            return next;
        }
    }
    // we have to check the next edge in the vehicle's route
    const MSEdge* nextEdge = v->succEdge(1);
    if (nextEdge == 0) {
        // end of route
        return 0;
    }
    return myEdges2FirstSegments[nextEdge->getNumericalID()];
}


int
MELoop::numSegmentsFor(const SUMOReal length, const SUMOReal sLength) {
    int no = (int)floor(length / sLength + 0.5);
    if (no == 0) { // assure there is at least one segment
        return 1;
    } else {
        return no;
    }
}


void
MELoop::buildSegmentsFor(const MSEdge& e, const OptionsCont& oc) {
    const SUMOReal length = e.getLength();
    int no = numSegmentsFor(length, oc.getFloat("meso-edgelength"));
    const SUMOReal slength = length / (SUMOReal)no;
    MESegment* newSegment = 0;
    MESegment* nextSegment = 0;
    bool multiQueue = oc.getBool("meso-multi-queue");
    bool junctionControl = oc.getBool("meso-junction-control");
    for (int s = no - 1; s >= 0; s--) {
        std::string id = e.getID() + ":" + toString(s);
        newSegment =
            new MESegment(id, e, nextSegment, slength,
                          e.getLanes()[0]->getSpeedLimit(), s,
                          string2time(oc.getString("meso-tauff")), string2time(oc.getString("meso-taufj")),
                          string2time(oc.getString("meso-taujf")), string2time(oc.getString("meso-taujj")),
                          oc.getFloat("meso-jam-threshold"), multiQueue, junctionControl);
        multiQueue = false;
        junctionControl = false;
        nextSegment = newSegment;
    }
    while (e.getNumericalID() >= static_cast<int>(myEdges2FirstSegments.size())) {
        myEdges2FirstSegments.push_back(0);
    }
    myEdges2FirstSegments[e.getNumericalID()] = newSegment;
}


MESegment*
MELoop::getSegmentForEdge(const MSEdge& e, SUMOReal pos) {
    MESegment* s = myEdges2FirstSegments[e.getNumericalID()];
    if (pos > 0) {
        SUMOReal cpos = 0;
        while (s->getNextSegment() != 0 && cpos + s->getLength() < pos) {
            cpos += s->getLength();
            s = s->getNextSegment();
        }
    }
    return s;
}


/****************************************************************************/
