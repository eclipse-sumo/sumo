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
/// @version $Id: IntermodalEdge.h v0_32_0+0134-9f1b8d0bad oss@behrisch.de 2018-01-04 21:53:06 +0100 $
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
#include <utils/common/ValueTimeLine.h>
#include "IntermodalTrip.h"


// ===========================================================================
// class definitions
// ===========================================================================
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


#endif

/****************************************************************************/
