/****************************************************************************/
/// @file    NBOwnTLDef.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 29.05.2005
/// @version $Id$
///
// A traffic light logics which must be computed (only nodes/edges are given)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
    /** @brief Constructor
     * @param[in] id The id of the tls
     * @param[in] junctions Junctions controlled by this tls
     */
    NBOwnTLDef(const std::string &id,
               const std::vector<NBNode*> &junctions) throw();


    /** @brief Constructor
     * @param[in] id The id of the tls
     * @param[in] junction The junction controlled by this tls
     */
    NBOwnTLDef(const std::string &id, NBNode *junction) throw();


    /** @brief Constructor
     * @param[in] id The id of the tls
     */
    NBOwnTLDef(const std::string &id) throw();


    /// @brief Destructor
    ~NBOwnTLDef() throw();


    /** @brief Builds the list of participating nodes/edges/links
     * @see NBTrafficLightDefinition::setParticipantsInformation
     */
    void setParticipantsInformation() throw();


    /// @name Public methods from NBTrafficLightDefinition-interface
    /// @{

    /** @brief Replaces occurences of the removed edge in incoming/outgoing edges of all definitions
     * @param[in] removed The removed edge
     * @param[in] incoming The edges to use instead if an incoming edge was removed
     * @param[in] outgoing The edges to use instead if an outgoing edge was removed
     * @see NBTrafficLightDefinition::remapRemoved
     */
    void remapRemoved(NBEdge *removed,
                      const EdgeVector &incoming, const EdgeVector &outgoing) throw();


    /** @brief Informs edges about being controlled by a tls
     * @param[in] ec The container of edges
     * @see NBTrafficLightDefinition::setTLControllingInformation
     */
    void setTLControllingInformation(const NBEdgeCont &ec) const throw();
    /// @}


protected:
    /// @name Protected methods from NBTrafficLightDefinition-interface
    /// @{

    /** @brief Computes the traffic light logic finally in dependence to the type
     * @param[in] ec The edge container
     * @param[in] brakingTime Duration a vehicle needs for braking in front of the tls
     * @return The computed logic
     * @see NBTrafficLightDefinition::myCompute
     */
    NBTrafficLightLogic *myCompute(const NBEdgeCont &ec,
                                   unsigned int brakingTime) throw();


    /** @brief Collects the nodes participating in this traffic light
     * @see NBTrafficLightDefinition::collectNodes
     */
    void collectNodes() throw();


    /** @brief Collects the links participating in this traffic light
     * @exception ProcessError If a link could not be found
     * @see NBTrafficLightDefinition::collectLinks
     */
    void collectLinks() throw(ProcessError);


    /** @brief Replaces a removed edge/lane
     * @param[in] removed The edge to replace
     * @param[in] removedLane The lane of this edge to replace
     * @param[in] by The edge to insert instead
     * @param[in] byLane This edge's lane to insert instead
     * @see NBTrafficLightDefinition::replaceRemoved
     */
    void replaceRemoved(NBEdge *removed, int removedLane,
                        NBEdge *by, int byLane) throw();
    /// @}


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


    /** @class edge_by_incoming_priority_sorter
     * @brief Sorts edges by their priority within the node they end at
     */
    class edge_by_incoming_priority_sorter {
    public:
        /** @brief comparing operator
         * @param[in] e1 an edge
         * @param[in] e2 an edge
         */
        int operator()(const NBEdge * const e1, const NBEdge * const e2) const {
            if (e1->getJunctionPriority(e1->getToNode())!=e2->getJunctionPriority(e2->getToNode())) {
                return e1->getJunctionPriority(e1->getToNode())> e2->getJunctionPriority(e2->getToNode());
            }
            return e1->getID() > e2->getID();
        }
    };

};


#endif

/****************************************************************************/

