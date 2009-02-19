/****************************************************************************/
/// @file    MSE3Collector.cpp
/// @author  Christian Roessel
/// @date    Tue Dec 02 2003 22:17 CET
/// @version $Id$
///
// A detector of vehicles passing an area between entry/exit points
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * MSE3Collector::MSE3EntryReminder - definitions
 * ----------------------------------------------------------------------- */
MSE3Collector::MSE3EntryReminder::MSE3EntryReminder(
    const MSCrossSection &crossSection, MSE3Collector& collector) throw()
        : MSMoveReminder(crossSection.myLane),
        myCollector(collector), myPosition(crossSection.myPosition)
{}


bool
MSE3Collector::MSE3EntryReminder::isStillActive(MSVehicle& veh, SUMOReal oldPos,
        SUMOReal newPos, SUMOReal newSpeed) throw()
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
MSE3Collector::MSE3EntryReminder::dismissByLaneChange(MSVehicle&) throw()
{
    // nothing to do for E3
}


bool
MSE3Collector::MSE3EntryReminder::isActivatedByEmitOrLaneChange(MSVehicle& veh, bool isEmit) throw()
{
    return veh.getPositionOnLane() <= myPosition;
}



/* -------------------------------------------------------------------------
 * MSE3Collector::MSE3LeaveReminder - definitions
 * ----------------------------------------------------------------------- */
MSE3Collector::MSE3LeaveReminder::MSE3LeaveReminder(
    const MSCrossSection &crossSection, MSE3Collector& collector) throw()
        : MSMoveReminder(crossSection.myLane),
        myCollector(collector), myPosition(crossSection.myPosition)
{}


bool
MSE3Collector::MSE3LeaveReminder::isStillActive(MSVehicle& veh, SUMOReal oldPos,
        SUMOReal newPos, SUMOReal newSpeed) throw()
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
MSE3Collector::MSE3LeaveReminder::dismissByLaneChange(MSVehicle&) throw()
{
    // nothing to do for E3
}


bool
MSE3Collector::MSE3LeaveReminder::isActivatedByEmitOrLaneChange(MSVehicle& veh, bool isEmit) throw()
{
    return veh.getPositionOnLane() - veh.getVehicleType().getLength() <= myPosition;
}



/* -------------------------------------------------------------------------
 * MSE3Collector - definitions
 * ----------------------------------------------------------------------- */
MSE3Collector::MSE3Collector(const std::string &id,
                             const CrossSectionVector &entries,
                             const CrossSectionVector &exits,
                             MetersPerSecond haltingSpeedThreshold,
                             SUMOTime haltingTimeThreshold) throw()
        : myID(id), myEntries(entries), myExits(exits),
        myHaltingTimeThreshold(haltingTimeThreshold), myHaltingSpeedThreshold(haltingSpeedThreshold),
        myCurrentMeanSpeed(0), myCurrentHaltingsNumber(0), myCurrentTouchedVehicles(0),
        myLastResetTime(-1)
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


MSE3Collector::~MSE3Collector() throw()
{
    for (std::map<MSVehicle*, E3Values>::iterator pair = myEnteredContainer.begin(); pair!=myEnteredContainer.end(); ++pair) {
        pair->first->quitRemindedLeft(this);
    }
    for (vector<MSE3EntryReminder*>::iterator i = myEntryReminders.begin(); i!=myEntryReminders.end(); ++i) {
        delete *i;
    }
    for (vector<MSE3LeaveReminder*>::iterator i = myLeaveReminders.begin(); i!=myLeaveReminders.end(); ++i) {
        delete *i;
    }
}


void
MSE3Collector::reset() throw()
{
    myLeftContainer.clear();
}



void
MSE3Collector::enter(MSVehicle& veh, SUMOReal entryTimestep) throw()
{
    if (myEnteredContainer.find(&veh)!=myEnteredContainer.end()) {
        MsgHandler::getWarningInstance()->inform("Vehicle '" + veh.getID() + "' reentered E3-detector '" + getID() + "'.");
        return;
    }
    veh.quitRemindedEntered(this);
    E3Values v;
    v.entryTime = entryTimestep;
    v.leaveTime = 0;
    v.speedSum = 0;
    v.haltings = 0;
    v.samples = 0;
    v.haltingBegin = -1;
    v.intervalSpeedSum = 0;
    v.intervalHaltings = 0;
    myEnteredContainer[&veh] = v;
}


