/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    IntermodalNetwork.h
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Robert Hilbrich
/// @date    Mon, 03 March 2014
/// @version $Id$
///
// The Edge definition for the Intermodal Router
/****************************************************************************/
#ifndef IntermodalNetwork_h
#define IntermodalNetwork_h


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
#include <utils/common/SUMOTime.h>
#include <utils/common/ToString.h>
#include <utils/common/Named.h>
#include "IntermodalEdge.h"

//#define IntermodalRouter_DEBUG_NETWORK


// ===========================================================================
// class definitions
// ===========================================================================
/// @brief the intermodal network storing edges, connections and the mappings to the "real" edges
template<class E, class L, class N, class V>
class IntermodalNetwork {
private:
    typedef IntermodalEdge<E, L, N, V> _IntermodalEdge;
    typedef PedestrianEdge<E, L, N, V> _PedestrianEdge;
    typedef std::pair<_IntermodalEdge*, _IntermodalEdge*> EdgePair;

public:
    /* @brief build the pedestrian part of the intermodal network (once)
     * @param edges The list of MSEdge or ROEdge to build from
     * @param numericalID the start number for the creation of new edges
     */
    IntermodalNetwork(const std::vector<E*>& edges, int numericalID = 0) {
#ifdef IntermodalRouter_DEBUG_NETWORK
        std::cout << "initIntermodalNetwork\n";
#endif
        const bool pedestrianOnly = numericalID == 0;
        // build the pedestrian edges and the depart / arrival connectors with lookup tables
        bool haveSeenWalkingArea = false;
        for (const E* const edge : edges) {
            if (edge->isInternal() || edge->isTazConnector()) {
                continue;
            }
            const L* lane = getSidewalk<E, L>(edge);
            if (lane != 0) {
                if (edge->isWalkingArea()) {
                    // only a single edge
                    addEdge(new _PedestrianEdge(numericalID++, edge, lane, true));
                    myBidiLookup[edge] = std::make_pair(myEdges.back(), myEdges.back());
                    myDepartLookup[edge].push_back(myEdges.back());
                    myArrivalLookup[edge].push_back(myEdges.back());
                    haveSeenWalkingArea = true;
                } else { // regular edge or crossing
                    // forward and backward edges
                    addEdge(new _PedestrianEdge(numericalID++, edge, lane, true));
                    addEdge(new _PedestrianEdge(numericalID++, edge, lane, false));
                    myBidiLookup[edge] = std::make_pair(myEdges[numericalID - 2], myEdges.back());
                }
            }
            if (!edge->isWalkingArea()) {
                // depart and arrival edges (the router can decide the initial direction to take and the direction to arrive from)
                _IntermodalEdge* const departConn = new _IntermodalEdge(edge->getID() + "_depart_connector", numericalID++, edge, "!connector");
                _IntermodalEdge* const arrivalConn = new _IntermodalEdge(edge->getID() + "_arrival_connector", numericalID++, edge, "!connector");
                addConnectors(departConn, arrivalConn, 0);
            }
        }

        // build the walking connectors if there are no walking areas
        for (const E* const edge : edges) {
            if (edge->isInternal() || edge->isTazConnector()) {
                continue;
            }
            if (haveSeenWalkingArea) {
                if (!pedestrianOnly && getSidewalk<E, L>(edge) == nullptr) {
                    const N* const node = edge->getToJunction();
                    if (myWalkingConnectorLookup.count(node) == 0) {
                        addEdge(new _IntermodalEdge(node->getID() + "_walking_connector", numericalID++, nullptr, "!connector"));
                        myWalkingConnectorLookup[node] = myEdges.back();
                    }
                }
            } else {
                for (const N* const node : {
                edge->getFromJunction(), edge->getToJunction()
                }) {
                    if (myWalkingConnectorLookup.count(node) == 0) {
                        addEdge(new _IntermodalEdge(node->getID() + "_walking_connector", numericalID++, nullptr, "!connector"));
                        myWalkingConnectorLookup[node] = myEdges.back();
                    }
                }
            }
        }
        // build the connections
        for (const E* const edge : edges) {
            const L* const sidewalk = getSidewalk<E, L>(edge);
            if (edge->isInternal() || sidewalk == 0) {
                continue;
            }
            // find all incoming and outgoing lanes for the sidewalk and
            // connect the corresponding IntermodalEdges
            const EdgePair& pair = getBothDirections(edge);
#ifdef IntermodalRouter_DEBUG_NETWORK
            std::cout << "  building connections from " << sidewalk->getID() << "\n";
#endif
            if (haveSeenWalkingArea) {
                const std::vector<const L*> outgoing = sidewalk->getOutgoingLanes();
                // if one of the outgoing lanes is a walking area it must be used.
                // All other connections shall be ignored
                // if it has no outgoing walking area, it probably is a walking area itself
                bool hasWalkingArea = false;
                for (const L* target : outgoing) {
                    if (target->getEdge().isWalkingArea()) {
                        hasWalkingArea = true;
                        break;
                    }
                }
                for (const L* target : outgoing) {
                    const E* const targetEdge = &(target->getEdge());
                    const bool used = (target == getSidewalk<E, L>(targetEdge)
                                       && (!hasWalkingArea || targetEdge->isWalkingArea()));
#ifdef IntermodalRouter_DEBUG_NETWORK
                    const L* potTarget = getSidewalk<E, L>(targetEdge);
                    std::cout << "   lane=" << (potTarget == 0 ? "NULL" : potTarget->getID()) << (used ? "(used)" : "") << "\n";
#endif
                    if (used) {
                        const EdgePair& targetPair = getBothDirections(targetEdge);
                        pair.first->addSuccessor(targetPair.first);
                        targetPair.second->addSuccessor(pair.second);
#ifdef IntermodalRouter_DEBUG_NETWORK
                        std::cout << "     " << pair.first->getID() << " -> " << targetPair.first->getID() << "\n";
                        std::cout << "     " << targetPair.second->getID() << " -> " << pair.second->getID() << "\n";
#endif
                    }
                }
            }
            // we may have a network without pedestrian structures or a car-only edge. Assume that
            // all sidewalks at a crossing are interconnected
            _IntermodalEdge* const toNodeConn = myWalkingConnectorLookup[edge->getToJunction()];
            if (toNodeConn != nullptr) {
                pair.first->addSuccessor(toNodeConn);
                toNodeConn->addSuccessor(pair.second);
            }
            _IntermodalEdge* const fromNodeConn = myWalkingConnectorLookup[edge->getFromJunction()];
            if (fromNodeConn != nullptr) {
                pair.second->addSuccessor(fromNodeConn);
                fromNodeConn->addSuccessor(pair.first);
            }
            if (edge->isWalkingArea()) {
                continue;
            }
            // build connections from depart connector
            _IntermodalEdge* startConnector = getDepartConnector(edge);
            startConnector->addSuccessor(pair.first);
            startConnector->addSuccessor(pair.second);
            // build connections to arrival connector
            _IntermodalEdge* endConnector = getArrivalConnector(edge);
            pair.first->addSuccessor(endConnector);
            pair.second->addSuccessor(endConnector);
#ifdef IntermodalRouter_DEBUG_NETWORK
            std::cout << "     " << startConnector->getID() << " -> " << pair.first->getID() << "\n";
            std::cout << "     " << startConnector->getID() << " -> " << pair.second->getID() << "\n";
            std::cout << "     " << pair.first->getID() << " -> " << endConnector->getID() << "\n";
            std::cout << "     " << pair.second->getID() << " -> " << endConnector->getID() << "\n";
#endif
        }
    }

