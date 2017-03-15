/****************************************************************************/
/// @file    TraCI_Vehicle.h
/// @author  Jakob Erdmann
/// @date    15.03.2017
/// @version $Id$
///
// C++ Vehicle API 
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
#include "../../config.h"
#endif

#include <microsim/MSVehicle.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <traci-server/TraCIDefs.h>
#include <traci-server/TraCIConstants.h>
#include "TraCI.h"
#include "TraCI_Vehicle.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member definitions
// ===========================================================================

MSVehicle* 
TraCI_Vehicle::getVehicle(const std::string& id) {
    SUMOVehicle* sumoVehicle = MSNet::getInstance()->getVehicleControl().getVehicle(id);
    if (sumoVehicle == 0) {
        throw TraCIException("Vehicle '" + id + "' is not known");
    }
    MSVehicle* v = dynamic_cast<MSVehicle*>(sumoVehicle);
    if (v == 0) {
        throw TraCIException("Vehicle '" + id +  "' is not a micro-simulation vehicle");
    }
    return v;
}


bool 
TraCI_Vehicle::isVisible(const MSVehicle* veh) {
    return veh->isOnRoad() || veh->isParking();
}

std::vector<std::string>
TraCI_Vehicle::getIDList() {
    std::vector<std::string> ids;
    MSVehicleControl& c = MSNet::getInstance()->getVehicleControl();
    for (MSVehicleControl::constVehIt i = c.loadedVehBegin(); i != c.loadedVehEnd(); ++i) {
        if ((*i).second->isOnRoad() || (*i).second->isParking()) {
            ids.push_back((*i).first);
        }
    }
    return ids;
}

int
TraCI_Vehicle::getIDCount() {
    return (int)getIDList().size();
}

double
TraCI_Vehicle::getSpeed(const std::string& vehicleID) {
    MSVehicle* veh = getVehicle(vehicleID);
    return isVisible(veh) ? veh->getSpeed() : INVALID_DOUBLE_VALUE;
}


double
TraCI_Vehicle::getSpeedWithoutTraCI(const std::string& vehicleID) {
    MSVehicle* veh = getVehicle(vehicleID);
    return isVisible(veh) ? veh->getSpeedWithoutTraciInfluence() : INVALID_DOUBLE_VALUE;
}


TraCIPosition
TraCI_Vehicle::getPosition(const std::string& vehicleID) {
    MSVehicle* veh = getVehicle(vehicleID);
    if (isVisible(veh)) {
        return TraCI::makeTraCIPosition(veh->getPosition());
    } else {
        TraCIPosition result;
        result.x = INVALID_DOUBLE_VALUE;
        result.y = INVALID_DOUBLE_VALUE;
        result.z = INVALID_DOUBLE_VALUE;
        return result;
    }
}


double
TraCI_Vehicle::getAngle(const std::string& vehicleID) {
    MSVehicle* veh = getVehicle(vehicleID);
    return isVisible(veh) ? veh->getSlope() : INVALID_DOUBLE_VALUE;
}

std::string
TraCI_Vehicle::getRoadID(const std::string& vehicleID) {
    MSVehicle* veh = getVehicle(vehicleID);
    return isVisible(veh) ? veh->getLane()->getEdge().getID() : "";
}


std::string
TraCI_Vehicle::getLaneID(const std::string& vehicleID) {
    MSVehicle* veh = getVehicle(vehicleID);
    return isVisible(veh) ? veh->getLane()->getID() : "";
}


int
TraCI_Vehicle::getLaneIndex(const std::string& vehicleID) {
    MSVehicle* veh = getVehicle(vehicleID);
    return veh->isOnRoad() ? veh->getLane()->getIndex() : INVALID_INT_VALUE;
}

std::string
TraCI_Vehicle::getTypeID(const std::string& vehicleID) {
    return getVehicle(vehicleID)->getVehicleType().getID();
}

std::string
TraCI_Vehicle::getRouteID(const std::string& vehicleID) {
    return getVehicle(vehicleID)->getRoute().getID();
}

int
TraCI_Vehicle::getRouteIndex(const std::string& vehicleID) {
    MSVehicle* veh = getVehicle(vehicleID);
    return veh->hasDeparted() ? veh->getRoutePosition() : INVALID_INT_VALUE;
}

std::vector<std::string>
TraCI_Vehicle::getEdges(const std::string& vehicleID) {
}

TraCIColor
TraCI_Vehicle::getColor(const std::string& vehicleID) {
}

