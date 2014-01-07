/****************************************************************************/
/// @file    SUMOVehicleClass.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Walter Bamberger
/// @date    2006-01-24
/// @version $Id$
///
// Definitions of SUMO vehicle classes and helper functions
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
#include <set>
#include <limits>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringBijection.h>
#include <utils/xml/SUMOXMLDefinitions.h>

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
    /// @brief render as a trolley bus
    SVS_BUS_TROLLEY,
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
    SVS_E_VEHICLE,
    /// @brief render as a giant ant
    SVS_ANT
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
    /// @brief vehicles ignoring classes
    SVC_IGNORING = 64,
    //@}


    /// @name vehicle size
    //@{

    /// @brief vehicle is a passenger car (a "normal" car)
    SVC_PASSENGER = 128,
    /// @brief vehicle is a HOV
    SVC_HOV = 256,
    /// @brief vehicle is a taxi
    SVC_TAXI = 512,
    /// @brief vehicle is a bus
    SVC_BUS = 1024,
    /// @brief vehicle is a small delivery vehicle
    SVC_DELIVERY = 2048,
    /// @brief vehicle is a large transport vehicle
    SVC_TRANSPORT = 4096,
    /// @brief vehicle is a light rail
    SVC_LIGHTRAIL = 8192,
    /// @brief vehicle is a city rail
    SVC_CITYRAIL = 16384,
    /// @brief vehicle is a slow moving transport rail
    SVC_RAIL_SLOW = 32768,
    /// @brief vehicle is a fast moving rail
    SVC_RAIL_FAST = 65536,

    /// @brief vehicle is a motorcycle
    SVC_MOTORCYCLE = 131072,
    /// @brief vehicle is a bicycle
    SVC_BICYCLE = 262144,
    /// @brief is a pedestrian
    SVC_PEDESTRIAN = 524288,
    /// @brief is a user-defined type
    SVC_CUSTOM1 = 1048576,
    /// @brief is a user-defined type
    SVC_CUSTOM2 = 2097152
                  //@}
};

extern const int SUMOVehicleClass_MAX;
extern StringBijection<SUMOVehicleClass> SumoVehicleClassStrings;
extern StringBijection<SUMOVehicleShape> SumoVehicleShapeStrings;

/* @brief bitset where each bit declares whether a certain SVC may use this edge/lane
 */
typedef int SVCPermissions;
extern const SVCPermissions SVCFreeForAll;


/**
 * @enum SUMOEmissionClass
 * @brief Definition of vehicle emission classes
 *
 * The order is important - HBEFA computation helper use it.
 * @see HelpersHBEFA
 */
