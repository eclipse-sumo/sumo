/****************************************************************************/
/// @file    MESegment.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, May 2005
/// @version $Id$
///
// A single mesoscopic segment (cell)
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

#include <algorithm>
#include <limits>
#include <utils/common/StdDefs.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSEdge.h>
#include <microsim/MSJunction.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSLinkCont.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSMoveReminder.h>
#include <microsim/output/MSXMLRawOut.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/devices/MSDevice.h>
#include <utils/common/FileHelpers.h>
#include <utils/iodevices/BinaryInputDevice.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/RandHelper.h>
#include "MEVehicle.h"
#include "MELoop.h"
#include "MESegment.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

#define DEFAULT_VEH_LENGHT_WITH_GAP (SUMOVTypeParameter::getDefault().length + SUMOVTypeParameter::getDefault().minGap)
// avoid division by zero when driving very slowly
#define MESO_MIN_SPEED ((SUMOReal)0.05)

// ===========================================================================
// static member defintion
// ===========================================================================
MSEdge MESegment::myDummyParent("MESegmentDummyParent", -1, MSEdge::EDGEFUNCTION_UNKNOWN, "", "", -1);
MESegment MESegment::myVaporizationTarget("vaporizationTarget");
const SUMOReal MESegment::DO_NOT_PATCH_JAM_THRESHOLD(std::numeric_limits<SUMOReal>::max());

// ===========================================================================
// method definitions
// ===========================================================================
MESegment::MESegment(const std::string& id,
                     const MSEdge& parent, MESegment* next,
                     SUMOReal length, SUMOReal speed,
                     int idx,
                     SUMOTime tauff, SUMOTime taufj,
                     SUMOTime taujf, SUMOTime taujj,
                     SUMOReal jamThresh, bool multiQueue, bool junctionControl) :
    Named(id), myEdge(parent), myNextSegment(next),
    myLength(length), myIndex(idx),
    myTau_ff((SUMOTime)(tauff / parent.getLanes().size())),
    myTau_fj((SUMOTime)(taufj / parent.getLanes().size())), // Eissfeldt p. 90 and 151 ff.
    myTau_jf((SUMOTime)(taujf / parent.getLanes().size())),
    myTau_jj((SUMOTime)(taujj / parent.getLanes().size())),
    myTau_length(MAX2(MESO_MIN_SPEED, speed) * parent.getLanes().size() / TIME2STEPS(1)),
    myHeadwayCapacity(length / DEFAULT_VEH_LENGHT_WITH_GAP * parent.getLanes().size())/* Eissfeldt p. 69 */,
    myCapacity(length * parent.getLanes().size()),
    myOccupancy(0.f),
    myJunctionControl(junctionControl),
    myTLSPenalty(MSGlobals::gMesoTLSPenalty > 0 &&
                 // only apply to the last segment of a tls-controlled edge
                 myNextSegment == 0 && (
                     parent.getToJunction()->getType() == NODETYPE_TRAFFIC_LIGHT ||
                     parent.getToJunction()->getType() == NODETYPE_TRAFFIC_LIGHT_NOJUNCTION ||
                     parent.getToJunction()->getType() == NODETYPE_TRAFFIC_LIGHT_RIGHT_ON_RED)),
    myMinorPenalty(MSGlobals::gMesoMinorPenalty > 0 &&
                   // only apply to the last segment of an uncontrolled edge that has at least 1 minor link
                   myNextSegment == 0 &&
                   parent.getToJunction()->getType() != NODETYPE_TRAFFIC_LIGHT &&
                   parent.getToJunction()->getType() != NODETYPE_TRAFFIC_LIGHT_NOJUNCTION &&
                   parent.getToJunction()->getType() != NODETYPE_TRAFFIC_LIGHT_RIGHT_ON_RED &&
                   parent.hasMinorLink()),
    myEntryBlockTime(SUMOTime_MIN),
    myLastHeadway(TIME2STEPS(-1)),
    myMeanSpeed(speed),
    myLastMeanSpeedUpdate(SUMOTime_MIN) {
    myCarQues.push_back(std::vector<MEVehicle*>());
    myBlockTimes.push_back(-1);
    if (useMultiQueue(multiQueue, parent)) {
        const std::vector<MSLane*>& lanes = parent.getLanes();
        while (myCarQues.size() < lanes.size()) {
            myCarQues.push_back(std::vector<MEVehicle*>());
            myBlockTimes.push_back(-1);
        }
        for (int i = 0; i < (int)parent.getNumSuccessors(); ++i) {
            const MSEdge* const edge = parent.getSuccessors()[i];
            const std::vector<MSLane*>* const allowed = parent.allowedLanes(*edge);
            assert(allowed != 0);
            assert(allowed->size() > 0);
            for (std::vector<MSLane*>::const_iterator j = allowed->begin(); j != allowed->end(); ++j) {
                std::vector<MSLane*>::const_iterator it = find(lanes.begin(), lanes.end(), *j);
                myFollowerMap[edge].push_back((int)distance(lanes.begin(), it));
            }
        }
    }
    recomputeJamThreshold(jamThresh);
}


