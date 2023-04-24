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
/// @file    GNEApplicationWindowHelper.h
/// @author  Pablo Alvarez Lopez
/// @date    mar 2020
///
// Functions from main window of netedit
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/common/SUMOTime.h>
#include <utils/foxtools/MFXSynchQue.h>
#include <utils/foxtools/MFXRecentNetworks.h>
#include <utils/foxtools/MFXThreadEvent.h>
#include <utils/foxtools/MFXInterThreadEventClient.h>
#include <utils/geom/Position.h>
#include <utils/gui/div/GUIMessageWindow.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/shapes/ShapeHandler.h>
#include <utils/options/OptionsCont.h>

#include "GNEViewNetHelper.h"


// ===========================================================================
// class declarations
// ===========================================================================

class GNEApplicationWindow;
class GNELoadThread;
class GNENet;
class GNEPythonTool;
class GNEPythonToolDialog;
class GNENetgenerateDialog;
class GNERunPythonToolDialog;
class GNERunNetgenerateDialog;
class GNEUndoList;
class GNEUndoListDialog;
class GNEViewNet;
class GUIEvent;
class MFXMenuCheckIcon;

// ===========================================================================
// class definition
// ===========================================================================

struct GNEApplicationWindowHelper {

    /// @brief struct for griped toolbars
    struct ToolbarsGrip {

        /// @brief constructor
        ToolbarsGrip(GNEApplicationWindow* GNEApp);

        /// @brief build menu toolbar grips
        void buildMenuToolbarsGrip();

        /// @brief build toolbars grips
        void buildViewParentToolbarsGrips();

        /// @brief build toolbars grips
        void destroyParentToolbarsGrips();

        /// @brief The application menu bar (for file, edit, processing...)
        FXMenuBar* menu = nullptr;

        /// @brief The application menu bar for supermodes (network, demand and data)
        FXMenuBar* superModes = nullptr;

        /// @brief The application menu bar for save elements
        FXMenuBar* saveElements = nullptr;

        /// @brief The application menu bar for navigation (zoom, coloring...)
        FXMenuBar* navigation = nullptr;

        /// @brief The application menu bar (for select, inspect...)
        FXMenuBar* modes = nullptr;

        /// @brief The application menu bar for mode options (show connections, select edges...)
        FXMenuBar* intervalBar = nullptr;

    private:
        /// @brief pointer to current GNEApplicationWindow
        GNEApplicationWindow* myGNEApp;

        /// @brief menu bar drag (for file, edit, processing...)
        FXToolBarShell* myPythonToolBarShellMenu = nullptr;

        /// @brief menu bar drag for super modes (network, demand and data)
        FXToolBarShell* myPythonToolBarShellSuperModes = nullptr;

        /// @brief menu bar drag for save elements
        FXToolBarShell* myPythonToolBarShellSaveElements = nullptr;

        /// @brief menu bar drag for navigation (Zoom, coloring...)
        FXToolBarShell* myPythonToolBarShellNavigation = nullptr;

        /// @brief menu bar drag for modes (select, inspect, delete...)
        FXToolBarShell* myPythonToolBarShellModes = nullptr;

        /// @brief menu bar drag for interval bar
        FXToolBarShell* myPythonToolBarShellIntervalBar = nullptr;

        /// @brief Invalidated copy constructor.
        ToolbarsGrip(const ToolbarsGrip&) = delete;

        /// @brief Invalidated assignment operator.
        ToolbarsGrip& operator=(const ToolbarsGrip&) = delete;
    };

    /// @brief struct for menu bar file
    struct MenuBarFile {

        /// @brief constructor
        MenuBarFile(GNEApplicationWindow* GNEApp);

        /// @brief build recent network
        void buildRecentNetworkFiles(FXMenuPane* fileMenu, FXMenuPane* fileMenuRecentNetworkFiles);

        /// @brief build recent config
        void buildRecentConfigFiles(FXMenuPane* fileMenu, FXMenuPane* fileMenuRecentConfigFiles);

        /// @brief List of recent networks
        MFXRecentNetworks myRecentNetworks;

        /// @brief List of recent configs
        MFXRecentNetworks myRecentConfigs;

    private:
        /// @brief pointer to current GNEApplicationWindow
        GNEApplicationWindow* myGNEApp;

        /// @brief Invalidated copy constructor.
        MenuBarFile(const MenuBarFile&) = delete;

        /// @brief Invalidated assignment operator.
        MenuBarFile& operator=(const MenuBarFile&) = delete;
    };

    /// @brief struct for File menu commands
    struct FileMenuCommands {

        /// @brief constructor
        FileMenuCommands(GNEApplicationWindow* GNEApp);

        /// @brief build menu commands
        void buildFileMenuCommands(FXMenuPane* fileMenu, FXMenuPane* fileMenuNEEDITConfig, FXMenuPane* fileMenuSumoConfig,
                                   FXMenuPane* fileMenuTLS, FXMenuPane* fileMenuEdgeTypes, FXMenuPane* fileMenuAdditionals,
                                   FXMenuPane* fileMenuDemandElements, FXMenuPane* fileMenuDataElements,
                                   FXMenuPane* fileMenuMeanDataElements);

