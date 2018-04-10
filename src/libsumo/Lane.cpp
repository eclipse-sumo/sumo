/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Lane.cpp
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
#include "Lane.h"


// ===========================================================================
// member definitions
// ===========================================================================
namespace libsumo {
std::vector<std::string>
Lane::getIDList() {
    std::vector<std::string> ids;
    MSLane::insertIDs(ids);
    return ids;
}


int
Lane::getIDCount() {
    return (int)getIDList().size();
}


std::string
Lane::getEdgeID(std::string laneID) {
    return getLane(laneID)->getEdge().getID();
}


double
Lane::getLength(std::string laneID) {
    return getLane(laneID)->getLength();
}


double
Lane::getMaxSpeed(std::string laneID) {
    return getLane(laneID)->getSpeedLimit();
}


int
Lane::getLinkNumber(std::string laneID) {
    return (int)getLane(laneID)->getLinkCont().size();
}


std::vector<TraCIConnection>
Lane::getLinks(std::string laneID) {
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
Lane::getAllowed(std::string laneID) {
    SVCPermissions permissions = getLane(laneID)->getPermissions();
    if (permissions == SVCAll) {  // special case: write nothing
        permissions = 0;
    }
    return getVehicleClassNamesList(permissions);
}


std::vector<std::string>
Lane::getDisallowed(std::string laneID) {
    return getVehicleClassNamesList(invertPermissions((getLane(laneID)->getPermissions()))); // negation yields disallowed
}


TraCIPositionVector
Lane::getShape(std::string laneID) {
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
Lane::getWidth(std::string laneID) {
    return getLane(laneID)->getWidth();
}


double
Lane::getCO2Emission(std::string laneID) {
    return getLane(laneID)->getCO2Emissions();
}


double
Lane::getCOEmission(std::string laneID) {
    return getLane(laneID)->getCOEmissions();
}


double
Lane::getHCEmission(std::string laneID) {
    return getLane(laneID)->getHCEmissions();
}


double
Lane::getPMxEmission(std::string laneID) {
    return getLane(laneID)->getPMxEmissions();
}


double
Lane::getNOxEmission(std::string laneID) {
    return getLane(laneID)->getNOxEmissions();
}

double
Lane::getFuelConsumption(std::string laneID) {
    return getLane(laneID)->getFuelConsumption();
}


double
Lane::getNoiseEmission(std::string laneID) {
    return getLane(laneID)->getHarmonoise_NoiseEmissions();
}


double
Lane::getElectricityConsumption(std::string laneID) {
    return getLane(laneID)->getElectricityConsumption();
}


double
Lane::getLastStepMeanSpeed(std::string laneID) {
    return getLane(laneID)->getMeanSpeed();
}


double
Lane::getLastStepOccupancy(std::string laneID) {
    return getLane(laneID)->getNettoOccupancy();
}


double
Lane::getLastStepLength(std::string laneID) {
    const MSLane* lane = getLane(laneID);
    double length = 0;
    const MSLane::VehCont& vehs = lane->getVehiclesSecure();
    for (MSLane::VehCont::const_iterator j = vehs.begin(); j != vehs.end(); ++j) {
        length += (*j)->getVehicleType().getLength();
    }
    if (vehs.size() > 0) {
        length = length / (double)vehs.size();
    }
    lane->releaseVehicles();
    return length;
}


double
Lane::getWaitingTime(std::string laneID) {
    return getLane(laneID)->getWaitingSeconds();
}


double
Lane::getTraveltime(std::string laneID) {
    const MSLane* lane = getLane(laneID);
    double meanSpeed = lane->getMeanSpeed();
    if (meanSpeed != 0) {
        return lane->getLength() / meanSpeed;
    } else {
        return 1000000.;
    }
}


int
Lane::getLastStepVehicleNumber(std::string laneID) {
    return (int)getLane(laneID)->getVehicleNumber();
}

int
Lane::getLastStepHaltingNumber(std::string laneID) {
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
Lane::getLastStepVehicleIDs(std::string laneID) {
    const MSLane* lane = getLane(laneID);
    std::vector<std::string> vehIDs;
    const MSLane::VehCont& vehs = lane->getVehiclesSecure();
    for (MSLane::VehCont::const_iterator j = vehs.begin(); j != vehs.end(); ++j) {
        vehIDs.push_back((*j)->getID());
    }
    lane->releaseVehicles();
    return vehIDs;
}

std::vector<std::string>
Lane::getFoes(const std::string& laneID, const std::string& toLaneID) {
    std::vector<std::string> foeIDs;
    const MSLane* from = getLane(laneID);
    const MSLane* to = getLane(toLaneID);
    const MSLink* link = MSLinkContHelper::getConnectingLink(*from, *to);
    if (link == 0) {
        throw TraCIException("No connection from lane '" + laneID + "' to lane '" + toLaneID + "'");
    }
    for (MSLink* foe : link->getFoeLinks()) {
        foeIDs.push_back(foe->getLaneBefore()->getID());
    }
    return foeIDs;
}


std::vector<std::string>
Lane::getInternalFoes(const std::string& laneID) {
    const MSLane* lane = getLane(laneID);
    const std::vector<const MSLane*>* foeLanes;
    std::vector<const MSLane*>::const_iterator it;
    std::vector<std::string> foeIDs;

    if (lane->isInternal()) {
        MSLink* link = lane->getLinkCont().front();
        foeLanes = &link->getFoeLanes();

        for (it = foeLanes->begin(); foeLanes->end() != it; ++it) {
            foeIDs.push_back((*it)->getID());
        }
    }
    return foeIDs;
}


void
Lane::setAllowed(std::string laneID, std::vector<std::string> allowedClasses) {
    MSLane* l = const_cast<MSLane*>(getLane(laneID));
    l->setPermissions(parseVehicleClasses(allowedClasses), MSLane::CHANGE_PERMISSIONS_PERMANENT);
    l->getEdge().rebuildAllowedLanes();
}


void
Lane::setDisallowed(std::string laneID, std::vector<std::string> disallowedClasses) {
    MSLane* l = const_cast<MSLane*>(getLane(laneID));
    l->setPermissions(invertPermissions(parseVehicleClasses(disallowedClasses)), MSLane::CHANGE_PERMISSIONS_PERMANENT); // negation yields allowed
    l->getEdge().rebuildAllowedLanes();
}


void
Lane::setMaxSpeed(std::string laneID, double speed) {
    MSLane* l = const_cast<MSLane*>(getLane(laneID));
    l->setMaxSpeed(speed);
}


void
Lane::setLength(std::string laneID, double length) {
    MSLane* l = const_cast<MSLane*>(getLane(laneID));
    l->setLength(length);
}


std::string
Lane::getParameter(const std::string& laneID, const std::string& param) {
    return getLane(laneID)->getParameter(param, "");
}


void
Lane::setParameter(const std::string& laneID, const std::string& key, const std::string& value) {
    MSLane* l = const_cast<MSLane*>(getLane(laneID));
    l->setParameter(key, value);
}


const MSLane*
Lane::getLane(const std::string& id) {
    const MSLane* r = MSLane::dictionary(id);
    if (r == 0) {
        throw TraCIException("Lane '" + id + "' is not known");
    }
    return r;
}
}


/****************************************************************************/
