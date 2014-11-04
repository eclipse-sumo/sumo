/****************************************************************************/
/// @file    PedestrianRouter.h
/// @author  Jakob Erdmann
/// @date    Mon, 03 March 2014
/// @version $Id$
///
// The Pedestrian Router build a special network and (delegegates to a SUMOAbstractRouter)
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
#include <utils/common/SysUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/ToString.h>
#include <utils/common/Named.h>
#include <utils/vehicle/SUMOAbstractRouter.h>
#include <utils/vehicle/DijkstraRouterTT.h>
#include <utils/vehicle/AStarRouter.h>

#define TL_RED_PENALTY 20

//#define PedestrianRouter_DEBUG_NETWORK
//#define PedestrianRouter_DEBUG_ROUTES
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

    PedestrianTrip(const E* _from, const E* _to, SUMOReal _departPos, SUMOReal _arrivalPos, SUMOReal _speed, SUMOReal _departTime, const N* _node) :
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
        return from->getID() + ":" + to->getID() + ":" + toString(departTime);
    }

    const E* from;
    const E* to;
    const N* node; // indicates whether only routing across this node shall be performed
    const SUMOReal departPos;
    const SUMOReal arrivalPos;
    const SUMOReal speed;
    const SUMOReal departTime;
private:
    /// @brief Invalidated assignment operator.
    PedestrianTrip& operator=(const PedestrianTrip&);
};


/// @brief the edge type that is given to the internal router (SUMOAbstractRouter)
template<class E, class L, class N>
class PedestrianEdge : public Named {
    typedef std::pair<PedestrianEdge*, PedestrianEdge*> EdgePair;
    /* brief build the pedestrian network (once)
     * @param noE The number of edges in the dictionary of E
     */

public:
    static size_t dictSize() {
        return myEdgeDict.size();
    }

    static void cleanup() {
        myFromToLookup.clear();
        myBidiLookup.clear();
        myEdgeDict.clear();
    }

