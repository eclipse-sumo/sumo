/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
#include <utils/common/Named.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/ToString.h>
#include <utils/common/ValueTimeLine.h>

#define TL_RED_PENALTY 20

//#define IntermodalRouter_DEBUG_EFFORTS


template <class E, class L>
inline const L* getSidewalk(const E* edge) {
    if (edge == nullptr) {
        return nullptr;
    }
    // prefer lanes that are exclusive to pedestrians
    const std::vector<L*>& lanes = edge->getLanes();
    for (const L* const lane : lanes) {
        if (lane->getPermissions() == SVC_PEDESTRIAN) {
            return lane;
        }
    }
    for (const L* const lane : lanes) {
        if (lane->allowsVehicleClass(SVC_PEDESTRIAN)) {
            return lane;
        }
    }
    return nullptr;
}


// ===========================================================================
// class definitions
// ===========================================================================

/// @brief the "vehicle" type that is given to the internal router (SUMOAbstractRouter)
template<class E, class N, class V>
struct IntermodalTrip {

    IntermodalTrip(const E* _from, const E* _to, double _departPos, double _arrivalPos,
                   double _speed, SUMOTime _departTime, const N* _node,
                   const V* _vehicle = 0, const SVCPermissions _modeSet = SVC_PEDESTRIAN) :
        from(_from),
        to(_to),
        departPos(_departPos < 0 ? _from->getLength() + _departPos : _departPos),
        arrivalPos(_arrivalPos < 0 ? _to->getLength() + _arrivalPos : _arrivalPos),
        speed(_speed),
        departTime(_departTime),
        node(_node),
        vehicle(_vehicle),
        modeSet(_modeSet) {
    }

    // exists just for debugging purposes
    std::string getID() const {
        return from->getID() + ":" + to->getID() + ":" + time2string(departTime);
    }


    inline SUMOVehicleClass getVClass() const {
        return vehicle != 0 ? vehicle->getVClass() : SVC_PEDESTRIAN;
    }

    const E* const from;
    const E* const to;
    const double departPos;
    const double arrivalPos;
    const double speed;
    const SUMOTime departTime;
    const N* const node; // indicates whether only routing across this node shall be performed
    const V* const vehicle; // indicates which vehicle may be used
    const SVCPermissions modeSet;
private:
    /// @brief Invalidated assignment operator.
    IntermodalTrip& operator=(const IntermodalTrip&);
};


/// @brief the base edge type that is given to the internal router (SUMOAbstractRouter)
template<class E, class L, class N, class V>
class IntermodalEdge : public Named {
public:
    IntermodalEdge(const std::string id, int numericalID, const E* edge, const std::string& line) :
        Named(id),
        myNumericalID(numericalID),
        myEdge(edge),
        myLine(line),
        myLength(edge == nullptr ? 0. : edge->getLength()),
        myEfforts(nullptr) { }

    virtual ~IntermodalEdge() {}

    virtual bool includeInRoute(bool /* allEdges */) const {
        return false;
    }

    inline const std::string& getLine() const {
        return myLine;
    }

    inline const E* getEdge() const {
        return myEdge;
    }

    int getNumericalID() const {
        return myNumericalID;
    }

    void addSuccessor(IntermodalEdge* s) {
        myFollowingEdges.push_back(s);
    }

    void setSuccessors(const std::vector<IntermodalEdge*>& edges) {
        myFollowingEdges = edges;
    }

    void clearSuccessors() {
        myFollowingEdges.clear();
    }

    void removeSuccessor(const IntermodalEdge* const edge) {
        myFollowingEdges.erase(std::find(myFollowingEdges.begin(), myFollowingEdges.end(), edge));
    }

    virtual const std::vector<IntermodalEdge*>& getSuccessors(SUMOVehicleClass /*vClass*/) const {
        // the network is already tailored. No need to check for permissions here
        return myFollowingEdges;
    }

    virtual bool prohibits(const IntermodalTrip<E, N, V>* const /* trip */) const {
        return false;
    }

