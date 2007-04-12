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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSE3Collector.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// variable declarations
// ===========================================================================
std::string MSE3Collector::xmlHeaderM("<?xml version=\"1.0\" standalone=\"yes\"?>\n\n");
std::string MSE3Collector::infoEndM = "</e3-detector>";


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * MSE3Collector::MSE3EntryReminder - definitions
 * ----------------------------------------------------------------------- */
MSE3Collector::MSE3EntryReminder::MSE3EntryReminder(
    const MSCrossSection &crossSection, MSE3Collector& collector)
        : MSMoveReminder(crossSection.laneM),
        collectorM(collector), posM(crossSection.posM)
{}


bool
MSE3Collector::MSE3EntryReminder::isStillActive(MSVehicle& veh, SUMOReal oldPos,
        SUMOReal newPos, SUMOReal newSpeed)
{
    if (newPos <= posM) {
        // crossSection not yet reached
        return true;
    }
    SUMOReal dist = SPEED2DIST(newSpeed);
    SUMOReal entryTimestep = (SUMOReal) MSNet::getInstance()->getCurrentTimeStep() - (SUMOReal) (1 - (posM - oldPos) / dist);
    collectorM.enter(veh, entryTimestep);
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
    return veh.getPositionOnLane() <= posM;
}



/* -------------------------------------------------------------------------
 * MSE3Collector::MSE3LeaveReminder - definitions
 * ----------------------------------------------------------------------- */
MSE3Collector::MSE3LeaveReminder::MSE3LeaveReminder(
    const MSCrossSection &crossSection, MSE3Collector& collector)
        : MSMoveReminder(crossSection.laneM),
        collectorM(collector), posM(crossSection.posM)
{}


bool
MSE3Collector::MSE3LeaveReminder::isStillActive(MSVehicle& veh, SUMOReal oldPos,
        SUMOReal newPos, SUMOReal newSpeed)
{
    if (newPos <= posM) {
        // crossSection not yet reached
        return true;
    }
    if (newPos - veh.getLength() > posM) {
        // crossSection completely left
        SUMOReal dist = SPEED2DIST(newSpeed);
        SUMOReal leaveTimestep = (SUMOReal) MSNet::getInstance()->getCurrentTimeStep() - (SUMOReal) (1 - (posM - oldPos) / dist);
        collectorM.leave(veh, leaveTimestep);
        return false;
    }
    // crossSection partially left
    return true;
}


void
MSE3Collector::MSE3LeaveReminder::dismissByLaneChange(MSVehicle&)
{
    // nothing to do for E3
}


bool
MSE3Collector::MSE3LeaveReminder::isActivatedByEmitOrLaneChange(MSVehicle& veh)
{
    return veh.getPositionOnLane() - veh.getLength() <= posM;
}



/* -------------------------------------------------------------------------
 * MSE3Collector - definitions
 * ----------------------------------------------------------------------- */
MSE3Collector::MSE3Collector(const std::string &id,
                             const CrossSectionVector &entries,
                             const CrossSectionVector &exits,
                             MSUnit::Seconds haltingTimeThreshold,
                             MSUnit::MetersPerSecond haltingSpeedThreshold)
        : idM(id), entriesM(entries), exitsM(exits),
        // !dk! kept for later use: haltingTimeThresholdM(MSUnit::getInstance()->getSteps(haltingTimeThreshold)),
        haltingSpeedThresholdM(MSUnit::getInstance()->getCellsPerStep(haltingSpeedThreshold))
{
    // Set MoveReminders to entries and exits
    for (CrossSectionVectorConstIt crossSec1 = entries.begin(); crossSec1!=entries.end(); ++crossSec1) {
        entryRemindersM.push_back(new MSE3EntryReminder(*crossSec1, *this));
    }
    for (CrossSectionVectorConstIt crossSec2 = exits.begin(); crossSec2!=exits.end(); ++crossSec2) {
        leaveRemindersM.push_back(new MSE3LeaveReminder(*crossSec2, *this));
    }
    MSUpdateEachTimestepContainer< MSE3Collector >::getInstance()->addItemToUpdate(this);
}