MESegment::MESegment(const std::string& id):
    Named(id),
    myEdge(myDummyParent), // arbitrary edge needed to supply the needed reference
    myNextSegment(0), myLength(0), myIndex(0),
    myTau_ff(0), myTau_fj(0), myTau_jf(0), myTau_jj(0), myTau_length(1),
    myHeadwayCapacity(0), myCapacity(0), myJunctionControl(false),
    myTLSPenalty(false),
    myMinorPenalty(false) {
}


bool
MESegment::useMultiQueue(bool multiQueue, const MSEdge& parent) {
    return multiQueue && parent.getLanes().size() > 1 && parent.getNumSuccessors() > 1;
}

void
MESegment::recomputeJamThreshold(SUMOReal jamThresh) {
    if (jamThresh == DO_NOT_PATCH_JAM_THRESHOLD) {
        return;
    }
    if (jamThresh < 0) {
        // compute based on speed
        SUMOReal speed = myEdge.getSpeedLimit();
        if (myTLSPenalty || myMinorPenalty) {
            SUMOReal travelTime = myLength / MAX2(speed, NUMERICAL_EPS) + getMaxPenaltySeconds();
            speed = myLength / travelTime;
        }
        myJamThreshold = jamThresholdForSpeed(speed, jamThresh);
    } else {
        // compute based on specified percentage
        myJamThreshold = jamThresh * myCapacity;
    }

    // update coefficients for the jam-jam headway function
    // this function models the effect that "empty space" needs to move
    // backwards through the downstream segment before the upstream segment may
    // send annother vehicle.
    // this allows jams to clear and move upstream.
    // the headway function f(x) depends on the number of vehicles in the
    // downstream segment x
    // f is a linear function that passes through the following fixed points:
    // f(n_jam_threshold) = tau_jf_withLength (for continuity)
    // f(myHeadwayCapacity) = myTau_jj * myHeadwayCapacity

    const SUMOTime tau_jf_withLength = tauWithVehLength(myTau_jf, DEFAULT_VEH_LENGHT_WITH_GAP);
    if (myJamThreshold < myCapacity) {
        // jamming is possible
        const SUMOReal n_jam_threshold = myHeadwayCapacity * myJamThreshold / myCapacity; // number of vehicles above which the segment is jammed
        // solving f(x) = a * x + b
        myA = (STEPS2TIME(myTau_jj) * myHeadwayCapacity - STEPS2TIME(tau_jf_withLength)) / (myHeadwayCapacity - n_jam_threshold);
        myB = myHeadwayCapacity * (STEPS2TIME(myTau_jj) - myA);

        // note that the original Eissfeldt model (p. 69) used different fixed points
        // f(n_jam_threshold) = n_jam_threshold * myTau_jj
        // f(myHeadwayCapacity) = myTau_jf * myHeadwayCapacity
        //
        // However, this systematically underestimates the backpropagation speed of the jam front (see #2244)
    } else {
        // dummy values. Should not be used
        myA = 0;
        myB = STEPS2TIME(tau_jf_withLength);
    }
}


