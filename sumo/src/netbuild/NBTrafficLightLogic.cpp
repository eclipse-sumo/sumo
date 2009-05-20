/****************************************************************************/
/// @file    NBTrafficLightLogic.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A SUMO-compliant built logic for a traffic light
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
#include <utils/common/StringTokenizer.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member method definitions
// ===========================================================================
NBTrafficLightLogic::NBTrafficLightLogic(const std::string &id,
        const std::string &subid, unsigned int noLinks) throw()
        : Named(id), myNoLinks(noLinks), mySubID(subid), 
        myOffset(0) {}


NBTrafficLightLogic::~NBTrafficLightLogic() throw() {}


void
NBTrafficLightLogic::addStep(SUMOTime duration, const std::string &state) throw() {
    myPhases.push_back(PhaseDefinition(duration, state));
}


void
NBTrafficLightLogic::writeXML(OutputDevice &into) const throw() {
    into << "   <tl-logic type=\"static\">\n";
    into << "      <key>" << getID() << "</key>\n";
    into << "      <subkey>" << mySubID << "</subkey>\n";
    into << "      <phaseno>" << myPhases.size() << "</phaseno>\n";
    into << "      <offset>" << myOffset << "</offset>\n";
    // write the phases
    for (PhaseDefinitionVector::const_iterator i=myPhases.begin(); i!=myPhases.end(); i++) {
        into << "      <phase duration=\"" << (*i).duration << "\" state=\"" << (*i).state << "\"/>\n";
    }
    into << "   </tl-logic>\n\n";
}


bool
NBTrafficLightLogic::equals(const NBTrafficLightLogic &logic) const throw() {
    if (myPhases.size()!=logic.myPhases.size()) {
        return false;
    }
    PhaseDefinitionVector::const_iterator i, j;
    for (i=myPhases.begin(), j=logic.myPhases.begin();
            i!=myPhases.end(); i++, j++) {
        if ((*i)!=(*j)) {
            return false;
        }
    }
    return true;
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



SUMOTime
NBTrafficLightLogic::computeOffsetFor(SUMOReal offsetMult) const throw() {
    SUMOTime dur = 0;
    for (size_t i=0; i<myPhases.size(); ++i) {
        dur += myPhases[i].duration;
    }
    return (SUMOTime)((SUMOReal) dur * offsetMult);
}



/****************************************************************************/

