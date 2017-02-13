/****************************************************************************/
/// @file    SUMOVehicleParameter.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Axel Wegener
/// @author  Michael Behrisch
/// @date    2006-01-24
/// @version $Id$
///
// Structure representing possible vehicle parameter
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
#ifndef SUMOVehicleParameter_h
#define SUMOVehicleParameter_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/Parameterised.h>
#include <utils/common/RGBColor.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/SUMOVehicleClass.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class OptionsCont;


// ===========================================================================
// value definitions
// ===========================================================================
const int VEHPARS_COLOR_SET = 1;
const int VEHPARS_VTYPE_SET = 2;
const int VEHPARS_DEPARTLANE_SET = 2 << 1;
const int VEHPARS_DEPARTPOS_SET = 2 << 2;
const int VEHPARS_DEPARTSPEED_SET = 2 << 3;
const int VEHPARS_PERIODNUM_SET = 2 << 4;
const int VEHPARS_PERIODFREQ_SET = 2 << 5;
const int VEHPARS_ROUTE_SET = 2 << 6;
const int VEHPARS_ARRIVALLANE_SET = 2 << 7;
const int VEHPARS_ARRIVALPOS_SET = 2 << 8;
const int VEHPARS_ARRIVALSPEED_SET = 2 << 9;
const int VEHPARS_LINE_SET = 2 << 10;
const int VEHPARS_FROM_TAZ_SET = 2 << 11;
const int VEHPARS_TO_TAZ_SET = 2 << 12;
const int VEHPARS_FORCE_REROUTE = 2 << 13;
const int VEHPARS_PERSON_CAPACITY_SET = 2 << 14;
const int VEHPARS_PERSON_NUMBER_SET = 2 << 15;
const int VEHPARS_CONTAINER_NUMBER_SET = 2 << 16;
const int VEHPARS_DEPARTPOSLAT_SET = 2 << 17;
const int VEHPARS_ARRIVALPOSLAT_SET = 2 << 18;

const int STOP_INDEX_END = -1;
const int STOP_INDEX_FIT = -2;

const int STOP_END_SET = 1;
const int STOP_START_SET = 2;
const int STOP_TRIGGER_SET = 2 << 1;
const int STOP_PARKING_SET = 2 << 2;
const int STOP_EXPECTED_SET = 2 << 3;
const int STOP_CONTAINER_TRIGGER_SET = 2 << 4;
const int STOP_EXPECTED_CONTAINERS_SET = 2 << 5;


// ===========================================================================
// enum definitions
// ===========================================================================
/**
 * @enum DepartDefinition
 * @brief Possible ways to depart
 */
enum DepartDefinition {
    /// @brief The time is given
    DEPART_GIVEN,
    /// @brief The departure is person triggered
    DEPART_TRIGGERED,
    /// @brief The departure is container triggered
    DEPART_CONTAINER_TRIGGERED,
    /// @brief The vehicle is discarded if emission fails (not fully implemented yet)
    DEPART_NOW,
    /// @brief Tag for the last element in the enum for safe int casting
    DEPART_DEF_MAX
};


/**
 * @enum DepartLaneDefinition
 * @brief Possible ways to choose a lane on depart
 */
enum DepartLaneDefinition {
    /// @brief No information given; use default
    DEPART_LANE_DEFAULT,
    /// @brief The lane is given
    DEPART_LANE_GIVEN,
    /// @brief The lane is chosen randomly
    DEPART_LANE_RANDOM,
    /// @brief The least occupied lane is used
    DEPART_LANE_FREE,
    /// @brief The least occupied lane from lanes which allow the continuation
    DEPART_LANE_ALLOWED_FREE,
    /// @brief The least occupied lane from best lanes
    DEPART_LANE_BEST_FREE,
    /// @brief The rightmost lane the vehicle may use
    DEPART_LANE_FIRST_ALLOWED,
    /// @brief Tag for the last element in the enum for safe int casting
    DEPART_LANE_DEF_MAX
};


/**
 * @enum DepartPosDefinition
 * @brief Possible ways to choose the departure position
 */
