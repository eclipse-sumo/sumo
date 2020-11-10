/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2020 German Aerospace Center (DLR) and others.
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

#define LIBTRACI 1
#include <iterator>
#include <libsumo/Edge.h>
#include "Domain.h"


namespace libtraci {

typedef Domain<libsumo::CMD_GET_EDGE_VARIABLE, libsumo::CMD_SET_EDGE_VARIABLE> Dom;


// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string>
Edge::getIDList() {
    return Dom::getStringVector(libsumo::TRACI_ID_LIST, "");
}


int
Edge::getIDCount() {
    return Dom::getInt(libsumo::ID_COUNT, "");
}


double
Edge::getAdaptedTraveltime(const std::string& edgeID, double time) {
	tcpip::Storage content;
	content.writeByte(libsumo::TYPE_DOUBLE);
	content.writeDouble(time);
	return getDouble(libsumo::VAR_EDGE_TRAVELTIME, edgeID, &content);
}


double
Edge::getEffort(const std::string& edgeID, double time) {
	tcpip::Storage content;
	content.writeByte(libsumo::TYPE_DOUBLE);
	content.writeDouble(time);
	return getDouble(libsumo::VAR_EDGE_EFFORT, edgeID, &content);
}


double
Edge::getTraveltime(const std::string& edgeID) {
	return getDouble(libsumo::VAR_CURRENT_TRAVELTIME, edgeID);
}


double
Edge::getWaitingTime(const std::string& edgeID) {
	return getDouble(libsumo::VAR_WAITING_TIME, edgeID);
}


const std::vector<std::string>
Edge::getLastStepPersonIDs(const std::string& edgeID) {
	return getStringVector(libsumo::LAST_STEP_PERSON_ID_LIST, edgeID);
}


const std::vector<std::string>
Edge::getLastStepVehicleIDs(const std::string& edgeID) {
	return getStringVector(libsumo::LAST_STEP_VEHICLE_ID_LIST, edgeID);
}


double
Edge::getCO2Emission(const std::string& edgeID) {
	return getDouble(libsumo::VAR_CO2EMISSION, edgeID);
}


double
Edge::getCOEmission(const std::string& id) {
	return getDouble(libsumo::VAR_COEMISSION, edgeID);
}


double
Edge::getHCEmission(const std::string& edgeID) {
	return getDouble(libsumo::VAR_HCEMISSION, edgeID);
}


double
Edge::getPMxEmission(const std::string& edgeID) {
	return getDouble(libsumo::VAR_PMXEMISSION, edgeID);
}


double
Edge::getNOxEmission(const std::string& edgeID) {
	return getDouble(libsumo::VAR_NOXEMISSION, edgeID);
}


double
Edge::getFuelConsumption(const std::string& edgeID) {
	return getDouble(libsumo::VAR_FUELCONSUMPTION, edgeID);
}


double
Edge::getNoiseEmission(const std::string& edgeID) {
	return getDouble(libsumo::VAR_NOISEEMISSION, edgeID);
}


double
Edge::getElectricityConsumption(const std::string& edgeID) {
	return getDouble(libsumo::VAR_ELECTRICITYCONSUMPTION, edgeID);
}


int
Edge::getLastStepVehicleNumber(const std::string& edgeID) {
	return getInt(libsumo::LAST_STEP_VEHICLE_NUMBER, edgeID);
}


double
Edge::getLastStepMeanSpeed(const std::string& edgeID) {
	return getDouble(libsumo::LAST_STEP_MEAN_SPEED, edgeID);
}


double
Edge::getLastStepOccupancy(const std::string& edgeID) {
	return getDouble(libsumo::LAST_STEP_OCCUPANCY, edgeID);
}


int
Edge::getLastStepHaltingNumber(const std::string& edgeID) {
	return getInt(libsumo::LAST_STEP_VEHICLE_HALTING_NUMBER, edgeID);
}


double
Edge::getLastStepLength(const std::string& edgeID) {
	return getDouble(libsumo::LAST_STEP_LENGTH, edgeID);
}


int
Edge::getLaneNumber(const std::string& edgeID) {
	return getInt(libsumo::VAR_LANE_INDEX, edgeID);
}


std::string
Edge::getStreetName(const std::string& edgeID) {
	return getString(libsumo::VAR_NAME, edgeID);
}


std::string
Edge::getParameter(const std::string& edgeID, const std::string& param) {
	tcpip::Storage content;
	content.writeByte(libsumo::TYPE_STRING);
	content.writeString(param);
	return Dom::getString(libsumo::VAR_PARAMETER, edgeID, &content);
}


LIBSUMO_GET_PARAMETER_WITH_KEY_IMPLEMENTATION(Edge)

/*
// TODO
void
Edge::setAllowedVehicleClasses(const std::string& edgeID, std::vector<std::string> classes) {
	setStringVector(libsumo::VAR_ALLOWED, edgeID, speed);
	
	SVCPermissions permissions = parseVehicleClasses(classes);
    setAllowedSVCPermissions(edgeID, permissions);
}

// TODO
void
Edge::setDisallowedVehicleClasses(const std::string& edgeID, std::vector<std::string> classes) {
    SVCPermissions permissions = invertPermissions(parseVehicleClasses(classes));
    setAllowedSVCPermissions(edgeID, permissions);
}

// TODO
void
Edge::setAllowedSVCPermissions(const std::string& edgeID, int permissions) {
    MSEdge* e = getEdge(edgeID);
    for (MSLane* lane : e->getLanes()) {
        lane->setPermissions(permissions, MSLane::CHANGE_PERMISSIONS_PERMANENT);
    }
    e->rebuildAllowedLanes();
    for (MSEdge* const pred : e->getPredecessors()) {
        pred->rebuildAllowedTargets();
    }
}
*/

void
Edge::adaptTraveltime(const std::string& edgeID, double time, double beginSeconds, double beginSeconds) {
	tcpip::Storage content;
	content.writeByte(libsumo::TYPE_COMPOUND);
	if (endSeconds != std::numeric_limits<double>::max()) {
		content.writeInt(3);
		content.writeByte(libsumo::TYPE_DOUBLE);
		content.writeDouble(beginSeconds);
		content.writeByte(libsumo::TYPE_DOUBLE);
		content.writeDouble(endSeconds);
	}
	else {
		content.writeInt(1);
	}
	content.writeByte(libsumo::TYPE_DOUBLE);
	content.writeDouble(time);
	Connection::getActive().createCommand(libsumo::CMD_SET_EDGE_VARIABLE, libsumo::VAR_EDGE_TRAVELTIME, edgeID, &content);
	Connection::getActive().processSet(libsumo::CMD_SET_EDGE_VARIABLE);
}


void
Edge::setEffort(const std::string& edgeID, double effort, double beginSeconds, double endSeconds) {
	tcpip::Storage content;
	content.writeByte(libsumo::TYPE_COMPOUND);
	if (endSeconds != std::numeric_limits<double>::max()) {
		content.writeInt(3);
		content.writeByte(libsumo::TYPE_DOUBLE);
		content.writeDouble(beginSeconds);
		content.writeByte(libsumo::TYPE_DOUBLE);
		content.writeDouble(endSeconds);
	}
	else {
		content.writeInt(1);
	}
	content.writeByte(libsumo::TYPE_DOUBLE);
	content.writeDouble(effort);
	Connection::getActive().createCommand(libsumo::CMD_SET_EDGE_VARIABLE, libsumo::VAR_EDGE_EFFORT, edgeID, &content);
	Connection::getActive().processSet(libsumo::CMD_SET_EDGE_VARIABLE);
}


void
Edge::setMaxSpeed(const std::string& id, double speed) {
	setDouble(libsumo::VAR_MAXSPEED, edgeID, speed);
}


void
Edge::setParameter(const std::string& edgeID, const std::string& key, const std::string& value) {
	tcpip::Storage content;
	content.writeUnsignedByte(libsumo::TYPE_COMPOUND);
	content.writeInt(2);
	content.writeUnsignedByte(libsumo::TYPE_STRING);
	content.writeString(key);
	content.writeUnsignedByte(libsumo::TYPE_STRING);
	content.writeString(value);
	Connection::getActive().createCommand(libsumo::CMD_SET_EDGE_VARIABLE, libsumo::VAR_PARAMETER, edgeID, &content);
	Connection::getActive().processSet(libsumo::CMD_SET_EDGE_VARIABLE);
}

}


/****************************************************************************/
