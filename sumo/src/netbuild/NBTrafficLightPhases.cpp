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
            bla=p._phasesVectorsByLength.begin(); 
            bla!=p._phasesVectorsByLength.end(); bla++) {
        for(NBTrafficLightPhases::PhasesVector::const_iterator 
                bla2=(*bla).begin();
                bla2!=(*bla).end();
                bla2++) {
		    PhaseIndexVector tmp = (*bla2);
		    for(PhaseIndexVector::const_iterator mutti=tmp.begin(); mutti!=tmp.end(); mutti++) {
			    os << (*mutti) << ", ";
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
                                    const NBRequestEdgeLinkIterator &cei1) const
{
    NBTrafficLightLogicVector *ret = new NBTrafficLightLogicVector();
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
    NBTrafficLightLogic *ret = new NBTrafficLightLogic(key, noLinks);
    for(size_t i=0; i<phaseList.size(); i++) {
        // add the complete phase
        std::bitset<64> driveMask;
        std::bitset<64> brakeMask;
        size_t pos = 0;
        for(size_t j=0; j<cei1.getNoValidLinks(); j++) {
            size_t noEdges = cei1.getNumberOfAssignedLinks(j);
            for(size_t k=0; k<noEdges; k++) {
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



