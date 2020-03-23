/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNETagProperties.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// Abstract Base class for tag properties used in GNEAttributeCarrier
/****************************************************************************/
#pragma once


// ===========================================================================
// included modules
// ===========================================================================

#include <utils/gui/images/GUIIcons.h>

#include "GNEAttributeProperties.h"


// ===========================================================================
// class definitions
// ===========================================================================

class GNETagProperties {

public:

    enum TagType {
        // basic types
        NETWORKELEMENT =    1 << 0,  // Network elements (Edges, Junctions, Lanes...)
        ADDITIONALELEMENT = 1 << 1,  // Additional elements (Bus Stops, Charging Stations, Detectors...)
        TAZ =               1 << 2,  // Traffic Assignment Zones
        SHAPE =             1 << 3,  // Shapes (Polygons and POIs) 
        DEMANDELEMENT =     1 << 4,  // Demand elements (Routes, Vehicles, Trips...)
        DATAELEMENT =       1 << 5,  // Data elements (DataSets, Data Intervals, EdgeData...)
        // sub additional elements
        STOPPINGPLACE =     1 << 6,  // StoppingPlaces (BusStops, ChargingStations...)
        DETECTOR =          1 << 7,  // Detectors (E1, E2...)
        // sub demand elements
        VTYPE =             1 << 8,  // Vehicle types (vType and pTye)
        VEHICLE =           1 << 9,  // Vehicles (Vehicles, trips, flows, and routeFlows)
        ROUTE =             1 << 10, // Routes and embedded routes
        STOP =              1 << 11, // Stops
        PERSON =            1 << 12, // Persons (Persons and personFlows)
        PERSONPLAN =        1 << 13, // Person plans (Walks, rides, personTrips and personStops)
        PERSONTRIP =        1 << 14, // Person Trips
        WALK =              1 << 15, // Walks
        RIDE =              1 << 16, // Rides
        PERSONSTOP =        1 << 17, // Person stops
        // sub data elements
        GENERICDATA =       1 << 18, // Generic data (GNEEdgeData, GNELaneData...)
    };

    enum TagProperty {
        DRAWABLE =              1 << 0,     // Element can be drawed in view
        BLOCKMOVEMENT =         1 << 1,     // Element can block their movement
        BLOCKSHAPE =            1 << 2,     // Element can block their shape
        CLOSESHAPE =            1 << 3,     // Element can close their shape
        GEOPOSITION =           1 << 4,     // Element's position can be defined using a GEO position
        GEOSHAPE =              1 << 5,     // Element's shape acn be defined using a GEO Shape
        DIALOG =                1 << 6,     // Element can be edited using a dialog (GNECalibratorDialog, GNERerouterDialog...)
        PARENT =                1 << 7,     // Element will be writed in XML as child of another element (E3Entry -> E3Detector...)
        MINIMUMCHILDREN =       1 << 8,     // Element will be only writed in XML if has a minimum number of children
        REPARENT =              1 << 9,     // Element can be reparent
        SYNONYM =               1 << 10,    // Element will be written with a different name in der XML
        AUTOMATICSORTING =      1 << 11,    // Element sort automatic their Children (used by Additionals)
        SELECTABLE =            1 << 12,    // Element is selectable
        MASKSTARTENDPOS =       1 << 13,    // Element mask attributes StartPos and EndPos as "length" (Only used in the appropiate GNEFrame)
        MASKXYZPOSITION =       1 << 14,    // Element mask attributes X, Y and Z as "Position"
        WRITECHILDRENSEPARATE = 1 << 15,    // Element writes their children in a separated filename
        NOPARAMETERS =          1 << 16,    // Element doesn't accept parameters "key1=value1|key2=value2|...|keyN=valueN" (by default all tags supports parameters)
        PARAMETERSDOUBLE =      1 << 17,    // Element only accept double parameters "key1=double1|key2=double1|...|keyN=doubleN"
        RTREE =                 1 << 18,    // Element is placed in RTREE
        SORTINGCHILDREN =       1 << 19,    // Element can be sorted in their parent element manually (in ACHierarchy)
        CENTERAFTERCREATION =   1 << 20,    // Camera is moved after element creation
    };

    /// @brief default constructor
    GNETagProperties();

    /// @brief parameter constructor
    GNETagProperties(SumoXMLTag tag, int tagType, int tagProperty, GUIIcon icon, SumoXMLTag parentTag = SUMO_TAG_NOTHING, SumoXMLTag tagSynonym = SUMO_TAG_NOTHING);

    /// @brief destructor
    ~GNETagProperties();

    /// @brief get Tag vinculated with this attribute Property
    SumoXMLTag getTag() const;

    /// @brief get Tag vinculated with this attribute Property in String Format (used to avoid multiple calls to toString(...)
    const std::string& getTagStr() const;

    /// @brief check Tag integrity (this include all their attributes)
    void checkTagIntegrity() const;

    /// @brief add attribute (duplicated attributed aren't allowed)
    void addAttribute(const GNEAttributeProperties& attributeProperty);

    /// @brief add deprecated Attribute
    void addDeprecatedAttribute(SumoXMLAttr attr);

    /// @brief get attribute (throw error if doesn't exist)
    const GNEAttributeProperties& getAttributeProperties(SumoXMLAttr attr) const;

    /// @brief get begin of attribute values (used for iterate)
    std::vector<GNEAttributeProperties>::const_iterator begin() const;

    /// @brief get end of attribute values (used for iterate)
    std::vector<GNEAttributeProperties>::const_iterator end() const;

    /// @brief get number of attributes
    int getNumberOfAttributes() const;

