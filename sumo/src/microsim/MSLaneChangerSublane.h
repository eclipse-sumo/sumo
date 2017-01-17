/****************************************************************************/
/// @file    MSLaneChangerSublane.cpp
/// @author  Jakob Erdmann
/// @date    Oct 2015
/// @version $Id$
///
// Performs sub-lane changing of vehicles
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
#ifndef MSLaneChangerSublane_h
#define MSLaneChangerSublane_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include "MSLaneChanger.h"


// ===========================================================================
// class declarations
// ===========================================================================


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSLaneChangerSublane
 * @brief Performs lane changing of vehicles
 */
class MSLaneChangerSublane : public MSLaneChanger {
public:
    /// Constructor
    MSLaneChangerSublane(const std::vector<MSLane*>* lanes, bool allowChanging);

    /// Destructor.
    ~MSLaneChangerSublane();

protected:

    /** Find a new candidate and try to change it. */
    virtual bool change();

    /** After the possible change, update the changer. */
    virtual void updateChanger(bool vehHasChanged);

    /** @brief check whether sub-lane changing in the given direction is desirable
     * and possible
     * @param[in] laneOffset The direction in which changing should be checked
     * @param[in] leaders The candidate vehicle's leaders
     * @param[in] preb The bestLanse of the candidaet vehicle
     * @param[out] latDist The distance by which the vehicle changes laterally
     */
    int checkChangeSublane(
        int laneOffset,
        const std::vector<MSVehicle::LaneQ>& preb,
        SUMOReal& latDist) const;

    ///  @brief change by the specified amount and return whether a new lane was entered
    bool startChangeSublane(MSVehicle* vehicle, ChangerIt& from, SUMOReal latDist);

    /// @brief get leaders for ego on the given lane
    MSLeaderDistanceInfo getLeaders(const ChangerIt& target, const MSVehicle* ego) const;

    typedef MSAbstractLaneChangeModel::StateAndDist StateAndDist;
    /// @brief helper function that calls checkChangeSublane and sets blocker information
    StateAndDist checkChangeHelper(MSVehicle* vehicle, int laneOffset);

private:
    /// Default constructor.
    MSLaneChangerSublane();

    /// Copy constructor.
    MSLaneChangerSublane(const MSLaneChangerSublane&);

    /// Assignment operator.
    MSLaneChangerSublane& operator=(const MSLaneChangerSublane&);
};


#endif

/****************************************************************************/

