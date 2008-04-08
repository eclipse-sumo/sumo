/****************************************************************************/
/// @file    SUMOVehicleParameter.h
/// @author  Daniel Krajzewicz
/// @date    2006-01-24
/// @version $Id: SUMOVehicleParameter.h 5002 2008-02-01 13:46:21Z dkrajzew $
///
// Structure representing possible vehicle parameter
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


// ===========================================================================
// enum definitions
// ===========================================================================
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
    DEPART_LANE_DEPARTLANE
};


/**
 * @enum DepartPosDefinition
 * @brief Possible ways to choose the departure position
 * @todo Recheck usage!!!
 */
enum DepartPosDefinition {
    /// @brief No information given; use default
    DEPART_POS_DEFAULT,
    /// @brief The position is given
    DEPART_POS_GIVEN,
    /// @brief The position is chosen randomly
    DEPART_POS_RANDOM,
    /// @brief A free position is chosen
    DEPART_POS_FREE
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
    SUMOVehicleParameter() 
        : depart(-1), departLaneProcedure(DEPART_LANE_DEFAULT), 
        departPosProcedure(DEPART_POS_DEFAULT), departSpeedProcedure(DEPART_SPEED_DEFAULT),
        repetitionNumber(-1), repetitionOffset(-1), setParameter(0)
    {
    }


    /// @brief The vehicle's id
    std::string id;

    //std::string refid;

    /// @brief The vehicle's route id
    std::string routeid;
    /// @brief The vehicle's type id
    std::string vtypeid;
    /// @brief The vehicle's color
    RGBColor color;


    /// @name Departure definition
    /// @{

    /// @brief The vehicle's departure time
    SUMOTime depart;
    /// @brief (optional) The lane the vehicle shall depart from
    std::string departLane;
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


    /// @name Departure definition
    /// @{

    /// @brief currently unused; not yet implemented
    std::string arrivalLane;
    /// @brief currently unused; not yet implemented
    SUMOReal arrivalPos;
    /// @brief currently unused; not yet implemented
    SUMOReal arrivalSpeed;
    /// @}


    /// @name Repetition definition
    /// @{

    /// @brief The number of times by which the vehicle shall be repeatedly inserted
    int repetitionNumber;
    /// @brief The time offset between vehicle reinsertions
    SUMOTime repetitionOffset;
    /// @}


    /// @brief Information which parameter have been set
    int setParameter;

};

#endif

/****************************************************************************/

