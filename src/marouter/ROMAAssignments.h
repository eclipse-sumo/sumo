/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    ROMAAssignments.h
/// @author  Yun-Pang Floetteroed
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    Feb 2013
///
// Assignment methods
/****************************************************************************/
#pragma once
#include <config.h>

#include <atomic>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>
#include <utils/router/RouteCostCalculator.h>
#include <utils/router/SUMOAbstractRouter.h>
#include <utils/common/SUMOTime.h>

// ===========================================================================
// class declarations
// ===========================================================================
class RONet;
class ODMatrix;
struct ODCell;
class Distribution_Points;
class ROEdge;
class ROMAEdge;
class RORoute;
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
    ROMAAssignments(const SUMOTime begin, const SUMOTime end, const bool additiveTraffic,
                    const double adaptionFactor, const int maxAlternatives, const bool defaultCapacities,
                    RONet& net, ODMatrix& matrix, SUMOAbstractRouter<ROEdge, ROVehicle>& router,
                    OutputDevice* netloadOutput);

    /// Destructor
    ~ROMAAssignments();

    ROVehicle* getDefaultVehicle() const {
        return myDefaultVehicle;
    }

    // @brief calculate edge capacity for the given edge
    double getCapacity(const ROEdge* edge) const;

    // @brief calculate edge travel time for the given edge and number of vehicles per hour
    double capacityConstraintFunction(const ROEdge* edge, const double flow) const;

    // @brief clear effort storage
    void resetFlows();

    // @brief Writes the travel times for a single interval
    void writeInterval(const SUMOTime begin, const SUMOTime end);

    // @brief incremental method
    void incremental(const int numIter, const bool verbose);

    // @brief UE method
    void ue();

    // @brief SUE method
    void sue(const int maxOuterIteration, const int maxInnerIteration, const int kPaths, const double penalty, const double tolerance, const std::string routeChoiceMethod);

    /** @brief Returns the effort to pass an edge including penalties
     *
     * This method is given to the used router in order to obtain the efforts
     *  to pass an edge from the internal edge weights container.
     *
     * @param[in] e The edge for which the effort to be passed shall be returned
     * @param[in] v The (default) vehicle that is routed
     * @param[in] t The time for which the effort shall be returned
     * @return The effort (time to pass in this case) for an edge
     * @see DijkstraRouter_ByProxi
     */
    static double getPenalizedEffort(const ROEdge* const e, const ROVehicle* const v, double t);

    /** @brief Returns the traveltime on an edge including penalties
     *
     * This method is given to the used router in order to obtain the efforts
     *  to pass an edge from the internal edge weights container.
     *
     * @param[in] e The edge for which the effort to be passed shall be returned
     * @param[in] v The (default) vehicle that is routed
     * @param[in] t The time for which the effort shall be returned
     * @return The effort (time to pass in this case) for an edge
     * @see DijkstraRouter_ByProxi
     */
    static double getPenalizedTT(const ROEdge* const e, const ROVehicle* const v, double t);

    /** @brief Returns the traveltime on an edge without penalties
     *
     * This method is given to the used router in order to obtain the efforts
     *  to pass an edge from the internal edge weights container.
     *
     * @param[in] e The edge for which the effort to be passed shall be returned
     * @param[in] v The (default) vehicle that is routed
     * @param[in] t The time for which the effort shall be returned
     * @return The effort (time to pass in this case) for an edge
     * @see DijkstraRouter_ByProxi
     */
    static double getTravelTime(const ROEdge* const e, const ROVehicle* const v, double t);

private:
    /// @brief add a route and check for duplicates
    bool addRoute(const ConstROEdgeVector& edges, std::vector<std::shared_ptr<RORoute> >& paths, std::string routeId, double prob);

    const ConstROEdgeVector computePath(ODCell* cell, const SUMOTime time = 0, const double probability = 0., SUMOAbstractRouter<ROEdge, ROVehicle>* router = nullptr, bool setBulkMode = false);

    /// @brief compute shortest paths for all cells grouped by origin using bulk routing
    bool computeBulkPaths(const std::map<std::string, std::vector<ODCell*> >& cellsByOrigin);

    /// @brief get the k shortest paths
    void getKPaths(const int kPaths, const double penalty);

