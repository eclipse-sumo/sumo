/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNENet.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// The lop level container for GNE-network-components such as GNEEdge and
// GNEJunction.  Contains an internal instances of NBNetBuilder GNE components
// wrap netbuild-components of this underlying NBNetBuilder and supply
// visualisation and editing capabilities (adapted from GUINet)
//
// WorkrouteFlow (rough draft)
//   wrap NB-components
//   do netedit stuff
//   call NBNetBuilder::buildLoaded to save results
//
/****************************************************************************/
#ifndef GNENet_h
#define GNENet_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fx.h>
#include <foreign/rtree/SUMORTree.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include <netedit/changes/GNEChange.h>
#include <utils/common/IDSupplier.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIShapeContainer.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>
#include <utils/shapes/ShapeContainer.h>
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class declarations
// ===========================================================================

class NBNetBuilder;
class GNEAdditional;
class GNEDemandElement;
class GNEApplicationWindow;
class GNEAttributeCarrier;
class GNEConnection;
class GNECrossing;
class GNEEdge;
class GNEJunction;
class GNELane;
class GNENetElement;
class GNEPOI;
class GNEPoly;
class GNEShape;
class GNEUndoList;
class GNEViewNet;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNENet
 * @brief A NBNetBuilder extended by visualisation and editing capabilities
 */
class GNENet : public GUIGlObject, public ShapeContainer {

    /// @brief declare friend class
    friend class GNEAdditionalHandler;
    friend class GNERouteHandler;
    friend class GNEChange_Junction;
    friend class GNEChange_Edge;
    friend class GNEChange_Lane;
    friend class GNEChange_Connection;
    friend class GNEChange_Shape;
    friend class GNEChange_CalibratorItem;
    friend class GNEChange_Additional;
    friend class GNEChange_DemandElement;

public:
    /// @brief struct used for saving all attribute carriers of net, in different formats
    struct AttributeCarriers {
        /// @brief map with the name and pointer to junctions of net
        std::map<std::string, GNEJunction*> junctions;

        /// @brief map with the name and pointer to edges of net
        std::map<std::string, GNEEdge*> edges;

        /// @brief map with the name and pointer to additional elements of net
        std::map<SumoXMLTag, std::map<std::string, GNEAdditional*> > additionals;

        /// @brief map with the name and pointer to demand elements of net
        std::map<SumoXMLTag, std::map<std::string, GNEDemandElement*> > demandElements;

        /// @brief special map used for saving Demand Elements of type "Vehicle" (Vehicles, routeFlows, etc.) sorted by depart time
        std::map<std::string, GNEDemandElement*> vehicleDepartures;
    };

    /**@brief Constructor
     * @param[in] netbuilder the netbuilder which may already have been filled
     * GNENet becomes responsible for cleaning this up
     **/
    GNENet(NBNetBuilder* netBuilder);

    /// @brief Destructor
    ~GNENet();

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

    /**@brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /**@brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const;

    /// @brief Returns the Z boundary (stored in the x() coordinate) values of 0 do not affect the boundary
    const Boundary& getZBoundary() const;

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;
    /// @}

    /// @name inherited from ShapeHandler
    /// @{

    /**@brief Builds a polygon using the given values and adds it to the container
    * @param[in] id The name of the polygon
    * @param[in] type The (abstract) type of the polygon
    * @param[in] color The color of the polygon
    * @param[in] layer The layer of the polygon
    * @param[in] angle The rotation of the polygon
    * @param[in] imgFile The raster image of the polygon
    * @param[in] relativePath set image file as relative path
    * @param[in] shape The shape of the polygon
    * @param[in] geo specify if shape was loaded as GEO coordinate
    * @param[in] fill Whether the polygon shall be filled
    * @param[in] lineWidth The widht for drawing unfiled polygon
    * @return whether the polygon could be added
    */
    bool addPolygon(const std::string& id, const std::string& type, const RGBColor& color, double layer,
                    double angle, const std::string& imgFile, bool relativePath, const PositionVector& shape,
                    bool geo, bool fill, double lineWidth, bool ignorePruning = false);

