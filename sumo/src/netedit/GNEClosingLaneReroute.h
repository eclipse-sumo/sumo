/****************************************************************************/
/// @file    GNEClosingLaneReroute.h
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
#ifndef GNEClosingLaneReroute_h
#define GNEClosingLaneReroute_h


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
#include <utils/common/SUMOVehicleClass.h>
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
 * @class GNEClosingLaneReroute
 * forces the rerouter to close the lane
 */

class GNEClosingLaneReroute {
public:
    /// @brief constructor
    GNEClosingLaneReroute(GNERerouterInterval *rerouterIntervalParent, std::string closedEdgeId, std::vector<SUMOVehicleClass> allowVehicles, std::vector<SUMOVehicleClass> disallowVehicles);

    /// @brief destructor
    ~GNEClosingLaneReroute();

    /**@brief insert an allow vehicle
     * @throw ProcessError if allowed vehicle was already inserted
     */
    void insertAllowVehicle(SUMOVehicleClass vclass);

    /**@brief remove a previously inserted allow vehicle
     * @throw ProcessError if allowed vehicle cannot be found
     */
    void removeAllowVehicle(SUMOVehicleClass vclass);

    /**@brief insert a disallow vehicle
     * @throw ProcessError if disallowed vehicle was already inserted
     */
    void insertDisallowVehicle(SUMOVehicleClass vclass);

    /**@brief remove a previously inserted disallow vehicle
     * @throw ProcessError if disallowed vehicle cannot be found
     */
    void removeDisallowVehicle(SUMOVehicleClass vclass);

    /// @brief get allow vehicles
    const std::vector<SUMOVehicleClass> &getAllowVehicles() const;

    /// @brief get disallow vehicles
    const std::vector<SUMOVehicleClass> &getDisallowVehicles() const;

    /// @brief get closed edge Id
    std::string getClosedEdgeId() const;

    /// @brief get tag
    SumoXMLTag getTag() const;

    /// @brief get rerouter interval parent
    GNERerouterInterval *getRerouterIntervalParent() const;

private:
    /// @brief pointer to rerouter interval parent
    GNERerouterInterval *myRerouterIntervalParent;

    /// @brief XML Tag of closing lane reroute
    SumoXMLTag myTag;

    /// @brief edge ID
    std::string myClosedEdgeId;

    /// @brief vector of allow vehicles
    std::vector<SUMOVehicleClass> myAllowVehicles;

    /// @brief vector of disallow vehicles
    std::vector<SUMOVehicleClass> myDisallowVehicles;
};

#endif

/****************************************************************************/