SUMOReal
MESegment::jamThresholdForSpeed(SUMOReal speed, SUMOReal jamThresh) const {
    // vehicles driving freely at maximum speed should not jam
    // we compute how many vehicles could possible enter the segment until the first vehicle leaves
    // and multiply by the space these vehicles would occupy
    // the jamThresh parameter is scale the resulting value
    if (speed == 0) {
        return std::numeric_limits<double>::max();  // never jam. Irrelevant at speed 0 anyway
    }
    return std::ceil(myLength / (-jamThresh * speed * STEPS2TIME(tauWithVehLength(myTau_ff, DEFAULT_VEH_LENGHT_WITH_GAP)))) * DEFAULT_VEH_LENGHT_WITH_GAP;
}


void
MESegment::addDetector(MSMoveReminder* data) {
    myDetectorData.push_back(data);
    for (Queues::const_iterator k = myCarQues.begin(); k != myCarQues.end(); ++k) {
        for (std::vector<MEVehicle*>::const_reverse_iterator i = k->rbegin(); i != k->rend(); ++i) {
            (*i)->addReminder(data);
        }
    }
}


void
MESegment::removeDetector(MSMoveReminder* data) {
    std::vector<MSMoveReminder*>::iterator it = find(
                myDetectorData.begin(), myDetectorData.end(), data);
    if (it != myDetectorData.end()) {
        myDetectorData.erase(it);
    }
    for (Queues::const_iterator k = myCarQues.begin(); k != myCarQues.end(); ++k) {
        for (std::vector<MEVehicle*>::const_reverse_iterator i = k->rbegin(); i != k->rend(); ++i) {
            (*i)->removeReminder(data);
        }
    }
}


void
MESegment::updateDetectorsOnLeave(MEVehicle* v, SUMOTime currentTime, MESegment* next) {
    MSMoveReminder::Notification reason;
    if (next == 0) {
        reason = MSMoveReminder::NOTIFICATION_ARRIVED;
    } else if (next == &myVaporizationTarget) {
        reason = MSMoveReminder::NOTIFICATION_VAPORIZED;
    } else if (myNextSegment == 0) {
        reason = MSMoveReminder::NOTIFICATION_JUNCTION;
    } else {
        reason = MSMoveReminder::NOTIFICATION_SEGMENT;
    }
    v->updateDetectors(currentTime, true, reason);
}


void
MESegment::prepareDetectorForWriting(MSMoveReminder& data) {
    const SUMOTime currentTime = MSNet::getInstance()->getCurrentTimeStep();
    for (Queues::const_iterator k = myCarQues.begin(); k != myCarQues.end(); ++k) {
        SUMOTime earliestExitTime = currentTime;
        for (std::vector<MEVehicle*>::const_reverse_iterator i = k->rbegin(); i != k->rend(); ++i) {
            const SUMOTime exitTime = MAX2(earliestExitTime, (*i)->getEventTime());
            (*i)->updateDetectorForWriting(&data, currentTime, exitTime);
            earliestExitTime = exitTime + tauWithVehLength(myTau_ff, (*i)->getVehicleType().getLengthWithGap());
        }
    }
}


