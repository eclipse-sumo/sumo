/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEEdge.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// A road/street connecting two junctions (netedit-version, adapted from GUIEdge)
// Basically a container for an NBEdge with drawing and editing capabilities
/****************************************************************************/
#ifndef GNEEdge_h
#define GNEEdge_h


// ===========================================================================
// included modules
// ===========================================================================

#include "GNENetElement.h"
#include <netbuild/NBEdge.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;
class GNEJunction;
class GNELane;
class GNEConnection;
class GNERouteProbe;
class GNEVaporizer;
class GNERerouter;
class GNECrossing;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEEdge
 * @brief A road/street connecting two junctions (netedit-version)
 *
 * @see MSEdge
 */
class GNEEdge : public GNENetElement {

    /// @brief Friend class
    friend class GNEChange_Lane;
    friend class GNEChange_Connection;

public:
    /// @brief Definition of the lane's vector
    typedef std::vector<GNELane*> LaneVector;

    /// @brief Definition of the connection's vector
    typedef std::vector<GNEConnection*> ConnectionVector;

    /**@brief Constructor.
     * @param[in] nbe The represented edge
     * @param[in] net The net to inform about gui updates
     * @param[in] loaded Whether the edge was loaded from a file
     */
    GNEEdge(NBEdge& nbe, GNENet* net, bool wasSplit = false, bool loaded = false);

    /// @brief Destructor.
    ~GNEEdge();

    /// @brief gererate a new ID for an element child
    std::string generateChildID(SumoXMLTag childTag);

    /// @name Functions related with geometry of element
    /// @{
    /// @brief update pre-computed geometry information
    void updateGeometry();

    /// @brief Returns position of hierarchical element in view
    Position getPositionInView() const;
    /// @}

    /// @name functions for edit start and end positions of shapes
    /// @{
    /// @brief return true if user clicked over ShapeStart
    bool clickedOverShapeStart(const Position& pos);

    /// @brief return true if user clicked over ShapeEnd
    bool clickedOverShapeEnd(const Position& pos);

    /// @brief move position of shape start without commiting change
    void moveShapeStart(const Position& oldPos, const Position& offset);

    /// @brief move position of shape end without commiting change
    void moveShapeEnd(const Position& oldPos, const Position& offset);

    /// @brief commit position changing in shape start
    void commitShapeStartChange(const Position& oldPos, GNEUndoList* undoList);

    /// @brief commit position changing in shape end
    void commitShapeEndChange(const Position& oldPos, GNEUndoList* undoList);
    /// @}

    /// @name functions for edit geometry
    /// @{
    /// @brief begin movement (used when user click over edge to start a movement, to avoid problems with problems with GL Tree)
    void startGeometryMoving();

    /// @brief begin movement (used when user click over edge to start a movement, to avoid problems with problems with GL Tree)
    void endGeometryMoving();
    /// @}

    /**@brief return index of a vertex of shape, or of a new vertex if position is over an shape's edge
    * @param pos position of new/existent vertex
    * @param createIfNoExist enable or disable creation of new verte if there isn't another vertex in position
    * @param snapToGrid enable or disable snapToActiveGrid
    * @return index of position vector
    */
    int getVertexIndex(Position pos, bool createIfNoExist, bool snapToGrid);

    /**@brief return index of a vertex of shape, or of a new vertex if position is over an shape's edge
    * @param offset position over edge
    * @param createIfNoExist enable or disable creation of new verte if there isn't another vertex in position
    * @param snapToGrid enable or disable snapToActiveGrid
    * @return index of position vector
    */
    int getVertexIndex(const double offset, bool createIfNoExist, bool snapToGrid);

    /**@brief change position of a vertex of shape without commiting change
    * @param[in] index index of Vertex shape
    * @param[in] newPos The new position of vertex
    * @return index of vertex (in some cases index can change
    */
    int moveVertexShape(const int index, const Position& oldPos, const Position& offset);

    /**@brief move entire shape without commiting change
    * @param[in] oldShape the old shape of polygon before moving
    * @param[in] offset the offset of movement
    */
    void moveEntireShape(const PositionVector& oldShape, const Position& offset);

    /**@brief commit geometry changes in the attributes of an element after use of changeShapeGeometry(...)
    * @param[in] oldShape the old shape of polygon
    * @param[in] undoList The undoList on which to register changes
    */
    void commitShapeChange(const PositionVector& oldShape, GNEUndoList* undoList);

