/****************************************************************************/
/// @file    PedestrianEdge.h
/// @author  Jakob Erdmann
/// @date    Mon, 03 March 2014
/// @version $Id$
///
// The Edge definition for the Pedestrian Router
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
#ifndef PedestrianEdge_h
#define PedestrianEdge_h


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

#define TL_RED_PENALTY 20

//#define PedestrianRouter_DEBUG_NETWORK
//#define PedestrianRouter_DEBUG_EFFORTS


template <class E, class L>
inline const L* getSidewalk(const E* edge) {
    if (edge == 0) {
        return 0;
    }
    const std::vector<L*>& lanes = edge->getLanes();
    for (typename std::vector<L*>::const_iterator it = lanes.begin(); it != lanes.end(); ++it) {
        if ((*it)->allowsVehicleClass(SVC_PEDESTRIAN)) {
            return *it;
        }
    }
    return 0;
}


// ===========================================================================
// class definitions
// ===========================================================================

/// @brief the "vehicle" type that is given to the internal router (SUMOAbstractRouter)
template<class E, class N>
struct PedestrianTrip {

    PedestrianTrip(const E* _from, const E* _to, SUMOReal _departPos, SUMOReal _arrivalPos, SUMOReal _speed, SUMOTime _departTime, const N* _node) :
        from(_from),
        to(_to),
        node(_node),
        departPos(_departPos < 0 ? _from->getLength() + _departPos : _departPos),
        arrivalPos(_arrivalPos < 0 ? _to->getLength() + _arrivalPos : _arrivalPos),
        speed(_speed),
        departTime(_departTime)
    {}

    // exists just for debugging purposes
    std::string getID() const {
        return from->getID() + ":" + to->getID() + ":" + time2string(departTime);
    }


    inline SUMOVehicleClass getVClass() const {
        return SVC_PEDESTRIAN;
    }

    const E* from;
    const E* to;
    const N* node; // indicates whether only routing across this node shall be performed
    const SUMOReal departPos;
    const SUMOReal arrivalPos;
    const SUMOReal speed;
    const SUMOTime departTime;
private:
    /// @brief Invalidated assignment operator.
    PedestrianTrip& operator=(const PedestrianTrip&);
};


/// @brief the edge type that is given to the internal router (SUMOAbstractRouter)
template<class E, class L, class N>
class PedestrianEdge : public Named {
public:
    PedestrianEdge(unsigned int numericalID, const E* edge, const L* lane, bool forward, bool connector=false, bool car=false) :
        Named(edge->getID() + (edge->isWalkingArea() ? "" : (car ? "_car" :
                               ((forward ? "_fwd" : "_bwd") + std::string(connector ? "_connector" : ""))))),
        myNumericalID(numericalID),
        myEdge(edge),
        myLane(lane),
        myForward(forward),
        myAmConnector(connector),
        myAmCar(car) { }

    bool includeInRoute(bool allEdges) const {
        return !myAmConnector && (allEdges || (!myEdge->isCrossing() && !myEdge->isWalkingArea()));
    }

    bool isCar() const {
        return myAmCar;
    }

    const E* getEdge() const {
        return myEdge;
    }

    /// @name The interface as required by SUMOAbstractRouter routes
    /// @{

    unsigned int getNumericalID() const {
        return myNumericalID;
    }

/*    /// @brief Returns the PedstrianEdge with the given numericalID
    static const PedestrianEdge* dictionary(size_t index) {
        assert(index < myEdgeDict.size());
        return myEdgeDict[index];
    }
    */
    virtual void addSuccessor(PedestrianEdge* s) {
        myFollowingEdges.push_back(s);
    }

    const std::vector<PedestrianEdge*>& getSuccessors(SUMOVehicleClass /*vClass*/) const {
        // the network is already tailored for pedestrians. No need to check for permissions here
        return myFollowingEdges;
    }

    bool prohibits(const PedestrianTrip<E, N>* const trip) const {
        if (trip->node == 0) {
            // network only includes PedestrianEdges
            return false;
        } else {
            // limit routing to the surroundings of the specified node
            return (myEdge->getFromJunction() != trip->node
                    && myEdge->getToJunction() != trip->node);
        }
    }

    /// @}

