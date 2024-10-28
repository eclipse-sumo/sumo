/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GUIVisualizationSettings.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// Stores the information about how to visualize structures
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <vector>
#include <map>
#include <utils/common/RGBColor.h>
#include <utils/common/ToString.h>

#include "GUIPropertySchemeStorage.h"


// ===========================================================================
// class declarations
// ===========================================================================
class BaseSchemeInfoSource;
class OutputDevice;
class GUIVisualizationSettings;
class GUIGlObject;


// ===========================================================================
// class definitions
// ===========================================================================

// cannot declare this as inner class because it needs to be used in forward
// declaration (@todo fix inclusion order by removing references to guisim!)
struct GUIVisualizationTextSettings {

    /// @brief constructor
    GUIVisualizationTextSettings(bool _show, double _size, RGBColor _color, RGBColor _bgColor = RGBColor(128, 0, 0, 0), bool _constSize = true, bool _onlySelected = false);

    /// @brief equality comparator
    bool operator==(const GUIVisualizationTextSettings& other);

    /// @brief inequality comparator
    bool operator!=(const GUIVisualizationTextSettings& other);

    /// @brief print values in output device
    void print(OutputDevice& dev, const std::string& name) const;

    /// @brief get scale size
    double scaledSize(double scale, double constFactor = 0.1) const;

    /// @brief whether to show the text
    bool show(const GUIGlObject* o) const;

    /// @brief flag show
    bool showText;

    /// @brief text size
    double size;

    /// @brief text color
    RGBColor color;

    /// @brief background text color
    RGBColor bgColor;

    /// @brif flag to avoid size changes
    bool constSize;

    /// @brief whether only selected objects shall have text drawn
    bool onlySelected;
};


struct GUIVisualizationRainbowSettings {

    /// @brief constructor
    GUIVisualizationRainbowSettings(bool _hideMin, double _minThreshold, bool _hideMax, double _maxThreshold, bool _setNeutral, double _neutralThreshold, bool _fixRange);

    /// @brief equality comparator
    bool operator==(const GUIVisualizationRainbowSettings& other);

    /// @brief inequality comparator
    bool operator!=(const GUIVisualizationRainbowSettings& other);

    /// @brief print values in output device
    void print(OutputDevice& dev, const std::string& name) const;

    /// @brief whether data below threshold should not be colored
    bool hideMin;
    /// @brief threshold below which value should not be colored
    double minThreshold;
    /// @brief whether data above threshold should not be colored
    bool hideMax;
    /// @brief threshold above which value should not be colored
    double maxThreshold;
    /// @brief whether the scale should be centered at a specific value
    bool setNeutral;
    /// @brief neutral point of scale
    double neutralThreshold;
    /// @brief whether the color scale should be fixed to the given min/max values
    bool fixRange;
    /// @brief color steps for the rainbow;
    std::vector<RGBColor> colors;
};


/// @brief struct for size settings
struct GUIVisualizationSizeSettings {

    /// @brief constructor
    GUIVisualizationSizeSettings(double _minSize, double _exaggeration = 1.0, bool _constantSize = false, bool _constantSizeSelected = false);

    /// @brief return the drawing size including exaggeration and constantSize values
    double getExaggeration(const GUIVisualizationSettings& s, const GUIGlObject* o, double factor = 20) const;

    /// @brief equality comparator
    bool operator==(const GUIVisualizationSizeSettings& other);

    /// @brief inequality comparator
    bool operator!=(const GUIVisualizationSizeSettings& other);

    /// @brief print values in output device
    void print(OutputDevice& dev, const std::string& name) const;

    /// @brief The minimum size to draw this object
    double minSize;

    /// @brief The size exaggeration (upscale)
    double exaggeration;

    /// @brief whether the object shall be drawn with constant size regardless of zoom
    bool constantSize;

    /// @brief whether only selected objects shall be drawn with constant
    bool constantSizeSelected;
};


/// @brief struct for color settings
struct GUIVisualizationColorSettings {

    /// @brief constructor
    GUIVisualizationColorSettings();