    /**@brief Builds a POI using the given values and adds it to the container
    * @param[in] id The name of the POI
    * @param[in] type The (abstract) type of the POI
    * @param[in] color The color of the POI
    * @param[in] pos The position of the POI
    * @param[in[ geo use GEO coordinates (lon/lat)
    * @param[in] lane The Lane in which this POI is placed
    * @param[in] posOverLane The position over Lane
    * @param[in] posLat The position lateral over Lane
    * @param[in] layer The layer of the POI
    * @param[in] angle The rotation of the POI
    * @param[in] imgFile The raster image of the POI
    * @param[in] relativePath set image file as relative path
    * @param[in] width The width of the POI image
    * @param[in] height The height of the POI image
    * @return whether the poi could be added
    */
    bool addPOI(const std::string& id, const std::string& type, const RGBColor& color, const Position& pos, bool geo,
                const std::string& lane, double posOverLane, double posLat, double layer, double angle,
                const std::string& imgFile, bool relativePath, double width, double height, bool ignorePruning = false);
    /// @}

    /// @brief returns the bounder of the network
    const Boundary& getBoundary() const;

    /**@brief Returns the RTree used for visualisation speed-up
     * @return The visualisation speed-up
     */
    SUMORTree& getVisualisationSpeedUp();

    /**@brief Returns the RTree used for visualisation speed-up
     * @return The visualisation speed-up
     */
    const SUMORTree& getVisualisationSpeedUp() const;

    /**@brief creates a new junction
     * @param[in] position The position of the new junction
     * @param[in] undoList The undolist in which to mark changes
     * @return the new junction
     */
    GNEJunction* createJunction(const Position& pos, GNEUndoList* undoList);

    /**@brief creates a new edge (unless an edge with the same geometry already
     * exists)
     * @param[in] src The starting junction
     * @param[in] dest The ending junction
     * @param[in] tpl The template edge from which to copy attributes (including lane attrs)
     * @param[in] undoList The undoList in which to mark changes
     * @param[in] suggestedName
     * @param[in] wasSplit Whether the edge was created from a split
     * @param[in] allowDuplicateGeom Whether to create the edge even though another edge with the same geometry exists
     * @param[in] recomputeConnections Whether connections on the affected junctions must be recomputed
     * @return The newly created edge or 0 if no edge was created
     */
    GNEEdge* createEdge(GNEJunction* src, GNEJunction* dest, GNEEdge* tpl, GNEUndoList* undoList,
                        const std::string& suggestedName = "", bool wasSplit = false, bool allowDuplicateGeom = false,
                        bool recomputeConnections = true);

    /**@brief removes junction and all incident edges
     * @param[in] junction The junction to be removed
     * @param[in] undoList The undolist in which to mark changes
     */
    void deleteJunction(GNEJunction* junction, GNEUndoList* undoList);

    /**@brief removes edge
     * @param[in] edge The edge to be removed
     * @param[in] undoList The undolist in which to mark changes
     */
    void deleteEdge(GNEEdge* edge, GNEUndoList* undoList, bool recomputeConnections);

    /**@brief replaces edge
     * @param[in] which The edge to be replaced
     * @param[in] by The replacement edge
     * @param[in] undoList The undolist in which to mark changes
     */
    void replaceIncomingEdge(GNEEdge* which, GNEEdge* by, GNEUndoList* undoList);

    /**@brief removes lane
     * @param[in] lane The lane to be removed
     * @param[in] undoList The undolist in which to mark changes
     */
    void deleteLane(GNELane* lane, GNEUndoList* undoList, bool recomputeConnections);

    /**@brief remove connection
     * @param[in] connection The connection to be removed
     * @param[in] undoList The undolist in which to mark changes
     */
    void deleteConnection(GNEConnection* connection, GNEUndoList* undoList);

    /**@brief remove crossing
     * @param[in] crossing The crossing to be removed
     * @param[in] undoList The undolist in which to mark changes
     */
    void deleteCrossing(GNECrossing* crossing, GNEUndoList* undoList);

