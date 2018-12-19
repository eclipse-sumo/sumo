/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
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
#include <config.h>

#include <utils/common/SUMOVehicleClass.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/geom/Position.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>
#include <utils/gui/settings/GUIPropertyScheme.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>


// ===========================================================================
// enum
// ===========================================================================

enum NetworkEditMode {
    ///@brief placeholder mode
    GNE_MODE_DUMMY,
    ///@brief mode for creating new edges
    GNE_MODE_CREATE_EDGE,
    ///@brief mode for moving things
    GNE_MODE_MOVE,
    ///@brief mode for deleting things
    GNE_MODE_DELETE,
    ///@brief mode for inspecting object attributes
    GNE_MODE_INSPECT,
    ///@brief mode for selecting objects
    GNE_MODE_SELECT,
    ///@brief mode for connecting lanes
    GNE_MODE_CONNECT,
    ///@brief mode for editing tls
    GNE_MODE_TLS,
    ///@brief Mode for editing additionals
    GNE_MODE_ADDITIONAL,
    ///@brief Mode for editing crossing
    GNE_MODE_CROSSING,
    ///@brief Mode for editing TAZ
    GNE_MODE_TAZ,
    ///@brief Mode for editing Polygons
    GNE_MODE_POLYGON,
    ///@brief Mode for editing connection prohibitions
    GNE_MODE_PROHIBITION
};

enum DemandEditMode {
    ///@brief Mode for editing routes
    GNE_MODE_ROUTES
};

// ===========================================================================
// class declarations
// ===========================================================================

class GNENet;
class GNENetElement;
class GNEJunction;
class GNEEdge;
class GNELane;
class GNEConnection;
class GNEViewParent;
class GNEUndoList;
class GNEAdditional;
class GNEShape;
class GNEPoly;
class GNEPOI;
class GNEFrame;
class GNECrossing;
class GNEAttributeCarrier;
class GNETAZ;
class GNEInternalLane;

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

