/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Edge.cpp
/// @author  Gregor Laemmel
/// @date    15.09.2017
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/


#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdgeWeightsStorage.h>
#include <microsim/MSTransportable.h>
#include <microsim/MSVehicle.h>
#include <libsumo/TraCIDefs.h>
#include <utils/emissions/HelpersHarmonoise.h>
#include "Edge.h"


namespace libsumo {
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

double Edge::retrieveExistingTravelTime(const std::string& id, double time) {
    const MSEdge* e = getEdge(id);
    double value;
    if (!MSNet::getInstance()->getWeightsStorage().retrieveExistingTravelTime(e, time, value)) {
        return -1.;
    }
    return value;
}

double
Edge::retrieveExistingEffort(const std::string& id, double time) {
    const MSEdge* e = getEdge(id);
    double value;
    if (!MSNet::getInstance()->getWeightsStorage().retrieveExistingEffort(e, time, value)) {
        return -1.;
    }
    return value;
}

double
Edge::getCurrentTravelTime(const std::string& id) {
    return getEdge(id)->getCurrentTravelTime();
}

MSEdge*
Edge::getEdge(const std::string& id) {
    MSEdge* e = MSEdge::dictionary(id);
    if (e == nullptr) {
        throw TraCIException("Edge '" + id + "' is not known");
    }
    return e;
}

double
Edge::getWaitingSeconds(const std::string& id) {
    double wtime = 0;
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        wtime += lane->getWaitingSeconds();
    }
    return wtime;
}

const std::vector<std::string>
Edge::getPersonIDs(const std::string& id) {
    std::vector<std::string> personIDs;
    std::vector<MSTransportable*> persons = getEdge(id)->getSortedPersons(MSNet::getInstance()->getCurrentTimeStep(), true);
    personIDs.reserve(persons.size());
    for (MSTransportable* p : persons) {
        personIDs.push_back(p->getID());
    }
    return personIDs;
}

const std::vector<std::string>
Edge::getVehicleIDs(const std::string& id) {
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
Edge::getCO2Emissions(const std::string& id) {
    double sum = 0;
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        sum += lane->getCO2Emissions();
    }
    return sum;
}

double
Edge::getCOEmissions(const std::string& id) {
    double sum = 0;
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        sum += lane->getCOEmissions();
    }
    return sum;
}

double
Edge::getHCEmissions(const std::string& id) {
    double sum = 0;
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        sum += lane->getHCEmissions();
    }
    return sum;
}

double
Edge::getPMxEmissions(const std::string& id) {
    double sum = 0;
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        sum += lane->getPMxEmissions();
    }
    return sum;
}

double
Edge::getNOxEmissions(const std::string& id) {
    double sum = 0;
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        sum += lane->getNOxEmissions();
    }
    return sum;
}

double
Edge::getFuelConsumption(const std::string& id) {
    double sum = 0;
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        sum += lane->getFuelConsumption();
    }
    return sum;
}

double
Edge::getNoiseEmissions(const std::string& id) {
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
Edge::getElectricityConsumption(const std::string& id) {
    double sum = 0;
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        sum += lane->getElectricityConsumption();
    }
    return sum;
}

int
Edge::getVehicleNumber(const std::string& id) {
    int sum = 0;
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        sum += lane->getVehicleNumber();
    }
    return sum;
}

double
Edge::getMeanSpeed(const std::string& id) {
    return getEdge(id)->getMeanSpeed();
}

double
Edge::getOccupancy(const std::string& id) {
    double sum = 0;
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        sum += lane->getNettoOccupancy();
    }
    return sum / (double)lanes.size();
}

int
Edge::getVehicleHaltingNumber(const std::string& id) {
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
Edge::getVehicleAverageLength(const std::string& id) {
    double lengthSum = 0;
    int noVehicles = 0;
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        const MSLane::VehCont& vehs = lane->getVehiclesSecure();
        for (auto veh : vehs) {
            lengthSum += veh->getVehicleType().getLength();
        }
        noVehicles += (int)vehs.size();
        lane->releaseVehicles();
    }
    if (noVehicles == 0) {
        return 0;
    }
    return lengthSum / (double)noVehicles;
}


int 
Edge::getLaneNumber(const std::string& id) {
    return (int)getEdge(id)->getLanes().size();
}


std::string
Edge::getParameter(const std::string& id, const std::string& paramName) {
    return getEdge(id)->getParameter(paramName, "");
}

void
Edge::setAllowedVehicleClasses(const std::string& id, std::vector<std::string> classes) {
    SVCPermissions permissions = parseVehicleClasses(classes);
    setAllowedSVCPermissions(id, permissions);
}

void
Edge::setDisallowedVehicleClasses(const std::string& id, std::vector<std::string> classes) {
    SVCPermissions permissions = invertPermissions(parseVehicleClasses(classes));
    setAllowedSVCPermissions(id, permissions);
}

void
Edge::setAllowedSVCPermissions(const std::string& id, SVCPermissions permissions) {
    MSEdge* e = getEdge(id);
    const std::vector<MSLane*>& lanes = e->getLanes();
    for (auto lane : lanes) {
        lane->setPermissions(permissions, MSLane::CHANGE_PERMISSIONS_PERMANENT);
    }
    e->rebuildAllowedLanes();
}

void
Edge::addTravelTime(const std::string& id, double begTime, double endTime, double value) {
    MSNet::getInstance()->getWeightsStorage().addTravelTime(getEdge(id), begTime, endTime, value);
}

void
Edge::addEffort(const std::string& id, double begTime, double endTime, double value) {
    MSNet::getInstance()->getWeightsStorage().addEffort(getEdge(id), begTime, endTime, value);
}

void
Edge::setMaxSpeed(const std::string& id, double value) {
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    for (auto lane : lanes) {
        lane->setMaxSpeed(value);
    }
}

void
Edge::setParameter(const std::string& id, const std::string& name, const std::string& value) {
    getEdge(id)->setParameter(name, value);
}

void
Edge::getShape(const std::string& id, PositionVector& shape) {
    const std::vector<MSLane*>& lanes = getEdge(id)->getLanes();
    shape = lanes.front()->getShape();
    if (lanes.size() > 1) {
        copy(lanes.back()->getShape().begin(), lanes.back()->getShape().end(), back_inserter(shape));
    }
}
}


/****************************************************************************/



