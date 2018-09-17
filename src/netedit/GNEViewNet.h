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

#include <string>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/Position.h>
#include <utils/common/RGBColor.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/common/StringBijection.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/options/OptionsCont.h>


// ===========================================================================
// enum
// ===========================================================================

enum EditMode {
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
    ///@brief Mode for editing Polygons
    GNE_MODE_POLYGON,
    ///@brief Mode for editing connection prohibits
    GNE_MODE_PROHIBITION
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
class GNENetElement;
class GNECrossing;
class GNEAttributeCarrier;

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
    virtual ~GNEViewNet();

    /// @brief builds the view toolbars
    virtual void buildViewToolBars(GUIGlChildWindow&);

    /** @brief Builds an entry which allows to (de)select the object
     * @param ret The popup menu to add the entry to
     * @param AC AttributeCarrier that will be select/unselected
     */
    void buildSelectionACPopupEntry(GUIGLObjectPopupMenu* ret, GNEAttributeCarrier* AC);

    /// @brief set color scheme
    bool setColorScheme(const std::string& name);

    ///@brief recalibrate color scheme according to the current value range
    void buildColorRainbow(GUIColorScheme& scheme, int active, GUIGlObjectType objectType);

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

    /// @brief get the current edit mode
    EditMode getCurrentEditMode() const;

    /// @brief get grid button
    FXMenuCheck* getMenuCheckShowGrid() const;

    /// @brief get AttributeCarrier under cursor
    const GNEAttributeCarrier* getACUnderCursor() const;

    /// @brief set attributeCarrier under cursor
    void setACUnderCursor(const GNEAttributeCarrier* AC);

    /// @brief check if lock icon should be visible
    bool showLockIcon() const;

    /// @brief set staturBar text
    void setStatusBarText(const std::string& text);

    /// @brief whether inspection, selection and inversion should apply to edges or to lanes
    bool selectEdges();

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

    /// @brief begin move selection
    void beginMoveSelection(GNEAttributeCarrier* originAC, const Position& originPosition);

    /// @brief move selection
    void moveSelection(const Position& offset);

    /// @brief finish moving selection
    void finishMoveSelection();

protected:
    /// @brief FOX needs this
    GNEViewNet() {}

    /// @brief do paintGL
    int doPaintGL(int mode, const Boundary& bound);

    /// @brief called after some features are already initialized
    void doInit();

private:
    /// @brief struct used to group all pointers to moved elements
    struct MovedItems {
        /// @brief constructor
        MovedItems() :
            junctionToMove(nullptr),
            edgeToMove(nullptr),
            polyToMove(nullptr),
            poiToMove(nullptr),
            additionalToMove(nullptr) {}

        /// @brief the Junction to be moved.
        GNEJunction* junctionToMove;

        /// @brief the edge of which geometry is being moved
        GNEEdge* edgeToMove;

        /// @brief the poly of which geometry is being moved
        GNEPoly* polyToMove;

        /// @brief the poi which is being moved
        GNEPOI* poiToMove;

        /// @brief the stoppingPlace element which shape is being moved
        GNEAdditional* additionalToMove;
    };

    /// @brief struct used to group all variables related with movement of single elements
    struct MoveSingleElementValues {
        /// @brief constructor
        MoveSingleElementValues() :
            movingStartPos(false),
            movingEndPos(false),
            movingIndexShape(-1) {}

        /// @brief variable for calculating moving offset (Used when user doesn't click exactly over the center of shape)
        Position movingReference;

        /// @brief original Position of element before moving (needed for commmit position changes)
        Position movingOriginalPosition;

        /// @brief Shape of elements before moving (needed for commmit shape changes)
        PositionVector movingOriginalShape;
        PositionVector movingOriginalShape2;

        /// @brief bool to indicate that startPos are being moved
        bool movingStartPos;
        bool movingEndPos;

        /// @brief current index of shape that are being moved
        int movingIndexShape;
    };

