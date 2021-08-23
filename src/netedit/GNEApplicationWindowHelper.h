/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
// Functions from main window of NETEDIT
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/common/SUMOTime.h>
#include <utils/foxtools/FXSynchQue.h>
#include <utils/foxtools/FXRecentNetworks.h>
#include <utils/foxtools/FXThreadEvent.h>
#include <utils/foxtools/MFXInterThreadEventClient.h>
#include <utils/geom/Position.h>
#include <utils/gui/div/GUIMessageWindow.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/shapes/ShapeHandler.h>

#include "GNEViewNetHelper.h"


// ===========================================================================
// class declarations
// ===========================================================================
class GNEApplicationWindow;
class GNELoadThread;
class GNEUndoList;
class GNENet;
class GNEViewNet;
class GUIEvent;
class FXMenuCheckIcon;


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
        FXMenuBar* menu;

        /// @brief The application menu bar for supermodes (network and demand)
        FXMenuBar* superModes;

        /// @brief The application menu bar for save elements (NetworkElements, additionals and demand elements)
        FXMenuBar* saveElements;

        /// @brief The application menu bar for navigation (zoom, coloring...)
        FXMenuBar* navigation;

        /// @brief The application menu bar (for select, inspect...)
        FXMenuBar* modes;

        /// @brief The application menu bar for mode options (show connections, select edges...)
        FXMenuBar* intervalBar;

    private:
        /// @brief pointer to current GNEApplicationWindow
        GNEApplicationWindow* myGNEApp;

        /// @brief menu bar drag (for file, edit, processing...)
        FXToolBarShell* myToolBarShellMenu;

        /// @brief menu bar drag for modes (network and demand)
        FXToolBarShell* myToolBarShellSuperModes;

        /// @brief menu bar drag for save elements (NetworkElements, additionals and demand elements)
        FXToolBarShell* myToolBarShellSaveElements;

        /// @brief menu bar drag for navigation (Zoom, coloring...)
        FXToolBarShell* myToolBarShellNavigation;

        /// @brief menu bar drag for modes (select, inspect, delete...)
        FXToolBarShell* myToolBarShellModes;

        /// @brief menu bar drag for interval bar
        FXToolBarShell* myToolBarShellIntervalBar;

        /// @brief Invalidated copy constructor.
        ToolbarsGrip(const ToolbarsGrip&) = delete;

        /// @brief Invalidated assignment operator.
        ToolbarsGrip& operator=(const ToolbarsGrip&) = delete;
    };

    /// @brief struct for menu bar file
    struct MenuBarFile {

        /// @brief constructor
        MenuBarFile(GNEApplicationWindow* GNEApp);

        /// @brief build recent files
        void buildRecentFiles(FXMenuPane* fileMenu);

        /// @brief List of recent nets and config files
        FXRecentNetworks myRecentNetsAndConfigs;

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
        void buildFileMenuCommands(FXMenuPane* fileMenu, FXMenuPane* fileMenuTLS, FXMenuPane* fileMenuEdgeTypes, FXMenuPane* fileMenuAdditionals,
                                   FXMenuPane* fileMenuDemandElements, FXMenuPane* fileMenuDataElements);

        /// @brief FXMenuCommand for enable or disable save additionals
        FXMenuCommand* saveAdditionals;

        /// @brief FXMenuCommand for enable or disable save additionals As
        FXMenuCommand* saveAdditionalsAs;

        /// @brief FXMenuCommand for reload additionals
        FXMenuCommand* reloadAdditionals;

        /// @brief FXMenuCommand for enable or disable save additionals
        FXMenuCommand* saveTLSPrograms;

        /// @brief FXMenuCommand for enable or disable save edgeTypes
        FXMenuCommand* saveEdgeTypes;

        /// @brief FXMenuCommand for enable or disable save demand elements
        FXMenuCommand* saveDemandElements;

        /// @brief FXMenuCommand for enable or disable save demand elements as
        FXMenuCommand* saveDemandElementsAs;

        /// @brief FXMenuCommand for reload demand elements
        FXMenuCommand* reloadDemandElements;

        /// @brief FXMenuCommand for enable or disable save data elements
        FXMenuCommand* saveDataElements;

        /// @brief FXMenuCommand for enable or disable save data elements as
        FXMenuCommand* saveDataElementsAs;

        /// @brief FXMenuCommand for reload data elements
        FXMenuCommand* reloadDataElements;

    private:
        /// @brief pointer to current GNEApplicationWindows
        GNEApplicationWindow* myGNEApp;

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

            /// @brief menu command for vehicle type mode
            FXMenuCommand* vehicleTypeMode;

            /// @brief menu command for stop mode
            FXMenuCommand* stopMode;

            /// @brief menu command for person type mode
            FXMenuCommand* personTypeMode;

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
            FXMenuCommand* edgeData;

            /// @brief menu command for edge rel mode
            FXMenuCommand* edgeRelData;

            /// @brief menu command for TAZ rel mode
            FXMenuCommand* TAZRelData;

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

            /// @brief menu check to show grid button
            FXMenuCheckIcon* menuCheckToggleGrid;

            /// @brief menu check to draw vehicles in begin position or spread in lane
            FXMenuCheckIcon* menuCheckDrawSpreadVehicles;

            /// @brief menu check to show Demand Elements
            FXMenuCheckIcon* menuCheckShowDemandElements;

            /// @brief menu check to select only edges
            FXMenuCheckIcon* menuCheckSelectEdges;

            /// @brief menu check to show connections
            FXMenuCheckIcon* menuCheckShowConnections;

            /// @brief menu check to hide connections in connect mode
            FXMenuCheckIcon* menuCheckHideConnections;

            /// @brief menu check to extend to edge nodes
            FXMenuCheckIcon* menuCheckExtendSelection;

            /// @brief menu check to set change all phases
            FXMenuCheckIcon* menuCheckChangeAllPhases;

            /// @brief menu check to we should warn about merging junctions
            FXMenuCheckIcon* menuCheckWarnAboutMerge;

            /// @brief menu check to show connection as buuble in "Move" mode.
            FXMenuCheckIcon* menuCheckShowJunctionBubble;

            /// @brief menu check to apply movement to elevation
            FXMenuCheckIcon* menuCheckMoveElevation;

            /// @brief menu check to the endpoint for a created edge should be set as the new source
            FXMenuCheckIcon* menuCheckChainEdges;

            /// @brief check checkable to create auto create opposite edge
            FXMenuCheckIcon* menuCheckAutoOppositeEdge;

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

            /// @brief menu check to show grid button
            FXMenuCheckIcon* menuCheckToggleGrid;

            /// @brief menu check to draw vehicles in begin position or spread in lane
            FXMenuCheckIcon* menuCheckDrawSpreadVehicles;

            /// @brief menu check for hide shapes (Polygons and POIs)
            FXMenuCheckIcon* menuCheckHideShapes;

            /// @brief menu check for show all trips plans
            FXMenuCheckIcon* menuCheckShowAllTrips;

            /// @brief menu check for hide non inspected demand elements
            FXMenuCheckIcon* menuCheckHideNonInspectedDemandElements;

            /// @brief menu check for show all person plans
            FXMenuCheckIcon* menuCheckShowAllPersonPlans;

            /// @brief menu check for lock Person
            FXMenuCheckIcon* menuCheckLockPerson;

            /// @brief menu check for show all container plans
            FXMenuCheckIcon* menuCheckShowAllContainerPlans;

            /// @brief menu check for lock Container
            FXMenuCheckIcon* menuCheckLockContainer;

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

            /// @brief menu check to show Additionals
            FXMenuCheckIcon* menuCheckShowAdditionals;

            /// @brief menu check to show Shapes
            FXMenuCheckIcon* menuCheckShowShapes;

            /// @brief menu check to show Demand Elements
            FXMenuCheckIcon* menuCheckShowDemandElements;

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

        /// @name Processing FXMenuCommands for Network mode
        /// @{
        /// @brief menu check to lock junction
        FXMenuCheckIcon* menuCheckLockJunction;

        /// @brief menu check to lock edges
        FXMenuCheckIcon* menuCheckLockEdges;

        /// @brief menu check to lock lanes
        FXMenuCheckIcon* menuCheckLockLanes;

        /// @brief menu check to lock connections
        FXMenuCheckIcon* menuCheckLockConnections;

        /// @brief menu check to lock crossings
        FXMenuCheckIcon* menuCheckLockCrossings;

        /// @brief menu check to lock additionals
        FXMenuCheckIcon* menuCheckLockAdditionals;

        /// @brief menu check to lock TAZs
        FXMenuCheckIcon* menuCheckLockTAZs;

        /// @brief menu check to lock polygons
        FXMenuCheckIcon* menuCheckLockPolygons;

        /// @brief menu check to lock POIs
        FXMenuCheckIcon* menuCheckLockPOIs;

        /// @}

        /// @name Processing FXMenuCommands for Demand mode
        /// @{
        /// @brief menu check to lock routes
        FXMenuCheckIcon* menuCheckLockRoutes;

        /// @brief menu check to lock vehicles
        FXMenuCheckIcon* menuCheckLockVehicles;

        /// @brief menu check to lock persons
        FXMenuCheckIcon* menuCheckLockPersons;

        /// @brief menu check to lock personTrips
        FXMenuCheckIcon* menuCheckLockPersonTrip;

        /// @brief menu check to lock walks
        FXMenuCheckIcon* menuCheckLockWalk;

        /// @brief menu check to lock rides
        FXMenuCheckIcon* menuCheckLockRides;

        /// @brief menu check to lock containers
        FXMenuCheckIcon* menuCheckLockContainers;

        /// @brief menu check to lock transports
        FXMenuCheckIcon* menuCheckLockTransports;

        /// @brief menu check to lock tranships
        FXMenuCheckIcon* menuCheckLockTranships;

        /// @brief menu check to lock stops
        FXMenuCheckIcon* menuCheckLockStops;

        /// @}

        /// @name Processing FXMenuCommands for Data mode
        /// @{
        /// @brief menu check to lock edge datas
        FXMenuCheckIcon* menuCheckLockEdgeDatas;

        /// @brief menu check to lock edgeRelDatas
        FXMenuCheckIcon* menuCheckLockEdgeRelDatas;

        /// @brief menu check to lock edgeTAZRels
        FXMenuCheckIcon* menuCheckLockEdgeTAZRels;

        /// @}

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
        void buildProcessingMenuCommands(FXMenuPane* editMenu);

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

        /// @name Processing FXMenuCommands for Network mode
        /// @{
        /// @brief FXMenuCommand for compute network
        FXMenuCommand* computeNetwork;

        /// @brief FXMenuCommand for compute network with volatile options
        FXMenuCommand* computeNetworkVolatile;

        /// @brief FXMenuCommand for clean junctions without edges
        FXMenuCommand* cleanJunctions;

        /// @brief FXMenuCommand for join selected junctions
        FXMenuCommand* joinJunctions;

        /// @brief FXMenuCommand for clear invalid crosings
        FXMenuCommand* clearInvalidCrossings;
        /// @}

        /// @name Processing FXMenuCommands for Demand mode
        /// @{
        /// @brief FXMenuCommand for compute demand elements
        FXMenuCommand* computeDemand;

        /// @brief FXMenuCommand for clean routes without vehicles
        FXMenuCommand* cleanRoutes;

        /// @brief FXMenuCommand for join routes
        FXMenuCommand* joinRoutes;

        /// @brief FXMenuCommand for adjust person plans
        FXMenuCommand* adjustPersonPlans;

        /// @brief FXMenuCommand for clear invalid demand elements
        FXMenuCommand* clearInvalidDemandElements;
        /// @}

        /// @name Processing FXMenuCommands for Data mode
        /// @{

        /// @}

        /// @brief FXMenuCommand for open option menus
        FXMenuCommand* optionMenus;

    private:
        /// @brief pointer to current GNEApplicationWindows
        GNEApplicationWindow* myGNEApp;

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

    /// @brief toggle edit options Network menu commands (called in GNEApplicationWindow::onCmdToggleEditOptions)
    static bool toggleEditOptionsNetwork(GNEViewNet* viewNet, const MFXCheckableButton* menuCheck,
                                         const int numericalKeyPressed, FXObject* obj, FXSelector sel);

    /// @brief toggle edit options Demand menu commands (called in GNEApplicationWindow::onCmdToggleEditOptions)
    static bool toggleEditOptionsDemand(GNEViewNet* viewNet, const MFXCheckableButton* menuCheck,
                                        const int numericalKeyPressed, FXObject* obj, FXSelector sel);

    /// @brief toggle edit options Data menu commands (called in GNEApplicationWindow::onCmdToggleEditOptions)
    static bool toggleEditOptionsData(GNEViewNet* viewNet, const MFXCheckableButton* menuCheck,
                                      const int numericalKeyPressed, FXObject* obj, FXSelector sel);

private:
    /// @brief Invalidated copy constructor.
    GNEApplicationWindowHelper(const GNEApplicationWindowHelper&) = delete;

    /// @brief Invalidated assignment operator.
    GNEApplicationWindowHelper& operator=(const GNEApplicationWindowHelper&) = delete;
};
