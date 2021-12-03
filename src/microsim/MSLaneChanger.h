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
/// @file    MSLaneChanger.h
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Fri, 01 Feb 2002
///
// Performs lane changing of vehicles
/****************************************************************************/
#pragma once
#include <config.h>

#include "MSLane.h"
#include "MSEdge.h"
#include "MSVehicle.h"
#include <vector>
#include <utils/iodevices/OutputDevice.h>


// ===========================================================================
// class declarations
// ===========================================================================


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSLaneChanger
 * @brief Performs lane changing of vehicles
 */
class MSLaneChanger {
public:
    /// Constructor
    MSLaneChanger(const std::vector<MSLane*>* lanes, bool allowChanging);

    /// Destructor.
    virtual ~MSLaneChanger();

    /// Start lane-change-process for all vehicles on the edge'e lanes.
    void laneChange(SUMOTime t);

public:
    /** Structure used for lane-change. For every lane you have to
        know four vehicles, the change-candidate veh and it's follower
        and leader. Further, information about the last vehicle that changed
        into this lane is needed */
    struct ChangeElem {

        ChangeElem(MSLane* _lane);

        /// @brief Register that vehicle belongs to Changer Item to after LC decisions
        void registerHop(MSVehicle* vehicle);

        ///@brief the leader vehicle for the current change candidate
        MSVehicle*                lead;
        ///@brief the lane corresponding to this ChangeElem (the current change candidate is on this lane)
        MSLane*                   lane;
        ///@brief last vehicle that changed into this lane
        MSVehicle*                hoppedVeh;
        /// @brief the next vehicle downstream of the ego vehicle that is blocked from changing to this lane
        MSVehicle*                lastBlocked;
        /// @brief the farthest downstream vehicle on this edge that is blocked from changing to this lane
        MSVehicle*                firstBlocked;

        double dens;

        /// @brief whether changing is possible to either direction
        bool mayChangeRight;
        bool mayChangeLeft;

        /// relative indices of internal lanes with the same origin lane (siblings)
        /// only used for changes on internal edges
        std::vector<int>          siblings;

        /// @name Members which are used only by MSLaneChangerSublane
        /// @{
        // the vehicles in front of the current vehicle (only on the current edge, continously updated during change() )
        MSLeaderInfo ahead;

        // the vehicles in front of the current vehicle (including those on the next edge, contiously update during change() ))
        MSLeaderDistanceInfo aheadNext;
        ///@}

    };

public:
    /** @brief The list of changers;
        For each lane, a ChangeElem is being build */
    typedef std::vector< ChangeElem > Changer;

    /// the iterator moving over the ChangeElems
    typedef Changer::iterator ChangerIt;

    /// the iterator moving over the ChangeElems
    typedef Changer::const_iterator ConstChangerIt;

    /// @brief return changer (only to be used by MSLaneChangerSublane from another instance)
    Changer& getChanger() {
        return myChanger;
    }


protected:
    /// Initialize the changer before looping over all vehicles.
    virtual void initChanger();

    /** @brief Check if there is a single change-candidate in the changer.
        Returns true if there is one. */
    bool vehInChanger() const {
        // If there is at least one valid vehicle under the veh's in myChanger
        // return true.
        for (ConstChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce) {
            if (veh(ce) != 0) {
                return true;
            }
        }
        return false;
    }

    /** Returns the furthes unhandled vehicle on this change-elements lane
        or 0 if there is none. */
    MSVehicle* veh(ConstChangerIt ce) const {
        // If ce has a valid vehicle, return it. Otherwise return 0.
        if (!ce->lane->myVehicles.empty()) {
            return ce->lane->myVehicles.back();
        } else {
            return 0;
        }
    }


    /** Find a new candidate and try to change it. */
    virtual bool change();


    /** try changing to the opposite direction edge. */
    bool changeOpposite(MSVehicle* vehicle, std::pair<MSVehicle*, double> leader);
    std::pair<MSVehicle* const, double> getOncomingVehicle(const MSLane* opposite, std::pair<MSVehicle*,
            double> neighOncoming, double searchDist, double& vMax, const MSVehicle* overtaken = nullptr);

    /** Update changer for vehicles that did not change */
    void registerUnchanged(MSVehicle* vehicle);

    /// @brief Take into account traci LC-commands.
    /// @note This is currently only used within non-actionsteps.
    void checkTraCICommands(MSVehicle* vehicle);

    /// @brief Execute TraCI LC-commands.
    /// @note This is currently only used within non-actionsteps for the non-sublane model.
    /// @return whether lane was changed
    bool applyTraCICommands(MSVehicle* vehicle);

    /** After the possible change, update the changer. */
    virtual void updateChanger(bool vehHasChanged);

    /** During lane-change a temporary vehicle container is filled within
        the lanes (bad practice to modify foreign members, I know). Swap
        this container with the real one. */
    void updateLanes(SUMOTime t);

