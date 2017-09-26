/****************************************************************************/
/// @file    TraCI_Edge.cpp
/// @author  Gregor Laemmel
/// @date    15.09.2017
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2017-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdgeWeightsStorage.h>
#include <microsim/MSTransportable.h>
#include <microsim/MSVehicle.h>
#include <traci-server/TraCIDefs.h>
#include <utils/emissions/HelpersHarmonoise.h>
#include "TraCI_Edge.h"


std::vector<std::string>
TraCI_Edge::getIDList() {
    std::vector<std::string> ids;
    MSEdge::insertIDs(ids);
    return ids;
}

int
TraCI_Edge::getIDCount() {
    return (int) getIDList().size();
}

double TraCI_Edge::retrieveExistingTravelTime(std::string& id, double time) {
    const MSEdge* e = getEdge(id);
    double value;
    if (!MSNet::getInstance()->getWeightsStorage().retrieveExistingTravelTime(e, time, value)) {
        return -1.;
    }
    return value;
}

double
TraCI_Edge::retrieveExistingEffort(std::string& id, double time) {
    const MSEdge* e = getEdge(id);
    double value;
    if (!MSNet::getInstance()->getWeightsStorage().retrieveExistingEffort(e, time, value)) {
        return -1.;
    }
    return value;
}

double
TraCI_Edge::getCurrentTravelTime(std::string& id) {
    return getEdge(id)->getCurrentTravelTime();
}

MSEdge*
TraCI_Edge::getEdge(const std::string& id) {
    MSEdge* e = MSEdge::dictionary(id);
    if (e == nullptr) {
        throw TraCIException("Edge '" + id + "' is not known");
    }
    return e;
}

double
TraCI_Edge::getWaitingSeconds(std::string& id) {
    double wtime = 0;
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        wtime += lane->getWaitingSeconds();
    }
    return wtime;
}

const std::vector<std::string>
TraCI_Edge::getPersonIDs(std::string& id) {
    std::vector<std::string> personIDs;
    std::vector<MSTransportable*> persons = getEdge(id)->getSortedPersons(MSNet::getInstance()->getCurrentTimeStep(), true);
    personIDs.reserve(persons.size());
    for (MSTransportable* p : persons) {
        personIDs.push_back(p->getID());
    }
    return personIDs;
}

const std::vector<std::string>
TraCI_Edge::getVehicleIDs(std::string& id) {
    std::vector<std::string> vehIDs;
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        const MSLane::VehCont& vehs = lane->getVehiclesSecure();
        for (auto veh : vehs) {
            vehIDs.push_back(veh->getID());
        }
        lane->releaseVehicles();
    }
    return vehIDs;
}


double
TraCI_Edge::getCO2Emissions(std::string& id) {
    double sum = 0;
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        sum += lane->getCO2Emissions();
    }
    return sum;
}

double
TraCI_Edge::getCOEmissions(std::string& id) {
    double sum = 0;
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        sum += lane->getCOEmissions();
    }
    return sum;
}

double
TraCI_Edge::getHCEmissions(std::string& id) {
    double sum = 0;
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        sum += lane->getHCEmissions();
    }
    return sum;
}

double
TraCI_Edge::getPMxEmissions(std::string& id) {
    double sum = 0;
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        sum += lane->getPMxEmissions();
    }
    return sum;
}

double
TraCI_Edge::getNOxEmissions(std::string& id) {
    double sum = 0;
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        sum += lane->getNOxEmissions();
    }
    return sum;
}

double
TraCI_Edge::getFuelConsumption(std::string& id) {
    double sum = 0;
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        sum += lane->getFuelConsumption();
    }
    return sum;
}

double
TraCI_Edge::getNoiseEmissions(std::string& id) {
    double sum = 0;
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        sum += pow(10., (lane->getHarmonoise_NoiseEmissions() / 10.));
    }
    if (sum != 0) {
        return HelpersHarmonoise::sum(sum);
    }
    return sum;
}

double
TraCI_Edge::getElectricityConsumption(std::string& id) {
    double sum = 0;
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        sum += lane->getElectricityConsumption();
    }
    return sum;
}

int
TraCI_Edge::getVehicleNumber(std::string& id) {
    int sum = 0;
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        sum += lane->getVehicleNumber();
    }
    return sum;
}

double
TraCI_Edge::getMeanSpeed(std::string& id) {
    return getEdge(id)->getMeanSpeed();
}

double
TraCI_Edge::getOccupancy(std::string& id) {
    double sum = 0;
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        sum += lane->getNettoOccupancy();
    }
    return sum / (double) lanes.size();
}

int
TraCI_Edge::getVehicleHaltingNumber(std::string& id) {
    int halting = 0;
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        const MSLane::VehCont& vehs = lane->getVehiclesSecure();
        for (auto veh : vehs) {
            if (veh->getSpeed() < SUMO_const_haltingSpeed) {
                ++halting;
            }
        }
        lane->releaseVehicles();
    }
    return halting;
}

double
TraCI_Edge::getVehicleAverageLength(std::string& id) {
    double lengthSum = 0;
    int noVehicles = 0;
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        const MSLane::VehCont& vehs = lane->getVehiclesSecure();
        for (auto veh : vehs) {
            lengthSum += veh->getVehicleType().getLength();
        }
        noVehicles += (int) vehs.size();
        lane->releaseVehicles();
    }
    if (noVehicles == 0) {
        return 0;
    }
    return lengthSum / (double) noVehicles;
}

const std::string&
TraCI_Edge::getParameter(std::string& id, std::string& paramName) {
    return getEdge(id)->getParameter(paramName, "");
}

void
TraCI_Edge::setAllowdVehicleClasses(std::string& id, std::vector<std::string> classes) {
    SVCPermissions permissions = parseVehicleClasses(classes);
    setAllowdSVCPermissions(id, permissions);
}

void
TraCI_Edge::setDisallowedVehicleClasses(std::string& id, std::vector<std::string> classes) {
    SVCPermissions permissions = invertPermissions(parseVehicleClasses(classes));
    setAllowdSVCPermissions(id, permissions);
}

void
TraCI_Edge::setAllowdSVCPermissions(std::string& id, SVCPermissions permissions) {
    MSEdge* e = getEdge(id);
    const std::vector<MSLane*>& lanes = e->getLanes();
    for (auto lane : lanes) {
        lane->setPermissions(permissions, MSLane::CHANGE_PERMISSIONS_PERMANENT);
    }
    e->rebuildAllowedLanes();
}

void
TraCI_Edge::addTravelTime(std::string& id, double begTime, double endTime, double value) {
    MSNet::getInstance()->getWeightsStorage().addTravelTime(getEdge(id), begTime, endTime, value);
}

void
TraCI_Edge::addEffort(std::string& id, double begTime, double endTime, double value) {
    MSNet::getInstance()->getWeightsStorage().addEffort(getEdge(id), begTime, endTime, value);
}

void
TraCI_Edge::setMaxSpeed(std::string& id, double value) {
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        lane->setMaxSpeed(value);
    }
}

void
TraCI_Edge::setParameter(std::string& id, std::string& name, std::string& value) {
    getEdge(id)->setParameter(name, value);
}

void
TraCI_Edge::getShape(const std::string& id, PositionVector& shape) {

    MSEdge* e = getEdge(id);
    const std::vector<MSLane*>& lanes = e->getLanes();
    shape = lanes.front()->getShape();
    if (lanes.size() > 1) {
        copy(lanes.back()->getShape().begin(), lanes.back()->getShape().end(), back_inserter(shape));
    }
}