    virtual double getTravelTime(const IntermodalTrip<E, N, V>* const /* trip */, double /* time */) const {
        return 0.;
    }

    static inline double getTravelTimeStatic(const IntermodalEdge* const edge, const IntermodalTrip<E, N, V>* const trip, double time) {
        return edge == nullptr ? 0. : edge->getTravelTime(trip, time);
    }

    virtual double getEffort(const IntermodalTrip<E, N, V>* const /* trip */, double /* time */) const {
        return 0.;
    }

    static inline double getEffortStatic(const IntermodalEdge* const edge, const IntermodalTrip<E, N, V>* const trip, double time) {
        return edge == nullptr || !edge->hasEffort() ? 0. : edge->getEffort(trip, time);
    }

    inline double getLength() const {
        return myLength;
    }

    inline void setLength(const double length) {
        myLength = length;
    }

    virtual bool hasEffort() {
        return myEfforts != nullptr;
    }

protected:
    /// @brief List of edges that may be approached from this edge
    std::vector<IntermodalEdge*> myFollowingEdges;

private:
    /// @brief the index in myEdges
    const int myNumericalID;

    /// @brief  the original edge
    const E* const myEdge;

    /// @brief public transport line or ped vs car
    const std::string myLine;

    /// @brief adaptable length (for splitted edges)
    double myLength;

    /// @brief Container for passing effort varying over time for the edge
    ValueTimeLine<double>* myEfforts;

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
    PedestrianEdge(int numericalID, const E* edge, const L* lane, bool forward, const double pos = -1.) :
        IntermodalEdge<E, L, N, V>(edge->getID() + (edge->isWalkingArea() ? "" : (forward ? "_fwd" : "_bwd")) + toString(pos), numericalID, edge, "!ped"),
        myLane(lane),
        myForward(forward),
        myStartPos(pos >= 0 ? pos : (forward ? 0. : edge->getLength())) { }

    bool includeInRoute(bool allEdges) const {
        return allEdges || (!this->getEdge()->isCrossing() && !this->getEdge()->isWalkingArea());
    }

    bool prohibits(const IntermodalTrip<E, N, V>* const trip) const {
        if (trip->node == 0) {
            // network only includes IntermodalEdges
            return false;
        } else {
            // limit routing to the surroundings of the specified node
            return (this->getEdge()->getFromJunction() != trip->node
                    && this->getEdge()->getToJunction() != trip->node);
        }
    }

    virtual double getTravelTime(const IntermodalTrip<E, N, V>* const trip, double time) const {
        double length = this->getLength();
        if (this->getEdge() == trip->from && !myForward && trip->departPos < myStartPos) {
            length = trip->departPos - (myStartPos - this->getLength());
        }
        if (this->getEdge() == trip->to && myForward && trip->arrivalPos < myStartPos + this->getLength()) {
            length = trip->arrivalPos - myStartPos;
        }
        if (this->getEdge() == trip->from && myForward && trip->departPos > myStartPos) {
            length -= (trip->departPos - myStartPos);
        }
        if (this->getEdge() == trip->to && !myForward && trip->arrivalPos > myStartPos - this->getLength()) {
            length -= (trip->arrivalPos - (myStartPos - this->getLength()));
        }
        // ensure that 'normal' edges always have a higher weight than connector edges
        length = MAX2(length, NUMERICAL_EPS);
        double tlsDelay = 0;
        // @note pedestrian traffic lights should never have LINKSTATE_TL_REDYELLOW
        if (this->getEdge()->isCrossing() && myLane->getIncomingLinkState() == LINKSTATE_TL_RED) {
            // red traffic lights occurring later in the route may be green by the time we arrive
            tlsDelay += MAX2(double(0), TL_RED_PENALTY - (time - STEPS2TIME(trip->departTime)));
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

    /// @brief the starting position for split edges
    const double myStartPos;

};


#endif

/****************************************************************************/
