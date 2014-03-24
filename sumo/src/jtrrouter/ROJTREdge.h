/****************************************************************************/
/// @file    ROJTREdge.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Yun-Pang Floetteroed
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// An edge the jtr-router may route through
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2004-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef ROJTREdge_h
#define ROJTREdge_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <vector>
#include <utils/common/ValueTimeLine.h>
#include <router/ROEdge.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ROLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROJTREdge
 * @brief An edge the jtr-router may route through
 *
 * A router edge extended by the definition about the probability a
 *  vehicle chooses a certain following edge over time.
 */
class ROJTREdge : public ROEdge {
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the edge
     * @param[in] from The node the edge begins at
     * @param[in] to The node the edge ends at
     * @param[in] index The numeric id of the edge
     */
    ROJTREdge(const std::string& id, RONode* from, RONode* to, unsigned int index, const int priority);


    /// @brief Destructor
    ~ROJTREdge();


    /** @brief Adds information about a connected edge
     *
     * Makes this edge know the given following edge. Calls ROEdge::addFollower.
     *
     * Additionally it generates the entry for the given following edge
     *  in myFollowingDefs.
     *
     * @param[in] s The following edge
     * @see ROEdge::addFollower
     */
    void addFollower(ROEdge* s, std::string dir = "");


    /** @brief adds the information about the percentage of using a certain follower
     *
     * @param[in] follower The following edge
     * @param[in] begTime Time begin for which this probability is valid
     * @param[in] endTime Time end for which this probability is valid
     * @param[in] probability The probability to use the given follower
     */
    void addFollowerProbability(ROJTREdge* follower,
                                SUMOTime begTime, SUMOTime endTime, SUMOReal probability);


    /** @brief Returns the next edge to use
     * @param[in] veh The vehicle to choose the next edge for
     * @param[in] time The time at which the next edge shall be entered
     * @return The chosen edge
     */
    ROJTREdge* chooseNext(const ROVehicle* const veh, SUMOTime time) const;


    /** @brief Sets the turning definition defaults
     * @param[in] def The turning percentage defaults
     */
    void setTurnDefaults(const std::vector<SUMOReal>& defs);


private:
    /// @brief Definition of a map that stores the probabilities of using a certain follower over time
    typedef std::map<ROJTREdge*, ValueTimeLine<SUMOReal>*, Named::ComparatorIdLess> FollowerUsageCont;

    /// @brief Storage for the probabilities of using a certain follower over time
    FollowerUsageCont myFollowingDefs;

    /// @brief The defaults for turnings
    std::vector<SUMOReal> myParsedTurnings;


private:
    /// @brief invalidated copy constructor
    ROJTREdge(const ROJTREdge& src);

    /// @brief invalidated assignment operator
    ROJTREdge& operator=(const ROJTREdge& src);


};


#endif

/****************************************************************************/