    /**@brief remove shape
     * @param[in] shape The Shape to be removed
     * @param[in] undoList The undolist in which to mark changes
     */
    void deleteShape(GNEShape* shape, GNEUndoList* undoList);

    /**@brief remove additional
     * @param[in] additional The Shape to be removed
     * @param[in] undoList The undolist in which to mark changes
     */
    void deleteAdditional(GNEAdditional* additional, GNEUndoList* undoList);

    /**@brief remove demand element
     * @param[in] demandElement The Shape to be removed
     * @param[in] undoList The undolist in which to mark changes
     */
    void deleteDemandElement(GNEDemandElement* demandElement, GNEUndoList* undoList);

    /**@brief duplicates lane
     * @param[in] lane The lane to be duplicated
     * @param[in] undoList The undolist in which to mark changes
     */
    void duplicateLane(GNELane* lane, GNEUndoList* undoList, bool recomputeConnections);

    /**@brief transform lane to restricted lane
     * @param[in] vclass vehicle class to restrict
     * @param[in] lane The lane to be transformed
     * @param[in] undoList The undolist in which to mark changes
     */
    bool restrictLane(SUMOVehicleClass vclass, GNELane* lane, GNEUndoList* undoList);

    /**@brief add restricted lane to edge
     * @param[in] vclass vehicle class to restrict
     * @param[in] edge The edge in which insert restricted lane
     * @param[in] undoList The undolist in which to mark changes
     */
    bool addRestrictedLane(SUMOVehicleClass vclass, GNEEdge& edge, int index, GNEUndoList* undoList);

    /**@brief remove restricted lane
     * @param[in] vclass vehicle class to restrict
     * @param[in] edge the edge in which remove sidewalk
     * @param[in] undoList The undolist in which to mark changes
     */
    bool removeRestrictedLane(SUMOVehicleClass vclass, GNEEdge& edge, GNEUndoList* undoList);

    /**@brief split edge at position by inserting a new junction
     * @param[in] edge The edge to be split
     * @param[in] pos The position on which to insert the new junction
     * @return The new junction
     */
    GNEJunction* splitEdge(GNEEdge* edge, const Position& pos, GNEUndoList* undoList, GNEJunction* newJunction = 0);

    /**@brief split all edges at position by inserting one new junction
     * @param[in] edge The edge to be split
     * @param[in] oppositeEdge The oppositeEdge to be split
     * @param[in] pos The position on which to insert the new junction
     */
    void splitEdgesBidi(GNEEdge* edge, GNEEdge* oppositeEdge, const Position& pos, GNEUndoList* undoList);

    /**@brief reverse edge
     * @param[in] edge The edge to be reversed
     */
    void reverseEdge(GNEEdge* edge, GNEUndoList* undoList);

    /**@brief add reversed edge
     * @param[in] edge The edge of which to add the reverse
     * @return Return the new edge or 0
     */
    GNEEdge* addReversedEdge(GNEEdge* edge, GNEUndoList* undoList);

    /**@brief merge the given junctions
     * edges between the given junctions will be deleted
     * @param[in] moved The junction that will be eliminated
     * @param[in] target The junction that will be enlarged
     * @param[in] undoList The undo list with which to register changes
     */
    void mergeJunctions(GNEJunction* moved, GNEJunction* target, GNEUndoList* undoList);

    /// @brief retrieve all attribute carriers of Net
    const AttributeCarriers& getAttributeCarriers() const;

    /**@brief get junction by id
     * @param[in] id The id of the desired junction
     * @param[in] failHard Whether attempts to retrieve a nonexisting junction should result in an exception
     * @throws UnknownElement
     */
    GNEJunction* retrieveJunction(const std::string& id, bool failHard = true);

    /**@brief get edge by id
     * @param[in] id The id of the desired edge
     * @param[in] failHard Whether attempts to retrieve a nonexisting edge should result in an exception
     * @throws UnknownElement
     */
    GNEEdge* retrieveEdge(const std::string& id, bool failHard = true);