        /// @brief enable menu cascades
        void enableMenuCascades();

        /// @brief disable menu cascades
        void disableMenuCascades();

    private:
        /// @brief build netedit config section
        void buildNeteditConfigSection(FXMenuPane* menuPane);

        /// @brief build SUMO Config section
        void buildSumoConfigSection(FXMenuPane* menuPane);

        /// @brief build traffic light section
        void buildTrafficLightSection(FXMenuPane* menuPane);

        /// @brief build edge type section
        void buildEdgeTypeSection(FXMenuPane* menuPane);

        /// @brief build additional section
        void buildAdditionalSection(FXMenuPane* menuPane);

        /// @brief build demand section
        void buildDemandSection(FXMenuPane* menuPane);

        /// @brief build data section
        void buildDataSection(FXMenuPane* menuPane);

        /// @brief build meanData section
        void buildMeanDataSection(FXMenuPane* menuPane);

        /// @brief pointer to current GNEApplicationWindows
        GNEApplicationWindow* myGNEApp = nullptr;

        /// @brief FXMenuCascade for neteditConfig
        FXMenuCascade* myNeteditConfigMenuCascade = nullptr;

        /// @brief FXMenuCascade for SumoConfig
        FXMenuCascade* mySumoConfigMenuCascade = nullptr;

        /// @brief FXMenuCascade for TLS
        FXMenuCascade* myTLSMenuCascade = nullptr;

        /// @brief FXMenuCascade for edgeTypes
        FXMenuCascade* myEdgeTypesMenuCascade = nullptr;

        /// @brief FXMenuCascade for additional
        FXMenuCascade* myAdditionalMenuCascade = nullptr;

        /// @brief FXMenuCascade for demand
        FXMenuCascade* myDemandMenuCascade = nullptr;

        /// @brief FXMenuCascade for data
        FXMenuCascade* myDataMenuCascade = nullptr;

        /// @brief FXMenuCascade for mean datadata
        FXMenuCascade* myMeanDataMenuCascade = nullptr;

        /// @brief Invalidated copy constructor.
        FileMenuCommands(const FileMenuCommands&) = delete;

        /// @brief Invalidated assignment operator.
        FileMenuCommands& operator=(const FileMenuCommands&) = delete;
    };

    /// @brief struct for modes menu commands
    struct ModesMenuCommands {

        /// @brief struct for common menu commands
        struct CommonMenuCommands {

            /// @brief constructor
            CommonMenuCommands(const ModesMenuCommands* modesMenuCommandsParent);

            /// @brief build menu commands
            void buildCommonMenuCommands(FXMenuPane* modesMenu);

            /// @brief menu command for delete mode
            FXMenuCommand* deleteMode;

            /// @brief menu command for inspect mode
            FXMenuCommand* inspectMode;

            /// @brief menu command for select mode
            FXMenuCommand* selectMode;

        private:
            /// @brief reference to ModesMenuCommands
            const ModesMenuCommands* myModesMenuCommandsParent;

            /// @brief Invalidated copy constructor.
            CommonMenuCommands(const CommonMenuCommands&) = delete;

            /// @brief Invalidated assignment operator.
            CommonMenuCommands& operator=(const CommonMenuCommands&) = delete;
        };

        /// @brief struct for network menu commands
        struct NetworkMenuCommands {

            /// @brief constructor
            NetworkMenuCommands(const ModesMenuCommands* modesMenuCommandsParent);

            /// @brief build menu commands
            void buildNetworkMenuCommands(FXMenuPane* modesMenu);

            /// @brief show all menu commands
            void showNetworkMenuCommands();

            /// @brief hide all menu commands
            void hideNetworkMenuCommands();

            /// @brief menu command for create edge
            FXMenuCommand* createEdgeMode;

            /// @brief menu command for move mode (network)
            FXMenuCommand* moveMode;

            /// @brief menu command for connect mode
            FXMenuCommand* connectMode;

            /// @brief menu command for prohibition mode
            FXMenuCommand* prohibitionMode;

            /// @brief menu command for TLS Mode
            FXMenuCommand* TLSMode;

            /// @brief menu command for additional mode
            FXMenuCommand* additionalMode;

            /// @brief menu command for crossing mode
            FXMenuCommand* crossingMode;

            /// @brief menu command for TAZ mode
            FXMenuCommand* TAZMode;

            /// @brief menu command for shape mode
            FXMenuCommand* shapeMode;

            /// @brief menu command for wire mode
            FXMenuCommand* wireMode;

        private:
            /// @brief reference to ModesMenuCommands
            const ModesMenuCommands* myModesMenuCommandsParent;

            /// @brief Invalidated copy constructor.
            NetworkMenuCommands(const NetworkMenuCommands&) = delete;

            /// @brief Invalidated assignment operator.
            NetworkMenuCommands& operator=(const NetworkMenuCommands&) = delete;
        };

        /// @brief struct for Demand menu commands
        struct DemandMenuCommands {

            /// @brief constructor
            DemandMenuCommands(const ModesMenuCommands* modesMenuCommandsParent);

            /// @brief build menu commands
            void buildDemandMenuCommands(FXMenuPane* modesMenu);

            /// @brief show all menu commands
            void showDemandMenuCommands();

            /// @brief hide all menu commands
            void hideDemandMenuCommands();

            /// @brief menu command for move mode (demand)
            FXMenuCommand* moveMode;

            /// @brief menu command for route mode
            FXMenuCommand* routeMode;

            /// @brief menu command for vehicle mode
            FXMenuCommand* vehicleMode;

            /// @brief menu command for type mode
            FXMenuCommand* typeMode;

            /// @brief menu command for stop mode
            FXMenuCommand* stopMode;

            /// @brief menu command for person mode
            FXMenuCommand* personMode;

            /// @brief menu command for person plan mode
            FXMenuCommand* personPlanMode;

            /// @brief menu command for container mode
            FXMenuCommand* containerMode;

            /// @brief menu command for container plan mode
            FXMenuCommand* containerPlanMode;

        private:
            /// @brief reference to ModesMenuCommands
            const ModesMenuCommands* myModesMenuCommandsParent;

            /// @brief Invalidated copy constructor.
            DemandMenuCommands(const DemandMenuCommands&) = delete;

            /// @brief Invalidated assignment operator.
            DemandMenuCommands& operator=(const DemandMenuCommands&) = delete;
        };

        /// @brief struct for Data menu commands
        struct DataMenuCommands {

            /// @brief constructor
            DataMenuCommands(const ModesMenuCommands* modesMenuCommandsParent);

            /// @brief build menu commands
            void buildDataMenuCommands(FXMenuPane* modesMenu);

            /// @brief show all menu commands
            void showDataMenuCommands();

            /// @brief hide all menu commands
            void hideDataMenuCommands();

            /// @brief menu command for edge mode
            FXMenuCommand* edgeData = nullptr;

            /// @brief menu command for edge rel mode
            FXMenuCommand* edgeRelData = nullptr;

            /// @brief menu command for TAZ rel mode
            FXMenuCommand* TAZRelData = nullptr;

            /// @brief menu command for mean data
            FXMenuCommand* meanData = nullptr;

        private:
            /// @brief reference to ModesMenuCommands
            const ModesMenuCommands* myModesMenuCommandsParent;

            /// @brief Invalidated copy constructor.
            DataMenuCommands(const DataMenuCommands&) = delete;

            /// @brief Invalidated assignment operator.
            DataMenuCommands& operator=(const DataMenuCommands&) = delete;
        };

        /// @brief constructor
        ModesMenuCommands(GNEApplicationWindow* GNEApp);

        /// @brief build modes menu commands
        void buildModesMenuCommands(FXMenuPane* modesMenu);

        /// @brief Common Menu Commands
        CommonMenuCommands commonMenuCommands;

        /// @brief Network Menu Commands
        NetworkMenuCommands networkMenuCommands;

        /// @brief Demand Menu Commands
        DemandMenuCommands demandMenuCommands;

        /// @brief Data Menu Commands
        DataMenuCommands dataMenuCommands;

    private:
        /// @brief pointer to current GNEApplicationWindows
        GNEApplicationWindow* myGNEApp;

        /// @brief Invalidated copy constructor.
        ModesMenuCommands(const ModesMenuCommands&) = delete;

        /// @brief Invalidated assignment operator.
        ModesMenuCommands& operator=(const ModesMenuCommands&) = delete;
    };

    /// @brief struct for edit menu commands
    struct EditMenuCommands {

        /// @brief struct used to group all variables related to view options in supermode Network
        struct NetworkViewOptions {

            /// @brief default constructor
            NetworkViewOptions(GNEApplicationWindow* GNEApp);

            /// @brief build menu checks
            void buildNetworkViewOptionsMenuChecks(FXMenuPane* editMenu);

            /// @brief hide all options menu checks
            void hideNetworkViewOptionsMenuChecks();

            /// @brief update shortcuts (alt+X)
            void updateShortcuts();

            /// @brief menu check to show grid button
            MFXMenuCheckIcon* menuCheckToggleGrid;

            /// @brief menu check to toggle draw junction shape
            MFXMenuCheckIcon* menuCheckToggleDrawJunctionShape;

            /// @brief menu check to draw vehicles in begin position or spread in lane
            MFXMenuCheckIcon* menuCheckDrawSpreadVehicles;

            /// @brief menu check to show Demand Elements
            MFXMenuCheckIcon* menuCheckShowDemandElements;

            /// @brief menu check to select only edges
            MFXMenuCheckIcon* menuCheckSelectEdges;