bool
MESegment::hasSpaceFor(const MEVehicle* veh, SUMOTime entryTime, bool init) const {
    if (myOccupancy == 0.) {
        // we have always space for at least one vehicle
        return true;
    }
    const SUMOReal newOccupancy = myOccupancy + veh->getVehicleType().getLengthWithGap();
    if (newOccupancy > myCapacity) {
        // we must ensure that occupancy remains below capacity
        return false;
    }
    // regular insertions and initial insertions must respect different constraints:
    // - regular insertions must respect entryBlockTime
    // - initial insertions should not cause additional jamming
    // - inserted vehicle should be able to continue at the current speed
    if (init) {
        if (free() && !hasBlockedLeader()) {
            return newOccupancy <= myJamThreshold;
        } else {
            return newOccupancy <= jamThresholdForSpeed(getMeanSpeed(false), -1);
        }
    }
    // maintain propper spacing between inflow from different lanes
    return entryTime >= myEntryBlockTime;
}


bool
MESegment::initialise(MEVehicle* veh, SUMOTime time) {
    if (hasSpaceFor(veh, time, true)) {
        receive(veh, time, true);
        // we can check only after insertion because insertion may change the route via devices
        std::string msg;
        if (MSGlobals::gCheckRoutes && !veh->hasValidRoute(msg)) {
            throw ProcessError("Vehicle '" + veh->getID() + "' has no valid route. " + msg);
        }
        return true;
    }
    return false;
}


int
MESegment::getCarNumber() const {
    int total = 0;
    for (Queues::const_iterator k = myCarQues.begin(); k != myCarQues.end(); ++k) {
        total += (int)k->size();
    }
    return total;
}


SUMOReal
MESegment::getMeanSpeed(bool useCached) const {
    const SUMOTime currentTime = MSNet::getInstance()->getCurrentTimeStep();
    if (currentTime != myLastMeanSpeedUpdate || !useCached) {
        myLastMeanSpeedUpdate = currentTime;
        const SUMOTime tau = free() ? myTau_ff : myTau_jf;
        SUMOReal v = 0;
        int count = 0;
        for (Queues::const_iterator k = myCarQues.begin(); k != myCarQues.end(); ++k) {
            SUMOTime earliestExitTime = currentTime;
            count += (int)k->size();
            for (std::vector<MEVehicle*>::const_reverse_iterator veh = k->rbegin(); veh != k->rend(); ++veh) {
                v += (*veh)->getConservativeSpeed(earliestExitTime); // earliestExitTime is updated!
                earliestExitTime += tauWithVehLength(tau, (*veh)->getVehicleType().getLengthWithGap());
            }
        }
        if (count == 0) {
            myMeanSpeed = myEdge.getSpeedLimit();
        } else {
            myMeanSpeed = v / (SUMOReal) count;
        }
    }
    return myMeanSpeed;
}


void
MESegment::writeVehicles(OutputDevice& of) const {
    for (Queues::const_iterator k = myCarQues.begin(); k != myCarQues.end(); ++k) {
        for (std::vector<MEVehicle*>::const_iterator veh = k->begin(); veh != k->end(); ++veh) {
            MSXMLRawOut::writeVehicle(of, *(*veh));
        }
    }
}


MEVehicle*
MESegment::removeCar(MEVehicle* v, SUMOTime leaveTime, MESegment* next) {
    myOccupancy = MAX2((SUMOReal)0, myOccupancy - v->getVehicleType().getLengthWithGap());
    std::vector<MEVehicle*>& cars = myCarQues[v->getQueIndex()];
    assert(std::find(cars.begin(), cars.end(), v) != cars.end());
    // One could be tempted to do  v->setSegment(next); here but position on lane will be invalid if next == 0
    updateDetectorsOnLeave(v, leaveTime, next);
    myEdge.lock();
    if (v == cars.back()) {
        cars.pop_back();
        if (!cars.empty()) {
            myEdge.unlock();
            return cars.back();
        }
    } else {
        cars.erase(std::find(cars.begin(), cars.end(), v));
    }
    myEdge.unlock();
    return 0;
}


SUMOTime
MESegment::getTimeHeadway(const MESegment* pred, const MEVehicle* veh) {
    const SUMOTime tau = (pred->free()
                          ? (free() ? myTau_ff : myTau_fj)
                          : (free() ? myTau_jf : TIME2STEPS(myA * getCarNumber() + myB)));
    return (SUMOTime)(tauWithVehLength(tau, veh->getVehicleType().getLengthWithGap()) / pred->getTLSCapacity(veh));
}


