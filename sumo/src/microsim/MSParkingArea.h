/****************************************************************************/
/// @file    MSParkingArea.h
/// @author  Mirco Sturari
/// @date    Tue, 19.01.2016
/// @version $Id$
///
// A area where vehicles can park next to the road
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2015-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSParkingArea_h
#define MSParkingArea_h


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
#include <utils/geom/PositionVector.h>
#include <utils/common/Named.h>
#include "MSStoppingPlace.h"


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
     * @param[in] lane The lane the stop is placed on
     * @param[in] begPos Begin position of the stop on the lane
     * @param[in] endPos End position of the stop on the lane
     * @param[in] capacity Capacity of the stop
     * @param[in] width Width of the default lot rectangle
     * @param[in] length Length of the default lot rectangle
     * @param[in] angle Angle of the default lot rectangle
     */
    MSParkingArea(const std::string& id,
                  const std::vector<std::string>& lines, MSLane& lane,
                  SUMOReal begPos, SUMOReal endPos, unsigned int capacity,
                  SUMOReal width, SUMOReal length, SUMOReal angle);

    /// @brief Destructor
    virtual ~MSParkingArea();


    /** @brief Returns the area capacity
     *
     * @return The capacity
     */
    int getCapacity() const;


    /** @brief Returns the area occupancy
     *
     * @return The occupancy computed as number of vehicles in myEndPositions
     */
    int getOccupancy() const;


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


    /** @brief Returns the position of parked vehicle
     *
     * @return The position of parked vehicle
     */
    Position getVehiclePosition(const SUMOVehicle& forVehicle);


    /** @brief Returns the angle of parked vehicle
     *
     * @return The angle of parked vehicle
     */
    SUMOReal getVehicleAngle(const SUMOVehicle& forVehicle);


    /** @brief Returns the space dimension
     *
     * @return The space dimension
     */
    SUMOReal getSpaceDim() const;


    /** @brief Add a lot entry to parking area
     *
     * @param[in] x X position of the lot center
     * @param[in] y Y position of the lot center
     * @param[in] z Z position of the lot center
     * @param[in] width Width of the lot rectangle
     * @param[in] length Length of the lot rectangle
     * @param[in] angle Angle of the lot rectangle
     * @return Whether the lot entry could be added
     */
    void addLotEntry(SUMOReal x, SUMOReal y, SUMOReal z,
                     SUMOReal width, SUMOReal length, SUMOReal angle);


    /** @brief Returns the lot rectangle width
     *
     * @return The width
     */
    SUMOReal getWidth() const;


    /** @brief Returns the lot rectangle length
     *
     * @return The length
     */
    SUMOReal getLength() const;


    /** @brief Returns the lot rectangle angle
     *
     * @return The angle
     */
    SUMOReal getAngle() const;

protected:

    /** @struct LotSpaceDefinition
    * @brief Representation of a single lot space
    */
    struct LotSpaceDefinition {
        /// @brief the running index
        unsigned int index;
        /// @brief The last parked vehicle or 0
        SUMOVehicle* vehicle;
        /// @brief The position of the vehicle when parking in this space
        Position myPosition;
        /// @brief The rotation
        SUMOReal myRotation;
        /// @brief The width
        SUMOReal myWidth;
        /// @brief The length
        SUMOReal myLength;
        /// @brief The position along the lane that the vehicle needs to reach for entering this lot
        SUMOReal myEndPos;
    };


    /** @brief Computes the last free position on this stop
     *
     * The last free position is the one, the last vehicle ends at.
     * It is stored in myLastFreePos. If no vehicle halts, the last free
     *  position gets the value of myEndPos.
     */
    void computeLastFreePos();

    /// @brief Last free lot number (0 no free lot)
    int myLastFreeLot;

    /// @brief Stop area capacity
    int myCapacity;

    /// @brief The default width of each parking space
    SUMOReal myWidth;

    /// @brief The default length of each parking space
    SUMOReal myLength;

    /// @brief The default angle of each parking space
    SUMOReal myAngle;


    /// @brief A map from objects (vehicles) to the areas they acquire after entering the stop
    std::map<unsigned int, LotSpaceDefinition > mySpaceOccupancies;

    /// @brief The roadside shape of this parkingArea
    PositionVector myShape;

private:

    /// @brief Invalidated copy constructor.
    MSParkingArea(const MSParkingArea&);

    /// @brief Invalidated assignment operator.
    MSParkingArea& operator=(const MSParkingArea&);

};


#endif

/****************************************************************************/
