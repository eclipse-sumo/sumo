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


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicle;
class MSEdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSVehicleTransfer
 * This object (each simulation owns exactly one) is responsible for the
 *  transfer of vehicles that got stocked within the network due to grid locks.
 *
 * The method addVeh is called by a lane if a vehicle stood to long at this
 *  lane's end. After being added to this transfer object and removed from the
 *  lane, it is moved over the consecutive edges. On each edge, it is tried to 
 *  insert the vehicle again. The lanes are of course chosen by examining the 
 *  vehicle's real route.
 *
 * This object is used as a singleton
 */
class MSVehicleTransfer {
public:
    /// @brief Destructor
    virtual ~MSVehicleTransfer() throw();


    /** @brief Adds a vehicle to this transfer object
     * 
     * The vehicle is removed from the network as it would end the trip.
     * If the vehicle's next edge is his last one, the vehicle is also
     *  removed from the vehicle control.
     *
     * @param[in] veh The vehicle to add
     */
    void addVeh(MSVehicle *veh) throw();


    /** @brief Checks "movement" of stored vehicles
     *
     * Checks whether one of the stored vehicles may be inserted back into 
     *  the network. If not, the vehicle may ove virtually to the next lane 
     *  of it's route
     *
     * @param[in] time The current simulation time
     */
    void checkEmissions(SUMOTime time) throw();


    /** @brief Returns the instance of this object
     * @return The singleton instance
     */
    static MSVehicleTransfer *getInstance() throw();


private:
    /// @brief Constructor
    MSVehicleTransfer() throw();


    /** @brief Moves the vehicle one edge further returning whether the destination is reached
     *
     * The vehicle is not inserted into the next edge, only internal variables
     *  are adapted.
     *
     * @param[in, out] veh The vehicle to move
     * @param[in] newEdge The next to move the vehicle to (virually)
     * @return Whether the vehicle has reached his destination
     * @todo The edges are compared by pointers only; this will not work if the seen edge occurs twice in the vehicle's route
     */
    bool proceedVirtualReturnWhetherEnded(MSVehicle &veh, const MSEdge *const newEdge) throw();


protected:
    /**
     * @struct VehicleInformation
     * @brief Holds the information needed to move the vehicle over the network
     */
    struct VehicleInformation {
        /// @brief The vehicle itself
        MSVehicle *myVeh;
        /// @brief The time the vehicle was inserted at
        SUMOTime myInsertTime;
        /// @brief The time the vehicle should be moved virtually one lane further
        SUMOTime myProceedTime;

        /** @brief Constructor
         * @param[in] veh The teleported vehicle
         * @param[in] insertTime The time the vehicle was inserted at
         */
        VehicleInformation(MSVehicle *veh, SUMOTime insertTime) throw()
                : myVeh(veh), myInsertTime(insertTime), myProceedTime(insertTime) { }

    };


    /// @brief Definition of a container for vehicle information
    typedef std::vector<VehicleInformation> VehicleInfVector;

    /// T@brief he information about stored vehicles to move virtually
    VehicleInfVector myVehicles;

    /// @brief A counter for vehicles that had to be moved virtually
    unsigned int myNoTransfered;

    /// @brief The static singleton-instance
    static MSVehicleTransfer *myInstance;

};


#endif

/****************************************************************************/

