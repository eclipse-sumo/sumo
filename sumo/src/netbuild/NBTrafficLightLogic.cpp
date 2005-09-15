//---------------------------------------------------------------------------//
//                        NBTrafficLightLogic.cpp -
//  A single traffic light logic (a possible variant)
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.18  2005/09/15 12:02:45  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.17  2005/04/27 11:48:26  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.16  2004/04/23 12:41:02  dkrajzew
// some further work on vissim-import
//
// Revision 1.15  2004/01/12 15:10:27  dkrajzew
// more wise definition of lane predeccessors implemented
//
// Revision 1.14  2003/12/04 13:03:58  dkrajzew
// possibility to pass the tl-type from the netgenerator added
//
// Revision 1.13  2003/11/17 07:26:02  dkrajzew
// computations needed for collecting e2-values over multiple lanes added
//
// Revision 1.12  2003/09/25 09:02:51  dkrajzew
// multiple lane in tl-logic - bug patched
//
// Revision 1.11  2003/09/24 09:56:07  dkrajzew
// inlanes added to a traffic light logic description
//
// Revision 1.10  2003/07/30 09:21:11  dkrajzew
// added the generation about link directions and priority
//
// Revision 1.9  2003/06/18 11:30:26  dkrajzew
// debug outputs now use a DEBUG_OUT macro instead of cout; this shall ease
//  the search for further couts which must be redirected to the messaging
//  subsystem
//
// Revision 1.8  2003/06/05 11:43:36  dkrajzew
// class templates applied; documentation added
//
// Revision 1.7  2003/05/21 15:18:19  dkrajzew
// yellow traffic lights implemented
//
// Revision 1.6  2003/04/07 12:15:44  dkrajzew
// first steps towards a junctions geometry; tyellow removed again,
//  traffic lights have yellow times given explicitely, now
//
// Revision 1.5  2003/04/01 15:15:21  dkrajzew
// some documentation added
//
// Revision 1.4  2003/03/20 16:23:10  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.3  2003/03/03 14:59:19  dkrajzew
// debugging; handling of imported traffic light definitions
//
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

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

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * some definitions (debugging only)
 * ======================================================================= */
#define DEBUG_OUT cout


/* =========================================================================
 * member method definitions
 * ======================================================================= */
NBTrafficLightLogic::NBTrafficLightLogic(const std::string &key,
                                         size_t noLinks)
    : _key(key), _noLinks(noLinks)
{
}


NBTrafficLightLogic::NBTrafficLightLogic(const NBTrafficLightLogic &s)
    : _key(s._key), _noLinks(s._noLinks), _phases(s._phases)
{
}


NBTrafficLightLogic::~NBTrafficLightLogic()
{
}


void
NBTrafficLightLogic::addStep(size_t duration,
                             std::bitset<64> driveMask,
                             std::bitset<64> brakeMask,
							 std::bitset<64> yellowMask)
{
    _phases.push_back(PhaseDefinition(duration, driveMask, brakeMask, yellowMask));
}


void
NBTrafficLightLogic::writeXML(ostream &into, size_t no, double distance,
                              std::string type,
                              const std::set<string> &inLanes) const
{
    into << "   <tl-logic type=\"" << type << "\">" << endl;
    into << "      <key>" << _key << "</key>" << endl;
    into << "      <logicno>" << no << "</logicno>" << endl;
    into << "      <phaseno>" << _phases.size() << "</phaseno>" << endl;
    int offset = getOffset();
    into << "      <offset>" << offset << "</offset>" << endl;
    // write the inlanes
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
    // write the phases
    for( PhaseDefinitionVector::const_iterator i=_phases.begin();
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
    for( PhaseDefinitionVector::const_iterator i=_phases.begin();
         i!=_phases.end(); i++) {
        DEBUG_OUT << (*i).duration << "s : " << (*i).driveMask << endl;
    }
}


bool
NBTrafficLightLogic::equals(const NBTrafficLightLogic &logic) const
{
    if(_phases.size()!=logic._phases.size()) {
        return false;
    }
    PhaseDefinitionVector::const_iterator i, j;
    for( i=_phases.begin(), j=logic._phases.begin();
         i!=_phases.end(); i++, j++) {
        if((*i)!=(*j)) {
            return false;
        }
    }
    return true;
}


void
NBTrafficLightLogic::closeBuilding()
{
    for(size_t i=0; i<_phases.size()-1; ) {
        if( _phases[i].driveMask!=_phases[i+1].driveMask
            ||
            _phases[i].brakeMask!=_phases[i+1].brakeMask
            ||
            _phases[i].yellowMask!=_phases[i+1].yellowMask ) {

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
    if(OptionsSubSys::getOptions().isSet("tl-logics.half-offset")) {
        if(checkOffsetFor("tl-logics.half-offset")) {
            return computeOffsetFor(0.5);
        }
    }
    // check whether any offsets shall be manipulated by setting
    //  them to half of the duration
    if(OptionsSubSys::getOptions().isSet("tl-logics.quarter-offset")) {
        if(checkOffsetFor("tl-logics.quarter-offset")) {
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
    while(st.hasNext()) {
        string key = st.next();
        if(key==_key) {
            return true;
        }
    }
    return false;
}


size_t
NBTrafficLightLogic::computeOffsetFor(double offsetMult) const
{
    size_t dur = 0;
    for(size_t i=0; i<_phases.size(); ++i) {
        dur += _phases[i].duration;
    }
    return (size_t) ((double) dur * offsetMult);
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


