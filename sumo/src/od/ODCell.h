/****************************************************************************/
/// @file    ODCell.h
/// @author  Peter Mieth
/// @author  Daniel Krajzewicz
/// @author  Yun-Pang Floetteroed
/// @date    Sept 2002
/// @version $Id$
///
// A single O/D-matrix cell
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef ODCell_h
#define ODCell_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <map>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class RORoute;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @struct ODCell
 * @brief A single O/D-matrix cell
 *
 * A single cell within an O/D-matrix. Contains the information about the origin
 *  and destination via string-ids of the district, the begin and the end time
 *  for which this cell is valid, the id of the vehicle type to use, and the
 *  amount of vehicles to insert during the described interval.
 */
struct ODCell {
    /// @brief The number of vehicles
    SUMOReal vehicleNumber;

    /// @brief The begin time this cell describes
    SUMOTime begin;

    /// @brief The end time this cell describes
    SUMOTime end;

    /// @brief Name of the origin district
    std::string origin;

    /// @brief Name of the destination district
    std::string destination;

    /// @brief Name of the vehicle type
    std::string vehicleType;

    /// @brief the list of paths / routes
    std::vector<RORoute*> pathsVector;  // path_id, string of edges?

    /// @brief mapping of departure times to departing vehicles, if already fixed
    std::map<SUMOTime, std::vector<std::string> > departures;
};


#endif

/****************************************************************************/

