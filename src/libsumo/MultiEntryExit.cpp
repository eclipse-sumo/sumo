/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MultiEntryExit.cpp
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
#include "MultiEntryExit.h"


// ===========================================================================
// member definitions
// ===========================================================================
namespace libsumo {
std::vector<std::string>
MultiEntryExit::getIDList() {
    std::vector<std::string> ids;
    MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_ENTRY_EXIT_DETECTOR).insertIDs(ids);
    return ids;
}


int
MultiEntryExit::getIDCount() {
    std::vector<std::string> ids;
    return (int)MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_ENTRY_EXIT_DETECTOR).size();
}


int
MultiEntryExit::getLastStepVehicleNumber(const std::string& detID) {
    return getDetector(detID)->getVehiclesWithin();
}


double
MultiEntryExit::getLastStepMeanSpeed(const std::string& detID) {
    return getDetector(detID)->getCurrentMeanSpeed();
}


std::vector<std::string>
MultiEntryExit::getLastStepVehicleIDs(const std::string& detID) {
    return getDetector(detID)->getCurrentVehicleIDs();
}


int
MultiEntryExit::getLastStepHaltingNumber(const std::string& detID) {
    return getDetector(detID)->getCurrentHaltingNumber();
}


MSE3Collector*
MultiEntryExit::getDetector(const std::string& id) {
    MSE3Collector* e3 = dynamic_cast<MSE3Collector*>(MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_ENTRY_EXIT_DETECTOR).get(id));
    if (e3 == 0) {
        throw TraCIException("Multi entry exit detector '" + id + "' is not known");
    }
    return e3;
}
}


/****************************************************************************/
