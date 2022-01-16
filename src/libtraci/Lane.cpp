/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2022 German Aerospace Center (DLR) and others.
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
/// @file    Lane.cpp
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Robert Hilbrich
/// @author  Leonhard Luecken
/// @date    30.05.2012
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>

#define LIBTRACI 1
#include <libsumo/Lane.h>
#include <libsumo/TraCIConstants.h>
#include "Domain.h"


namespace libtraci {

typedef Domain<libsumo::CMD_GET_LANE_VARIABLE, libsumo::CMD_SET_LANE_VARIABLE> Dom;



// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string>
Lane::getIDList() {
    return Dom::getStringVector(libsumo::TRACI_ID_LIST, "");
}


int
Lane::getIDCount() {
    return Dom::getInt(libsumo::ID_COUNT, "");
}


std::string
Lane::getEdgeID(std::string laneID) {
    return Dom::getString(libsumo::LANE_EDGE_ID, laneID);
}


double
Lane::getLength(std::string laneID) {
    return Dom::getDouble(libsumo::VAR_LENGTH, laneID);
}


double
Lane::getMaxSpeed(std::string laneID) {
    return Dom::getDouble(libsumo::VAR_MAXSPEED, laneID);
}


int
Lane::getLinkNumber(std::string laneID) {
    return Dom::getInt(libsumo::LANE_LINK_NUMBER, laneID);
}


std::vector<libsumo::TraCIConnection>
Lane::getLinks(std::string laneID) {
    std::vector<libsumo::TraCIConnection> ret;
    tcpip::Storage& sto = Dom::get(libsumo::LANE_LINKS, laneID);
    sto.readUnsignedByte();
    sto.readInt();

    int linkNo = sto.readInt();
    for (int i = 0; i < linkNo; ++i) {

        sto.readUnsignedByte();
        std::string approachedLane = sto.readString();

        sto.readUnsignedByte();
        std::string approachedLaneInternal = sto.readString();

        sto.readUnsignedByte();
        bool hasPrio = sto.readUnsignedByte() != 0;

        sto.readUnsignedByte();
        bool isOpen = sto.readUnsignedByte() != 0;

        sto.readUnsignedByte();
        bool hasFoe = sto.readUnsignedByte() != 0;

        sto.readUnsignedByte();
        std::string state = sto.readString();

        sto.readUnsignedByte();
        std::string direction = sto.readString();

        sto.readUnsignedByte();
        double length = sto.readDouble();

        ret.push_back(libsumo::TraCIConnection(approachedLane,
                                               hasPrio,
                                               isOpen,
                                               hasFoe,
                                               approachedLaneInternal,
                                               state,
                                               direction,
                                               length));

    }
    return ret;
}


std::vector<std::string>
Lane::getAllowed(std::string laneID) {
    return Dom::getStringVector(libsumo::LANE_ALLOWED, laneID);
}


std::vector<std::string>
Lane::getDisallowed(std::string laneID) {
    return Dom::getStringVector(libsumo::LANE_DISALLOWED, laneID); // negation yields disallowed
}


libsumo::TraCIPositionVector
Lane::getShape(std::string laneID) {
    return Dom::getPolygon(libsumo::VAR_SHAPE, laneID);
}


double
Lane::getWidth(std::string laneID) {
    return Dom::getDouble(libsumo::VAR_WIDTH, laneID);
}


double
Lane::getCO2Emission(std::string laneID) {
    return Dom::getDouble(libsumo::VAR_CO2EMISSION, laneID);
}


double
Lane::getCOEmission(std::string laneID) {
    return Dom::getDouble(libsumo::VAR_COEMISSION, laneID);
}


double
Lane::getHCEmission(std::string laneID) {
    return Dom::getDouble(libsumo::VAR_HCEMISSION, laneID);
}


double
Lane::getPMxEmission(std::string laneID) {
    return Dom::getDouble(libsumo::VAR_PMXEMISSION, laneID);
}


double
Lane::getNOxEmission(std::string laneID) {
    return Dom::getDouble(libsumo::VAR_NOXEMISSION, laneID);
}

double
Lane::getFuelConsumption(std::string laneID) {
    return Dom::getDouble(libsumo::VAR_FUELCONSUMPTION, laneID);
}


double
Lane::getNoiseEmission(std::string laneID) {
    return Dom::getDouble(libsumo::VAR_NOISEEMISSION, laneID);
}


double
Lane::getElectricityConsumption(std::string laneID) {
    return Dom::getDouble(libsumo::VAR_ELECTRICITYCONSUMPTION, laneID);
}


double
Lane::getLastStepMeanSpeed(std::string laneID) {
    return Dom::getDouble(libsumo::LAST_STEP_MEAN_SPEED, laneID);
}


double
Lane::getLastStepOccupancy(std::string laneID) {
    return Dom::getDouble(libsumo::LAST_STEP_OCCUPANCY, laneID);
}


double
Lane::getLastStepLength(std::string laneID) {
    return Dom::getDouble(libsumo::LAST_STEP_LENGTH, laneID);
}


double
Lane::getWaitingTime(std::string laneID) {
    return Dom::getDouble(libsumo::VAR_WAITING_TIME, laneID);
}


double
Lane::getTraveltime(std::string laneID) {
    return Dom::getDouble(libsumo::VAR_CURRENT_TRAVELTIME, laneID);
}


int
Lane::getLastStepVehicleNumber(std::string laneID) {
    return Dom::getInt(libsumo::LAST_STEP_VEHICLE_NUMBER, laneID);
}

int
Lane::getLastStepHaltingNumber(std::string laneID) {
    return Dom::getInt(libsumo::LAST_STEP_VEHICLE_HALTING_NUMBER, laneID);
}


std::vector<std::string>
Lane::getLastStepVehicleIDs(std::string laneID) {
    return Dom::getStringVector(libsumo::LAST_STEP_VEHICLE_ID_LIST, laneID);
}


std::vector<std::string>
Lane::getFoes(const std::string& laneID, const std::string& toLaneID) {
    tcpip::Storage content;
    content.writeUnsignedByte(libsumo::TYPE_STRING);
    content.writeString(toLaneID);
    return Dom::getStringVector(libsumo::VAR_FOES, laneID, &content);
}

// XXX: there seems to be no "Dom::getFoes"
std::vector<std::string>
Lane::getInternalFoes(const std::string& laneID) {
    //tcpip::Storage content;
    //content.writeUnsignedByte(libsumo::TYPE_STRING);
    //content.writeString("");
    //return Dom::getStringVector(libsumo::VAR_FOES, laneID, &content);
    return getFoes(laneID, "");
    //return Dom::getFoes(laneID, "");
}

const std::vector<std::string>
Lane::getPendingVehicles(const std::string& laneID) {
    return Dom::getStringVector(libsumo::VAR_PENDING_VEHICLES, laneID);
}

void
Lane::setAllowed(std::string laneID, std::string allowedClass) {
    Dom::setString(libsumo::LANE_ALLOWED, laneID, allowedClass);
}


void
Lane::setAllowed(std::string laneID, std::vector<std::string> allowedClasses) {
    Dom::setStringVector(libsumo::LANE_ALLOWED, laneID, allowedClasses);
}


void
Lane::setDisallowed(std::string laneID, std::vector<std::string> disallowedClasses) {
    Dom::setStringVector(libsumo::LANE_DISALLOWED, laneID, disallowedClasses);
}


void
Lane::setMaxSpeed(std::string laneID, double speed) {
    Dom::setDouble(libsumo::VAR_MAXSPEED, laneID, speed);
}


void
Lane::setLength(std::string laneID, double length) {
    Dom::setDouble(libsumo::VAR_LENGTH, laneID, length);
}


LIBTRACI_SUBSCRIPTION_IMPLEMENTATION(Lane, LANE)
LIBTRACI_PARAMETER_IMPLEMENTATION(Lane, LANE)

}


/****************************************************************************/
