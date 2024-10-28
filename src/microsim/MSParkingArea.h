/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2015-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSParkingArea.h
/// @author  Mirco Sturari
/// @author  Mirko Barthauer
/// @date    Tue, 19.01.2016
///
// A area where vehicles can park next to the road
/****************************************************************************/
#pragma once
#include <config.h>

#include <vector>
#include <algorithm>
#include <map>
#include <string>
#include <utils/geom/PositionVector.h>
#include <utils/common/Named.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include "MSStoppingPlace.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;
class SUMOVehicle;
class MSTransportable;
class MSBaseVehicle;
class Position;
class Command;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSParkingArea
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
class MSParkingArea : public MSStoppingPlace {
public:

    /** @brief Constructor
     *
     * @param[in] id The id of the stop
     * @param[in] net The net the stop belongs to
     * @param[in] lines Names of the lines that halt on this stop
     * @param[in] badges Names which grant access to this parking area
     * @param[in] lane The lane the stop is placed on
     * @param[in] begPos Begin position of the stop on the lane
     * @param[in] endPos End position of the stop on the lane
     * @param[in] capacity Capacity of the stop
     * @param[in] width Width of the default lot rectangle
     * @param[in] length Length of the default lot rectangle
     * @param[in] angle Angle of the default lot rectangle
     */
    MSParkingArea(const std::string& id,
                  const std::vector<std::string>& lines,
                  const std::vector<std::string>& badges, MSLane& lane,
                  double begPos, double endPos, int capacity,
                  double width, double length, double angle, const std::string& name,
                  bool onRoad,
                  const std::string& departPos,
                  bool lefthand);

    /// @brief Destructor
    virtual ~MSParkingArea();

    /// @brief needed for patching its capacity
    friend class NLTriggerBuilder;

    /// @brief Returns the area capacity
    int getCapacity() const;

    /// @brief whether vehicles park on the road
    bool parkOnRoad() const;

    /// @brief compute lot for this vehicle
    int getLotIndex(const SUMOVehicle* veh) const;

    /** @brief Returns the area occupancy
     *
     * @return The occupancy computed as number of vehicles in myEndPositions
     * (reduced by 1 if at least one vehicle has finished parking but is blocked
     * from entering the road)
     */
    int getOccupancy() const;

    /// @brief Returns the area occupancy
    int getOccupancyIncludingBlocked() const;

    /// @brief Returns the area occupancy at the end of the last simulation step
    int getLastStepOccupancy() const;

    /// @brief Add a badge to the accepted set
    void accept(std::string badge);

    /// @brief Add badges to the accepted set
    void accept(std::vector<std::string> badges);

    /// @brief Remove the access right for the given badge
    void refuse(std::string badge);

    /// @brief Return the parking accepts the vehicle (due to its given badges)
    bool accepts(MSBaseVehicle* veh) const;

    /** @brief Called if a vehicle enters this stop
     *
     * Stores the position of the entering vehicle in myEndPositions.
     *
     * Recomputes the free space using "computeLastFreePos" then.
     *
     * @param[in] what The vehicle that enters the parking area
     * @see computeLastFreePos
     */
    void enter(SUMOVehicle* veh);

    /** @brief Called if a vehicle leaves this stop
     *
     * Removes the position of the vehicle from myEndPositions.
     *
     * Recomputes the free space using "computeLastFreePos" then.
     *
     * @param[in] what The vehicle that leaves the parking area
     * @see computeLastFreePos
     */
    void leaveFrom(SUMOVehicle* what);

    /** @brief Called at the end of the time step
     *
     * Stores the current occupancy.
     *
     * @param[in] currentTime The current simulation time (unused)
     * @return Always 0 (the event is not rescheduled)
     */
    SUMOTime updateOccupancy(SUMOTime currentTime);

    /// @brief Returns the last free position on this stop
    double getLastFreePos(const SUMOVehicle& forVehicle, double brakePos = 0) const;

    /** @brief Returns the last free position on this stop including
     * reservations from the current lane and time step
     *
     * @return The last free position of this bus stop
     */
    double getLastFreePosWithReservation(SUMOTime t, const SUMOVehicle& forVehicle, double brakePos);

    /// @brief Returns the position of parked vehicle
    Position getVehiclePosition(const SUMOVehicle& forVehicle) const;

    /// @brief Returns the insertion position of a parked vehicle
    double getInsertionPosition(const SUMOVehicle& forVehicle) const;

    /// @brief Returns the angle of parked vehicle
    double getVehicleAngle(const SUMOVehicle& forVehicle) const;

    /// @brief Returns the slope of parked vehicle
    double getVehicleSlope(const SUMOVehicle& forVehicle) const;

    /** @brief Return the angle of myLastFreeLot - the next parking lot
     *         only expected to be called after we have established there is space in the parking area
     *
     * @return The angle of the lot in degrees
     */
    int getLastFreeLotAngle() const;

