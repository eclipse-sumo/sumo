/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MultiEntryExit.cpp
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    30.05.2012
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>

#include <microsim/output/MSDetectorControl.h>
#include <microsim/output/MSE2Collector.h>
#include <microsim/MSNet.h>
#include <libsumo/Helper.h>
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
    return (int)MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_ENTRY_EXIT_DETECTOR).size();
}


std::vector<std::string>
MultiEntryExit::getEntryLanes(const std::string& detID) {
    std::vector<std::string> ids;
    for (const MSCrossSection& cs : getDetector(detID)->getEntries()) {
        ids.push_back(cs.myLane->getID());
    }
    return ids;
}


std::vector<std::string>
MultiEntryExit::getExitLanes(const std::string& detID) {
    std::vector<std::string> ids;
    for (const MSCrossSection& cs : getDetector(detID)->getExits()) {
        ids.push_back(cs.myLane->getID());
    }
    return ids;
}


std::vector<double>
MultiEntryExit::getEntryPositions(const std::string& detID) {
    std::vector<double> pos;
    for (const MSCrossSection& cs : getDetector(detID)->getEntries()) {
        pos.push_back(cs.myPosition);
    }
    return pos;
}


std::vector<double>
MultiEntryExit::getExitPositions(const std::string& detID) {
    std::vector<double> pos;
    for (const MSCrossSection& cs : getDetector(detID)->getExits()) {
        pos.push_back(cs.myPosition);
    }
    return pos;
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


double
MultiEntryExit::getLastIntervalMeanTravelTime(const std::string& detID) {
    return getDetector(detID)->getLastIntervalMeanTravelTime();
}


double
MultiEntryExit::getLastIntervalMeanHaltsPerVehicle(const std::string& detID) {
    return getDetector(detID)->getLastIntervalMeanHaltsPerVehicle();
}


double
MultiEntryExit::getLastIntervalMeanTimeLoss(const std::string& detID) {
    return getDetector(detID)->getLastIntervalMeanTimeLoss();
}


int
MultiEntryExit::getLastIntervalVehicleSum(const std::string& detID) {
    return getDetector(detID)->getLastIntervalVehicleSum();
}


std::string
MultiEntryExit::getParameter(const std::string& detID, const std::string& param) {
    return getDetector(detID)->getParameter(param, "");
}


LIBSUMO_GET_PARAMETER_WITH_KEY_IMPLEMENTATION(MultiEntryExit)


void
MultiEntryExit::setParameter(const std::string& detID, const std::string& name, const std::string& value) {
    getDetector(detID)->setParameter(name, value);
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
MultiEntryExit::handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper, tcpip::Storage* paramData) {
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
        case VAR_LAST_INTERVAL_TRAVELTIME:
            return wrapper->wrapDouble(objID, variable, getLastIntervalMeanTravelTime(objID));
        case VAR_LAST_INTERVAL_MEAN_HALTING_NUMBER:
            return wrapper->wrapDouble(objID, variable, getLastIntervalMeanHaltsPerVehicle(objID));
        case VAR_TIMELOSS:
            return wrapper->wrapDouble(objID, variable, getLastIntervalMeanTimeLoss(objID));
        case VAR_LAST_INTERVAL_VEHICLE_NUMBER:
            return wrapper->wrapInt(objID, variable, getLastIntervalVehicleSum(objID));
        case VAR_LANES:
            return wrapper->wrapStringList(objID, variable, getEntryLanes(objID));
        case VAR_EXIT_LANES:
            return wrapper->wrapStringList(objID, variable, getExitLanes(objID));
        case VAR_POSITION:
            return wrapper->wrapDoubleList(objID, variable, getEntryPositions(objID));
        case VAR_EXIT_POSITIONS:
            return wrapper->wrapDoubleList(objID, variable, getExitPositions(objID));
        case libsumo::VAR_PARAMETER:
            paramData->readUnsignedByte();
            return wrapper->wrapString(objID, variable, getParameter(objID, paramData->readString()));
        case libsumo::VAR_PARAMETER_WITH_KEY:
            paramData->readUnsignedByte();
            return wrapper->wrapStringPair(objID, variable, getParameterWithKey(objID, paramData->readString()));
        default:
            return false;
    }
}


}


/****************************************************************************/