SUMOTime
MESegment::getNextInsertionTime(SUMOTime earliestEntry) const {
    // since we do not know which queue will be used we give a conservative estimate
    SUMOTime earliestLeave = earliestEntry;
    for (int i = 0; i < (int)myCarQues.size(); ++i) {
        earliestLeave = MAX2(earliestLeave, myBlockTimes[i]);
    }
    if (myEdge.getSpeedLimit() == 0) {
        return MAX2(earliestEntry, myEntryBlockTime);    // FIXME: This line is just an adhoc-fix to avoid division by zero (Leo)
    } else {
        return MAX3(earliestEntry, earliestLeave - TIME2STEPS(myLength / myEdge.getSpeedLimit()), myEntryBlockTime);
    }
}


MSLink*
MESegment::getLink(const MEVehicle* veh, bool penalty) const {
    if (myJunctionControl || penalty) {
        const MSEdge* const nextEdge = veh->succEdge(1);
        if (nextEdge == 0) {
            return 0;
        }
        // try to find any link leading to our next edge, start with the lane pointed to by the que index
        const MSLane* const bestLane = myEdge.getLanes()[veh->getQueIndex()];
        const MSLinkCont& links = bestLane->getLinkCont();
        for (std::vector<MSLink*>::const_iterator j = links.begin(); j != links.end(); ++j) {
            if (&(*j)->getLane()->getEdge() == nextEdge) {
                return *j;
            }
        }
        // this is for the non-multique case, maybe we should use caching here !!!
        for (std::vector<MSLane*>::const_iterator l = myEdge.getLanes().begin(); l != myEdge.getLanes().end(); ++l) {
            if ((*l) != bestLane) {
                const MSLinkCont& links = (*l)->getLinkCont();
                for (std::vector<MSLink*>::const_iterator j = links.begin(); j != links.end(); ++j) {
                    if (&(*j)->getLane()->getEdge() == nextEdge) {
                        return *j;
                    }
                }
            }
        }
    }
    return 0;
}


bool
MESegment::isOpen(const MEVehicle* veh) const {
    if (myTLSPenalty) {
        // XXX should limited control take precedence over tls penalty?
        return true;
    }
    const MSLink* link = getLink(veh);
    return (link == 0
            || link->havePriority()
            || limitedControlOverride(link)
            || link->opened(veh->getEventTime(), veh->getSpeed(), veh->estimateLeaveSpeed(link),
                            veh->getVehicleType().getLengthWithGap(), veh->getImpatience(),
                            veh->getVehicleType().getCarFollowModel().getMaxDecel(), veh->getWaitingTime()));
}


bool
MESegment::limitedControlOverride(const MSLink* link) const {
    assert(link != 0);
    if (!MSGlobals::gMesoLimitedJunctionControl) {
        return false;
    }
    // if the target segment of this link is not saturated junction control is disabled
    const MSEdge& targetEdge = link->getLane()->getEdge();
    const MESegment* target = MSGlobals::gMesoNet->getSegmentForEdge(targetEdge);
    return target->myOccupancy * 2 < target->myJamThreshold;
}


void
MESegment::send(MEVehicle* veh, MESegment* next, SUMOTime time) {
    assert(isInvalid(next) || time >= myBlockTimes[veh->getQueIndex()]);
    MSLink* link = getLink(veh);
    if (link != 0) {
        link->removeApproaching(veh);
    }
    MEVehicle* lc = removeCar(veh, time, next); // new leaderCar
    myBlockTimes[veh->getQueIndex()] = time;
    if (!isInvalid(next)) {
        myLastHeadway = next->getTimeHeadway(this, veh);
        myBlockTimes[veh->getQueIndex()] += myLastHeadway;
    }
    if (lc != 0) {
        lc->setEventTime(MAX2(lc->getEventTime(), myBlockTimes[veh->getQueIndex()]));
        MSGlobals::gMesoNet->addLeaderCar(lc, getLink(lc));
    }
}