    /** @brief Return the GUI angle of myLastFreeLot - the angle the GUI uses to rotate into the next parking lot
     *         as above, only expected to be called after we have established there is space in the parking area
     *
     * @return The GUI angle, relative to the lane, in radians
     */
    double getLastFreeLotGUIAngle() const;

    /// @brief Return the manoeuver angle of the lot where the vehicle is parked
    int getManoeuverAngle(const SUMOVehicle& forVehicle) const;

    /// @brief  Return the GUI angle of the lot where the vehicle is parked
    double getGUIAngle(const SUMOVehicle& forVehicle) const;

    /** @brief Add a lot entry to parking area
     *
     * @param[in] x X position of the lot center
     * @param[in] y Y position of the lot center
     * @param[in] z Z position of the lot center
     * @param[in] width Width of the lot rectangle
     * @param[in] length Length of the lot rectangle
     * @param[in] angle Angle of the lot rectangle
     * @param[in] slope Slope of the lot rectangle
     * @return Whether the lot entry could be added
     */
    virtual void addLotEntry(double x, double y, double z,
                             double width, double length,
                             double angle, double slope);

    /// @brief Returns the lot rectangle width
    double getWidth() const;

    /// @brief Returns the lot rectangle length
    double getLength() const;

    /// @brief Returns the lot rectangle angle
    double getAngle() const;

    /// @brief update state so that vehicles wishing to enter cooperate with exiting vehicles
    void notifyEgressBlocked();

    /// @brief get number alternatives
    int getNumAlternatives() const;

    /// @brief set number alternatives
    void setNumAlternatives(int alternatives);

    /// @brief get the accepted badges
    std::vector<std::string> getAcceptedBadges() const;

    /// @brief set the accepted badges
    void setAcceptedBadges(const std::vector<std::string>& badges);


protected:
    /// @brief overwrite the capacity (caution: will delete ANY previous parking space definitions)
    void setRoadsideCapacity(int capactity);

protected:
    /** @struct LotSpaceDefinition
     * @brief Representation of a single lot space
     */
    struct LotSpaceDefinition {
        /// @brief default constructor
        LotSpaceDefinition();

        /// @brief parameter constructor
        LotSpaceDefinition(int index, SUMOVehicle* vehicle, double x, double y, double z, double rotation, double slope, double width, double length);

        /// @brief the running index
        const int index;

        /// @brief The last parked vehicle or 0
        const SUMOVehicle* vehicle;

        /// @brief The position of the vehicle when parking in this space
        const Position position;

        /// @brief The rotation
        const double rotation;

        /// @brief The slope
        const double slope;

        /// @brief The width
        const double width;

        /// @brief The length
        const double length;

        /// @brief The position along the lane that the vehicle needs to reach for entering this lot
        double endPos;

        ///@brief The angle between lane and lot through which a vehicle must manoeuver to enter the lot
        double manoeuverAngle;

        ///@brief Whether the lot is on the LHS of the lane relative to the lane direction
        bool sideIsLHS;
    };

    /** @brief Computes the last free position on this stop
     *
     * The last free position is the one, the last vehicle ends at.
     * It is stored in myLastFreePos. If no vehicle halts, the last free
     *  position gets the value of myEndPos.
     */
    void computeLastFreePos();

    /// @brief Last free lot number (-1 no free lot)
    int myLastFreeLot;

    /// @brief Stop area capacity configured via roadsideCapacity
    int myRoadSideCapacity;

    /// @brief Stop area total capacity
    int myCapacity;

    /// @brief Whether vehicles stay on the road
    bool myOnRoad;

    /// @brief The default width of each parking space
    double myWidth;

    /// @brief The default length of each parking space
    double myLength;

    /// @brief The default angle of each parking space
    double myAngle;

    /// @brief All the spaces in this parking area
    std::vector<LotSpaceDefinition> mySpaceOccupancies;

    /// @brief The roadside shape of this parkingArea
    PositionVector myShape;

    /// @brief The parking badges to grant access
    std::set<std::string> myAcceptedBadges;

    /// @brief whether a vehicle wants to exit but is blocked
    bool myEgressBlocked;

    /// @brief track parking reservations from the lane for the current time step
    SUMOTime myReservationTime;

    /// @brief number of reservations
    int myReservations;

    /// @brief reservation max length
    double myReservationMaxLength;

    /// @brief the number of alternative parkingAreas that are assigned to parkingAreaRerouter
    int myNumAlternatives;

    /// @brief Changes to the occupancy in the current time step
    int myLastStepOccupancy;

    /// @brief custom departPos
    double myDepartPos;
    DepartPosDefinition myDepartPosDefinition;

    /// @brief Event for updating the occupancy
    Command* myUpdateEvent;

private:
    /// @brief Invalidated copy constructor.
    MSParkingArea(const MSParkingArea&) = delete;

    /// @brief Invalidated assignment operator.
    MSParkingArea& operator=(const MSParkingArea&) = delete;
};
