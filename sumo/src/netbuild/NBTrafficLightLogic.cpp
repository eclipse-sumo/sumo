//---------------------------------------------------------------------------//
//                        NBTrafficLightLogic.cpp -  ccc
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
                             std::bitset<64> brakeMask)
{
    _phases.push_back(PhaseDefinition(duration, driveMask, brakeMask));
}


void
NBTrafficLightLogic::writeXML(ostream &into, size_t no,
                              const EdgeVector &inLanes) const
{
    into << "   <tl-logic type=\"static\">" << endl;
    into << "      <key>" << _key << "</key>" << endl;
    into << "      <logicno>" << no << "</logicno>" << endl;
    into << "      <phaseno>" << _phases.size() << "</phaseno>" << endl;
    // write the inlanes
    into << "      <inlanes>";
    bool first = true;
    for(EdgeVector::const_iterator j=inLanes.begin(); j!=inLanes.end(); j++) {
        size_t noLanes = (*j)->getNoLanes();
        for(size_t k=0; k<noLanes; k++) {
            if(!first) {
                into << " ";
            }
            first = false;
            into << (*j)->getID() << "_" << k;
        }
    }
    into << "</inlanes>" << endl;
    // wrte the phases
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
        into << " brake=\"" << tmp2.str().substr(64-_noLinks) << "\"/>"
            << endl;
    }
    into << "   </tl-logic>" << endl << endl;
}


void
NBTrafficLightLogic::_debugWritePhases() const
{
    for( PhaseDefinitionVector::const_iterator i=_phases.begin();
         i!=_phases.end(); i++) {
        cout << (*i).duration << "s : " << (*i).driveMask << endl;
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