bool
MESegment::overtake() {
    return MSGlobals::gMesoOvertaking && myCapacity > myLength && RandHelper::rand() > (myOccupancy / myCapacity);
}


void
MESegment::addReminders(MEVehicle* veh) const {
    for (std::vector<MSMoveReminder*>::const_iterator i = myDetectorData.begin(); i != myDetectorData.end(); ++i) {
        veh->addReminder(*i);
    }
}

void
MESegment::receive(MEVehicle* veh, SUMOTime time, bool isDepart, bool afterTeleport) {
    const SUMOReal speed = isDepart ? -1 : MAX2(veh->getSpeed(), MESO_MIN_SPEED); // on the previous segment
    veh->setSegment(this); // for arrival checking
    veh->setLastEntryTime(time);
    veh->setBlockTime(SUMOTime_MAX);
    if (!isDepart && (
                // arrival on entering a new edge
                ((myIndex == 0 || afterTeleport) && veh->moveRoutePointer())
                // arrival on entering a new segment
                || veh->hasArrived())) {
        // route has ended
        veh->setEventTime(time + TIME2STEPS(myLength / speed)); // for correct arrival speed
        addReminders(veh);
        veh->activateReminders(MSMoveReminder::NOTIFICATION_JUNCTION);
        updateDetectorsOnLeave(veh, time, 0);
        MSNet::getInstance()->getVehicleControl().scheduleVehicleRemoval(veh);
        return;
    }
    // route continues
    const SUMOReal maxSpeedOnEdge = veh->getEdge()->getVehicleMaxSpeed(veh);
    const SUMOReal uspeed = MAX2(maxSpeedOnEdge, MESO_MIN_SPEED);
    int nextQueIndex = 0;
    if (myCarQues.size() > 1) {
        const MSEdge* succ = veh->succEdge(1);
        // succ may be invalid if called from initialise() with an invalid route
        if (succ != 0 && myFollowerMap.count(succ) > 0) {
            const std::vector<int>& indices = myFollowerMap[succ];
            nextQueIndex = indices[0];
            for (std::vector<int>::const_iterator i = indices.begin() + 1; i != indices.end(); ++i) {
                if (myCarQues[*i].size() < myCarQues[nextQueIndex].size()) {
                    nextQueIndex = *i;
                }
            }
        }
    }
    std::vector<MEVehicle*>& cars = myCarQues[nextQueIndex];
    MEVehicle* newLeader = 0; // first vehicle in the current queue
    SUMOTime tleave = MAX2(time + TIME2STEPS(myLength / uspeed) + veh->getStoptime(this) + getLinkPenalty(veh), myBlockTimes[nextQueIndex]);
    myEdge.lock();
    if (cars.empty()) {
        cars.push_back(veh);
        newLeader = veh;
    } else {
        SUMOTime leaderOut = cars[0]->getEventTime();
        if (!isDepart && leaderOut > tleave && overtake()) {
            if (cars.size() == 1) {
                MSGlobals::gMesoNet->removeLeaderCar(cars[0]);
                newLeader = veh;
            }
            cars.insert(cars.begin() + 1, veh);
        } else {
            tleave = MAX2(leaderOut + tauWithVehLength(myTau_ff, cars[0]->getVehicleType().getLengthWithGap()), tleave);
            cars.insert(cars.begin(), veh);
        }
    }
    myEdge.unlock();
    if (!isDepart) {
        // regular departs could take place anywhere on the edge so they should not block regular flow
        // the -1 facilitates interleaving of multiple streams
        myEntryBlockTime = time + tauWithVehLength(myTau_ff, veh->getVehicleType().getLengthWithGap()) - 1;
    }
    veh->setEventTime(tleave);
    veh->setSegment(this, nextQueIndex);
    myOccupancy = MIN2(myCapacity, myOccupancy + veh->getVehicleType().getLengthWithGap());
    addReminders(veh);
    if (isDepart) {
        veh->onDepart();
        veh->activateReminders(MSMoveReminder::NOTIFICATION_DEPARTED);
    } else if (myIndex == 0 || afterTeleport) {
        veh->activateReminders(MSMoveReminder::NOTIFICATION_JUNCTION);
    } else {
        veh->activateReminders(MSMoveReminder::NOTIFICATION_SEGMENT);
    }
    if (newLeader != 0) {
        MSGlobals::gMesoNet->addLeaderCar(newLeader, getLink(newLeader));
    }
}