    /// @brief class used to group all variables related with objects under cursor after a click over view
    class ObjectsUnderCursor {
    public:
        /// @brief constructor
        ObjectsUnderCursor():
            eventInfo(nullptr),
            glID(0),
            glType(GLO_NETWORK),
            attributeCarrier(nullptr),
            netElement(nullptr),
            additional(nullptr),
            shape(nullptr),
            junction(nullptr),
            edge(nullptr),
            lane(nullptr),
            crossing(nullptr),
            connection(nullptr),
            poi(nullptr),
            poly(nullptr) {}

        /// @brief update objects under cursor (Called only in onLeftBtnPress(...) function)
        void updateObjectUnderCursor(GUIGlID glIDObject, GNEPoly* editedPolyShape, FXEvent* ev);

        /// @brief swap lane to edge
        void swapLane2Edge();

        /// @brief check if SHIFT key was pressed during click
        bool shiftKeyPressed() const;

        /// @brief check if CONTROL key was pressed during click
        bool controlKeyPressed() const;

        /// @brief information of event of onLeftBtnPress
        FXEvent* eventInfo;

        /// @brief GUI GL ID
        GUIGlID glID;

        /// @brief GUI GL object type
        GUIGlObjectType glType;

        /// @brief attribute carrier
        GNEAttributeCarrier* attributeCarrier;

        /// @brief net element
        GNENetElement* netElement;

        /// @brief additional element
        GNEAdditional* additional;

        /// @brief shape element (Poly and POIs)
        GNEShape* shape;

        /// @brief junction
        GNEJunction* junction;

        /// @brief edge
        GNEEdge* edge;

        /// @brief lane
        GNELane* lane;

        /// @brief crossing
        GNECrossing* crossing;

        /// @brief connection
        GNEConnection* connection;

        /// @brief POI
        GNEPOI* poi;

        /// @brief Poly
        GNEPoly* poly;
    };

    /// @brief struct used to group all variables related with selecting using a square or polygon
    /// @note in the future the variables used for selecting throught a polygon will be placed here
    struct SelectingArea {
        /// @brief default constructor
        SelectingArea() :
            selectingUsingRectangle(false) {}

        /// @brief Process Selection
        void processSelection(GNEViewNet* viewNet, bool shiftKeyPressed);

        /// @brief whether we have started rectangle-selection
        bool selectingUsingRectangle;

        /// @brief firstcorner of the rectangle-selection
        Position selectionCorner1;

        /// @brief second corner of the rectangle-selection
        Position selectionCorner2;

        /// @brief inform about selection size
        std::string reportDimensions();
    };

    /// @brief struct used to group all variables related with testing
    struct testingMode {
        /// @brief default constructor
        testingMode() :
            testingEnabled(OptionsCont::getOptions().getBool("gui-testing")),
            testingWidth(0),
            testingHeight(0) {}

        /// @brief flag to enable or disable testing mode
        bool testingEnabled;

        /// @brief Width of viewNet in testing mode
        int testingWidth;

        /// @brief Height of viewNet in testing mode
        int testingHeight;
    };

    /// @brief view parent
    GNEViewParent* myViewParent;

    /// @brief Pointer to current net. (We are not responsible for deletion)
    GNENet* myNet;

    /// @brief the current edit mode
    EditMode myEditMode;

    /// @brief the current frame
    GNEFrame* myCurrentFrame;

    /// @brief menu check to select only edges
    FXMenuCheck* myMenuCheckSelectEdges;

    /// @brief menu check to show connections
    FXMenuCheck* myMenuCheckShowConnections;

    /// @brief menu check to hide connections in connect mode
    FXMenuCheck* myMenuCheckHideConnections;

    /// @brief menu check to extend to edge nodes
    FXMenuCheck* myMenuCheckExtendToEdgeNodes;

    /// @brief menu check to set change all phases
    FXMenuCheck* myMenuCheckChangeAllPhases;

    /// @brief show grid button
    FXMenuCheck* myMenuCheckShowGrid;

    /// @brief whether show connections has been activated once
    bool myShowConnections;

