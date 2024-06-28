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
// to the best StoppingPlace according to the evaluation components and
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
#include <microsim/MSNet.h>
#include <microsim/MSStoppingPlace.h>

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
    StoppingPlaceParamSwitchMap_t invertParams = { {"probability", false}, { "capacity", true }, { "timefrom", false }, { "timeto", false }, { "distancefrom", false }, { "distanceto", false }, { "absfreespace", true }, { "relfreespace", true }, });

    // Destructor
    virtual ~MSStoppingPlaceRerouter() {}

    /** @brief main method to trigger the rerouting to the "best" StoppingPlace according to the custom evaluation function
     *
     * @param[in] veh the concerned vehicle
     * @param[in] stoppingPlaceCandidates stopping places to choose from and whether they are visible for the vehicle
     * @param[in] probs probabilities of all candidate stopping places
     * @param[in] newDestination whether the destination changed
     * @param[out] newRoute the route to/from the chosen stopping place is stored here
     * @param[in] closedEdges edges to avoid during routing
     * @return the best stopping place according to the target function or nullptr
     */
    MSStoppingPlace* reroute(std::vector<StoppingPlaceVisible>& stoppingPlaceCandidates, const std::vector<double>& probs, SUMOVehicle& veh,
                             bool& newDestination, ConstMSEdgeVector& newRoute, const MSEdgeVector& closedEdges = {});

    /** @brief compute the target function for a single alternative
     *
     * @param[in] veh the concerned vehicle
     * @param[in] brakeGap the distance before which the vehicle cannot stop
     * @param[in] newDestination whether the destination changed
     * @param[in] alternative the stopping place to evaluate
     * @param[in] occupancy occupancy of the stopping place
     * @param[in] prob the predefined probability of this stopping place
     * @param[in] router the router to use for evaluation if needed
     * @param[in,out] stoppingPlaces the data structure to write the evaluation values to
     * @param[in,out] newRoutes the data structure to write the chosen route to/from the stopping place to
     * @param[in,out] stoppingPlaceApproaches the data structure to write the chosen route to the stopping place to
     * @param[in,out] maxValues maximum values for all evaluation components
     * @return false if the stopping place cannot be used
     */
    virtual bool evaluateDestination(SUMOVehicle& veh, double brakeGap, bool newDestination,
                                     MSStoppingPlace* alternative, double occupancy, double prob,
                                     SUMOAbstractRouter<MSEdge, SUMOVehicle>& router, StoppingPlaceMap_t& stoppingPlaces,
                                     std::map<MSStoppingPlace*, ConstMSEdgeVector>& newRoutes,
                                     std::map<MSStoppingPlace*, ConstMSEdgeVector>& stoppingPlaceApproaches,
                                     StoppingPlaceParamMap_t& maxValues);

    /** @brief Compute some custom target function components
     *
     * @param[in] veh the concerned vehicle
     * @param[in] brakeGap the distance before which the vehicle cannot stop
     * @param[in] newDestination whether the destination changed
     * @param[in] alternative the stopping place to evaluate
     * @param[in] occupancy occupancy of the stopping place
     * @param[in] router the router to use for evaluation if needed
     * @param[in,out] stoppingPlaceValues the data structure to write the evaluation values to
     * @param[in] newRoute the complete route to the destination passing by the stopping place
     * @param[in] stoppingPlaceApproach the route to the stopping place
     * @return false if the stopping place cannot be used according to the custom evaluation components
     */
    virtual bool evaluateCustomComponents(SUMOVehicle& veh, double brakeGap, bool newDestination,
                                          MSStoppingPlace* alternative, double occupancy, double prob,
                                          SUMOAbstractRouter<MSEdge, SUMOVehicle>& router, StoppingPlaceParamMap_t& stoppingPlaceValues,
                                          ConstMSEdgeVector& newRoute,
                                          ConstMSEdgeVector& stoppingPlaceApproach,
                                          StoppingPlaceParamMap_t& maxValues);

    /// @brief Return the number of occupied places of the StoppingPlace
    virtual double getStoppingPlaceOccupancy(MSStoppingPlace* stoppingPlace) = 0;

    /// @brief Return the number of occupied places of the StoppingPlace from the previous time step
    virtual double getLastStepStoppingPlaceOccupancy(MSStoppingPlace* stoppingPlace) = 0;

    /// @brief Return the number of places the StoppingPlace provides
    virtual double getStoppingPlaceCapacity(MSStoppingPlace* stoppingPlace) = 0;

    /// @brief store the blocked stopping place in the vehicle
    virtual void rememberBlockedStoppingPlace(SUMOVehicle& veh, const MSStoppingPlace* stoppingPlace, bool blocked) = 0;

    /// @brief store the stopping place score in the vehicle
    virtual void rememberStoppingPlaceScore(SUMOVehicle& veh, MSStoppingPlace* place, const std::string& score) = 0;

    /// @brief forget all stopping place score for this vehicle
    virtual void resetStoppingPlaceScores(SUMOVehicle& veh) = 0;

    /// @brief ask the vehicle when it has seen the stopping place
    virtual SUMOTime sawBlockedStoppingPlace(SUMOVehicle& veh, MSStoppingPlace* place, bool local) = 0;

    /// @brief ask how many times already the vehicle has been rerouted to another stopping place
    virtual int getNumberStoppingPlaceReroutes(SUMOVehicle& veh) = 0;

    /// @brief update the number of reroutes for the vehicle
    virtual void setNumberStoppingPlaceReroutes(SUMOVehicle& veh, int value) = 0;

    /// @brief read target function weights for this vehicle
    virtual StoppingPlaceParamMap_t collectWeights(SUMOVehicle& veh);

    /** @brief read the value of a stopping place search param, e.g. a component weight factor
     *
     * @param[in] veh the concerned vehicle
     * @param[in] param the name of the stopping place search param, excluding the param prefix (e.g. "parking.")
     * @param[in] defaultWeight value to return in case the param hasn't been defined for the vehicle
     * @param[in] warn whether a warning message shall be issued if the param is not defined for the vehicle
     * @return param value
     */
    double getWeight(SUMOVehicle& veh, const std::string param, const double defaultWeight, const bool warn = false);

    /** @brief keep track of the maximum values of each component
     *
     * @param[in] stoppingPlaceValues the target function component values of a vehicle
     * @param[in,out] maxValues stores the maximum values of the given stoppingPlaceValues and previously given maxValues
     */
    static void updateMaxValues(StoppingPlaceParamMap_t& stoppingPlaceValues, StoppingPlaceParamMap_t& maxValues);

    /** @brief compute the scalar target function value by means of a linear combination of all components/weights after normalising and optionally inverting the values
     *
     * @param[in] absValues the component values
     * @param[in] maxValues max values for all components
     * @param[in] weights weight factors for all components
     * @param[in] norm which component should be normalised
     * @param[in] invert which component should be inverted
     * @return target function value for a single stopping place and vehicle
     */
    static double getTargetValue(const StoppingPlaceParamMap_t& absValues, const StoppingPlaceParamMap_t& maxValues, const StoppingPlaceParamMap_t& weights, const StoppingPlaceParamSwitchMap_t& norm, const StoppingPlaceParamSwitchMap_t& invert);

protected:
    /// @brief Ask the vehicle about the relevant rerouting parameters and initiate the maximum value data structure
    void readEvaluationWeights(SUMOVehicle& veh, StoppingPlaceParamMap_t& stoppingPlaceParams, StoppingPlaceParamMap_t& stoppingPlaceDefaults, StoppingPlaceParamMap_t& maxValues) {
        for (StoppingPlaceParamMap_t::iterator it = stoppingPlaceParams.begin(); it != stoppingPlaceParams.end(); ++it) {
            double value = getWeight(veh, it->first, stoppingPlaceDefaults[it->first]);
            it->second = value;
            if (value > maxValues[it->first]) {
                maxValues[it->first] = value;
            }
        }
    }

private:

    ///@brief Constructor
    MSStoppingPlaceRerouter() = delete;

protected:
    const SumoXMLTag myStoppingType;
    const std::string myParamPrefix;
    StoppingPlaceParamMap_t myEvalParams;
    StoppingPlaceParamSwitchMap_t myNormParams;
    const StoppingPlaceParamSwitchMap_t myInvertParams;
};