bool
MESegment::vaporizeAnyCar(SUMOTime currentTime) {
    MEVehicle* remove = 0;
    for (Queues::const_iterator k = myCarQues.begin(); k != myCarQues.end(); ++k) {
        if (!k->empty()) {
            // remove last in queue
            remove = k->front();
            if (k->size() == 1) {
                MSGlobals::gMesoNet->removeLeaderCar(remove);
            }
            MSGlobals::gMesoNet->changeSegment(remove, currentTime, &myVaporizationTarget);
            return true;
        }
    }
    return false;
}


void
MESegment::setSpeedForQueue(SUMOReal newSpeed, SUMOTime currentTime, SUMOTime blockTime, const std::vector<MEVehicle*>& vehs) {
    MEVehicle* v = vehs.back();
    v->updateDetectors(currentTime, false);
    SUMOTime newEvent = MAX2(newArrival(v, newSpeed, currentTime), blockTime);
    if (v->getEventTime() != newEvent) {
        MSGlobals::gMesoNet->removeLeaderCar(v);
        v->setEventTime(newEvent);
        MSGlobals::gMesoNet->addLeaderCar(v, getLink(v));
    }
    for (std::vector<MEVehicle*>::const_reverse_iterator i = vehs.rbegin() + 1; i != vehs.rend(); ++i) {
        (*i)->updateDetectors(currentTime, false);
        newEvent = MAX2(newArrival(*i, newSpeed, currentTime), newEvent + myTau_ff);
        //newEvent = MAX2(newArrival(*i, newSpeed, currentTime), newEvent + myTau_ff + (SUMOTime)((*(i - 1))->getVehicleType().getLength() / myTau_length));
        (*i)->setEventTime(newEvent);
    }
}


SUMOTime
MESegment::newArrival(const MEVehicle* const v, SUMOReal newSpeed, SUMOTime currentTime) {
    // since speed is only an upper bound pos may be to optimistic
    const SUMOReal pos = MIN2(myLength, STEPS2TIME(currentTime - v->getLastEntryTime()) * v->getSpeed());
    // traveltime may not be 0
    return currentTime + MAX2(TIME2STEPS((myLength - pos) / newSpeed), SUMOTime(1));
}


void
MESegment::setSpeed(SUMOReal newSpeed, SUMOTime currentTime, SUMOReal jamThresh) {
    recomputeJamThreshold(jamThresh);
    //myTau_length = MAX2(MESO_MIN_SPEED, newSpeed) * myEdge.getLanes().size() / TIME2STEPS(1);
    for (int i = 0; i < (int)myCarQues.size(); ++i) {
        if (myCarQues[i].size() != 0) {
            setSpeedForQueue(newSpeed, currentTime, myBlockTimes[i], myCarQues[i]);
        }
    }
}


SUMOTime
MESegment::getEventTime() const {
    SUMOTime result = SUMOTime_MAX;
    for (int i = 0; i < (int)myCarQues.size(); ++i) {
        if (myCarQues[i].size() != 0 && myCarQues[i].back()->getEventTime() < result) {
            result = myCarQues[i].back()->getEventTime();
        }
    }
    if (result < SUMOTime_MAX) {
        return result;
    }
    return -1;
}


