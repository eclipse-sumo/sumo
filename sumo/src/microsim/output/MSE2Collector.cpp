/****************************************************************************/
/// @file    MSE2Collector.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Robbin Blokpoel
/// @author  Jakob Erdmann
/// @author  Leonhard Luecken
/// @date    Mon Feb 03 2014 10:13 CET
/// @version $Id$
///
// An areal (along a single lane) detector
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

#include <cassert>
#include <algorithm>
#include "MSE2Collector.h"
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleType.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

//#define DEBUG_NOTIFY_MOVE

// ===========================================================================
// method definitions
// ===========================================================================
MSE2Collector::MSE2Collector(const std::string& id, DetectorUsage usage,
                             MSLane* const lane, SUMOReal startPos, SUMOReal detLength,
                             SUMOTime haltingTimeThreshold,
                             SUMOReal haltingSpeedThreshold,
                             SUMOReal jamDistThreshold,
                             const std::string& vTypes) :
    MSMoveReminder(id, lane),
    MSDetectorFileOutput(id, vTypes),
    myJamHaltingSpeedThreshold(haltingSpeedThreshold),
    myJamHaltingTimeThreshold(haltingTimeThreshold),
    myJamDistanceThreshold(jamDistThreshold),
    myStartPos(startPos), myEndPos(startPos + detLength),
    myUsage(usage),
    myCurrentOccupancy(0), myCurrentMeanSpeed(-1), myCurrentJamNo(0),
    myCurrentMaxJamLengthInMeters(0), myCurrentMaxJamLengthInVehicles(0),
    myCurrentJamLengthInMeters(0), myCurrentJamLengthInVehicles(0), myCurrentStartedHalts(0),
    myCurrentHaltingsNumber(0), myPassedVeh(0) {
    assert(myLane != 0);
    assert(myStartPos >= 0 && myStartPos < myLane->getLength());
    assert(myEndPos - myStartPos > 0 && myEndPos <= myLane->getLength());
    reset();
}


MSE2Collector::MSE2Collector(const std::string& id, DetectorUsage usage,
                             MSLane* const lane, SUMOReal startPos, SUMOReal detLength,
                             SUMOTime haltingTimeThreshold,
                             SUMOReal haltingSpeedThreshold,
                             SUMOReal jamDistThreshold,
                             const std::set<std::string>& vTypes) :
    MSMoveReminder(id, lane),
    MSDetectorFileOutput(id, vTypes),
    myJamHaltingSpeedThreshold(haltingSpeedThreshold),
    myJamHaltingTimeThreshold(haltingTimeThreshold),
    myJamDistanceThreshold(jamDistThreshold),
    myStartPos(startPos), myEndPos(startPos + detLength),
    myUsage(usage),
    myCurrentOccupancy(0), myCurrentMeanSpeed(-1), myCurrentJamNo(0),
    myCurrentMaxJamLengthInMeters(0), myCurrentMaxJamLengthInVehicles(0),
    myCurrentJamLengthInMeters(0), myCurrentJamLengthInVehicles(0), myCurrentStartedHalts(0),
    myCurrentHaltingsNumber(0), myPassedVeh(0) {
    assert(myLane != 0);
    assert(myStartPos >= 0 && myStartPos < myLane->getLength());
    assert(myEndPos - myStartPos > 0 && myEndPos <= myLane->getLength());
    reset();
}


MSE2Collector::~MSE2Collector() {
    myKnownVehicles.clear();
}


