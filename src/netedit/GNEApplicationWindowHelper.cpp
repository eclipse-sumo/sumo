/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    GNEApplicationWindowHelper.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
///
// The main window of Netedit (adapted from GUIApplicationWindow)
/****************************************************************************/
#include <netbuild/NBFrame.h>
#include <netedit/elements/GNEAttributeCarrier.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/options/OptionsCont.h>
#include <utils/foxtools/FXMenuCheckIcon.h>

#include "GNEApplicationWindow.h"
#include "GNEViewNet.h"

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

// ===========================================================================
// GNEApplicationWindowHelper::ToolbarsGrip method definitions
// ===========================================================================

GNEApplicationWindowHelper::ToolbarsGrip::ToolbarsGrip(GNEApplicationWindow* GNEApp) :
    menu(nullptr),
    superModes(nullptr),
    saveElements(nullptr),
    navigation(nullptr),
    modes(nullptr),
    intervalBar(nullptr),
    myGNEApp(GNEApp),
    myToolBarShellMenu(nullptr),
    myToolBarShellSuperModes(nullptr),
    myToolBarShellSaveElements(nullptr),
    myToolBarShellNavigation(nullptr),
    myToolBarShellModes(nullptr),
    myToolBarShellIntervalBar(nullptr) {
}


void
GNEApplicationWindowHelper::ToolbarsGrip::buildMenuToolbarsGrip() {
    // build menu bar (for File, edit, processing...) using specify design
    myToolBarShellMenu = new FXToolBarShell(myGNEApp, GUIDesignToolBar);
    menu = new FXMenuBar(myGNEApp->getTopDock(), myToolBarShellMenu, GUIDesignToolbarMenuBarNetedit);
    // declare toolbar grip for menu bar
    new FXToolBarGrip(menu, menu, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
}


void
GNEApplicationWindowHelper::ToolbarsGrip::buildViewParentToolbarsGrips() {
    // build menu bar for supermodes (next to menu bar)
    myToolBarShellSuperModes = new FXToolBarShell(myGNEApp, GUIDesignToolBar);
    superModes = new FXMenuBar(myGNEApp->getTopDock(), myToolBarShellSuperModes, GUIDesignToolBarRaisedSame);
    // declare toolbar grip for menu bar superModes
    new FXToolBarGrip(superModes, superModes, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
    // build menu bar for save elements (bot to menu bar)
    myToolBarShellSaveElements = new FXToolBarShell(myGNEApp, GUIDesignToolBar);
    saveElements = new FXMenuBar(myGNEApp->getTopDock(), myToolBarShellSaveElements, GUIDesignToolBarRaisedNext);
    // declare toolbar grip for menu bar saveElements
    new FXToolBarGrip(saveElements, saveElements, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
    // build menu bar for navigation
    myToolBarShellNavigation = new FXToolBarShell(myGNEApp, GUIDesignToolBar);
    navigation = new FXMenuBar(myGNEApp->getTopDock(), myToolBarShellNavigation, GUIDesignToolBarRaisedSame);
    // declare toolbar grip for menu bar navigation
    new FXToolBarGrip(navigation, navigation, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
    // build menu bar for modes
    myToolBarShellModes = new FXToolBarShell(myGNEApp, GUIDesignToolBar);
    // create modes depending of option "gui-testing" (note: Used for NETEDIT test)
    if (OptionsCont::getOptions().getBool("gui-testing")) {
        modes = new FXMenuBar(myGNEApp->getTopDock(), myToolBarShellModes, GUIDesignToolBarRaisedNext);
    } else {
        modes = new FXMenuBar(myGNEApp->getTopDock(), myToolBarShellModes, GUIDesignToolBarRaisedSame);
    }
    // declare toolbar grip for menu bar modes
    new FXToolBarGrip(modes, modes, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
    // build menu bar for interal
    myToolBarShellIntervalBar = new FXToolBarShell(myGNEApp, GUIDesignToolBar);
    intervalBar = new FXMenuBar(myGNEApp->getTopDock(), myToolBarShellIntervalBar, GUIDesignToolBarRaisedNext);
    // declare toolbar grip for menu bar modes
    new FXToolBarGrip(intervalBar, intervalBar, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
    // create menu bars
    superModes->create();
    saveElements->create();
    navigation->create();
    modes->create();
    intervalBar->create();
    // create toolbar shells
    myToolBarShellSuperModes->create();
    myToolBarShellSaveElements->create();
    myToolBarShellNavigation->create();
    myToolBarShellModes->create();
    myToolBarShellIntervalBar->create();
    // recalc top dop after creating elements
    myGNEApp->getTopDock()->recalc();
}


void
GNEApplicationWindowHelper::ToolbarsGrip::destroyParentToolbarsGrips() {
    // delete Menu bars
    delete superModes;
    delete saveElements;
    delete navigation;
    delete modes;
    delete intervalBar;
    // also delete toolbar shells to avoid floating windows
    delete myToolBarShellSuperModes;
    delete myToolBarShellSaveElements;
    delete myToolBarShellNavigation;
    delete myToolBarShellModes;
    delete myToolBarShellIntervalBar;
    // recalc top dop after deleting elements
    myGNEApp->getTopDock()->recalc();
}

// ===========================================================================
// GNEApplicationWindowHelper::MenuBarFile method definitions
// ===========================================================================

GNEApplicationWindowHelper::MenuBarFile::MenuBarFile(GNEApplicationWindow* GNEApp) :
    myRecentNetsAndConfigs(GNEApp->getApp(), "nets"),
    myGNEApp(GNEApp)
{ }


void
GNEApplicationWindowHelper::MenuBarFile::buildRecentFiles(FXMenuPane* fileMenu) {
    FXMenuSeparator* sep1 = new FXMenuSeparator(fileMenu);
    sep1->setSelector(FXRecentFiles::ID_ANYFILES);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenu, "", &myRecentNetsAndConfigs, FXRecentFiles::ID_FILE_1);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenu, "", &myRecentNetsAndConfigs, FXRecentFiles::ID_FILE_2);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenu, "", &myRecentNetsAndConfigs, FXRecentFiles::ID_FILE_3);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenu, "", &myRecentNetsAndConfigs, FXRecentFiles::ID_FILE_4);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenu, "", &myRecentNetsAndConfigs, FXRecentFiles::ID_FILE_5);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenu, "", &myRecentNetsAndConfigs, FXRecentFiles::ID_FILE_6);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenu, "", &myRecentNetsAndConfigs, FXRecentFiles::ID_FILE_7);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenu, "", &myRecentNetsAndConfigs, FXRecentFiles::ID_FILE_8);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenu, "", &myRecentNetsAndConfigs, FXRecentFiles::ID_FILE_9);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenu, "", &myRecentNetsAndConfigs, FXRecentFiles::ID_FILE_10);
    GUIDesigns::buildFXMenuCommand(fileMenu, "Cl&ear Recent Files", nullptr, &myRecentNetsAndConfigs, FXRecentFiles::ID_CLEAR);
    GUIDesigns::buildFXMenuCommand(fileMenu, "No Recent Files", nullptr, &myRecentNetsAndConfigs, FXRecentNetworks::ID_NOFILES);
    myRecentNetsAndConfigs.setTarget(myGNEApp);
    myRecentNetsAndConfigs.setSelector(MID_RECENTFILE);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::FileMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::FileMenuCommands::FileMenuCommands(GNEApplicationWindow* GNEApp) :
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindowHelper::FileMenuCommands::buildFileMenuCommands(FXMenuPane* fileMenu, FXMenuPane* fileMenuTLS, FXMenuPane* fileMenuEdgeTypes,
        FXMenuPane* fileMenuAdditionals, FXMenuPane* fileMenuDemandElements, FXMenuPane* fileMenuDataElements) {
    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           "&New Network", "Ctrl+N", "Create a new network.",
                                           GUIIconSubSys::getIcon(GUIIcon::NEW_NET), myGNEApp, MID_HOTKEY_CTRL_N_NEWNETWORK);
    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           "New Window", "Ctrl+Shift+N", "Open a new netedit window.",
                                           nullptr, myGNEApp, MID_HOTKEY_CTRL_SHIFT_N_NEWWINDOW);
    new FXMenuSeparator(fileMenu);
    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           "&Open Network...", "Ctrl+O", "Open a SUMO network.",
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_NET), myGNEApp, MID_OPEN_NETWORK);
    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           "Open Netconvert Configura&tion...", "Ctrl+Shift+O", "Open a configuration file with NETCONVERT options.",
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_CONFIG), myGNEApp, MID_OPEN_CONFIG);
    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           "Import &Foreign Network...", "", "Import a foreign network such as OSM.",
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_NET), myGNEApp, MID_GNE_TOOLBARFILE_OPENFOREIGN);
    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           "&Reload", "Ctrl+R", "Reload the network.",
                                           GUIIconSubSys::getIcon(GUIIcon::RELOAD), myGNEApp, MID_HOTKEY_CTRL_R_RELOAD);
    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           "&Save Network", "Ctrl+S", "Save the network.",
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE), myGNEApp, MID_HOTKEY_CTRL_S_STOPSIMULATION_SAVENETWORK);
    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           "Save Net&work As...", "Ctrl+Shift+S", "Save the network to another file.",
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE), myGNEApp, MID_HOTKEY_CTRL_SHIFT_S_SAVENETWORK_AS);
    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           "Save Plain XM&L...", "Ctrl+L", "Save plain XML representation the network.",
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE), myGNEApp, MID_HOTKEY_CTRL_L_SAVEASPLAINXML);
    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           "Save &Joined Junctions...", "", "Save log of joined junctions (allows reproduction of joins).",
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE), myGNEApp, MID_GNE_SAVEJOINEDJUNCTIONS);
    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           "Save All Elements", "", "Save all elements (network, additional, demand and data)",
                                           GUIIconSubSys::getIcon(GUIIcon::SAVEALLELEMENTS), myGNEApp, MID_GNE_SAVEALLELEMENTS);
    // create TLS menu options
    GUIDesigns::buildFXMenuCommandShortcut(fileMenuTLS,
                                           "Load TLS Programs...", "Ctrl+K", "Load programs for traffic lights in the current net.",
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_TLSPROGRAMS), myGNEApp, MID_HOTKEY_CTRL_K_OPENTLSPROGRAMS);
    reloadTLSPrograms = GUIDesigns::buildFXMenuCommandShortcut(fileMenuTLS,
                        "Reload TLS Programs", "", "Reload TLS Programs.",
                        GUIIconSubSys::getIcon(GUIIcon::RELOAD), myGNEApp, MID_GNE_TOOLBARFILE_RELOAD_TLSPROGRAMS);
    saveTLSPrograms = GUIDesigns::buildFXMenuCommandShortcut(fileMenuTLS,
                      "Save TLS Programs", "Ctrl+Shift+K", "Save all traffic light programs of the current net.",
                      GUIIconSubSys::getIcon(GUIIcon::SAVE), myGNEApp, MID_HOTKEY_CTRL_SHIFT_K_SAVETLS);
    saveTLSPrograms->disable();
    GUIDesigns::buildFXMenuCommandShortcut(fileMenuTLS,
                                           "Save TLS Programs As...", "", "Save all traffic light programs of the current net to another file.",
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE), myGNEApp, MID_GNE_TOOLBARFILE_SAVETLSPROGRAMS_AS);
    TLSMenuCascade = new FXMenuCascade(fileMenu, "Traffic Lights", GUIIconSubSys::getIcon(GUIIcon::MODETLS), fileMenuTLS);
    // create edgeTypes menu options
    GUIDesigns::buildFXMenuCommandShortcut(fileMenuEdgeTypes,
                                           "Load Edge Types...", "Ctrl+H", "Load edge types in the current net.",
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_TLSPROGRAMS), myGNEApp, MID_HOTKEY_CTRL_H_OPENEDGETYPES);
    reloadEdgeTypes = GUIDesigns::buildFXMenuCommandShortcut(fileMenuEdgeTypes,
                      "Reload edge types", "", "Reload edge types.",
                      GUIIconSubSys::getIcon(GUIIcon::RELOAD), myGNEApp, MID_GNE_TOOLBARFILE_RELOAD_EDGETYPES);
    saveEdgeTypes = GUIDesigns::buildFXMenuCommandShortcut(fileMenuEdgeTypes,
                    "Save Edge Types", "Ctrl+Shift+H", "Save edge types of the current net.",
                    GUIIconSubSys::getIcon(GUIIcon::SAVE), myGNEApp, MID_HOTKEY_CTRL_SHIFT_H_SAVEEDGETYPES);
    saveEdgeTypes->disable();
    GUIDesigns::buildFXMenuCommandShortcut(fileMenuEdgeTypes,
                                           "Save Edge Types As...", "", "Save edge types of the current net to another file.",
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE), myGNEApp, MID_GNE_TOOLBARFILE_SAVEEDGETYPES_AS);
    edgeTypesMenuCascade = new FXMenuCascade(fileMenu, "Edge Types", GUIIconSubSys::getIcon(GUIIcon::MODECREATEEDGE), fileMenuEdgeTypes);
    // create Additionals menu options
    GUIDesigns::buildFXMenuCommandShortcut(fileMenuAdditionals,
                                           "Load Additionals...", "Ctrl+A", "Load additionals and shapes.",
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_ADDITIONALS), myGNEApp, MID_HOTKEY_CTRL_A_STARTSIMULATION_OPENADDITIONALS);
    reloadAdditionals = GUIDesigns::buildFXMenuCommandShortcut(fileMenuAdditionals,
                        "Reload Additionals", "", "Reload additionals.",
                        GUIIconSubSys::getIcon(GUIIcon::RELOAD), myGNEApp, MID_GNE_TOOLBARFILE_RELOAD_ADDITIONALS);
    reloadAdditionals->disable();
    saveAdditionals = GUIDesigns::buildFXMenuCommandShortcut(fileMenuAdditionals,
                      "Save Additionals", "Ctrl+Shift+A", "Save additionals and shapes.",
                      GUIIconSubSys::getIcon(GUIIcon::SAVEADDITIONALELEMENTS), myGNEApp, MID_HOTKEY_CTRL_SHIFT_A_SAVEADDITIONALS);
    saveAdditionals->disable();
    saveAdditionalsAs = GUIDesigns::buildFXMenuCommandShortcut(fileMenuAdditionals,
                        "Save Additionals As...", "", "Save additional elements to another file.",
                        GUIIconSubSys::getIcon(GUIIcon::SAVEADDITIONALELEMENTS), myGNEApp, MID_GNE_TOOLBARFILE_SAVEADDITIONALS_AS);
    saveAdditionalsAs->disable();
    additionalMenuCascade = new FXMenuCascade(fileMenu, "Additionals and Shapes", GUIIconSubSys::getIcon(GUIIcon::MODEADDITIONAL), fileMenuAdditionals);
    // create DemandElements menu options
    GUIDesigns::buildFXMenuCommandShortcut(fileMenuDemandElements,
                                           "Load Demand Elements...", "Ctrl+D", "Load demand elements.",
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_ADDITIONALS), myGNEApp, MID_HOTKEY_CTRL_D_SINGLESIMULATIONSTEP_OPENDEMANDELEMENTS);
    reloadDemandElements = GUIDesigns::buildFXMenuCommandShortcut(fileMenuDemandElements,
                           "Reload Demand Elements", "", "Reload demand elements.",
                           GUIIconSubSys::getIcon(GUIIcon::RELOAD), myGNEApp, MID_GNE_TOOLBARFILE_RELOAD_DEMANDELEMENTS);
    reloadDemandElements->disable();
    saveDemandElements = GUIDesigns::buildFXMenuCommandShortcut(fileMenuDemandElements,
                         "Save Demand Elements", "Ctrl+Shift+D", "Save demand elements.",
                         GUIIconSubSys::getIcon(GUIIcon::SAVEDEMANDELEMENTS), myGNEApp, MID_HOTKEY_CTRL_SHIFT_D_SAVEDEMANDELEMENTS);
    saveDemandElements->disable();
    saveDemandElementsAs = GUIDesigns::buildFXMenuCommandShortcut(fileMenuDemandElements,
                           "Save Demand Elements As...", "", "Save demand elements to another file.",
                           GUIIconSubSys::getIcon(GUIIcon::SAVEDEMANDELEMENTS), myGNEApp, MID_GNE_TOOLBARFILE_SAVEDEMAND_AS);
    saveDemandElementsAs->disable();
    demandMenuCascade = new FXMenuCascade(fileMenu, "Demand Elements", GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDEMAND), fileMenuDemandElements);
    // create DataElements menu options
    GUIDesigns::buildFXMenuCommandShortcut(fileMenuDataElements,
                                           "Load Data Elements...", "Ctrl+B", "Load data elements.",
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_ADDITIONALS), myGNEApp, MID_HOTKEY_CTRL_B_EDITBREAKPOINT_OPENDATAELEMENTS);
    reloadDataElements = GUIDesigns::buildFXMenuCommandShortcut(fileMenuDataElements,
                         "Reload Data Elements", "", "Reload data elements.",
                         GUIIconSubSys::getIcon(GUIIcon::RELOAD), myGNEApp, MID_GNE_TOOLBARFILE_RELOAD_DATAELEMENTS);
    reloadDataElements->disable();
    saveDataElements = GUIDesigns::buildFXMenuCommandShortcut(fileMenuDataElements,
                       "Save Data Elements", "Ctrl+Shift+B", "Save data elements.",
                       GUIIconSubSys::getIcon(GUIIcon::SAVEDATAELEMENTS), myGNEApp, MID_HOTKEY_CTRL_SHIFT_B_SAVEDATAELEMENTS);
    saveDataElements->disable();
    saveDataElementsAs = GUIDesigns::buildFXMenuCommandShortcut(fileMenuDataElements,
                         "Save Data Elements As...", "", "Save data elements to another file.",
                         GUIIconSubSys::getIcon(GUIIcon::SAVEDATAELEMENTS), myGNEApp, MID_GNE_TOOLBARFILE_SAVEDATA_AS);
    saveDataElementsAs->disable();
    dataMenuCascade = new FXMenuCascade(fileMenu, "Data Elements", GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDATA), fileMenuDataElements);
    // close network
    new FXMenuSeparator(fileMenu);
    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           "Close", "Ctrl+W", "Close the network.",
                                           GUIIconSubSys::getIcon(GUIIcon::CLOSE), myGNEApp, MID_HOTKEY_CTRL_W_CLOSESIMULATION);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::ModesMenuCommands::CommonMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::ModesMenuCommands::CommonMenuCommands::CommonMenuCommands(const ModesMenuCommands* modesMenuCommandsParent) :
    deleteMode(nullptr),
    inspectMode(nullptr),
    selectMode(nullptr),
    myModesMenuCommandsParent(modesMenuCommandsParent) {
}


