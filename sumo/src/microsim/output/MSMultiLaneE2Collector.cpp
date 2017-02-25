/****************************************************************************/
/// @file    MSMultiLaneE2Collector.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Robbin Blokpoel
/// @author  Jakob Erdmann
/// @author  Leonhard Luecken
/// @date    Mon Feb 03 2014 10:13 CET
/// @version $Id: MSMultiLaneE2Collector.cpp 22841 2017-02-03 22:10:57Z luecken $
///
// An areal detector corresponding to a sequence of consecutive lanes
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


/* TODO:
 * friendly position?
 * tests:
 *  - many vehicles
 *  - subsecond variant, ballistic variant
 *  - Should pass the original E2-tests
 *
 *
 * Meso-compatibility? (esp. enteredLane-argument for MSBaseVehicle::notifyEnter() is not treated)
 * Compatibility without internal lanes?
 * Include leftVehicles into output?
*/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cassert>
#include <algorithm>
#include "MSMultiLaneE2Collector.h"
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleType.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

#define DEBUG_LSD_NOTIFY_MOVE
#define DEBUG_LSD_CONSTRUCTOR
#define DEBUG_LSD_NOTIFY_ENTER_AND_LEAVE
#define DEBUG_LSD_MAKE_VEHINFO
#define DEBUG_LSD_DETECTOR_UPDATE
#define DEBUG_LSD_TIME_ON_DETECTOR
#define DEBUG_LSD_JAMS


MSMultiLaneE2Collector::MSMultiLaneE2Collector(const std::string& id,
        DetectorUsage usage, MSLane* lane, SUMOReal startPos, SUMOReal endPos, SUMOReal length,
        SUMOReal haltingTimeThreshold, SUMOReal haltingSpeedThreshold, SUMOReal jamDistThreshold,
        const std::string& vTypes, bool /* friendlyPositioning */, bool /*showDetector*/) :
    MSMoveReminder(id, lane, false),
    MSDetectorFileOutput(id, vTypes),
    myUsage(usage),
    myJamHaltingSpeedThreshold(haltingSpeedThreshold),
    myJamHaltingTimeThreshold(haltingTimeThreshold),
    myJamDistanceThreshold(jamDistThreshold),
    myCurrentOccupancy(0),
    myCurrentMeanSpeed(-1),
    myCurrentJamNo(0),
    myCurrentMaxJamLengthInMeters(0),
    myCurrentMaxJamLengthInVehicles(0),
    myCurrentJamLengthInMeters(0),
    myCurrentJamLengthInVehicles(0),
    myCurrentStartedHalts(0),
    myCurrentHaltingsNumber(0)
    {

#ifdef DEBUG_LSD_CONSTRUCTOR
    std::cout << "\n" << "Creating MSMultiLaneE2Collector " << id
                    << " with lane = " << lane->getID()
                    << " startPos = " << startPos
                    << " endPos = " << endPos
                    << " length = " << length
                    << " haltingTimeThreshold = " << haltingTimeThreshold
                    << " haltingSpeedThreshold = " << haltingSpeedThreshold
                    << " jamDistThreshold = " << jamDistThreshold
                    << std::endl;
#endif

    assert(lane != 0);

    // check that exactly one of length, startPos, endPos is invalid
    bool lengthInvalid = length == std::numeric_limits<SUMOReal>::max();
    bool endPosInvalid = endPos == std::numeric_limits<SUMOReal>::max();
    bool posInvalid = startPos == std::numeric_limits<SUMOReal>::max();
    int numInvalid = (int) lengthInvalid + (int) endPosInvalid + (int) posInvalid;
    assert(numInvalid == 1);

    // check and normalize positions (assure positive values for pos and endPos, snap to lane-ends)
    if (lengthInvalid) {
        // assume that the detector is only located on a single lane
        endPos = endPos < 0 ? lane->getLength() - endPos : endPos;
        startPos = startPos < 0 ? lane->getLength() - startPos : startPos;
        bool valid = endPos <= lane->getLength() && 0 <= startPos && startPos < endPos;
        if (!valid){
            WRITE_ERROR("Error in specification for E2Detector '" + id + "'. Positional argument is malformed. 0 <= pos < endPos <= lane.getLength() is required.");
        }
        if (fabs(lane->getLength() - endPos) < POSITION_EPS){
            endPos = lane->getLength();
        }
        if (startPos < POSITION_EPS){
            startPos = 0;
        }
        length = endPos - startPos;
    } else if (posInvalid) {
        // endPosInvalid == false
        endPos = endPos < 0 ? lane->getLength() - endPos : endPos;
        if (fabs(lane->getLength() - endPos) < POSITION_EPS){
            endPos = lane->getLength();
        }
    } else {
        // posInvalid == false
        startPos = startPos < 0 ? lane->getLength() - startPos : startPos;
        if (startPos < POSITION_EPS){
            startPos = 0;
        }
    }

    myStartPos = startPos;
    myEndPos = endPos;

    std::vector<MSLane*> lanes;
    if (posInvalid) {
        lanes = selectLanes(lane, length, "bw");
    } else if (endPosInvalid) {
        lanes = selectLanes(lane, length, "fw");
    } else {
        // assuming detector is only located at a single lane
        lanes.push_back(lane);
    }

    initAuxiliaries(lanes);

    // check if detector was truncated
    if (myDetectorLength < length - NUMERICAL_EPS) {
        std::stringstream ss;
        ss << "Cannot build detector of length " << length
                << " because no further continuation lane was found for lane '" << lanes[lanes.size()-1]->getID()
                << "'! Truncated detector at length " << myDetectorLength << ".";
        WRITE_WARNING(ss.str());
    }

    assert((myStartPos >= POSITION_EPS || myStartPos == 0) && myStartPos < myFirstLane->getLength());
    assert(myEndPos > POSITION_EPS && myEndPos <= myLastLane->getLength());
    assert(myFirstLane != myLastLane || myEndPos - myStartPos > 0);

    addDetectorToLanes(lanes);
}

