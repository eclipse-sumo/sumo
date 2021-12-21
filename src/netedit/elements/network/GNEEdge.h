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
/// @file    GNEEdge.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
///
// A road/street connecting two junctions (netedit-version, adapted from GUIEdge)
// Basically a container for an NBEdge with drawing and editing capabilities
/****************************************************************************/
#pragma once
#include <config.h>

#include <netbuild/NBEdge.h>
#include <netedit/elements/GNECandidateElement.h>
#include <utils/gui/div/GUIGeometry.h>
#include <utils/gui/div/GUIDottedGeometry.h>

#include "GNENetworkElement.h"


// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;
class GNEJunction;
class GNELane;
class GNEConnection;
class GNERouteProbe;
class GNECrossing;
class GNEEdgeType;
class GNEEdgeTemplate;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEEdge
 * @brief A road/street connecting two junctions (netedit-version)
 *
 * @see MSEdge
 */
class GNEEdge : public GNENetworkElement, public GNECandidateElement {

    /// @brief Friend class
    friend class GNEChange_Lane;
    friend class GNEChange_Connection;

public:
    /// @brief Definition of the lane's vector
    typedef std::vector<GNELane*> LaneVector;

    /// @brief Definition of the connection's vector
    typedef std::vector<GNEConnection*> ConnectionVector;

    /**@brief Constructor
     * @param[in] net The net to inform about gui updates
     * @param[in] nbe The represented edge
     * @param[in] loaded Whether the edge was loaded from a file
     */
    GNEEdge(GNENet* net, NBEdge* nbe, bool wasSplit = false, bool loaded = false);

    /// @brief Destructor.
    ~GNEEdge();

    /// @brief get from Junction (only used to increase readability)
    inline GNEJunction* getFromJunction() const {
        return getParentJunctions().front();
    }

    /// @brief get from Junction (only used to increase readability)
    inline GNEJunction* getToJunction() const {
        return getParentJunctions().back();
    }

    /// @name Functions related with geometry of element
    /// @{
    /// @brief update pre-computed geometry information
    void updateGeometry();

    /// @brief Returns position of hierarchical element in view
    Position getPositionInView() const;
    /// @}

    /// @name Functions related with move elements
    /// @{
    /// @brief get move operation for the given shapeOffset (can be nullptr)
    GNEMoveOperation* getMoveOperation();

