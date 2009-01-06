/****************************************************************************/
/// @file    MSVehicleTransfer.h
/// @author  Daniel Krajzewicz
/// @date    Sep 2003
/// @version $Id$
///
// A mover of vehicles that got stucked due to grid locks
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSVehicleTransfer_h
#define MSVehicleTransfer_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include "MSNet.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;
class MSVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSVehicleTransfer
 * This object (each simulation owns exactly one) is responsible for the
 * transfer of vehicles that got stocked within the network due to grid locks.
 * The method addVeh is called by a lane if a vehicle stood to long at this
 * lane's end. After being added to this transfer object and removed from the
 * lane, it is moved with a speed similar to the mean speed of the consecutive
 * lanes over them. On each edge, it is tried to insert the vehicle again. The
 * lanes are of course chosen by examining the vehicle's real route.
 *
 * This object is used as a singleton
 */
class MSVehicleTransfer
{
public:
    // Default constructor
    MSVehicleTransfer();

    /** @brief Adds a vehicle to this transfer object */
    void addVeh(MSVehicle *veh);

    /** @brief Checks whether one of the stored vehicles may be inserted back into the network
        Otherwise, the vehicle may ove virtually to the next lane of it's route */
    void checkEmissions(SUMOTime time);

    /// Returns the instance of this object
    static MSVehicleTransfer *getInstance();

    /// Sets the instance - necessary as a gui version exists
    static void setInstance(MSVehicleTransfer *vt);

    /// destructor
    virtual ~MSVehicleTransfer();

private:
    /**
     * @struct VehicleInformation
     * Holds the information needed to move the vehicle over the network
     */
    struct VehicleInformation {
        /// The vehicle itself
        MSVehicle *myVeh;

        /// The time the vehicle was inserted at
        SUMOTime myInsertTime;

        /// The time the vehicle should be moved virtually one lane further
        SUMOTime myProceedTime;

        /// Constructor
        VehicleInformation(MSVehicle *veh, SUMOTime insertTime)
                : myVeh(veh), myInsertTime(insertTime), myProceedTime(insertTime) { }

    };

    /// Definition of a container for vehicle information
    typedef std::vector<VehicleInformation> VehicleInfVector;

    /// The information about stored vehicles to move virtually
    VehicleInfVector myVehicles;

    /// A counter for vehicles that had to be moved virtually
    size_t myNoTransfered;

    /// The static singleton-instance
    static MSVehicleTransfer *myInstance;

};


#endif

/****************************************************************************/

