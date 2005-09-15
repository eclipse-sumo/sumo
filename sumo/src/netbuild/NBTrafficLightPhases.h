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
// Revision 1.10  2005/09/15 12:02:45  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.9  2005/04/27 11:48:26  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.8  2003/12/04 13:03:58  dkrajzew
// possibility to pass the tl-type from the netgenerator added
//
// Revision 1.7  2003/06/05 11:43:36  dkrajzew
// class templates applied; documentation added
//
// Revision 1.6  2003/04/04 07:43:04  dkrajzew
// Yellow phases must be now explicetely given; comments added; order of edge sorting (false lane connections) debugged
//
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
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
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


/* =========================================================================
 * class definitions
 * ======================================================================= */
/// Definition of a vector holding indices of phases
typedef std::vector<size_t> PhaseIndexVector;

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

    /// Adds a single list of phases
    void add(const PhaseIndexVector &phase);

    /// Adds all phase lists from the given storage
    void add(const NBTrafficLightPhases &phases, bool skipLarger);

    /// Computes the list of traffic lights logics from the stored phase lists
    NBTrafficLightLogicVector *computeLogics(const std::string &key,
        std::string type,
        size_t noLinks, const NBRequestEdgeLinkIterator &cei1,
        const NBConnectionVector &inLinks, size_t breakingTime) const;

    /// Output operator
    friend std::ostream &operator<<(std::ostream &os,
        const NBTrafficLightPhases &p);

private:
    /** Build a single traffic lights logic from the given values */
    NBTrafficLightLogic *buildTrafficLightsLogic(const std::string &key,
        size_t noLinks, const PhaseIndexVector &phaseList,
        const NBRequestEdgeLinkIterator &cei1,
        size_t breakingTime) const;

    /**
     * @class phase_length_finder
     * Searches for index vectors (phase lists) with the same length
     * as the one given
     */
    class phase_length_finder {
    private:
        /// The wished index vector size
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


    /**
     * @class included_finder
     * Virtual class for searching an element which holds all phases
     * from the supplied.
     * When building lists of possible phases, sometimes some of them
     * may contain already build ones (which are also complete) and some
     * further phases. Such lists are not needed as already the shorter
     * ones let all vehicle üass. This class is used to search these lists.
     */
    class included_finder {
    protected:
        /// The list to find all elements of within the supplied lists
        const PhaseIndexVector &_vector;

    public:
        /** constructor */
        included_finder(const PhaseIndexVector &v)
            : _vector(v) { }

    protected:
        /** @brief Returns the information whether v1 is included in v2
            v1 is the first argument, v2 is the second one */
        bool isIn(const PhaseIndexVector &v1, const PhaseIndexVector &v2) {
            // As the lists are sorted, we do not have to perform
            //  the whole exhaustive search, but may start to search
            //  after the last elements found
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


    /**
     * @class shorter_included_finder
     * Returns the information whether the current list is shorter than
     * the one supplied at construction and is a part of it.
     */
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


    /**
     * @class larger_included_finder
     * Returns the information whether the current list is larger than
     * the one supplied at construction and contains all elements of
     * the one supplied at construction
     */
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

    /// Definition of a vector of indices of phases which themselves are defined within another container
    typedef std::vector<PhaseIndexVector> PhasesVector;

    /// Definition of a vector of phase lists
    typedef std::vector<PhasesVector> PhasesVectorVector;

    /// Container for phase lists, sorted by their length
    PhasesVectorVector _phasesVectorsByLength;

    /// The information about friendly cliqeus to build the phase lists from
    const NBLinkCliqueContainer &_cliques;

    /// Counter of how many phase lists are available
    size_t _noPhaseVectors;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

