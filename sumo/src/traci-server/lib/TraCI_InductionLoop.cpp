/****************************************************************************/
/// @file    TraCI_InductionLoop.cpp
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
#include <microsim/output/MSInductLoop.h>
#include <microsim/MSNet.h>
#include <traci-server/TraCIDefs.h>
#include "TraCI_InductionLoop.h"


// ===========================================================================
// member definitions
// ===========================================================================
std::vector<std::string>
TraCI_InductionLoop::getIDList() {
    std::vector<std::string> ids;
    MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_INDUCTION_LOOP).insertIDs(ids);
    return ids;
}


int
TraCI_InductionLoop::getIDCount() {
    std::vector<std::string> ids;
    return (int)MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_INDUCTION_LOOP).size();
}


double
TraCI_InductionLoop::getPosition(const std::string& detID) {
    return getDetector(detID)->getPosition();
}


std::string
TraCI_InductionLoop::getLaneID(const std::string& detID) {
    return getDetector(detID)->getLane()->getID();
}


int
TraCI_InductionLoop::getLastStepVehicleNumber(const std::string& detID) {
    return getDetector(detID)->getCurrentPassedNumber();
}


double
TraCI_InductionLoop::getLastStepMeanSpeed(const std::string& detID) {
    return getDetector(detID)->getCurrentSpeed();
}


std::vector<std::string>
TraCI_InductionLoop::getLastStepVehicleIDs(const std::string& detID) {
    return getDetector(detID)->getCurrentVehicleIDs();
}


double
TraCI_InductionLoop::getLastStepOccupancy(const std::string& detID) {
    return getDetector(detID)->getCurrentOccupancy();
}


double
TraCI_InductionLoop::getLastStepMeanLength(const std::string& detID) {
    return getDetector(detID)->getCurrentLength();
}


double
TraCI_InductionLoop::getTimeSinceDetection(const std::string& detID) {
    return getDetector(detID)->getTimeSinceLastDetection();
}


std::vector<TraCIVehicleData>
TraCI_InductionLoop::getVehicleData(const std::string& detID) {
    std::vector<MSInductLoop::VehicleData> vd = getDetector(detID)->collectVehiclesOnDet(MSNet::getInstance()->getCurrentTimeStep() - DELTA_T, true);
    std::vector<TraCIVehicleData> tvd;
    for (std::vector<MSInductLoop::VehicleData>::const_iterator vdi = vd.begin(); vdi != vd.end(); ++vdi) {
        tvd.push_back(TraCIVehicleData());
        tvd.back().id = vdi->idM;
        tvd.back().length = vdi->lengthM;
        tvd.back().entryTime = vdi->entryTimeM;
        tvd.back().leaveTime = vdi->leaveTimeM;
        tvd.back().typeID = vdi->typeIDM;
    }
    return tvd;

}


MSInductLoop*
TraCI_InductionLoop::getDetector(const std::string& id) {
    MSInductLoop* il = dynamic_cast<MSInductLoop*>(MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_INDUCTION_LOOP).get(id));
    if (il == 0) {
        throw TraCIException("Induction loop '" + id + "' is not known");
    }
    return il;
}


/****************************************************************************/
