#include <vector>
#include <bitset>
#include <iostream> // !!! debug only
#include "NBLinkCliqueContainer.h"
#include "NBLinkPossibilityMatrix.h"
#include "NBTrafficLightPhases.h"

using namespace std;

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
                ( !skipLarger || usedCliques.size()<minSize-1 )
        ) {
            usedCliques.push_back(i);
            vorhanden |= _cliques[i++];//getIncludedCliqueLinks(v, clVektor, ++i);//(*v)[(*clVektor)[++i]];
        }
        // Note: we do not check for a maximum number of joinable cliques
        // Note: the order of steps was changed
        // step3
        if(vorhanden==needed) {
            if( minSize==INT_MAX || 
                !appendSmallestOnly || 
                (usedCliques.size()<=minSize+1) ) 
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
    return _cliques[itemIndex].test(linkIndex);
}


bool
NBLinkCliqueContainer::furtherResolutionPossible(std::bitset<64> vorhanden,
                                                 std::bitset<64> needed,
                                                 size_t next) const
{
    vorhanden |= _further[next];
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
    schnitt[0] = (*v)[0];
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
                if(schnitt[j].test(k)) {
                    nextSet = k;
                }
            }
            if(nextSet>0) {
                i = nextSet;
                j++;
                letzterSchnitt = schnitt[j-1];
                letzterSchnitt &= (*v)[i];
                schnitt[j] = letzterSchnitt;
                idxKeller[j] = i;
            }
        }
        // step 3
        letzterSchnitt = schnitt[j];
        letzterSchnitt &= verbStromNr;
        if(letzterSchnitt.none()) {
            _cliques.push_back(schnitt[j]);
        }
        //
        i = idxKeller[j];
        verbStromNr.set(i, 1);
        for(int k=i+1; k<maxStromAnz; k++) {
            verbStromNr.set(k, 0);
        }
        //
        j--;
        //
        if(j<0) {
            i++;
            schnitt[0] = (*v)[i];
            idxKeller[0] = i;
            j = 0;
        }
    }
#ifdef TL_DEBUG
	cout << "Cliquen:" << endl;
	for(LinkCliqueContainer::iterator bla=_cliques.begin(); bla!=_cliques.end(); bla++) {
		cout << (*bla) << endl;
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

