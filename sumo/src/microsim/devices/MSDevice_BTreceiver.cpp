/****************************************************************************/
/// @file    MSDevice_BTreceiver.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    14.08.2013
/// @version $Id$
///
// A BT sender
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2013-2014 DLR (http://www.dlr.de/) and contributors
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
#include <utils/vehicle/SUMOVehicle.h>
#include <utils/geom/Position.h>
#include <utils/geom/Line.h>
#include <utils/geom/GeomHelper.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSVehicle.h>
#include "MSDevice_Tripinfo.h"
#include "MSDevice_BTreceiver.h"
#include "MSDevice_BTsender.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static members
// ===========================================================================
bool MSDevice_BTreceiver::myWasInitialised = false;
SUMOReal MSDevice_BTreceiver::myRange = -1.;
SUMOReal MSDevice_BTreceiver::myOffTime = -1.;
MTRand MSDevice_BTreceiver::sRecognitionRNG;
std::map<std::string, MSDevice_BTreceiver::VehicleInformation*> MSDevice_BTreceiver::sVehicles;


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

    oc.doRegister("device.btreceiver.all-recognitions", new Option_Bool(false));
    oc.addDescription("device.btreceiver.all-recognitions", "Communication", "Whether all recognition point shall be written");

    oc.doRegister("device.btreceiver.offtime", new Option_Float(0.64));
    oc.addDescription("device.btreceiver.offtime", "Communication", "The offtime used for calculating detection probability (in seconds)");

    myWasInitialised = false;
}


void
MSDevice_BTreceiver::buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into) {
    OptionsCont& oc = OptionsCont::getOptions();
    if (equippedByDefaultAssignmentOptions(oc, "btreceiver", v)) {
        MSDevice_BTreceiver* device = new MSDevice_BTreceiver(v, "btreceiver_" + v.getID());
        into.push_back(device);
        if (!myWasInitialised) {
            new BTreceiverUpdate();
            myWasInitialised = true;
            myRange = oc.getFloat("device.btreceiver.range");
            myOffTime = oc.getFloat("device.btreceiver.offtime");
            sRecognitionRNG.seed(oc.getInt("seed"));
        }
    }
}


// ---------------------------------------------------------------------------
// MSDevice_BTreceiver::BTreceiverUpdate-methods
// ---------------------------------------------------------------------------
MSDevice_BTreceiver::BTreceiverUpdate::BTreceiverUpdate() {
    MSNet::getInstance()->getEndOfTimestepEvents()->addEvent(this, 0, MSEventControl::ADAPT_AFTER_EXECUTION);
}


MSDevice_BTreceiver::BTreceiverUpdate::~BTreceiverUpdate() {
    for (std::map<std::string, MSDevice_BTsender::VehicleInformation*>::const_iterator i = MSDevice_BTsender::sVehicles.begin(); i != MSDevice_BTsender::sVehicles.end(); ++i) {
        (*i).second->amOnNet = false;
        (*i).second->haveArrived = true;
    }
    for (std::map<std::string, MSDevice_BTreceiver::VehicleInformation*>::const_iterator i = MSDevice_BTreceiver::sVehicles.begin(); i != MSDevice_BTreceiver::sVehicles.end(); ++i) {
        (*i).second->amOnNet = false;
        (*i).second->haveArrived = true;
    }
    execute(MSNet::getInstance()->getCurrentTimeStep());
}


