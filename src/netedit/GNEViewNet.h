/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEViewNet.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// A view on the network being edited (adapted from GUIViewTraffic)
/****************************************************************************/
#ifndef GNEViewNet_h
#define GNEViewNet_h


// ===========================================================================
// included modules
// ===========================================================================
#include "GNEViewNetHelper.h"

#include <utils/common/SUMOVehicleClass.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/geom/Position.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>
#include <utils/gui/settings/GUIPropertyScheme.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEViewNet
 * Microsocopic view at the simulation
 */
class GNEViewNet : public GUISUMOAbstractView {
    /// @brief FOX-declaration
    FXDECLARE(GNEViewNet)

    /// @brief declare GNEViewNetHelper as friend struct
    friend struct GNEViewNetHelper;

public:
    /* @brief constructor
     * @param[in] tmpParent temporal FXFrame parent so that we can add items to view area in the desired order
     * @param[in] actualParent FXFrame parent of GNEViewNet
     * @param[in] app main windows
     * @param[in] viewParent viewParent of this viewNet
     * @param[in] net traffic net
     * @param[in] undoList pointer to UndoList modul
     * @param[in] glVis a reference to GLVisuals
     * @param[in] share a reference to FXCanvas
     */
    GNEViewNet(FXComposite* tmpParent, FXComposite* actualParent, GUIMainWindow& app,
               GNEViewParent* viewParent, GNENet* net, GNEUndoList* undoList,
               FXGLVisual* glVis, FXGLCanvas* share);

    /// @brief destructor
    ~GNEViewNet();

    /// @brief builds the view toolbars
    void buildViewToolBars(GUIGlChildWindow&);

    /// @brief Mark the entire GNEViewNet to be repainted later
    void update() const;

    /// @brief get AttributeCarriers in Boundary
    std::set<std::pair<std::string, GNEAttributeCarrier*> > getAttributeCarriersInBoundary(const Boundary& boundary, bool forceSelectEdges = false);

    /** @brief Builds an entry which allows to (de)select the object
     * @param ret The popup menu to add the entry to
     * @param AC AttributeCarrier that will be select/unselected
     */
    void buildSelectionACPopupEntry(GUIGLObjectPopupMenu* ret, GNEAttributeCarrier* AC);

    /// @brief set color scheme
    bool setColorScheme(const std::string& name);

    ///@brief recalibrate color scheme according to the current value range
    void buildColorRainbow(const GUIVisualizationSettings& s, GUIColorScheme& scheme, int active, GUIGlObjectType objectType,
                           bool hide = false, double hideThreshold = 0.);

    //@brief open object dialog
    void openObjectDialog();

    /// @brief get edit modes
    const GNEViewNetHelper::EditModes& getEditModes() const;

    /// @brief get Common view options
    const GNEViewNetHelper::CommonViewOptions& getCommonViewOptions() const;

    /// @brief get network view options
    const GNEViewNetHelper::NetworkViewOptions& getNetworkViewOptions() const;

    /// @brief get demand view options
    const GNEViewNetHelper::DemandViewOptions& getDemandViewOptions() const;

    /// @brief get Key Pressed modul
    const GNEViewNetHelper::KeyPressed& getKeyPressed() const;

    /// @brief get Edit Shape modul
    const GNEViewNetHelper::EditShapes& getEditShapes() const;

    /// @name overloaded handlers
    /// @{
    /// @brief called when user press mouse's left button
    long onLeftBtnPress(FXObject*, FXSelector, void*);

    /// @brief called when user releases mouse's left button
    long onLeftBtnRelease(FXObject*, FXSelector, void*);

    /// @brief called when user press mouse's right button
    long onRightBtnPress(FXObject*, FXSelector, void*);

    /// @brief called when user releases mouse's right button
    long onRightBtnRelease(FXObject*, FXSelector, void*);

    /// @brief called when user moves mouse
    long onMouseMove(FXObject*, FXSelector, void*);

    /// @brief called when user press a key
    long onKeyPress(FXObject* o, FXSelector sel, void* data);

    /// @brief called when user release a key
    long onKeyRelease(FXObject* o, FXSelector sel, void* data);
    /// @}

    /// @name set modes call backs
    /// @{
    /// @brief called when user press a supermode button
    long onCmdSetSupermode(FXObject*, FXSelector sel, void*);

    /// @brief called when user press a mode button (Network or demand)
    long onCmdSetMode(FXObject*, FXSelector sel, void*);

    /// @}

    /// @brief split edge at cursor position
    long onCmdSplitEdge(FXObject*, FXSelector, void*);