enum DepartPosDefinition {
    /// @brief No information given; use default
    DEPART_POS_DEFAULT,
    /// @brief The position is given
    DEPART_POS_GIVEN,
    /// @brief The position is chosen randomly
    DEPART_POS_RANDOM,
    /// @brief A free position is chosen
    DEPART_POS_FREE,
    /// @brief Back-at-zero position
    DEPART_POS_BASE,
    /// @brief Insert behind the last vehicle as close as possible to still allow the specified departSpeed. Fallback to DEPART_POS_BASE if there is no vehicle on the departLane yet.
    DEPART_POS_LAST,
    /// @brief If a fixed number of random choices fails, a free position is chosen
    DEPART_POS_RANDOM_FREE,
    /// @brief Tag for the last element in the enum for safe int casting
    DEPART_POS_DEF_MAX
};


/**
 * @enum DepartPosDefinition
 * @brief Possible ways to choose the departure position
 */
enum DepartPosLatDefinition {
    /// @brief No information given; use default
    DEPART_POSLAT_DEFAULT,
    /// @brief The position is given
    DEPART_POSLAT_GIVEN,
    /// @brief At the rightmost side of the lane
    DEPART_POSLAT_RIGHT,
    /// @brief At the center of the lane
    DEPART_POSLAT_CENTER,
    /// @brief At the leftmost side of the lane
    DEPART_POSLAT_LEFT,
    /// @brief The lateral position is chosen randomly
    DEPART_POSLAT_RANDOM,
    /// @brief A free lateral position is chosen
    DEPART_POSLAT_FREE,
    /// @brief If a fixed number of random choices fails, a free lateral position is chosen
    DEPART_POSLAT_RANDOM_FREE,
    /// @brief Tag for the last element in the enum for safe int casting
    DEPART_POSLAT_DEF_MAX
};


/**
 * @enum DepartSpeedDefinition
 * @brief Possible ways to choose the departure speed
 */
enum DepartSpeedDefinition {
    /// @brief No information given; use default
    DEPART_SPEED_DEFAULT,
    /// @brief The speed is given
    DEPART_SPEED_GIVEN,
    /// @brief The speed is chosen randomly
    DEPART_SPEED_RANDOM,
    /// @brief The maximum speed is used
    DEPART_SPEED_MAX,
    /// @brief Tag for the last element in the enum for safe int casting
    DEPART_SPEED_DEF_MAX
};


/**
 * @enum ArrivalLaneDefinition
 * @brief Possible ways to choose the arrival lane
 */
enum ArrivalLaneDefinition {
    /// @brief No information given; use default
    ARRIVAL_LANE_DEFAULT,
    /// @brief The arrival lane is given
    ARRIVAL_LANE_GIVEN,
    /// @brief The current lane shall be used
    ARRIVAL_LANE_CURRENT,
    /// @brief Tag for the last element in the enum for safe int casting
    ARRIVAL_LANE_DEF_MAX
};


/**
 * @enum ArrivalPosDefinition
 * @brief Possible ways to choose the arrival position
 */
enum ArrivalPosDefinition {
    /// @brief No information given; use default
    ARRIVAL_POS_DEFAULT,
    /// @brief The arrival position is given
    ARRIVAL_POS_GIVEN,
    /// @brief The arrival position is chosen randomly
    ARRIVAL_POS_RANDOM,
    /// @brief The maximum arrival position is used
    ARRIVAL_POS_MAX,
    /// @brief Tag for the last element in the enum for safe int casting
    ARRIVAL_POS_DEF_MAX
};


/**
 * @enum ArrivalPosLatDefinition
 * @brief Possible ways to choose the departure position
 */
enum ArrivalPosLatDefinition {
    /// @brief No information given; use default
    ARRIVAL_POSLAT_DEFAULT,
    /// @brief The position is given
    ARRIVAL_POSLAT_GIVEN,
    /// @brief At the rightmost side of the lane
    ARRIVAL_POSLAT_RIGHT,
    /// @brief At the center of the lane
    ARRIVAL_POSLAT_CENTER,
    /// @brief At the leftmost side of the lane
    ARRIVAL_POSLAT_LEFT,
    /// @brief Tag for the last element in the enum for safe int casting
    ARRIVAL_POSLAT_DEF_MAX
};


/**
 * @enum ArrivalSpeedDefinition
 * @brief Possible ways to choose the arrival speed
 */
enum ArrivalSpeedDefinition {
    /// @brief No information given; use default
    ARRIVAL_SPEED_DEFAULT,
    /// @brief The speed is given
    ARRIVAL_SPEED_GIVEN,
    /// @brief The current speed is used
    ARRIVAL_SPEED_CURRENT,
    /// @brief Tag for the last element in the enum for safe int casting
    ARRIVAL_SPEED_DEF_MAX
};