    ~IntermodalNetwork() {
        for (typename std::vector<_IntermodalEdge*>::iterator it = myEdges.begin(); it != myEdges.end(); ++it) {
            delete *it;
        }
    }

    void addEdge(_IntermodalEdge* edge) {
        while ((int)myEdges.size() <= edge->getNumericalID()) {
            myEdges.push_back(0);
        }
        myEdges[edge->getNumericalID()] = edge;
    }

    void addConnectors(_IntermodalEdge* const depConn, _IntermodalEdge* const arrConn, const int splitIndex) {
        addEdge(depConn);
        addEdge(arrConn);
        myDepartLookup[depConn->getEdge()].insert(myDepartLookup[depConn->getEdge()].begin() + splitIndex, depConn);
        myArrivalLookup[arrConn->getEdge()].insert(myArrivalLookup[arrConn->getEdge()].begin() + splitIndex, arrConn);
    }

    const std::vector<_IntermodalEdge*>& getAllEdges() {
        return myEdges;
    }

    /// @brief Returns the pair of forward and backward edge
    const EdgePair& getBothDirections(const E* e) const {
        typename std::map<const E*, EdgePair>::const_iterator it = myBidiLookup.find(e);
        if (it == myBidiLookup.end()) {
            assert(false);
            throw ProcessError("Edge '" + e->getID() + "' not found in intermodal network '");
        }
        return (*it).second;
    }