MSE3Collector::~MSE3Collector(void)
{
    for (std::map<MSVehicle*, E3Values>::iterator pair = myEnteredContainer.begin(); pair!=myEnteredContainer.end(); ++pair) {
        pair->first->quitRemindedLeft(this);
    }
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
    if(myEnteredContainer.find(&veh)!=myEnteredContainer.end()) {
        MsgHandler::getWarningInstance()->inform("Vehicle '" + veh.getID() + "' reentered E3-detector '" + getID() + "'.");
    }
    myEnteredContainer[&veh] = v;
}


void
MSE3Collector::leave(MSVehicle& veh, SUMOReal leaveTimestep)
{
    if(myEnteredContainer.find(&veh)==myEnteredContainer.end()) {
        MsgHandler::getWarningInstance()->inform("Vehicle '" + veh.getID() + "' left E3-detector '" + getID() + "' before entering it.");
    } else {
        E3Values v = myEnteredContainer[&veh];
        v.leaveTime = leaveTimestep;
        myEnteredContainer.erase(&veh);
        myLeftContainer[&veh] = v;
    }
    veh.quitRemindedLeft(this);
}


const std::string&
MSE3Collector::getID() const
{
    return idM;
}


void
MSE3Collector::removeOnTripEnd(MSVehicle *veh)
{
    if(myEnteredContainer.find(veh)==myEnteredContainer.end()) {
        MsgHandler::getWarningInstance()->inform("Vehicle '" + veh->getID() + "' left E3-detector '" + getID() + "' before entering it.");
    } else {
        myEnteredContainer.erase(veh);
    }
}


void
MSE3Collector::writeXMLHeader(OutputDevice &dev) const
{
    dev.writeString(xmlHeaderM);
}


void
MSE3Collector::writeXMLOutput(OutputDevice &dev,
                              SUMOTime startTime, SUMOTime stopTime)
{
    dev.writeString("   <interval begin=\"").writeString(
        toString(startTime)).writeString("\" end=\"").writeString(
            toString(stopTime)).writeString("\" ");
    if (dev.needsDetectorName()) {
        dev.writeString("id=\"").writeString(idM).writeString("\" ");
    }
    // collect values
    SUMOReal vehicleSum = (SUMOReal) myLeftContainer.size();
    SUMOReal meanTravelTime = 0.;
    SUMOReal meanSpeed = 0.;
    SUMOReal meanHaltsPerVehicle = 0.;
        //
    for(std::map<MSVehicle*, E3Values>::iterator i=myLeftContainer.begin(); i!=myLeftContainer.end(); ++i) {
        meanHaltsPerVehicle += (SUMOReal) (*i).second.haltings;
        SUMOReal steps = (SUMOReal) ((*i).second.leaveTime-(*i).second.entryTime);
        meanTravelTime += steps;
        meanSpeed += (SUMOReal) ((*i).second.speedSum / steps);
    }
    meanTravelTime /= vehicleSum;
    meanSpeed /= vehicleSum;
    meanHaltsPerVehicle /= vehicleSum;
    // write values
    dev.writeString("meanTravelTime=\"").writeString(toString(meanTravelTime)).writeString(
        "\" meanSpeed=\"").writeString(toString(meanSpeed)).writeString(
        "\" meanHaltsPerVehicle=\"").writeString(toString(meanHaltsPerVehicle)).writeString(
        "\" vehicleSum=\"").writeString(toString(vehicleSum)).writeString("\"/>");
    // clear container
    myLeftContainer.clear();
}