void
MESegment::saveState(OutputDevice& out) {
    out.openTag(SUMO_TAG_SEGMENT);
    for (int i = 0; i < (int)myCarQues.size(); ++i) {
        out.openTag(SUMO_TAG_VIEWSETTINGS_VEHICLES).writeAttr(SUMO_ATTR_TIME, toString<SUMOTime>(myBlockTimes[i]));
        out.writeAttr(SUMO_ATTR_VALUE, myCarQues[i]);
        out.closeTag();
    }
    out.closeTag();
}


void
MESegment::loadState(std::vector<std::string>& vehIds, MSVehicleControl& vc, const SUMOTime block, const int queIdx) {
    for (std::vector<std::string>::const_iterator it = vehIds.begin(); it != vehIds.end(); ++it) {
        MEVehicle* v = static_cast<MEVehicle*>(vc.getVehicle(*it));
        assert(v != 0);
        assert(v->getSegment() == this);
        myCarQues[queIdx].push_back(v);
        myOccupancy += v->getVehicleType().getLengthWithGap();
    }
    if (myCarQues[queIdx].size() != 0) {
        // add the last vehicle of this queue
        // !!! one question - what about the previously added vehicle? Is it stored twice?
        MEVehicle* veh = myCarQues[queIdx].back();
        MSGlobals::gMesoNet->addLeaderCar(veh, getLink(veh));
    }
    myBlockTimes[queIdx] = block;
    myOccupancy = MIN2(myCapacity, myOccupancy);
}


std::vector<const MEVehicle*>
MESegment::getVehicles() const {
    std::vector<const MEVehicle*> result;
    for (Queues::const_iterator k = myCarQues.begin(); k != myCarQues.end(); ++k) {
        result.insert(result.end(), k->begin(), k->end());
    }
    return result;
}


bool
MESegment::hasBlockedLeader() const {
    for (Queues::const_iterator k = myCarQues.begin(); k != myCarQues.end(); ++k) {
        if (k->size() > 0 && (*k).back()->getWaitingTime() > 0) {
            return true;
        }
    }
    return false;
}


SUMOReal
MESegment::getFlow() const {
    return 3600 * getCarNumber() * getMeanSpeed() / myLength;
}


SUMOTime
MESegment::getLinkPenalty(const MEVehicle* veh) const {
    const MSLink* link = getLink(veh, myTLSPenalty || myMinorPenalty);
    if (link != 0) {
        SUMOTime result = 0;
        if (link->isTLSControlled()) {
            result += link->getMesoTLSPenalty();
        }
        // minor tls links may get an additional penalty
        if (!link->havePriority() &&
                // do not apply penalty if limited control is active
                (!MSGlobals::gMesoLimitedJunctionControl || limitedControlOverride(link))) {
            result += MSGlobals::gMesoMinorPenalty;
        }
        return result;
    } else {
        return 0;
    }
}


SUMOReal
MESegment::getTLSCapacity(const MEVehicle* veh) const {
    if (myTLSPenalty) {
        const MSLink* link = getLink(veh, true);
        if (link != 0) {
            assert(link->isTLSControlled());
            assert(link->getGreenFraction() > 0);
            return link->getGreenFraction();
        }
    }
    return 1;
}


SUMOReal
MESegment::getMaxPenaltySeconds() const {
    SUMOReal maxPenalty = 0;
    for (std::vector<MSLane*>::const_iterator i = myEdge.getLanes().begin(); i != myEdge.getLanes().end(); ++i) {
        MSLane* l = *i;
        const MSLinkCont& lc = l->getLinkCont();
        for (MSLinkCont::const_iterator j = lc.begin(); j != lc.end(); ++j) {
            MSLink* link = *j;
            maxPenalty = MAX2(maxPenalty, STEPS2TIME(
                                  link->getMesoTLSPenalty() + (link->havePriority() ? 0 : MSGlobals::gMesoMinorPenalty)));
        }
    }
    return maxPenalty;
}

/****************************************************************************/