    /// @brief remove geometry point in the clicked position
    void removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList);
    /// @}

    /// @name functions for geometry points
    /// @{
    /// @brief check if current edge has custom end points
    bool hasCustomEndPoints() const;

    /// @brief return true if user clicked over ShapeStart
    bool clickedOverShapeStart(const Position& pos) const;

    /// @brief return true if user clicked over ShapeEnd
    bool clickedOverShapeEnd(const Position& pos) const;

    /// @brief return true if user clicked over a Geometry Point
    bool clickedOverGeometryPoint(const Position& pos) const;
    /// @}

    /// @brief update edge geometry after junction move
    void updateJunctionPosition(GNEJunction* junction, const Position& origPos);

    /// @name inherited from GUIGlObject
    /// @{
    /**@brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /// @brief return exaggeration asociated with this GLObject
    double getExaggeration(const GUIVisualizationSettings& s) const;

    /// @brief update centering boundary (implies change in RTREE)
    void updateCenteringBoundary(const bool updateGrid);

    /// @brief Returns the street name
    const std::string getOptionalName() const;

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;
    /// @}

    /// @brief returns the internal NBEdge
    NBEdge* getNBEdge() const;

    /// @brief get opposite edge
    GNEEdge* getOppositeEdge() const;

    /// @brief makes pos the new geometry endpoint at the appropriate end, or remove current existent endpoint
    void editEndpoint(Position pos, GNEUndoList* undoList);

    /// @brief restores the endpoint to the junction position at the appropriate end
    void resetEndpoint(const Position& pos, GNEUndoList* undoList);

    /// @brief restores both endpoint to the junction position at the appropriate end
    void resetBothEndpoint(GNEUndoList* undoList);

    /// @name inherited from GNEAttributeCarrier
    /// @{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getAttribute(SumoXMLAttr key) const;
    std::string getAttributeForSelection(SumoXMLAttr key) const;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    bool isValid(SumoXMLAttr key, const std::string& value);

    /* @brief method for check if the value for certain attribute is set
     * @param[in] key The attribute key
     */
    bool isAttributeEnabled(SumoXMLAttr key) const;

    /* @brief method for check if the value for certain attribute is computed (for example, due a network recomputing)
     * @param[in] key The attribute key
     */
    bool isAttributeComputed(SumoXMLAttr key) const;

    /// @}

    /// @brief get parameters map
    const std::map<std::string, std::string>& getACParametersMap() const;

    /// @brief set responsibility for deleting internal structures
    void setResponsible(bool newVal);

    /**@brief update edge geometry and inform the lanes
     * @param[in] geom The new geometry
     * @param[in] inner Whether geom is only the inner points
     */
    void setGeometry(PositionVector geom, bool inner);

    /// @brief get front up shape position
    const Position getFrontUpShapePosition() const;

    /// @brief get front down shape position
    const Position getFrontDownShapePosition() const;

    /// @brief get back up shape position
    const Position getBackUpShapePosition() const;

    /// @brief get back down shape position
    const Position getBackDownShapePosition() const;

    /// @brief remake connections
    void remakeGNEConnections();

    /// @brief copy edge attributes from edgetemplate
    void copyTemplate(const GNEEdgeTemplate* edgeTemplate, GNEUndoList* undoList);

    /// @brief copy edge attributes from edgeType
    void copyEdgeType(const GNEEdgeType* edgeType, GNEUndoList* undoList);

    /// @brief returns GLIDs of all lanes
    std::set<GUIGlID> getLaneGlIDs() const;

    /// @brief returns a reference to the lane vector
    const std::vector<GNELane*>& getLanes() const;

    /// @brief returns a reference to the GNEConnection vector
    const std::vector<GNEConnection*>& getGNEConnections() const;

    /// @brief get GNEConnection if exist, and if not create it if create is enabled
    GNEConnection* retrieveGNEConnection(int fromLane, NBEdge* to, int toLane, bool createIfNoExist = true);

    /// @brief whether this edge was created from a split
    bool wasSplit();

    /* @brief compute a splitting position which keeps the resulting edges
     * straight unless the user clicked near a geometry point */
    Position getSplitPos(const Position& clickPos);

    /// @brief override to also set lane ids
    void setMicrosimID(const std::string& newID);

    /// @brief check if edge has a restricted lane
    bool hasRestrictedLane(SUMOVehicleClass vclass) const;

    // @brief the radius in which to register clicks for geometry nodes
    static const double SNAP_RADIUS;

    // @brief the radius in which to register clicks for geometry nodes (Squared)
    static const double SNAP_RADIUS_SQUARED;

    /// @brief clear current connections
    void clearGNEConnections();

    /// @brief obtain relative positions of RouteProbes
    int getRouteProbeRelativePosition(GNERouteProbe* routeProbe) const;

    /// @brief get GNECrossings vinculated with this Edge
    std::vector<GNECrossing*> getGNECrossings();

    /// @brief make geometry smooth
    void smooth(GNEUndoList* undoList);

    /// @brief interpolate z values linear between junctions
    void straightenElevation(GNEUndoList* undoList);

    /// @brief smooth elevation with regard to adjoining edges
    void smoothElevation(GNEUndoList* undoList);

    /// @brief return smoothed shape
    PositionVector smoothShape(const PositionVector& shape, bool forElevation);

    /// @brief return the first lane that allow a vehicle of type vClass (or the first lane, if none was found)
    GNELane* getLaneByAllowedVClass(const SUMOVehicleClass vClass) const;

    /// @brief return the first lane that disallow a vehicle of type vClass (or the first lane, if none was found)
    GNELane* getLaneByDisallowedVClass(const SUMOVehicleClass vClass) const;

    // @brief update vehicle spread geometries
    void updateVehicleSpreadGeometries();

    // @brief update vehicle geometries
    void updateVehicleStackLabels();

    /// @brief draw edge geometry points (note: This function is called by GNELane::drawGL(...)
    void drawEdgeGeometryPoints(const GUIVisualizationSettings& s, const GNELane* lane) const;

    /// @brief draw dotted contour for the given dottedGeometries
    static void drawDottedContourEdge(const GUIDottedGeometry::DottedContourType type, const GUIVisualizationSettings& s, const GNEEdge* edge,
                                      const bool drawFrontExtreme, const bool drawBackExtreme);

    /// @brief check if edge makes a convex angle [0 - 180) degrees
    bool isConvexAngle() const;