            /// @brief menu check to show connections
            MFXMenuCheckIcon* menuCheckShowConnections;

            /// @brief menu check to hide connections in connect mode
            MFXMenuCheckIcon* menuCheckHideConnections;

            /// @brief menu check to show additional sub-elements
            MFXMenuCheckIcon* menuCheckShowAdditionalSubElements;

            /// @brief menu check to show TAZ elements
            MFXMenuCheckIcon* menuCheckShowTAZElements;

            /// @brief menu check to extend to edge nodes
            MFXMenuCheckIcon* menuCheckExtendSelection;

            /// @brief menu check to set change all phases
            MFXMenuCheckIcon* menuCheckChangeAllPhases;

            /// @brief menu check to we should warn about merging junctions
            MFXMenuCheckIcon* menuCheckWarnAboutMerge;

            /// @brief menu check to show connection as bubble in "Move" mode.
            MFXMenuCheckIcon* menuCheckShowJunctionBubble;

            /// @brief menu check to apply movement to elevation
            MFXMenuCheckIcon* menuCheckMoveElevation;

            /// @brief menu check to the endpoint for a created edge should be set as the new source
            MFXMenuCheckIcon* menuCheckChainEdges;

            /// @brief check checkable to create auto create opposite edge
            MFXMenuCheckIcon* menuCheckAutoOppositeEdge;

            /// @brief separator
            FXMenuSeparator* separator;

        private:
            /// @brief pointer to current GNEApplicationWindows
            GNEApplicationWindow* myGNEApp;

            /// @brief Invalidated copy constructor.
            NetworkViewOptions(const NetworkViewOptions&) = delete;

            /// @brief Invalidated assignment operator.
            NetworkViewOptions& operator=(const NetworkViewOptions&) = delete;
        };

        /// @brief struct used to group all variables related to view options in supermode Demand
        struct DemandViewOptions {

            /// @brief default constructor
            DemandViewOptions(GNEApplicationWindow* GNEApp);

            /// @brief build menu checks
            void buildDemandViewOptionsMenuChecks(FXMenuPane* editMenu);

            /// @brief hide all options menu checks
            void hideDemandViewOptionsMenuChecks();

            /// @brief update shortcuts (alt+X)
            void updateShortcuts();

            /// @brief menu check to show grid button
            MFXMenuCheckIcon* menuCheckToggleGrid;

            /// @brief menu check to toggle draw junction shape
            MFXMenuCheckIcon* menuCheckToggleDrawJunctionShape;

            /// @brief menu check to draw vehicles in begin position or spread in lane
            MFXMenuCheckIcon* menuCheckDrawSpreadVehicles;

            /// @brief menu check for hide shapes (Polygons and POIs)
            MFXMenuCheckIcon* menuCheckHideShapes;

            /// @brief menu check for show all trips plans
            MFXMenuCheckIcon* menuCheckShowAllTrips;

            /// @brief menu check for hide non inspected demand elements
            MFXMenuCheckIcon* menuCheckHideNonInspectedDemandElements;

            /// @brief menu check for show overlapped routes
            MFXMenuCheckIcon* menuCheckShowOverlappedRoutes;

            /// @brief menu check for show all person plans
            MFXMenuCheckIcon* menuCheckShowAllPersonPlans;

            /// @brief menu check for lock Person
            MFXMenuCheckIcon* menuCheckLockPerson;

            /// @brief menu check for show all container plans
            MFXMenuCheckIcon* menuCheckShowAllContainerPlans;

            /// @brief menu check for lock Container
            MFXMenuCheckIcon* menuCheckLockContainer;

            /// @brief separator
            FXMenuSeparator* separator;

        private:
            /// @brief pointer to current GNEApplicationWindows
            GNEApplicationWindow* myGNEApp;

            /// @brief Invalidated copy constructor.
            DemandViewOptions(const DemandViewOptions&) = delete;

            /// @brief Invalidated assignment operator.
            DemandViewOptions& operator=(const DemandViewOptions&) = delete;
        };

        /// @brief struct used to group all variables related to view options in supermode Data
        struct DataViewOptions {

            /// @brief default constructor
            DataViewOptions(GNEApplicationWindow* GNEApp);

            /// @brief build menu checks
            void buildDataViewOptionsMenuChecks(FXMenuPane* editMenu);

            /// @brief hide all options menu checks
            void hideDataViewOptionsMenuChecks();

            /// @brief update shortcuts (alt+X)
            void updateShortcuts();

            /// @brief menu check to toggle draw junction shape
            MFXMenuCheckIcon* menuCheckToggleDrawJunctionShape;

            /// @brief menu check to show Additionals
            MFXMenuCheckIcon* menuCheckShowAdditionals;

            /// @brief menu check to show Shapes
            MFXMenuCheckIcon* menuCheckShowShapes;

            /// @brief menu check to show Demand Elements
            MFXMenuCheckIcon* menuCheckShowDemandElements;

