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
 * @enum SUMOVehicleShape
 * @brief Definition of vehicle classes to differ between different appearences
 */
enum SUMOVehicleShape {
    /// @brief not defined
    SVS_UNKNOWN,
    /// @brief render as a pedestrian
    SVS_PEDESTRIAN,
    /// @brief render as a bicycle
    SVS_BICYCLE,
    /// @brief render as a motorcycle
    SVS_MOTORCYCLE,
    /// @brief render as a passenger vehicle
    SVS_PASSENGER,
    /// @brief render as a sedan passenger vehicle ("Stufenheck")
    SVS_PASSENGER_SEDAN,
    /// @brief render as a hatchback passenger vehicle ("Fliessheck")
    SVS_PASSENGER_HATCHBACK,
    /// @brief render as a wagon passenger vehicle ("Combi")
    SVS_PASSENGER_WAGON,
    /// @brief render as a van
    SVS_PASSENGER_VAN,
    /// @brief render as a delivery vehicle
    SVS_DELIVERY,
    /// @brief render as a transport vehicle
    SVS_TRANSPORT,
    /// @brief render as a semi-trailer transport vehicle ("Sattelschlepper")
    SVS_TRANSPORT_SEMITRAILER,
    /// @brief render as a transport vehicle with one trailer
    SVS_TRANSPORT_1TRAILER,
    /// @brief render as a bus
    SVS_BUS,
    /// @brief render as a city bus
    SVS_BUS_CITY,
    /// @brief render as a flexible city bus
    SVS_BUS_CITY_FLEXIBLE,
    /// @brief render as a overland bus
    SVS_BUS_OVERLAND,
    /// @brief render as a rail
    SVS_RAIL,
    /// @brief render as a light rail
    SVS_RAIL_LIGHT,
    /// @brief render as a city rail
    SVS_RAIL_CITY,
    /// @brief render as a slow (passenger) train
    SVS_RAIL_SLOW,
    /// @brief render as a fast (passenger) train
    SVS_RAIL_FAST,
    /// @brief render as a cargo train
    SVS_RAIL_CARGO,
    /// @brief render as a (futuristic) e-vehicle
    SVS_E_VEHICLE
};



/**
 * @enum SUMOVehicleClass
 * @brief Definition of vehicle classes to differ between different lane usage and authority types
 *
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

    /// @brief private vehicles
    SVC_PRIVATE = 1,
    /// @brief public transport vehicles
    SVC_PUBLIC_TRANSPORT = 2,
    /// @brief public emergency vehicles
    SVC_PUBLIC_EMERGENCY = 4,
    /// @brief authorities vehicles
    SVC_PUBLIC_AUTHORITY = 8,
    /// @brief army vehicles
    SVC_PUBLIC_ARMY = 16,
    /// @brief vip vehicles
    SVC_VIP = 32,
    //@}


    /// @name vehicle size
    //@{

    /// @brief vehicle is a passenger car
    SVC_PASSENGER = 256,
    /// @brief vehicle is a HOV
    SVC_HOV = 512,
    /// @brief vehicle is a taxi
    SVC_TAXI = 1024,
    /// @brief vehicle is a bus
    SVC_BUS = 2048,
    /// @brief vehicle is a small delivery vehicle
    SVC_DELIVERY = 4096,
    /// @brief vehicle is a large transport vehicle
    SVC_TRANSPORT = 8192,
    /// @brief vehicle is a light rail
    SVC_LIGHTRAIL = 16384,
    /// @brief vehicle is a city rail
    SVC_CITYRAIL = 32768,
    /// @brief vehicle is a slow moving transport rail
    SVC_RAIL_SLOW = 65536,
    /// @brief vehicle is a fast moving rail
    SVC_RAIL_FAST = 131072,

    /// @brief vehicle is a motorcycle
    SVC_MOTORCYCLE = 262144,
    /// @brief vehicle is a bicycle
    SVC_BICYCLE = 524288,
    /// @brief is a pedestrian
    SVC_PEDESTRIAN = 1048576
    //@}

};


/**
 * @enum SUMOEmissionClass
 * @brief Definition of vehicle emission classes
 */
enum SUMOEmissionClass {
    SVE_UNKNOWN,
    SVE_PASSENGER_EURO4__1_4__2l,
    SVE_BUS_CITY,
    SVE_BUS_OVERLAND,
    SVE_HDV_7_5t__EURO4,
    SVE_ZERO_EMISSIONS
};


// ===========================================================================
// method declarations
// ===========================================================================
extern void initGuiShapeNames() throw();
extern std::string getVehicleClassName(SUMOVehicleClass id) throw();
extern SUMOVehicleClass getVehicleClassID(const std::string &name) throw();
extern std::string getVehicleShapeName(SUMOVehicleShape id) throw();
extern SUMOVehicleShape getVehicleShapeID(const std::string &name) throw();
extern std::string getVehicleEmissionTypeName(SUMOEmissionClass id) throw();
extern SUMOEmissionClass getVehicleEmissionTypeID(const std::string &name) throw();


// ===========================================================================
// default vehicle type parameter
// ===========================================================================
const std::string DEFAULT_VTYPE_ID = "DEFAULT_VEHTYPE";
const SUMOReal DEFAULT_VEH_MAXSPEED = (SUMOReal) 70.;
const SUMOReal DEFAULT_VEH_A = (SUMOReal) 2.6;
const SUMOReal DEFAULT_VEH_B = (SUMOReal) 4.5;
const SUMOReal DEFAULT_VEH_SIGMA = (SUMOReal) 0.5;
const SUMOReal DEFAULT_VEH_LENGTH = (SUMOReal) 7.5;
const SUMOReal DEFAULT_VEH_TAU = (SUMOReal) 1.;
const SUMOVehicleClass DEFAULT_VEH_CLASS = SVC_UNKNOWN;
const SUMOReal DEFAULT_VEH_PROB = (SUMOReal) 1.;
const SUMOReal DEFAULT_VEH_SPEEDFACTOR = (SUMOReal) 1.;
const SUMOReal DEFAULT_VEH_SPEEDDEV = (SUMOReal) 0.;
const SUMOReal DEFAULT_VEH_GUIWIDTH = (SUMOReal) 2.;
const SUMOReal DEFAULT_VEH_GUIOFFSET = (SUMOReal) 2.5;
const std::string DEFAULT_VEH_FOLLOW_MODEL = "Krauss";
const std::string DEFAULT_VEH_LANE_CHANGE_MODEL = "dkrajzew2008";


#endif

/****************************************************************************/

