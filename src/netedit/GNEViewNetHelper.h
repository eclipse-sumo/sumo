/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEViewNetHelper.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2019
///
// A file used to reduce the size of GNEViewNet.h grouping structs and classes
/****************************************************************************/
#pragma once
#include <config.h>

#include <unordered_set>
#include <netedit/elements/moving/GNEMoveElement.h>
#include <utils/foxtools/MFXButtonTooltip.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/foxtools/MFXLCDLabel.h>
#include <utils/foxtools/MFXMenuButtonTooltip.h>
#include <utils/gui/div/GUIGlobalViewObjectsHandler.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>
#include <utils/xml/CommonXMLStructure.h>

// ===========================================================================
// enum
// ===========================================================================

/// @brie enum for supermodes
enum class Supermode {
    /// @brief Network mode (Edges, junctions, etc..)
    NETWORK,
    ///@brief Demand mode (Routes, Vehicles etc..)
    DEMAND,
    ///@brief Data mode (edgeData, LaneData etc..)
    DATA
};

/// @brie enum for network edit modes
enum class NetworkEditMode {
    /// @brief empty Network mode
    NETWORK_NONE,
    ///@brief mode for inspecting network elements
    NETWORK_INSPECT,
    ///@brief mode for deleting network elements
    NETWORK_DELETE,
    ///@brief mode for selecting network elements
    NETWORK_SELECT,
    ///@brief mode for moving network elements
    NETWORK_MOVE,
    ///@brief mode for creating new edges
    NETWORK_CREATE_EDGE,
    ///@brief mode for connecting lanes
    NETWORK_CONNECT,
    ///@brief mode for editing tls
    NETWORK_TLS,
    ///@brief Mode for editing additionals
    NETWORK_ADDITIONAL,
    ///@brief Mode for editing crossing
    NETWORK_CROSSING,
    ///@brief Mode for editing TAZ
    NETWORK_TAZ,
    ///@brief Mode for editing Polygons
    NETWORK_SHAPE,
    ///@brief Mode for editing connection prohibitions
    NETWORK_PROHIBITION,
    ///@brief Mode for editing wires
    NETWORK_WIRE,
    ///@brief Mode for editing decals
    NETWORK_DECAL
};

/// @brie enum for demand edit modes
enum class DemandEditMode {
    /// @brief empty Demand mode
    DEMAND_NONE,
    ///@brief mode for inspecting demand elements
    DEMAND_INSPECT,
    ///@brief mode for deleting demand elements
    DEMAND_DELETE,
    ///@brief mode for selecting demand elements
    DEMAND_SELECT,
    ///@brief mode for moving demand elements
    DEMAND_MOVE,
    ///@brief Mode for editing routes
    DEMAND_ROUTE,
    ///@brief Mode for editing route distributions
    DEMAND_ROUTEDISTRIBUTION,
    ///@brief Mode for editing vehicles
    DEMAND_VEHICLE,
    ///@brief Mode for editing types
    DEMAND_TYPE,
    ///@brief Mode for editing type distributions
    DEMAND_TYPEDISTRIBUTION,
    ///@brief Mode for editing stops
    DEMAND_STOP,
    ///@brief Mode for editing person
    DEMAND_PERSON,
    ///@brief Mode for editing person plan
    DEMAND_PERSONPLAN,
    ///@brief Mode for editing container
    DEMAND_CONTAINER,
    ///@brief Mode for editing container plan
    DEMAND_CONTAINERPLAN
};

/// @brief enum for data edit modes
enum class DataEditMode {
    /// @brief empty Data mode
    DATA_NONE,
    /// @brief mode for inspecting data elements
    DATA_INSPECT,
    /// @brief mode for deleting data elements
    DATA_DELETE,
    /// @brief mode for selecting data elements
    DATA_SELECT,
    /// @brief mode for create edgeData elements
    DATA_EDGEDATA,
    /// @brief mode for create edgeRelData elements
    DATA_EDGERELDATA,
    /// @brief mode for create TAZRelData elements
    DATA_TAZRELDATA,
    /// @brief mode for create meanData elements
    DATA_MEANDATA
};

// ===========================================================================
// class declarations
// ===========================================================================

class GNEAttributeCarrier;
class GNEFrame;
class GNENet;
class GNETagPropertiesDatabase;
class GNEUndoList;
class GNEViewNet;
class GNEViewParent;
// dialogs
class GNEVClassesDialog;
class GNEFixNetworkElements;
class GNEFixAdditionalElementsDialog;
class GNEFixDemandElementsDialog;
// network elements
class GNENetworkElement;
class GNEJunction;
class GNEEdge;
class GNELane;
class GNEConnection;
class GNECrossing;
class GNEWalkingArea;
class GNEInternalLane;
// additional elements
class GNEAdditional;
class GNEPoly;
class GNEPOI;
class GNETAZ;
// demand elements
class GNEDemandElement;
// data elements
class GNEDataSet;
class GNEGenericData;
class GNEEdgeData;
class GNEEdgeRelData;
class GNETAZRelData;

// ===========================================================================
// classes and structs definitions
// ===========================================================================

struct GNEViewNetHelper {

    /// @brief lock manager
    class LockManager {

    public:
        /// @brief constructor
        LockManager(GNEViewNet* viewNet);

        /// @brief destructor
        ~LockManager();

        /// @brief check if given GLObject is locked for inspect, select, delete and move
        bool isObjectLocked(GUIGlObjectType objectType, const bool selected) const;

        /// @brief update flags
        void updateFlags();

        /// @brief update lock inspect menuBar
        void updateLockMenuBar();

    private:
        /// @brief operation locked
        class OperationLocked {

        public:
            /// @brief constructor
            OperationLocked();

            /// @brief parameter constructor
            OperationLocked(Supermode supermode);

            /// @brief destructor
            ~OperationLocked();

            /// @brief get supermode
            Supermode getSupermode() const;

            /// @brief flag for lock/unlock
            bool lock = false;

