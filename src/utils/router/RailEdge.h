/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    RailEdge.h
/// @author  Jakob Erdmann
/// @date    26.02.2020
///
// The RailEdge is a wrapper around a ROEdge or a MSEdge used for railway routing
/****************************************************************************/
#pragma once
#include <config.h>
#include <cassert>

//#define RailEdge_DEBUG_TURNS
//#define RailEdge_DEBUG_SUCCESSORS
#define RailEdge_DEBUGID ""
#define RailEdge_DEBUG_COND(obj) ((obj != 0 && (obj)->getID() == RailEdge_DEBUGID))

// ===========================================================================
// class definitions
// ===========================================================================
/// @brief the edge type representing backward edges
template<class E, class V>
class RailEdge {
public:
    typedef RailEdge<E, V> _RailEdge;
    typedef std::vector<std::pair<const _RailEdge*, const _RailEdge*> > ConstEdgePairVector;

    RailEdge(const E* orig) : 
        myNumericalID(orig->getNumericalID()),
        myOriginal(orig)
    { }

    RailEdge(const E* turnStart, const E* turnEnd, int numericalID, double maxTrainLength) : 
        myNumericalID(numericalID),
        myID("TrainReversal!" + turnStart->getID() + "->" + turnEnd->getID()),
        myOriginal(nullptr),
        myMaxLength(0),
        myStartLength(turnStart->getLength())
    { 
        myViaSuccessors.push_back(std::make_pair(turnEnd->getRailwayRoutingEdge(), nullptr));
        // compute length and replacement edges
        auto result = findReversal(maxTrainLength, turnStart, turnEnd);
        myMaxLength = result.first + myStartLength;
        myReplacementEdges = result.second;
        //std::cout << getID() << " maxLength=" << myMaxLength << " replacement=" << toString(myReplacementEdges) << "\n";
    }

    std::pair<double, std::vector<const E*> > findReversal(double dist, const E* forward, const E* backward) {
        if (dist <= 0) {
            return std::make_pair(0, std::vector<const E*>({}));
        }
        std::pair<double, std::vector<const E*> > best(0, {});
        for (const auto& viaPair : forward->getViaSuccessors()) {
            const E* next = viaPair.first;
            if (next == backward) {
                continue;
            }
            const E* bidi = next->getBidiEdge();
            if (bidi != nullptr && bidi->isConnectedTo(*backward, SVC_IGNORING)) {
                auto subResult = findReversal(dist - next->getLength(), next, bidi);
                const double seen = next->getLength() + subResult.first;
                if (seen > best.first) {
                    best.first = seen;
                    best.second.clear();
                    best.second.push_back(next);
                    best.second.insert(best.second.end(), subResult.second.begin(), subResult.second.end());
                }
                if (seen >= dist) {
                    return best;
                }
            }
        }
        return best;
    }

    void init(std::vector<_RailEdge*>& railEdges, int& numericalID, double maxTrainLength) {
        // replace turnaround-via with an explicit RailEdge that checks length
        for (const auto& viaPair : myOriginal->getViaSuccessors()) {
            if (viaPair.first == myOriginal->getBidiEdge()) {
                // direction reversal
                _RailEdge* turnEdge = new _RailEdge(myOriginal, viaPair.first, numericalID++, maxTrainLength);
                railEdges.push_back(turnEdge);
                myViaSuccessors.push_back(std::make_pair(turnEdge, nullptr));
            } else {
                myViaSuccessors.push_back(std::make_pair(viaPair.first->getRailwayRoutingEdge(), 
                            viaPair.second == nullptr ? nullptr : viaPair.second->getRailwayRoutingEdge()));
            }
        }
#ifdef RailEdge_DEBUG_TURNS
        std::cout << "RailEdge " << getID() << " successors=" << myViaSuccessors.size() << " orig=" << myOriginal->getViaSuccessors().size() << "\n";
        for (const auto& viaPair : myViaSuccessors) {
            std::cout << "    " << viaPair.first->getID() << "\n";
        }
#endif
    }

    /// @brief Returns the index (numeric id) of the edge
    inline int getNumericalID() const {
        return myNumericalID;
    }

    /// @brief Returns the original edge
    const E* getOriginal() const {
        return myOriginal;
    }

    /** @brief Returns the id of the edge
     * @return The original edge's id
     */
    const std::string& getID() const {
        return myOriginal != nullptr ? myOriginal->getID() : myID;
    }

    void insertOriginalEdges(double length, std::vector<const E*>& into) const {
        if (myOriginal != nullptr) {
            into.push_back(myOriginal);
        } else {
            double seen = myStartLength;
            int nPushed = 0;
            for (const E* edge : myReplacementEdges) {
                if (seen >= length) {
                    break;
                }
                into.push_back(edge);
                nPushed++;
                seen += edge->getLength();
                //std::cout << "insertOriginalEdges length=" << length << " seen=" << seen << " into=" << toString(into) << "\n";
            }
            const int last = (int)into.size() - 1;
            for (int i = 0; i < nPushed; i++) {
                into.push_back(into[last - i]->getBidiEdge());
            }
        }
    }

    /** @brief Returns the length of the edge
     * @return The original edge's length
     */
    double getLength() const {
        return myOriginal == nullptr ? 0 : myOriginal->getLength();
    }

    //const RailEdge* getBidiEdge() const {
    //    return myOriginal->getBidiEdge()->getRailwayRoutingEdge();
    //}

    bool isInternal() const {
        return myOriginal->isInternal();
    }

    inline bool prohibits(const V* const vehicle) const {
#ifdef RailEdge_DEBUG_TURNS
        if (myOriginal == nullptr && RailEdge_DEBUG_COND(vehicle)) {
            std::cout << getID() << " maxLength=" << myMaxLength << " veh=" << vehicle->getID() << " length=" << vehicle->getLength() << "\n";
        }
#endif
        return vehicle->getLength() > myMaxLength || (myOriginal != nullptr && myOriginal->prohibits(vehicle));
    }

    inline bool restricts(const V* const vehicle) const {
        return myOriginal != nullptr && myOriginal->restricts(vehicle);
    }

    const ConstEdgePairVector& getViaSuccessors(SUMOVehicleClass vClass = SVC_IGNORING) const {
        if (vClass == SVC_IGNORING || myOriginal == nullptr || myOriginal->isTazConnector()) { // || !MSNet::getInstance()->hasPermissions()) {
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
        ConstEdgePairVector& result = myClassesViaSuccessorMap[vClass];
        // this vClass is requested for the first time. rebuild all successors
        for (const auto& viaPair : myViaSuccessors) {
            if (viaPair.first->myOriginal == nullptr 
                    || viaPair.first->myOriginal->isTazConnector() 
                    || myOriginal->isConnectedTo(*viaPair.first->myOriginal, vClass)) {
                result.push_back(viaPair);
            }
        }
        return result;
    }

private:
    const int myNumericalID;
    const std::string myID;
    const E* myOriginal;

    /// @brief actual edges to return when passing this (turnaround) edge - only forward
    std::vector<const E*> myReplacementEdges;

    /// @brief maximum train length for passing this (turnaround) edge
    double myMaxLength = std::numeric_limits<double>::max();
    /// @brief length of the edge where this turn starts
    double myStartLength = 0;

    /// @brief The successors available for a given vClass
    mutable std::map<SUMOVehicleClass, ConstEdgePairVector> myClassesViaSuccessorMap;

    mutable ConstEdgePairVector myViaSuccessors;

#ifdef HAVE_FOX
    /// @brief Mutex for accessing successor edges
    mutable FXMutex mySuccessorMutex;
#endif

};