SUMOTime
MSDevice_BTreceiver::BTreceiverUpdate::execute(SUMOTime /*currentTime*/) {
    // build rtree with senders
    NamedRTree rt;
    for (std::map<std::string, MSDevice_BTsender::VehicleInformation*>::const_iterator i = MSDevice_BTsender::sVehicles.begin(); i != MSDevice_BTsender::sVehicles.end(); ++i) {
        MSDevice_BTsender::VehicleInformation* vi = (*i).second;
        Boundary b = vi->getBoxBoundary();
        b.grow(POSITION_EPS);
        const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
        const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
        rt.Insert(cmin, cmax, vi);
    }

    // check visibility for all receivers
    OptionsCont& oc = OptionsCont::getOptions();
    bool allRecognitions = oc.getBool("device.btreceiver.all-recognitions");
    bool haveOutput = oc.isSet("bt-output");
    for (std::map<std::string, MSDevice_BTreceiver::VehicleInformation*>::iterator i = MSDevice_BTreceiver::sVehicles.begin(); i != MSDevice_BTreceiver::sVehicles.end();) {
        // collect surrounding vehicles
        MSDevice_BTreceiver::VehicleInformation* vi = (*i).second;
        Boundary b = vi->getBoxBoundary();
        b.grow(vi->range);
        const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
        const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
        std::set<std::string> surroundingVehicles;
        Named::StoringVisitor sv(surroundingVehicles);
        rt.Search(cmin, cmax, sv);

        // loop over surrounding vehicles, check visibility status
        for (std::set<std::string>::const_iterator j = surroundingVehicles.begin(); j != surroundingVehicles.end(); ++j) {
            if ((*i).first == *j) {
                // seeing oneself? skip
                continue;
            }
            updateVisibility(*vi, *MSDevice_BTsender::sVehicles.find(*j)->second);
        }

        if (vi->haveArrived) {
            // vehicle has left the simulation; remove
            if (haveOutput) {
                writeOutput((*i).first, vi->seen, allRecognitions);
            }
            delete vi;
            MSDevice_BTreceiver::sVehicles.erase(i++);
        } else {
            // vehicle is still in the simulation; reset state
            vi->updates.erase(vi->updates.begin(), vi->updates.end() - 1);
            ++i;
        }
    }

    // remove arrived senders / reset state
    for (std::map<std::string, MSDevice_BTsender::VehicleInformation*>::iterator i = MSDevice_BTsender::sVehicles.begin(); i != MSDevice_BTsender::sVehicles.end();) {
        MSDevice_BTsender::VehicleInformation* vi = (*i).second;
        if (vi->haveArrived) {
            delete vi;
            MSDevice_BTsender::sVehicles.erase(i++);
        } else {
            vi->updates.erase(vi->updates.begin(), vi->updates.end() - 1);
            ++i;
        }
    }
    return DELTA_T;
}


