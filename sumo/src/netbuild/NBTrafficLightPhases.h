/****************************************************************************/
/// @file    NBTrafficLightPhases.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A container for traffic light phases
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
#ifndef NBTrafficLightPhases_h
#define NBTrafficLightPhases_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <algorithm>
#include <iostream>
#include "NBTrafficLightLogicVector.h"


// ===========================================================================
// class declarations
// ===========================================================================
class NBLinkCliqueContainer;
class NBRequestEdgeLinkIterator;
class NBTrafficLightLogic;


// ===========================================================================
// class definitions
// ===========================================================================
/// Definition of a vector holding indices of phases
typedef std::vector<size_t> PhaseIndexVector;

/**
 * @class NBTrafficLightPhases
 * An intermediate class for building of traffic light logics. Obtains
 * a container with cliques and builds logics.
 */
class NBTrafficLightPhases
{
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
            size_t noLinks, const std::string &type, const PhaseIndexVector &phaseList,
            const NBRequestEdgeLinkIterator &cei1,
            size_t breakingTime) const;

    /**
     * @class phase_length_finder
     * Searches for index vectors (phase lists) with the same length
     * as the one given
     */
    class phase_length_finder
    {
    private:
        /// The wished index vector size
        size_t mySize;

    public:
        /** constructor */
        explicit phase_length_finder(size_t size)
                : mySize(size) { }

        /** the comparing function */
        bool operator()(const PhaseIndexVector &p) {
            return p.size() == mySize;
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
    class included_finder
    {
    protected:
        /// The list to find all elements of within the supplied lists
        const PhaseIndexVector &myVector;

    public:
        /** constructor */
        included_finder(const PhaseIndexVector &v)
                : myVector(v) { }

    protected:
        /** @brief Returns the information whether v1 is included in v2
            v1 is the first argument, v2 is the second one */
        bool isIn(const PhaseIndexVector &v1, const PhaseIndexVector &v2) {
            // As the lists are sorted, we do not have to perform
            //  the whole exhaustive search, but may start to search
            //  after the last elements found
            PhaseIndexVector::const_iterator v2curr = v2.begin();
            for (PhaseIndexVector::const_iterator i=v1.begin(); i!=v1.end(); ++i) {
                v2curr = std::find(v2curr, v2.end(), *i);
                if (v2curr==v2.end()) {
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
class shorter_included_finder : public included_finder
    {
    public:
        /** constructor */
        shorter_included_finder(const PhaseIndexVector &v)
                : included_finder(v) { }

        /** the comparing function */
        bool operator()(const PhaseIndexVector &p) {
            return isIn(p, myVector);
        }

    };


    /**
     * @class larger_included_finder
     * Returns the information whether the current list is larger than
     * the one supplied at construction and contains all elements of
     * the one supplied at construction
     */
class larger_included_finder : public included_finder
    {
    public:
        /** constructor */
        larger_included_finder(const PhaseIndexVector &v)
                : included_finder(v) { }

        /** the comparing function */
        bool operator()(const PhaseIndexVector &p) {
            return isIn(myVector, p);
        }

    };

    /// Definition of a vector of indices of phases which themselves are defined within another container
    typedef std::vector<PhaseIndexVector> PhasesVector;

    /// Definition of a vector of phase lists
    typedef std::vector<PhasesVector> PhasesVectorVector;

    /// Container for phase lists, sorted by their length
    PhasesVectorVector myPhasesVectorsByLength;

    /// The information about friendly cliqeus to build the phase lists from
    const NBLinkCliqueContainer &myCliques;

    /// Counter of how many phase lists are available
    size_t myNoPhaseVectors;

};


#endif

/****************************************************************************/