void
GNEApplicationWindowHelper::ModesMenuCommands::CommonMenuCommands::buildCommonMenuCommands(FXMenuPane* modesMenu) {
    // build every FXMenuCommand giving it a shortcut
    inspectMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                  "&Inspect mode", "I", "Inspect elements and change their attributes.",
                  GUIIconSubSys::getIcon(GUIIcon::MODEINSPECT), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_I_MODE_INSPECT);
    deleteMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                 "&Delete mode", "D", "Delete elements.",
                 GUIIconSubSys::getIcon(GUIIcon::MODEDELETE), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_D_MODE_DELETE);
    selectMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                 "&Select mode", "S", "Select elements.",
                 GUIIconSubSys::getIcon(GUIIcon::MODESELECT), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_S_MODE_SELECT);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::ModesMenuCommands::NetworkMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::ModesMenuCommands::NetworkMenuCommands::NetworkMenuCommands(const ModesMenuCommands* modesMenuCommandsParent) :
    createEdgeMode(nullptr),
    moveMode(nullptr),
    connectMode(nullptr),
    prohibitionMode(nullptr),
    TLSMode(nullptr),
    additionalMode(nullptr),
    crossingMode(nullptr),
    TAZMode(nullptr),
    shapeMode(nullptr),
    wireMode(nullptr),
    myModesMenuCommandsParent(modesMenuCommandsParent) {
}


void
GNEApplicationWindowHelper::ModesMenuCommands::NetworkMenuCommands::showNetworkMenuCommands() {
    createEdgeMode->show();
    moveMode->show();
    connectMode->show();
    prohibitionMode->show();
    TLSMode->show();
    additionalMode->show();
    crossingMode->show();
    TAZMode->show();
    shapeMode->show();
    wireMode->show();
}


void
GNEApplicationWindowHelper::ModesMenuCommands::NetworkMenuCommands::hideNetworkMenuCommands() {
    createEdgeMode->hide();
    moveMode->hide();
    connectMode->hide();
    prohibitionMode->hide();
    TLSMode->hide();
    additionalMode->hide();
    crossingMode->hide();
    TAZMode->hide();
    shapeMode->hide();
    wireMode->hide();
}


