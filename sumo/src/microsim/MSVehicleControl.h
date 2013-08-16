/****************************************************************************/
/// @file    MSVehicleControl.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Wed, 10. Dec 2003
/// @version $Id$
///
// The class responsible for building and deletion of vehicles
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSVehicleControl_h
#define MSVehicleControl_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <math.h>
#include <string>
#include <map>
#include <set>
#include "MSGlobals.h"
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/RandomDistributor.h>
#include <utils/common/SUMOVehicleParameter.h>


// ===========================================================================
// class declarations
// ===========================================================================
class SUMOVehicle;
class MSVehicle;
class MSRoute;
class MSVehicleType;
class BinaryInputDevice;
class MSEdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSVehicleControl
 * @brief The class responsible for building and deletion of vehicles
 *
 * This class is responsible for vehicle building and deletion. It stores
 *  vehicle types, vehicles and statistics about the last.
 *
 * This class also realizes the tripinfos and the vehroutes - outputs, both
 *  generated when a vehicle is removed from the simulation, see
 *  scheduleVehicleRemoval.
 *
 * Use this class for the pure microsim and GUIVehicleControl within the gui.
 *
 * @see GUIVehicleControl
 */
class MSVehicleControl {
public:
    /// @brief Definition of the internal vehicles map iterator
    typedef std::map<std::string, SUMOVehicle*>::const_iterator constVehIt;

public:
    /// @brief Constructor
    MSVehicleControl();


    /// @brief Destructor
    virtual ~MSVehicleControl();


    /// @name Vehicle creation
    /// @{

    /** @brief Builds a vehicle, increases the number of built vehicles
     *
     * Builds a MSVehicle instance using the given parameter.
     *  Increases the number of loaded vehicles ("myLoadedVehNo").
     *
     * @param[in] defs The parameter defining the vehicle
     * @param[in] route The route of this vehicle
     * @param[in] type The type of this vehicle
     * @return The built vehicle (MSVehicle instance)
     */
    virtual SUMOVehicle* buildVehicle(SUMOVehicleParameter* defs, const MSRoute* route,
                                      const MSVehicleType* type);
    /// @}



    /// @name Insertion, deletion and retrieval of vehicles
    /// @{

    /** @brief Tries to insert the vehicle into the internal vehicle container
     *
     * Checks whether another vehicle with the same id exists; returns false
     *  if so. Otherwise, the vehicle is added to "myVehicleDict" and
     *  true is returned.
     *
     * The vehicle control gets responsible for vehicle deletion.
     *
     * @param[in] id The id of the vehicle
     * @param[in] v The vehicle
     * @return Whether the vehicle could be inserted (no other vehicle with the same id was inserted before)
     */
    virtual bool addVehicle(const std::string& id, SUMOVehicle* v);


    /** @brief Returns the vehicle with the given id
     *
     * If no vehicle with the given id is store din "myVehicleDict", 0
     *  is returned.
     *
     * @param[in] id The id of the vehicle to retrieve
     * @return The vehicle with the given id, 0 if no such vehicle exists
     */
    SUMOVehicle* getVehicle(const std::string& id) const;


    /** @brief Deletes the vehicle
     *
     * @param[in] v The vehicle to delete
     * @param[discard] Whether the vehicle is discard during loading (scale < 1)
     * @todo Isn't this quite insecure?
     */
    virtual void deleteVehicle(SUMOVehicle* v, bool discard = false);


    /** @brief Removes a vehicle after it has ended
     *
     * Writes output to tripinfos and vehroutes if wished; decrements
     *  the number of running vehicles and increments the number of ended
     *  vehicles. Then deletes the vehicle using "deleteVehicle".
     *
     * This method should be called for each vehicle that was inserted
     *  into the network and quits its ride.
     *
     * @param[in] veh The vehicle to remove
     */
    void scheduleVehicleRemoval(SUMOVehicle* veh);


    /** @brief Returns the begin of the internal vehicle map
     *
     * @return The begin of the internal vehicle map
     */
    constVehIt loadedVehBegin() const;


    /** @brief Returns the end of the internal vehicle map
     *
     * @return The end of the internal vehicle map
     */
    constVehIt loadedVehEnd() const;
    /// @}



    /// @name Setting vehicle statistics
    /// @{

