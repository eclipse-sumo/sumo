/****************************************************************************/
/// @file    TraCI_LaneAreaDetector.h
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
#include "../../config.h"
#endif

#include <microsim/output/MSDetectorControl.h>
#include <microsim/MSNet.h>
#include "TraCI_LaneAreaDetector.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member definitions
// ===========================================================================
std::vector<std::string>
TraCI_LaneAreaDetector::getIDList() {
    std::vector<std::string> ids;
    MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_LANE_AREA_DETECTOR).insertIDs(ids);
    return ids;
}


int
TraCI_LaneAreaDetector::getIDCount() {
    std::vector<std::string> ids;
    return (int)MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_LANE_AREA_DETECTOR).size();
}


int
TraCI_LaneAreaDetector::getJamLengthVehicle(const std::string& detID) {
}


double
TraCI_LaneAreaDetector::getJamLengthMeters(const std::string& detID) {
}


double
TraCI_LaneAreaDetector::getLastStepMeanSpeed(const std::string& detID) {
}


std::vector<std::string>
TraCI_LaneAreaDetector::getLastStepVehicleIDs(const std::string& detID) {
}


double
TraCI_LaneAreaDetector::getLastStepOccupancy(const std::string& detID) {
}


double
TraCI_LaneAreaDetector::getPosition(const std::string& detID) {
}


std::string
TraCI_LaneAreaDetector::getLaneID(const std::string& detID) {
}


double
TraCI_LaneAreaDetector::getLength(const std::string& detID) {
}


int
TraCI_LaneAreaDetector::getLastStepVehicleNumber(const std::string& detID) {
}


/****************************************************************************/
