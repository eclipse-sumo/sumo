//---------------------------------------------------------------------------//
//                        NBLinkCliqueContainer.cpp -
//  A container for link cliques
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
#include <iostream> // !!! debug only
#include <cassert>
#include "NBLinkCliqueContainer.h"
#include "NBLinkPossibilityMatrix.h"
#include "NBTrafficLightPhases.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NBLinkCliqueContainer::NBLinkCliqueContainer(NBLinkPossibilityMatrix *v,
                                             size_t maxStromAnz)
{
    buildCliques(v, maxStromAnz);
    buildFurther();
}


NBLinkCliqueContainer::~NBLinkCliqueContainer()
{
}


NBTrafficLightPhases *
NBLinkCliqueContainer::computePhases(NBLinkPossibilityMatrix *v,
                                     size_t noLinks,
                                     bool appendSmallestOnly,
                                     bool skipLarger) const
{
    assert(_cliques.size()!=0);
    NBTrafficLightPhases *ret = new NBTrafficLightPhases(*this, noLinks);
    size_t clAnzahl = _cliques.size();
    std::bitset<64> aktStrNr;
    std::bitset<64> vorhanden;
    std::bitset<64> needed;
    std::vector<size_t> usedCliques;
    for(size_t k=0; k<noLinks; k++) {
        needed.set(k, 1);
    }
    size_t minSize = INT_MAX;
    usedCliques.push_back(0);
    assert(_cliques.size()>0);
    vorhanden = _cliques[0];
    // maybe the first is already enough
    if(vorhanden==needed) {
        ret->add(usedCliques);
        minSize = 1;
    }
    // loop adding valid phase lists
    for(size_t i=1;
            i<_cliques.size() &&  // do not read over the available elements
            (usedCliques.size()!=0 ||  // check all combinations (until stack empty)
                i<_cliques.size()-1); )
    {
        // step 1
        while(vorhanden!=needed && i<clAnzahl &&
                ( !skipLarger || usedCliques.size()<minSize-1 ) &&
                furtherResolutionPossible(vorhanden, needed, i)
        ) {
            usedCliques.push_back(i);
            assert(i<_cliques.size());
            vorhanden |= _cliques[i++];//getIncludedCliqueLinks(v, clVektor, ++i);//(*v)[(*clVektor)[++i]];
        }
        // Note: we do not check for a maximum number of joinable cliques
        // Note: the order of steps was changed
        // step3
        if(vorhanden==needed) {
            if( minSize==INT_MAX || // nothing was appended, yet
                !appendSmallestOnly || // everything shall be appended
                (usedCliques.size()<=minSize+1) ) // is (one of) the smallest
            {
                ret->add(usedCliques);
                if(minSize>usedCliques.size()) {
                    minSize = usedCliques.size();
                }
            }
        }
        // step 2
        bool newFound = false;
        while(usedCliques.size()>0&&!newFound) {
            i = usedCliques.back();
            usedCliques.pop_back();
            if( i<clAnzahl-1 &&
                furtherResolutionPossible(vorhanden, needed, i+1) )
            {
                vorhanden.reset();
                for(std::vector<size_t>::iterator l=usedCliques.begin(); l!=usedCliques.end(); l++) {
                    size_t a = _cliques.size();
                    assert(*l<_cliques.size());
                    vorhanden |= _cliques[*l];
                }
                newFound = true;
                i++;
            }
        }
        if(!newFound&&usedCliques.size()==0) {
            i = _cliques.size() + 1;
        }
    }
    //
//    cout << *ret;
    return ret;
}


bool
NBLinkCliqueContainer::test(size_t itemIndex, size_t linkIndex) const
{
    assert(itemIndex<_cliques.size());
    return _cliques[itemIndex].test(linkIndex);
}


bool
NBLinkCliqueContainer::furtherResolutionPossible(std::bitset<64> vorhanden,
                                                 std::bitset<64> needed,
                                                 size_t next) const
{
    if(next<_further.size()) {
        vorhanden |= _further[next];
    }
    return vorhanden==needed;
}


void
NBLinkCliqueContainer::buildCliques(NBLinkPossibilityMatrix *v,
                                    size_t maxStromAnz)
{
    std::bitset<64> verbStromNr;
    std::vector<std::bitset<64> > schnitt(maxStromAnz);
    std::vector<size_t> idxKeller(maxStromAnz);
    std::bitset<64> letzterSchnitt;
    // step 1
    assert(schnitt.size()>0);
    schnitt[0] = (*v)[0];
    assert(idxKeller.size()>0);
    idxKeller[0] = 0;
    int j = 0;
    for(size_t i=0; i<maxStromAnz; ) {
        // step 2
        int nextSet = 0;
        while(nextSet>=0) {
            nextSet = -1;
            // suche nach
            int k;
            for(k=i+1; k<maxStromAnz&&nextSet<0; k++) {
                assert(j<schnitt.size());
                if(schnitt[j].test(k)) {
                    nextSet = k;
                }
            }
            if(nextSet>0) {
                i = nextSet;
                j++;
                assert(j>0);
                assert(j-1<schnitt.size());
                letzterSchnitt = schnitt[j-1];
                assert(i<(*v).size());
                assert(i>=0);
                letzterSchnitt &= (*v)[i];
                assert(j<schnitt.size());
                assert(j>=0);
                schnitt[j] = letzterSchnitt;
                assert(j<idxKeller.size());
                assert(j>=0);
                idxKeller[j] = i;
            }
        }
        // step 3
        assert(j<schnitt.size());
        letzterSchnitt = schnitt[j];
        letzterSchnitt &= verbStromNr;
        if(letzterSchnitt.none()) {
            assert(j<schnitt.size());
            _cliques.push_back(schnitt[j]);
        }
        //
        assert(i<idxKeller.size());
        i = idxKeller[j];
        verbStromNr.set(i, 1);
        for(int k=i+1; k<maxStromAnz; k++) {
            assert(k>0&&k<64);
            verbStromNr.set(k, 0);
        }
        //
        j--;
        //
        if(j<0) {
            i++;
            if(i<v->size()) {
                assert(i>=0&&i<(*v).size());
                schnitt[0] = (*v)[i];
            }
            idxKeller[0] = i;
            j = 0;
        }
    }
#ifdef TL_DEBUG
	cout << "Cliquen:" << endl;
	for(LinkCliqueContainer::iterator a=_cliques.begin(); a!=_cliques.end(); a++) {
		cout << (*a) << endl;
	}
	cout << "--------------------------------" << endl;
#endif
}


void
NBLinkCliqueContainer::buildFurther()
{
    std::bitset<64> current;
    int i;
    // preinit the further-structure
    _further.reserve(_cliques.size());
    for(i=0; i<_cliques.size(); i++) {
        _further.push_back(current);
    }
    // compute the following clique definition
    for(i=0; i<_cliques.size(); i++) {
        current.reset();
        for(size_t j=i; j<_cliques.size(); j++) {
            current |= _cliques[j];
        }
        _further[i] = current;
    }
	//
#ifdef TL_DEBUG
	cout << "------------" << endl << "Cliquen: " << endl;
	for(i=0; i<_cliques.size(); i++) {
		cout << _cliques[i] << endl;
	}
/*	cout << "------------" << endl;
	for(i=0; i<_cliques.size(); i++) {
		cout << _further[i] <<endl;
	}
	cout << "------------" << endl;*/
#endif
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBLinkCliqueContainer.icc"
//#endif

// Local Variables:
// mode:C++
// End:
