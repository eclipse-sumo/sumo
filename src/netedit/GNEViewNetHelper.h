/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEViewNetHelper.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2019
/// @version $Id$
///
// A file used to reduce the size of GNEViewNet.h grouping structs and classes
/****************************************************************************/
#ifndef GNEViewNetHelper_h
#define GNEViewNetHelper_h


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

/// @brie enum for supermodes
enum Supermode {
    /// @brief empty super mode
    GNE_SUPERMODE_NONE,
    /// @brief Network mode (Edges, junctions, etc..)
    GNE_SUPERMODE_NETWORK,
    ///@brief Demanding mode (Routes, Vehicles etc..)
    GNE_SUPERMODE_DEMAND
};

/// @brie enum for network edit modes
enum NetworkEditMode {
    /// @brief empty Network mode
    GNE_NMODE_NONE,
    ///@brief mode for inspecting network elements
    GNE_NMODE_INSPECT,
    ///@brief mode for deleting network elements
    GNE_NMODE_DELETE,
    ///@brief mode for selecting network elements
    GNE_NMODE_SELECT,
    ///@brief mode for moving network elements
    GNE_NMODE_MOVE,
    ///@brief mode for creating new edges
    GNE_NMODE_CREATE_EDGE,
    ///@brief mode for connecting lanes
    GNE_NMODE_CONNECT,
    ///@brief mode for editing tls
    GNE_NMODE_TLS,
    ///@brief Mode for editing additionals
    GNE_NMODE_ADDITIONAL,
    ///@brief Mode for editing crossing
    GNE_NMODE_CROSSING,
    ///@brief Mode for editing TAZ
    GNE_NMODE_TAZ,
    ///@brief Mode for editing Polygons
    GNE_NMODE_POLYGON,
    ///@brief Mode for editing connection prohibitions
    GNE_NMODE_PROHIBITION
};

/// @brie enum for demand edit modes
enum DemandEditMode {
    /// @brief empty Demand mode
    GNE_DMODE_NONE,
    ///@brief mode for inspecting demand elements
    GNE_DMODE_INSPECT,
    ///@brief mode for deleting demand elements
    GNE_DMODE_DELETE,
    ///@brief mode for selecting demand elements
    GNE_DMODE_SELECT,
    ///@brief mode for moving demand elements
    GNE_DMODE_MOVE,
    ///@brief Mode for editing routes
    GNE_DMODE_ROUTE,
    ///@brief Mode for editing vehicles
    GNE_DMODE_VEHICLE,
    ///@brief Mode for editing vehicle types
    GNE_DMODE_VEHICLETYPES,
    ///@brief Mode for editing stops
    GNE_DMODE_STOP,
    ///@brief Mode for editing person types
    GNE_DMODE_PERSONTYPES,
    ///@brief Mode for editing person
    GNE_DMODE_PERSON,
    ///@brief Mode for editing person plan
    GNE_DMODE_PERSONPLAN
};

// ===========================================================================
// class declarations
// ===========================================================================

// main elements
class GNEViewParent;
class GNEViewNet;
class GNEUndoList;
class GNEFrame;
class GNEAttributeCarrier;
// net elements
class GNENet;
class GNENetElement;
class GNEJunction;
class GNEEdge;
class GNELane;
class GNEConnection;
class GNECrossing;
class GNEInternalLane;
// additional elements
class GNEAdditional;
class GNEShape;
class GNEPoly;
class GNEPOI;
class GNETAZ;
// demand elements
class GNEDemandElement;
class GNEVehicleType;
class GNEFlow;
class GNEVehicle;
class GNEtrip;

// ===========================================================================
// classes and structs definitions
// ===========================================================================

struct GNEViewNetHelper {

    /// @brief class used to group all variables related with objects under cursor after a click over view
    class ObjectsUnderCursor {
    public:
        /// @brief constructor
        ObjectsUnderCursor();

        /// @brief update objects under cursor (Called only in onLeftBtnPress(...) function)
        void updateObjectUnderCursor(const std::vector<GUIGlObject*>& GUIGlObjects, GNEPoly* editedPolyShape);

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

        /// @brief get front net element element (or a pointer to nullptr if there isn't)
        GNEDemandElement* getDemandElementFront() const;

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
        const std::vector<GNEAttributeCarrier*>& getClickedAttributeCarriers() const;

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

        /// @brief vector with the clicked demand elements
        std::vector<GNEDemandElement*> myDemandElements;

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
        void sortGUIGlObjectsByAltitude(const std::vector<GUIGlObject*>& GUIGlObjects);

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
        void update(void* eventData);

        /// @brief check if SHIFT key was pressed during click
        bool shiftKeyPressed() const;

        /// @brief check if CONTROL key was pressed during click
        bool controlKeyPressed() const;

    private:
        /// @brief information of event
        FXEvent* myEventInfo;

