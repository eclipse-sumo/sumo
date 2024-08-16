/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    FlippedEdge.h
/// @author  Ruediger Ebendt
/// @date    01.12.2023
///
// Extension of ReversedEdge, which is a wrapper around a ROEdge  
// or an MSEdge used for backward search. In contrast to reversed 
// edges, flipped edges have flipped nodes instead of standard nodes. 
// Introduced for the arc flag router.
/****************************************************************************/
#pragma once
#include <config.h>
#include <utils/common/SUMOVehicleClass.h>
#ifdef HAVE_FOX
#include <utils/foxtools/MsgHandlerSynchronized.h>
#endif
#include "ReversedEdge.h"


// ===========================================================================
// class declarations
// ===========================================================================
template<class E, class N, class V>
class FlippedNode;

// ===========================================================================
// class definitions
// ===========================================================================
/// @brief The edge type representing backward edges with flipped nodes
template<class E, class N, class V>
class FlippedEdge : public ReversedEdge<E, V> {
public:

    typedef std::vector<std::pair<const FlippedEdge<E, N, V>*, const FlippedEdge<E, N, V>*> > ConstFlippedEdgePairVector;

    /** @brief Constructor
     * @param[in] originalEdge The original (forward) edge
     */
    FlippedEdge(const E* originalEdge) :
        ReversedEdge<E, V>(originalEdge), 
        myFromJunction(this->getOriginalEdge()->getToJunction()->getFlippedRoutingNode()),
        myToJunction(this->getOriginalEdge()->getFromJunction()->getFlippedRoutingNode())
    {}
    
    /// @brief Destructor
    ~FlippedEdge() {}

    /// @brief Initialize the flipped edge
    void init();
    /// @brief Returns the from-junction
    const FlippedNode<E, N, V>* getFromJunction() const {
        return myFromJunction;
    }
    /// @brief Returns the to-junction
    const FlippedNode<E, N, V>* getToJunction() const {
        return myToJunction;
    }
    /** @brief Returns the time for travelling on the given edge with the given vehicle at the given time 
     * @param[in] edge The edge
     * @param[in] veh The vehicle
     * @param[in] time The time
     * @return The time for travelling on the given edge with the given vehicle at the given time 
     */
    static double getTravelTimeStatic(const FlippedEdge<E, N, V>* const edge, const V* const veh, double time) {
        return edge->getOriginalEdge()->getTravelTime(veh, time);
    }
    /** @brief Returns the randomized time for travelling on the given edge with the given vehicle at the given time
     * @param[in] edge The edge
     * @param[in] veh The vehicle
     * @param[in] time The time
     * @return The randomized time for travelling on the given edge with the given vehicle at the given time
     */
    static double getTravelTimeStaticRandomized(const FlippedEdge<E, N, V>* const edge, const V* const veh, double time) {
        return edge->getOriginalEdge()->getTravelTimeStaticRandomized(edge->getOriginalEdge(), veh, time);
    }

    /** @brief Returns the via successors
     * @param[in] vClass The vehicle class
     * @param[in] ignoreTransientPermissions Unused parameter
     * @return The via successors
     */
    const ConstFlippedEdgePairVector& getViaSuccessors(SUMOVehicleClass vClass = SVC_IGNORING, bool ignoreTransientPermissions = false) const {
        UNUSED_PARAMETER(ignoreTransientPermissions); // @todo this should be changed (somewhat hidden by #14756)
        if (vClass == SVC_IGNORING || this->getOriginalEdge()->isTazConnector()) { // || !MSNet::getInstance()->hasPermissions()) {
            return myViaSuccessors;
        }
#ifdef HAVE_FOX
        FXMutexLock lock(mySuccessorMutex);
#endif
        auto i = myClassesViaSuccessorMap.find(vClass);
        if (i != myClassesViaSuccessorMap.end()) {
            // can use cached value
            return i->second;
        }
        // instantiate vector
        ConstFlippedEdgePairVector& result = myClassesViaSuccessorMap[vClass];
        // this vClass is requested for the first time. rebuild all successors
        for (const auto& viaPair : myViaSuccessors) {
            if (viaPair.first->getOriginalEdge()->isTazConnector() 
                || viaPair.first->getOriginalEdge()->isConnectedTo(*(this->getOriginalEdge()), vClass)) {
                result.push_back(viaPair);
            }
        }
        return result;
    }

