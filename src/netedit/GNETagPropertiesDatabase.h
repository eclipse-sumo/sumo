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
// Database with all information about netedit elements
/****************************************************************************/
#pragma once
#include <config.h>

#include "GNETagProperties.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNETagPropertiesDatabase {

public:
    /// @brief constructor
    GNETagPropertiesDatabase();

    /// @brief destructor
    ~GNETagPropertiesDatabase();

    /// @brief get tagProperty associated to the given tag
    const GNETagProperties* getTagProperty(const SumoXMLTag tag, const bool hardFail) const;

    /// @brief get tagProperties associated to the given set tag (persons, stops, plans, etc.)
    const std::vector<const GNETagProperties*> getTagPropertiesSet(const SumoXMLTag tag, const bool hardFail) const;

    /// @brief get tagProperties associated to the given GNETagProperties::Type (NETWORKELEMENT, ADDITIONALELEMENT, VEHICLE, etc.)
    const std::vector<const GNETagProperties*> getTagPropertiesByType(const GNETagProperties::Type type) const;

    /// @brief max number of editable (non extended) attributes
    int getMaxNumberOfEditableAttributeRows() const;

    /// @brief get max number of geo attribute rows
    int getMaxNumberOfGeoAttributeRows() const;

    /// @brief get max number of flow attribute rows
    int getMaxNumberOfFlowAttributeRows() const;

    /// @brief get max number of netedit attribute rows
    int getMaxNumberOfNeteditAttributesRows() const;

    /// @brief get hierarchy dept
    int getHierarchyDepth() const;

    /// @brief write machine readable attribute help to file
    void writeAttributeHelp() const;

protected:
    /// @brief fill hierarchy elements
    void fillHierarchy();

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

    /// @brief fill common attributes
    void fillCommonAttributes(GNETagProperties* tagProperties);

    /// @brief fill common stoppingPlace attributes
    void fillCommonStoppingPlaceAttributes(GNETagProperties* tagProperties, const bool includeColor, const bool parkingAreaAngle);

    /// @brief fill common POI attributes
    void fillCommonPOIAttributes(GNETagProperties* tagProperties);

    /// @brief fill common route attributes
    void fillCommonRouteAttributes(GNETagProperties* tagProperties);

    /// @brief fill common vType attributes
    void fillCommonVTypeAttributes(GNETagProperties* tagProperties);

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

    /// @name specific attributes
    /// @{

    /// @brief fill ID attribute
    void fillIDAttribute(GNETagProperties* tagProperties, const bool createMode);

    /// @brief fill name attribute
    void fillNameAttribute(GNETagProperties* tagProperties);

    /// @brief fill edge attribute
    void fillEdgeAttribute(GNETagProperties* tagProperties, const bool synonymID);

    /// @brief fill lane attribute
    void fillLaneAttribute(GNETagProperties* tagProperties, const bool synonymID);

    /// @brief fill friendlyPos attribute
    void fillFriendlyPosAttribute(GNETagProperties* tagProperties);

    /// @brief fill vTypes attribute
    void fillVTypesAttribute(GNETagProperties* tagProperties);

    /// @brief fill file attribute
    void fillFileAttribute(GNETagProperties* tagProperties);

    /// @brief fill output attribute
    void fillOutputAttribute(GNETagProperties* tagProperties);

    /// @brief fill imgFile attribute
    void fillImgFileAttribute(GNETagProperties* tagProperties, const bool isExtended);

    /// @brief fill depart attribute
    void fillDepartAttribute(GNETagProperties* tagProperties);

    /// @brief fill allowDisallow attributes
    void fillAllowDisallowAttributes(GNETagProperties* tagProperties);

    /// @brief fill lane position attribute
    void fillPosOverLaneAttribute(GNETagProperties* tagProperties);

    /// @brief fill detect person attribute
    void fillDetectPersonsAttribute(GNETagProperties* tagProperties);

    /// @brief fill color attribute
    void fillColorAttribute(GNETagProperties* tagProperties, const std::string& defaultColor);

    /// @brief fill detector period attribute
    void fillDetectorPeriodAttribute(GNETagProperties* tagProperties);

    /// @brief fill detector next edges attribute
    void fillDetectorNextEdgesAttribute(GNETagProperties* tagProperties);

    /// @brief fill detector threshold attribute
    void fillDetectorThresholdAttributes(GNETagProperties* tagProperties, const bool includingJam);

    /// @brief fill distribution probability attribute
    void fillDistributionProbability(GNETagProperties* tagProperties, const bool visible);

    /// @}

    /// @brief update max number of attributes by type
    void updateMaxNumberOfAttributesEditorRows();

    /// @brief update max hierarchy depth
    void updateMaxHierarchyDepth();

private:
    /// @brief max number of editable (non extended) attributes (needed for attributes editor)
    int myMaxNumberOfEditableAttributeRows = 0;

    /// @brief max number of geo attributes (needed for geo attributes editor)
    int myMaxNumberOfGeoAttributeRows = 0;

    /// @brief max number of flow attributes (needed for geo attributes editor)
    int myMaxNumberOfFlowAttributeRows = 0;

    /// @brief max number of netedit attributes (needed for netedit attributes editor)
    int myMaxNumberOfNeteditAttributeRows = 0;

    /// @brief hierarchy dept
    int myHierarchyDepth = 0;

    /// @brief map with tag properties sets (persons, stops, etc.)
    std::map<SumoXMLTag, GNETagProperties*> mySetTagProperties;

    /// @brief map with the tags properties
    std::map<SumoXMLTag, GNETagProperties*> myTagProperties;

    /// @brief Invalidated copy constructor.
    GNETagPropertiesDatabase(const GNETagPropertiesDatabase&) = delete;

    /// @brief Invalidated assignment operator
    GNETagPropertiesDatabase& operator=(const GNETagPropertiesDatabase& src) = delete;
};
