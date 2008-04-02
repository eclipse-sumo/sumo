/****************************************************************************/
/// @file    MSEmitControl.h
/// @author  Christian Roessel
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// Inserts vehicles into the network when their departure time is reached
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
 * A vehicle emitter; Holds a list of vehicles which may be extended by new
 * vehicles read by MSRouteLoaders. Tries to emit vehicles departing at a time
 * into the network when this time is reached and restores them when the emission
 * fails.
 * Otherwise, the control is given to the lanes.
 */
class MSEmitControl
{
public:
    /** Use this constructor only. It will sort the vehicles by their
        departure time. */
    MSEmitControl(MSVehicleControl &vc, SUMOTime maxDepartDelay);

    /// Destructor.
    ~MSEmitControl();

    /** @brief Emits vehicles at time, if which want to depart at this.
        If emission is not possible, the vehicles remain in the list.
    Returns the number of emitted vehicles */
    size_t emitVehicles(SUMOTime time);

    /** @brief Adds a single vehicle for departure */
    void add(MSVehicle *veh);

    /// adds a list of vehicles to the container
    void moveFrom(MSVehicleContainer &cont);


    /** @brief Returns the number of waiting vehicles
     * 
     * The sizes of refused emits (sum of vehicles in myRefusedEmits1 and
     *  myRefusedEmits2) is returned.
     * @return The number of vehicles that could not (yet) be inserted into the net
     */
    unsigned int getWaitingVehicleNo() const throw();


private:
    /** @brief Tries to emit the vehicle
        If the emission fails, the vehicle is inserted into the given
        container.
    Returns the number of emitted vehicles */
    size_t tryEmit(SUMOTime time, MSVehicle *veh,
                   MSVehicleContainer::VehicleVector &refusedEmits);

    /** Moves all vehicles which should have been emitted previously to the given time
        into the container of previously refused vehicles */
    void checkPrevious(SUMOTime time);

private:
    MSVehicleControl &myVehicleControl;

    /** @brief The entirety of loaded vehicles that will drive through the net.
        The vehicles know their departure-time and route. The container
        is sorted by the vehicles departure time. */
    MSVehicleContainer myAllVeh;

    /** Buffer#1 for vehicles that were not allowed to enter their lane. */
    MSVehicleContainer::VehicleVector myRefusedEmits1;

    /** Buffer#1 for vehicles that were not allowed to enter their lane. */
    MSVehicleContainer::VehicleVector myRefusedEmits2;

    MSVehicleContainer::VehicleVector myNewPeriodicalAdds;

    SUMOTime myMaxDepartDelay;

private:
    /// Copy constructor.
    MSEmitControl(const MSEmitControl&);

    /// Assignment operator.
    MSEmitControl& operator=(const MSEmitControl&);


};


#endif

/****************************************************************************/

