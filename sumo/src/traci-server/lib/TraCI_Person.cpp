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
#include <utils/common/StringTokenizer.h>
#include "TraCI_VehicleType.h"
#include "TraCI_Person.h"


// ===========================================================================
// member definitions
// ===========================================================================

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




void
TraCI_Person::setSpeed(const std::string& personID, double speed) {
    getPerson(personID)->setSpeed(speed);
}


void
TraCI_Person::setType(const std::string& personID, const std::string& typeID) {
    MSVehicleType* vehicleType = MSNet::getInstance()->getVehicleControl().getVType(typeID);
    if (vehicleType == 0) {
        throw TraCIException("The vehicle type '" + typeID + "' is not known.");
    }
    getPerson(personID)->replaceVehicleType(vehicleType);
}


void
TraCI_Person::add(const std::string& personID, const std::string& edgeID, double pos, double departInSecs, const std::string typeID) {
    MSTransportable* p;
    try {
        p = getPerson(personID);
    } catch (TraCIException&) {
        p = nullptr;
    }

    if (p != nullptr) {
        throw TraCIException("The person " + personID + " to add already exists.");
    }

    SUMOTime depart = TIME2STEPS(departInSecs);
    SUMOVehicleParameter vehicleParams;
    vehicleParams.id = personID;

    MSVehicleType* vehicleType = MSNet::getInstance()->getVehicleControl().getVType(typeID);
    if (!vehicleType) {
        throw TraCIException("Invalid type '" + typeID + "' for person '" + personID + "'");
    }

    const MSEdge* edge = MSEdge::dictionary(edgeID);
    if (!edge) {
        throw TraCIException("Invalid edge '" + edgeID + "' for person: '" + personID + "'");
    }

    if (depart < 0) {
        const int proc = (int)-depart;
        if (proc >= static_cast<int>(DEPART_DEF_MAX)) {
            throw TraCIException("Invalid departure time.");
        }
        vehicleParams.departProcedure = (DepartDefinition)proc;
        vehicleParams.depart = MSNet::getInstance()->getCurrentTimeStep();
    } else if (depart < MSNet::getInstance()->getCurrentTimeStep()) {
        vehicleParams.depart = MSNet::getInstance()->getCurrentTimeStep();
        WRITE_WARNING("Departure time " + toString(departInSecs) + " for person '" + personID 
                + "' is in the past; using current time " + time2string(vehicleParams.depart) + " instead.");
    } else {
        vehicleParams.depart = depart;
    }

    vehicleParams.departPosProcedure = DEPART_POS_GIVEN;
    if (fabs(pos) > edge->getLength()) {
        throw TraCIException("Invalid departure position.");
    }
    if (pos < 0) {
        pos += edge->getLength();
    }
    vehicleParams.departPos = pos;

    SUMOVehicleParameter* params = new SUMOVehicleParameter(vehicleParams);
    MSTransportable::MSTransportablePlan* plan = new MSTransportable::MSTransportablePlan();
    plan->push_back(new MSTransportable::Stage_Waiting(*edge, 0, depart, pos, "awaiting departure", true));

    try {
        MSTransportable* person = MSNet::getInstance()->getPersonControl().buildPerson(params, vehicleType, plan);
        MSNet::getInstance()->getPersonControl().add(person);
    } catch (ProcessError& e) {
        delete params;
        delete plan;
        throw TraCIException(e.what());
    }
}


void
TraCI_Person::appendDrivingStage(const std::string& personID, const std::string& toEdge, const std::string& lines, const std::string& stopID) {
    MSTransportable* p = getPerson(personID);
    const MSEdge* edge = MSEdge::dictionary(toEdge);
    if (!edge) {
        throw TraCIException("Invalid edge '" + toEdge + "' for person: '" + personID + "'");
    }
    if (lines.size() == 0) {
        return throw TraCIException("Empty lines parameter for person: '" + personID + "'");
    }
    MSStoppingPlace* bs = 0;
    if (stopID != "") {
        bs = MSNet::getInstance()->getBusStop(stopID);
        if (bs == 0) {
            throw TraCIException("Invalid stopping place id '" + stopID + "' for person: '" + personID + "'");
        }
    }
    p->appendStage(new MSPerson::MSPersonStage_Driving(*edge, bs, -NUMERICAL_EPS, StringTokenizer(lines).getVector()));
}


void
TraCI_Person::appendWaitingStage(const std::string& personID, double duration, const std::string& description, const std::string& stopID) {
    MSTransportable* p = getPerson(personID);
    if (duration < 0) {
        throw TraCIException("Duration for person: '" + personID + "' must not be negative");
    }
    MSStoppingPlace* bs = 0;
    if (stopID != "") {
        bs = MSNet::getInstance()->getBusStop(stopID);
        if (bs == 0) {
            throw TraCIException("Invalid stopping place id '" + stopID + "' for person: '" + personID + "'");
        }
    }
    p->appendStage(new MSTransportable::Stage_Waiting(*p->getArrivalEdge(), TIME2STEPS(duration), 0, p->getArrivalPos(), description, false));
}


