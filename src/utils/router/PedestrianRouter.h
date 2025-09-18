/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    PedestrianRouter.h
/// @author  Jakob Erdmann
/// @date    Mon, 03 March 2014
///
// The Pedestrian Router builds a special network and delegates to a SUMOAbstractRouter.
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/ToString.h>
#include "SUMOAbstractRouter.h"
#include "DijkstraRouter.h"
#include "IntermodalNetwork.h"

//#define PedestrianRouter_DEBUG_ROUTES


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class PedestrianRouter
 * The router for pedestrians (on a bidirectional network of sidewalks and crossings)
 */
template<class E, class L, class N, class V>
class PedestrianRouter : public SUMOAbstractRouter<E, IntermodalTrip<E, N, V> > {
private:
    typedef IntermodalEdge<E, L, N, V> _IntermodalEdge;
    typedef IntermodalNetwork<E, L, N, V> _IntermodalNetwork;
    typedef IntermodalTrip<E, N, V> _IntermodalTrip;
    typedef DijkstraRouter<_IntermodalEdge, _IntermodalTrip> _InternalRouter;

public:
    /// Constructor
    PedestrianRouter():
        SUMOAbstractRouter<E, _IntermodalTrip>("PedestrianRouter", true, nullptr, nullptr, false, false), myAmClone(false) {
        myPedNet = new _IntermodalNetwork(E::getAllEdges(), true);
        myInternalRouter = new _InternalRouter(myPedNet->getAllEdges(), true,
                                               gWeightsRandomFactor > 1 ? &_IntermodalEdge::getTravelTimeStaticRandomized : &_IntermodalEdge::getTravelTimeStatic,
                                               nullptr, false, nullptr, true);
    }

    PedestrianRouter(_IntermodalNetwork* net):
        SUMOAbstractRouter<E, _IntermodalTrip>("PedestrianRouterClone", true, nullptr, nullptr, false, false), myAmClone(true) {
        myPedNet = net;
        myInternalRouter = new _InternalRouter(myPedNet->getAllEdges(), true,
                                               gWeightsRandomFactor > 1 ? &_IntermodalEdge::getTravelTimeStaticRandomized : &_IntermodalEdge::getTravelTimeStatic,
                                               nullptr, false, nullptr, true);
    }

    /// Destructor
    virtual ~PedestrianRouter() {
        delete myInternalRouter;
        if (!myAmClone) {
            delete myPedNet;
        }
    }

    virtual SUMOAbstractRouter<E, _IntermodalTrip>* clone() {
        return new PedestrianRouter<E, L, N, V>(myPedNet);
    }

    /** @brief Builds the route between the given edges using the minimum effort at the given time
        The definition of the effort depends on the wished routing scheme */
    double compute(const E* from, const E* to, double departPos, double arrivalPos, double speed,
                   SUMOTime msTime, const N* onlyNode, const SUMOVTypeParameter& pars, std::vector<const E*>& into, bool allEdges = false) const {
        if (getSidewalk<E, L>(from) == 0) {
            WRITE_WARNINGF(TL("Departure edge '%' does not allow pedestrians."), from->getID());
            return false;
        }
        if (getSidewalk<E, L>(to) == 0) {
            WRITE_WARNINGF(TL("Destination edge '%' does not allow pedestrians."), to->getID());
            return false;
        }
        _IntermodalTrip trip(from, to, departPos, arrivalPos, speed, msTime, onlyNode, pars);
        std::vector<const _IntermodalEdge*> intoPed;
        const bool silent = allEdges; // no warning is needed when called from MSPModel_Striping
        const bool success = myInternalRouter->compute(myPedNet->getDepartConnector(from),
                             myPedNet->getArrivalConnector(to),
                             &trip, msTime, intoPed, silent);
        double time = 0.;
        if (success) {
            for (const _IntermodalEdge* pedEdge : intoPed) {
                if (pedEdge->includeInRoute(allEdges)) {
                    into.push_back(pedEdge->getEdge());
                }
                time += myInternalRouter->getEffort(pedEdge, &trip, time);
            }
        }
#ifdef PedestrianRouter_DEBUG_ROUTES
        std::cout << TIME2STEPS(msTime) << " trip from " << from->getID() << " to " << to->getID()
                  << " departPos=" << departPos
                  << " arrivalPos=" << arrivalPos
                  << " onlyNode=" << (onlyNode == 0 ? "NULL" : onlyNode->getID())
                  << " edges=" << toString(intoPed)
                  << " resultEdges=" << toString(into)
                  << " time=" << time
                  << "\n";
#endif
        return success ? time : -1.;
    }

