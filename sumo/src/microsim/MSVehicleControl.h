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
 * This class is responsible for vehicle building and deletion. It was
 *  reinvented due to the handling of GUIVehicles and the different deletion
 *  modalities within the pure microsim and the gui version.
 * Use this class for the pure microsim and GUIVehicleControl within the gui.
 */
class MSVehicleControl
{
public:
    /// Constructor
    MSVehicleControl();

    /// Destructor
    virtual ~MSVehicleControl();

    /// Builds a vehicle
    virtual MSVehicle *buildVehicle(std::string id, MSRoute* route,
                                    SUMOTime departTime, const MSVehicleType* type,
                                    int repNo, int repOffset);

    /// Removes the vehicle
    void scheduleVehicleRemoval(MSVehicle *v);

    /// Informs this instance about the existance of a new, not yet build vehicle
    void newUnbuildVehicleLoaded();

    /// Informs this instance the new vehicle was build
    void newUnbuildVehicleBuild();

    /// Returns the number of build vehicles
    unsigned int getLoadedVehicleNo() const;

    /// Returns the number of removed vehicles
    unsigned int getEndedVehicleNo() const;

    /// Returns the number of build and emitted, but not yet deleted vehicles
    unsigned int getRunningVehicleNo() const;

    /// Returns the number of emitted vehicles
    unsigned int getEmittedVehicleNo() const;

    /// Return the meaning waiting time of vehicles (corn-dependent value)
    SUMOReal getMeanWaitingTime() const;

    /// Return the meaning waiting time of vehicles (corn-dependent value)
    SUMOReal getMeanTravelTime() const;

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
    /// The number of build vehicles
    unsigned int myLoadedVehNo;

    /// The number of vehicles within the network (build and emitted but not removed)
    unsigned int myRunningVehNo;

    /// The number of removed vehicles
    unsigned int myEndedVehNo;

    /// The aggregated time vehicles had to wait for departure
    long myAbsVehWaitingTime;

    /// The aggregated time vehicles needed to aacomplish their route
    long myAbsVehTravelTime;

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

