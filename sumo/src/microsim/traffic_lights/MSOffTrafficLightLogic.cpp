/****************************************************************************/
/// @file    MSOffTrafficLightLogic.cpp
/// @author  Daniel Krajzewicz
/// @date    08.05.2007
/// @version $Id$
///
// A traffic lights logic which represents a tls in an off-mode
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include <cassert>
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
MSOffTrafficLightLogic::MSOffTrafficLightLogic(MSTLLogicControl &tlcontrol,
        const std::string &id) throw()
        : MSTrafficLightLogic(tlcontrol, id, "off", 0) {
    myDefaultCycleTime = 120;
}


MSOffTrafficLightLogic::~MSOffTrafficLightLogic() throw() {
    for (MSTrafficLightLogic::Phases::const_iterator i=myPhaseDefinition.begin(); i!=myPhaseDefinition.end(); ++i) {
        delete *i;
    }
}


void
MSOffTrafficLightLogic::init(NLDetectorBuilder &nb) throw(ProcessError) {
    rebuildPhase();
}


// ----------- Handling of controlled links
void
MSOffTrafficLightLogic::adaptLinkInformationFrom(const MSTrafficLightLogic &logic) throw() {
    MSTrafficLightLogic::adaptLinkInformationFrom(logic);
    rebuildPhase();
}


void
MSOffTrafficLightLogic::rebuildPhase() throw() {
    size_t no = getLinks().size();
    std::string state;
    for (unsigned int i=0; i<no; ++i) {
        // !!! no brake mask!
        state += 'o';
    }
    for (MSTrafficLightLogic::Phases::const_iterator i=myPhaseDefinition.begin(); i!=myPhaseDefinition.end(); ++i) {
        delete *i;
    }
    myPhaseDefinition.clear();
    myPhaseDefinition.push_back(new MSPhaseDefinition(1, state));
}


// ------------ Static Information Retrieval
unsigned int
MSOffTrafficLightLogic::getPhaseNumber() const throw() {
    return 0;
}


const MSOffTrafficLightLogic::Phases &
MSOffTrafficLightLogic::getPhases() const throw() {
    return myPhaseDefinition;
}


const MSPhaseDefinition &
MSOffTrafficLightLogic::getPhase(unsigned int givenstep) const throw() {
    return *myPhaseDefinition[0];
}


// ------------ Dynamic Information Retrieval
unsigned int
MSOffTrafficLightLogic::getCurrentPhaseIndex() const throw() {
    return 0;
}


const MSPhaseDefinition &
MSOffTrafficLightLogic::getCurrentPhaseDef() const throw() {
    return *myPhaseDefinition[0];
}


// ------------ Conversion between time and phase
unsigned int
MSOffTrafficLightLogic::getPhaseIndexAtTime(SUMOTime simStep) const throw() {
    return 0;
}


unsigned int
MSOffTrafficLightLogic::getOffsetFromIndex(unsigned int index) const throw() {
    return 0;
}


unsigned int
MSOffTrafficLightLogic::getIndexFromOffset(unsigned int offset) const throw() {
    return 0;
}



/****************************************************************************/