void
GNEApplicationWindowHelper::ModesMenuCommands::NetworkMenuCommands::buildNetworkMenuCommands(FXMenuPane* modesMenu) {
    // build every FXMenuCommand giving it a shortcut
    moveMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
               "&Move mode", "M", "Move elements.",
               GUIIconSubSys::getIcon(GUIIcon::MODEMOVE), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_M_MODE_MOVE);
    createEdgeMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                     "&Edge mode", "E", "Create junction and edges.",
                     GUIIconSubSys::getIcon(GUIIcon::MODECREATEEDGE), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_E_MODE_EDGE_EDGEDATA);
    connectMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                  "&Connection mode", "C", "Modes connections between lanes.",
                  GUIIconSubSys::getIcon(GUIIcon::MODECONNECTION), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_C_MODE_CONNECT_PERSONPLAN);
    prohibitionMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                      "Pro&hibition mode", "H", "Modes connection prohibitions.",
                      GUIIconSubSys::getIcon(GUIIcon::MODEPROHIBITION), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_H_MODE_PROHIBITION_CONTAINERPLAN);
    TLSMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
              "&Traffic light mode", "T", "Modes traffic lights over junctions.",
              GUIIconSubSys::getIcon(GUIIcon::MODETLS), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_T_MODE_TLS_TYPE);
    additionalMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                     "&Additional mode", "A", "Create additional elements.",
                     GUIIconSubSys::getIcon(GUIIcon::MODEADDITIONAL), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_A_MODE_ADDITIONAL_STOP);
    crossingMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                   "C&rossing mode", "R", "Create crossings between edges.",
                   GUIIconSubSys::getIcon(GUIIcon::MODECROSSING), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_R_MODE_CROSSING_ROUTE_EDGERELDATA);
    TAZMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
              "TA&Z mode", "Z", "Create Traffic Assignment Zones.",
              GUIIconSubSys::getIcon(GUIIcon::MODETAZ), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_Z_MODE_TAZ_TAZREL);
    shapeMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                "&POI-Poly mode", "P", "Create Points-Of-Interest and polygons.",
                GUIIconSubSys::getIcon(GUIIcon::MODESHAPE), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_P_MODE_POLYGON_PERSON);
    wireMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
               "&Wire mode", "W", "Create wires.",
               GUIIconSubSys::getIcon(GUIIcon::MODEWIRE), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_W_MODE_WIRE);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::ModesMenuCommands::DemandMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::ModesMenuCommands::DemandMenuCommands::DemandMenuCommands(const ModesMenuCommands* modesMenuCommandsParent) :
    moveMode(nullptr),
    routeMode(nullptr),
    vehicleMode(nullptr),
    typeMode(nullptr),
    stopMode(nullptr),
    personMode(nullptr),
    personPlanMode(nullptr),
    containerMode(nullptr),
    containerPlanMode(nullptr),
    myModesMenuCommandsParent(modesMenuCommandsParent) {
}


void
GNEApplicationWindowHelper::ModesMenuCommands::DemandMenuCommands::showDemandMenuCommands() {
    moveMode->show();
    routeMode->show();
    vehicleMode->show();
    typeMode->show();
    stopMode->show();
    personMode->show();
    personPlanMode->show();
    containerMode->show();
    containerPlanMode->show();
}


void
GNEApplicationWindowHelper::ModesMenuCommands::DemandMenuCommands::hideDemandMenuCommands() {
    moveMode->hide();
    routeMode->hide();
    vehicleMode->hide();
    typeMode->hide();
    stopMode->hide();
    personMode->hide();
    personPlanMode->hide();
    containerMode->hide();
    containerPlanMode->hide();
}


void
GNEApplicationWindowHelper::ModesMenuCommands::DemandMenuCommands::buildDemandMenuCommands(FXMenuPane* modesMenu) {
    // build every FXMenuCommand giving it a shortcut
    moveMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
               "&Move mode", "M", "Move elements.",
               GUIIconSubSys::getIcon(GUIIcon::MODEMOVE), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_M_MODE_MOVE);
    routeMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                "Route mode", "R", "Create Routes.",
                GUIIconSubSys::getIcon(GUIIcon::MODEROUTE), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_R_MODE_CROSSING_ROUTE_EDGERELDATA);
    vehicleMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                  "Vehicle mode", "V", "Create vehicles.",
                  GUIIconSubSys::getIcon(GUIIcon::MODEVEHICLE), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_V_MODE_VEHICLE);
    typeMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
               "Type mode", "T", "Create types (vehicles, person an containers).",
               GUIIconSubSys::getIcon(GUIIcon::MODETYPE), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_T_MODE_TLS_TYPE);
    stopMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
               "Stop mode", "A", "Create stops.",
               GUIIconSubSys::getIcon(GUIIcon::MODESTOP), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_A_MODE_ADDITIONAL_STOP);
    personMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                 "Person mode", "P", "Create persons.",
                 GUIIconSubSys::getIcon(GUIIcon::MODEPERSON), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_P_MODE_POLYGON_PERSON);
    personPlanMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                     "Person plan mode", "C", "Create person plans.",
                     GUIIconSubSys::getIcon(GUIIcon::MODEPERSONPLAN), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_C_MODE_CONNECT_PERSONPLAN);
    containerMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                    "Container mode", "G", "Create containers.",
                    GUIIconSubSys::getIcon(GUIIcon::MODECONTAINER), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_G_MODE_CONTAINER);
    containerPlanMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                        "Container plan mode", "H", "Create container plans.",
                        GUIIconSubSys::getIcon(GUIIcon::MODECONTAINERPLAN), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_H_MODE_PROHIBITION_CONTAINERPLAN);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::ModesMenuCommands::DataMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::ModesMenuCommands::DataMenuCommands::DataMenuCommands(const ModesMenuCommands* modesMenuCommandsParent) :
    edgeData(nullptr),
    edgeRelData(nullptr),
    TAZRelData(nullptr),
    myModesMenuCommandsParent(modesMenuCommandsParent) {
}


void
GNEApplicationWindowHelper::ModesMenuCommands::DataMenuCommands::showDataMenuCommands() {
    edgeData->show();
    edgeRelData->show();
    TAZRelData->show();
}


void
GNEApplicationWindowHelper::ModesMenuCommands::DataMenuCommands::hideDataMenuCommands() {
    edgeData->hide();
    edgeRelData->hide();
    TAZRelData->hide();
}


void
GNEApplicationWindowHelper::ModesMenuCommands::DataMenuCommands::buildDataMenuCommands(FXMenuPane* modesMenu) {
    // build every FXMenuCommand giving it a shortcut
    edgeData = GUIDesigns::buildFXMenuCommand(modesMenu,
               "EdgeData Mode\tE\tCreate edgeData elements.",
               GUIIconSubSys::getIcon(GUIIcon::MODEEDGEDATA), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_E_MODE_EDGE_EDGEDATA);
    edgeRelData = GUIDesigns::buildFXMenuCommand(modesMenu,
                  "EdgeRelation Mode\tR\tCreate edgeRelation elements.",
                  GUIIconSubSys::getIcon(GUIIcon::MODEEDGERELDATA), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_R_MODE_CROSSING_ROUTE_EDGERELDATA);
    TAZRelData = GUIDesigns::buildFXMenuCommand(modesMenu,
                 "TAZRelation Mode\tZ\tCreate TAZRelation elements.",
                 GUIIconSubSys::getIcon(GUIIcon::MODETAZRELDATA), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_Z_MODE_TAZ_TAZREL);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::ModesMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::ModesMenuCommands::ModesMenuCommands(GNEApplicationWindow* GNEApp) :
    commonMenuCommands(this),
    networkMenuCommands(this),
    demandMenuCommands(this),
    dataMenuCommands(this),
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindowHelper::ModesMenuCommands::buildModesMenuCommands(FXMenuPane* modesMenu) {
    // build Common modes commands and hide it
    commonMenuCommands.buildCommonMenuCommands(modesMenu);
    // build Network modes commands and hide it
    networkMenuCommands.buildNetworkMenuCommands(modesMenu);
    networkMenuCommands.hideNetworkMenuCommands();
    // build Demand Modes commands
    demandMenuCommands.buildDemandMenuCommands(modesMenu);
    demandMenuCommands.hideDemandMenuCommands();
    // build Data Modes commands
    dataMenuCommands.buildDataMenuCommands(modesMenu);
    dataMenuCommands.hideDataMenuCommands();
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::EditMenuCommands::NetworkViewOptions - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::EditMenuCommands::NetworkViewOptions::NetworkViewOptions(GNEApplicationWindow* GNEApp) :
    menuCheckToggleGrid(nullptr),
    menuCheckToggleDrawJunctionShape(nullptr),
    menuCheckDrawSpreadVehicles(nullptr),
    menuCheckShowDemandElements(nullptr),
    menuCheckSelectEdges(nullptr),
    menuCheckShowConnections(nullptr),
    menuCheckHideConnections(nullptr),
    menuCheckShowAdditionalSubElements(nullptr),
    menuCheckShowTAZElements(nullptr),
    menuCheckExtendSelection(nullptr),
    menuCheckChangeAllPhases(nullptr),
    menuCheckWarnAboutMerge(nullptr),
    menuCheckShowJunctionBubble(nullptr),
    menuCheckMoveElevation(nullptr),
    menuCheckChainEdges(nullptr),
    menuCheckAutoOppositeEdge(nullptr),
    separator(nullptr),
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindowHelper::EditMenuCommands::NetworkViewOptions::buildNetworkViewOptionsMenuChecks(FXMenuPane* editMenu) {
    // create menu checks
    menuCheckToggleGrid = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                          "Show grid", "Ctrl+G or Alt+1", "",
                          GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_TOGGLEGRID),
                          myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_TOGGLEGRID);

    menuCheckToggleDrawJunctionShape = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                       "Draw junction shape", "Ctrl+J or Alt+2", "",
                                       GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_TOGGLEDRAWJUNCTIONSHAPE),
                                       myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE);

    menuCheckDrawSpreadVehicles = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                  "Draw vehicles spread in lane or in depart position", "Alt+3", "",
                                  GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_SPREADVEHICLE),
                                  myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_DRAWSPREADVEHICLES);

    menuCheckShowDemandElements = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                  "Show demand elements", "Alt+4", "",
                                  GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_SHOWDEMANDELEMENTS),
                                  myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_SHOWDEMANDELEMENTS);

    menuCheckSelectEdges = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                           "Clicking should select edges or lanes", "Alt+5", "",
                           GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_SELECTEDGES),
                           myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_SELECTEDGES);

    menuCheckShowConnections = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                               "Show connections over junctions", "Alt+6", "",
                               GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_SHOWCONNECTIONS),
                               myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_SHOWCONNECTIONS);

    menuCheckHideConnections = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                               "Hide connections", "Alt+7", "",
                               GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_HIDECONNECTIONS),
                               myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_HIDECONNECTIONS);

    menuCheckShowAdditionalSubElements = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                         "Show sub-additional elements", "Alt+8", "",
                                         GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_SHOWSUBADDITIONALS),
                                         myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_SHOWSUBADDITIONALS);

    menuCheckShowTAZElements = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                               "Show TAZ elements", "Alt+9", "",
                               GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_SHOWTAZELEMENTS),
                               myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_SHOWTAZELEMENTS);

    menuCheckExtendSelection = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                               "Selecting multiple edges automatically select their junctions", "Alt+10", "",
                               GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_AUTOSELECTJUNCTIONS),
                               myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_EXTENDSELECTION);

    menuCheckChangeAllPhases = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                               "Clicking should apply state changes to all phases of TLS plan", "Alt+5", "",
                               GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_APPLYTOALLPHASES),
                               myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_CHANGEALLPHASES);

    menuCheckWarnAboutMerge = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                              "Ask for confirmation before merging junction", "Alt+5", "",
                              GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_ASKFORMERGE),
                              myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_ASKFORMERGE);

    menuCheckShowJunctionBubble = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                  "Show bubbles over junctions shapes", "Alt+6", "",
                                  GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_BUBBLES),
                                  myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_SHOWBUBBLES);

    menuCheckMoveElevation = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                             "Apply mouse movement to elevation", "Alt+7", "",
                             GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_ELEVATION),
                             myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_MOVEELEVATION);

    menuCheckChainEdges = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                          "Create consecutive edges", "Alt+5", "",
                          GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_CHAIN),
                          myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_CHAINEDGES);

    menuCheckAutoOppositeEdge = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                "Create an edge in the opposite direction", "Alt+6", "",
                                GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_TWOWAY),
                                myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_AUTOOPPOSITEEDGES);

    // build separator
    separator = new FXMenuSeparator(editMenu);
}


