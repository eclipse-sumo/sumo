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
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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

#include "MSE3Collector.h"
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


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
MSE3Collector::MSE3EntryReminder::notifyMove(SUMOVehicle& veh, SUMOReal oldPos,
        SUMOReal newPos, SUMOReal newSpeed) {
    if (myCollector.myEnteredContainer.find(&veh) == myCollector.myEnteredContainer.end() && newPos >= myPosition) {
        if (oldPos > myPosition) {
            // was behind the detector
            return false;
        } else {
            SUMOReal entryTime = STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep());
            if (newSpeed != 0) {
                if (myPosition > oldPos) {
                    entryTime += (myPosition - oldPos) / newSpeed;
                }
            }
            myCollector.enter(veh, entryTime);
        }
    }
    return true;
}


bool
MSE3Collector::MSE3EntryReminder::notifyLeave(SUMOVehicle& veh, SUMOReal, MSMoveReminder::Notification reason) {
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
MSE3Collector::MSE3LeaveReminder::notifyMove(SUMOVehicle& veh, SUMOReal oldPos,
        SUMOReal newPos, SUMOReal newSpeed) {
    if (newPos <= myPosition) {
        // crossSection not yet reached
        return true;
    }
    if (oldPos > myPosition) {
        // crossSection was not passed
        return false;
    }
    // crossSection left
    SUMOReal leaveTime = STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep());
    if (myPosition > oldPos) {
        leaveTime += (myPosition - oldPos) / newSpeed;
    }
    myCollector.leave(veh, leaveTime);
    return false;
}


