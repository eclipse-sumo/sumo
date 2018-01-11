/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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