        /// @brief Invalidated copy constructor.
        KeyPressed(const KeyPressed&) = delete;

        /// @brief Invalidated assignment operator.
        KeyPressed& operator=(const KeyPressed&) = delete;
    };

    /// @brief struct used to group all variables related with Supermodes
    struct EditModes {

        /// @brief default constructor
        EditModes(GNEViewNet* viewNet);

        /// @brief build checkable buttons
        void buildSuperModeButtons();

        /// @brief set Network edit mode
        void setSupermode(Supermode supermode);

        /// @brief set Network edit mode
        void setNetworkEditMode(NetworkEditMode networkMode, bool force = false);

        /// @brief set Demand edit mode
        void setDemandEditMode(DemandEditMode demandMode, bool force = false);

        /// @brief the current supermode
        Supermode currentSupermode;

        /// @brief the current Network edit mode
        NetworkEditMode networkEditMode;

        /// @brief the current Demand edit mode
        DemandEditMode demandEditMode;

        /// @brief chekable button for supermode Network
        MFXCheckableButton* networkButton;

        /// @brief chekable button for supermode Demand
        MFXCheckableButton* demandButton;

    private:
        /// @brief pointer to viewNet
        GNEViewNet* myViewNet;

        /// @brief Invalidated copy constructor.
        EditModes(const EditModes&) = delete;

        /// @brief Invalidated assignment operator.
        EditModes& operator=(const EditModes&) = delete;
    };

    /// @brief struct used to group all variables related to view options in all supermodes
    struct CommonViewOptions {

        /// @brief default constructor
        CommonViewOptions(GNEViewNet* viewNet);

        /// @brief build menu checks
        void buildCommonViewOptionsMenuChecks();

        /// @brief hide all options menu checks
        void hideCommonViewOptionsMenuChecks();

        /// @brief get visible common menu commands
        void getVisibleCommonMenuCommands(std::vector<FXMenuCheck*>& commands) const;

        /// @brief menu check to show grid button
        FXMenuCheck* menuCheckShowGrid;

    private:
        /// @brief pointer to viewNet
        GNEViewNet* myViewNet;

        /// @brief Invalidated copy constructor.
        CommonViewOptions(const CommonViewOptions&) = delete;

        /// @brief Invalidated assignment operator.
        CommonViewOptions& operator=(const CommonViewOptions&) = delete;
    };

    /// @brief struct used to group all variables related to view options in supermode Network
    struct NetworkViewOptions {

        /// @brief default constructor
        NetworkViewOptions(GNEViewNet* viewNet);

        /// @brief build menu checks
        void buildNetworkViewOptionsMenuChecks();

        /// @brief hide all options menu checks
        void hideNetworkViewOptionsMenuChecks();

        /// @brief get visible network menu commands
        void getVisibleNetworkMenuCommands(std::vector<FXMenuCheck*>& commands) const;

        /// @brief check if show demand elements checkbox is enabled
        bool showDemandElements() const;

        /// @brief check if select edges checkbox is enabled
        bool selectEdges() const;

        /// @brief check if select show connections checkbox is enabled
        bool showConnections() const;

        /// @brief check if we're editing elevation
        bool editingElevation() const;

        /// @brief menu check to show Demand Elements
        FXMenuCheck* menuCheckShowDemandElements;

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

        /// @brief menu check to we should warn about merging junctions
        FXMenuCheck* menuCheckWarnAboutMerge;

        /// @brief menu check to show connection as buuble in "Move" mode.
        FXMenuCheck* menuCheckShowJunctionBubble;

        /// @brief menu check to apply movement to elevation
        FXMenuCheck* menuCheckMoveElevation;

        /// @brief menu check to the endpoint for a created edge should be set as the new source
        FXMenuCheck* menuCheckChainEdges;

        /// @brief menu check to create auto create opposite edge
        FXMenuCheck* menuCheckAutoOppositeEdge;
    private:
        /// @brief pointer to viewNet
        GNEViewNet* myViewNet;

        /// @brief Invalidated copy constructor.
        NetworkViewOptions(const NetworkViewOptions&) = delete;

        /// @brief Invalidated assignment operator.
        NetworkViewOptions& operator=(const NetworkViewOptions&) = delete;
    };

    /// @brief struct used to group all variables related to view options in supermode Demand
    struct DemandViewOptions {

        /// @brief default constructor
        DemandViewOptions(GNEViewNet* viewNet);

        /// @brief build menu checks
        void buildDemandViewOptionsMenuChecks();

        /// @brief hide all options menu checks
        void hideDemandViewOptionsMenuChecks();

        /// @brief get visible demand menu commands
        void getVisibleDemandMenuCommands(std::vector<FXMenuCheck*>& commands) const;

