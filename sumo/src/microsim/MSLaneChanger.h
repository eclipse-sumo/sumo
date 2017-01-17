/****************************************************************************/
/// @file    MSLaneChanger.h
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Fri, 01 Feb 2002
/// @version $Id$
///
// Performs lane changing of vehicles
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSLaneChanger_h
#define MSLaneChanger_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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

        /// the vehicle in front of the current vehicle
        MSVehicle*                lead;
        /// the lane the vehicle is on
        MSLane*                   lane;
        /// last vehicle that changed into this lane
        MSVehicle*                hoppedVeh;
        /// the vehicle that really wants to change to this lane
        MSVehicle*                lastBlocked;
        /// the first vehicle on this edge that wants to change to this lane
        MSVehicle*                firstBlocked;

        SUMOReal dens;

        /// @name Members which are used only by MSLaneChangerSublane
        /// @{
        // the vehicles in from of the current vehicle
        MSLeaderInfo ahead;


        void addLink(MSLink* link);
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

protected:
    /// Initialize the changer before looping over all vehicles.
    void initChanger();

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
    virtual bool changeOpposite(std::pair<MSVehicle*, SUMOReal> leader);

    /** Update changer for vehicles that did not change */
    void registerUnchanged(MSVehicle* vehicle);

    /** After the possible change, update the changer. */
    virtual void updateChanger(bool vehHasChanged);

    /** During lane-change a temporary vehicle container is filled within
        the lanes (bad pratice to modify foreign members, I know). Swap
        this container with the real one. */
    void updateLanes(SUMOTime t);

    /** @brief Find current candidate.
        If there is none, myChanger.end() is returned. */
    ChangerIt findCandidate();

    /* @brief check whether lane changing in the given direction is desirable
     * and possible */
    int checkChangeWithinEdge(
        int laneOffset,
        const std::pair<MSVehicle* const, SUMOReal>& leader,
        const std::vector<MSVehicle::LaneQ>& preb) const;

    /* @brief check whether lane changing in the given direction is desirable
     * and possible */
    int checkChange(
        int laneOffset,
        const MSLane* targetLane,
        const std::pair<MSVehicle* const, SUMOReal>& leader,
        const std::pair<MSVehicle* const, SUMOReal>& neighLead,
        const std::pair<MSVehicle* const, SUMOReal>& neighFollow,
        const std::vector<MSVehicle::LaneQ>& preb) const;

    ///  @brief start the lane change maneuver (and finish it instantly if gLaneChangeDuration == 0)
    void startChange(MSVehicle* vehicle, ChangerIt& from, int direction);

    ///  @brief continue a lane change maneuver and return whether the midpoint was passed in this step (used if gLaneChangeDuration > 0)
    bool continueChange(MSVehicle* vehicle, ChangerIt& from);

    std::pair<MSVehicle* const, SUMOReal> getRealFollower(const ChangerIt& target) const;

    std::pair<MSVehicle* const, SUMOReal> getRealLeader(const ChangerIt& target) const;

    /// @brief whether changing to the lane in the given direction should be considered
    bool mayChange(int direction) const;

    /// @brief return the closer follower of ego
    static MSVehicle* getCloserFollower(const SUMOReal maxPos, MSVehicle* follow1, MSVehicle* follow2);

    /** @brief Compute the time and space required for overtaking the given leader
     * @param[in] vehicle The vehicle that wants to overtake
     * @param[in] leader The vehicle to be overtaken
     * @param[in] gap The gap between vehicle and leader
     * @param[out] timeToOvertake The time for overtaking
     * @param[out] spaceToOvertake The space for overtaking
     */
    static void computeOvertakingTime(const MSVehicle* vehicle, const MSVehicle* leader, SUMOReal gap, SUMOReal& timeToOvertake, SUMOReal& spaceToOvertake);

protected:
    /// Container for ChangeElemements, one for every lane in the edge.
    Changer   myChanger;

    /** Change-candidate. Last of the vehicles in changer. Only this one
        will try to change. Every vehicle on the edge will be a candidate
        once in the change-process. */
    ChangerIt myCandi;

    /* @brief Whether vehicles may start to change lanes on this edge
     * (finishing a change in progress is always permitted) */
    bool myAllowsChanging;

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


#endif

/****************************************************************************/