//void
//MSMultiLaneE2Collector::adjustEndPos(const MSLane* lane, SUMOReal& endPos, SUMOReal& length) const {
//    // Adjust end position if closer than POSITION_EPS to the end of the lane
//    assert(endPos > 0);
//    assert(endPos <= lane->getLength());
//    if (lane->getLength() - endPos < POSITION_EPS){
//        WRITE_WARNING("Minimal endPos distance to lane end for E2 Detector '" + getID() + "' (" + POSITION_EPS + "m.) is violated. Setting endPos to lane end.");
//        length += lane->getLength() - endPos;
//        endPos = lane->getLength();
//    } else if (endPos < POSITION_EPS) {
//        WRITE_WARNING("Minimal endPos distance to lane begin for E2 Detector '" + getID() + "' (" + POSITION_EPS + "m.) is violated. Setting endPos to previous lane end.");
//        length += POSITION_EPS - endPos;
//        endPos = MIN2(POSITION_EPS, lane->getLength());
//    }
//}
//
//void
//MSMultiLaneE2Collector::adjustLength(const MSLane* lane, SUMOReal& startPos, SUMOReal& endPos, SUMOReal& length) const {
//    // Adjust positional values if length < POS_EPSILON or 0 < startPos < POS_EPSILON
//    assert(startPos >= 0);
//    assert(length >= 0);
//    if (length < POSITION_EPS) {
//        WRITE_WARNING("Minimal length for E2 Detector '" + getID() + "' (" + POSITION_EPS + "m.) is violated. User specification may be overridden.");
//        // Restrict detector extent to a single lane
//        endPos = endPos < 0 ? lane->getLength() - endPos : endPos;
//        if (lane->getLength() > POSITION_EPS) {
//            // there's some room
//            length = POSITION_EPS;
//            endPos = startPos + POSITION_EPS;
//            if (endPos > lane->getLength()) {
//                SUMOReal off = lane->getLength() - endPos;
//                endPos = lane->getLength();
//                startPos -= off;
//            }
//        } else {
//            // full lane covered
//            length = lane->getLength();
//            startPos = 0;
//            endPos = length;
//            return;
//        }
//    }
//
//    if (startPos < POSITION_EPS) {
//        WRITE_WARNING("Minimal startPos for E2 Detector '" + getID() + "' (" + POSITION_EPS + "m.) is violated. Setting startPos to lane start.");
//        length += startPos;
//        startPos = 0.;
//    }
//}


MSMultiLaneE2Collector::MSMultiLaneE2Collector(const std::string& id,
        DetectorUsage usage, std::vector<MSLane*> lanes, SUMOReal startPos, SUMOReal endPos,
        SUMOReal haltingTimeThreshold, SUMOReal haltingSpeedThreshold, SUMOReal jamDistThreshold,
        const std::string& vTypes, bool /* friendlyPositioning */, bool /*showDetector*/) :
    MSMoveReminder(id, lanes[lanes.size()-1], false), // assure that lanes.size() > 0 at caller side!!!
    MSDetectorFileOutput(id, vTypes),
    myStartPos(startPos),
    myEndPos(endPos),
    myUsage(usage),
    myJamHaltingSpeedThreshold(haltingSpeedThreshold),
    myJamHaltingTimeThreshold(haltingTimeThreshold),
    myJamDistanceThreshold(jamDistThreshold),
    myCurrentOccupancy(0),
    myCurrentMeanSpeed(-1),
    myCurrentJamNo(0),
    myCurrentMaxJamLengthInMeters(0),
    myCurrentMaxJamLengthInVehicles(0),
    myCurrentJamLengthInMeters(0),
    myCurrentJamLengthInVehicles(0),
    myCurrentStartedHalts(0),
    myCurrentHaltingsNumber(0)
    {

    for (std::vector<MSLane*>::const_iterator i = lanes.begin(); i != lanes.end(); ++i){
        assert((*i) != 0);
    }

#ifdef DEBUG_LSD_CONSTRUCTOR
    std::cout << "\n" << "Creating MSMultiLaneE2Collector " << id
                    << " with endLane = " << myLastLane->getID()
                    << " endPos = " << endPos
                    << " startLane = " << myFirstLane->getID()
                    << " startPos = " << startPos
                    << " haltingTimeThreshold = " << haltingTimeThreshold
                    << " haltingSpeedThreshold = " << haltingSpeedThreshold
                    << " jamDistThreshold = " << jamDistThreshold
                    << std::endl;
#endif

    myStartPos = myStartPos < 0 ? lanes[0]->getLength() - myStartPos : myStartPos;
    myEndPos = myEndPos < 0 ? lanes[lanes.size()-1]->getLength() - myEndPos : myEndPos;

    if(myStartPos < POSITION_EPS) {
        myStartPos = 0;
    }
    if(myEndPos > lanes[lanes.size()-1]->getLength() - POSITION_EPS) {
        myEndPos = lanes[lanes.size()-1]->getLength();
    }


    initAuxiliaries(lanes);

    assert((myStartPos >= POSITION_EPS || myStartPos == 0) && myStartPos < myFirstLane->getLength());
    assert(myEndPos > POSITION_EPS && myEndPos <= myLastLane->getLength());
    assert(myFirstLane != myLastLane || myEndPos - myStartPos > 0);

    addDetectorToLanes(lanes);
}




std::vector<MSLane*>
MSMultiLaneE2Collector::selectLanes(MSLane* lane, SUMOReal length, std::string dir){
    // direction of detector extension
    assert(dir == "fw" || dir == "bw");
    bool fw = dir == "fw";

#ifdef DEBUG_LSD_CONSTRUCTOR
    std::cout << "\n" << "selectLanes()" << std::endl;
#endif
    std::vector<MSLane*> lanes;
    // Selected lanes are stacked into vector 'lanes'. If dir == "bw" lanes will be reversed after this is done.
    // The length is reduced while adding lanes to the detector
    // adjust starting value for length (in the first iteration, the whole length of the first considered lane is substracted,
    // while it might only be partially covered by the detector)
    if (fw) {
        length += lane->getLength() - myEndPos;
    } else {
        length += myStartPos;
    }
    length = MAX2(POSITION_EPS, length); // this assures to add at least one lane to lanes
    while(length >= NUMERICAL_EPS && lane != 0){
        // Break loop for length <= NUMERICAL_EPS to avoid placement of very small
        // detector piece on the end or beginning of one lane due to numerical rounding errors.
        lanes.push_back(lane);
#ifdef DEBUG_LSD_CONSTRUCTOR
        std::cout << "Added lane " << lane->getID()
                << " (length: "<< lane->getLength() << ")" << std::endl;
#endif
        length -= lane->getLength();

        // proceed to upstream predecessor
        if (fw) {
            lane = lane->getCanonicalSuccessorLane();
        } else {
            lane = lane->getCanonicalPredecessorLane();
        }

#ifdef DEBUG_LSD_CONSTRUCTOR
        if (lane!=0) {
            std::cout << (fw ? "Successor lane: " : "Predecessor lane: ") << "'" << lane->getID() << "'";
        }
        std::cout << std::endl;
#endif
    }

    // reverse lanes if lane selection was backwards
    if (!fw) {
        std::reverse(lanes.begin(), lanes.end());
    }

    return lanes;
}

void
MSMultiLaneE2Collector::addDetectorToLanes(std::vector<MSLane*>& lanes) {
#ifdef DEBUG_LSD_CONSTRUCTOR
    std::cout << "\n" << "Adding detector " << myID << " to lanes:" << std::endl;
#endif
    for (std::vector<MSLane*>::iterator l = lanes.begin(); l != lanes.end(); ++l){
        (*l)->addMoveReminder(this);
#ifdef DEBUG_LSD_CONSTRUCTOR
    std::cout << (*l)->getID() << std::endl;
#endif
    }
}

