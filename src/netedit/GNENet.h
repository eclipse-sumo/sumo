/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
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
// Workflow (rough draft)
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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utility>
#include <utils/gui/globjects/GLIncludes.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/Position.h>
#include <foreign/rtree/SUMORTree.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIShapeContainer.h>
#include <utils/common/StringUtils.h>
#include <utils/common/RGBColor.h>
#include <utils/common/IDSupplier.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include "GNEChange.h"


// ===========================================================================
// class declarations
// ===========================================================================

class GNEAdditional;
class GNEApplicationWindow;
class GNEAttributeCarrier;
class GNECalibratorFlow;
class GNECalibratorRoute;
class GNECalibratorVehicleType;
class GNEConnection;
class GNECrossing;
class GNEEdge;
class GNEJunction;
class GNELane;
class GNENetElement;
class GNEPOI;
class GNEPOILane;
class GNEPoly;
class GNERerouterInterval;
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
    friend class GNEChange_Junction;
    friend class GNEChange_Edge;
    friend class GNEChange_Lane;
    friend class GNEChange_Connection;
    friend class GNEChange_Shape;
    friend class GNEChange_CalibratorItem;
    friend class GNEChange_Additional;

public:
    /// @name color of selected objects
    /// @{
    /// @brief color of selection
    static const RGBColor selectionColor;

    /// @brief color of selected lane
    static const RGBColor selectedLaneColor;

    /// @brief color of selected connection
    static const RGBColor selectedConnectionColor;

    /// @brief color of selected additionals
    static const RGBColor selectedAdditionalColor;
    /// @}

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
    * @param[in] shape The shape of the polygon
    * @param[in] geo specify if shape was loaded as GEO coordinate
    * @param[in] fill Whether the polygon shall be filled
    * @return whether the polygon could be added
    */
    bool addPolygon(const std::string& id, const std::string& type, const RGBColor& color, double layer,
                    double angle, const std::string& imgFile, const PositionVector& shape, bool fill,
                    bool geo, bool ignorePruning = false);

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
    * @param[in] width The width of the POI image
    * @param[in] height The height of the POI image
    * @return whether the poi could be added
    */
    bool addPOI(const std::string& id, const std::string& type, const RGBColor& color, const Position& pos, bool geo,
                const std::string& lane, double posOverLane, double posLat, double layer, double angle,
                const std::string& imgFile, double width, double height, bool ignorePruning = false);
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
    bool addSRestrictedLane(SUMOVehicleClass vclass, GNEEdge& edge, GNEUndoList* undoList);

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
     * @param[in] edges The edges to be split
     * @param[in] pos The position on which to insert the new junction
     */
    void splitEdgesBidi(const std::set<GNEEdge*>& edges, const Position& pos, GNEUndoList* undoList);

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

    /**@brief get POILane by id
    * @param[in] id The id of the desired POILane
    * @param[in] failHard Whether attempts to retrieve a nonexisting POILane should result in an exception
    * @throws UnknownElement
    */
    GNEPOILane* retrievePOILane(const std::string& id, bool failHard = true) const;

    /**@brief get a single attribute carrier based on a GLID
    * @param[in] ids the GL IDs for which to retrive the AC
    * @param[in] failHard Whether attempts to retrieve a nonexisting AttributeCarrier should result in an exception
    * @throws InvalidArgument if GL ID doesn't have a associated Attribute Carrier
    */
    GNEAttributeCarrier* retrieveAttributeCarrier(const GUIGlID id, bool failHard = true);

    /**@brief get the attribute carriers based on GlIDs
     * @param[in] ids The set of ids for which to retrive the ACs
     * @param[in] type The GUI-type of the objects with the given ids.
     * @throws InvalidArgument if any given id does not match the declared type
     */
    std::vector<GNEAttributeCarrier*> retrieveAttributeCarriers(const std::set<GUIGlID>& ids, GUIGlObjectType type);

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

    /**@brief return all shapes
    * @param[in] shapeTag Type of shape. SUMO_TAG_NOTHING returns all shapes
    * @param[in] onlySelected Whether to return only selected junctions
    */
    std::vector<GNEShape*> retrieveShapes(SumoXMLTag shapeTag = SUMO_TAG_NOTHING, bool onlySelected = false);

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

    /// @brief refreshes boundary information for o and update
    void refreshElement(GUIGlObject* o);

    /// @brief generate an ID for vaporizers
    std::string generateVaporizerID() const;

    /// @brief updates the map and reserves new id
    void renameEdge(GNEEdge* edge, const std::string& newID);

    /// @brief updates the map and reserves new id
    void renameJunction(GNEJunction* junction, const std::string& newID);

    /// @brief modifies endpoins of the given edge
    void changeEdgeEndpoints(GNEEdge* edge, const std::string& newSourceID, const std::string& newDestID);

    /// @brief get view net
    GNEViewNet* getViewNet() const;

    /// @brief returns the tllcont of the underlying netbuilder
    NBTrafficLightLogicCont& getTLLogicCont();

    /**@brief get ids of currently active objects
     * @param[in] type If type != GLO_MAX, get active ids of that type, otherwise get all active ids
     */
    std::set<GUIGlID> getGlIDs(GUIGlObjectType type = GLO_MAX);

    /// @brief initialize GNEConnections
    void initGNEConnections();

    /// @brief recompute the network and update lane geometries
    void computeAndUpdate(OptionsCont& oc, bool volatileOptions);

    /**@brief trigger full netbuild computation
     * param[in] window The window to inform about delay
     * param[in] force Whether to force recomputation even if not needed
     * param[in] volatileOptions enable or disable volatile options
     * param[in] additionalPath path in wich additionals were saved before recomputing with volatile options
     * param[in] shapePath path in wich shapes were saved before recomputing with volatile options
     */
    void computeEverything(GNEApplicationWindow* window, bool force = false, bool volatileOptions = false, std::string additionalPath = "", std::string shapePath = "");

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

    /// @brief replace the selected junction by geometry node(s) and merge the edges
    void replaceJunctionByGeometry(GNEJunction* junction, GNEUndoList* undoList);

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
     * @param[in] id The id of the additional to return.
     * @param[in] failHard Whether attempts to retrieve a nonexisting additional should result in an exception
     */
    GNEAdditional* retrieveAdditional(const std::string& id, bool hardFail = true) const;

    /**@brief return all additionals
     * @param[in] onlySelected Whether to return only selected additionals
     */
    std::vector<GNEAdditional*> retrieveAdditionals(bool onlySelected = false);

    /**@brief Returns the named additional
     * @param[in] type tag with the type of additional
     * @param[in] id The id of the additional to return.
     * @return The named additional, or 0 if don't exists
     */
    GNEAdditional* getAdditional(SumoXMLTag type, const std::string& id) const;

    /**@brief get vector with additionals
     * @param[in] type type of additional to get. SUMO_TAG_NOTHING will get all additionals
     * @return vector with pointers to additionals.
     */
    std::vector<GNEAdditional*> getAdditionals(SumoXMLTag type = SUMO_TAG_NOTHING) const;

    /* @brief retrieve Rerouter Interval
     * @param rerouterIntervalID ID of rerouter interval
     * @param rerouter interval if was found, or NULL in other case
     */
    GNERerouterInterval* getRerouterInterval(const std::string& rerouterIntervalID) const;

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
    void requiereSaveAdditionals();

    /**@brief save additional elements of the network
    * @param[in] filename name of the file in wich save additionals
    */
    void saveAdditionals(const std::string& filename);

    /// @}

    /// @name Functions related to Calibrator Items
    /// @note all three duplicates functions will be unified using GNERoute class
    /// @{

    /**@brief Returns the named calibrator route
    * @param[in] id The id of the calibrator route to return.
    * @param[in] failHard Whether attempts to retrieve a nonexisting calibrator route should result in an exception
    */
    GNECalibratorRoute* retrieveCalibratorRoute(const std::string& id, bool hardFail = true) const;

    /**@brief Returns the named calibrator vehicle type
    * @param[in] id The id of the calibrator vehicle type to return.
    * @param[in] failHard Whether attempts to retrieve a nonexisting calibrator vehicle type should result in an exception
    */
    GNECalibratorVehicleType* retrieveCalibratorVehicleType(const std::string& id, bool hardFail = true) const;

    /**@brief Returns the named calibrator flow
    * @param[in] id The id of the calibrator flow to return.
    * @param[in] failHard Whether attempts to retrieve a nonexisting calibrator flow should result in an exception
    */
    GNECalibratorFlow* retrieveCalibratorFlow(const std::string& id, bool hardFail = true) const;

    /// @brief generate a new Calibrator Route ID
    std::string generateCalibratorRouteID() const;

    /// @brief generate a new Calibrator Vehicle Type ID
    std::string generateCalibratorVehicleTypeID() const;

    /// @brief generate a new Calibrator Flow ID
    std::string generateCalibratorFlowID() const;

    /// @brief change Calibrator Route ID
    void changeCalibratorRouteID(GNECalibratorRoute* route, const std::string& oldID);

    /// @brief change Calibrator Vehicle Type ID
    void changeCalibratorVehicleTypeID(GNECalibratorVehicleType* vehicleType, const std::string& oldID);

    /// @brief change Calibrator Flow ID
    void changeCalibratorFlowID(GNECalibratorFlow* flow, const std::string& oldID);

    /// @}

    /// @name Functions related to Shapes
    /// @{

    /**@brief Builds a special polygon used for edit Junctions's shapes
     * @param[in] netElement GNENetElement to be edited
     * @param[in] shape shape to be edited
     * @param[in] fill enable or disable fill polygon
     * @throw processError if shape is empty
     * @return created GNEPoly
     */
    GNEPoly* addPolygonForEditShapes(GNENetElement* netElement, const PositionVector& shape, bool fill);

    /// @brief remove Polygon for edit shapes
    void removePolygonForEditShapes(GNEPoly* polygon);

    /// @brief generate Shape ID
    std::string generateShapeID(SumoXMLTag shapeTag) const;

    /// @brief change Shape ID
    void changeShapeID(GNEShape* s, const std::string& OldID);

    /// @brief inform that shapes has to be saved
    void requiereSaveShapes();

    /**@brief save shapes elements of the network
     * @param[in] filename name of the file in wich save shapes
     */
    void saveShapes(const std::string& filename);

    /// @brief get number of shapes
    int getNumberOfShapes() const;
    /// @}

