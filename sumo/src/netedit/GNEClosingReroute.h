/****************************************************************************/
/// @file    GNEClosingReroute.h
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
 * @class GNEClosingReroute
 * the rerouter to close the street (edge)
 */

class GNEClosingReroute {
public:
    /// @brief constructor
    GNEClosingReroute(GNERerouterInterval& rerouterIntervalParent, GNEEdge* closedEdge,
                      SVCPermissions allowedVehicles = SVC_IGNORING, SVCPermissions disallowedVehicles = SVC_IGNORING);

    /// @brief destructor
    ~GNEClosingReroute();

    /// @brief get allowed vehicles
    SVCPermissions getAllowedVehicles() const;

    /// @brief get disallowed vehicles
    SVCPermissions getDisallowedVehicles() const;

    /// @brief set allowed vehicles
    void setAllowedVehicles(SVCPermissions allowed);

    /// @brief set disallowed vehicles
    void setDisallowedVehicles(SVCPermissions disallowed);

    /// @brief get closed edge
    GNEEdge* getClosedEdge() const;

    /// @brief set closed edge
    void setClosedEdge(GNEEdge* edge);

    /// @brief get tag
    SumoXMLTag getTag() const;

    /// @brief get rerouter interval parent
    const GNERerouterInterval& getRerouterIntervalParent() const;

    /// @brief overload operator ==
    bool operator==(const GNEClosingReroute& closingReroute);

protected:
    /// @brief reference to rerouter interval parent
    GNERerouterInterval* myRerouterIntervalParent;

    /// @brief closed edge
    GNEEdge* myClosedEdge;

    /// @brief codified allowed vehicles
    SVCPermissions myAllowedVehicles;

    /// @brief codified disallowed vehicles
    SVCPermissions myDisallowedVehicles;

    /// @brief XML Tag of closing reroute
    SumoXMLTag myTag;
};

#endif

/****************************************************************************/
