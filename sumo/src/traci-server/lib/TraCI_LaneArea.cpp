/****************************************************************************/
/// @file    TraCI_LaneArea.cpp
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    30.05.2012
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors
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

#include <microsim/output/MSDetectorControl.h>
#include <microsim/output/MSE2Collector.h>
#include <microsim/MSNet.h>
#include <traci-server/TraCIDefs.h>
#include "TraCI_LaneArea.h"


// ===========================================================================
// member definitions
// ===========================================================================
std::vector<std::string>
TraCI_LaneArea::getIDList() {
    std::vector<std::string> ids;
    MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_LANE_AREA_DETECTOR).insertIDs(ids);
    return ids;
}


int
TraCI_LaneArea::getIDCount() {
    std::vector<std::string> ids;
    return (int)MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_LANE_AREA_DETECTOR).size();
}


int
TraCI_LaneArea::getJamLengthVehicle(const std::string& detID) {
    return getDetector(detID)->getCurrentJamLengthInVehicles();
}


double
TraCI_LaneArea::getJamLengthMeters(const std::string& detID) {
    return getDetector(detID)->getCurrentJamLengthInMeters();
}


double
TraCI_LaneArea::getLastStepMeanSpeed(const std::string& detID) {
    return getDetector(detID)->getCurrentMeanSpeed();
}


std::vector<std::string>
TraCI_LaneArea::getLastStepVehicleIDs(const std::string& detID) {
    return getDetector(detID)->getCurrentVehicleIDs();
}


double
TraCI_LaneArea::getLastStepOccupancy(const std::string& detID) {
    return getDetector(detID)->getCurrentOccupancy();
}


double
TraCI_LaneArea::getPosition(const std::string& detID) {
    return getDetector(detID)->getStartPos();
}


std::string
TraCI_LaneArea::getLaneID(const std::string& detID) {
    return getDetector(detID)->getLane()->getID();
}


double
TraCI_LaneArea::getLength(const std::string& detID) {
    const MSE2Collector* const e2 = getDetector(detID);
    return e2->getEndPos() - e2->getStartPos();
}


int
TraCI_LaneArea::getLastStepVehicleNumber(const std::string& detID) {
    return getDetector(detID)->getCurrentVehicleNumber();
}


int
TraCI_LaneArea::getLastStepHaltingNumber(const std::string& detID) {
    return getDetector(detID)->getCurrentHaltingNumber();
}


MSE2Collector*
TraCI_LaneArea::getDetector(const std::string& id) {
    MSE2Collector* e2 = dynamic_cast<MSE2Collector*>(MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_LANE_AREA_DETECTOR).get(id));
    if (e2 == 0) {
        throw TraCIException("Lane area detector '" + id + "' is not known");
    }
    return e2;
}


/****************************************************************************/
