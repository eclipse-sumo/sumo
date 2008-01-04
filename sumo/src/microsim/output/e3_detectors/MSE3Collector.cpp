/****************************************************************************/
/// @file    MSE3Collector.cpp
/// @author  Christian Roessel
/// @date    Tue Dec 02 2003 22:17 CET
/// @version $Id$
///
// A detector of vehicles passing an area between entry/exit points
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include "MSE3Collector.h"

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
    const MSCrossSection &crossSection, MSE3Collector& collector)
        : MSMoveReminder(crossSection.myLane),
        myCollector(collector), myPosition(crossSection.myPosition)
{}


bool
MSE3Collector::MSE3EntryReminder::isStillActive(MSVehicle& veh, SUMOReal oldPos,
        SUMOReal newPos, SUMOReal newSpeed)
{
    if (newPos <= myPosition) {
        // crossSection not yet reached
        return true;
    }
    SUMOReal entryTimestep = (SUMOReal) 
        ((SUMOReal) MSNet::getInstance()->getCurrentTimeStep() + ((myPosition - oldPos) / newSpeed));
    myCollector.enter(veh, entryTimestep);
    return false;
}


void
MSE3Collector::MSE3EntryReminder::dismissByLaneChange(MSVehicle&)
{
    // nothing to do for E3
}


bool
MSE3Collector::MSE3EntryReminder::isActivatedByEmitOrLaneChange(MSVehicle& veh)
{
    return veh.getPositionOnLane() <= myPosition;
}



/* -------------------------------------------------------------------------
 * MSE3Collector::MSE3LeaveReminder - definitions
 * ----------------------------------------------------------------------- */
MSE3Collector::MSE3LeaveReminder::MSE3LeaveReminder(
    const MSCrossSection &crossSection, MSE3Collector& collector)
        : MSMoveReminder(crossSection.myLane),
        myCollector(collector), myPosition(crossSection.myPosition)
{}


bool
MSE3Collector::MSE3LeaveReminder::isStillActive(MSVehicle& veh, SUMOReal oldPos,
        SUMOReal newPos, SUMOReal newSpeed)
{
    if (newPos <= myPosition) {
        // crossSection not yet reached
        return true;
    }
    // crossSection left
    SUMOReal leaveTimestep =  
        ((SUMOReal) MSNet::getInstance()->getCurrentTimeStep() + ((myPosition - oldPos) / newSpeed));
    myCollector.leave(veh, leaveTimestep);
    return false;
}


void
MSE3Collector::MSE3LeaveReminder::dismissByLaneChange(MSVehicle&)
{
    // nothing to do for E3
}


bool
MSE3Collector::MSE3LeaveReminder::isActivatedByEmitOrLaneChange(MSVehicle& veh)
{
    return veh.getPositionOnLane() - veh.getLength() <= myPosition;
}



/* -------------------------------------------------------------------------
 * MSE3Collector - definitions
 * ----------------------------------------------------------------------- */
MSE3Collector::MSE3Collector(const std::string &id,
                             const CrossSectionVector &entries,
                             const CrossSectionVector &exits,
                             MetersPerSecond haltingSpeedThreshold)
        : myID(id), myEntries(entries), myExits(exits),
        // !dk! kept for later use: haltingTimeThresholdM(MSUnit::getInstance()->getSteps(haltingTimeThreshold)),
        myHaltingSpeedThreshold(haltingSpeedThreshold),
        myCurrentMeanSpeed(0), myCurrentHaltingsNumber(0), myCurrentTouchedVehicles(0)

{
    // Set MoveReminders to entries and exits
    for (CrossSectionVectorConstIt crossSec1 = entries.begin(); crossSec1!=entries.end(); ++crossSec1) {
        myEntryReminders.push_back(new MSE3EntryReminder(*crossSec1, *this));
    }
    for (CrossSectionVectorConstIt crossSec2 = exits.begin(); crossSec2!=exits.end(); ++crossSec2) {
        myLeaveReminders.push_back(new MSE3LeaveReminder(*crossSec2, *this));
    }
    reset();
}


MSE3Collector::~MSE3Collector(void)
{
    for (std::map<MSVehicle*, E3Values>::iterator pair = myEnteredContainer.begin(); pair!=myEnteredContainer.end(); ++pair) {
        pair->first->quitRemindedLeft(this);
    }
}


void 
MSE3Collector::reset()
{
    myLeftContainer.clear();
}



void
MSE3Collector::enter(MSVehicle& veh, SUMOReal entryTimestep)
{
    veh.quitRemindedEntered(this);
    E3Values v;
    v.entryTime = entryTimestep;
    v.leaveTime = 0;
    v.speedSum = 0;
    v.haltings = 0;
    v.samples = 0;
    if (myEnteredContainer.find(&veh)!=myEnteredContainer.end()) {
        MsgHandler::getWarningInstance()->inform("Vehicle '" + veh.getID() + "' reentered E3-detector '" + getID() + "'.");
    }
    myEnteredContainer[&veh] = v;
}