double
TraCI_Vehicle::getLanePosition(const std::string& vehicleID) {
    MSVehicle* veh = getVehicle(vehicleID);
    return veh->isOnRoad() ? veh->getPositionOnLane() : INVALID_DOUBLE_VALUE;
}

double
TraCI_Vehicle::getCO2Emission(const std::string& vehicleID) {
}

double
TraCI_Vehicle::getCOEmission(const std::string& vehicleID) {
}

double
TraCI_Vehicle::getHCEmission(const std::string& vehicleID) {
}

double
TraCI_Vehicle::getPMxEmission(const std::string& vehicleID) {
}

double
TraCI_Vehicle::getNOxEmission(const std::string& vehicleID) {
}

double
TraCI_Vehicle::getFuelConsumption(const std::string& vehicleID) {
}

double
TraCI_Vehicle::getNoiseEmission(const std::string& vehicleID) {
}

double
TraCI_Vehicle::getElectricityConsumption(const std::string& vehicleID) {
}

int
TraCI_Vehicle::getSignalStates(const std::string& vehicleID) {
}

double
TraCI_Vehicle::getWaitingTime(const std::string& vehicleID) {
}

std::vector<TraCI_Vehicle::NextTLSData>
TraCI_Vehicle::getNextTLS(const std::string& vehID) {
}

int
TraCI_Vehicle::getSpeedMode(const std::string& vehicleID) {
}

double
TraCI_Vehicle::getSlope(const std::string& vehicleID) {
}

std::string
TraCI_Vehicle::getLine(const std::string& vehicleID) {
}

std::vector<std::string>
TraCI_Vehicle::getVia(const std::string& vehicleID) {
}

std::string
TraCI_Vehicle::getEmissionClass(const std::string& vehicleID) {
}

std::string
TraCI_Vehicle::getShapeClass(const std::string& vehicleID) {
}


int
TraCI_Vehicle::getBestLanes(const std::string& vehicleID) {
}

int
TraCI_Vehicle::getStopState(const std::string& vehicleID) {
}

double
TraCI_Vehicle::getLength(const std::string& vehicleID) {
}

double
TraCI_Vehicle::getAccel(const std::string& vehicleID) {
}

double
TraCI_Vehicle::getDecel(const std::string& vehicleID) {
}

double
TraCI_Vehicle::getTau(const std::string& vehicleID) {
}

double
TraCI_Vehicle::getImperfection(const std::string& vehicleID) {
}

double
TraCI_Vehicle::getSpeedFactor(const std::string& vehicleID) {
}

double
TraCI_Vehicle::getSpeedDeviation(const std::string& vehicleID) {
}

std::string
TraCI_Vehicle::getVClass(const std::string& vehicleID) {
}

double
TraCI_Vehicle::getMinGap(const std::string& vehicleID) {
}

double
TraCI_Vehicle::getMaxSpeed(const std::string& vehicleID) {
}


double
TraCI_Vehicle::getWidth(const std::string& vehicleID) {
}


void 
TraCI_Vehicle::add(const std::string& vehicleID,
 const std::string& routeID,
 const std::string& typeID,
 std::string depart,
 const std::string& departLane,
 const std::string& departPos,
 const std::string& departSpeed,
 const std::string& arrivalLane,
 const std::string& arrivalPos,
 const std::string& arrivalSpeed,
 const std::string& fromTaz,
 const std::string& toTaz,
 const std::string& line,
 int personCapacity,
 int personNumber) {
}

void
TraCI_Vehicle::moveTo(const std::string& vehicleID, const std::string& laneID, double position) {
}

void
TraCI_Vehicle::moveToXY(const std::string& vehicleID, const std::string& edgeID, const int lane, const double x, const double y, const double angle, const int keepRoute) {
}

void
TraCI_Vehicle::slowDown(const std::string& vehicleID, double speed, int duration) {
}

void
TraCI_Vehicle::setSpeed(const std::string& vehicleID, double speed) {
}

void
TraCI_Vehicle::setMaxSpeed(const std::string& vehicleID, double speed) {
}

void
TraCI_Vehicle::remove(const std::string& vehicleID, char reason) {
}

void
TraCI_Vehicle::setColor(const std::string& vehicleID, const TraCIColor& c) {
}

void
TraCI_Vehicle::setLine(const std::string& vehicleID, const std::string& line) {
}

void
TraCI_Vehicle::setVia(const std::string& vehicleID, const std::vector<std::string>& via) {
}

void
TraCI_Vehicle::setShapeClass(const std::string& vehicleID, const std::string& clazz) {
}

void
TraCI_Vehicle::setEmissionClass(const std::string& vehicleID, const std::string& clazz) {
}


/****************************************************************************/
