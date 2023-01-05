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
/// @file    LaneArea.cpp
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
#include "LaneArea.h"


namespace libsumo {
// ===========================================================================
// static member initializations
// ===========================================================================
SubscriptionResults LaneArea::mySubscriptionResults;
ContextSubscriptionResults LaneArea::myContextSubscriptionResults;
NamedRTree* LaneArea::myTree(nullptr);


// ===========================================================================
// static member definitions
// ===========================================================================
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
    return e2->getLength();
}


int
LaneArea::getLastStepVehicleNumber(const std::string& detID) {
    return getDetector(detID)->getCurrentVehicleNumber();
}


int
LaneArea::getLastStepHaltingNumber(const std::string& detID) {
    return getDetector(detID)->getCurrentHaltingNumber();
}


double
LaneArea::getIntervalOccupancy(const std::string& detID) {
    return getDetector(detID)->getIntervalOccupancy();
}

double
LaneArea::getIntervalMeanSpeed(const std::string& detID) {
    return getDetector(detID)->getIntervalMeanSpeed();
}

double
LaneArea::getIntervalMaxJamLengthInMeters(const std::string& detID) {
    return getDetector(detID)->getIntervalMaxJamLengthInMeters();
}

int
LaneArea::getIntervalVehicleNumber(const std::string& detID) {
    return getDetector(detID)->getIntervalVehicleNumber();
}

double
LaneArea::getLastIntervalOccupancy(const std::string& detID) {
    return getDetector(detID)->getLastIntervalOccupancy();
}

double
LaneArea::getLastIntervalMeanSpeed(const std::string& detID) {
    return getDetector(detID)->getLastIntervalMeanSpeed();
}

double
LaneArea::getLastIntervalMaxJamLengthInMeters(const std::string& detID) {
    return getDetector(detID)->getLastIntervalMaxJamLengthInMeters();
}

int
LaneArea::getLastIntervalVehicleNumber(const std::string& detID) {
    return getDetector(detID)->getLastIntervalVehicleNumber();
}


std::string
LaneArea::getParameter(const std::string& detID, const std::string& param) {
    return getDetector(detID)->getParameter(param, "");
}


LIBSUMO_GET_PARAMETER_WITH_KEY_IMPLEMENTATION(LaneArea)


void
LaneArea::setParameter(const std::string& detID, const std::string& name, const std::string& value) {
    getDetector(detID)->setParameter(name, value);
}


LIBSUMO_SUBSCRIPTION_IMPLEMENTATION(LaneArea, LANEAREA)


MSE2Collector*
LaneArea::getDetector(const std::string& id) {
    MSE2Collector* e2 = dynamic_cast<MSE2Collector*>(MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_LANE_AREA_DETECTOR).get(id));
    if (e2 == nullptr) {
        throw TraCIException("Lane area detector '" + id + "' is not known");
    }
    return e2;
}


NamedRTree*
LaneArea::getTree() {
    if (myTree == nullptr) {
        myTree = new NamedRTree();
        for (const std::string& id : getIDList()) {
            PositionVector shape;
            storeShape(id, shape);
            Boundary b = shape.getBoxBoundary();
            const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
            const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
            myTree->Insert(cmin, cmax, getDetector(id));
        }
    }
    return myTree;
}


void
LaneArea::cleanup() {
    delete myTree;
    myTree = nullptr;
}


void
LaneArea::storeShape(const std::string& id, PositionVector& shape) {
    MSE2Collector* const det = getDetector(id);
    shape.push_back(det->getLanes().front()->getShape().positionAtOffset(det->getStartPos()));
    shape.push_back(det->getLanes().back()->getShape().positionAtOffset(det->getEndPos()));
}


std::shared_ptr<VariableWrapper>
LaneArea::makeWrapper() {
    return std::make_shared<Helper::SubscriptionWrapper>(handleVariable, mySubscriptionResults, myContextSubscriptionResults);
}


void
LaneArea::overrideVehicleNumber(const std::string& detID, int num) {
    getDetector(detID)->overrideVehicleNumber(num);
}


bool
LaneArea::handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper, tcpip::Storage* paramData) {
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
        case JAM_LENGTH_VEHICLE:
            return wrapper->wrapInt(objID, variable, getJamLengthVehicle(objID));
        case JAM_LENGTH_METERS:
            return wrapper->wrapDouble(objID, variable, getJamLengthMeters(objID));
        case LAST_STEP_OCCUPANCY:
            return wrapper->wrapDouble(objID, variable, getLastStepOccupancy(objID));
        case VAR_POSITION:
            return wrapper->wrapDouble(objID, variable, getPosition(objID));
        case VAR_LANE_ID:
            return wrapper->wrapString(objID, variable, getLaneID(objID));
        case VAR_LENGTH:
            return wrapper->wrapDouble(objID, variable, getLength(objID));
        case VAR_INTERVAL_OCCUPANCY:
            return wrapper->wrapDouble(objID, variable, getIntervalOccupancy(objID));
        case VAR_INTERVAL_SPEED:
            return wrapper->wrapDouble(objID, variable, getIntervalMeanSpeed(objID));
        case VAR_INTERVAL_MAX_JAM_LENGTH_METERS:
            return wrapper->wrapDouble(objID, variable, getIntervalMaxJamLengthInMeters(objID));
        case VAR_INTERVAL_NUMBER:
            return wrapper->wrapInt(objID, variable, getIntervalVehicleNumber(objID));
        case VAR_LAST_INTERVAL_OCCUPANCY:
            return wrapper->wrapDouble(objID, variable, getLastIntervalOccupancy(objID));
        case VAR_LAST_INTERVAL_SPEED:
            return wrapper->wrapDouble(objID, variable, getLastIntervalMeanSpeed(objID));
        case VAR_LAST_INTERVAL_MAX_JAM_LENGTH_METERS:
            return wrapper->wrapDouble(objID, variable, getLastIntervalMaxJamLengthInMeters(objID));
        case VAR_LAST_INTERVAL_NUMBER:
            return wrapper->wrapInt(objID, variable, getLastIntervalVehicleNumber(objID));
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
