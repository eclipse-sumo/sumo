/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2007-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSRoutingEngine.h
/// @author  Michael Behrisch
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Tue, 04 Dec 2007
/// @version $Id$
///
// A device that performs vehicle rerouting based on current edge speeds
/****************************************************************************/
#ifndef MSRoutingEngine_h
#define MSRoutingEngine_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <set>
#include <vector>
#include <map>
#include <utils/common/SUMOTime.h>
#include <utils/common/WrappingCommand.h>
#include <utils/router/SUMOAbstractRouter.h>
#include <utils/router/AStarRouter.h>
#include <microsim/MSVehicle.h>
#include "MSDevice.h"

#ifdef HAVE_FOX
#include <utils/foxtools/FXWorkerThread.h>
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSRoutingEngine
 * @brief A device that performs vehicle rerouting based on current edge speeds
 *
 * The routing-device system consists of in-vehicle devices that perform a routing
 *  and a simulation-wide (static) methods for colecting edge weights.
 *
 * The edge weights container "myEdgeSpeeds" is pre-initialised as soon as one
 *  device is built and is kept updated via an event that adapts it to the current
 *  mean speed on the simulated network's edges.
 *
 * A device is assigned to a vehicle using the common explicit/probability - procedure.
 *
 * A device computes a new route for a vehicle as soon as the vehicle is inserted
 *  (within "enterLaneAtInsertion") - and, if the given period is larger than 0 - each
 *  x time steps where x is the period. This is triggered by an event that executes
 *  "wrappedRerouteCommandExecute".
 */
class MSRoutingEngine {
public:
    /// @brief intialize period edge weight update
    static void initWeightUpdate();

    /// @brief initialize the edge weights if not done before
    static void initEdgeWeights();

    /// @brief returns whether any routing actions take place
    static bool hasEdgeUpdates() {
        return myEdgeWeightSettingCommand != nullptr;
    }

    /// @brief Information when the last edge weight adaptation occurred
    static SUMOTime getLastAdaptation() {
        return myLastAdaptation;
    }

    /// @brief return the cached route or nullptr on miss
    static const MSRoute* getCachedRoute(const std::pair<const MSEdge*, const MSEdge*>& key);

    /// @brief initiate the rerouting, create router / thread pool on first use
    static void reroute(SUMOVehicle& vehicle, const SUMOTime currentTime, const bool onInit);

    /// @brief adapt the known travel time for an edge
    static void setEdgeTravelTime(const MSEdge* const edge, const double travelTime);

    /// @brief deletes the router instance
    static void cleanup();

    /// @brief returns whether any routing actions take place
    static bool isEnabled() {
        return !myWithTaz && myAdaptationInterval >= 0;
    }

    /// @brief return the router instance
    static SUMOAbstractRouter<MSEdge, SUMOVehicle>& getRouterTT(
        const MSEdgeVector& prohibited = MSEdgeVector());

    /** @brief Returns the effort to pass an edge
    *
    * This method is given to the used router in order to obtain the efforts
    *  to pass an edge from the internal edge weights container.
    *
    * The time is not used, here, as the current simulation state is
    *  used in an aggregated way.
    *
    * @param[in] e The edge for which the effort to be passed shall be returned
    * @param[in] v The vehicle that is rerouted
    * @param[in] t The time for which the effort shall be returned
    * @return The effort (time to pass in this case) for an edge
    * @see DijkstraRouter_ByProxi
    */
    static double getEffort(const MSEdge* const e, const SUMOVehicle* const v, double t);
    static double getEffortExtra(const MSEdge* const e, const SUMOVehicle* const v, double t);
    static SUMOAbstractRouter<MSEdge, SUMOVehicle>::Operation myEffortFunc;

