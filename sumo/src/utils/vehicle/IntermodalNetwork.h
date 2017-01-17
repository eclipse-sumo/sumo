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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
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
/// @brief the pedestrian network storing edges, connections and the mappings to the "real" edges
template<class E, class L, class N, class V>
class IntermodalNetwork {
private:
    typedef IntermodalEdge<E, L, N, V> _IntermodalEdge;
    typedef PedestrianEdge<E, L, N, V> _PedestrianEdge;
    typedef std::pair<_IntermodalEdge*, _IntermodalEdge*> EdgePair;

public:
    /* brief build the pedestrian network (once)
     * @param noE The number of edges in the dictionary of E
     */
    IntermodalNetwork(const std::vector<E*>& edges, int numericalID = 0) {
#ifdef IntermodalRouter_DEBUG_NETWORK
        std::cout << "initIntermodalNetwork\n";
#endif
        // build the Intermodal edges and the lookup tables
        bool haveSeenWalkingArea = false;
        for (typename std::vector<E*>::const_iterator i = edges.begin(); i != edges.end(); ++i) {
            const E* const edge = *i;
            if (edge->isInternal()) {
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
                addEdge(new _IntermodalEdge(edge->getID() + "_depart_connector", numericalID++, edge, "!connector"));
                myDepartLookup[edge].push_back(myEdges.back());
                addEdge(new _IntermodalEdge(edge->getID() + "_arrival_connector", numericalID++, edge, "!connector"));
                myArrivalLookup[edge].push_back(myEdges.back());
            }
        }

        // build the connections
        for (typename std::vector<E*>::const_iterator i = edges.begin(); i != edges.end(); ++i) {
            E* const edge = *i;
            const L* sidewalk = getSidewalk<E, L>(edge);
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
                std::vector<const L*> outgoing = sidewalk->getOutgoingLanes();
                // if one of the outgoing lanes is a walking area it must be used.
                // All other connections shall be ignored
                bool hasWalkingArea = false;
                for (typename std::vector<const L*>::iterator it = outgoing.begin(); it != outgoing.end(); ++it) {
                    const L* target = *it;
                    const E* targetEdge = &(target->getEdge());
                    if (targetEdge->isWalkingArea()) {
                        hasWalkingArea = true;
                        break;
                    }
                }
                for (typename std::vector<const L*>::iterator it = outgoing.begin(); it != outgoing.end(); ++it) {
                    const L* target = *it;
                    const E* targetEdge = &(target->getEdge());
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
            } else {
                // we have a network without pedestrian structures. Assume that
                // all sidewalks at a crossing are interconnected
                const N* toNode = edge->getToJunction();
                std::vector<const E*> outgoing = toNode->getOutgoing();
                for (typename std::vector<const E*>::iterator it = outgoing.begin(); it != outgoing.end(); ++it) {
                    // build forward and backward connections for all outgoing sidewalks
                    const E* targetEdge = *it;
                    const L* target = getSidewalk<E, L>(targetEdge);
                    if (targetEdge->isInternal() || target == 0) {
                        continue;
                    }
                    const EdgePair& targetPair = getBothDirections(targetEdge);
                    pair.first->addSuccessor(targetPair.first);
                    targetPair.second->addSuccessor(pair.second);
#ifdef IntermodalRouter_DEBUG_NETWORK
                    std::cout << "     " << pair.first->getID() << " -> " << targetPair.first->getID() << "\n";
                    std::cout << "     " << targetPair.second->getID() << " -> " << pair.second->getID() << "\n";
#endif
                }
                std::vector<const E*> incoming = toNode->getIncoming();
                for (typename std::vector<const E*>::iterator it = incoming.begin(); it != incoming.end(); ++it) {
                    // build forward-to-backward connections for all incoming sidewalks
                    const E* targetEdge = *it;
                    const L* target = getSidewalk<E, L>(targetEdge);
                    if (targetEdge->isInternal() || target == 0 || targetEdge == edge) {
                        continue;
                    }
                    const EdgePair& targetPair = getBothDirections(targetEdge);
                    pair.first->addSuccessor(targetPair.second); // change direction
#ifdef IntermodalRouter_DEBUG_NETWORK
                    std::cout << "     " << pair.first->getID() << " -> " << targetPair.second->getID() << "\n";
#endif
                }
                const N* fromNode = edge->getFromJunction();
                outgoing = fromNode->getOutgoing();
                for (typename std::vector<const E*>::iterator it = outgoing.begin(); it != outgoing.end(); ++it) {
                    // build backward-to-forward connections for all outgoing sidewalks at the fromNode
                    const E* targetEdge = *it;
                    const L* target = getSidewalk<E, L>(targetEdge);
                    if (targetEdge->isInternal() || target == 0 || targetEdge == edge) {
                        continue;
                    }
                    const EdgePair& targetPair = getBothDirections(targetEdge);
                    pair.second->addSuccessor(targetPair.first);
#ifdef IntermodalRouter_DEBUG_NETWORK
                    std::cout << "     " << pair.second->getID() << " -> " << targetPair.first->getID() << "\n";
#endif
                }
            }
            if (edge->isWalkingArea()) {
                continue;
            }
            // build connections from depart connector
            _IntermodalEdge* startConnector = getDepartEdge(edge);
            startConnector->addSuccessor(pair.first);
            startConnector->addSuccessor(pair.second);
            // build connections to arrival connector
            _IntermodalEdge* endConnector = getArrivalEdge(edge);
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
    const EdgePair& getBothDirections(const E* e) {
        typename std::map<const E*, EdgePair>::const_iterator it = myBidiLookup.find(e);
        if (it == myBidiLookup.end()) {
            assert(false);
            throw ProcessError("Edge '" + e->getID() + "' not found in pedestrian network '");
        }
        return (*it).second;
    }

    /// @brief Returns the departing Intermodal edge
    _IntermodalEdge* getDepartEdge(const E* e, const SUMOReal pos = -1.) {
        typename std::map<const E*, std::vector<_IntermodalEdge*> >::const_iterator it = myDepartLookup.find(e);
        if (it == myDepartLookup.end()) {
            throw ProcessError("Depart edge '" + e->getID() + "' not found in pedestrian network.");
        }
        const std::vector<_IntermodalEdge*>& splitList = it->second;
        typename std::vector<_IntermodalEdge*>::const_iterator splitIt = splitList.begin();
        SUMOReal totalLength = 0.;
        while (splitIt != splitList.end() && totalLength + (*splitIt)->getLength() + POSITION_EPS < pos) {
            totalLength += (*splitIt)->getLength();
            ++splitIt;
        }
        return *splitIt;
    }

    /// @brief Returns the arriving Intermodal edge
    _IntermodalEdge* getArrivalEdge(const E* e, const SUMOReal pos = -1.) {
        typename std::map<const E*, std::vector<_IntermodalEdge*> >::const_iterator it = myArrivalLookup.find(e);
        if (it == myArrivalLookup.end()) {
            throw ProcessError("Arrival edge '" + e->getID() + "' not found in pedestrian network.");
        }
        const std::vector<_IntermodalEdge*>& splitList = it->second;
        typename std::vector<_IntermodalEdge*>::const_iterator splitIt = splitList.begin();
        SUMOReal totalLength = 0.;
        while (splitIt != splitList.end() && totalLength + (*splitIt)->getLength() + POSITION_EPS < pos) {
            totalLength += (*splitIt)->getLength();
            ++splitIt;
        }
        return *splitIt;
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

};


#endif

/****************************************************************************/