    /// @brief equality comparator
    bool operator==(const GUIVisualizationColorSettings& other);

    /// @brief inequality comparator
    bool operator!=(const GUIVisualizationColorSettings& other);

    /// @brief basic selection color
    RGBColor selectionColor;

    /// @brief edge selection color
    RGBColor selectedEdgeColor;

    /// @brief lane selection color
    RGBColor selectedLaneColor;

    /// @brief connection selection color
    RGBColor selectedConnectionColor;

    /// @brief prohibition selection color
    RGBColor selectedProhibitionColor;

    /// @brief crossings selection color
    RGBColor selectedCrossingColor;

    /// @brief additional selection color (busStops, Detectors...)
    RGBColor selectedAdditionalColor;

    /// @brief route selection color (used for routes and vehicle stops)
    RGBColor selectedRouteColor;

    /// @brief vehicle selection color
    RGBColor selectedVehicleColor;

    /// @brief person selection color
    RGBColor selectedPersonColor;

    /// @brief person plan selection color (Rides, Walks, stopPersons...)
    RGBColor selectedPersonPlanColor;

    /// @brief container selection color
    RGBColor selectedContainerColor;

    /// @brief container plan selection color (Rides, Walks, containerStops...)
    RGBColor selectedContainerPlanColor;

    /// @brief edge data selection color
    RGBColor selectedEdgeDataColor;

    /// @brief color for busStops
    RGBColor busStopColor;

    /// @brief color for busStops signs
    RGBColor busStopColorSign;

    /// @brief color for trainStops
    RGBColor trainStopColor;

    /// @brief color for trainStops signs
    RGBColor trainStopColorSign;

    /// @brief color for containerStops
    RGBColor containerStopColor;

    /// @brief color for containerStop signs
    RGBColor containerStopColorSign;

    /// @brief color for chargingStations
    RGBColor chargingStationColor;

    /// @brief color for chargingStation sign
    RGBColor chargingStationColorSign;

    /// @brief color for chargingStation during charging
    RGBColor chargingStationColorCharge;

    /// @brief color for parkingAreas
    RGBColor parkingAreaColor;

    /// @brief color for parkingArea sign
    RGBColor parkingAreaColorSign;

    /// @brief color for parkingSpace contour
    RGBColor parkingSpaceColorContour;

    /// @brief color for parkingSpace innen
    RGBColor parkingSpaceColor;

    /// @brief color for Stops
    RGBColor stopColor;

    /// @brief color for Waypoints
    RGBColor waypointColor;

    /// @brief color for vehicle trips
    RGBColor vehicleTripColor;

    /// @brief color for stopPersons
    RGBColor stopPersonColor;

    /// @brief color for stopPersons
    RGBColor personTripColor;

    /// @brief color for walks
    RGBColor walkColor;

    /// @brief color for rides
    RGBColor rideColor;

    /// @brief color for containerStops
    RGBColor stopContainerColor;

    /// @brief color for transport
    RGBColor transportColor;

    /// @brief color for tranships
    RGBColor transhipColor;

    /// @brief color for highlighthing deadends
    static const RGBColor SUMO_color_DEADEND_SHOW;

    /// @brief color for child connections between parents and child elements
    static const RGBColor childConnectionColor;

    /// @brief color for edited shapes (Junctions, crossings and connections)
    static const RGBColor editShapeColor;

    /// @brief color for crossings
    static const RGBColor crossingColor;

    /// @brief color for priority crossing
    static const RGBColor crossingPriorityColor;

    /// @brief color for invalid crossing
    static const RGBColor crossingInvalidColor;

};


/// @brief struct for candidate color settings
struct GUIVisualizationCandidateColorSettings {

    /// @brief color for possible candidate element
    static const RGBColor possible;

    /// @brief color for selected candidate source
    static const RGBColor source;

    /// @brief color for selected candidate target
    static const RGBColor target;

    /// @brief color for selected special candidate element (Usually selected using shift+click)
    static const RGBColor special;

    /// @brief color for selected conflict candidate element (Usually selected using ctrl+click)
    static const RGBColor conflict;