void
TraCI_Person::appendWalkingStage(const std::string& personID, const std::vector<std::string>& edgeIDs, double arrivalPos, double duration, double speed, const std::string& stopID) {
    MSTransportable* p = getPerson(personID);
    ConstMSEdgeVector edges;
    try {
        MSEdge::parseEdgesList(edgeIDs, edges, "<unknown>");
    } catch (ProcessError& e) {
        throw TraCIException(e.what());
    }
    if (edges.empty()) {
        throw TraCIException("Empty edge list for walking stage of person '" + personID+ "'.");
    }
    if (fabs(arrivalPos) > edges.back()->getLength()) {
        throw TraCIException("Invalid arrivalPos for walking stage of person '" + personID  + "'.");
    }
    if (arrivalPos < 0) {
        arrivalPos += edges.back()->getLength();
    }
    if (speed < 0) {
        speed = p->getVehicleType().getMaxSpeed();
    }
    MSStoppingPlace* bs = 0;
    if (stopID != "") {
        bs = MSNet::getInstance()->getBusStop(stopID);
        if (bs == 0) {
            throw TraCIException("Invalid stopping place id '" + stopID + "' for person: '" + personID + "'");
        }
    }
    p->appendStage(new MSPerson::MSPersonStage_Walking(edges, bs, TIME2STEPS(duration), speed, p->getArrivalPos(), arrivalPos, 0));
}


void
TraCI_Person::removeStage(const std::string& personID, int nextStageIndex) {
    MSTransportable* p = getPerson(personID);
    if (nextStageIndex >= p->getNumRemainingStages()) {
        throw TraCIException("The stage index must be lower than the number of remaining stages.");
    }
    if (nextStageIndex < 0) {
        throw TraCIException("The stage index may not be negative.");
    }
    p->removeStage(nextStageIndex);
}


void
TraCI_Person::rerouteTraveltime(const std::string& personID) {
    MSTransportable* p = getPerson(personID);
    if (p->getNumRemainingStages() == 0 || p->getCurrentStageType() != MSTransportable::MOVING_WITHOUT_VEHICLE) {
        throw TraCIException("Person '" + personID + "' is not currenlty walking.");
    }
    const MSEdge* from = p->getEdge();
    double  departPos = p->getEdgePos();
    const MSEdge* to = p->getArrivalEdge();
    double  arrivalPos = p->getArrivalPos();
    double speed = p->getVehicleType().getMaxSpeed();
    ConstMSEdgeVector newEdges;
    MSNet::getInstance()->getPedestrianRouter().compute(from, to, departPos, arrivalPos, speed, 0, 0, newEdges);
    if (newEdges.empty()) {
        throw TraCIException("Could not find new route for person '" + personID + "'.");
    }
    ConstMSEdgeVector oldEdges = p->getEdges(0);
    assert(!oldEdges.empty());
    if (oldEdges.front()->getFunction() != EDGEFUNC_NORMAL) {
        oldEdges.erase(oldEdges.begin());
    }
    if (newEdges == oldEdges) {
        return;
    }
    if (newEdges.front() != from) {
        // @note: maybe this should be done automatically by the router
        newEdges.insert(newEdges.begin(), from);
    }
    //std::cout << " from=" << from->getID() << " to=" << to->getID() << " newEdges=" << toString(newEdges) << "\n";
    MSPerson::MSPersonStage_Walking* newStage = new MSPerson::MSPersonStage_Walking(newEdges, 0, -1, speed, departPos, arrivalPos, 0);
    if (p->getNumRemainingStages() == 1) {
        // Do not remove the last stage (a waiting stage would be added otherwise)
        p->appendStage(newStage);
        //std::cout << "case a: remaining=" << p->getNumRemainingStages() << "\n";
        p->removeStage(0);
    } else {
        p->removeStage(0);
        p->appendStage(newStage);
        //std::cout << "case b: remaining=" << p->getNumRemainingStages() << "\n";
    }
}


/** untested setter functions which alter the person's vtype ***/

void
TraCI_Person::setParameter(const std::string& personID, const std::string& key, const std::string& value) {
    MSTransportable* p = getPerson(personID);
    ((SUMOVehicleParameter&) p->getParameter()).addParameter(key, value);
}

void
TraCI_Person::setLength(const std::string& personID, double length) {
    TraCI_VehicleType::getVType(getSingularVType(personID))->setLength(length);
}

void
TraCI_Person::setWidth(const std::string& personID, double width) {
    TraCI_VehicleType::getVType(getSingularVType(personID))->setWidth(width);
}

void
TraCI_Person::setHeight(const std::string& personID, double height) {
    TraCI_VehicleType::getVType(getSingularVType(personID))->setHeight(height);
}

void
TraCI_Person::setMinGap(const std::string& personID, double minGap) {
    TraCI_VehicleType::getVType(getSingularVType(personID))->setMinGap(minGap);
}

void
TraCI_Person::setColor(const std::string& personID, const TraCIColor& c) {
    TraCI_VehicleType::getVType(getSingularVType(personID))->setColor(RGBColor(c.r, c.g, c.b, c.a));
}



/******** private functions *************/

MSTransportable*
TraCI_Person::getPerson(const std::string& personID) {
    MSTransportableControl& c = MSNet::getInstance()->getPersonControl();
    MSTransportable* p = c.get(personID);
    if (p == 0) {
        throw TraCIException("Person '" + personID + "' is not known");
    }
    return p;
}

std::string
TraCI_Person::getSingularVType(const std::string& personID) {
    return getPerson(personID)->getSingularType().getID();
}


/****************************************************************************/
