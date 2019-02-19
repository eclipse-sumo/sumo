/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
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
#include <config.h>

#include <microsim/output/MSDetectorControl.h>
#include <microsim/output/MSE2Collector.h>
#include <microsim/MSNet.h>
#include <libsumo/TraCIConstants.h>
#include "MultiEntryExit.h"


namespace libsumo {
// ===========================================================================
// static member initializations
// ===========================================================================
SubscriptionResults MultiEntryExit::mySubscriptionResults;
ContextSubscriptionResults MultiEntryExit::myContextSubscriptionResults;


// ===========================================================================
// static member definitions
// ===========================================================================
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


LIBSUMO_SUBSCRIPTION_IMPLEMENTATION(MultiEntryExit, MULTIENTRYEXIT)


MSE3Collector*
MultiEntryExit::getDetector(const std::string& id) {
    MSE3Collector* e3 = dynamic_cast<MSE3Collector*>(MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_ENTRY_EXIT_DETECTOR).get(id));
    if (e3 == nullptr) {
        throw TraCIException("Multi entry exit detector '" + id + "' is not known");
    }
    return e3;
}


std::shared_ptr<VariableWrapper>
MultiEntryExit::makeWrapper() {
    return std::make_shared<Helper::SubscriptionWrapper>(handleVariable, mySubscriptionResults, myContextSubscriptionResults);
}


bool
MultiEntryExit::handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper) {
    switch (variable) {
        case TRACI_ID_LIST:
            return wrapper->wrapStringList(objID, variable, getIDList());
        case ID_COUNT:
            return wrapper->wrapInt(objID, variable, getIDCount());
        case LAST_STEP_VEHICLE_NUMBER:
            return wrapper->wrapInt(objID, variable, getLastStepVehicleNumber(objID));
        case LAST_STEP_MEAN_SPEED:
            return wrapper->wrapDouble(objID, variable, getLastStepMeanSpeed(objID));
        case LAST_STEP_VEHICLE_ID_LIST:
            return wrapper->wrapStringList(objID, variable, getLastStepVehicleIDs(objID));
        case LAST_STEP_VEHICLE_HALTING_NUMBER:
            return wrapper->wrapInt(objID, variable, getLastStepHaltingNumber(objID));
        default:
            return false;
    }
}


}


/****************************************************************************/