    /// @brief split edge at cursor position
    long onCmdSplitEdgeBidi(FXObject*, FXSelector, void*);

    /// @brief reverse edge
    long onCmdReverseEdge(FXObject*, FXSelector, void*);

    /// @brief add reversed edge
    long onCmdAddReversedEdge(FXObject*, FXSelector, void*);

    /// @brief change geometry endpoint
    long onCmdEditEdgeEndpoint(FXObject*, FXSelector, void*);

    /// @brief change geometry endpoint
    long onCmdResetEdgeEndpoint(FXObject*, FXSelector, void*);

    /// @brief makes selected edges straight
    long onCmdStraightenEdges(FXObject*, FXSelector, void*);

    /// @brief smooth geometry
    long onCmdSmoothEdges(FXObject*, FXSelector, void*);

    /// @brief interpolate z values linear between junctions
    long onCmdStraightenEdgesElevation(FXObject*, FXSelector, void*);

    /// @brief smooth elevation with regard to adjoining edges
    long onCmdSmoothEdgesElevation(FXObject*, FXSelector, void*);

    /// @brief simply shape of current polygon
    long onCmdSimplifyShape(FXObject*, FXSelector, void*);

    /// @brief delete the closes geometry point
    long onCmdDeleteGeometryPoint(FXObject*, FXSelector, void*);

    /// @brief close opened polygon
    long onCmdClosePolygon(FXObject*, FXSelector, void*);

    /// @brief open closed polygon
    long onCmdOpenPolygon(FXObject*, FXSelector, void*);

    /// @brief set as first geometry point the closes geometry point
    long onCmdSetFirstGeometryPoint(FXObject*, FXSelector, void*);

    /// @brief Transform POI to POILane, and viceversa
    long onCmdTransformPOI(FXObject*, FXSelector, void*);

    /// @brief duplicate selected lane
    long onCmdDuplicateLane(FXObject*, FXSelector, void*);

    /// @brief reset custom shapes of selected lanes
    long onCmdResetLaneCustomShape(FXObject*, FXSelector, void*);

    /// @brief add/remove/restrict lane
    long onCmdLaneOperation(FXObject*, FXSelector sel, void*);

    /// @brief open additional dialog
    long onCmdOpenAdditionalDialog(FXObject*, FXSelector, void*);

    /// @brief edit junction shape
    long onCmdEditJunctionShape(FXObject*, FXSelector, void*);

    /// @brief reset junction shape
    long onCmdResetJunctionShape(FXObject*, FXSelector, void*);

    /// @brief replace node by geometry
    long onCmdReplaceJunction(FXObject*, FXSelector, void*);

    /// @brief split junction into multiple junctions
    long onCmdSplitJunction(FXObject*, FXSelector, void*);

    /// @brief split junction into multiple junctions and reconnect them
    long onCmdSplitJunctionReconnect(FXObject*, FXSelector, void*);

    /// @brief clear junction connections
    long onCmdClearConnections(FXObject*, FXSelector, void*);

    /// @brief reset junction connections
    long onCmdResetConnections(FXObject*, FXSelector, void*);

    /// @brief edit connection shape
    long onCmdEditConnectionShape(FXObject*, FXSelector, void*);

    /// @brief edit crossing shape
    long onCmdEditCrossingShape(FXObject*, FXSelector, void*);

    /// @name View options network call backs
    /// @{
    /// @brief toogle show demand elements
    long onCmdToogleShowDemandElements(FXObject*, FXSelector, void*);

    /// @brief toogle select edges
    long onCmdToogleSelectEdges(FXObject*, FXSelector, void*);

    /// @brief toogle show connections
    long onCmdToogleShowConnections(FXObject*, FXSelector, void*);

    /// @brief toogle hide connections
    long onCmdToogleHideConnections(FXObject*, FXSelector, void*);

    /// @brief toogle extend selection
    long onCmdToogleExtendSelection(FXObject*, FXSelector, void*);

    /// @brief toogle change all phases
    long onCmdToogleChangeAllPhases(FXObject*, FXSelector, void*);

    /// @brief toogle show grid
    long onCmdToogleShowGrid(FXObject*, FXSelector, void*);

    /// @brief toogle warn for merge
    long onCmdToogleWarnAboutMerge(FXObject*, FXSelector, void*);

    /// @brief toogle show junction bubbles
    long onCmdToogleShowJunctionBubbles(FXObject*, FXSelector, void*);

    /// @brief toogle move elevation
    long onCmdToogleMoveElevation(FXObject*, FXSelector, void*);

    /// @brief toogle chain edges
    long onCmdToogleChainEdges(FXObject*, FXSelector, void*);

