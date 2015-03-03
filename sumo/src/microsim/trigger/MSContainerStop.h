/****************************************************************************/
/// @file    MSContainerStop.h
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @date    Mon, 13.12.2005
/// @version $Id: MSContainerStop.h 16005 2014-03-24 12:46:02Z cschmidt87 $
///
// A lane area vehicles can halt at and load and unload containers
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2005-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSContainerStop_h
#define MSContainerStop_h


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
#include <set>
#include <utils/common/Named.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;
class SUMOVehicle;
class MSContainer;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSContainerStop
 * @brief A lane area vehicles can halt at and load and unload containers
 *
 * A container stop tracks the last free space a vehicle may halt at by being
 *  informed about a vehicle's entering and depart. It keeps the information
 *  about entered vehicles' begin and end position within an internal
 *  container ("myEndPositions") and is so able to compute the last free space.
 *
 * Please note that using the last free space disallows vehicles to enter a
 *  free space in between other vehicles.
 */
class MSContainerStop : public Named {
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the container stop
     * @param[in] net The net the container stop belongs to
     * @param[in] lines Names of the vehicle lines that halt on this container stop
     * @param[in] lane The lane the container stop is placed on
     * @param[in] begPos Begin position of the container stop on the lane
     * @param[in] endPos End position of the container stop on the lane
     */
    MSContainerStop(const std::string& id,
              const std::vector<std::string>& lines, MSLane& lane,
              SUMOReal begPos, SUMOReal endPos);


    /// @brief Destructor
    virtual ~MSContainerStop();


    /** @brief Returns the lane this container stop is located at
     *
     * @return Reference to the lane the container stop is located at
     */
    const MSLane& getLane() const;


    /** @brief Returns the begin position of this container stop
     *
     * @return The position the container stop begins at
     */
    SUMOReal getBeginLanePosition() const;


    /** @brief Returns the end position of this container stop
     *
     * @return The position the container stop ends at
     */
    SUMOReal getEndLanePosition() const;


    /** @brief Called if a vehicle enters this stop
     *
     * Stores the position of the entering vehicle in myEndPositions.
     *
     * Recomputes the free space using "computeLastFreePos" then.
     *
     * @param[in] what The vehicle that enters the container stop
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
     * @param[in] what The vehicle that leaves the container stop
     * @see computeLastFreePos
     */
    void leaveFrom(SUMOVehicle* what);


    /** @brief Returns the last free position on this stop
     *
     * @return The last free position of this container stop
     */
    SUMOReal getLastFreePos(const SUMOVehicle& forVehicle) const;


    /** @brief Returns the number of containers waiting on this stop
    */
    unsigned int getContainerNumber() const {
        return static_cast<unsigned int>(myWaitingContainers.size());
    }

    /// @brief Adds a container to this stop
    void addContainer(MSContainer* container) {
        myWaitingContainers.push_back(container);
    }

    /// @brief Removes a container from this stop
    void removeContainer(MSContainer* container) {
        std::vector<MSContainer*>::iterator i = std::find(myWaitingContainers.begin(), myWaitingContainers.end(), container);
        if (i != myWaitingContainers.end()) {
            myWaitingContainers.erase(i);
        }
    }

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
    std::map<SUMOVehicle*, std::pair<SUMOReal, SUMOReal> > myEndPositions;  
        
    /// @brief The lane this stop is located at
    MSLane& myLane;

    /// @brief The begin position this stop is located at
    SUMOReal myBegPos;

    /// @brief The end position this stop is located at
    SUMOReal myEndPos;
        
    /// @brief The last free position at this stop (variable)
    SUMOReal myLastFreePos;
        
    /// @brief Containers waiting at this stop
    std::vector<MSContainer*> myWaitingContainers;


private:
    /// @brief Invalidated copy constructor.
    MSContainerStop(const MSContainerStop&);

    /// @brief Invalidated assignment operator.
    MSContainerStop& operator=(const MSContainerStop&);


};


#endif

/****************************************************************************/

