#ifndef NBTrafficLightPhases_h
#define NBTrafficLightPhases_h
//---------------------------------------------------------------------------//
//                        NBTrafficLightPhases.h -
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
// $Log$
// Revision 1.5  2003/04/01 15:15:24  dkrajzew
// some documentation added
//
// Revision 1.4  2003/03/20 16:23:10  dkrajzew
// windows eol removed; multiple vehicle emission added
//
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

#include <vector>
#include <algorithm>
#include <iostream>
#include "NBTrafficLightLogicVector.h"

/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBLinkCliqueContainer;
class NBRequestEdgeLinkIterator;
class NBTrafficLightLogic;

/// Definition of a vector holding indices of phases
typedef std::vector<size_t> PhaseIndexVector;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NBTrafficLightPhases
 * An intermediate class for building of traffic light logics. Obtains
 * a container with cliques and builds logics.
 */
class NBTrafficLightPhases {
public:
    /// Constructor
    NBTrafficLightPhases(const NBLinkCliqueContainer &cliques,
        size_t noCliques);

    /// Destructor
    ~NBTrafficLightPhases();

    /// Adds
    void add(const PhaseIndexVector &phase);
    void add(const NBTrafficLightPhases &phases, bool skipLarger);
//    PhaseIndexVector getBest() const;
    NBTrafficLightLogicVector *computeLogics(const std::string &key,
        size_t noLinks, const NBRequestEdgeLinkIterator &cei1,
        const EdgeVector &inLanes) const;
    NBTrafficLightLogic *buildTrafficLightsLogic(const std::string &key,
        size_t noLinks, const PhaseIndexVector &phaseList,
        const NBRequestEdgeLinkIterator &cei1) const;
    friend std::ostream &operator<<(std::ostream &os,
        const NBTrafficLightPhases &p);

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



};




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NBTrafficLightPhases.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

