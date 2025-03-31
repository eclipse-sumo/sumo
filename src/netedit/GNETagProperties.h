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
/// @file    GNETagProperties.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// Abstract Base class for tag properties used in GNEAttributeCarrier
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/gui/images/GUIIcons.h>
#include <netedit/GNEViewNetHelper.h>

#include "GNEAttributeProperties.h"

// ===========================================================================
// class definitions
// ===========================================================================

class GNETagProperties {

public:

    /// @brief tag types
    enum TagType {
        // basic types
        NETWORKELEMENT =    1 << 0,  // Network elements (Edges, Junctions, Lanes...)
        ADDITIONALELEMENT = 1 << 1,  // Additional elements (Bus Stops, Charging Stations, Detectors...)
        DEMANDELEMENT =     1 << 2,  // Demand elements (Routes, Vehicles, Trips...)
        DATAELEMENT =       1 << 3,  // Data elements (DataSets, Data Intervals, EdgeData...)
        // sub additional elements
        STOPPINGPLACE =     1 << 4,  // StoppingPlaces (BusStops, ChargingStations...)
        DETECTOR =          1 << 5,  // Detectors (E1, E2...)
        CALIBRATOR =        1 << 6,  // Calibrators
        SHAPE =             1 << 7,  // Shapes (Polygons and POIs)
        TAZELEMENT =        1 << 8,  // Traffic Assignment Zones
        WIRE =              1 << 9,  // Wire elements
        JUPEDSIM =          1 << 10, // JuPedSim elements
        // sub demand elements
        VTYPE =             1 << 11, // Vehicle types (vType and vTypeDistribution)
        VEHICLE =           1 << 12, // Vehicles (Vehicles, trips, flows...)
        ROUTE =             1 << 13, // Routes and embedded routes
        STOP_VEHICLE =      1 << 14, // Vehicle stops
        WAYPOINT_VEHICLE =  1 << 15, // Vehicle waypoints (note: All waypoints are also Stops)
        FLOW =              1 << 16, // Flows
        // persons
        PERSON =            1 << 17, // Persons (Persons and personFlows)
        PERSONPLAN =        1 << 18, // Person plans (Walks, rides, personTrips and stopPersons)
        PERSONTRIP =        1 << 19, // Person Trips
        WALK =              1 << 20, // Walks
        RIDE =              1 << 21, // Rides
        STOP_PERSON =       1 << 22, // Person stops
        // containers
        CONTAINER =         1 << 23, // Containers (Containers and personFlows)
        CONTAINERPLAN =     1 << 24, // Container plans (transport, tranships and containerStops)
        TRANSPORT =         1 << 25, // Transport
        TRANSHIP =          1 << 26, // Tranship
        STOP_CONTAINER =    1 << 27, // Container stops
        // sub data elements
        GENERICDATA =       1 << 28, // Generic data (GNEEdgeData, GNELaneData...)
        MEANDATA =          1 << 29, // Mean datas
        // other
        INTERNALLANE =      1 << 30, // Internal Lane
        OTHER =             1 << 31, // Other type (used for TAZSourceSinks, VTypes, etc.)
    };

    /// @brief general tag properties
    enum TagProperty {
        NO_PROPERTY =           1 << 0,     // Element doesn't have properties
        NOTDRAWABLE =           1 << 1,     // Element cannot be drawn in view
        GEOSHAPE =              1 << 2,     // Element's shape acn be defined using a GEO Shape
        DIALOG =                1 << 3,     // Element can be edited using a dialog (GNECalibratorDialog, GNERerouterDialog...)
        CHILD =                 1 << 4,     // Element is child of another element and will be written in XML without id (Example: E3Entry -> E3Detector...)
        REPARENT =              1 << 5,     // Element can be reparent
        NOTSELECTABLE =         1 << 6,     // Element cannot be selected
        NOPARAMETERS =          1 << 7,     // Element doesn't accept parameters "key1=value1|key2=value2|...|keyN=valueN" (by default all tags supports parameters)
        RTREE =                 1 << 8,     // Element is placed in RTREE
        CENTERAFTERCREATION =   1 << 9,     // Camera is moved after element creation
        REQUIRE_PROJ =          1 << 10,    // Element require a geo-projection defined in network
        VCLASS_ICON =           1 << 11,    // Element returns icon depending of their vClass
        SYMBOL =                1 << 12,    // Element is a symbol (VSSSymbols, RerouterSymbols...)
        EXTENDED =              1 << 13,    // Element contains extended attributes (Usually vTypes)
        HIERARCHICAL =          1 << 14,    // Element is a hierarchical
    };

