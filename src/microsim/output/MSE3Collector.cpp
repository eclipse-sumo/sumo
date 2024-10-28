/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MSE3Collector.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Tue Dec 02 2003 22:17 CET
///
// A detector of vehicles passing an area between entry/exit points
/****************************************************************************/
#include <config.h>

#include <algorithm>
#ifdef HAVE_FOX
#include <utils/common/ScopedLocker.h>
#endif
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>
#include <microsim/transportables/MSTransportable.h>
#include <microsim/transportables/MSPModel.h>
#include "MSE3Collector.h"

//#define DEBUG_E3_NOTIFY_MOVE
//#define DEBUG_E3_NOTIFY_ENTER
//#define DEBUG_E3_NOTIFY_LEAVE
//#define DEBUG_E3_DETECTORUPDATE

//#define DEBUG_COND(obj) ((obj.getID() == ""))
//#define DEBUG_COND_VEH(veh) ((veh).getID() == "")
//#define DEBUG_COND_VEH(veh) ((veh).isSelected())
//#define DEBUG_COND(collector) (true)
//#define DEBUG_COND_VEH(veh) (true)


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * MSE3Collector::MSE3EntryReminder - definitions
 * ----------------------------------------------------------------------- */
MSE3Collector::MSE3EntryReminder::MSE3EntryReminder(
    const MSCrossSection& crossSection, MSE3Collector& collector) :
    MSMoveReminder(collector.getID() + "_entry", crossSection.myLane),
    myCollector(collector), myPosition(crossSection.myPosition) {
}


bool
MSE3Collector::MSE3EntryReminder::notifyEnter(SUMOTrafficObject& veh, Notification reason, const MSLane* enteredLane) {
#ifdef DEBUG_E3_NOTIFY_ENTER
    if (DEBUG_COND(myCollector) && DEBUG_COND_VEH(veh)) {
        std::cout << SIMTIME
                  << " MSE3EntryReminder::notifyEnter() (" << getDescription() << "on lane '" << myLane->getID() << "')"
                  << " vehicle '" << veh.getID() << "'"
                  << " enteredLane=" << enteredLane->getID()
                  << " reason=" << reason
                  << "\n";
    }
#endif
    if (reason != NOTIFICATION_JUNCTION) {
        const double posOnLane = veh.getBackPositionOnLane(enteredLane) + veh.getVehicleType().getLength();
        if (myLane == enteredLane && posOnLane > myPosition) {
#ifdef HAVE_FOX
            ScopedLocker<> lock(myCollector.myContainerMutex, MSGlobals::gNumSimThreads > 1);
#endif
            const auto& itVeh = myCollector.myEnteredContainer.find(&veh);
            if (itVeh == myCollector.myEnteredContainer.end() ||
                    itVeh->second.entryReminder != this) {
#ifdef DEBUG_E3_NOTIFY_ENTER
                if (DEBUG_COND(myCollector) && DEBUG_COND_VEH(veh)) {
                    std::cout << "  assume already known\n";
                }
#endif
                // if the vehicle changes into a covered section we assume it was already registered on another lane
                return false;
            }
        }
    }
    return true;
}