bool
MSE2Collector::notifyMove(SUMOVehicle& veh, SUMOReal oldPos,
                          SUMOReal newPos, SUMOReal newSpeed) {
    if (newPos <= myStartPos) {
        // detector not yet reached
        return true;
    }


    const SUMOReal oldSpeed = veh.getPreviousSpeed();
    SUMOReal enterSpeed = MSGlobals::gSemiImplicitEulerUpdate ? newSpeed : oldSpeed; // NOTE: For the euler update, the vehicle is assumed to travel at constant speed for the whole time step
    SUMOReal leaveSpeed = newSpeed;

    SUMOReal lengthOnDet = MIN2(veh.getVehicleType().getLength(), newPos - myStartPos);
    if (newPos > myEndPos) {
        lengthOnDet = MAX2(SUMOReal(0), lengthOnDet - (newPos - myEndPos));
    }

    SUMOReal timeOnDet = TS;

    // Treat the case that the vehicle entered the lane in the last step
    if (newPos > myStartPos && oldPos <= myStartPos) {
        // Vehicle was not on this lane in the last time step
        const SUMOReal timeBeforeEnter = MSCFModel::passingTime(oldPos, myStartPos, newPos, oldSpeed, newSpeed);
        timeOnDet -= timeBeforeEnter;
        enterSpeed = MSCFModel::speedAfterTime(timeBeforeEnter, oldSpeed, newPos - oldPos);
        myPassedVeh++;
    }

    // Treat the case that the vehicle's back left the lane in the last step
    const SUMOReal oldBackPos = oldPos - veh.getVehicleType().getLength();
    const SUMOReal newBackPos = newPos - veh.getVehicleType().getLength();
    const SUMOReal vmax = myLane->getVehicleMaxSpeed(&veh);
    if (newBackPos > myEndPos) {
        assert(oldBackPos <= myEndPos);
        const SUMOReal timeBeforeLeave = MSCFModel::passingTime(oldBackPos, myEndPos, newBackPos, oldSpeed, newSpeed);
        const SUMOReal timeAfterLeave = TS - timeBeforeLeave;
        timeOnDet -= timeAfterLeave;
        leaveSpeed = MSCFModel::speedAfterTime(timeBeforeLeave, oldSpeed, newPos - oldPos);
        // XXX: Do we really need this? Why would this "reduce rounding errors"? (Leo) Refs. #2579
        if (fabs(timeOnDet) < NUMERICAL_EPS) { // reduce rounding errors
            timeOnDet = 0.;
        } else {
            const SUMOReal averageSpeedOnDetector = (enterSpeed + leaveSpeed) / 2.;
            myKnownVehicles.push_back(VehicleInfo(veh.getID(), veh.getVehicleType().getID(), averageSpeedOnDetector,
                                                  timeOnDet, lengthOnDet, newPos,
                                                  veh.getVehicleType().getLengthWithGap(), veh.getAcceleration(), vmax, false));
        }
        return false;
    }
    const SUMOReal averageSpeedOnDetector = (enterSpeed + leaveSpeed) / 2.;
    myKnownVehicles.push_back(VehicleInfo(veh.getID(), veh.getVehicleType().getID(), averageSpeedOnDetector,
                                          timeOnDet, lengthOnDet, newPos,
                                          veh.getVehicleType().getLengthWithGap(), veh.getAcceleration(), vmax, true));
//    DBG(
#ifdef DEBUG_NOTIFY_MOVE
    std::ostringstream str;
    str << time2string(MSNet::getInstance()->getCurrentTimeStep())
        << " MSE2Collector::notifyMove::"
        << " lane " << myLane->getID()
        << " passedVeh " << myPassedVeh
        << " myKnownVehicles-length " << myPreviousKnownVehicles.size();
//        WRITE_MESSAGE(str.str());
    std::cout << str.str() << std::endl;
#endif
//    )
    return true;
}


bool
MSE2Collector::notifyLeave(SUMOVehicle& /* veh */, SUMOReal /* lastPos */, MSMoveReminder::Notification reason) {
    if (reason != MSMoveReminder::NOTIFICATION_JUNCTION) {
        return false;
    }
    return true;
}


bool
MSE2Collector::notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification /* reason */) {
    if (!vehicleApplies(veh)) {
        return false;
    }
    if (!veh.isOnRoad()) {
        // vehicle is teleporting over the edge
        return false;
    }
    // is vehicle beyond detector?
    return veh.getBackPositionOnLane(myLane) < myEndPos;
}


void
MSE2Collector::reset() {
    mySpeedSum = 0;
    myStartedHalts = 0;
    myJamLengthInMetersSum = 0;
    myJamLengthInVehiclesSum = 0;
    myVehicleSamples = 0;
    myTimeLossSum = 0;
    myOccupancySum = 0;
    myMaxOccupancy = 0;
    myMeanMaxJamInVehicles = 0;
    myMeanMaxJamInMeters = 0;
    myMaxJamInVehicles = 0;
    myMaxJamInMeters = 0;
    myTimeSamples = 0;
    myMeanVehicleNumber = 0;
    myMaxVehicleNumber = 0;
    for (std::map<std::string, SUMOTime>::iterator i = myIntervalHaltingVehicleDurations.begin(); i != myIntervalHaltingVehicleDurations.end(); ++i) {
        (*i).second = 0;
    }
    myPastStandingDurations.clear();
    myPastIntervalStandingDurations.clear();
    myPassedVeh = 0;
}