    /// @brief delete the geometry point closest to the given pos
    void deleteGeometryPoint(const Position& pos, bool allowUndo = true);

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

    /**@brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const;

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

    /// @brief returns the source-junction
    GNEJunction* getGNEJunctionSource() const;

    /// @brief returns the destination-junction
    GNEJunction* getGNEJunctionDestiny() const;

    /// @brief get opposite edge
    GNEEdge* getOppositeEdge() const;

    /// @brief makes pos the new geometry endpoint at the appropriate end, or remove current existent endpoint
    void editEndpoint(Position pos, GNEUndoList* undoList);

    /// @brief restores the endpoint to the junction position at the appropriate end
    void resetEndpoint(const Position& pos, GNEUndoList* undoList);

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
    /// @}

    /// @name Function related with Generic Parameters
    /// @{

    /// @brief return generic parameters in string format
    std::string getGenericParametersStr() const;

    /// @brief return generic parameters as vector of pairs format
    std::vector<std::pair<std::string, std::string> > getGenericParameters() const;

    /// @brief set generic parameters in string format
    void setGenericParametersStr(const std::string& value);

    /// @}

    /// @brief set responsibility for deleting internal strctures
    void setResponsible(bool newVal);

    /**@brief update edge geometry and inform the lanes
     * @param[in] geom The new geometry
     * @param[in] inner Whether geom is only the inner points
     */
    void setGeometry(PositionVector geom, bool inner);

    /// @brief remake connections
    void remakeGNEConnections();

    /// @brief copy edge attributes from tpl
    void copyTemplate(GNEEdge* tpl, GNEUndoList* undolist);

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

    // the radius in which to register clicks for geometry nodes
    static const double SNAP_RADIUS;

    /// @brief Dummy edge to use when a reference must be supplied in the no-arguments constructor (FOX technicality)
    static GNEEdge& getDummyEdge();

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
    GNELane* getLaneByVClass(const SUMOVehicleClass vClass) const;

    /**@brief return the first lane that allow a vehicle of type vClass (or the first lane, if none was found)
     * @note flag "found" will be changed depending if lane was found
     */
    GNELane* getLaneByVClass(const SUMOVehicleClass vClass, bool& found) const;

    /// @brief draw partial route
    void drawPartialRoute(const GUIVisualizationSettings& s, const GNEDemandElement* route, const GNEJunction* junction) const;

    /// @brief draw partial trip and Flow
    void drawPartialTripFromTo(const GUIVisualizationSettings& s, const GNEDemandElement* tripOrFromTo, const GNEJunction* junction) const;

    /// @brief draw partial person plan
    void drawPartialPersonPlan(const GUIVisualizationSettings& s, const GNEDemandElement* personPlan, const GNEJunction* junction) const;

protected:
    /// @brief the underlying NBEdge
    NBEdge& myNBEdge;

    /// @brief variable used to save shape bevore moving (used to avoid inconsistences in GL Tree)
    PositionVector myMovingShape;

    /// @brief pointer to GNEJunction source
    GNEJunction* myGNEJunctionSource;

    /// @brief pointer to GNEJunction destiny
    GNEJunction* myGNEJunctionDestiny;

    /// @brief vectgor with the lanes of this edge
    LaneVector myLanes;

    /// @brief vector with the connections of this edge
    ConnectionVector myGNEConnections;

    /// @brief whether we are responsible for deleting myNBNode
    bool myAmResponsible;

    /// @brief whether this edge was created from a split
    bool myWasSplit;

    /// @brief modification status of the connections
    std::string myConnectionStatus;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /**@brief changes the number of lanes.
     * When reducing the number of lanes, higher-numbered lanes are removed first.
     * When increasing the number of lanes, the last known attributes for a lane
     * with this number are restored. If none are found the attributes for the
     * leftmost lane are copied
     */
    void setNumLanes(int numLanes, GNEUndoList* undoList);

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

    /// @brief draw geometry points
    void drawGeometryPoints(const GUIVisualizationSettings& s) const;

    /// @brief draw edge name
    void drawEdgeName(const GUIVisualizationSettings& s) const;

    /// @brief draw Rerouter symbols
    void drawRerouterSymbol(const GUIVisualizationSettings& s, GNEAdditional* rerouter) const;

    /// @brief invalidated copy constructor
    GNEEdge(const GNEEdge& s) = delete;

    /// @brief invalidated assignment operator
    GNEEdge& operator=(const GNEEdge& s) = delete;

    /// @brief constructor for dummy edge
    GNEEdge();
};


#endif

/****************************************************************************/