void
MSMultiLaneE2Collector::initAuxiliaries(std::vector<MSLane*>& lanes){
    // Checks integrity of myLanes, adds internal-lane information, inits myLength, myFirstLane, myLastLane, myOffsets
    myFirstLane = lanes[0];
    myLastLane = lanes[lanes.size()-1];

#ifdef DEBUG_LSD_CONSTRUCTOR
    std::cout << "\n" << "Initializing auxiliaries:"
            << "\nFirst lane: " << myFirstLane->getID() << " (startPos = " << myStartPos << ")"
            << "\nLast lane: " << myLastLane->getID() << " (endPos = " << myEndPos << ")"
            << std::endl;
#endif

    // Init myOffsets and myDetectorLength.
    // The loop below runs through the given lanes assuming the possibility that only non-internal lanes are given
    // or at least not all relevant internal lanes are considered. During this a new, complete list of lane ids is
    // built into myLanes.
    myLanes.clear();

    // myDetectorLength will be increased in the loop below, always giving
    // the offset of the currently considered lane to the detector start
    myDetectorLength = -myStartPos;
    myOffsets.clear();

    // loop over detector lanes and accumulate offsets with respect to the first lane's begin
    // (these will be corrected afterwards by substracting the start position.)
    std::vector<MSLane*>::iterator il = lanes.begin();

    // start on an internal lane?
    // (This may happen if specifying the detector by its upstream
    //  length starting from a given end position)
    const MSLane* internal = (*il)->isInternal() ? *il : 0;

#ifdef DEBUG_LSD_CONSTRUCTOR
    std::cout << "\n" << "Initializing offsets:"<<std::endl;
#endif

    while (true) {
        // Consider the next internal lanes
        while(internal != 0){
            myLanes.push_back(internal->getID());
            myOffsets.push_back(myDetectorLength);

#ifdef DEBUG_LSD_CONSTRUCTOR
    std::cout << "Offset for lane " << internal->getID() << " = " << myDetectorLength
            << std::endl;
#endif

            myDetectorLength += internal->getLength();
            if(internal->getID() == myLastLane->getID()){
                break;
            }

            // There should be a unique continuation for each internal lane
            assert(internal->getLinkCont().size()==1);

            internal = internal->getLinkCont()[0]->getViaLaneOrLane();
            if (!internal->isInternal()){
                // passed the junction
                internal = 0;
                break;
            }
        }

        // Consider the next non-internal lane
        // This is the first lane in the first iteration, if it is non-internal
        // However, it can equal myLanes.end() if the myLastLane is internal. In that case we break.

        // Move il to next non-internal
        while (il != lanes.end() && (*il)->isInternal()) il++;
        if (il == lanes.end()) break;

        // There is still a non-internal lane to consider
        MSLane* lane = *il;
        myLanes.push_back(lane->getID());

#ifdef DEBUG_LSD_CONSTRUCTOR
    std::cout << "Offset for lane " << lane->getID() << " = " << myDetectorLength
            << std::endl;
#endif

        // Offset to detector start for this lane
        myOffsets.push_back(myDetectorLength);

        // Add the lanes length to the detector offset
        myDetectorLength += lane->getLength();

        // Get the next lane if this lane isn't the last one
        if (++il == lanes.end()) break;

        if ((*il)->isInternal()) {
            // next lane in myLanes is internal
            internal = *il;
            continue;
        }

        // find the connection to next
        const MSLink* link = lane->getLinkTo(*il);
        assert(link!=0);

        if (!MSGlobals::gUsingInternalLanes){
            myDetectorLength += link->getLength();
        } else {
            internal = link->getViaLane();
        }
    }

    // Substract distance not covered on the last lane
    myDetectorLength -= myLastLane->getLength() - myEndPos;


#ifdef DEBUG_LSD_CONSTRUCTOR
    std::cout << "Total detector length = "<< myDetectorLength << std::endl;
#endif
}





bool
MSMultiLaneE2Collector::notifyMove(SUMOVehicle& veh, SUMOReal oldPos,
                          SUMOReal newPos, SUMOReal newSpeed) {
    VehicleInfoMap::iterator vi = myVehicleInfos.find(veh.getID());
    assert(vi != myVehicleInfos.end()); // all vehicles calling notifyMove() should have called notifyEnter() before

    const std::string& vehID = veh.getID();
    VehicleInfo& vehInfo = vi->second;

    // position relative to the detector start
    SUMOReal relPos = vehInfo.entryOffset + newPos;

    // update current distance to the detector end
    vehInfo.distToDetectorEnd = myDetectorLength - relPos;

#ifdef DEBUG_LSD_NOTIFY_MOVE
        std::cout << "\n" << SIMTIME
        << " MSMultiLaneE2Collector::notifyMove()"
        << " called by vehicle '" << vehID << "'"
        << " at relative position " << relPos
        << ", distToDetectorEnd = " << vehInfo.distToDetectorEnd << std::endl;
#endif

    // Check whether vehicle has reached the detector begin
    if (relPos <= 0) {
        // detector not yet reached, request being informed further
#ifdef DEBUG_LSD_NOTIFY_MOVE
        std::cout << "Vehicle has not yet reached the detector start position." << std::endl;
#endif
        return true;
    }

    myMoveNotifications.push_back(makeMoveNotification(veh, oldPos, newPos, newSpeed, vehInfo));

    // determine whether vehicle has moved beyond the detector's end
    bool vehPassedDetectorEnd = relPos - veh.getVehicleType().getLength() > vehInfo.exitOffset;

    if (vehPassedDetectorEnd) {
#ifdef DEBUG_LSD_NOTIFY_MOVE
        std::cout << "Vehicle has left the detector along a junction." << std::endl;
#endif
        // Vehicle is beyond the detector, unsubscribe and register removal from myVehicleInfos
        myLeftVehicles.insert(vehID);
        return false;
    } else {
        // Receive further notifications
        return true;
    }
}