    /// @brief tag parents
    enum TagParents {
        NO_PARENTS =                1 << 0,     // No parents
        // exclusive of vehicles
        VEHICLE_ROUTE =             1 << 1,     // Vehicle is placed over route
        VEHICLE_ROUTE_EMBEDDED =    1 << 2,     // Vehicle has an embedded route
        VEHICLE_EDGES =             1 << 3,     // Vehicle is placed over a from-to edges
        VEHICLE_JUNCTIONS =         1 << 4,     // Vehicle is placed over a from-to junctions
        VEHICLE_TAZS =              1 << 5,     // Vehicle is placed over a from-to TAZs
        // exclusive of plans
        PLAN_CONSECUTIVE_EDGES =    1 << 6,     // Plan placed in consecutive edges
        PLAN_ROUTE =                1 << 7,     // Plan placed in route
        PLAN_EDGE =                 1 << 8,     // Plan placed in single edge
        PLAN_BUSSTOP =              1 << 9,     // Plan placed in single busStop
        PLAN_TRAINSTOP =            1 << 10,    // Plan placed in single trainStop
        PLAN_CONTAINERSTOP =        1 << 11,    // Plan placed in single containerStop
        PLAN_CHARGINGSTATION =      1 << 12,    // Plan placed in single charging station
        PLAN_PARKINGAREA =          1 << 13,    // Plan placed in single parking area
        PLAN_FROM_EDGE =            1 << 14,    // Plan starts in edge
        PLAN_FROM_TAZ =             1 << 15,    // Plan starts in TAZ
        PLAN_FROM_JUNCTION =        1 << 16,    // Plan starts in junction
        PLAN_FROM_BUSSTOP =         1 << 17,    // Plan starts in busStop
        PLAN_FROM_TRAINSTOP =       1 << 18,    // Plan starts in trainStop
        PLAN_FROM_CONTAINERSTOP =   1 << 19,    // Plan starts in containerStop
        PLAN_FROM_CHARGINGSTATION = 1 << 20,    // Plan starts in chargingStation
        PLAN_FROM_PARKINGAREA =     1 << 21,    // Plan starts in parkingArea
        PLAN_TO_EDGE =              1 << 22,    // Plan ends in edge
        PLAN_TO_TAZ =               1 << 23,    // Plan ends in TAZ
        PLAN_TO_JUNCTION =          1 << 24,    // Plan ends in junction
        PLAN_TO_BUSSTOP =           1 << 25,    // Plan ends in busStop
        PLAN_TO_TRAINSTOP =         1 << 26,    // Plan ends in trainStop
        PLAN_TO_CONTAINERSTOP =     1 << 27,    // Plan ends in containerStop
        PLAN_TO_CHARGINGSTATION =   1 << 28,    // Plan ends in chargingStation
        PLAN_TO_PARKINGAREA =       1 << 29,    // Plan ends in parkingArea
    };

    // @brief conflicts
    enum Conflicts {
        NO_CONFLICTS =              1 << 0,     // Element doesn't have conflicts
        POS_LANE =                  1 << 1,     // Position over lane isn't valid
        POS_LANE_START =            1 << 2,     // Start position over lane isn't valid
        POS_LANE_END =              1 << 3,     // End position over lane isn't valid
        NO_ADDITIONAL_CHILDREN =    1 << 4,     // Element doesn't have additional children
    };

    /// @brief declare friend class
    friend class GNEAttributeProperties;

    /// @brief parameter constructor
    GNETagProperties(const SumoXMLTag tag, GNETagProperties* parent, const int tagType, const int tagProperty, const int tagParents, const int conflicts,
                     const GUIIcon icon, const SumoXMLTag XMLTag, const std::string tooltip, std::vector<SumoXMLTag> XMLParentTags = {},
                     const unsigned int backgroundColor = FXRGBA(255, 255, 255, 255), const std::string selectorText = "");

    /// @brief parameter constructor for hierarchical elements
    GNETagProperties(const SumoXMLTag tag, GNETagProperties* parent, const GUIIcon icon, const std::string tooltip,
                     const unsigned int backgroundColor = FXRGBA(255, 255, 255, 255), const std::string selectorText = "");

    /// @brief destructor
    ~GNETagProperties();

    /// @brief get Tag vinculated with this attribute Property
    SumoXMLTag getTag() const;

    /// @brief get Tag vinculated with this attribute Property in String Format (used to avoid multiple calls to toString(...)
    const std::string& getTagStr() const;

