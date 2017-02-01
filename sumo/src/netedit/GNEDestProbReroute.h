/****************************************************************************/
/// @file    GNEDestProbReroute.h
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
#ifndef GNEDestProbReroute_h
#define GNEDestProbReroute_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <utils/common/UtilExceptions.h>
#include <utils/xml/SUMOXMLDefinitions.h>

// ===========================================================================
// class declarations
// ===========================================================================

class GNEEdge;
class GNERerouterInterval;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEDestProbReroute
 * forces the rerouter to assign a new route
 */
class GNEDestProbReroute {
public:
    /// @brief constructor
    GNEDestProbReroute(GNERerouterInterval *rerouterIntervalParent, GNEEdge *newEdgeDestination, SUMOReal probability);

    /// @brief destructor
    ~GNEDestProbReroute();

    /// @brief get edge destination
    GNEEdge *getNewDestination() const;

    /// @brief get probability
    SUMOReal getProbability() const;

    /// @brief set probability, if new probability is valid
    bool setProbability(SUMOReal probability);

    /// @brief get tag
    SumoXMLTag getTag() const;

    /// @brief get rerouter interval parent
    GNERerouterInterval *getRerouterIntervalParent() const;

private:
    /// @brief pointer to rerouter interval parent
    GNERerouterInterval *myRerouterIntervalParent;

    /// @brief XML Tag of destiny probability reroute
    SumoXMLTag myTag;

    /// @brief id of new edge destination
    GNEEdge *myNewEdgeDestination;

    /// @brief probability with which a vehicle will use the given edge as destination
    SUMOReal myProbability;
};

#endif

/****************************************************************************/