    /// @brief color for invalid elements
    static const RGBColor invalid;
};

/// @brief struct for connection settings
struct GUIVisualizationNeteditSizeSettings {

    /// @brief junction bubble radius
    static const double junctionBubbleRadius;

    /// @brief moving junction geometry point radius
    static const double junctionGeometryPointRadius;

    /// @brief moving edge geometry point radius
    static const double edgeGeometryPointRadius;

    /// @brief moving lane geometry point radius
    static const double laneGeometryPointRadius;

    /// @brief moving connection geometry point radius
    static const double connectionGeometryPointRadius;

    /// @brief moving crossing geometry point radius
    static const double crossingGeometryPointRadius;

    /// @brief moving geometry point radius
    static const double polygonGeometryPointRadius;

    /// @brief polygon contour width
    static const double polygonContourWidth;

    /// @brief poly line width
    static const double polylineWidth;

    /// @brief moving additional geometry point radius
    static const double additionalGeometryPointRadius;
};

/// @brief struct for connection settings
struct GUIVisualizationConnectionSettings {

    /// @brief connection width
    static const double connectionWidth;
};


/// @brief struct for additional settings
struct GUIVisualizationAdditionalSettings {

    /// @brief color for rerouters
    static const RGBColor rerouterColor;

    /// @brief rerouter size
    static const double rerouterSize;

    /// @brief color for VSS
    static const RGBColor VSSColor;

    /// @brief VSS size
    static const double VSSSize;

    /// @brief color for Calibrators
    static const RGBColor calibratorColor;

    /// @brief Calibrator width
    static const double calibratorWidth;

    /// @brief Calibrator height
    static const double calibratorHeight;

    /// @brief color for route probes
    static const RGBColor routeProbeColor;

    /// @brief RouteProbe size
    static const double routeProbeSize;

    /// @brief color for vaporizers
    static const RGBColor vaporizerColor;

    /// @brief Vaporizer size
    static const double vaporizerSize;

    /// @brief stopEdges size
    static const double stopEdgeSize;

    /// @brief connection color
    static const RGBColor connectionColor;

    /// @brief connection color selected
    static const RGBColor connectionColorSelected;

    /// @brief tractionSubstation size
    static const double tractionSubstationSize;

    /// @brief overhead wire color top
    static const RGBColor overheadWireColorTop;

    /// @brief overhead wire color bot
    static const RGBColor overheadWireColorBot;

    /// @brief overhead wire color selected
    static const RGBColor overheadWireColorSelected;

    /// @brief arrow width
    static const double arrowWidth;

    /// @brief arrow length
    static const double arrowLength;

    /// @brief arrow offset
    static const double arrowOffset;

    /// @brief connection color between E1/junctions and TLS
    static const RGBColor TLSConnectionColor;
};


/// @brief struct for detector settings
struct GUIVisualizationDetectorSettings {

    /// @brief color for E1 detectors
    static const RGBColor E1Color;

    /// @brief E1 detector widths
    static const double E1Width;

    /// @brief E1 Instant detector heights
    static const double E1Height;

    /// @brief color for E1 Instant detectors
    static const RGBColor E1InstantColor;

    /// @brief E1 Instant detector widths
    static const double E1InstantWidth;

    /// @brief E1 Instant detector heights
    static const double E1InstantHeight;

    /// @brief color for E2 detectors
    static const RGBColor E2Color;

    /// @brief E2 detector widths
    static const double E2Width;

    /// @brief E3 detector size
    static const double E3Size;

    /// @brief color for Entrys
    static const RGBColor E3EntryColor;

    /// @brief color for Exits
    static const RGBColor E3ExitColor;

    /// @brief E3 Entry/Exit detector width
    static const double E3EntryExitWidth;

    /// @brief E3 Entry/Exit detector height
    static const double E3EntryExitHeight;
};


/// @brief struct for stoppingPlace settings
struct GUIVisualizationStoppingPlaceSettings {
    /// @brief busStop offset
    static const double stoppingPlaceSignOffset;

    /// @brief busStop width
    static const double busStopWidth;

    /// @brief trainStop width
    static const double trainStopWidth;

