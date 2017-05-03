/****************************************************************************/
/// @file    TraCI_Lane.h
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Robert Hilbrich
/// @author  Leonhard Luecken
/// @date    30.05.2012
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSVehicle.h>
#include "TraCI_Lane.h"


// ===========================================================================
// member definitions
// ===========================================================================
std::vector<std::string>
TraCI_Lane::getIDList() {
    std::vector<std::string> ids;
    MSLane::insertIDs(ids);
    return ids;
}


int
TraCI_Lane::getIDCount() {
    return (int)getIDList().size();
}


std::string
TraCI_Lane::getEdgeID(std::string laneID) {
    return getLane(laneID)->getEdge().getID();
}


double
TraCI_Lane::getLength(std::string laneID) {
    return getLane(laneID)->getLength();
}


double
TraCI_Lane::getMaxSpeed(std::string laneID) {
    return getLane(laneID)->getSpeedLimit();
}


int
TraCI_Lane::getLinkNumber(std::string laneID) {
    return (int) getLane(laneID)->getLinkCont().size();
}


std::vector<TraCIConnection>
TraCI_Lane::getLinks(std::string laneID) {
    std::vector<TraCIConnection> v;
    const MSLane* lane = getLane(laneID);
    const SUMOTime currTime = MSNet::getInstance()->getCurrentTimeStep();
    const MSLinkCont& links = lane->getLinkCont();
    for (MSLinkCont::const_iterator i = links.begin(); i != links.end(); ++i) {
        MSLink* link = (*i);
        const std::string approachedLane = link->getLane() != 0 ? link->getLane()->getID() : "";
        const bool hasPrio = link->havePriority() ? 1 : 0;
        const double speed = MIN2(lane->getSpeedLimit(), link->getLane()->getSpeedLimit());
        const bool isOpen = link->opened(currTime, speed, speed, SUMOVTypeParameter::getDefault().length,
                                         SUMOVTypeParameter::getDefault().impatience, SUMOVTypeParameter::getDefaultDecel(), 0) ? 1 : 0;
        const bool hasFoe = link->hasApproachingFoe(currTime, currTime, 0, SUMOVTypeParameter::getDefaultDecel()) ? 1 : 0;
        const std::string approachedInternal = link->getViaLane() != 0 ? link->getViaLane()->getID() : "";
        const std::string state = SUMOXMLDefinitions::LinkStates.getString(link->getState());
        const std::string direction = SUMOXMLDefinitions::LinkDirections.getString(link->getDirection());
        const double length = link->getLength();
        v.push_back(TraCIConnection(approachedLane, hasPrio, isOpen, hasFoe, approachedInternal, state, direction, length));
    }
    return v;
}


std::vector<std::string>
TraCI_Lane::getAllowed(std::string laneID) {
    SVCPermissions permissions = getLane(laneID)->getPermissions();
    if (permissions == SVCAll) {  // special case: write nothing
        permissions = 0;
    }
    return getVehicleClassNamesList(permissions);
}


std::vector<std::string>
TraCI_Lane::getDisallowed(std::string laneID) {
    return getVehicleClassNamesList(~(getLane(laneID)->getPermissions())); // negation yields disallowed
}


TraCIPositionVector
TraCI_Lane::getShape(std::string laneID) {
    TraCIPositionVector pv;
    const PositionVector& shp = getLane(laneID)->getShape();
    for (PositionVector::const_iterator pi = shp.begin(); pi != shp.end(); ++pi) {
        TraCIPosition p;
        p.x = pi->x();
        p.y = pi->y();
        p.z = pi->z();
        pv.push_back(p);
    }
    return pv;
}


double
TraCI_Lane::getWidth(std::string laneID) {
    return getLane(laneID)->getWidth();
}


double
TraCI_Lane::getCO2Emission(std::string laneID) {
    return getLane(laneID)->getCO2Emissions();
}


double
TraCI_Lane::getCOEmission(std::string laneID) {
    return getLane(laneID)->getCOEmissions();
}


double
TraCI_Lane::getHCEmission(std::string laneID) {
    return getLane(laneID)->getHCEmissions();
}


double
TraCI_Lane::getPMxEmission(std::string laneID) {
    return getLane(laneID)->getPMxEmissions();
}


double
TraCI_Lane::getNOxEmission(std::string laneID) {
    return getLane(laneID)->getNOxEmissions();
}

double
TraCI_Lane::getFuelConsumption(std::string laneID) {
    return getLane(laneID)->getFuelConsumption();
}