void
MSE3Collector::leave(MSVehicle& veh, SUMOReal leaveTimestep)
{
    if (myEnteredContainer.find(&veh)==myEnteredContainer.end()) {
        MsgHandler::getWarningInstance()->inform("Vehicle '" + veh.getID() + "' left E3-detector '" + getID() + "' before entering it.");
    } else {
        E3Values v = myEnteredContainer[&veh];
        v.leaveTime = leaveTimestep;
        SUMOReal leaveTimestepFraction = leaveTimestep - (SUMOReal) ((int) leaveTimestep);
        v.speedSum += (veh.getSpeed() * leaveTimestepFraction);
        if (veh.getSpeed() < myHaltingSpeedThreshold) {
            v.haltings++;
        }
        myEnteredContainer.erase(&veh);
        myLeftContainer[&veh] = v;
    }
    veh.quitRemindedLeft(this);
}


const std::string&
MSE3Collector::getID() const
{
    return myID;
}


void
MSE3Collector::removeOnTripEnd(MSVehicle *veh)
{
    if (myEnteredContainer.find(veh)==myEnteredContainer.end()) {
        MsgHandler::getWarningInstance()->inform("Vehicle '" + veh->getID() + "' left E3-detector '" + getID() + "' before entering it.");
    } else {
        myEnteredContainer.erase(veh);
    }
}


void
MSE3Collector::writeXMLOutput(OutputDevice &dev,
                              SUMOTime startTime, SUMOTime stopTime)
{
    dev<<"   <interval begin=\""<<startTime<<"\" end=\""<< stopTime<<"\" "<<"id=\""<<myID<<"\" ";
    // collect values
    unsigned vehicleSum = myLeftContainer.size();
    SUMOReal meanTravelTime = 0.;
    SUMOReal meanSpeed = 0.;
    SUMOReal meanHaltsPerVehicle = 0.;
    //
    for (std::map<MSVehicle*, E3Values>::iterator i=myLeftContainer.begin(); i!=myLeftContainer.end(); ++i) {
        meanHaltsPerVehicle += (SUMOReal) (*i).second.haltings;
        SUMOReal steps = (*i).second.leaveTime-(*i).second.entryTime;
        meanTravelTime += steps;
        meanSpeed += (SUMOReal)((*i).second.speedSum / steps);
    }
    meanTravelTime = vehicleSum!=0 ? meanTravelTime / (SUMOReal) vehicleSum : -1;
    meanSpeed = vehicleSum!=0 ?  meanSpeed / (SUMOReal) vehicleSum : -1;
    meanHaltsPerVehicle = vehicleSum!=0 ? meanHaltsPerVehicle / (SUMOReal) vehicleSum : -1;
    // write values
    dev<<"meanTravelTime=\""<<meanTravelTime<<"\" meanSpeed=\""<<meanSpeed<<
    "\" meanHaltsPerVehicle=\""<<meanHaltsPerVehicle<<
    "\" vehicleSum=\""<<vehicleSum<<"\"/>\n";
    // clear container
    myLeftContainer.clear();
}


void
MSE3Collector::writeXMLDetectorProlog(OutputDevice &dev) const
{
    dev.writeXMLHeader("e3-detector");
}


void
MSE3Collector::update(SUMOTime execTime)
{
    myCurrentMeanSpeed = 0;
    myCurrentHaltingsNumber = 0;
    myCurrentTouchedVehicles = 0;
    for (std::map<MSVehicle*, E3Values>::iterator pair = myEnteredContainer.begin(); pair!=myEnteredContainer.end(); ++pair) {
        MSVehicle* veh = pair->first;
        E3Values& values = pair->second;
        if(values.samples!=0) {
            values.speedSum += veh->getSpeed();
            myCurrentMeanSpeed += veh->getSpeed();
            myCurrentTouchedVehicles += 1;
        } else {
            SUMOReal entryTimestepFraction = (SUMOReal) ((int) values.entryTime) + 1 - values.entryTime;
            values.speedSum += (veh->getSpeed() * entryTimestepFraction);
            myCurrentMeanSpeed += (veh->getSpeed() * entryTimestepFraction);
            myCurrentTouchedVehicles += entryTimestepFraction;
        }
        if (veh->getSpeed() < myHaltingSpeedThreshold) {
            values.haltings++;
            myCurrentHaltingsNumber++;
        }
        values.samples++;
        myCurrentMeanSpeed /= myCurrentTouchedVehicles;
        /*
        halting.myPosition += veh->getMovedDistance();
        if (veh->getSpeed() >= speedThresholdM) {
            halting.timeBelowSpeedThresholdM = 0;
            halting.isHaltingM = false;
            halting.haltingDurationM = 0.0;
        } else {
            halting.timeBelowSpeedThresholdM++;
            if (halting.timeBelowSpeedThresholdM > timeThresholdM) {
                if (! halting.isHaltingM) {
                    // beginning of new halt detected
                    halting.isHaltingM = true;
                    // time to detect halting contributes to
                    // halting-duration
                    halting.haltingDurationM = halting.timeBelowSpeedThresholdM++;
                    halting.nHalts++;
                } else {
                    halting.haltingDurationM++;
                }
            }
        }
        */
    }
}


SUMOReal 
MSE3Collector::getCurrentMeanSpeed() const
{
    return myCurrentMeanSpeed;
}


SUMOReal 
MSE3Collector::getCurrentHaltingNumber() const
{
    return myCurrentHaltingsNumber;
}


SUMOReal 
MSE3Collector::getCurrentTouchedVehicles() const
{
    return myCurrentTouchedVehicles;
}


/****************************************************************************/

