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
#include "MSDevice_BTsender.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static members
// ===========================================================================
bool MSDevice_BTreceiver::myWasInitialised = false;
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
}


void
MSDevice_BTreceiver::buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into) {
    OptionsCont& oc = OptionsCont::getOptions();
    if (equippedByDefaultAssignmentOptions(oc, "btreceiver", v)) {
        MSDevice_BTreceiver* device = new MSDevice_BTreceiver(v, "btreceiver_" + v.getID(), oc.getFloat("device.btreceiver.range"));
        into.push_back(device);
        if (!myWasInitialised) {
            new BTreceiverUpdate();
            myWasInitialised = true;
            sRecognitionRNG.seed(oc.getInt("seed"));
        }
    }
}


// ---------------------------------------------------------------------------
// MSDevice_BTreceiver::BTreceiverUpdate-methods
// ---------------------------------------------------------------------------
MSDevice_BTreceiver::BTreceiverUpdate::BTreceiverUpdate() {
    MSNet::getInstance()->getEndOfTimestepEvents().addEvent(this, 0, MSEventControl::ADAPT_AFTER_EXECUTION);
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
        b.grow(3.);
        const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
        const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
        rt.Insert(cmin, cmax, vi);
    }

    // check visibility for all receivers
    OptionsCont& oc = OptionsCont::getOptions();
    bool allRecognitions = oc.getBool("device.btreceiver.all-recognitions");
    bool haveOutput = oc.isSet("bt-output");
    for (std::map<std::string, MSDevice_BTreceiver::VehicleInformation*>::iterator i = MSDevice_BTreceiver::sVehicles.begin(); i != MSDevice_BTreceiver::sVehicles.end();) {
        // compute own direction vector
        MSDevice_BTreceiver::VehicleState& state = (*i).second->updates.back();
        Position egoPosition = state.position;
        SUMOReal angle = state.angle * M_PI / 180.;
        SUMOReal speed = state.speed;
        SUMOReal dist = SPEED2DIST(speed);
        Position egoP2 = egoPosition;
        Position egoP1(egoP2.x() - sin(angle)*dist, egoP2.y() + cos(angle)*dist);
        Position egoD = egoP2 - egoP1;

        // collect surrounding vehicles
        MSDevice_BTreceiver::VehicleInformation* vi = (*i).second;
        Boundary b = vi->getBoxBoundary();
        b.grow(vi->range + 500 / 3.6); // two vehicles passing at 250*2 km/h
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
            updateVisibility(*vi, *MSDevice_BTsender::sVehicles.find(*j)->second, egoP1, egoD);
        }

        if (vi->haveArrived) {
            // vehicle has left the simulation; remove
            if (haveOutput) {
                writeOutput((*i).first, vi->seen, allRecognitions);
            }
            delete(*i).second;
            MSDevice_BTreceiver::sVehicles.erase(i++);
        } else {
            // vehicle is still in the simulation; reset state
            VehicleState last = (*i).second->updates.back();
            (*i).second->updates.clear();
            (*i).second->updates.push_back(last);
            ++i;
        }
    }

    // remove arrived senders / reset state
    for (std::map<std::string, MSDevice_BTsender::VehicleInformation*>::iterator i = MSDevice_BTsender::sVehicles.begin(); i != MSDevice_BTsender::sVehicles.end();) {
        if ((*i).second->haveArrived) {
            delete(*i).second;
            MSDevice_BTsender::sVehicles.erase(i++);
        } else {
            MSDevice_BTsender::VehicleState last = (*i).second->updates.back();
            (*i).second->updates.clear();
            (*i).second->updates.push_back(last);
            ++i;
        }
    }
    return DELTA_T;
}