        private:
            /// @brief supermode associated with this operation locked
            Supermode mySupermode;
        };

        /// @brief pointer to viewNet
        GNEViewNet* myViewNet;

        /// @brief map with locked elements
        std::map<GUIGlObjectType, OperationLocked> myLockedElements;

        /// @brief Invalidated copy constructor.
        LockManager(const LockManager&) = delete;

        /// @brief Invalidated assignment operator.
        LockManager& operator=(const LockManager&) = delete;
    };

    /// @brief class used for group inspected elements
    class InspectedElements {

    public:
        /// @brief constructor
        InspectedElements();

        /// @name inspect functions
        /// @{
        /// @brief inspect AC
        void inspectAC(GNEAttributeCarrier* AC);

        /// @brief inspect multiple ACs
        void inspectACs(const std::vector<GNEAttributeCarrier*>& ACs);

        /// @brief uninspect AC
        void uninspectAC(GNEAttributeCarrier* AC);

        /// @brief clear inspected AC
        void clearInspectedElements();

        /// @}

        /// @name get functions
        /// @{
        /// @brief get first inspected AC (needed because the main container is a hash)
        GNEAttributeCarrier* getFirstAC() const;

        /// @brief get hash table with all inspected ACs
        const std::unordered_set<GNEAttributeCarrier*>& getACs() const;

        /// @}

        /// @name check functions
        /// @{
        // @brief check if the given AC is inspected
        bool isACInspected(GNEAttributeCarrier* AC) const;

        // @brief check if the given constant AC is inspected
        bool isACInspected(const GNEAttributeCarrier* AC) const;

        /// @brief check if we're inspecting at least one element
        bool isInspectingElements() const;

        /// @brief check if we're inspecting exactly one element
        bool isInspectingSingleElement() const;

        /// @brief check if we're inspecting more than one elements
        bool isInspectingMultipleElements() const;

        /// @}

    private:
        /// @brief first inspected element (usually the clicked element)
        GNEAttributeCarrier* myFirstInspectedAC = nullptr;

        /// @brief hash table with all inspected ACs (we use a set to make deletion of massive elements more quickly)
        std::unordered_set<GNEAttributeCarrier*> myInspectedACs;

        /// @brief Invalidated copy constructor.
        InspectedElements(const InspectedElements&) = delete;

        /// @brief Invalidated assignment operator.
        InspectedElements& operator=(const InspectedElements&) = delete;
    };

    /// @brief class used for group front elements
    class MarkFrontElements {

    public:
        /// @brief constructor
        MarkFrontElements();

        /// @brief mark AC as drawing front
        void markAC(GNEAttributeCarrier* AC);

        /// @brief unmark AC for drawing front
        void unmarkAC(GNEAttributeCarrier* AC);

        /// @brief unmark all ACs
        void unmarkAll();

        /// @brief get hash table with all fronted ACs
        const std::unordered_set<GNEAttributeCarrier*>& getACs() const;

    private:
        /// @brief hash table with all marked ACs (we use a set to make deletion of massive elements more quickly)
        std::unordered_set<GNEAttributeCarrier*> myMarkedACs;

        /// @brief Invalidated copy constructor.
        MarkFrontElements(const MarkFrontElements&) = delete;

        /// @brief Invalidated assignment operator.
        MarkFrontElements& operator=(const MarkFrontElements&) = delete;
    };

    /// @brief class used to group all variables related with objects under cursor after a click over view
    class ViewObjectsSelector {

    public:
        /// @brief constructor
        ViewObjectsSelector(GNEViewNet* viewNet);

        /// @brief update objects (using gViewObjectsHandler)
        void updateObjects();

        /// @brief update merging junctions
        void updateMergingJunctions();

        /// @brief fill the given SUMO base object with the current single objects
        void fillSumoBaseObject(CommonXMLStructure::SumoBaseObject* baseObjet) const;

        /// @brief filter all elements except the given GLO type
        void filterAllExcept(GUIGlObjectType exception);

        /// @brief filter by supermode
        void filterBySuperMode();

        /// @brief filter (remove) junctions
        void filterJunctions();

        /// @brief filter (remove) edges
        void filterEdges();

        /// @brief filter (remove) lanes
        void filterLanes();

        /// @brief filter (remove) connections
        void filterConnections();

        /// @brief filter (remove) crossings
        void filterCrossings();

        /// @brief filter (remove) walkingAreas
        void filterWalkingAreas();

        /// @brief filter (remove) polys and POIs
        void filterShapes();

        /// @brief filter (remove) additionals
        void filterAdditionals(const bool includeStoppigPlaces, const bool includeTAZs);

        /// @brief filter (remove) network elements
        void filterNetworkElements();

        /// @brief filter (remove) demand elements
        void filterDemandElements(const bool includeRoutes);

        /// @brief filter (remove) datga elements
        void filterDataElements();

        /// @brief filter locked elements (except the ignoreFilter)
        void filterLockedElements(const std::vector<GUIGlObjectType> ignoreFilter = {});

        /// @brief get front GUIGLObject or a pointer to nullptr
        const GUIGlObject* getGUIGlObjectFront() const;

        /// @brief get front attribute carrier or a pointer to nullptr
        GNEAttributeCarrier* getAttributeCarrierFront() const;

        /// @brief get front network element or a pointer to nullptr
        GNENetworkElement* getNetworkElementFront() const;

        /// @brief get front additional element or a pointer to nullptr
        GNEAdditional* getAdditionalFront() const;

        /// @brief get front demand element or a pointer to nullptr
        GNEDemandElement* getDemandElementFront() const;

        /// @brief get generic data element or a pointer to nullptr
        GNEGenericData* getGenericDataElementFront() const;

        /// @brief get front junction or a pointer to nullptr
        GNEJunction* getJunctionFront() const;

        /// @brief get front edge or a pointer to nullptr
        GNEEdge* getEdgeFront() const;

        /// @brief get front lane or a pointer to nullptr
        GNELane* getLaneFront() const;