enum SUMOEmissionClass {
    SVE_UNKNOWN = -1,
    // heavy duty vehicles; 3 clusters
    SVE_HDV_3_1 = 0,
    SVE_HDV_3_2,
    SVE_HDV_3_3,
    // heavy duty vehicles; 6 clusters
    SVE_HDV_6_1,
    SVE_HDV_6_2,
    SVE_HDV_6_3,
    SVE_HDV_6_4,
    SVE_HDV_6_5,
    SVE_HDV_6_6,
    // heavy duty vehicles; 12 clusters
    SVE_HDV_12_1,
    SVE_HDV_12_2,
    SVE_HDV_12_3,
    SVE_HDV_12_4,
    SVE_HDV_12_5,
    SVE_HDV_12_6,
    SVE_HDV_12_7,
    SVE_HDV_12_8,
    SVE_HDV_12_9,
    SVE_HDV_12_10,
    SVE_HDV_12_11,
    SVE_HDV_12_12,
    // passenger & light duty vehicles; 7 clusters
    SVE_P_LDV_7_1,
    SVE_P_LDV_7_2,
    SVE_P_LDV_7_3,
    SVE_P_LDV_7_4,
    SVE_P_LDV_7_5,
    SVE_P_LDV_7_6,
    SVE_P_LDV_7_7,
    // passenger & light duty vehicles; 14 clusters
    SVE_P_LDV_14_1,
    SVE_P_LDV_14_2,
    SVE_P_LDV_14_3,
    SVE_P_LDV_14_4,
    SVE_P_LDV_14_5,
    SVE_P_LDV_14_6,
    SVE_P_LDV_14_7,
    SVE_P_LDV_14_8,
    SVE_P_LDV_14_9,
    SVE_P_LDV_14_10,
    SVE_P_LDV_14_11,
    SVE_P_LDV_14_12,
    SVE_P_LDV_14_13,
    SVE_P_LDV_14_14,
    // no emissions
    SVE_ZERO_EMISSIONS,
    // heavy duty vehicles, no accel; 3 clusters
    SVE_HDV_A0_3_1,
    SVE_HDV_A0_3_2,
    SVE_HDV_A0_3_3,
    // heavy duty vehicles, no accel; 6 clusters
    SVE_HDV_A0_6_1,
    SVE_HDV_A0_6_2,
    SVE_HDV_A0_6_3,
    SVE_HDV_A0_6_4,
    SVE_HDV_A0_6_5,
    SVE_HDV_A0_6_6,
    // heavy duty vehicles, no accel; 12 clusters
    SVE_HDV_A0_12_1,
    SVE_HDV_A0_12_2,
    SVE_HDV_A0_12_3,
    SVE_HDV_A0_12_4,
    SVE_HDV_A0_12_5,
    SVE_HDV_A0_12_6,
    SVE_HDV_A0_12_7,
    SVE_HDV_A0_12_8,
    SVE_HDV_A0_12_9,
    SVE_HDV_A0_12_10,
    SVE_HDV_A0_12_11,
    SVE_HDV_A0_12_12,
    // passenger & light duty vehicles, no accel; 7 clusters
    SVE_P_LDV_A0_7_1,
    SVE_P_LDV_A0_7_2,
    SVE_P_LDV_A0_7_3,
    SVE_P_LDV_A0_7_4,
    SVE_P_LDV_A0_7_5,
    SVE_P_LDV_A0_7_6,
    SVE_P_LDV_A0_7_7,
    // passenger & light duty vehicles, no accel; 14 clusters
    SVE_P_LDV_A0_14_1,
    SVE_P_LDV_A0_14_2,
    SVE_P_LDV_A0_14_3,
    SVE_P_LDV_A0_14_4,
    SVE_P_LDV_A0_14_5,
    SVE_P_LDV_A0_14_6,
    SVE_P_LDV_A0_14_7,
    SVE_P_LDV_A0_14_8,
    SVE_P_LDV_A0_14_9,
    SVE_P_LDV_A0_14_10,
    SVE_P_LDV_A0_14_11,
    SVE_P_LDV_A0_14_12,
    SVE_P_LDV_A0_14_13,
    SVE_P_LDV_A0_14_14
};

extern StringBijection<SUMOEmissionClass> SumoEmissionClassStrings;


// ===========================================================================
// method declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// abstract vehicle class / purpose
// ---------------------------------------------------------------------------
/* @brief SUMOVehicleClass is meant to be OR'ed to combine information about vehicle
 * ownership and vehicle "size" into one int.
 * These OR'ed values cannot be translated directly into strings with toString().
 * The names of all base values are concatenated with '|' as a separator.
 */
extern std::string getVehicleClassCompoundName(int id);


/** @brief Returns the ids of the given classes, divided using a ' '
 * @param[in] the permissions to encode
 * @return The string representation of these classes
 */
extern std::string getAllowedVehicleClassNames(SVCPermissions permissions);


/** @brief Returns the ids of the given classes, divided using a ' '
 * @param[in] the permissions to encode
 * @return The string representation of these classes as a vector
 */
extern std::vector<std::string> getAllowedVehicleClassNamesList(SVCPermissions permissions);

/** @brief returns the shorter encoding of the given permissions
 * (selects automatically wether to use allow or disallow attribute)
 * @return the string and true for allow, false for disallow
 */
extern std::pair<std::string, bool> getPermissionEncoding(SVCPermissions permissions);


/** @brief Returns the class id of the abstract class given by its name
 * @param[in] name The name of the abstract vehicle class
 * @return The internal representation of this class. Name must not be a
 * compound name
 */
extern SUMOVehicleClass getVehicleClassID(const std::string& name);

/** @brief Returns the OR'ed id of the compound class given by its name
 * @param[in] name The name of the abstract vehicle class
 * @return The OR'ed combination of base enum values
 */
