/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNENet.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
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
#pragma once
#include <config.h>

#include "GNENetHelper.h"
#include "GNEPathManager.h"

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNENet
 * @brief A NBNetBuilder extended by visualisation and editing capabilities
 */
class GNENet : public GUIGlObject {

public:
    /**@brief Constructor
     * @param[in] netbuilder the netbuilder which may already have been filled
     * GNENet becomes responsible for cleaning this up
     **/
    GNENet(NBNetBuilder* netBuilder);

    /// @brief Destructor
    ~GNENet();

    /// @brief get all attribute carriers used in this net
    GNENetHelper::AttributeCarriers* getAttributeCarriers() const;

    /// @brief get saving status
    GNENetHelper::SavingStatus* getSavingStatus() const;

    /// @brief get path manager
    GNEPathManager* getPathManager();

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

    /// @brief expand boundary
    void expandBoundary(const Boundary& newBoundary);

    /// @brief Returns the Z boundary (stored in the x() coordinate) values of 0 do not affect the boundary
    const Boundary& getZBoundary() const;

    /// @brief add Z in net boundary
    void addZValueInBoundary(const double z);

    /**@brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;

    /// @}

    /// @brief returns the bounder of the network
    const Boundary& getBoundary() const;

    /**@brief Returns the RTree used for visualisation speed-up
     * @return The visualisation speed-up
     * @note only use in GNEViewNet constructor
     */
    SUMORTree& getGrid();

    /// @brief et edges and number of lanes
    const std::map<std::string, int>& getEdgesAndNumberOfLanes() const;

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
     * @param[in] edgeTemplate The template edge from which to copy attributes (including lane attrs)
     * @param[in] undoList The undoList in which to mark changes
     * @param[in] suggestedName
     * @param[in] wasSplit Whether the edge was created from a split
     * @param[in] allowDuplicateGeom Whether to create the edge even though another edge with the same geometry exists
     * @param[in] recomputeConnections Whether connections on the affected junctions must be recomputed
     * @return The newly created edge or 0 if no edge was created
     */
    GNEEdge* createEdge(GNEJunction* src, GNEJunction* dest, GNEEdge* edgeTemplate, GNEUndoList* undoList,
                        const std::string& suggestedName = "", bool wasSplit = false, bool allowDuplicateGeom = false,
                        bool recomputeConnections = true);

    /**@brief delete network element
     * @param[in] networkElement The network element to be removed
     * @param[in] undoList The undolist in which to mark changes
     */
    void deleteNetworkElement(GNENetworkElement* networkElement, GNEUndoList* undoList);

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

    /**@brief remove data set
     * @param[in] dataSet The data set to be removed
     * @param[in] undoList The undolist in which to mark changes
     */
    void deleteDataSet(GNEDataSet* dataSet, GNEUndoList* undoList);

    /**@brief remove data interval
     * @param[in] dataInterval The data interval to be removed
     * @param[in] undoList The undolist in which to mark changes
     */
    void deleteDataInterval(GNEDataInterval* dataInterval, GNEUndoList* undoList);

    /**@brief remove generic data
     * @param[in] genericData The generic data to be removed
     * @param[in] undoList The undolist in which to mark changes
     */
    void deleteGenericData(GNEGenericData* genericData, GNEUndoList* undoList);

    /**@brief remove generic data
     * @param[in] genericData The generic data to be removed
     * @param[in] undoList The undolist in which to mark changes
     */
    void deleteMeanData(GNEMeanData* meanData, GNEUndoList* undoList);

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
     * @param[in] index to be changed
     * @param[in] undoList The undolist in which to mark changes
     */
    bool addRestrictedLane(SUMOVehicleClass vclass, GNEEdge* edge, int index, GNEUndoList* undoList);

    /**@brief add restricted lane to edge
     * @param[in] edge The edge in which insert restricted lane
     * @param[in] index to be changed
     * @param[in] undoList The undolist in which to mark changes
     */
    bool addGreenVergeLane(GNEEdge* edge, int index, GNEUndoList* undoList);

    /**@brief remove restricted lane
     * @param[in] vclass vehicle class to restrict
     * @param[in] edge the edge in which remove sidewalk
     * @param[in] undoList The undolist in which to mark changes
     */
    bool removeRestrictedLane(SUMOVehicleClass vclass, GNEEdge* edge, GNEUndoList* undoList);

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
     * @param[in] disconnected add edge reversed or disconnected parallel
     * @return Return the new edge or 0
     */
    GNEEdge* addReversedEdge(GNEEdge* edge, const bool disconnected, GNEUndoList* undoList);

    /**@brief merge the given junctions
     * edges between the given junctions will be deleted
     * @param[in] moved The junction that will be eliminated
     * @param[in] target The junction that will be enlarged
     * @param[in] undoList The undo list with which to register changes
     */
    void mergeJunctions(GNEJunction* moved, GNEJunction* target, GNEUndoList* undoList);