    /// @brief check Tag integrity (this include all their attributes)
    void checkTagIntegrity() const;

    /// @brief get field string (by default tag in string format)
    const std::string& getSelectorText() const;

    /// @brief get tooltip text
    const std::string& getTooltipText() const;

    /// @brief get background color
    unsigned int getBackGroundColor() const;

    /// @brief get all attribute properties
    const std::vector<const GNEAttributeProperties*>& getAttributeProperties() const;

    /// @brief get attribute propety associated with the given Sumo XML Attribute (throw error if doesn't exist)
    const GNEAttributeProperties* getAttributeProperties(SumoXMLAttr attr) const;

    /// @brief get attribute propety by index (throw error if doesn't exist)
    const GNEAttributeProperties* getAttributeProperties(const int index) const;

    /// @brief get attribute value
    const GNEAttributeProperties* at(int index) const;

    /// @brief check if current TagProperties owns the attribute "attr"
    bool hasAttribute(SumoXMLAttr attr) const;

    /// @brief get number of attributes
    int getNumberOfAttributes() const;

    /// @brief get GUI icon associated to this Tag
    GUIIcon getGUIIcon() const;

    /// @brief get XML tag
    SumoXMLTag getXMLTag() const;

    /// @brief get XML parent tags
    const std::vector<SumoXMLTag>& getXMLParentTags() const;

    /// @brief default values
    /// @{

    /// @brief return the default value of the attribute of an element
    const std::string& getDefaultStringValue(SumoXMLAttr attr) const;

    /// @brief get default int value
    int getDefaultIntValue(SumoXMLAttr attr) const;

    /// @brief get default double value
    double getDefaultDoubleValue(SumoXMLAttr attr) const;

    /// @brief get default time value
    SUMOTime getDefaultTimeValue(SumoXMLAttr attr) const;

    /// @brief get default bool value
    bool getDefaultBoolValue(SumoXMLAttr attr) const;

    /// @brief get default bool value
    const RGBColor& getDefaultColorValue(SumoXMLAttr attr) const;

    /// @}

    /// @brief hierarchy functions
    /// @{

    /// @brief get parent of this element
    const GNETagProperties* getParent() const;

    /// @brief get all parents, beginning from current element (root not included) untils this element
    const std::vector<const GNETagProperties*> getParentHierarchy() const;

    /// @brief get children of this tag property
    const std::vector<const GNETagProperties*>& getTagChildren() const;

    /// @brief get all children tags (Including children of their children)
    std::vector<const GNETagProperties*> getTagChildrenRecursively() const;

    /// @brief get all children attributes sorted by name (Including this)
    std::map<std::string, const GNEAttributeProperties*> getAttributeChildrenRecursively(const bool onlyCommon, const bool onlyDrawables) const;

    /// @brief get supermode associated with this tag
    Supermode getSupermode() const;

    /// @}

    /// @brief check if this is a hirearchical tag
    bool isHierarchicalTag() const;

    /// @brief network elements
    /// @{

    /// @brief return true if tag correspond to a network element
    bool isNetworkElement() const;

    /// @brief return true if tag correspond to an additional element (note: this include TAZ, shapes and wires)
    bool isAdditionalElement() const;

    /// @brief return true if tag correspond to a pure additional element
    bool isAdditionalPureElement() const;

    /// @brief return true if tag correspond to a demand element
    bool isDemandElement() const;

    /// @brief return true if tag correspond to a data element
    bool isDataElement() const;

    /// @brief return true if tag correspond to a other element (sourceSinks, vTypes, etc.)
    bool isOtherElement() const;

    /// @}

    /// @brief additional elements
    /// @{
    /// @brief return true if tag correspond to a detector (Only used to group all stoppingPlaces in the output XML)
    bool isStoppingPlace() const;

    /// @brief return true if tag correspond to a shape (Only used to group all detectors in the XML)
    bool isDetector() const;

    /// @brief return true if tag correspond to a calibrator (Only used to group all detectors in the XML)
    bool isCalibrator() const;

    /// @brief return true if tag correspond to a shape
    bool isShapeElement() const;

    /// @brief return true if tag correspond to a TAZ element
    bool isTAZElement() const;

    /// @brief return true if tag correspond to a Wire element
    bool isWireElement() const;

    /// @brief return true if tag correspond to a JuPedSim element
    bool isJuPedSimElement() const;

    /// @}

    /// @brief demand elements
    /// @{
    /// @brief return true if tag correspond to a vehicle/person/container type element
    bool isType() const;

    /// @brief return true if tag correspond to a type distribution element
    bool isTypeDist() const;