// ===========================================================================
// struct definitions
// ===========================================================================
/**
 * @class SUMOVehicleParameter
 * @brief Structure representing possible vehicle parameter
 *
 * When used within a vehicle, parameter are usually const except for selected items
 *  adaptable via TraCI which are flagged as mutable below
 * The fields yielding with "Procedure" describe whether the according value
 *  shall be used or another procedure is used to choose the value.
 * @see DepartLaneDefinition
 * @see DepartPosDefinition
 * @see DepartSpeedDefinition
 */
class SUMOVehicleParameter : public Parameterised {
public:
    /** @brief Constructor
     *
     * Initialises the structure with default values
     */
    SUMOVehicleParameter();

    /// @brief Destructor
    ~SUMOVehicleParameter();

    /** @brief Returns whether the given parameter was set
     * @param[in] what The parameter which one asks for
     * @return Whether the given parameter was set
     */
    bool wasSet(int what) const {
        return (setParameter & what) != 0;
    }


    /** @brief Writes the parameters as a beginning element
     *
     * @param[in, out] dev The device to write into
     * @param[in] oc The options to get defaults from
     * @param[in] tag The "root" tag to write (defaults to vehicle)
     * @param[in] tag The typeID to write (defaults to member vtypeid)
     * @exception IOError not yet implemented
     */
    void write(OutputDevice& dev, const OptionsCont& oc, const SumoXMLTag tag = SUMO_TAG_VEHICLE, const std::string& typeID = "") const;


    /** @brief Returns whether the defaults shall be used
     * @param[in] oc The options to get the options from
     * @param[in] optionName The name of the option to determine whether its value shall be used
     * @return Whether the option is set and --defaults-override was set
     */
    bool defaultOptionOverrides(const OptionsCont& oc, const std::string& optionName) const;



    /// @name Depart/arrival-attributes verification
    /// @{

    /** @brief Validates a given depart value
     * @param[in] val The depart value to parse
     * @param[in] element The name of the type of the parsed element, for building the error message
     * @param[in] id The id of the parsed element, for building the error message
     * @param[out] depart The parsed depart time, if given
     * @param[out] dd The parsed departProcedure definition
     * @param[out] error Error message, if an error occures
     * @return Whether the given value is a valid depart definition
     */
    static bool parseDepart(const std::string& val, const std::string& element, const std::string& id,
                            SUMOTime& depart, DepartDefinition& dd, std::string& error);


    /** @brief Validates a given departLane value
     * @param[in] val The departLane value to parse
     * @param[in] element The name of the type of the parsed element, for building the error message
     * @param[in] id The id of the parsed element, for building the error message
     * @param[out] lane The parsed lane, if given
     * @param[out] dld The parsed departLane definition
     * @param[out] error Error message, if an error occures
     * @return Whether the given value is a valid departLane definition
     */
    static bool parseDepartLane(const std::string& val, const std::string& element, const std::string& id,
                                int& lane, DepartLaneDefinition& dld, std::string& error);


    /** @brief Validates a given departPos value
     * @param[in] val The departPos value to parse
     * @param[in] element The name of the type of the parsed element, for building the error message
     * @param[in] id The id of the parsed element, for building the error message
     * @param[out] pos The parsed position, if given
     * @param[out] dpd The parsed departPos definition
     * @param[out] error Error message, if an error occures
     * @return Whether the given value is a valid departPos definition
     */
    static bool parseDepartPos(const std::string& val, const std::string& element, const std::string& id,
                               SUMOReal& pos, DepartPosDefinition& dpd, std::string& error);


    /** @brief Validates a given departPosLat value
     * @param[in] val The departPosLat value to parse
     * @param[in] element The name of the type of the parsed element, for building the error message
     * @param[in] id The id of the parsed element, for building the error message
     * @param[out] pos The parsed position, if given
     * @param[out] dpd The parsed departPos definition
     * @param[out] error Error message, if an error occures
     * @return Whether the given value is a valid departPos definition
     */
    static bool parseDepartPosLat(const std::string& val, const std::string& element, const std::string& id,
                                  SUMOReal& pos, DepartPosLatDefinition& dpd, std::string& error);