void
MSE2Collector::detectorUpdate(const SUMOTime /* step */) {
    JamInfo* currentJam = 0;
    std::map<std::string, SUMOTime> haltingVehicles;
    std::map<std::string, SUMOTime> intervalHaltingVehicles;
    std::vector<JamInfo*> jams;

    SUMOReal lengthSum = 0;
    myCurrentMeanSpeed = 0;
    myCurrentMeanLength = 0;
    myCurrentStartedHalts = 0;
    myCurrentHaltingsNumber = 0;

    // go through the (sorted) list of vehicles positioned on the detector
    //  sum up values and prepare the list of jams
    std::vector<VehicleInfo>::const_iterator iv;
    const int numVehicles = (int)myKnownVehicles.size();
    for (std::vector<VehicleInfo>::const_iterator i = myKnownVehicles.begin(); i != myKnownVehicles.end(); ++i) {
        myVehicleSamples += i->timeOnDet;
        mySpeedSum += i->speed * i->timeOnDet;
        myTimeLossSum += i->timeLoss;
        myCurrentMeanSpeed += i->speed * i->timeOnDet;
        lengthSum += i->lengthOnDet;
        myCurrentMeanLength += i->lengthOnDet;

        // jam-checking begins
        bool isInJam = false;
        // first, check whether the vehicle is slow enough to be states as halting
        if (i->speed < myJamHaltingSpeedThreshold) {
            myCurrentHaltingsNumber++;
            // we have to track the time it was halting;
            //  so let's look up whether it was halting before and compute the overall halting time
            bool wasHalting = myHaltingVehicleDurations.count(i->id) > 0;
            if (wasHalting) {
                haltingVehicles[i->id] = myHaltingVehicleDurations[i->id] + DELTA_T;
                intervalHaltingVehicles[i->id] = myIntervalHaltingVehicleDurations[i->id] + DELTA_T;
            } else {
                haltingVehicles[i->id] = DELTA_T;
                intervalHaltingVehicles[i->id] = DELTA_T;
                myCurrentStartedHalts++;
                myStartedHalts++;
            }
            // we now check whether the halting time is large enough
            if (haltingVehicles[i->id] > myJamHaltingTimeThreshold) {
                // yep --> the vehicle is a part of a jam
                isInJam = true;
            }
        } else {
            // is not standing anymore; keep duration information
            std::map<std::string, SUMOTime>::iterator v = myHaltingVehicleDurations.find(i->id);
            if (v != myHaltingVehicleDurations.end()) {
                myPastStandingDurations.push_back((*v).second);
                myHaltingVehicleDurations.erase(v);
            }
            v = myIntervalHaltingVehicleDurations.find(i->id);
            if (v != myIntervalHaltingVehicleDurations.end()) {
                myPastIntervalStandingDurations.push_back((*v).second);
                myIntervalHaltingVehicleDurations.erase(v);
            }
        }

        // jam-building
        if (isInJam) {
            // the vehicle is in a jam;
            //  it may be a new one or already an existing one
            if (currentJam == 0) {
                // the vehicle is the first vehicle in a jam
                currentJam = new JamInfo();
                currentJam->firstStandingVehicle = i;
            } else {
                // ok, we have a jam already. But - maybe it is too far away
                //  ... honestly, I can hardly find a reason for doing this,
                //  but jams were defined this way in an earlier version...
                if (i->position - currentJam->lastStandingVehicle->position > myJamDistanceThreshold) {
                    // yep, yep, yep - it's a new one...
                    //  close the frist, build a new
                    jams.push_back(currentJam);
                    currentJam = new JamInfo();
                    currentJam->firstStandingVehicle = i;
                }
            }
            currentJam->lastStandingVehicle = i;
        } else {
            // the vehicle is not part of a jam...
            //  maybe we have to close an already computed jam
            if (currentJam != 0) {
                jams.push_back(currentJam);
                currentJam = 0;
            }
        }
    }
    if (currentJam != 0) {
        jams.push_back(currentJam);
        currentJam = 0;
    }

    myCurrentMaxJamLengthInMeters = 0;
    myCurrentMaxJamLengthInVehicles = 0;
    myCurrentJamLengthInMeters = 0;
    myCurrentJamLengthInVehicles = 0;
    // process jam information
    for (std::vector<JamInfo*>::iterator i = jams.begin(); i != jams.end(); ++i) {
        // compute current jam's values
        const SUMOReal jamLengthInMeters = (*i)->firstStandingVehicle->position
                                           - (*i)->lastStandingVehicle->position
                                           + (*i)->lastStandingVehicle->lengthOnDet;
        const int jamLengthInVehicles = (int)distance((*i)->firstStandingVehicle, (*i)->lastStandingVehicle) + 1;
        // apply them to the statistics
        myCurrentMaxJamLengthInMeters = MAX2(myCurrentMaxJamLengthInMeters, jamLengthInMeters);
        myCurrentMaxJamLengthInVehicles = MAX2(myCurrentMaxJamLengthInVehicles, jamLengthInVehicles);
        myJamLengthInMetersSum += jamLengthInMeters;
        myJamLengthInVehiclesSum += jamLengthInVehicles;
        myCurrentJamLengthInMeters += jamLengthInMeters;
        myCurrentJamLengthInVehicles += jamLengthInVehicles;
    }
    myCurrentJamNo = (int) jams.size();

    myTimeSamples += 1;
    // compute occupancy values
    SUMOReal currentOccupancy = lengthSum / (myEndPos - myStartPos) * (SUMOReal) 100.;
    myCurrentOccupancy = currentOccupancy;
    myOccupancySum += currentOccupancy;
    myMaxOccupancy = MAX2(myMaxOccupancy, currentOccupancy);
    // compute jam values
    myMeanMaxJamInVehicles += myCurrentMaxJamLengthInVehicles;
    myMeanMaxJamInMeters += myCurrentMaxJamLengthInMeters;
    myMaxJamInVehicles = MAX2(myMaxJamInVehicles, myCurrentMaxJamLengthInVehicles);
    myMaxJamInMeters = MAX2(myMaxJamInMeters, myCurrentMaxJamLengthInMeters);
    // save information about halting vehicles
    myHaltingVehicleDurations = haltingVehicles;
    myIntervalHaltingVehicleDurations = intervalHaltingVehicles;
    // compute information about vehicle numbers
    myMeanVehicleNumber += numVehicles;
    myMaxVehicleNumber = MAX2(numVehicles, myMaxVehicleNumber);
    // norm current values
    myCurrentMeanSpeed = numVehicles != 0 ? myCurrentMeanSpeed / (SUMOReal) numVehicles : -1;
    myCurrentMeanLength = numVehicles != 0 ? myCurrentMeanLength / (SUMOReal) numVehicles : -1;

    // clean up
    for (std::vector<JamInfo*>::iterator i = jams.begin(); i != jams.end(); ++i) {
        delete *i;
    }


    // Accumulate the timelosses for the individual vehicles
    // TODO: This could be speeded up by (1) not starting the search from the begin() but using the previous iv+1 as a start point and
    //       looking in a zigzag fashion for the next vehicle; (2) not using strings in the comparison but (maybe) pointers
    //       (include in VehicleInfo?); (3) including this code into the first loop above on the price of using a non-constant
    //       iterator there.
    for (std::vector<VehicleInfo>::iterator i = myKnownVehicles.begin(); i != myKnownVehicles.end(); ++i) {
        for (iv = myPreviousKnownVehicles.begin(); iv != myPreviousKnownVehicles.end(); ++iv) {
            // if the vehicle was here before, account for the accumulated timeloss as well (i->timeLoss is only the last step's timeloss)
            if (iv->id == i->id) {
                i->accumulatedTimeLoss += iv->timeLoss;
                break;
            }
        }
    }


    myPreviousKnownVehicles = myKnownVehicles;
    myKnownVehicles.clear();
}



