/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2012-2024 German Aerospace Center (DLR) and others.
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
/// @file    InductionLoop.cpp
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
#include <microsim/output/MSInductLoop.h>
#include <mesosim/MEInductLoop.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <libsumo/Helper.h>
#include <libsumo/TraCIDefs.h>
#include <libsumo/TraCIConstants.h>
#include "InductionLoop.h"


namespace libsumo {
// ===========================================================================
// static member initializations
// ===========================================================================
SubscriptionResults InductionLoop::mySubscriptionResults;
ContextSubscriptionResults InductionLoop::myContextSubscriptionResults;
NamedRTree* InductionLoop::myTree(nullptr);


// ===========================================================================
// member definitions
// ===========================================================================
std::vector<std::string>
InductionLoop::getIDList() {
    std::vector<std::string> ids;
    MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_INDUCTION_LOOP).insertIDs(ids);
    return ids;
}


int
InductionLoop::getIDCount() {
    std::vector<std::string> ids;
    return (int)MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_INDUCTION_LOOP).size();
}


double
InductionLoop::getPosition(const std::string& loopID) {
    return getDetector(loopID)->getPosition();
}


std::string
InductionLoop::getLaneID(const std::string& loopID) {
    return getDetector(loopID)->getLane()->getID();
}


int
InductionLoop::getLastStepVehicleNumber(const std::string& loopID) {
    return (int)getDetector(loopID)->getEnteredNumber((int)DELTA_T);
}


double
InductionLoop::getLastStepMeanSpeed(const std::string& loopID) {
    return getDetector(loopID)->getSpeed((int)DELTA_T);
}


std::vector<std::string>
InductionLoop::getLastStepVehicleIDs(const std::string& loopID) {
    return getDetector(loopID)->getVehicleIDs((int)DELTA_T);
}


double
InductionLoop::getLastStepOccupancy(const std::string& loopID) {
    return getDetector(loopID)->getOccupancy();
}


double
InductionLoop::getLastStepMeanLength(const std::string& loopID) {
    return getDetector(loopID)->getVehicleLength((int)DELTA_T);
}


double
InductionLoop::getTimeSinceDetection(const std::string& loopID) {
    return getDetector(loopID)->getTimeSinceLastDetection();
}

std::vector<libsumo::TraCIVehicleData>
InductionLoop::getVehicleData(const std::string& loopID) {
    const std::vector<MSInductLoop::VehicleData> vd = getDetector(loopID)->collectVehiclesOnDet(SIMSTEP - DELTA_T, true, true);
    std::vector<libsumo::TraCIVehicleData> tvd;
    for (const MSInductLoop::VehicleData& vdi : vd) {
        tvd.push_back(libsumo::TraCIVehicleData());
        tvd.back().id = vdi.idM;
        tvd.back().length = vdi.lengthM;
        tvd.back().entryTime = vdi.entryTimeM;
        tvd.back().leaveTime = vdi.leaveTimeM;
        tvd.back().typeID = vdi.typeIDM;
    }
    return tvd;
}


double
InductionLoop::getIntervalOccupancy(const std::string& loopID) {
    if (MSGlobals::gUseMesoSim) {
        const MEInductLoop* det = getMEDetector(loopID);
        const auto& meanData = det->getMeanData();
        return meanData.getOccupancy(SIMSTEP - meanData.getResetTime(), det->getEdge().getNumLanes());
    } else {
        return getDetector(loopID)->getIntervalOccupancy();
    }
}


double
InductionLoop::getIntervalMeanSpeed(const std::string& loopID) {
    if (MSGlobals::gUseMesoSim) {
        const MEInductLoop* det = getMEDetector(loopID);
        const auto& meanData = det->getMeanData();
        if (meanData.getSamples() != 0) {
            return meanData.getTravelledDistance() / meanData.getSamples();
        } else {
            const double defaultTravelTime = det->getEdge().getLength() / det->getEdge().getSpeedLimit();
            return meanData.getLaneLength() / defaultTravelTime;
        }
    } else {
        return getDetector(loopID)->getIntervalMeanSpeed();
    }
}


int
InductionLoop::getIntervalVehicleNumber(const std::string& loopID) {
    if (MSGlobals::gUseMesoSim) {
        const auto& meanData = getMEDetector(loopID)->getMeanData();
        return meanData.nVehDeparted + meanData.nVehEntered;
    } else {
        return getDetector(loopID)->getIntervalVehicleNumber();
    }
}


std::vector<std::string>
InductionLoop::getIntervalVehicleIDs(const std::string& loopID) {
    if (MSGlobals::gUseMesoSim) {
        WRITE_ERROR("getIntervalVehicleIDs not applicable for meso");
        return std::vector<std::string>();
    }
    return getDetector(loopID)->getIntervalVehicleIDs();
}


double
InductionLoop::getLastIntervalOccupancy(const std::string& loopID) {
    if (MSGlobals::gUseMesoSim) {
        WRITE_ERROR("getLastIntervalOccupancy not applicable for meso");
        return INVALID_DOUBLE_VALUE;
    }
    return getDetector(loopID)->getIntervalOccupancy(true);
}


double
InductionLoop::getLastIntervalMeanSpeed(const std::string& loopID) {
    if (MSGlobals::gUseMesoSim) {
        WRITE_ERROR("getLastIntervalMeanSpeed not applicable for meso");
        return INVALID_DOUBLE_VALUE;
    }
    return getDetector(loopID)->getIntervalMeanSpeed(true);
}


