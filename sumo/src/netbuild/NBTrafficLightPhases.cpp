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
// Revision 1.16  2005/04/27 11:48:26  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.15  2003/12/04 13:03:58  dkrajzew
// possibility to pass the tl-type from the netgenerator added
//
// Revision 1.14  2003/07/30 09:21:11  dkrajzew
// added the generation about link directions and priority
//
// Revision 1.13  2003/07/21 11:04:06  dkrajzew
// the default duration of green light phases may now be changed on startup
//
// Revision 1.12  2003/07/07 08:22:42  dkrajzew
// some further refinements due to the new 1:N traffic lights and usage of geometry information
//
// Revision 1.11  2003/06/18 11:30:26  dkrajzew
// debug outputs now use a DEBUG_OUT macro instead of cout; this shall ease the search for further couts which must be redirected to the messaaging subsystem
//
// Revision 1.10  2003/06/05 11:43:36  dkrajzew
// class templates applied; documentation added
//
// Revision 1.9  2003/05/21 15:18:19  dkrajzew
// yellow traffic lights implemented
//
// Revision 1.8  2003/05/20 09:33:48  dkrajzew
// false computation of yielding on lane ends debugged; some debugging on tl-import; further work on vissim-import
//
// Revision 1.7  2003/04/07 12:15:46  dkrajzew
// first steps towards a junctions geometry; tyellow removed again, traffic lights have yellow times given explicitely, now
//
// Revision 1.6  2003/04/04 07:43:04  dkrajzew
// Yellow phases must be now explicetely given; comments added; order of edge sorting (false lane connections) debugged
//
// Revision 1.5  2003/04/01 15:15:24  dkrajzew
// some documentation added
//
// Revision 1.4  2003/03/17 14:22:33  dkrajzew
// further debug and windows eol removed
//
// Revision 1.3  2003/03/03 14:59:22  dkrajzew
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
#include "config.h"
#endif // HAVE_CONFIG_H

#include <cassert>
#include <vector>
#include <algorithm>
#include <iostream>
#include <bitset>
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include "NBTrafficLightLogic.h"
#include "NBTrafficLightLogicVector.h"
#include "NBRequestEdgeLinkIterator.h"
#include "NBTrafficLightPhases.h"
#include "NBLinkCliqueContainer.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
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
                                    std::string type,
                                    size_t noLinks,
                                    const NBRequestEdgeLinkIterator &cei1,
                                    const NBConnectionVector &inLinks,
                                    size_t breakingTime) const
{
    NBTrafficLightLogicVector *ret =
        new NBTrafficLightLogicVector(inLinks, type);
    for(size_t i=0; i<_phasesVectorsByLength.size(); i++) {
        for(size_t j=0; j<_phasesVectorsByLength[i].size(); j++) {
            ret->add(
                buildTrafficLightsLogic(
                    key, noLinks, _phasesVectorsByLength[i][j], cei1,
                    breakingTime));
        }
    }
    return ret;
}


NBTrafficLightLogic *
NBTrafficLightPhases::buildTrafficLightsLogic(const std::string &key,
                                              size_t noLinks,
                                              const PhaseIndexVector &phaseList,
                                              const NBRequestEdgeLinkIterator &cei1,
                                              size_t breakingTime) const
{
    NBTrafficLightLogic *ret =
        new NBTrafficLightLogic(key, noLinks);
    for(size_t i=0; i<phaseList.size(); i++) {
        // build and add the complete phase
        std::bitset<64> driveMask;
        std::bitset<64> brakeMask;
        size_t pos = 0;
        // go through the regarded links (in dependence whether
        //  left mover etc. were joined with te current)
        size_t j = 0;
        for(; j<cei1.getNoValidLinks(); j++) {
            // check how many real links are assigned to it
            size_t noEdges = cei1.getNumberOfAssignedLinks(j);
            // go through these links
            for(size_t k=0; k<noEdges; k++) {
                // set information for this link
                assert(i<phaseList.size());
                driveMask.set(pos, _cliques.test(phaseList[i], j));
                pos++;
            }
        }
        pos = 0;
        j = 0;
        for(; j<cei1.getNoValidLinks(); j++) {
            // check how many real links are assigned to it
            size_t noEdges = cei1.getNumberOfAssignedLinks(j);
            // go through these links
            for(size_t k=0; k<noEdges; k++) {
                // set information for this link
                assert(i<phaseList.size());
                if(driveMask.test(pos)) {
                    // the vehicle is allowed to drive, but may
                    //  have to brake due to a higher priorised
                    //  stream
                    brakeMask.set(pos,
                        cei1.testBrakeMask(pos, driveMask));
                } else {
                    // the vehicle is not allowed to drive anyway
                    brakeMask.set(pos, true);
                }
                pos++;
            }
        }
        // add phase
        size_t duration = 20;
        if(OptionsSubSys::getOptions().isSet("traffic-light-green")) {
            duration = OptionsSubSys::getOptions().getInt("traffic-light-green");
        }
        ret->addStep(duration, driveMask, brakeMask, std::bitset<64>());
        // add possible additional left-turn phase when existing
		std::bitset<64> yellow = driveMask;
        std::bitset<64> oldBrakeMask = brakeMask;
        cei1.resetNonLeftMovers(driveMask, brakeMask, yellow);
        std::bitset<64> inv;
        inv.flip();
        if(driveMask.any()) {
			// let the junction be clear from any vehicles before alowing turn left
            //  left movers may drive but have to wait if another vehicle is passing
            if(breakingTime!=0) {
                ret->addStep(breakingTime, driveMask, oldBrakeMask|yellow, yellow);
            }
			// let vehicles moving left pass secure
            if(breakingTime!=0) { // !! something else
                ret->addStep(3, driveMask, brakeMask, std::bitset<64>());
            }
            yellow = driveMask;
        }
        // add the dead phase for this junction
        if(breakingTime!=0) {
            ret->addStep(breakingTime, std::bitset<64>(), inv, yellow);
        }
    }
#ifdef TL_DEBUG
    DEBUG_OUT << "Phasenfolge (Ende):" << endl;
    ret->_debugWritePhases();
    DEBUG_OUT << "----------------------------------" << endl;
#endif
    return ret;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


