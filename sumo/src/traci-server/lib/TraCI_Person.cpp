/****************************************************************************/
/// @file    TraCI_Person.cpp
/// @author  Leonhard Luecken
/// @date    15.09.2017
/// @version $Id: TraCI_Person.cpp 25277 2017-07-20 11:15:41Z behrisch $
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

#include <microsim/MSTransportableControl.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSNet.h>
#include <microsim/pedestrians/MSPerson.h>
#include <utils/geom/GeomHelper.h>
#include "TraCI_Person.h"


// ===========================================================================
// member definitions
// ===========================================================================

MSTransportable*
TraCI_Person::getPerson(const std::string& personID) {
    MSTransportableControl& c = MSNet::getInstance()->getPersonControl();
    MSTransportable* p = c.get(personID);
    if (p == 0) {
        throw TraCIException("Person '" + personID + "' is not known");
    }
    return p;
}


std::vector<std::string>
TraCI_Person::getIDList() {
    MSTransportableControl& c = MSNet::getInstance()->getPersonControl();
    std::vector<std::string> ids;
    for (MSTransportableControl::constVehIt i = c.loadedBegin(); i != c.loadedEnd(); ++i) {
        if (i->second->getCurrentStageType() != MSTransportable::WAITING_FOR_DEPART) {
            ids.push_back(i->first);
        }
    }
    return std::move(ids);
};


int
TraCI_Person::getIDCount() {
    return MSNet::getInstance()->getPersonControl().size();
}


TraCIPosition
TraCI_Person::getPosition(const std::string& personID) {
    MSTransportable* p = getPerson(personID);
    TraCIPosition pos;
    pos.x = p->getPosition().x();
    pos.y = p->getPosition().y();
    pos.z = p->getPosition().z();
    return pos;
}


double
TraCI_Person::getAngle(const std::string& personID){
    return GeomHelper::naviDegree(getPerson(personID)->getAngle());
}


double
TraCI_Person::getSpeed(const std::string& personID){
    return getPerson(personID)->getSpeed();
}


std::string
TraCI_Person::getRoadID(const std::string& personID){
    return getPerson(personID)->getEdge()->getID();
}


double
TraCI_Person::getLanePosition(const std::string& personID) {
    return getPerson(personID)->getEdgePos();
}


TraCIColor
TraCI_Person::getColor(const std::string& personID){
    const RGBColor& col = getPerson(personID)->getParameter().color;
    TraCIColor tcol;
    tcol.r = col.red();
    tcol.g = col.green();
    tcol.b = col.blue();
    tcol.a = col.alpha();
    return tcol;
}


std::string
TraCI_Person::getTypeID(const std::string& personID) {
    return getPerson(personID)->getVehicleType().getID();
}


double
TraCI_Person::getWaitingTime(const std::string& personID) {
    return getPerson(personID)->getWaitingSeconds();
}


std::string
TraCI_Person::getNextEdge(const std::string& personID) {
    return dynamic_cast<MSPerson*>(getPerson(personID))->getNextEdge();
}


std::vector<std::string>
TraCI_Person::getEdges(const std::string& personID, int nextStageIndex) {
    MSTransportable* p = getPerson(personID);
    if (nextStageIndex >= p->getNumRemainingStages()) {
        throw TraCIException("The stage index must be lower than the number of remaining stages.");
    }
    if (nextStageIndex < (p->getNumRemainingStages() - p->getNumStages())) {
        throw TraCIException("The negative stage index must refer to a valid previous stage.");
    }

    std::vector<std::string> edgeIDs;
    for(auto& e : p->getEdges(nextStageIndex)) {
        edgeIDs.push_back(e->getID());
    }
    return edgeIDs;
}


int
TraCI_Person::getStage(const std::string& personID, int nextStageIndex) {
    MSTransportable* p = getPerson(personID);
    if (nextStageIndex >= p->getNumRemainingStages()) {
        throw TraCIException("The stage index must be lower than the number of remaining stages.");
    }
    if (nextStageIndex < (p->getNumRemainingStages() - p->getNumStages())) {
        throw TraCIException("The negative stage index must refer to a valid previous stage.");
    }
    return p->getStageType(nextStageIndex);
}


int
TraCI_Person::getRemainingStages(const std::string& personID) {
    return getPerson(personID)->getNumRemainingStages();
}


std::string
TraCI_Person::getVehicle(const std::string& personID) {
    const SUMOVehicle* veh = getPerson(personID)->getVehicle();
    if (veh == nullptr) {
        return "";
    } else {
        return veh->getID();
    }
}


std::string
TraCI_Person::getParameter(const std::string& personID, const std::string& param) {
    return getPerson(personID)->getParameter().getParameter(param, "");
}




//void
//TraCI_Person::removeStages(const std::string& personID) const;
//void
//TraCI_Person::add(const std::string& personID, const std::string& edgeID, double pos, double depart = DEPARTFLAG_NOW, const std::string typeID = "DEFAULT_PEDTYPE");
//void
//TraCI_Person::appendWaitingStage(const std::string& personID, double duration, const std::string& description = "waiting", const std::string& stopID = "");
//void
//TraCI_Person::appendWalkingStage(const std::string& personID, const std::vector<std::string>& edges, double arrivalPos, double duration = -1, double speed = -1, const std::string& stopID = "");
//void
//TraCI_Person::appendDrivingStage(const std::string& personID, const std::string& toEdge, const std::string& lines, const std::string& stopID = "");
//void
//TraCI_Person::removeStage(const std::string& personID, int nextStageIndex) const;
//void
//TraCI_Person::rerouteTraveltime(const std::string& personID) const;
//void
//TraCI_Person::setSpeed(const std::string& personID, double speed) const;
//void
//TraCI_Person::setType(const std::string& personID, const std::string& typeID) const;
//void
//TraCI_Person::setLength(const std::string& personID, double length) const;
//void
//TraCI_Person::setWidth(const std::string& personID, double width) const;
//void
//TraCI_Person::setHeight(const std::string& personID, double height) const;
//void
//TraCI_Person::setMinGap(const std::string& personID, double minGap) const;
//void
//TraCI_Person::setColor(const std::string& personID, const TraCIColor& c) const;


/****************************************************************************/