        /// @brief check if non inspected element has to be hidden
        bool showNonInspectedDemandElements(const GNEDemandElement* demandElement) const;

        /// @brief check if shapes has to be hide
        bool showShapes() const;

        /// @brief check all person plans has to be show
        bool showAllPersonPlans() const;

        /// @brief lock person
        void lockPerson(const GNEDemandElement* person);

        /// @brief unlock person
        void unlockPerson();

        /// @brief get locked person
        const GNEDemandElement* getLockedPerson() const;

        /// @brief Hide shapes (Polygons and POIs)
        FXMenuCheck* menuCheckHideShapes;

        /// @brief Hide non inspected demand elements
        FXMenuCheck* menuCheckHideNonInspectedDemandElements;

        /// @brief show all person plans
        FXMenuCheck* menuCheckShowAllPersonPlans;

        /// @brief Lock Person
        FXMenuCheck* menuCheckLockPerson;

    private:
        /// @brief pointer to viewNet
        GNEViewNet* myViewNet;

        /// @brief pointer to locked person
        const GNEDemandElement* myLockedPerson;

        /// @brief Invalidated copy constructor.
        DemandViewOptions(const DemandViewOptions&) = delete;

        /// @brief Invalidated assignment operator.
        DemandViewOptions& operator=(const DemandViewOptions&) = delete;
    };

    /// @brief struct used to group all variables related with movement of single elements
    struct MoveSingleElementValues {

        /// @brief constructor
        MoveSingleElementValues(GNEViewNet* viewNet);

        /// @brief begin move single element in Network mode
        bool beginMoveSingleElementNetworkMode();

        /// @brief begin move single element in Demand mode
        bool beginMoveSingleElementDemandMode();

        /// @brief move single element in Network AND Demand mode
        void moveSingleElement();

        /// @brief finish moving single elements in Network AND Demand mode
        void finishMoveSingleElement();

        /// @brief original shape of element before start moving (used by polygons, edges, etc., needed for commmit position changes)
        PositionVector originalShapeBeforeMoving;

        /// @brief index moved
        int movingIndexShape;

        /// @brief original position of geometry position (needed for commmit position changes)
        Position originalPositionInView;

    private:
        /// calculate Poly movement values (Position, Index, etc.)
        bool calculatePolyValues();

        /// calculate Edge movement values (Position, Index, etc.)
        bool calculateEdgeValues();

        /// calculate TAZ movement values (Position, Index, etc.)
        bool calculateTAZValues();

        /// @brief pointer to viewNet
        GNEViewNet* myViewNet;

        /// @brief relative position of Clicked Position regarding to originalGeometryPointPosition (Used when user doesn't click exactly over the center of element)
        Position myRelativeClickedPosition;

        /// @brief bool to indicate that startPos of an edge is being moved
        bool myMovingStartPos;

        /// @brief bool to indicate that end pos of an edge is being moved
        bool myMovingEndPos;

        /// @brief the Junction to be moved.
        GNEJunction* myJunctionToMove;

        /// @brief the edge of which geometry is being moved
        GNEEdge* myEdgeToMove;

        /// @brief the poly of which geometry is being moved
        GNEPoly* myPolyToMove;

        /// @brief the poi which position is being moved
        GNEPOI* myPOIToMove;

        /// @brief the additional element which position is being moved
        GNEAdditional* myAdditionalToMove;

        /// @brief the demand element which position is being moved
        GNEDemandElement* myDemandElementToMove;

        /// @brief the TAZ element which their Shape is being moved (it's the only additional with a shape instead a position)
        GNETAZ* myTAZToMove;
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

    /// @brief struct used to group all variables related with movement of groups of elements
    struct VehicleOptions {

        /// @brief constructor
        VehicleOptions(GNEViewNet* viewNet);

        /// @brief build menu checks
        void buildVehicleOptionsMenuChecks();

        /// @brief hide all options menu checks
        void hideVehicleOptionsMenuChecks();

    private:
        /// @brief pointer to viewNet
        GNEViewNet* myViewNet;
    };

    /// @brief struct used to group all variables related with movement of groups of elements
    struct VehicleTypeOptions {

        /// @brief constructor
        VehicleTypeOptions(GNEViewNet* viewNet);

        /// @brief build menu checks
        void buildVehicleTypeOptionsMenuChecks();

        /// @brief hide all options menu checks
        void hideVehicleTypeOptionsMenuChecks();

    private:
        /// @brief pointer to viewNet
        GNEViewNet* myViewNet;
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
        void processShapeSelection(const PositionVector& shape);

        /// @brief draw rectangle selection
        void drawRectangleSelection(const RGBColor& color) const;

        /// @brief whether we have started rectangle-selection
        bool selectingUsingRectangle;

        /// @brief whether we have started rectangle-selection
        bool startDrawing;

    private:
        /// @brief Process boundary Selection
        void processBoundarySelection(const Boundary& boundary);

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