    /**@brief get edge by from and to GNEJunction
     * @param[in] id The id of the desired edge
     * @param[in] failHard Whether attempts to retrieve a nonexisting edge should result in an exception
     * @throws UnknownElement
     */
    GNEEdge* retrieveEdge(GNEJunction* from, GNEJunction* to, bool failHard = true);

    /**@brief get Polygon by id
    * @param[in] id The id of the desired polygon
    * @param[in] failHard Whether attempts to retrieve a nonexisting polygon should result in an exception
    * @throws UnknownElement
    */
    GNEPoly* retrievePolygon(const std::string& id, bool failHard = true) const;

    /**@brief get POI by id
    * @param[in] id The id of the desired POI
    * @param[in] failHard Whether attempts to retrieve a nonexisting POI should result in an exception
    * @throws UnknownElement
    */
    GNEPOI* retrievePOI(const std::string& id, bool failHard = true) const;

    /**@brief get Connection by id
    * @param[in] id The id of the desired Connection
    * @param[in] failHard Whether attempts to retrieve a nonexisting Connection should result in an exception
    * @throws UnknownElement
    */
    GNEConnection* retrieveConnection(const std::string& id, bool failHard = true) const;

    /**@brief return all connections
    * @param[in] onlySelected Whether to return only selected connections
    */
    std::vector<GNEConnection*> retrieveConnections(bool onlySelected = false) const;

    /**@brief get Crossing by id
    * @param[in] id The id of the desired Crossing
    * @param[in] failHard Whether attempts to retrieve a nonexisting Crossing should result in an exception
    * @throws UnknownElement
    */
    GNECrossing* retrieveCrossing(const std::string& id, bool failHard = true) const;

    /**@brief return all crossings
    * @param[in] onlySelected Whether to return only selected crossings
    */
    std::vector<GNECrossing*> retrieveCrossings(bool onlySelected = false) const;

    /**@brief get a single attribute carrier based on a GLID
    * @param[in] ids the GL IDs for which to retrive the AC
    * @param[in] failHard Whether attempts to retrieve a nonexisting AttributeCarrier should result in an exception
    * @throws InvalidArgument if GL ID doesn't have a associated Attribute Carrier
    */
    GNEAttributeCarrier* retrieveAttributeCarrier(const GUIGlID id, bool failHard = true);

    /**@brief get the attribute carriers based on Type
     * @param[in] type The GUI-type of the AC. SUMO_TAG_NOTHING returns all elements (Warning: bottleneck)
     */
    std::vector<GNEAttributeCarrier*> retrieveAttributeCarriers(SumoXMLTag type = SUMO_TAG_NOTHING);

    /**@brief return all edges
     * @param[in] onlySelected Whether to return only selected edges
     */
    std::vector<GNEEdge*> retrieveEdges(bool onlySelected = false);

    /**@brief return all lanes
     * @param[in] onlySelected Whether to return only selected lanes
     */
    std::vector<GNELane*> retrieveLanes(bool onlySelected = false);

    /**@brief get lane by id
     * @param[in] id The id of the desired lane
     * @param[in] failHard Whether attempts to retrieve a nonexisting lane should result in an exception
     * @param[in] checkVolatileChange Used by additionals after recomputing with volatile options.
     * @throws UnknownElement
     */
    GNELane* retrieveLane(const std::string& id, bool failHard = true, bool checkVolatileChange = false);

    /**@brief return all junctions
    * @param[in] onlySelected Whether to return only selected junctions
    */
    std::vector<GNEJunction*> retrieveJunctions(bool onlySelected = false);

    /**@brief return shape by type shapes
    * @param[in] shapeTag Type of shape.
    * @param[in] onlySelected Whether to return only selected junctions
    */
    std::vector<GNEShape*> retrieveShapes(SumoXMLTag shapeTag, bool onlySelected = false);

    /**@brief return all shapes
    * @param[in] onlySelected Whether to return only selected junctions
    */
    std::vector<GNEShape*> retrieveShapes(bool onlySelected = false);