extern int getVehicleClassCompoundID(const std::string& name);

/** @brief Parses the given definition of allowed vehicle classes into the given containers
 *
 * @param[in] classNames Space separated class names
 * @param[out] container The set of vehicle classes to fill
 * throws ProcessError if parsing fails
 */
extern SVCPermissions parseVehicleClasses(const std::string& allowedS);


/** @brief Checks whether the given string contains only known vehicle classes
 */
extern bool canParseVehicleClasses(const std::string& classes);

/** @brief Parses the given definition of allowed/disallowed vehicle classes into the given containers
 *
 * @param[in] allowedS Definition which classes are allowed
 * @param[in] disallowedS Definition which classes are not allowed
 */
/** @brief Encodes the given vector of allowed and disallowed classs into a bitset
 * @param[in] allowedS Definition which classes are allowed
 * @param[in] disallowedS Definition which classes are not allowed
 */
extern SVCPermissions parseVehicleClasses(const std::string& allowedS, const std::string& disallowedS);


/** @brief Encodes the given vector of allowed classs into a bitset
 * @param[in] classesS The names vector to parse
 */
extern SVCPermissions parseVehicleClasses(const std::vector<std::string>& allowedS);


// ---------------------------------------------------------------------------
// vehicle shape class
// ---------------------------------------------------------------------------
/** @brief Returns the class name of the shape class given by its id
 * @param[in] id The id of the shape class
 * @return The string representation of this class
 */
extern std::string getVehicleShapeName(SUMOVehicleShape id);


/** @brief Returns the class id of the shape class given by its name
 * @param[in] name The name of the shape class
 * @return The internal representation of this class
 */
extern SUMOVehicleShape getVehicleShapeID(const std::string& name);


// ---------------------------------------------------------------------------
// emission class
// ---------------------------------------------------------------------------
/** @brief Returns the class name of the emission class given by its id
 * @param[in] id The id of the emission class
 * @return The string representation of this class
 */
extern std::string getVehicleEmissionTypeName(SUMOEmissionClass id);


/** @brief Returns the class id of the emission class given by its name
 * @param[in] name The name of the emission class
 * @return The internal representation of this class
 */
extern SUMOEmissionClass getVehicleEmissionTypeID(const std::string& name);


/** @brief Returns whether an edge with the given permission is a railway edge
 * @param[in] permissions The permissions of the edge
 * @return Whether the edge is a railway edge
 */
extern bool isRailway(SVCPermissions permissions);


// ---------------------------------------------------------------------------
// default vehicle type parameter
// ---------------------------------------------------------------------------
extern const std::string DEFAULT_VTYPE_ID;
extern const SUMOReal DEFAULT_VEH_MAXSPEED;
extern const SUMOReal DEFAULT_VEH_ACCEL;
extern const SUMOReal DEFAULT_VEH_DECEL;
extern const SUMOReal DEFAULT_VEH_SIGMA;
extern const SUMOReal DEFAULT_VEH_LENGTH;
extern const SUMOReal DEFAULT_VEH_MINGAP;
extern const SUMOReal DEFAULT_VEH_TAU;
extern const SUMOVehicleClass DEFAULT_VEH_CLASS;
extern const SUMOReal DEFAULT_VEH_PROB;
extern const SUMOReal DEFAULT_VEH_SPEEDFACTOR;
extern const SUMOReal DEFAULT_VEH_SPEEDDEV;
extern const SUMOReal DEFAULT_VEH_WIDTH;
extern const SUMOReal DEFAULT_VEH_HEIGHT;
extern const SumoXMLTag DEFAULT_VEH_FOLLOW_MODEL;
extern const LaneChangeModel DEFAULT_VEH_LANE_CHANGE_MODEL;
extern const SUMOVehicleShape DEFAULT_VEH_SHAPE;
extern const SUMOReal DEFAULT_VEH_TMP1;
extern const SUMOReal DEFAULT_VEH_TMP2;
extern const SUMOReal DEFAULT_VEH_TMP3;
extern const SUMOReal DEFAULT_VEH_TMP4;
extern const SUMOReal DEFAULT_VEH_TMP5;

extern const SUMOReal DEFAULT_PERSON_SPEED;

#endif

/****************************************************************************/