void
MSE3Collector::leave(MSVehicle& veh, SUMOReal leaveTimestep) throw()
{
    if (myEnteredContainer.find(&veh)==myEnteredContainer.end()) {
        MsgHandler::getWarningInstance()->inform("Vehicle '" + veh.getID() + "' left E3-detector '" + getID() + "' before entering it.");
    } else {
        E3Values values = myEnteredContainer[&veh];
        values.leaveTime = leaveTimestep;
        SUMOReal leaveTimestepFraction = leaveTimestep - (SUMOReal)((int) leaveTimestep);
        SUMOReal speedFraction = (veh.getSpeed() * leaveTimestepFraction);
        values.speedSum += speedFraction;
        values.intervalSpeedSum += speedFraction;
        if (veh.getSpeed() < myHaltingSpeedThreshold && values.haltingBegin!=-1 && leaveTimestep-values.haltingBegin>myHaltingTimeThreshold) {
            values.haltings++;
            values.intervalHaltings++;
        }
        myEnteredContainer.erase(&veh);
        myLeftContainer[&veh] = values;
    }
    veh.quitRemindedLeft(this);
}


const std::string&
MSE3Collector::getID() const throw()
{
    return myID;
}


void
MSE3Collector::removeOnTripEnd(MSVehicle *veh) throw()
{
    if (myEnteredContainer.find(veh)==myEnteredContainer.end()) {
        MsgHandler::getWarningInstance()->inform("Vehicle '" + veh->getID() + "' left E3-detector '" + getID() + "' before entering it.");
    } else {
        myEnteredContainer.erase(veh);
    }
}


void
MSE3Collector::writeXMLOutput(OutputDevice &dev,
                              SUMOTime startTime, SUMOTime stopTime) throw(IOError)
{
    dev<<"   <interval begin=\""<<startTime<<"\" end=\""<< stopTime<<"\" "<<"id=\""<<myID<<"\" ";
    // collect values about vehicles that have left the area
    unsigned vehicleSum = (unsigned) myLeftContainer.size();
    SUMOReal meanTravelTime = 0.;
    SUMOReal meanSpeed = 0.;
    SUMOReal meanHaltsPerVehicle = 0.;
    for (std::map<MSVehicle*, E3Values>::iterator i=myLeftContainer.begin(); i!=myLeftContainer.end(); ++i) {
        meanHaltsPerVehicle += (SUMOReal)(*i).second.haltings;
        SUMOReal steps = (*i).second.leaveTime-(*i).second.entryTime;
        meanTravelTime += steps;
        meanSpeed += ((*i).second.speedSum / steps);
    }
    meanTravelTime = vehicleSum!=0 ? meanTravelTime / (SUMOReal) vehicleSum : -1;
    meanSpeed = vehicleSum!=0 ?  meanSpeed / (SUMOReal) vehicleSum : -1;
    meanHaltsPerVehicle = vehicleSum!=0 ? meanHaltsPerVehicle / (SUMOReal) vehicleSum : -1;
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
    for (std::map<MSVehicle*, E3Values>::iterator i=myEnteredContainer.begin(); i!=myEnteredContainer.end(); ++i) {
        meanHaltsPerVehicleWithin += (SUMOReal)(*i).second.haltings;
        meanIntervalHaltsPerVehicleWithin += (SUMOReal)(*i).second.intervalHaltings;
        SUMOReal steps = (SUMOReal) stopTime - (*i).second.entryTime + (SUMOReal) 1.;
        SUMOReal stepsWithin = (*i).second.entryTime>startTime ? steps : (SUMOReal)(stopTime - startTime + 1.);
        meanSpeedWithin += ((*i).second.speedSum / steps);
        meanIntervalSpeedWithin += ((*i).second.intervalSpeedSum / stepsWithin);
        meanDurationWithin += steps;
        meanIntervalDurationWithin += stepsWithin;
        // reset interval values
        (*i).second.intervalHaltings = 0;
        (*i).second.intervalSpeedSum = 0;
    }
    myLastResetTime = stopTime;
    meanSpeedWithin = vehicleSumWithin!=0 ?  meanSpeedWithin / (SUMOReal) vehicleSumWithin : -1;
    meanHaltsPerVehicleWithin = vehicleSumWithin!=0 ? meanHaltsPerVehicleWithin / (SUMOReal) vehicleSumWithin : -1;
    meanDurationWithin = vehicleSumWithin!=0 ? meanDurationWithin / (SUMOReal) vehicleSumWithin : -1;
    meanIntervalSpeedWithin = vehicleSumWithin!=0 ?  meanIntervalSpeedWithin / (SUMOReal) vehicleSumWithin : -1;
    meanIntervalHaltsPerVehicleWithin = vehicleSumWithin!=0 ? meanIntervalHaltsPerVehicleWithin / (SUMOReal) vehicleSumWithin : -1;
    meanIntervalDurationWithin = vehicleSumWithin!=0 ? meanIntervalDurationWithin / (SUMOReal) vehicleSumWithin : -1;

    // write values
    dev<<"meanTravelTime=\""<<meanTravelTime
    <<"\" meanSpeed=\""<<meanSpeed
    <<"\" meanHaltsPerVehicle=\""<<meanHaltsPerVehicle
    <<"\" vehicleSum=\""<<vehicleSum
    <<"\" meanSpeedWithin=\""<<meanSpeedWithin
    <<"\" meanHaltsPerVehicleWithin=\""<<meanHaltsPerVehicleWithin
    <<"\" meanDurationWithin=\""<<meanDurationWithin
    <<"\" vehicleSumWithin=\""<<vehicleSumWithin
    <<"\" meanIntervalSpeedWithin=\""<<meanIntervalSpeedWithin
    <<"\" meanIntervalHaltsPerVehicleWithin=\""<<meanIntervalHaltsPerVehicleWithin
    <<"\" meanIntervalDurationWithin=\""<<meanIntervalDurationWithin
    <<"\"/>\n";
}


