/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2023 German Aerospace Center (DLR) and others.
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
/// @file    Edge.cpp
/// @author  Gregor Laemmel
/// @date    15.09.2017
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>

#include <iterator>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdgeWeightsStorage.h>
#include <microsim/transportables/MSTransportable.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSInsertionControl.h>
#include <libsumo/Helper.h>
#include <libsumo/TraCIDefs.h>
#include <libsumo/TraCIConstants.h>
#include <utils/emissions/HelpersHarmonoise.h>
#include "Edge.h"


namespace libsumo {
// ===========================================================================
// static member initializations
// ===========================================================================
SubscriptionResults Edge::mySubscriptionResults;
ContextSubscriptionResults Edge::myContextSubscriptionResults;


// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string>
Edge::getIDList() {
    std::vector<std::string> ids;
    MSEdge::insertIDs(ids);
    return ids;
}


int
Edge::getIDCount() {
    return (int)getIDList().size();
}


double
Edge::getAdaptedTraveltime(const std::string& edgeID, double time) {
    const MSEdge* e = getEdge(edgeID);
    double value;
    if (!MSNet::getInstance()->getWeightsStorage().retrieveExistingTravelTime(e, time, value)) {
        return -1.;
    }
    return value;
}


double
Edge::getEffort(const std::string& edgeID, double time) {
    const MSEdge* e = getEdge(edgeID);
    double value;
    if (!MSNet::getInstance()->getWeightsStorage().retrieveExistingEffort(e, time, value)) {
        return -1.;
    }
    return value;
}


double
Edge::getTraveltime(const std::string& edgeID) {
    return getEdge(edgeID)->getCurrentTravelTime();
}


MSEdge*
Edge::getEdge(const std::string& edgeID) {
    MSEdge* e = MSEdge::dictionary(edgeID);
    if (e == nullptr) {
        throw TraCIException("Edge '" + edgeID + "' is not known");
    }
    return e;
}


double
Edge::getWaitingTime(const std::string& edgeID) {
    return getEdge(edgeID)->getWaitingSeconds();
}


const std::vector<std::string>
Edge::getLastStepPersonIDs(const std::string& edgeID) {
    std::vector<std::string> personIDs;
    std::vector<MSTransportable*> persons = getEdge(edgeID)->getSortedPersons(MSNet::getInstance()->getCurrentTimeStep(), true);
    personIDs.reserve(persons.size());
    for (MSTransportable* p : persons) {
        personIDs.push_back(p->getID());
    }
    return personIDs;
}


const std::vector<std::string>
Edge::getLastStepVehicleIDs(const std::string& edgeID) {
    std::vector<std::string> vehIDs;
    for (const SUMOVehicle* veh : getEdge(edgeID)->getVehicles()) {
        vehIDs.push_back(veh->getID());
    }
    return vehIDs;
}


double
Edge::getCO2Emission(const std::string& edgeID) {
    double sum = 0;
    for (MSLane* lane : getEdge(edgeID)->getLanes()) {
        sum += lane->getEmissions<PollutantsInterface::CO2>();
    }
    return sum;
}


double
Edge::getCOEmission(const std::string& edgeID) {
    double sum = 0;
    for (MSLane* lane : getEdge(edgeID)->getLanes()) {
        sum += lane->getEmissions<PollutantsInterface::CO>();
    }
    return sum;
}


double
Edge::getHCEmission(const std::string& edgeID) {
    double sum = 0;
    for (MSLane* lane : getEdge(edgeID)->getLanes()) {
        sum += lane->getEmissions<PollutantsInterface::HC>();
    }
    return sum;
}


double
Edge::getPMxEmission(const std::string& edgeID) {
    double sum = 0;
    for (MSLane* lane : getEdge(edgeID)->getLanes()) {
        sum += lane->getEmissions<PollutantsInterface::PM_X>();
    }
    return sum;
}


double
Edge::getNOxEmission(const std::string& edgeID) {
    double sum = 0;
    for (MSLane* lane : getEdge(edgeID)->getLanes()) {
        sum += lane->getEmissions<PollutantsInterface::NO_X>();
    }
    return sum;
}


double
Edge::getFuelConsumption(const std::string& edgeID) {
    double sum = 0;
    for (MSLane* lane : getEdge(edgeID)->getLanes()) {
        sum += lane->getEmissions<PollutantsInterface::FUEL>();
    }
    return sum;
}


double
Edge::getNoiseEmission(const std::string& edgeID) {
    double sum = 0;
    for (MSLane* lane : getEdge(edgeID)->getLanes()) {
        sum += pow(10., (lane->getHarmonoise_NoiseEmissions() / 10.));
    }
    if (sum != 0) {
        return HelpersHarmonoise::sum(sum);
    }
    return sum;
}


double
Edge::getElectricityConsumption(const std::string& edgeID) {
    double sum = 0;
    for (MSLane* lane : getEdge(edgeID)->getLanes()) {
        sum += lane->getEmissions<PollutantsInterface::ELEC>();
    }
    return sum;
}


int
Edge::getLastStepVehicleNumber(const std::string& edgeID) {
    return getEdge(edgeID)->getVehicleNumber();
}


double
Edge::getLastStepMeanSpeed(const std::string& edgeID) {
    return getEdge(edgeID)->getMeanSpeed();
}

double
Edge::getMeanFriction(const std::string& edgeID) {
    return getEdge(edgeID)->getMeanFriction();
}


double
Edge::getLastStepOccupancy(const std::string& edgeID) {
    return getEdge(edgeID)->getOccupancy();
}


int
Edge::getLastStepHaltingNumber(const std::string& edgeID) {
    int result = 0;
    for (const SUMOVehicle* veh : getEdge(edgeID)->getVehicles()) {
        if (veh->getSpeed() < SUMO_const_haltingSpeed) {
            result++;
        }
    }
    return result;
}


double
Edge::getLastStepLength(const std::string& edgeID) {
    double lengthSum = 0;
    int numVehicles = 0;
    for (const SUMOVehicle* veh : getEdge(edgeID)->getVehicles()) {
        numVehicles++;
        lengthSum += dynamic_cast<const MSBaseVehicle*>(veh)->getVehicleType().getLength();
    }
    if (numVehicles == 0) {
        return 0;
    }
    return lengthSum / numVehicles;
}


int
Edge::getLaneNumber(const std::string& edgeID) {
    return (int)getEdge(edgeID)->getLanes().size();
}


std::string
Edge::getStreetName(const std::string& edgeID) {
    return getEdge(edgeID)->getStreetName();
}


const std::vector<std::string>
Edge::getPendingVehicles(const std::string& edgeID) {
    getEdge(edgeID); // validate edgeID
    std::vector<std::string> vehIDs;
    for (const SUMOVehicle* veh : MSNet::getInstance()->getInsertionControl().getPendingVehicles()) {
        if (veh->getEdge()->getID() == edgeID) {
            vehIDs.push_back(veh->getID());
        }
    }
    return vehIDs;
}

std::string
Edge::getParameter(const std::string& edgeID, const std::string& param) {
    return getEdge(edgeID)->getParameter(param, "");
}


LIBSUMO_GET_PARAMETER_WITH_KEY_IMPLEMENTATION(Edge)


void
Edge::setAllowed(const std::string& edgeID, std::string allowedClasses) {
    setAllowedSVCPermissions(edgeID, parseVehicleClasses(allowedClasses));
}


void
Edge::setAllowed(const std::string& edgeID, std::vector<std::string> allowedClasses) {
    setAllowedSVCPermissions(edgeID, parseVehicleClasses(allowedClasses));
}


void
Edge::setDisallowed(const std::string& edgeID, std::string disallowedClasses) {
    setAllowedSVCPermissions(edgeID, invertPermissions(parseVehicleClasses(disallowedClasses)));
}


void
Edge::setDisallowed(const std::string& edgeID, std::vector<std::string> disallowedClasses) {
    setAllowedSVCPermissions(edgeID, invertPermissions(parseVehicleClasses(disallowedClasses)));
}


void
Edge::setAllowedSVCPermissions(const std::string& edgeID, int permissions) {
    MSEdge* e = getEdge(edgeID);
    for (MSLane* lane : e->getLanes()) {
        lane->setPermissions(permissions, MSLane::CHANGE_PERMISSIONS_PERMANENT);
    }
    e->rebuildAllowedLanes();
}


void
Edge::adaptTraveltime(const std::string& edgeID, double time, double beginSeconds, double endSeconds) {
    MSNet::getInstance()->getWeightsStorage().addTravelTime(getEdge(edgeID), beginSeconds, endSeconds, time);
}


void
Edge::setEffort(const std::string& edgeID, double effort, double beginSeconds, double endSeconds) {
    MSNet::getInstance()->getWeightsStorage().addEffort(getEdge(edgeID), beginSeconds, endSeconds, effort);
}


void
Edge::setMaxSpeed(const std::string& edgeID, double speed) {
    for (MSLane* lane : getEdge(edgeID)->getLanes()) {
        lane->setMaxSpeed(speed);
    }
}

void
Edge::setFriction(const std::string& edgeID, double value) {
    for (MSLane* lane : getEdge(edgeID)->getLanes()) {
        lane->setFrictionCoefficient(value);
    }
}

void
Edge::setParameter(const std::string& edgeID, const std::string& name, const std::string& value) {
    getEdge(edgeID)->setParameter(name, value);
}


LIBSUMO_SUBSCRIPTION_IMPLEMENTATION(Edge, EDGE)


void
Edge::storeShape(const std::string& edgeID, PositionVector& shape) {
    const MSEdge* const e = getEdge(edgeID);
    const std::vector<MSLane*>& lanes = e->getLanes();
    shape = lanes.front()->getShape();
    if (lanes.size() > 1) {
        copy(lanes.back()->getShape().begin(), lanes.back()->getShape().end(), back_inserter(shape));
    }
}


std::shared_ptr<VariableWrapper>
Edge::makeWrapper() {
    return std::make_shared<Helper::SubscriptionWrapper>(handleVariable, mySubscriptionResults, myContextSubscriptionResults);
}


bool
Edge::handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper, tcpip::Storage* paramData) {
    switch (variable) {
        case TRACI_ID_LIST:
            return wrapper->wrapStringList(objID, variable, getIDList());
        case ID_COUNT:
            return wrapper->wrapInt(objID, variable, getIDCount());
        case VAR_CURRENT_TRAVELTIME:
            return wrapper->wrapDouble(objID, variable, getTraveltime(objID));
        case VAR_WAITING_TIME:
            return wrapper->wrapDouble(objID, variable, getWaitingTime(objID));
        case LAST_STEP_PERSON_ID_LIST:
            return wrapper->wrapStringList(objID, variable, getLastStepPersonIDs(objID));
        case LAST_STEP_VEHICLE_ID_LIST:
            return wrapper->wrapStringList(objID, variable, getLastStepVehicleIDs(objID));
        case VAR_CO2EMISSION:
            return wrapper->wrapDouble(objID, variable, getCO2Emission(objID));
        case VAR_COEMISSION:
            return wrapper->wrapDouble(objID, variable, getCOEmission(objID));
        case VAR_HCEMISSION:
            return wrapper->wrapDouble(objID, variable, getHCEmission(objID));
        case VAR_PMXEMISSION:
            return wrapper->wrapDouble(objID, variable, getPMxEmission(objID));
        case VAR_NOXEMISSION:
            return wrapper->wrapDouble(objID, variable, getNOxEmission(objID));
        case VAR_FUELCONSUMPTION:
            return wrapper->wrapDouble(objID, variable, getFuelConsumption(objID));
        case VAR_NOISEEMISSION:
            return wrapper->wrapDouble(objID, variable, getNoiseEmission(objID));
        case VAR_ELECTRICITYCONSUMPTION:
            return wrapper->wrapDouble(objID, variable, getElectricityConsumption(objID));
        case LAST_STEP_VEHICLE_NUMBER:
            return wrapper->wrapInt(objID, variable, getLastStepVehicleNumber(objID));
        case LAST_STEP_MEAN_SPEED:
            return wrapper->wrapDouble(objID, variable, getLastStepMeanSpeed(objID));
        case VAR_FRICTION:
            return wrapper->wrapDouble(objID, variable, getMeanFriction(objID));
        case LAST_STEP_OCCUPANCY:
            return wrapper->wrapDouble(objID, variable, getLastStepOccupancy(objID));
        case LAST_STEP_VEHICLE_HALTING_NUMBER:
            return wrapper->wrapInt(objID, variable, getLastStepHaltingNumber(objID));
        case LAST_STEP_LENGTH:
            return wrapper->wrapDouble(objID, variable, getLastStepLength(objID));
        case VAR_LANE_INDEX:
            return wrapper->wrapInt(objID, variable, getLaneNumber(objID));
        case VAR_NAME:
            return wrapper->wrapString(objID, variable, getStreetName(objID));
        case VAR_PENDING_VEHICLES:
            return wrapper->wrapStringList(objID, variable, getPendingVehicles(objID));
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