void
GNEApplicationWindowHelper::EditMenuCommands::NetworkViewOptions::hideNetworkViewOptionsMenuChecks() {
    menuCheckToggleGrid->hide();
    menuCheckToggleDrawJunctionShape->hide();
    menuCheckDrawSpreadVehicles->hide();
    menuCheckShowDemandElements->hide();
    menuCheckSelectEdges->hide();
    menuCheckShowConnections->hide();
    menuCheckHideConnections->hide();
    menuCheckShowAdditionalSubElements->hide();
    menuCheckShowTAZElements->hide();
    menuCheckExtendSelection->hide();
    menuCheckChangeAllPhases->hide();
    menuCheckWarnAboutMerge->hide();
    menuCheckShowJunctionBubble->hide();
    menuCheckMoveElevation->hide();
    menuCheckChainEdges->hide();
    menuCheckAutoOppositeEdge->hide();
    separator->hide();
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::DemandViewOptions - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::EditMenuCommands::DemandViewOptions::DemandViewOptions(GNEApplicationWindow* GNEApp) :
    menuCheckToggleGrid(nullptr),
    menuCheckToggleDrawJunctionShape(nullptr),
    menuCheckDrawSpreadVehicles(nullptr),
    menuCheckHideShapes(nullptr),
    menuCheckShowAllTrips(nullptr),
    menuCheckHideNonInspectedDemandElements(nullptr),
    menuCheckShowOverlappedRoutes(nullptr),
    menuCheckShowAllPersonPlans(nullptr),
    menuCheckLockPerson(nullptr),
    menuCheckShowAllContainerPlans(nullptr),
    menuCheckLockContainer(nullptr),
    separator(nullptr),
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindowHelper::EditMenuCommands::DemandViewOptions::buildDemandViewOptionsMenuChecks(FXMenuPane* editMenu) {
    // create menu checks
    menuCheckToggleGrid = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                          "Show grid", "Ctrl+G or Alt+1", "",
                          GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_TOGGLEGRID),
                          myGNEApp, MID_GNE_DEMANDVIEWOPTIONS_SHOWGRID);

    menuCheckToggleDrawJunctionShape = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                       "Draw junction shape", "Ctrl+J or Alt+2", "",
                                       GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_TOGGLEDRAWJUNCTIONSHAPE),
                                       myGNEApp, MID_GNE_DEMANDVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE);

    menuCheckDrawSpreadVehicles = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                  "Draw vehicles spread/depart position", "Alt+3", "",
                                  GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_SPREADVEHICLE),
                                  myGNEApp, MID_GNE_DEMANDVIEWOPTIONS_DRAWSPREADVEHICLES);

    menuCheckHideShapes = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                          "Show shapes", "Alt+4", "",
                          GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_HIDESHAPES),
                          myGNEApp, MID_GNE_DEMANDVIEWOPTIONS_HIDESHAPES);

    menuCheckShowAllTrips = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                            "Show all trips", "Alt+5", "",
                            GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_SHOWTRIPS),
                            myGNEApp, MID_GNE_DEMANDVIEWOPTIONS_SHOWTRIPS);

    menuCheckShowAllPersonPlans = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                  "Show all person plans", "Alt+6", "",
                                  GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_SHOWPERSONPLANS),
                                  myGNEApp, MID_GNE_DEMANDVIEWOPTIONS_SHOWALLPERSONPLANS);

    menuCheckLockPerson = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                          "Lock selected person", "Alt+7", "",
                          GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_LOCKPERSON),
                          myGNEApp, MID_GNE_DEMANDVIEWOPTIONS_LOCKPERSON);

    menuCheckShowAllContainerPlans = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                     "Show all container plans", "Alt+6", "",
                                     GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_SHOWCONTAINERPLANS),
                                     myGNEApp, MID_GNE_DEMANDVIEWOPTIONS_SHOWALLCONTAINERPLANS);

    menuCheckLockContainer = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                             "Lock selected container", "Alt+7", "",
                             GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_LOCKCONTAINER),
                             myGNEApp, MID_GNE_DEMANDVIEWOPTIONS_LOCKCONTAINER);

    menuCheckHideNonInspectedDemandElements = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
            "Show non-inspected demand elements", "Alt+8", "",
            GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_HIDENONINSPECTEDDEMANDELEMENTS),
            myGNEApp, MID_GNE_DEMANDVIEWOPTIONS_HIDENONINSPECTED);

    menuCheckShowOverlappedRoutes = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                    "Show number of overlapped routes", "Alt+9", "",
                                    GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_SHOWOVERLAPPEDROUTES),
                                    myGNEApp, MID_GNE_DEMANDVIEWOPTIONS_SHOWOVERLAPPEDROUTES);

    // build separator
    separator = new FXMenuSeparator(editMenu);
}


void
GNEApplicationWindowHelper::EditMenuCommands::DemandViewOptions::hideDemandViewOptionsMenuChecks() {
    menuCheckToggleGrid->hide();
    menuCheckToggleDrawJunctionShape->hide();
    menuCheckDrawSpreadVehicles->hide();
    menuCheckHideShapes->hide();
    menuCheckShowAllTrips->hide();
    menuCheckHideNonInspectedDemandElements->hide();
    menuCheckShowOverlappedRoutes->hide();
    menuCheckShowAllPersonPlans->hide();
    menuCheckLockPerson->hide();
    menuCheckShowAllContainerPlans->hide();
    menuCheckLockContainer->hide();
    separator->hide();
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::EditMenuCommands::DataViewOptions - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::EditMenuCommands::DataViewOptions::DataViewOptions(GNEApplicationWindow* GNEApp) :
    menuCheckToggleDrawJunctionShape(nullptr),
    menuCheckShowAdditionals(nullptr),
    menuCheckShowShapes(nullptr),
    menuCheckShowDemandElements(nullptr),
    menuCheckToogleTAZRelDrawing(nullptr),
    menuCheckToogleTAZDrawFill(nullptr),
    menuCheckToogleTAZRelOnlyFrom(nullptr),
    menuCheckToogleTAZRelOnlyTo(nullptr),
    separator(nullptr),
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindowHelper::EditMenuCommands::DataViewOptions::buildDataViewOptionsMenuChecks(FXMenuPane* editMenu) {
    // create menu checks
    menuCheckToggleDrawJunctionShape = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                       "Draw junction shape", "Ctrl+J or Alt+1", "",
                                       GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_TOGGLEDRAWJUNCTIONSHAPE),
                                       myGNEApp, MID_GNE_DEMANDVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE);

    menuCheckShowAdditionals = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                               "Show additionals", "Alt+2", "",
                               GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_SHOWADDITIONALS),
                               myGNEApp, MID_GNE_DATAVIEWOPTIONS_SHOWADDITIONALS);

    menuCheckShowShapes = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                          "Show shapes", "Alt+3", "",
                          GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_SHOWSHAPES),
                          myGNEApp, MID_GNE_DATAVIEWOPTIONS_SHOWSHAPES);

    menuCheckShowDemandElements = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                  "Show demand elements", "Alt+4", "",
                                  GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_SHOWDEMANDELEMENTS),
                                  myGNEApp, MID_GNE_DATAVIEWOPTIONS_SHOWDEMANDELEMENTS);

    menuCheckToogleTAZRelDrawing = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                   "Draw TAZRel from center", "Alt+5", "",
                                   GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_TAZRELDRAWING),
                                   myGNEApp, MID_GNE_DATAVIEWOPTIONS_TAZRELDRAWING);

    menuCheckToogleTAZDrawFill = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                 "Draw TAZ fill", "Alt+6", "",
                                 GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_TAZDRAWFILL),
                                 myGNEApp, MID_GNE_DATAVIEWOPTIONS_TAZDRAWFILL);

    menuCheckToogleTAZRelOnlyFrom = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                    "Only draw TAZRel from", "Alt+6", "",
                                    GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_TAZRELONLYFROM),
                                    myGNEApp, MID_GNE_DATAVIEWOPTIONS_TAZRELONLYFROM);

    menuCheckToogleTAZRelOnlyTo = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                  "Only draw TAZRel to", "Alt+7", "",
                                  GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_TAZRELONLYTO),
                                  myGNEApp, MID_GNE_DATAVIEWOPTIONS_TAZRELONLYTO);
    // build separator
    separator = new FXMenuSeparator(editMenu);
}


void
GNEApplicationWindowHelper::EditMenuCommands::DataViewOptions::hideDataViewOptionsMenuChecks() {
    menuCheckShowAdditionals->hide();
    menuCheckToggleDrawJunctionShape->hide();
    menuCheckShowShapes->hide();
    menuCheckShowDemandElements->hide();
    menuCheckToogleTAZRelDrawing->hide();
    menuCheckToogleTAZDrawFill->hide();
    menuCheckToogleTAZRelOnlyFrom->hide();
    menuCheckToogleTAZRelOnlyTo->hide();
    separator->hide();
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::EditMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::EditMenuCommands::EditMenuCommands(GNEApplicationWindow* GNEApp) :
    undoLastChange(nullptr),
    redoLastChange(nullptr),
    openUndolistDialog(nullptr),
    networkViewOptions(GNEApp),
    demandViewOptions(GNEApp),
    dataViewOptions(GNEApp),
    editViewScheme(nullptr),
    editViewPort(nullptr),
    clearFrontElement(nullptr),
    loadAdditionalsInSUMOGUI(nullptr),
    loadDemandInSUMOGUI(nullptr),
    openInSUMOGUI(nullptr),
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindowHelper::EditMenuCommands::buildUndoRedoMenuCommands(FXMenuPane* editMenu) {
    // build undo/redo command
    undoLastChange = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
                     "Undo", "Ctrl+Z", "Undo the last change.",
                     GUIIconSubSys::getIcon(GUIIcon::UNDO), myGNEApp, MID_HOTKEY_CTRL_Z_UNDO);
    redoLastChange = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
                     "Redo", "Ctrl+Y", "Redo the last change.",
                     GUIIconSubSys::getIcon(GUIIcon::REDO), myGNEApp, MID_HOTKEY_CTRL_Y_REDO);
    openUndolistDialog = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
                         "Show undo/redo history", "", "Open undo/redo history dialog.",
                         GUIIconSubSys::getIcon(GUIIcon::UNDOLIST), myGNEApp, MID_GNE_UNDOLISTDIALOG);
}


void
GNEApplicationWindowHelper::EditMenuCommands::buildViewMenuCommands(FXMenuPane* editMenu) {
    // build rest of menu commands
    editViewScheme = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
                     "Edit Visualisation", "F9", "Opens a dialog for editing visualization settings.",
                     GUIIconSubSys::getIcon(GUIIcon::COLORWHEEL), myGNEApp, MID_HOTKEY_F9_EDIT_VIEWSCHEME);
    editViewPort = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
                   "Edit Viewport", "Ctrl+I", "Opens a dialog for editing viewing are, zoom and rotation.",
                   GUIIconSubSys::getIcon(GUIIcon::EDITVIEWPORT), myGNEApp, MID_HOTKEY_CTRL_I_EDITVIEWPORT);
}