    /** @brief Validates a given departSpeed value
     * @param[in] val The departSpeed value to parse
     * @param[in] element The name of the type of the parsed element, for building the error message
     * @param[in] id The id of the parsed element, for building the error message
     * @param[out] speed The parsed speed, if given
     * @param[out] dsd The parsed departSpeed definition
     * @param[out] error Error message, if an error occures
     * @return Whether the given value is a valid departSpeed definition
     */
    static bool parseDepartSpeed(const std::string& val, const std::string& element, const std::string& id,
                                 SUMOReal& speed, DepartSpeedDefinition& dsd, std::string& error);


    /** @brief Validates a given arrivalLane value
     * @param[in] val The arrivalLane value to parse
     * @param[in] element The name of the type of the parsed element, for building the error message
     * @param[in] id The id of the parsed element, for building the error message
     * @param[out] lane The parsed lane, if given
     * @param[out] ald The parsed arrivalLane definition
     * @param[out] error Error message, if an error occures
     * @return Whether the given value is a valid arrivalLane definition
     */
    static bool parseArrivalLane(const std::string& val, const std::string& element, const std::string& id,
                                 int& lane, ArrivalLaneDefinition& ald, std::string& error);


    /** @brief Validates a given arrivalPos value
     * @param[in] val The arrivalPos value to parse
     * @param[in] element The name of the type of the parsed element, for building the error message
     * @param[in] id The id of the parsed element, for building the error message
     * @param[out] pos The parsed position, if given
     * @param[out] apd The parsed arrivalPos definition
     * @param[out] error Error message, if an error occures
     * @return Whether the given value is a valid arrivalPos definition
     */
    static bool parseArrivalPos(const std::string& val, const std::string& element, const std::string& id,
                                SUMOReal& pos, ArrivalPosDefinition& apd, std::string& error);


    /** @brief Validates a given arrivalPosLat value
     * @param[in] val The arrivalPosLat value to parse
     * @param[in] element The name of the type of the parsed element, for building the error message
     * @param[in] id The id of the parsed element, for building the error message
     * @param[out] pos The parsed position, if given
     * @param[out] apd The parsed arrivalPos definition
     * @param[out] error Error message, if an error occures
     * @return Whether the given value is a valid arrivalPos definition
     */
    static bool parseArrivalPosLat(const std::string& val, const std::string& element, const std::string& id,
                                   SUMOReal& pos, ArrivalPosLatDefinition& apd, std::string& error);


    /** @brief Validates a given arrivalSpeed value
     * @param[in] val The arrivalSpeed value to parse
     * @param[in] element The name of the type of the parsed element, for building the error message
     * @param[in] id The id of the parsed element, for building the error message
     * @param[out] speed The parsed speed, if given
     * @param[out] asd The parsed arrivalSpeed definition
     * @param[out] error Error message, if an error occures
     * @return Whether the given value is a valid arrivalSpeed definition
     */
    static bool parseArrivalSpeed(const std::string& val, const std::string& element, const std::string& id,
                                  SUMOReal& speed, ArrivalSpeedDefinition& asd, std::string& error);
    /// @}


    /** @brief Interprets negative edge positions and fits them onto a given edge
     * @param[in] pos The position to be interpreted
     * @param[in] maximumValue The maximum allowed value (edge length)
     * @param[in] attr The attribute from which the value originated
     * @param[in] id The id of the object to which this attribute belongs
     * @return Whether the interpreted position
     */
    static SUMOReal interpretEdgePos(SUMOReal pos, SUMOReal maximumValue, SumoXMLAttr attr, const std::string& id);

    /// @brief The vehicle's id
    std::string id;

    /// @brief The vehicle's route id
    std::string routeid;
    /// @brief The vehicle's type id
    std::string vtypeid;
    /// @brief The vehicle's color, TraCI may change this
    mutable RGBColor color;


    /// @name Departure definition
    /// @{

    /// @brief The vehicle's departure time
    SUMOTime depart;
    /// @brief Information how the vehicle shall choose the depart time
    DepartDefinition departProcedure;
    /// @brief (optional) The lane the vehicle shall depart from (index in edge)
    int departLane;
    /// @brief Information how the vehicle shall choose the lane to depart from
    DepartLaneDefinition departLaneProcedure;
    /// @brief (optional) The position the vehicle shall depart from
    SUMOReal departPos;
    /// @brief Information how the vehicle shall choose the departure position
    DepartPosDefinition departPosProcedure;
    /// @brief (optional) The lateral position the vehicle shall depart from
    SUMOReal departPosLat;
    /// @brief Information how the vehicle shall choose the lateral departure position
    DepartPosLatDefinition departPosLatProcedure;
    /// @brief (optional) The initial speed of the vehicle
    SUMOReal departSpeed;
    /// @brief Information how the vehicle's initial speed shall be chosen
    DepartSpeedDefinition departSpeedProcedure;
    /// @}