public:
    /// @brief class used to group all variables related with objects under cursor after a click over view
    class ObjectsUnderCursor {
    public:
        /// @brief constructor
        ObjectsUnderCursor();

        /// @brief update objects under cursor (Called only in onLeftBtnPress(...) function)
        void updateObjectUnderCursor(const std::vector<GUIGlObject*> &GUIGlObjects, GNEPoly* editedPolyShape);

        /// @brief swap lane to edge
        void swapLane2Edge();

        /// @brief set created junction
        void setCreatedJunction(GNEJunction* junction);

        /// @brief get front GUI GL ID (or a pointer to nullptr if there isn't)
        GUIGlID getGlIDFront() const;

        /// @brief get front GUI GL object type (or a pointer to nullptr if there isn't)
        GUIGlObjectType getGlTypeFront() const;

        /// @brief get front attribute carrier (or a pointer to nullptr if there isn't)
        GNEAttributeCarrier* getAttributeCarrierFront() const;

        /// @brief get front net element (or a pointer to nullptr if there isn't)
        GNENetElement* getNetElementFront() const;

        /// @brief get front additional element (or a pointer to nullptr if there isn't)
        GNEAdditional* getAdditionalFront() const;

        /// @brief get front shape element (or a pointer to nullptr if there isn't)
        GNEShape* getShapeFront() const;

        /// @brief get front junction (or a pointer to nullptr if there isn't)
        GNEJunction* getJunctionFront() const;

        /// @brief get front edge (or a pointer to nullptr if there isn't)
        GNEEdge* getEdgeFront() const;

        /// @brief get front lane (or a pointer to nullptr if there isn't)
        GNELane* getLaneFront() const;

        /// @brief get front crossing (or a pointer to nullptr if there isn't)
        GNECrossing* getCrossingFront() const;

        /// @brief get front connection (or a pointer to nullptr if there isn't)
        GNEConnection* getConnectionFront() const;

        /// @brief get front TAZ (or a pointer to nullptr if there isn't)
        GNETAZ* getTAZFront() const;

        /// @brief get front POI (or a pointer to nullptr if there isn't)
        GNEPOI* getPOIFront() const;

        /// @brief get front Poly (or a pointer to nullptr if there isn't)
        GNEPoly* getPolyFront() const;

        /// @brief get vector with clicked ACs
        const std::vector<GNEAttributeCarrier*> &getClickedAttributeCarriers() const;

    private:
        /// @brief vector with the clicked GUIGlObjects
        std::vector<GUIGlObject*> myGUIGlObjects;

        /// @brief vector with the clicked attribute carriers
        std::vector<GNEAttributeCarrier*> myAttributeCarriers;

        /// @brief vector with the clicked net elements
        std::vector<GNENetElement*> myNetElements;

        /// @brief vector with the clicked additional elements
        std::vector<GNEAdditional*> myAdditionals;

        /// @brief vector with the clicked shape elements (Poly and POIs)
        std::vector<GNEShape*> myShapes;

        /// @brief vector with the clicked junctions
        std::vector<GNEJunction*> myJunctions;

        /// @brief vector with the clicked edges
        std::vector<GNEEdge*> myEdges;

        /// @brief vector with the clicked lanes
        std::vector<GNELane*> myLanes;

        /// @brief vector with the clicked crossings
        std::vector<GNECrossing*> myCrossings;

        /// @brief vector with the clicked connections
        std::vector<GNEConnection*> myConnections;

        /// @brief vector with the clicked TAZ elements (needed because uses a shape instead a position)
        std::vector<GNETAZ*> myTAZs;

        /// @brief vector with the clicked POIs
        std::vector<GNEPOI*> myPOIs;

        /// @brief vector with the clicked Polys
        std::vector<GNEPoly*> myPolys;

        /// @brief invert GUIGlObjects
        void sortGUIGlObjectsByAltitude(const std::vector<GUIGlObject*> &GUIGlObjects);

        /// @brief Invalidated copy constructor.
        ObjectsUnderCursor(const ObjectsUnderCursor&) = delete;

        /// @brief Invalidated assignment operator.
        ObjectsUnderCursor& operator=(const ObjectsUnderCursor&) = delete;
    };

    /// @brief class used to group all variables related with key pressed after certain events
    struct KeyPressed {

        /// @brief constructor
        KeyPressed();

        /// @brief update status of KeyPressed
        void update(void *eventData);

        /// @brief check if SHIFT key was pressed during click
        bool shiftKeyPressed() const;

        /// @brief check if CONTROL key was pressed during click
        bool controlKeyPressed() const;

    private:
        /// @brief information of event
        FXEvent* myEventInfo;
    };

    /* @brief constructor
     * @param[in] tmpParent temporal FXFrame parent so that we can add items to view area in the desired order
     * @param[in] actualParent FXFrame parent of GNEViewNet
     * @param[in] app main windows
     * @param[in] viewParent viewParent of this viewNet
     * @param[in] net traffic net
     * @param[in] undoList pointer to UndoList modul
     * @param[in] glVis a reference to GLVisuals
     * @param[in] share a reference to FXCanvas
     * @param[in] toolbar A reference to the parents toolbar
     */
    GNEViewNet(FXComposite* tmpParent, FXComposite* actualParent, GUIMainWindow& app,
               GNEViewParent* viewParent, GNENet* net, GNEUndoList* undoList,
               FXGLVisual* glVis, FXGLCanvas* share, FXToolBar* toolBar);

    /// @brief destructor
    ~GNEViewNet();

    /// @brief builds the view toolbars
    virtual void buildViewToolBars(GUIGlChildWindow&);

    /// @brief get AttributeCarriers in Boundary
    std::set<std::pair<std::string, GNEAttributeCarrier*> > getAttributeCarriersInBoundary(const Boundary &boundary, bool forceSelectEdges = false);

    /** @brief Builds an entry which allows to (de)select the object
     * @param ret The popup menu to add the entry to
     * @param AC AttributeCarrier that will be select/unselected
     */
    void buildSelectionACPopupEntry(GUIGLObjectPopupMenu* ret, GNEAttributeCarrier* AC);

    /// @brief set color scheme
    bool setColorScheme(const std::string& name);

    ///@brief recalibrate color scheme according to the current value range
    void buildColorRainbow(const GUIVisualizationSettings& s, GUIColorScheme& scheme, int active, GUIGlObjectType objectType);

    //@brief open object dialog
    void openObjectDialog();

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

    /// @name set mode call backs
    /// @{
    /// @brief called when user press the button for create edge mode
    long onCmdSetModeCreateEdge(FXObject*, FXSelector, void*);

    /// @brief called when user press the button for move mode
    long onCmdSetModeMove(FXObject*, FXSelector, void*);

    /// @brief called when user press the button for delete mode
    long onCmdSetModeDelete(FXObject*, FXSelector, void*);

    /// @brief called when user press the button for inspect mode
    long onCmdSetModeInspect(FXObject*, FXSelector, void*);

    /// @brief called when user press the button for select mode
    long onCmdSetModeSelect(FXObject*, FXSelector, void*);

    /// @brief called when user press the button for connect mode
    long onCmdSetModeConnect(FXObject*, FXSelector, void*);

    /// @brief called when user press the button for traffic lights mode
    long onCmdSetModeTLS(FXObject*, FXSelector, void*);

    /// @brief called when user press the button for additional mode
    long onCmdSetModeAdditional(FXObject*, FXSelector, void*);

    /// @brief called when user press the button for crossing mode
    long onCmdSetModeCrossing(FXObject*, FXSelector, void*);

    /// @brief called when user press the button for TAZ mode
    long onCmdSetModeTAZ(FXObject*, FXSelector, void*);

    /// @brief called when user press the button for polygon mode
    long onCmdSetModePolygon(FXObject*, FXSelector, void*);

    /// @brief called when user press the button for polygon mode
    long onCmdSetModeProhibition(FXObject*, FXSelector, void*);

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

    /// @brief restrict lane to pedestrians
    long onCmdRestrictLaneSidewalk(FXObject*, FXSelector, void*);

    /// @brief restrict lane to bikes
    long onCmdRestrictLaneBikelane(FXObject*, FXSelector, void*);

    /// @brief restrict lane to buslanes
    long onCmdRestrictLaneBuslane(FXObject*, FXSelector, void*);

    /// @brief restrict lane to all vehicles
    long onCmdRestrictLaneGreenVerge(FXObject*, FXSelector, void*);

    /// @brief Add restricted lane for pedestrians
    long onCmdAddRestrictedLaneSidewalk(FXObject*, FXSelector, void*);

    /// @brief Add restricted lane for bikes
    long onCmdAddRestrictedLaneBikelane(FXObject*, FXSelector, void*);

    /// @brief Add restricted lane for buses
    long onCmdAddRestrictedLaneBuslane(FXObject*, FXSelector, void*);

    /// @brief Add restricted lane for all vehicles
    long onCmdAddRestrictedLaneGreenVerge(FXObject*, FXSelector, void*);

    /// @brief remove restricted lane for pedestrians
    long onCmdRemoveRestrictedLaneSidewalk(FXObject*, FXSelector, void*);

    /// @brief remove restricted lane for bikes
    long onCmdRemoveRestrictedLaneBikelane(FXObject*, FXSelector, void*);

    /// @brief remove restricted lane for bus
    long onCmdRemoveRestrictedLaneBuslane(FXObject*, FXSelector, void*);

    /// @brief remove restricted lane for all vehicles
    long onCmdRemoveRestrictedLaneGreenVerge(FXObject*, FXSelector, void*);

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

    /// @brief clear junction connections
    long onCmdClearConnections(FXObject*, FXSelector, void*);

    /// @brief reset junction connections
    long onCmdResetConnections(FXObject*, FXSelector, void*);

    /// @brief edit connection shape
    long onCmdEditConnectionShape(FXObject*, FXSelector, void*);

    /// @brief edit crossing shape
    long onCmdEditCrossingShape(FXObject*, FXSelector, void*);

    /// @brief toogle show connections
    long onCmdToogleShowConnection(FXObject*, FXSelector, void*);

    /// @brief toogle selet edges
    long onCmdToogleSelectEdges(FXObject*, FXSelector, void*);

    /// @brief toogle show bubbles
    long onCmdToogleShowBubbles(FXObject*, FXSelector, void*);

    /// @brief toogle move elevation
    long onCmdToogleMoveElevation(FXObject*, FXSelector, void*);

    /// @brief select AC under cursor
    long onCmdAddSelected(FXObject*, FXSelector, void*);

    /// @brief unselect AC under cursor
    long onCmdRemoveSelected(FXObject*, FXSelector, void*);

    /// @brief toogle show grid
    long onCmdShowGrid(FXObject*, FXSelector, void*);

    /**@brief sets edit mode (from hotkey)
     * @param[in] selid An id MID_GNE_SETMODE_<foo> as defined in GUIAppEnum
     **/
    void setEditModeFromHotkey(FXushort selid);

    /// @brief abort current edition operation
    void abortOperation(bool clearSelection = true);

    /// @brief handle del keypress
    void hotkeyDel();

    /// @brief handle enter keypress
    void hotkeyEnter();

    /// @brief handle focus frame keypress
    void hotkeyFocusFrame();

    /// @brief get the net object
    GNEViewParent* getViewParent() const;

    /// @brief get the net object
    GNENet* getNet() const;

    /// @brief get the undoList object
    GNEUndoList* getUndoList() const;

    /// @brief get the current Network edit mode
    NetworkEditMode getCurrentNetworkEditMode() const;

    /// @brief get the current Demand edit mode
    DemandEditMode getCurrentDemandEditMode() const;

    /// @brief get Key Pressed modul
    const KeyPressed &getKeyPressed() const;

    /// @brief get grid button
    FXMenuCheck* getMenuCheckShowGrid() const;

    /// @brief get AttributeCarrier under cursor
    const GNEAttributeCarrier* getDottedAC() const;

    /// @brief set attributeCarrier under cursor
    void setDottedAC(const GNEAttributeCarrier* AC);

    /// @brief check if lock icon should be visible
    bool showLockIcon() const;

    /// @brief set staturBar text
    void setStatusBarText(const std::string& text);

    /// @brief whether inspection, selection and inversion should apply to edges or to lanes
    bool selectEdges() const;

    /// @brief return true if elevation is being edited
    bool editingElevation() const;

    /// @brief show connections over junctions
    bool showConnections();

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

    /// @brief start edit custom shape
    void startEditCustomShape(GNENetElement* element, const PositionVector& shape, bool fill);

    /// @brief edit edit shape
    void stopEditCustomShape();

