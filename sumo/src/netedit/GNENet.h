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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
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
#include "GNEDetectorE3.h"


// ===========================================================================
// class declarations
// ===========================================================================
class GNEApplicationWindow;
class GNEAttributeCarrier;
class GNEEdge;
class GNELane;
class GNEJunction;
class GNEUndoList;
class GNEAdditional;
class GNEBusStop;
class GNEChargingStation;
class GNEDetectorE1;
class GNEDetectorE2;
class GNEDetectorE3;
class GNEDetectorE3EntryExit;
class GNEConnection;
class GNECrossing;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNENet
 * @brief A NBNetBuilder extended by visualisation and editing capabilities
 */
class GNENet : public GUIGlObject {

    /// @brief declare friend class
    friend class GNEChange_Junction;
    friend class GNEChange_Edge;
    friend class GNEChange_Connection;

public:
    /// @brief color of selection
    static const RGBColor selectionColor;

    /// @brief color of selected lane
    static const RGBColor selectedLaneColor;

    /// @brief color of selected connection
    static const RGBColor selectedConnectionColor;

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

    /// @brief returns the bounder of the network
    const Boundary& getBoundary() const;

    /// @brief Returns the RTree used for visualisation speed-up
    /// @return The visualisation speed-up
    SUMORTree& getVisualisationSpeedUp();

    /// @brief Returns the RTree used for visualisation speed-up
    /// @return The visualisation speed-up
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
     * @return The newly created edge or 0 if no edge was created
     */
    GNEEdge* createEdge(GNEJunction* src, GNEJunction* dest, GNEEdge* tpl, GNEUndoList* undoList,
                        const std::string& suggestedName = "", bool wasSplit = false, bool allowDuplicateGeom = false);

    /**@brief removes junction and all incident edges
     * @param[in] junction The junction to be removed
     * @param[in] undoList The undolist in which to mark changes
     */
    void deleteJunction(GNEJunction* junction, GNEUndoList* undoList);

    /**@brief removes edge
     * @param[in] edge The edge to be removed
     * @param[in] undoList The undolist in which to mark changes
     */
    void deleteEdge(GNEEdge* edge, GNEUndoList* undoList);

    /**@brief removes lane
     * @param[in] lane The lane to be removed
     * @param[in] undoList The undolist in which to mark changes
     */
    void deleteLane(GNELane* lane, GNEUndoList* undoList);

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

    /**@brief duplicates lane
     * @param[in] lane The lane to be duplicated
     * @param[in] undoList The undolist in which to mark changes
     */
    void duplicateLane(GNELane* lane, GNEUndoList* undoList);


    /**@brief transform lane to restricted lane
     * @param[in] vclass vehicle class to restrict
     * @param[in] lane The lane to be transformed
     * @param[in] undoList The undolist in which to mark changes
     */
    bool restrictLane(SUMOVehicleClass vclass, GNELane* lane, GNEUndoList* undoList);

    /**@brief revert prevously restricted lane
     * @param[in] lane The lane to be unrestricted
     * @param[in] undoList The undolist in which to mark changes
     */
    bool revertLaneRestriction(GNELane* lane, GNEUndoList* undoList);

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

    /**@brief removes geometry when pos is close to a geometry node, deletes
     * the whole edge otherwise
     * @param[in] edge The edge to be removed
     * @param[in] pos The position that was clicked upon
     * @param[in] undoList The undolist in which to mark changes
     */
    void deleteGeometryOrEdge(GNEEdge* edge, const Position& pos, GNEUndoList* undoList);

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

    /**@brief get the attribute carriers based on GlIDs
     * @param[in] ids The set of ids for which to retrive the ACs
     * @param[in] type The GUI-type of the objects with the given ids
     * @throws InvalidArgument if any given id does not match the declared type
     */
    std::vector<GNEAttributeCarrier*> retrieveAttributeCarriers(const std::set<GUIGlID>& ids, GUIGlObjectType type);

