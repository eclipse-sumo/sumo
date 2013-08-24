/****************************************************************************/
/// @file    MSDevice_HBEFA.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Fri, 30.01.2009
/// @version $Id$
///
// A device which collects vehicular emissions (using HBEFA-reformulation)
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
#ifndef MSDevice_HBEFA_h
#define MSDevice_HBEFA_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <set>
#include <vector>
#include <map>
#include "MSDevice.h"
#include <utils/common/SUMOTime.h>
#include <microsim/MSVehicle.h>
#include <utils/common/WrappingCommand.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDevice_HBEFA
 * @brief A device which collects vehicular emissions (using HBEFA-reformulation)
 *
 * Each device collects the vehicular emissions / fuel consumption by being
 *  called each time step, computing the current values using HelpersHBEFA, and
 *  aggregating them into internal storages over the whol journey.
 *
 * @see MSDevice
 * @see HelpersHBEFA
 */
class MSDevice_HBEFA : public MSDevice {
public:
    /** @brief Inserts MSDevice_HBEFA-options
     * @param[filled] oc The options container to add the options to
     */
    static void insertOptions(OptionsCont& oc);


    /** @brief Build devices for the given vehicle, if needed
     *
     * The options are read and evaluated whether hbefa-devices shall be built
     *  for the given vehicle.
     *
     * For each seen vehicle, the global vehicle index is increased.
     *
     * The built device is stored in the given vector.
     *
     * @param[in] v The vehicle for which a device may be built
     * @param[in, filled] into The vector to store the built device in
     */
    static void buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into);


public:
    /// @brief Destructor.
    ~MSDevice_HBEFA();



    /// @name Methods called on vehicle movement / state change, overwriting MSDevice
    /// @{

    /** @brief Computes current emission values and adds them to their sums
        *
        * The vehicle's current emission values
        *  are computed using the current velocity and acceleration.
        *
        * @param[in] veh The regarded vehicle
        * @param[in] oldPos Position before the move-micro-timestep.
        * @param[in] newPos Position after the move-micro-timestep.
        * @param[in] newSpeed The vehicle's current speed
        * @return false, if the vehicle is beyond the lane, true otherwise
        * @see MSMoveReminder
        * @see MSMoveReminder::notifyMove
        * @see HelpersHBEFA
        */
    bool notifyMove(SUMOVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed);
    /// @}



    /** @brief Called on writing tripinfo output
     *
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     * @see MSDevice::tripInfoOutput
     */
    void generateOutput() const;


private:
    /** @brief Constructor
     *
     * @param[in] holder The vehicle that holds this device
     * @param[in] id The ID of the device
     */
    MSDevice_HBEFA(SUMOVehicle& holder, const std::string& id);


private:
    /// @name Internal storages for pollutant/fuel sum in mg or ml
    /// @{

    SUMOReal myCO2, myCO, myHC, myPMx, myNOx, myFuel;
    /// @}


private:
    /// @brief Invalidated copy constructor.
    MSDevice_HBEFA(const MSDevice_HBEFA&);

    /// @brief Invalidated assignment operator.
    MSDevice_HBEFA& operator=(const MSDevice_HBEFA&);


};


#endif

/****************************************************************************/

