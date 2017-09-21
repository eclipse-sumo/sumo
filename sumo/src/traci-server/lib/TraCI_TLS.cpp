/****************************************************************************/
/// @file    TraCI_TLS.cpp
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
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

#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <microsim/traffic_lights/MSSimpleTrafficLightLogic.h>
#include "TraCI_TLS.h"


// ===========================================================================
// member definitions
// ===========================================================================
std::vector<std::string>
TraCI_TLS::getIDList() {
    return MSNet::getInstance()->getTLSControl().getAllTLIds();
}


int
TraCI_TLS::getIDCount() {
    return (int)getIDList().size();
}


std::string
TraCI_TLS::getRedYellowGreenState(const std::string& tlsID) {
    return getTLS(tlsID).getActive()->getCurrentPhaseDef().getState();
}


std::vector<TraCILogic>
TraCI_TLS::getCompleteRedYellowGreenDefinition(const std::string& tlsID) {
    std::vector<TraCILogic> result;
    std::vector<MSTrafficLightLogic*> logics = getTLS(tlsID).getAllLogics();
    for (MSTrafficLightLogic* logic : logics) {
        TraCILogic l(logic->getProgramID(), 0, std::map<std::string, double>(), logic->getCurrentPhaseIndex(), std::vector<TraCIPhase>());
        for (int j = 0; j < logic->getPhaseNumber(); ++j) {
            MSPhaseDefinition phase = logic->getPhase(j);
            l.phases.emplace_back(TraCIPhase(phase.duration, phase.minDuration, phase.maxDuration, phase.getState()));
        }
    }
    return result;
}



std::vector<std::string>
TraCI_TLS::getControlledLanes(const std::string& tlsID) {
    std::vector<std::string> laneIDs;
    const MSTrafficLightLogic::LaneVectorVector& lanes = getTLS(tlsID).getActive()->getLaneVectors();
    for (const MSTrafficLightLogic::LaneVector& llanes : lanes) {
        for (const MSLane* l : llanes) {
            laneIDs.push_back(l->getID());
        }
    }
    return laneIDs;
}



std::vector<TraCILink>
TraCI_TLS::getControlledLinks(const std::string& tlsID) {
    std::vector<TraCILink> result;
    const MSTrafficLightLogic::LaneVectorVector& lanes = getTLS(tlsID).getActive()->getLaneVectors();
    const MSTrafficLightLogic::LinkVectorVector& links = getTLS(tlsID).getActive()->getLinks();
    for (int i = 0; i < (int)lanes.size(); ++i) {
        const MSTrafficLightLogic::LaneVector& llanes = lanes[i];
        const MSTrafficLightLogic::LinkVector& llinks = links[i];
        // number of links controlled by this signal (signal i)
        for (int j = 0; j < (int)llanes.size(); ++j) {
            MSLink* link = llinks[j];
            // approached non-internal lane (if any)
            const std::string to = link->getLane() != 0 ? link->getLane()->getID() : "";
            // approached "via", internal lane (if any)
            const std::string via = link->getViaLane() != 0 ? link->getViaLane()->getID() : "";
            result.emplace_back(TraCILink(llanes[j]->getID(), to, via));
        }
    }
    return result;
}



std::string
TraCI_TLS::getProgram(const std::string& tlsID) {
    return getTLS(tlsID).getActive()->getProgramID();
}



int
TraCI_TLS::getPhase(const std::string& tlsID) {
    return getTLS(tlsID).getActive()->getCurrentPhaseIndex();
}



SUMOTime
TraCI_TLS::gePhaseDuration(const std::string& tlsID) {
    return getTLS(tlsID).getActive()->getCurrentPhaseDef().duration;
}


SUMOTime
TraCI_TLS::getNextSwitch(const std::string& tlsID) {
    return getTLS(tlsID).getActive()->getNextSwitchTime();
}


void
TraCI_TLS::setRedYellowGreenState(const std::string& tlsID, const std::string& state) {
    getTLS(tlsID).setStateInstantiatingOnline(MSNet::getInstance()->getTLSControl(), state);
}


void
TraCI_TLS::setPhase(const std::string& tlsID, const int index) {
    const SUMOTime cTime = MSNet::getInstance()->getCurrentTimeStep();
    const SUMOTime duration = getTLS(tlsID).getActive()->getPhase(index).duration;
    getTLS(tlsID).getActive()->changeStepAndDuration(MSNet::getInstance()->getTLSControl(), cTime, index, duration);
}



void
TraCI_TLS::setProgram(const std::string& tlsID, const std::string& programID) {
    getTLS(tlsID).switchTo(MSNet::getInstance()->getTLSControl(), programID);
}



void
TraCI_TLS::setPhaseDuration(const std::string& tlsID, const SUMOTime phaseDuration) {
    const SUMOTime cTime = MSNet::getInstance()->getCurrentTimeStep();
    const int index = getTLS(tlsID).getActive()->getCurrentPhaseIndex();
    getTLS(tlsID).getActive()->changeStepAndDuration(MSNet::getInstance()->getTLSControl(), cTime, index, phaseDuration);
}



void
TraCI_TLS::setCompleteRedYellowGreenDefinition(const std::string& tlsID, const TraCILogic& logic) {
    std::vector<MSPhaseDefinition*> phases;
/*    MSPhaseDefinition* phase = new MSPhaseDefinition(duration, minDuration, maxDuration, state);
    phases.push_back(phase);
    if (vars.getLogic(subid) == 0) {
        MSTrafficLightLogic* logic = new MSSimpleTrafficLightLogic(tlsControl, id, subid, phases, index, 0, std::map<std::string, std::string>());
        vars.addLogic(subid, logic, true, true);
    } else {
        static_cast<MSSimpleTrafficLightLogic*>(vars.getLogic(subid))->setPhases(phases, index);
    }*/
}


MSTLLogicControl::TLSLogicVariants&
TraCI_TLS::getTLS(const std::string& id) {
    if (!MSNet::getInstance()->getTLSControl().knows(id)) {
        throw TraCIException("Traffic light '" + id + "' is not known");
    }
    return MSNet::getInstance()->getTLSControl().get(id);
}


/****************************************************************************/