        /// @brief get front lane or a pointer to nullptr checking if is locked
        GNELane* getLaneFrontNonLocked() const;

        /// @brief get lanes
        const std::vector<GNELane*>& getLanes() const;

        /// @brief get front crossing or a pointer to nullptr
        GNECrossing* getCrossingFront() const;

        /// @brief get front walkingArea or a pointer to nullptr
        GNEWalkingArea* getWalkingAreaFront() const;

        /// @brief get front connection or a pointer to nullptr
        GNEConnection* getConnectionFront() const;

        /// @brief get front internal lane or a pointer to nullptr
        GNEInternalLane* getInternalLaneFront() const;

        /// @brief get front TAZ or a pointer to nullptr
        GNETAZ* getTAZFront() const;

        /// @brief get front POI or a pointer to nullptr
        GNEPOI* getPOIFront() const;

        /// @brief get front Poly or a pointer to nullptr
        GNEPoly* getPolyFront() const;

        /// @brief get edge data element or a pointer to nullptr
        GNEEdgeData* getEdgeDataElementFront() const;

        /// @brief get edge rel data element or a pointer to nullptr
        GNEEdgeRelData* getEdgeRelDataElementFront() const;

        /// @brief get TAZ rel data element or a pointer to nullptr
        GNETAZRelData* getTAZRelDataElementFront() const;

        /// @brief get vector with GL objects
        const std::vector<GUIGlObject*>& getGLObjects() const;

        /// @brief get vector with ACs
        const std::vector<GNEAttributeCarrier*>& getAttributeCarriers() const;

        /// @brief get vector with junctions
        const std::vector<GNEJunction*>& getJunctions() const;

        /// @brief get vector with edges
        const std::vector<GNEEdge*>& getEdges() const;

        /// @brief get vector with TAZs
        const std::vector<GNETAZ*>& getTAZs() const;

        /// @brief get vector with additionals
        const std::vector<GNEAdditional*>& getAdditionals() const;

        /// @brief get vector with Demand Elements
        const std::vector<GNEDemandElement*>& getDemandElements() const;

        /// @brief get merging junctions
        const std::vector<const GNEJunction*>& getMergingJunctions() const;

    protected:
        /// @brief objects container
        class ViewObjectsContainer {

        public:
            /// @brief constructor
            ViewObjectsContainer();

            /// @brief clear elements
            void clearElements();

            /// @brief reseve memory for all elements
            void reserve(int size);

            /// @brief filter elements
            void filterElements(const std::vector<const GUIGlObject*>& objects);

            /// @brief vector with the GUIGlObjects
            std::vector<GUIGlObject*> GUIGlObjects;

            /// @brief vector with the attribute carriers
            std::vector<GNEAttributeCarrier*> attributeCarriers;

            /// @brief vector with the network elements
            std::vector<GNENetworkElement*> networkElements;

            /// @brief vector with the additional elements
            std::vector<GNEAdditional*> additionals;

            /// @brief vector with the demand elements
            std::vector<GNEDemandElement*> demandElements;

            /// @brief vector with the generic datas
            std::vector<GNEGenericData*> genericDatas;

            /// @brief vector with the junctions
            std::vector<GNEJunction*> junctions;

            /// @brief vector with the  edges
            std::vector<GNEEdge*> edges;

            /// @brief vector with the lanes
            std::vector<GNELane*> lanes;

            /// @brief vector with the crossings
            std::vector<GNECrossing*> crossings;

            /// @brief vector with the walkingAreas
            std::vector<GNEWalkingArea*> walkingAreas;

            /// @brief vector with the connections
            std::vector<GNEConnection*> connections;

            /// @brief vector with the internal lanes
            std::vector<GNEInternalLane*> internalLanes;

            /// @brief vector with the TAZ elements
            std::vector<GNETAZ*> TAZs;

            /// @brief vector with the POIs
            std::vector<GNEPOI*> POIs;

            /// @brief vector with the polys
            std::vector<GNEPoly*> polys;

            /// @brief vector with the edge datas
            std::vector<GNEEdgeData*> edgeDatas;

            /// @brief vector with the edge relation datas
            std::vector<GNEEdgeRelData*> edgeRelDatas;

            /// @brief vector with the TAZ relation datas
            std::vector<GNETAZRelData*> TAZRelDatas;

        private:
            /// @brief Invalidated copy constructor.
            ViewObjectsContainer(const ViewObjectsContainer&) = delete;

            /// @brief Invalidated assignment operator.
            ViewObjectsContainer& operator=(const ViewObjectsContainer&) = delete;
        };

        /// @brief pointer to viewNet
        const GNEViewNet* myViewNet;

        /// @brief objects container with selected objects
        ViewObjectsContainer myViewObjects;

        /// @brief merging junctions
        std::vector<const GNEJunction*> myMergingJunctions;

    private:
        /// @brief update network elements
        void updateNetworkElements(ViewObjectsContainer& container, const GUIGlObject* glObject);

        /// @brief update additional elements
        void updateAdditionalElements(ViewObjectsContainer& container, const GUIGlObject* glObject);

        /// @brief update shape elements
        void updateShapeElements(ViewObjectsContainer& container, const GUIGlObject* glObject);

        /// @brief update TAZ elements
        void updateTAZElements(ViewObjectsContainer& container, const GUIGlObject* glObject);

        /// @brief update demand elements
        void updateDemandElements(ViewObjectsContainer& container, const GUIGlObject* glObject);

        /// @brief update generic data elements
        void updateGenericDataElements(ViewObjectsContainer& container, const GUIGlObject* glObject);

        /// @brief process GL objects
        void processGUIGlObjects(const GUIViewObjectsHandler::GLObjectsSortedContainer& objectsContainer);

        /// @brief default constructor
        ViewObjectsSelector();

        /// @brief Invalidated copy constructor.
        ViewObjectsSelector(const ViewObjectsSelector&) = delete;

        /// @brief Invalidated assignment operator.
        ViewObjectsSelector& operator=(const ViewObjectsSelector&) = delete;
    };

