/****************************************************************************/
/// @file    GNERouteProbReroute.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
/// @version $Id$
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
 * @class GNERerouter
 * GNERouteProbReroute
 */
class GNERouteProbReroute {
public:
    /// @brief constructor
    GNERouteProbReroute(GNERerouterInterval& rerouterIntervalParent, std::string newRouteId, SUMOReal probability);

    /// @brief destructor
    ~GNERouteProbReroute();

    /// @brief get new route id
    std::string getNewRouteId() const;

    /// @brief set new route id
    void setNewRouteId(std::string newRouteId);

    /// @brief get probability
    SUMOReal getProbability() const;

    /// @brief set probability, if the new probability is valid
    bool setProbability(SUMOReal probability);

    /// @brief get tag
    SumoXMLTag getTag() const;

    /// @brief get rerouter interval parent
    const GNERerouterInterval& getRerouterIntervalParent() const;

    /// @brief overload == operator
    bool operator==(const GNERouteProbReroute& routeProbReroute);

protected:
    /// @brief reference to rerouter interval parent
    GNERerouterInterval* myRerouterIntervalParent;

    /// @brief id of new route
    /// @todo change string to GNERoute
    std::string myNewRouteId;

    /// @brief probability with which a vehicle will use the given edge as destination
    SUMOReal myProbability;

    /// @brief XML Tag of route probability reroute
    SumoXMLTag myTag;
};

#endif

/****************************************************************************/