void
MSDevice_BTreceiver::BTreceiverUpdate::updateVisibility(MSDevice_BTreceiver::VehicleInformation& receiver, MSDevice_BTsender::VehicleInformation& sender,
        const Position& receiverStartPos, const Position& receiverD) {
    Position receiverPos = receiver.updates.back().position;
    std::vector<SUMOReal> intersections;
    if (!receiver.amOnNet || !sender.amOnNet) {
        // at least one of the vehicles has left the simulation area for any reason
        if (receiver.currentlySeen.find(sender.getID()) != receiver.currentlySeen.end()) {
            leaveRange(receiver.currentlySeen, receiver.seen, receiverPos, receiver.updates.back().speed, receiver.updates.back().laneID, receiver.updates.back().lanePos,
                       sender.getID(), sender.updates.back().position, sender.updates.back().speed, sender.updates.back().laneID, sender.updates.back().lanePos, 0);
        }
    }

    // get the encountered vehicle's current and prior position (based on speed and direction)
    Position otherPosition = sender.updates.back().position;
    Position otherP2 = otherPosition;
    SUMOReal angle = sender.updates.back().angle * M_PI / 180.;
    SUMOReal speed = sender.updates.back().speed;
    SUMOReal dist = SPEED2DIST(speed);

    // let the other's current position be the one obtained by applying the relative direction vector to the initial position
    Position otherP1(otherPosition.x() - sin(angle)*dist, otherPosition.y() + cos(angle)*dist);
    Position otherD = otherP2 - otherP1;
    otherP2 = otherP1 - receiverD + otherD;
    // find crossing points
    GeomHelper::FindLineCircleIntersections(receiverStartPos, receiver.range, otherP1, otherP2, intersections);
    int count = (int) intersections.size();
    switch (count) {
        case 0:
            // no intersections -> other vehicle either stays within or beyond range
            if (receiver.amOnNet && sender.amOnNet && receiverPos.distanceTo(otherPosition) < receiver.range) {
                if (receiver.currentlySeen.find(sender.getID()) == receiver.currentlySeen.end()) {
                    SUMOReal atOffset = 0;
                    enterRange(atOffset, receiverPos, receiver.updates.back().speed, receiver.updates.back().laneID, receiver.updates.back().lanePos,
                               sender.getID(), otherPosition, sender.updates.back().speed, sender.updates.back().laneID, sender.updates.back().lanePos, receiver.currentlySeen);
                } else {
                    addRecognitionPoint(STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep()), receiverPos, receiver.updates.back().speed, receiver.updates.back().laneID, receiver.updates.back().lanePos,
                                        sender.getID(), otherPosition, sender.updates.back().speed, sender.updates.back().laneID, sender.updates.back().lanePos, receiver.currentlySeen);
                }
            } else {
                if (receiver.currentlySeen.find(sender.getID()) != receiver.currentlySeen.end()) {
                    leaveRange(receiver.currentlySeen, receiver.seen, receiverPos, receiver.updates.back().speed, receiver.updates.back().laneID, receiver.updates.back().lanePos,
                               sender.getID(), otherPosition, sender.updates.back().speed, sender.updates.back().laneID, sender.updates.back().lanePos, 0);
                }
            }
            break;
        case 1: {
            // one intersection -> other vehicle either enters or leaves the range
            Position intersection1Other = otherP1 + otherD * intersections.front();
            Position intersection1Ego = receiverStartPos + receiverD * intersections.front();
            if (receiver.currentlySeen.find(sender.getID()) != receiver.currentlySeen.end()) {
                leaveRange(receiver.currentlySeen, receiver.seen, intersection1Ego, receiver.updates.back().speed, receiver.updates.back().laneID, receiver.updates.back().lanePos,
                           sender.getID(), intersection1Other, sender.updates.back().speed, sender.updates.back().laneID, sender.updates.back().lanePos, -1. + intersections.front());
            } else {
                enterRange(-1. + intersections.front(), intersection1Ego, receiver.updates.back().speed, receiver.updates.back().laneID, receiver.updates.back().lanePos,
                           sender.getID(), intersection1Other, sender.updates.back().speed, sender.updates.back().laneID, sender.updates.back().lanePos, receiver.currentlySeen);
            }
        }
        break;
        case 2:
            // two intersections -> other vehicle enters and leaves the range
            if (receiver.currentlySeen.find(sender.getID()) == receiver.currentlySeen.end()) {
                Position intersection1Other = otherP1 + otherD * intersections.front();
                Position intersection1Ego = receiverStartPos + receiverD * intersections.front();
                enterRange(-1. + intersections.front(), intersection1Ego, receiver.updates.back().speed, receiver.updates.back().laneID, receiver.updates.back().lanePos,
                           sender.getID(), intersection1Other, sender.updates.back().speed, sender.updates.back().laneID, sender.updates.back().lanePos, receiver.currentlySeen);
                Position intersection2Other = otherP1 + otherD * intersections[1];
                Position intersection2Ego = receiverStartPos + receiverD * intersections[1];
                leaveRange(receiver.currentlySeen, receiver.seen, intersection2Ego, receiver.updates.back().speed, receiver.updates.back().laneID, receiver.updates.back().lanePos,
                           sender.getID(), intersection2Other, sender.updates.back().speed, sender.updates.back().laneID, sender.updates.back().lanePos, -1. + intersections.front());
            } else {
                WRITE_WARNING("Nope, a vehicle cannot be in the range, leave, and enter it in one step.");
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
    MeetingPoint mp(STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep()) + atOffset, thisPos, thisSpeed, thisLaneID, thisLanePos, otherPos, otherSpeed, otherLaneID, otherLanePos);
    SeenDevice* sd = new SeenDevice(mp);
    currentlySeen[otherID] = sd;
    addRecognitionPoint(STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep()), thisPos, thisSpeed, thisLaneID, thisLanePos,
                        otherID, otherPos, otherSpeed, otherLaneID, otherLanePos, currentlySeen);
}


