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
#include <iostream>
#include <string>
#include <sstream>
#include <cassert>
#include "NBEdge.h"
#include "NBEdgeCont.h"
#include "NBTrafficLightLogic.h"
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/common/StringTokenizer.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// some definitions (debugging only)
// ===========================================================================
#define DEBUG_OUT cout


// ===========================================================================
// member method definitions
// ===========================================================================
NBTrafficLightLogic::NBTrafficLightLogic(const std::string &key,
        size_t noLinks)
        : _key(key), _noLinks(noLinks)
{}


NBTrafficLightLogic::NBTrafficLightLogic(const NBTrafficLightLogic &s)
        : _key(s._key), _noLinks(s._noLinks), _phases(s._phases)
{}


NBTrafficLightLogic::~NBTrafficLightLogic()
{}


void
NBTrafficLightLogic::addStep(size_t duration,
                             std::bitset<64> driveMask,
                             std::bitset<64> brakeMask,
                             std::bitset<64> yellowMask)
{
    _phases.push_back(PhaseDefinition(duration, driveMask, brakeMask, yellowMask));
}


void
NBTrafficLightLogic::writeXML(ostream &into, size_t no, SUMOReal /*distance*/,
                              std::string type,
                              const std::set<string> &/*inLanes*/) const
    {
        into << "   <tl-logic type=\"" << type << "\">" << endl;
        into << "      <key>" << _key << "</key>" << endl;
        into << "      <subkey>" << no << "</subkey>" << endl;
        into << "      <phaseno>" << _phases.size() << "</phaseno>" << endl;
        int offset = getOffset();
        into << "      <offset>" << offset << "</offset>" << endl;
        // write the inlanes
        /*
        std::set<string>::const_iterator j;
        into << "      <inclanes>";
        //    bool first = true;
        for(j=inLanes.begin(); j!=inLanes.end(); j++) {
            if(j!=inLanes.begin()) {
                into << " ";
            }
        //        first = false;
            into << (*j);
        }
        into << "</inclanes>" << endl;
        */
        // write the phases
        for (PhaseDefinitionVector::const_iterator i=_phases.begin();
                i!=_phases.end(); i++) {
            std::bitset<64> mask = (*i).driveMask;
            stringstream tmp1;
            tmp1 << mask;
            into << "      <phase duration=\"" << (*i).duration
            << "\" phase=\"" << tmp1.str().substr(64-_noLinks) << "\"";
            // by now, only the vehicles that are not allowed to drive are
            //  breaking; later the right-arrow - rule should be concerned
            stringstream tmp2;
            mask = (*i).brakeMask;
            tmp2 << mask;
            into << " brake=\"" << tmp2.str().substr(64-_noLinks) << "\"";
            // write the information which link have a yellow light
            stringstream tmp3;
            mask = (*i).yellowMask;
            tmp3 << mask;
            into << " yellow=\"" << tmp3.str().substr(64-_noLinks) << "\"";
            // close phase information
            into << "/>" << endl;
        }
        into << "   </tl-logic>" << endl << endl;
    }


void
NBTrafficLightLogic::_debugWritePhases() const
{
    for (PhaseDefinitionVector::const_iterator i=_phases.begin();
            i!=_phases.end(); i++) {
        DEBUG_OUT << (*i).duration << "s : " << (*i).driveMask << endl;
    }
}


bool
NBTrafficLightLogic::equals(const NBTrafficLightLogic &logic) const
{
    if (_phases.size()!=logic._phases.size()) {
        return false;
    }
    PhaseDefinitionVector::const_iterator i, j;
    for (i=_phases.begin(), j=logic._phases.begin();
            i!=_phases.end(); i++, j++) {
        if ((*i)!=(*j)) {
            return false;
        }
    }
    return true;
}


void
NBTrafficLightLogic::closeBuilding()
{
    for (size_t i=0; i<_phases.size()-1;) {
        if (_phases[i].driveMask!=_phases[i+1].driveMask
                ||
                _phases[i].brakeMask!=_phases[i+1].brakeMask
                ||
                _phases[i].yellowMask!=_phases[i+1].yellowMask) {

            i++;
            continue;
        }

        _phases[i].duration += _phases[i+1].duration;
        _phases.erase(_phases.begin()+i+1);
    }
}


size_t
NBTrafficLightLogic::getOffset() const
{
    // check whether any offsets shall be manipulated by setting
    //  them to half of the duration
    if (OptionsSubSys::getOptions().isSet("tl-logics.half-offset")) {
        if (checkOffsetFor("tl-logics.half-offset")) {
            return computeOffsetFor(0.5);
        }
    }
    // check whether any offsets shall be manipulated by setting
    //  them to half of the duration
    if (OptionsSubSys::getOptions().isSet("tl-logics.quarter-offset")) {
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
        OptionsSubSys::getOptions().getString(optionName);
    StringTokenizer st(offsets, ";");
    while (st.hasNext()) {
        string key = st.next();
        if (key==_key) {
            return true;
        }
    }
    return false;
}


size_t
NBTrafficLightLogic::computeOffsetFor(SUMOReal offsetMult) const
{
    size_t dur = 0;
    for (size_t i=0; i<_phases.size(); ++i) {
        dur += _phases[i].duration;
    }
    return (size_t)((SUMOReal) dur * offsetMult);
}



/****************************************************************************/