    static void initPedestrianNetwork(size_t noE) {
        if (myEdgeDict.size() > 0) {
            return;
        }
#ifdef PedestrianRouter_DEBUG_NETWORK
        std::cout << "initPedestrianNetwork\n";
#endif
        // build the Pedestrian edges
        unsigned int numericalID = 0;
        for (size_t i = 0; i < noE; i++) {
            E* edge = E::dictionary(i);
            const L* lane = getSidewalk<E, L>(edge);
            if (edge->isInternal() || lane == 0) {
                continue;
            } else if (edge->isWalkingArea()) {
                // only a single edge
                myEdgeDict.push_back(PedestrianEdge(numericalID++, edge, lane, true));
            } else { // regular edge or crossing
                // forward and backward edges
                myEdgeDict.push_back(PedestrianEdge(numericalID++, edge, lane, true));
                myEdgeDict.push_back(PedestrianEdge(numericalID++, edge, lane, false));
                // depart and arrival edges for (the router can decide the initial direction to take and the direction to arrive from)
                myEdgeDict.push_back(PedestrianEdge(numericalID++, edge, lane, true, true));
                myEdgeDict.push_back(PedestrianEdge(numericalID++, edge, lane, false, true));
            }

        }
        // build the lookup tables after myEdgeDict is complete
        numericalID = 0;
        for (size_t i = 0; i < noE; i++) {
            E* edge = E::dictionary(i);
            const L* lane = getSidewalk<E, L>(edge);
            if (edge->isInternal() || lane == 0) {
                continue;
            } else if (edge->isWalkingArea()) {
                // only a single edge and no connector edges
                myBidiLookup[edge] = std::make_pair(&myEdgeDict[numericalID], &myEdgeDict[numericalID]);
                myFromToLookup[edge] = std::make_pair(&myEdgeDict[numericalID], &myEdgeDict[numericalID]);
                numericalID += 1;
            } else { // regular edge or crossing
                myBidiLookup[edge] = std::make_pair(&myEdgeDict[numericalID], &myEdgeDict[numericalID + 1]);
                myFromToLookup[edge] = std::make_pair(&myEdgeDict[numericalID + 2], &myEdgeDict[numericalID + 3]);
                numericalID += 4;
            }
        }

        // build the connections
        for (size_t i = 0; i < noE; i++) {
            E* edge = E::dictionary(i);
            const L* lane = getSidewalk<E, L>(edge);
            if (edge->isInternal() || lane == 0) {
                continue;
            }
            // find all incoming and outgoing lanes for the sidewalk and
            // connect the corresponding PedestrianEdges
            const L* sidewalk = getSidewalk<E, L>(edge);
            const EdgePair& pair = getBothDirections(edge);

#ifdef PedestrianRouter_DEBUG_NETWORK
            std::cout << "  building connections from " << sidewalk->getID() << "\n";
#endif
            std::vector<const L*> outgoing = sidewalk->getOutgoingLanes();
            for (typename std::vector<const L*>::iterator it = outgoing.begin(); it != outgoing.end(); ++it) {
                const L* target = *it;
                const E* targetEdge = &(target->getEdge());
#ifdef PedestrianRouter_DEBUG_NETWORK
                std::cout << "   lane=" << getSidewalk<E, L>(targetEdge)->getID() << (target == getSidewalk<E, L>(targetEdge) ? "(used)" : "") << "\n";
#endif
                if (target == getSidewalk<E, L>(targetEdge)) {
                    const EdgePair& targetPair = getBothDirections(targetEdge);
                    pair.first->myFollowingEdges.push_back(targetPair.first);
                    targetPair.second->myFollowingEdges.push_back(pair.second);
#ifdef PedestrianRouter_DEBUG_NETWORK
                    std::cout << "     " << pair.first->getID() << " -> " << targetPair.first->getID() << "\n";
                    std::cout << "     " << targetPair.second->getID() << " -> " << pair.second->getID() << "\n";
#endif
                }
            }
            if (edge->isWalkingArea()) {
                continue;
            }
            // build connections from depart connector
            PedestrianEdge* startConnector = getDepartEdge(edge);
            startConnector->myFollowingEdges.push_back(pair.first);
            startConnector->myFollowingEdges.push_back(pair.second);
            // build connections to arrival connector
            PedestrianEdge* endConnector = getArrivalEdge(edge);
            pair.first->myFollowingEdges.push_back(endConnector);
            pair.second->myFollowingEdges.push_back(endConnector);
#ifdef PedestrianRouter_DEBUG_NETWORK
            std::cout << "     " << startConnector->getID() << " -> " << pair.first->getID() << "\n";
            std::cout << "     " << startConnector->getID() << " -> " << pair.second->getID() << "\n";
            std::cout << "     " << pair.first->getID() << " -> " << endConnector->getID() << "\n";
            std::cout << "     " << pair.second->getID() << " -> " << endConnector->getID() << "\n";
#endif
        }
    }

    bool includeInRoute(bool allEdges) const {
        return !myAmConnector && (allEdges || (!myEdge->isCrossing() && !myEdge->isWalkingArea()));
    }

    const E* getEdge() const {
        return myEdge;
    }

    /// @brief Returns the pair of forward and backward edge
    static const EdgePair& getBothDirections(const E* e) {
        typename std::map<const E*, EdgePair>::const_iterator it = myBidiLookup.find(e);
        if (it == myBidiLookup.end()) {
            assert(false);
            throw ProcessError("Edge '" + e->getID() + "' not found in pedestrian network '");
        }
        return (*it).second;
    }

    /// @brief Returns the departing Pedestrian edge
    static PedestrianEdge* getDepartEdge(const E* e) {
        typename std::map<const E*, EdgePair>::const_iterator it = myFromToLookup.find(e);
        if (it == myFromToLookup.end()) {
            assert(false);
            throw ProcessError("Edge '" + e->getID() + "' not found in pedestrian network '");
        }
        return (*it).second.first;
    }