    /**@brief return all edges
     * @param[in] onlySelected Whether to return only selected edges
     * */
    std::vector<GNEEdge*> retrieveEdges(bool onlySelected = false);

    /**@brief return all lanes
     * @param[in] onlySelected Whether to return only selected lanes
     * */
    std::vector<GNELane*> retrieveLanes(bool onlySelected = false);

    /**@brief get lane by id
     * @param[in] id The id of the desired lane
     * @param[in] failHard Whether attempts to retrieve a nonexisting lane should result in an exception
     * @throws UnknownElement
    */
    GNELane* retrieveLane(const std::string& id, bool failHard = true);

    /**@brief return all junctions
     * @param[in] onlySelected Whether to return only selected junctions
     * */
    std::vector<GNEJunction*> retrieveJunctions(bool onlySelected = false);

    /**@brief save the network
     * @param[in] oc The OptionsCont which knows how and where to save
     */
    void save(OptionsCont& oc);

    /**@brief save additional elements of the network
     * @param[in] filename name of the file in wich save additionals
     */
    void saveAdditionals(const std::string& filename);

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

    /// @brief refreshes boundary information of an additional after a geometry update
    /// @note only non removed additional will be refresh
    void refreshAdditional(GNEAdditional* additional);

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

    /* @brief get ids of currently active objects
     * param[in] type If type != GLO_MAX, get active ids of that type, otherwise get all active ids
     */
    std::set<GUIGlID> getGlIDs(GUIGlObjectType type = GLO_MAX);

    /// @brief initialize GNEConnections
    void initGNEConnections();

    /// @brief recompute the network and update lane geometries
    void computeAndUpdate(OptionsCont& oc);

    /* @brief trigger full netbuild computation
     * param[in] window The window to inform about delay
     * param[in] force Whether to force recomputation even if not needed
     */
    void computeEverything(GNEApplicationWindow* window, bool force = false);

    /* @brief join selected junctions
     * @note difference to mergeJunctions:
     *  - can join more than 2
     *  - connected edges will keep their geometry (big junction shape is created)
     *  - no hirarchy: if any junction has a traffic light than the resuling junction will
     */
    void joinSelectedJunctions(GNEUndoList* undoList);

    /// @brief removes junctions that have no edges
    void removeSolitaryJunctions(GNEUndoList* undoList);

    /// @brief replace the selected junction by geometry node(s) and merge the edges
    void replaceJunctionByGeometry(GNEJunction* junction, GNEUndoList* undoList);

    /* @brief trigger recomputation of junction shape and logic
     * param[in] window The window to inform about delay
     */
    void computeJunction(GNEJunction* junction);

    /// @brief inform the net about the need for recomputation
    void requireRecompute();

    /// @brief check if net has crossings
    bool netHasCrossings() const;

    /// @brief get pointer to the main App
    FXApp* getApp();

    /// @brief get net builder
    NBNetBuilder* getNetBuilder() const;

    /// @brief add edge id to the list of explicit turnarounds
    void addExplicitTurnaround(std::string id);

    /// @brief remove edge id from the list of explicit turnarounds
    void removeExplicitTurnaround(std::string id);

    /// @brief move all selected junctions and edges
    /// @note: inner points of an edge will only be modified if the edge and its endpoints are selected
    void moveSelection(const Position& moveSrc, const Position& moveDest);

    /// @brief register changes to junction and edge positions with the undoList
    void finishMoveSelection(GNEUndoList* undoList);

    /// @brief get shape container
    ShapeContainer& getShapeContainer();

    /**@brief Insert a additional element previously created in GNEAdditionalHandler
     * @param[in] additional pointer to the additional element to add
     * @param[in] hardFail enable or disable exception if additional to insert is duplicated
     */
    void insertAdditional(GNEAdditional* additional, bool hardFail = true);

    /**@brief delete additional element previously inserted
     * @param[in] additional The additional element to remove
     */
    void deleteAdditional(GNEAdditional* additional);