    /// @brief containerStop width
    static const double containerStopWidth;

    /// @brief chargingStation width
    static const double chargingStationWidth;

    /// @brief symbol external radius
    static const double symbolExternalRadius;

    /// @brief symbol internal radius
    static const double symbolInternalRadius;

    /// @brief symbol internal text size
    static const double symbolInternalTextSize;
};


/// @brief struct for dotted contour settings
struct GUIVisualizationDottedContourSettings {

    /// @brief width of dotted contour segments
    static const double segmentWidth;

    /// @brief width of small dotted contour segments
    static const double segmentWidthSmall;

    /// @brief width of large dotted contour segments
    static const double segmentWidthLarge;

    /// @brief length of dotted contour segments
    static const double segmentLength;

    /// @brief first color of dotted inspected contour
    static const RGBColor firstInspectedColor;

    /// @brief second color of dotted inspectedcontour
    static const RGBColor secondInspectedColor;

    /// @brief first color of dotted front contour
    static const RGBColor firstFrontColor;

    /// @brief second color of dotted front contour
    static const RGBColor secondFrontColor;
};


/// @brief struct for width settings
struct GUIVisualizationWidthSettings {

    /// @brief constructor
    GUIVisualizationWidthSettings();

    /// @brief equality comparator
    bool operator==(const GUIVisualizationWidthSettings& other);

    /// @brief inequality comparator
    bool operator!=(const GUIVisualizationWidthSettings& other);

    /// @brief width for trips
    double tripWidth;

    /// @brief width for person trips
    double personTripWidth;

    /// @brief width for walks
    double walkWidth;

    /// @brief width for rides
    double rideWidth;

    /// @brief width for rides
    double transportWidth;

    /// @brief width for rides
    double transhipWidth;

    /// @brief width for routes
    static const double routeWidth;

    /// @brief width for embeddedroutes
    static const double embeddedRouteWidth;
};


/// @brief struct for detail settings
struct GUIVisualizationDetailSettings {

    /// @brief draw connections in demand mode
    static const double tmp;
};


/**
 * @class GUIVisualizationSettings
 * @brief Stores the information about how to visualize structures
 */
class GUIVisualizationSettings {

public:

    enum class Detail : int {
        Level0 = 0,
        CircleResolution32 = 0,     // circle resolution = 32
        DrawPolygonTesselation = 0, // draw polygons tesselated
        LaneDetails = 0,            // offset, icons, indicators...
        Text = 0,                   // draw text (E2, routes...)
        VehiclePoly = 0,            // draw vehicles as polygons
        JunctionElementDetails = 0, // draw junction elements with high detail (crossings, connections..)
        LockedIcons = 0,            // draw lock icons

        Level1 = 1,
        CircleResolution16 = 1,         // circle resolution = 16
        VehicleBox = 1,                 // vehicles as boxes
        AdditionalDetails = 1,          // stoppingPlace signs, EntryExit arrows...
        GeometryPoint = 1,              // draw geometry points
        JunctionElement = 1,            // crossings, walking area, connections and internal lanes
        DottedContoursResampled = 1,    // resample dotted contours
        PreciseSelection = 1,           // precise selection using boundaries

        Level2 = 2,
        CircleResolution8 = 2,  // circle resolution = 8
        DrawPolygonSquare = 2,  // draw polygons as squares
        VehicleTriangle = 2,    // draw vehicles as triangles
        Additionals = 2,        // draw additional elements
        DottedContours = 2,     // draw dotted contours
        GeometryBoxLines = 2,   // draw lines instead boxes in GUIGeometry::drawGeometry

        Level3 = 3,
        CircleResolution4 = 3,  // draw circle resolution as squares
        TLSIcon = 3,            // draw TLS icons

        Level4 = 4,
        GeometryBoxSimpleLine = 4,  // draw lines with width = 1 instead boxes in GUIGeometry::drawGeometry
    };

    /// @brief constructor
    GUIVisualizationSettings(const std::string& _name, bool _netedit = false);

    /// @brief check if draw junction
    bool checkDrawJunction(const Boundary& b, const bool selected) const;

