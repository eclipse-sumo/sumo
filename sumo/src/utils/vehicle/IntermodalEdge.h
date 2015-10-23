/****************************************************************************/
/// @file    IntermodalEdge.h
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Robert Hilbrich
/// @date    Mon, 03 March 2014
/// @version $Id$
///
// The Edge definition for the Intermodal Router
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
#ifndef IntermodalEdge_h
#define IntermodalEdge_h


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

//#define IntermodalRouter_DEBUG_EFFORTS


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
template<class E, class N, class V>
struct IntermodalTrip {

    IntermodalTrip(const E* _from, const E* _to, SUMOReal _departPos, SUMOReal _arrivalPos, SUMOReal _speed, SUMOTime _departTime, const N* _node, const V* _vehicle=0) :
        from(_from),
        to(_to),
        node(_node),
        vehicle(_vehicle),
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

    const E* const from;
    const E* const to;
    const N* const node; // indicates whether only routing across this node shall be performed
    const V* const vehicle; // indicates which vehicle may be used
    const SUMOReal departPos;
    const SUMOReal arrivalPos;
    const SUMOReal speed;
    const SUMOTime departTime;
private:
    /// @brief Invalidated assignment operator.
    IntermodalTrip& operator=(const IntermodalTrip&);
};


/// @brief the base edge type that is given to the internal router (SUMOAbstractRouter)
template<class E, class L, class N, class V>
class IntermodalEdge : public Named {
public:
    IntermodalEdge(const std::string id, unsigned int numericalID, const E* edge, const std::string& line) :
        Named(id),
        myNumericalID(numericalID),
        myEdge(edge),
        myLine(line) { }

    virtual ~IntermodalEdge() {}

    virtual bool includeInRoute(bool /* allEdges */) const {
        return false;
    }

    const std::string& getLine() const {
        return myLine;
    }

    const E* getEdge() const {
        return myEdge;
    }

    /// @name The interface as required by SUMOAbstractRouter routes
    /// @{

    unsigned int getNumericalID() const {
        return myNumericalID;
    }

    virtual void addSuccessor(IntermodalEdge* s) {
        myFollowingEdges.push_back(s);
    }

    const std::vector<IntermodalEdge*>& getSuccessors(SUMOVehicleClass /*vClass*/) const {
        // the network is already tailored for pedestrians. No need to check for permissions here
        return myFollowingEdges;
    }

    virtual bool prohibits(const IntermodalTrip<E, N, V>* const /* trip */) const {
        return false;
    }

    /// @}

    virtual SUMOReal getTravelTime(const IntermodalTrip<E, N, V>* const /* trip */, SUMOReal /* time */) const {
        return 0;
    }

    static SUMOReal getTravelTimeStatic(const IntermodalEdge* const edge, const IntermodalTrip<E, N, V>* const trip, SUMOReal time) {
        return edge->getTravelTime(trip, time);
    }


protected:
    /// @brief  the original edge
    const E* const myEdge;


private:
    /// @brief the index in myEdgeDict
    const unsigned int myNumericalID;

    /// @brief List of edges that may be approached from this edge
    std::vector<IntermodalEdge*> myFollowingEdges;

    /// @brief public transport line or ped vs car
    const std::string myLine;

private:
    /// @brief Invalidated copy constructor
    IntermodalEdge(const IntermodalEdge& src);

    /// @brief Invalidated assignment operator
    IntermodalEdge& operator=(const IntermodalEdge& src);

};


/// @brief the pedestrian edge type that is given to the internal router (SUMOAbstractRouter)
template<class E, class L, class N, class V>
class PedestrianEdge : public IntermodalEdge<E, L, N, V> {
public:
    PedestrianEdge(unsigned int numericalID, const E* edge, const L* lane, bool forward) :
        IntermodalEdge<E, L, N, V>(edge->getID() + (edge->isWalkingArea() ? "" : (forward ? "_fwd" : "_bwd")), numericalID, edge, "!ped"),
        myLane(lane),
        myForward(forward) { }

    bool includeInRoute(bool allEdges) const {
        return allEdges || (!myEdge->isCrossing() && !myEdge->isWalkingArea());
    }

    /// @name The interface as required by SUMOAbstractRouter routes
    /// @{

    bool prohibits(const IntermodalTrip<E, N, V>* const trip) const {
        if (trip->node == 0) {
            // network only includes IntermodalEdges
            return false;
        } else {
            // limit routing to the surroundings of the specified node
            return (myEdge->getFromJunction() != trip->node
                    && myEdge->getToJunction() != trip->node);
        }
    }

    /// @}

    virtual SUMOReal getTravelTime(const IntermodalTrip<E, N, V>* const trip, SUMOReal time) const {
        SUMOReal length = myEdge->getLength();
        if (myEdge == trip->from) {
            if (myForward) {
                length -= trip->departPos;
            } else {
                length = trip->departPos;
            }
        }
        if (myEdge == trip->to) {
            if (myForward) {
                length = trip->arrivalPos;
            } else {
                length -= trip->arrivalPos;
            }
        }
        // ensure that 'normal' edges always have a higher weight than connector edges
        length = MAX2(length, POSITION_EPS);
        SUMOReal tlsDelay = 0;
        // @note pedestrian traffic lights should never have LINKSTATE_TL_REDYELLOW
        if (myEdge->isCrossing() && myLane->getIncomingLinkState() == LINKSTATE_TL_RED) {
            // red traffic lights occurring later in the route may be green by the time we arive
            tlsDelay += MAX2(SUMOReal(0), TL_RED_PENALTY - (time - STEPS2TIME(trip->departTime)));
        }
#ifdef IntermodalRouter_DEBUG_EFFORTS
        std::cout << " effort for " << trip->getID() << " at " << time << " edge=" << edge->getID() << " effort=" << length / trip->speed + tlsDelay << " l=" << length << " s=" << trip->speed << " tlsDelay=" << tlsDelay << "\n";
#endif
        return length / trip->speed + tlsDelay;
    }

private:
    /// @brief  the original edge
    const L* myLane;

    /// @brief the direction of this edge
    const bool myForward;

};


#endif

/****************************************************************************/
