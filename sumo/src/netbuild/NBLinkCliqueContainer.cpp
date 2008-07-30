/****************************************************************************/
/// @file    NBLinkCliqueContainer.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A container for link cliques
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
#include <iostream> // !!! debug only
#include <cassert>
#include <climits>
#include "NBLinkCliqueContainer.h"
#include "NBLinkPossibilityMatrix.h"
#include "NBTrafficLightPhases.h"

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
// method definitions
// ===========================================================================
NBLinkCliqueContainer::NBLinkCliqueContainer(NBLinkPossibilityMatrix *v,
        size_t maxStromAnz)
{
    buildCliques(v, maxStromAnz);
    buildFurther();
}


NBLinkCliqueContainer::~NBLinkCliqueContainer()
{}


NBTrafficLightPhases *
NBLinkCliqueContainer::computePhases(NBLinkPossibilityMatrix *,
                                     size_t noLinks,
                                     bool appendSmallestOnly,
                                     bool skipLarger) const
{
    assert(myCliques.size()!=0);
    NBTrafficLightPhases *ret = new NBTrafficLightPhases(*this, noLinks);
    size_t clAnzahl = myCliques.size();
//    std::bitset<64> aktStrNr;
    std::bitset<64> vorhanden;
    std::bitset<64> needed;
    std::vector<size_t> usedCliques;
    for (size_t k=0; k<noLinks; k++) {
        needed.set(k, 1);
    }
    size_t minSize = INT_MAX;
    usedCliques.push_back(0);
    assert(myCliques.size()>0);
    vorhanden = myCliques[0];
    // maybe the first is already enough
    if (vorhanden==needed) {
        ret->add(usedCliques);
        minSize = 1;
    }
    // loop adding valid phase lists
    for (size_t i=1;
            i<myCliques.size() &&  // do not read over the available elements
            (usedCliques.size()!=0 ||  // check all combinations (until stack empty)
             i<myCliques.size()-1);) {
        // step 1
        while (vorhanden!=needed && i<clAnzahl &&
                (!skipLarger || usedCliques.size()<minSize-1) &&
                furtherResolutionPossible(vorhanden, needed, i)
              ) {
            usedCliques.push_back(i);
            assert(i<myCliques.size());
            vorhanden |= myCliques[i++];//getIncludedCliqueLinks(v, clVektor, ++i);//(*v)[(*clVektor)[++i]];
        }
        // Note: we do not check for a maximum number of joinable cliques
        // Note: the order of steps was changed
        // step3
        if (vorhanden==needed) {
            if (minSize==INT_MAX || // nothing was appended, yet
                    !appendSmallestOnly || // everything shall be appended
                    (usedCliques.size()<=minSize+1)) { // is (one of) the smallest
                ret->add(usedCliques);
                if (minSize>usedCliques.size()) {
                    minSize = usedCliques.size();
                }
            }
        }
        // step 2
        bool newFound = false;
        while (usedCliques.size()>0&&!newFound) {
            i = usedCliques.back();
            usedCliques.pop_back();
            if (i<clAnzahl-1 &&
                    furtherResolutionPossible(vorhanden, needed, i+1)) {
                vorhanden.reset();
                for (std::vector<size_t>::iterator l=usedCliques.begin(); l!=usedCliques.end(); l++) {
                    assert(*l<myCliques.size());
                    vorhanden |= myCliques[*l];
                }
                newFound = true;
                i++;
            }
        }
        if (!newFound&&usedCliques.size()==0) {
            i = myCliques.size() + 1;
        }
    }
    //
    return ret;
}


bool
NBLinkCliqueContainer::test(size_t itemIndex, size_t linkIndex) const
{
    assert(itemIndex<myCliques.size());
    return myCliques[itemIndex].test(linkIndex);
}


bool
NBLinkCliqueContainer::furtherResolutionPossible(std::bitset<64> vorhanden,
        std::bitset<64> needed,
        size_t next) const
{
    if (next<myFurther.size()) {
        vorhanden |= myFurther[next];
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
    for (size_t i=0; i<maxStromAnz;) {
        // step 2
        int nextSet = 0;
        while (nextSet>=0) {
            nextSet = -1;
            // suche nach
            size_t k;
            for (k=i+1; k<maxStromAnz&&nextSet<0; k++) {
                assert(j<(int) schnitt.size());
                if (schnitt[j].test(k)) {
                    nextSet = k;
                }
            }
            if (nextSet>0) {
                i = nextSet;
                j++;
                assert(j>0);
                assert(j-1<(int) schnitt.size());
                letzterSchnitt = schnitt[j-1];
                assert(i<(*v).size());
                letzterSchnitt &= (*v)[i];
                assert(j<(int) schnitt.size());
                assert(j>=0);
                schnitt[j] = letzterSchnitt;
                assert(j<(int) idxKeller.size());
                assert(j>=0);
                idxKeller[j] = i;
            }
        }
        // step 3
        assert(j<(int) schnitt.size());
        letzterSchnitt = schnitt[j];
        letzterSchnitt &= verbStromNr;
        if (letzterSchnitt.none()) {
            assert(j<(int) schnitt.size());
            myCliques.push_back(schnitt[j]);
        }
        //
        assert(i<idxKeller.size());
        i = idxKeller[j];
        verbStromNr.set(i, 1);
        for (size_t k=i+1; k<maxStromAnz; k++) {
            assert(k>0&&k<64);
            verbStromNr.set(k, 0);
        }
        //
        j--;
        //
        if (j<0) {
            i++;
            if (i<v->size()) {
                assert(i<(*v).size());
                schnitt[0] = (*v)[i];
            }
            idxKeller[0] = i;
            j = 0;
        }
    }
#ifdef TL_DEBUG
    DEBUG_OUT << "Cliquen:" << endl;
    for (LinkCliqueContainer::iterator a=myCliques.begin(); a!=myCliques.end(); a++) {
        DEBUG_OUT << (*a) << endl;
    }
    DEBUG_OUT << "--------------------------------" << endl;
#endif
}


void
NBLinkCliqueContainer::buildFurther()
{
    std::bitset<64> current;
    size_t i;
    // preinit the further-structure
    myFurther.reserve(myCliques.size());
    for (i=0; i<myCliques.size(); i++) {
        myFurther.push_back(current);
    }
    // compute the following clique definition
    for (i=0; i<myCliques.size(); i++) {
        current.reset();
        for (size_t j=i; j<myCliques.size(); j++) {
            current |= myCliques[j];
        }
        myFurther[i] = current;
    }
    //
#ifdef TL_DEBUG
    DEBUG_OUT << "------------" << endl << "Cliquen: " << endl;
    for (i=0; i<myCliques.size(); i++) {
        DEBUG_OUT << myCliques[i] << endl;
    }
#endif
}



/****************************************************************************/