    /// @brief update additional ID in container
    /// @note this function is automatically called when user changes the ID of an additional
    void updateAdditionalID(const std::string& oldID, GNEAdditional* additional);

    /**@brief Returns the named additional
     * @param[in] id The id of the additional to return.
     * @param[in] hardFail enable or disable exception if additional to insert is duplicated
     */
    GNEAdditional* retrieveAdditional(const std::string& idl, bool hardFail = true) const;

    /**@brief Returns the named additional
     * @param[in] type tag with the type of additional
     * @param[in] id The id of the additional to return.
     * @return The named additional, or 0 if don't exists
     */
    GNEAdditional* getAdditional(SumoXMLTag type, const std::string& id) const;

    /**@brief Returns the additional close to the given position
     * @param[in] type tag with the type of additional
     * @param[in] lane the lane of the additional to return.
     * @param[in] pos the position of the additional to return.
     * @return The additional id on the location, or "" if don't exists
     */
    std::string getAdditionalID(SumoXMLTag type, const GNELane* lane, const SUMOReal pos) const;

    /**@brief get vector with additionals
     * @param[in] type type of additional to get. SUMO_TAG_NOTHING will get all additionals
     * @return vector with pointers to additionals.
     */
    std::vector<GNEAdditional*> getAdditionals(SumoXMLTag type = SUMO_TAG_NOTHING);

    /**@brief Returns the number of additionals of the net
     * @param[in] type type of additional to count. SUMO_TAG_NOTHING will count all additionals
     * @return Number of additionals of the net
     */
    int getNumberOfAdditionals(SumoXMLTag type = SUMO_TAG_NOTHING);

protected:
    /// @brief the rtree which contains all GUIGlObjects (so named for historical reasons)
    SUMORTree myGrid;

    /// @brief The viewNet to be notofied of about changes
    GNEViewNet* myViewNet;

    /// @brief The internal netbuilder
    NBNetBuilder* myNetBuilder;

    /// @name internal GNE components
    /// @{
    typedef std::map<std::string, GNEJunction*> GNEJunctions;
    typedef std::map<std::string, GNEEdge*> GNEEdges;
    typedef std::map<std::pair<std::string, SumoXMLTag>, GNEAdditional*> GNEAdditionals;
    // @}

    /// @brief map with the name and pointer to junctions of net
    GNEJunctions myJunctions;

    /// @brief map with the name and pointer to edges of net
    GNEEdges myEdges;

    /// @brief map with the name and pointer to additional elements of net
    GNEAdditionals myAdditionals;

    /// @name ID Suppliers for newly created edges and junctions
    // @{
    IDSupplier myEdgeIDSupplier;
    IDSupplier myJunctionIDSupplier;
    // @}

    /// @brief the container for additional pois and polygons
    GUIShapeContainer myShapeContainer;

    /// @brief list of edge ids for which turn-arounds must be added explicitly
    std::set<std::string> myExplicitTurnarounds;

    /// @brief whether the net needs recomputation
    bool myNeedRecompute;

private:
    /// @brief Initialises the detector wrappers
    void initDetectors();

    /// @brief Initialises the tl-logic map and wrappers
    void initTLMap();

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

    /// @brief notify myViewNet
    void update();

    /// @brief reserve edge ID (To avoid duplicates)
    void reserveEdgeID(const std::string& id);

    /// @brief reserve junction ID (To avoid duplicates)
    void reserveJunctionID(const std::string& id);

    /* @brief helper function for changing the endpoints of a junction
     * @param[in] keepEndpoints Whether to keep the original edge endpoints (influences junction shape)
     */
    void remapEdge(GNEEdge* oldEdge, GNEJunction* from, GNEJunction* to, GNEUndoList* undoList, bool keepEndpoints = false);

    /// @brief the z boundary (stored in the x-coordinate), values of 0 are ignored
    Boundary myZBoundary;

    /// @brief marker for whether the z-boundary is initialized
    static const SUMOReal Z_INITIALIZED;
};

#endif

/****************************************************************************/
