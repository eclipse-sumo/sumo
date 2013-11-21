/****************************************************************************/
/// @file    TraCIServerAPI_TLS.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting traffic light values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

#include "TraCIConstants.h"
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <microsim/MSLane.h>
#include "TraCIServerAPI_TLS.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_TLS::processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                               tcpip::Storage& outputStorage) {
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable != ID_LIST && variable != TL_RED_YELLOW_GREEN_STATE && variable != TL_COMPLETE_DEFINITION_RYG
            && variable != TL_CONTROLLED_LANES && variable != TL_CONTROLLED_LINKS
            && variable != TL_CURRENT_PHASE && variable != TL_CURRENT_PROGRAM
            && variable != TL_NEXT_SWITCH && variable != TL_PHASE_DURATION && variable != ID_COUNT) {
        return server.writeErrorStatusCmd(CMD_GET_TL_VARIABLE, "Get TLS Variable: unsupported variable specified", outputStorage);
    }
    // begin response building
    tcpip::Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_TL_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    if (variable == ID_LIST) {
        std::vector<std::string> ids = MSNet::getInstance()->getTLSControl().getAllTLIds();
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    } else if (variable == ID_COUNT) {
        std::vector<std::string> ids = MSNet::getInstance()->getTLSControl().getAllTLIds();
        tempMsg.writeUnsignedByte(TYPE_INTEGER);
        tempMsg.writeInt((int) ids.size());
    } else {
        if (!MSNet::getInstance()->getTLSControl().knows(id)) {
            return server.writeErrorStatusCmd(CMD_GET_TL_VARIABLE, "Traffic light '" + id + "' is not known", outputStorage);
        }
        MSTLLogicControl::TLSLogicVariants& vars = MSNet::getInstance()->getTLSControl().get(id);
        switch (variable) {
            case ID_LIST:
                break;
            case TL_RED_YELLOW_GREEN_STATE: {
                tempMsg.writeUnsignedByte(TYPE_STRING);
                std::string state = vars.getActive()->getCurrentPhaseDef().getState();
                tempMsg.writeString(state);
            }
            break;
            case TL_COMPLETE_DEFINITION_RYG: {
                std::vector<MSTrafficLightLogic*> logics = vars.getAllLogics();
                tempMsg.writeUnsignedByte(TYPE_COMPOUND);
                tcpip::Storage tempContent;
                unsigned int cnt = 0;
                tempContent.writeUnsignedByte(TYPE_INTEGER);
                tempContent.writeInt((int) logics.size());
                ++cnt;
                for (unsigned int i = 0; i < logics.size(); ++i) {
                    MSTrafficLightLogic* logic = logics[i];
                    tempContent.writeUnsignedByte(TYPE_STRING);
                    tempContent.writeString(logic->getProgramID());
                    ++cnt;
                    // type (always 0 by now)
                    tempContent.writeUnsignedByte(TYPE_INTEGER);
                    tempContent.writeInt(0);
                    ++cnt;
                    // subparameter (always 0 by now)
                    tempContent.writeUnsignedByte(TYPE_COMPOUND);
                    tempContent.writeInt(0);
                    ++cnt;
                    // (current) phase index
                    tempContent.writeUnsignedByte(TYPE_INTEGER);
                    tempContent.writeInt((int) logic->getCurrentPhaseIndex());
                    ++cnt;
                    // phase number
                    unsigned int phaseNo = logic->getPhaseNumber();
                    tempContent.writeUnsignedByte(TYPE_INTEGER);
                    tempContent.writeInt((int) phaseNo);
                    ++cnt;
                    for (unsigned int j = 0; j < phaseNo; ++j) {
                        MSPhaseDefinition phase = logic->getPhase(j);
                        tempContent.writeUnsignedByte(TYPE_INTEGER);
                        tempContent.writeInt(phase.duration);
                        ++cnt;
                        tempContent.writeUnsignedByte(TYPE_INTEGER);
                        tempContent.writeInt(phase.minDuration);
                        ++cnt; // not implemented
                        tempContent.writeUnsignedByte(TYPE_INTEGER);
                        tempContent.writeInt(phase.maxDuration);
                        ++cnt; // not implemented
                        const std::string& state = phase.getState();
                        //unsigned int linkNo = (unsigned int)(vars.getActive()->getLinks().size());
                        tempContent.writeUnsignedByte(TYPE_STRING);
                        tempContent.writeString(state);
                        ++cnt;
                    }
                }
                tempMsg.writeInt((int) cnt);
                tempMsg.writeStorage(tempContent);
            }
            break;
            case TL_CONTROLLED_LANES: {
                const MSTrafficLightLogic::LaneVectorVector& lanes = vars.getActive()->getLanes();
                tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
                std::vector<std::string> laneIDs;
                for (MSTrafficLightLogic::LaneVectorVector::const_iterator i = lanes.begin(); i != lanes.end(); ++i) {
                    const MSTrafficLightLogic::LaneVector& llanes = (*i);
                    for (MSTrafficLightLogic::LaneVector::const_iterator j = llanes.begin(); j != llanes.end(); ++j) {
                        laneIDs.push_back((*j)->getID());
                    }
                }
                tempMsg.writeStringList(laneIDs);
            }
            break;
            case TL_CONTROLLED_LINKS: {
                const MSTrafficLightLogic::LaneVectorVector& lanes = vars.getActive()->getLanes();
                const MSTrafficLightLogic::LinkVectorVector& links = vars.getActive()->getLinks();
                //
                tempMsg.writeUnsignedByte(TYPE_COMPOUND);
                tcpip::Storage tempContent;
                unsigned int cnt = 0;
                tempContent.writeUnsignedByte(TYPE_INTEGER);
                unsigned int no = (unsigned int) lanes.size();
                tempContent.writeInt((int) no);
                for (unsigned int i = 0; i < no; ++i) {
                    const MSTrafficLightLogic::LaneVector& llanes = lanes[i];
                    const MSTrafficLightLogic::LinkVector& llinks = links[i];
                    // number of links controlled by this signal (signal i)
                    tempContent.writeUnsignedByte(TYPE_INTEGER);
                    unsigned int no2 = (unsigned int) llanes.size();
                    tempContent.writeInt((int) no2);
                    ++cnt;
                    for (unsigned int j = 0; j < no2; ++j) {
                        MSLink* link = llinks[j];
                        std::vector<std::string> def;
                        // incoming lane
                        def.push_back(llanes[j]->getID());
                        // approached non-internal lane (if any)
                        def.push_back(link->getLane() != 0 ? link->getLane()->getID() : "");
                        // approached "via", internal lane (if any)
#ifdef HAVE_INTERNAL_LANES
                        def.push_back(link->getViaLane() != 0 ? link->getViaLane()->getID() : "");
#else
                        def.push_back("");
#endif
                        tempContent.writeUnsignedByte(TYPE_STRINGLIST);
                        tempContent.writeStringList(def);
                        ++cnt;
                    }
                }
                tempMsg.writeInt((int) cnt);
                tempMsg.writeStorage(tempContent);
            }
            break;
            case TL_CURRENT_PHASE:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt((int) vars.getActive()->getCurrentPhaseIndex());
                break;
            case TL_CURRENT_PROGRAM:
                tempMsg.writeUnsignedByte(TYPE_STRING);
                tempMsg.writeString(vars.getActive()->getProgramID());
                break;
            case TL_PHASE_DURATION:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt((int) vars.getActive()->getCurrentPhaseDef().duration);
                break;
            case TL_NEXT_SWITCH:
                tempMsg.writeUnsignedByte(TYPE_INTEGER);
                tempMsg.writeInt((int) vars.getActive()->getNextSwitchTime());
                break;
            case TL_CONTROLLED_JUNCTIONS: {
            }
            break;
            default:
                break;
        }
    }
    server.writeStatusCmd(CMD_GET_TL_VARIABLE, RTYPE_OK, "", outputStorage);
    server.writeResponseWithLength(outputStorage, tempMsg);
    return true;
}


