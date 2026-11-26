/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    MSDepot.h
/// @author  Jakob Erdmann
/// @date    2026
///
// Constrains vehicle insertions and arrivals on a set of edges by vehicle type
/****************************************************************************/
#pragma once
#include <config.h>

#include <map>
#include <set>
#include <string>
#include <tuple>
#include <vector>
#include <microsim/MSMoveReminder.h>
#include <utils/common/Named.h>
#include <utils/common/Parameterised.h>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSEdge;
class MSLane;
class OutputDevice;
class SUMOPolygon;
class SUMOVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDepot
 * @brief Constrains the number of vehicles inserted and arriving in a given area
 */
class MSDepot : public Named, public Parameterised {
public:
    /// @brief Constructor
    MSDepot(const std::string& id,
            const std::vector<MSEdge*>& edges,
            const std::string& type,
            int initialCount,
            int maxCount,
            const std::string& file,
            SUMOPolygon* poly = nullptr);

    /// @brief Destructor
    ~MSDepot() override;

    /// @brief Whether any depot instances exist
    static inline bool hasDepots() {
        return !myInstances.empty();
    }

    /// @brief Return all MSDepot instances
    static const std::map<std::string, MSDepot*>& getInstances() {
        return myInstances;
    }

    /// @brief Check whether veh is allowed to depart on edge
    static bool canDepart(const MSEdge* edge, const SUMOVehicle& veh);

    /// @brief Notify depots on edge that veh has departed
    static void notifyDepart(const MSEdge* edge, const SUMOVehicle& veh, SUMOTime time);

    /// @brief Check whether veh is allowed to arrive on edge
    static bool canArrive(const MSEdge* edge, const SUMOVehicle& veh);

    /// @brief Notify depots on edge that veh has arrived
    static void notifyArrive(const MSEdge* edge, const SUMOVehicle& veh, SUMOTime time);

    /// @brief Warn if two depots share an edge for an overlapping type
    static void checkOverlappingDepots();

    /// @brief Reset depot counters to initialCount
    static void clearState();

    /// @brief Delete all depot instances
    static void cleanup();

    /// @brief Return whether veh matches the configured type or vTypeDistribution
    bool vehicleApplies(const SUMOVehicle& veh) const;

    /// @brief Return current vehicle count in the depot
    int getCount() const {
        return myCount;
    }

    /// @brief Return initial vehicle count
    int getInitialCount() const {
        return myInitialCount;
    }

    /// @brief Return maximum vehicle count
    int getMaxCount() const {
        return myMaxCount;
    }

    /// @brief Return configured type / vTypeDistribution ID
    const std::string& getType() const {
        return myType;
    }

    /// @brief Return depot edges
    const std::vector<MSEdge*>& getEdges() const {
        return myEdges;
    }

private:
    /// @class EdgeReminder
    /// @brief Move reminder tracking vehicle arrivals on depot edges
    class EdgeReminder : public MSMoveReminder {
    public:
        EdgeReminder(MSDepot* depot, MSLane* lane, bool doAdd);
        bool notifyEnter(SUMOTrafficObject& veh, Notification reason, const MSLane* enteredLane = nullptr) override;
        bool notifyLeave(SUMOTrafficObject& veh, double lastPos, Notification reason, const MSLane* enteredLane = nullptr) override;

    private:
        MSDepot* myDepot;

        EdgeReminder(const EdgeReminder&) = delete;
        EdgeReminder& operator=(const EdgeReminder&) = delete;
    };

    /// @brief Decrement count on departure and optionally write event output
    void handleDepart(const SUMOVehicle& veh, const MSEdge* edge, SUMOTime time);

    /// @brief Increment count on arrival (deduplicated per vehicle) and optionally write event output
    void handleArrive(const SUMOVehicle& veh, const MSEdge* edge, SUMOTime time);

    /// @brief Update polygon parameter and write optional XML event
    void recordEvent(const SUMOVehicle& veh, const MSEdge* edge, SUMOTime time, const std::string& action);

    /// @brief Update GUI shape parameter if polygon visualization is present
    void updatePolygonParameter();

    /// @brief Return shared type ID if two depots have overlapping vehicle types, else empty string
    static std::string getOverlappingType(const MSDepot& d1, const MSDepot& d2);

private:
    const std::vector<MSEdge*> myEdges;
    const std::string myType;
    const int myInitialCount;
    const int myMaxCount;
    int myCount;
    OutputDevice* myOutput;
    SUMOPolygon* myPolygon;
    std::vector<EdgeReminder*> myReminders;
    SUMOTime myLastArriveStep;
    std::set<long long int> myArrivedVehNumericalIDs;

    static std::map<std::string, MSDepot*> myInstances;
    static std::map<const MSEdge*, std::vector<MSDepot*> > myEdgeDepots;
    static std::set<std::tuple<std::string, std::string, std::string> > myWarnedOverlaps;
    static bool myOverlapsChecked;

    MSDepot(const MSDepot&) = delete;
    MSDepot& operator=(const MSDepot&) = delete;
};