    /// @brief class used to group all variables related with mouse buttons and key pressed after certain events
    struct MouseButtonKeyPressed {

        /// @brief constructor
        MouseButtonKeyPressed();

        /// @brief update status of MouseButtonKeyPressed during current event
        void update(void* eventData);

        /// @brief check if SHIFT is pressed during current event
        bool shiftKeyPressed() const;

        /// @brief check if CONTROL is pressed during current event
        bool controlKeyPressed() const;

        /// @brief check if ALT is pressed during current event
        bool altKeyPressed() const;

        /// @brief check if mouse left button is pressed during current event
        bool mouseLeftButtonPressed() const;

        /// @brief check if mouse right button is pressed during current event
        bool mouseRightButtonPressed() const;

    private:
        /// @brief information of event (must be updated)
        FXEvent* myEventInfo;

        /// @brief Invalidated copy constructor.
        MouseButtonKeyPressed(const MouseButtonKeyPressed&) = delete;

        /// @brief Invalidated assignment operator.
        MouseButtonKeyPressed& operator=(const MouseButtonKeyPressed&) = delete;
    };

    /// @brief struct used to group all variables related with save elements
    struct SaveElements {

        /// @brief default constructor
        SaveElements(GNEViewNet* viewNet);

        /// @brief destructor
        ~SaveElements();

        /// @brief build save buttons
        void buildSaveElementsButtons();

        /// @brief enable or disable save individual files
        void setSaveIndividualFiles(bool value);

    private:
        /// @brief pointer to net
        GNEViewNet* myViewNet;

        /// The locator menu
        FXPopup* mySaveIndividualFilesPopup = nullptr;

        /// @brief checkable button for save individual files
        MFXMenuButtonTooltip* mySaveIndividualFiles = nullptr;

        /// @brief checkable button for save netedit config
        MFXButtonTooltip* mySaveNeteditConfig = nullptr;

        /// @brief checkable button for save SUMO config
        MFXButtonTooltip* mySaveSumoConfig = nullptr;

        /// @brief checkable button for save network
        MFXButtonTooltip* mySaveNetwork = nullptr;

        /// @brief checkable button for save additional elements
        MFXButtonTooltip* mySaveAdditionalElements = nullptr;

        /// @brief checkable button for save demand elements
        MFXButtonTooltip* mySaveDemandElements = nullptr;

        /// @brief checkable button for save genericdata elements
        MFXButtonTooltip* mySaveDataElements = nullptr;

        /// @brief checkable button for save meanData elements
        MFXButtonTooltip* mySaveMeanDataElements = nullptr;

        /// @brief Invalidated copy constructor.
        SaveElements(const SaveElements&) = delete;

        /// @brief Invalidated assignment operator.
        SaveElements& operator=(const SaveElements&) = delete;
    };

    /// @brief struct used to group all variables related with time format
    struct TimeFormat {

        /// @brief default constructor
        TimeFormat(GNEViewNet* viewNet);

        /// @brief build time format buttons
        void buildTimeFormatButtons();

        /// @brief switch time format
        void switchTimeFormat();

        /// @brief update button label
        void updateButtonLabel();

    private:
        /// @brief pointer to net
        GNEViewNet* myViewNet;

        /// @brief checkable button for switch between timeSteps and HH:MM:SS
        MFXButtonTooltip* mySwitchButton = nullptr;

        /// @brief Invalidated copy constructor.
        TimeFormat(const TimeFormat&) = delete;

        /// @brief Invalidated assignment operator.
        TimeFormat& operator=(const TimeFormat&) = delete;
    };

    /// @brief struct used to group all variables related with Supermodes
    struct EditModes {

        /// @brief constructor
        EditModes(GNEViewNet* viewNet);

        /// @brief destructor
        ~EditModes();

        /// @brief build checkable buttons
        void buildSuperModeButtons();

        /// @brief set supermode
        void setSupermode(Supermode supermode, const bool force);

        /// @brief set Network edit mode
        void setNetworkEditMode(NetworkEditMode networkMode, const bool force = false);

        /// @brief set Demand edit mode
        void setDemandEditMode(DemandEditMode demandMode, const bool force = false);

        /// @brief set Data edit mode
        void setDataEditMode(DataEditMode dataMode, const bool force = false);

        /// @check if current supermode is Network
        bool isCurrentSupermodeNetwork() const;

        /// @check if current supermode is Demand
        bool isCurrentSupermodeDemand() const;

        /// @check if current supermode is Data
        bool isCurrentSupermodeData() const;

        /// @brief set view
        void setView(FXSelector sel);

        /// @brief check if default view is enabled
        bool isDefaultView() const;

        /// @brief check if default view is enabled
        bool isJuPedSimView() const;

        /// @brief the current supermode
        Supermode currentSupermode;

        /// @brief the current Network edit mode
        NetworkEditMode networkEditMode;

        /// @brief the current Demand edit mode
        DemandEditMode demandEditMode;

        /// @brief the current Data edit mode
        DataEditMode dataEditMode;

        /// @brief checkable button for supermode Network
        MFXCheckableButton* networkButton = nullptr;

        /// @brief checkable button for supermode Demand
        MFXCheckableButton* demandButton = nullptr;

        /// @brief checkable button for supermode Data
        MFXCheckableButton* dataButton = nullptr;

    private:
        /// @brief pointer to net
        GNEViewNet* myViewNet;

        /// @brief The netedit views menu
        FXPopup* myNeteditViewsPopup = nullptr;

        /// @brief The netedit views button
        MFXMenuButtonTooltip* myNeteditViewsButton = nullptr;

        /// @brief The default view button
        MFXButtonTooltip* myDefaultViewButton = nullptr;

        /// @brief The jupedsim view button
        MFXButtonTooltip* myJuPedSimViewButton = nullptr;

        /// @brief Invalidated copy constructor.
        EditModes(const EditModes&) = delete;

