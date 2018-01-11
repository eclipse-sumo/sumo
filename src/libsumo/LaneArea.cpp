/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    LaneArea.cpp
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    30.05.2012
/// @version $Id$
///
// C++ TraCI client API implementation
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
#include <libsumo/TraCIDefs.h>
#include "LaneArea.h"


// ===========================================================================
// member definitions
// ===========================================================================
namespace libsumo {
std::vector<std::string>
LaneArea::getIDList() {
    std::vector<std::string> ids;
    MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_LANE_AREA_DETECTOR).insertIDs(ids);
    return ids;
}


int
LaneArea::getIDCount() {
    std::vector<std::string> ids;
    return (int)MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_LANE_AREA_DETECTOR).size();
}


int
LaneArea::getJamLengthVehicle(const std::string& detID) {
    return getDetector(detID)->getCurrentJamLengthInVehicles();
}


double
LaneArea::getJamLengthMeters(const std::string& detID) {
    return getDetector(detID)->getCurrentJamLengthInMeters();
}


double
LaneArea::getLastStepMeanSpeed(const std::string& detID) {
    return getDetector(detID)->getCurrentMeanSpeed();
}


std::vector<std::string>
LaneArea::getLastStepVehicleIDs(const std::string& detID) {
    return getDetector(detID)->getCurrentVehicleIDs();
}


double
LaneArea::getLastStepOccupancy(const std::string& detID) {
    return getDetector(detID)->getCurrentOccupancy();
}


double
LaneArea::getPosition(const std::string& detID) {
    return getDetector(detID)->getStartPos();
}


std::string
LaneArea::getLaneID(const std::string& detID) {
    return getDetector(detID)->getLane()->getID();
}


double
LaneArea::getLength(const std::string& detID) {
    const MSE2Collector* const e2 = getDetector(detID);
    return e2->getEndPos() - e2->getStartPos();
}


int
LaneArea::getLastStepVehicleNumber(const std::string& detID) {
    return getDetector(detID)->getCurrentVehicleNumber();
}


int
LaneArea::getLastStepHaltingNumber(const std::string& detID) {
    return getDetector(detID)->getCurrentHaltingNumber();
}


MSE2Collector*
LaneArea::getDetector(const std::string& id) {
    MSE2Collector* e2 = dynamic_cast<MSE2Collector*>(MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_LANE_AREA_DETECTOR).get(id));
    if (e2 == 0) {
        throw TraCIException("Lane area detector '" + id + "' is not known");
    }
    return e2;
}
}


/****************************************************************************/
