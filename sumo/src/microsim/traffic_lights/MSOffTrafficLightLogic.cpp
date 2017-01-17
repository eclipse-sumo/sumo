/****************************************************************************/
/// @file    MSOffTrafficLightLogic.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    08.05.2007
/// @version $Id$
///
// A traffic lights logic which represents a tls in an off-mode
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

#include <utility>
#include <vector>
#include <bitset>
#include <sstream>
#include <microsim/MSEventControl.h>
#include "MSTrafficLightLogic.h"
#include "MSOffTrafficLightLogic.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
MSOffTrafficLightLogic::MSOffTrafficLightLogic(MSTLLogicControl& tlcontrol,
        const std::string& id) :
    MSTrafficLightLogic(tlcontrol, id, "off", 0, std::map<std::string, std::string>()) {
    myDefaultCycleTime = TIME2STEPS(120);
}


MSOffTrafficLightLogic::~MSOffTrafficLightLogic() {
    for (MSTrafficLightLogic::Phases::const_iterator i = myPhaseDefinition.begin(); i != myPhaseDefinition.end(); ++i) {
        delete *i;
    }
}


void
MSOffTrafficLightLogic::init(NLDetectorBuilder&) {
    rebuildPhase();
}


// ----------- Handling of controlled links
void
MSOffTrafficLightLogic::adaptLinkInformationFrom(const MSTrafficLightLogic& logic) {
    MSTrafficLightLogic::adaptLinkInformationFrom(logic);
    rebuildPhase();
}


void
MSOffTrafficLightLogic::rebuildPhase() {
    int no = (int)getLinks().size();
    std::string state;
    for (int i = 0; i < no; ++i) {
        // !!! no brake mask!
        state += 'o';
    }
    for (MSTrafficLightLogic::Phases::const_iterator i = myPhaseDefinition.begin(); i != myPhaseDefinition.end(); ++i) {
        delete *i;
    }
    myPhaseDefinition.clear();
    myPhaseDefinition.push_back(new MSPhaseDefinition(TIME2STEPS(120), state));
}


// ------------ Static Information Retrieval
int
MSOffTrafficLightLogic::getPhaseNumber() const {
    return 0;
}


const MSOffTrafficLightLogic::Phases&
MSOffTrafficLightLogic::getPhases() const {
    return myPhaseDefinition;
}


const MSPhaseDefinition&
MSOffTrafficLightLogic::getPhase(int) const {
    return *myPhaseDefinition[0];
}


// ------------ Dynamic Information Retrieval
int
MSOffTrafficLightLogic::getCurrentPhaseIndex() const {
    return 0;
}


const MSPhaseDefinition&
MSOffTrafficLightLogic::getCurrentPhaseDef() const {
    return *myPhaseDefinition[0];
}


// ------------ Conversion between time and phase
SUMOTime
MSOffTrafficLightLogic::getPhaseIndexAtTime(SUMOTime) const {
    return 0;
}


SUMOTime
MSOffTrafficLightLogic::getOffsetFromIndex(int) const {
    return 0;
}


int
MSOffTrafficLightLogic::getIndexFromOffset(SUMOTime) const {
    return 0;
}



/****************************************************************************/

