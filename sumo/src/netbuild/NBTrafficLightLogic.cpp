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
// Revision 1.9  2003/06/18 11:30:26  dkrajzew
// debug outputs now use a DEBUG_OUT macro instead of cout; this shall ease the search for further couts which must be redirected to the messaaging subsystem
//
// Revision 1.8  2003/06/05 11:43:36  dkrajzew
// class templates applied; documentation added
//
// Revision 1.7  2003/05/21 15:18:19  dkrajzew
// yellow traffic lights implemented
//
// Revision 1.6  2003/04/07 12:15:44  dkrajzew
// first steps towards a junctions geometry; tyellow removed again, traffic lights have yellow times given explicitely, now
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
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include <bitset>
#include <utility>
#include <iostream>
#include <string>
#include <sstream>
#include "NBEdge.h"
#include "NBTrafficLightLogic.h"


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
NBTrafficLightLogic::writeXML(ostream &into, size_t no,
                              const NBConnectionVector &inLinks) const
{
    into << "   <tl-logic type=\"static\">" << endl;
    into << "      <key>" << _key << "</key>" << endl;
    into << "      <logicno>" << no << "</logicno>" << endl;
    into << "      <phaseno>" << _phases.size() << "</phaseno>" << endl;
/*
    // write the inlanes
    into << "      <inlinks>";
    bool first = true;
    for(NBConnectionVector::const_iterator j=inLinks.begin(); j!=inLinks.end(); j++) {
        if(!first) {
            into << " ";
        }
        first = false;
        into << (*j).getID();
    }
    into << "</inlinks>" << endl;
    */
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




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBTrafficLightLogic.icc"
//#endif

// Local Variables:
// mode:C++
// End:


