/****************************************************************************/
/// @file    NBTrafficLightLogic.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A single traffic light logic (a possible variant)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
NBTrafficLightLogic::NBTrafficLightLogic(const std::string &key,
        size_t noLinks)
        : myKey(key), myNoLinks(noLinks)
{}


NBTrafficLightLogic::NBTrafficLightLogic(const NBTrafficLightLogic &s)
        : myKey(s.myKey), myNoLinks(s.myNoLinks), myPhases(s.myPhases)
{}


NBTrafficLightLogic::~NBTrafficLightLogic()
{}


void
NBTrafficLightLogic::addStep(size_t duration,
                             std::bitset<64> driveMask,
                             std::bitset<64> brakeMask,
                             std::bitset<64> yellowMask)
{
    myPhases.push_back(PhaseDefinition(duration, driveMask, brakeMask, yellowMask));
}


void
NBTrafficLightLogic::writeXML(OutputDevice &into, size_t no, SUMOReal /*distance*/,
                              std::string type,
                              const std::set<string> &/*inLanes*/) const
{
    into << "   <tl-logic type=\"" << type << "\">\n";
    into << "      <key>" << myKey << "</key>\n";
    into << "      <subkey>" << no << "</subkey>\n";
    into << "      <phaseno>" << myPhases.size() << "</phaseno>\n";
    int offset = getOffset();
    into << "      <offset>" << offset << "</offset>\n";
    // write the phases
    for (PhaseDefinitionVector::const_iterator i=myPhases.begin(); i!=myPhases.end(); i++) {
        std::bitset<64> mask = (*i).driveMask;
        stringstream tmp1;
        tmp1 << mask;
        into << "      <phase duration=\"" << (*i).duration
        << "\" phase=\"" << tmp1.str().substr(64-myNoLinks) << "\"";
        // by now, only the vehicles that are not allowed to drive are
        //  breaking; later the right-arrow - rule should be concerned
        stringstream tmp2;
        mask = (*i).brakeMask;
        tmp2 << mask;
        into << " brake=\"" << tmp2.str().substr(64-myNoLinks) << "\"";
        // write the information which link have a yellow light
        stringstream tmp3;
        mask = (*i).yellowMask;
        tmp3 << mask;
        into << " yellow=\"" << tmp3.str().substr(64-myNoLinks) << "\"";
        // close phase information
        into << "/>\n";
    }
    into << "   </tl-logic>\n\n";
}


bool
NBTrafficLightLogic::equals(const NBTrafficLightLogic &logic) const
{
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


void
NBTrafficLightLogic::closeBuilding()
{
    for (size_t i=0; i<myPhases.size()-1;) {
        if (myPhases[i].driveMask!=myPhases[i+1].driveMask
                ||
                myPhases[i].brakeMask!=myPhases[i+1].brakeMask
                ||
                myPhases[i].yellowMask!=myPhases[i+1].yellowMask) {

            i++;
            continue;
        }

        myPhases[i].duration += myPhases[i+1].duration;
        myPhases.erase(myPhases.begin()+i+1);
    }
}


size_t
NBTrafficLightLogic::getOffset() const
{
    // check whether any offsets shall be manipulated by setting
    //  them to half of the duration
    if (OptionsCont::getOptions().isSet("tl-logics.half-offset")) {
        if (checkOffsetFor("tl-logics.half-offset")) {
            return computeOffsetFor(0.5);
        }
    }
    // check whether any offsets shall be manipulated by setting
    //  them to half of the duration
    if (OptionsCont::getOptions().isSet("tl-logics.quarter-offset")) {
        if (checkOffsetFor("tl-logics.quarter-offset")) {
            return computeOffsetFor(0.25);
        }
    }
    // The key was not found within the offsets to change
    //  or nothing shall be changed
    return 0;
}


bool
NBTrafficLightLogic::checkOffsetFor(const std::string &optionName) const
{
    string offsets =
        OptionsCont::getOptions().getString(optionName);
    StringTokenizer st(offsets, ";");
    while (st.hasNext()) {
        string key = st.next();
        if (key==myKey) {
            return true;
        }
    }
    return false;
}


size_t
NBTrafficLightLogic::computeOffsetFor(SUMOReal offsetMult) const
{
    size_t dur = 0;
    for (size_t i=0; i<myPhases.size(); ++i) {
        dur += myPhases[i].duration;
    }
    return (size_t)((SUMOReal) dur * offsetMult);
}



/****************************************************************************/

