/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEClosingLaneReroute.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
/// @version $Id$
///
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

#include "GNEAttributeCarrier.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNELane;
class GNERerouterInterval;
class GNERerouterIntervalDialog;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEClosingLaneReroute
 * forces the rerouter to close the lane
 */
class GNEClosingLaneReroute : public GNEAttributeCarrier {

public:
    /// @brief constructor (Used in GNERerouterIntervalDialog)
    GNEClosingLaneReroute(GNERerouterIntervalDialog* rerouterIntervalDialog);

    /// @brief constructor
    GNEClosingLaneReroute(GNERerouterInterval* rerouterIntervalParent, GNELane* closedLane,
                          SVCPermissions allowedVehicles = SVC_IGNORING, SVCPermissions disallowedVehicles = SVC_IGNORING);

    /// @brief destructor
    ~GNEClosingLaneReroute();

    // @brief write closing lane reroute
    void writeClosingLaneReroute(OutputDevice& device) const;

    /// @brief get rerouter interval parent
    GNERerouterInterval* getRerouterIntervalParent() const;

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /* @brief method for getting the Attribute of an XML key
    * @param[in] key The attribute key
    * @return string with the value associated to key
    */
    std::string getAttribute(SumoXMLAttr key) const;

    /* @brief method for setting the attribute and letting the object perform additional changes
    * @param[in] key The attribute key
    * @param[in] value The new value
    * @param[in] undoList The undoList on which to register changes
    */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for checking if the key and their correspond attribute are valids
    * @param[in] key The attribute key
    * @param[in] value The value asociated to key key
    * @return true if the value is valid, false in other case
    */
    bool isValid(SumoXMLAttr key, const std::string& value);
    /// @}

protected:
    /// @brief reference to rerouter interval parent
    GNERerouterInterval* myRerouterIntervalParent;

    /// @brief closed lane
    GNELane* myClosedLane;

    /// @brief codified allowed vehicles
    SVCPermissions myAllowedVehicles;

    /// @brief codified disallowed vehicles
    SVCPermissions myDisallowedVehicles;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNEClosingLaneReroute(const GNEClosingLaneReroute&) = delete;

    /// @brief Invalidated assignment operator.
    GNEClosingLaneReroute& operator=(const GNEClosingLaneReroute&) = delete;
};

#endif

/****************************************************************************/
