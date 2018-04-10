/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSE3Collector.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Tue Dec 02 2003 22:17 CET
/// @version $Id$
///
// A detector of vehicles passing an area between entry/exit points
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

#include "MSE3Collector.h"
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * MSE3Collector::MSE3EntryReminder - definitions
 * ----------------------------------------------------------------------- */
MSE3Collector::MSE3EntryReminder::MSE3EntryReminder(
    const MSCrossSection& crossSection, MSE3Collector& collector) :
    MSMoveReminder(collector.getID() + "_entry", crossSection.myLane),
    myCollector(collector), myPosition(crossSection.myPosition) {}


bool
MSE3Collector::MSE3EntryReminder::notifyMove(SUMOVehicle& veh, double oldPos,
        double newPos, double newSpeed) {
    if (myCollector.myEnteredContainer.find(&veh) == myCollector.myEnteredContainer.end() && newPos > myPosition) {
        if (oldPos > myPosition) {
            // was behind the detector already in the last step
            return false;
        } else {
            // entered in this step
            const double oldSpeed = veh.getPreviousSpeed();
            const double entryTime = STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep());
            assert(!MSGlobals::gSemiImplicitEulerUpdate || newSpeed != 0); // how could it move across the detector otherwise
            const double timeBeforeEnter = MSCFModel::passingTime(oldPos, myPosition, newPos, oldSpeed, newSpeed);
            const double fractionTimeOnDet = TS - timeBeforeEnter;
            myCollector.enter(veh, entryTime - fractionTimeOnDet, fractionTimeOnDet);
        }
    }
    return true;
}


bool
MSE3Collector::MSE3EntryReminder::notifyLeave(SUMOVehicle& veh, double, MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
    if (reason >= MSMoveReminder::NOTIFICATION_ARRIVED) {
        WRITE_WARNING("Vehicle '" + veh.getID() + "' arrived inside " + toString(SUMO_TAG_E3DETECTOR) + " '" + myCollector.getID() + "'.");
        myCollector.myEnteredContainer.erase(&veh);
        return false;
    }
    return true;
}


/* -------------------------------------------------------------------------
 * MSE3Collector::MSE3LeaveReminder - definitions
 * ----------------------------------------------------------------------- */
MSE3Collector::MSE3LeaveReminder::MSE3LeaveReminder(
    const MSCrossSection& crossSection, MSE3Collector& collector) :
    MSMoveReminder(collector.getID() + "_exit", crossSection.myLane),
    myCollector(collector), myPosition(crossSection.myPosition) {}


bool
MSE3Collector::MSE3LeaveReminder::notifyMove(SUMOVehicle& veh, double oldPos,
        double newPos, double newSpeed) {
    if (newPos < myPosition) {
        // crossSection not yet reached
        return true;
    }
    const double oldSpeed = veh.getPreviousSpeed();
    if (oldPos < myPosition) {
        assert(!MSGlobals::gSemiImplicitEulerUpdate || newSpeed != 0); // how could it move across the detector otherwise
        const double timeBeforeLeave = MSCFModel::passingTime(oldPos, myPosition, newPos, oldSpeed, newSpeed);
//        const double leaveTimeFront = SIMTIME - TS + (myPosition - oldPos) / newSpeed;
        const double leaveTimeFront = SIMTIME - TS + timeBeforeLeave;
        myCollector.leaveFront(veh, leaveTimeFront);
    }
    const double backPos = newPos - veh.getVehicleType().getLength();
    if (backPos < myPosition) {
        // crossSection not yet left
        return true;
    }
    // crossSection left
    const double oldBackPos = oldPos - veh.getVehicleType().getLength();
    const double leaveStep = SIMTIME;
    assert(!MSGlobals::gSemiImplicitEulerUpdate || newSpeed != 0); // how could it move across the detector otherwise
    const double timeBeforeLeave = MSCFModel::passingTime(oldBackPos, myPosition, backPos, oldSpeed, newSpeed);
    myCollector.leave(veh, leaveStep - TS + timeBeforeLeave, timeBeforeLeave);
    return false;
}


bool
MSE3Collector::MSE3LeaveReminder::notifyLeave(SUMOVehicle&  veh , double /* lastPos */, MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
    if (reason == MSMoveReminder::NOTIFICATION_LANE_CHANGE) {
        return false;
    }
    if (reason == MSMoveReminder::NOTIFICATION_TELEPORT) {
        WRITE_WARNING("Vehicle '" + veh.getID() + "' teleported from " + toString(SUMO_TAG_E3DETECTOR) + " '" + myCollector.getID() + "'.");
        myCollector.myEnteredContainer.erase(&veh);
        return false;
    }
    return true;
}

