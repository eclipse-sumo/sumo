/****************************************************************************/
/// @file    MSVehicleControl.h
/// @author  Daniel Krajzewicz
/// @date    Wed, 10. Dec 2003
/// @version $Id$
///
// The class responsible for building and deletion of vehicles
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
#include <utils/common/SUMOTime.h>
#include <utils/common/RandomDistributor.h>
#include <utils/common/SUMOVehicleParameter.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicle;
class MSRoute;
class MSVehicleType;
class BinaryInputDevice;


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
class MSVehicleControl
{
public:
    /// @brief Definition of the internal vehicles map iterator
    typedef std::map<std::string, MSVehicle*>::const_iterator constVehIt;

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
    virtual MSVehicle *buildVehicle(SUMOVehicleParameter* defs, const MSRoute* route,
                                    const MSVehicleType* type) throw();
    /// @}



    /// @name Insertion, deletion and retrieal of vehicles 
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
    virtual bool addVehicle(const std::string &id, MSVehicle *v) throw();


    /** @brief Returns the vehicle with the given id
     *
     * If no vehicle with the given id is store din "myVehicleDict", 0 
     *  is returned.
     *
     * @param[in] id The id of the vehicle to retrieve
     * @return The vehicle with the given id, 0 if no such vehicle exists
     */
    virtual MSVehicle *getVehicle(const std::string &id) throw();


    /** @brief Deletes the vehicle
     *
     * @param[in] v The vehicle to delete
     * @todo Isn't this quite insecure?
     */
    virtual void deleteVehicle(MSVehicle *v) throw();


    /** @brief Removes a vehicle after it has ended
     *
     * Writes output to tripinfos and vehroutes if wished; decrements
     *  the number of running vehicles and increments the number of ended 
     *  vehicles. Then deletes the vehicle using "deleteVehicle".
     *
     * This method should be called for each vehicle that was inserted 
     *  into the network and quits its ride.
     *
     * @param[in] v The vehicle to remove
     */
    void scheduleVehicleRemoval(MSVehicle *v) throw();

    
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

    /** @brief Informs this instance about the existance of a new, not yet build vehicle
     */
    inline void newUnbuildVehicleLoaded() throw() {
        ++myLoadedVehNo;
    }


    /** @brief Informs this instance the new vehicle was build
     */
    inline void newUnbuildVehicleBuild() throw() {
        --myLoadedVehNo;
    }


    /** @brief Informs this control about a vehicle's emission
     * 
     * If the mean waiting time shall be computed (f.e. for emissions-output),
     *  the absolut waiting time is increased by the waiting time of the given
     *  vehicle.
     * @param[in] v The emitted vehicle
     * @todo Consolidate with vehiclesEmitted
     */
    virtual void vehicleEmitted(const MSVehicle &v) throw();
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
        return myLoadedVehNo==myEndedVehNo;
    }
    /// @}



    /// @name Retrieval of vehicle statistics (availability depends on simulation settings)
    /// @{

    /** @brief Returns the mean waiting time of vehicles (corn-dependent value)
     *
     * This value is only available if MSCORN::CORN_MEAN_VEH_WAITINGTIME is "wished".
     *  This is the case if the emissions-output shall be generated.
     * 
     * @return The mean time vehicles had to wait for being emitted (-1 if no vehicle was emitted, yet)
     * @see MSCORN
     * @todo Enable this for guisim?
     */
    SUMOReal getMeanWaitingTime() const throw();


    /** @brief Returns the mean travel time of vehicles (corn-dependent value)
     *
     * This value is only available if MSCORN::CORN_MEAN_VEH_TRAVELTIME is "wished".
     *  This is the case if the emissions-output shall be generated.
     * 
     * @return The mean travel time of ended vehicles (-1 if no vehicle has ended, yet)
     * @see MSCORN
     * @todo Enable this for guisim?
     */
    SUMOReal getMeanTravelTime() const throw();
    /// @}




    /// @name Insertion and retrieal of vehcile types
    /// @{

    /** @brief Returns one of the active vehicle types
     * @return A random vehicle type (from active)
     */
    MSVehicleType *getRandomVType() const throw();


    /** @brief Adds a vehicle type with his probability to be chosen
     *
     * If another vehicle ype with the same id exists, false is returned.
     *  Otherwise, the vehicle type is added to the internal vehicle type
     *  container "myVTypeDict" and to the vehicle type distribution 
     *  "myVehicleTypeDistribution". 
     *
     * If no other type was loaded before, the default vehicle type is 
     *  descheduled (but not deleted as there may be already vehicles
     *  in the simulation that use it).
     *
     * This control get responsible for deletion of the added vehicle
     *  type.
     *
     * @param[in] vehType The vehicle type to add
     * @param[in] prob The probability to use the vehicle type
     * @return Whether the vehicle type could be added
     */
    bool addVType(MSVehicleType* vehType, SUMOReal prob) throw();


    /** @brief Returns the named vehicle type
     * @param[in] id The id of the vehicle type to return
     * @return The named vehicle type, or 0 if no such type exists
     * @todo Recheck whether a descheduled default vehicle type may be returned, too
     */
    MSVehicleType *getVType(const std::string &id) throw();
    /// @}



#ifdef HAVE_MESOSIM
    /// @name State I/O (mesosim only)
    /// @{

    /** @brief Loads the state of this control from the given stream
     * @todo Does not work for microsim
     */
    void saveState(std::ostream &os) throw();

    /** @brief Saves the current state into the given stream
     * @todo Does not work for microsim
     */
    void loadState(BinaryInputDevice &bis) throw();
    /// @}
#endif


private:
    /** @brief Deletes the named vehicle
     *
     * Removed the vehicle from the internal dictionary
     * @param[in] id The id of the vehicle to delete
     */
    void deleteVehicle(const std::string &id) throw();


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


    /// @name Vehicle statistics (availability depends on simulation settings)
    /// @{

    /// @brief The aggregated time vehicles had to wait for departure
    long myAbsVehWaitingTime;

    /// @brief The aggregated time vehicles needed to aacomplish their route
    long myAbsVehTravelTime;
    /// @}


    /// @name Vehicle container
    /// @{

    /// @brief Vehicle dictionary type
    typedef std::map< std::string, MSVehicle* > VehicleDictType;
    /// @brief Dictionary of vehicles
    VehicleDictType myVehicleDict;
    /// @}


    /// @name Vehicle type container
    /// @{

    /// @brief Vehicle type dictionary type
    typedef std::map< std::string, MSVehicleType* > VehTypeDictType;
    /// @brief Dictionary of vehicle types
    VehTypeDictType myVTypeDict;

    /// @brief A distribution of vehicle types (probability->vehicle type)
    RandomDistributor<MSVehicleType*> myVehicleTypeDistribution;

    /// @brief Whether no vehicle type was loaded
    bool myHaveDefaultVTypeOnly;

    /// @brief Vehicle types that may no longer be assigned by a probability
    std::vector<MSVehicleType*> myObsoleteVehicleTypes;
    

private:
    /// @brief invalidated copy constructor
    MSVehicleControl(const MSVehicleControl &s);

    /// @brief invalidated assignment operator
    MSVehicleControl &operator=(const MSVehicleControl &s);


};


#endif

/****************************************************************************/