    /** @brief Find current candidate.
        If there is none, myChanger.end() is returned. */
    ChangerIt findCandidate();

    /* @brief check whether lane changing in the given direction is desirable
     * and possible */
    int checkChangeWithinEdge(
        int laneOffset,
        const std::pair<MSVehicle* const, double>& leader,
        const std::vector<MSVehicle::LaneQ>& preb) const;

    /* @brief check whether lane changing in the given direction is desirable
     * and possible */
    int checkChange(
        int laneOffset,
        const MSLane* targetLane,
        const std::pair<MSVehicle* const, double>& leader,
        const std::pair<MSVehicle* const, double>& follower,
        const std::pair<MSVehicle* const, double>& neighLead,
        const std::pair<MSVehicle* const, double>& neighFollow,
        const std::vector<MSVehicle::LaneQ>& preb) const;

    /* @brief call lanechange model to check the merits of an opposite-direction
     * change and update state accordingly */
    virtual bool checkChangeOpposite(
        MSVehicle* vehicle,
        int laneOffset,
        MSLane* targetLane,
        const std::pair<MSVehicle* const, double>& leader,
        const std::pair<MSVehicle* const, double>& neighLead,
        const std::pair<MSVehicle* const, double>& neighFollow,
        const std::vector<MSVehicle::LaneQ>& preb);


    /*  @brief start the lane change maneuver (and finish it instantly if gLaneChangeDuration == 0)
     *  @return False when aborting the change due to being remote controlled*/
    bool startChange(MSVehicle* vehicle, ChangerIt& from, int direction);

    ///  @brief continue a lane change maneuver and return whether the vehicle has completely moved onto the new lane (used if gLaneChangeDuration > 0)
    bool continueChange(MSVehicle* vehicle, ChangerIt& from);

    std::pair<MSVehicle* const, double> getRealFollower(const ChangerIt& target) const;

    std::pair<MSVehicle* const, double> getRealLeader(const ChangerIt& target) const;

    /// @brief whether changing to the lane in the given direction should be considered
    bool mayChange(int direction) const;

    /// @brief return the closer follower of ego
    static MSVehicle* getCloserFollower(const double maxPos, MSVehicle* follow1, MSVehicle* follow2);

    /** @brief Compute the time and space required for overtaking the given leader
     * @param[in] vehicle The vehicle that wants to overtake
     * @param[in] leader The vehicle to be overtaken
     * @param[in] gap The gap between vehicle and leader
     * @param[out] timeToOvertake The time for overtaking
     * @param[out] spaceToOvertake The space for overtaking
     */
    static void computeOvertakingTime(const MSVehicle* vehicle, double vMax, const MSVehicle* leader, double gap, double& timeToOvertake, double& spaceToOvertake);

    // @brief return leader vehicle that is to be overtaken
    static std::pair<MSVehicle*, double> getColumnleader(MSVehicle* vehicle, std::pair<MSVehicle*, double> leader, double maxLookAhead = std::numeric_limits<double>::max());

    /// @brief return the next lane in conts beyond lane or nullptr
    static const MSLane* getLaneAfter(const MSLane* lane, const std::vector<MSLane*>& conts, bool allowMinor);

    /// @brief whether vehicle has an opposite-direction stop within relevant range
    static bool hasOppositeStop(MSVehicle* vehicle);

    // @brief compute distance that can safely be driven on the opposite side
    static double computeSurplusGap(const MSVehicle* vehicle, const MSLane* opposite, std::pair<MSVehicle*, double> oncoming, double timeToOvertake, double spaceToOvertake, double& oncomingSpeed);

    // @brief find hilltop within searchDistance
    static bool foundHilltop(MSVehicle* vehicle, bool foundHill, double searchDist, const std::vector<MSLane*>& bestLanes, int view, double pos, double lastMax, double hilltopThreshold);

    /// @brief add LaneQ for opposite lanes
    static std::vector<MSVehicle::LaneQ> getBestLanesOpposite(MSVehicle* vehicle, const MSLane* stopLane, double oppositeLength);

protected:
    /// Container for ChangeElemements, one for every lane in the edge.
    Changer   myChanger;

    /** Change-candidate. Last of the vehicles in changer. Only this one
        will try to change. Every vehicle on the edge will be a candidate
        once in the change-process. */
    ChangerIt myCandi;

    /* @brief Whether vehicles may start to change lanes on this edge
     * (finishing a change in progress is always permitted) */
    const bool myAllowsChanging;

    /// @brief whether this edge allows changing to the opposite direction edge
    const bool myChangeToOpposite;

private:
    /// Default constructor.
    MSLaneChanger();

    /// Copy constructor.
    MSLaneChanger(const MSLaneChanger&);

    /// Assignment operator.
    MSLaneChanger& operator=(const MSLaneChanger&);
};