double
TraCI_Lane::getNoiseEmission(std::string laneID) {
    return getLane(laneID)->getHarmonoise_NoiseEmissions();
}


double
TraCI_Lane::getElectricityConsumption(std::string laneID) {
    return getLane(laneID)->getElectricityConsumption();
}


double
TraCI_Lane::getLastStepMeanSpeed(std::string laneID) {
    return getLane(laneID)->getMeanSpeed();
}


double
TraCI_Lane::getLastStepOccupancy(std::string laneID) {
    return getLane(laneID)->getNettoOccupancy();
}


double
TraCI_Lane::getLastStepLength(std::string laneID) {
    const MSLane* lane = getLane(laneID);
    double length = 0;
    const MSLane::VehCont& vehs = lane->getVehiclesSecure();
    for (MSLane::VehCont::const_iterator j = vehs.begin(); j != vehs.end(); ++j) {
        length += (*j)->getVehicleType().getLength();
    }
    if (vehs.size() > 0) {
        length = length / (double) vehs.size();
    }
    lane->releaseVehicles();
    return length;
}


double
TraCI_Lane::getWaitingTime(std::string laneID) {
    return getLane(laneID)->getWaitingSeconds();
}


double
TraCI_Lane::getTraveltime(std::string laneID) {
    const MSLane* lane = getLane(laneID);
    double meanSpeed = lane->getMeanSpeed();
    if (meanSpeed != 0) {
        return lane->getLength() / meanSpeed;
    } else {
        return 1000000.;
    }
}


int
TraCI_Lane::getLastStepVehicleNumber(std::string laneID) {
    return (int) getLane(laneID)->getVehicleNumber();
}

int
TraCI_Lane::getLastStepHaltingNumber(std::string laneID) {
    const MSLane* lane = getLane(laneID);
    int halting = 0;
    const MSLane::VehCont& vehs = lane->getVehiclesSecure();
    for (MSLane::VehCont::const_iterator j = vehs.begin(); j != vehs.end(); ++j) {
        if ((*j)->getSpeed() < SUMO_const_haltingSpeed) {
            ++halting;
        }
    }
    lane->releaseVehicles();
    return halting;
}


std::vector<std::string>
TraCI_Lane::getLastStepVehicleIDs(std::string laneID) {
    const MSLane* lane = getLane(laneID);
    std::vector<std::string> vehIDs;
    const MSLane::VehCont& vehs = lane->getVehiclesSecure();
    for (MSLane::VehCont::const_iterator j = vehs.begin(); j != vehs.end(); ++j) {
        vehIDs.push_back((*j)->getID());
    }
    lane->releaseVehicles();
    return vehIDs;
}




void
TraCI_Lane::setAllowed(std::string laneID, std::vector<std::string> allowedClasses) {
    MSLane* l = const_cast<MSLane*>(getLane(laneID));
    l->setPermissions(parseVehicleClasses(allowedClasses), MSLane::CHANGE_PERMISSIONS_PERMANENT);
    l->getEdge().rebuildAllowedLanes();
}


void
TraCI_Lane::setDisallowed(std::string laneID, std::vector<std::string> disallowedClasses) {
    MSLane* l = const_cast<MSLane*>(getLane(laneID));
    l->setPermissions(~parseVehicleClasses(disallowedClasses), MSLane::CHANGE_PERMISSIONS_PERMANENT); // negation yields allowed
    l->getEdge().rebuildAllowedLanes();
}


void
TraCI_Lane::setMaxSpeed(std::string laneID, double speed) {
    MSLane* l = const_cast<MSLane*>(getLane(laneID));
    l->setMaxSpeed(speed);
}


void
TraCI_Lane::setLength(std::string laneID, double length) {
    MSLane* l = const_cast<MSLane*>(getLane(laneID));
    l->setLength(length);
}


std::string
TraCI_Lane::getParameter(const std::string& laneID, const std::string& param) {
    return getLane(laneID)->getParameter(param, "");
}


void
TraCI_Lane::setParameter(const std::string& laneID, const std::string& key, const std::string& value) {
    MSLane* l = const_cast<MSLane*>(getLane(laneID));
    l->addParameter(key, value);
}


const MSLane*
TraCI_Lane::getLane(const std::string& id) {
    const MSLane* r = MSLane::dictionary(id);
    if (r == 0) {
        throw TraCIException("Lane '" + id + "' is not known");
    }
    return r;
}


/****************************************************************************/
