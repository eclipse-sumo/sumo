/****************************************************************************/
/// @file    TraCI_TLS.cpp
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    30.05.2012
/// @version $Id: TraCI_TLS.cpp 25277 2017-07-20 11:15:41Z behrisch $
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
    /*
    tempMsg.writeUnsignedByte(TYPE_COMPOUND);
    tcpip::Storage tempContent;
    int cnt = 0;
    tempContent.writeUnsignedByte(TYPE_INTEGER);
    int no = (int)lanes.size();
    tempContent.writeInt((int)no);
    for (int i = 0; i < no; ++i) {
        const MSTrafficLightLogic::LaneVector& llanes = lanes[i];
        const MSTrafficLightLogic::LinkVector& llinks = links[i];
        // number of links controlled by this signal (signal i)
        tempContent.writeUnsignedByte(TYPE_INTEGER);
        int no2 = (int)llanes.size();
        tempContent.writeInt((int)no2);
        ++cnt;
        for (int j = 0; j < no2; ++j) {
            MSLink* link = llinks[j];
            std::vector<std::string> def;
            // incoming lane
            def.push_back(llanes[j]->getID());
            // approached non-internal lane (if any)
            def.push_back(link->getLane() != 0 ? link->getLane()->getID() : "");
            // approached "via", internal lane (if any)
            def.push_back(link->getViaLane() != 0 ? link->getViaLane()->getID() : "");
            tempContent.writeUnsignedByte(TYPE_STRINGLIST);
            tempContent.writeStringList(def);
            ++cnt;
        }
    }*/
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

}


void
TraCI_TLS::setPhase(const std::string& tlsID, int index) {

}



void
TraCI_TLS::setProgram(const std::string& tlsID, const std::string& programID) {

}



void
TraCI_TLS::setPhaseDuration(const std::string& tlsID, int phaseDuration) {

}



void
TraCI_TLS::setCompleteRedYellowGreenDefinition(const std::string& tlsID, const TraCILogic& logic) {

}


MSTLLogicControl::TLSLogicVariants&
TraCI_TLS::getTLS(const std::string& id) {
    if (!MSNet::getInstance()->getTLSControl().knows(id)) {
        throw TraCIException("Traffic light '" + id + "' is not known");
    }
    return MSNet::getInstance()->getTLSControl().get(id);
}


/****************************************************************************/