        /// @brief Invalidated assignment operator.
        EditModes& operator=(const EditModes&) = delete;
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
        void getVisibleNetworkMenuCommands(std::vector<MFXCheckableButton*>& commands) const;

        /// @brief check if vehicles must be drawn spread
        bool drawSpreadVehicles() const;

        /// @brief check if show demand elements checkbox is enabled
        bool showDemandElements() const;

        /// @brief check if select edges checkbox is enabled
        bool selectEdges() const;

        /// @brief check if select show connections checkbox is enabled
        bool showConnections() const;

        /// @brief check if show sub-additionals
        bool showSubAdditionals() const;

        /// @brief check if show TAZ Elements
        bool showTAZElements() const;

        /// @brief check if we're editing elevation
        bool editingElevation() const;

        /// @brief checkable button to show grid button
        MFXCheckableButton* menuCheckToggleGrid = nullptr;

        /// @brief checkable button to show junction shapes
        MFXCheckableButton* menuCheckToggleDrawJunctionShape = nullptr;

        /// @brief checkable button to draw vehicles in begin position or spread in lane
        MFXCheckableButton* menuCheckDrawSpreadVehicles = nullptr;

        /// @brief checkable button to show Demand Elements
        MFXCheckableButton* menuCheckShowDemandElements = nullptr;

        /// @brief checkable button to select only edges
        MFXCheckableButton* menuCheckSelectEdges = nullptr;

        /// @brief checkable button to show connections
        MFXCheckableButton* menuCheckShowConnections = nullptr;

        /// @brief checkable button to hide connections in connect mode
        MFXCheckableButton* menuCheckHideConnections = nullptr;

        /// @brief checkable button to show additional sub-elements
        MFXCheckableButton* menuCheckShowAdditionalSubElements = nullptr;

        /// @brief checkable button to show TAZ elements
        MFXCheckableButton* menuCheckShowTAZElements = nullptr;

        /// @brief checkable button to extend to edge nodes
        MFXCheckableButton* menuCheckExtendSelection = nullptr;

        /// @brief checkable button to set change all phases
        MFXCheckableButton* menuCheckChangeAllPhases = nullptr;

        /// @brief checkable button to we should't warn about merging junctions
        MFXCheckableButton* menuCheckMergeAutomatically = nullptr;

        /// @brief checkable button to show connection as bubble in "Move" mode.
        MFXCheckableButton* menuCheckShowJunctionBubble = nullptr;

        /// @brief checkable button to apply movement to elevation
        MFXCheckableButton* menuCheckMoveElevation = nullptr;

        /// @brief checkable button to the endpoint for a created edge should be set as the new source
        MFXCheckableButton* menuCheckChainEdges = nullptr;

        /// @brief check checkable to create auto create opposite edge
        MFXCheckableButton* menuCheckAutoOppositeEdge = nullptr;

    private:
        /// @brief pointer to net
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
        void getVisibleDemandMenuCommands(std::vector<MFXCheckableButton*>& commands) const;

        /// @brief check if vehicles must be drawn spread
        bool drawSpreadVehicles() const;

        /// @brief check if non inspected element has to be hidden
        bool showNonInspectedDemandElements(const GNEDemandElement* demandElement) const;

        /// @brief check if shapes has to be drawn
        bool showShapes() const;

        /// @brief check if trips has to be drawn
        bool showAllTrips() const;

        /// @brief check all person plans has to be show
        bool showAllPersonPlans() const;

        /// @brief lock person
        void lockPerson(const GNEDemandElement* person);

        /// @brief unlock person
        void unlockPerson();

        /// @brief get locked person
        const GNEDemandElement* getLockedPerson() const;

        /// @brief check all container plans has to be show
        bool showAllContainerPlans() const;

        /// @brief lock container
        void lockContainer(const GNEDemandElement* container);

        /// @brief unlock container
        void unlockContainer();

        /// @brief show overlapped routes
        bool showOverlappedRoutes() const;

        /// @brief get locked container
        const GNEDemandElement* getLockedContainer() const;

        /// @brief menu check to show grid button
        MFXCheckableButton* menuCheckToggleGrid = nullptr;

        /// @brief checkable button to show junction shapes
        MFXCheckableButton* menuCheckToggleDrawJunctionShape = nullptr;

        /// @brief menu check to draw vehicles in begin position or spread in lane
        MFXCheckableButton* menuCheckDrawSpreadVehicles = nullptr;

        /// @brief Hide shapes (Polygons and POIs)
        MFXCheckableButton* menuCheckHideShapes = nullptr;

        /// @brief show all trips
        MFXCheckableButton* menuCheckShowAllTrips = nullptr;

        /// @brief show all person plans
        MFXCheckableButton* menuCheckShowAllPersonPlans = nullptr;

        /// @brief Lock Person
        MFXCheckableButton* menuCheckLockPerson = nullptr;

        /// @brief show all container plans
        MFXCheckableButton* menuCheckShowAllContainerPlans = nullptr;

        /// @brief Lock Container
        MFXCheckableButton* menuCheckLockContainer = nullptr;

        /// @brief Hide non inspected demand elements
        MFXCheckableButton* menuCheckHideNonInspectedDemandElements = nullptr;

        /// @brief show overlapped routes
        MFXCheckableButton* menuCheckShowOverlappedRoutes = nullptr;

    private:
        /// @brief pointer to net
        GNEViewNet* myViewNet;

        /// @brief pointer to locked person
        const GNEDemandElement* myLockedPerson = nullptr;

        /// @brief pointer to locked container
        const GNEDemandElement* myLockedContainer = nullptr;

        /// @brief Invalidated copy constructor.
        DemandViewOptions(const DemandViewOptions&) = delete;

        /// @brief Invalidated assignment operator.
        DemandViewOptions& operator=(const DemandViewOptions&) = delete;
    };

    /// @brief struct used to group all variables related to view options in supermode Data
    struct DataViewOptions {

        /// @brief default constructor
        DataViewOptions(GNEViewNet* viewNet);