void
GNEApplicationWindowHelper::EditMenuCommands::buildFrontElementMenuCommand(FXMenuPane* editMenu) {
    // add clear front element
    clearFrontElement = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
                        "Clear front element", "F11", "Clear current front element",
                        GUIIconSubSys::getIcon(GUIIcon::FRONTELEMENT), myGNEApp, MID_HOTKEY_F11_FRONTELEMENT);
}


void
GNEApplicationWindowHelper::EditMenuCommands::buildOpenSUMOMenuCommands(FXMenuPane* editMenu) {
    // add open in sumo options
    loadAdditionalsInSUMOGUI = new FXMenuCheck(editMenu,
            "Load additionals in sumo-gui\t\tLoad additionals in sumo-gui.",
            myGNEApp, MID_GNE_TOOLBAREDIT_LOADADDITIONALS);
    loadAdditionalsInSUMOGUI->setCheck(TRUE);
    loadDemandInSUMOGUI = new FXMenuCheck(editMenu,
                                          "Load demand in sumo-gui\t\tLoad demand in sumo-gui.",
                                          myGNEApp, MID_GNE_TOOLBAREDIT_LOADDEMAND);
    loadDemandInSUMOGUI->setCheck(TRUE);
    openInSUMOGUI = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
                    "Open in sumo-gui", "Ctrl+T", "Opens the sumo-gui application with the current network.",
                    GUIIconSubSys::getIcon(GUIIcon::SUMO_MINI), myGNEApp, MID_HOTKEY_CTRL_T_OPENSUMONETEDIT);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::LockMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::LockMenuCommands::LockMenuCommands(GNEApplicationWindow* GNEApp) :
    menuCheckLockJunction(nullptr),
    menuCheckLockEdges(nullptr),
    menuCheckLockLanes(nullptr),
    menuCheckLockConnections(nullptr),
    menuCheckLockCrossings(nullptr),
    menuCheckLockAdditionals(nullptr),
    menuCheckLockTAZs(nullptr),
    menuCheckLockPolygons(nullptr),
    menuCheckLockPOIs(nullptr),
    menuCheckLockRoutes(nullptr),
    menuCheckLockVehicles(nullptr),
    menuCheckLockPersons(nullptr),
    menuCheckLockPersonTrip(nullptr),
    menuCheckLockWalk(nullptr),
    menuCheckLockRides(nullptr),
    menuCheckLockContainers(nullptr),
    menuCheckLockTransports(nullptr),
    menuCheckLockTranships(nullptr),
    menuCheckLockStops(nullptr),
    menuCheckLockEdgeDatas(nullptr),
    menuCheckLockEdgeRelDatas(nullptr),
    menuCheckLockEdgeTAZRels(nullptr),
    menuCheckLockSelectedElements(nullptr),
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindowHelper::LockMenuCommands::buildLockMenuCommands(FXMenuPane* fileMenu) {
    // newtork
    menuCheckLockJunction = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                            "Lock &junctions", "", "",
                            GUIIconSubSys::getIcon(GUIIcon::JUNCTION),
                            myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockEdges = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                         "Lock &edges", "", "",
                         GUIIconSubSys::getIcon(GUIIcon::EDGE),
                         myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockLanes = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                         "Lock &lanes", "", "",
                         GUIIconSubSys::getIcon(GUIIcon::LANE),
                         myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockConnections = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                               "Lock &connections", "", "",
                               GUIIconSubSys::getIcon(GUIIcon::CONNECTION),
                               myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockCrossings = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                             "Lock c&rossings", "", "",
                             GUIIconSubSys::getIcon(GUIIcon::CROSSING),
                             myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockAdditionals = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                               "Lock &additionals", "", "",
                               GUIIconSubSys::getIcon(GUIIcon::BUSSTOP),
                               myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockTAZs = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                        "Lock &TAZs", "", "",
                        GUIIconSubSys::getIcon(GUIIcon::TAZ),
                        myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockPolygons = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                            "Lock &polygons", "", "",
                            GUIIconSubSys::getIcon(GUIIcon::POLY),
                            myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockPOIs = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                        "Lock P&OIs", "", "",
                        GUIIconSubSys::getIcon(GUIIcon::POI),
                        myGNEApp, MID_GNE_LOCK_ELEMENT);
    // demand
    menuCheckLockRoutes = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                          "Lock &routes", "", "",
                          GUIIconSubSys::getIcon(GUIIcon::ROUTE),
                          myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockVehicles = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                            "Lock &vehicles", "", "",
                            GUIIconSubSys::getIcon(GUIIcon::VEHICLE),
                            myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockPersons = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                           "Lock &persons", "", "",
                           GUIIconSubSys::getIcon(GUIIcon::PERSON),
                           myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockPersonTrip = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                              "Lock person&Trips", "", "",
                              GUIIconSubSys::getIcon(GUIIcon::PERSONTRIP_FROMTO),
                              myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockWalk = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                        "Lock &walks", "", "",
                        GUIIconSubSys::getIcon(GUIIcon::WALK_FROMTO),
                        myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockRides = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                         "Lock ri&des", "", "",
                         GUIIconSubSys::getIcon(GUIIcon::RIDE_FROMTO),
                         myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockContainers = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                              "Lock &containers", "", "",
                              GUIIconSubSys::getIcon(GUIIcon::CONTAINER),
                              myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockTransports = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                              "Lock tra&nsports", "", "",
                              GUIIconSubSys::getIcon(GUIIcon::TRANSPORT_FROMTO),
                              myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockTranships = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                             "Lock trans&hips", "", "",
                             GUIIconSubSys::getIcon(GUIIcon::TRANSHIP_FROMTO),
                             myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockStops = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                         "Lock stop&s", "", "",
                         GUIIconSubSys::getIcon(GUIIcon::STOPELEMENT),
                         myGNEApp, MID_GNE_LOCK_ELEMENT);
    // data
    menuCheckLockEdgeDatas = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                             "Lock &edgeDatas", "", "",
                             GUIIconSubSys::getIcon(GUIIcon::EDGEDATA),
                             myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockEdgeRelDatas = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                                "Lock e&dgeRelDatas", "", "",
                                GUIIconSubSys::getIcon(GUIIcon::EDGERELDATA),
                                myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockEdgeTAZRels = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                               "Lock edge&TAZRel", "", "",
                               GUIIconSubSys::getIcon(GUIIcon::TAZRELDATA),
                               myGNEApp, MID_GNE_LOCK_ELEMENT);

    // separator
    new FXMenuSeparator(fileMenu);

    menuCheckLockSelectedElements = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                                    "Lock selected elements", "", "Lock selected elements",
                                    GUIIconSubSys::getIcon(GUIIcon::LOCK_SELECTED), myGNEApp, MID_GNE_LOCK_SELECTEDELEMENTS);

    // separator
    new FXMenuSeparator(fileMenu);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           "Lock all elements", "", "Lock all elements",
                                           GUIIconSubSys::getIcon(GUIIcon::LOCK), myGNEApp, MID_GNE_LOCK_ALLELEMENTS);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           "Unlock all elements", "", "Unlock all elements",
                                           GUIIconSubSys::getIcon(GUIIcon::UNLOCK), myGNEApp, MID_GNE_UNLOCK_ALLELEMENTS);
}


void
GNEApplicationWindowHelper::LockMenuCommands::removeHotkeys() {
    // network
    menuCheckLockJunction->remHotKey(parseHotKey('j'));
    menuCheckLockEdges->remHotKey(parseHotKey('e'));
    menuCheckLockLanes->remHotKey(parseHotKey('l'));
    menuCheckLockConnections->remHotKey(parseHotKey('c'));
    menuCheckLockCrossings->remHotKey(parseHotKey('r'));
    menuCheckLockAdditionals->remHotKey(parseHotKey('a'));
    menuCheckLockTAZs->remHotKey(parseHotKey('t'));
    menuCheckLockPolygons->remHotKey(parseHotKey('p'));
    menuCheckLockPOIs->remHotKey(parseHotKey('o'));
    // demand
    menuCheckLockRoutes->remHotKey(parseHotKey('r'));
    menuCheckLockVehicles->remHotKey(parseHotKey('v'));
    menuCheckLockPersons->remHotKey(parseHotKey('p'));
    menuCheckLockPersonTrip->remHotKey(parseHotKey('t'));
    menuCheckLockWalk->remHotKey(parseHotKey('w'));
    menuCheckLockRides->remHotKey(parseHotKey('d'));
    menuCheckLockContainers->remHotKey(parseHotKey('c'));
    menuCheckLockTransports->remHotKey(parseHotKey('n'));
    menuCheckLockTranships->remHotKey(parseHotKey('h'));
    menuCheckLockStops->remHotKey(parseHotKey('s'));
    // data
    menuCheckLockEdgeDatas->remHotKey(parseHotKey('e'));
    menuCheckLockEdgeRelDatas->remHotKey(parseHotKey('d'));
    menuCheckLockEdgeTAZRels->remHotKey(parseHotKey('t'));
}

void
GNEApplicationWindowHelper::LockMenuCommands::showNetworkLockMenuCommands() {
    // first enable menu commands
    menuCheckLockJunction->enable();
    menuCheckLockEdges->enable();
    menuCheckLockLanes->enable();
    menuCheckLockConnections->enable();
    menuCheckLockCrossings->enable();
    menuCheckLockAdditionals->enable();
    menuCheckLockTAZs->enable();
    menuCheckLockPolygons->enable();
    menuCheckLockPOIs->enable();
    // set accels
    menuCheckLockJunction->addHotKey(parseHotKey('j'));
    menuCheckLockEdges->addHotKey(parseHotKey('e'));
    menuCheckLockLanes->addHotKey(parseHotKey('l'));
    menuCheckLockConnections->addHotKey(parseHotKey('c'));
    menuCheckLockCrossings->addHotKey(parseHotKey('r'));
    menuCheckLockAdditionals->addHotKey(parseHotKey('a'));
    menuCheckLockTAZs->addHotKey(parseHotKey('t'));
    menuCheckLockPolygons->addHotKey(parseHotKey('p'));
    menuCheckLockPOIs->addHotKey(parseHotKey('o'));
    // now show it
    menuCheckLockJunction->show();
    menuCheckLockEdges->show();
    menuCheckLockLanes->show();
    menuCheckLockConnections->show();
    menuCheckLockCrossings->show();
    menuCheckLockAdditionals->show();
    menuCheckLockTAZs->show();
    menuCheckLockPolygons->show();
    menuCheckLockPOIs->show();
}


void
GNEApplicationWindowHelper::LockMenuCommands::hideNetworkLockMenuCommands() {
    // first disable menu commands
    menuCheckLockJunction->disable();
    menuCheckLockEdges->disable();
    menuCheckLockLanes->disable();
    menuCheckLockConnections->disable();
    menuCheckLockCrossings->disable();
    menuCheckLockAdditionals->disable();
    menuCheckLockTAZs->disable();
    menuCheckLockPolygons->disable();
    menuCheckLockPOIs->disable();
    // now hide it
    menuCheckLockJunction->hide();
    menuCheckLockEdges->hide();
    menuCheckLockLanes->hide();
    menuCheckLockConnections->hide();
    menuCheckLockCrossings->hide();
    menuCheckLockAdditionals->hide();
    menuCheckLockTAZs->hide();
    menuCheckLockPolygons->hide();
    menuCheckLockPOIs->hide();
}