    /// @brief inform that net has to be saved
    void requiereSaveNet(bool value);

    /// @brief return if net has to be saved
    bool isNetSaved() const;

    /**@brief save the network
     * @param[in] oc The OptionsCont which knows how and where to save
     */
    void save(OptionsCont& oc);

    /**@brief save plain xml representation of the network (and nothing else)
     * @param[in] oc The OptionsCont which knows how and where to save
     */
    void savePlain(OptionsCont& oc);

    /**@brief save log of joined junctions (and nothing else)
     * @param[in] oc The OptionsCont which knows how and where to save
     */
    void saveJoined(OptionsCont& oc);

    /// @brief Set the viewNet to be notified of network changes
    void setViewNet(GNEViewNet* viewNet);

    /// @brief add GL Object into net
    void addGLObjectIntoGrid(GUIGlObject* o);

    /// @brief add GL Object into net
    void removeGLObjectFromGrid(GUIGlObject* o);

    /// @brief updates the map and reserves new id
    void renameEdge(GNEEdge* edge, const std::string& newID);

    /// @brief updates the map and reserves new id
    void renameJunction(GNEJunction* junction, const std::string& newID);

    /// @brief modifies endpoins of the given edge
    void changeEdgeEndpoints(GNEEdge* edge, const std::string& newSourceID, const std::string& newDestID);

    /// @brief get view net
    GNEViewNet* getViewNet() const;

    /// @brief get all selected attribute carriers (or only relative to current supermode
    std::vector<GNEAttributeCarrier*> getSelectedAttributeCarriers(bool ignoreCurrentSupermode);

    /// @brief returns the tllcont of the underlying netbuilder
    NBTrafficLightLogicCont& getTLLogicCont();

    /// @brief returns the NBEdgeCont of the underlying netbuilder
    NBEdgeCont& getEdgeCont();

    /// @brief initialize GNEConnections
    void initGNEConnections();

    /// @brief recompute the network and update lane geometries
    void computeAndUpdate(OptionsCont& oc, bool volatileOptions);

    /**@brief trigger full netbuild computation
     * param[in] window The window to inform about delay
     * param[in] force Whether to force recomputation even if not needed
     * param[in] volatileOptions enable or disable volatile options
     * param[in] additionalPath path in wich additionals were saved before recomputing with volatile options
     * param[in] demandPath path in wich demand elements were saved before recomputing with volatile options
     */
    void computeNetwork(GNEApplicationWindow* window, bool force = false, bool volatileOptions = false, std::string additionalPath = "", std::string demandPath = "");

    /**@brief compute demand elements
     * param[in] window The window to inform about delay
     */
    void computeDemandElements(GNEApplicationWindow* window);

    /**@brief join selected junctions
     * @note difference to mergeJunctions:
     *  - can join more than 2
     *  - connected edges will keep their geometry (big junction shape is created)
     *  - no hirarchy: if any junction has a traffic light than the resuling junction will
     */
    bool joinSelectedJunctions(GNEUndoList* undoList);

    /// @brief clear invalid crossings
    bool cleanInvalidCrossings(GNEUndoList* undoList);

    /// @brief removes junctions that have no edges
    void removeSolitaryJunctions(GNEUndoList* undoList);

    /// @brief clean unused routes
    void cleanUnusedRoutes(GNEUndoList* undoList);

    /// @brief join routes
    void joinRoutes(GNEUndoList* undoList);

    /// @brief clean invalid demand elements
    void cleanInvalidDemandElements(GNEUndoList* undoList);

    /// @brief replace the selected junction by geometry node(s) and merge the edges
    void replaceJunctionByGeometry(GNEJunction* junction, GNEUndoList* undoList);

    /// @brief replace the selected junction by a list of junctions for each unique edge endpoint
    void splitJunction(GNEJunction* junction, bool reconnect, GNEUndoList* undoList);

    /// @brief clear junction's connections
    void clearJunctionConnections(GNEJunction* junction, GNEUndoList* undoList);

