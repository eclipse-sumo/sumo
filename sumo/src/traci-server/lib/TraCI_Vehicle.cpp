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

MSVehicle* getVehicle(const std::string& id) {
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
isVisible(const MSVehicle* veh) {
    return veh->isOnRoad() || veh->isParking();
}

std::vector<std::string>
getIDList() {
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
getIDCount() {
    return (int)getIDList().size();
}

double
getSpeed(const std::string& vehicleID) {
    MSVehicle* veh = getVehicle(vehicleID);
    return isVisible(veh) ? veh->getSpeed() : INVALID_DOUBLE_VALUE;
}


double
getSpeedWithoutTraCI(const std::string& vehicleID) {
    MSVehicle* veh = getVehicle(vehicleID);
    return isVisible(veh) ? veh->getSpeedWithoutTraciInfluence() : INVALID_DOUBLE_VALUE;
}


TraCIPosition
getPosition(const std::string& vehicleID) {
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
getAngle(const std::string& vehicleID) {
}

std::string
getRoadID(const std::string& vehicleID) {
}

std::string
getLaneID(const std::string& vehicleID) {
}

int
getLaneIndex(const std::string& vehicleID) {
}

std::string
getTypeID(const std::string& vehicleID) {
}

std::string
getRouteID(const std::string& vehicleID) {
}

int
getRouteIndex(const std::string& vehicleID) {
}

std::vector<std::string>
getEdges(const std::string& vehicleID) {
}

TraCIColor
getColor(const std::string& vehicleID) {
}

double
getLanePosition(const std::string& vehicleID) {
}

double
getCO2Emission(const std::string& vehicleID) {
}

double
getCOEmission(const std::string& vehicleID) {
}

double
getHCEmission(const std::string& vehicleID) {
}

double
getPMxEmission(const std::string& vehicleID) {
}

double
getNOxEmission(const std::string& vehicleID) {
}

double
getFuelConsumption(const std::string& vehicleID) {
}

double
getNoiseEmission(const std::string& vehicleID) {
}

double
getElectricityConsumption(const std::string& vehicleID) {
}

int
getSignalStates(const std::string& vehicleID) {
}

double
getWaitingTime(const std::string& vehicleID) {
}

std::vector<NextTLSData>
getNextTLS(const std::string& vehID) {
}

int
getSpeedMode(const std::string& vehicleID) {
}

double
getSlope(const std::string& vehicleID) {
}

std::string
getLine(const std::string& vehicleID) {
}

std::vector<std::string>
getVia(const std::string& vehicleID) {
}

std::string
getEmissionClass(const std::string& vehicleID) {
}

std::string
getShapeClass(const std::string& vehicleID) {
}

double
getCO2Emissions(const std::string& vehicleID) {
}

double
getCOEmissions(const std::string& vehicleID) {
}

double
getHCEmissions(const std::string& vehicleID) {
}

double
getPMxEmissions(const std::string& vehicleID) {
}

double
getNOxEmissions(const std::string& vehicleID) {
}

double
getFuelConsumption(const std::string& vehicleID) {
}

double
getNoiseEmission(const std::string& vehicleID) {
}

int
getBestLanes(const std::string& vehicleID) {
}

int
getStopState(const std::string& vehicleID) {
}

double
getLength(const std::string& vehicleID) {
}

double
getAccel(const std::string& vehicleID) {
}

double
getDecel(const std::string& vehicleID) {
}

double
getTau(const std::string& vehicleID) {
}

double
getImperfection(const std::string& vehicleID) {
}

double
getSpeedFactor(const std::string& vehicleID) {
}

double
getSpeedDeviation(const std::string& vehicleID) {
}

std::string
getVClass(const std::string& vehicleID) {
}

double
getMinGap(const std::string& vehicleID) {
}

double
getMaxSpeed(const std::string& vehicleID) {
}


double
getWidth(const std::string& vehicleID) {
}


void 
add(const std::string& vehicleID,
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
moveTo(const std::string& vehicleID, const std::string& laneID, double position) {
}

void
moveToXY(const std::string& vehicleID, const std::string& edgeID, const int lane, const double x, const double y, const double angle, const int keepRoute) {
}

void
slowDown(const std::string& vehicleID, double speed, int duration) {
}

void
setSpeed(const std::string& vehicleID, double speed) {
}

void
setMaxSpeed(const std::string& vehicleID, double speed) {
}

void
remove(const std::string& vehicleID, char reason) {
}

void
setColor(const std::string& vehicleID, const TraCIColor& c) {
}

void
setLine(const std::string& vehicleID, const std::string& line) {
}

void
setVia(const std::string& vehicleID, const std::vector<std::string>& via) {
}

void
setShapeClass(const std::string& vehicleID, const std::string& clazz) {
}

void
setEmissionClass(const std::string& vehicleID, const std::string& clazz) {
}


/****************************************************************************/