    /*@brief the function called by RouterTT_direct
     * (distance is used as effort, effort is assumed to be independent of time
     */
    static SUMOReal getEffort(const PedestrianEdge* const edge, const PedestrianTrip<E, N>* const trip, SUMOReal time) {
        if (edge->myAmConnector) {
            return 0;
        }
        SUMOReal length = edge->myEdge->getLength();
        if (edge->myEdge == trip->from) {
            if (edge->myForward) {
                length -= trip->departPos;
            } else {
                length = trip->departPos;
            }
        }
        if (edge->myEdge == trip->to) {
            if (edge->myForward) {
                length = trip->arrivalPos;
            } else {
                length -= trip->arrivalPos;
            }
        }
        // ensure that 'normal' edges always have a higher weight than connector edges
        length = MAX2(length, POSITION_EPS);
        SUMOReal tlsDelay = 0;
        // @note pedestrian traffic lights should never have LINKSTATE_TL_REDYELLOW
        if (edge->myEdge->isCrossing() && edge->myLane->getIncomingLinkState() == LINKSTATE_TL_RED) {
            // red traffic lights occurring later in the route may be green by the time we arive
            tlsDelay += MAX2(SUMOReal(0), TL_RED_PENALTY - (time - STEPS2TIME(trip->departTime)));

        }
#ifdef PedestrianRouter_DEBUG_EFFORTS
        std::cout << " effort for " << trip->getID() << " at " << time << " edge=" << edge->getID() << " effort=" << length / trip->speed + tlsDelay << " l=" << length << " s=" << trip->speed << " tlsDelay=" << tlsDelay << "\n";
#endif
        return length / trip->speed + tlsDelay;
    }

private:
    /// @brief the index in myEdgeDict
    unsigned int myNumericalID;

    /// @brief  the original edge
    const E* myEdge;

    /// @brief  the original edge
    const L* myLane;

    /// @brief the direction of this edge
    const bool myForward;

    /// @brief whether it is a special departure / arrival edge
    const bool myAmConnector;

    /// @brief whether it is a car edge
    const bool myAmCar;

    /// @brief List of edges that may be approached from this edge
    std::vector<PedestrianEdge*> myFollowingEdges;

private:
    /// @brief Invalidated copy constructor
    PedestrianEdge(const PedestrianEdge& src);

    /// @brief Invalidated assignment operator
    PedestrianEdge& operator=(const PedestrianEdge& src);

};


/// @brief the pedestrian network storing edges, connections and the mappings to the "real" edges
template<class E, class L, class N>
class PedestrianNetwork {
    typedef std::pair<PedestrianEdge<E, L, N>*, PedestrianEdge<E, L, N>*> EdgePair;

public:
    /* brief build the pedestrian network (once)
     * @param noE The number of edges in the dictionary of E
     */
    PedestrianNetwork(const std::vector<E*>& edges, const bool addCarEdges=false) {
#ifdef PedestrianRouter_DEBUG_NETWORK
        std::cout << "initPedestrianNetwork\n";
#endif
        // build the Pedestrian edges and the lookup tables
        bool haveSeenWalkingArea = false;
        unsigned int numericalID = 0;
        for (typename std::vector<E*>::const_iterator i = edges.begin(); i != edges.end(); ++i) {
            E* const edge = *i;
            const L* lane = getSidewalk<E, L>(edge);
            if (edge->isInternal() || lane == 0) {
                continue;
            } else if (edge->isWalkingArea()) {
                // only a single edge
                myEdgeDict.push_back(new PedestrianEdge<E, L, N>(numericalID++, edge, lane, true));
                myBidiLookup[edge] = std::make_pair(myEdgeDict.back(), myEdgeDict.back());
                myFromToLookup[edge] = std::make_pair(myEdgeDict.back(), myEdgeDict.back());
                haveSeenWalkingArea = true;
            } else { // regular edge or crossing
                // forward and backward edges
                myEdgeDict.push_back(new PedestrianEdge<E, L, N>(numericalID++, edge, lane, true));
                myEdgeDict.push_back(new PedestrianEdge<E, L, N>(numericalID++, edge, lane, false));
                myBidiLookup[edge] = std::make_pair(myEdgeDict[numericalID - 2], myEdgeDict.back());
                // depart and arrival edges for (the router can decide the initial direction to take and the direction to arrive from)
                myEdgeDict.push_back(new PedestrianEdge<E, L, N>(numericalID++, edge, lane, true, true));
                myEdgeDict.push_back(new PedestrianEdge<E, L, N>(numericalID++, edge, lane, false, true));
                myFromToLookup[edge] = std::make_pair(myEdgeDict[numericalID - 2], myEdgeDict.back());
            }
            if (addCarEdges) {
                myEdgeDict.push_back(new PedestrianEdge<E, L, N>(numericalID++, edge, lane, true, false, true));
                myCarLookup[edge] = myEdgeDict.back();
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
            // connect the corresponding PedestrianEdges
            const EdgePair& pair = getBothDirections(edge);
#ifdef PedestrianRouter_DEBUG_NETWORK
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
#ifdef PedestrianRouter_DEBUG_NETWORK
                    const L* potTarget = getSidewalk<E, L>(targetEdge);
                    std::cout << "   lane=" << (potTarget == 0 ? "NULL" : potTarget->getID()) << (used ? "(used)" : "") << "\n";
#endif
                    if (used) {
                        const EdgePair& targetPair = getBothDirections(targetEdge);
                        pair.first->addSuccessor(targetPair.first);
                        targetPair.second->addSuccessor(pair.second);
#ifdef PedestrianRouter_DEBUG_NETWORK
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
#ifdef PedestrianRouter_DEBUG_NETWORK
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
#ifdef PedestrianRouter_DEBUG_NETWORK
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
#ifdef PedestrianRouter_DEBUG_NETWORK
                    std::cout << "     " << pair.second->getID() << " -> " << targetPair.first->getID() << "\n";
#endif
                }
            }
            if (edge->isWalkingArea()) {
                continue;
            }
            // build connections from depart connector
            PedestrianEdge<E, L, N>* startConnector = getDepartEdge(edge);
            startConnector->addSuccessor(pair.first);
            startConnector->addSuccessor(pair.second);
            // build connections to arrival connector
            PedestrianEdge<E, L, N>* endConnector = getArrivalEdge(edge);
            pair.first->addSuccessor(endConnector);
            pair.second->addSuccessor(endConnector);
            if (addCarEdges) {
                // build connections from car edge
                PedestrianEdge<E, L, N>* carEdge = getCarEdge(edge);
                carEdge->addSuccessor(pair.first);
                carEdge->addSuccessor(pair.second);
                const std::vector<E*>& successors = edge->getSuccessors();
                for (std::vector<E*>::const_iterator it = successors.begin(); it != successors.end(); ++it) {
                    carEdge->addSuccessor(getCarEdge(*it));
                }
                startConnector->addSuccessor(carEdge);
            }
#ifdef PedestrianRouter_DEBUG_NETWORK
            std::cout << "     " << startConnector->getID() << " -> " << pair.first->getID() << "\n";
            std::cout << "     " << startConnector->getID() << " -> " << pair.second->getID() << "\n";
            std::cout << "     " << pair.first->getID() << " -> " << endConnector->getID() << "\n";
            std::cout << "     " << pair.second->getID() << " -> " << endConnector->getID() << "\n";
#endif
        }
    }

