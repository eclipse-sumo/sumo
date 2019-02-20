/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    CHRouterWrapper.h
/// @author  Jakob Erdmann
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    March 2012
/// @version $Id$
///
// Wraps multiple CHRouters for different vehicle types
/****************************************************************************/
#ifndef CHRouterWrapper_h
#define CHRouterWrapper_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <functional>
#include <vector>
#include <set>
#include <limits>
#include <algorithm>
#include <iterator>
#include <utils/common/SysUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StdDefs.h>
#include <utils/router/SUMOAbstractRouter.h>
#include <utils/common/SUMOVehicleClass.h>
#include "CHRouter.h"

#ifdef HAVE_FOX
#include <utils/foxtools/FXWorkerThread.h>
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class CHRouterWrapper
 * @brief Computes the shortest path through a contracted network
 *
 * The template parameters are:
 * @param E The edge class to use (MSEdge/ROEdge)
 * @param V The vehicle class to use (MSVehicle/ROVehicle)
 * @param PF The prohibition function to use (prohibited_withPermissions/noProhibitions)
 *
 * The router is edge-based. It must know the number of edges for internal reasons
 *  and whether a missing connection between two given edges (unbuild route) shall
 *  be reported as an error or as a warning.
 *
 */
template<class E, class V, class BASE>
class CHRouterWrapper: public BASE {

public:
    /// Type of the function that is used to retrieve the edge effort.
    typedef double(* Operation)(const E* const, const V* const, double);

    /** @brief Constructor
     */
    CHRouterWrapper(const std::vector<E*>& edges, const bool ignoreErrors, typename BASE::Operation operation,
                    const SUMOTime begin, const SUMOTime end, const SUMOTime weightPeriod, const int numThreads) :
        BASE("CHRouterWrapper", ignoreErrors, operation),
        myEdges(edges),
        myIgnoreErrors(ignoreErrors),
        myBegin(begin),
        myEnd(end),
        myWeightPeriod(weightPeriod),
        myMaxNumInstances(numThreads) {
    }

    ~CHRouterWrapper() {
        for (typename RouterMap::iterator i = myRouters.begin(); i != myRouters.end(); ++i) {
            for (typename std::vector<CHRouterType*>::iterator j = i->second.begin(); j != i->second.end(); ++j) {
                delete *j;
            }
        }
    }


    virtual SUMOAbstractRouter<E, V>* clone() {
        CHRouterWrapper<E, V, BASE>* clone = new CHRouterWrapper<E, V, BASE>(myEdges, myIgnoreErrors, this->myOperation, myBegin, myEnd, myWeightPeriod, myMaxNumInstances);
        for (typename RouterMap::iterator i = myRouters.begin(); i != myRouters.end(); ++i) {
            for (typename std::vector<CHRouterType*>::iterator j = i->second.begin(); j != i->second.end(); ++j) {
                clone->myRouters[i->first].push_back(static_cast<CHRouterType*>((*j)->clone()));
            }
        }
        return clone;
    }


    bool compute(const E* from, const E* to, const V* const vehicle,
                 SUMOTime msTime, std::vector<const E*>& into, bool silent = false) {
        const std::pair<const SUMOVehicleClass, const double> svc = std::make_pair(vehicle->getVClass(), vehicle->getMaxSpeed());
        int index = 0;
        int numIntervals = 1;
#ifdef HAVE_FOX
        if (myMaxNumInstances >= 2 && myEnd < std::numeric_limits<int>::max()) {
            index = (int)((msTime - myBegin) / myWeightPeriod);
            numIntervals = (int)((myEnd - myBegin) / myWeightPeriod);
            if (numIntervals > 0) {
                while ((int)myThreadPool.size() < myMaxNumInstances) {
                    new FXWorkerThread(myThreadPool);
                }
            } else {
                // this covers the cases of negative (unset) end time and unset weight period (no weight file)
                numIntervals = 1;
            }
        }
#endif
        if (myRouters.count(svc) == 0) {
            // create new router for the given permissions and maximum speed
            // XXX a new router may also be needed if vehicles differ in speed factor
            for (int i = 0; i < numIntervals; i++) {
                myRouters[svc].push_back(new CHRouterType(
                                             myEdges, myIgnoreErrors, &E::getTravelTimeStatic, svc.first, myWeightPeriod, false));
#ifdef HAVE_FOX
                if (myThreadPool.size() > 0) {
                    myThreadPool.add(new ComputeHierarchyTask(myRouters[svc].back(), vehicle, myBegin + i * myWeightPeriod));
                }
#endif
            }
#ifdef HAVE_FOX
            if (myThreadPool.size() > 0) {
                myThreadPool.waitAll();
            }
#endif
        }
        return myRouters[svc][index]->compute(from, to, vehicle, msTime, into, silent);
    }


private:
    typedef CHRouter<E, V, SUMOAbstractRouter<E, V> > CHRouterType;

#ifdef HAVE_FOX
private:
    class ComputeHierarchyTask : public FXWorkerThread::Task {
    public:
        ComputeHierarchyTask(CHRouterType* router, const V* const vehicle, const SUMOTime msTime)
            : myRouter(router), myVehicle(vehicle), myStartTime(msTime) {}
        void run(FXWorkerThread* /* context */) {
            myRouter->buildContractionHierarchy(myStartTime, myVehicle);
        }
    private:
        CHRouterType* myRouter;
        const V* const myVehicle;
        const SUMOTime myStartTime;
    private:
        /// @brief Invalidated assignment operator.
        ComputeHierarchyTask& operator=(const ComputeHierarchyTask&);
    };


private:
    /// @brief for multi threaded routing
    FXWorkerThread::Pool myThreadPool;
#endif

private:
    typedef std::map<std::pair<const SUMOVehicleClass, const double>, std::vector<CHRouterType*> > RouterMap;

    RouterMap myRouters;

    /// @brief all edges with numerical ids
    const std::vector<E*>& myEdges;

    const bool myIgnoreErrors;

    const SUMOTime myBegin;
    const SUMOTime myEnd;
    const SUMOTime myWeightPeriod;
    const int myMaxNumInstances;
};


#endif

/****************************************************************************/