    /// @brief flag to check if select edges is enabled
    bool mySelectEdges;

    /// @brief flag to check if shift key is pressed (can be changed after Key Press/Released and mouse Move)
    bool myShiftKeyPressed;

    /// @name the state-variables of the create-edge state-machine
    // @{
    /// @brief source junction for new edge 0 if no edge source is selected an existing (or newly created) junction otherwise
    GNEJunction* myCreateEdgeSource;

    /// @brief whether the endpoint for a created edge should be set as the new source
    FXMenuCheck* myChainCreateEdge;
    FXMenuCheck* myAutoCreateOppositeEdge;
    // @}

    /// @name the state-variables of the move state-machine
    // @{
    /// @brief whether we should warn about merging junctions
    FXMenuCheck* myMenuCheckWarnAboutMerge;

    /// @brief show connection as buuble in "Move" mode.
    FXMenuCheck* myMenuCheckShowBubbleOverJunction;

    /// @brief apply movement to elevation
    FXMenuCheck* myMenuCheckMoveElevation;
    // @}

    /// @brief variable use to save all pointers to objects under cursor after a click
    ObjectsUnderCursor myObjectsUnderCursor;

    /// @brief variable use to save pointers to moved elements
    MovedItems myMovedItems;

    /// @brief variable used to save variables related with movement of single elements
    MoveSingleElementValues myMoveSingleElementValues;

    /// @brief variable used to save variables related with selecting areas
    SelectingArea mySelectingArea;

    /// @brief variable used to save variables related with testing mode
    testingMode myTestingMode;

    /// @brief whether a selection is being moved
    bool myMovingSelection;

    /// @brief Selected Junctions that are being moved
    /** NOTE: IN the future will be changed to std::map<GNENetElement*, Position> **/
    std::map<GNEJunction*, Position> myOriginPositionOfMovedJunctions;

    /// @brief Selected Edges that are being moved < Edge, PositionVector >
    std::map<GNEEdge*, PositionVector> myOriginShapesMovedEntireShapes;

    struct MovingEdges {
        PositionVector originalShape;
        int index;
        Position originalPosition;
    };

    std::map<GNEEdge*, MovingEdges> myOriginShapesMovedPartialShapes;
    // @}

    /// @brief a reference to the toolbar in myParent
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

    /// @brief chekable button for edit mode polygon
    MFXCheckableButton* myEditModePolygon;

    /// @brief checkable button for edit mode polygon
    MFXCheckableButton* myEditModeProhibition;

    /// @}

    /// @brief since we cannot switch on strings we map the mode names to an enum
    /// @{
    /// @brief stringBijection for edit mode names
    StringBijection<EditMode> myEditModeNames;

    /// @brief stringBijection for edit additional mode names
    StringBijection<EditMode> myEditAdditionalModeNames;
    /// @}

    /// @brief a reference to the undolist maintained in the application
    GNEUndoList* myUndoList;

    /// @brief current AttributeCarrier under Mouse position
    const GNEAttributeCarrier* myACUnderCursor;

    /// @name variables for edit shapes
    /// @{
    /// @brief  polygon used for edit shapes
    GNEPoly* myEditShapePoly;

    /// @brief the previous edit mode before edit junction's shapes
    EditMode myPreviousEditMode;
    /// @}

    /// @brief set edit mode
    void setEditMode(EditMode mode);

    /// @brief adds controls for setting the edit mode
    void buildEditModeControls();

    /// @brief updates mode specific controls
    void updateModeSpecificControls();

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

    /// @brief try to retrieve multiple edges at popup position
    std::set<GNEEdge*> getEdgesAtPopupPosition();

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
    void processClick(FXEvent* e, void* data);

    /// @brief Invalidated copy constructor.
    GNEViewNet(const GNEViewNet&) = delete;

    /// @brief Invalidated assignment operator.
    GNEViewNet& operator=(const GNEViewNet&) = delete;
};


#endif

/****************************************************************************/
