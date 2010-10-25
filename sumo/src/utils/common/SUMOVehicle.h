/****************************************************************************/
/// @file    SUMOVehicle.h
/// @author  Michael Behrisch
/// @date    Tue, 17 Feb 2009
/// @version $Id$
///
// Abstract base class for vehicle representations
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef SUMOVehicle_h
#define SUMOVehicle_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicleType;
class MSRoute;
class MSEdge;
class MSLane;
class MSDevice;
class MSVehicleQuitReminded;

typedef std::vector<const MSEdge*> MSEdgeVector;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SUMOVehicle
 * @brief Representation of a vehicle
 */
class SUMOVehicle {
public:
    /** @brief Get the vehicle's position along the lane
     * @return The position of the vehicle (in m from the lane's begin)
     */
    virtual const std::string& getID() const throw() = 0;

    /** @brief Get the vehicle's position along the lane
     * @return The position of the vehicle (in m from the lane's begin)
     */
    virtual SUMOReal getPositionOnLane() const throw() = 0;

    /** @brief Returns the vehicle's maximum speed
     * @return The vehicle's maximum speed
     */
    virtual SUMOReal getMaxSpeed() const = 0;

    /** @brief Returns the vehicle's current speed
     * @return The vehicle's speed
     */
    virtual SUMOReal getSpeed() const throw() = 0;

    /** @brief Returns the vehicle's type
     * @return The vehicle's type
     */
    virtual const MSVehicleType &getVehicleType() const = 0;

    /// Returns the current route
    virtual const MSRoute &getRoute() const = 0;

    virtual const std::vector<MSDevice*> &getDevices() const = 0;

    /** @brief Returns the nSuccs'th successor of edge the vehicle is currently at
     *
     * If the rest of the route (counted from the current edge) than nSuccs,
     *  0 is returned.
     * @param[in] nSuccs The number of edge to look forward
     * @return The nSuccs'th following edge in the vehicle's route
     */
    virtual const MSEdge* succEdge(unsigned int nSuccs) const throw() = 0;

    /** @brief Adapt the vehicle's maximum speed depending on the reference speed
     * @param[in] the maximum speed on the edge
     * @return The vehicle's new maximum speed
     */
    virtual SUMOReal adaptMaxSpeed(SUMOReal referenceSpeed) = 0;

    /// Replaces the current route by the given edges
    virtual bool replaceRoute(const MSEdgeVector &edges, SUMOTime simTime, bool onInit=false) = 0;

    /// Replaces the current route by the given one
    virtual bool replaceRoute(const MSRoute* route, SUMOTime simTime, bool onInit=false) = 0;

    /** @brief Update when the vehicle enters a new lane in the move step.
     *
     * @param[in] enteredLane The lane the vehicle enters
     * @param[in] driven The distance driven by the vehicle within this time step
     * @param[in] onTeleporting Whether the lane was entered while being teleported
     * @return Whether the vehicle's route has ended (due to vaporization, or because the destination was reached)
     */
    virtual bool enterLaneAtMove(MSLane* enteredLane, SUMOReal driven, bool onTeleporting=false) = 0;

    virtual SUMOReal getPreDawdleAcceleration() const throw() = 0;

    virtual MSLane &getLane() const throw() = 0;

    virtual SUMOReal getPositionOnActiveMoveReminderLane(const MSLane * const searchedLane) const throw() = 0;

    virtual SUMOTime getLastEntryTime() const throw() = 0;

    virtual SUMOReal getSegmentLength() const throw() = 0;

};


#endif

/****************************************************************************/
