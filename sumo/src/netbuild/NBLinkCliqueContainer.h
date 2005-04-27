#ifndef NBLinkCliqueContainer_h
#define NBLinkCliqueContainer_h
//---------------------------------------------------------------------------//
//                        NBLinkCliqueContainer.h -
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
// $Log$
// Revision 1.3  2005/04/27 11:48:25  dkrajzew
// level3 warnings removed; made containers non-static
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

#include <vector>
#include <bitset>
#include "NBLinkPossibilityMatrix.h"
#include "NBTrafficLightPhases.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NBLinkCliqueContainer
 * This container stores linkcliques. A link clique is a set of links of a
 * junction that may be used parallel, that means, that none of the links
 * within such a clique is a foe to another one from the same clique.
 * For a faster computation of permutations of the cliques that do regard all
 * links, a second structure, _further,  also stores the information which
 * links are regarded within the following cliques (the sizes of _cliques
 * and _further are the same)
 */
class NBLinkCliqueContainer
{
public:
    /// constructor
    NBLinkCliqueContainer(NBLinkPossibilityMatrix *v,
        size_t maxStromAnz);

    /// destructor
    ~NBLinkCliqueContainer();

    /** computes all possible phase combinations that regard all links
        of the junction */
    NBTrafficLightPhases *computePhases(NBLinkPossibilityMatrix *v,
        size_t noLinks, bool appendSmallestOnly, bool skipLarger) const;

    /** Tests whether the item at the given position is set.
	The position is given by the clique index and the index within this
        clique */
    bool test(size_t itemIndex, size_t linkIndex) const;

private:
    /// builds the possible cliques
    void buildCliques(NBLinkPossibilityMatrix *v, size_t maxStromAnz);

    /// builds the information which links are set in further steps
    void buildFurther();

    /** returns the information whether all links are regarded when
        a certain list of regarded links and the position within the clique
        vector is given */
    bool furtherResolutionPossible(std::bitset<64> vorhanden,
        std::bitset<64> needed, size_t next) const;

private:
    /// the definitions of the list of cliques
    typedef std::vector<std::bitset<64> > LinkCliqueContainer;

    /// the list of cliques
    LinkCliqueContainer _cliques;

    /// the definitions of the list of further set cliques
    typedef std::vector<std::bitset<64> > FurtherPossibleContainer;

    /// the list of further set cliques
    FurtherPossibleContainer _further;
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

