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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
// class declarations
// ===========================================================================
class OutputDevice;


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
    /// @brief render as a moped
    SVS_MOPED,
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
    SVS_TRUCK,
    /// @brief render as a semi-trailer transport vehicle ("Sattelschlepper")
    SVS_TRUCK_SEMITRAILER,
    /// @brief render as a transport vehicle with one trailer
    SVS_TRUCK_1TRAILER,
    /// @brief render as a bus
    SVS_BUS,
    /// @brief render as a coach
    SVS_BUS_COACH,
    /// @brief render as a flexible city bus
    SVS_BUS_FLEXIBLE,
    /// @brief render as a trolley bus
    SVS_BUS_TROLLEY,
    /// @brief render as a rail
    SVS_RAIL,
    /// @brief render as a (city) rail without locomotive
    SVS_RAIL_CAR,
    /// @brief render as a cargo train
    SVS_RAIL_CARGO,
    /// @brief render as a (futuristic) e-vehicle
    SVS_E_VEHICLE,
    /// @brief render as a giant ant
    SVS_ANT,
    /// @brief render as a arbitrary ship
    SVS_SHIP,
    /// @brief render as an emergency vehicle
    SVS_EMERGENCY
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
    /// @brief vehicles ignoring classes
    SVC_IGNORING = 0,

    /// @name vehicle ownership
    //@{

    /// @brief private vehicles
    SVC_PRIVATE = 1,
    /// @brief public emergency vehicles
    SVC_EMERGENCY = 1 << 1,
    /// @brief authorities vehicles
    SVC_AUTHORITY = 1 << 2,
    /// @brief army vehicles
    SVC_ARMY = 1 << 3,
    /// @brief vip vehicles
    SVC_VIP = 1 << 4,
    //@}


    /// @name vehicle size
    //@{

    /// @brief vehicle is a passenger car (a "normal" car)
    SVC_PASSENGER = 1 << 5,
    /// @brief vehicle is a HOV
    SVC_HOV = 1 << 6,
    /// @brief vehicle is a taxi
    SVC_TAXI = 1 << 7,
    /// @brief vehicle is a bus
    SVC_BUS = 1 << 8,
    /// @brief vehicle is a coach
    SVC_COACH = 1 << 9,
    /// @brief vehicle is a small delivery vehicle
    SVC_DELIVERY = 1 << 10,
    /// @brief vehicle is a large transport vehicle
    SVC_TRUCK = 1 << 11,
    /// @brief vehicle is a large transport vehicle
    SVC_TRAILER = 1 << 12,
    /// @brief vehicle is a light rail
    SVC_TRAM = 1 << 13,
    /// @brief vehicle is a city rail
    SVC_RAIL_URBAN = 1 << 14,
    /// @brief vehicle is a not electrified rail
    SVC_RAIL = 1 << 15,
    /// @brief vehicle is a (possibly fast moving) electric rail
    SVC_RAIL_ELECTRIC = 1 << 16,

    /// @brief vehicle is a motorcycle
    SVC_MOTORCYCLE = 1 << 17,
    /// @brief vehicle is a moped
    SVC_MOPED = 1 << 18,
    /// @brief vehicle is a bicycle
    SVC_BICYCLE = 1 << 19,
    /// @brief is a pedestrian
    SVC_PEDESTRIAN = 1 << 20,
    /// @brief is an electric vehicle
    SVC_E_VEHICLE = 1 << 21,
    /// @brief is an arbitrary ship
    SVC_SHIP = 1 << 22,
    /// @brief is a user-defined type
    SVC_CUSTOM1 = 1 << 23,
    /// @brief is a user-defined type
    SVC_CUSTOM2 = 1 << 24,
    //@}

    /// @brief classes which (normally) do not drive on normal roads
    SVC_NON_ROAD = SVC_TRAM | SVC_RAIL | SVC_RAIL_URBAN | SVC_RAIL_ELECTRIC | SVC_SHIP
};

extern const int SUMOVehicleClass_MAX;
extern StringBijection<SUMOVehicleClass> SumoVehicleClassStrings;
extern std::set<std::string> deprecatedVehicleClassesSeen;
extern StringBijection<SUMOVehicleShape> SumoVehicleShapeStrings;

/* @brief bitset where each bit declares whether a certain SVC may use this edge/lane
 */
typedef int SVCPermissions;
extern const SVCPermissions SVCAll; // everything allowed
extern const SVCPermissions SVC_UNSPECIFIED; // permissions not specified


/**
 * @enum SUMOEmissionClass
 * @brief Definition of vehicle emission classes
 * @see PollutantsInterface
 */
typedef int SUMOEmissionClass;


// ===========================================================================
// method declarations
// ===========================================================================

// ---------------------------------------------------------------------------
// abstract vehicle class / purpose
// ---------------------------------------------------------------------------
/** @brief Returns the ids of the given classes, divided using a ' '
 * @param[in] the permissions to encode
 * @return The string representation of these classes
 */
extern std::string getVehicleClassNames(SVCPermissions permissions);


/** @brief Returns the ids of the given classes, divided using a ' '
 * @param[in] the permissions to encode
 * @return The string representation of these classes as a vector
 */
extern std::vector<std::string> getVehicleClassNamesList(SVCPermissions permissions);


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
 * Deprecated classes go into a separate container.
 *
 * @param[in] classNames Space separated class names
 * @param[out] container The set of vehicle classes to fill
 * throws ProcessError if parsing fails
 */
extern SVCPermissions parseVehicleClasses(const std::string& allowedS);


/** @brief Checks whether the given string contains only known vehicle classes
 */
extern bool canParseVehicleClasses(const std::string& classes);

/** @brief Encodes the given vector of allowed and disallowed classes into a bitset
 * @param[in] allowedS Definition which classes are allowed
 * @param[in] disallowedS Definition which classes are not allowed
 */
extern SVCPermissions parseVehicleClasses(const std::string& allowedS, const std::string& disallowedS);


/** @brief Encodes the given vector of allowed classs into a bitset
 * Unlike the methods which parse a string it gives immediately a warning output on deprecated vehicle classes.
 * @param[in] classesS The names vector to parse
 */
extern SVCPermissions parseVehicleClasses(const std::vector<std::string>& allowedS);


/// @brief writes allowed disallowed attributes if needed;
extern void writePermissions(OutputDevice& into, SVCPermissions permissions);

/// @brief writes allowed disallowed attributes if needed;
extern void writePreferences(OutputDevice& into, SVCPermissions preferred);

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


/** @brief Returns whether an edge with the given permission is a railway edge
 * @param[in] permissions The permissions of the edge
 * @return Whether the edge is a railway edge
 */
extern bool isRailway(SVCPermissions permissions);

/** @brief Returns whether an edge with the given permission is a waterway edge
 * @param[in] permissions The permissions of the edge
 * @return Whether the edge is a waterway edge
 */
extern bool isWaterway(SVCPermissions permissions);

/** @brief Returns whether an edge with the given permission is a forbidden edge
 * @param[in] permissions The permissions of the edge
 * @return Whether the edge is forbidden
 */
extern bool isForbidden(SVCPermissions permissions);

// ---------------------------------------------------------------------------
// default vehicle type parameter
// ---------------------------------------------------------------------------
extern const std::string DEFAULT_VTYPE_ID;
extern const std::string DEFAULT_PEDTYPE_ID;

extern const SUMOReal DEFAULT_VEH_PROB; // !!! does this belong here?

extern const SUMOReal DEFAULT_PEDESTRIAN_SPEED;

extern const SUMOReal DEFAULT_CONTAINER_TRANSHIP_SPEED;

#endif

/****************************************************************************/