        /// @brief build menu checks
        void buildDataViewOptionsMenuChecks();

        /// @brief hide all options menu checks
        void hideDataViewOptionsMenuChecks();

        /// @brief get visible demand menu commands
        void getVisibleDataMenuCommands(std::vector<MFXCheckableButton*>& commands) const;

        /// @brief check if additionals has to be drawn
        bool showAdditionals() const;

        /// @brief check if shapes has to be drawn
        bool showShapes() const;

        /// @brief check if show demand elements checkbox is enabled
        bool showDemandElements() const;

        /// @brief check if toggle TAZRel drawing checkbox is enabled
        bool TAZRelDrawing() const;

        /// @brief check if toggle TAZ draw fill checkbox is enabled
        bool TAZDrawFill() const;

        /// @brief check if toggle TAZRel only from checkbox is enabled
        bool TAZRelOnlyFrom() const;

        /// @brief check if toggle TAZRel only to checkbox is enabled
        bool TAZRelOnlyTo() const;

        /// @brief checkable button to show junction shapes
        MFXCheckableButton* menuCheckToggleDrawJunctionShape = nullptr;

        /// @brief menu check to show Additionals
        MFXCheckableButton* menuCheckShowAdditionals = nullptr;

        /// @brief menu check to show Shapes
        MFXCheckableButton* menuCheckShowShapes = nullptr;

        /// @brief menu check to show Demand Elements
        MFXCheckableButton* menuCheckShowDemandElements = nullptr;

        /// @brief menu check to toggle TAZ Rel drawing
        MFXCheckableButton* menuCheckToggleTAZRelDrawing = nullptr;

        /// @brief menu check to toggle TAZ draw fill
        MFXCheckableButton* menuCheckToggleTAZDrawFill = nullptr;

        /// @brief menu check to toggle TAZRel only from
        MFXCheckableButton* menuCheckToggleTAZRelOnlyFrom = nullptr;

        /// @brief menu check to toggle TAZRel only to
        MFXCheckableButton* menuCheckToggleTAZRelOnlyTo = nullptr;

    private:
        /// @brief pointer to net
        GNEViewNet* myViewNet;

        /// @brief Invalidated copy constructor.
        DataViewOptions(const DataViewOptions&) = delete;

        /// @brief Invalidated assignment operator.
        DataViewOptions& operator=(const DataViewOptions&) = delete;
    };

    /// @brief class used to group all variables related to interval bar
    class IntervalBar {

    public:
        /// @brief default constructor
        IntervalBar(GNEViewNet* viewNet);

        /// @brief build interval bar elements
        void buildIntervalBarElements();

        /// @brief show interval option bar
        void showIntervalBar();

        /// @brief hide all options menu checks
        void hideIntervalBar();

        /// @brief update interval bar
        void updateIntervalBar();

        // @brief mark for update
        void markForUpdate();

        /// @name get functions (called by GNEViewNet)
        /// @{

        /// @brief get generic data type
        SumoXMLTag getGenericDataType() const;

        /// @brief get dataSet
        GNEDataSet* getDataSet() const;

        /// @brief get begin
        double getBegin() const;

        /// @brief get end
        double getEnd() const;

        /// @brief get parameter
        std::string getParameter() const;

        /// @}

        /// @name set functions (called by GNEViewNet)
        /// @{

        /// @brief set generic data type
        void setGenericDataType();

        /// @brief set dataSet
        void setDataSet();

        /// @brief update limit by interval
        void setInterval();

        /// @brief set begin
        void setBegin();

        /// @brief set end
        void setEnd();

        /// @brief set parameter
        void setParameter();

        /// @}

    protected:
        /// @brief enable interval bar
        void enableIntervalBar();

        /// @brief disable interval bar
        void disableIntervalBar();

    private:
        /// @brief pointer to net
        GNEViewNet* myViewNet;

        /// @brief flag for update interval bar
        bool myUpdateInterval = true;

        /// @brief combo box for generic data types
        MFXComboBoxIcon* myGenericDataTypesComboBox = nullptr;

        /// @brief combo box for data sets
        MFXComboBoxIcon* myDataSetsComboBox = nullptr;

        /// @brief checkbox for limit data elements by interval
        FXCheckButton* myIntervalCheckBox = nullptr;

        /// @brief text field for interval begin
        FXTextField* myBeginTextField = nullptr;

        /// @brief text field for interval end
        FXTextField* myEndTextField = nullptr;

        /// @brief combo box for filtered parameters
        MFXComboBoxIcon* myParametersComboBox = nullptr;

        /// @brief current dataSets
        std::vector<std::string> myDataSets;

        /// @brief current parameters
        std::set<std::string> myParameters;

        /// @brief Invalidated copy constructor.
        IntervalBar(const IntervalBar&) = delete;

        /// @brief Invalidated assignment operator.
        IntervalBar& operator=(const IntervalBar&) = delete;
    };

    /// @brief struct used to group all variables related with movement of single elements
    struct MoveSingleElementModul {

        /// @brief constructor
        MoveSingleElementModul(GNEViewNet* viewNet);

        /// @brief begin move network elementshape
        bool beginMoveNetworkElementShape();

        /// @brief begin move single element in Network mode
        bool beginMoveSingleElementNetworkMode();

        /// @brief begin move single element in Demand mode
        bool beginMoveSingleElementDemandMode();

        /// @brief move single element in Network AND Demand mode
        void moveSingleElement(const bool mouseLeftButtonPressed);

        /// @brief finish moving single elements in Network AND Demand mode
        void finishMoveSingleElement();

        /// @brief check if there are moving elements
        bool isCurrentlyMovingSingleElement() const;

        /// @brief get moved element
        GNEMoveElement* getMovedElement() const;

    protected:
        /// @brief calculate offset
        const GNEMoveOffset calculateMoveOffset() const;

    private:
        /// @brief pointer to net
        GNEViewNet* myViewNet;