void
MSDevice_BTreceiver::BTreceiverUpdate::updateVisibility(MSDevice_BTreceiver::VehicleInformation& receiver,
        MSDevice_BTsender::VehicleInformation& sender) {
    const MSDevice_BTsender::VehicleState& receiverData = receiver.updates.back();
    const MSDevice_BTsender::VehicleState& senderData = sender.updates.back();
    if (!receiver.amOnNet || !sender.amOnNet) {
        // at least one of the vehicles has left the simulation area for any reason
        if (receiver.currentlySeen.find(sender.getID()) != receiver.currentlySeen.end()) {
            leaveRange(receiver.currentlySeen, receiver.seen, receiverData.position, receiverData.speed, receiverData.laneID, receiverData.lanePos,
                       sender.getID(), senderData.position, senderData.speed, senderData.laneID, senderData.lanePos, 0);
        }
    }

    const Position& oldReceiverPosition = receiver.updates.front().position;
    const Position& oldSenderPosition = sender.updates.front().position;

    // let the other's current position be the one obtained by applying the relative direction vector to the initial position
    const Position senderDelta = senderData.position - oldSenderPosition;
    const Position receiverDelta = receiverData.position - oldReceiverPosition;
    const Position translatedSender = senderData.position - receiverDelta;
    // find crossing points
    std::vector<SUMOReal> intersections;
    GeomHelper::FindLineCircleIntersections(oldReceiverPosition, receiver.range, oldSenderPosition, translatedSender, intersections);
    switch (intersections.size()) {
        case 0:
            // no intersections -> other vehicle either stays within or beyond range
            if (receiver.amOnNet && sender.amOnNet && receiverData.position.distanceTo(senderData.position) < receiver.range) {
                if (receiver.currentlySeen.find(sender.getID()) == receiver.currentlySeen.end()) {
                    enterRange(0., receiverData.position, receiverData.speed, receiverData.laneID, receiverData.lanePos,
                               sender.getID(), senderData.position, senderData.speed, senderData.laneID, senderData.lanePos, receiver.currentlySeen);
                } else {
                    addRecognitionPoint(SIMTIME, receiverData.position, receiverData.speed, receiverData.laneID, receiverData.lanePos,
                                        senderData.position, senderData.speed, senderData.laneID, senderData.lanePos, receiver.currentlySeen[sender.getID()]);
                }
            } else {
                if (receiver.currentlySeen.find(sender.getID()) != receiver.currentlySeen.end()) {
                    leaveRange(receiver.currentlySeen, receiver.seen, receiverData.position, receiverData.speed, receiverData.laneID, receiverData.lanePos,
                               sender.getID(), senderData.position, senderData.speed, senderData.laneID, senderData.lanePos, 0.);
                }
            }
            break;
        case 1: {
            // one intersection -> other vehicle either enters or leaves the range
            const Position intersection1Sender = oldSenderPosition + senderDelta * intersections.front();
            const Position intersection1Receiver = oldReceiverPosition + receiverDelta * intersections.front();
            if (receiver.currentlySeen.find(sender.getID()) != receiver.currentlySeen.end()) {
                leaveRange(receiver.currentlySeen, receiver.seen, intersection1Receiver, receiverData.speed, receiverData.laneID, receiverData.lanePos,
                           sender.getID(), intersection1Sender, senderData.speed, senderData.laneID, senderData.lanePos, (intersections.front() - 1.) * TS);
            } else {
                enterRange((intersections.front() - 1.) * TS, intersection1Receiver, receiverData.speed, receiverData.laneID, receiverData.lanePos,
                           sender.getID(), intersection1Sender, senderData.speed, senderData.laneID, senderData.lanePos, receiver.currentlySeen);
            }
        }
        break;
        case 2:
            // two intersections -> other vehicle enters and leaves the range
            if (receiver.currentlySeen.find(sender.getID()) == receiver.currentlySeen.end()) {
                const Position intersection1Sender = oldSenderPosition + senderDelta * intersections.front();
                const Position intersection1Receiver = oldReceiverPosition + receiverDelta * intersections.front();
                enterRange((intersections.front() - 1.) * TS, intersection1Receiver, receiverData.speed, receiverData.laneID, receiverData.lanePos,
                           sender.getID(), intersection1Sender, senderData.speed, senderData.laneID, senderData.lanePos, receiver.currentlySeen);
                const Position intersection2Sender = oldSenderPosition + senderDelta * intersections.back();
                const Position intersection2Receiver = oldReceiverPosition + receiverDelta * intersections.back();
                leaveRange(receiver.currentlySeen, receiver.seen, intersection2Receiver, receiverData.speed, receiverData.laneID, receiverData.lanePos,
                           sender.getID(), intersection2Sender, senderData.speed, senderData.laneID, senderData.lanePos, (intersections.back() - 1.) * TS);
            } else {
                WRITE_WARNING("The vehicle '" + sender.getID() + "' cannot be in the range of '" + receiver.getID() + "', leave, and enter it in one step.");
            }
            break;
        default:
            WRITE_WARNING("Nope, a circle cannot be crossed more often than twice by a line.");
            break;
    }
}


void
MSDevice_BTreceiver::BTreceiverUpdate::enterRange(SUMOReal atOffset,
        const Position& thisPos, SUMOReal thisSpeed, const std::string& thisLaneID, SUMOReal thisLanePos,
        const std::string& otherID, const Position& otherPos, SUMOReal otherSpeed, const std::string& otherLaneID, SUMOReal otherLanePos,
        std::map<std::string, SeenDevice*>& currentlySeen) {
    MeetingPoint mp(SIMTIME + atOffset, thisPos, thisSpeed, thisLaneID, thisLanePos, otherPos, otherSpeed, otherLaneID, otherLanePos);
    SeenDevice* sd = new SeenDevice(mp);
    currentlySeen[otherID] = sd;
    addRecognitionPoint(SIMTIME, thisPos, thisSpeed, thisLaneID, thisLanePos,
                        otherPos, otherSpeed, otherLaneID, otherLanePos, sd);
}