    /// @brief select all roundabout edges and junctions for the current roundabout
    void selectRoundabout(GNEJunction* junction, GNEUndoList* undoList);

    /// @brief transform the given junction into a roundabout
    void createRoundabout(GNEJunction* junction, GNEUndoList* undoList);

    /// @brief save the network
    void saveNetwork();

    /// @brief save plain xml representation of the network (and nothing else)
    void savePlain(const std::string& prefix);

    /// @brief save log of joined junctions (and nothing else)
    void saveJoined(const std::string& filename);

    /// @brief Set the net to be notified of network changes
    void setViewNet(GNEViewNet* viewNet);

    /// @brief add GL Object into net
    void addGLObjectIntoGrid(GNEAttributeCarrier* AC);

    /// @brief add GL Object into net
    void removeGLObjectFromGrid(GNEAttributeCarrier* AC);

    /// @brief modifies endpoins of the given edge
    void changeEdgeEndpoints(GNEEdge* edge, const std::string& newSourceID, const std::string& newDestID);

    /// @brief get view net
    GNEViewNet* getViewNet() const;

    /// @brief returns the tllcont of the underlying netbuilder
    NBTrafficLightLogicCont& getTLLogicCont();

    /// @brief returns the NBEdgeCont of the underlying netbuilder
    NBEdgeCont& getEdgeCont();

    /// @brief initialize GNEConnections
    void initGNEConnections();

    /// @brief recompute the network and update lane geometries
    void computeAndUpdate(OptionsCont& neteditOptions, bool volatileOptions);

    /**@brief trigger full netbuild computation
     * param[in] window The window to inform about delay
     * param[in] force Whether to force recomputation even if not needed
     * param[in] volatileOptions enable or disable volatile options
     */
    void computeNetwork(GNEApplicationWindow* window, bool force = false, bool volatileOptions = false);

    /**@brief compute demand elements
     * param[in] window The window to inform about delay
     */
    void computeDemandElements(GNEApplicationWindow* window);

    /**@brief compute data elements
     * param[in] window The window to inform about delay
     */
    void computeDataElements(GNEApplicationWindow* window);

    /**@brief join selected junctions
     * @note difference to mergeJunctions:
     *  - can join more than 2
     *  - connected edges will keep their geometry (big junction shape is created)
     *  - no hierarchy: if any junction has a traffic light than the resulting junction will
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

    /// @brief adjust person plans
    void adjustPersonPlans(GNEUndoList* undoList);

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

    /// @brief clear additionals
    void clearAdditionalElements(GNEUndoList* undoList);

    /// @brief clear demand elements
    void clearDemandElements(GNEUndoList* undoList);

    /// @brief clear data elements
    void clearDataElements(GNEUndoList* undoList);

    /// @brief clear meanDatas
    void clearMeanDataElements(GNEUndoList* undoList);

    /**@brief trigger recomputation of junction shape and logic
     * param[in] window The window to inform about delay
     */
    void computeJunction(GNEJunction* junction);

    /// @brief inform the net about the need for recomputation
    void requireRecompute();

    /// @brief check if net require recomputing
    bool isNetRecomputed() const;

    /// @brief get pointer to the main App
    FXApp* getApp();

    /// @brief get net builder
    NBNetBuilder* getNetBuilder() const;

    /// @brief add edge id to the list of explicit turnarounds
    void addExplicitTurnaround(std::string id);

    /// @brief remove edge id from the list of explicit turnarounds
    void removeExplicitTurnaround(std::string id);

    /// @brief save additional elements
    void saveAdditionals();

    /// @brief save demand element elements of the network
    void saveDemandElements();

    /// @brief save data set elements of the network
    void saveDataElements();

    /// @brief get minimum interval
    double getDataSetIntervalMinimumBegin() const;

    /// @brief get maximum interval
    double getDataSetIntervalMaximumEnd() const;

    /// @brief save meanData elements of the network
    void saveMeanDatas();

    /**@brief save TLS Programs elements of the network
     * @param[in] filename name of the file in which save TLS Programs
     */
    void saveTLSPrograms(const std::string& filename);

    /// @brief get number of TLS Programs
    int getNumberOfTLSPrograms() const;

    /**@brief save edgeTypes elements of the network
     * @param[in] filename name of the file in which save edgeTypes
    */
    void saveEdgeTypes(const std::string& filename);

    /// @name Functions related to Enable or disable update geometry of elements after insertion
    /// @{
    /// @brief enable update geometry of elements after inserting or removing an element in net
    void enableUpdateGeometry();

    /// @brief disable update geometry of elements after inserting or removing an element in net
    void disableUpdateGeometry();

    /// @brief check if update geometry after inserting or removing has to be updated
    bool isUpdateGeometryEnabled() const;

    /// @}