    /// @brief return current travel speed assumption
    static double getAssumedSpeed(const MSEdge* edge);

#ifdef HAVE_FOX
    static void waitForAll();
    static void lock() {
        myThreadPool.lock();
    }
    static void unlock() {
        myThreadPool.unlock();
    }
    static bool isParallel() {
        return myThreadPool.size() > 0;
    }
#endif


private:
#ifdef HAVE_FOX
    /**
     * @class WorkerThread
     * @brief the thread which provides the router instance as context
     */
    class WorkerThread : public FXWorkerThread {
    public:
        WorkerThread(FXWorkerThread::Pool& pool,
                     SUMOAbstractRouter<MSEdge, SUMOVehicle>* router)
            : FXWorkerThread(pool), myRouter(router) {}
        SUMOAbstractRouter<MSEdge, SUMOVehicle>& getRouter() const {
            return *myRouter;
        }
        virtual ~WorkerThread() {
            stop();
            delete myRouter;
        }
    private:
        SUMOAbstractRouter<MSEdge, SUMOVehicle>* myRouter;
    };

    /**
     * @class RoutingTask
     * @brief the routing task which mainly calls reroute of the vehicle
     */
    class RoutingTask : public FXWorkerThread::Task {
    public:
        RoutingTask(SUMOVehicle& v, const SUMOTime time, const bool onInit)
            : myVehicle(v), myTime(time), myOnInit(onInit) {}
        void run(FXWorkerThread* context);
    private:
        SUMOVehicle& myVehicle;
        const SUMOTime myTime;
        const bool myOnInit;
    private:
        /// @brief Invalidated assignment operator.
        RoutingTask& operator=(const RoutingTask&);
    };
#endif

    /// @name Network state adaptation
    /// @{

    /** @brief Adapt edge efforts by the current edge states
     *
     * This method is called by the event handler at the end of a simulation
     *  step. The current edge weights are combined with the previously stored.
     *
     * @param[in] currentTime The current simulation time
     * @return The offset to the next call (always 1 in this case - edge weights are updated each time step)
     * @todo Describe how the weights are adapted
     * @see MSEventHandler
     * @see StaticCommand
     */
    static SUMOTime adaptEdgeEfforts(SUMOTime currentTime);
    /// @}


private:
    /// @brief The weights adaptation/overwriting command
    static Command* myEdgeWeightSettingCommand;

    /// @brief The container of edge speeds
    static std::vector<double> myEdgeSpeeds;

    /// @brief Information which weight prior edge efforts have
    static double myAdaptationWeight;

    /// @brief At which time interval the edge weights get updated
    static SUMOTime myAdaptationInterval;

    /// @brief Information when the last edge weight adaptation occurred
    static SUMOTime myLastAdaptation;

    /// @brief The number of steps for averaging edge speeds (ring-buffer)
    static int myAdaptationSteps;

    /// @brief The current index in the pastEdgeSpeed ring-buffer
    static int myAdaptationStepsIndex;

    /// @brief The container of edge speeds
    static std::vector<std::vector<double> > myPastEdgeSpeeds;

    /// @brief whether taz shall be used at initial rerouting
    static bool myWithTaz;

    /// @brief The router to use
    static SUMOAbstractRouter<MSEdge, SUMOVehicle>* myRouter;

    /// @brief The router to use by rerouter elements
    static AStarRouter<MSEdge, SUMOVehicle, SUMOAbstractRouterPermissions<MSEdge, SUMOVehicle> >* myRouterWithProhibited;

    /// @brief The container of pre-calculated routes
    static std::map<std::pair<const MSEdge*, const MSEdge*>, const MSRoute*> myCachedRoutes;

#ifdef HAVE_FOX
    static FXWorkerThread::Pool myThreadPool;
#endif

private:
    /// @brief Invalidated copy constructor.
    MSRoutingEngine(const MSRoutingEngine&);

    /// @brief Invalidated assignment operator.
    MSRoutingEngine& operator=(const MSRoutingEngine&);


};


#endif

/****************************************************************************/

