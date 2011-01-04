/****************************************************************************/
/// @file    SUMOVehicleParameter.h
/// @author  Daniel Krajzewicz
/// @date    2006-01-24
/// @version $Id$
///
// Structure representing possible vehicle parameter
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include "SUMOVehicleClass.h"
#include "RGBColor.h"
#include "SUMOTime.h"


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
const int VEHPARS_DEPARTLANE_SET = 4;
const int VEHPARS_DEPARTPOS_SET = 8;
const int VEHPARS_DEPARTSPEED_SET = 16;
const int VEHPARS_PERIODNUM_SET = 32;
const int VEHPARS_PERIODFREQ_SET = 64;
const int VEHPARS_ROUTE_SET = 128;
const int VEHPARS_ARRIVALLANE_SET = 256;
const int VEHPARS_ARRIVALPOS_SET = 512;
const int VEHPARS_ARRIVALSPEED_SET = 1024;
const int VEHPARS_LINE_SET = 2048;
const int VEHPARS_TAZ_SET = 4096;

const int STOP_INDEX_END = -1;
const int STOP_INDEX_FIT = -2;


// ===========================================================================
// enum definitions
// ===========================================================================
/**
 * @enum DepartDefinition
 * @brief Possible ways to depart
 * @todo Recheck usage!!!
 */
enum DepartDefinition {
    /// @brief The time is given
    DEPART_GIVEN,
    /// @brief The departure is person triggered
    DEPART_TRIGGERED
};


/**
 * @enum DepartLaneDefinition
 * @brief Possible ways to choose a lane on depart
 * @todo Recheck usage!!!
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
    /// @brief The edge's departure lane is used
    DEPART_LANE_DEPARTLANE,
    /// @brief The least occupied lane from lanes which allow the continuation
    DEPART_LANE_ALLOWED_FREE,
    /// @brief The least occupied lane from best lanes
    DEPART_LANE_BEST_FREE
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
    /// @brief If a fixed number of random choices fails, a free position is chosen
    DEPART_POS_RANDOM_FREE
};


/**
 * @enum DepartSpeedDefinition
 * @brief Possible ways to choose the departure speed
 * @todo Recheck usage!!!
 */
enum DepartSpeedDefinition {
    /// @brief No information given; use default
    DEPART_SPEED_DEFAULT,
    /// @brief The speed is given
    DEPART_SPEED_GIVEN,
    /// @brief The speed is chosen randomly
    DEPART_SPEED_RANDOM,
    /// @brief The maximum speed is used
    DEPART_SPEED_MAX
};


/**
 * @enum ArrivalLaneDefinition
 * @brief Possible ways to choose the arrival lane
 * @todo Recheck usage!!!
 */
enum ArrivalLaneDefinition {
    /// @brief No information given; use default
    ARRIVAL_LANE_DEFAULT,
    /// @brief The arrival lane is given
    ARRIVAL_LANE_GIVEN,
    /// @brief The current lane shall be used
    ARRIVAL_LANE_CURRENT
};


/**
 * @enum ArrivalPosDefinition
 * @brief Possible ways to choose the arrival position
 * @todo Recheck usage!!!
 */
enum ArrivalPosDefinition {
    /// @brief No information given; use default
    ARRIVAL_POS_DEFAULT,
    /// @brief The arrival position is given
    ARRIVAL_POS_GIVEN,
    /// @brief The arrival position is chosen randomly
    ARRIVAL_POS_RANDOM,
    /// @brief The maximum arrival position is used
    ARRIVAL_POS_MAX
};


/**
 * @enum ArrivalSpeedDefinition
 * @brief Possible ways to choose the arrival speed
 * @todo Recheck usage!!!
 */
enum ArrivalSpeedDefinition {
    /// @brief No information given; use default
    ARRIVAL_SPEED_DEFAULT,
    /// @brief The speed is given
    ARRIVAL_SPEED_GIVEN,
    /// @brief The current speed is used
    ARRIVAL_SPEED_CURRENT
};


// ===========================================================================
// struct definitions
// ===========================================================================
/**
 * @class SUMOVehicleParameter
 * @brief Structure representing possible vehicle parameter
 *
 * The fields yielding with "Procedure" describe whether the according value
 *  shall be used or another procedure is used to choose the value.
 * @see DepartLaneDefinition
 * @see DepartPosDefinition
 * @see DepartSpeedDefinition
 */
class SUMOVehicleParameter {
public:
    /** @brief Constructor
     *
     * Initialises the structure with default values
     */
    SUMOVehicleParameter() throw();


    /** @brief Returns whether the given parameter was set
     * @param[in] what The parameter which one asks for
     * @return Whether the given parameter was set
     */
    bool wasSet(int what) const throw() {
        return (setParameter&what)!=0;
    }


    /** @brief Writes the parameters as a beginning element
     *
     * @param[in] xmlElem The name of the element to write
     * @param[in, out] dev The device to write into
     * @param[in] oc The options to get defaults from
     * @exception IOError not yet implemented
     */
    void writeAs(const std::string &xmlElem, OutputDevice &dev,
                 const OptionsCont &oc) const throw(IOError);


    /** @brief Returns whether the defaults shall be used
     * @param[in] oc The options to get the options from
     * @param[in] optionName The name of the option to determine whether its value shall be used
     * @return Whether the option is set and --defaults-override was set
     */
    bool defaultOptionOverrides(const OptionsCont &oc, const std::string &optionName) const throw();



    /// @name Depart/arrival-attributes verification
    /// @{

    /** @brief Validates a given departlane value
     * @param[in] val The departlane value to validate
     * @return Whether the given value is a valid departlane definition
     */
    static bool departlaneValidate(const std::string &val) throw();


    /** @brief Validates a given departpos value
     * @param[in] val The departpos value to validate
     * @return Whether the given value is a valid departpos definition
     */
    static bool departposValidate(const std::string &val) throw();


    /** @brief Validates a given departspeed value
     * @param[in] val The departspeed value to validate
     * @return Whether the given value is a valid departspeed definition
     */
    static bool departspeedValidate(const std::string &val) throw();


    /** @brief Validates a given arrivallane value
     * @param[in] val The arrivallane value to validate
     * @return Whether the given value is a valid arrivallane definition
     */
    static bool arrivallaneValidate(const std::string &val) throw();


    /** @brief Validates a given arrivalpos value
     * @param[in] val The arrivalpos value to validate
     * @return Whether the given value is a valid arrivalpos definition
     */
    static bool arrivalposValidate(const std::string &val) throw();


    /** @brief Validates a given arrivalspeed value
     * @param[in] val The arrivalspeed value to validate
     * @return Whether the given value is a valid arrivalspeed definition
     */
    static bool arrivalspeedValidate(const std::string &val) throw();
    /// @}



    /// @brief The vehicle's id
    std::string id;

    /// @brief The vehicle's route id
    std::string routeid;
    /// @brief The vehicle's type id
    std::string vtypeid;
    /// @brief The vehicle's color
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
#ifdef HAVE_SUBSECOND_TIMESTEPS
    SUMOTime repetitionOffset;
#else
    SUMOReal repetitionOffset;
#endif
    /// @}


    /// @brief The vehicle's line (mainly for public transport)
    std::string line;

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
        /// @brief whether the vehicle is removed from the net while stopping
        bool parking;
        /// @brief at which position in the stops list
        int index;
    };

    /// @brief List of the stops the vehicle will make
    std::vector<Stop> stops;

    /// @brief Information for the router which parameter were set
    int setParameter;

};

#endif

/****************************************************************************/