void
GNEApplicationWindowHelper::LockMenuCommands::showDemandLockMenuCommands() {
    // first enable menu commands
    menuCheckLockRoutes->enable();
    menuCheckLockVehicles->enable();
    menuCheckLockPersons->enable();
    menuCheckLockPersonTrip->enable();
    menuCheckLockWalk->enable();
    menuCheckLockRides->enable();
    menuCheckLockContainers->enable();
    menuCheckLockTransports->enable();
    menuCheckLockTranships->enable();
    menuCheckLockStops->enable();
    // set accels
    menuCheckLockRoutes->addHotKey(parseHotKey('r'));
    menuCheckLockVehicles->addHotKey(parseHotKey('v'));
    menuCheckLockPersons->addHotKey(parseHotKey('p'));
    menuCheckLockPersonTrip->addHotKey(parseHotKey('t'));
    menuCheckLockWalk->addHotKey(parseHotKey('w'));
    menuCheckLockRides->addHotKey(parseHotKey('d'));
    menuCheckLockContainers->addHotKey(parseHotKey('c'));
    menuCheckLockTransports->addHotKey(parseHotKey('n'));
    menuCheckLockTranships->addHotKey(parseHotKey('h'));
    menuCheckLockStops->addHotKey(parseHotKey('s'));
    // now show it
    menuCheckLockRoutes->show();
    menuCheckLockVehicles->show();
    menuCheckLockPersons->show();
    menuCheckLockPersonTrip->show();
    menuCheckLockWalk->show();
    menuCheckLockRides->show();
    menuCheckLockContainers->show();
    menuCheckLockTransports->show();
    menuCheckLockTranships->show();
    menuCheckLockStops->show();
}


void
GNEApplicationWindowHelper::LockMenuCommands::hideDemandLockMenuCommands() {
    // first disable menu commands
    menuCheckLockRoutes->disable();
    menuCheckLockVehicles->disable();
    menuCheckLockPersons->disable();
    menuCheckLockPersonTrip->disable();
    menuCheckLockWalk->disable();
    menuCheckLockRides->disable();
    menuCheckLockContainers->disable();
    menuCheckLockTransports->disable();
    menuCheckLockTranships->disable();
    menuCheckLockStops->disable();
    // now hide it
    menuCheckLockRoutes->hide();
    menuCheckLockVehicles->hide();
    menuCheckLockPersons->hide();
    menuCheckLockPersonTrip->hide();
    menuCheckLockWalk->hide();
    menuCheckLockRides->hide();
    menuCheckLockContainers->hide();
    menuCheckLockTransports->hide();
    menuCheckLockTranships->hide();
    menuCheckLockStops->hide();
}


void
GNEApplicationWindowHelper::LockMenuCommands::showDataLockMenuCommands() {
    // first enable menu commands
    menuCheckLockEdgeDatas->enable();
    menuCheckLockEdgeRelDatas->enable();
    menuCheckLockEdgeTAZRels->enable();
    // set accels
    menuCheckLockEdgeDatas->addHotKey(parseHotKey('e'));
    menuCheckLockEdgeRelDatas->addHotKey(parseHotKey('d'));
    menuCheckLockEdgeTAZRels->addHotKey(parseHotKey('t'));
    // now show it
    menuCheckLockEdgeDatas->show();
    menuCheckLockEdgeRelDatas->show();
    menuCheckLockEdgeTAZRels->show();
}


void
GNEApplicationWindowHelper::LockMenuCommands::hideDataLockMenuCommands() {
    // first disable menu commands
    menuCheckLockEdgeDatas->disable();
    menuCheckLockEdgeRelDatas->disable();
    menuCheckLockEdgeTAZRels->disable();
    // now hide it
    menuCheckLockEdgeDatas->hide();
    menuCheckLockEdgeRelDatas->hide();
    menuCheckLockEdgeTAZRels->hide();
}


void
GNEApplicationWindowHelper::LockMenuCommands::lockAll() {
    menuCheckLockJunction->setCheck(TRUE);
    menuCheckLockEdges->setCheck(TRUE);
    menuCheckLockLanes->setCheck(TRUE);
    menuCheckLockConnections->setCheck(TRUE);
    menuCheckLockCrossings->setCheck(TRUE);
    menuCheckLockAdditionals->setCheck(TRUE);
    menuCheckLockTAZs->setCheck(TRUE);
    menuCheckLockPolygons->setCheck(TRUE);
    menuCheckLockPOIs->setCheck(TRUE);
    menuCheckLockRoutes->setCheck(TRUE);
    menuCheckLockVehicles->setCheck(TRUE);
    menuCheckLockPersons->setCheck(TRUE);
    menuCheckLockPersonTrip->setCheck(TRUE);
    menuCheckLockWalk->setCheck(TRUE);
    menuCheckLockRides->setCheck(TRUE);
    menuCheckLockContainers->setCheck(TRUE);
    menuCheckLockTransports->setCheck(TRUE);
    menuCheckLockTranships->setCheck(TRUE);
    menuCheckLockStops->setCheck(TRUE);
    menuCheckLockEdgeDatas->setCheck(TRUE);
    menuCheckLockEdgeRelDatas->setCheck(TRUE);
    menuCheckLockEdgeTAZRels->setCheck(TRUE);
}


void
GNEApplicationWindowHelper::LockMenuCommands::unlockAll() {
    menuCheckLockJunction->setCheck(FALSE);
    menuCheckLockEdges->setCheck(FALSE);
    menuCheckLockLanes->setCheck(FALSE);
    menuCheckLockConnections->setCheck(FALSE);
    menuCheckLockCrossings->setCheck(FALSE);
    menuCheckLockAdditionals->setCheck(FALSE);
    menuCheckLockTAZs->setCheck(FALSE);
    menuCheckLockPolygons->setCheck(FALSE);
    menuCheckLockPOIs->setCheck(FALSE);
    menuCheckLockRoutes->setCheck(FALSE);
    menuCheckLockVehicles->setCheck(FALSE);
    menuCheckLockPersons->setCheck(FALSE);
    menuCheckLockPersonTrip->setCheck(FALSE);
    menuCheckLockWalk->setCheck(FALSE);
    menuCheckLockRides->setCheck(FALSE);
    menuCheckLockContainers->setCheck(FALSE);
    menuCheckLockTransports->setCheck(FALSE);
    menuCheckLockTranships->setCheck(FALSE);
    menuCheckLockStops->setCheck(FALSE);
    menuCheckLockEdgeDatas->setCheck(FALSE);
    menuCheckLockEdgeRelDatas->setCheck(FALSE);
    menuCheckLockEdgeTAZRels->setCheck(FALSE);
}


void
GNEApplicationWindowHelper::LockMenuCommands::editLocking(const GNEAttributeCarrier* AC, const FXbool value) {
    // check elements
    if (AC->getTagProperty().getTag() == SUMO_TAG_JUNCTION) {
        menuCheckLockJunction->setCheck(value);
    } else if (AC->getTagProperty().getTag() == SUMO_TAG_EDGE) {
        menuCheckLockEdges->setCheck(value);
    } else if (AC->getTagProperty().getTag() == SUMO_TAG_LANE) {
        menuCheckLockLanes->setCheck(value);
    } else if (AC->getTagProperty().getTag() == SUMO_TAG_CONNECTION) {
        menuCheckLockConnections->setCheck(value);
    } else if (AC->getTagProperty().getTag() == SUMO_TAG_CROSSING) {
        menuCheckLockCrossings->setCheck(value);
    } else if (AC->getTagProperty().isAdditionalElement()) {
        menuCheckLockAdditionals->setCheck(value);
    } else if (AC->getTagProperty().getTag() == SUMO_TAG_TAZ) {
        menuCheckLockTAZs->setCheck(value);
    } else if (AC->getTagProperty().getTag() == SUMO_TAG_POLY) {
        menuCheckLockPolygons->setCheck(value);
    } else if (AC->getTagProperty().getTag() == SUMO_TAG_POI) {
        menuCheckLockPOIs->setCheck(value);
    } else if (AC->getTagProperty().isRoute()) {
        menuCheckLockRoutes->setCheck(value);
    } else if (AC->getTagProperty().isVehicle()) {
        menuCheckLockVehicles->setCheck(value);
    } else if (AC->getTagProperty().isPerson()) {
        menuCheckLockPersons->setCheck(value);
    } else if (AC->getTagProperty().isPersonTrip()) {
        menuCheckLockPersonTrip->setCheck(value);
    } else if (AC->getTagProperty().isWalk()) {
        menuCheckLockWalk->setCheck(value);
    } else if (AC->getTagProperty().isRide()) {
        menuCheckLockRides->setCheck(value);
    } else if (AC->getTagProperty().isContainer()) {
        menuCheckLockContainers->setCheck(value);
    } else if (AC->getTagProperty().isTransportPlan()) {
        menuCheckLockTransports->setCheck(value);
    } else if (AC->getTagProperty().isTranshipPlan()) {
        menuCheckLockTranships->setCheck(value);
    } else if (AC->getTagProperty().isStop() || AC->getTagProperty().isStopPerson()) {
        menuCheckLockStops->setCheck(value);
    } else if (AC->getTagProperty().getTag() == SUMO_TAG_MEANDATA_EDGE) {
        menuCheckLockEdgeDatas->setCheck(value);
    } else if (AC->getTagProperty().getTag() == SUMO_TAG_EDGEREL) {
        menuCheckLockEdgeRelDatas->setCheck(value);
    } else if (AC->getTagProperty().getTag() == SUMO_TAG_TAZREL) {
        menuCheckLockEdgeTAZRels->setCheck(value);
    }
}


