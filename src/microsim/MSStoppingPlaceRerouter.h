/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    StoppingPlaceRerouter.h
/// @author  Mirko Barthauer
/// @date    Mon, 17 June 2024
///
// The StoppingPlaceRerouter provides an interface to structure the rerouting
// to the best StoppingPlace according to the evaluation criteria and
// associated weights.
/****************************************************************************/
#pragma once
#include <config.h>
#include <map>
#include <functional>
#include <utils/common/MsgHandler.h>
#include <utils/common/Named.h>
#include <utils/common/SUMOTime.h>
#include <utils/router/SUMOAbstractRouter.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include "MSNet.h"
#include "MSStoppingPlace.h"

class MSEdge;

/// @brief store information for a single stopping place
struct StoppingPlaceMemoryEntry {
    StoppingPlaceMemoryEntry() : blockedAtTime(-1), blockedAtTimeLocal(-1), score("") {}

    SUMOTime blockedAtTime;
    SUMOTime blockedAtTimeLocal;
    std::string score;
};


class StoppingPlaceMemory {
public:
    /// @brief Definition of the map containing all visited stopping places
    typedef std::map<const MSStoppingPlace*, StoppingPlaceMemoryEntry, ComparatorIdLess> StoppingPlaceMap;

    ///@brief Constructor
    StoppingPlaceMemory() {}

    // Destructor
    virtual ~StoppingPlaceMemory() {}

    /** @brief Removes an item
     * @param[in] id The id of the item to remove
     * @return If the item could be removed (an item with the id was within the container before)
     */
    bool remove(MSStoppingPlace* id) {
        auto it = myMap.find(id);
        if (it == myMap.end()) {
            return false;
        } else {
            myMap.erase(it);
            return true;
        }
    }

    /// @brief Removes all data about evaluated StoppingPlace items
    void clear() {
        myMap.clear();
    }

    /// @brief Returns the number of stored items within the container
    int size() const {
        return (int)myMap.size();
    }

    /// @brief Store the time the StoppingPlace was confirmed to be blocked
    void rememberBlockedStoppingPlace(const MSStoppingPlace* stoppingPlace, bool local) {
        myMap[stoppingPlace].blockedAtTime = SIMSTEP;
        if (local) {
            myMap[stoppingPlace].blockedAtTimeLocal = SIMSTEP;
        }
    }

    /// @brief Get the time the StoppingPlace was confirmed to be blocked
    SUMOTime sawBlockedStoppingPlace(const MSStoppingPlace* stoppingPlace, bool local) const {
        auto it = myMap.find(stoppingPlace);
        if (it == myMap.end()) {
            return -1;
        } else {
            return local ? it->second.blockedAtTimeLocal : it->second.blockedAtTime;
        }
    }

    /// @brief score only needed when running with gui
    void rememberStoppingPlaceScore(const MSStoppingPlace* stoppingPlace, const std::string& score) {
        myMap[stoppingPlace].score = score;
    }

    void resetStoppingPlaceScores() {
        for (auto& item : myMap) {
            item.second.score = "";
        }
    }

    /// @brief Returns a reference to the begin iterator for the internal map
    typename StoppingPlaceMap::const_iterator begin() const {
        return myMap.begin();
    }

    /// @brief Returns a reference to the end iterator for the internal map
    typename StoppingPlaceMap::const_iterator end() const {
        return myMap.end();
    }

private:
    /// @brief The map from StoppingPlace to single evaluation
    StoppingPlaceMap myMap;

};


class MSStoppingPlaceRerouter {
public:
    typedef std::map<std::string, double> StoppingPlaceParamMap_t;
    typedef std::map<std::string, bool> StoppingPlaceParamSwitchMap_t;
    typedef std::map<MSStoppingPlace*, StoppingPlaceParamMap_t, ComparatorIdLess> StoppingPlaceMap_t;
    typedef std::pair<MSStoppingPlace*, bool> StoppingPlaceVisible;