    /// @brief return true if tag correspond to a vehicle element
    bool isVehicle() const;

    /// @brief return true if tag correspond to a route element
    bool isRoute() const;

    /// @brief return true if tag correspond to a vehicle stop element
    bool isVehicleStop() const;

    /// @brief return true if tag correspond to a vehicle waypoint element
    bool isVehicleWaypoint() const;

    /// @brief return true if tag correspond to a flow element
    bool isFlow() const;

    /// @brief return true if tag correspond to a person element
    bool isPerson() const;

    /// @brief return true if tag correspond to a container element
    bool isContainer() const;

    /// @brief return true if tag correspond to an element with a type as a first parent
    bool hasTypeParent() const;

    /// @}

    /// @brief plans
    /// @{
    /// @brief return true if tag correspond to a plan
    bool isPlan() const;

    /// @brief return true if tag correspond to a person plan
    bool isPlanPerson() const;

    /// @brief return true if tag correspond to a container plan
    bool isPlanContainer() const;

    /// @brief return true if tag correspond to a person trip plan
    bool isPlanPersonTrip() const;

    /// @brief return true if tag correspond to a walk plan
    bool isPlanWalk() const;

    /// @brief return true if tag correspond to a ride plan
    bool isPlanRide() const;

    /// @brief return true if tag correspond to a transport
    bool isPlanTransport() const;

    /// @brief return true if tag correspond to a tranship
    bool isPlanTranship() const;

    /// @brief return true if tag correspond to a stop plan
    bool isPlanStop() const;

    /// @brief return true if tag correspond to a person stop plan
    bool isPlanStopPerson() const;

    /// @brief return true if tag correspond to a container stop plan
    bool isPlanStopContainer() const;

    /// @}

    /// @brief data elements
    /// @{
    /// @brief return true if tag correspond to a generic data element
    bool isGenericData() const;

    /// @brief return true if tag correspond to a mean data element
    bool isMeanData() const;

    /// @}

    /// @brief plan parents
    /// @{

    /// @brief return true if tag correspond to a vehicle placed over a route
    bool vehicleRoute() const;

    /// @brief return true if tag correspond to a vehicle placed over an embedded route
    bool vehicleRouteEmbedded() const;

    /// @brief return true if tag correspond to a vehicle placed over from-to edges
    bool vehicleEdges() const;

    /// @brief return true if tag correspond to a vehicle placed over from-to junctions
    bool vehicleJunctions() const;

    /// @brief return true if tag correspond to a vehicle placed over from-to TAZs
    bool vehicleTAZs() const;

    /// @}

    /// @brief plan parents
    /// @{
    /// @brief return true if tag correspond to a plan placed over edges
    bool planConsecutiveEdges() const;

    /// @brief return true if tag correspond to a plan placed over route
    bool planRoute() const;

    /// @brief return true if tag correspond to a plan placed over edge
    bool planEdge() const;

    /// @brief return true if tag correspond to a plan placed over busStop
    bool planBusStop() const;

    /// @brief return true if tag correspond to a plan placed over trainStop
    bool planTrainStop() const;

    /// @brief return true if tag correspond to a plan placed over containerStop
    bool planContainerStop() const;

    /// @brief return true if tag correspond to a plan placed over chargingStation
    bool planChargingStation() const;

    /// @brief return true if tag correspond to a plan placed over parkingArea
    bool planParkingArea() const;

    /// @brief return true if tag correspond to a plan placed in stoppingPlace
    bool planStoppingPlace() const;

    /// @brief return true if tag correspond to a plan with from-to parents
    bool planFromTo() const;

    /// @brief return true if tag correspond to a plan that starts in edge
    bool planFromEdge() const;

    /// @brief return true if tag correspond to a plan that starts in TAZ
    bool planFromTAZ() const;

    /// @brief return true if tag correspond to a plan that starts in junction
    bool planFromJunction() const;

    /// @brief return true if tag correspond to a plan that starts in busStop
    bool planFromBusStop() const;

    /// @brief return true if tag correspond to a plan that starts in trainStop
    bool planFromTrainStop() const;

    /// @brief return true if tag correspond to a plan that starts in containerStop
    bool planFromContainerStop() const;

    /// @brief return true if tag correspond to a plan that starts in chargingStation
    bool planFromChargingStation() const;

    /// @brief return true if tag correspond to a plan that starts in parkingAera
    bool planFromParkingArea() const;

    /// @brief return true if tag correspond to a plan that starts in stoppingPlace
    bool planFromStoppingPlace() const;