bool
TraCIServerAPI_TLS::processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                               tcpip::Storage& outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable != TL_PHASE_INDEX && variable != TL_PROGRAM
            && variable != TL_PHASE_DURATION && variable != TL_RED_YELLOW_GREEN_STATE && variable != TL_COMPLETE_PROGRAM_RYG) {
        return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "Change TLS State: unsupported variable specified", outputStorage);
    }
    std::string id = inputStorage.readString();
    if (!MSNet::getInstance()->getTLSControl().knows(id)) {
        return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "Traffic light '" + id + "' is not known", outputStorage);
    }
    MSTLLogicControl& tlsControl = MSNet::getInstance()->getTLSControl();
    SUMOTime cTime = MSNet::getInstance()->getCurrentTimeStep();
    MSTLLogicControl::TLSLogicVariants& vars = tlsControl.get(id);
    switch (variable) {
        case TL_PHASE_INDEX: {
            int index = 0;
            if (!server.readTypeCheckingInt(inputStorage, index)) {
                return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "The phase index must be given as an integer.", outputStorage);
            }
            if (index < 0 || vars.getActive()->getPhaseNumber() <= (unsigned int)index) {
                return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "The phase index is not in the allowed range.", outputStorage);
            }
            int duration = vars.getActive()->getPhase(index).duration;
            vars.getActive()->changeStepAndDuration(tlsControl, cTime, index, duration);
        }
        break;
        case TL_PROGRAM: {
            std::string subID;
            if (!server.readTypeCheckingString(inputStorage, subID)) {
                return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "The program must be given as a string.", outputStorage);
            }
            try {
                vars.switchTo(tlsControl, subID);
            } catch (ProcessError& e) {
                return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, e.what(), outputStorage);
            }
        }
        break;
        case TL_PHASE_DURATION: {
            int duration = 0;
            if (!server.readTypeCheckingInt(inputStorage, duration)) {
                return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "The phase duration must be given as an integer.", outputStorage);
            }
            int index = vars.getActive()->getCurrentPhaseIndex();
            vars.getActive()->changeStepAndDuration(tlsControl, cTime, index, duration);
        }
        break;
        case TL_RED_YELLOW_GREEN_STATE: {
            std::string state;
            if (!server.readTypeCheckingString(inputStorage, state)) {
                return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "The phase must be given as a string.", outputStorage);
            }
            vars.setStateInstantiatingOnline(tlsControl, state);
        }
        break;
        case TL_COMPLETE_PROGRAM_RYG: {
            if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "A compound object is needed for setting a new program.", outputStorage);
            }
            int type = 0, index = 0, phaseNo = 0;
            //read itemNo
            inputStorage.readInt();
            std::string subid;
            if (!server.readTypeCheckingString(inputStorage, subid)) {
                return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "set program: 1. parameter (subid) must be a string.", outputStorage);
            }
            if (!server.readTypeCheckingInt(inputStorage, type)) {
                return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "set program: 2. parameter (type) must be an int.", outputStorage);
            }
            if (inputStorage.readUnsignedByte() != TYPE_COMPOUND) {
                return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "set program: 3. parameter (subparams) must be a compound object.", outputStorage);
            }
            inputStorage.readInt();
            if (!server.readTypeCheckingInt(inputStorage, index)) {
                return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "set program: 4. parameter (index) must be an int.", outputStorage);
            }
            if (!server.readTypeCheckingInt(inputStorage, phaseNo)) {
                return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "set program: 5. parameter (phase number) must be an int.", outputStorage);
            }
            // make sure index and phaseNo are consistent
            if (index >= phaseNo) {
                return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "set program: 4/5. parameter (index) must be less than parameter (phase number).", outputStorage);
            }

            std::vector<MSPhaseDefinition*> phases;
            for (int j = 0; j < phaseNo; ++j) {
                int duration = 0, minDuration = 0, maxDuration = 0;
                if (!server.readTypeCheckingInt(inputStorage, duration)) {
                    return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "set program: 6.1. parameter (duration) must be an int.", outputStorage);
                }
                if (!server.readTypeCheckingInt(inputStorage, minDuration)) {
                    return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "set program: 6.2. parameter (min duration) must be an int.", outputStorage);
                }
                if (!server.readTypeCheckingInt(inputStorage, maxDuration)) {
                    return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "set program: 6.3. parameter (max duration) must be an int.", outputStorage);
                }
                std::string state;
                if (!server.readTypeCheckingString(inputStorage, state)) {
                    return server.writeErrorStatusCmd(CMD_SET_TL_VARIABLE, "set program: 6.4. parameter (phase) must be a string.", outputStorage);
                }
                MSPhaseDefinition* phase = new MSPhaseDefinition(duration, minDuration, maxDuration, state);
                phases.push_back(phase);
            }
            if (vars.getLogic(subid) == 0) {
                MSTrafficLightLogic* logic = new MSSimpleTrafficLightLogic(tlsControl, id, subid, phases, index, 0, std::map<std::string, std::string>());
                vars.addLogic(subid, logic, true, true);
            } else {
                static_cast<MSSimpleTrafficLightLogic*>(vars.getLogic(subid))->setPhases(phases, index);
            }
        }
        break;
        default:
            break;
    }
    server.writeStatusCmd(CMD_SET_TL_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}

#endif


/****************************************************************************/