void
MSE3Collector::writeXMLDetectorProlog(OutputDevice &dev) const throw(IOError)
{
    dev.writeXMLHeader("e3-detector");
}


void
MSE3Collector::update(SUMOTime execTime) throw()
{
    myCurrentMeanSpeed = 0;
    myCurrentHaltingsNumber = 0;
    myCurrentTouchedVehicles = 0;
    for (std::map<MSVehicle*, E3Values>::iterator pair = myEnteredContainer.begin(); pair!=myEnteredContainer.end(); ++pair) {
        MSVehicle* veh = pair->first;
        E3Values& values = pair->second;
        if (values.samples!=0) {
            values.speedSum += veh->getSpeed();
            values.intervalSpeedSum += veh->getSpeed();
            myCurrentMeanSpeed += veh->getSpeed();
            myCurrentTouchedVehicles += 1;
        } else {
            SUMOReal entryTimestepFraction = (SUMOReal)((int) values.entryTime) + 1 - values.entryTime;
            values.speedSum += (veh->getSpeed() * entryTimestepFraction);
            values.intervalSpeedSum += (veh->getSpeed() * entryTimestepFraction);
            myCurrentMeanSpeed += (veh->getSpeed() * entryTimestepFraction);
            myCurrentTouchedVehicles += entryTimestepFraction;
        }
        if (veh->getSpeed() < myHaltingSpeedThreshold) {
            if (values.haltingBegin==-1) {
                values.haltingBegin = execTime;
            }
            if (execTime-values.haltingBegin>myHaltingTimeThreshold) {
                values.haltings++;
                values.intervalHaltings++;
                myCurrentHaltingsNumber++;
            }
        } else {
            values.haltingBegin = -1;
        }
        values.samples++;
        myCurrentMeanSpeed /= myCurrentTouchedVehicles;
    }
}


SUMOReal
MSE3Collector::getCurrentMeanSpeed() const throw()
{
    SUMOReal ret = 0;
    if(myEnteredContainer.size()==0) {
        return -1;
    }
    for (std::map<MSVehicle*, E3Values>::const_iterator pair = myEnteredContainer.begin(); pair!=myEnteredContainer.end(); ++pair) {
        ret += (*pair).first->getSpeed();
    }
    return ret / SUMOReal(myEnteredContainer.size());
}


SUMOReal
MSE3Collector::getCurrentHaltingNumber() const throw()
{
    return myCurrentHaltingsNumber;
}


SUMOReal
MSE3Collector::getVehiclesWithin() const throw()
{
    return (SUMOReal) myEnteredContainer.size();
}


std::vector<std::string> 
MSE3Collector::getCurrentVehicleIDs() const throw()
{
    std::vector<std::string> ret;
    for (std::map<MSVehicle*, E3Values>::const_iterator pair = myEnteredContainer.begin(); pair!=myEnteredContainer.end(); ++pair) {
        ret.push_back((*pair).first->getID());
    }
    return ret;
}


/****************************************************************************/