            /// @brief menu check to toggle TAZRel drawing mode
            MFXMenuCheckIcon* menuCheckToggleTAZRelDrawing;

            /// @brief menu check to toggle TAZ draw fill
            MFXMenuCheckIcon* menuCheckToggleTAZDrawFill;

            /// @brief menu check to toggle TAZRel only from
            MFXMenuCheckIcon* menuCheckToggleTAZRelOnlyFrom;

            /// @brief menu check to toggle TAZRel only to
            MFXMenuCheckIcon* menuCheckToggleTAZRelOnlyTo;

            /// @brief separator
            FXMenuSeparator* separator;

        private:
            /// @brief pointer to current GNEApplicationWindows
            GNEApplicationWindow* myGNEApp;

            /// @brief Invalidated copy constructor.
            DataViewOptions(const DataViewOptions&) = delete;

            /// @brief Invalidated assignment operator.
            DataViewOptions& operator=(const DataViewOptions&) = delete;
        };

        /// @brief constructor
        EditMenuCommands(GNEApplicationWindow* GNEApp);

        /// @brief build undo-redo menu commands
        void buildUndoRedoMenuCommands(FXMenuPane* editMenu);

        /// @brief build view menu commands
        void buildViewMenuCommands(FXMenuPane* editMenu);

        /// @brief build front element commands
        void buildFrontElementMenuCommand(FXMenuPane* editMenu);

        /// @brief build open sumo menu commands
        void buildOpenSUMOMenuCommands(FXMenuPane* editMenu);

        /// @brief FXMenuCommand for undo last change
        FXMenuCommand* undoLastChange;

        /// @brief FXMenuCommand for redo last change
        FXMenuCommand* redoLastChange;

        /// @brief FXMenuCommand for open undolist dialog
        FXMenuCommand* openUndolistDialog;

        /// @brief network view options
        NetworkViewOptions networkViewOptions;

        /// @brief demand view options
        DemandViewOptions demandViewOptions;

        /// @brief data view options
        DataViewOptions dataViewOptions;

        /// @brief FXMenuCommand for edit view scheme
        FXMenuCommand* editViewScheme;

        /// @brief FXMenuCommand for edit view port
        FXMenuCommand* editViewPort;

        /// @brief FXMenuCommand for clear Front element
        FXMenuCommand* clearFrontElement;

        /// @brief menu check for load additionals in SUMO GUI
        FXMenuCheck* loadAdditionalsInSUMOGUI;

        /// @brief menu check for load demand in SUMO GUI
        FXMenuCheck* loadDemandInSUMOGUI;

        /// @brief FXMenuCommand for open in SUMO GUI
        FXMenuCommand* openInSUMOGUI;

    private:
        /// @brief pointer to current GNEApplicationWindows
        GNEApplicationWindow* myGNEApp;

        /// @brief Invalidated copy constructor.
        EditMenuCommands(const EditMenuCommands&) = delete;

        /// @brief Invalidated assignment operator.
        EditMenuCommands& operator=(const EditMenuCommands&) = delete;
    };

    /// @brief struct for processing menu commands
    struct LockMenuCommands {

        /// @brief constructor
        LockMenuCommands(GNEApplicationWindow* GNEApp);

        /// @brief build menu commands
        void buildLockMenuCommands(FXMenuPane* editMenu);

        /// @brief remove hotkeys
        void removeHotkeys();

        /// @brief show network processing menu commands
        void showNetworkLockMenuCommands();

        /// @brief show network processing menu commands
        void hideNetworkLockMenuCommands();

        /// @brief show demand processing menu commands
        void showDemandLockMenuCommands();

        /// @brief show demand processing menu commands
        void hideDemandLockMenuCommands();

        /// @brief show data processing menu commands
        void showDataLockMenuCommands();

        /// @brief show data processing menu commands
        void hideDataLockMenuCommands();

        /// @brief lock all
        void lockAll();

        /// @brief unlock all
        void unlockAll();

        /// @brief lock or unlock object based in a tag property
        void editLocking(const GNEAttributeCarrier* AC, const FXbool value);

        /// @name Processing FXMenuCommands for Network mode
        /// @{
        /// @brief menu check to lock junction
        MFXMenuCheckIcon* menuCheckLockJunction = nullptr;

        /// @brief menu check to lock edges
        MFXMenuCheckIcon* menuCheckLockEdges = nullptr;

        /// @brief menu check to lock lanes
        MFXMenuCheckIcon* menuCheckLockLanes = nullptr;

        /// @brief menu check to lock connections
        MFXMenuCheckIcon* menuCheckLockConnections = nullptr;

        /// @brief menu check to lock crossings
        MFXMenuCheckIcon* menuCheckLockCrossings = nullptr;

        /// @brief menu check to lock walkingAreas
        MFXMenuCheckIcon* menuCheckLockWalkingAreas = nullptr;

        /// @brief menu check to lock additionals
        MFXMenuCheckIcon* menuCheckLockAdditionals = nullptr;