    /** @brief Informs this control about a vehicle's departure
     *
     * If the mean waiting time shall be computed (f.e. for summary-output),
     *  the absolut waiting time is increased by the waiting time of the given
     *  vehicle.
     * @param[in] v The inserted vehicle
     */
    void vehicleDeparted(const SUMOVehicle& v);
    /// @}



    /// @name Retrieval of vehicle statistics (always accessable)
    /// @{

    /** @brief Returns the number of build vehicles
     * @return The number of loaded (build) vehicles
     */
    unsigned int getLoadedVehicleNo() const {
        return myLoadedVehNo;
    }


    /** @brief Returns the number of removed vehicles
     * @return The number of vehicles that have left the simulation
     */
    unsigned int getEndedVehicleNo() const {
        return myEndedVehNo;
    }


    /** @brief Returns the number of build and inserted, but not yet deleted vehicles
     * @return The number simulated vehicles (including those in teleporter)
     */
    unsigned int getRunningVehicleNo() const {
        return myRunningVehNo;
    }


    /** @brief Returns the number of inserted vehicles
     * @return The number of vehicles that have entered the simulation so far
     */
    unsigned int getDepartedVehicleNo() const {
        return myRunningVehNo + myEndedVehNo - myDiscarded;
    }


    /** @brief Returns the information whether the currently vehicle number shall be emitted
     * considering that only frac of all vehicles shall be emitted overall
     * if a negative fraction is given the demand scaling factor is used
     * (--scale or --incremental-dua-step / --incremental-dua-base)
     * @return True iff the vehicle number is acceptable
     */
    bool isInQuota(SUMOReal frac = -1) const;


    /** @brief Returns the number of build vehicles that have not been removed
     * or need to wait for a passenger
     * @return Number of active vehicles
     */
    int getActiveVehicleCount() const {
        return myLoadedVehNo - (myWaitingForPerson + myEndedVehNo);
    }

    /// @brief return the number of collisions
    unsigned int getCollisionCount() const {
        return myCollisions;
    }


    /// @brief return the number of teleports (including collisions)
    unsigned int getTeleportCount() const {
        return myTeleports;
    }
    /// @}


    /// @name Retrieval of vehicle statistics (availability depends on simulation settings)
    /// @{

    /** @brief Prints the mean waiting time of vehicles.
     *  The mean time vehicles had to wait for being inserted (-1 if no vehicle was inserted, yet)
     * @todo Enable this for guisim?
     */
    void printMeanWaitingTime(OutputDevice& od) const;


    /** @brief Returns the mean travel time of vehicles
     * The mean travel time of ended vehicles (-1 if no vehicle has ended, yet)
     * @todo Enable this for guisim?
     */
    void printMeanTravelTime(OutputDevice& od) const;
    /// @}




    /// @name Insertion and retrieval of vehicle types
    /// @{

    /** @brief Adds a vehicle type
     *
     * If another vehicle type (or distribution) with the same id exists, false is returned.
     *  Otherwise, the vehicle type is added to the internal vehicle type
     *  container "myVTypeDict".
     *
     * This control get responsible for deletion of the added vehicle
     *  type.
     *
     * @param[in] vehType The vehicle type to add
     * @return Whether the vehicle type could be added
     */
    bool addVType(MSVehicleType* vehType);


    /** @brief Adds a vehicle type distribution
     *
     * If another vehicle type (or distribution) with the same id exists, false is returned.
     *  Otherwise, the vehicle type distribution is added to the internal vehicle type distribution
     *  container "myVTypeDistDict".
     *
     * This control get responsible for deletion of the added vehicle
     *  type distribution.
     *
     * @param[in] id The id of the distribution to add
     * @param[in] vehTypeDistribution The vehicle type distribution to add
     * @return Whether the vehicle type could be added
     */
    bool addVTypeDistribution(const std::string& id, RandomDistributor<MSVehicleType*>* vehTypeDistribution);


    /** @brief Asks for a vehicle type distribution
     *
     * If vehicle type distribution with the id exists, true is returned, false otherwise.
     *
     * @param[in] id The id of the distribution
     * @return Whether the vehicle type distribution exists
     */
    bool hasVTypeDistribution(const std::string& id) const;


    /** @brief Returns the named vehicle type or a sample from the named distribution
     * @param[in] id The id of the vehicle type to return. If left out, the default type is returned.
     * @return The named vehicle type, or 0 if no such type exists
     */
    MSVehicleType* getVType(const std::string& id = DEFAULT_VTYPE_ID);