    ~PedestrianNetwork() {
        myFromToLookup.clear();
        myBidiLookup.clear();
        for (typename std::vector<PedestrianEdge<E, L, N>*>::iterator it = myEdgeDict.begin(); it != myEdgeDict.end(); ++it) {
            delete *it;
        }
        myEdgeDict.clear();
    }

    const std::vector<PedestrianEdge<E, L, N>*>& getAllEdges() {
        return myEdgeDict;
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

    /// @brief Returns the departing Pedestrian edge
    PedestrianEdge<E, L, N>* getDepartEdge(const E* e) {
        typename std::map<const E*, EdgePair>::const_iterator it = myFromToLookup.find(e);
        if (it == myFromToLookup.end()) {
            throw ProcessError("Depart edge '" + e->getID() + "' not found in pedestrian network.");
        }
        return (*it).second.first;
    }

    /// @brief Returns the arriving Pedestrian edge
    PedestrianEdge<E, L, N>* getArrivalEdge(const E* e) {
        typename std::map<const E*, EdgePair>::const_iterator it = myFromToLookup.find(e);
        if (it == myFromToLookup.end()) {
            throw ProcessError("Arrival edge '" + e->getID() + "' not found in pedestrian network.");
        }
        return (*it).second.second;
    }

    /// @brief Returns the associated car edge
    PedestrianEdge<E, L, N>* getCarEdge(const E* e) {
        typename std::map<const E*, PedestrianEdge<E, L, N>*>::const_iterator it = myCarLookup.find(e);
        if (it == myCarLookup.end()) {
            throw ProcessError("Edge '" + e->getID() + "' not found in pedestrian network.");
        }
        return it->second;
    }


private:
    /// @brief the edge dictionary
    std::vector<PedestrianEdge<E, L, N>*> myEdgeDict;

    /// @brief retrieve the forward and backward edge for the given input edge E
    std::map<const E*, EdgePair> myBidiLookup;

    /// @brief retrieve the depart and arrival edge for the given input edge E
    std::map<const E*, EdgePair> myFromToLookup;

    /// @brief retrieve the car edge for the given input edge E
    std::map<const E*, PedestrianEdge<E, L, N>*> myCarLookup;

};


#endif

/****************************************************************************/
