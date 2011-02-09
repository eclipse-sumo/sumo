/****************************************************************************/
/// @file    MSVehicleControl.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 10. Dec 2003
/// @version $Id$
///
// The class responsible for building and deletion of vehicles
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include <string>
#include <map>
#include <set>
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
    MSVehicleControl() throw();


    /// @brief Destructor
    virtual ~MSVehicleControl() throw();


    /// @name Vehicle creation
    /// @{

    /** @brief Builds a vehicle, increases the number of built vehicles
     *
     * Builds a MSVehicle instance using the given parameter.
     *  Increases the number of loaded vehicles ("myLoadedVehNo").
     *
     * @param[in] id The id of the vehicle to build
     * @param[in] route The route of this vehicle
     * @param[in] departTime The departure time of this vehicle
     * @param[in] type The type of this vehicle
     * @param[in] repNo The number of repetitions
     * @param[in] repOffset The repetition offset
     * @return The built vehicle (MSVehicle instance)
     */
    virtual SUMOVehicle *buildVehicle(SUMOVehicleParameter* defs, const MSRoute* route,
                                      const MSVehicleType* type) throw(ProcessError);
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
    virtual bool addVehicle(const std::string &id, SUMOVehicle *v) throw();


    /** @brief Returns the vehicle with the given id
     *
     * If no vehicle with the given id is store din "myVehicleDict", 0
     *  is returned.
     *
     * @param[in] id The id of the vehicle to retrieve
     * @return The vehicle with the given id, 0 if no such vehicle exists
     */
    SUMOVehicle *getVehicle(const std::string &id) const throw();


    /** @brief Deletes the vehicle
     *
     * @param[in] v The vehicle to delete
     * @todo Isn't this quite insecure?
     */
    virtual void deleteVehicle(SUMOVehicle *v) throw();


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
    void scheduleVehicleRemoval(SUMOVehicle *veh) throw();


    /** @brief Returns the begin of the internal vehicle map
     *
     * @return The begin of the internal vehicle map
     */
    constVehIt loadedVehBegin() const throw();


    /** @brief Returns the end of the internal vehicle map
     *
     * @return The end of the internal vehicle map
     */
    constVehIt loadedVehEnd() const throw();
    /// @}



    /// @name Setting vehicle statistics
    /// @{

    /** @brief Informs this control about a vehicle's emission
     *
     * If the mean waiting time shall be computed (f.e. for emissions-output),
     *  the absolut waiting time is increased by the waiting time of the given
     *  vehicle.
     * @param[in] v The emitted vehicle
     * @todo Consolidate with vehiclesEmitted
     */
    void vehicleEmitted(const SUMOVehicle &v) throw();
    /// @}



    /// @name Retrieval of vehicle statistics (always accessable)
    /// @{

    /** @brief Returns the number of build vehicles
     * @return The number of loaded (build) vehicles
     */
    unsigned int getLoadedVehicleNo() const throw() {
        return myLoadedVehNo;
    }


    /** @brief Returns the number of removed vehicles
     * @return The number of vehicles that have left the simulation
     */
    unsigned int getEndedVehicleNo() const throw() {
        return myEndedVehNo;
    }


    /** @brief Returns the number of build and emitted, but not yet deleted vehicles
     * @return The number simulated vehicles (including those in teleporter)
     */
    unsigned int getRunningVehicleNo() const throw() {
        return myRunningVehNo;
    }


    /** @brief Returns the number of emitted vehicles
     * @return The number of vehicles that have entered the simulation so far
     */
    unsigned int getEmittedVehicleNo() const throw() {
        return myRunningVehNo + myEndedVehNo;
    }


    /** @brief Returns the information whether all build vehicles have been removed
     * @return Whether all loaded vehicles have ended
     */
    bool haveAllVehiclesQuit() const throw() {
        return myLoadedVehNo == myEndedVehNo;
    }


    /** @brief Returns the information whether all build vehicles have either been removed
     * or need to wait for a passenger
     */
    bool haveAllActiveVehiclesQuit() const throw() {
        return myLoadedVehNo == (myWaitingForPerson + myEndedVehNo);
    }
    /// @}


    /// @name Retrieval of vehicle statistics (availability depends on simulation settings)
    /// @{

    /** @brief Prints the mean waiting time of vehicles.
     *  The mean time vehicles had to wait for being emitted (-1 if no vehicle was emitted, yet)
     * @todo Enable this for guisim?
     */
    void printMeanWaitingTime(OutputDevice& od) const throw();


    /** @brief Returns the mean travel time of vehicles
     * The mean travel time of ended vehicles (-1 if no vehicle has ended, yet)
     * @todo Enable this for guisim?
     */
    void printMeanTravelTime(OutputDevice& od) const throw();
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
    bool addVType(MSVehicleType* vehType) throw();


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
    bool addVTypeDistribution(const std::string &id, RandomDistributor<MSVehicleType*> *vehTypeDistribution) throw();


    /** @brief Asks for a vehicle type distribution
     *
     * If vehicle type distribution with the id exists, true is returned, false otherwise.
     *
     * @param[in] id The id of the distribution
     * @return Whether the vehicle type distribution exists
     */
    bool hasVTypeDistribution(const std::string &id) const throw();


    /** @brief Returns the named vehicle type or a sample from the named distribution
     * @param[in] id The id of the vehicle type to return. If left out, the default type is returned.
     * @return The named vehicle type, or 0 if no such type exists
     */
    MSVehicleType *getVType(const std::string &id=DEFAULT_VTYPE_ID) throw();


    /** @brief Inserts ids of all known vehicle types and vehicle type distributions to the given vector
     * @param[in] into The vector to fill with ids
     */
    void insertVTypeIDs(std::vector<std::string> &into) const throw();
    /// @}

    void addWaiting(const MSEdge* const edge, SUMOVehicle *vehicle) throw();

    void removeWaiting(const MSEdge* const edge, SUMOVehicle *vehicle) throw();

    SUMOVehicle *getWaitingVehicle(const MSEdge* const edge, const std::set<std::string> &lines) throw();

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

    /// @name State I/O (mesosim only)
    /// @{

    /** @brief Loads the state of this control from the given stream
     * @todo Does not work for microsim
     */
    virtual void saveState(std::ostream &os) throw();

    /** @brief Saves the current state into the given stream
     * @todo Does not work for microsim
     */
    virtual void loadState(BinaryInputDevice &bis, const SUMOTime offset) throw();
    /// @}
    //

    /** @brief removes any vehicles that are still waiting
     */
    void abortWaiting() throw();


private:
    /** @brief Checks whether the vehicle type (distribution) may be added
     *
     * Removed the vehicle from the internal dictionary
     * @param[in] id The id of the vehicle type (distribution) to add
     * @return Whether the type (distribution) may be added
     */
    bool checkVType(const std::string &id) throw();


protected:
    /// @name Vehicle statistics (always accessable)
    /// @{

    /// @brief The number of build vehicles
    unsigned int myLoadedVehNo;

    /// @brief The number of vehicles within the network (build and emitted but not removed)
    unsigned int myRunningVehNo;

    /// @brief The number of removed vehicles
    unsigned int myEndedVehNo;
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


private:
    /// @brief invalidated copy constructor
    MSVehicleControl(const MSVehicleControl &s);

    /// @brief invalidated assignment operator
    MSVehicleControl &operator=(const MSVehicleControl &s);


};


#endif

/****************************************************************************/

