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
/// @author  Michael Behrisch
/// @date    29.01.2020
///
// The RailEdge is a wrapper around a ROEdge or a MSEdge used for railway routing
/****************************************************************************/
#pragma once
#include <config.h>


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
        myOriginal(orig) { }

    RailEdge(const E* turnStart, const E* turnEnd, int numericalID) : 
        myNumericalID(numericalID),
        myID("TrainReversal!" + turnStart->getID() + "->" + turnEnd->getID()),
        myOriginal(nullptr),
        myMaxLength(0)
    { 
        myViaSuccessors.push_back(std::make_pair(turnEnd->getRailwayRoutingEdge(), nullptr));

    }

    void init(std::vector<_RailEdge*>& railEdges, int& numericalID) {
        // replace turnaround-via with an explicit RailEdge that checks length
        if (!myOriginal->isInternal()) {
            for (const auto& viaPair : myOriginal->getViaSuccessors()) {
                if (viaPair.first == myOriginal->getBidiEdge()) {
                    // direction reversal
                    _RailEdge* turnEdge = new _RailEdge(myOriginal, viaPair.first, numericalID++);
                    railEdges.push_back(turnEdge);
                    myViaSuccessors.push_back(std::make_pair(turnEdge, nullptr));
                } else {
                    myViaSuccessors.push_back(std::make_pair(viaPair.first->getRailwayRoutingEdge(), 
                                viaPair.second == nullptr ? nullptr : viaPair.second->getRailwayRoutingEdge()));
                }
            }
        }
        std::cout << "railEdges::init " << getID() << " myViaSuccessors=" << myViaSuccessors.size() << " origSuccessors=" << myOriginal->getViaSuccessors().size() << "\n";
    }

    /// @brief Returns the index (numeric id) of the edge
    inline int getNumericalID() const {
        return myNumericalID;
    }

    /** @brief Returns the id of the edge
     * @return The original edge's id
     */
    const std::string& getID() const {
        return myOriginal != nullptr ? myOriginal->getID() : myID;
    }

    void insertOriginalEdges(std::vector<const E*>& into) const {
        if (myReplacementEdges.empty()) {
            into.push_back(myOriginal);
        } else {
            into.insert(into.end(), myReplacementEdges.begin(), myReplacementEdges.end());
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
            if (viaPair.first->myOriginal->isTazConnector() || myOriginal->isConnectedTo(*viaPair.first->myOriginal, vClass)) {
                result.push_back(viaPair);
            }
        }
        return result;
    }

private:
    const int myNumericalID;
    const std::string myID;
    const E* myOriginal;

    /// @brief actual edges to return when passing this (turnaround) edge
    std::vector<E*> myReplacementEdges;

    /// @brief maximum train length for passing this (turnaround) edge
    double myMaxLength = std::numeric_limits<double>::max();

    /// @brief The successors available for a given vClass
    mutable std::map<SUMOVehicleClass, ConstEdgePairVector> myClassesViaSuccessorMap;

    mutable ConstEdgePairVector myViaSuccessors;

#ifdef HAVE_FOX
    /// @brief Mutex for accessing successor edges
    mutable FXMutex mySuccessorMutex;
#endif

};