void
MSDevice_BTreceiver::BTreceiverUpdate::leaveRange(std::map<std::string, SeenDevice*>& currentlySeen, std::map<std::string, std::vector<SeenDevice*> >& seen,
        const Position& thisPos, SUMOReal thisSpeed, const std::string& thisLaneID, SUMOReal thisLanePos,
        const std::string& otherID, const Position& otherPos, SUMOReal otherSpeed, const std::string& otherLaneID, SUMOReal otherLanePos,
        SUMOReal tOffset) {
    std::map<std::string, SeenDevice*>::iterator i = currentlySeen.find(otherID);
    // check whether the other was recognized
    addRecognitionPoint(SIMTIME + tOffset, thisPos, thisSpeed, thisLaneID, thisLanePos,
                        otherPos, otherSpeed, otherLaneID, otherLanePos, (*i).second);
    // build leaving point
    MeetingPoint mp(STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep()) + tOffset, thisPos, thisSpeed, thisLaneID, thisLanePos, otherPos, otherSpeed, otherLaneID, otherLanePos);
    (*i).second->meetingEnd = mp;
    if (seen.find(otherID) == seen.end()) {
        seen[otherID] = std::vector<SeenDevice*>();
    }
    seen[otherID].push_back((*i).second);
    currentlySeen.erase(i);
}


void
MSDevice_BTreceiver::BTreceiverUpdate::addRecognitionPoint(const SUMOReal tEnd, const Position& thisPos, const SUMOReal thisSpeed, const std::string& thisLaneID, const SUMOReal thisLanePos,
        const Position& otherPos, const SUMOReal otherSpeed, const std::string& otherLaneID, const SUMOReal otherLanePos,
        SeenDevice* otherDevice) const {
    const SUMOReal t = tEnd - MAX2(SIMTIME - TS, otherDevice->lastView);
    // probability of a miss 0.5 (may be in the wrong train), backoff time 0.64s
    if (sRecognitionRNG.rand() <= 1 - pow(0.5, t / myOffTime)) {
        otherDevice->lastView = tEnd;
        MeetingPoint* mp = new MeetingPoint(tEnd, thisPos, thisSpeed, thisLaneID, thisLanePos, otherPos, otherSpeed, otherLaneID, otherLanePos);
        otherDevice->recognitionPoints.push_back(mp);
    }
}


void
MSDevice_BTreceiver::BTreceiverUpdate::writeOutput(const std::string& id, const std::map<std::string, std::vector<SeenDevice*> >& seen, bool allRecognitions) {
    OutputDevice& os = OutputDevice::getDeviceByOption("bt-output");
    os.openTag("bt").writeAttr("id", id);
    for (std::map<std::string, std::vector<SeenDevice*> >::const_iterator j = seen.begin(); j != seen.end(); ++j) {
        const std::vector<SeenDevice*>& sts = (*j).second;
        for (std::vector<SeenDevice*>::const_iterator k = sts.begin(); k != sts.end(); ++k) {
            os.openTag("seen").writeAttr("id", (*j).first);
            os.writeAttr("tBeg", (*k)->meetingBegin.t)
            .writeAttr("observerPosBeg", (*k)->meetingBegin.observerPos).writeAttr("observerSpeedBeg", (*k)->meetingBegin.observerSpeed)
            .writeAttr("observerLaneIDBeg", (*k)->meetingBegin.observerLaneID).writeAttr("observerLanePosBeg", (*k)->meetingBegin.observerLanePos)
            .writeAttr("seenPosBeg", (*k)->meetingBegin.seenPos).writeAttr("seenSpeedBeg", (*k)->meetingBegin.seenSpeed)
            .writeAttr("seenLaneIDBeg", (*k)->meetingBegin.seenLaneID).writeAttr("seenLanePosBeg", (*k)->meetingBegin.seenLanePos);
            os.writeAttr("tEnd", (*k)->meetingEnd.t)
            .writeAttr("observerPosEnd", (*k)->meetingEnd.observerPos).writeAttr("observerSpeedEnd", (*k)->meetingEnd.observerSpeed)
            .writeAttr("observerLaneIDEnd", (*k)->meetingEnd.observerLaneID).writeAttr("observerLanePosEnd", (*k)->meetingEnd.observerLanePos)
            .writeAttr("seenPosEnd", (*k)->meetingEnd.seenPos).writeAttr("seenSpeedEnd", (*k)->meetingEnd.seenSpeed)
            .writeAttr("seenLaneIDEnd", (*k)->meetingEnd.seenLaneID).writeAttr("seenLanePosEnd", (*k)->meetingEnd.seenLanePos);
            for (std::vector<MeetingPoint*>::iterator l = (*k)->recognitionPoints.begin(); l != (*k)->recognitionPoints.end(); ++l) {
                os.openTag("recognitionPoint").writeAttr("t", (*l)->t)
                .writeAttr("observerPos", (*l)->observerPos).writeAttr("observerSpeed", (*l)->observerSpeed)
                .writeAttr("observerLaneID", (*l)->observerLaneID).writeAttr("observerLanePos", (*l)->observerLanePos)
                .writeAttr("seenPos", (*l)->seenPos).writeAttr("seenSpeed", (*l)->seenSpeed)
                .writeAttr("seenLaneID", (*l)->seenLaneID).writeAttr("seenLanePos", (*l)->seenLanePos)
                .closeTag();
                if (!allRecognitions) {
                    break;
                }
            }
            os.closeTag();
        }
    }
    os.closeTag();
}