    /// @brief reset junction's connections
    void resetJunctionConnections(GNEJunction* junction, GNEUndoList* undoList);

    /**@brief trigger recomputation of junction shape and logic
     * param[in] window The window to inform about delay
     */
    void computeJunction(GNEJunction* junction);

    /// @brief inform the net about the need for recomputation
    void requireRecompute();

    /// @brief check if net has GNECrossings
    bool netHasGNECrossings() const;

    /// @brief get pointer to the main App
    FXApp* getApp();

    /// @brief get net builder
    NBNetBuilder* getNetBuilder() const;

    /// @brief add edge id to the list of explicit turnarounds
    void addExplicitTurnaround(std::string id);

    /// @brief remove edge id from the list of explicit turnarounds
    void removeExplicitTurnaround(std::string id);

    /// @name Functions related to Additional Items
    /// @{

    /**@brief Returns the named additional
     * @param[in] type tag with the type of additional
     * @param[in] id The id of the additional to return.
     * @param[in] failHard Whether attempts to retrieve a nonexisting additional should result in an exception
     */
    GNEAdditional* retrieveAdditional(SumoXMLTag type, const std::string& id, bool hardFail = true) const;

    /**@brief return all additionals
     * @param[in] onlySelected Whether to return only selected additionals
     */
    std::vector<GNEAdditional*> retrieveAdditionals(bool onlySelected = false) const;

    /**@brief Returns the number of additionals of the net
     * @param[in] type type of additional to count. SUMO_TAG_NOTHING will count all additionals
     * @return Number of additionals of the net
     */
    int getNumberOfAdditionals(SumoXMLTag type = SUMO_TAG_NOTHING) const;

    /**@brief update additional ID in container
    * @note this function is automatically called when user changes the ID of an additional
    */
    void updateAdditionalID(const std::string& oldID, GNEAdditional* additional);

    /// @brief inform that additionals has to be saved
    void requiereSaveAdditionals(bool value);

    /**@brief save additional elements of the network
    * @param[in] filename name of the file in wich save additionals
    */
    void saveAdditionals(const std::string& filename);

    /// @brief check if additionals are saved
    bool isAdditionalsSaved() const;

    /// @brief generate additional id
    std::string generateAdditionalID(SumoXMLTag type) const;

    /// @}

    /// @name Functions related to DemandElement Items
    /// @{

    /**@brief Returns the named demand element
     * @param[in] type tag with the type of demand element
     * @param[in] id The id of the demand element to return.
     * @param[in] failHard Whether attempts to retrieve a nonexisting demand element should result in an exception
     */
    GNEDemandElement* retrieveDemandElement(SumoXMLTag type, const std::string& id, bool hardFail = true) const;

    /**@brief return all demand elements
     * @param[in] onlySelected Whether to return only selected demand elements
     */
    std::vector<GNEDemandElement*> retrieveDemandElements(bool onlySelected = false) const;

    /**@brief Returns the number of demand elements of the net
     * @param[in] type type of demand element to count. SUMO_TAG_NOTHING will count all demand elements
     * @return Number of demand elements of the net
     */
    int getNumberOfDemandElements(SumoXMLTag type = SUMO_TAG_NOTHING) const;

    /**@brief update demand element ID in container
    * @note this function is automatically called when user changes the ID of an demand element
    */
    void updateDemandElementID(const std::string& oldID, GNEDemandElement* demandElement);

    /**@brief update demand element begin in container
    * @note this function is automatically called when user changes the begin/departure of an demand element
    */
    void updateDemandElementBegin(const std::string& oldBegin, GNEDemandElement* demandElement);

    /// @brief inform that demand elements has to be saved
    void requiereSaveDemandElements(bool value);

    /**@brief save demand element elements of the network
    * @param[in] filename name of the file in wich save demand elements
    */
    void saveDemandElements(const std::string& filename);

    /// @brief check if demand elements are saved
    bool isDemandElementsSaved() const;