    /// @brief return true if tag correspond to a plan that starts in edge
    bool planToEdge() const;

    /// @brief return true if tag correspond to a plan that starts in TAZ
    bool planToTAZ() const;

    /// @brief return true if tag correspond to a plan that starts in junction
    bool planToJunction() const;

    /// @brief return true if tag correspond to a plan that starts in busStop
    bool planToBusStop() const;

    /// @brief return true if tag correspond to a plan that starts in trainStop
    bool planToTrainStop() const;

    /// @brief return true if tag correspond to a plan that starts in containerStop
    bool planToContainerStop() const;

    /// @brief return true if tag correspond to a plan that starts in chargingStation
    bool planToChargingStation() const;

    /// @brief return true if tag correspond to a plan that starts in parkingArea
    bool planToParkingArea() const;

    /// @brief return true if tag correspond to a plan that ends in stoppingPlace
    bool planToStoppingPlace() const;

    /// @}

    /// @brief return true if tag correspond to an element child of another element (Example: E3->Entry/Exit)
    bool isChild() const;

    /// @brief return true if tag correspond to a symbol element
    bool isSymbol() const;

    /// @brief return true if tag correspond to an internal lane
    bool isInternalLane() const;

    /// @brief return true if tag correspond to a drawable element
    bool isDrawable() const;

    /// @brief return true if tag correspond to a selectable element
    bool isSelectable() const;

    /// @brief return true if tag correspond to an element that can use a geo shape
    bool hasGEOShape() const;

    /// @brief return true if tag correspond to an element that can be edited using a dialog
    bool hasDialog() const;

    /// @brief return true if tag correspond to an element that contains extended attributes
    bool hasExtendedAttributes() const;

    /// @brief return true if Tag correspond to an element that supports parameters "key1=value1|key2=value2|...|keyN=valueN"
    bool hasParameters() const;

    /// @brief return true if Tag correspond to an element that has to be placed in RTREE
    bool isPlacedInRTree() const;

    /// @brief return true if tag correspond to an element that can be reparent
    bool canBeReparent() const;

    /// @brief return true if tag correspond to an element that center camera after creation
    bool canCenterCameraAfterCreation() const;

    /// @brief return true if tag correspond to an element that requires a geo projection
    bool requireProj() const;

    /// @brief return true if tag correspond to an element that has vClass icons
    bool vClassIcon() const;

protected:
    /// @brief add child
    void addChild(const GNETagProperties* child);

private:
    /// @brief Sumo XML Tag vinculated wit this tag Property
    const SumoXMLTag myTag = SUMO_TAG_NOTHING;

    /// @brief Sumo XML Tag vinculated wit this tag Property in String format
    const std::string myTagStr;

    /// @brief tag property parent
    const GNETagProperties* myParent = nullptr;

    /// @brief tag property children
    std::vector<const GNETagProperties*> myChildren;

    /// @brief tag Types
    const int myTagType = -1;

    /// @brief tag properties
    const int myTagProperty = -1;

    /// @brief tag parents
    const int myTagParents = -1;

    /// @brief conflicts
    const int myConflicts = -1;

    /// @brief vector with the attribute values vinculated with this Tag
    std::vector<const GNEAttributeProperties*> myAttributeProperties;

    /// @brief icon associated to this Tag
    const GUIIcon myIcon = GUIIcon::EMPTY;

    /// @brief Tag written in XML and used in GNENetHelper::AttributeCarriers
    const SumoXMLTag myXMLTag = SUMO_TAG_NOTHING;

    /// @brief tooltip text
    const std::string myTooltipText;

    /// @brief vector with XML parent tags (used by child elements like access or spaces)
    const std::vector<SumoXMLTag> myXMLParentTags;

    /// @brief text show in selector text
    const std::string mySelectorText;

    /// @brief background color (used in labels and textFields, by default white)
    const unsigned int myBackgroundColor = 0;

    /// @brief recursive function for get all children tag properites (Including this)
    void getChildrenTagProperties(const GNETagProperties* tagProperties, std::vector<const GNETagProperties*>& result) const;

    /// @brief recursive function for get all children attributes (Including this)
    void getChildrenAttributes(const GNETagProperties* tagProperties, std::map<std::string, const GNEAttributeProperties*>& result, const bool onlyDrawables) const;

    /// @brief default constructor
    GNETagProperties() = delete;

    /// @brief Invalidated copy constructor.
    GNETagProperties(const GNETagProperties&) = delete;

    /// @brief Invalidated assignment operator
    GNETagProperties& operator=(const GNETagProperties& src) = delete;
};

/****************************************************************************/