FXHotKey
GNEApplicationWindowHelper::LockMenuCommands::parseHotKey(const FXwchar character) {
    FXuint code = 0, mods = 0;
    if (Unicode::isAlphaNumeric(character)) {
        mods = ALTMASK;
        code = fxucs2keysym(Unicode::toLower(character));
    }
    return MKUINT(code, mods);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::ProcessingMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::ProcessingMenuCommands::ProcessingMenuCommands(GNEApplicationWindow* GNEApp) :
    computeNetwork(nullptr),
    computeNetworkVolatile(nullptr),
    cleanJunctions(nullptr),
    joinJunctions(nullptr),
    clearInvalidCrossings(nullptr),
    computeDemand(nullptr),
    cleanRoutes(nullptr),
    joinRoutes(nullptr),
    adjustPersonPlans(nullptr),
    clearInvalidDemandElements(nullptr),
    optionMenus(nullptr),
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindowHelper::ProcessingMenuCommands::buildProcessingMenuCommands(FXMenuPane* fileMenu) {
    // build network processing menu commands
    computeNetwork = GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                     "Compute Junctions", "F5", "Compute junction shape and logic.",
                     GUIIconSubSys::getIcon(GUIIcon::COMPUTEJUNCTIONS), myGNEApp, MID_HOTKEY_F5_COMPUTE_NETWORK_DEMAND);
    computeNetworkVolatile = GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                             "Compute Junctions with volatile options", "Shift+F5", "Compute junction shape and logic using volatile junctions.",
                             GUIIconSubSys::getIcon(GUIIcon::COMPUTEJUNCTIONS), myGNEApp, MID_HOTKEY_SHIFT_F5_COMPUTEJUNCTIONS_VOLATILE);
    cleanJunctions = GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                     "Clean Junctions", "F6", "Remove solitary junctions.",
                     GUIIconSubSys::getIcon(GUIIcon::CLEANJUNCTIONS), myGNEApp, MID_HOTKEY_F6_CLEAN_SOLITARYJUNCTIONS_UNUSEDROUTES);
    joinJunctions = GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                    "Join Selected Junctions", "F7", "Join selected junctions into a single junction.",
                    GUIIconSubSys::getIcon(GUIIcon::JOINJUNCTIONS), myGNEApp, MID_HOTKEY_F7_JOIN_SELECTEDJUNCTIONS_ROUTES);
    clearInvalidCrossings = GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                            "Clean invalid crossings", "F8", "Clear invalid crossings.",
                            GUIIconSubSys::getIcon(GUIIcon::CLEANJUNCTIONS), myGNEApp, MID_HOTKEY_F8_CLEANINVALID_CROSSINGS_DEMANDELEMENTS);
    // build demand  processing menu commands
    computeDemand = GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                    "Compute demand", "F5", "Computes demand elements.",
                    GUIIconSubSys::getIcon(GUIIcon::COMPUTEDEMAND), myGNEApp, MID_HOTKEY_F5_COMPUTE_NETWORK_DEMAND);
    cleanRoutes = GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                  "Clean routes", "F6", "Removes routes without vehicles.",
                  GUIIconSubSys::getIcon(GUIIcon::CLEANROUTES), myGNEApp, MID_HOTKEY_F6_CLEAN_SOLITARYJUNCTIONS_UNUSEDROUTES);
    joinRoutes = GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                 "Join routes", "F7", "Joins routes with the same edges.",
                 GUIIconSubSys::getIcon(GUIIcon::JOINROUTES), myGNEApp, MID_HOTKEY_F7_JOIN_SELECTEDJUNCTIONS_ROUTES);
    adjustPersonPlans = GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                        "Adjust person plans", "Shift+F7", "Adjust person plans (start/end positions, arrival positions, etc.)",
                        GUIIconSubSys::getIcon(GUIIcon::ADJUSTPERSONPLANS), myGNEApp, MID_HOTKEY_SHIFT_F7_ADJUST_PERSON_PLANS);
    clearInvalidDemandElements = GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                 "Clean invalid route elements", "F8", "Clear elements with an invalid path (routes, Trips, Flows...).",
                                 GUIIconSubSys::getIcon(GUIIcon::CLEANJUNCTIONS), myGNEApp, MID_HOTKEY_F8_CLEANINVALID_CROSSINGS_DEMANDELEMENTS);
    // create separator
    mySeparator = new FXMenuSeparator(fileMenu);
    // create optionmenus
    optionMenus = GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                  "Options", "F10", "Configure Processing Options.",
                  GUIIconSubSys::getIcon(GUIIcon::OPTIONS), myGNEApp, MID_HOTKEY_F10_OPTIONSMENU);
}


void
GNEApplicationWindowHelper::ProcessingMenuCommands::showNetworkProcessingMenuCommands() {
    // first enable menu commands
    computeNetwork->enable();
    computeNetworkVolatile->enable();
    cleanJunctions->enable();
    joinJunctions->enable();
    clearInvalidCrossings->enable();
    // now show it
    computeNetwork->show();
    computeNetworkVolatile->show();
    cleanJunctions->show();
    joinJunctions->show();
    clearInvalidCrossings->show();
}


void
GNEApplicationWindowHelper::ProcessingMenuCommands::hideNetworkProcessingMenuCommands() {
    // first disable menu commands
    computeNetwork->disable();
    computeNetworkVolatile->disable();
    cleanJunctions->disable();
    joinJunctions->disable();
    clearInvalidCrossings->disable();
    // now hide it
    computeNetwork->hide();
    computeNetworkVolatile->hide();
    cleanJunctions->hide();
    joinJunctions->hide();
    clearInvalidCrossings->hide();
}


void
GNEApplicationWindowHelper::ProcessingMenuCommands::showDemandProcessingMenuCommands() {
    // first enable menu commands
    computeDemand->enable();
    cleanRoutes->enable();
    joinRoutes->enable();
    adjustPersonPlans->enable();
    clearInvalidDemandElements->enable();
    // now show it
    computeDemand->show();
    cleanRoutes->show();
    joinRoutes->show();
    adjustPersonPlans->show();
    clearInvalidDemandElements->show();
}


void
GNEApplicationWindowHelper::ProcessingMenuCommands::hideDemandProcessingMenuCommands() {
    // first disable menu commands
    computeDemand->disable();
    cleanRoutes->disable();
    joinRoutes->disable();
    adjustPersonPlans->disable();
    clearInvalidDemandElements->disable();
    // now hide it
    computeDemand->hide();
    cleanRoutes->hide();
    joinRoutes->hide();
    adjustPersonPlans->hide();
    clearInvalidDemandElements->hide();
}


void
GNEApplicationWindowHelper::ProcessingMenuCommands::showDataProcessingMenuCommands() {
    // currently nothing to show
}


void
GNEApplicationWindowHelper::ProcessingMenuCommands::hideDataProcessingMenuCommands() {
    // currently nothing to hide
}

void
GNEApplicationWindowHelper::ProcessingMenuCommands::showSeparator() {
    mySeparator->show();
}


void
GNEApplicationWindowHelper::ProcessingMenuCommands::hideSeparator() {
    mySeparator->hide();
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::LocateMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::LocateMenuCommands::LocateMenuCommands(GNEApplicationWindow* GNEApp) :
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindowHelper::LocateMenuCommands::buildLocateMenuCommands(FXMenuPane* locateMenu) {
    // build locate menu commands
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           "Locate &Junctions", "Shift+J", "Open a dialog for locating a Junction.",
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEJUNCTION), myGNEApp, MID_LOCATEJUNCTION);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           "Locate &Edges", "Shift+E", "Open a dialog for locating an Edge.",
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEEDGE), myGNEApp, MID_LOCATEEDGE);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           "Locate &Vehicles", "Shift+V", "Open a dialog for locating a Vehicle.",
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEVEHICLE), myGNEApp, MID_LOCATEVEHICLE);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           "Locate &Persons", "Shift+P", "Open a dialog for locating a Person.",
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEPERSON), myGNEApp, MID_LOCATEPERSON);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           "Locate &Route", "Shift+R", "Open a dialog for locating a Route.",
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEROUTE), myGNEApp, MID_LOCATEROUTE);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           "Locate &Stops", "Shift+S", "Open a dialog for locating a Stop.",
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATESTOP), myGNEApp, MID_LOCATESTOP);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           "Locate &TLS", "Shift+T", "Open a dialog for locating a Traffic Light.",
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATETLS), myGNEApp, MID_LOCATETLS);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           "Locate &Additional", "Shift+A", "Open a dialog for locating an Additional Structure.",
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEADD), myGNEApp, MID_LOCATEADD);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           "Locate P&oI", "Shift+O", "Open a dialog for locating a Point of Interest.",
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEPOI), myGNEApp, MID_LOCATEPOI);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           "Locate Po&lygon", "Shift+L", "Open a dialog for locating a Polygon.",
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEPOLY), myGNEApp, MID_LOCATEPOLY);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::ToolsMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::ToolsMenuCommands::ToolsMenuCommands(GNEApplicationWindow* GNEApp) :
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindowHelper::ToolsMenuCommands::buildToolsMenuCommands(FXMenuPane* locateMenu) {
    // build locate menu commands
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           "&NetDiff", "", "Execute NETDIFF.",
                                           GUIIconSubSys::getIcon(GUIIcon::TOOL_NETDIFF), myGNEApp, MID_GNE_TOOLBARTOOLS_NETDIFF);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::WindowsMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::WindowsMenuCommands::WindowsMenuCommands(GNEApplicationWindow* GNEApp) :
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindowHelper::WindowsMenuCommands::buildWindowsMenuCommands(FXMenuPane* windowsMenu, FXStatusBar* statusbar, GUIMessageWindow* messageWindow) {
    // build windows menu commands
    GUIDesigns::buildFXMenuCheckbox(windowsMenu,
                                    "Show Status Line", "Toggle this Status Bar on/off.",
                                    statusbar, FXWindow::ID_TOGGLESHOWN);
    GUIDesigns::buildFXMenuCheckbox(windowsMenu,
                                    "Show Message Window", "Toggle the Message Window on/off.",
                                    messageWindow, FXWindow::ID_TOGGLESHOWN);
    GUIDesigns::buildFXMenuCommandShortcut(windowsMenu,
                                           "Clear Message Window", "", "Clear the Message Window.",
                                           GUIIconSubSys::getIcon(GUIIcon::CLEARMESSAGEWINDOW), myGNEApp, MID_CLEARMESSAGEWINDOW);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::NetworkCheckableButtons - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::SupermodeCommands::SupermodeCommands(GNEApplicationWindow* GNEApp) :
    networkMode(nullptr),
    demandMode(nullptr),
    dataMode(nullptr),
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindowHelper::SupermodeCommands::showSupermodeCommands() {
    networkMode->show();
    demandMode->show();
    dataMode->show();
}


void
GNEApplicationWindowHelper::SupermodeCommands::hideSupermodeCommands() {
    networkMode->hide();
    demandMode->hide();
    dataMode->hide();
}


void
GNEApplicationWindowHelper::SupermodeCommands::buildSupermodeCommands(FXMenuPane* editMenu) {
    // build supermode menu commands
    networkMode = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
                  "&Network mode", "F2", "Select network mode.",
                  GUIIconSubSys::getIcon(GUIIcon::SUPERMODENETWORK), myGNEApp, MID_HOTKEY_F2_SUPERMODE_NETWORK);
    demandMode = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
                 "&Demand mode", "F3", "Select demand mode.",
                 GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDEMAND), myGNEApp, MID_HOTKEY_F3_SUPERMODE_DEMAND);
    dataMode = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
               "&Data mode", "F4", "Select data mode.",
               GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDATA), myGNEApp, MID_HOTKEY_F4_SUPERMODE_DATA);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper - methods
// ---------------------------------------------------------------------------