bool
MSE3Collector::MSE3EntryReminder::notifyMove(SUMOTrafficObject& veh, double oldPos,
        double newPos, double newSpeed) {
#ifdef DEBUG_E3_NOTIFY_MOVE
    if (DEBUG_COND(myCollector) && DEBUG_COND_VEH(veh)) {
        std::cout << SIMTIME
                  << " MSE3EntryReminder::notifyMove() (" << getDescription() << "on lane '" << myLane->getID() << "')"
                  << " vehicle '" << veh.getID() << "'"
                  << " entered. oldPos=" << oldPos << " newPos=" << newPos << " newSpeed=" << newSpeed
                  << " myPosition=" << myPosition
                  << "\n";
    }
#endif
#ifdef HAVE_FOX
    ScopedLocker<> lock(myCollector.myContainerMutex, MSGlobals::gNumSimThreads > 1);
#endif
    if ((myCollector.myEnteredContainer.find(&veh) == myCollector.myEnteredContainer.end() ||
            (veh.isPerson() && dynamic_cast<const MSTransportable&>(veh).getDirection() != MSPModel::FORWARD))
            && newPos > myPosition) {
        if (oldPos > myPosition) {
            // was behind the detector already in the last step
#ifdef DEBUG_E3_NOTIFY_MOVE
            if (DEBUG_COND(myCollector) && DEBUG_COND_VEH(veh)) {
                std::cout << "    already behind\n";
            }
#endif
            return false;
        } else {
            // entered in this step
            const double oldSpeed = veh.getPreviousSpeed();
            const double entryTime = STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep());
            assert(!MSGlobals::gSemiImplicitEulerUpdate || newSpeed != 0); // how could it move across the detector otherwise
            const double timeBeforeEnter = MSCFModel::passingTime(oldPos, myPosition, newPos, oldSpeed, newSpeed);
            const double fractionTimeOnDet = TS - timeBeforeEnter;
            myCollector.enter(veh, entryTime - fractionTimeOnDet, fractionTimeOnDet, this);
#ifdef DEBUG_E3_NOTIFY_MOVE
            if (DEBUG_COND(myCollector) && DEBUG_COND_VEH(veh)) {
                std::cout << "    enter\n";
            }
#endif
        }
    }
    return true;
}