    /// @brief check if draw edge
    bool checkDrawEdge(const Boundary& b) const;

    /// @brief update ignore hide by zoom (call BEFORE drawing all elements).
    void updateIgnoreHideByZoom();

    /// @brief check if draw additionals
    bool checkDrawAdditional(Detail d, const bool selected) const;

    /// @brief check if draw polygon
    bool checkDrawPoly(const Boundary& b, const bool selected) const;

    /// @brief check if draw POI
    bool checkDrawPOI(const double w, const double h, const Detail d, const bool selected) const;

    /// @brief check if draw vehicle
    bool checkDrawVehicle(Detail d, const bool selected) const;

    /// @brief check if draw person
    bool checkDrawPerson(Detail d, const bool selected) const;

    /// @brief check if draw container
    bool checkDrawContainer(Detail d, const bool selected) const;

    /// @brief copy all content from another GUIVisualizationSettings (note: DON'T USE in DrawGL functions!)
    void copy(const GUIVisualizationSettings& s);

    /// @brief init default settings
    void initNeteditDefaults();
    void initSumoGuiDefaults();

    /** @brief Writes the settings into an output device
     * @param[in] dev The device to write the settings into
     */
    void save(OutputDevice& dev) const;

    /** @brief Returns the number of the active lane (edge) coloring schme
     * @return number of the active scheme
     */
    int getLaneEdgeMode() const;

    /** @brief Returns the number of the active lane (edge) scaling schme
     * @return number of the active scheme
     */
    int getLaneEdgeScaleMode() const;

    /** @brief Returns the current lane (edge) coloring schme
     * @return current scheme
     */
    GUIColorScheme& getLaneEdgeScheme();

    /** @brief Returns the current lane (edge) scaling schme
     * @return current scheme
     */
    GUIScaleScheme& getLaneEdgeScaleScheme();

    /// @brief Comparison operator
    bool operator==(const GUIVisualizationSettings& vs2);

    /// @brief map from LinkState to color constants
    static const RGBColor& getLinkColor(const LinkState& ls, bool realistic = false);

    /// @brief return an angle that is suitable for reading text aligned with the given angle (degrees)
    double getTextAngle(double objectAngle) const;

    /// @brief return wether the text was flipped for reading at the given angle
    bool flippedTextAngle(double objectAngle) const;

    /// @brief return the detail level
    Detail getDetailLevel(const double exaggeration) const;

    /// @brief check if details can be drawn for the given GUIVisualizationDetailSettings and current scale and exxageration
    bool drawDetail(const double detail, const double exaggeration) const;

    /// @brief check if moving geometry point can be draw
    bool drawMovingGeometryPoint(const double exaggeration, const double radius) const;

    /// @brief The name of this setting
    std::string name;

    /// @brief Whether the settings are for Netedit
    bool netedit;

    /// @brief The current view rotation angle
    double angle;

    /// @brief Information whether dithering shall be enabled
    bool dither;

    /// @brief Information whether frames-per-second should be drawn
    bool fps;

    /// @brief drawl all objects according to their z data
    bool trueZ;

    /// @name Background visualization settings
    /// @{

    /// @brief The background color to use
    RGBColor backgroundColor;

    /// @brief Information whether a grid shall be shown
    bool showGrid;

    /// @brief Information about the grid spacings
    double gridXSize, gridYSize;
    /// @}

    /// @name lane visualization settings
    /// @{

    /// @brief The mesoscopic edge colorer
    GUIColorer edgeColorer;

    /// @brief The mesoscopic edge scaler
    GUIScaler edgeScaler;

    /// @brief this should be set at the same time as MSGlobals::gUseMesoSim
    static bool UseMesoSim;

    /// @brief The lane colorer
    GUIColorer laneColorer;

    /// @brief The lane scaler
    GUIScaler laneScaler;

    /// @brief Information whether lane borders shall be drawn
    bool laneShowBorders;

    /// @brief Information whether bicycle lane marking shall be drawn
    bool showBikeMarkings;