    /// @brief Returns the arriving Pedestrian edge
    static PedestrianEdge* getArrivalEdge(const E* e) {
        typename std::map<const E*, EdgePair>::const_iterator it = myFromToLookup.find(e);
        if (it == myFromToLookup.end()) {
            assert(false);
            throw ProcessError("Edge '" + e->getID() + "' not found in pedestrian network '");
        }
        return (*it).second.second;
    }

    /// @name The interface as required by SUMOAbstractRouter routes
    /// @{

    unsigned int getNumericalID() const {
        return myNumericalID;
    }

    /// @brief Returns the PedstrianEdge with the given numericalID
    static const PedestrianEdge* dictionary(size_t index) {
        assert(index < myEdgeDict.size());
        return &myEdgeDict[index];
    }

    unsigned int getNumSuccessors() const {
        return (unsigned int)myFollowingEdges.size();
    }

    PedestrianEdge* getSuccessor(unsigned int i) const {
        return myFollowingEdges[i];
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
            tlsDelay += MAX2(SUMOReal(0), TL_RED_PENALTY - (time - trip->departTime));

        }
#ifdef PedestrianRouter_DEBUG_EFFORTS
        std::cout << " effort for " << getID() << " at " << time << ": " << length / trip->speed + tlsDelay << " l=" << length << " s=" << trip->speed << " tlsDelay=" << tlsDelay << "\n";
#endif
        return length / trip->speed + tlsDelay;
    }

private:
    PedestrianEdge(unsigned int numericalID, const E* edge, const L* lane, bool forward, bool connector = false) :
        Named(edge->getID() + (edge->isWalkingArea() ? "" :
                               ((forward ? "_fwd" : "_bwd") + std::string(connector ? "_connector" : "")))),
        myNumericalID(numericalID),
        myEdge(edge),
        myLane(lane),
        myForward(forward),
        myAmConnector(connector) { }

    /// @brief the index in myEdgeDict
    unsigned int myNumericalID;

    /// @brief  the original edge
    const E* myEdge;

    /// @brief  the original edge
    const L* myLane;

    /// @brief the direction of this edge
    bool myForward;

    /// @brief the direction of this edge
    bool myAmConnector;

    /// @brief List of edges that may be approached from this edge
    std::vector<PedestrianEdge*> myFollowingEdges;

    /// @brief the edge dictionary
    static std::vector<PedestrianEdge> myEdgeDict;

    /// @brief retrieve the forward and backward edge for the given input edge E
    static std::map<const E*, EdgePair> myBidiLookup;

    /// @brief retrieve the depart and arrival edge for the given input edge E
    static std::map<const E*, EdgePair> myFromToLookup;

};


/**
 * @class PedestrianRouter
 * The router for pedestrians (on a bidirectional network of sidewalks and crossings
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
        std::cout << TIME2STEPS(msTime) << " trip from " << from->getID() << " to " << to->getID()
                  << " departPos=" << departPos
                  << " arrivalPos=" << arrivalPos
                  << " onlyNode=" << (onlyNode == 0 ? "NULL" : onlyNode->getID())
                  << " edges=" << toString(intoPed)
                  << " resultEdges=" << toString(into)
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

private:

};

// common specializations
template<class E, class L, class N>
class PedestrianRouterDijkstra : public PedestrianRouter < E, L, N,
        DijkstraRouterTT<PedestrianEdge<E, L, N>, PedestrianTrip<E, N>, prohibited_withRestrictions<PedestrianEdge<E, L, N>, PedestrianTrip<E, N> > > > { };


// ===========================================================================
// static member definitions (PedestrianEdge)
// ===========================================================================

template<class E, class L, class N>
std::vector<PedestrianEdge<E, L, N> > PedestrianEdge<E, L, N>::myEdgeDict;

template<class E, class L, class N>
std::map<const E*, typename PedestrianEdge<E, L, N>::EdgePair> PedestrianEdge<E, L, N>::myBidiLookup;

template<class E, class L, class N>
std::map<const E*, typename PedestrianEdge<E, L, N>::EdgePair> PedestrianEdge<E, L, N>::myFromToLookup;

#endif


/****************************************************************************/

