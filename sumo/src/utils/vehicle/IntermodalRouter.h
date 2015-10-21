/****************************************************************************/
/// @file    IntermodalRouter.h
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
#ifndef IntermodalRouter_h
#define IntermodalRouter_h


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

//#define IntermodalRouter_DEBUG_ROUTES


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class IntermodalRouter
 * The router for pedestrians (on a bidirectional network of sidewalks and crossings)
 */
template<class E, class L, class N, class V, class INTERNALROUTER>
class IntermodalRouter : public SUMOAbstractRouter<E, PedestrianTrip<E, N, V> > {
private:

    typedef PedestrianEdge<E, L, N, V> _PedestrianEdge;
    typedef PedestrianNetwork<E, L, N, V> _PedestrianNetwork;
    typedef PedestrianTrip<E, N, V> _PedestrianTrip;

public:
    /// Constructor
    IntermodalRouter():
        SUMOAbstractRouter<E, _PedestrianTrip>(0, "IntermodalRouter"), myAmClone(false) {
        myPedNet = new _PedestrianNetwork(E::getAllEdges(), true);
        myInternalRouter = new INTERNALROUTER(myPedNet->getAllEdges(), true, &_PedestrianEdge::getEffort);
    }

    IntermodalRouter(_PedestrianNetwork* net):
        SUMOAbstractRouter<E, _PedestrianTrip>(0, "PedestrianRouter"), myAmClone(true) {
        myPedNet = net;
        myInternalRouter = new INTERNALROUTER(myPedNet->getAllEdges(), true, &_PedestrianEdge::getEffort);
    }

    /// Destructor
    virtual ~IntermodalRouter() {
        delete myInternalRouter;
        if (!myAmClone) {
            delete myPedNet;
        }
    }

    virtual SUMOAbstractRouter<E, PedestrianTrip<E, N, V> >* clone() const {
        return new IntermodalRouter<E, L, N, V, INTERNALROUTER>();
    }

    /** @brief Builds the route between the given edges using the minimum effort at the given time
        The definition of the effort depends on the wished routing scheme */
    void compute(const E* from, const E* to, SUMOReal departPos, SUMOReal arrivalPos, SUMOReal speed,
                 const V* const vehicle, SUMOTime msTime, std::vector<std::pair<std::string, std::vector<const E*> > >& into) {
        //startQuery();
        _PedestrianTrip trip(from, to, departPos, arrivalPos, speed, msTime, 0, vehicle);
        std::vector<const _PedestrianEdge*> intoPed;
        myInternalRouter->compute(myPedNet->getDepartEdge(from),
                                  myPedNet->getArrivalEdge(to), &trip, msTime, intoPed);
        if (!intoPed.empty()) {
            into.push_back(std::make_pair(vehicle->getID(), std::vector<const E*>()));
            for (size_t i = 0; i < intoPed.size(); ++i) {
                if (intoPed[i]->includeInRoute(false)) {
                    if (into.size() == 1 && !intoPed[i]->isCar()) {
                        into.push_back(std::make_pair("", std::vector<const E*>()));
                    }
                    into.back().second.push_back(intoPed[i]->getEdge());
                }
            }
        }
#ifdef IntermodalRouter_DEBUG_ROUTES
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
            toProhibitPE.push_back(myPedNet->getBothDirections(*it).first);
            toProhibitPE.push_back(myPedNet->getBothDirections(*it).second);
            toProhibitPE.push_back(myPedNet->getCarEdge(*it));
        }
        myInternalRouter->prohibit(toProhibitPE);
    }

private:
    const bool myAmClone;
    INTERNALROUTER* myInternalRouter;
    _PedestrianNetwork* myPedNet;


private:
    /// @brief Invalidated assignment operator
    IntermodalRouter& operator=(const IntermodalRouter& s);

};

// common specializations
template<class E, class L, class N, class V>
class IntermodalRouterDijkstra : public IntermodalRouter < E, L, N, V,
        DijkstraRouterTT<PedestrianEdge<E, L, N, V>, PedestrianTrip<E, N, V>, prohibited_withPermissions<PedestrianEdge<E, L, N, V>, PedestrianTrip<E, N, V> > > > { };


#endif

/****************************************************************************/