protected:
    /// @brief the rtree which contains all GUIGlObjects (so named for historical reasons)
    SUMORTree myGrid;

    /// @brief The viewNet to be notofied of about changes
    GNEViewNet* myViewNet;

    /// @brief The internal netbuilder
    NBNetBuilder* myNetBuilder;

    /// @brief map with the name and pointer to junctions of net
    std::map<std::string, GNEJunction*> myJunctions;

    /// @brief map with the name and pointer to edges of net
    std::map<std::string, GNEEdge*> myEdges;

    /// @brief map with the name and pointer to additional elements of net
    std::map<std::pair<std::string, SumoXMLTag>, GNEAdditional*> myAdditionals;

    /// @brief map with the name and pointer to Calibrator Routes of net
    std::map<std::string, GNECalibratorRoute*> myCalibratorRoutes;

    /// @brief map with the name and pointer to Calibrator Vehicle Types of net
    std::map<std::string, GNECalibratorVehicleType*> myCalibratorVehicleTypes;

    /// @brief map with the name and pointer to Calibrator Flows of net
    std::map<std::string, GNECalibratorFlow*> myCalibratorFlows;

    /// @name ID Suppliers for newly created edges and junctions
    // @{
    IDSupplier myEdgeIDSupplier;
    IDSupplier myJunctionIDSupplier;
    // @}

    /// @brief list of edge ids for which turn-arounds must be added explicitly
    std::set<std::string> myExplicitTurnarounds;

    /// @brief whether the net needs recomputation
    bool myNeedRecompute;

    /// @brief Flag to check if additionals has to be saved
    bool myAdditionalsSaved;

    /// @brief Flag to check if shapes hast o be saved
    bool myShapesSaved;

    /// @name Insertion and erasing of GNEAdditionals items
    /// @{

    /**@brief Insert a additional element int GNENet container.
     * @throw processError if route was already inserted
     */
    void insertAdditional(GNEAdditional* additional);

    /**@brief delete additional element of GNENet container
     * @throw processError if additional wasn't previously inserted
     */
    void deleteAdditional(GNEAdditional* additional);

    /// @}

    /// @name Insertion and erasing of GNECalibrator items
    /// @{

    /**@brief insert Calibrator Route in net
     * @throw processError if route was already inserted
     */
    void insertCalibratorRoute(GNECalibratorRoute* route);

    /**@brief delete Calibrator Route in net
    * @throw processError if route wasn't previously inserted
    */
    void deleteCalibratorRoute(GNECalibratorRoute* route);

    /**@brief insert Calibrator Flow in net
    * @throw processError if flow was already inserted
    */
    void insertCalibratorFlow(GNECalibratorFlow* flow);

    /**@brief delete Calibrator Flow in net
    * @throw processError if flow wasn't previously inserted
    */
    void deleteCalibratorFlow(GNECalibratorFlow* flow);

    /**@brief insert Calibrator VehicleType in net
    * @throw processError if vehicleType was already inserted
    */
    void insertCalibratorVehicleType(GNECalibratorVehicleType* vehicleType);

    /**@brief delete Calibrator VehicleType in net
    * @throw processError if vehicleType wasn't previously inserted
    */
    void deleteCalibratorVehicleType(GNECalibratorVehicleType* vehicleType);

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
    void deleteSingleJunction(GNEJunction* junction);

    /// @brief deletes a single edge
    void deleteSingleEdge(GNEEdge* edge);

    /// @brief insert shape
    void insertShape(GNEShape* shape);

    /// @brief remove created shape (but NOT delete)
    void removeShape(GNEShape* shape);

    /// @brief notify myViewNet
    void update();

    /// @brief reserve edge ID (To avoid duplicates)
    void reserveEdgeID(const std::string& id);

    /// @brief reserve junction ID (To avoid duplicates)
    void reserveJunctionID(const std::string& id);

    /* @brief helper function for changing the endpoints of a junction
     * @param[in] keepEndpoints Whether to keep the original edge endpoints (influences junction shape)
     */
    void remapEdge(GNEEdge* oldEdge, GNEJunction* from, GNEJunction* to, GNEUndoList* undoList, bool preserveShapeStart, bool preserveShapeEnd);

    /// @brief return true if there are already a Junction in the given position, false in other case
    bool checkJunctionPosition(const Position& pos);

    static void replaceInListAttribute(GNEAttributeCarrier* ac, SumoXMLAttr key, const std::string& which, const std::string& by, GNEUndoList* undoList);

    /// @brief the z boundary (stored in the x-coordinate), values of 0 are ignored
    Boundary myZBoundary;

    /// @brief marker for whether the z-boundary is initialized
    static const double Z_INITIALIZED;

    /// @brief map with the Edges and their number of lanes
    std::map<std::string, int> myEdgesAndNumberOfLanes;

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