    /// @brief return the default value of the attribute of an element
    const std::string& getDefaultValue(SumoXMLAttr attr) const;

    /// @brief get GUI icon associated to this Tag
    GUIIcon getGUIIcon() const;

    /// @brief if Tag owns a parent, return parent tag
    SumoXMLTag getParentTag() const;

    /// @brief get tag synonym
    SumoXMLTag getTagSynonym() const;

    /// @brief check if current TagProperties owns the attribute "attr"
    bool hasAttribute(SumoXMLAttr attr) const;

    /// @brief return true if tag correspond to a network element
    bool isNetworkElement() const;

    /// @brief return true if tag correspond to an additional element
    bool isAdditionalElement() const;

    /// @brief return true if tag correspond to a shape
    bool isShape() const;

    /// @brief return true if tag correspond to a TAZ
    bool isTAZ() const;

    /// @brief return true if tag correspond to a demand element
    bool isDemandElement() const;

    /// @brief return true if tag correspond to a data element
    bool isDataElement() const;

    /// @brief return true if tag correspond to a detector (Only used to group all stoppingPlaces in the output XML)
    bool isStoppingPlace() const;

    /// @brief return true if tag correspond to a shape (Only used to group all detectors in the XML)
    bool isDetector() const;

    /// @brief return true if tag correspond to a vehicle type element
    bool isVehicleType() const;

    /// @brief return true if tag correspond to a vehicle element
    bool isVehicle() const;

    /// @brief return true if tag correspond to a route element
    bool isRoute() const;

    /// @brief return true if tag correspond to a stop element
    bool isStop() const;

    /// @brief return true if tag correspond to a person element
    bool isPerson() const;

    /// @brief return true if tag correspond to a person plan
    bool isPersonPlan() const;

    /// @brief return true if tag correspond to a person trip
    bool isPersonTrip() const;

    /// @brief return true if tag correspond to a walk element
    bool isWalk() const;

    /// @brief return true if tag correspond to a ride element
    bool isRide() const;

    /// @brief return true if tag correspond to a person stop element
    bool isPersonStop() const;

    /// @brief return true if tag correspond to a generic data element
    bool isGenericData() const;

    /// @brief return true if tag correspond to a drawable element
    bool isDrawable() const;

    /// @brief return true if tag correspond to a selectable element
    bool isSelectable() const;

    /// @brief return true if tag correspond to an element that can block their movement
    bool canBlockMovement() const;

    /// @brief return true if tag correspond to an element that can block their shape
    bool canBlockShape() const;

    /// @brief return true if tag correspond to an element that can close their shape
    bool canCloseShape() const;

    /// @brief return true if tag correspond to an element that can use a geo position
    bool hasGEOPosition() const;

    /// @brief return true if tag correspond to an element that can use a geo shape
    bool hasGEOShape() const;

    /// @brief return true if tag correspond to an element that can had another element as parent
    bool hasParent() const;

    /// @brief return true if tag correspond to an element that will be written in XML with another tag
    bool hasTagSynonym() const;

    /// @brief return true if tag correspond to an element that can be edited using a dialog
    bool hasDialog() const;

    /// @brief return true if tag correspond to an element that only have a limited number of children
    bool hasMinimumNumberOfChildren() const;

    /// @brief return true if Tag correspond to an element that supports parameters "key1=value1|key2=value2|...|keyN=valueN"
    bool hasParameters() const;

    /// @brief return true if Tag correspond to an element that supports double parameters "key1=double1|key2=double2|...|keyN=doubleN"
    bool hasDoubleParameters() const;

    /// @brief return true if Tag correspond to an element that has has to be placed in RTREE
    bool isPlacedInRTree() const;

    /// @brief return true if Tag correspond to an element that can be sorted within their parent
    bool canBeSortedManually() const;

    /// @brief return true if tag correspond to an element that can be reparent
    bool canBeReparent() const;

    /// @brief return true if tag correspond to an element that can sort their children automatic
    bool canAutomaticSortChildren() const;

    /// @brief return true if tag correspond to an element that can sort their children automatic
    bool canWriteChildrenSeparate() const;

    /// @brief return true if tag correspond to an element that can mask the attributes "start" and "end" position as attribute "length"
    bool canMaskStartEndPos() const;

    /// @brief return true if tag correspond to an element that can mask the attributes "X", "Y" and "Z" position as attribute "Position"
    bool canMaskXYZPositions() const;

    /// @brief return true if tag correspond to an element that center camera after creation
    bool canCenterCameraAfterCreation() const;

    /// @brief return true if attribute of this tag is deprecated
    bool isAttributeDeprecated(SumoXMLAttr attr) const;

private:
    /// @brief Sumo XML Tag vinculated wit this tag Property
    SumoXMLTag myTag;

    /// @brief Sumo XML Tag vinculated wit this tag Property in String format
    std::string myTagStr;

    /// @brief Attribute Type
    int myTagType;

    /// @brief Attribute properties
    int myTagProperty;

    /// @brief vector with the attribute values vinculated with this Tag
    std::vector<GNEAttributeProperties> myAttributeProperties;

    /// @brief icon associated to this Tag
    GUIIcon myIcon;

    /// @brief parent tag
    SumoXMLTag myParentTag;

    /// @brief Tag written in XML (If is SUMO_TAG_NOTHING), original Tag name will be written)
    SumoXMLTag myTagSynonym;

    /// @brief List with the deprecated Attributes
    std::vector<SumoXMLAttr> myDeprecatedAttributes;

    /// @brief max number of attributes allowed for every tag
    static const size_t MAXNUMBEROFATTRIBUTES;
};

/****************************************************************************/

