/****************************************************************************/
/// @file    MSDevice_HBEFA.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 30.01.2009
/// @version $Id$
///
// A device which collects vehicular emissions (using HBEFA-reformulation)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
     */
    static void insertOptions() throw();


    /** @brief Build devices for the given vehicle, if needed
     *
     * The options are read and evaluated whether hbefa-devices shall be built
     *  for the given vehicle.
     *
     * When the first device is built, the static container of edge weights
     *  used for routing is initialised with the mean speed the edges allow.
     *  In addition, an event is generated which updates these weights is
     *  built and added to the list of events to execute at a simulation end.
     *
     * For each seen vehicle, the global vehicle index is increased.
     *
     * The built device is stored in the given vector.
     *
     * @param[in] v The vehicle for which a device may be built
     * @param[in, filled] into The vector to store the built device in
     */
    static void buildVehicleDevices(MSVehicle &v, std::vector<MSDevice*> &into) throw();


public:
    /// @name Methods called on vehicle movement / state change, overwriting MSDevice
    /// @{

    /** @brief Computes a new route on vehicle emission
     *
     * An event is created and added to the list of simulation step end
     *  events which executes "wrappedComputeCommandExecute".
     *
     * @param[in] enteredLane The lane the vehicle enters
     * @param[in] state The vehicle's state during the emission
     * @see MSVehicle::reroute
     * @see MSEventHandler
     * @see WrappingCommand
     */
    void enterLaneAtEmit(MSLane* enteredLane, const MSVehicle::State &state);
    /// @}


    /** @brief Called on writing tripinfo output
     *
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     * @see MSDevice::tripInfoOutput
     */
    void tripInfoOutput(OutputDevice &os) const throw(IOError);


    /** @brief Performs rerouting after a period
     *
     * The current amounts of emitted pollutants / consumed fuel is computed
     *  using the according methods from "HelpersHBEFA" and are added to the internal
     *  sums.
     *
     * This method is called from the event handler at the end of a simulation
     *  step.
     *
     * @param[in] currentTime The current simulation time
     * @return Always 1 (for being computed in the next step)
     * @see HelpersHBEFA
     * @see MSEventHandler
     * @see WrappingCommand
     */
    SUMOTime wrappedComputeCommandExecute(SUMOTime currentTime) throw(ProcessError);



private:
    /** @brief Constructor
     *
     * @param[in] holder The vehicle that holds this device
     * @param[in] id The ID of the device
     */
    MSDevice_HBEFA(MSVehicle &holder, const std::string &id) throw();


    /// @brief Destructor.
    ~MSDevice_HBEFA() throw();


private:
    /// @brief A static vehicle index for computing deterministic vehicle fractions
    static int myVehicleIndex;

    /// @brief The (optional) command responsible for rerouting
    WrappingCommand< MSDevice_HBEFA >* myComputeAndCollectCommand;

    /// @name Internal storages for pollutant/fuel sum
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