    /** @brief Inserts ids of all known vehicle types and vehicle type distributions to the given vector
     * @param[in] into The vector to fill with ids
     */
    void insertVTypeIDs(std::vector<std::string>& into) const;
    /// @}

    void addWaiting(const MSEdge* const edge, SUMOVehicle* vehicle);

    void removeWaiting(const MSEdge* const edge, SUMOVehicle* vehicle);

    SUMOVehicle* getWaitingVehicle(const MSEdge* const edge, const std::set<std::string>& lines);

    /** @brief increases the count of vehicles waiting for a person to allow recogniztion of person related deadlocks
     */
    void registerOneWaitingForPerson() {
        myWaitingForPerson++;
    }

    /** @brief decreases the count of vehicles waiting for a person to allow recogniztion of person related deadlocks
     */
    void unregisterOneWaitingForPerson() {
        myWaitingForPerson--;
    }

    /// @brief registers one collision-related teleport
    void registerCollision() {
        myTeleports++;
        myCollisions++;
    }

    /// @brief register one non-collision-related teleport
    void registerTeleport() {
        myTeleports++;
    }

    /// @name State I/O (mesosim only)
    /// @{

    /** @brief Sets the current state variables as loaded from the stream
     */
    void setState(int runningVehNo, int endedVehNo, SUMOReal totalDepartureDelay, SUMOReal totalTravelTime);

    /** @brief Saves the current state into the given stream
     */
    void saveState(OutputDevice& out);
    /// @}


    /** @brief removes any vehicles that are still waiting
     */
    void abortWaiting();


public:
    /// @brief A random number generator used to choose from vtype/route distributions and computing the speed factors
    static MTRand myVehicleParamsRNG;


private:
    /** @brief Checks whether the vehicle type (distribution) may be added
     *
     * This method checks also whether the default type may still be replaced
     * @param[in] id The id of the vehicle type (distribution) to add
     * @return Whether the type (distribution) may be added
     */
    bool checkVType(const std::string& id);

protected:
    /// @name Vehicle statistics (always accessable)
    /// @{

    /// @brief The number of build vehicles
    unsigned int myLoadedVehNo;

    /// @brief The number of vehicles within the network (build and inserted but not removed)
    unsigned int myRunningVehNo;

    /// @brief The number of removed vehicles
    unsigned int myEndedVehNo;

    /// @brief The number of vehicles which were discarded while loading
    unsigned int myDiscarded;

    /// @brief The number of collisions
    unsigned int myCollisions;

    /// @brief The number of teleports (including collisions)
    unsigned int myTeleports;
    /// @}


    /// @name Vehicle statistics
    /// @{

    /// @brief The aggregated time vehicles had to wait for departure (in seconds)
    SUMOReal myTotalDepartureDelay;

    /// @brief The aggregated time vehicles needed to aacomplish their route (in seconds)
    SUMOReal myTotalTravelTime;
    /// @}


    /// @name Vehicle container
    /// @{

    /// @brief Vehicle dictionary type
    typedef std::map< std::string, SUMOVehicle* > VehicleDictType;
    /// @brief Dictionary of vehicles
    VehicleDictType myVehicleDict;
    /// @}


    /// @name Vehicle type container
    /// @{

    /// @brief Vehicle type dictionary type
    typedef std::map< std::string, MSVehicleType* > VTypeDictType;
    /// @brief Dictionary of vehicle types
    VTypeDictType myVTypeDict;

    /// @brief Vehicle type distribution dictionary type
    typedef std::map< std::string, RandomDistributor<MSVehicleType*>* > VTypeDistDictType;
    /// @brief A distribution of vehicle types (probability->vehicle type)
    VTypeDistDictType myVTypeDistDict;

    /// @brief Whether no vehicle type was loaded
    bool myDefaultVTypeMayBeDeleted;

    /// the lists of waiting vehicles
    std::map<const MSEdge* const, std::vector<SUMOVehicle*> > myWaiting;

    /// the number of vehicles contained in myWaiting which can only continue by being triggered
    unsigned int myWaitingForPerson;

    /// @brief The scaling factor (especially for inc-dua)
    SUMOReal myScale;

private:
    /// @brief invalidated copy constructor
    MSVehicleControl(const MSVehicleControl& s);

    /// @brief invalidated assignment operator
    MSVehicleControl& operator=(const MSVehicleControl& s);


};


#endif

/****************************************************************************/

