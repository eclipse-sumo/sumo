/****************************************************************************/
/// @file    MSE3Collector.cpp
/// @author  Christian Roessel
/// @date    Tue Dec 02 2003 22:17 CET
/// @version $Id$
///
// / @author  Christian Roessel <christian.roessel@dlr.de>
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
std::string MSE3Collector::xmlHeaderM(
    "<?xml version=\"1.0\" standalone=\"yes\"?>\n\n"
    "<!--\n"
    "- nVehicles [veh]\n"
    "- meanTraveltime [s]\n"
    "- meanNHaltsPerVehicle [n]\n"
    "-->\n\n");


std::string MSE3Collector::infoEndM = "</detector>";


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
MSE3Collector::MSE3EntryReminder::isStillActive(MSVehicle& veh, SUMOReal ,
        SUMOReal newPos, SUMOReal)
{
    if (newPos <= posM) {
        // crossSection not yet reached
        return true;
    }
    collectorM.enter(veh);
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
MSE3Collector::MSE3LeaveReminder::isStillActive(MSVehicle& veh, SUMOReal ,
        SUMOReal newPos, SUMOReal)
{
    if (newPos <= posM) {
        // crossSection not yet reached
        return true;
    }
    if (newPos - veh.getLength() > posM) {
        // crossSection completely left
        collectorM.leave(veh);
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
 * definitions of operators
 * ----------------------------------------------------------------------- */
MSE3Collector::DetType&
operator++(MSE3Collector::DetType& det)
{
    return det = (MSE3Collector::ALL == det) ? MSE3Collector::MEAN_TRAVELTIME : MSE3Collector::DetType(det + 1);
}


MSE3Collector::Containers&
operator++(MSE3Collector::Containers& cont)
{
    return cont = (MSE3Collector::TRAVELTIME == cont) ? MSE3Collector::VEHICLES : MSE3Collector::Containers(cont + 1);
}



/* -------------------------------------------------------------------------
 * MSE3Collector - definitions
 * ----------------------------------------------------------------------- */
MSE3Collector::MSE3Collector(const std::string &id,
                             const CrossSectionVector &entries,
                             const CrossSectionVector &exits,
                             MSUnit::Seconds haltingTimeThreshold,
                             MSUnit::MetersPerSecond haltingSpeedThreshold,
                             SUMOTime deleteDataAfterSeconds)
        : idM(id), entriesM(entries), exitsM(exits),
        haltingTimeThresholdM(MSUnit::getInstance()->getSteps(haltingTimeThreshold)),
        haltingSpeedThresholdM(MSUnit::getInstance()->getCellsPerStep(haltingSpeedThreshold)),
        deleteDataAfterSecondsM(deleteDataAfterSeconds),
        detectorsM(3), containersM(3)
{
    // Set MoveReminders to entries and exits
    for (CrossSectionVectorConstIt crossSec1 = entries.begin(); crossSec1!=entries.end(); ++crossSec1) {
        entryRemindersM.push_back(new MSE3EntryReminder(*crossSec1, *this));
    }
    for (CrossSectionVectorConstIt crossSec2 = exits.begin(); crossSec2!=exits.end(); ++crossSec2) {
        leaveRemindersM.push_back(new MSE3LeaveReminder(*crossSec2, *this));
    }
}


MSE3Collector::~MSE3Collector(void)
{
    deleteContainer(entryRemindersM);
    deleteContainer(leaveRemindersM);
    deleteContainer(detectorsM);
    deleteContainer(containersM);
}


void
MSE3Collector::enter(MSVehicle& veh)
{
    for (ContainerContIter it = containersM.begin(); it != containersM.end(); ++it) {
        if (*it != 0) {
            (*it)->enterDetectorByMove(&veh);
        }
    }
    veh.quitRemindedEntered(this);
}


void
MSE3Collector::leave(MSVehicle& veh)
{
    for (DetContIter det = detectorsM.begin(); det != detectorsM.end(); ++det) {
        if (*det != 0) {
            (*det)->leave(veh);
        }
    }
    for (ContainerContIter cont = containersM.begin(); cont != containersM.end(); ++cont) {
        if (*cont != 0) {
            (*cont)->leaveDetectorByMove(&veh);
        }
    }
    veh.quitRemindedLeft(this);
}


void
MSE3Collector::addDetector(DetType type, std::string detId)
{
    if (detId == "") {
        detId = idM;
    }
    if (type != ALL) {
        createDetector(type, detId);
    } else {
        for (DetType typ = MEAN_TRAVELTIME; typ <ALL; ++typ) {
            createDetector(typ, detId);
        }
    }
}


bool
MSE3Collector::hasDetector(DetType type)
{
    return getDetector(type) != 0;
}


SUMOReal
MSE3Collector::getAggregate(DetType type, MSUnit::Seconds lastNSeconds)
{
    assert(type != ALL);
    LDDetector* det = getDetector(type);
    if (det != 0) {
        return det->getAggregate(lastNSeconds);
    }
    // requested type not present
    // create it and return nonsens value for the first access
    addDetector(type, "");
    return -1;
}


const std::string&
MSE3Collector::getID() const
{
    return idM;
}


void
MSE3Collector::removeOnTripEnd(MSVehicle *veh)
{
    for (ContainerContIter cont = containersM.begin(); cont != containersM.end(); ++cont) {
        if (*cont != 0) {
            (*cont)->removeOnTripEnd(veh);
        }
    }
}


void
MSE3Collector::writeXMLHeader(XMLDevice &dev) const
{
    dev.writeString(xmlHeaderM);
}


void
MSE3Collector::writeXMLOutput(XMLDevice &dev,
                              SUMOTime startTime, SUMOTime stopTime)
{
    dev.writeString("<interval begin=\"").writeString(
        toString(startTime)).writeString("\" end=\"").writeString(
            toString(stopTime)).writeString("\" ");
    if (dev.needsDetectorName()) {
        dev.writeString("id=\"").writeString(idM).writeString("\" ");
    }
    writeXMLOutput(dev, detectorsM, startTime, stopTime);
    dev.writeString("/>");
}


void
MSE3Collector::writeXMLDetectorInfoStart(XMLDevice &dev) const
{
    dev.writeString("<detector type=\"E3_Collector\" id=\"");
    dev.writeString(idM).writeString("\" >\n");
    std::string entries;
    CrossSectionVectorConstIt crossSec;
    for (crossSec = entriesM.begin(); crossSec != entriesM.end(); ++crossSec) {
        dev.writeString("  <entry lane=\"").writeString(
            crossSec->laneM->getID()).writeString("\" pos=\"").writeString(
                toString(crossSec->posM)).writeString("\" />\n");
    }
    std::string exits;
    for (crossSec = exitsM.begin(); crossSec != exitsM.end(); ++crossSec) {
        dev.writeString("  <exit lane=\"").writeString(
            crossSec->laneM->getID()).writeString("\" pos=\"").writeString(
                toString(crossSec->posM)).writeString("\" />\n");
    }
}


void
MSE3Collector::writeXMLDetectorInfoEnd(XMLDevice &dev) const
{
    dev.writeString(infoEndM);
}


SUMOTime
MSE3Collector::getDataCleanUpSteps(void) const
{
    return deleteDataAfterSecondsM; // !!! Konvertierung
}


MSE3Collector::LDDetector*
MSE3Collector::getDetector(DetType type) const
{
    assert(type != ALL);
    return detectorsM[ type ];
}


void
MSE3Collector::createContainer(Containers type)
{
    switch (type) {
    case VEHICLES: {
        if (containersM[ VEHICLES ] == 0) {
            containersM[ VEHICLES ] = new DetectorContainer::VehicleMap();
        }
        break;
    }
    case TRAVELTIME: {
        if (containersM[ TRAVELTIME ] == 0) {
            containersM[ TRAVELTIME ] = new DetectorContainer::TimeMap();
        }
        break;
    }
    case HALTINGS: {
        if (containersM[ HALTINGS ] == 0) {
            containersM[ HALTINGS ] = new DetectorContainer::HaltingsMap(
                                          haltingTimeThresholdM, haltingSpeedThresholdM);
        }
        break;
    }
    default: {
        assert(0);
    }
    }
}


void
MSE3Collector::createDetector(DetType type, std::string detId)
{
    if (hasDetector(type)) {
        return;
    }
    using namespace Detector;
    switch (type) {
    case MEAN_TRAVELTIME: {
        createContainer(TRAVELTIME);
        detectorsM[ MEAN_TRAVELTIME ] =
            new E3Traveltime(
                E3Traveltime::getDetectorName() + detId,
                (MSUnit::Seconds) deleteDataAfterSecondsM,
                *static_cast< DetectorContainer::TimeMap* >(containersM[ TRAVELTIME ]));
        break;
    }
    case MEAN_NUMBER_OF_HALTINGS_PER_VEHICLE: {
        createContainer(HALTINGS);
        detectorsM[ MEAN_NUMBER_OF_HALTINGS_PER_VEHICLE ] =
            new E3MeanNHaltings(
                E3MeanNHaltings::getDetectorName() + detId,
                (MSUnit::Seconds) deleteDataAfterSecondsM,
                *static_cast< DetectorContainer::HaltingsMap* >(containersM[ HALTINGS ]));
        break;
    }
    case NUMBER_OF_VEHICLES: {
        createContainer(VEHICLES);
        detectorsM[ NUMBER_OF_VEHICLES ] =
            new E3NVehicles(
                E3NVehicles::getDetectorName() + detId,
                (MSUnit::Seconds) deleteDataAfterSecondsM,
                *static_cast< DetectorContainer::VehicleMap* >(containersM[ VEHICLES ]));
        break;
    }
    default: {
        assert(0);
    }
    }
}



/****************************************************************************/