    /// @name Functions related to Enable or disable update data of elements after insertion
    /// @{
    /// @brief enable update data elements after inserting or removing an element in net
    void enableUpdateData();

    /// @brief disable update data elements after inserting or removing an element in net
    void disableUpdateData();

    /// @brief check if update data after inserting or removing has to be updated
    bool isUpdateDataEnabled() const;

    /// @}

    /// @brief variable used for write headers in additional, demand and data elements
    static const std::map<SumoXMLAttr, std::string> EMPTY_HEADER;

protected:
    /// @brief the rtree which contains all GUIGlObjects (so named for historical reasons)
    SUMORTree myGrid;

    /// @brief The internal netbuilder
    NBNetBuilder* myNetBuilder;

    /// @brief The net to be notified of about changes
    GNEViewNet* myViewNet = nullptr;

    /// @brief AttributeCarriers of net
    GNENetHelper::AttributeCarriers* myAttributeCarriers = nullptr;

    /// @brief AttributeCarriers of net
    GNENetHelper::SavingStatus* mySavingStatus;

    /// @brief Path manager
    GNEPathManager* myPathManager;

    /// @name counters for junction/edge IDs
    // @{
    unsigned int myJunctionIDCounter = 0;
    unsigned int myEdgeIDCounter = 0;
    // @}

    /// @brief list of edge ids for which turn-arounds must be added explicitly
    std::set<std::string> myExplicitTurnarounds;

    /// @brief whether the net needs recomputation
    bool myNeedRecompute = true;

    /// @brief Flag to enable or disable update geometry of elements after inserting or removing element in net
    bool myUpdateGeometryEnabled = true;

    /// @brief Flag to enable or disable update data elements after inserting or removing element in net
    bool myUpdateDataEnabled = true;

private:
    /// @brief Init Junctions and edges
    void initJunctionsAndEdges();

    /// @brief return true if there are already a Junction in the given position, false in other case
    bool checkJunctionPosition(const Position& pos);

    /// @brief save additionals after confirming invalid objects
    void saveAdditionalsConfirmed();

    /// @brief save demand elements after confirming invalid objects
    void saveDemandElementsConfirmed();

    /// @brief save data elements after confirming invalid objects
    void saveDataElementsConfirmed();

    /// @brief save meanDatas
    void saveMeanDatasConfirmed();

    /// @brief write additional element by type and sorted by ID
    void writeAdditionalByType(OutputDevice& device, const std::vector<SumoXMLTag> tags) const;

    /// @brief write demand element by type and sorted by ID
    void writeDemandByType(OutputDevice& device, SumoXMLTag tag) const;

    /// @brief write route sorted by ID
    void writeRoutes(OutputDevice& device, const bool additionalFile) const;

    /// @brief write vTypes sorted by ID
    void writeVTypes(OutputDevice& device, const bool additionalFile) const;

    /// @brief write meanData element by type and sorted by ID
    void writeMeanDatas(OutputDevice& device, SumoXMLTag tag) const;

    /// @brief write vType comment
    bool writeVTypeComment(OutputDevice& device, const bool additionalFile) const;

    /// @brief write route comment
    bool writeRouteComment(OutputDevice& device, const bool additionalFile) const;

    /// @brief write routeProbe comment
    bool writeRouteProbeComment(OutputDevice& device) const;

    /// @brief write calibrator comment
    bool writeCalibratorComment(OutputDevice& device) const;

    /// @brief write stoppingPlace comment
    bool writeStoppingPlaceComment(OutputDevice& device) const;

    /// @brief write detector comment
    bool writeDetectorComment(OutputDevice& device) const;

    /// @brief write other additional comment
    bool writeOtherAdditionalsComment(OutputDevice& device) const;

    /// @brief write shape comment
    bool writeShapesComment(OutputDevice& device) const;

    /// @brief write TAZ comment
    bool writeTAZComment(OutputDevice& device) const;

    /// @brief write Wire comment
    bool writeWireComment(OutputDevice& device) const;

    /// @brief write meanDataEdge comment
    bool writeMeanDataEdgeComment(OutputDevice& device) const;

    /// @brief write Wire comment
    bool writeMeanDataLaneComment(OutputDevice& device) const;

    /// @brief replace in list attribute
    static void replaceInListAttribute(GNEAttributeCarrier* ac, SumoXMLAttr key, const std::string& which, const std::string& by, GNEUndoList* undoList);

    /// @brief the z boundary (stored in the x-coordinate), values of 0 are ignored
    Boundary myZBoundary;

    /// @brief map with the Edges and their number of lanes
    std::map<std::string, int> myEdgesAndNumberOfLanes;

    /// @brief marker for whether the z-boundary is initialized
    static const double Z_INITIALIZED;

    /// @brief Invalidated copy constructor.
    GNENet(const GNENet&) = delete;

    /// @brief Invalidated assignment operator.
    GNENet& operator=(const GNENet&) = delete;
};