SUMOReal
MSE3Collector::getValue(MSE3Collector::Value which) const
{
    SUMOTime ctime = MSNet::getInstance()->getCurrentTimeStep();
    switch(which) {
    case MEAN_TRAVELTIME:
        {
            SUMOReal meanTravelTime = 0.;
            size_t noFound = 0;
            for(std::map<MSVehicle*, E3Values>::const_iterator i=myLeftContainer.begin(); i!=myLeftContainer.end(); ++i) {
                if((*i).second.leaveTime==ctime) {
                    SUMOReal steps = (SUMOReal) ((*i).second.leaveTime-(*i).second.entryTime);
                    meanTravelTime += steps;
                    noFound++;
                }
            }
            return noFound!=0 ? meanTravelTime / (SUMOReal) myLeftContainer.size() : 0;
        }
    case MEAN_NUMBER_OF_HALTINGS_PER_VEHICLE:
        {
            SUMOReal meanHaltsPerVehicle = 0.;
            size_t noFound = 0;
            for(std::map<MSVehicle*, E3Values>::const_iterator i=myLeftContainer.begin(); i!=myLeftContainer.end(); ++i) {
                if((*i).second.leaveTime==ctime) {
                    meanHaltsPerVehicle += (SUMOReal) (*i).second.haltings;
                    noFound++;
                }
            }
            return noFound!=0 ? meanHaltsPerVehicle / (SUMOReal) myLeftContainer.size() : 0;
        }
    case NUMBER_OF_VEHICLES:
        {
            SUMOReal vehicleNumber = 0.;
            for(std::map<MSVehicle*, E3Values>::const_iterator i=myLeftContainer.begin(); i!=myLeftContainer.end(); ++i) {
                if((*i).second.leaveTime==ctime) {
                    vehicleNumber += 1.;
                }
            }
            return vehicleNumber;
        }
    case MEAN_SPEED:
        {
            SUMOReal meanSpeed = 0.;
            size_t noFound = 0;
            for(std::map<MSVehicle*, E3Values>::const_iterator i=myLeftContainer.begin(); i!=myLeftContainer.end(); ++i) {
                if((*i).second.leaveTime==ctime) {
                    SUMOReal steps = (SUMOReal) ((*i).second.leaveTime-(*i).second.entryTime);
                    meanSpeed += (SUMOReal) ((*i).second.speedSum / steps);
                    noFound++;
                }
            }
            return noFound!=0 ? meanSpeed / (SUMOReal) myLeftContainer.size() : 0;
        }
    default:
        return 0;
    }
}


void
MSE3Collector::writeXMLDetectorInfoStart(OutputDevice &dev) const
{
    dev.writeString("<e3-detector>\n");
    std::string entries;
    CrossSectionVectorConstIt crossSec;
    for (crossSec = entriesM.begin(); crossSec != entriesM.end(); ++crossSec) {
        dev.writeString("   <entry lane=\"").writeString(
            crossSec->laneM->getID()).writeString("\" pos=\"").writeString(
                toString(crossSec->posM)).writeString("\"/>\n");
    }
    std::string exits;
    for (crossSec = exitsM.begin(); crossSec != exitsM.end(); ++crossSec) {
        dev.writeString("   <exit lane=\"").writeString(
            crossSec->laneM->getID()).writeString("\" pos=\"").writeString(
                toString(crossSec->posM)).writeString("\"/>\n");
    }
}


void
MSE3Collector::writeXMLDetectorInfoEnd(OutputDevice &dev) const
{
    dev.writeString(infoEndM);
}


bool 
MSE3Collector::updateEachTimestep(void)
{
    for (std::map<MSVehicle*, E3Values>::iterator pair = myEnteredContainer.begin(); pair!=myEnteredContainer.end(); ++pair) {
        MSVehicle* veh = pair->first;
        E3Values& values = pair->second;
        values.speedSum += veh->getSpeed();
        if (veh->getSpeed() < haltingSpeedThresholdM) {
            values.haltings++;
        } 
        /*
        halting.posM += veh->getMovedDistance();
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
    return false; // to please MSVC++
}


/****************************************************************************/

