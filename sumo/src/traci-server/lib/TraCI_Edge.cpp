/****************************************************************************/
/// @file    TraCI_Edge.cpp
/// @author  Gregor Laemmel
/// @date    15.09.2017
/// @version $Id: TraCI_Edge.cpp 25296 2017-07-22 18:29:42Z behrisch $
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


#include <microsim/MSRoute.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdgeWeightsStorage.h>
#include <microsim/MSTransportable.h>
#include <traci-server/TraCIDefs.h>
#include "TraCI_Edge.h"


std::vector<std::string>
TraCI_Edge::getIDList() {
    std::vector<std::string> ids;
    MSRoute::insertIDs(ids);
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
TraCI_Edge::getEdge(std::string& id) {
    MSEdge* e = MSEdge::dictionary(id);
    if (e == nullptr) {
        throw TraCIException("Unknown edge '" + id + "'.");
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
const std::vector<std::string> TraCI_Edge::getPersonIDs(std::string& id) {
    std::vector<std::string> personIDs;
    std::vector<MSTransportable*> persons = getEdge(id)->getSortedPersons(MSNet::getInstance()->getCurrentTimeStep(),
    for (std::vector<MSTransportable*>::iterator it = persons.begin(); it != persons.end(); ++it) {
        personIDs.push_back((*it)->getID());
    }

    return personIDs;
}

