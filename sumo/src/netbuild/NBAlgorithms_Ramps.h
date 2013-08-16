/****************************************************************************/
/// @file    NBAlgorithms_Ramps.h
/// @author  Daniel Krajzewicz
/// @date    29. March 2012
/// @version $Id$
///
// Algorithms for highway on-/off-ramps computation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NBAlgorithms_Ramps_h
#define NBAlgorithms_Ramps_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>


// ===========================================================================
// class declarations
// ===========================================================================
class NBNetBuilder;
class OptionsCont;
class NBNode;
class NBEdgeCont;
class NBDistrictCont;


// ===========================================================================
// class definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// NBAlgorithms_Ramps
// ---------------------------------------------------------------------------
/* @class NBRampsComputer
 * @brief Computes highway on-/off-ramps (if wished)
 */
class NBRampsComputer {
public:
    /** @brief Computes highway on-/off-ramps (if wished)
     * @param[in, changed] nb The network builder which contains the current network representation
     * @param[in] oc The options container
     */
    static void computeRamps(NBNetBuilder& nb, OptionsCont& oc);

    /// @brief suffix for newly generated on-ramp edges
    static const std::string ADDED_ON_RAMP_EDGE;

private:
    /** @brief Determines whether the given node may be an on-ramp begin
     * @param[in] cur The node to check
     * @param[in] minHighwaySpeed The minimum speed limit a highway must have for being a highway
     * @param[in] maxRampSpeed The maximum speed limit a ramp must have for being a ramp
     * @return Whether the node is assumed to be an on-ramp begin
     */
    static bool mayNeedOnRamp(NBNode* cur, SUMOReal minHighwaySpeed, SUMOReal maxRampSpeed);


    /** @brief Determines whether the given node may be an off-ramp end
     * @param[in] cur The node to check
     * @param[in] minHighwaySpeed The minimum speed limit a highway must have for being a highway
     * @param[in] maxRampSpeed The maximum speed limit a ramp must have for being a ramp
     * @return Whether the node is assumed to be an off-ramp end
     */
    static bool mayNeedOffRamp(NBNode* cur, SUMOReal minHighwaySpeed, SUMOReal maxRampSpeed);


    /** @brief Builds an on-ramp starting at the given node
     * @param[in] cur The node at which the on-ramp shall begin
     * @param[in] nc The container of nodes
     * @param[in] ec The container of edges
     * @param[in] dc The container of districts
     * @param[in] rampLength The wished ramp length
     * @param[in] dontSplit Whether no edges shall be split
     * @param[in, filled] incremented The list of edges which lane number was already incremented
     */
    static void buildOnRamp(NBNode* cur, NBNodeCont& nc, NBEdgeCont& ec, NBDistrictCont& dc, SUMOReal rampLength, bool dontSplit, std::set<NBEdge*>& incremented);


    /** @brief Builds an off-ramp ending at the given node
     * @param[in] cur The node at which the off-ramp shall end
     * @param[in] nc The container of nodes
     * @param[in] ec The container of edges
     * @param[in] dc The container of districts
     * @param[in] rampLength The wished ramp length
     * @param[in] dontSplit Whether no edges shall be split
     * @param[in, filled] incremented The list of edges which lane number was already incremented
     */
    static void buildOffRamp(NBNode* cur, NBNodeCont& nc, NBEdgeCont& ec, NBDistrictCont& dc, SUMOReal rampLength, bool dontSplit, std::set<NBEdge*>& incremented);


    static void getOnRampEdges(NBNode* n, NBEdge** potHighway, NBEdge** potRamp, NBEdge** other);
    static void getOffRampEdges(NBNode* n, NBEdge** potHighway, NBEdge** potRamp, NBEdge** other);
    static bool determinedBySpeed(NBEdge** potHighway, NBEdge** potRamp);
    static bool determinedByLaneNumber(NBEdge** potHighway, NBEdge** potRamp);

    /** @brief Checks whether an on-/off-ramp can be bult here
     *
     * - none of the participating edges must be a macroscopic connector
     * - ramp+highways together must have more lanes than the continuation
     * - speeds must match the defined swells
     * @param[in] potHighway The highway part to check
     * @param[in] potRamp The ramp part to check
     * @param[in] other The successor/predecessor edge
     * @param[in] minHighwaySpeed The minimum speed limit a highway must have for being a highway
     * @param[in] maxRampSpeed The maximum speed limit a ramp must have for being a ramp
     * @return Whether a ramp can be built here
     */
    static bool fulfillsRampConstraints(NBEdge* potHighway, NBEdge* potRamp, NBEdge* other, SUMOReal minHighwaySpeed, SUMOReal maxRampSpeed);


    /** @brief Moves the ramp to the right, as new lanes were added
     * @param[in] ramp The ramp to move
     * @param[in] addedLanes The number of added lanes
     */
    static void moveRampRight(NBEdge* ramp, int addedLanes);

};


#endif

/****************************************************************************/