bool
MSE3Collector::MSE3EntryReminder::notifyLeave(SUMOTrafficObject& veh, double, MSMoveReminder::Notification reason, const MSLane* /* enteredLane */) {
#ifdef DEBUG_E3_NOTIFY_LEAVE
    if (DEBUG_COND(myCollector) && DEBUG_COND_VEH(veh)) {
        std::cout << SIMTIME
                  << " MSE3EntryReminder::notifyLeave() (" << getDescription() << "on lane '" << myLane->getID() << "')"
                  << " vehicle '" << veh.getID() << "'"
                  << " reason=" << reason
                  << "\n";
    }
#endif
    if (reason >= MSMoveReminder::NOTIFICATION_ARRIVED) {
#ifdef HAVE_FOX
        ScopedLocker<> lock(myCollector.myContainerMutex, MSGlobals::gNumSimThreads > 1);
#endif
        if (myCollector.myEnteredContainer.erase(&veh) > 0) {
            if (!myCollector.myExpectArrival) {
                WRITE_WARNINGF("Vehicle '%' arrived inside % '%', time=%.", veh.getID(), toString(SUMO_TAG_E3DETECTOR), myCollector.getID(), time2string(SIMSTEP));
            }
        }
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
MSE3Collector::MSE3LeaveReminder::notifyEnter(SUMOTrafficObject& veh, Notification reason, const MSLane* enteredLane) {
#ifdef DEBUG_E3_NOTIFY_ENTER
    if (DEBUG_COND(myCollector) && DEBUG_COND_VEH(veh)) {
        std::cout << SIMTIME
                  << " MSE3LeaveReminder::notifyEnter() (" << getDescription() << "on lane '" << myLane->getID() << "')"
                  << " vehicle '" << veh.getID() << "'"
                  << " enteredLane=" << enteredLane->getID()
                  << " reason=" << reason
                  << "\n";
    }
#endif
    // this method does not access containers, so no locking here
    if (reason != NOTIFICATION_JUNCTION) {
        const double backPosOnLane = veh.getBackPositionOnLane(enteredLane);
        if (backPosOnLane > myPosition) {
            // if the vehicle changes into a covered section we assume it was already registered on another lane
            // however, if it is not fully past the detector we still need to track it
#ifdef DEBUG_E3_NOTIFY_ENTER
            if (DEBUG_COND(myCollector) && DEBUG_COND_VEH(veh)) {
                std::cout << "  assume already known\n";
            }
#endif
            return false;
        }
    }
    return true;
}


bool
MSE3Collector::MSE3LeaveReminder::notifyMove(SUMOTrafficObject& veh, double oldPos,
        double newPos, double newSpeed) {
#ifdef DEBUG_E3_NOTIFY_MOVE
    if (DEBUG_COND(myCollector) && DEBUG_COND_VEH(veh)) {
        std::cout << SIMTIME
                  << " MSE3LeaveReminder::notifyMove() (" << getDescription() << " on lane '" << myLane->getID() << "')"
                  << " vehicle '" << veh.getID() << "'"
                  << " entered. oldPos=" << oldPos << " newPos=" << newPos << " newSpeed=" << newSpeed
                  << " myPosition=" << myPosition
                  << "\n";
    }
#endif
    if (newPos < myPosition) {
        // crossSection not yet reached
        return true;
    }
#ifdef HAVE_FOX
    ScopedLocker<> lock(myCollector.myContainerMutex, MSGlobals::gNumSimThreads > 1);
#endif
    const double oldSpeed = veh.getPreviousSpeed();
    if (oldPos < myPosition) {
        assert(!MSGlobals::gSemiImplicitEulerUpdate || newSpeed != 0); // how could it move across the detector otherwise
        const double timeBeforeLeave = MSCFModel::passingTime(oldPos, myPosition, newPos, oldSpeed, newSpeed);
//        const double leaveTimeFront = SIMTIME - TS + (myPosition - oldPos) / newSpeed;
        const double leaveTimeFront = SIMTIME - TS + timeBeforeLeave;
        myCollector.leaveFront(veh, leaveTimeFront);
#ifdef DEBUG_E3_NOTIFY_MOVE
        if (DEBUG_COND(myCollector) && DEBUG_COND_VEH(veh)) {
            std::cout << "    leaveFront\n";
        }
#endif
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
#ifdef DEBUG_E3_NOTIFY_MOVE
    if (DEBUG_COND(myCollector) && DEBUG_COND_VEH(veh)) {
        std::cout << "    leave\n";
    }
#endif
    return false;
}


bool
MSE3Collector::MSE3LeaveReminder::notifyLeave(SUMOTrafficObject&  veh, double /* lastPos */, MSMoveReminder::Notification reason, const MSLane* enteredLane) {
#ifdef DEBUG_E3_NOTIFY_LEAVE
    if (DEBUG_COND(myCollector) && DEBUG_COND_VEH(veh)) {
        std::cout << SIMTIME
                  << " MSE3LeaveReminder::notifyLeave() (" << getDescription() << "on lane '" << myLane->getID() << "')"
                  << " vehicle '" << veh.getID() << "'"
                  << " reason=" << reason
                  << "\n";
    }
#endif
    if (reason == MSMoveReminder::NOTIFICATION_LANE_CHANGE && &enteredLane->getEdge() == &myLane->getEdge()) {
        // keep the detector when changing while still on the exit detector but already on a new lane (#4803)
#ifdef DEBUG_E3_NOTIFY_LEAVE
        if (DEBUG_COND(myCollector) && DEBUG_COND_VEH(veh)) {
            std::cout << "  remove reminder, keep in container\n";
        }
#endif
        return false;
    }
#ifdef HAVE_FOX
    ScopedLocker<> lock(myCollector.myContainerMutex, MSGlobals::gNumSimThreads > 1);
#endif
    if (reason == MSMoveReminder::NOTIFICATION_TELEPORT) {
        WRITE_WARNINGF("Vehicle '%' teleported from % '%'.", veh.getID(), toString(SUMO_TAG_E3DETECTOR), myCollector.getID());
        myCollector.myEnteredContainer.erase(&veh);
        return false;
    }
    if (reason >= MSMoveReminder::NOTIFICATION_ARRIVED) {
        if (myCollector.myEnteredContainer.erase(&veh) > 0) {
            if (!myCollector.myExpectArrival) {
                WRITE_WARNINGF("Vehicle '%' arrived inside % '%', time=%.", veh.getID(), toString(SUMO_TAG_E3DETECTOR), myCollector.getID(), time2string(SIMSTEP));
            }
        }
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
                             const std::string name, const std::string& vTypes,
                             const std::string& nextEdges,
                             int detectPersons,
                             bool openEntry, bool expectArrival) :
    MSDetectorFileOutput(id, vTypes, nextEdges, detectPersons),
    myName(name),
    myEntries(entries),
    myExits(exits),
    myHaltingTimeThreshold(haltingTimeThreshold), myHaltingSpeedThreshold(haltingSpeedThreshold),
    myCurrentMeanSpeed(0), myCurrentHaltingsNumber(0),
    myLastMeanTravelTime(0), myLastMeanHaltsPerVehicle(0), myLastMeanTimeLoss(0), myLastVehicleSum(0),
    myLastResetTime(-1), myOpenEntry(openEntry), myExpectArrival(expectArrival) {
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
MSE3Collector::enter(const SUMOTrafficObject& veh, const double entryTimestep, const double fractionTimeOnDet, MSE3EntryReminder* entryReminder, bool isBackward) {
    if (myDetectPersons > (int)PersonMode::WALK && !veh.isPerson()) {
        const MSBaseVehicle& v = dynamic_cast<const MSBaseVehicle&>(veh);
        for (MSTransportable* p : v.getPersons()) {
            enter(*p, entryTimestep, fractionTimeOnDet, entryReminder);
        }
        return;
    }
    if (!vehicleApplies(veh)) {
        return;
    }
    if (veh.isPerson() && !isBackward && dynamic_cast<const MSTransportable&>(veh).getDirection() != MSPModel::FORWARD) {
        // walking backward over an entry detector means "leaving"
        // std::cout << veh.getID() << " leave at entryDetector\n";
        leave(veh, entryTimestep, fractionTimeOnDet, true);
        return;
    }
    if (myEnteredContainer.find(&veh) != myEnteredContainer.end()) {
        WRITE_WARNINGF("Vehicle '%' reentered % '%', time=%.", veh.getID(), toString(SUMO_TAG_E3DETECTOR), getID(), time2string(SIMSTEP));
        return;
    }
#ifdef DEBUG_E3_NOTIFY_ENTER
    std::cout << veh.getID() << " enters\n";
#endif
    const double speedFraction = veh.getSpeed() * fractionTimeOnDet;
    E3Values v;
    v.entryTime = entryTimestep;
    v.frontLeaveTime = 0;
    v.backLeaveTime = 0;
    v.speedSum = speedFraction;
    v.haltingBegin = veh.getSpeed() < myHaltingSpeedThreshold ? TIME2STEPS(entryTimestep) : -1;
    v.intervalSpeedSum = entryTimestep >= STEPS2TIME(myLastResetTime) ? speedFraction : 0;
    v.haltings = 0;
    v.intervalHaltings = 0;
    if (veh.getSpeed() < myHaltingSpeedThreshold) {
        if (TIME2STEPS(fractionTimeOnDet) > myHaltingTimeThreshold) {
            v.haltings++;
            v.intervalHaltings++;
        }
    }
    v.hadUpdate = false;
    if (!MSGlobals::gUseMesoSim && veh.isVehicle()) {
        v.timeLoss = dynamic_cast<const MSVehicle&>(veh).getTimeLoss();
        v.intervalTimeLoss = v.timeLoss;
    }
    v.entryReminder = entryReminder;
    myEnteredContainer[&veh] = v;
}


void
MSE3Collector::leaveFront(const SUMOTrafficObject& veh, const double leaveTimestep) {
    if (myDetectPersons > (int)PersonMode::WALK && !veh.isPerson()) {
        const MSBaseVehicle& v = dynamic_cast<const MSBaseVehicle&>(veh);
        for (MSTransportable* p : v.getPersons()) {
            leaveFront(*p, leaveTimestep);
        }
        return;
    }
    if (!vehicleApplies(veh)) {
        return;
    }
    if (myEnteredContainer.find(&veh) == myEnteredContainer.end()) {
        if (!myOpenEntry && veh.isVehicle()) {
            WRITE_WARNINGF("Vehicle '%' left % '%' without entering it, time=%.", veh.getID(), toString(SUMO_TAG_E3DETECTOR), getID(), time2string(SIMSTEP));
        }
    } else {
        myEnteredContainer[&veh].frontLeaveTime = leaveTimestep;
    }
}


void
MSE3Collector::leave(const SUMOTrafficObject& veh, const double leaveTimestep, const double fractionTimeOnDet, bool isBackward) {
    if (myDetectPersons > (int)PersonMode::WALK && !veh.isPerson()) {
        const MSBaseVehicle& v = dynamic_cast<const MSBaseVehicle&>(veh);
        for (MSTransportable* p : v.getPersons()) {
            leave(*p, leaveTimestep, fractionTimeOnDet);
        }
        return;
    }
    if (!vehicleApplies(veh)) {
        return;
    }
    if (veh.isPerson() && !isBackward && dynamic_cast<const MSTransportable&>(veh).getDirection() != MSPModel::FORWARD) {
        // walking backward over an exit detector means "entering"
        // std::cout << veh.getID() << " enter at exitDetector\n";
        enter(veh, leaveTimestep, fractionTimeOnDet, nullptr, true);
        return;
    }
    if (myEnteredContainer.find(&veh) == myEnteredContainer.end()) {
        if (!myOpenEntry && veh.isVehicle()) {
            WRITE_WARNINGF("Vehicle '%' left % '%' without entering it, time=%.", veh.getID(), toString(SUMO_TAG_E3DETECTOR), getID(), time2string(SIMSTEP));
        }
    } else {
#ifdef DEBUG_E3_NOTIFY_LEAVE
        std::cout << veh.getID() << " leaves\n";
#endif
        E3Values values = myEnteredContainer[&veh];
        values.backLeaveTime = leaveTimestep;
        const double speedFraction = veh.getSpeed() * (TS - fractionTimeOnDet);
        values.speedSum -= speedFraction;
        values.intervalSpeedSum -= speedFraction;
        if (MSGlobals::gUseMesoSim || !veh.isVehicle()) {
            // not yet supported
            values.timeLoss = 0;
            if (isBackward) {
                // leaveFront may not have been called
                values.frontLeaveTime = leaveTimestep;
            }
        } else {
            // timeLoss was initialized when entering
            values.timeLoss = dynamic_cast<const MSVehicle&>(veh).getTimeLoss() - values.timeLoss;
        }
        myEnteredContainer.erase(&veh);
        myLeftContainer.push_back(values);
    }
}


void
MSE3Collector::writeXMLOutput(OutputDevice& dev,
                              SUMOTime startTime, SUMOTime stopTime) {
    dev << "   <interval begin=\"" << time2string(startTime) << "\" end=\"" << time2string(stopTime) << "\" " << "id=\"" << myID << "\" ";
    // collect values about vehicles that have left the area
    myLastVehicleSum = (int) myLeftContainer.size();
    myLastMeanTravelTime = 0;
    double meanOverlapTravelTime = 0.;
    double meanSpeed = 0.;
    myLastMeanHaltsPerVehicle = 0;
    myLastMeanTimeLoss = 0.;
    for (const E3Values& values : myLeftContainer) {
        myLastMeanHaltsPerVehicle += (double)values.haltings;
        myLastMeanTravelTime += values.frontLeaveTime - values.entryTime;
        const double steps = values.backLeaveTime - values.entryTime;
        meanOverlapTravelTime += steps;
        meanSpeed += (values.speedSum / steps);
        myLastMeanTimeLoss += STEPS2TIME(values.timeLoss);
    }
    myLastMeanTravelTime = myLastVehicleSum != 0 ? myLastMeanTravelTime / (double)myLastVehicleSum : -1;
    meanOverlapTravelTime = myLastVehicleSum != 0 ? meanOverlapTravelTime / (double)myLastVehicleSum : -1;
    meanSpeed = myLastVehicleSum != 0 ? meanSpeed / (double)myLastVehicleSum : -1;
    myLastMeanHaltsPerVehicle = myLastVehicleSum != 0 ? myLastMeanHaltsPerVehicle / (double) myLastVehicleSum : -1;
    myLastMeanTimeLoss = myLastVehicleSum != 0 ? myLastMeanTimeLoss / (double) myLastVehicleSum : -1;
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
    for (std::map<const SUMOTrafficObject*, E3Values>::iterator i = myEnteredContainer.begin(); i != myEnteredContainer.end(); ++i) {
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

        if (!MSGlobals::gUseMesoSim && i->first->isVehicle()) {
            const SUMOTime currentTimeLoss = dynamic_cast<const MSVehicle*>(i->first)->getTimeLoss();
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
    dev << "meanTravelTime=\"" << myLastMeanTravelTime
        << "\" meanOverlapTravelTime=\"" << meanOverlapTravelTime
        << "\" meanSpeed=\"" << meanSpeed
        << "\" meanHaltsPerVehicle=\"" << myLastMeanHaltsPerVehicle
        << "\" meanTimeLoss=\"" << myLastMeanTimeLoss
        << "\" vehicleSum=\"" << myLastVehicleSum
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
MSE3Collector::notifyMovePerson(MSTransportable* p, MSMoveReminder* rem, double detPos, int dir, double pos) {
    if (personApplies(*p, dir)) {
        const double newSpeed = p->getSpeed();
        const double newPos = (dir == MSPModel::FORWARD
                               ? pos
                               // position relative to detector end position
                               : detPos - (pos - detPos));
        const double oldPos = newPos - SPEED2DIST(newSpeed);
        if (oldPos - p->getVehicleType().getLength() <= detPos) {
            rem->notifyMove(*p, oldPos, newPos, newSpeed);
        }
    }
}


void
MSE3Collector::detectorUpdate(const SUMOTime step) {

    if (myDetectPersons != (int)PersonMode::NONE) {
        for (auto rem : myEntryReminders) {
            const MSLane* lane = rem->getLane();
            if (lane->hasPedestrians()) {
                for (MSTransportable* p : lane->getEdge().getPersons()) {
                    if (p->getLane() == lane && vehicleApplies(*p)) {
                        notifyMovePerson(p, rem, rem->getPosition(), p->getDirection(), p->getPositionOnLane());
                    }
                }
            }
        }
        for (auto rem : myLeaveReminders) {
            const MSLane* lane = rem->getLane();
            if (lane->hasPedestrians()) {
                for (MSTransportable* p : lane->getEdge().getPersons()) {
                    if (p->getLane() == lane && vehicleApplies(*p)) {
                        notifyMovePerson(p, rem, rem->getPosition(), p->getDirection(), p->getPositionOnLane());
                    }
                }
            }
        }
    }

    myCurrentMeanSpeed = 0;
    myCurrentHaltingsNumber = 0;
    for (std::map<const SUMOTrafficObject*, E3Values>::iterator pair = myEnteredContainer.begin(); pair != myEnteredContainer.end(); ++pair) {
        const SUMOTrafficObject* veh = pair->first;
#ifdef DEBUG_E3_DETECTORUPDATE
        //if (DEBUG_COND(*this) && DEBUG_COND_VEH(*veh)) {
        if (DEBUG_COND(*this)) {
            std::cout << SIMTIME << " vehPtr=" << veh << "\n";
            std::cout << "       veh=" << veh->getID() << "\n";
        }
#endif
        E3Values& values = pair->second;
        myCurrentMeanSpeed += veh->getSpeed();
        values.hadUpdate = true;
        values.speedSum += veh->getSpeed() * TS;
        values.intervalSpeedSum += veh->getSpeed() * TS;
        if (veh->getSpeed() < myHaltingSpeedThreshold) {
            if (values.haltingBegin == -1) {
                values.haltingBegin = step;
            }
            SUMOTime haltingDuration = step - values.haltingBegin;
            if (haltingDuration >= myHaltingTimeThreshold
                    && haltingDuration < (myHaltingTimeThreshold + DELTA_T)) {
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
        myCurrentMeanSpeed /= (double)myEnteredContainer.size();
    }
}


const CrossSectionVector&
MSE3Collector::getEntries() const {
    return myEntries;
}


const CrossSectionVector&
MSE3Collector::getExits() const {
    return myExits;
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
    for (std::map<const SUMOTrafficObject*, E3Values>::const_iterator pair = myEnteredContainer.begin(); pair != myEnteredContainer.end(); ++pair) {
        ret.push_back((*pair).first->getID());
    }
    std::sort(ret.begin(), ret.end());
    return ret;
}

void
MSE3Collector::clearState(SUMOTime /* step */) {
    myEnteredContainer.clear();
    myLeftContainer.clear();
}

/****************************************************************************/
