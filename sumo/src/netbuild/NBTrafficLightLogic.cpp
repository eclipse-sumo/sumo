/****************************************************************************/
/// @file    NBTrafficLightLogic.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A SUMO-compliant built logic for a traffic light
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
NBTrafficLightLogic::NBTrafficLightLogic(const std::string &id,
        const std::string &subid, const std::string &type,
        unsigned int noLinks) throw()
        : Named(id), myNoLinks(noLinks), mySubID(subid), myType(type),
        myOffset(0)
{}


NBTrafficLightLogic::~NBTrafficLightLogic() throw()
{}


void
NBTrafficLightLogic::addStep(SUMOTime duration,
                             std::bitset<64> driveMask,
                             std::bitset<64> brakeMask,
                             std::bitset<64> yellowMask) throw()
{
    myPhases.push_back(PhaseDefinition(duration, driveMask, brakeMask, yellowMask));
}


void
NBTrafficLightLogic::writeXML(OutputDevice &into, size_t no, SUMOReal /*distance*/,
                              std::string type,
                              const std::set<string> &/*inLanes*/) const throw()
{
    into << "   <tl-logic type=\"" << type << "\">\n";
    into << "      <key>" << getID() << "</key>\n";
    into << "      <subkey>" << no << "</subkey>\n";
    into << "      <phaseno>" << myPhases.size() << "</phaseno>\n";
    into << "      <offset>" << myOffset << "</offset>\n";
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
NBTrafficLightLogic::equals(const NBTrafficLightLogic &logic) const throw()
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


SUMOTime
NBTrafficLightLogic::getDuration() const throw()
{
    SUMOTime duration = 0;
    for (PhaseDefinitionVector::const_iterator i=myPhases.begin(); i!=myPhases.end(); ++i) {
        duration += (*i).duration;
    }
    return duration;
}


void
NBTrafficLightLogic::closeBuilding() throw()
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



SUMOTime
NBTrafficLightLogic::computeOffsetFor(SUMOReal offsetMult) const throw()
{
    SUMOTime dur = 0;
    for (size_t i=0; i<myPhases.size(); ++i) {
        dur += myPhases[i].duration;
    }
    return (SUMOTime)((SUMOReal) dur * offsetMult);
}



/****************************************************************************/

