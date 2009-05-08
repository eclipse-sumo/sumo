/****************************************************************************/
/// @file    NBOwnTLDef.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.05.2005
/// @version $Id$
///
// A traffic light logics which must be computed (only nodes/edges are given)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NBOwnTLDef_h
#define NBOwnTLDef_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <set>
#include "NBTrafficLightDefinition.h"


// ===========================================================================
// class declarations
// ===========================================================================
class NBNode;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBOwnTLDef
 * @brief A traffic light logics which must be computed (only nodes/edges are given)
 */
class NBOwnTLDef : public NBTrafficLightDefinition {
public:
    /// Constructor
    NBOwnTLDef(const std::string &id,
               const std::set<NBNode*> &junctions) throw();

    /// Constructor
    NBOwnTLDef(const std::string &id, NBNode *junction) throw();

    /// Constructor
    NBOwnTLDef(const std::string &id) throw();

    /// Destructor
    ~NBOwnTLDef() throw();

    void setParticipantsInformation() throw();

public:
    void remapRemoved(NBEdge *removed,
                      const EdgeVector &incoming, const EdgeVector &outgoing) throw();

protected:
    /// Computes the traffic light logic
    NBTrafficLightLogicVector *myCompute(const NBEdgeCont &ec,
                                         unsigned int breakingTime) throw();

    /// Collects the nodes participating in this traffic light
    void collectNodes() throw();

    void collectLinks() throw(ProcessError);

    void replaceRemoved(NBEdge *removed, int removedLane,
                        NBEdge *by, int byLane) throw();

    void setTLControllingInformation(const NBEdgeCont &ec) const throw();


protected:
    int getToPrio(NBEdge *e) throw();
    SUMOReal computeUnblockedWeightedStreamNumber(NBEdge* e1, NBEdge *e2) throw();
    std::pair<NBEdge*, NBEdge*> getBestCombination(const std::vector<NBEdge*> &edges) throw();
    std::pair<NBEdge*, NBEdge*> getBestPair(std::vector<NBEdge*> &incoming) throw();

    /**
     * edge_by_incoming_priority_sorter
     */
    class edge_by_incoming_priority_sorter {
    public:
        /// comparing operator
        int operator()(NBEdge *e1, NBEdge *e2) const {
            if (e1->getJunctionPriority(e1->getToNode())!=e2->getJunctionPriority(e2->getToNode())) {
                return e1->getJunctionPriority(e1->getToNode())> e2->getJunctionPriority(e2->getToNode());
            }
            return e1->getID() > e2->getID();
        }
    };

};


#endif

/****************************************************************************/

