/****************************************************************************/
/// @file    MSStoppingPlace.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 13.12.2005
/// @version $Id$
///
// A lane area vehicles can halt at
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2005-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSStoppingPlace_h
#define MSStoppingPlace_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <algorithm>
#include <map>
#include <string>
#include <utils/common/Named.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;
class SUMOVehicle;
class MSTransportable;
class Position;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSStoppingPlace
 * @brief A lane area vehicles can halt at
 *
 * The stop tracks the last free space a vehicle may halt at by being
 *  informed about a vehicle's entering and depart. It keeps the information
 *  about entered vehicles' begin and end position within an internal
 *  container ("myEndPositions") and is so able to compute the last free space.
 *
 * Please note that using the last free space disallows vehicles to enter a
 *  free space in between other vehicles.
 */
class MSStoppingPlace : public Named {
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the stop
     * @param[in] net The net the stop belongs to
     * @param[in] lines Names of the lines that halt on this stop
     * @param[in] lane The lane the stop is placed on
     * @param[in] begPos Begin position of the stop on the lane
     * @param[in] endPos End position of the stop on the lane
     */
    MSStoppingPlace(const std::string& id,
                    const std::vector<std::string>& lines, MSLane& lane,
                    SUMOReal begPos, SUMOReal endPos);


    /// @brief Destructor
    virtual ~MSStoppingPlace();


    /** @brief Returns the lane this stop is located at
     *
     * @return Reference to the lane the stop is located at
     */
    const MSLane& getLane() const;


    /** @brief Returns the begin position of this stop
     *
     * @return The position the stop begins at
     */
    SUMOReal getBeginLanePosition() const;


    /** @brief Returns the end position of this stop
     *
     * @return The position the stop ends at
     */
    SUMOReal getEndLanePosition() const;


    /** @brief Called if a vehicle enters this stop
     *
     * Stores the position of the entering vehicle in myEndPositions.
     *
     * Recomputes the free space using "computeLastFreePos" then.
     *
     * @param[in] what The vehicle that enters the bus stop
     * @param[in] beg The begin halting position of the vehicle
     * @param[in] what The end halting position of the vehicle
     * @see computeLastFreePos
     */
    void enter(SUMOVehicle* what, SUMOReal beg, SUMOReal end);


    /** @brief Called if a vehicle leaves this stop
     *
     * Removes the position of the vehicle from myEndPositions.
     *
     * Recomputes the free space using "computeLastFreePos" then.
     *
     * @param[in] what The vehicle that leaves the bus stop
     * @see computeLastFreePos
     */
    void leaveFrom(SUMOVehicle* what);


    /** @brief Returns the last free position on this stop
     *
     * @return The last free position of this bus stop
     */
    SUMOReal getLastFreePos(const SUMOVehicle& forVehicle) const;


    /** @brief Returns the next free waiting place for pedestrians / containers
     *
     * @return The next free waiting place for pedestrians / containers
     */
    Position getWaitPosition() const;


    /** @brief For vehicles at the stop this gives the the actual stopping
     *         position of the vehicle. For all others the last free stopping position
     *
     */
    SUMOReal getStoppingPosition(const SUMOVehicle* veh) const;

    /** @brief Returns the number of transportables waiting on this stop
    */
    int getTransportableNumber() const {
        return (int)myWaitingTransportables.size();
    }

    /// @brief adds a transportable to this stop
    void addTransportable(MSTransportable* p);

    /// @brief Removes a transportable from this stop
    void removeTransportable(MSTransportable* p);

    /// @brief adds an access point to this stop
    virtual void addAccess(MSLane* lane, const SUMOReal pos) {
        myAccessPos.insert(std::make_pair(lane, pos));
    }

    /// @brief checks whether this stop provides access to the given edge
    bool hasAccess(const MSEdge* edge) const;

protected:
    /** @brief Computes the last free position on this stop
     *
     * The last free position is the one, the last vehicle ends at.
     * It is stored in myLastFreePos. If no vehicle halts, the last free
     *  position gets the value of myEndPos.
     */
    void computeLastFreePos();


protected:
    /// @brief The list of lines that are assigned to this stop
    std::vector<std::string> myLines;

    /// @brief A map from objects (vehicles) to the areas they acquire after entering the stop
    std::map<const SUMOVehicle*, std::pair<SUMOReal, SUMOReal> > myEndPositions;

    /// @brief The lane this bus stop is located at
    const MSLane& myLane;

    /// @brief The begin position this bus stop is located at
    const SUMOReal myBegPos;

    /// @brief The end position this bus stop is located at
    const SUMOReal myEndPos;

    /// @brief The last free position at this stop (variable)
    SUMOReal myLastFreePos;

    /// @brief The next free position for persons / containers
    SUMOReal myWaitingPos;

    /// @brief Persons waiting at this stop
    std::vector<MSTransportable*> myWaitingTransportables;

    /// @brief lanes and positions connected to this stop
    std::multimap<MSLane*, SUMOReal> myAccessPos;

private:
    /// @brief Invalidated copy constructor.
    MSStoppingPlace(const MSStoppingPlace&);

    /// @brief Invalidated assignment operator.
    MSStoppingPlace& operator=(const MSStoppingPlace&);


};


#endif

/****************************************************************************/