        /// @brief relative position of Clicked Position regarding to originalGeometryPointPosition (Used when user doesn't click exactly over the center of element)
        Position myRelativeClickedPosition;

        /// @brief move operations
        GNEMoveOperation* myMoveOperation = nullptr;

        /// @brief Invalidated copy constructor.
        MoveSingleElementModul(const MoveSingleElementModul&) = delete;

        /// @brief Invalidated assignment operator.
        MoveSingleElementModul& operator=(const MoveSingleElementModul&) = delete;
    };

    /// @brief struct used to group all variables related with movement of groups of elements
    struct MoveMultipleElementModul {

        /// @brief constructor
        MoveMultipleElementModul(GNEViewNet* viewNet);

        /// @brief begin move selection
        void beginMoveSelection();

        /// @brief move selection
        void moveSelection(const bool mouseLeftButtonPressed);

        /// @brief finish moving selection
        void finishMoveSelection();

        /// @brief check if currently there is element being moved
        bool isMovingSelection() const;

        /// @brief flag for moving edge
        bool isMovingSelectedEdge() const;

        /// @brief reset flag for moving edge
        void resetMovingSelectedEdge();

        /// @brief edge offset
        double getEdgeOffset() const;

        /// @brief check if there are moving elements
        bool isCurrentlyMovingMultipleElements() const;

    protected:
        /// @brief calculate move offset
        const GNEMoveOffset calculateMoveOffset() const;

        /// @brief calculate junction selection
        void calculateJunctionSelection();

        /// @brief calculate edge selection
        void calculateEdgeSelection(const GNEEdge* clickedEdge);

    private:
        /// @brief pointer to net
        GNEViewNet* myViewNet;

        /// @brief original clicked position when moveSelection is called (used for calculate offset during moveSelection())
        Position myClickedPosition;

        /// @brief flag for enable moving edge
        bool myMovingSelectedEdge;

        /// @brief offset of moved edge
        double myEdgeOffset;

        /// @brief move operations
        std::vector<GNEMoveOperation*> myMoveOperations;

        /// @brief Invalidated copy constructor.
        MoveMultipleElementModul(const MoveMultipleElementModul&) = delete;

        /// @brief Invalidated assignment operator.
        MoveMultipleElementModul& operator=(const MoveMultipleElementModul&) = delete;
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
        /// @brief pointer to net
        GNEViewNet* myViewNet;

        /// @brief Invalidated copy constructor.
        VehicleOptions(const VehicleOptions&) = delete;

        /// @brief Invalidated assignment operator.
        VehicleOptions& operator=(const VehicleOptions&) = delete;
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
        /// @brief pointer to net
        GNEViewNet* myViewNet;

        /// @brief Invalidated copy constructor.
        VehicleTypeOptions(const VehicleTypeOptions&) = delete;

        /// @brief Invalidated assignment operator.
        VehicleTypeOptions& operator=(const VehicleTypeOptions&) = delete;
    };

    /// @brief struct used to group all variables related with selecting using a square or polygon
    /// @note in the future the variables used for selecting through a polygon will be placed here
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

        /// @brief draw rectangle selection
        void drawRectangleSelection(const RGBColor& color) const;

        /// @brief whether we have started rectangle-selection
        bool selectingUsingRectangle;

        /// @brief whether we have started rectangle-selection
        bool startDrawing;

    private:
        /// @brief Process boundary Selection
        void processBoundarySelection(const Boundary& boundary);

        /// @brief first corner of the rectangle-selection
        Position selectionCorner1;

        /// @brief second corner of the rectangle-selection
        Position selectionCorner2;

        /// @brief pointer to net
        GNEViewNet* myViewNet;

        /// @brief Invalidated copy constructor.
        SelectingArea(const SelectingArea&) = delete;

        /// @brief Invalidated assignment operator.
        SelectingArea& operator=(const SelectingArea&) = delete;
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
        /// @brief pointer to net
        GNEViewNet* myViewNet;

        /// @brief Width of net in testing mode
        int myTestingWidth = 0;

        /// @brief Height of net in testing mode
        int myTestingHeight = 0;

        /// @brief Invalidated copy constructor.
        TestingMode(const TestingMode&) = delete;

