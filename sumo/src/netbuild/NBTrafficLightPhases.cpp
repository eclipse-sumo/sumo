//---------------------------------------------------------------------------//
//                        NBTrafficLightPhases.cpp -
//  A container for traffic light phases
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
// Revision 1.3  2003/03/03 14:59:22  dkrajzew
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

#include <cassert>
#include <vector>
#include <algorithm>
#include <iostream>
#include <bitset>
#include "NBTrafficLightLogic.h"
#include "NBTrafficLightLogicVector.h"
#include "NBRequestEdgeLinkIterator.h"
#include "NBTrafficLightPhases.h"

using namespace std;

NBTrafficLightPhases::NBTrafficLightPhases(
    const NBLinkCliqueContainer &cliques, size_t noCliques)
    : _phasesVectorsByLength(noCliques, PhasesVector()),
    _cliques(cliques), _noPhaseVectors(0)
{
}


NBTrafficLightPhases::~NBTrafficLightPhases()
{
}


void
NBTrafficLightPhases::add(const PhaseIndexVector &phase)
{
    // check whether the given phasevector contains one of the already
    //  added phases in full
    //  do this for smaller vectors only
    size_t size = phase.size();
    size_t i;
    for(i=0; i<size&&i<_phasesVectorsByLength.size(); i++) {
        assert(i<_phasesVectorsByLength.size());
        PhasesVector::iterator j = find_if(
            _phasesVectorsByLength[i].begin(),
            _phasesVectorsByLength[i].end(),
            shorter_included_finder(phase));
        // shorter fully included by current was found;
        //  return without adding
        if(j!=_phasesVectorsByLength[i].end()) {
            return;
        }
    }
    // check whether the given phasevector is inside one of the larger
    //  already added phases
    for(i=size+1; i<_phasesVectorsByLength.size(); i++) {
        assert(i<_phasesVectorsByLength.size());
        PhasesVector::iterator j = find_if(
            _phasesVectorsByLength[i].begin(),
            _phasesVectorsByLength[i].end(),
            larger_included_finder(phase));
        if(j!=_phasesVectorsByLength[i].end()) {
            _phasesVectorsByLength[i].erase(j);
        }
    }
    // append empty vectors when needed
    while(_phasesVectorsByLength.size()<phase.size()+1)  {
        _phasesVectorsByLength.push_back(PhasesVector());
    }
    // add the current phase to the list
    assert(phase.size()<_phasesVectorsByLength.size());
    _phasesVectorsByLength[phase.size()].push_back(phase);
    _noPhaseVectors += 1;
}


void
NBTrafficLightPhases::add(const NBTrafficLightPhases &phases,
                          bool skipLarger)
{
    size_t size = _phasesVectorsByLength.size();
    size_t i;
    for(i=0; i<size&&i<phases._phasesVectorsByLength.size(); i++) {
        for(size_t j=0; j<phases._phasesVectorsByLength[i].size(); j++) {
            add(phases._phasesVectorsByLength[i][j]);
        }
    }
    if(skipLarger) {
        return;
    }
    for(; i<phases._phasesVectorsByLength.size(); i++) {
        for(size_t j=0; j<phases._phasesVectorsByLength[i].size(); j++) {
            add(phases._phasesVectorsByLength[i][j]);
        }
    }
}


std::ostream &operator<<(std::ostream &os, const NBTrafficLightPhases &p)
{
	os << "Folgen:" << endl;
    for(NBTrafficLightPhases::PhasesVectorVector::const_iterator
            i=p._phasesVectorsByLength.begin();
            i!=p._phasesVectorsByLength.end(); i++) {
        for(NBTrafficLightPhases::PhasesVector::const_iterator
                j=(*i).begin();
                j!=(*i).end();
                j++) {
		    PhaseIndexVector tmp = (*j);
		    for(PhaseIndexVector::const_iterator k=tmp.begin(); k!=tmp.end(); k++) {
			    os << (*k) << ", ";
		    }
		    os << endl;
        }
	}
	os << "--------------------------------" << endl;
    return os;
}


NBTrafficLightLogicVector *
NBTrafficLightPhases::computeLogics(const std::string &key,
                                    size_t noLinks,
                                    const NBRequestEdgeLinkIterator &cei1,
                                    const EdgeVector &inLanes) const
{
    NBTrafficLightLogicVector *ret = new NBTrafficLightLogicVector(inLanes);
    for(size_t i=0; i<_phasesVectorsByLength.size(); i++) {
        for(size_t j=0; j<_phasesVectorsByLength[i].size(); j++) {
            ret->add(
                buildTrafficLightsLogic(
                    key, noLinks, _phasesVectorsByLength[i][j], cei1));
        }
    }
    return ret;
}




NBTrafficLightLogic *
NBTrafficLightPhases::buildTrafficLightsLogic(const std::string &key,
                                              size_t noLinks,
                                              const PhaseIndexVector &phaseList,
                                              const NBRequestEdgeLinkIterator &cei1) const
{
    NBTrafficLightLogic *ret = 
        new NBTrafficLightLogic(key, noLinks);
    for(size_t i=0; i<phaseList.size(); i++) {
        // add the complete phase
        std::bitset<64> driveMask;
        std::bitset<64> brakeMask;
        size_t pos = 0;
        for(size_t j=0; j<cei1.getNoValidLinks(); j++) {
            size_t noEdges = cei1.getNumberOfAssignedLinks(j);
            for(size_t k=0; k<noEdges; k++) {
                assert(i<phaseList.size());
                driveMask.set(pos, _cliques.test(phaseList[i], j));
                brakeMask.set(pos,
                    cei1.testBrakeMask(_cliques.test(phaseList[i], j), pos));
                pos++;
            }
        }
        size_t duration = 30;
        ret->addStep(duration, driveMask, brakeMask);
        // add possible additional left-turn phase when existing
        cei1.resetNonLeftMovers(driveMask, brakeMask);
        if(driveMask.any()) {
            duration = 10;
            ret->addStep(duration, driveMask, brakeMask);
        }
    }
#ifdef TL_DEBUG
    cout << "Phasenfolge (Ende):" << endl;
    ret->_debugWritePhases();
    cout << "----------------------------------" << endl;
#endif
    return ret;
}





/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBTrafficLightPhases.icc"
//#endif

// Local Variables:
// mode:C++
// End:


