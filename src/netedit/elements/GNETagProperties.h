/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEAttributeProperties.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// Abstract Base class for tag properties used in GNEAttributeCarrier
/****************************************************************************/
#ifndef GNETagProperties_h
#define GNETagProperties_h


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
        TAGTYPE_NETWORKELEMENT =    1 << 0, // Edges, Junctions, Lanes...
        TAGTYPE_ADDITIONALELEMENT = 1 << 1,  // Bus Stops, Charging Stations, Detectors...
        TAGTYPE_SHAPE =             1 << 2,  // POIs, Polygons
        TAGTYPE_DEMANDELEMENT =     1 << 3,  // Routes, Vehicles, Trips...
        TAGTYPE_DATAELEMENT =       1 << 4,  // EdgeData, LaneData...
        TAGTYPE_TAZ =               1 << 5,  // Traffic Assignment Zones
        TAGTYPE_STOPPINGPLACE =     1 << 6,  // StoppingPlaces (BusStops, ChargingStations...)
        TAGTYPE_DETECTOR =          1 << 7,  // Detectors (E1, E2...)
        TAGTYPE_VTYPE =             1 << 8,  // Vehicle types (vType and pTye)
        TAGTYPE_VEHICLE =           1 << 9,  // Vehicles (Flows, trips...)
        TAGTYPE_ROUTE =             1 << 10, // Routes and embedded routes
        TAGTYPE_STOP =              1 << 11, // Stops
        TAGTYPE_PERSON =            1 << 12, // Persons
        TAGTYPE_PERSONPLAN =        1 << 13, // Person plans (Walks, rides, ...)
        TAGTYPE_PERSONTRIP =        1 << 14, // Walks
        TAGTYPE_WALK =              1 << 15, // Walks
        TAGTYPE_RIDE =              1 << 16, // Rides
        TAGTYPE_PERSONSTOP =        1 << 17, // Person stops
    };

    enum TagProperty {
        TAGPROPERTY_DRAWABLE =              1 << 0,     // Element can be drawed in view
        TAGPROPERTY_BLOCKMOVEMENT =         1 << 1,     // Element can block their movement
        TAGPROPERTY_BLOCKSHAPE =            1 << 2,     // Element can block their shape
        TAGPROPERTY_CLOSESHAPE =            1 << 3,     // Element can close their shape
        TAGPROPERTY_GEOPOSITION =           1 << 4,     // Element's position can be defined using a GEO position
        TAGPROPERTY_GEOSHAPE =              1 << 5,     // Element's shape acn be defined using a GEO Shape
        TAGPROPERTY_DIALOG =                1 << 6,     // Element can be edited using a dialog (GNECalibratorDialog, GNERerouterDialog...)
        TAGPROPERTY_PARENT =                1 << 7,     // Element will be writed in XML as child of another element (E3Entry -> E3Detector...)
        TAGPROPERTY_MINIMUMCHILDREN =       1 << 8,     // Element will be only writed in XML if has a minimum number of children
        TAGPROPERTY_REPARENT =              1 << 9,     // Element can be reparent
        TAGPROPERTY_SYNONYM =               1 << 10,    // Element will be written with a different name in der XML
        TAGPROPERTY_AUTOMATICSORTING =      1 << 11,    // Element sort automatic their Children (used by Additionals)
        TAGPROPERTY_SELECTABLE =            1 << 12,    // Element is selectable
        TAGPROPERTY_MASKSTARTENDPOS =       1 << 13,    // Element mask attributes StartPos and EndPos as "length" (Only used in the appropiate GNEFrame)
        TAGPROPERTY_MASKXYZPOSITION =       1 << 14,    // Element mask attributes X, Y and Z as "Position"
        TAGPROPERTY_WRITECHILDRENSEPARATE = 1 << 15,    // Element writes their children in a separated filename
        TAGPROPERTY_NOPARAMETERS =          1 << 16,    // Element doesn't accept parameters "key1=value1|key2=value2|...|keyN=valueN" (by default all tags supports parameters)
        TAGPROPERTY_RTREE =                 1 << 17,    // Element is placed in RTREE
        TAGPROPERTY_SORTINGCHILDREN =       1 << 18,    // Element can be sorted in their parent element manually (in ACHierarchy)
        TAGPROPERTY_CENTERAFTERCREATION =   1 << 19,    // Camera is moved after element creation
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

#endif

/****************************************************************************/