    /// @brief Information whether link textures (arrows) shall be drawn
    bool showLinkDecals;

    /// @brief Information whether link rules (colored bars) shall be drawn with a realistic color scheme
    bool realisticLinkRules;

    /// @brief Information whether link rules (colored bars) shall be drawn
    bool showLinkRules;

    /// @brief Information whether rails shall be drawn
    bool showRails;

    /// @brief Setting bundles for optional drawing names with size and color
    GUIVisualizationTextSettings edgeName, internalEdgeName, cwaEdgeName, streetName, edgeValue, edgeScaleValue;

    /// @brief flag to show or hide connectors
    bool hideConnectors;

    /// @brief The lane exaggeration (upscale thickness)
    double laneWidthExaggeration;

    /// @brief The minimum visual lane width for drawing
    double laneMinSize;

    /// @brief Whether to show direction indicators for lanes
    bool showLaneDirection;

    /// @brief Whether to show sublane boundaries
    bool showSublanes;

    /// @brief Whether to improve visualisation of superposed (rail) edges
    bool spreadSuperposed;

    /// @brief disable hide by zoom
    bool disableHideByZoom;

    /// @brief key for coloring by edge parameter
    std::string edgeParam, laneParam;
    /// @brief key for coloring by vehicle parameter
    std::string vehicleParam;
    /// @brief key for scaling by vehicle parameter
    std::string vehicleScaleParam;
    /// @brief key for rendering vehicle textual parameter
    std::string vehicleTextParam;

    /// @brief key for coloring by edgeData
    std::string edgeData;
    /// @brief id for coloring by live edgeData
    std::string edgeDataID;
    /// @brief key for scaling by edgeData
    std::string edgeDataScaling;

    /// @brief checks and thresholds for rainbow coloring
    GUIVisualizationRainbowSettings edgeValueRainBow;
    /// @}

    /// @name vehicle visualization settings
    /// @{

    /// @brief The vehicle colorer
    GUIColorer vehicleColorer;

    /// @brief The size scaling settings for vehicles
    GUIScaler vehicleScaler;

    /// @brief The quality of vehicle drawing
    int vehicleQuality;

    /// @brief Information whether vehicle blinkers shall be drawn
    bool showBlinker;

    /// @brief Information whether the lane change preference shall be drawn
    bool drawLaneChangePreference;

    /// @brief Information whether the minimum gap shall be drawn
    bool drawMinGap;

    /// @brief Information whether the brake gap shall be drawn
    bool drawBrakeGap;

    /// @brief Information whether the communication range shall be drawn
    bool showBTRange;

    /// @brief Information whether the route index should be shown
    bool showRouteIndex;

    /// @brief Whether vehicle length shall be scaled with length/geometry factor
    bool scaleLength;

    /// @brief Whether to draw reversed vehicles in their reversed state
    bool drawReversed;

    /// @brief Set whether parking related information should be shown
    bool showParkingInfo;

    /// @brief Set whether the charging search related information should be shown
    bool showChargingInfo;

    // Setting bundles for controling the size of the drawn vehicles
    GUIVisualizationSizeSettings vehicleSize;

    // Setting bundles for optional drawing vehicle names or color value
    GUIVisualizationTextSettings vehicleName, vehicleValue, vehicleScaleValue, vehicleText;

    /// @}


    /// @name person visualization settings
    /// @{

    /// @brief The person colorer
    GUIColorer personColorer;

    /// @brief The quality of person drawing
    int personQuality;

    // Setting bundles for controling the size of the drawn persons
    GUIVisualizationSizeSettings personSize;

    // Setting bundles for optional drawing person names
    GUIVisualizationTextSettings personName, personValue;

    /// @brief Flag for visualizing the pedestrian network generated for JuPedSim
    bool showPedestrianNetwork;

    /// @brief The color of the pedestrian network generated for JuPedSim
    RGBColor pedestrianNetworkColor;
    /// @}


    /// @name container visualization settings
    /// @{

    /// @brief The container colorer
    GUIColorer containerColorer;

    /// @brief The quality of container drawing
    int containerQuality;

