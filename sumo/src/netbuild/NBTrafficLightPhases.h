#ifndef NBTrafficLightPhases_h
#define NBTrafficLightPhases_h

#include <vector>
#include <algorithm>
#include <iostream>
#include "NBTrafficLightLogicVector.h"

class NBLinkCliqueContainer;
class NBRequestEdgeLinkIterator;
class NBTrafficLightLogic;

typedef std::vector<size_t> PhaseIndexVector;

class NBTrafficLightPhases {
private:
    typedef std::vector<PhaseIndexVector> PhasesVector;
    typedef std::vector<PhasesVector> PhasesVectorVector;
    PhasesVectorVector _phasesVectorsByLength;
    const NBLinkCliqueContainer &_cliques;
    size_t _noPhaseVectors;

    class phase_length_finder {
    private:
        size_t _size;
    public:
        /** constructor */
        explicit phase_length_finder(size_t size) 
            : _size(size) { }
        /** the comparing function */
        bool operator() (const PhaseIndexVector &p) {
            return p.size() == _size;
        }
    };


    class included_finder {
    protected:
        const PhaseIndexVector &_vector;
    public:
        /** constructor */
        included_finder(const PhaseIndexVector &v) 
            : _vector(v) { }
    protected:
        bool isIn(const PhaseIndexVector &v1, const PhaseIndexVector &v2) {
            PhaseIndexVector::const_iterator v2curr = v2.begin();
            for(PhaseIndexVector::const_iterator i=v1.begin(); i!=v1.end(); i++) {
                v2curr = std::find(v2curr, v2.end(), *i);
                if(v2curr==v2.end()) {
                    return false;
                }
            }
            return true;
        }
    };

    class shorter_included_finder : public included_finder {
    public:
        /** constructor */
        shorter_included_finder(const PhaseIndexVector &v) 
            : included_finder(v) { }
        /** the comparing function */
        bool operator() (const PhaseIndexVector &p) {
            return isIn(p, _vector);
        }
    };

    class larger_included_finder : public included_finder {
    public:
        /** constructor */
        larger_included_finder(const PhaseIndexVector &v) 
            : included_finder(v) { }
        /** the comparing function */
        bool operator() (const PhaseIndexVector &p) {
            return isIn(_vector, p);
        }
    };


    
public:
    NBTrafficLightPhases(const NBLinkCliqueContainer &cliques, 
        size_t noCliques);
    ~NBTrafficLightPhases();
    void add(const PhaseIndexVector &phase);
    void add(const NBTrafficLightPhases &phases, bool skipLarger);
//    PhaseIndexVector getBest() const;
    NBTrafficLightLogicVector *computeLogics(const std::string &key,
        size_t noLinks, 
        const NBRequestEdgeLinkIterator &cei1) const;
    NBTrafficLightLogic *buildTrafficLightsLogic(const std::string &key,
        size_t noLinks, const PhaseIndexVector &phaseList,
        const NBRequestEdgeLinkIterator &cei1) const;
    friend std::ostream &operator<<(std::ostream &os, 
        const NBTrafficLightPhases &p);
};


#endif