bool
MSE3Collector::MSE3LeaveReminder::notifyLeave(SUMOVehicle& /* veh */, SUMOReal /* lastPos */, MSMoveReminder::Notification reason) {
    if (reason == MSMoveReminder::NOTIFICATION_LANE_CHANGE) {
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
                             SUMOReal haltingSpeedThreshold,
                             SUMOTime haltingTimeThreshold)
    : MSDetectorFileOutput(id), myEntries(entries), myExits(exits),
      myHaltingTimeThreshold(haltingTimeThreshold), myHaltingSpeedThreshold(haltingSpeedThreshold),
      myCurrentMeanSpeed(0), myCurrentHaltingsNumber(0), myCurrentTouchedVehicles(0),
      myLastResetTime(-1) {
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
MSE3Collector::enter(SUMOVehicle& veh, SUMOReal entryTimestep) {
    if (myEnteredContainer.find(&veh) != myEnteredContainer.end()) {
        WRITE_WARNING("Vehicle '" + veh.getID() + "' reentered " + toString(SUMO_TAG_E3DETECTOR) + " '" + getID() + "'.");
        return;
    }
    const SUMOReal entryTimestepFraction = ((SUMOReal) DELTA_T - fmod(entryTimestep * 1000., 1000.)) / (SUMOReal) DELTA_T;
    const SUMOReal speedFraction = veh.getSpeed() * entryTimestepFraction;
    E3Values v;
    v.entryTime = entryTimestep;
    v.leaveTime = 0;
    v.speedSum = speedFraction * TS;
    v.haltingBegin = veh.getSpeed() < myHaltingSpeedThreshold ? entryTimestep : -1;
    v.intervalSpeedSum = speedFraction * TS;
    v.haltings = 0;
    v.intervalHaltings = 0;
    if (veh.getSpeed() < myHaltingSpeedThreshold) {
        if (1. - entryTimestepFraction > myHaltingTimeThreshold) {
            v.haltings++;
            v.intervalHaltings++;
        }
    }
    v.hadUpdate = false;
    myEnteredContainer[&veh] = v;
}


void
MSE3Collector::leave(SUMOVehicle& veh, SUMOReal leaveTimestep) {
    if (myEnteredContainer.find(&veh) == myEnteredContainer.end()) {
        WRITE_WARNING("Vehicle '" + veh.getID() + "' left " + toString(SUMO_TAG_E3DETECTOR) + " '" + getID() + "' before entering it.");
    } else {
        E3Values values = myEnteredContainer[&veh];
        values.leaveTime = leaveTimestep;
        SUMOReal leaveTimestepFraction = leaveTimestep - (SUMOReal)((int) leaveTimestep);
        leaveTimestepFraction = fmod(leaveTimestep * 1000., 1000.) / (SUMOReal) DELTA_T;
        if (values.hadUpdate) {
            SUMOReal speedFraction = (veh.getSpeed() * leaveTimestepFraction);
            values.speedSum += speedFraction * TS;
            values.intervalSpeedSum += speedFraction * TS;
            if (veh.getSpeed() < myHaltingSpeedThreshold && values.haltingBegin != -1 && leaveTimestep - values.haltingBegin > myHaltingTimeThreshold) {
                values.haltings++;
                values.intervalHaltings++;
            }
        } else {
            SUMOReal speedFraction = (veh.getSpeed() * SUMOReal(1. - leaveTimestepFraction));
            values.speedSum -= speedFraction * TS;
            values.intervalSpeedSum -= speedFraction * TS;
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
    unsigned vehicleSum = (unsigned) myLeftContainer.size();
    SUMOReal meanTravelTime = 0.;
    SUMOReal meanSpeed = 0.;
    SUMOReal meanHaltsPerVehicle = 0.;
    for (std::map<SUMOVehicle*, E3Values>::iterator i = myLeftContainer.begin(); i != myLeftContainer.end(); ++i) {
        meanHaltsPerVehicle += (SUMOReal)(*i).second.haltings;
        SUMOReal steps = (*i).second.leaveTime - (*i).second.entryTime;
        meanTravelTime += steps;
        meanSpeed += ((*i).second.speedSum / steps);
    }
    meanTravelTime = vehicleSum != 0 ? meanTravelTime / (SUMOReal) vehicleSum : -1;
    meanSpeed = vehicleSum != 0 ?  meanSpeed / (SUMOReal) vehicleSum : -1;
    meanHaltsPerVehicle = vehicleSum != 0 ? meanHaltsPerVehicle / (SUMOReal) vehicleSum : -1;
    // clear container
    myLeftContainer.clear();

    // collect values about vehicles within the container
    unsigned vehicleSumWithin = (unsigned) myEnteredContainer.size();
    SUMOReal meanSpeedWithin = 0.;
    SUMOReal meanDurationWithin = 0.;
    SUMOReal meanHaltsPerVehicleWithin = 0.;
    SUMOReal meanIntervalSpeedWithin = 0.;
    SUMOReal meanIntervalHaltsPerVehicleWithin = 0.;
    SUMOReal meanIntervalDurationWithin = 0.;
    for (std::map<SUMOVehicle*, E3Values>::iterator i = myEnteredContainer.begin(); i != myEnteredContainer.end(); ++i) {
        meanHaltsPerVehicleWithin += (SUMOReal)(*i).second.haltings;
        meanIntervalHaltsPerVehicleWithin += (SUMOReal)(*i).second.intervalHaltings;
        const SUMOReal time = STEPS2TIME(stopTime) - (*i).second.entryTime;
        const SUMOReal timeWithin = MIN2(time, STEPS2TIME(stopTime - startTime));
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
    }
    myLastResetTime = stopTime;
    meanSpeedWithin = vehicleSumWithin != 0 ?  meanSpeedWithin / (SUMOReal) vehicleSumWithin : -1;
    meanHaltsPerVehicleWithin = vehicleSumWithin != 0 ? meanHaltsPerVehicleWithin / (SUMOReal) vehicleSumWithin : -1;
    meanDurationWithin = vehicleSumWithin != 0 ? meanDurationWithin / (SUMOReal) vehicleSumWithin : -1;
    meanIntervalSpeedWithin = vehicleSumWithin != 0 ?  meanIntervalSpeedWithin / (SUMOReal) vehicleSumWithin : -1;
    meanIntervalHaltsPerVehicleWithin = vehicleSumWithin != 0 ? meanIntervalHaltsPerVehicleWithin / (SUMOReal) vehicleSumWithin : -1;
    meanIntervalDurationWithin = vehicleSumWithin != 0 ? meanIntervalDurationWithin / (SUMOReal) vehicleSumWithin : -1;

    // write values
    dev << "meanTravelTime=\"" << meanTravelTime
        << "\" meanSpeed=\"" << meanSpeed
        << "\" meanHaltsPerVehicle=\"" << meanHaltsPerVehicle
        << "\" vehicleSum=\"" << vehicleSum
        << "\" meanSpeedWithin=\"" << meanSpeedWithin
        << "\" meanHaltsPerVehicleWithin=\"" << meanHaltsPerVehicleWithin
        << "\" meanDurationWithin=\"" << meanDurationWithin
        << "\" vehicleSumWithin=\"" << vehicleSumWithin
        << "\" meanIntervalSpeedWithin=\"" << meanIntervalSpeedWithin
        << "\" meanIntervalHaltsPerVehicleWithin=\"" << meanIntervalHaltsPerVehicleWithin
        << "\" meanIntervalDurationWithin=\"" << meanIntervalDurationWithin
        << "\"/>\n";
}


void
MSE3Collector::writeXMLDetectorProlog(OutputDevice& dev) const {
    dev.writeXMLHeader("e3Detector");
}


void
MSE3Collector::detectorUpdate(const SUMOTime step) {
    myCurrentMeanSpeed = 0;
    myCurrentHaltingsNumber = 0;
    myCurrentTouchedVehicles = 0;
    for (std::map<SUMOVehicle*, E3Values>::iterator pair = myEnteredContainer.begin(); pair != myEnteredContainer.end(); ++pair) {
        SUMOVehicle* veh = pair->first;
        E3Values& values = pair->second;
        values.hadUpdate = true;
        if (values.entryTime * 1000. >= step) {
            // vehicle entered at this time step
            SUMOReal fraction = step + 1. - values.entryTime;
            myCurrentMeanSpeed += fraction * veh->getSpeed();
            myCurrentTouchedVehicles += fraction;
            if (values.haltingBegin >= 0) {
                myCurrentHaltingsNumber++;
            }
            continue;
        }
        values.speedSum += veh->getSpeed() * TS;
        values.intervalSpeedSum += veh->getSpeed() * TS;
        myCurrentMeanSpeed += veh->getSpeed();
        myCurrentTouchedVehicles += 1;
        if (veh->getSpeed() < myHaltingSpeedThreshold) {
            if (values.haltingBegin == -1) {
                values.haltingBegin = step;
            }
            if (step - values.haltingBegin > myHaltingTimeThreshold) {
                values.haltings++;
                values.intervalHaltings++;
                myCurrentHaltingsNumber++;
            }
        } else {
            values.haltingBegin = -1;
        }
        myCurrentMeanSpeed /= myCurrentTouchedVehicles;
    }
}


SUMOReal
MSE3Collector::getCurrentMeanSpeed() const {
    SUMOReal ret = 0;
    if (myEnteredContainer.size() == 0) {
        return -1;
    }
    for (std::map<SUMOVehicle*, E3Values>::const_iterator pair = myEnteredContainer.begin(); pair != myEnteredContainer.end(); ++pair) {
        ret += (*pair).first->getSpeed();
    }
    return ret / SUMOReal(myEnteredContainer.size());
}


SUMOReal
MSE3Collector::getCurrentHaltingNumber() const {
    return myCurrentHaltingsNumber;
}


SUMOReal
MSE3Collector::getVehiclesWithin() const {
    return (SUMOReal) myEnteredContainer.size();
}


std::vector<std::string>
MSE3Collector::getCurrentVehicleIDs() const {
    std::vector<std::string> ret;
    for (std::map<SUMOVehicle*, E3Values>::const_iterator pair = myEnteredContainer.begin(); pair != myEnteredContainer.end(); ++pair) {
        ret.push_back((*pair).first->getID());
    }
    std::sort(ret.begin(), ret.end());
    return ret;
}


/****************************************************************************/

