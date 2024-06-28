/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2021 German Aerospace Center (DLR) and others.
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
/// @file    MSDriveWay.h
/// @author  Jakob Erdmann
/// @date    December 2021
///
// A sequende of rail tracks (lanes) that may be used as a "set route" (Fahrstraße)
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/common/Named.h>
#include <microsim/MSMoveReminder.h>

// ===========================================================================
// class declarations
// ===========================================================================
class SUMOVehicle;
class MSLane;
class MSLink;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDriveWay
 */
class MSDriveWay : public MSMoveReminder, public Named {
public:
    typedef std::pair<const SUMOVehicle* const, const MSLink::ApproachingVehicleInformation> Approaching;
    typedef std::set<const MSLane*, ComparatorNumericalIdLess> LaneSet;
    typedef std::map<const MSLane*, int, ComparatorNumericalIdLess> LaneVisitedMap;

    /*  The driveways (Fahrstrassen) for each link index of MSRailSignal
     *  Each link index has at least one driveway
     *  A driveway describes one possible route that passes the signal up to
     *  the next secure point
     *  When a signal guards a switch (indirect guard) that signal stores two
     *  or more driveways
     */
    MSDriveWay(const std::string& id, bool temporary = false);

    /// @brief Destructor
    virtual ~MSDriveWay();

    bool notifyEnter(SUMOTrafficObject& veh, Notification reason, const MSLane* enteredLane);
    bool notifyLeave(SUMOTrafficObject& veh, double lastPos, Notification reason, const MSLane* enteredLane = 0);
    bool notifyLeaveBack(SUMOTrafficObject& veh, Notification reason, const MSLane* leftLane);

    /// @brief Wether there is a flank conflict with the given driveway
    bool flankConflict(const MSDriveWay& other) const;
    bool crossingConflict(const MSDriveWay& other) const;

    /// @brief whether any of myConflictLanes is occupied (vehicles that are the target of a join must be ignored)
    bool conflictLaneOccupied(const std::string& joinVehicle = "", bool store = true, const SUMOVehicle* ego = nullptr) const;

    /// @brief whether any of myFoes is occupied (vehicles that are the target of a join must be ignored)
    bool foeDriveWayOccupied(const std::string& joinVehicle, bool store, const SUMOVehicle* ego) const;

    /// @brief Whether any of the conflict links have approaching vehicles
    bool conflictLinkApproached() const;

    /// @brief attempt reserve this driveway for the given vehicle
    bool reserve(const Approaching& closest, MSEdgeVector& occupied);

    /// @brief find protection for the given vehicle  starting at a switch
    bool findProtection(const Approaching& veh, MSLink* link) const;

    /// @brief Write block items for this driveway
    void writeBlocks(OutputDevice& od) const;
    void writeBlockVehicles(OutputDevice& od) const;

    const std::vector<const MSEdge*>& getRoute() const {
        return myRoute;
    }

    const std::vector<const MSLane*>& getFlank() const {
        return myFlank;
    }

    const std::vector<const MSLane*>& getBidi() const {
        return myBidi;
    }

    const std::vector<MSLink*>& getConflictLinks() const {
        return myConflictLinks;
    }

    const std::vector<MSLink*>& getProtectingSwitchesBidi() const {
        return myProtectingSwitchesBidi;
    }

    int getNumericalID() const {
        return myNumericalID;
    }

    /// @brief whether the give route matches this driveway
    bool match(const MSRoute& route, MSRouteIterator firstIt) const;

    static void init();

    /// @brief Whether veh must yield to the foe train
    static bool mustYield(const Approaching& veh, const Approaching& foe);

    /// @brief construct a new driveway by searching along the given route until all block structures are found
    static MSDriveWay* buildDriveWay(const std::string& id, const MSLink* link, MSRouteIterator first, MSRouteIterator end);

    /// @brief return logicID_linkIndex in a way that allows clicking in sumo-gui
    static std::string getClickableTLLinkID(const MSLink* link);

    static void buildDepartureDriveway(const SUMOVehicle* veh);

    static void writeDepatureBlocks(OutputDevice& od, bool writeVehicles);

protected:

    /// @brief global driveway index
    int myNumericalID;

    /// @brief the maximum flank length searched while building this driveway
    double myMaxFlankLength;

    /// @brief whether the current signal is switched green for a train approaching this block
    const SUMOVehicle* myActive;

    /// @brief switch assumed safe from bidi-traffic
    const MSEdge* myProtectedBidi;

    /// @brief list of edges for matching against train routes
    std::vector<const MSEdge*> myRoute;

    /// @brief number of edges in myRoute where overlap with other driveways is forbidden
    int myCoreSize;

    /// @brief whether this driveway ends its forward section with a rail signal (and thus comprises a full block)
    bool myFoundSignal;
    bool myFoundReversal;

    /* @brief the actual driveway part up to the next railsignal (halting position)
     * This must be free of other trains */
    std::vector<const MSLane*> myForward;

