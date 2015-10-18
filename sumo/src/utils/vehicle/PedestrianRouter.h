/****************************************************************************/
/// @file    PedestrianRouter.h
/// @author  Jakob Erdmann
/// @date    Mon, 03 March 2014
/// @version $Id$
///
// The Pedestrian Router build a special network and (delegegates to a SUMOAbstractRouter)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef PedestrianRouter_h
#define PedestrianRouter_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/ToString.h>
#include "SUMOAbstractRouter.h"
#include "DijkstraRouterTT.h"
#include "PedestrianEdge.h"

//#define PedestrianRouter_DEBUG_ROUTES


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class PedestrianRouter
 * The router for pedestrians (on a bidirectional network of sidewalks and crossings)
 */
template<class E, class L, class N, class INTERNALROUTER>
class PedestrianRouter : public SUMOAbstractRouter<E, PedestrianTrip<E, N> > {
public:

    typedef PedestrianEdge<E, L, N> _PedestrianEdge;
    typedef PedestrianTrip<E, N> _PedestrianTrip;

    /// Constructor
    PedestrianRouter():
        SUMOAbstractRouter<E, _PedestrianTrip>(0, "PedestrianRouter") {
        _PedestrianEdge::initPedestrianNetwork(E::dictSize());
        myInternalRouter = new INTERNALROUTER(_PedestrianEdge::dictSize(), true, &_PedestrianEdge::getEffort);
    }

    /// Destructor
    virtual ~PedestrianRouter() {
        delete myInternalRouter;
    }

    virtual SUMOAbstractRouter<E, PedestrianTrip<E, N> >* clone() const {
        return new PedestrianRouter<E, L, N, INTERNALROUTER>();
    }

    /** @brief Builds the route between the given edges using the minimum effort at the given time
        The definition of the effort depends on the wished routing scheme */
    void compute(const E* from, const E* to, SUMOReal departPos, SUMOReal arrivalPos, SUMOReal speed,
                 SUMOTime msTime, const N* onlyNode, std::vector<const E*>& into, bool allEdges = false) {
        //startQuery();
        if (getSidewalk<E, L>(from) == 0) {
            WRITE_WARNING("Departure edge '" + from->getID() + "' does not allow pedestrians.");
            return;
        }
        if (getSidewalk<E, L>(to) == 0) {
            WRITE_WARNING("Destination edge '" + to->getID() + "' does not allow pedestrians.");
            return;
        }
        _PedestrianTrip trip(from, to, departPos, arrivalPos, speed, msTime, onlyNode);
        std::vector<const _PedestrianEdge*> intoPed;
        myInternalRouter->compute(_PedestrianEdge::getDepartEdge(from),
                                  _PedestrianEdge::getArrivalEdge(to), &trip, msTime, intoPed);
        for (size_t i = 0; i < intoPed.size(); ++i) {
            if (intoPed[i]->includeInRoute(allEdges)) {
                into.push_back(intoPed[i]->getEdge());
            }
        }
#ifdef PedestrianRouter_DEBUG_ROUTES
        SUMOReal time = msTime;
        for (size_t i = 0; i < intoPed.size(); ++i) {
            time += myInternalRouter->getEffort(intoPed[i], &trip, time);
        }
        std::cout << TIME2STEPS(msTime) << " trip from " << from->getID() << " to " << to->getID()
                  << " departPos=" << departPos
                  << " arrivalPos=" << arrivalPos
                  << " onlyNode=" << (onlyNode == 0 ? "NULL" : onlyNode->getID())
                  << " edges=" << toString(intoPed)
                  << " resultEdges=" << toString(into)
                  << " time=" << time
                  << "\n";
#endif
        //endQuery();
    }

    /** @brief Builds the route between the given edges using the minimum effort at the given time
        The definition of the effort depends on the wished routing scheme */
    void compute(const E*, const E*, const _PedestrianTrip* const,
                 SUMOTime, std::vector<const E*>&) {
        throw ProcessError("Do not use this method");
    }

    SUMOReal recomputeCosts(const std::vector<const E*>&, const _PedestrianTrip* const, SUMOTime) const {
        throw ProcessError("Do not use this method");
    }

    void prohibit(const std::vector<E*>& toProhibit) {
        std::vector<_PedestrianEdge*> toProhibitPE;
        for (typename std::vector<E*>::const_iterator it = toProhibit.begin(); it != toProhibit.end(); ++it) {
            toProhibitPE.push_back(_PedestrianEdge::getBothDirections(*it).first);
            toProhibitPE.push_back(_PedestrianEdge::getBothDirections(*it).second);
        }
        myInternalRouter->prohibit(toProhibitPE);
    }

private:
    INTERNALROUTER* myInternalRouter;


private:
    /// @brief Invalidated assignment operator
    PedestrianRouter& operator=(const PedestrianRouter& s);

};

// common specializations
template<class E, class L, class N>
class PedestrianRouterDijkstra : public PedestrianRouter < E, L, N,
        DijkstraRouterTT<PedestrianEdge<E, L, N>, PedestrianTrip<E, N>, prohibited_withPermissions<PedestrianEdge<E, L, N>, PedestrianTrip<E, N> > > > { };


/**
 * @class RouterProvider
 * The encapsulation of two router for vehicles and pedestrians
 */
template<class E, class V, class L, class N>
class RouterProvider {
public:
    RouterProvider(SUMOAbstractRouter<E, V>* vehRouter,
                   PedestrianRouterDijkstra<E, L, N>* pedRouter)
        : myVehRouter(vehRouter), myPedRouter(pedRouter) {}

    RouterProvider(const RouterProvider& original)
        : myVehRouter(original.getVehicleRouter().clone()),
        myPedRouter(static_cast<PedestrianRouterDijkstra<E, L, N>*>(original.getPedestrianRouter().clone())) {}

    SUMOAbstractRouter<E, V>& getVehicleRouter() const {
        return *myVehRouter;
    }

    PedestrianRouterDijkstra<E, L, N>& getPedestrianRouter() const {
        return *myPedRouter;
    }

    virtual ~RouterProvider() {
        delete myVehRouter;
        delete myPedRouter;
    }


private:
    SUMOAbstractRouter<E, V>* const myVehRouter;
    PedestrianRouterDijkstra<E, L, N>* const myPedRouter;
};


#endif

/****************************************************************************/