    /// @brief struct used to group all variables related with common chekable Buttons
    struct CommonCheckableButtons {

        /// @brief default constructor
        CommonCheckableButtons(GNEViewNet* viewNet);

        /// @brief build checkable buttons
        void buildCommonCheckableButtons();

        /// @brief show all Common Checkable Buttons
        void showCommonCheckableButtons();

        /// @brief hide all Common Checkable Buttons
        void hideCommonCheckableButtons();

        /// @brief hide all options menu checks
        void disableCommonCheckableButtons();

        /// @brief update Common checkable buttons
        void updateCommonCheckableButtons();

        /// @brief chekable button for edit mode inspect
        MFXCheckableButton* inspectButton;

        /// @brief chekable button for edit mode delete
        MFXCheckableButton* deleteButton;

        /// @brief chekable button for edit mode select
        MFXCheckableButton* selectButton;

        /// @brief chekable button for edit mode move
        MFXCheckableButton* moveButton;

    private:
        /// @brief pointer to viewNet
        GNEViewNet* myViewNet;
    };

    /// @brief struct used to group all variables related with Network chekable Buttons
    struct NetworkCheckableButtons {

        /// @brief default constructor
        NetworkCheckableButtons(GNEViewNet* viewNet);

        /// @brief build checkable buttons
        void buildNetworkCheckableButtons();

        /// @brief show all Network Checkable Buttons
        void showNetworkCheckableButtons();

        /// @brief hide all Network Checkable Buttons
        void hideNetworkCheckableButtons();

        /// @brief hide all options menu checks
        void disableNetworkCheckableButtons();

        /// @brief update network checkable buttons
        void updateNetworkCheckableButtons();

        /// @brief chekable button for edit mode create edge
        MFXCheckableButton* createEdgeButton;

        /// @brief chekable button for edit mode connection
        MFXCheckableButton* connectionButton;

        /// @brief chekable button for edit mode traffic light
        MFXCheckableButton* trafficLightButton;

        /// @brief chekable button for edit mode additional
        MFXCheckableButton* additionalButton;

        /// @brief chekable button for edit mode crossing
        MFXCheckableButton* crossingButton;

        /// @brief chekable button for edit mode TAZ
        MFXCheckableButton* TAZButton;

        /// @brief chekable button for edit mode shape
        MFXCheckableButton* shapeButton;

        /// @brief checkable button for edit mode polygon
        MFXCheckableButton* prohibitionButton;

    private:
        /// @brief pointer to viewNet
        GNEViewNet* myViewNet;
    };

    /// @brief struct used to group all variables related with Demand checkable Buttons
    struct DemandCheckableButtons {

        /// @brief default constructor
        DemandCheckableButtons(GNEViewNet* viewNet);

        /// @brief build checkable buttons
        void buildDemandCheckableButtons();

        /// @brief show all Demand Checkable Buttons
        void showDemandCheckableButtons();

        /// @brief hide all Demand Checkable Buttons
        void hideDemandCheckableButtons();

        /// @brief hide all options menu checks
        void disableDemandCheckableButtons();

        /// @brief update Demand checkable buttons
        void updateDemandCheckableButtons();

        /// @brief chekable button for edit mode create routes
        MFXCheckableButton* routeButton;

        /// @brief chekable button for edit mode create vehicles
        MFXCheckableButton* vehicleButton;

        /// @brief chekable button for edit mode create vehicle type
        MFXCheckableButton* vehicleTypeButton;

        /// @brief chekable button for edit mode create stops
        MFXCheckableButton* stopButton;

        /// @brief chekable button for edit mode create person type
        MFXCheckableButton* personTypeButton;

        /// @brief chekable button for edit mode create persons
        MFXCheckableButton* personButton;

        /// @brief chekable button for edit mode create person plans
        MFXCheckableButton* personPlanButton;

    private:
        /// @brief pointer to viewNet
        GNEViewNet* myViewNet;
    };

    /// @brief struct used to group all variables related with edit shapes of NetElements
    struct EditShapes {

        /// @brief default constructor
        EditShapes(GNEViewNet* viewNet);

        /// @brief start edit custom shape
        void startEditCustomShape(GNENetElement* element, const PositionVector& shape, bool fill);

        /// @brief edit edit shape
        void stopEditCustomShape();

        /// @brief save edited shape
        void saveEditedShape();

        /// @brief polygon used for edit shapes
        GNEPoly* editedShapePoly;

        /// @brief flag to edit net element shapes
        bool editingNetElementShapes;

    private:
        /// @brief the previous edit mode before edit NetElement's shapes
        NetworkEditMode myPreviousNetworkEditMode;

        /// @brief pointer to viewNet
        GNEViewNet* myViewNet;
    };
};

#endif

/****************************************************************************/