/* -------------------------------------------------------------------------
 * MSE3Collector - definitions
 * ----------------------------------------------------------------------- */
MSE3Collector::MSE3Collector(const std::string& id,
                             const CrossSectionVector& entries,
                             const CrossSectionVector& exits,
                             double haltingSpeedThreshold,
                             SUMOTime haltingTimeThreshold,
                             const std::string& vTypes)
    : MSDetectorFileOutput(id, vTypes), myEntries(entries), myExits(exits),
      myHaltingTimeThreshold(haltingTimeThreshold), myHaltingSpeedThreshold(haltingSpeedThreshold),
      myCurrentMeanSpeed(0), myCurrentHaltingsNumber(0), myLastResetTime(-1) {
    // Set MoveReminders to entries and exits
    for (CrossSectionVectorConstIt crossSec1 = entries.begin(); crossSec1 != entries.end(); ++crossSec1) {
        myEntryReminders.push_back(new MSE3EntryReminder(*crossSec1, *this));
    }
    for (CrossSectionVectorConstIt crossSec2 = exits.begin(); crossSec2 != exits.end(); ++crossSec2) {
        myLeaveReminders.push_back(new MSE3LeaveReminder(*crossSec2, *this));
    }
    reset();
}


MSE3Collector::~MSE3Collector() {
    for (std::vector<MSE3EntryReminder*>::iterator i = myEntryReminders.begin(); i != myEntryReminders.end(); ++i) {
        delete *i;
    }
    for (std::vector<MSE3LeaveReminder*>::iterator i = myLeaveReminders.begin(); i != myLeaveReminders.end(); ++i) {
        delete *i;
    }
}


void
MSE3Collector::reset() {
    myLeftContainer.clear();
}



void
MSE3Collector::enter(const SUMOVehicle& veh, const double entryTimestep, const double fractionTimeOnDet) {
    if (myEnteredContainer.find(&veh) != myEnteredContainer.end()) {
        WRITE_WARNING("Vehicle '" + veh.getID() + "' reentered " + toString(SUMO_TAG_E3DETECTOR) + " '" + getID() + "'.");
        return;
    }
    const double speedFraction = veh.getSpeed() * fractionTimeOnDet;
    E3Values v;
    v.entryTime = entryTimestep;
    v.frontLeaveTime = 0;
    v.backLeaveTime = 0;
    v.speedSum = speedFraction;
    v.haltingBegin = veh.getSpeed() < myHaltingSpeedThreshold ? entryTimestep : -1;
    v.intervalSpeedSum = entryTimestep >= STEPS2TIME(myLastResetTime) ? speedFraction : 0;
    v.haltings = 0;
    v.intervalHaltings = 0;
    if (veh.getSpeed() < myHaltingSpeedThreshold) {
        if (fractionTimeOnDet > myHaltingTimeThreshold) {
            v.haltings++;
            v.intervalHaltings++;
        }
    }
    v.hadUpdate = false;
    if (!MSGlobals::gUseMesoSim) {
        v.timeLoss = static_cast<const MSVehicle&>(veh).getTimeLoss();
        v.intervalTimeLoss = v.timeLoss;
    }

    myEnteredContainer[&veh] = v;
}


void
MSE3Collector::leaveFront(const SUMOVehicle& veh, const double leaveTimestep) {
    if (myEnteredContainer.find(&veh) == myEnteredContainer.end()) {
        WRITE_WARNING("Vehicle '" + veh.getID() + "' left " + toString(SUMO_TAG_E3DETECTOR) + " '" + getID() + "' without entering it.");
    } else {
        myEnteredContainer[&veh].frontLeaveTime = leaveTimestep;
    }
}