    /// @name Arrival definition
    /// @{

    /// @brief (optional) The lane the vehicle shall arrive on (not used yet)
    int arrivalLane;
    /// @brief Information how the vehicle shall choose the lane to arrive on
    ArrivalLaneDefinition arrivalLaneProcedure;
    /// @brief (optional) The position the vehicle shall arrive on
    SUMOReal arrivalPos;
    /// @brief Information how the vehicle shall choose the arrival position
    ArrivalPosDefinition arrivalPosProcedure;
    /// @brief (optional) The lateral position the vehicle shall arrive on
    SUMOReal arrivalPosLat;
    /// @brief Information how the vehicle shall choose the lateral arrival position
    ArrivalPosLatDefinition arrivalPosLatProcedure;
    /// @brief (optional) The final speed of the vehicle (not used yet)
    SUMOReal arrivalSpeed;
    /// @brief Information how the vehicle's end speed shall be chosen
    ArrivalSpeedDefinition arrivalSpeedProcedure;
    /// @}


    /// @name Repetition definition
    /// @{

    /// @brief The number of times the vehicle shall be repeatedly inserted
    int repetitionNumber;
    /// @brief The number of times the vehicle was already inserted
    int repetitionsDone;
    /// @brief The time offset between vehicle reinsertions
    SUMOTime repetitionOffset;
    /// @brief The probability for emitting a vehicle per second
    SUMOReal repetitionProbability;
    /// @brief The time at which the flow ends (only needed when using repetitionProbability)
    SUMOTime repetitionEnd;
    /// @}


    /// @brief The vehicle's line (mainly for public transport)
    mutable std::string line;

    /// @brief The vehicle's origin zone (district)
    std::string fromTaz;

    /// @brief The vehicle's destination zone (district)
    std::string toTaz;

    /** @struct Stop
     * @brief Definition of vehicle stop (position and duration)
     */
    struct Stop {
        /// @brief The lane to stop at
        std::string lane;
        /// @brief (Optional) bus stop if one is assigned to the stop
        std::string busstop;
        /// @brief (Optional) container stop if one is assigned to the stop
        std::string containerstop;
        /// @brief (Optional) parking area if one is assigned to the stop
        std::string parkingarea;
        /// @brief (Optional) charging station if one is assigned to the stop
        std::string chargingStation;
        /// @brief The stopping position start
        SUMOReal startPos;
        /// @brief The stopping position end
        SUMOReal endPos;
        /// @brief The stopping duration
        SUMOTime duration;
        /// @brief The time at which the vehicle may continue its journey
        SUMOTime until;
        /// @brief whether an arriving person lets the vehicle continue
        bool triggered;
        /// @brief whether an arriving container lets the vehicle continue
        bool containerTriggered;
        /// @brief whether the vehicle is removed from the net while stopping
        bool parking;
        /// @brief IDs of persons the vehicle has to wait for until departing
        std::set<std::string> awaitedPersons;
        /// @brief IDs of containers the vehicle has to wait for until departing
        std::set<std::string> awaitedContainers;
        /// @brief lanes and positions connected to this stop
        std::multimap<std::string, SUMOReal> accessPos;
        /// @brief at which position in the stops list
        int index;
        /// @brief Information for the output which parameter were set
        int setParameter;

        /** @brief Writes the stop as XML
         *
         * @param[in, out] dev The device to write into
         * @exception IOError not yet implemented
         */
        void write(OutputDevice& dev) const;
    };

    /// @brief List of the stops the vehicle will make, TraCI may add entries here
    mutable std::vector<Stop> stops;

    /// @brief List of the via-edges the vehicle must visit
    mutable std::vector<std::string> via;

    /// @brief The static number of persons in the vehicle when it departs (not including boarding persons)
    int personNumber;

    /// @brief The static number of containers in the vehicle when it departs
    int containerNumber;

    /// @brief Information for the router which parameter were set, TraCI may modify this (whe changing color)
    mutable int setParameter;


};

#endif

/****************************************************************************/


