/****************************************************************************/
/// @file    TraCIServerAPI_TLS.cpp
/// @author  Daniel Krajzewicz
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting traffic light values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include "TraCIConstants.h"
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <microsim/MSLane.h>
#include "TraCIServerAPI_TLS.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;
using namespace traci;
using namespace tcpip;


// ===========================================================================
// static variables
// ===========================================================================
bool TraCIServerAPI_TLS::myHaveWarnedAboutDeprecatedPhases = false;


// ===========================================================================
// method definitions
// ===========================================================================
bool
TraCIServerAPI_TLS::processGet(TraCIServer &server, tcpip::Storage &inputStorage,
                               tcpip::Storage &outputStorage) {
    std::string warning = ""; // additional description for response
    // variable & id
    int variable = inputStorage.readUnsignedByte();
    std::string id = inputStorage.readString();
    // check variable
    if (variable!=ID_LIST&&variable!=TL_RED_YELLOW_GREEN_STATE&&variable!=TL_PHASE_BRAKE_YELLOW_STATE
            &&variable!=TL_COMPLETE_DEFINITION_PBY&&variable!=TL_COMPLETE_DEFINITION_RYG
            &&variable!=TL_CONTROLLED_LANES&&variable!=TL_CONTROLLED_LINKS
            &&variable!=TL_CURRENT_PHASE&&variable!=TL_CURRENT_PROGRAM
            &&variable!=TL_NEXT_SWITCH&&variable!=TL_PHASE_DURATION) {
        server.writeStatusCmd(CMD_GET_TL_VARIABLE, RTYPE_ERR, "Get TLS Variable: unsupported variable specified", outputStorage);
        return false;
    }
    // begin response building
    Storage tempMsg;
    //  response-code, variableID, objectID
    tempMsg.writeUnsignedByte(RESPONSE_GET_TL_VARIABLE);
    tempMsg.writeUnsignedByte(variable);
    tempMsg.writeString(id);
    if (variable==ID_LIST) {
        std::vector<std::string> ids = MSNet::getInstance()->getTLSControl().getAllTLIds();
        tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
        tempMsg.writeStringList(ids);
    } else {
        if (!MSNet::getInstance()->getTLSControl().knows(id)) {
            server.writeStatusCmd(CMD_GET_TL_VARIABLE, RTYPE_ERR, "Traffic light '" + id + "' is not known", outputStorage);
            return false;
        }
        MSTLLogicControl::TLSLogicVariants &vars = MSNet::getInstance()->getTLSControl().get(id);
        switch (variable) {
        case ID_LIST:
            break;
        case TL_RED_YELLOW_GREEN_STATE: {
            tempMsg.writeUnsignedByte(TYPE_STRING);
            std::string state = vars.getActive()->getCurrentPhaseDef().getState();
            tempMsg.writeString(state);
        }
        break;
        case TL_PHASE_BRAKE_YELLOW_STATE: {
            const std::string &state = vars.getActive()->getCurrentPhaseDef().getState();
            unsigned int linkNo = vars.getActive()->getLinks().size();
            tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
            std::vector<std::string> phaseDef;
            phaseDef.push_back(MSPhaseDefinition::new2driveMask(state));
            phaseDef.push_back(MSPhaseDefinition::new2brakeMask(state));
            phaseDef.push_back(MSPhaseDefinition::new2yellowMask(state));
            tempMsg.writeStringList(phaseDef);
            if (!myHaveWarnedAboutDeprecatedPhases) {
                myHaveWarnedAboutDeprecatedPhases = true;
                warning = "Defining phases using drive/brake/yellow mask is deprecated. Move to states.";
            }
        }
        break;
        case TL_COMPLETE_DEFINITION_PBY: {
            std::vector<MSTrafficLightLogic*> logics = vars.getAllLogics();
            tempMsg.writeUnsignedByte(TYPE_COMPOUND);
            Storage tempContent;
            unsigned int cnt = 0;
            tempContent.writeUnsignedByte(TYPE_INTEGER);
            tempContent.writeInt((int) logics.size());
            ++cnt;
            for (unsigned int i=0; i<logics.size(); ++i) {
                MSTrafficLightLogic *logic = logics[i];
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
                for (unsigned int j=0; j<phaseNo; ++j) {
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
                    const std::string &state = phase.getState();
                    unsigned int linkNo = vars.getActive()->getLinks().size();
                    tempContent.writeUnsignedByte(TYPE_STRINGLIST);
                    std::vector<std::string> phaseDef;
                    phaseDef.push_back(MSPhaseDefinition::new2driveMask(state));
                    phaseDef.push_back(MSPhaseDefinition::new2brakeMask(state));
                    phaseDef.push_back(MSPhaseDefinition::new2yellowMask(state));
                    tempContent.writeStringList(phaseDef);
                    ++cnt;
                }
            }
            tempMsg.writeInt((int) cnt);
            tempMsg.writeStorage(tempContent);
            if (!myHaveWarnedAboutDeprecatedPhases) {
                myHaveWarnedAboutDeprecatedPhases = true;
                warning = "Defining phases using drive/brake/yellow mask is deprecated. Move to states.";
            }
        }
        break;
        case TL_COMPLETE_DEFINITION_RYG: {
            std::vector<MSTrafficLightLogic*> logics = vars.getAllLogics();
            tempMsg.writeUnsignedByte(TYPE_COMPOUND);
            Storage tempContent;
            unsigned int cnt = 0;
            tempContent.writeUnsignedByte(TYPE_INTEGER);
            tempContent.writeInt((int) logics.size());
            ++cnt;
            for (unsigned int i=0; i<logics.size(); ++i) {
                MSTrafficLightLogic *logic = logics[i];
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
                for (unsigned int j=0; j<phaseNo; ++j) {
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
                    const std::string &state = phase.getState();
                    unsigned int linkNo = vars.getActive()->getLinks().size();
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
            const MSTrafficLightLogic::LaneVectorVector &lanes = vars.getActive()->getLanes();
            tempMsg.writeUnsignedByte(TYPE_STRINGLIST);
            std::vector<std::string> laneIDs;
            for (MSTrafficLightLogic::LaneVectorVector::const_iterator i=lanes.begin(); i!=lanes.end(); ++i) {
                const MSTrafficLightLogic::LaneVector &llanes = (*i);
                for (MSTrafficLightLogic::LaneVector::const_iterator j=llanes.begin(); j!=llanes.end(); ++j) {
                    laneIDs.push_back((*j)->getID());
                }
            }
            tempMsg.writeStringList(laneIDs);
        }
        break;
        case TL_CONTROLLED_LINKS: {
            const MSTrafficLightLogic::LaneVectorVector &lanes = vars.getActive()->getLanes();
            const MSTrafficLightLogic::LinkVectorVector &links = vars.getActive()->getLinks();
            //
            tempMsg.writeUnsignedByte(TYPE_COMPOUND);
            Storage tempContent;
            unsigned int cnt = 0;
            tempContent.writeUnsignedByte(TYPE_INTEGER);
            unsigned int no = (unsigned int) lanes.size();
            tempContent.writeInt((int) no);
            for (unsigned int i=0; i<no; ++i) {
                const MSTrafficLightLogic::LaneVector &llanes = lanes[i];
                const MSTrafficLightLogic::LinkVector &llinks = links[i];
                // number of links controlled by this signal (signal i)
                tempContent.writeUnsignedByte(TYPE_INTEGER);
                unsigned int no2 = (unsigned int) llanes.size();
                tempContent.writeInt((int) no2);
                ++cnt;
                for (unsigned int j=0; j<no2; ++j) {
                    MSLink *link = llinks[j];
                    std::vector<std::string> def;
                    // incoming lane
                    def.push_back(llanes[j]->getID());
                    // approached non-internal lane (if any)
                    def.push_back(link->getLane()!=0 ? link->getLane()->getID() : "");
                    // approached "via", internal lane (if any)
#ifdef HAVE_INTERNAL_LANES
                    def.push_back(link->getViaLane()!=0 ? link->getViaLane()->getID() : "");
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
    server.writeStatusCmd(CMD_GET_TL_VARIABLE, RTYPE_OK, warning, outputStorage);
    // send response
    outputStorage.writeUnsignedByte(0); // command length -> extended
    outputStorage.writeInt(1 + 4 + tempMsg.size());
    outputStorage.writeStorage(tempMsg);
    return true;
}


bool
TraCIServerAPI_TLS::processSet(TraCIServer &server, tcpip::Storage &inputStorage,
                               tcpip::Storage &outputStorage) {
    std::string warning = ""; // additional description for response
    // variable
    int variable = inputStorage.readUnsignedByte();
    if (variable!=TL_PHASE_BRAKE_YELLOW_STATE&&variable!=TL_PHASE_INDEX&&variable!=TL_PROGRAM
            &&variable!=TL_PHASE_DURATION&&variable!=TL_RED_YELLOW_GREEN_STATE&&variable!=TL_COMPLETE_PROGRAM_RYG) {
        server.writeStatusCmd(CMD_SET_TL_VARIABLE, RTYPE_ERR, "Change TLS State: unsupported variable specified", outputStorage);
        return false;
    }
    std::string id = inputStorage.readString();
    if (!MSNet::getInstance()->getTLSControl().knows(id)) {
        server.writeStatusCmd(CMD_SET_TL_VARIABLE, RTYPE_ERR, "Traffic light '" + id + "' is not known", outputStorage);
        return false;
    }
    MSTLLogicControl &tlsControl = MSNet::getInstance()->getTLSControl();
    SUMOTime cTime = MSNet::getInstance()->getCurrentTimeStep();
    MSTLLogicControl::TLSLogicVariants &vars = tlsControl.get(id);
    int valueDataType = inputStorage.readUnsignedByte();
    switch (variable) {
    case TL_PHASE_BRAKE_YELLOW_STATE: {
        if (valueDataType!=TYPE_STRINGLIST) {
            server.writeStatusCmd(CMD_SET_TL_VARIABLE, RTYPE_ERR, "The phase must be given as three strings.", outputStorage);
            return false;
        }
        std::vector<std::string> defs = inputStorage.readStringList();
        if (defs.size()!=3) {
            server.writeStatusCmd(CMD_SET_TL_VARIABLE, RTYPE_ERR, "The phase must be given as three strings.", outputStorage);
            return false;
        }
        // build only once...
        std::string state = MSPhaseDefinition::old2new(defs[0], defs[1], defs[2]);
        MSPhaseDefinition *phase = new MSPhaseDefinition(DELTA_T, state);
        std::vector<MSPhaseDefinition*> phases;
        phases.push_back(phase);
        MSTrafficLightLogic *logic = new MSSimpleTrafficLightLogic(tlsControl, id, "online", phases, 0, cTime+DELTA_T);
        if (!vars.addLogic("online", logic, true, true)) {
            delete logic;
            MSPhaseDefinition nphase(DELTA_T, state);
            *(static_cast<MSSimpleTrafficLightLogic*>(vars.getLogic("online"))->getPhases()[0]) = nphase;
            vars.getActive()->setTrafficLightSignals(MSNet::getInstance()->getCurrentTimeStep());
            vars.executeOnSwitchActions();
        }
        if (!myHaveWarnedAboutDeprecatedPhases) {
            myHaveWarnedAboutDeprecatedPhases = true;
            warning = "Defining phases using drive/brake/yellow mask is deprecated. Move to states.";
        }
    }
    break;
    case TL_PHASE_INDEX: {
        if (valueDataType!=TYPE_INTEGER) {
            server.writeStatusCmd(CMD_SET_TL_VARIABLE, RTYPE_ERR, "The phase index must be given as an integer.", outputStorage);
            return false;
        }
        int index = inputStorage.readInt();
        if (index<0||vars.getActive()->getPhaseNumber()<=index) {
            server.writeStatusCmd(CMD_SET_TL_VARIABLE, RTYPE_ERR, "The phase index is not in the allowed range.", outputStorage);
            return false;
        }
        int duration = vars.getActive()->getPhase(index).duration;
        vars.getActive()->changeStepAndDuration(tlsControl, cTime, index, duration);
    }
    break;
    case TL_PROGRAM: {
        if (valueDataType!=TYPE_STRING) {
            server.writeStatusCmd(CMD_SET_TL_VARIABLE, RTYPE_ERR, "The program must be given as a string.", outputStorage);
            return false;
        }
        std::string subID = inputStorage.readString();
        try {
            vars.switchTo(tlsControl, subID);
        } catch (ProcessError &e) {
            server.writeStatusCmd(CMD_SET_TL_VARIABLE, RTYPE_ERR, e.what(), outputStorage);
            return false;
        }
    }
    break;
    case TL_PHASE_DURATION: {
        if (valueDataType!=TYPE_INTEGER) {
            server.writeStatusCmd(CMD_SET_TL_VARIABLE, RTYPE_ERR, "The phase duration must be given as an integer.", outputStorage);
            return false;
        }
        int duration = inputStorage.readInt();
        int index = vars.getActive()->getCurrentPhaseIndex();
        vars.getActive()->changeStepAndDuration(tlsControl, cTime, index, duration);
    }
    break;
    case TL_RED_YELLOW_GREEN_STATE: {
        if (valueDataType!=TYPE_STRING) {
            server.writeStatusCmd(CMD_SET_TL_VARIABLE, RTYPE_ERR, "The phase must be given as a string.", outputStorage);
            return false;
        }
        // build only once...
        std::string state = inputStorage.readString();
        MSPhaseDefinition *phase = new MSPhaseDefinition(DELTA_T, state);
        std::vector<MSPhaseDefinition*> phases;
        phases.push_back(phase);
        if (vars.getLogic("online")==0) {
            MSTrafficLightLogic *logic = new MSSimpleTrafficLightLogic(tlsControl, id, "online", phases, 0, cTime+DELTA_T);
            vars.addLogic("online", logic, true, true);
            vars.getActive()->setTrafficLightSignals(MSNet::getInstance()->getCurrentTimeStep());
            vars.executeOnSwitchActions();
        } else {
            MSPhaseDefinition nphase(DELTA_T, state);
            *(static_cast<MSSimpleTrafficLightLogic*>(vars.getLogic("online"))->getPhases()[0]) = nphase;
            vars.getActive()->setTrafficLightSignals(MSNet::getInstance()->getCurrentTimeStep());
            vars.executeOnSwitchActions();
        }
    }
    break;
    case TL_COMPLETE_PROGRAM_RYG: {
        if (valueDataType!=TYPE_COMPOUND) {
            server.writeStatusCmd(CMD_SET_TL_VARIABLE, RTYPE_ERR, "A compound object is needed for setting a new program.", outputStorage);
            return false;
        }
        unsigned int itemNo = inputStorage.readInt();
        if (inputStorage.readUnsignedByte()!=TYPE_STRING) {
            server.writeStatusCmd(CMD_SET_TL_VARIABLE, RTYPE_ERR, "set program: 1. parameter (subid) must be a string.", outputStorage);
            return false;
        }
        std::string subid = inputStorage.readString();
        if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
            server.writeStatusCmd(CMD_SET_TL_VARIABLE, RTYPE_ERR, "set program: 2. parameter (type) must be an int.", outputStorage);
            return false;
        }
        int type = inputStorage.readInt();
        if (inputStorage.readUnsignedByte()!=TYPE_COMPOUND) {
            server.writeStatusCmd(CMD_SET_TL_VARIABLE, RTYPE_ERR, "set program: 3. parameter (subparams) must be a compound object.", outputStorage);
            return false;
        }
        int sublength = inputStorage.readInt();
        if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
            server.writeStatusCmd(CMD_SET_TL_VARIABLE, RTYPE_ERR, "set program: 4. parameter (index) must be an int.", outputStorage);
            return false;
        }
        int index = inputStorage.readInt();
        if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
            server.writeStatusCmd(CMD_SET_TL_VARIABLE, RTYPE_ERR, "set program: 5. parameter (phase number) must be an int.", outputStorage);
            return false;
        }
        int phaseNo = inputStorage.readInt();
        std::vector<MSPhaseDefinition*> phases;
        for (unsigned int j=0; j<phaseNo; ++j) {
            if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
                server.writeStatusCmd(CMD_SET_TL_VARIABLE, RTYPE_ERR, "set program: 6.1. parameter (duration) must be an int.", outputStorage);
                return false;
            }
            int duration = inputStorage.readInt();
            if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
                server.writeStatusCmd(CMD_SET_TL_VARIABLE, RTYPE_ERR, "set program: 6.2. parameter (min duration) must be an int.", outputStorage);
                return false;
            }
            int minDuration = inputStorage.readInt();
            if (inputStorage.readUnsignedByte()!=TYPE_INTEGER) {
                server.writeStatusCmd(CMD_SET_TL_VARIABLE, RTYPE_ERR, "set program: 6.3. parameter (max duration) must be an int.", outputStorage);
                return false;
            }
            int maxDuration = inputStorage.readInt();
            if (inputStorage.readUnsignedByte()!=TYPE_STRING) {
                server.writeStatusCmd(CMD_SET_TL_VARIABLE, RTYPE_ERR, "set program: 6.4. parameter (phase) must be a string.", outputStorage);
                return false;
            }
            std::string state = inputStorage.readString();
            MSPhaseDefinition *phase = new MSPhaseDefinition(duration, minDuration, maxDuration, state);
            phases.push_back(phase);
        }
        if (vars.getLogic(subid)==0) {
            MSTrafficLightLogic *logic = new MSSimpleTrafficLightLogic(tlsControl, id, subid, phases, index, 0);
            vars.addLogic(subid, logic, true, true);
            vars.getActive()->setTrafficLightSignals(MSNet::getInstance()->getCurrentTimeStep());
            vars.executeOnSwitchActions();
        } else {
            static_cast<MSSimpleTrafficLightLogic*>(vars.getLogic(subid))->getPhases() = phases;
            vars.getActive()->setTrafficLightSignals(MSNet::getInstance()->getCurrentTimeStep());
            vars.executeOnSwitchActions();
        }
    }
    break;
    default:
        break;
    }
    server.writeStatusCmd(CMD_SET_TL_VARIABLE, RTYPE_OK, warning, outputStorage);
    return true;
}



// ------ "old" API functions ------
bool
TraCIServerAPI_TLS::commandGetAllTLIds(TraCIServer &server, tcpip::Storage&/*inputStorage*/, tcpip::Storage &outputStorage) {
    // get the TLLogicControl
    MSTLLogicControl &tlsControl = MSNet::getInstance()->getTLSControl();
    // get the ids
    std::vector<std::string> idList = tlsControl.getAllTLIds();
    if (idList.size() == 0) {
        // create negative response message
        server.writeStatusCmd(CMD_GETALLTLIDS, RTYPE_ERR, "Could not retrieve any traffic light id");
        return false;
    }
    // create positive response message
    server.writeStatusCmd(CMD_GETALLTLIDS, RTYPE_OK, "");
    // create a response command for each std::string id
    for (std::vector<std::string>::iterator iter = idList.begin(); iter != idList.end(); iter++) {
        outputStorage.writeByte(2 + (4 + (*iter).size())); // command length
        outputStorage.writeByte(CMD_TLIDLIST); // command type
        outputStorage.writeString((*iter)); // id string
    }
    return true;
}


bool
TraCIServerAPI_TLS::commandGetTLStatus(TraCIServer &server, tcpip::Storage &inputStorage, tcpip::Storage &outputStorage) {
    SUMOTime lookback = 60*1000.; // Time to look in history for recognizing yellowTimes
    tcpip::Storage tempMsg;

    int extId = inputStorage.readInt(); // trafic light id
    SUMOTime timeFrom = inputStorage.readInt(); // start of time interval
    SUMOTime timeTo = inputStorage.readInt(); // end of time interval

    // get the running programm of the traffic light
    MSTrafficLightLogic* const tlLogic = server.getTLLogicByExtId(extId);
    // error checking
    if (tlLogic == 0) {
        server.writeStatusCmd(CMD_GETTLSTATUS, RTYPE_ERR, "Could not retrieve traffic light with given id");
        return false;
    }
    if ((timeTo < timeFrom) || (timeTo < 0) || (timeFrom < 0)) {
        server.writeStatusCmd(CMD_GETTLSTATUS, RTYPE_ERR, "The given time interval is not valid");
        return false;
    }
    // acknowledge the request
    server.writeStatusCmd(CMD_GETTLSTATUS, RTYPE_OK, "");
    std::vector<MSLink::LinkState> linkStates;
    std::vector<double> yellowTimes;
    size_t lastStep = tlLogic->getCurrentPhaseIndex();
    MSPhaseDefinition phase = tlLogic->getCurrentPhaseDef();
    MSTrafficLightLogic::LinkVectorVector affectedLinks = tlLogic->getLinks();
    // save the current link states
    for (int i = 0; i < affectedLinks.size(); i++) {
        linkStates.push_back(phase.getSignalState(i));
        yellowTimes.push_back(-1);
    }
    // check every second of the given time interval for a switch in the traffic light's phases
    for (SUMOTime time = timeFrom - lookback; time <= timeTo; time+=DELTA_T) {
        if (time < 0) time = 0;
        SUMOTime position = tlLogic->getPhaseIndexAtTime(time);
        size_t currentStep = tlLogic->getIndexFromOffset(position);
        if (currentStep != lastStep) {
            lastStep = currentStep;
            phase = tlLogic->getPhase(currentStep);
            // for every link of the tl's junction, compare the actual and the last red/green state
            // for each link with new red/green status, write a TLSWITCH command
            std::map<const MSEdge*, pair<const MSEdge*, int> > writtenEdgePairs;
            for (int i = 0; i < linkStates.size(); i++) {
                MSLink::LinkState nextLinkState = phase.getSignalState(i);
                if (nextLinkState == MSLink::LINKSTATE_TL_YELLOW_MAJOR || nextLinkState == MSLink::LINKSTATE_TL_YELLOW_MINOR) {
                    if (yellowTimes[i] < 0) yellowTimes[i] = time;
                } else {
                    if (nextLinkState != linkStates[i] && time >= timeFrom) {
                        linkStates[i] = nextLinkState;
                        // get the group of links that is affected by the changed light status
                        MSTrafficLightLogic::LinkVector linkGroup = affectedLinks[i];
                        // get the group of preceding lanes of the link group
                        MSTrafficLightLogic::LaneVector laneGroup = tlLogic->getLanesAt(i);
                        for (int j = 0; j < linkGroup.size(); j++) {
                            MSEdge &precEdge = laneGroup[j]->getEdge();
                            MSEdge &succEdge = linkGroup[j]->getLane()->getEdge();
                            // for each pair of edges and every different tl state, write only one tl switch command
                            std::map<const MSEdge*, pair<const MSEdge*, int> >::iterator itPair = writtenEdgePairs.find(&precEdge);
                            if (itPair != writtenEdgePairs.end()) {
                                if (itPair->second.first == &succEdge && itPair->second.second == nextLinkState) {
                                    continue;
                                }
                            }
                            // remember the current edge pair and tl status
                            writtenEdgePairs[&precEdge] = std::make_pair(&succEdge, nextLinkState);
                            // time of the switch
                            tempMsg.writeInt(time);
                            // preceeding edge id
                            tempMsg.writeString(precEdge.getID());
                            // traffic light's position on preceeding edge
                            tempMsg.writeFloat(laneGroup[j]->getShape().length());
                            // succeeding edge id
                            tempMsg.writeString(succEdge.getID());
                            // new status
                            switch (nextLinkState) {
                            case MSLink::LINKSTATE_TL_GREEN_MAJOR:
                            case MSLink::LINKSTATE_TL_GREEN_MINOR:
                                tempMsg.writeUnsignedByte(TLPHASE_GREEN);
                                break;
                            case MSLink::LINKSTATE_TL_RED:
                                tempMsg.writeUnsignedByte(TLPHASE_RED);
                                break;
                            case MSLink::LINKSTATE_TL_OFF_BLINKING:
                                tempMsg.writeUnsignedByte(TLPHASE_BLINKING);
                                break;
                            case MSLink::LINKSTATE_TL_OFF_NOSIGNAL:
                                tempMsg.writeUnsignedByte(TLPHASE_NOSIGNAL);
                                break;
                            default:
                                tempMsg.writeUnsignedByte(TLPHASE_NOSIGNAL);
                            }
                            //yellow time
                            tempMsg.writeInt(yellowTimes[i]<0 ? 0 : time - yellowTimes[i]);

                            if (tempMsg.size() <= 253) {
                                // command length
                                outputStorage.writeUnsignedByte(1 + 1 + tempMsg.size());
                            } else {
                                // command length extended
                                outputStorage.writeUnsignedByte(0);
                                outputStorage.writeInt(1 + 4 + 1 + tempMsg.size());
                            }
                            // command type
                            outputStorage.writeUnsignedByte(CMD_TLSWITCH);
                            // command content
                            outputStorage.writeStorage(tempMsg);
                            tempMsg.reset();
                        }
                    }
                    yellowTimes[i] = -1;
                }
            }
        }
    }
    return true;
}


/****************************************************************************/