bool
MSMultiLaneE2Collector::notifyLeave(SUMOVehicle& veh, SUMOReal /* lastPos */, MSMoveReminder::Notification reason, const MSLane* leftLane, const MSLane* enteredLane) {
#ifdef DEBUG_LSD_NOTIFY_ENTER_AND_LEAVE
        std::cout << "\n" << SIMTIME << " notifyLeave() called by vehicle '" << veh.getID() << "'" << std::endl;
#endif

    if (reason == MSMoveReminder::NOTIFICATION_JUNCTION) {
        // vehicle left lane via junction, unsubscription and registering in myLeftVehicles when
        // moving beyond the detector end is controlled in notifyMove.
#ifdef DEBUG_LSD_NOTIFY_ENTER_AND_LEAVE
        std::cout << SIMTIME << " Left longitudinally (along junction) -> keep subscription [handle exit in notifyMove()]" << std::endl;
#endif

        if (std::find(myLanes.begin(), myLanes.end(), enteredLane->getID()) == myLanes.end()){
            // Entered lane is not part of the detector
            VehicleInfoMap::iterator vi = myVehicleInfos.find(veh.getID());
            // Determine exit offset, where vehicle left the detector
            SUMOReal exitOffset = myOffsets[vi->second.currentOffsetIndex] + vi->second.currentLane->getLength();
            vi->second.exitOffset = MIN2(vi->second.exitOffset, exitOffset);
#ifdef DEBUG_LSD_NOTIFY_ENTER_AND_LEAVE
        std::cout << SIMTIME << " Vehicle '" << veh.getID() << "' leaves the detector. Exit offset = " << exitOffset << std::endl;
#endif
        }

        return true;
    } else {
        VehicleInfoMap::iterator vi = myVehicleInfos.find(veh.getID());
        // erase vehicle, which leaves in a non-longitudinal way, immediately
        myVehicleInfos.erase(vi);
        myNumberOfLeftVehicles++;
#ifdef DEBUG_LSD_NOTIFY_ENTER_AND_LEAVE
        std::cout << SIMTIME << " Left non-longitudinally (lanechange, teleport, parking, etc) -> discard subscription" << std::endl;
#endif
        return false;
    }
}


bool
MSMultiLaneE2Collector::notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification /* reason */, const MSLane* enteredLane) {
#ifdef DEBUG_LSD_NOTIFY_ENTER_AND_LEAVE
        std::cout << "\n" << SIMTIME << " notifyEnter() called by vehicle '" << veh.getID()
                << "' entering lane '" << (enteredLane!=0 ? enteredLane->getID() : "NULL") << "'" << std::endl;
#endif

    // notifyEnter() should only be called for lanes of the detector
    assert(std::find(myLanes.begin(), myLanes.end(), enteredLane->getID()) != myLanes.end());

    if (!veh.isOnRoad()) {
        // Vehicle is teleporting over the edge
#ifdef DEBUG_LSD_NOTIFY_ENTER_AND_LEAVE
        std::cout << "Vehicle is off road (teleporting over edge)..." << std::endl;
#endif
//        return false;
    }
    if (!vehicleApplies(veh)) {
        // That's not my type...
        return false;
    }
    const std::string& vehID = veh.getID();
    VehicleInfoMap::iterator vi = myVehicleInfos.find(vehID);
    if (vi != myVehicleInfos.end()) {
#ifdef DEBUG_LSD_NOTIFY_ENTER_AND_LEAVE
        std::cout << SIMTIME << " Vehicle '" << veh.getID() << "' already known. No new VehicleInfo is created." << std::endl;
#endif

        // Register move current offset to the next lane
        vi->second.currentOffsetIndex++;
        vi->second.currentLane = enteredLane;
        assert(myLanes[vi->second.currentOffsetIndex] == enteredLane->getID());

        // but don't add a second subscription for another lane
        return false;
    }

#ifdef DEBUG_LSD_NOTIFY_ENTER_AND_LEAVE
        std::cout << SIMTIME << " Adding VehicleInfo for vehicle '" << veh.getID() << "'." << std::endl;
#endif

    // Add vehicle info
    myVehicleInfos.insert(std::make_pair(vehID, makeVehicleInfo(veh, enteredLane)));
    myNumberOfEnteredVehicles++;
    // Subscribe to vehicle's movement notifications
    return true;
}

MSMultiLaneE2Collector::VehicleInfo
MSMultiLaneE2Collector::makeVehicleInfo(const SUMOVehicle& veh, const MSLane* enteredLane) const {
    // The vehicle's distance to the detector end
    std::size_t j = std::find(myLanes.begin(), myLanes.end(), enteredLane->getID()) - myLanes.begin();
    assert(j >= 0 && j < myLanes.size());
    SUMOReal entryOffset = myOffsets[j];
    SUMOReal distToDetectorEnd = myDetectorLength - (entryOffset + veh.getPositionOnLane());
    bool onDetector = entryOffset < veh.getPositionOnLane() && distToDetectorEnd > -veh.getVehicleType().getLength();

#ifdef DEBUG_LSD_MAKE_VEHINFO
    std::cout << SIMTIME << " Making VehicleInfo for vehicle '" << veh.getID() << "'."
            << "\ndistToDetectorEnd = " << distToDetectorEnd
            << "\nveh.getPositionOnLane() = " << veh.getPositionOnLane()
            << "\nentry lane offset (lane begin from detector begin) = " << entryOffset
            << std::endl;
#endif
    return VehicleInfo(veh.getID(), veh.getVehicleType().getID(), veh.getVehicleType().getLength(), veh.getVehicleType().getMinGap(), enteredLane, entryOffset, j, myDetectorLength, distToDetectorEnd, onDetector);
}

void
MSMultiLaneE2Collector::detectorUpdate(const SUMOTime /* step */) {

#ifdef DEBUG_LSD_DETECTOR_UPDATE
    std::cout << "\n" << SIMTIME << " detectorUpdate() for detector '" << myID << "'" << std::endl;
#endif

    // sort myMoveNotifications (required for jam processing) ascendingly according to vehicle's distance to the detector end
    // (min = myMoveNotifications[0].distToDetectorEnd)
    std::sort(myMoveNotifications.begin(), myMoveNotifications.end(), compareMoveNotification);

    // reset values concerning current time step (these are updated in integrateMoveNotification() and aggregateOutputValues())
    myCurrentMeanSpeed = 0;
    myCurrentMeanLength = 0;
    myCurrentStartedHalts = 0;
    myCurrentHaltingsNumber = 0;

    JamInfo* currentJam = 0;
    std::vector<JamInfo*> jams;
    std::map<std::string, SUMOTime> haltingVehicles;
    std::map<std::string, SUMOTime> intervalHaltingVehicles;

    // go through the list of vehicles positioned on the detector
    for (std::vector<MoveNotificationInfo>::iterator i = myMoveNotifications.begin(); i != myMoveNotifications.end(); ++i) {
        // The ID of the vehicle that has sent this notification in the last step
        const std::string& vehID = i->id;
        VehicleInfoMap::iterator vi = myVehicleInfos.find(vehID);

        if (vi == myVehicleInfos.end()) {
            // The vehicle has already left the detector by lanechange, teleport, etc. (not longitudinal)
            integrateMoveNotification(0, *i);
        } else {
            // Add move notification infos to detector values and VehicleInfo
            integrateMoveNotification(&vi->second, *i);
        }
        // construct jam structure
        bool isInJam = checkJam(i, haltingVehicles, intervalHaltingVehicles);
        buildJam(isInJam, i, currentJam, jams);
    }

    // extract some aggregated values from the jam structure
    processJams(jams, currentJam);

    // Aggregate and normalize values for the detector output
    aggregateOutputValues();

    // save information about halting vehicles
    myHaltingVehicleDurations = haltingVehicles;
    myIntervalHaltingVehicleDurations = intervalHaltingVehicles;

#ifdef DEBUG_LSD_DETECTOR_UPDATE
    std::cout << "\n" << SIMTIME << " Current lanes for vehicles still on the detector:" << std::endl;
#endif
    // update current and entered lanes for remaining vehicles
    VehicleInfoMap::iterator iv;
    for (iv = myVehicleInfos.begin(); iv != myVehicleInfos.end(); ++iv) {
#ifdef DEBUG_LSD_DETECTOR_UPDATE
        std::cout << " Vehicle '" << iv->second.id << "'" << ": '"
            << iv->second.currentLane->getID() << "'"
            << std::endl;
#endif
    }

#ifdef DEBUG_LSD_DETECTOR_UPDATE
    std::cout << SIMTIME << " Discarding vehicles that have left the detector:" << std::endl;
#endif
    // Remove the vehicles that have left the detector
    std::set<std::string>::const_iterator i;
    for(i = myLeftVehicles.begin(); i != myLeftVehicles.end(); ++i) {
        VehicleInfoMap::iterator j = myVehicleInfos.find(*i);
        myVehicleInfos.erase(*i);
        myNumberOfLeftVehicles++;
#ifdef DEBUG_LSD_DETECTOR_UPDATE
        std::cout << "Erased vehicle '" << *i << "'" << std::endl;
#endif
    }
    myLeftVehicles.clear();

    // reset move notifications
    myMoveNotifications.clear();
}