    /// @brief generate demand element id
    std::string generateDemandElementID(const std::string& prefix, SumoXMLTag type) const;

    /// @}

    /// @name Functions related to Shapes
    /// @{

    /**@brief Builds a special polygon used for edit Junctions's shapes
     * @param[in] netElement GNENetElement to be edited
     * @param[in] shape shape to be edited
     * @param[in] fill enable or disable fill polygon
     * @param[in] col The color for drawing the polygon
     * @throw processError if shape is empty
     * @return created GNEPoly
     */
    GNEPoly* addPolygonForEditShapes(GNENetElement* netElement, const PositionVector& shape, bool fill, RGBColor col);

    /// @brief remove Polygon for edit shapes
    void removePolygonForEditShapes(GNEPoly* polygon);

    /// @brief generate Shape ID
    std::string generateShapeID(SumoXMLTag shapeTag) const;

    /// @brief change Shape ID
    void changeShapeID(GNEShape* s, const std::string& OldID);

    /// @brief get number of shapes
    int getNumberOfShapes() const;
    /// @}

    /// @name Functions related to TLS Programs
    /// @{
    /// @brief inform that TLS Programs has to be saved
    void requiereSaveTLSPrograms();

    /**@brief save TLS Programs elements of the network
     * @param[in] filename name of the file in wich save TLS Programs
     */
    void saveTLSPrograms(const std::string& filename);

    /// @brief get number of TLS Programs
    int getNumberOfTLSPrograms() const;
    /// @}

    /// @name Functions related to Enable or disable update geometry of elements after insertio
    /// @{
    /// @brief enable update geometry of elements after inserting or removing an element in net
    void enableUpdateGeometry();

    /// @brief disable update geometry of elements after inserting or removing an element in net
    void disableUpdateGeometry();

    /// @brief check if update geometry after inserting or removing has to be updated
    bool isUpdateGeometryEnabled() const;

    /// @}

protected:
    /// @brief the rtree which contains all GUIGlObjects (so named for historical reasons)
    SUMORTree myGrid;

    /// @brief The viewNet to be notofied of about changes
    GNEViewNet* myViewNet;

    /// @brief The internal netbuilder
    NBNetBuilder* myNetBuilder;

    /// @brief AttributeCarriers of net
    AttributeCarriers myAttributeCarriers;

    /// @name ID Suppliers for newly created edges and junctions
    // @{
    IDSupplier myEdgeIDSupplier;
    IDSupplier myJunctionIDSupplier;
    // @}

    /// @brief list of edge ids for which turn-arounds must be added explicitly
    std::set<std::string> myExplicitTurnarounds;

    /// @brief whether the net needs recomputation
    bool myNeedRecompute;

    /// @brief Flag to check if net has to be saved
    bool myNetSaved;

    /// @brief Flag to check if additionals has to be saved
    bool myAdditionalsSaved;

    /// @brief Flag to check if shapes has to be saved
    bool myTLSProgramsSaved;

    /// @brief Flag to check if demand elements has to be saved
    bool myDemandElementsSaved;

    /// @brief Flag to enable or disable update geometry of elements after inserting or removing element in net
    bool myUpdateGeometryEnabled;

    /// @name Insertion and erasing of GNEAdditionals items
    /// @{

    /// @brief return true if additional exist (use pointer instead ID)
    bool additionalExist(GNEAdditional* additional) const;

    /**@brief Insert a additional element int GNENet container.
     * @throw processError if route was already inserted
     */
    void insertAdditional(GNEAdditional* additional);

    /**@brief delete additional element of GNENet container
     * @throw processError if additional wasn't previously inserted
     */
    bool deleteAdditional(GNEAdditional* additional, bool updateViewAfterDeleting);

    /// @}

    /// @name Insertion and erasing of GNEDemandElements items
    /// @{

    /// @brief return true if demand element exist (use pointer instead ID)
    bool demandElementExist(GNEDemandElement* demandElement) const;

    /**@brief Insert a demand element element int GNENet container.
     * @throw processError if route was already inserted
     */
    void insertDemandElement(GNEDemandElement* demandElement);