protected:
    /// @brief the underlying NBEdge
    NBEdge* myNBEdge;

    /// @brief vector with the lanes of this edge
    LaneVector myLanes;

    /// @brief vector with the connections of this edge
    ConnectionVector myGNEConnections;

    /// @brief position used for move Lanes
    Position myPositionBeforeMoving;

    /// @brief whether we are responsible for deleting myNBNode
    bool myAmResponsible;

    /// @brief whether this edge was created from a split
    bool myWasSplit;

    /// @brief modification status of the connections
    std::string myConnectionStatus;

private:
    /// @brief Stack position (used to stack demand elements over edges)
    class StackPosition : public std::pair<double, double> {

    public:
        /// @brief constructor
        StackPosition(const double departPos, const double length);

        /// @brief get begin position
        double beginPosition() const;

        /// @brief get end position
        double endPosition() const;
    };

    /// @brief Stack demand elements
    class StackDemandElements : public std::pair<StackPosition, std::vector<GNEDemandElement*> > {

    public:
        /// @brief constructor
        StackDemandElements(const StackPosition stackedPosition, GNEDemandElement* demandElement);

        /// @brief add demand elment to current StackDemandElements
        void addDemandElements(GNEDemandElement* demandElement);

        /// @brief get stack position
        const StackPosition& getStackPosition() const;

        /// @brief get demand elements
        const std::vector<GNEDemandElement*>& getDemandElements() const;
    };

    /// @brif flag to enable/disable update geometry of lanes (used mainly by setNumLanes)
    bool myUpdateGeometry;

    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief set move shape
    void setMoveShape(const GNEMoveResult& moveResult);

    /// @brief commit move shape
    void commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList);

    /**@brief changes the number of lanes.
     * When reducing the number of lanes, higher-numbered lanes are removed first.
     * When increasing the number of lanes, the last known attributes for a lane
     * with this number are restored. If none are found the attributes for the
     * leftmost lane are copied
     */
    void setNumLanes(int numLanes, GNEUndoList* undoList);

    /// @brief update front parent junction
    void updateFirstParentJunction(const std::string& value);

    /// @brief update last parent junction
    void updateSecondParentJunction(const std::string& value);

    /// @brief@brief increase number of lanes by one use the given attributes and restore the GNELane
    void addLane(GNELane* lane, const NBEdge::Lane& laneAttrs, bool recomputeConnections);

    /// @briefdecrease the number of lanes by one. argument is only used to increase robustness (assertions)
    void removeLane(GNELane* lane, bool recomputeConnections);

    /// @brief adds a connection
    void addConnection(NBEdge::Connection nbCon, bool selectAfterCreation = false);

    /// @brief removes a connection
    void removeConnection(NBEdge::Connection nbCon);

    /// @brief remove crossing of junction
    void removeEdgeFromCrossings(GNEJunction* junction, GNEUndoList* undoList);

    /// @brief change Shape StartPos
    void setShapeStartPos(const Position& pos);

    /// @brief change Shape EndPos
    void setShapeEndPos(const Position& pos);

    /// @brief get vehicles a that start over this edge
    const std::map<const GNELane*, std::vector<GNEDemandElement*> > getVehiclesOverEdgeMap() const;

    /// @brief draw edge name
    void drawEdgeName(const GUIVisualizationSettings& s) const;

    /// @brief draw edgeStopOffset
    void drawLaneStopOffset(const GUIVisualizationSettings& s) const;

    /// @brief check if given stacked positions are overlapped
    bool areStackPositionOverlapped(const GNEEdge::StackPosition& vehicleA, const GNEEdge::StackPosition& vehicleB) const;

    /// @brief process moving edge when only from junction is selected
    GNEMoveOperation* processMoveFromJunctionSelected(const PositionVector originalShape, const Position mousePosition, const double snapRadius);

    /// @brief process moving edge when only to junction is selected
    GNEMoveOperation* processMoveToJunctionSelected(const PositionVector originalShape, const Position mousePosition, const double snapRadius);

    /// @brief process moving edge when both junctions are selected
    GNEMoveOperation* processMoveBothJunctionSelected();

    /// @brief process moving edge when none junction are selected
    GNEMoveOperation* processNoneJunctionSelected(const double snapRadius);

    /// @brief invalidated copy constructor
    GNEEdge(const GNEEdge& s) = delete;

    /// @brief invalidated assignment operator
    GNEEdge& operator=(const GNEEdge& s) = delete;
};