void
MSMultiLaneE2Collector::aggregateOutputValues(){
    myTimeSamples += 1;
    // compute occupancy values (note myCurrentMeanLength is still not normalized here, but holds the sum of all vehicle lengths)
    SUMOReal currentOccupancy = myCurrentMeanLength / myDetectorLength * (SUMOReal) 100.;
    myCurrentOccupancy = currentOccupancy;
    myOccupancySum += currentOccupancy;
    myMaxOccupancy = MAX2(myMaxOccupancy, currentOccupancy);
    // compute jam values
    myMeanMaxJamInVehicles += myCurrentMaxJamLengthInVehicles;
    myMeanMaxJamInMeters += myCurrentMaxJamLengthInMeters;
    myMaxJamInVehicles = MAX2(myMaxJamInVehicles, myCurrentMaxJamLengthInVehicles);
    myMaxJamInMeters = MAX2(myMaxJamInMeters, myCurrentMaxJamLengthInMeters);
    // compute information about vehicle numbers
    const size_t numVehicles = myMoveNotifications.size();
    myMeanVehicleNumber += numVehicles;
    myMaxVehicleNumber = MAX2(numVehicles, myMaxVehicleNumber);
    // norm current values
    myCurrentMeanSpeed = numVehicles != 0 ? myCurrentMeanSpeed / (SUMOReal) numVehicles : -1;
    myCurrentMeanLength = numVehicles != 0 ? myCurrentMeanLength / (SUMOReal) numVehicles : -1;
}



void
MSMultiLaneE2Collector::integrateMoveNotification(VehicleInfo* vi, const MoveNotificationInfo& mni){

#ifdef DEBUG_LSD_DETECTOR_UPDATE
    std::cout << SIMTIME << " integrateMoveNotification() for vehicle '" << mni.id << "'" << std::endl;
#endif

    // Accumulate detector values
    myVehicleSamples += mni.timeOnDetector;
    myTotalTimeLoss += mni.timeLoss;
    mySpeedSum += mni.speed * mni.timeOnDetector;
    myCurrentMeanSpeed += mni.speed * mni.timeOnDetector;
    myCurrentMeanLength += mni.lengthOnDetector;

    if (vi!=0){
        // Accumulate individual values for the vehicle.
        // @note vi==0 occurs, if the vehicle info has been erased at
        //       notifyLeave() in case of a non-longitudinal exit (lanechange, teleport, etc.)
        vi->totalTimeOnDetector += mni.timeOnDetector;
        vi->accumulatedTimeLoss += mni.timeLoss;
        vi->lastAccel = mni.accel;
        vi->lastSpeed = mni.speed;
        vi->lastPos = myStartPos + vi->entryOffset + mni.newPos;
        vi->onDetector= mni.onDetector;
    }
}



MSMultiLaneE2Collector::MoveNotificationInfo
MSMultiLaneE2Collector::makeMoveNotification(const SUMOVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed, const VehicleInfo& vehInfo) const {
#ifdef DEBUG_LSD_NOTIFY_MOVE
    std::cout << SIMTIME << " makeMoveNotification() for vehicle '" << veh.getID() << "'"
            << " oldPos = " << oldPos << " newPos = " << newPos << " newSpeed = " << newSpeed
            << std::endl;
#endif

    // Timefraction in [0,TS] the vehicle has spend on the detector in the last step
    SUMOReal timeOnDetector = calculateTimeOnDetector(veh, oldPos, newPos, vehInfo);

    // Note that at this point, vehInfo.currentLane points to the lane at the beginning of the last timestep,
    // and vehInfo.enteredLanes is a list of lanes entered in the last timestep
    SUMOReal timeLoss = calculateTimeLoss(veh, oldPos, newPos, vehInfo);

    // The length of the part of the vehicle on the detector at the end of the last time step
    // XXX: Fulfulling the specifications of the documentation (lengthOnDetector = time integral
    //      over length of the vehicle's part on the detector) would be much more cumbersome.
    // may be shorter than vehicle's length if its back reaches out
    SUMOReal lengthOnDetector = MAX2(MIN2(vehInfo.length, newPos + vehInfo.entryOffset), 0.);
    SUMOReal distToExit = vehInfo.exitOffset - vehInfo.entryOffset - newPos;
    // Eventually decrease further to account for the front reaching out
    lengthOnDetector = MAX2(0., lengthOnDetector - MAX2(0., distToExit));

    // whether the vehicle is still on the detector at the end of the time step
    bool stillOnDetector = -distToExit < vehInfo.length;

#ifdef DEBUG_LSD_NOTIFY_MOVE
    std::cout << SIMTIME << " lengthOnDetector = " << lengthOnDetector << std::endl;
#endif

    /* Store new infos */
    return MoveNotificationInfo(veh.getID(), oldPos, newPos, newSpeed, veh.getAcceleration(), myDetectorLength - (vehInfo.entryOffset + newPos), timeOnDetector, lengthOnDetector, timeLoss, stillOnDetector);
}