    /// @brief toogle autoOpposite edge
    long onCmdToogleAutoOppositeEdge(FXObject*, FXSelector, void*);

    /// @brief toogle hide non inspected demand elements
    long onCmdToogleHideNonInspecteDemandElements(FXObject*, FXSelector, void*);

    /// @brief toogle hide shapes in super mode demand
    long onCmdToogleHideShapes(FXObject*, FXSelector, void*);

    /// @brief toogle show all person plans in super mode demand
    long onCmdToogleShowAllPersonPlans(FXObject*, FXSelector, void*);

    /// @brief toogle lock person in super mode demand
    long onCmdToogleLockPerson(FXObject*, FXSelector, void*);
    /// @}

    /// @brief select AC under cursor
    long onCmdAddSelected(FXObject*, FXSelector, void*);

    /// @brief unselect AC under cursor
    long onCmdRemoveSelected(FXObject*, FXSelector, void*);

    /// @brief abort current edition operation
    void abortOperation(bool clearSelection = true);

    /// @brief handle del keypress
    void hotkeyDel();

    /// @brief handle enter keypress
    void hotkeyEnter();

    /// @brief handle backspace keypress
    void hotkeyBackSpace();

    /// @brief handle focus frame keypress
    void hotkeyFocusFrame();

    /// @brief get the net object
    GNEViewParent* getViewParent() const;

    /// @brief get the net object
    GNENet* getNet() const;

    /// @brief get the undoList object
    GNEUndoList* getUndoList() const;

    /// @brief get AttributeCarrier under cursor
    const GNEAttributeCarrier* getDottedAC() const;

    /// @brief set attributeCarrier under cursor
    void setDottedAC(const GNEAttributeCarrier* AC);

    /// @brief check if lock icon should be visible
    bool showLockIcon() const;

    /// @brief set staturBar text
    void setStatusBarText(const std::string& text);

    /// @brief whether to autoselect nodes or to lanes
    bool autoSelectNodes();

    /// @brief set selection scaling
    void setSelectionScaling(double selectionScale);

    /// @brief update control contents after undo/redo or recompute
    void updateControls();

    /// @brief change all phases
    bool changeAllPhases() const;

    /// @brief return true if junction must be showed as bubbles
    bool showJunctionAsBubbles() const;

protected:
    /// @brief FOX needs this
    GNEViewNet();

    /// @brief do paintGL
    int doPaintGL(int mode, const Boundary& bound);

    /// @brief called after some features are already initialized
    void doInit();

private:
    /// @name structs related with modes and testing mode
    /// @{

    /// @brief variable used to save variables related with edit moves modes
    GNEViewNetHelper::EditModes myEditModes;

    /// @brief variable used to save variables related with testing mode
    GNEViewNetHelper::TestingMode myTestingMode;
    /// @}

    /// @name structs related with input (keyboard and mouse)
    /// @{

    /// @brief variable used to save key status after certain events
    GNEViewNetHelper::KeyPressed myKeyPressed;

    /// @brief variable use to save all pointers to objects under cursor after a click
    GNEViewNetHelper::ObjectsUnderCursor myObjectsUnderCursor;
    /// @}

    /// @name structs related with checkable buttons
    /// @{

    /// @brief variable used to save checkable buttons for common supermodes
    GNEViewNetHelper::CommonCheckableButtons myCommonCheckableButtons;

    /// @brief variable used to save checkable buttons for Supermode Network
    GNEViewNetHelper::NetworkCheckableButtons myNetworkCheckableButtons;

    /// @brief variable used to save checkable buttons for Supermode Demand
    GNEViewNetHelper::DemandCheckableButtons myDemandCheckableButtons;
    /// @}

    /// @name structs related with view options
    /// @{

    /// @brief variable used to save variables related with common view options
    GNEViewNetHelper::CommonViewOptions myCommonViewOptions;

    /// @brief variable used to save variables related with view options in Network Supermode
    GNEViewNetHelper::NetworkViewOptions myNetworkViewOptions;

    /// @brief variable used to save variables related with view options in Demand Supermode
    GNEViewNetHelper::DemandViewOptions myDemandViewOptions;
    /// @}

    /// @name structs related with move elements
    /// @{
    /// @brief variable used to save variables related with movement of single elements
    GNEViewNetHelper::MoveSingleElementValues myMoveSingleElementValues;

    /// @brief variable used to save variables related with movement of multiple elements
    GNEViewNetHelper::MoveMultipleElementValues myMoveMultipleElementValues;
    // @}

    /// @name structs related with Demand options
    /// @{