    /// @brief Returns the departing intermodal edge
    _IntermodalEdge* getDepartEdge(const E* e, const double pos) const {
        typename std::map<const E*, std::vector<_IntermodalEdge*> >::const_iterator it = myDepartLookup.find(e);
        if (it == myDepartLookup.end()) {
            throw ProcessError("Depart edge '" + e->getID() + "' not found in intermodal network.");
        }
        const std::vector<_IntermodalEdge*>& splitList = it->second;
        typename std::vector<_IntermodalEdge*>::const_iterator splitIt = splitList.begin();
        double totalLength = 0.;
        while (splitIt != splitList.end() && totalLength + (*splitIt)->getLength() < pos) {
            totalLength += (*splitIt)->getLength();
            ++splitIt;
        }
        return *splitIt;
    }

    /// @brief Returns the departing intermodal connector at the given split offset
    _IntermodalEdge* getDepartConnector(const E* e, const int splitIndex = 0) const {
        return myDepartLookup.find(e)->second[splitIndex];
    }

    /// @brief Returns the arriving intermodal edge
    _IntermodalEdge* getArrivalEdge(const E* e, const double pos) const {
        typename std::map<const E*, std::vector<_IntermodalEdge*> >::const_iterator it = myArrivalLookup.find(e);
        if (it == myArrivalLookup.end()) {
            throw ProcessError("Arrival edge '" + e->getID() + "' not found in intermodal network.");
        }
        const std::vector<_IntermodalEdge*>& splitList = it->second;
        typename std::vector<_IntermodalEdge*>::const_iterator splitIt = splitList.begin();
        double totalLength = 0.;
        while (splitIt != splitList.end() && totalLength + (*splitIt)->getLength() < pos) {
            totalLength += (*splitIt)->getLength();
            ++splitIt;
        }
        return *splitIt;
    }

    /// @brief Returns the arriving intermodal connector at the given split offset
    _IntermodalEdge* getArrivalConnector(const E* e, const int splitIndex = 0) const {
        return myArrivalLookup.find(e)->second[splitIndex];
    }

    /// @brief Returns the outgoing pedestrian edge, which is either a walking area or a walking connector
    _IntermodalEdge* getWalkingConnector(const E* e) const {
        typename std::map<const N*, _IntermodalEdge*>::const_iterator it = myWalkingConnectorLookup.find(e->getToJunction());
        if (it == myWalkingConnectorLookup.end()) {
            const L* const sidewalk = getSidewalk<E, L>(e);
            if (e->isInternal() || sidewalk == 0) {
                return 0;
            }
            for (const L* target : sidewalk->getOutgoingLanes()) {
                if (target->getEdge().isWalkingArea()) {
                    return getBothDirections(&target->getEdge()).first;
                }
            }
            return 0;
        }
        return it->second;
    }


private:
    /// @brief the edge dictionary
    std::vector<_IntermodalEdge*> myEdges;

    /// @brief retrieve the forward and backward edge for the given input edge E
    std::map<const E*, EdgePair> myBidiLookup;

    /// @brief retrieve the depart edges for the given input edge E
    std::map<const E*, std::vector<_IntermodalEdge*> > myDepartLookup;

    /// @brief retrieve the arrival edges for the given input edge E
    std::map<const E*, std::vector<_IntermodalEdge*> > myArrivalLookup;

    /// @brief the walking connector edge (fake walking area)
    std::map<const N*, _IntermodalEdge*> myWalkingConnectorLookup;

};


#endif

/****************************************************************************/