private:
    const SUMOTime myBegin;
    const SUMOTime myEnd;
    const bool myAdditiveTraffic;
    const double myAdaptionFactor;
    const int myMaxAlternatives;
    const bool myUseDefaultCapacities;
    RONet& myNet;
    ODMatrix& myMatrix;
    SUMOAbstractRouter<ROEdge, ROVehicle>& myRouter;
    static thread_local std::unordered_map<const ROEdge*, double> myPenalties;
    ROVehicle* myDefaultVehicle;
    OutputDevice* const myNetloadOutput;

#ifdef HAVE_FOX
private:
    class RoutingTask : public MFXWorkerThread::Task {
    public:
        RoutingTask(ROMAAssignments& assign, ODCell* c, const SUMOTime begin, const double linkFlow, const bool setBulkMode = false)
            : myAssign(assign), myCell(c), myBegin(begin), myLinkFlow(linkFlow), mySetBulkMode(setBulkMode) {}
        void run(MFXWorkerThread* context);
    private:
        ROMAAssignments& myAssign;
        ODCell* const myCell;
        const SUMOTime myBegin;
        const double myLinkFlow;
        const bool mySetBulkMode;
    private:
        /// @brief Invalidated assignment operator.
        RoutingTask& operator=(const RoutingTask&) = delete;
    };

    class SUEBulkOriginTask : public MFXWorkerThread::Task {
    public:
        SUEBulkOriginTask(ROMAAssignments& assign, const std::vector<ODCell*>& cells, std::atomic<bool>& newRoute)
            : myAssign(assign), myCells(cells), myNewRoute(newRoute) {}
        void run(MFXWorkerThread* context);
    private:
        ROMAAssignments& myAssign;
        const std::vector<ODCell*>& myCells;
        std::atomic<bool>& myNewRoute;
    private:
        /// @brief Invalidated assignment operator.
        SUEBulkOriginTask& operator=(const SUEBulkOriginTask&) = delete;
    };

    class KPathsTask : public MFXWorkerThread::Task {
    public:
        KPathsTask(ROMAAssignments& assign, const std::vector<ODCell*>& cells, const std::vector<size_t>& prevSizes,
                   const int kPaths, const double penalty)
            : myAssign(assign), myCells(cells), myPrevSizes(prevSizes), myKPaths(kPaths), myPenalty(penalty) {}
        void run(MFXWorkerThread* context);
    private:
        ROMAAssignments& myAssign;
        const std::vector<ODCell*>& myCells;
        const std::vector<size_t>& myPrevSizes;
        const int myKPaths;
        const double myPenalty;
    private:
        /// @brief Invalidated assignment operator.
        KPathsTask& operator=(const KPathsTask&) = delete;
    };

    class SUEInnerTask : public MFXWorkerThread::Task {
    public:
        SUEInnerTask(ROMAAssignments& assign, const std::vector<ODCell*>& cells,
                     const size_t beginIdx, const size_t endIdx,
                     RouteCostCalculator<RORoute, ROEdge, ROVehicle>& calc)
            : myAssign(assign), myCells(cells), myBeginIdx(beginIdx), myEndIdx(endIdx), myCalc(calc) {}
        void run(MFXWorkerThread* context);
    private:
        ROMAAssignments& myAssign;
        const std::vector<ODCell*>& myCells;
        const size_t myBeginIdx;
        const size_t myEndIdx;
        RouteCostCalculator<RORoute, ROEdge, ROVehicle>& myCalc;
    private:
        /// @brief Invalidated assignment operator.
        SUEInnerTask& operator=(const SUEInnerTask&) = delete;
    };
#endif


private:
    /// @brief Invalidated assignment operator
    ROMAAssignments& operator=(const ROMAAssignments& src) = delete;

};