void
MSE3Collector::leave(const SUMOVehicle& veh, const double leaveTimestep, const double fractionTimeOnDet) {
    if (myEnteredContainer.find(&veh) == myEnteredContainer.end()) {
        WRITE_WARNING("Vehicle '" + veh.getID() + "' left " + toString(SUMO_TAG_E3DETECTOR) + " '" + getID() + "' without entering it.");
    } else {
        E3Values values = myEnteredContainer[&veh];
        values.backLeaveTime = leaveTimestep;
        const double speedFraction = veh.getSpeed() * (TS - fractionTimeOnDet);
        values.speedSum -= speedFraction;
        values.intervalSpeedSum -= speedFraction;
        if (MSGlobals::gUseMesoSim) {
            // not yet supported
            values.timeLoss = 0;
        } else {
            // timeLoss was initialized when entering
            values.timeLoss = static_cast<const MSVehicle&>(veh).getTimeLoss() - values.timeLoss;
        }
        myEnteredContainer.erase(&veh);
        myLeftContainer[&veh] = values;
    }
}


void
MSE3Collector::writeXMLOutput(OutputDevice& dev,
                              SUMOTime startTime, SUMOTime stopTime) {
    dev << "   <interval begin=\"" << time2string(startTime) << "\" end=\"" << time2string(stopTime) << "\" " << "id=\"" << myID << "\" ";
    // collect values about vehicles that have left the area
    const int vehicleSum = (int) myLeftContainer.size();
    double meanTravelTime = 0.;
    double meanOverlapTravelTime = 0.;
    double meanSpeed = 0.;
    double meanHaltsPerVehicle = 0.;
    double meanTimeLoss = 0.;
    for (std::map<const SUMOVehicle*, E3Values>::iterator i = myLeftContainer.begin(); i != myLeftContainer.end(); ++i) {
        meanHaltsPerVehicle += (double)(*i).second.haltings;
        meanTravelTime += (*i).second.frontLeaveTime - (*i).second.entryTime;
        const double steps = (*i).second.backLeaveTime - (*i).second.entryTime;
        meanOverlapTravelTime += steps;
        meanSpeed += ((*i).second.speedSum / steps);
        meanTimeLoss += STEPS2TIME((*i).second.timeLoss);
    }
    meanTravelTime = vehicleSum != 0 ? meanTravelTime / (double)vehicleSum : -1;
    meanOverlapTravelTime = vehicleSum != 0 ? meanOverlapTravelTime / (double)vehicleSum : -1;
    meanSpeed = vehicleSum != 0 ? meanSpeed / (double)vehicleSum : -1;
    meanHaltsPerVehicle = vehicleSum != 0 ? meanHaltsPerVehicle / (double) vehicleSum : -1;
    meanTimeLoss = vehicleSum != 0 ? meanTimeLoss / (double) vehicleSum : -1;
    // clear container
    myLeftContainer.clear();

    // collect values about vehicles within the container
    const int vehicleSumWithin = (int) myEnteredContainer.size();
    double meanSpeedWithin = 0.;
    double meanDurationWithin = 0.;
    double meanHaltsPerVehicleWithin = 0.;
    double meanIntervalSpeedWithin = 0.;
    double meanIntervalHaltsPerVehicleWithin = 0.;
    double meanIntervalDurationWithin = 0.;
    double meanTimeLossWithin = 0.;
    for (std::map<const SUMOVehicle*, E3Values>::iterator i = myEnteredContainer.begin(); i != myEnteredContainer.end(); ++i) {
        meanHaltsPerVehicleWithin += (double)(*i).second.haltings;
        meanIntervalHaltsPerVehicleWithin += (double)(*i).second.intervalHaltings;
        const double end = (*i).second.backLeaveTime == 0 ? STEPS2TIME(stopTime) : (*i).second.backLeaveTime;
        const double time = end - (*i).second.entryTime;
        const double timeWithin = MIN2(time, end - STEPS2TIME(startTime));
        if (i->second.speedSum > 0.) {
            meanSpeedWithin += i->second.speedSum / time;
        }
        if (i->second.intervalSpeedSum > 0.) {
            meanIntervalSpeedWithin += i->second.intervalSpeedSum / timeWithin;
        }
        meanDurationWithin += time;
        meanIntervalDurationWithin += timeWithin;
        // reset interval values
        (*i).second.intervalHaltings = 0;
        (*i).second.intervalSpeedSum = 0;

        if (!MSGlobals::gUseMesoSim) {
            const SUMOTime currentTimeLoss = static_cast<const MSVehicle*>(i->first)->getTimeLoss();
            meanTimeLossWithin += STEPS2TIME(currentTimeLoss - (*i).second.intervalTimeLoss);
            (*i).second.intervalTimeLoss = currentTimeLoss;
        }
    }
    myLastResetTime = stopTime;
    meanSpeedWithin = vehicleSumWithin != 0 ?  meanSpeedWithin / (double) vehicleSumWithin : -1;
    meanHaltsPerVehicleWithin = vehicleSumWithin != 0 ? meanHaltsPerVehicleWithin / (double) vehicleSumWithin : -1;
    meanDurationWithin = vehicleSumWithin != 0 ? meanDurationWithin / (double) vehicleSumWithin : -1;
    meanIntervalSpeedWithin = vehicleSumWithin != 0 ?  meanIntervalSpeedWithin / (double) vehicleSumWithin : -1;
    meanIntervalHaltsPerVehicleWithin = vehicleSumWithin != 0 ? meanIntervalHaltsPerVehicleWithin / (double) vehicleSumWithin : -1;
    meanIntervalDurationWithin = vehicleSumWithin != 0 ? meanIntervalDurationWithin / (double) vehicleSumWithin : -1;
    meanTimeLossWithin = vehicleSumWithin != 0 ? meanTimeLossWithin / (double) vehicleSumWithin : -1;

    // write values
    dev << "meanTravelTime=\"" << meanTravelTime
        << "\" meanOverlapTravelTime=\"" << meanOverlapTravelTime
        << "\" meanSpeed=\"" << meanSpeed
        << "\" meanHaltsPerVehicle=\"" << meanHaltsPerVehicle
        << "\" meanTimeLoss=\"" << meanTimeLoss
        << "\" vehicleSum=\"" << vehicleSum
        << "\" meanSpeedWithin=\"" << meanSpeedWithin
        << "\" meanHaltsPerVehicleWithin=\"" << meanHaltsPerVehicleWithin
        << "\" meanDurationWithin=\"" << meanDurationWithin
        << "\" vehicleSumWithin=\"" << vehicleSumWithin
        << "\" meanIntervalSpeedWithin=\"" << meanIntervalSpeedWithin
        << "\" meanIntervalHaltsPerVehicleWithin=\"" << meanIntervalHaltsPerVehicleWithin
        << "\" meanIntervalDurationWithin=\"" << meanIntervalDurationWithin
        << "\" meanTimeLossWithin=\"" << meanTimeLossWithin
        << "\"/>\n";
}


