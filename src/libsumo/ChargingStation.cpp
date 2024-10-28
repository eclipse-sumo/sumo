/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2017-2024 German Aerospace Center (DLR) and others.
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
/// @file    ChargingStation.cpp
/// @author  Jakob Erdmann
/// @date    16.03.2020
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>

#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSStoppingPlace.h>
#include <microsim/trigger/MSChargingStation.h>
#include <libsumo/TraCIConstants.h>
#include <utils/common/SUMOTime.h>
#include "Helper.h"
#include "ChargingStation.h"


namespace libsumo {
// ===========================================================================
// static member initializations
// ===========================================================================

SubscriptionResults ChargingStation::mySubscriptionResults;
ContextSubscriptionResults ChargingStation::myContextSubscriptionResults;

// ===========================================================================
// static member definitions
// ===========================================================================

std::vector<std::string>
ChargingStation::getIDList() {
    std::vector<std::string> ids;
    for (auto& item : MSNet::getInstance()->getStoppingPlaces(SUMO_TAG_CHARGING_STATION)) {
        ids.push_back(item.first);
    }
    std::sort(ids.begin(), ids.end());
    return ids;
}


int
ChargingStation::getIDCount() {
    return (int)getIDList().size();
}


std::string
ChargingStation::getLaneID(const std::string& stopID) {
    return getChargingStation(stopID)->getLane().getID();
}

double
ChargingStation::getStartPos(const std::string& stopID) {
    return getChargingStation(stopID)->getBeginLanePosition();
}


double
ChargingStation::getEndPos(const std::string& stopID) {
    return getChargingStation(stopID)->getEndLanePosition();
}


std::string
ChargingStation::getName(const std::string& stopID) {
    return getChargingStation(stopID)->getMyName();
}


int
ChargingStation::getVehicleCount(const std::string& stopID) {
    return (int)getChargingStation(stopID)->getStoppedVehicles().size();
}


std::vector<std::string>
ChargingStation::getVehicleIDs(const std::string& stopID) {
    std::vector<std::string> result;
    for (const SUMOVehicle* veh : getChargingStation(stopID)->getStoppedVehicles()) {
        result.push_back(veh->getID());
    }
    return result;
}


double
ChargingStation::getChargingPower(const std::string& stopID) {
    return dynamic_cast<MSChargingStation*>(getChargingStation(stopID))->getChargingPower(true);
}


double
ChargingStation::getEfficiency(const std::string& stopID) {
    return dynamic_cast<MSChargingStation*>(getChargingStation(stopID))->getEfficency();
}


double
ChargingStation::getChargeDelay(const std::string& stopID) {
    return STEPS2TIME(dynamic_cast<MSChargingStation*>(getChargingStation(stopID))->getChargeDelay());
}


int
ChargingStation::getChargeInTransit(const std::string& stopID) {
    return dynamic_cast<MSChargingStation*>(getChargingStation(stopID))->getChargeInTransit();
}


void
ChargingStation::setChargingPower(const std::string& stopID, double chargingpower) {
    dynamic_cast<MSChargingStation*>(getChargingStation(stopID))->setChargingPower(chargingpower);
}


void
ChargingStation::setEfficiency(const std::string& stopID, double efficiency) {
    dynamic_cast<MSChargingStation*>(getChargingStation(stopID))->setEfficiency(efficiency);
}


void
ChargingStation::setChargeDelay(const std::string& stopID, double delay) {
    dynamic_cast<MSChargingStation*>(getChargingStation(stopID))->setChargeDelay(TIME2STEPS(delay));
}


void
ChargingStation::setChargeInTransit(const std::string& stopID, bool value) {
    dynamic_cast<MSChargingStation*>(getChargingStation(stopID))->setChargeInTransit(value);
}


std::string
ChargingStation::getParameter(const std::string& stopID, const std::string& param) {
    return getChargingStation(stopID)->getParameter(param, "");
}


LIBSUMO_GET_PARAMETER_WITH_KEY_IMPLEMENTATION(ChargingStation)

void
ChargingStation::setParameter(const std::string& stopID, const std::string& key, const std::string& value) {
    getChargingStation(stopID)->setParameter(key, value);
}


LIBSUMO_SUBSCRIPTION_IMPLEMENTATION(ChargingStation, CHARGINGSTATION)


MSStoppingPlace*
ChargingStation::getChargingStation(const std::string& id) {
    return Helper::getStoppingPlace(id, SUMO_TAG_CHARGING_STATION);
}


std::shared_ptr<VariableWrapper>
ChargingStation::makeWrapper() {
    return std::make_shared<Helper::SubscriptionWrapper>(handleVariable, mySubscriptionResults, myContextSubscriptionResults);
}


bool
ChargingStation::handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper, tcpip::Storage* paramData) {
    switch (variable) {
        case TRACI_ID_LIST:
            return wrapper->wrapStringList(objID, variable, getIDList());
        case ID_COUNT:
            return wrapper->wrapInt(objID, variable, getIDCount());
        case VAR_LANE_ID:
            return wrapper->wrapString(objID, variable, getLaneID(objID));
        case VAR_POSITION:
            return wrapper->wrapDouble(objID, variable, getStartPos(objID));
        case VAR_LANEPOSITION:
            return wrapper->wrapDouble(objID, variable, getEndPos(objID));
        case VAR_NAME:
            return wrapper->wrapString(objID, variable, getName(objID));
        case VAR_STOP_STARTING_VEHICLES_NUMBER:
            return wrapper->wrapInt(objID, variable, getVehicleCount(objID));
        case VAR_STOP_STARTING_VEHICLES_IDS:
            return wrapper->wrapStringList(objID, variable, getVehicleIDs(objID));
        case VAR_CS_POWER:
            return wrapper->wrapDouble(objID, variable, getChargingPower(objID));
        case VAR_CS_EFFICIENCY:
            return wrapper->wrapDouble(objID, variable, getEfficiency(objID));
        case VAR_CS_CHARGE_DELAY:
            return wrapper->wrapDouble(objID, variable, STEPS2TIME(getChargeDelay(objID)));
        case VAR_CS_CHARGE_IN_TRANSIT:
            return wrapper->wrapInt(objID, variable, getChargeInTransit(objID));
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
