/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
#ifndef ODCell_h
#define ODCell_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

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
    double vehicleNumber;

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

    /// @brief the origin "district" is an edge id
    bool originIsEdge = false;

    /// @brief the destination "district" is an edge id
    bool destinationIsEdge = false;
};


#endif

/****************************************************************************/