void
MSE2Collector::writeXMLOutput(OutputDevice& dev, SUMOTime startTime, SUMOTime stopTime) {
    dev << "   <interval begin=\"" << time2string(startTime) << "\" end=\"" << time2string(stopTime) << "\" " << "id=\"" << getID() << "\" ";

    const SUMOReal meanSpeed = myVehicleSamples != 0 ? mySpeedSum / myVehicleSamples : -1;
    const SUMOReal meanOccupancy = myTimeSamples != 0 ? myOccupancySum / (SUMOReal) myTimeSamples : 0;
    const SUMOReal meanJamLengthInMeters = myTimeSamples != 0 ? myMeanMaxJamInMeters / (SUMOReal) myTimeSamples : 0;
    const SUMOReal meanJamLengthInVehicles = myTimeSamples != 0 ? myMeanMaxJamInVehicles / (SUMOReal) myTimeSamples : 0;
    const SUMOReal meanVehicleNumber = myTimeSamples != 0 ? (SUMOReal) myMeanVehicleNumber / (SUMOReal) myTimeSamples : 0;
    const SUMOReal meanTimeLoss = meanVehicleNumber != 0 ? myTimeLossSum / meanVehicleNumber : -1;

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
        << "nVehEntered=\"" << myPassedVeh << "\" "
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
MSE2Collector::writeXMLDetectorProlog(OutputDevice& dev) const {
    dev.writeXMLHeader("detector", "det_e2_file.xsd");
}


int
MSE2Collector::getCurrentVehicleNumber() const {
    int result = 0;
    for (std::vector<VehicleInfo>::const_iterator it = myPreviousKnownVehicles.begin();
            it != myPreviousKnownVehicles.end(); it++) {
        if (it->stillOnDet) {
            result++;
        }
    }
    return result;
}

int
MSE2Collector::getEstimatedCurrentVehicleNumber(SUMOReal speedThreshold) const {

    SUMOReal distance = std::numeric_limits<SUMOReal>::max();
    SUMOReal thresholdSpeed = myLane->getSpeedLimit() / speedThreshold;

    int count = 0;
    for (std::vector<VehicleInfo>::const_iterator it = myPreviousKnownVehicles.begin();
            it != myKnownVehicles.end(); it++) {
        if (it->stillOnDet) {
            if (it->position < distance) {
                distance = it->position;
            }
            const SUMOReal realDistance = myLane->getLength() - distance; // the closer vehicle get to the light the greater is the distance
            if (it->speed <= thresholdSpeed || it->accel > 0) { //TODO speed less than half of the maximum speed for the lane NEED TUNING
                count = (int)(realDistance / it->lengthWithGap) + 1;
            }
        }
    }

    return count;
}

SUMOReal
MSE2Collector::getEstimateQueueLength() const {

    if (myPreviousKnownVehicles.empty()) {
        return -1;
    }

    SUMOReal distance = std::numeric_limits<SUMOReal>::max();
    SUMOReal realDistance = 0;
    bool flowing =  true;
    for (std::vector<VehicleInfo>::const_iterator it = myPreviousKnownVehicles.begin();
            it != myPreviousKnownVehicles.end(); it++) {
        if (it->stillOnDet) {
            if (it->position < distance) {
                distance = it->position;
            }
            //	SUMOReal distanceTemp = myLane->getLength() - distance;
            if (it->speed <= 0.5) {
                realDistance = distance - it->lengthWithGap;
                flowing = false;
            }
            DBG(
                std::ostringstream str;
                str << time2string(MSNet::getInstance()->getCurrentTimeStep())
                << " MSE2Collector::getEstimateQueueLength::"
                << " lane " << myLane->getID()
                << " vehicle " << it->id
                << " positionOnLane " << it->position
                << " vel " << it->speed
                << " realDistance " << realDistance;
                WRITE_MESSAGE(str.str());
            )
        }
    }
    if (flowing) {
        return 0;
    } else {
        return myLane->getLength() - realDistance;
    }
}


SUMOReal
MSE2Collector::getCurrentOccupancy() const {
    return myCurrentOccupancy;
}


SUMOReal
MSE2Collector::getCurrentMeanSpeed() const {
    return myCurrentMeanSpeed;
}


SUMOReal
MSE2Collector::getCurrentMeanLength() const {
    return myCurrentMeanLength;
}


int
MSE2Collector::getCurrentJamNumber() const {
    return myCurrentJamNo;
}


int
MSE2Collector::getCurrentMaxJamLengthInVehicles() const {
    return myCurrentMaxJamLengthInVehicles;
}


SUMOReal
MSE2Collector::getCurrentMaxJamLengthInMeters() const {
    return myCurrentMaxJamLengthInMeters;
}


int
MSE2Collector::getCurrentJamLengthInVehicles() const {
    return myCurrentJamLengthInVehicles;
}


SUMOReal
MSE2Collector::getCurrentJamLengthInMeters() const {
    return myCurrentJamLengthInMeters;
}


int
MSE2Collector::getCurrentStartedHalts() const {
    return myCurrentStartedHalts;
}


int
MSE2Collector::getCurrentHaltingNumber() const {
    return myCurrentHaltingsNumber;
}


std::vector<std::string>
MSE2Collector::getCurrentVehicleIDs() const {
    std::vector<std::string> ret;
    for (std::vector<VehicleInfo>::const_iterator i = myPreviousKnownVehicles.begin(); i != myPreviousKnownVehicles.end(); ++i) {
        ret.push_back(i->id);
    }
    std::sort(ret.begin(), ret.end());
    return ret;
}


const std::vector<MSE2Collector::VehicleInfo>&
MSE2Collector::getCurrentVehicles() const {
    return myPreviousKnownVehicles;
}

/****************************************************************************/

