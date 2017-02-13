/****************************************************************************/
/// @file    TraCIServerAPI_Person.cpp
/// @author  Daniel Krajzewicz
/// @date    26.05.2014
/// @version $Id$
///
// APIs for getting/setting person values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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

#ifndef NO_TRACI

#include <utils/common/StringTokenizer.h>
#include <microsim/MSTransportableControl.h>
#include <microsim/pedestrians/MSPerson.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include "TraCIConstants.h"
#include "TraCIServer.h"
#include "TraCIServerAPI_Person.h"
#include "TraCIServerAPI_VehicleType.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_Person::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                                  tcpip::Storage& outputStorage) {
    // variable
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable != ID_LIST && variable != ID_COUNT
            && variable != VAR_POSITION && variable != VAR_POSITION3D && variable != VAR_ANGLE && variable != VAR_SPEED
            && variable != VAR_ROAD_ID && variable != VAR_LANEPOSITION
            && variable != VAR_WIDTH && variable != VAR_LENGTH && variable != VAR_MINGAP
            && variable != VAR_TYPE && variable != VAR_SHAPECLASS && variable != VAR_COLOR
            && variable != VAR_WAITING_TIME && variable != VAR_PARAMETER
            && variable != VAR_NEXT_EDGE
            && variable != VAR_EDGES
            && variable != VAR_STAGE
            && variable != VAR_STAGES_REMAINING
            && variable != VAR_VEHICLE
       ) {
        return server.writeErrorStatusCmd(CMD_GET_PERSON_VARIABLE, "Get Person Variable: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_PERSON_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    MSTransportableControl& c = MSNet::getInstance()->getPersonControl();
    if (variable == ID_LIST || variable == ID_COUNT) {
        if (variable == ID_LIST) {
            std::vector<std::string> ids;
            for (MSTransportableControl::constVehIt i = c.loadedBegin(); i != c.loadedEnd(); ++i) {
                if (i->second->getCurrentStageType() != MSTransportable::WAITING_FOR_DEPART) {
                    ids.push_back(i->first);
                }
            }
            tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
            tempMsg.writeStringList(ids);
        } else {
            tempMsg.writeUnsignedByte(TYPE_INTEGER);
            tempMsg.writeInt((int) c.size());
        }
    } else {
        MSTransportable* p = c.get(id);
        if (p == 0) {
            return server.writeErrorStatusCmd(CMD_GET_PERSON_VARIABLE, "Person '" + id + "' is not known", outputStorage);
        }
        switch (variable) {
            case VAR_POSITION: {
                tempMsg.writeUnsignedByte(POSITION_2D);
                tempMsg.writeDouble(p->getPosition().x());
                tempMsg.writeDouble(p->getPosition().y());
            }
            break;
            case VAR_POSITION3D:
                tempMsg.writeUnsignedByte(POSITION_3D);
                tempMsg.writeDouble(p->getPosition().x());
                tempMsg.writeDouble(p->getPosition().y());
                tempMsg.writeDouble(p->getPosition().z());
                break;
            case VAR_ANGLE:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(GeomHelper::naviDegree(p->getAngle()));
                break;
            case VAR_SPEED:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(p->getSpeed());
                break;
            case VAR_ROAD_ID:
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(p->getEdge()->getID());
                break;
            case VAR_LANEPOSITION:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(p->getEdgePos());
                break;
            case VAR_COLOR:
                tempMsg.writeUnsignedByte(TYPE_COLOR);
                tempMsg.writeUnsignedByte(p->getParameter().color.red());
                tempMsg.writeUnsignedByte(p->getParameter().color.green());
                tempMsg.writeUnsignedByte(p->getParameter().color.blue());
                tempMsg.writeUnsignedByte(p->getParameter().color.alpha());
                break;
            case VAR_WAITING_TIME:
                tempMsg.writeUnsignedByte(TYPE_DOUBLE);
                tempMsg.writeDouble(p->getWaitingSeconds());
                break;
            case VAR_TYPE:
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(p->getVehicleType().getID());
                break;
            case VAR_NEXT_EDGE:
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(dynamic_cast<MSPerson*>(p)->getNextEdge());
                break;
            case VAR_EDGES: {
                int nextStageIndex = 0;
                if (!server.readTypeCheckingInt(inputStorage, nextStageIndex)) {
                    return server.writeErrorStatusCmd(CMD_GET_PERSON_VARIABLE, "The message must contain the stage index.", outputStorage);
                }
                if (nextStageIndex >= p->getNumRemainingStages()) {
                    return server.writeErrorStatusCmd(CMD_GET_PERSON_VARIABLE, "The stage index must be lower than the number of remaining stages.", outputStorage);
                }
                if (nextStageIndex < (p->getNumRemainingStages() - p->getNumStages())) {
                    return server.writeErrorStatusCmd(CMD_GET_PERSON_VARIABLE, "The negative stage index must refer to a valid previous stage.", outputStorage);
                }
                ConstMSEdgeVector edges = p->getEdges(nextStageIndex);
                tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                tempMsg.writeInt((int)edges.size());
                for (ConstMSEdgeVector::const_iterator i = edges.begin(); i != edges.end(); ++i) {
                    tempMsg.writeString((*i)->getID());
                }
                break;
            }
            case VAR_STAGE: {
                int nextStageIndex = 0;
                if (!server.readTypeCheckingInt(inputStorage, nextStageIndex)) {
                    return server.writeErrorStatusCmd(CMD_GET_PERSON_VARIABLE, "The message must contain the stage index.", outputStorage);
                }
                if (nextStageIndex >= p->getNumRemainingStages()) {
                    return server.writeErrorStatusCmd(CMD_GET_PERSON_VARIABLE, "The stage index must be lower than the number of remaining stages.", outputStorage);
                }
                if (nextStageIndex < (p->getNumRemainingStages() - p->getNumStages())) {
                    return server.writeErrorStatusCmd(CMD_GET_PERSON_VARIABLE, "The negative stage index must refer to a valid previous stage.", outputStorage);
                }
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(p->getStageType(nextStageIndex));
                break;
            }
            case VAR_STAGES_REMAINING:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt(p->getNumRemainingStages());
                break;
            case VAR_VEHICLE: {
                const SUMOVehicle* veh = p->getVehicle();
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(veh == 0 ? "" : veh->getID());
                break;
            }
            case VAR_PARAMETER: {
                std::string paramName = "";
                if (!server.readTypeCheckingString(inputStorage, paramName)) {
                    return server.writeErrorStatusCmd(CMD_GET_PERSON_VARIABLE, "Retrieval of a parameter requires its name.", outputStorage);
                }
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(p->getParameter().getParameter(paramName, ""));
            }
            default:
                TraCIServerAPI_VehicleType::getVariable(variable, p->getVehicleType(), tempMsg);
                break;
        }
    }
    server.writeStatusCmd(CMD_GET_PERSON_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, tempMsg);
    return true;
}


bool
TraCIServerAPI_Person::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                                  tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != VAR_PARAMETER
            && variable != ADD
            && variable != APPEND_STAGE
            && variable != REMOVE_STAGE
            && variable != VAR_SPEED
            && variable != VAR_TYPE
            && variable != VAR_LENGTH
            && variable != VAR_WIDTH
            && variable != VAR_HEIGHT
            && variable != VAR_MINGAP
            && variable != VAR_COLOR
       ) {
        return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Change Person State: unsupported variable " + toHex(variable, 2) + " specified", outputStorage);
    }
    // id
    MSTransportableControl& c = MSNet::getInstance()->getPersonControl();
    std::string id = inputStorage.readString();
    const bool shouldExist = variable != ADD;
    MSTransportable* p = c.get(id);
    if (p == 0 && shouldExist) {
        return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Person '" + id + "' is not known", outputStorage);
    }
    // process
    switch (variable) {
        case VAR_SPEED: {
            double speed = 0;
            if (!server.readTypeCheckingDouble(inputStorage, speed)) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Setting speed requires a double.", outputStorage);
            }
            // set the speed for all (walking) stages
            p->setSpeed(speed);
            // modify the vType so that stages added later are also affected
            TraCIServerAPI_VehicleType::setVariable(CMD_SET_VEHICLE_VARIABLE, variable, getSingularType(p), server, inputStorage, outputStorage);
        }
        break;
        case VAR_TYPE: {
            std::string vTypeID;
            if (!server.readTypeCheckingString(inputStorage, vTypeID)) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "The vehicle type id must be given as a string.", outputStorage);
            }
            MSVehicleType* vehicleType = MSNet::getInstance()->getVehicleControl().getVType(vTypeID);
            if (vehicleType == 0) {
                return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The vehicle type '" + vTypeID + "' is not known.", outputStorage);
            }
            p->replaceVehicleType(vehicleType);
            break;
        }
        case ADD: {
            if (p != 0) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "The person " + id + " to add already exists.", outputStorage);
            }
            if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Adding a person requires a compound object.", outputStorage);
            }
            if (inputStorage.readInt() != 4) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Adding a person needs four parameters.", outputStorage);
            }
            SUMOVehicleParameter vehicleParams;
            vehicleParams.id = id;

            std::string vTypeID;
            if (!server.readTypeCheckingString(inputStorage, vTypeID)) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "First parameter (type) requires a string.", outputStorage);
            }
            MSVehicleType* vehicleType = MSNet::getInstance()->getVehicleControl().getVType(vTypeID);
            if (!vehicleType) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Invalid type '" + vTypeID + "' for person '" + id + "'", outputStorage);
            }

            std::string edgeID;
            if (!server.readTypeCheckingString(inputStorage, edgeID)) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Second parameter (edge) requires a string.", outputStorage);
            }
            const MSEdge* edge = MSEdge::dictionary(edgeID);
            if (!edge) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Invalid edge '" + edgeID + "' for person: '" + id + "'", outputStorage);
            }
            int depart;
            if (!server.readTypeCheckingInt(inputStorage, depart)) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Third parameter (depart) requires an integer.", outputStorage);
            }
            if (depart < 0) {
                const int proc = -depart;
                if (proc >= static_cast<int>(DEPART_DEF_MAX)) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Invalid departure time.", outputStorage);
                }
                vehicleParams.departProcedure = (DepartDefinition)proc;
                vehicleParams.depart = MSNet::getInstance()->getCurrentTimeStep();
            } else if (depart < MSNet::getInstance()->getCurrentTimeStep()) {
                vehicleParams.depart = MSNet::getInstance()->getCurrentTimeStep();
                WRITE_WARNING("Departure time for person '" + id + "' is in the past; using current time instead.");
            } else {
                vehicleParams.depart = depart;
            }

            double pos;
            if (!server.readTypeCheckingDouble(inputStorage, pos)) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Fourth parameter (position) requires a double.", outputStorage);
            }
            vehicleParams.departPosProcedure = DEPART_POS_GIVEN;
            if (fabs(pos) > edge->getLength()) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Invalid departure position.", outputStorage);
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
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, e.what(), outputStorage);
            }
        }
        break;
        case APPEND_STAGE: {
            if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Adding a person stage requires a compound object.", outputStorage);
            }
            int numParameters = inputStorage.readInt();
            int stageType;
            if (!server.readTypeCheckingInt(inputStorage, stageType)) {
                return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "The first parameter for adding a stage must be the stage type given as int.", outputStorage);
            }
            // append driving stage
            if (stageType == MSTransportable::DRIVING) {
                if (numParameters != 4) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Adding a driving stage needs four parameters.", outputStorage);
                }
                std::string edgeID;
                if (!server.readTypeCheckingString(inputStorage, edgeID)) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Second parameter (edge) requires a string.", outputStorage);
                }
                const MSEdge* edge = MSEdge::dictionary(edgeID);
                if (!edge) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Invalid edge '" + edgeID + "' for person: '" + id + "'", outputStorage);
                }
                std::string lines;
                if (!server.readTypeCheckingString(inputStorage, lines)) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Third parameter (lines) requires a string.", outputStorage);
                }
                if (lines.size() == 0) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Empty lines parameter for person: '" + id + "'", outputStorage);
                }
                std::string stopID;
                MSStoppingPlace* bs = 0;
                if (!server.readTypeCheckingString(inputStorage, stopID)) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Fourth parameter (stopID) requires a string.", outputStorage);
                }
                if (stopID != "") {
                    bs = MSNet::getInstance()->getBusStop(stopID);
                    if (bs == 0) {
                        return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Invalid stopping place id '" + stopID + "' for person: '" + id + "'", outputStorage);
                    }
                }
                p->appendStage(new MSPerson::MSPersonStage_Driving(*edge, bs, -NUMERICAL_EPS, StringTokenizer(lines).getVector()));

                // append waiting stage
            } else if (stageType == MSTransportable::WAITING) {
                if (numParameters != 4) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Adding a waiting stage needs four parameters.", outputStorage);
                }
                int duration;
                if (!server.readTypeCheckingInt(inputStorage, duration)) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Second parameter (duration) requires an int.", outputStorage);
                }
                if (duration < 0) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Duration for person: '" + id + "' must not be negative", outputStorage);
                }
                std::string description;
                if (!server.readTypeCheckingString(inputStorage, description)) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Third parameter (description) requires a string.", outputStorage);
                }
                std::string stopID;
                MSStoppingPlace* bs = 0;
                if (!server.readTypeCheckingString(inputStorage, stopID)) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Fourth parameter (stopID) requires a string.", outputStorage);
                }
                if (stopID != "") {
                    bs = MSNet::getInstance()->getBusStop(stopID);
                    if (bs == 0) {
                        return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Invalid stopping place id '" + stopID + "' for person: '" + id + "'", outputStorage);
                    }
                }
                p->appendStage(new MSTransportable::Stage_Waiting(*p->getArrivalEdge(), duration, 0, p->getArrivalPos(), description, false));

                // append walking stage
            } else if (stageType == MSTransportable::MOVING_WITHOUT_VEHICLE) {
                if (numParameters != 6) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Adding a walking stage needs six parameters.", outputStorage);
                }
                std::vector<std::string> edgeIDs;
                if (!server.readTypeCheckingStringList(inputStorage, edgeIDs)) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Second parameter (edges) route must be defined as a list of edge ids.", outputStorage);
                }
                ConstMSEdgeVector edges;
                try {
                    MSEdge::parseEdgesList(edgeIDs, edges, "<unknown>");
                } catch (ProcessError& e) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, e.what(), outputStorage);
                }
                if (edges.empty()) {
                    return server.writeErrorStatusCmd(CMD_SET_VEHICLE_VARIABLE, "Empty edge list for walking stage of person '" + id + "'.", outputStorage);
                }
                double arrivalPos;
                if (!server.readTypeCheckingDouble(inputStorage, arrivalPos)) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Third parameter (arrivalPos) requires a double.", outputStorage);
                }
                if (fabs(arrivalPos) > edges.back()->getLength()) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Invalid arrivalPos for walking stage of person '" + id + "'.", outputStorage);
                }
                if (arrivalPos < 0) {
                    arrivalPos += edges.back()->getLength();
                }
                int duration;
                if (!server.readTypeCheckingInt(inputStorage, duration)) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Fourth parameter (duration) requires an int.", outputStorage);
                }
                double speed;
                if (!server.readTypeCheckingDouble(inputStorage, speed)) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Fifth parameter (speed) requires a double.", outputStorage);
                }
                if (speed < 0) {
                    speed = p->getVehicleType().getMaxSpeed();
                }
                std::string stopID;
                MSStoppingPlace* bs = 0;
                if (!server.readTypeCheckingString(inputStorage, stopID)) {
                    return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Fourth parameter (stopID) requires a string.", outputStorage);
                }
                if (stopID != "") {
                    bs = MSNet::getInstance()->getBusStop(stopID);
                    if (bs == 0) {
                        return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Invalid stopping place id '" + stopID + "' for person: '" + id + "'", outputStorage);
                    }
                }
                p->appendStage(new MSPerson::MSPersonStage_Walking(edges, bs, duration, speed, p->getArrivalPos(), arrivalPos, 0));


            } else {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "Invalid stage type for person '" + id + "'", outputStorage);
            }
        }
        break;
        case REMOVE_STAGE: {
            int nextStageIndex = 0;
            if (!server.readTypeCheckingInt(inputStorage, nextStageIndex)) {
                return server.writeErrorStatusCmd(CMD_GET_PERSON_VARIABLE, "The message must contain the stage index.", outputStorage);
            }
            if (nextStageIndex >= p->getNumRemainingStages()) {
                return server.writeErrorStatusCmd(CMD_GET_PERSON_VARIABLE, "The stage index must be lower than the number of remaining stages.", outputStorage);
            }
            if (nextStageIndex < 0) {
                return server.writeErrorStatusCmd(CMD_GET_PERSON_VARIABLE, "The stage index may not be negative.", outputStorage);
            }
            p->removeStage(nextStageIndex);
        }
        break;
        case VAR_PARAMETER: {
            if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "A compound object is needed for setting a parameter.", outputStorage);
            }
            //readt itemNo
            inputStorage.readInt();
            std::string name;
            if (!server.readTypeCheckingString(inputStorage, name)) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "The name of the parameter must be given as a string.", outputStorage);
            }
            std::string value;
            if (!server.readTypeCheckingString(inputStorage, value)) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, "The value of the parameter must be given as a string.", outputStorage);
            }
            ((SUMOVehicleParameter&) p->getParameter()).addParameter(name, value);
        }
        break;
        default:
            try {
                if (!TraCIServerAPI_VehicleType::setVariable(CMD_SET_PERSON_VARIABLE, variable, getSingularType(p), server, inputStorage, outputStorage)) {
                    return false;
                }
            } catch (ProcessError& e) {
                return server.writeErrorStatusCmd(CMD_SET_PERSON_VARIABLE, e.what(), outputStorage);
            }
            break;
    }
    server.writeStatusCmd(CMD_SET_PERSON_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}


bool
TraCIServerAPI_Person::getPosition(const std::string& id, Position& p) {
    MSPerson* person = dynamic_cast<MSPerson*>(MSNet::getInstance()->getPersonControl().get(id));
    if (person == 0) {
        return false;
    }
    p = person->getPosition();
    return true;
}


MSVehicleType&
TraCIServerAPI_Person::getSingularType(MSTransportable* const t) {
    const MSVehicleType& oType = t->getVehicleType();
    std::string newID = oType.getID().find('@') == std::string::npos ? oType.getID() + "@" + t->getID() : oType.getID();
    MSVehicleType* type = MSVehicleType::build(newID, &oType);
    t->replaceVehicleType(type);
    return *type;
}



#endif


/****************************************************************************/