void
MSMultiLaneE2Collector::buildJam(bool isInJam, std::vector<MoveNotificationInfo>::const_iterator mni, JamInfo*& currentJam, std::vector<JamInfo*>& jams){
#ifdef DEBUG_LSD_JAMS
    std::cout << SIMTIME << " buildJam() for vehicle '" << mni->id << "'" << std::endl;
#endif
    if (isInJam) {
        // The vehicle is in a jam;
        //  it may be a new one or already an existing one
        if (currentJam == 0) {
#ifdef DEBUG_LSD_JAMS
    std::cout << SIMTIME << " vehicle '" << mni->id << "' forms the start of the first jam" << std::endl;
#endif
            // the vehicle is the first vehicle in a jam
            currentJam = new JamInfo();
            currentJam->firstStandingVehicle = mni;
        } else {
            // ok, we have a jam already. But - maybe it is too far away
            //  ... honestly, I can hardly find a reason for doing this,
            //  but jams were defined this way in an earlier version...
            if (currentJam->lastStandingVehicle->distToDetectorEnd - mni->distToDetectorEnd > myJamDistanceThreshold) {
#ifdef DEBUG_LSD_JAMS
    std::cout << SIMTIME << " vehicle '" << mni->id << "' forms the start of a new jam" << std::endl;
#endif
                // yep, yep, yep - it's a new one...
                //  close the frist, build a new
                jams.push_back(currentJam);
                currentJam = new JamInfo();
                currentJam->firstStandingVehicle = mni;
            }
        }
        currentJam->lastStandingVehicle = mni;
    } else {
        // the vehicle is not part of a jam...
        //  maybe we have to close an already computed jam
        if (currentJam != 0) {
#ifdef DEBUG_LSD_JAMS
            std::cout << SIMTIME << " Closing current jam." << std::endl;
#endif
            jams.push_back(currentJam);
            currentJam = 0;
        }
    }
}


bool
MSMultiLaneE2Collector::checkJam(std::vector<MoveNotificationInfo>::const_iterator mni, std::map<std::string, SUMOTime>& haltingVehicles, std::map<std::string, SUMOTime>& intervalHaltingVehicles){
#ifdef DEBUG_LSD_JAMS
    std::cout << SIMTIME << " CheckJam() for vehicle '" << mni->id << "'" << std::endl;
#endif
    // jam-checking begins
    bool isInJam = false;
    // first, check whether the vehicle is slow enough to be counted as halting
    if (mni->speed < myJamHaltingSpeedThreshold) {
        myCurrentHaltingsNumber++;
        // we have to track the time it was halting;
        // so let's look up whether it was halting before and compute the overall halting time
        bool wasHalting = myHaltingVehicleDurations.count(mni->id) > 0;
        if (wasHalting) {
            haltingVehicles[mni->id] = myHaltingVehicleDurations[mni->id] + DELTA_T;
            intervalHaltingVehicles[mni->id] = myIntervalHaltingVehicleDurations[mni->id] + DELTA_T;
        } else {
#ifdef DEBUG_LSD_JAMS
    std::cout << SIMTIME << " vehicle '" << mni->id << "' starts halting." << std::endl;
#endif
            haltingVehicles[mni->id] = DELTA_T;
            intervalHaltingVehicles[mni->id] = DELTA_T;
            myCurrentStartedHalts++;
            myStartedHalts++;
        }
        // we now check whether the halting time is large enough
        if (haltingVehicles[mni->id] > myJamHaltingTimeThreshold) {
            // yep --> the vehicle is a part of a jam
            isInJam = true;
        }
    } else {
        // is not standing anymore; keep duration information
        std::map<std::string, SUMOTime>::iterator v = myHaltingVehicleDurations.find(mni->id);
        if (v != myHaltingVehicleDurations.end()) {
            myPastStandingDurations.push_back(v->second);
            myHaltingVehicleDurations.erase(v);
        }
        v = myIntervalHaltingVehicleDurations.find(mni->id);
        if (v != myIntervalHaltingVehicleDurations.end()) {
            myPastIntervalStandingDurations.push_back((*v).second);
            myIntervalHaltingVehicleDurations.erase(v);
        }
    }
#ifdef DEBUG_LSD_JAMS
    std::cout << SIMTIME << " vehicle '" << mni->id << "'" << (isInJam ? "is jammed." : "is not jammed.") << std::endl;
#endif
    return isInJam;
}


void
MSMultiLaneE2Collector::processJams(std::vector<JamInfo*>& jams, JamInfo* currentJam){
    // push last jam
    if (currentJam != 0) {
        jams.push_back(currentJam);
        currentJam = 0;
    }

#ifdef DEBUG_LSD_JAMS
    std::cout << "\n" << SIMTIME << " processJams()"
            << "\nNumber of jams: " << jams.size() << std::endl;
#endif

    // process jam information
    myCurrentMaxJamLengthInMeters = 0;
    myCurrentMaxJamLengthInVehicles = 0;
    myCurrentJamLengthInMeters = 0;
    myCurrentJamLengthInVehicles = 0;
    for (std::vector<JamInfo*>::const_iterator i = jams.begin(); i != jams.end(); ++i) {
        // compute current jam's values
        const SUMOReal jamLengthInMeters = - (*i)->firstStandingVehicle->distToDetectorEnd
                                           + (*i)->lastStandingVehicle->distToDetectorEnd
                                           + (*i)->lastStandingVehicle->lengthOnDetector;
        const int jamLengthInVehicles = (int) distance((*i)->firstStandingVehicle, (*i)->lastStandingVehicle) + 1;
        // apply them to the statistics
        myCurrentMaxJamLengthInMeters = MAX2(myCurrentMaxJamLengthInMeters, jamLengthInMeters);
        myCurrentMaxJamLengthInVehicles = MAX2(myCurrentMaxJamLengthInVehicles, jamLengthInVehicles);
        myJamLengthInMetersSum += jamLengthInMeters;
        myJamLengthInVehiclesSum += jamLengthInVehicles;
        myCurrentJamLengthInMeters += jamLengthInMeters;
        myCurrentJamLengthInVehicles += jamLengthInVehicles;
#ifdef DEBUG_LSD_JAMS
        std::cout << SIMTIME << " processing jam nr." << ((int) distance((std::vector<JamInfo*>::const_iterator) jams.begin(), i) + 1)
                << "\njamLengthInMeters = " << jamLengthInMeters
                << " jamLengthInVehicles = " << jamLengthInVehicles
                << std::endl;
#endif
    }
    myCurrentJamNo = (int) jams.size();

    // clean up jam structure
    for (std::vector<JamInfo*>::iterator i = jams.begin(); i != jams.end(); ++i) {
        delete *i;
    }
}