    // Setting bundles for controling the size of the drawn containers
    GUIVisualizationSizeSettings containerSize;

    // Setting bundles for optional drawing container names
    GUIVisualizationTextSettings containerName;
    /// @}


    /// @name junction visualization settings
    /// @{

    /// @brief The junction colorer
    GUIColorer junctionColorer;

    // Setting bundles for optional drawing junction names and indices
    GUIVisualizationTextSettings drawLinkTLIndex, drawLinkJunctionIndex, junctionID, junctionName, internalJunctionName, tlsPhaseIndex, tlsPhaseName;

    /// @brief Information whether lane-to-lane arrows shall be drawn
    bool showLane2Lane;
    /// @brief whether the shape of the junction should be drawn
    bool drawJunctionShape;
    /// @brief whether crosings and walkingareas shall be drawn
    bool drawCrossingsAndWalkingareas;
    // Setting bundles for controling the size of the drawn junction
    GUIVisualizationSizeSettings junctionSize;

    GUIVisualizationRainbowSettings junctionValueRainBow;
    /// @}


    /// @name Additional structures visualization settings
    /// @{

    /// @brief The additional structures visualization scheme
    // @todo decouple addExageration for POIs, Polygons, Triggers etc
    int addMode;
    // Setting bundles for controling the size of additional items
    GUIVisualizationSizeSettings addSize;
    // Setting bundles for optional drawing additional names
    GUIVisualizationTextSettings addName;
    // Setting bundles for optional drawing additional full names
    GUIVisualizationTextSettings addFullName;
    /// @}


    /// @name shapes visualization settings
    /// @{

    /// @brief The POI colorer
    GUIColorer poiColorer;

    // Setting bundles for controling the size of the drawn POIs
    GUIVisualizationSizeSettings poiSize;

    /// @brief The detail level for drawing POIs
    int poiDetail;

    // Setting bundles for optional drawing poi names
    GUIVisualizationTextSettings poiName;

    // Setting bundles for optional drawing poi types
    GUIVisualizationTextSettings poiType;

    // Setting bundles for optional drawing poi text
    GUIVisualizationTextSettings poiText;

    /// @brief key for rendering poi textual parameter
    std::string poiTextParam;

    /// @brief The polygon colorer
    GUIColorer polyColorer;

    // Setting bundles for controling the size of the drawn polygons
    GUIVisualizationSizeSettings polySize;

    // Setting bundles for optional drawing polygon names
    GUIVisualizationTextSettings polyName;

    // Setting bundles for optional drawing polygon types
    GUIVisualizationTextSettings polyType;
    /// @}


    /// @name data mode visualization settings
    /// @{
    /// @brief the edgeRelation / tazRelation colorer
    GUIColorer dataColorer;
    GUIVisualizationTextSettings dataValue;

    /// @brief The tazRelation exaggeration (upscale thickness)
    double tazRelWidthExaggeration;

    /// @brief The edgeRelation exaggeration (upscale thickness)
    double edgeRelWidthExaggeration;

    /// @brief key for coloring by edgeRelation / tazRelation attribute
    std::string relDataAttr;

    /// @brief value below which edgeData and edgeRelation data value should not be rendered
    GUIVisualizationRainbowSettings dataValueRainBow;
    /// @}


    /// @name 3D visualization settings
    /// @{
    /// @brief whether the TLS link markers should be drawn
    bool show3DTLSLinkMarkers;

    /// @brief whether the semi-transparent domes around 3D TL models should be drawn
    bool show3DTLSDomes;

    /// @brief whether 3D TLS models should be generated automatically
    bool generate3DTLSModels;

    /// @brief whether to draw the head up display items
    bool show3DHeadUpDisplay;

    /// @brief 3D material light components
    RGBColor ambient3DLight;
    RGBColor diffuse3DLight;

    /// @brief sky background color
    RGBColor skyColor;

    /// @}


    /// @brief Information whether the size legend shall be drawn
    bool showSizeLegend;

    /// @brief Information whether the edge color legend shall be drawn
    bool showColorLegend;

    /// @brief Information whether the vehicle color legend shall be drawn
    bool showVehicleColorLegend;

