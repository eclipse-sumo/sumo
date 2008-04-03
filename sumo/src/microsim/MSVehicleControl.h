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
    /// @brief Constructor
    MSVehicleControl();


    /// @brief Destructor
    virtual ~MSVehicleControl();


    /** @brief Builds a vehicle, increases the number of built vehicles
     *
     * Builds a MSVehicle instance using the given parameter
     *
     * @param[in] id The id of the vehicle to build
     * @param[in] route The route of this vehicle
     * @param[in] departTime The departure time of this vehicle
     * @param[in] type The type of this vehicle
     * @param[in] repNo The number of repetitions
     * @param[in] repOffset The repetition offset
     * @return The built vehicle (GUIVehicle instance)
     */
    virtual MSVehicle *buildVehicle(std::string id, MSRoute* route,
                                    SUMOTime departTime, const MSVehicleType* type,
                                    int repNo, int repOffset);

    /// Removes the vehicle
    void scheduleVehicleRemoval(MSVehicle *v);

    /// Informs this instance about the existance of a new, not yet build vehicle
    void newUnbuildVehicleLoaded();

    /// Informs this instance the new vehicle was build
    void newUnbuildVehicleBuild();


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


    /// Informs this instance about the successfull emission of a vehicle
    void vehiclesEmitted(unsigned int no=1);

    /// Returns the information whether all build vehicles have been removed
    bool haveAllVehiclesQuit() const;

    /** @brief Informs this control about a vehicle's emission (corn-dependent value)
        Normally, this is done "in a batch" as the number of emitted vehicles
        is given and no explicite information about s single vehicle's emission
        is needed.
        Still, we do need this if we want to compute the mean waiting time. */
    virtual void vehicleEmitted(MSVehicle *v);

    void saveState(std::ostream &os);
    void loadState(BinaryInputDevice &bis);

    virtual bool addVehicle(const std::string &id, MSVehicle *v);
    virtual MSVehicle *getVehicle(const std::string &id);
    virtual void deleteVehicle(MSVehicle *v);
    MSVehicle *detachVehicle(const std::string &id);

    typedef std::map<std::string, MSVehicle*>::const_iterator constVehIt;

    constVehIt loadedVehBegin() const;
    constVehIt loadedVehEnd() const;

    MSVehicleType *getRandomVType() const;
    bool addVType(MSVehicleType* vehType, SUMOReal prob);
    MSVehicleType *getVType(const std::string &id);


private:
    void deleteVehicle(const std::string &id);

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


    /// Vehicle dictionary type
    typedef std::map< std::string, MSVehicle* > VehicleDictType;

    /// Dictionary of vehicles
    VehicleDictType myVehicleDict;

    typedef std::map< std::string, MSVehicleType* > VehTypeDictType;
    VehTypeDictType myVTypeDict;

    RandomDistributor<MSVehicleType*> myVehicleTypeDistribution;

    bool myHaveDefaultVTypeOnly;

    std::vector<MSVehicleType*> myObsoleteVehicleTypes;

};


#endif

/****************************************************************************/

