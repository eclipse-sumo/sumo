/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
#include <config.h>


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
        SHAPE =             1 << 2,  // Shapes (Polygons and POIs)
        TAZELEMENT =        1 << 3,  // Traffic Assignment Zones
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
        // persons
        PERSON =            1 << 12, // Persons (Persons and personFlows)
        PERSONPLAN =        1 << 13, // Person plans (Walks, rides, personTrips and personStops)
        PERSONTRIP =        1 << 14, // Person Trips
        WALK =              1 << 15, // Walks
        RIDE =              1 << 16, // Rides
        PERSONSTOP =        1 << 17, // Person stops
        // containers
        CONTAINER =         1 << 18, // Containers (Containers and personFlows)
        CONTAINERPLAN =     1 << 19, // Container plans (tranship and transport)
        TRANSPORT =         1 << 20, // Transport
        TRANSHIP =          1 << 21, // Tranship
        CONTAINERSTOP =     1 << 22, // Container stops
        // sub data elements
        GENERICDATA =       1 << 23, // Generic data (GNEEdgeData, GNELaneData...)
        // other
        SYMBOL =            1 << 24, // Symbol elements (VSSSymbols, RerouterSymbols...)
        INTERNALLANE =      1 << 25, // Internal Lane
    };

    enum TagProperty {
        DRAWABLE =                  1 << 0,     // Element can be drawed in view
        BLOCKMOVEMENT =             1 << 1,     // Element can block their movement
        CLOSESHAPE =                1 << 2,     // Element can close their shape
        GEOSHAPE =                  1 << 3,     // Element's shape acn be defined using a GEO Shape
        DIALOG =                    1 << 4,     // Element can be edited using a dialog (GNECalibratorDialog, GNERerouterDialog...)
        SLAVE =                     1 << 5,     // Element is slave and will be writed in XML without id as child of another element (E3Entry -> E3Detector...)
        MINIMUMCHILDREN =           1 << 6,     // Element will be only writed in XML if has a minimum number of children
        REPARENT =                  1 << 7,     // Element can be reparent
        SELECTABLE =                1 << 8,     // Element is selectable
        MASKSTARTENDPOS =           1 << 9,    // Element mask attributes StartPos and EndPos as "length" (Only used in the appropiate GNEFrame)
        MASKXYZPOSITION =           1 << 10,    // Element mask attributes X, Y and Z as "Position"
        WRITECHILDRENSEPARATE =     1 << 11,    // Element writes their children in a separated filename
        NOPARAMETERS =              1 << 12,    // Element doesn't accept parameters "key1=value1|key2=value2|...|keyN=valueN" (by default all tags supports parameters)
        PARAMETERSDOUBLE =          1 << 13,    // Element only accept double parameters "key1=double1|key2=double1|...|keyN=doubleN"
        RTREE =                     1 << 14,    // Element is placed in RTREE
        CENTERAFTERCREATION =       1 << 15,    // Camera is moved after element creation
        EMBEDDED_ROUTE =            1 << 16,    // Element has an embedded route
    };

    /// @brief default constructor
    GNETagProperties();

    /// @brief parameter constructor
    GNETagProperties(const SumoXMLTag tag, int tagType, int tagProperty, GUIIcon icon, const SumoXMLTag XMLTag, const std::vector<SumoXMLTag>& masterTags = {});

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

    /// @brief get attribute value
    const GNEAttributeProperties& at(int index) const;

    /// @brief get number of attributes
    int getNumberOfAttributes() const;

    /// @brief return the default value of the attribute of an element
    const std::string& getDefaultValue(SumoXMLAttr attr) const;

    /// @brief get GUI icon associated to this Tag
    GUIIcon getGUIIcon() const;

    /// @brief get XML tag
    SumoXMLTag getXMLTag() const;

    /// @brief get master tags
    const std::vector<SumoXMLTag>& getMasterTags() const;

    /// @brief check if current TagProperties owns the attribute "attr"
    bool hasAttribute(SumoXMLAttr attr) const;

    /// @brief return true if tag correspond to a network element
    bool isNetworkElement() const;

    /// @brief return true if tag correspond to an additional element
    bool isAdditionalElement() const;

    /// @brief return true if tag correspond to a shape
    bool isShape() const;

    /// @brief return true if tag correspond to a TAZ element
    bool isTAZElement() const;

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

    /// @brief return true if tag correspond to a container element
    bool isContainer() const;

    /// @brief return true if tag correspond to a container plan
    bool isContainerPlan() const;

    /// @brief return true if tag correspond to a transport
    bool isTransportPlan() const;

    /// @brief return true if tag correspond to a tranship
    bool isTranshipPlan() const;

    /// @brief return true if tag correspond to a container stop element
    bool isContainerStop() const;

    /// @brief return true if tag correspond to a generic data element
    bool isGenericData() const;

    /// @brief return true if tag correspond to an element slave of another element (I.e. doesn't have their own ID)
    bool isSlave() const;

    /// @brief return true if tag correspond to a symbol element
    bool isSymbol() const;

    /// @brief return true if tag correspond to a internal lane
    bool isInternalLane() const;

    /// @brief return true if tag correspond to a drawable element
    bool isDrawable() const;

    /// @brief return true if tag correspond to a selectable element
    bool isSelectable() const;

    /// @brief return true if tag correspond to an element that can block their movement
    bool canBlockMovement() const;

    /// @brief return true if tag correspond to an element that can close their shape
    bool canCloseShape() const;

    /// @brief return true if tag correspond to an element that can use a geo shape
    bool hasGEOShape() const;

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

    /// @brief return true if tag correspond to an element that can be reparent
    bool canBeReparent() const;

    /// @brief return true if tag correspond to an element that can write their child in a different file
    bool canWriteChildrenSeparate() const;

    /// @brief return true if tag correspond to an element that can mask the attributes "start" and "end" position as attribute "length"
    bool canMaskStartEndPos() const;

    /// @brief return true if tag correspond to an element that can mask the attributes "X", "Y" and "Z" position as attribute "Position"
    bool canMaskXYZPositions() const;

    /// @brief return true if tag correspond to an element that center camera after creation
    bool canCenterCameraAfterCreation() const;

    /// @brief return true if tag correspond to an element that owns a embebbed route
    bool embebbedRoute() const;

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

    /// @brief Tag written in XML and used in GNENetHelper::AttributeCarriers
    SumoXMLTag myXMLTag;

    /// @brief vector with master tags (used by slave elements)
    std::vector<SumoXMLTag> myMasterTags;

    /// @brief List with the deprecated Attributes
    std::vector<SumoXMLAttr> myDeprecatedAttributes;

    /// @brief max number of attributes allowed for every tag
    static const size_t MAXNUMBEROFATTRIBUTES;
};

/****************************************************************************/

