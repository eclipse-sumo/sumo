/****************************************************************************/
/// @file    GNEClosingLaneReroute.h
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

class GNELane;
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
    GNEClosingLaneReroute(GNERerouterInterval& rerouterIntervalParent, GNELane* closedLane,
                          SVCPermissions allowedVehicles = SVC_IGNORING, SVCPermissions disallowedVehicles = SVC_IGNORING);

    /// @brief destructor
    ~GNEClosingLaneReroute();

    /// @brief get allowed vehicles
    SVCPermissions getAllowedVehicles() const;

    /// @brief get disallowed vehicles
    SVCPermissions getDisallowedVehicles() const;

    /// @brief set allowed vehicles
    void setAllowedVehicles(SVCPermissions allowed);

    /// @brief set disallowed vehicles
    void setDisallowedVehicles(SVCPermissions disallowed);

    /// @brief get closed lane Id
    GNELane* getClosedLane() const;

    /// @brief set closed lane Id
    void setClosedLane(GNELane* lane);

    /// @brief get tag
    SumoXMLTag getTag() const;

    /// @brief get rerouter interval parent
    const GNERerouterInterval& getRerouterIntervalParent() const;

    /// @brief overload operator ==
    bool operator==(const GNEClosingLaneReroute& closingLaneReroute);

protected:
    /// @brief reference to rerouter interval parent
    GNERerouterInterval* myRerouterIntervalParent;

    /// @brief closed lane
    GNELane* myClosedLane;

    /// @brief codified allowed vehicles
    SVCPermissions myAllowedVehicles;

    /// @brief codified disallowed vehicles
    SVCPermissions myDisallowedVehicles;

    /// @brief XML Tag of closing lane reroute
    SumoXMLTag myTag;
};

#endif

/****************************************************************************/
