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
#include "NBMMLDirections.h"


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
    /** @brief Returns the weight of a stream given its direction
     * @param[in] dir The direction of the stream
     * @return This stream's weight
     * @todo There are several magic numbers; describe
     */
    SUMOReal getDirectionalWeight(NBMMLDirection dir) throw();


    /** @brief Returns this edge's priority at the node it ends at
     * @param[in] e The edge to ask for his priority
     * @return The edge's priority at his destination node
     */
    int getToPrio(const NBEdge * const e) throw();


    /** @brief Returns how many streams outgoing from the edges can pass the junction without being blocked
     * @param[in] e1 The first edge
     * @param[in] e2 The second edge
     * @todo There are several magic numbers; describe
     */
    SUMOReal computeUnblockedWeightedStreamNumber(const NBEdge * const e1, const NBEdge * const e2) throw();


    /** @brief Returns the combination of two edges from the given which has most unblocked streams
     * @param[in] edges The list of edges to include in the computation
     * @return The two edges for which the weighted number of unblocked streams is the highest
     */
    std::pair<NBEdge*, NBEdge*> getBestCombination(const std::vector<NBEdge*> &edges) throw();


    /** @brief Returns the combination of two edges from the given which has most unblocked streams
     *
     * The chosen edges are removed from the given vector 
     *
     * @param[in, changed] incoming The list of edges which are participating in the logic
     * @return The two edges for which the weighted number of unblocked streams is the highest
     */
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