SUMOReal
MSMultiLaneE2Collector::calculateTimeOnDetector(const SUMOVehicle& veh, SUMOReal oldPos, SUMOReal newPos, const VehicleInfo& vi) const {
    assert(veh.getID() == vi.id);
    assert(newPos + vi.entryOffset >= 0);

    if (oldPos == newPos) {
        // vehicle is stopped
        return TS;
    }

    // Eventual positional offset of the detector start from the lane's start
    SUMOReal entryPos = MAX2(-vi.entryOffset,0.);

    // Time of this vehicle entering the detector in the last time step
    SUMOReal entryTime = 0;

    // Take into account the time before entering the detector, if there is.
    if (oldPos < entryPos) {
        // Vehicle entered the detector in the last step, either traversing the detector start or somewhere in the middle.
        entryTime = MSCFModel::passingTime(oldPos, entryPos, newPos, veh.getPreviousSpeed(), veh.getSpeed());
    }

    // Calculate time spent on detector until reaching newPos or a detector exit
    SUMOReal exitPos = MIN2(newPos, vi.exitOffset + vi.length - vi.entryOffset);
    assert(entryPos < exitPos);
    // calculate vehicle's time spent on the detector
    SUMOReal exitTime = MSCFModel::passingTime(oldPos, exitPos, newPos, veh.getPreviousSpeed(), veh.getSpeed());
    // Time spent on the detector
    SUMOReal timeOnDetector = exitTime - entryTime;

#ifdef DEBUG_LSD_TIME_ON_DETECTOR
    std::cout << SIMTIME << " calculateTimeOnDetector() for vehicle '" << veh.getID() << "'"
            << " oldPos = " << oldPos << " newPos = " << newPos
            << " entryPos = " << entryPos << " exitPos = " << exitPos
            << " timeOnDetector = " << timeOnDetector
            << std::endl;
#endif

    return timeOnDetector;
}




SUMOReal
MSMultiLaneE2Collector::calculateTimeLoss(const SUMOVehicle& veh, SUMOReal oldPos, SUMOReal newPos, const VehicleInfo& vi) const {
    assert(veh.getID() == vi.id);
    assert(newPos + vi.entryOffset >= 0);

    if (oldPos == newPos) {
        // vehicle is stopped
        return TS;
    }

    // Eventual positional offset of the detector start from the lane's start
    SUMOReal entryPos = MAX2(-vi.entryOffset,0.);
    // Time of this vehicle entering the detector in the last time step
    SUMOReal entryTime = 0;
    // Take into account the time before entering the detector, if there is.
    if (oldPos < entryPos) {
        // Vehicle entered the detector in the last step, either traversing the detector start or somewhere in the middle.
        entryTime = MSCFModel::passingTime(oldPos, entryPos, newPos, veh.getPreviousSpeed(), veh.getSpeed());
    }
    // speed at detector entry
    SUMOReal entrySpeed = MSCFModel::speedAfterTime(entryTime, veh.getPreviousSpeed(), newPos-oldPos);
    // Calculate time spent on detector until reaching newPos or a detector exit
    SUMOReal exitPos = MIN2(newPos, vi.exitOffset + vi.length - vi.entryOffset);
    assert(entryPos < exitPos);
    // calculate vehicle's time spent on the detector
    SUMOReal exitTime = MSCFModel::passingTime(oldPos, exitPos, newPos, veh.getPreviousSpeed(), veh.getSpeed());
    // Vehicle's Speed when leaving the detector
    SUMOReal exitSpeed = MSCFModel::speedAfterTime(exitTime, veh.getPreviousSpeed(), newPos-oldPos);

    // Maximal speed on vehicle's current lane (== lane before last time step)
    // Note: this disregards the possibility of different maximal speeds on different traversed lanes.
    //       (we accept this as discretization error)
    SUMOReal vmax = MAX2(veh.getLane()->getVehicleMaxSpeed(&veh), NUMERICAL_EPS);

    // Time loss suffered on the detector
    SUMOReal timeLoss = MAX2(0., (exitTime - entryTime)*(vmax - (entrySpeed + exitSpeed)/2)/vmax);

#ifdef DEBUG_LSD_TIME_ON_DETECTOR
    std::cout << SIMTIME << " calculateTimeLoss() for vehicle '" << veh.getID() << "'"
            << " oldPos = " << oldPos << " newPos = " << newPos
            << " entryPos = " << entryPos << " exitPos = " << exitPos
            << " timeLoss = " << timeLoss
            << std::endl;
#endif

    return timeLoss;
}


void
MSMultiLaneE2Collector::writeXMLDetectorProlog(OutputDevice& dev) const{
    dev.writeXMLHeader("detector", "det_e2_file.xsd");
}