        /// @brief menu check to lock TAZs
        MFXMenuCheckIcon* menuCheckLockTAZs = nullptr;

        /// @brief menu check to lock Wires
        MFXMenuCheckIcon* menuCheckLockWires = nullptr;

        /// @brief menu check to lock polygons
        MFXMenuCheckIcon* menuCheckLockPolygons = nullptr;

        /// @brief menu check to lock POIs
        MFXMenuCheckIcon* menuCheckLockPOIs = nullptr;

        /// @}

        /// @name Processing FXMenuCommands for Demand mode
        /// @{
        /// @brief menu check to lock routes
        MFXMenuCheckIcon* menuCheckLockRoutes;

        /// @brief menu check to lock vehicles
        MFXMenuCheckIcon* menuCheckLockVehicles;

        /// @brief menu check to lock persons
        MFXMenuCheckIcon* menuCheckLockPersons;

        /// @brief menu check to lock personTrips
        MFXMenuCheckIcon* menuCheckLockPersonTrip;

        /// @brief menu check to lock walks
        MFXMenuCheckIcon* menuCheckLockWalk;

        /// @brief menu check to lock rides
        MFXMenuCheckIcon* menuCheckLockRides;

        /// @brief menu check to lock containers
        MFXMenuCheckIcon* menuCheckLockContainers;

        /// @brief menu check to lock transports
        MFXMenuCheckIcon* menuCheckLockTransports;

        /// @brief menu check to lock tranships
        MFXMenuCheckIcon* menuCheckLockTranships;

        /// @brief menu check to lock stops
        MFXMenuCheckIcon* menuCheckLockStops;

        /// @}

        /// @name Processing FXMenuCommands for Data mode
        /// @{
        /// @brief menu check to lock edge datas
        MFXMenuCheckIcon* menuCheckLockEdgeDatas;

        /// @brief menu check to lock edgeRelDatas
        MFXMenuCheckIcon* menuCheckLockEdgeRelDatas;

        /// @brief menu check to lock edgeTAZRels
        MFXMenuCheckIcon* menuCheckLockEdgeTAZRels;

        /// @}

        /// @brief menu check for lock selected elements
        MFXMenuCheckIcon* menuCheckLockSelectedElements;

    protected:
        /// @brief Parse hot key from string
        FXHotKey parseHotKey(const FXwchar character);

    private:
        /// @brief pointer to current GNEApplicationWindows
        GNEApplicationWindow* myGNEApp;

        /// @brief Invalidated copy constructor.
        LockMenuCommands(const LockMenuCommands&) = delete;

        /// @brief Invalidated assignment operator.
        LockMenuCommands& operator=(const LockMenuCommands&) = delete;
    };

    /// @brief struct for processing menu commands
    struct ProcessingMenuCommands {

        /// @brief constructor
        ProcessingMenuCommands(GNEApplicationWindow* GNEApp);

        /// @brief build menu commands
        void buildProcessingMenuCommands(FXMenuPane* processingMenu);

        /// @brief show network processing menu commands
        void showNetworkProcessingMenuCommands();

        /// @brief show network processing menu commands
        void hideNetworkProcessingMenuCommands();

        /// @brief show demand processing menu commands
        void showDemandProcessingMenuCommands();

        /// @brief show demand processing menu commands
        void hideDemandProcessingMenuCommands();

        /// @brief show data processing menu commands
        void showDataProcessingMenuCommands();

        /// @brief show data processing menu commands
        void hideDataProcessingMenuCommands();

        /// @brief show separator
        void showSeparator();

        /// @brief hide separator
        void hideSeparator();

        /// @name Processing FXMenuCommands for Network mode
        /// @{
        /// @brief FXMenuCommand for compute network
        FXMenuCommand* computeNetwork = nullptr;

        /// @brief FXMenuCommand for compute network with volatile options
        FXMenuCommand* computeNetworkVolatile = nullptr;

        /// @brief FXMenuCommand for clean junctions without edges
        FXMenuCommand* cleanJunctions = nullptr;

        /// @brief FXMenuCommand for join selected junctions
        FXMenuCommand* joinJunctions = nullptr;

        /// @brief FXMenuCommand for clear invalid crossings
        FXMenuCommand* clearInvalidCrossings = nullptr;

        /// @brief checkBox for recomputing when changing data mode
        MFXMenuCheckIcon* menuCheckRecomputeDataMode = nullptr;

        /// @}

        /// @name Processing FXMenuCommands for Demand mode
        /// @{
        /// @brief FXMenuCommand for compute demand elements
        FXMenuCommand* computeDemand = nullptr;

        /// @brief FXMenuCommand for clean routes without vehicles
        FXMenuCommand* cleanRoutes = nullptr;

        /// @brief FXMenuCommand for join routes
        FXMenuCommand* joinRoutes = nullptr;

        /// @brief FXMenuCommand for adjust person plans
        FXMenuCommand* adjustPersonPlans = nullptr;