bool
GNEApplicationWindowHelper::toggleEditOptionsNetwork(GNEViewNet* viewNet, const MFXCheckableButton* menuCheck, const int numericalKeyPressed, FXObject* obj, FXSelector sel) {
    // finally function correspond to visibleMenuCommands[numericalKeyPressed]
    if (menuCheck == viewNet->getNetworkViewOptions().menuCheckToggleGrid) {
        // Toggle menuCheckToggleGrid
        if (viewNet->getNetworkViewOptions().menuCheckToggleGrid->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled toggle show grid throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled toggle show grid throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowGrid
        viewNet->onCmdToggleShowGrid(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckToggleDrawJunctionShape) {
        // Toggle menuCheckToggleDrawJunctionShape
        if (viewNet->getNetworkViewOptions().menuCheckToggleDrawJunctionShape->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled draw junction shape throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled draw junction shape throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleDrawJunctionShape
        viewNet->onCmdToggleDrawJunctionShape(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckDrawSpreadVehicles) {
        // Toggle menuCheckDrawSpreadVehicles
        if (viewNet->getNetworkViewOptions().menuCheckDrawSpreadVehicles->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled toggle draw spread vehicles throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled toggle spread vehicles throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleDrawSpreadVehicles
        viewNet->onCmdToggleDrawSpreadVehicles(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckShowDemandElements) {
        // Toggle menuCheckShowDemandElements
        if (viewNet->getNetworkViewOptions().menuCheckShowDemandElements->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled show demand elements throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled show demand elements throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowDemandElementsNetwork
        viewNet->onCmdToggleShowDemandElementsNetwork(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckSelectEdges) {
        // Toggle menuCheckSelectEdges
        if (viewNet->getNetworkViewOptions().menuCheckSelectEdges->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled select edges throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled select edges throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleSelectEdges
        viewNet->onCmdToggleSelectEdges(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckShowConnections) {
        // Toggle menuCheckShowConnections
        if (viewNet->getNetworkViewOptions().menuCheckShowConnections->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled show connections throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled show connections throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowConnections
        viewNet->onCmdToggleShowConnections(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckHideConnections) {
        // Toggle menuCheckHideConnections
        if (viewNet->getNetworkViewOptions().menuCheckHideConnections->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled hide connections throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled hide connections throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleHideConnections
        viewNet->onCmdToggleHideConnections(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckShowAdditionalSubElements) {
        // Toggle menuCheckShowAdditionalSubElements
        if (viewNet->getNetworkViewOptions().menuCheckShowAdditionalSubElements->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled show additional sub-elements throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled show additional sub-elements throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowAdditionalSubElements
        viewNet->onCmdToggleShowAdditionalSubElements(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckShowTAZElements) {
        // Toggle menuCheckShowTAZElements
        if (viewNet->getNetworkViewOptions().menuCheckShowTAZElements->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled show TAZ elements throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled show TAZ elements throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowTAZElements
        viewNet->onCmdToggleShowTAZElements(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckExtendSelection) {
        // Toggle menuCheckExtendSelection
        if (viewNet->getNetworkViewOptions().menuCheckExtendSelection->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled extend selection throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled extend selection throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleExtendSelection
        viewNet->onCmdToggleExtendSelection(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckChangeAllPhases) {
        // Toggle menuCheckChangeAllPhases
        if (viewNet->getNetworkViewOptions().menuCheckChangeAllPhases->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled change all phases throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled change all phases throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleChangeAllPhases
        viewNet->onCmdToggleChangeAllPhases(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckWarnAboutMerge) {
        // Toggle menuCheckWarnAboutMerge
        if (viewNet->getNetworkViewOptions().menuCheckWarnAboutMerge->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled warn about merge throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled warn about merge throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleWarnAboutMerge
        viewNet->onCmdToggleWarnAboutMerge(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckShowJunctionBubble) {
        // Toggle menuCheckShowJunctionBubble
        if (viewNet->getNetworkViewOptions().menuCheckShowJunctionBubble->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled show junction as bubble throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled show junction as bubble throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowJunctionBubble
        viewNet->onCmdToggleShowJunctionBubbles(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckMoveElevation) {
        // Toggle menuCheckMoveElevation
        if (viewNet->getNetworkViewOptions().menuCheckMoveElevation->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled move elevation throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled move elevation throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleMoveElevation
        viewNet->onCmdToggleMoveElevation(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckChainEdges) {
        // Toggle menuCheckChainEdges
        if (viewNet->getNetworkViewOptions().menuCheckChainEdges->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled chain edges throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled chain edges throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleChainEdges
        viewNet->onCmdToggleChainEdges(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckAutoOppositeEdge) {
        // Toggle menuCheckAutoOppositeEdge
        if (viewNet->getNetworkViewOptions().menuCheckAutoOppositeEdge->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled auto opposite edge throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled auto opposite edge throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleAutoOppositeEdge
        viewNet->onCmdToggleAutoOppositeEdge(obj, sel, nullptr);
    } else {
        return false;
    }
    return true;
}


bool
GNEApplicationWindowHelper::toggleEditOptionsDemand(GNEViewNet* viewNet, const MFXCheckableButton* menuCheck, const int numericalKeyPressed, FXObject* obj, FXSelector sel) {
    if (menuCheck == viewNet->getDemandViewOptions().menuCheckToggleGrid) {
        // Toggle menuCheckToggleGrid
        if (viewNet->getDemandViewOptions().menuCheckToggleGrid->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled toggle show grid throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled toggle show grid throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowGrid
        viewNet->onCmdToggleShowGrid(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckToggleDrawJunctionShape) {
        // Toggle menuCheckToggleDrawJunctionShape
        if (viewNet->getDemandViewOptions().menuCheckToggleDrawJunctionShape->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled draw junction shape throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled draw junction shape throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleDrawJunctionShape
        viewNet->onCmdToggleDrawJunctionShape(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckDrawSpreadVehicles) {
        // Toggle menuCheckDrawSpreadVehicles
        if (viewNet->getDemandViewOptions().menuCheckDrawSpreadVehicles->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled toggle draw spread vehicles throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled toggle spread vehicles throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleDrawSpreadVehicles
        viewNet->onCmdToggleDrawSpreadVehicles(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckHideShapes) {
        // Toggle menuCheckHideShapes
        if (viewNet->getDemandViewOptions().menuCheckHideShapes->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled hide shapes throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled hide shapes throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleHideShapes
        viewNet->onCmdToggleHideShapes(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckShowAllTrips) {
        // Toggle menuCheckShowAllTrips
        if (viewNet->getDemandViewOptions().menuCheckShowAllTrips->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled show all trips throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled show all trips throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowTrips
        viewNet->onCmdToggleShowTrips(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckShowAllPersonPlans) {
        // Toggle menuCheckShowAllPersonPlans
        if (viewNet->getDemandViewOptions().menuCheckShowAllPersonPlans->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled show all person plans throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled show all person plans throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowAllPersonPlans
        viewNet->onCmdToggleShowAllPersonPlans(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckLockPerson) {
        // Toggle menuCheckShowAllPersonPlans
        if (viewNet->getDemandViewOptions().menuCheckLockPerson->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled lock person plan throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled lock person plan throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleLockPerson
        viewNet->onCmdToggleLockPerson(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckShowAllContainerPlans) {
        // Toggle menuCheckShowAllContainerPlans
        if (viewNet->getDemandViewOptions().menuCheckShowAllContainerPlans->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled show all container plans throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled show all container plans throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowAllContainerPlans
        viewNet->onCmdToggleShowAllContainerPlans(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckLockContainer) {
        // Toggle menuCheckShowAllContainerPlans
        if (viewNet->getDemandViewOptions().menuCheckLockContainer->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled lock container plan throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled lock container plan throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleLockContainer
        viewNet->onCmdToggleLockContainer(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckHideNonInspectedDemandElements) {
        // Toggle menuCheckHideNonInspectedDemandElements
        if (viewNet->getDemandViewOptions().menuCheckHideNonInspectedDemandElements->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled hide non inspected demand elements throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled hide non inspected demand elements throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleHideNonInspecteDemandElements
        viewNet->onCmdToggleHideNonInspecteDemandElements(obj, sel, nullptr);

    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckShowOverlappedRoutes) {
        // Toggle menuCheckShowOverlappedRoutes
        if (viewNet->getDemandViewOptions().menuCheckShowOverlappedRoutes->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled show overlapped routes throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled show overlapped routes throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleHideNonInspecteDemandElements
        viewNet->onCmdToggleShowOverlappedRoutes(obj, sel, nullptr);
    } else {
        return false;
    }
    return true;
}


bool
GNEApplicationWindowHelper::toggleEditOptionsData(GNEViewNet* viewNet, const MFXCheckableButton* menuCheck, const int numericalKeyPressed, FXObject* obj, FXSelector sel) {
    if (menuCheck == viewNet->getDataViewOptions().menuCheckToggleDrawJunctionShape) {
        // Toggle menuCheckToggleDrawJunctionShape
        if (viewNet->getDataViewOptions().menuCheckToggleDrawJunctionShape->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled draw junction shape throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled draw junction shape throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleDrawJunctionShape
        viewNet->onCmdToggleDrawJunctionShape(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDataViewOptions().menuCheckShowAdditionals) {
        // Toggle menuCheckHideShapes
        if (viewNet->getDataViewOptions().menuCheckShowAdditionals->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled show additionals throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled show shapes throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowAdditionals
        viewNet->onCmdToggleShowAdditionals(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDataViewOptions().menuCheckShowShapes) {
        // Toggle menuCheckHideShapes
        if (viewNet->getDataViewOptions().menuCheckShowShapes->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled show shapes throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled show shapes throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowShapes
        viewNet->onCmdToggleShowShapes(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDataViewOptions().menuCheckShowDemandElements) {
        // Toggle menuCheckShowDemandElements
        if (viewNet->getDataViewOptions().menuCheckShowDemandElements->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled show demand elements throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled show demand elements throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowDemandElementsData
        viewNet->onCmdToggleShowDemandElementsData(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDataViewOptions().menuCheckToogleTAZRelDrawing) {
        // Toggle menuCheckToogleTAZRelDrawing
        if (viewNet->getDataViewOptions().menuCheckToogleTAZRelDrawing->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled toogle TAXRel drawing throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled toogle TAXRel drawing throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleTAZRelDrawing
        viewNet->onCmdToggleTAZRelDrawing(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDataViewOptions().menuCheckToogleTAZDrawFill) {
        // Toggle menuCheckToogleTAZDrawFill
        if (viewNet->getDataViewOptions().menuCheckToogleTAZDrawFill->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled toogle TAZ draw fill throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled toogle TAZ draw fill throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleTAZDrawFill
        viewNet->onCmdToggleTAZDrawFill(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDataViewOptions().menuCheckToogleTAZRelOnlyFrom) {
        // Toggle menuCheckToogleTAZRelOnlyFrom
        if (viewNet->getDataViewOptions().menuCheckToogleTAZRelOnlyFrom->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled toogle TAZRel only from throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled toogle TAZRel only from throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleTAZRelOnlyFrom
        viewNet->onCmdToggleTAZRelOnlyFrom(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDataViewOptions().menuCheckToogleTAZRelOnlyTo) {
        // Toggle menuCheckToogleTAZRelOnlyTo
        if (viewNet->getDataViewOptions().menuCheckToogleTAZRelOnlyTo->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled toogle TAZRel only to throught alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled toogle TAZRel only to throught alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleTAZRelOnlyTo
        viewNet->onCmdToggleTAZRelOnlyTo(obj, sel, nullptr);
    } else {
        return false;
    }
    return true;
}


bool
GNEApplicationWindowHelper::stringEndsWith(const std::string &str, const std::string &suffix) {
    if (str.length() < suffix.length()) {
        return false;
    } else {
        return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
    }
}

/****************************************************************************/