void
MSE3Collector::writeXMLDetectorProlog(OutputDevice& dev) const {
    dev.writeXMLHeader("e3Detector", "det_e3_file.xsd");
}


void
MSE3Collector::detectorUpdate(const SUMOTime step) {
    myCurrentMeanSpeed = 0;
    myCurrentHaltingsNumber = 0;
    for (std::map<const SUMOVehicle*, E3Values>::iterator pair = myEnteredContainer.begin(); pair != myEnteredContainer.end(); ++pair) {
        const SUMOVehicle* veh = pair->first;
        E3Values& values = pair->second;
        myCurrentMeanSpeed += veh->getSpeed();
        values.hadUpdate = true;
        values.speedSum += veh->getSpeed() * TS;
        values.intervalSpeedSum += veh->getSpeed() * TS;
        if (veh->getSpeed() < myHaltingSpeedThreshold) {
            if (values.haltingBegin == -1) {
                values.haltingBegin = STEPS2TIME(step);
            }
            if (step - values.haltingBegin > myHaltingTimeThreshold) {
                values.haltings++;
                values.intervalHaltings++;
                myCurrentHaltingsNumber++;
            }
        } else {
            values.haltingBegin = -1;
        }
    }
    if (myEnteredContainer.size() == 0) {
        myCurrentMeanSpeed = -1;
    } else {
        myCurrentMeanSpeed /= myEnteredContainer.size();
    }
}


double
MSE3Collector::getCurrentMeanSpeed() const {
    return myCurrentMeanSpeed;
}


int
MSE3Collector::getCurrentHaltingNumber() const {
    return myCurrentHaltingsNumber;
}


int
MSE3Collector::getVehiclesWithin() const {
    return (int) myEnteredContainer.size();
}


std::vector<std::string>
MSE3Collector::getCurrentVehicleIDs() const {
    std::vector<std::string> ret;
    for (std::map<const SUMOVehicle*, E3Values>::const_iterator pair = myEnteredContainer.begin(); pair != myEnteredContainer.end(); ++pair) {
        ret.push_back((*pair).first->getID());
    }
    std::sort(ret.begin(), ret.end());
    return ret;
}


/****************************************************************************/