protected:
    /// @brief FOX needs this
    GNEViewNet();

    /// @brief do paintGL
    int doPaintGL(int mode, const Boundary& bound);

    /// @brief called after some features are already initialized
    void doInit();

private:
    /// @brief struct used to group all pointers to moved elements
    struct MovedItems {

        /// @brief constructor
        MovedItems();

        /// @brief the Junction to be moved.
        GNEJunction* junctionToMove;

        /// @brief the edge of which geometry is being moved
        GNEEdge* edgeToMove;

        /// @brief the poly of which geometry is being moved
        GNEPoly* polyToMove;

        /// @brief the poi which position is being moved
        GNEPOI* poiToMove;

        /// @brief the additional element which position is being moved
        GNEAdditional* additionalToMove;

        /// @brief the TAZ element which their Shape is being moved (it's the only additional with a shape instead a position)
        GNETAZ* tazToMove;
    };

    /// @brief struct used to group all variables related with movement of single elements
    struct MoveSingleElementValues {

        /// @brief constructor
        MoveSingleElementValues(GNEViewNet* viewNet);

        /// @brief calculate offset movement
        Position calculateOffsetMovement() const;

        /// calculate Poly movement values (Position, Index, etc.)
        void calculatePolyValues();

        /// calculate Edge movement values (Position, Index, etc.)
        void calculateEdgeValues();

        /// calculate TAZ movement values (Position, Index, etc.)
        void calculateTAZValues();

        /// @brief original shape of element before start moving (used by polygons, edges, etc., needed for commmit position changes)
        PositionVector originalShapeBeforeMoving;

        /// @brief index moved
        int movingIndexShape;

        /// @brief original position of geometry position (needed for commmit position changes)
        Position originalPositionInView;

        /// @brief relative position of Clicked Position regarding to originalGeometryPointPosition (Used when user doesn't click exactly over the center of element)
        Position relativeClickedPosition;

        /// @brief bool to indicate that startPos are being moved
        bool movingStartPos;
        bool movingEndPos;

    private:
        /// @brief pointer to viewNet
        GNEViewNet* myViewNet;
    };

    /// @brief struct used to group all variables related with movement of groups of elements
    struct MoveMultipleElementValues {

        /// @brief constructor
        MoveMultipleElementValues(GNEViewNet* viewNet);

        /// @brief begin move selection
        void beginMoveSelection(GNEAttributeCarrier* originAC);

        /// @brief move selection
        void moveSelection();

        /// @brief finish moving selection
        void finishMoveSelection();

        /// @brief check if currently there is element being moved
        bool isMovingSelection() const;

    private:
        /// @brief pointer to viewNet
        GNEViewNet* myViewNet;

        /// @brief original clicked position when moveSelection is called (used for calculate offset during moveSelection())
        Position myClickedPosition;

        /// @brief flag to check if a selection is being moved
        bool myMovingSelection;

        /// @brief container used for move junctions 
        std::map<GNEJunction*, Position> myMovedJunctionOriginPositions;

        /// @brief container used for move entire edges
        std::map<GNEEdge*, PositionVector> myMovedEdgesOriginShape;
    
        /// @brief container used for move GeometryPoints of edges
        std::map<GNEEdge*, MoveSingleElementValues*> myMovedEgdesGeometryPoints;
    };

    /// @brief struct used to group all variables related with selecting using a square or polygon
    /// @note in the future the variables used for selecting throught a polygon will be placed here
    struct SelectingArea {

        /// @brief default constructor
        SelectingArea(GNEViewNet* viewNet);

        /// @brief begin rectangle selection
        void beginRectangleSelection();

        /// @brief move rectangle selection
        void moveRectangleSelection();

        /// @brief finish rectangle selection
        void finishRectangleSelection();

        /// @brief process rectangle Selection
        void processRectangleSelection();

        /// @brief process rectangle Selection (only limited to Edges)
        std::vector<GNEEdge*> processEdgeRectangleSelection();

        /// @brief process shape selection
        void processShapeSelection(const PositionVector &shape);
        
        /// @brief draw rectangle selection
        void drawRectangleSelection(const RGBColor& color) const;

        /// @brief whether we have started rectangle-selection
        bool selectingUsingRectangle;
                
        /// @brief whether we have started rectangle-selection
        bool startDrawing;

    private:
        /// @brief Process boundary Selection
        void processBoundarySelection(const Boundary &boundary);

        /// @brief firstcorner of the rectangle-selection
        Position selectionCorner1;

        /// @brief second corner of the rectangle-selection
        Position selectionCorner2;

        /// @brief pointer to viewNet
        GNEViewNet* myViewNet;
    };

    /// @brief struct used to group all variables related with testing
    struct TestingMode {

        /// @brief default constructor
        TestingMode(GNEViewNet* viewNet);

        /// @brief init testing mode
        void initTestingMode();

        /// @brief draw testing element
        void drawTestingElements(GUIMainWindow* mainWindow);

    private:
        /// @brief pointer to viewNet
        GNEViewNet* myViewNet;

        /// @brief flag to enable or disable testing mode
        bool myTestingEnabled;

        /// @brief Width of viewNet in testing mode
        int myTestingWidth;

        /// @brief Height of viewNet in testing mode
        int myTestingHeight;
    };

    /// @brief struct used to group all variables related to create edges
    struct CreateEdgeOptions {

        /// @brief default constructor
        CreateEdgeOptions(GNEViewNet* viewNet);

        /// @brief build menu checks
        void buildCreateEdgeOptionMenuChecks();

        /// @brief hide all MenuChecks
        void hideCreateEdgeOptionMenuChecks();

        /// @brief source junction for new edge 0 if no edge source is selected an existing (or newly created) junction otherwise
        GNEJunction* createEdgeSource;

        /// @brief whether the endpoint for a created edge should be set as the new source
        FXMenuCheck* chainCreateEdge;

        /// @brief create auto create opposite edge
        FXMenuCheck* autoCreateOppositeEdge;

        /// @brief whether we should warn about merging junctions
        FXMenuCheck* menuCheckWarnAboutMerge;

        /// @brief show connection as buuble in "Move" mode.
        FXMenuCheck* menuCheckShowBubbleOverJunction;

        /// @brief apply movement to elevation
        FXMenuCheck* menuCheckMoveElevation;

    private:
        /// @brief pointer to viewNet
        GNEViewNet* myViewNet;
    };

    /// @brief struct used to group all variables related to view options
    struct ViewOptions {

        /// @brief default constructor
        ViewOptions(GNEViewNet* viewNet);

        /// @brief build menu checks
        void buildViewOptionsMenuChecks();

        /// @brief hide all options menu checks
        void hideViewOptionsMenuChecks();

        /// @brief check if select edges checkbox is enabled
        bool selectEdges() const;

        /// @brief check if select show connections checkbox is enabled
        bool showConnections() const;

        /// @brief menu check to select only edges
        FXMenuCheck* menuCheckSelectEdges;

        /// @brief menu check to show connections
        FXMenuCheck* menuCheckShowConnections;

        /// @brief menu check to hide connections in connect mode
        FXMenuCheck* menuCheckHideConnections;

        /// @brief menu check to extend to edge nodes
        FXMenuCheck* menuCheckExtendSelection;

        /// @brief menu check to set change all phases
        FXMenuCheck* menuCheckChangeAllPhases;

        /// @brief show grid button
        FXMenuCheck* menuCheckShowGrid;

    private:
        /// @brief pointer to viewNet
        GNEViewNet* myViewNet;
    };

    /// @brief view parent
    GNEViewParent* myViewParent;

    /// @brief Pointer to current net. (We are not responsible for deletion)
    GNENet* myNet;

    /// @brief the current Network edit mode
    NetworkEditMode myNetworkEditMode;

    /// @brief the current Demandedit mode
    DemandEditMode myDemandEditMode;

    /// @brief the current frame
    GNEFrame* myCurrentFrame;

    /// @name structs
    /// @{

    /// @brief variable used to save key status after certain events 
    KeyPressed myKeyPressed;

    /// @brief variable use to save all pointers to objects under cursor after a click
    ObjectsUnderCursor myObjectsUnderCursor;

    /// @brief variable used to save all elements related to creation of Edges
    CreateEdgeOptions myCreateEdgeOptions;

    /// @brief variable use to save pointers to moved elements
    MovedItems myMovedItems;

    /// @brief variable used to save variables related with movement of single elements
    MoveSingleElementValues myMoveSingleElementValues;

    /// @brief variable used to save variables related with movement of multiple elements
    MoveMultipleElementValues myMoveMultipleElementValues;

    /// @brief variable used to save variables related with selecting areas
    SelectingArea mySelectingArea;

    /// @brief variable used to save variables related with testing mode
    TestingMode myTestingMode;
    
    /// @brief variable used to save variables related with view options
    ViewOptions myViewOptions;
    // @}

    /// @brief a reference to the toolbar in View Parent
    FXToolBar* myToolbar;

    /// @name buttons  for selecting the edit mode
    /// @{
    /// @brief chekable button for edit mode create edge
    MFXCheckableButton* myEditModeCreateEdge;

    /// @brief chekable button for edit mode move
    MFXCheckableButton* myEditModeMove;

    /// @brief chekable button for edit mode delete
    MFXCheckableButton* myEditModeDelete;

    /// @brief chekable button for edit mode inspect
    MFXCheckableButton* myEditModeInspect;

    /// @brief chekable button for edit mode select
    MFXCheckableButton* myEditModeSelect;

    /// @brief chekable button for edit mode connection
    MFXCheckableButton* myEditModeConnection;

    /// @brief chekable button for edit mode traffic light
    MFXCheckableButton* myEditModeTrafficLight;

    /// @brief chekable button for edit mode additional
    MFXCheckableButton* myEditModeAdditional;

    /// @brief chekable button for edit mode crossing
    MFXCheckableButton* myEditModeCrossing;

    /// @brief chekable button for edit mode TAZ
    MFXCheckableButton* myEditModeTAZ;

    /// @brief chekable button for edit mode polygon
    MFXCheckableButton* myEditModePolygon;

    /// @brief checkable button for edit mode polygon
    MFXCheckableButton* myEditModeProhibition;

    /// @}

    /// @brief a reference to the undolist maintained in the application
    GNEUndoList* myUndoList;

    /**@brief current AttributeCarrier that is drawn using with a dotted contour 
     * note: it's constant because is edited from constant functions (example: drawGL(...) const)
     */
    const GNEAttributeCarrier* myDottedAC;

    /// @name variables for edit shapes
    /// @{
    /// @brief  polygon used for edit shapes
    GNEPoly* myEditShapePoly;

    /// @brief the previous edit mode before edit junction's shapes
    NetworkEditMode myPreviousNetworkEditMode;
    /// @}

    /// @brief set Network edit mode
    void setNetworkEditMode(NetworkEditMode networkMode);

    /// @brief set Demand edit mode
    void setDemandEditMode(DemandEditMode demandMode);

    /// @brief adds controls for setting the Network edit mode
    void buildNetworkEditModeControls();

    /// @brief updates Network mode specific controls
    void updateNetworkModeSpecificControls();

    /// @brief adds controls for setting the Demand edit mode
    void buildDemandEditModeControls();

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
   
    /// @brief Invalidated copy constructor.
    GNEViewNet(const GNEViewNet&) = delete;

    /// @brief Invalidated assignment operator.
    GNEViewNet& operator=(const GNEViewNet&) = delete;
};


#endif

/****************************************************************************/