    /// @brief variable used to save variables related with vehicle options
    GNEViewNetHelper::VehicleOptions myVehicleOptions;

    /// @brief variable used to save variables related with vehicle type options
    GNEViewNetHelper::VehicleTypeOptions myVehicleTypeOptions;
    // @}

    /// @brief variable used to save variables related with selecting areas
    GNEViewNetHelper::SelectingArea mySelectingArea;

    /// @brief struct for grouping all variables related with edit shapes
    GNEViewNetHelper::EditShapes myEditShapes;

    /// @brief view parent
    GNEViewParent* myViewParent;

    /// @brief Pointer to current net. (We are not responsible for deletion)
    GNENet* myNet;

    /// @brief the current frame
    GNEFrame* myCurrentFrame;

    /// @brief a reference to the undolist maintained in the application
    GNEUndoList* myUndoList;

    /**@brief current AttributeCarrier that is drawn using with a dotted contour
     * note: it's constant because is edited from constant functions (example: drawGL(...) const)
     */
    const GNEAttributeCarrier* myDottedAC;

    /// @brief create edit mode buttons and elements
    void buildEditModeControls();

    /// @brief updates Network mode specific controls
    void updateNetworkModeSpecificControls();

    /// @brief updates Demand mode specific controls
    void updateDemandModeSpecificControls();

    /// @brief delete all currently selected junctions
    void deleteSelectedJunctions();

    /// @brief delete all currently selected lanes
    void deleteSelectedLanes();

    /// @brief delete all currently selected edges
    void deleteSelectedEdges();

    /// @brief delete all currently selected additionals
    void deleteSelectedAdditionals();

    /// @brief delete all currently selected demand elements
    void deleteSelectedDemandElements();

    /// @brief delete all currently selected crossings
    void deleteSelectedCrossings();

    /// @brief delete all currently selected connections
    void deleteSelectedConnections();

    /// @brief delete all currently selected shapes
    void deleteSelectedShapes();

    /// @brief try to merge moved junction with another junction in that spot return true if merging did take place
    bool mergeJunctions(GNEJunction* moved, const Position& oldPos);

    /// @brief try to retrieve an edge at popup position
    GNEEdge* getEdgeAtPopupPosition();

    /// @brief try to retrieve a lane at popup position
    GNELane* getLaneAtPopupPosition();

    /// @brief try to retrieve a junction at popup position
    GNEJunction* getJunctionAtPopupPosition();

    /// @brief try to retrieve a connection at popup position
    GNEConnection* getConnectionAtPopupPosition();

    /// @brief try to retrieve a crossing at popup position
    GNECrossing* getCrossingAtPopupPosition();

    /// @brief try to retrieve a additional at popup position
    GNEAdditional* getAdditionalAtPopupPosition();

    /// @brief try to retrieve a polygon at popup position
    GNEPoly* getPolygonAtPopupPosition();

    /// @brief try to retrieve a POILane at popup position
    GNEPOI* getPOIAtPopupPosition();

    /// @brief restrict lane
    bool restrictLane(SUMOVehicleClass vclass);

    /// @brief add restricted lane
    bool addRestrictedLane(SUMOVehicleClass vclass);

    /// @brief remove restricted lane
    bool removeRestrictedLane(SUMOVehicleClass vclass);

    /// @brief Auxiliar function used by onLeftBtnPress(...)
    void processClick(void* eventData);

    /// @brief update cursor after every click/key press/release
    void updateCursor();

    /// @brief draw functions
    /// @{

    /// @brief draw connections between lane candidates during selecting lane mode in Additional mode
    void drawLaneCandidates() const;

    /// @brief draw temporal polygon  shape in Polygon Mode
    void drawTemporalDrawShape() const;
    /// @}

    /// @brief mouse process functions
    /// @{

    /// @brief process left button press function in Supermode Network
    void processLeftButtonPressNetwork(void* eventData);

    /// @brief process left button release function in Supermode Network
    void processLeftButtonReleaseNetwork();

    /// @brief process move mouse function in Supermode Network
    void processMoveMouseNetwork();

    /// @brief process left button press function in Supermode Demand
    void processLeftButtonPressDemand(void* eventData);

    /// @brief process left button release function in Supermode Demand
    void processLeftButtonReleaseDemand();

    /// @brief process move mouse function in Supermode Demand
    void processMoveMouseDemand();

    /// @brief Invalidated copy constructor.
    GNEViewNet(const GNEViewNet&) = delete;

    /// @brief Invalidated assignment operator.
    GNEViewNet& operator=(const GNEViewNet&) = delete;
};


#endif

/****************************************************************************/