void
MSMultiLaneE2Collector::writeXMLOutput(OutputDevice& dev, SUMOTime startTime, SUMOTime stopTime){
    dev << "   <interval begin=\"" << time2string(startTime) << "\" end=\"" << time2string(stopTime) << "\" " << "id=\"" << getID() << "\" ";

    const SUMOReal meanSpeed = myVehicleSamples != 0 ? mySpeedSum / myVehicleSamples : -1;
    const SUMOReal meanOccupancy = myTimeSamples != 0 ? myOccupancySum / (SUMOReal) myTimeSamples : 0;
    const SUMOReal meanJamLengthInMeters = myTimeSamples != 0 ? myMeanMaxJamInMeters / (SUMOReal) myTimeSamples : 0;
    const SUMOReal meanJamLengthInVehicles = myTimeSamples != 0 ? myMeanMaxJamInVehicles / (SUMOReal) myTimeSamples : 0;
    const SUMOReal meanVehicleNumber = myTimeSamples != 0 ? (SUMOReal) myMeanVehicleNumber / (SUMOReal) myTimeSamples : 0;
    const SUMOReal meanTimeLoss = meanVehicleNumber != 0 ? myTotalTimeLoss / meanVehicleNumber : -1;

    SUMOTime haltingDurationSum = 0;
    SUMOTime maxHaltingDuration = 0;
    int haltingNo = 0;
    for (std::vector<SUMOTime>::iterator i = myPastStandingDurations.begin(); i != myPastStandingDurations.end(); ++i) {
        haltingDurationSum += (*i);
        maxHaltingDuration = MAX2(maxHaltingDuration, (*i));
        haltingNo++;
    }
    for (std::map<std::string, SUMOTime> ::iterator i = myHaltingVehicleDurations.begin(); i != myHaltingVehicleDurations.end(); ++i) {
        haltingDurationSum += (*i).second;
        maxHaltingDuration = MAX2(maxHaltingDuration, (*i).second);
        haltingNo++;
    }
    const SUMOTime meanHaltingDuration = haltingNo != 0 ? haltingDurationSum / haltingNo : 0;

    SUMOTime intervalHaltingDurationSum = 0;
    SUMOTime intervalMaxHaltingDuration = 0;
    int intervalHaltingNo = 0;
    for (std::vector<SUMOTime>::iterator i = myPastIntervalStandingDurations.begin(); i != myPastIntervalStandingDurations.end(); ++i) {
        intervalHaltingDurationSum += (*i);
        intervalMaxHaltingDuration = MAX2(intervalMaxHaltingDuration, (*i));
        intervalHaltingNo++;
    }
    for (std::map<std::string, SUMOTime> ::iterator i = myIntervalHaltingVehicleDurations.begin(); i != myIntervalHaltingVehicleDurations.end(); ++i) {
        intervalHaltingDurationSum += (*i).second;
        intervalMaxHaltingDuration = MAX2(intervalMaxHaltingDuration, (*i).second);
        intervalHaltingNo++;
    }
    const SUMOTime intervalMeanHaltingDuration = intervalHaltingNo != 0 ? intervalHaltingDurationSum / intervalHaltingNo : 0;

    dev << "sampledSeconds=\"" << myVehicleSamples << "\" "
        << "nVehEntered=\"" << myNumberOfEnteredVehicles << "\" "
        << "nVehLeft=\"" << myNumberOfLeftVehicles << "\" "
        << "meanSpeed=\"" << meanSpeed << "\" "
        << "meanTimeLoss=\"" << meanTimeLoss << "\" "
        << "meanOccupancy=\"" << meanOccupancy << "\" "
        << "maxOccupancy=\"" << myMaxOccupancy << "\" "
        << "meanMaxJamLengthInVehicles=\"" << meanJamLengthInVehicles << "\" "
        << "meanMaxJamLengthInMeters=\"" << meanJamLengthInMeters << "\" "
        << "maxJamLengthInVehicles=\"" << myMaxJamInVehicles << "\" "
        << "maxJamLengthInMeters=\"" << myMaxJamInMeters << "\" "
        << "jamLengthInVehiclesSum=\"" << myJamLengthInVehiclesSum << "\" "
        << "jamLengthInMetersSum=\"" << myJamLengthInMetersSum << "\" "
        << "meanHaltingDuration=\"" << STEPS2TIME(meanHaltingDuration) << "\" "
        << "maxHaltingDuration=\"" << STEPS2TIME(maxHaltingDuration) << "\" "
        << "haltingDurationSum=\"" << STEPS2TIME(haltingDurationSum) << "\" "
        << "meanIntervalHaltingDuration=\"" << STEPS2TIME(intervalMeanHaltingDuration) << "\" "
        << "maxIntervalHaltingDuration=\"" << STEPS2TIME(intervalMaxHaltingDuration) << "\" "
        << "intervalHaltingDurationSum=\"" << STEPS2TIME(intervalHaltingDurationSum) << "\" "
        << "startedHalts=\"" << myStartedHalts << "\" "
        << "meanVehicleNumber=\"" << meanVehicleNumber << "\" "
        << "maxVehicleNumber=\"" << myMaxVehicleNumber << "\" "
        << "/>\n";
    reset();

}

void
MSMultiLaneE2Collector::reset() {
    myVehicleSamples = 0;
    myTotalTimeLoss = 0.;
    myNumberOfEnteredVehicles = 0;
    myNumberOfLeftVehicles = 0;
    myMaxVehicleNumber = 0;

    mySpeedSum = 0;
    myStartedHalts = 0;
    myJamLengthInMetersSum = 0;
    myJamLengthInVehiclesSum = 0;
    myOccupancySum = 0;
    myMaxOccupancy = 0;
    myMeanMaxJamInVehicles = 0;
    myMeanMaxJamInMeters = 0;
    myMaxJamInVehicles = 0;
    myMaxJamInMeters = 0;
    myTimeSamples = 0;
    myMeanVehicleNumber = 0;
    for (std::map<std::string, SUMOTime>::iterator i = myIntervalHaltingVehicleDurations.begin(); i != myIntervalHaltingVehicleDurations.end(); ++i) {
        (*i).second = 0;
    }
    myPastStandingDurations.clear();
    myPastIntervalStandingDurations.clear();
}


int
MSMultiLaneE2Collector::getCurrentVehicleNumber() const {
    int result = 0;
    for (VehicleInfoMap::const_iterator it = myVehicleInfos.begin(); it != myVehicleInfos.end(); it++) {
        if (it->second.onDetector) {
            result++;
        }
    }
    return result;
}



std::vector<std::string>
MSMultiLaneE2Collector::getCurrentVehicleIDs() const {
    std::vector<std::string> ret;
    for (VehicleInfoMap::const_iterator i = myVehicleInfos.begin(); i != myVehicleInfos.end(); ++i) {
        ret.push_back(i->second.id);
    }
    std::sort(ret.begin(), ret.end());
    return ret;
}


const MSMultiLaneE2Collector::VehicleInfoMap&
MSMultiLaneE2Collector::getCurrentVehicles() const {
    return myVehicleInfos;
}



int
MSMultiLaneE2Collector::getEstimatedCurrentVehicleNumber(SUMOReal speedThreshold) const {

    SUMOReal distance = std::numeric_limits<SUMOReal>::max();
    SUMOReal thresholdSpeed = myLane->getSpeedLimit() / speedThreshold;

    int count = 0;
    for (VehicleInfoMap::const_iterator it = myVehicleInfos.begin();
            it != myVehicleInfos.end(); it++) {
        if (it->second.onDetector) {
//            if (it->position < distance) {
//                distance = it->position;
//            }
//            const SUMOReal realDistance = myLane->getLength() - distance; // the closer vehicle get to the light the greater is the distance
            const SUMOReal realDistance = it->second.distToDetectorEnd;
            if (it->second.lastSpeed <= thresholdSpeed || it->second.lastAccel > 0) { //TODO speed less half of the maximum speed for the lane NEED TUNING
                count = (int)(realDistance / (it->second.length + it->second.minGap)) + 1;
            }
        }
    }

    return count;
}

SUMOReal
MSMultiLaneE2Collector::getEstimateQueueLength() const {

    if (myVehicleInfos.empty()) {
        return -1;
    }

    SUMOReal distance = std::numeric_limits<SUMOReal>::max();
    SUMOReal realDistance = 0;
    bool flowing =  true;
    for (VehicleInfoMap::const_iterator it = myVehicleInfos.begin();
            it != myVehicleInfos.end(); it++) {
        if (it->second.onDetector) {
            distance = MIN2(it->second.lastPos, distance);
            //  SUMOReal distanceTemp = myLane->getLength() - distance;
            if (it->second.lastSpeed <= 0.5) {
                realDistance = distance - it->second.length + it->second.minGap;
                flowing = false;
            }
//            DBG(
//                std::ostringstream str;
//                str << time2string(MSNet::getInstance()->getCurrentTimeStep())
//                << " MSE2Collector::getEstimateQueueLength::"
//                << " lane " << myLane->getID()
//                << " vehicle " << it->second.id
//                << " positionOnLane " << it->second.position
//                << " vel " << it->second.speed
//                << " realDistance " << realDistance;
//                WRITE_MESSAGE(str.str());
//            )
        }
    }
    if (flowing) {
        return 0;
    } else {
        return myLane->getLength() - realDistance;
    }
}

/****************************************************************************/

