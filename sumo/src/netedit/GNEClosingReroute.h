/****************************************************************************/
/// @file    GNEClosingReroute.h
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
#ifndef GNEClosingReroute_h
#define GNEClosingReroute_h


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
class GNERerouterInterval;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEClosingReroute
 * ------------
 */

class GNEClosingReroute {
public:
    /// @brief constructor
    GNEClosingReroute(GNERerouterInterval *rerouterIntervalParent, std::string closedEdgeId, std::vector<std::string> allowVehicles, std::vector<std::string> disallowVehicles);

    /// @brief destructor
    ~GNEClosingReroute();

    /// @brief insert an allow vehicle
    /// @throw ProcessError if allowed vehicle was already inserted
    void insertAllowVehicle(std::string vehicleid);

    /// @brief remove a previously inserted allow vehicle
    /// @throw ProcessError if allowed vehicle cannot be found in the container
    void removeAllowVehicle(std::string vehicleid);

    /// @brief insert a disallow vehicle
    /// @throw ProcessError if disallowed vehicle was already inserted
    void insertDisallowVehicle(std::string vehicleid);

    /// @brief remove a previously inserted disallow vehicle
    /// @throw ProcessError if disallowed vehicle cannot be found in the container
    void removeDisallowVehicle(std::string vehicleid);

    /// @brief get allow vehicles
    std::vector<std::string> getAllowVehicles() const;

    /// @brief get disallow vehicles
    std::vector<std::string> getDisallowVehicles() const;

    /// @brief get closed edge Id
    std::string getClosedEdgeId() const;

    /// @brief get tag
    SumoXMLTag getTag() const;

private:
    /// @brief pointer to rerouter interval parent
    GNERerouterInterval *myRerouterIntervalParent;

    /// @brief XML Tag of closing reroute
    SumoXMLTag myTag;

    /// @brief edge ID
    std::string myClosedEdgeId;

    /// @brief vector of allow vehicles
    std::vector<std::string> myAllowVehicles;

    /// @brief vector of disallow vehicles
    std::vector<std::string> myDisallowVehicles;
};

#endif

/****************************************************************************/