        /// @brief Invalidated assignment operator.
        TestingMode& operator=(const TestingMode&) = delete;
    };

    /// @brief struct used to group all variables related with common checkable Buttons
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

        /// @brief checkable button for edit mode inspect
        MFXCheckableButton* inspectButton = nullptr;

        /// @brief checkable button for edit mode delete
        MFXCheckableButton* deleteButton = nullptr;

        /// @brief checkable button for edit mode select
        MFXCheckableButton* selectButton = nullptr;

    private:
        /// @brief pointer to net
        GNEViewNet* myViewNet;

        /// @brief Invalidated copy constructor.
        CommonCheckableButtons(const CommonCheckableButtons&) = delete;

        /// @brief Invalidated assignment operator.
        CommonCheckableButtons& operator=(const CommonCheckableButtons&) = delete;
    };

    /// @brief struct used to group all variables related with Network checkable Buttons
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

        /// @brief checkable button for edit mode "move network elements"
        MFXCheckableButton* moveNetworkElementsButton = nullptr;

        /// @brief checkable button for edit mode create edge
        MFXCheckableButton* createEdgeButton = nullptr;

        /// @brief checkable button for edit mode connection
        MFXCheckableButton* connectionButton = nullptr;

        /// @brief checkable button for edit mode traffic light
        MFXCheckableButton* trafficLightButton = nullptr;

        /// @brief checkable button for edit mode additional
        MFXCheckableButton* additionalButton = nullptr;

        /// @brief checkable button for edit mode crossing
        MFXCheckableButton* crossingButton = nullptr;

        /// @brief checkable button for edit mode TAZ
        MFXCheckableButton* TAZButton = nullptr;

        /// @brief checkable button for edit mode shape
        MFXCheckableButton* shapeButton = nullptr;

        /// @brief checkable button for edit mode prohibition
        MFXCheckableButton* prohibitionButton = nullptr;

        /// @brief checkable button for edit mode wires
        MFXCheckableButton* wireButton = nullptr;

        /// @brief checkable button for edit mode decals
        MFXCheckableButton* decalButton = nullptr;

    private:
        /// @brief pointer to net
        GNEViewNet* myViewNet;

        /// @brief Invalidated copy constructor.
        NetworkCheckableButtons(const NetworkCheckableButtons&) = delete;

        /// @brief Invalidated assignment operator.
        NetworkCheckableButtons& operator=(const NetworkCheckableButtons&) = delete;
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

        /// @brief checkable button for edit mode "move demand elements"
        MFXCheckableButton* moveDemandElementsButton = nullptr;

        /// @brief checkable button for edit mode create routes
        MFXCheckableButton* routeButton = nullptr;

        /// @brief checkable button for edit mode create route distributions
        MFXCheckableButton* routeDistributionButton = nullptr;

        /// @brief checkable button for edit mode create vehicles
        MFXCheckableButton* vehicleButton = nullptr;

        /// @brief checkable button for edit mode create type
        MFXCheckableButton* typeButton = nullptr;

        /// @brief checkable button for edit mode create type distribution
        MFXCheckableButton* typeDistributionButton = nullptr;

        /// @brief checkable button for edit mode create stops
        MFXCheckableButton* stopButton = nullptr;

        /// @brief checkable button for edit mode create persons
        MFXCheckableButton* personButton = nullptr;

        /// @brief checkable button for edit mode create person plans
        MFXCheckableButton* personPlanButton = nullptr;

        /// @brief checkable button for edit mode create containers
        MFXCheckableButton* containerButton = nullptr;

        /// @brief checkable button for edit mode create container plans
        MFXCheckableButton* containerPlanButton = nullptr;

    private:
        /// @brief pointer to net
        GNEViewNet* myViewNet;

        /// @brief Invalidated copy constructor.
        DemandCheckableButtons(const DemandCheckableButtons&) = delete;

        /// @brief Invalidated assignment operator.
        DemandCheckableButtons& operator=(const DemandCheckableButtons&) = delete;
    };

    /// @brief struct used to group all variables related with Data checkable Buttons
    struct DataCheckableButtons {

        /// @brief default constructor
        DataCheckableButtons(GNEViewNet* viewNet);

        /// @brief build checkable buttons
        void buildDataCheckableButtons();

        /// @brief show all Data Checkable Buttons
        void showDataCheckableButtons();

        /// @brief hide all Data Checkable Buttons
        void hideDataCheckableButtons();

        /// @brief hide all options menu checks
        void disableDataCheckableButtons();

        /// @brief update Data checkable buttons
        void updateDataCheckableButtons();

        /// @brief checkable button for edit mode "edgeData"
        MFXCheckableButton* edgeDataButton = nullptr;

        /// @brief checkable button for edit mode "edgeRelData"
        MFXCheckableButton* edgeRelDataButton = nullptr;

        /// @brief checkable button for edit mode "TAZRelData"
        MFXCheckableButton* TAZRelDataButton = nullptr;

        /// @brief checkable button for edit mode "meanData"
        MFXCheckableButton* meanDataButton = nullptr;

    private:
        /// @brief pointer to net
        GNEViewNet* myViewNet;

        /// @brief Invalidated copy constructor.
        DataCheckableButtons(const DataCheckableButtons&) = delete;

        /// @brief Invalidated assignment operator.
        DataCheckableButtons& operator=(const DataCheckableButtons&) = delete;
    };

    /// @brief struct used to group all variables related with edit shapes of NetworkElements
    struct EditNetworkElementShapes {

        /// @brief default constructor
        EditNetworkElementShapes(GNEViewNet* viewNet);

        /// @brief start edit custom shape
        void startEditCustomShape(GNENetworkElement* element);

        /// @brief edit edit shape
        void stopEditCustomShape();

        /// @brief save edited shape
        void commitShapeEdited();

        /// @brief pointer to edited network element
        GNENetworkElement* getEditedNetworkElement() const;

    private:
        /// @brief pointer to viewNet
        GNEViewNet* myViewNet;

        /// @brief pointer to edited network element
        GNENetworkElement* myEditedNetworkElement = nullptr;

        /// @brief the previous edit mode before edit NetworkElement's shapes
        NetworkEditMode myPreviousNetworkEditMode;

        /// @brief Invalidated copy constructor.
        EditNetworkElementShapes(const EditNetworkElementShapes&) = delete;

        /// @brief Invalidated assignment operator.
        EditNetworkElementShapes& operator=(const EditNetworkElementShapes&) = delete;
    };

    /// @brief struct for pack all variables and functions related with Block Icon
    struct LockIcon {
        /// @brief draw lock icon
        static void drawLockIcon(const GUIVisualizationSettings::Detail d, const GNEAttributeCarrier* AC, GUIGlObjectType type, const Position position,
                                 const double exaggeration, const double size = 0.5,
                                 const double offsetx = 0, const double offsety = 0);

        /// @brief check if icon can be drawn
        static bool checkDrawing(const GUIVisualizationSettings::Detail d, const GNEAttributeCarrier* AC,
                                 GUIGlObjectType type, const double exaggeration);
    private:
        /// @brief constructor
        LockIcon();

        /// @brief Invalidated copy constructor.
        LockIcon(const LockIcon&) = delete;

        /// @brief Invalidated assignment operator.
        LockIcon& operator=(const LockIcon&) = delete;
    };

    /// @brief get scaled rainbow colors
    static const std::vector<RGBColor>& getRainbowScaledColors();

    /// @brief get rainbow scaled color
    static const RGBColor& getRainbowScaledColor(const double min, const double max, const double value);

    /// @brief filter elements based on the layer
    static std::vector<GUIGlObject*> filterElementsByLayer(const std::vector<GUIGlObject*>& GLObjects);

private:
    /// @brief scale (rainbow) colors
    static std::vector<RGBColor> myRainbowScaledColors;
};