        /// @brief FXMenuCommand for clear invalid demand elements
        FXMenuCommand* clearInvalidDemandElements = nullptr;
        /// @}

        /// @name Processing FXMenuCommands for Data mode
        /// @{

        /// @}

        /// @brief FXMenuCommand for open option menus
        FXMenuCommand* optionMenus = nullptr;

    private:
        /// @brief pointer to current GNEApplicationWindows
        GNEApplicationWindow* myGNEApp;

        /// @brief separator for optionsMenu
        FXMenuSeparator* myOptionsSeparator = nullptr;

        /// @brief separator for checkBox
        FXMenuSeparator* mySeparatorCheckBox = nullptr;

        /// @brief Invalidated copy constructor.
        ProcessingMenuCommands(const ProcessingMenuCommands&) = delete;

        /// @brief Invalidated assignment operator.
        ProcessingMenuCommands& operator=(const ProcessingMenuCommands&) = delete;
    };

    /// @brief struct for locate menu commands
    struct LocateMenuCommands {

        /// @brief constructor
        LocateMenuCommands(GNEApplicationWindow* GNEApp);

        /// @brief build menu commands
        void buildLocateMenuCommands(FXMenuPane* locateMenu);

    private:
        /// @brief pointer to current GNEApplicationWindows
        GNEApplicationWindow* myGNEApp;

        /// @brief Invalidated copy constructor.
        LocateMenuCommands(const LocateMenuCommands&) = delete;

        /// @brief Invalidated assignment operator.
        LocateMenuCommands& operator=(const LocateMenuCommands&) = delete;
    };

    /// @brief struct for locate menu commands
    struct ToolsMenuCommands {

        /// @brief constructor
        ToolsMenuCommands(GNEApplicationWindow* GNEApp);

        /// @brief destructor
        ~ToolsMenuCommands();

        /// @brief build tools (and menu commands)
        void buildTools(FXMenuPane* toolsMenu, const std::map<std::string, FXMenuPane*>& menuPaneToolMaps);

        /// @brief show tool
        long showTool(FXObject* menuCommand) const;

        /// @brief show netgenerate dialog
        long showNetgenerateDialog() const;

        /// @brief run tool dialog
        long runToolDialog(FXObject* menuCommand) const;

        /// @brief run postprocessing
        long postProcessing(FXObject* menuCommand) const;

        /// @brief run netgenerate dialog
        long runNetgenerateDialog(const OptionsCont* netgenerateOptions) const;

    private:
        /// @brief map with python tools
        std::vector<GNEPythonTool*> myPythonTools;

        /// @brief python tool dialog
        GNEPythonToolDialog* myPythonToolDialog = nullptr;

        /// @brief netgenerate dialog
        GNENetgenerateDialog* myNetgenerateDialog = nullptr;

        /// @brief run python tool dialog
        GNERunPythonToolDialog* myRunPythonToolDialog = nullptr;

        /// @brief run netgenerate dialog
        GNERunNetgenerateDialog* myRunNetgenerateDialog = nullptr;

        /// @brief pointer to current GNEApplicationWindows
        GNEApplicationWindow* myGNEApp;

        /// @brief Invalidated copy constructor.
        ToolsMenuCommands(const ToolsMenuCommands&) = delete;

        /// @brief Invalidated assignment operator.
        ToolsMenuCommands& operator=(const ToolsMenuCommands&) = delete;
    };

    /// @brief struct for windows menu commands
    struct WindowsMenuCommands {

        /// @brief constructor
        WindowsMenuCommands(GNEApplicationWindow* GNEApp);

        /// @brief build menu commands
        void buildWindowsMenuCommands(FXMenuPane* windowsMenu, FXStatusBar* statusbar, GUIMessageWindow* messageWindow);

    private:
        /// @brief pointer to current GNEApplicationWindows
        GNEApplicationWindow* myGNEApp;

        /// @brief Invalidated copy constructor.
        WindowsMenuCommands(const WindowsMenuCommands&) = delete;

        /// @brief Invalidated assignment operator.
        WindowsMenuCommands& operator=(const WindowsMenuCommands&) = delete;
    };

    /// @brief struct for help menu commands
    struct HelpMenuCommands {

        /// @brief constructor
        HelpMenuCommands(GNEApplicationWindow* GNEApp);

        /// @brief build menu commands
        void buildHelpMenuCommands(FXMenuPane* helpMenu);

    private:
        /// @brief pointer to current GNEApplicationWindow
        GNEApplicationWindow* myGNEApp;

        /// @brief Invalidated copy constructor.
        HelpMenuCommands(const HelpMenuCommands&) = delete;

        /// @brief Invalidated assignment operator.
        HelpMenuCommands& operator=(const HelpMenuCommands&) = delete;
    };

    /// @brief struct for supermode commands
    struct SupermodeCommands {

        /// @brief constructor
        SupermodeCommands(GNEApplicationWindow* GNEApp);