// ---------------------------------------------------------------------------
// MSDevice_BTreceiver-methods
// ---------------------------------------------------------------------------
MSDevice_BTreceiver::MSDevice_BTreceiver(SUMOVehicle& holder, const std::string& id)
    : MSDevice(holder, id) {
}


MSDevice_BTreceiver::~MSDevice_BTreceiver() {
}


bool
MSDevice_BTreceiver::notifyEnter(SUMOVehicle& veh, Notification reason) {
    if (reason == MSMoveReminder::NOTIFICATION_DEPARTED && sVehicles.find(veh.getID()) == sVehicles.end()) {
        sVehicles[veh.getID()] = new VehicleInformation(veh.getID(), myRange);
    }
    if (reason == MSMoveReminder::NOTIFICATION_TELEPORT && sVehicles.find(veh.getID()) != sVehicles.end()) {
        sVehicles[veh.getID()]->amOnNet = true;
    }
    sVehicles[veh.getID()]->updates.push_back(MSDevice_BTsender::VehicleState(veh.getSpeed(), static_cast<MSVehicle&>(veh).getPosition(), static_cast<MSVehicle&>(veh).getLane()->getID(), veh.getPositionOnLane()));
    return true;
}


bool
MSDevice_BTreceiver::notifyMove(SUMOVehicle& veh, SUMOReal /* oldPos */, SUMOReal newPos, SUMOReal newSpeed) {
    if (sVehicles.find(veh.getID()) == sVehicles.end()) {
        WRITE_WARNING("btreceiver: Can not update position of a vehicle that is not within the road network (" + veh.getID() + ").");
        return true;
    }
    sVehicles[veh.getID()]->updates.push_back(MSDevice_BTsender::VehicleState(newSpeed, static_cast<MSVehicle&>(veh).getPosition(), static_cast<MSVehicle&>(veh).getLane()->getID(), newPos));
    return true;
}


bool
MSDevice_BTreceiver::notifyLeave(SUMOVehicle& veh, SUMOReal /* lastPos */, Notification reason) {
    if (reason < MSMoveReminder::NOTIFICATION_TELEPORT) {
        return true;
    }
    if (sVehicles.find(veh.getID()) == sVehicles.end()) {
        WRITE_WARNING("btreceiver: Can not update position of a vehicle that is not within the road network (" + veh.getID() + ").");
        return true;
    }
    sVehicles[veh.getID()]->updates.push_back(MSDevice_BTsender::VehicleState(veh.getSpeed(), static_cast<MSVehicle&>(veh).getPosition(), static_cast<MSVehicle&>(veh).getLane()->getID(), veh.getPositionOnLane()));
    if (reason == MSMoveReminder::NOTIFICATION_TELEPORT) {
        sVehicles[veh.getID()]->amOnNet = false;
    }
    if (reason >= MSMoveReminder::NOTIFICATION_ARRIVED) {
        sVehicles[veh.getID()]->amOnNet = false;
        sVehicles[veh.getID()]->haveArrived = true;
    }
    return true;
}





/****************************************************************************/