    /** @brief Builds the route between the given edges using the minimum effort at the given time
        The definition of the effort depends on the wished routing scheme */
    bool compute(const E*, const E*, const _IntermodalTrip* const,
                 SUMOTime, std::vector<const E*>&, bool) {
        throw ProcessError(TL("Do not use this method"));
    }

    void prohibit(const std::map<const E*, double>& toProhibit) {
        std::map<const _IntermodalEdge*, double> toProhibitPE;
        for (auto item : toProhibit) {
            toProhibitPE[myPedNet->getBothDirections(item.first).first] = item.second;
            toProhibitPE[myPedNet->getBothDirections(item.first).second] = item.second;
        }
        myInternalRouter->prohibit(toProhibitPE);
    }

    double recomputeWalkCosts(const std::vector<const E*>& edges, double speed, double fromPos, double toPos, SUMOTime msTime, const SUMOVTypeParameter& pars, double& length) const {
        // edges are normal edges so we need to reconstruct paths across intersection
        if (edges.size() == 0) {
            length = 0;
            return 0;
        } else if (edges.size() == 1) {
            length = fabs(toPos - fromPos);
            return length / speed;
        } else {
            double cost = 0;
            int last = (int)edges.size() - 1;
            for (int i = 0; i < last; i++) {
                std::vector<const E*> into;
                const E* from = edges[i];
                const E* to = edges[i + 1];
                const double fp = (i == 0 ? fromPos : from->getLength() / 2);
                const double tp = (i == (last - 1) ? toPos : to->getLength() / 2);
                const N* node = getCommonNode(from, to);
                if (i == 0) {
                    if (node == from->getToJunction()) {
                        length += from->getLength() - fromPos;
                    } else {
                        length += fromPos;
                    }
                } else  {
                    length += from->getLength();
                }
                if (i == (last - 1)) {
                    if (node == to->getFromJunction()) {
                        length += toPos;
                    } else {
                        length += to->getLength() - toPos;
                    }
                }
                double time = this->compute(from, to, fp, tp, speed, msTime, node, pars, into, true);
                if (time >= 0) {
                    cost += time;
                    for (const E* edge : into) {
                        if (edge->isCrossing()) {
                            length += edge->getLength();
                        } else if (edge->isWalkingArea()) {
                            // this is wrong because the length is path-dependent
                            length += edge->getLength();
                        }
                    }
                } else {
                    throw ProcessError("Could not compute cost between edge '" + from->getID() + "' and edge '" + to->getID() + "'.");
                }
            }
            return cost;
        }
    }


private:
    const bool myAmClone;
    _InternalRouter* myInternalRouter;
    _IntermodalNetwork* myPedNet;

    const N* getCommonNode(const E* from, const E* to) const {
        if (from->getToJunction() == to->getFromJunction() || from->getToJunction() == to->getToJunction()) {
            return from->getToJunction();
        } else if (from->getFromJunction() == to->getFromJunction() || from->getFromJunction() == to->getToJunction()) {
            return from->getFromJunction();
        } else {
            return nullptr;
        }
    }

private:
    /// @brief Invalidated assignment operator
    PedestrianRouter& operator=(const PedestrianRouter& s);

};
