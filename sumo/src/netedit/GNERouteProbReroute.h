/****************************************************************************/
/// @file    GNERouteProbReroute.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
/// @version $Id: GNERerouter.h 22608 2017-01-17 06:28:54Z behrisch $
///
///
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNERouteProbReroute_h
#define GNERouteProbReroute_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEEdge;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNERerouter
 * GNERouteProbReroute
 */
class GNERouteProbReroute {
public:
    /// @brief constructor
    GNERouteProbReroute(std::string newRouteId, SUMOReal probability);

    /// @brief destructor
    ~GNERouteProbReroute();

    /// @brief get new route id
    std::string getNewRouteId() const;

    /// @brief get probability
    SUMOReal getProbability() const;

    /// @brief set probability
    /// @throw InvalidArgument if probability isn't valid
    void setProbability(SUMOReal probability);

private:
    /// @brief id of new route
    std::string myNewRouteId;

    /// @brief probability with which a vehicle will use the given edge as destination
    SUMOReal myProbability;
};

#endif

/****************************************************************************/
