/****************************************************************************/
/// @file    ROMAAssignments.h
/// @author  Yun-Pang Floetteroed
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    Feb 2013
/// @version $Id$
///
// Assignment methods
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef ROMAAssignments_h
#define ROMAAssignments_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/vehicle/SUMOAbstractRouter.h>
#include <utils/common/SUMOTime.h>

// ===========================================================================
// class declarations
// ===========================================================================
class RONet;
class ODMatrix;
class Distribution_Points;
class ROEdge;
class ROMAEdge;
class ROVehicle;



// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROMAAssignments
 * @brief assignment methods
 *
 */
class ROMAAssignments {
public:
    /// Constructor
    ROMAAssignments(const SUMOTime begin, const SUMOTime end, RONet& net, ODMatrix& matrix, SUMOAbstractRouter<ROEdge, ROVehicle>& router);

    /// Destructor
    ~ROMAAssignments();

    // @brief calculate edge travel time with the given road class and max link speed
    SUMOReal capacityConstraintFunction(const ROEdge* edge, const SUMOReal flow) const;

    // @brief incremental method
    void incremental(const int numIter);

    // @brief UE method
    void ue();

    // @brief SUE method
    void sue(const int maxOuterIteration, const int maxInnerIteration, const int kPaths, const SUMOReal penalty, const SUMOReal tolerance, const std::string routeChoiceMethod);

    /** @brief Returns the effort to pass an edge including penalties
     *
     * This method is given to the used router in order to obtain the efforts
     *  to pass an edge from the internal edge weights container.
     *
     * @param[in] e The edge for which the effort to be passed shall be returned
     * @param[in] v The (default) vehicle that is routed
     * @param[in] t The time for which the effort shall be returned
     * @return The effort (time to pass in this case) for an edge
     * @see DijkstraRouterTT_ByProxi
     */
    static SUMOReal getPenalizedEffort(const ROEdge* const e, const ROVehicle* const v, SUMOReal t);

    /** @brief Returns the traveltime on an edge including penalties
     *
     * This method is given to the used router in order to obtain the efforts
     *  to pass an edge from the internal edge weights container.
     *
     * @param[in] e The edge for which the effort to be passed shall be returned
     * @param[in] v The (default) vehicle that is routed
     * @param[in] t The time for which the effort shall be returned
     * @return The effort (time to pass in this case) for an edge
     * @see DijkstraRouterTT_ByProxi
     */
    static SUMOReal getPenalizedTT(const ROEdge* const e, const ROVehicle* const v, SUMOReal t);

    /** @brief Returns the traveltime on an edge without penalties
     *
     * This method is given to the used router in order to obtain the efforts
     *  to pass an edge from the internal edge weights container.
     *
     * @param[in] e The edge for which the effort to be passed shall be returned
     * @param[in] v The (default) vehicle that is routed
     * @param[in] t The time for which the effort shall be returned
     * @return The effort (time to pass in this case) for an edge
     * @see DijkstraRouterTT_ByProxi
     */
    static SUMOReal getTravelTime(const ROEdge* const e, const ROVehicle* const v, SUMOReal t);

private:
    /// @brief add a route and check for duplicates
    bool addRoute(std::vector<const ROEdge*>& edges, std::vector<RORoute*>& paths, std::string routeId, SUMOReal costs, SUMOReal prob);

    /// @brief get the k shortest paths
    void getKPaths(const int kPaths, const SUMOReal penalty);

private:
    const SUMOTime myBegin;
    const SUMOTime myEnd;
    RONet& myNet;
    ODMatrix& myMatrix;
    SUMOAbstractRouter<ROEdge, ROVehicle>& myRouter;
    static std::map<const ROEdge* const, SUMOReal> myPenalties;
    static ROVehicle* myDefaultVehicle;

private:
    /// @brief Invalidated assignment operator
    ROMAAssignments& operator=(const ROMAAssignments& src);

};

#endif
