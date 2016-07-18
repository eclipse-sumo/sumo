/****************************************************************************/
/// @file    MSContainerTerminal.h
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @date    Mon, 13.12.2005
/// @version $Id$
///
// A collection of stops for container where containers can be transhiped
// from one stop to another within the terminal.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2005-2016 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSContainerTerminal_h
#define MSContainerTerminal_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

//#include <vector>
////#include <algorithm>
//#include <map>
//#include <string>
#include <set>
#include <utils/common/Named.h>


// ===========================================================================
// class declarations
// ===========================================================================
//class MSLane;
//class SUMOVehicle;
class MSContainerStop;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSContainerTerminal
 * @brief A collection of stops for containers where containers can be
 *        transhiped from one stop to another within the terminal.
 *
 * A container terminal consists of several container stops. See MSContainerStop.
 *
 *  Once a container arrives at the terminal (either by insertion or by arriving
 *  via a vehicle at a certain stop of the terminal), the container teleports
 *  immediately to the stop it has to depart from.
 *
 */
class MSContainerTerminal : public Named {
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the container terminal
     * @param[in] myStops The set of stops that belong to the container terminal
     */
    MSContainerTerminal(const std::string& id,
                        const std::set<MSContainerStop*> stops);


    /// @brief Destructor
    virtual ~MSContainerTerminal();
//
//
//    /** @brief Returns the lane this bus stop is located at
//     *
//     * @return Reference to the lane the bus stop is located at
//     */
//    const MSLane& getLane() const;
//
//
//    /** @brief Returns the begin position of this bus stop
//     *
//     * @return The position the bus stop begins at
//     */
//    SUMOReal getBeginLanePosition() const;
//
//
//    /** @brief Returns the end position of this bus stop
//     *
//     * @return The position the bus stop ends at
//     */
//    SUMOReal getEndLanePosition() const;
//
//
//    /** @brief Called if a vehicle enters this stop
//     *
//     * Stores the position of the entering vehicle in myEndPositions.
//     *
//     * Recomputes the free space using "computeLastFreePos" then.
//     *
//     * @param[in] what The vehicle that enters the bus stop
//     * @param[in] beg The begin halting position of the vehicle
//     * @param[in] what The end halting position of the vehicle
//     * @see computeLastFreePos
//     */
//    void enter(SUMOVehicle* what, SUMOReal beg, SUMOReal end);
//
//
//    /** @brief Called if a vehicle leaves this stop
//     *
//     * Removes the position of the vehicle from myEndPositions.
//     *
//     * Recomputes the free space using "computeLastFreePos" then.
//     *
//     * @param[in] what The vehicle that leaves the bus stop
//     * @see computeLastFreePos
//     */
//    void leaveFrom(SUMOVehicle* what);
//
//
//    /** @brief Returns the last free position on this stop
//     *
//     * @return The last free position of this bus stop
//     */
//    SUMOReal getLastFreePos(const SUMOVehicle& forVehicle) const;
//
//
//    /** @brief Returns the number of persons waiting on this stop
//    */
//    int getPersonNumber() const {
//        return static_cast<int>(myWaitingPersons.size());
//    }
//
//    void addPerson(MSPerson* p) {
//        myWaitingPersons.push_back(p);
//    }
//
//    void removePerson(MSPerson* p) {
//        std::vector<MSPerson*>::iterator i = std::find(myWaitingPersons.begin(), myWaitingPersons.end(), p);
//        if (i != myWaitingPersons.end()) {
//            myWaitingPersons.erase(i);
//        }
//    }
//
//protected:
//    /** @brief Computes the last free position on this stop
//     *
//     * The last free position is the one, the last vehicle ends at.
//     * It is stored in myLastFreePos. If no vehicle halts, the last free
//     *  position gets the value of myEndPos.
//     */
//    void computeLastFreePos();
//
//
protected:
    /// @brief The set of stops of this terminal
    std::set<MSContainerStop*> myStops;

private:
    /// @brief Invalidated copy constructor.
    MSContainerTerminal(const MSContainerTerminal&);

    /// @brief Invalidated assignment operator.
    MSContainerTerminal& operator=(const MSContainerTerminal&);

};


#endif

/****************************************************************************/