void
MSDevice_BTreceiver::BTreceiverUpdate::leaveRange(std::map<std::string, SeenDevice*>& currentlySeen, std::map<std::string, std::vector<SeenDevice*> >& seen,
        const Position& thisPos, SUMOReal thisSpeed, const std::string& thisLaneID, SUMOReal thisLanePos,
        const std::string& otherID, const Position& otherPos, SUMOReal otherSpeed, const std::string& otherLaneID, SUMOReal otherLanePos,
        SUMOReal tOffset) {
    // check whether the other was recognized
    addRecognitionPoint(STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep()) + tOffset, thisPos, thisSpeed, thisLaneID, thisLanePos,
                        otherID, otherPos, otherSpeed, otherLaneID, otherLanePos, currentlySeen);
    // build leaving point
    MeetingPoint mp(STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep()) + tOffset, thisPos, thisSpeed, thisLaneID, thisLanePos, otherPos, otherSpeed, otherLaneID, otherLanePos);
    std::map<std::string, SeenDevice*>::iterator i = currentlySeen.find(otherID);
    (*i).second->meetingEnd = mp;
    if (seen.find(otherID) == seen.end()) {
        seen[otherID] = std::vector<SeenDevice*>();
    }
    seen[otherID].push_back((*i).second);
    currentlySeen.erase(i);
}


void
MSDevice_BTreceiver::BTreceiverUpdate::addRecognitionPoint(SUMOReal tEnd, const Position& thisPos, SUMOReal thisSpeed, const std::string& thisLaneID, SUMOReal thisLanePos,
        const std::string& otherID, const Position& otherPos, SUMOReal otherSpeed, const std::string& otherLaneID, SUMOReal otherLanePos,
        std::map<std::string, SeenDevice*>& currentlySeen) {
    SUMOReal t = tEnd - currentlySeen.find(otherID)->second->lastView;
    if (sRecognitionRNG.rand() <= 1 - exp(-0.24 * pow(t, 2.68))) {
        currentlySeen.find(otherID)->second->lastView = tEnd;
        MeetingPoint* mp = new MeetingPoint(tEnd, thisPos, thisSpeed, thisLaneID, thisLanePos, otherPos, otherSpeed, otherLaneID, otherLanePos);
        std::map<std::string, SeenDevice*>::iterator i = currentlySeen.find(otherID);
        if (i != currentlySeen.end()) {
            (*i).second->recognitionPoints.push_back(mp);
        } else {
            WRITE_WARNING("Could not add a recognition point as the sender '" + otherID + "' is not currently seen.");
        }
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
MSDevice_BTreceiver::MSDevice_BTreceiver(SUMOVehicle& holder, const std::string& id,  SUMOReal range)
    : MSDevice(holder, id), myRange(range) {
}


MSDevice_BTreceiver::~MSDevice_BTreceiver() {
}


bool
MSDevice_BTreceiver::notifyEnter(SUMOVehicle& veh, Notification reason) {
    if (reason == MSMoveReminder::NOTIFICATION_DEPARTED && sVehicles.find(veh.getID()) == sVehicles.end()) {
        sVehicles[veh.getID()] = new VehicleInformation(veh.getID(), myRange);
    }
    sVehicles[veh.getID()]->updates.push_back(VehicleState(
                MSNet::getInstance()->getCurrentTimeStep(), veh.getSpeed(), static_cast<MSVehicle&>(veh).getAngle(), static_cast<MSVehicle&>(veh).getPosition(), static_cast<MSVehicle&>(veh).getLane()->getID(), veh.getPositionOnLane()
            ));
    return true;
}


bool
MSDevice_BTreceiver::notifyMove(SUMOVehicle& veh, SUMOReal /* oldPos */, SUMOReal newPos, SUMOReal newSpeed) {
    if (sVehicles.find(veh.getID()) == sVehicles.end()) {
        WRITE_WARNING("btreceiver: Can not update position of a vehicle that is not within the road network (" + veh.getID() + ").");
        return true;
    }
    sVehicles[veh.getID()]->updates.push_back(VehicleState(
                MSNet::getInstance()->getCurrentTimeStep(), newSpeed, static_cast<MSVehicle&>(veh).getAngle(), static_cast<MSVehicle&>(veh).getPosition(), static_cast<MSVehicle&>(veh).getLane()->getID(), newPos
            ));
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
    sVehicles[veh.getID()]->updates.push_back(VehicleState(
                MSNet::getInstance()->getCurrentTimeStep(), veh.getSpeed(), static_cast<MSVehicle&>(veh).getAngle(), static_cast<MSVehicle&>(veh).getPosition(), static_cast<MSVehicle&>(veh).getLane()->getID(), veh.getPositionOnLane()
            ));
    if (reason >= MSMoveReminder::NOTIFICATION_TELEPORT) {
        sVehicles[veh.getID()]->amOnNet = false;
    }
    if (reason >= MSMoveReminder::NOTIFICATION_ARRIVED) {
        sVehicles[veh.getID()]->haveArrived = true;
    }
    return true;
}





/****************************************************************************/

