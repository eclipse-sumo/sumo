/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GNETagPropertiesDatabase.h
/// @author  Pablo Alvarez lopez
/// @date    Feb 2025
///
// Database with all tag properties
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNETagProperties.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNETagPropertiesDatabase
 *
 * Abstract Base class for gui objects which carry attributes
 * inherits from GNEReferenceCounter for convenience
 */
class GNETagPropertiesDatabase {

public:
    /// @brief Constructor
    GNETagPropertiesDatabase(GNENet* net);

    /// @brief destructor
    ~GNETagPropertiesDatabase() {}

    /// @brief write machine readable attribute help to file
    void writeAttributeHelp();

    /// @brief max number of editable (non extended) attributes (needed for attributes editor)
    int maxNumberOfEditableAttributes;

    /// @brief max number of geo attributes (needed for geo attributes editor)
    int maxNumberOfGeoAttributes;

    /// @brief max number of flow attributes (needed for geo attributes editor)
    int maxNumberOfFlowAttributes;

    /// @brief max number of netedit attributes (needed for netedit attributes editor)
    int maxNumberOfNeteditAttributes;

protected:
    /// @brief pointer to net
    GNENet* myNet = nullptr;

private:
    /// @brief fill network elements
    void fillNetworkElements();

    /// @brief fill additional elements
    void fillAdditionalElements();

    /// @brief fill shape elements
    void fillShapeElements();

    /// @brief fill TAZ elements
    void fillTAZElements();

    /// @brief fill Wire elements
    void fillWireElements();

    /// @brief fill JuPedSim elements
    void fillJuPedSimElements();

    /// @brief fill demand elements
    void fillDemandElements();

    /// @brief fill vehicle elements
    void fillVehicleElements();

    /// @brief fill stop elements
    void fillStopElements();

    /// @brief fill waypoint elements
    void fillWaypointElements();

    /// @brief fill person elements
    void fillPersonElements();

    /// @brief fill person plan trips
    void fillPersonPlanTrips();

    /// @brief fill person plan walks
    void fillPersonPlanWalks();

    /// @brief fill person plan rides
    void fillPersonPlanRides();

    /// @brief fill person stop elements
    void fillPersonStopElements();

    /// @brief fill container elements
    void fillContainerElements();

    /// @brief fill container transport elements
    void fillContainerTransportElements();

    /// @brief fill container tranship elements
    void fillContainerTranshipElements();

    /// @brief fill container stop elements
    void fillContainerStopElements();

    /// @brief fill common POI attributes
    void fillCommonAttributes(GNETagProperties* tagProperties);

    /// @brief fill common POI attributes
    void fillPOIAttributes(GNETagProperties* tagProperties);

    /// @brief fill common vehicle attributes (used by vehicles, trips, routeFlows and flows)
    void fillCommonVehicleAttributes(GNETagProperties* tagProperties);

    /// @brief fill common flow attributes (used by flows, routeFlows and personFlows)
    void fillCommonFlowAttributes(GNETagProperties* tagProperties, SumoXMLAttr perHour);

    /// @brief fill Car Following Model of Vehicle/Person Types
    void fillCarFollowingModelAttributes(GNETagProperties* tagProperties);

    /// @brief fill Junction Model Attributes of Vehicle/Person Types
    void fillJunctionModelAttributes(GNETagProperties* tagProperties);

    /// @brief fill Junction Model Attributes of Vehicle/Person Types
    void fillLaneChangingModelAttributes(GNETagProperties* tagProperties);

    /// @brief fill common person attributes (used by person and personFlows)
    void fillCommonPersonAttributes(GNETagProperties* tagProperties);

    /// @brief fill common container attributes (used by container and containerFlows)
    void fillCommonContainerAttributes(GNETagProperties* tagProperties);

    /// @brief fill stop person attributes
    void fillCommonStopAttributes(GNETagProperties* tagProperties, const bool waypoint);

    /// @brief fill plan from-to attribute
    void fillPlanParentAttributes(GNETagProperties* tagProperties);

    /// @brief fill person trip common attributes
    void fillPersonTripCommonAttributes(GNETagProperties* tagProperties);

    /// @brief fill walk common attributes
    void fillWalkCommonAttributes(GNETagProperties* tagProperties);

    /// @brief fill ride common attributes
    void fillRideCommonAttributes(GNETagProperties* tagProperties);

    /// @brief fill transport common attributes
    void fillTransportCommonAttributes(GNETagProperties* tagProperties);

    /// @brief fill ride common attributes
    void fillTranshipCommonAttributes(GNETagProperties* tagProperties);

    /// @brief fill plan stop common attributes
    void fillPlanStopCommonAttributes(GNETagProperties* tagProperties);

    /// @brief fill Data elements
    void fillDataElements();

    /// @brief fill stop person attributes
    void fillCommonMeanDataAttributes(GNETagProperties* tagProperties);

    /// @brief update max number of attributes by type
    void updateMaxNumberOfAttributes();

    /// @brief map with the tags properties
    std::map<SumoXMLTag, GNETagProperties*> myTagProperties;

    /// @brief map with the merged tags properties
    std::map<SumoXMLTag, GNETagProperties*> myMergedPlanTagProperties;

    /// @brief Invalidated default constructor.
    GNETagPropertiesDatabase() = delete;

    /// @brief Invalidated copy constructor.
    GNETagPropertiesDatabase(const GNETagPropertiesDatabase&) = delete;

    /// @brief Invalidated assignment operator
    GNETagPropertiesDatabase& operator=(const GNETagPropertiesDatabase& src) = delete;
};