    /// @brief information about a lane's width (temporary, used for a single view)
    double scale;

    /// @brief whether the application is in gaming mode or not
    bool gaming;

    /// @brief enable or disable draw boundaries
    bool drawBoundaries;

    /// @brief the current selection scaling in netedit (set in SelectorFrame)
    double selectorFrameScale;

    /// @brief whether drawing is performed for the purpose of selecting objects in view using ViewObjectsHandler
    bool drawForViewObjectsHandler;

    /// @brief whether drawing is performed for the purpose of selecting objects using a rectangle
    bool drawForRectangleSelection;

    /// @brief flag to force draw for rectangle selection (see drawForRectangleSelection)
    bool forceDrawForRectangleSelection;

    /// @brief flag for disable dotted contours in netedit
    bool disableDottedContours;

    // Setting bundles for optional drawing geometry point indices
    GUIVisualizationTextSettings geometryIndices;

    /// @brief whether secondary lane shape shall be drawn
    bool secondaryShape;

    /**@brief whether drawing is performed in left-hand networks
     * @note used to avoid calls to OptionsCont::getOptions() in every drawgl(...) function, and
     * updated in every doPaintGL(int mode, const Boundary& bound) call
     */
    bool lefthand;

    /**@brief whether drawing is performed in left-hand networks
     * @note used to avoid calls to OptionsCont::getOptions() in every drawgl(...) function, and
     * updated in every doPaintGL(int mode, const Boundary& bound) call
     */
    bool disableLaneIcons;

    /// @brief scheme names
    static const std::string SCHEME_NAME_EDGE_PARAM_NUMERICAL;
    static const std::string SCHEME_NAME_LANE_PARAM_NUMERICAL;
    static const std::string SCHEME_NAME_PARAM_NUMERICAL;
    static const std::string SCHEME_NAME_EDGEDATA_NUMERICAL;
    static const std::string SCHEME_NAME_DATA_ATTRIBUTE_NUMERICAL;
    static const std::string SCHEME_NAME_SELECTION;
    static const std::string SCHEME_NAME_TYPE;
    static const std::string SCHEME_NAME_PERMISSION_CODE;
    static const std::string SCHEME_NAME_EDGEDATA_LIVE;

    static const double MISSING_DATA;
    static RGBColor COL_MISSING_DATA;

    static std::map<std::string, std::vector<RGBColor> > RAINBOW_SCHEMES;

    /// @brief color settings
    GUIVisualizationColorSettings colorSettings;

    /// @brief candidate color settings
    GUIVisualizationCandidateColorSettings candidateColorSettings;

    /// @brief netedit size settings
    GUIVisualizationNeteditSizeSettings neteditSizeSettings;

    /// @brief connection settings
    GUIVisualizationConnectionSettings connectionSettings;

    /// @brief Additional settings
    GUIVisualizationAdditionalSettings additionalSettings;

    /// @brief Detector settings
    GUIVisualizationDetectorSettings detectorSettings;

    /// @brief StoppingPlace settings
    GUIVisualizationStoppingPlaceSettings stoppingPlaceSettings;

    /// @brief dotted contour settings
    GUIVisualizationDottedContourSettings dottedContourSettings;

    /// @brief width settings
    GUIVisualizationWidthSettings widthSettings;

    /// @brief detail settings
    GUIVisualizationDetailSettings detailSettings;

    /// @brief alt key pressed (only used for draw polygons under other elements in SUMO-GUI, store is not needed)
    bool altKeyPressed = false;

    /// @brief constant for boundary size drawing (20 for slow computers, 10 for quick computers)
    double BoundarySizeDrawing = 15;

protected:
    /// @brief flag for ignore hide by zoom (used if we're drawing elements with constant size, their ID/name/etc. texts, etc.)
    bool myIgnoreHideByZoom;

private:
    /// @brief set copy constructor private
    GUIVisualizationSettings(const GUIVisualizationSettings&) = default;

    /// @brief set assignment operator private
    GUIVisualizationSettings& operator=(const GUIVisualizationSettings&) = default;
};