int
InductionLoop::getLastIntervalVehicleNumber(const std::string& loopID) {
    if (MSGlobals::gUseMesoSim) {
        WRITE_ERROR("getLastIntervalVehicleNumber not applicable for meso");
        return INVALID_INT_VALUE;
    }
    return getDetector(loopID)->getIntervalVehicleNumber(true);
}


std::vector<std::string>
InductionLoop::getLastIntervalVehicleIDs(const std::string& loopID) {
    if (MSGlobals::gUseMesoSim) {
        WRITE_ERROR("getLastIntervalVehicleIDs not applicable for meso");
        return std::vector<std::string>();
    }
    return getDetector(loopID)->getIntervalVehicleIDs(true);
}


void
InductionLoop::overrideTimeSinceDetection(const std::string& loopID, double time) {
    getDetector(loopID)->overrideTimeSinceDetection(time);
}


MSInductLoop*
InductionLoop::getDetector(const std::string& id) {
    MSInductLoop* il = dynamic_cast<MSInductLoop*>(MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_INDUCTION_LOOP).get(id));
    if (il == nullptr) {
        throw TraCIException("Induction loop '" + id + "' is not known");
    }
    return il;
}


MEInductLoop*
InductionLoop::getMEDetector(const std::string& id) {
    MEInductLoop* il = dynamic_cast<MEInductLoop*>(MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_INDUCTION_LOOP).get(id));
    if (il == nullptr) {
        throw TraCIException("Induction loop '" + id + "' is not known");
    }
    return il;
}



std::string
InductionLoop::getParameter(const std::string& loopID, const std::string& param) {
    return getDetector(loopID)->getParameter(param, "");
}


LIBSUMO_GET_PARAMETER_WITH_KEY_IMPLEMENTATION(InductionLoop)


void
InductionLoop::setParameter(const std::string& loopID, const std::string& name, const std::string& value) {
    getDetector(loopID)->setParameter(name, value);
}


LIBSUMO_SUBSCRIPTION_IMPLEMENTATION(InductionLoop, INDUCTIONLOOP)


NamedRTree*
InductionLoop::getTree() {
    if (myTree == nullptr) {
        myTree = new NamedRTree();
        for (const auto& i : MSNet::getInstance()->getDetectorControl().getTypedDetectors(SUMO_TAG_INDUCTION_LOOP)) {
            MSInductLoop* il = static_cast<MSInductLoop*>(i.second);
            Position p = il->getLane()->getShape().positionAtOffset(il->getPosition());
            const float cmin[2] = {(float) p.x(), (float) p.y()};
            const float cmax[2] = {(float) p.x(), (float) p.y()};
            myTree->Insert(cmin, cmax, il);
        }
    }
    return myTree;
}

void
InductionLoop::cleanup() {
    delete myTree;
    myTree = nullptr;
}

void
InductionLoop::storeShape(const std::string& id, PositionVector& shape) {
    MSInductLoop* const il = getDetector(id);
    shape.push_back(il->getLane()->getShape().positionAtOffset(il->getPosition()));
}


std::shared_ptr<VariableWrapper>
InductionLoop::makeWrapper() {
    return std::make_shared<Helper::SubscriptionWrapper>(handleVariable, mySubscriptionResults, myContextSubscriptionResults);
}


bool
InductionLoop::handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper, tcpip::Storage* paramData) {
    switch (variable) {
        case TRACI_ID_LIST:
            return wrapper->wrapStringList(objID, variable, getIDList());
        case ID_COUNT:
            return wrapper->wrapInt(objID, variable, getIDCount());
        case VAR_POSITION:
            return wrapper->wrapDouble(objID, variable, getPosition(objID));
        case VAR_LANE_ID:
            return wrapper->wrapString(objID, variable, getLaneID(objID));
        case LAST_STEP_VEHICLE_NUMBER:
            return wrapper->wrapInt(objID, variable, getLastStepVehicleNumber(objID));
        case LAST_STEP_MEAN_SPEED:
            return wrapper->wrapDouble(objID, variable, getLastStepMeanSpeed(objID));
        case LAST_STEP_VEHICLE_ID_LIST:
            return wrapper->wrapStringList(objID, variable, getLastStepVehicleIDs(objID));
        case LAST_STEP_OCCUPANCY:
            return wrapper->wrapDouble(objID, variable, getLastStepOccupancy(objID));
        case LAST_STEP_LENGTH:
            return wrapper->wrapDouble(objID, variable, getLastStepMeanLength(objID));
        case LAST_STEP_TIME_SINCE_DETECTION:
            return wrapper->wrapDouble(objID, variable, getTimeSinceDetection(objID));
        case VAR_INTERVAL_OCCUPANCY:
            return wrapper->wrapDouble(objID, variable, getIntervalOccupancy(objID));
        case VAR_INTERVAL_SPEED:
            return wrapper->wrapDouble(objID, variable, getIntervalMeanSpeed(objID));
        case VAR_INTERVAL_NUMBER:
            return wrapper->wrapInt(objID, variable, getIntervalVehicleNumber(objID));
        case VAR_INTERVAL_IDS:
            return wrapper->wrapStringList(objID, variable, getIntervalVehicleIDs(objID));
        case VAR_LAST_INTERVAL_OCCUPANCY:
            return wrapper->wrapDouble(objID, variable, getLastIntervalOccupancy(objID));
        case VAR_LAST_INTERVAL_SPEED:
            return wrapper->wrapDouble(objID, variable, getLastIntervalMeanSpeed(objID));
        case VAR_LAST_INTERVAL_NUMBER:
            return wrapper->wrapInt(objID, variable, getLastIntervalVehicleNumber(objID));
        case VAR_LAST_INTERVAL_IDS:
            return wrapper->wrapStringList(objID, variable, getLastIntervalVehicleIDs(objID));
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
