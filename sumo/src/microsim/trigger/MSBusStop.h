/****************************************************************************/
/// @file    MSBusStop.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 13.12.2005
/// @version $Id$
///
// A lane area vehicles can halt at
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSBusStop_h
#define MSBusStop_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <map>
#include <string>
#include "MSTrigger.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSBusStop
 * @brief A lane area vehicles can halt at
 *
 * The bus stops tracks the last free space a vehicle may halt at by being 
 *  informed about a vehicle's entering and depart. It keeps the information
 *  about entered vehicles' begin and end position within an internal 
 *  container ("myEndPositions") and is so able to compute the last free space.
 *
 * Please remark that using the last free space disallows vehicles to enter a
 *  free space in between other vehicles.
 */
class MSBusStop : public MSTrigger
{
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the bus stop
     * @param[in] net The net the bus stop belongs to
     * @param[in] lines Names of the bus lines that halt on this bus stop
     * @param[in] lane The lane the bus stop is placed on
     * @param[in] begPos Begin position of the bus stop on the lane
     * @param[in] endPos End position of the bus stop on the lane
     */
    MSBusStop(const std::string &id,
              const std::vector<std::string> &lines, MSLane &lane,
              SUMOReal begPos, SUMOReal endPos) throw();


    /// @brief Destructor
    virtual ~MSBusStop() throw();


    /** @brief Returns the lane this bus stop is located at
     *
     * @return Reference to the lane the bus stop is located at
     */
    const MSLane &getLane() const throw();


    /** @brief Returns the begin position of this bus stop
     *
     * @return The position the bus stop begins at
     */
    SUMOReal getBeginLanePosition() const throw();


    /** @brief Returns the end position of this bus stop
     *
     * @return The position the bus stop ends at
     */
    SUMOReal getEndLanePosition() const throw();


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
    void enter(void *what, SUMOReal beg, SUMOReal end) throw();


    /** @brief Called if a vehicle leaves this stop
     *
     * Removes the position of the vehicle from myEndPositions.
     *
     * Recomputes the free space using "computeLastFreePos" then.
     *
     * @param[in] what The vehicle that leaves the bus stop
     * @see computeLastFreePos
     */
    void leaveFrom(void *what) throw();


    /** @brief Returns the last free position on this stop
     *
     * @return The last free position of this bus stop
     */
    SUMOReal getLastFreePos() const throw();


protected:
    /** @brief Computes the last free position on this stop
     *
     * The last free position is the one, the last vehicle ends at.
     * It is stored in myLastFreePos. If no vehicle halts, the last free
     *  position gets the value of myEndPos.
     */
    void computeLastFreePos() throw();


protected:
    /// @brief The list of lines that are assigned to this stop
    std::vector<std::string> myLines;

    /// @brief A map from objects (vehicles) to the areas they acquire after entering the stop
    std::map<void*, std::pair<SUMOReal, SUMOReal> > myEndPositions;

    /// @brief The lane this bus stop is located at
    MSLane &myLane;

    /// @brief The begin position this bus stop is located at
    SUMOReal myBegPos;

    /// @brief The end position this bus stop is located at
    SUMOReal myEndPos;

    /// @brief The last free position at this stop (variable)
    SUMOReal myLastFreePos;


};


#endif

/****************************************************************************/