    ///@brief Constructor
    MSStoppingPlaceRerouter(SumoXMLTag stoppingType, std::string paramPrefix = "",
    StoppingPlaceParamMap_t evalParams = { {"probability", 0.}, {"capacity", 0.}, {"timefrom", 0.}, {"timeto", 0.}, {"distancefrom", 0.}, {"distanceto", 1.}, {"absfreespace", 0.}, {"relfreespace", 0.}, },
    StoppingPlaceParamSwitchMap_t invertParams = { {"probability", true}, { "capacity", true }, { "timefrom", false }, { "timeto", false }, { "distancefrom", false }, { "distanceto", false }, { "absfreespace", true }, { "relfreespace", true }, });

    // Destructor
    virtual ~MSStoppingPlaceRerouter() {}

    /// @brief main method to trigger the rerouting to the "best" StoppingPlace according to the custom evaluation function
    MSStoppingPlace* reroute(std::vector<StoppingPlaceVisible>& stoppingPlaceCandidates, const std::vector<double>& probs, SUMOVehicle& veh,
                             bool& newDestination, ConstMSEdgeVector& newRoute, const MSEdgeVector& closedEdges = {});

    /// @brief compute the target function for a single alternative
    virtual bool evaluateDestination(SUMOVehicle& veh, double brakeGap, bool newDestination,
                                     MSStoppingPlace* alternative, double occupancy, double prob,
                                     SUMOAbstractRouter<MSEdge, SUMOVehicle>& router, StoppingPlaceMap_t& stoppingPlaces,
                                     std::map<MSStoppingPlace*, ConstMSEdgeVector>& newRoutes,
                                     std::map<MSStoppingPlace*, ConstMSEdgeVector>& stoppingPlaceApproaches,
                                     StoppingPlaceParamMap_t& maxValues);

    /// @brief Ask the vehicle about the relevant rerouting parameters and init the maximum value data structure
    void readEvaluationWeights(SUMOVehicle& veh, StoppingPlaceParamMap_t& stoppingPlaceParams, StoppingPlaceParamMap_t& stoppingPlaceDefaults, StoppingPlaceParamMap_t& maxValues) {
        for (StoppingPlaceParamMap_t::iterator it = stoppingPlaceParams.begin(); it != stoppingPlaceParams.end(); ++it) {
            double value = getWeight(veh, it->first, stoppingPlaceDefaults[it->first]);
            it->second = value;
            if (value > maxValues[it->first]) {
                maxValues[it->first] = value;
            }
        }
    }

    /// @brief Return the number of occupied places of the StoppingPlace
    virtual double getStoppingPlaceOccupancy(MSStoppingPlace* stoppingPlace) = 0;

    /// @brief Return the number of places the StoppingPlace provides
    virtual double getStoppingPlaceCapacity(MSStoppingPlace* stoppingPlace) = 0;

    /// @brief store the blocked stopping place in the vehicle
    virtual void rememberBlockedStoppingPlace(SUMOVehicle& veh, const MSStoppingPlace* stoppingPlace, bool blocked) = 0;

    virtual void rememberStoppingPlaceScore(SUMOVehicle& veh, MSStoppingPlace* place, const std::string& score) = 0;

    virtual void resetStoppingPlaceScores(SUMOVehicle& veh) = 0;

    virtual SUMOTime sawBlockedStoppingPlace(SUMOVehicle& veh, MSStoppingPlace* place, bool local) = 0;

    virtual StoppingPlaceParamMap_t collectWeights(SUMOVehicle& veh);

    double getWeight(SUMOVehicle& veh, const std::string param, const double defaultWeight);

    /// @brief keep track of the maximum values of each component
    static void updateMaxValues(StoppingPlaceParamMap_t& stoppingPlaceValues, StoppingPlaceParamMap_t& maxValues);

    /// @brief compute the final evaluation scalar from the components/weights
    static double getTargetValue(const StoppingPlaceParamMap_t& absValues, const StoppingPlaceParamMap_t& maxValues, const StoppingPlaceParamMap_t& weights, const StoppingPlaceParamSwitchMap_t& norm, const StoppingPlaceParamSwitchMap_t& invert);

private:

    ///@brief Constructor
    MSStoppingPlaceRerouter() = delete;

public:
    const StoppingPlaceParamMap_t myEvalParams;
    const StoppingPlaceParamSwitchMap_t myInvertParams;
    StoppingPlaceParamSwitchMap_t myNormParams;
    const SumoXMLTag myStoppingType;

private:
    const std::string myParamPrefix;
};