        /// @brief build menu commands
        void buildSupermodeCommands(FXMenuPane* editMenu);

        /// @brief show all menu commands
        void showSupermodeCommands();

        /// @brief hide all menu commands
        void hideSupermodeCommands();

        /// @brief FXMenuCommand for network supermode
        FXMenuCommand* networkMode;

        /// @brief FXMenuCommand for demand supermode
        FXMenuCommand* demandMode;

        /// @brief FXMenuCommand for data supermode
        FXMenuCommand* dataMode;

    private:
        /// @brief pointer to current GNEApplicationWindows
        GNEApplicationWindow* myGNEApp;

        /// @brief Invalidated copy constructor.
        SupermodeCommands(const SupermodeCommands&) = delete;

        /// @brief Invalidated assignment operator.
        SupermodeCommands& operator=(const SupermodeCommands&) = delete;
    };

    /// @brief SUMO config handler
    class GNESumoConfigHandler {

    public:
        /// @brief Constructor
        GNESumoConfigHandler(OptionsCont& sumoOptions, const std::string& file);

        /// @brief load SUMO config
        bool loadSumoConfig();

    private:
        /// @brief sumo options
        OptionsCont& mySumoOptions;

        /// @brief SUMO config file
        const std::string myFile;
    };

    /// @brief netedit config handler
    class GNENeteditConfigHandler {

    public:
        /// @brief Constructor
        GNENeteditConfigHandler(const std::string& file);

        /// @brief load netedit config
        bool loadNeteditConfig();

    private:
        /// @brief netedit config file
        const std::string myFile;
    };

    /// @brief toggle edit options Network menu commands (called in GNEApplicationWindow::onCmdToggleEditOptions)
    static bool toggleEditOptionsNetwork(GNEViewNet* viewNet, const MFXCheckableButton* menuCheck,
                                         const int numericalKeyPressed, FXObject* obj, FXSelector sel);

    /// @brief toggle edit options Demand menu commands (called in GNEApplicationWindow::onCmdToggleEditOptions)
    static bool toggleEditOptionsDemand(GNEViewNet* viewNet, const MFXCheckableButton* menuCheck,
                                        const int numericalKeyPressed, FXObject* obj, FXSelector sel);

    /// @brief toggle edit options Data menu commands (called in GNEApplicationWindow::onCmdToggleEditOptions)
    static bool toggleEditOptionsData(GNEViewNet* viewNet, const MFXCheckableButton* menuCheck,
                                      const int numericalKeyPressed, FXObject* obj, FXSelector sel);

    /// @brief check if a string ends with another string
    static bool stringEndsWith(const std::string& str, const std::string& suffix);

    /// @brief open general file dialog
    static std::string openFileDialog(FXWindow* window, bool save, bool multi);

    /// @brief open netconvert file dialog
    static std::string openNetworkFileDialog(FXWindow* window, bool save, bool multi = false);

    /// @brief open netconvert file dialog
    static std::string openNetconvertFileDialog(FXWindow* window);

    /// @brief open plain xml file dialog
    static std::string savePlainXMLFileDialog(FXWindow* window);

    /// @brief open joined junctions file dialog
    static std::string saveJoinedJunctionsFileDialog(FXWindow* window);

    /// @brief open tool file dialog
    static std::string saveToolLog(FXWindow* window);

    /// @brief open OSM file dialog
    static std::string openOSMFileDialog(FXWindow* window);

    /// @brief open netedit config file dialog
    static std::string openNeteditConfigFileDialog(FXWindow* window, bool save);

    /// @brief open SUMO config file dialog
    static std::string openSumoConfigFileDialog(FXWindow* window, bool save, bool multi = false);

    /// @brief open TLS file dialog
    static std::string openTLSFileDialog(FXWindow* window, bool save);

    /// @brief open edgeType file dialog
    static std::string openEdgeTypeFileDialog(FXWindow* window, bool save);

    /// @brief open additional dialog
    static std::string openAdditionalFileDialog(FXWindow* window, bool save, bool multi = false);

    /// @brief open route file dialog
    static std::string openRouteFileDialog(FXWindow* window, bool save, bool multi = false);

    /// @brief open data file dialog
    static std::string openDataFileDialog(FXWindow* window, bool save, bool multi = false);

    /// @brief open meandata filename dialog
    static std::string openMeanDataDialog(FXWindow* window, bool save, bool multi = false);

    /// @brief open option dialog
    static std::string openOptionFileDialog(FXWindow* window, bool save);

private:
    /// @brief open filename dialog
    static std::string openFileDialog(FXWindow* window, const std::string title, GUIIcon icon, const std::string patternList, bool save, bool multi = false);

    /// @brief Invalidated copy constructor.
    GNEApplicationWindowHelper(const GNEApplicationWindowHelper&) = delete;

    /// @brief Invalidated assignment operator.
    GNEApplicationWindowHelper& operator=(const GNEApplicationWindowHelper&) = delete;
};
