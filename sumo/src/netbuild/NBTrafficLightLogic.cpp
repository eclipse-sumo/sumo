/****************************************************************************/
/// @file    NBTrafficLightLogic.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A SUMO-compliant built logic for a traffic light
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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

#include <vector>
#include <bitset>
#include <utility>
#include <string>
#include <sstream>
#include <cassert>
#include "NBEdge.h"
#include "NBEdgeCont.h"
#include "NBTrafficLightLogic.h"
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/common/ToString.h>
#include <utils/common/StringTokenizer.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
NBTrafficLightLogic::NBTrafficLightLogic(const std::string &id,
        const std::string &subid, unsigned int noLinks) throw() :
        Named(id), myNumLinks(noLinks), mySubID(subid),
        myOffset(0) {}

NBTrafficLightLogic::NBTrafficLightLogic(const NBTrafficLightLogic* logic) :
        Named(logic->getID()),
        myNumLinks(logic->myNumLinks),
        mySubID(logic->getProgramID()),
        myOffset(logic->getOffset()),
        myPhases(logic->myPhases.begin(), logic->myPhases.end()) {}


NBTrafficLightLogic::~NBTrafficLightLogic() throw() {}


void
NBTrafficLightLogic::addStep(SUMOTime duration, const std::string &state) {
    if (myNumLinks == 0) {
        // initialize
        myNumLinks = (unsigned int)state.size();
    } else if (state.size() != myNumLinks) {
        throw ProcessError("When adding phase: state length of " + toString(state.size()) +
                           " does not match declared number of links " + toString(myNumLinks));
    }
    myPhases.push_back(PhaseDefinition(duration, state));
}


SUMOTime
NBTrafficLightLogic::getDuration() const throw() {
    SUMOTime duration = 0;
    for (PhaseDefinitionVector::const_iterator i=myPhases.begin(); i!=myPhases.end(); ++i) {
        duration += (*i).duration;
    }
    return duration;
}


void
NBTrafficLightLogic::closeBuilding() throw() {
    for (unsigned int i=0; i<myPhases.size()-1;) {
        if (myPhases[i].state!=myPhases[i+1].state) {
            ++i;
            continue;
        }
        myPhases[i].duration += myPhases[i+1].duration;
        myPhases.erase(myPhases.begin()+i+1);
    }
}



/****************************************************************************/

