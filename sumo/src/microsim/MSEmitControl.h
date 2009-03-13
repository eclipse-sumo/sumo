/****************************************************************************/
/// @file    MSEmitControl.h
/// @author  Christian Roessel
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// Inserts vehicles into the network when their departure time is reached
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
#ifndef MSEmitControl_h
#define MSEmitControl_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSVehicleContainer.h"
#include <vector>
#include <map>
#include <string>


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicle;
class MSVehicleControl;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSEmitControl
 * @brief Inserts vehicles into the network when their departure time is reached
 *
 * A vehicle emitter; Holds a list of vehicles which may be filled by vehicles
 *  read by MSRouteLoaders. Tries to emit vehicles departing at a time into the
 *  network as soon this time is reached and keeps them as long the emission
 *  fails.
 *
 * If a vehicle is emitted, the control about it is given to the lanes.
 *
 * Vehicles are not controlled (created, deleted) by this class.
 *
 * @todo Whe a vehicle is deleted due to waiting too long or because of vaporizing, ths is not reported anywhere
 */
class MSEmitControl {
public:
    /** @brief Constructor
     *
     * @param[in] vc The assigned vehicle control (needed for vehicle reemission and deletion)
     * @param[in] maxDepartDelay Vehicles waiting longer than this for emission are deleted (-1: no deletion)
     */
    MSEmitControl(MSVehicleControl &vc, SUMOTime maxDepartDelay) throw();


    /// @brief Destructor.
    ~MSEmitControl() throw();


    /** @brief Emits vehicles that want to depart at the given time
     *
     * All vehicles scheduled for this time are tried to be emitted. This
     *  includes those with a depart time as the given time and those that
     *  wait for being emitted due they could not be inserted in previous
     *  steps.
     *
     * For each vehicle, tryEmit is called. If this fails, a vehicle
     *  keeps within the refused emit containers ("myRefusedEmits1",
     *  "myRefusedEmits2") so that it may be emitted within the next steps.
     *
     * Returns the number of vehicles that could be inserted into the net.
     *
     * @param[in] time The current simulation time
     * @return The number of vehicles that could be inserted into the net
     */
    unsigned int emitVehicles(SUMOTime time) throw();


    /** @brief Adds a single vehicle for departure
     *
     * The vehicle is added to "myAllVeh".
     *
     * @param[in] veh The vehicle to add for later emission
     */
    void add(MSVehicle *veh) throw();


    /** @brief Adds all vehicles from the given container to the internal
     *
     * All vehicles stored in "cont" are added to "myAllVeh". "cont"
     *  is cleared.
     *
     * @param[in] cont The container of vehicles to move all vehicles from
     */
    void moveFrom(MSVehicleContainer &cont) throw();


    /** @brief Returns the number of waiting vehicles
     *
     * The sizes of refused emits (sum of vehicles in "myRefusedEmits1" and
     *  "myRefusedEmits2") is returned.
     *
     * @return The number of vehicles that could not (yet) be inserted into the net
     * @todo Note that vehicles in emitters are not counted!
     */
    unsigned int getWaitingVehicleNo() const throw();


private:
    /** @brief Tries to emit the vehicle
     *
     * If the emission fails, it is examined whether the reason was a vaporizing
     *  edge. If so, the vehicle is deleted. Otherwise, it is checked whether the
     *  time the vehicle had to wait so far is larger than the maximum allowed
     *  waiting time. If so, the vehicle is deleted, too. If both does not match,
     *  the vehicle is reinserted to "refusedEmits" in order to be emitted in
     *  next steps.
     *
     * As soon as the vehicle is emitted or deleted, it is checked whether
     *  a vehicle with same parameter shall be reinserter by calling
     *  "checkReemission".
     *
     * @param[in] time The current simulation time
     * @param[in] veh The vehicle to emit
     * @param[in] refusedEmits Container to insert vehicles that could not be emitted into
     * @return The number of emitted vehicles (0 or 1)
     * @todo Reinsertion seems to be buggy - consecutive vehicles of one that wais a long time are inserted too late
     */
    unsigned int tryEmit(SUMOTime time, MSVehicle *veh,
                         MSVehicleContainer::VehicleVector &refusedEmits) throw();


    /** @brief Adds all vehicles that should have been emitted earlier to the refuse container
     *
     * @param[in] time The current simulation time
     * @todo recheck
     */
    void checkPrevious(SUMOTime time) throw();


    /** @brief Checks whether a further vehicle with the same parameter as the given shall be inserted
     *
     * If the vehicle is periodical, the next is built and inserted
     *  into the list of vehicles to emit. These vehicles are inserted
     *  into "myNewPeriodicalAdds", so that the main container his iterators
     *  are not broken.
     *
     * @param[in] veh The vehicle to check
     */
    void checkReemission(MSVehicle *veh) throw();


private:
    /// @brief The assigned vehicle control (needed for vehicle reemission and deletion)
    MSVehicleControl &myVehicleControl;

    /// @brief All loaded vehicles sorted by their departure time
    MSVehicleContainer myAllVeh;

    /// @brief Buffers for vehicles that could not been inserted
    MSVehicleContainer::VehicleVector myRefusedEmits1, myRefusedEmits2;

    /// @brief Container for newly built periodical vehicles
    MSVehicleContainer::VehicleVector myNewPeriodicalAdds;

    /// @brief The maximum waiting time; vehicles waiting longer are deleted (-1: no deletion)
    SUMOTime myMaxDepartDelay;


private:
    /// @brief Invalidated copy constructor.
    MSEmitControl(const MSEmitControl&);

    /// @brief Invalidated assignment operator.
    MSEmitControl& operator=(const MSEmitControl&);


};


#endif

/****************************************************************************/