    /** @brief Returns the bidirectional edge
     * @return The bidirectional edge
     */
    const FlippedEdge<E, N, V>* getBidiEdge() const {
        return this->getOriginalEdge()->getBidiEdge()->getFlippedRoutingEdge();
    }
    /** @brief Returns the distance to another flipped edge
     * param[in] other The other flipped edge
     * param[in] doBoundaryEstimate The boolean flag indicating whether the distance is estimated using both boundaries or not
     * @return The distance to another flipped edge
     */
    double getDistanceTo(const FlippedEdge<E, N, V>* other, const bool doBoundaryEstimate = false) const {
        return this->getOriginalEdge()->getDistanceTo(other->getOriginalEdge(), doBoundaryEstimate);
    }
    /** @brief Returns the minimum travel time
     * @param[in] veh The vehicle
     * @return The minimum travel time
     */
    double getMinimumTravelTime(const V* const veh) const {
        return this->getOriginalEdge()->getMinimumTravelTime(veh);
    }
    /** @brief Returns the time penalty
     * @return The time penalty
     */ 
    double getTimePenalty() const {
        return this->getOriginalEdge()->getTimePenalty();
    }
    /** @brief Returns a boolean flag indicating whether this edge has loaded travel times or not
     * @return true iff this edge has loaded travel times
     */
    bool hasLoadedTravelTimes() const {
        return this->getOriginalEdge()->hasLoadedTravelTimes();
    }
    /** @brief Returns the speed allowed on this edge
     * @return The speed allowed on this edge
     */
    double getSpeedLimit() const {
        return this->getOriginalEdge()->getSpeedLimit();
    }
    /** @brief Returns a lower bound on shape.length() / myLength
     * @note The bound is sufficient for the astar air-distance heuristic
     * @return A lower bound on shape.length() / myLength
     */
    double getLengthGeometryFactor() const {
        return this->getOriginalEdge()->getLengthGeometryFactor();
    }
    /** @brief Returns the edge priority (road class)
     * @return The edge priority (road class)
     */ 
    int getPriority() const {
        return this->getOriginalEdge()->getPriority();
    }

protected:
    /// @brief The junctions for this edge
    FlippedNode<E, N, V>* myFromJunction;
    FlippedNode<E, N, V>* myToJunction;

private:
    /// @brief The successors available for a given vClass
    mutable std::map<SUMOVehicleClass, ConstFlippedEdgePairVector> myClassesViaSuccessorMap;
    mutable ConstFlippedEdgePairVector myViaSuccessors;

#ifdef HAVE_FOX
    /// @brief Mutex for accessing successor edges
    mutable FXMutex mySuccessorMutex;
#endif
};

// ===========================================================================
// method definitions
// ===========================================================================

template<class E, class N, class V>
void FlippedEdge<E, N, V>::init() {
    if (!this->getOriginalEdge()->isInternal()) {
        for (const auto& viaPair : this->getOriginalEdge()->getViaSuccessors()) {
            const FlippedEdge<E, N, V>* revSource = viaPair.first->getFlippedRoutingEdge();
            const E* via = viaPair.second;
            const FlippedEdge<E, N, V>* preVia = nullptr;
            while (via != nullptr && via->isInternal()) {
                via->getFlippedRoutingEdge()->myViaSuccessors.push_back(std::make_pair(this, preVia));
                preVia = via->getFlippedRoutingEdge();
                via = via->getViaSuccessors().front().second;
            }
            revSource->myViaSuccessors.push_back(std::make_pair(this, preVia));
        }
    }
}

