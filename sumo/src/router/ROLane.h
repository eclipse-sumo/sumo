/****************************************************************************/
/// @file    ROLane.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Sept 2002
/// @version $Id$
///
// A single lane the router may use
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef ROLane_h
#define ROLane_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <utils/common/Named.h>
#include <utils/common/SUMOVehicleClass.h>
#include "ROEdge.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROLane
 * @brief A single lane the router may use
 *
 * Currently, the lane has no other purpose then storing the allowed vehicle
 *  classes. They are even only stored herein and used by computing the vehicle
 *  classes allowed on the according edge.
 * @see ROEdge
 */
class ROLane : public Named {
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the lane
     * @param[in] length The length of the lane
     * @param[in] maxSpeed The maximum speed allowed on the lane
     * @param[in] permissions Vehicle classes that may pass this lane
     */
    ROLane(const std::string& id, ROEdge* edge, SUMOReal length, SUMOReal maxSpeed, SVCPermissions permissions) :
        Named(id), myEdge(edge), myLength(length), myMaxSpeed(maxSpeed), myPermissions(permissions)
    {}


    /// @brief Destructor
    ~ROLane() { }


    /** @brief Returns the length of the lane
     * @return The length of this lane
     */
    SUMOReal getLength() const {
        return myLength;
    }


    /** @brief Returns the maximum speed allowed on this lane
     * @return The maximum speed allowed on this lane
     */
    SUMOReal getSpeed() const {
        return myMaxSpeed;
    }


    /** @brief Returns the list of allowed vehicle classes
     * @return The list of vehicle classes allowed on this lane
     */
    inline SVCPermissions getPermissions() {
        return myPermissions;
    }

    /** @brief Returns the lane's edge
     * @return This lane's edge
     */
    ROEdge& getEdge() const {
        return *myEdge;
    }

    /// @brief get the list of outgoing lanes
    const std::vector<const ROLane*>& getOutgoingLanes() const {
        return myOutgoingLanes;
    }

    void addOutgoingLane(ROLane* lane) {
        myOutgoingLanes.push_back(lane);
    }

    /// @brief get the state of the link from the logical predecessor to this lane (ignored for routing)
    inline LinkState getIncomingLinkState() const {
        return LINKSTATE_MAJOR;
    }

    inline bool allowsVehicleClass(SUMOVehicleClass vclass) const {
        return (myPermissions & vclass) == vclass;
    }

private:
    /// @brief The parent edge of this lane
    ROEdge* myEdge;

    /// @brief The length of the lane
    SUMOReal myLength;

    /// @brief The maximum speed allowed on the lane
    SUMOReal myMaxSpeed;

    /// @brief The encoding of allowed vehicle classes
    SVCPermissions myPermissions;

    std::vector<const ROLane*> myOutgoingLanes;


private:
    /// @brief Invalidated copy constructor
    ROLane(const ROLane& src);

    /// @brief Invalidated assignment operator
    ROLane& operator=(const ROLane& src);

};


#endif

/****************************************************************************/