    /**@brief delete demand element element of GNENet container
     * @throw processError if demand element wasn't previously inserted
     */
    bool deleteDemandElement(GNEDemandElement* demandElement, bool updateViewAfterDeleting);

    /// @}

private:
    /// @brief Init Junctions and edges
    void initJunctionsAndEdges();

    /// @brief inserts a single junction into the net and into the underlying netbuild-container
    void insertJunction(GNEJunction* junction);

    /// @brief inserts a single edge into the net and into the underlying netbuild-container
    void insertEdge(GNEEdge* edge);

    /// @brief registers a junction with GNENet containers
    GNEJunction* registerJunction(GNEJunction* junction);

    /// @brief registers an edge with GNENet containers
    GNEEdge* registerEdge(GNEEdge* edge);

    /// @brief deletes a single junction
    void deleteSingleJunction(GNEJunction* junction, bool updateViewAfterDeleting);

    /// @brief deletes a single edge
    void deleteSingleEdge(GNEEdge* edge, bool updateViewAfterDeleting);

    /// @brief insert shape
    void insertShape(GNEShape* shape, bool updateViewAfterDeleting);

    /// @brief remove created shape (but NOT delete)
    void removeShape(GNEShape* shape, bool updateViewAfterDeleting);

    /// @brief notify myViewNet
    void update();

    /// @brief reserve edge ID (To avoid duplicates)
    void reserveEdgeID(const std::string& id);

    /// @brief reserve junction ID (To avoid duplicates)
    void reserveJunctionID(const std::string& id);

    /// @brief return true if there are already a Junction in the given position, false in other case
    bool checkJunctionPosition(const Position& pos);

    /// @brief save additionals after confirming invalid objects
    void saveAdditionalsConfirmed(const std::string& filename);

    /// @brief save demand elements after confirming invalid objects
    void saveDemandElementsConfirmed(const std::string& filename);

    static void replaceInListAttribute(GNEAttributeCarrier* ac, SumoXMLAttr key, const std::string& which, const std::string& by, GNEUndoList* undoList);

    /// @brief the z boundary (stored in the x-coordinate), values of 0 are ignored
    Boundary myZBoundary;

    /// @brief marker for whether the z-boundary is initialized
    static const double Z_INITIALIZED;

    /// @brief map with the Edges and their number of lanes
    std::map<std::string, int> myEdgesAndNumberOfLanes;

    /// @brief flag used to indicate if shaped created can be undo
    bool myAllowUndoShapes;

    /// @brief class for GNEChange_ReplaceEdgeInTLS
    class GNEChange_ReplaceEdgeInTLS : public GNEChange {
        FXDECLARE_ABSTRACT(GNEChange_ReplaceEdgeInTLS)

    public:
        /// @brief constructor
        GNEChange_ReplaceEdgeInTLS(NBTrafficLightLogicCont& tllcont, NBEdge* replaced, NBEdge* by) :
            GNEChange(0, true),
            myTllcont(tllcont), myReplaced(replaced), myBy(by) { }

        /// @bief destructor
        ~GNEChange_ReplaceEdgeInTLS() {};

        /// @brief undo name
        FXString undoName() const {
            return "Redo replace in TLS";
        }

        /// @brief get Redo name
        FXString redoName() const {
            return "Undo replace in TLS";
        }

        /// @brief undo action
        void undo() {
            myTllcont.replaceRemoved(myBy, -1, myReplaced, -1);
        }

        /// @brief redo action
        void redo() {
            myTllcont.replaceRemoved(myReplaced, -1, myBy, -1);
        }

        /// @brief wether original and new value differ
        bool trueChange() {
            return myReplaced != myBy;
        }

    private:
        /// @brief container for traffic light logic
        NBTrafficLightLogicCont& myTllcont;

        /// @brief replaced NBEdge
        NBEdge* myReplaced;

        /// @brief replaced by NBEdge
        NBEdge* myBy;
    };

};

#endif

/****************************************************************************/