    /* @brief the list of bidirectional edges that can enter the forward
     * section and which must also be free of traffic
     * (up to the first element that could give protection) */
    std::vector<const MSLane*> myBidi;

    /* @brief the list of bidirectional edges that can enter the forward
     * section and which might contain deadlock-relevant traffic */
    std::vector<const MSLane*> myBidiExtended;

    /* @brief the list of edges that merge with the forward section
     * (found via backward search, up to the first element that could give protection) */
    std::vector<const MSLane*> myFlank;

    /// @brief the lanes that must be clear of trains before this signal can switch to green
    std::vector<const MSLane*> myConflictLanes;

    /* @brief the list of (first) switches that could give protection from oncoming/flanking vehicles
     * if any of them fails to do so, upstream search must be performed
     * until protection or conflict is found
     */
    std::vector<MSLink*> myProtectingSwitches;
    /// @brief subset of myProtectingSwitches that protects from oncoming trains
    std::vector<MSLink*> myProtectingSwitchesBidi;

    /* The conflict links for this block
     * Conflict resolution must be performed if vehicles are approaching the
     * current link and any of the conflict links */
    std::vector<MSLink*> myConflictLinks;

    /// @brief whether any of myBidiExtended is occupied by a vehicle that targets myBidi
    bool deadlockLaneOccupied(bool store = true) const;

    /// @brief Whether the approaching vehicle is prevent from driving by another vehicle approaching the given link
    bool hasLinkConflict(const Approaching& closest, MSLink* foeLink) const;

    /// @brief Wether this driveway (route) overlaps with the given one
    bool overlap(const MSDriveWay& other) const;

    /* @brief determine route that identifies this driveway (a subset of the
     * vehicle route)
     * collects:
     *   myRoute
     *   myForward
     *   myBidi
     *   myProtectedBidi
     *
     * returns edge that is assumed to safe from oncoming-deadlock or nullptr
     */
    void buildRoute(const MSLink* origin, double length, MSRouteIterator next, MSRouteIterator end, LaneVisitedMap& visited, std::set<MSLink*>&);

    /* @brief find switches that threaten this driveway
     * @param[out] flankSwitches collect the switches
     */
    void checkFlanks(const MSLink* originLink, const std::vector<const MSLane*>& lanes, const LaneVisitedMap& visited, bool allFoes, std::set<MSLink*>& flankSwitches) const;

    /* @brief find links that cross the driveway without entering it
     * @param[out] flankSwitches collect the switches
     */
    void checkCrossingFlanks(MSLink* dwLink, const LaneVisitedMap& visited, std::set<MSLink*>& flankSwitches) const;

    /* @brief find upstream protection from the given link
     * @param[out] flank: the stored flank lanes
     */
    void findFlankProtection(MSLink* link, double length, LaneVisitedMap& visited, MSLink* origLink, std::vector<const MSLane*>& flank);

    /// @brief add all driveWays that start at the given link as foes
    void addFoes(const MSLink* link);

    /// @brief add all driveWays that pass the given link as foes
    void addSwitchFoes(const MSLink* link);

    /// @brief derive foe driveways based on myBidi
    void addBidiFoes(const MSRailSignal* ownSignal);

    /// @brief add symmetical conflict link for foes when building a new driveway
    void addConflictLink(const MSLink* link);

    /// @brief return logicID_linkIndex
    static std::string getTLLinkID(const MSLink* link);

    /// @brief return junctionID_junctionLinkIndex
    static std::string getJunctionLinkID(const MSLink* link);

    /// @brief print link descriptions
    static std::string formatVisitedMap(const LaneVisitedMap& visited);

    /// @brief append to map by map index and avoid undefined behavior
    static void appendMapIndex(LaneVisitedMap& map, const MSLane* lane);

private:

    std::set<SUMOVehicle*> myTrains;

    struct VehicleEvent {
        VehicleEvent(SUMOTime _time, bool _isEntry, const std::string& _id, Notification _reason):
            time(_time), isEntry(_isEntry), id(_id), reason(_reason) {}
        SUMOTime time;
        bool isEntry;
        std::string id;
        Notification reason;
    };
    std::vector<VehicleEvent> myVehicleEvents;
    std::vector<MSDriveWay*> myFoes;

    /// @brief track own occurences in mySwitchDriveWays for cleanup in destructor
    std::vector<const MSLink*> myForwardSwitches;

    static int myGlobalDriveWayIndex;
    static int myDepartDriveWayIndex;
    static int myNumWarnings;
    static bool myWriteVehicles;

    /// @brief all driveways passing the given switch (used to look up flank foes)
    static std::map<const MSLink*, std::vector<MSDriveWay*> > mySwitchDriveWays;

    /// @brief all driveways that do not start at a rail signal (and are only used at departure)
    static std::map<const MSEdge*, std::vector<MSDriveWay*> > myDepartureDriveways;

    /// @brief all driveways that end on the given edge
    static std::map<const MSEdge*, std::vector<MSDriveWay*> > myEndingDriveways;

};


