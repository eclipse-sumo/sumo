/****************************************************************************/
/// @file    SUMOVehicleClass.h
/// @author  Daniel Krajzewicz
/// @date    2006-01-24
/// @version $Id$
///
// missing_desc
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
#ifndef SUMOVehicleClass_h
#define SUMOVehicleClass_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>


// ===========================================================================
// enum definitions
// ===========================================================================
/**
 * Bits:
 * @arg 0-7:  vehicle ownership
 * @arg 8-23: vehicle size
 *
 * From NavTeq:
 * @arg [0] All
 * @arg [1] Passenger cars
 * @arg [2] High Occupancy Vehicle
 * @arg [3] Emergency Vehicle
 * @arg [4] Taxi
 * @arg [5] Public Bus
 * @arg [6] Delivery Truck
 * @arg [7] Transport Truck
 * @arg [8] Bicycle
 * @arg [9] Pedestrian
 */
enum SUMOVehicleClass {
    SVC_UNKNOWN = 0,

    /// @name vehicle ownership
    //@{

    /// private vehicles
    SVC_PRIVATE = 1,
    /// public transport vehicles
    SVC_PUBLIC_TRANSPORT = 2,
    /// public emergency vehicles
    SVC_PUBLIC_EMERGENCY = 4,
    /// authorities vehicles
    SVC_PUBLIC_AUTHORITY = 8,
    /// army vehicles
    SVC_PUBLIC_ARMY = 16,
    /// army vehicles
    SVC_VIP = 32,
    //@}


    /// @name vehicle size
    //@{

    /// vehicle is a passenger car
    SVC_PASSENGER = 256,
    /// vehicle is a HOV
    SVC_HOV = 512,
    /// vehicle is a taxi
    SVC_TAXI = 1024,
    /// vehicle is a bus
    SVC_BUS = 2048,
    /// vehicle is a small delivery vehicle
    SVC_DELIVERY = 4096,
    /// vehicle is a large transport vehicle
    SVC_TRANSPORT = 8192,
    /// vehicle is a light rail
    SVC_LIGHTRAIL = 16384,
    /// vehicle is a city rail
    SVC_CITYRAIL = 32768,
    /// vehicle is a slow moving transport rail
    SVC_RAIL_SLOW = 65536,
    /// vehicle is a fast moving rail
    SVC_RAIL_FAST = 131072,

    /// vehicle is a motorcycle
    SVC_MOTORCYCLE = 262144,
    /// vehicle is a bicycle
    SVC_BICYCLE = 524288,
    /// is a pedestrian
    SVC_PEDESTRIAN = 1048576
    //@}

};


// ===========================================================================
// method declarations
// ===========================================================================
extern std::string getVehicleClassName(SUMOVehicleClass id) throw();
extern SUMOVehicleClass getVehicleClassID(const std::string &name) throw();


// ===========================================================================
// default vehicle type parameter
// ===========================================================================
const SUMOReal DEFAULT_VEH_MAXSPEED = (SUMOReal) 70.;
const SUMOReal DEFAULT_VEH_A = (SUMOReal) 2.6;
const SUMOReal DEFAULT_VEH_B = (SUMOReal) 4.5;
const SUMOReal DEFAULT_VEH_SIGMA = (SUMOReal) 0.5;
const SUMOReal DEFAULT_VEH_LENGTH = (SUMOReal) 5.;
const SUMOReal DEFAULT_VEH_TAU = (SUMOReal) 1.;
const SUMOVehicleClass DEFAULT_VEH_CLASS = SVC_UNKNOWN;
const SUMOReal DEFAULT_VEH_PROB = (SUMOReal) 1.;
const SUMOReal DEFAULT_VEH_SPEEDFACTOR = (SUMOReal) 1.;
const SUMOReal DEFAULT_VEH_SPEEDDEV = (SUMOReal) 0.;


#endif

/****************************************************************************/

