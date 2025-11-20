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
/// @file    GNEApplicationWindowHelper.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
///
// The main window of Netedit (adapted from GUIApplicationWindow)
/****************************************************************************/
#include <config.h>

#include <regex>

#include <netedit/dialogs/file/GNEFileDialog.h>
#include <netedit/dialogs/tools/GNEPythonToolDialog.h>
#include <netedit/dialogs/tools/GNERunPythonToolDialog.h>
#include <netedit/elements/GNEAttributeCarrier.h>
#include <netedit/GNETagProperties.h>
#include <netedit/templates.h>
#include <netedit/tools/GNENetDiffTool.h>
#include <utils/common/FileHelpers.h>
#include <utils/foxtools/MFXMenuCheckIcon.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/handlers/TemplateHandler.h>
#include <utils/options/OptionsLoader.h>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#include "GNEApplicationWindow.h"
#include "GNEViewNet.h"
#include "GNENet.h"

// ===========================================================================
// GNEApplicationWindowHelper::ToolbarsGrip method definitions
// ===========================================================================

GNEApplicationWindowHelper::ToolbarsGrip::ToolbarsGrip(GNEApplicationWindow* applicationWindow) :
    myApplicationWindow(applicationWindow) {
}


void
GNEApplicationWindowHelper::ToolbarsGrip::buildMenuToolbarsGrip() {
    // build menu bar (for File, edit, processing...) using specify design
    myPythonToolBarShellMenu = new FXToolBarShell(myApplicationWindow, GUIDesignToolBar);
    menu = new FXMenuBar(myApplicationWindow->getTopDock(), myPythonToolBarShellMenu, GUIDesignToolbarMenuBarNetedit);
    // declare toolbar grip for menu bar
    new FXToolBarGrip(menu, menu, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
}


void
GNEApplicationWindowHelper::ToolbarsGrip::buildViewParentToolbarsGrips() {
    // build toolbar shells
    myPythonToolBarShellSuperModes = new FXToolBarShell(myApplicationWindow, GUIDesignToolBar);
    myPythonToolBarShellSaveElements = new FXToolBarShell(myApplicationWindow, GUIDesignToolBar);
    myPythonToolBarShellTimeFormat = new FXToolBarShell(myApplicationWindow, GUIDesignToolBar);
    myPythonToolBarShellNavigation = new FXToolBarShell(myApplicationWindow, GUIDesignToolBar);
    myPythonToolBarShellModes = new FXToolBarShell(myApplicationWindow, GUIDesignToolBar);
    myPythonToolBarShellIntervalBar = new FXToolBarShell(myApplicationWindow, GUIDesignToolBar);
    // build menu bars
    superModes = new FXMenuBar(myApplicationWindow->getTopDock(), myPythonToolBarShellSuperModes, GUIDesignToolBarRaisedSupermodes);
    saveElements = new FXMenuBar(myApplicationWindow->getTopDock(), myPythonToolBarShellSaveElements, GUIDesignToolBarRaisedNext);
    timeSwitch = new FXMenuBar(myApplicationWindow->getTopDock(), myPythonToolBarShellTimeFormat, GUIDesignToolBarRaisedSame);
    navigation = new FXMenuBar(myApplicationWindow->getTopDock(), myPythonToolBarShellNavigation, GUIDesignToolBarRaisedSame);
    if (OptionsCont::getOptions().getBool("gui-testing")) {
        modes = new FXMenuBar(myApplicationWindow->getTopDock(), myPythonToolBarShellModes, GUIDesignToolBarRaisedNext);
    } else {
        modes = new FXMenuBar(myApplicationWindow->getTopDock(), myPythonToolBarShellModes, GUIDesignToolBarRaisedSame);
    }
    intervalBar = new FXMenuBar(myApplicationWindow->getTopDock(), myPythonToolBarShellIntervalBar, GUIDesignToolBarRaisedNext);
    // build FXToolBarGrip
    new FXToolBarGrip(superModes, superModes, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
    new FXToolBarGrip(saveElements, saveElements, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
    new FXToolBarGrip(timeSwitch, timeSwitch, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
    new FXToolBarGrip(navigation, navigation, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
    new FXToolBarGrip(modes, modes, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
    new FXToolBarGrip(intervalBar, intervalBar, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
    // create menu bars
    superModes->create();
    saveElements->create();
    timeSwitch->create();
    navigation->create();
    modes->create();
    intervalBar->create();
    // create toolbar shells
    myPythonToolBarShellSuperModes->create();
    myPythonToolBarShellSaveElements->create();
    myPythonToolBarShellTimeFormat->create();
    myPythonToolBarShellNavigation->create();
    myPythonToolBarShellModes->create();
    myPythonToolBarShellIntervalBar->create();
    // recalc top dop after creating elements
    myApplicationWindow->getTopDock()->recalc();
}


void
GNEApplicationWindowHelper::ToolbarsGrip::destroyParentToolbarsGrips() {
    // delete Menu bars
    delete superModes;
    delete saveElements;
    delete timeSwitch;
    delete navigation;
    delete modes;
    delete intervalBar;
    // also delete toolbar shells to avoid floating windows
    delete myPythonToolBarShellSuperModes;
    delete myPythonToolBarShellSaveElements;
    delete myPythonToolBarShellTimeFormat;
    delete myPythonToolBarShellNavigation;
    delete myPythonToolBarShellModes;
    delete myPythonToolBarShellIntervalBar;
    // recalc top dop after deleting elements
    myApplicationWindow->getTopDock()->recalc();
}

// ===========================================================================
// GNEApplicationWindowHelper::MenuBarFile method definitions
// ===========================================================================

GNEApplicationWindowHelper::MenuBarFile::MenuBarFile(GNEApplicationWindow* applicationWindow) :
    myRecentNetworks(applicationWindow->getApp(), TL("nets")),
    myRecentConfigs(applicationWindow->getApp(), TL("configs")),
    myApplicationWindow(applicationWindow) {
}


void
GNEApplicationWindowHelper::MenuBarFile::buildRecentNetworkFiles(FXMenuPane* fileMenu, FXMenuPane* fileMenuRecentNetworkFiles) {
    // for whatever reason, sonar complains in the next line that sep1 may leak, but fox does the cleanup
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentNetworkFiles, "", &myRecentNetworks, FXRecentFiles::ID_FILE_1);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentNetworkFiles, "", &myRecentNetworks, FXRecentFiles::ID_FILE_2);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentNetworkFiles, "", &myRecentNetworks, FXRecentFiles::ID_FILE_3);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentNetworkFiles, "", &myRecentNetworks, FXRecentFiles::ID_FILE_4);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentNetworkFiles, "", &myRecentNetworks, FXRecentFiles::ID_FILE_5);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentNetworkFiles, "", &myRecentNetworks, FXRecentFiles::ID_FILE_6);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentNetworkFiles, "", &myRecentNetworks, FXRecentFiles::ID_FILE_7);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentNetworkFiles, "", &myRecentNetworks, FXRecentFiles::ID_FILE_8);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentNetworkFiles, "", &myRecentNetworks, FXRecentFiles::ID_FILE_9);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentNetworkFiles, "", &myRecentNetworks, FXRecentFiles::ID_FILE_10);
    new FXMenuSeparator(fileMenuRecentNetworkFiles);  // NOSONAR, Fox does the cleanup
    GUIDesigns::buildFXMenuCommand(fileMenuRecentNetworkFiles, TL("Cl&ear Recent Networks"), nullptr, &myRecentNetworks, FXRecentFiles::ID_CLEAR);
    GUIDesigns::buildFXMenuCommand(fileMenuRecentNetworkFiles, TL("No Recent Networks"), nullptr, &myRecentNetworks, MFXRecentNetworks::ID_NOFILES);
    // set target
    myRecentNetworks.setTarget(myApplicationWindow);
    myRecentNetworks.setSelector(MID_RECENTFILE);
    new FXMenuCascade(fileMenu, TL("Recent Networks"), nullptr, fileMenuRecentNetworkFiles);
}


void
GNEApplicationWindowHelper::MenuBarFile::buildRecentConfigFiles(FXMenuPane* fileMenu, FXMenuPane* fileMenuRecentConfigFiles) {
    // for whatever reason, sonar complains in the next line that sep1 may leak, but fox does the cleanup
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentConfigFiles, "", &myRecentConfigs, FXRecentFiles::ID_FILE_1);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentConfigFiles, "", &myRecentConfigs, FXRecentFiles::ID_FILE_2);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentConfigFiles, "", &myRecentConfigs, FXRecentFiles::ID_FILE_3);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentConfigFiles, "", &myRecentConfigs, FXRecentFiles::ID_FILE_4);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentConfigFiles, "", &myRecentConfigs, FXRecentFiles::ID_FILE_5);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentConfigFiles, "", &myRecentConfigs, FXRecentFiles::ID_FILE_6);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentConfigFiles, "", &myRecentConfigs, FXRecentFiles::ID_FILE_7);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentConfigFiles, "", &myRecentConfigs, FXRecentFiles::ID_FILE_8);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentConfigFiles, "", &myRecentConfigs, FXRecentFiles::ID_FILE_9);
    GUIDesigns::buildFXMenuCommandRecentFile(fileMenuRecentConfigFiles, "", &myRecentConfigs, FXRecentFiles::ID_FILE_10);
    new FXMenuSeparator(fileMenuRecentConfigFiles);  // NOSONAR, Fox does the cleanup
    GUIDesigns::buildFXMenuCommand(fileMenuRecentConfigFiles, TL("Cl&ear Recent Configs"), nullptr, &myRecentConfigs, FXRecentFiles::ID_CLEAR);
    GUIDesigns::buildFXMenuCommand(fileMenuRecentConfigFiles, TL("No Recent Configs"), nullptr, &myRecentConfigs, MFXRecentNetworks::ID_NOFILES);
    // set target
    myRecentConfigs.setTarget(myApplicationWindow);
    myRecentConfigs.setSelector(MID_RECENTFILE);
    new FXMenuCascade(fileMenu, TL("Recent Configs"), nullptr, fileMenuRecentConfigFiles);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::FileMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::FileMenuCommands::FileMenuCommands(GNEApplicationWindow* applicationWindow) :
    myApplicationWindow(applicationWindow) {
}


void
GNEApplicationWindowHelper::FileMenuCommands::buildFileMenuCommands(FXMenuPane* fileMenu, FXMenuPane* fileMenuNeteditConfig,
        FXMenuPane* fileMenuSumoConfig, FXMenuPane* fileMenuTLS, FXMenuPane* fileMenuEdgeTypes, FXMenuPane* fileMenuAdditionals,
        FXMenuPane* fileMenuDemandElements, FXMenuPane* fileMenuDataElements, FXMenuPane* fileMenuMeanDataElements) {

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("New Network"), "Ctrl+N", TL("Create a new network."),
                                           GUIIconSubSys::getIcon(GUIIcon::NEW_NET), myApplicationWindow, MID_HOTKEY_CTRL_N_OPENNETWORK_NEWNETWORK);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("New Window"), "Ctrl+Shift+N", TL("Open a new netedit window."),
                                           nullptr, myApplicationWindow, MID_HOTKEY_CTRL_SHIFT_N_NEWWINDOW);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Generate Network"), "", TL("Create network using netgenerate."),
                                           GUIIconSubSys::getIcon(GUIIcon::NETGENERATE), myApplicationWindow, MID_GNE_NETGENERATE);

    // load sumo/netedit configs
    new FXMenuSeparator(fileMenu);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Load Netedit config..."), "Ctrl+E", TL("Load a netedit configuration file."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_NETEDITCONFIG), myApplicationWindow, MID_HOTKEY_CTRL_E_EDITSELECTION_LOADNETEDITCONFIG);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Load Sumo config..."), "Ctrl+M", TL("Load a SUMO configuration file."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_SUMOCONFIG), myApplicationWindow, MID_HOTKEY_CTRL_M_OPENSUMOCONFIG);

    // load networks
    new FXMenuSeparator(fileMenu);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Open Network..."), "Ctrl+O", TL("Open a SUMO network."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_NET), myApplicationWindow, MID_HOTKEY_CTRL_O_OPENSIMULATION_OPENNETWORK);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Open Netconvert Configuration..."), "Ctrl+Shift+O", TL("Open a configuration file with NETCONVERT options."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN), myApplicationWindow, MID_HOTKEY_CTRL_SHIFT_O_OPENNETCONVERTFILE);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Import Foreign Network..."), "", TL("Import a foreign network such as OSM."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_NET), myApplicationWindow, MID_GNE_TOOLBARFILE_OPENFOREIGN);

    // save networks
    new FXMenuSeparator(fileMenu);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Save Network"), "Ctrl+S", TL("Save the network."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_NETWORKELEMENTS), myApplicationWindow, MID_HOTKEY_CTRL_S_STOPSIMULATION_SAVENETWORK);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Save Network As..."), "", TL("Save the network to another file."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_NETWORKELEMENTS), myApplicationWindow, MID_GNE_TOOLBARFILE_SAVENETWORK_AS);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Save Plain XML..."), "Ctrl+L", TL("Save a plain XML representation of the network."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE), myApplicationWindow, MID_HOTKEY_CTRL_L_SAVEASPLAINXML);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Save Joined Junctions..."), "", TL("Save log of joined junctions (allows reproduction of joins)."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE), myApplicationWindow, MID_GNE_SAVEJOINEDJUNCTIONS);

    // reload
    new FXMenuSeparator(fileMenu);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Reload"), "Ctrl+R", TL("Reload the network."),
                                           GUIIconSubSys::getIcon(GUIIcon::RELOAD), myApplicationWindow, MID_HOTKEY_CTRL_R_RELOAD);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Reload Network"), "", TL("Reload the network."),
                                           GUIIconSubSys::getIcon(GUIIcon::RELOAD), myApplicationWindow, MID_GNE_TOOLBARFILE_RELOADNETWORK);

    new FXMenuSeparator(fileMenu);

    // create netedit config menu options
    buildNeteditConfigSection(fileMenuNeteditConfig);
    myNeteditConfigMenuCascade = new FXMenuCascade(fileMenu, TL("Netedit Config"), GUIIconSubSys::getIcon(GUIIcon::NETEDIT_MINI), fileMenuNeteditConfig);

    // create SumoConfig menu options
    buildSumoConfigSection(fileMenuSumoConfig);
    mySumoConfigMenuCascade = new FXMenuCascade(fileMenu, TL("Sumo Config"), GUIIconSubSys::getIcon(GUIIcon::SUMO_MINI), fileMenuSumoConfig);

    // create TLS menu options
    buildTrafficLightSection(fileMenuTLS);
    myTLSMenuCascade = new FXMenuCascade(fileMenu, TL("Traffic Lights"), GUIIconSubSys::getIcon(GUIIcon::MODETLS), fileMenuTLS);

    // create edgeTypes menu options
    buildEdgeTypeSection(fileMenuEdgeTypes);
    myEdgeTypesMenuCascade = new FXMenuCascade(fileMenu, TL("Edge Types"), GUIIconSubSys::getIcon(GUIIcon::MODECREATEEDGE), fileMenuEdgeTypes);

    // create Additionals menu options
    buildAdditionalSection(fileMenuAdditionals);
    myAdditionalMenuCascade = new FXMenuCascade(fileMenu, TL("Additionals and Shapes"), GUIIconSubSys::getIcon(GUIIcon::MODEADDITIONAL), fileMenuAdditionals);

    // create DemandElements menu options
    buildDemandSection(fileMenuDemandElements);
    myDemandMenuCascade = new FXMenuCascade(fileMenu, TL("Demand Elements"), GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDEMAND), fileMenuDemandElements);

    // create DataElements menu options
    buildDataSection(fileMenuDataElements);
    myDataMenuCascade = new FXMenuCascade(fileMenu, TL("Data Elements"), GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDATA), fileMenuDataElements);

    // create MeanDatas menu options
    buildMeanDataSection(fileMenuMeanDataElements);
    myMeanDataMenuCascade = new FXMenuCascade(fileMenu, TL("MeanData Elements"), GUIIconSubSys::getIcon(GUIIcon::MODEMEANDATA), fileMenuMeanDataElements);

    // add checkBox for quick load
    new FXMenuSeparator(fileMenu);
    menuCheckAllowUndoRedoLoading = GUIDesigns::buildFXMenuCheckbox(fileMenu,
                                    TL("Allow undoing element loading"), TL("Allow to undo loading of elements (Slow if a lot of elements are loaded)"),
                                    myApplicationWindow, MID_GNE_TOGGLE_UNDOREDO_LOADING);
    // set default value
    menuCheckAllowUndoRedoLoading->setCheck(myApplicationWindow->getApp()->reg().readBoolEntry("NETEDIT", "AllowUndoRedoLoading", true));

    // close network
    new FXMenuSeparator(fileMenu);
    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Close"), "Ctrl+W", TL("Close the network."),
                                           GUIIconSubSys::getIcon(GUIIcon::CLOSE), myApplicationWindow, MID_HOTKEY_CTRL_W_CLOSESIMULATION);
}


void
GNEApplicationWindowHelper::FileMenuCommands::enableMenuCascades() {
    mySumoConfigMenuCascade->enable();
    myNeteditConfigMenuCascade->enable();
    myTLSMenuCascade->enable();
    myEdgeTypesMenuCascade->enable();
    myAdditionalMenuCascade->enable();
    myDemandMenuCascade->enable();
    myDataMenuCascade->enable();
    myMeanDataMenuCascade->enable();
}


void
GNEApplicationWindowHelper::FileMenuCommands::disableMenuCascades() {
    mySumoConfigMenuCascade->disable();
    myNeteditConfigMenuCascade->disable();
    myTLSMenuCascade->disable();
    myEdgeTypesMenuCascade->disable();
    myAdditionalMenuCascade->disable();
    myDemandMenuCascade->disable();
    myDataMenuCascade->disable();
    myMeanDataMenuCascade->disable();
}


void
GNEApplicationWindowHelper::FileMenuCommands::setDefaultView() {
    myTLSMenuCascade->show();
    myDataMenuCascade->show();
    myMeanDataMenuCascade->show();
}


void
GNEApplicationWindowHelper::FileMenuCommands::setJuPedSimView() {
    myTLSMenuCascade->hide();
    myDataMenuCascade->hide();
    myMeanDataMenuCascade->hide();
}


void
GNEApplicationWindowHelper::FileMenuCommands::buildNeteditConfigSection(FXMenuPane* menuPane) {
    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save Netedit Config"), "Ctrl+Shift+E", TL("Save netedit configuration file."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_NETEDITCONFIG), myApplicationWindow, MID_HOTKEY_CTRL_SHIFT_E_SAVENETEDITCONFIG);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save Netedit Config As..."), "", TL("Save netedit configuration in a new file."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_NETEDITCONFIG), myApplicationWindow, MID_GNE_TOOLBARFILE_SAVENETEDITCONFIG_AS);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Reload Netedit Config"), "", TL("Reload netedit configuration."),
                                           GUIIconSubSys::getIcon(GUIIcon::RELOAD), myApplicationWindow, MID_GNE_TOOLBARFILE_RELOAD_NETEDITCONFIG);
}


void
GNEApplicationWindowHelper::FileMenuCommands::buildSumoConfigSection(FXMenuPane* menuPane) {
    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save Sumo Config"), "Ctrl+Shift+S", TL("Save sumo configuration file."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_SUMOCONFIG), myApplicationWindow, MID_HOTKEY_CTRL_SHIFT_S_SAVESUMOCONFIG);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save Sumo Config As..."), "", TL("Save sumo configuration in a new file."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_SUMOCONFIG), myApplicationWindow, MID_GNE_TOOLBARFILE_SAVESUMOCONFIG_AS);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Reload Sumo Config"), "", TL("Reload sumo configuration."),
                                           GUIIconSubSys::getIcon(GUIIcon::RELOAD), myApplicationWindow, MID_GNE_TOOLBARFILE_RELOAD_SUMOCONFIG);
}


void
GNEApplicationWindowHelper::FileMenuCommands::buildTrafficLightSection(FXMenuPane* menuPane) {
    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Load TLS Programs..."), "Ctrl+K", TL("Load programs for traffic lights in the current net."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_TLSPROGRAMS), myApplicationWindow, MID_HOTKEY_CTRL_K_OPENTLSPROGRAMS);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save TLS Programs"), "Ctrl+Shift+K", TL("Save all traffic light programs of the current network."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE), myApplicationWindow, MID_HOTKEY_CTRL_SHIFT_K_SAVETLS);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save TLS Programs As..."), "", TL("Save all traffic light programs of the current network to another file."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE), myApplicationWindow, MID_GNE_TOOLBARFILE_SAVETLSPROGRAMS_AS);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Reload TLS Programs"), "", TL("Reload TLS Programs."),
                                           GUIIconSubSys::getIcon(GUIIcon::RELOAD), myApplicationWindow, MID_GNE_TOOLBARFILE_RELOAD_TLSPROGRAMS);
}


void
GNEApplicationWindowHelper::FileMenuCommands::buildEdgeTypeSection(FXMenuPane* menuPane) {
    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Load Edge Types..."), "Ctrl+H", TL("Load edge types in the current net."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_TLSPROGRAMS), myApplicationWindow, MID_HOTKEY_CTRL_H_APPSETTINGS_OPENEDGETYPES);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save Edge Types"), "Ctrl+Shift+H", TL("Save edge types of the current net."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE), myApplicationWindow, MID_HOTKEY_CTRL_SHIFT_H_SAVEEDGETYPES);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save Edge Types As..."), "", TL("Save edge types of the current net to another file."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE), myApplicationWindow, MID_GNE_TOOLBARFILE_SAVEEDGETYPES_AS);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Reload edge types"), "", TL("Reload edge types."),
                                           GUIIconSubSys::getIcon(GUIIcon::RELOAD), myApplicationWindow, MID_GNE_TOOLBARFILE_RELOAD_EDGETYPES);
}


void
GNEApplicationWindowHelper::FileMenuCommands::buildAdditionalSection(FXMenuPane* menuPane) {
    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Load Additionals..."), "Ctrl+A", TL("Load additionals and shapes."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_ADDITIONALS), myApplicationWindow, MID_HOTKEY_CTRL_A_STARTSIMULATION_OPENADDITIONALELEMENTS);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save Additionals"), "Ctrl+Shift+A", TL("Save additionals and shapes."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_ADDITIONALELEMENTS), myApplicationWindow, MID_HOTKEY_CTRL_SHIFT_A_SAVEADDITIONALELEMENTS);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save Additionals unified as..."), "", TL("Save all additional elements to the chosen file."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_ADDITIONALELEMENTS), myApplicationWindow, MID_GNE_TOOLBARFILE_SAVEADDITIONALELEMENTS_UNIFIED);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save JuPedSim Elements as..."), "", TL("Save JuPedSim elements in a separated file."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_JUPEDSIMELEMENTS), myApplicationWindow, MID_GNE_TOOLBARFILE_SAVEJUPEDSIMELEMENTS_AS);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Reload Additionals"), "", TL("Reload additionals."),
                                           GUIIconSubSys::getIcon(GUIIcon::RELOAD), myApplicationWindow, MID_GNE_TOOLBARFILE_RELOAD_ADDITIONALELEMENTS);
}


void
GNEApplicationWindowHelper::FileMenuCommands::buildDemandSection(FXMenuPane* menuPane) {
    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Load Demand Elements..."), "Ctrl+D", TL("Load demand elements."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_ADDITIONALS), myApplicationWindow, MID_HOTKEY_CTRL_D_SINGLESIMULATIONSTEP_OPENDEMANDELEMENTS);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save Demand Elements"), "Ctrl+Shift+D", TL("Save demand elements."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_DEMANDELEMENTS), myApplicationWindow, MID_HOTKEY_CTRL_SHIFT_D_SAVEDEMANDELEMENTS);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save Demand Elements unified as..."), "", TL("Save all demand elements to the chosen file"),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_DEMANDELEMENTS), myApplicationWindow, MID_GNE_TOOLBARFILE_SAVEDEMANDELEMENTS_UNIFIED);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Reload Demand Elements"), "", TL("Reload demand elements."),
                                           GUIIconSubSys::getIcon(GUIIcon::RELOAD), myApplicationWindow, MID_GNE_TOOLBARFILE_RELOAD_DEMANDELEMENTS);
}


void
GNEApplicationWindowHelper::FileMenuCommands::buildDataSection(FXMenuPane* menuPane) {
    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Load Data Elements..."), "Ctrl+B", TL("Load data elements."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_ADDITIONALS), myApplicationWindow, MID_HOTKEY_CTRL_B_EDITBREAKPOINT_OPENDATAELEMENTS);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save Data Elements"), "Ctrl+Shift+B", TL("Save data elements."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_DATAELEMENTS), myApplicationWindow, MID_HOTKEY_CTRL_SHIFT_B_SAVEDATAELEMENTS);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save Data Elements unified as..."), "", TL("Save all data elements to the chosen file."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_DATAELEMENTS), myApplicationWindow, MID_GNE_TOOLBARFILE_SAVEDATAELEMENTS_UNIFIED);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Reload Data Elements"), "", TL("Reload data elements."),
                                           GUIIconSubSys::getIcon(GUIIcon::RELOAD), myApplicationWindow, MID_GNE_TOOLBARFILE_RELOAD_DATAELEMENTS);
}


void
GNEApplicationWindowHelper::FileMenuCommands::buildMeanDataSection(FXMenuPane* menuPane) {
    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Load MeanDatas..."), "", TL("Load meanDatas and shapes."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_MEANDATAS), myApplicationWindow, MID_GNE_TOOLBARFILE_OPENMEANDATAELEMENTS);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save MeanDatas"), "", TL("Save meanDatas and shapes."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_MEANDATAELEMENTS), myApplicationWindow, MID_HOTKEY_CTRL_SHIFT_M_SAVEMEANDATAELEMENTS);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save MeanDatas unified as..."), "Ctrl+Shift+M", TL("Save all meanData elements to the chosen file."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_MEANDATAELEMENTS), myApplicationWindow, MID_GNE_TOOLBARFILE_SAVEMEANDATAELEMENTS_UNIFIED);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Reload MeanDatas"), "", TL("Reload meanDatas."),
                                           GUIIconSubSys::getIcon(GUIIcon::RELOAD), myApplicationWindow, MID_GNE_TOOLBARFILE_RELOAD_MEANDATAELEMENTS);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::ModesMenuCommands::CommonMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::ModesMenuCommands::CommonMenuCommands::CommonMenuCommands(const ModesMenuCommands* modesMenuCommandsParent) :
    myModesMenuCommandsParent(modesMenuCommandsParent) {
}


void
GNEApplicationWindowHelper::ModesMenuCommands::CommonMenuCommands::buildCommonMenuCommands(FXMenuPane* modesMenu) {
    // build every FXMenuCommand giving it a shortcut
    inspectMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                  TL("&Inspect"), "I", TL("Inspect elements and change their attributes."),
                  GUIIconSubSys::getIcon(GUIIcon::MODEINSPECT), myModesMenuCommandsParent->myApplicationWindow, MID_HOTKEY_I_MODE_INSPECT);
    deleteMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                 TL("&Delete"), "D", TL("Delete elements."),
                 GUIIconSubSys::getIcon(GUIIcon::MODEDELETE), myModesMenuCommandsParent->myApplicationWindow, MID_HOTKEY_D_MODE_SINGLESIMULATIONSTEP_DELETE);
    selectMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                 TL("&Select"), "S", TL("Select elements."),
                 GUIIconSubSys::getIcon(GUIIcon::MODESELECT), myModesMenuCommandsParent->myApplicationWindow, MID_HOTKEY_S_MODE_STOPSIMULATION_SELECT);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::ModesMenuCommands::NetworkMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::ModesMenuCommands::NetworkMenuCommands::NetworkMenuCommands(const ModesMenuCommands* modesMenuCommandsParent) :
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
    decalMode->show();
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
    decalMode->hide();
}


void
GNEApplicationWindowHelper::ModesMenuCommands::NetworkMenuCommands::buildNetworkMenuCommands(FXMenuPane* modesMenu) {
    // build every FXMenuCommand giving it a shortcut
    moveMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
               TL("&Move"), "M", TL("Move elements."),
               GUIIconSubSys::getIcon(GUIIcon::MODEMOVE), myModesMenuCommandsParent->myApplicationWindow,
               MID_HOTKEY_M_MODE_MOVE_MEANDATA);
    createEdgeMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                     TL("&Edge"), "E", TL("Create junction and edges."),
                     GUIIconSubSys::getIcon(GUIIcon::MODECREATEEDGE), myModesMenuCommandsParent->myApplicationWindow,
                     MID_HOTKEY_E_MODE_EDGE_EDGEDATA);
    TLSMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
              TL("&Traffic light"), "T", TL("Modes traffic lights over junctions."),
              GUIIconSubSys::getIcon(GUIIcon::MODETLS), myModesMenuCommandsParent->myApplicationWindow,
              MID_HOTKEY_T_MODE_TLS_TYPE);
    connectMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                  TL("&Connection"), "C", TL("Modes connections between lanes."),
                  GUIIconSubSys::getIcon(GUIIcon::MODECONNECTION), myModesMenuCommandsParent->myApplicationWindow,
                  MID_HOTKEY_C_MODE_CONNECT_CONTAINER);
    prohibitionMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                      TL("Pro&hibition"), "H", TL("Modes connection prohibitions."),
                      GUIIconSubSys::getIcon(GUIIcon::MODEPROHIBITION), myModesMenuCommandsParent->myApplicationWindow,
                      MID_HOTKEY_H_MODE_PROHIBITION_CONTAINERPLAN);
    crossingMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                   TL("C&rossing"), "R", TL("Create crossings between edges."),
                   GUIIconSubSys::getIcon(GUIIcon::MODECROSSING), myModesMenuCommandsParent->myApplicationWindow,
                   MID_HOTKEY_R_MODE_CROSSING_ROUTE_EDGERELDATA);
    additionalMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                     TL("&Additional"), "A", TL("Create additional elements."),
                     GUIIconSubSys::getIcon(GUIIcon::MODEADDITIONAL), myModesMenuCommandsParent->myApplicationWindow,
                     MID_HOTKEY_A_MODE_STARTSIMULATION_ADDITIONALS_STOPS);
    wireMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
               TL("&Wire"), "W", TL("Create wires."),
               GUIIconSubSys::getIcon(GUIIcon::MODEWIRE), myModesMenuCommandsParent->myApplicationWindow,
               MID_HOTKEY_W_MODE_WIRE_ROUTEDISTRIBUTION);
    TAZMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
              TL("TA&Z"), "Z", TL("Create Traffic Assignment Zones."),
              GUIIconSubSys::getIcon(GUIIcon::MODETAZ), myModesMenuCommandsParent->myApplicationWindow,
              MID_HOTKEY_Z_MODE_TAZ_TAZREL);
    shapeMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                TL("&POI-Poly"), "P", TL("Create Points-Of-Interest and polygons."),
                GUIIconSubSys::getIcon(GUIIcon::MODESHAPE), myModesMenuCommandsParent->myApplicationWindow,
                MID_HOTKEY_P_MODE_POLYGON_PERSON);
    decalMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                TL("Deca&ls"), "U", TL("Create decals."),
                GUIIconSubSys::getIcon(GUIIcon::MODEDECAL), myModesMenuCommandsParent->myApplicationWindow,
                MID_HOTKEY_U_MODE_DECAL_TYPEDISTRIBUTION);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::ModesMenuCommands::DemandMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::ModesMenuCommands::DemandMenuCommands::DemandMenuCommands(const ModesMenuCommands* modesMenuCommandsParent) :
    myModesMenuCommandsParent(modesMenuCommandsParent) {
}


void
GNEApplicationWindowHelper::ModesMenuCommands::DemandMenuCommands::showDemandMenuCommands() {
    moveMode->show();
    routeMode->show();
    routeDistributionMode->show();
    vehicleMode->show();
    typeMode->show();
    typeDistributionMode->show();
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
    routeDistributionMode->hide();
    vehicleMode->hide();
    typeMode->hide();
    typeDistributionMode->hide();
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
               TL("&Move"), "M", TL("Move elements."),
               GUIIconSubSys::getIcon(GUIIcon::MODEMOVE), myModesMenuCommandsParent->myApplicationWindow, MID_HOTKEY_M_MODE_MOVE_MEANDATA);
    routeMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                TL("Route"), "R", TL("Create Routes."),
                GUIIconSubSys::getIcon(GUIIcon::MODEROUTE), myModesMenuCommandsParent->myApplicationWindow, MID_HOTKEY_R_MODE_CROSSING_ROUTE_EDGERELDATA);
    routeDistributionMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                            TL("Route distribution"), "W", TL("Create and edit route distributions."),
                            GUIIconSubSys::getIcon(GUIIcon::MODEROUTEDISTRIBUTION), myModesMenuCommandsParent->myApplicationWindow,
                            MID_HOTKEY_W_MODE_WIRE_ROUTEDISTRIBUTION);
    vehicleMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                  TL("Vehicle"), "V", TL("Create vehicles."),
                  GUIIconSubSys::getIcon(GUIIcon::MODEVEHICLE), myModesMenuCommandsParent->myApplicationWindow, MID_HOTKEY_V_MODE_VEHICLE);
    typeMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
               TL("Type"), "T", TL("Create types (vehicles, person and containers)."),
               GUIIconSubSys::getIcon(GUIIcon::MODETYPE), myModesMenuCommandsParent->myApplicationWindow, MID_HOTKEY_T_MODE_TLS_TYPE);
    typeDistributionMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                           TL("TypeDistribution"), "U", TL("Create and edit type distributions."),
                           GUIIconSubSys::getIcon(GUIIcon::MODETYPEDISTRIBUTION), myModesMenuCommandsParent->myApplicationWindow, MID_HOTKEY_U_MODE_DECAL_TYPEDISTRIBUTION);
    stopMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
               TL("Stop"), "A", TL("Create stops."),
               GUIIconSubSys::getIcon(GUIIcon::MODESTOP), myModesMenuCommandsParent->myApplicationWindow, MID_HOTKEY_A_MODE_STARTSIMULATION_ADDITIONALS_STOPS);
    personMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                 TL("Person"), "P", TL("Create persons."),
                 GUIIconSubSys::getIcon(GUIIcon::MODEPERSON), myModesMenuCommandsParent->myApplicationWindow, MID_HOTKEY_P_MODE_POLYGON_PERSON);
    personPlanMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                     TL("Person plan"), "L", TL("Create person plans."),
                     GUIIconSubSys::getIcon(GUIIcon::MODEPERSONPLAN), myModesMenuCommandsParent->myApplicationWindow, MID_HOTKEY_L_MODE_PERSONPLAN);
    containerMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                    TL("Container"), "C", TL("Create containers."),
                    GUIIconSubSys::getIcon(GUIIcon::MODECONTAINER), myModesMenuCommandsParent->myApplicationWindow, MID_HOTKEY_C_MODE_CONNECT_CONTAINER);
    containerPlanMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                        TL("Container plan"), "H", TL("Create container plans."),
                        GUIIconSubSys::getIcon(GUIIcon::MODECONTAINERPLAN), myModesMenuCommandsParent->myApplicationWindow, MID_HOTKEY_H_MODE_PROHIBITION_CONTAINERPLAN);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::ModesMenuCommands::DataMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::ModesMenuCommands::DataMenuCommands::DataMenuCommands(const ModesMenuCommands* modesMenuCommandsParent) :
    myModesMenuCommandsParent(modesMenuCommandsParent) {
}


void
GNEApplicationWindowHelper::ModesMenuCommands::DataMenuCommands::showDataMenuCommands() {
    edgeData->show();
    edgeRelData->show();
    TAZRelData->show();
    meanData->show();
}


void
GNEApplicationWindowHelper::ModesMenuCommands::DataMenuCommands::hideDataMenuCommands() {
    edgeData->hide();
    edgeRelData->hide();
    TAZRelData->hide();
    meanData->hide();
}


void
GNEApplicationWindowHelper::ModesMenuCommands::DataMenuCommands::buildDataMenuCommands(FXMenuPane* modesMenu) {
    // build every FXMenuCommand giving it a shortcut
    edgeData = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
               TL("&EdgeData"), "E", TL("Create edgeData elements."),
               GUIIconSubSys::getIcon(GUIIcon::MODEEDGEDATA), myModesMenuCommandsParent->myApplicationWindow, MID_HOTKEY_E_MODE_EDGE_EDGEDATA);
    edgeRelData = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                  TL("Edge&Relation"), "R", TL("Create edgeRelation elements."),
                  GUIIconSubSys::getIcon(GUIIcon::MODEEDGERELDATA), myModesMenuCommandsParent->myApplicationWindow, MID_HOTKEY_R_MODE_CROSSING_ROUTE_EDGERELDATA);
    TAZRelData = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                 TL("TA&ZRelation"), "Z", TL("Create TAZRelation elements."),
                 GUIIconSubSys::getIcon(GUIIcon::MODETAZRELDATA), myModesMenuCommandsParent->myApplicationWindow, MID_HOTKEY_Z_MODE_TAZ_TAZREL);
    meanData = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
               TL("&MeanData"), "M", TL("Create MeanData edge/lanes."),
               GUIIconSubSys::getIcon(GUIIcon::MODEMEANDATA), myModesMenuCommandsParent->myApplicationWindow, MID_HOTKEY_M_MODE_MOVE_MEANDATA);
}


// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::ModesMenuCommands - methods
// ---------------------------------------------------------------------------
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4355) // mask warning about "this" in initializers
#endif
GNEApplicationWindowHelper::ModesMenuCommands::ModesMenuCommands(GNEApplicationWindow* applicationWindow) :
    commonMenuCommands(this),
    networkMenuCommands(this),
    demandMenuCommands(this),
    dataMenuCommands(this),
    myApplicationWindow(applicationWindow) {
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif


void
GNEApplicationWindowHelper::ModesMenuCommands::buildModesMenuCommands(FXMenuPane* modesMenu) {
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


void
GNEApplicationWindowHelper::ModesMenuCommands::setDefaultView(Supermode supermode) {
    if (supermode == Supermode::NETWORK) {
        networkMenuCommands.showNetworkMenuCommands();
    } else if (supermode == Supermode::DEMAND) {
        demandMenuCommands.showDemandMenuCommands();
    } else if (supermode == Supermode::DATA) {
        dataMenuCommands.showDataMenuCommands();
    }
}


void
GNEApplicationWindowHelper::ModesMenuCommands::setJuPedSimView(Supermode supermode) {
    if (supermode == Supermode::NETWORK) {
        networkMenuCommands.connectMode->hide();
        networkMenuCommands.TLSMode->hide();
        networkMenuCommands.prohibitionMode->hide();
        networkMenuCommands.wireMode->hide();
        networkMenuCommands.decalMode->hide();
    } else if (supermode == Supermode::DEMAND) {
        demandMenuCommands.moveMode->hide();
        demandMenuCommands.vehicleMode->hide();
        demandMenuCommands.stopMode->hide();
        demandMenuCommands.containerMode->hide();
        demandMenuCommands.containerPlanMode->hide();
    } else if (supermode == Supermode::DATA) {
        dataMenuCommands.hideDataMenuCommands();
    }
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::EditMenuCommands::NetworkViewOptions - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::EditMenuCommands::NetworkViewOptions::NetworkViewOptions(GNEApplicationWindow* applicationWindow) :
    myApplicationWindow(applicationWindow) {
}


void
GNEApplicationWindowHelper::EditMenuCommands::NetworkViewOptions::buildNetworkViewOptionsMenuChecks(FXMenuPane* editMenu) {
    // create menu checks
    menuCheckToggleGrid = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                          TL("Show grid"), "Ctrl+G or Alt+1", "",
                          GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_TOGGLEGRID),
                          myApplicationWindow, MID_GNE_NETWORKVIEWOPTIONS_TOGGLEGRID);

    menuCheckToggleDrawJunctionShape = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                       TL("Hide junction shape"), "Ctrl+J or Alt+2", "",
                                       GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_TOGGLEDRAWJUNCTIONSHAPE),
                                       myApplicationWindow, MID_GNE_NETWORKVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE);

    menuCheckDrawSpreadVehicles = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                  TL("Draw vehicles spread in lane or in depart position"), "Alt+3", "",
                                  GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_SPREADVEHICLE),
                                  myApplicationWindow, MID_GNE_NETWORKVIEWOPTIONS_DRAWSPREADVEHICLES);

    menuCheckShowDemandElements = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                  TL("Show demand elements"), "Alt+4", "",
                                  GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_SHOWDEMANDELEMENTS),
                                  myApplicationWindow, MID_GNE_NETWORKVIEWOPTIONS_SHOWDEMANDELEMENTS);

    menuCheckSelectEdges = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                           TL("Clicking should target lanes"), "Alt+5", "",
                           GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_SELECTEDGES),
                           myApplicationWindow, MID_GNE_NETWORKVIEWOPTIONS_SELECTEDGES);

    menuCheckShowConnections = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                               TL("Show connections over junctions"), "Alt+6", "",
                               GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_SHOWCONNECTIONS),
                               myApplicationWindow, MID_GNE_NETWORKVIEWOPTIONS_SHOWCONNECTIONS);

    menuCheckHideConnections = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                               TL("Hide connections"), "Alt+7", "",
                               GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_HIDECONNECTIONS),
                               myApplicationWindow, MID_GNE_NETWORKVIEWOPTIONS_HIDECONNECTIONS);

    menuCheckShowAdditionalSubElements = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                         TL("Show sub-additional elements"), "Alt+8", "",
                                         GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_SHOWSUBADDITIONALS),
                                         myApplicationWindow, MID_GNE_NETWORKVIEWOPTIONS_SHOWSUBADDITIONALS);

    menuCheckShowTAZElements = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                               TL("Show TAZ elements"), "Alt+9", "",
                               GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_SHOWTAZELEMENTS),
                               myApplicationWindow, MID_GNE_NETWORKVIEWOPTIONS_SHOWTAZELEMENTS);

    menuCheckExtendSelection = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                               TL("Selecting multiple edges automatically select their junctions"), "Alt+10", "",
                               GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_AUTOSELECTJUNCTIONS),
                               myApplicationWindow, MID_GNE_NETWORKVIEWOPTIONS_EXTENDSELECTION);

    menuCheckChangeAllPhases = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                               TL("Clicking should apply state changes to all phases of TLS plan"), "Alt+5", "",
                               GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_APPLYTOALLPHASES),
                               myApplicationWindow, MID_GNE_NETWORKVIEWOPTIONS_CHANGEALLPHASES);

    menuCheckMergeAutomatically = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                  TL("Don't ask for confirmation before merging junction"), "Alt+5", "",
                                  GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_MERGEAUTOMATICALLY),
                                  myApplicationWindow, MID_GNE_NETWORKVIEWOPTIONS_MERGEAUTOMATICALLY);

    menuCheckChainEdges = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                          TL("Create consecutive edges"), "Alt+5", "",
                          GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_CHAIN),
                          myApplicationWindow, MID_GNE_NETWORKVIEWOPTIONS_CHAINEDGES);

    menuCheckAutoOppositeEdge = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                TL("Create an edge in the opposite direction"), "Alt+6", "",
                                GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_TWOWAY),
                                myApplicationWindow, MID_GNE_NETWORKVIEWOPTIONS_AUTOOPPOSITEEDGES);

    menuCheckMoveElevation = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                             TL("Apply mouse movement to elevation"), "Alt+7", "",
                             GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_ELEVATION),
                             myApplicationWindow, MID_GNE_NETWORKVIEWOPTIONS_MOVEELEVATION);

    menuCheckShowJunctionBubble = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                  TL("Show bubbles over junctions shapes"), "Alt+8", "",
                                  GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_BUBBLES),
                                  myApplicationWindow, MID_GNE_NETWORKVIEWOPTIONS_SHOWBUBBLES);

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
    menuCheckMergeAutomatically->hide();
    menuCheckShowJunctionBubble->hide();
    menuCheckMoveElevation->hide();
    menuCheckChainEdges->hide();
    menuCheckAutoOppositeEdge->hide();
    separator->hide();
}


void
GNEApplicationWindowHelper::EditMenuCommands::NetworkViewOptions::updateShortcuts() {
    int index = 2;
    // update all accel text
    if (menuCheckDrawSpreadVehicles->shown()) {
        menuCheckDrawSpreadVehicles->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckDrawSpreadVehicles->shown()) {
        menuCheckDrawSpreadVehicles->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckShowDemandElements->shown()) {
        menuCheckShowDemandElements->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckSelectEdges->shown()) {
        menuCheckSelectEdges->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckShowConnections->shown()) {
        menuCheckShowConnections->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckHideConnections->shown()) {
        menuCheckHideConnections->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckShowAdditionalSubElements->shown()) {
        menuCheckShowAdditionalSubElements->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckShowTAZElements->shown()) {
        menuCheckShowTAZElements->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckExtendSelection->shown()) {
        menuCheckExtendSelection->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckChangeAllPhases->shown()) {
        menuCheckChangeAllPhases->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckMergeAutomatically->shown()) {
        menuCheckMergeAutomatically->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckChainEdges->shown()) {
        menuCheckChainEdges->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckAutoOppositeEdge->shown()) {
        menuCheckAutoOppositeEdge->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckMoveElevation->shown()) {
        menuCheckMoveElevation->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckShowJunctionBubble->shown()) {
        menuCheckShowJunctionBubble->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::DemandViewOptions - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::EditMenuCommands::DemandViewOptions::DemandViewOptions(GNEApplicationWindow* applicationWindow) :
    myApplicationWindow(applicationWindow) {
}


void
GNEApplicationWindowHelper::EditMenuCommands::DemandViewOptions::buildDemandViewOptionsMenuChecks(FXMenuPane* editMenu) {
    // create menu checks
    menuCheckToggleGrid = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                          TL("Show grid"), "Ctrl+G or Alt+1", "",
                          GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_TOGGLEGRID),
                          myApplicationWindow, MID_GNE_DEMANDVIEWOPTIONS_SHOWGRID);

    menuCheckToggleDrawJunctionShape = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                       TL("Hide junction shape"), "Ctrl+J or Alt+2", "",
                                       GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_TOGGLEDRAWJUNCTIONSHAPE),
                                       myApplicationWindow, MID_GNE_DEMANDVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE);

    menuCheckDrawSpreadVehicles = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                  TL("Draw vehicles spread/depart position"), "Alt+3", "",
                                  GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_SPREADVEHICLE),
                                  myApplicationWindow, MID_GNE_DEMANDVIEWOPTIONS_DRAWSPREADVEHICLES);

    menuCheckHideShapes = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                          TL("Show shapes"), "Alt+4", "",
                          GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_HIDESHAPES),
                          myApplicationWindow, MID_GNE_DEMANDVIEWOPTIONS_HIDESHAPES);

    menuCheckShowAllTrips = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                            TL("Show all trips"), "Alt+5", "",
                            GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_SHOWTRIPS),
                            myApplicationWindow, MID_GNE_DEMANDVIEWOPTIONS_SHOWTRIPS);

    menuCheckShowAllPersonPlans = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                  TL("Show all person plans"), "Alt+6", "",
                                  GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_SHOWPERSONPLANS),
                                  myApplicationWindow, MID_GNE_DEMANDVIEWOPTIONS_SHOWALLPERSONPLANS);

    menuCheckLockPerson = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                          TL("Lock selected person"), "Alt+7", "",
                          GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_LOCKPERSON),
                          myApplicationWindow, MID_GNE_DEMANDVIEWOPTIONS_LOCKPERSON);

    menuCheckShowAllContainerPlans = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                     TL("Show all container plans"), "Alt+6", "",
                                     GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_SHOWCONTAINERPLANS),
                                     myApplicationWindow, MID_GNE_DEMANDVIEWOPTIONS_SHOWALLCONTAINERPLANS);

    menuCheckLockContainer = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                             TL("Lock selected container"), "Alt+7", "",
                             GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_LOCKCONTAINER),
                             myApplicationWindow, MID_GNE_DEMANDVIEWOPTIONS_LOCKCONTAINER);

    menuCheckHideNonInspectedDemandElements = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
            TL("Show non-inspected demand elements"), "Alt+8", "",
            GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_HIDENONINSPECTEDDEMANDELEMENTS),
            myApplicationWindow, MID_GNE_DEMANDVIEWOPTIONS_HIDENONINSPECTED);

    menuCheckShowOverlappedRoutes = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                    TL("Show number of overlapped routes"), "Alt+9", "",
                                    GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_SHOWOVERLAPPEDROUTES),
                                    myApplicationWindow, MID_GNE_DEMANDVIEWOPTIONS_SHOWOVERLAPPEDROUTES);

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


void
GNEApplicationWindowHelper::EditMenuCommands::DemandViewOptions::updateShortcuts() {
    int index = 2;
    // update all accel text
    if (menuCheckDrawSpreadVehicles->shown()) {
        menuCheckDrawSpreadVehicles->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckHideShapes->shown()) {
        menuCheckHideShapes->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckShowAllTrips->shown()) {
        menuCheckShowAllTrips->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckHideNonInspectedDemandElements->shown()) {
        menuCheckHideNonInspectedDemandElements->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckShowOverlappedRoutes->shown()) {
        menuCheckShowOverlappedRoutes->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckShowAllPersonPlans->shown()) {
        menuCheckShowAllPersonPlans->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckLockPerson->shown()) {
        menuCheckLockPerson->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckShowAllContainerPlans->shown()) {
        menuCheckShowAllContainerPlans->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckLockContainer->shown()) {
        menuCheckLockContainer->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::EditMenuCommands::DataViewOptions - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::EditMenuCommands::DataViewOptions::DataViewOptions(GNEApplicationWindow* applicationWindow) :
    myApplicationWindow(applicationWindow) {
}


void
GNEApplicationWindowHelper::EditMenuCommands::DataViewOptions::buildDataViewOptionsMenuChecks(FXMenuPane* editMenu) {
    // create menu checks
    menuCheckToggleDrawJunctionShape = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                       TL("Hide junction shape"), "Ctrl+J or Alt+1", "",
                                       GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_TOGGLEDRAWJUNCTIONSHAPE),
                                       myApplicationWindow, MID_GNE_DEMANDVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE);

    menuCheckShowAdditionals = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                               TL("Show additionals"), "Alt+2", "",
                               GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_SHOWADDITIONALS),
                               myApplicationWindow, MID_GNE_DATAVIEWOPTIONS_SHOWADDITIONALS);

    menuCheckShowShapes = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                          TL("Show shapes"), "Alt+3", "",
                          GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_SHOWSHAPES),
                          myApplicationWindow, MID_GNE_DATAVIEWOPTIONS_SHOWSHAPES);

    menuCheckShowDemandElements = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                  TL("Show demand elements"), "Alt+4", "",
                                  GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_SHOWDEMANDELEMENTS),
                                  myApplicationWindow, MID_GNE_DATAVIEWOPTIONS_SHOWDEMANDELEMENTS);

    menuCheckToggleTAZRelDrawing = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                   TL("Draw TAZRel from center"), "Alt+5", "",
                                   GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_TAZRELDRAWING),
                                   myApplicationWindow, MID_GNE_DATAVIEWOPTIONS_TAZRELDRAWING);

    menuCheckToggleTAZDrawFill = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                 TL("Draw TAZ fill"), "Alt+6", "",
                                 GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_TAZDRAWFILL),
                                 myApplicationWindow, MID_GNE_DATAVIEWOPTIONS_TAZDRAWFILL);

    menuCheckToggleTAZRelOnlyFrom = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                    TL("Only draw TAZRel from"), "Alt+6", "",
                                    GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_TAZRELONLYFROM),
                                    myApplicationWindow, MID_GNE_DATAVIEWOPTIONS_TAZRELONLYFROM);

    menuCheckToggleTAZRelOnlyTo = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                  TL("Only draw TAZRel to"), "Alt+7", "",
                                  GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_TAZRELONLYTO),
                                  myApplicationWindow, MID_GNE_DATAVIEWOPTIONS_TAZRELONLYTO);
    // build separator
    separator = new FXMenuSeparator(editMenu);
}


void
GNEApplicationWindowHelper::EditMenuCommands::DataViewOptions::hideDataViewOptionsMenuChecks() {
    menuCheckToggleDrawJunctionShape->hide();
    menuCheckShowAdditionals->hide();
    menuCheckShowShapes->hide();
    menuCheckShowDemandElements->hide();
    menuCheckToggleTAZRelDrawing->hide();
    menuCheckToggleTAZDrawFill->hide();
    menuCheckToggleTAZRelOnlyFrom->hide();
    menuCheckToggleTAZRelOnlyTo->hide();
    separator->hide();
}


void
GNEApplicationWindowHelper::EditMenuCommands::DataViewOptions::updateShortcuts() {
    int index = 2;
    // update all accel text
    if (menuCheckShowAdditionals->shown()) {
        menuCheckShowAdditionals->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckShowShapes->shown()) {
        menuCheckShowShapes->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckShowDemandElements->shown()) {
        menuCheckShowDemandElements->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckToggleTAZRelDrawing->shown()) {
        menuCheckToggleTAZRelDrawing->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckToggleTAZDrawFill->shown()) {
        menuCheckToggleTAZDrawFill->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckToggleTAZRelOnlyFrom->shown()) {
        menuCheckToggleTAZRelOnlyFrom->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckToggleTAZRelOnlyTo->shown()) {
        menuCheckToggleTAZRelOnlyTo->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::EditMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::EditMenuCommands::EditMenuCommands(GNEApplicationWindow* applicationWindow) :
    networkViewOptions(applicationWindow),
    demandViewOptions(applicationWindow),
    dataViewOptions(applicationWindow),
    myApplicationWindow(applicationWindow) {
}


void
GNEApplicationWindowHelper::EditMenuCommands::buildUndoRedoMenuCommands(FXMenuPane* editMenu) {
    // build undo/redo command
    undoLastChange = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
                     TL("Undo"), "Ctrl+Z", TL("Undo the last change."),
                     GUIIconSubSys::getIcon(GUIIcon::UNDO), myApplicationWindow, MID_HOTKEY_CTRL_Z_UNDO);
    redoLastChange = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
                     TL("Redo"), "Ctrl+Y", TL("Redo the last change."),
                     GUIIconSubSys::getIcon(GUIIcon::REDO), myApplicationWindow, MID_HOTKEY_CTRL_Y_REDO);
    openUndolistDialog = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
                         TL("Show undo/redo history"), "", TL("Open undo/redo history dialog."),
                         GUIIconSubSys::getIcon(GUIIcon::UNDOLIST), myApplicationWindow, MID_GNE_UNDOLISTDIALOG);
    // add checkBox for recomputing in data mode
    menuCheckAllowUndoRedo = GUIDesigns::buildFXMenuCheckbox(editMenu,
                             TL("Allow undo-redo"), "",
                             myApplicationWindow, MID_GNE_TOGGLE_UNDOREDO);
    menuCheckAllowUndoRedo->setCheck(myApplicationWindow->getApp()->reg().readBoolEntry("NETEDIT", "AllowUndoRedo", true));
}


void
GNEApplicationWindowHelper::EditMenuCommands::buildViewMenuCommands(FXMenuPane* editMenu) {
    // build rest of menu commands
    editViewScheme = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
                     TL("Edit Visualisation"), "F9", TL("Opens a dialog for editing visualization settings."),
                     GUIIconSubSys::getIcon(GUIIcon::COLORWHEEL), myApplicationWindow, MID_HOTKEY_F9_EDIT_VIEWSCHEME);
    editViewPort = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
                   TL("Edit Viewport"), "Ctrl+I", TL("Opens a dialog for editing viewing area, zoom and rotation."),
                   GUIIconSubSys::getIcon(GUIIcon::EDITVIEWPORT), myApplicationWindow, MID_HOTKEY_CTRL_I_EDITVIEWPORT);
}


void
GNEApplicationWindowHelper::EditMenuCommands::buildFrontElementMenuCommand(FXMenuPane* editMenu) {
    // build toggle front element
    toggleFrontElement = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
                         TL("Front element"), "F11", TL("Mark current inspected element as front element"),
                         GUIIconSubSys::getIcon(GUIIcon::FRONTELEMENT), myApplicationWindow, MID_HOTKEY_F11_FRONTELEMENT);
}


void
GNEApplicationWindowHelper::EditMenuCommands::buildOpenSUMOMenuCommands(FXMenuPane* editMenu) {
    // add open in sumo options
    loadAdditionalsInSUMOGUI = GUIDesigns::buildFXMenuCheckbox(editMenu,
                               TL("Load additionals in sumo-gui"), TL("Load additionals in sumo-gui."),
                               myApplicationWindow, MID_TOOLBAREDIT_LOADADDITIONALS);
    loadAdditionalsInSUMOGUI->setCheck(TRUE);
    loadDemandInSUMOGUI = GUIDesigns::buildFXMenuCheckbox(editMenu,
                          TL("Load demand in sumo-gui"), TL("Load demand in sumo-gui."),
                          myApplicationWindow, MID_TOOLBAREDIT_LOADDEMAND);
    loadDemandInSUMOGUI->setCheck(TRUE);
    openInSUMOGUI = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
                    TL("Open in sumo-gui"), "Ctrl+T", TL("Opens the sumo-gui application with the current network."),
                    GUIIconSubSys::getIcon(GUIIcon::SUMO_MINI), myApplicationWindow, MID_HOTKEY_CTRL_T_OPENNETEDIT_OPENSUMO);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::LockMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::LockMenuCommands::LockMenuCommands(GNEApplicationWindow* applicationWindow) :
    myApplicationWindow(applicationWindow) {
}


void
GNEApplicationWindowHelper::LockMenuCommands::buildLockMenuCommands(FXMenuPane* fileMenu) {
    // network
    menuCheckLockJunctions = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                             TL("&Junctions"), "", "",
                             GUIIconSubSys::getIcon(GUIIcon::JUNCTION),
                             myApplicationWindow, MID_GNE_LOCK_ELEMENT);

    menuCheckLockEdges = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                         TL("&Edges"), "", "",
                         GUIIconSubSys::getIcon(GUIIcon::EDGE),
                         myApplicationWindow, MID_GNE_LOCK_ELEMENT);

    menuCheckLockLanes = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                         TL("&Lanes"), "", "",
                         GUIIconSubSys::getIcon(GUIIcon::LANE),
                         myApplicationWindow, MID_GNE_LOCK_ELEMENT);

    menuCheckLockConnections = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                               TL("&Connections"), "", "",
                               GUIIconSubSys::getIcon(GUIIcon::CONNECTION),
                               myApplicationWindow, MID_GNE_LOCK_ELEMENT);

    menuCheckLockCrossings = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                             TL("C&rossings"), "", "",
                             GUIIconSubSys::getIcon(GUIIcon::CROSSING),
                             myApplicationWindow, MID_GNE_LOCK_ELEMENT);

    menuCheckLockWalkingAreas = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                                TL("Walkin&gAreas"), "", "",
                                GUIIconSubSys::getIcon(GUIIcon::WALKINGAREA),
                                myApplicationWindow, MID_GNE_LOCK_ELEMENT);

    menuCheckLockAdditionals = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                               TL("&Additionals"), "", "",
                               GUIIconSubSys::getIcon(GUIIcon::BUSSTOP),
                               myApplicationWindow, MID_GNE_LOCK_ELEMENT);

    menuCheckLockTAZs = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                        TL("&TAZs"), "", "",
                        GUIIconSubSys::getIcon(GUIIcon::TAZ),
                        myApplicationWindow, MID_GNE_LOCK_ELEMENT);

    menuCheckLockWires = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                         TL("&Wires"), "", "",
                         GUIIconSubSys::getIcon(GUIIcon::OVERHEADWIRE),
                         myApplicationWindow, MID_GNE_LOCK_ELEMENT);

    menuCheckLockPolygons = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                            TL("&Polygons"), "", "",
                            GUIIconSubSys::getIcon(GUIIcon::POLY),
                            myApplicationWindow, MID_GNE_LOCK_ELEMENT);

    menuCheckLockPOIs = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                        TL("P&OIs"), "", "",
                        GUIIconSubSys::getIcon(GUIIcon::POI),
                        myApplicationWindow, MID_GNE_LOCK_ELEMENT);

    menuCheckLockJpsWalkableAreas = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                                    TL("WalkableAreas"), "", "",
                                    GUIIconSubSys::getIcon(GUIIcon::JPS_WALKABLEAREA),
                                    myApplicationWindow, MID_GNE_LOCK_ELEMENT);

    menuCheckLockJpsObstacles = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                                TL("Obstacles"), "", "",
                                GUIIconSubSys::getIcon(GUIIcon::JPS_OBSTACLE),
                                myApplicationWindow, MID_GNE_LOCK_ELEMENT);
    // demand
    menuCheckLockRoutes = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                          TL("&Routes"), "", "",
                          GUIIconSubSys::getIcon(GUIIcon::ROUTE),
                          myApplicationWindow, MID_GNE_LOCK_ELEMENT);

    menuCheckLockVehicles = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                            TL("&Vehicles"), "", "",
                            GUIIconSubSys::getIcon(GUIIcon::VEHICLE),
                            myApplicationWindow, MID_GNE_LOCK_ELEMENT);

    menuCheckLockPersons = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                           TL("&Persons"), "", "",
                           GUIIconSubSys::getIcon(GUIIcon::PERSON),
                           myApplicationWindow, MID_GNE_LOCK_ELEMENT);

    menuCheckLockPersonTrips = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                               TL("Person&Trips"), "", "",
                               GUIIconSubSys::getIcon(GUIIcon::PERSONTRIP_EDGE),
                               myApplicationWindow, MID_GNE_LOCK_ELEMENT);

    menuCheckLockWalks = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                         TL("&Walks"), "", "",
                         GUIIconSubSys::getIcon(GUIIcon::WALK_EDGE),
                         myApplicationWindow, MID_GNE_LOCK_ELEMENT);

    menuCheckLockRides = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                         TL("Ri&des"), "", "",
                         GUIIconSubSys::getIcon(GUIIcon::RIDE_EDGE),
                         myApplicationWindow, MID_GNE_LOCK_ELEMENT);

    menuCheckLockContainers = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                              TL("&Containers"), "", "",
                              GUIIconSubSys::getIcon(GUIIcon::CONTAINER),
                              myApplicationWindow, MID_GNE_LOCK_ELEMENT);

    menuCheckLockTransports = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                              TL("Tra&nsports"), "", "",
                              GUIIconSubSys::getIcon(GUIIcon::TRANSPORT_EDGE),
                              myApplicationWindow, MID_GNE_LOCK_ELEMENT);

    menuCheckLockTranships = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                             TL("Trans&hips"), "", "",
                             GUIIconSubSys::getIcon(GUIIcon::TRANSHIP_EDGE),
                             myApplicationWindow, MID_GNE_LOCK_ELEMENT);

    menuCheckLockStops = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                         TL("Stop&s"), "", "",
                         GUIIconSubSys::getIcon(GUIIcon::STOPELEMENT),
                         myApplicationWindow, MID_GNE_LOCK_ELEMENT);
    // data
    menuCheckLockEdgeDatas = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                             TL("&EdgeDatas"), "", "",
                             GUIIconSubSys::getIcon(GUIIcon::EDGEDATA),
                             myApplicationWindow, MID_GNE_LOCK_ELEMENT);

    menuCheckLockEdgeRelDatas = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                                TL("E&dgeRelDatas"), "", "",
                                GUIIconSubSys::getIcon(GUIIcon::EDGERELDATA),
                                myApplicationWindow, MID_GNE_LOCK_ELEMENT);

    menuCheckLockEdgeTAZRels = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                               TL("Edge&TAZRel"), "", "",
                               GUIIconSubSys::getIcon(GUIIcon::TAZRELDATA),
                               myApplicationWindow, MID_GNE_LOCK_ELEMENT);

    // separator
    new FXMenuSeparator(fileMenu);

    menuCheckLockSelectedElements = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                                    TL("Lock selected elements"), "", TL("selected elements"),
                                    GUIIconSubSys::getIcon(GUIIcon::LOCK_SELECTED), myApplicationWindow, MID_GNE_LOCK_SELECTEDELEMENTS);

    // separator
    new FXMenuSeparator(fileMenu);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Lock all elements"), "", TL("all elements"),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCK), myApplicationWindow, MID_GNE_LOCK_ALLELEMENTS);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Unlock all elements"), "", TL("Unlock all elements"),
                                           GUIIconSubSys::getIcon(GUIIcon::UNLOCK), myApplicationWindow, MID_GNE_UNLOCK_ALLELEMENTS);
}


void
GNEApplicationWindowHelper::LockMenuCommands::removeHotkeys() {
    // network
    menuCheckLockJunctions->remHotKey(parseHotKey('j'));
    menuCheckLockEdges->remHotKey(parseHotKey('e'));
    menuCheckLockLanes->remHotKey(parseHotKey('l'));
    menuCheckLockConnections->remHotKey(parseHotKey('c'));
    menuCheckLockCrossings->remHotKey(parseHotKey('r'));
    menuCheckLockWalkingAreas->remHotKey(parseHotKey('g'));
    menuCheckLockAdditionals->remHotKey(parseHotKey('a'));
    menuCheckLockTAZs->remHotKey(parseHotKey('t'));
    menuCheckLockWires->remHotKey(parseHotKey('w'));
    menuCheckLockPolygons->remHotKey(parseHotKey('p'));
    menuCheckLockPOIs->remHotKey(parseHotKey('o'));
    menuCheckLockJpsWalkableAreas->remHotKey(parseHotKey('n'));
    menuCheckLockJpsObstacles->remHotKey(parseHotKey('b'));
    // demand
    menuCheckLockRoutes->remHotKey(parseHotKey('r'));
    menuCheckLockVehicles->remHotKey(parseHotKey('v'));
    menuCheckLockPersons->remHotKey(parseHotKey('p'));
    menuCheckLockPersonTrips->remHotKey(parseHotKey('t'));
    menuCheckLockWalks->remHotKey(parseHotKey('w'));
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
    menuCheckLockJunctions->enable();
    menuCheckLockEdges->enable();
    menuCheckLockLanes->enable();
    menuCheckLockConnections->enable();
    menuCheckLockCrossings->enable();
    menuCheckLockWalkingAreas->enable();
    menuCheckLockAdditionals->enable();
    menuCheckLockTAZs->enable();
    menuCheckLockWires->enable();
    menuCheckLockPolygons->enable();
    menuCheckLockPOIs->enable();
    menuCheckLockJpsWalkableAreas->enable();
    menuCheckLockJpsObstacles->enable();
    // set accels
    menuCheckLockJunctions->addHotKey(parseHotKey('j'));
    menuCheckLockEdges->addHotKey(parseHotKey('e'));
    menuCheckLockLanes->addHotKey(parseHotKey('l'));
    menuCheckLockConnections->addHotKey(parseHotKey('c'));
    menuCheckLockCrossings->addHotKey(parseHotKey('r'));
    menuCheckLockWalkingAreas->addHotKey(parseHotKey('g'));
    menuCheckLockAdditionals->addHotKey(parseHotKey('a'));
    menuCheckLockTAZs->addHotKey(parseHotKey('t'));
    menuCheckLockWires->addHotKey(parseHotKey('w'));
    menuCheckLockPolygons->addHotKey(parseHotKey('p'));
    menuCheckLockPOIs->addHotKey(parseHotKey('o'));
    menuCheckLockJpsWalkableAreas->addHotKey(parseHotKey('n'));
    menuCheckLockJpsObstacles->addHotKey(parseHotKey('b'));
    // now show it
    menuCheckLockJunctions->show();
    menuCheckLockEdges->show();
    menuCheckLockLanes->show();
    menuCheckLockConnections->show();
    menuCheckLockCrossings->show();
    menuCheckLockWalkingAreas->show();
    menuCheckLockAdditionals->show();
    menuCheckLockTAZs->show();
    menuCheckLockWires->show();
    menuCheckLockPolygons->show();
    menuCheckLockPOIs->show();
    menuCheckLockJpsWalkableAreas->show();
    menuCheckLockJpsObstacles->show();
}


void
GNEApplicationWindowHelper::LockMenuCommands::hideNetworkLockMenuCommands() {
    // first disable menu commands
    menuCheckLockJunctions->disable();
    menuCheckLockEdges->disable();
    menuCheckLockLanes->disable();
    menuCheckLockConnections->disable();
    menuCheckLockCrossings->disable();
    menuCheckLockWalkingAreas->disable();
    menuCheckLockAdditionals->disable();
    menuCheckLockTAZs->disable();
    menuCheckLockPolygons->disable();
    menuCheckLockPOIs->disable();
    menuCheckLockJpsWalkableAreas->disable();
    menuCheckLockJpsObstacles->disable();
    // now hide it
    menuCheckLockJunctions->hide();
    menuCheckLockEdges->hide();
    menuCheckLockLanes->hide();
    menuCheckLockConnections->hide();
    menuCheckLockCrossings->hide();
    menuCheckLockWalkingAreas->hide();
    menuCheckLockAdditionals->hide();
    menuCheckLockTAZs->hide();
    menuCheckLockWires->hide();
    menuCheckLockPolygons->hide();
    menuCheckLockPOIs->hide();
    menuCheckLockJpsWalkableAreas->hide();
    menuCheckLockJpsObstacles->hide();
}


void
GNEApplicationWindowHelper::LockMenuCommands::showDemandLockMenuCommands() {
    // first enable menu commands
    menuCheckLockRoutes->enable();
    menuCheckLockVehicles->enable();
    menuCheckLockPersons->enable();
    menuCheckLockPersonTrips->enable();
    menuCheckLockWalks->enable();
    menuCheckLockRides->enable();
    menuCheckLockContainers->enable();
    menuCheckLockTransports->enable();
    menuCheckLockTranships->enable();
    menuCheckLockStops->enable();
    // set accels
    menuCheckLockRoutes->addHotKey(parseHotKey('r'));
    menuCheckLockVehicles->addHotKey(parseHotKey('v'));
    menuCheckLockPersons->addHotKey(parseHotKey('p'));
    menuCheckLockPersonTrips->addHotKey(parseHotKey('t'));
    menuCheckLockWalks->addHotKey(parseHotKey('w'));
    menuCheckLockRides->addHotKey(parseHotKey('d'));
    menuCheckLockContainers->addHotKey(parseHotKey('c'));
    menuCheckLockTransports->addHotKey(parseHotKey('n'));
    menuCheckLockTranships->addHotKey(parseHotKey('h'));
    menuCheckLockStops->addHotKey(parseHotKey('s'));
    // now show it
    menuCheckLockRoutes->show();
    menuCheckLockVehicles->show();
    menuCheckLockPersons->show();
    menuCheckLockPersonTrips->show();
    menuCheckLockWalks->show();
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
    menuCheckLockPersonTrips->disable();
    menuCheckLockWalks->disable();
    menuCheckLockRides->disable();
    menuCheckLockContainers->disable();
    menuCheckLockTransports->disable();
    menuCheckLockTranships->disable();
    menuCheckLockStops->disable();
    // now hide it
    menuCheckLockRoutes->hide();
    menuCheckLockVehicles->hide();
    menuCheckLockPersons->hide();
    menuCheckLockPersonTrips->hide();
    menuCheckLockWalks->hide();
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
    menuCheckLockJunctions->setCheck(TRUE);
    menuCheckLockEdges->setCheck(TRUE);
    menuCheckLockLanes->setCheck(TRUE);
    menuCheckLockConnections->setCheck(TRUE);
    menuCheckLockCrossings->setCheck(TRUE);
    menuCheckLockWalkingAreas->setCheck(TRUE);
    menuCheckLockAdditionals->setCheck(TRUE);
    menuCheckLockTAZs->setCheck(TRUE);
    menuCheckLockWires->setCheck(TRUE);
    menuCheckLockPolygons->setCheck(TRUE);
    menuCheckLockPOIs->setCheck(TRUE);
    menuCheckLockJpsWalkableAreas->setCheck(TRUE);
    menuCheckLockJpsObstacles->setCheck(TRUE);
    menuCheckLockRoutes->setCheck(TRUE);
    menuCheckLockVehicles->setCheck(TRUE);
    menuCheckLockPersons->setCheck(TRUE);
    menuCheckLockPersonTrips->setCheck(TRUE);
    menuCheckLockWalks->setCheck(TRUE);
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
    menuCheckLockJunctions->setCheck(FALSE);
    menuCheckLockEdges->setCheck(FALSE);
    menuCheckLockLanes->setCheck(FALSE);
    menuCheckLockConnections->setCheck(FALSE);
    menuCheckLockCrossings->setCheck(FALSE);
    menuCheckLockWalkingAreas->setCheck(FALSE);
    menuCheckLockAdditionals->setCheck(FALSE);
    menuCheckLockTAZs->setCheck(FALSE);
    menuCheckLockWires->setCheck(FALSE);
    menuCheckLockPolygons->setCheck(FALSE);
    menuCheckLockPOIs->setCheck(FALSE);
    menuCheckLockJpsWalkableAreas->setCheck(FALSE);
    menuCheckLockJpsObstacles->setCheck(FALSE);
    menuCheckLockRoutes->setCheck(FALSE);
    menuCheckLockVehicles->setCheck(FALSE);
    menuCheckLockPersons->setCheck(FALSE);
    menuCheckLockPersonTrips->setCheck(FALSE);
    menuCheckLockWalks->setCheck(FALSE);
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
    if (AC->getTagProperty()->getTag() == SUMO_TAG_JUNCTION) {
        menuCheckLockJunctions->setCheck(value);
    } else if (AC->getTagProperty()->getTag() == SUMO_TAG_EDGE) {
        menuCheckLockEdges->setCheck(value);
    } else if (AC->getTagProperty()->getTag() == SUMO_TAG_LANE) {
        menuCheckLockLanes->setCheck(value);
    } else if (AC->getTagProperty()->getTag() == SUMO_TAG_CONNECTION) {
        menuCheckLockConnections->setCheck(value);
    } else if (AC->getTagProperty()->getTag() == SUMO_TAG_CROSSING) {
        menuCheckLockCrossings->setCheck(value);
    } else if (AC->getTagProperty()->getTag() == SUMO_TAG_WALKINGAREA) {
        menuCheckLockWalkingAreas->setCheck(value);
    } else if (AC->getTagProperty()->isAdditionalElement()) {
        menuCheckLockAdditionals->setCheck(value);
    } else if (AC->getTagProperty()->isTAZElement()) {
        menuCheckLockTAZs->setCheck(value);
    } else if (AC->getTagProperty()->isWireElement()) {
        menuCheckLockWires->setCheck(value);
    } else if (AC->getTagProperty()->getTag() == SUMO_TAG_POLY) {
        menuCheckLockPolygons->setCheck(value);
    } else if (AC->getTagProperty()->getTag() == GNE_TAG_JPS_WALKABLEAREA) {
        menuCheckLockJpsWalkableAreas->setCheck(value);
    } else if (AC->getTagProperty()->getTag() == GNE_TAG_JPS_OBSTACLE) {
        menuCheckLockJpsObstacles->setCheck(value);
    } else if ((AC->getTagProperty()->getTag() == SUMO_TAG_POI) ||
               (AC->getTagProperty()->getTag() == GNE_TAG_POILANE) ||
               (AC->getTagProperty()->getTag() == GNE_TAG_POIGEO)) {
        menuCheckLockPOIs->setCheck(value);
    } else if (AC->getTagProperty()->isRoute()) {
        menuCheckLockRoutes->setCheck(value);
    } else if (AC->getTagProperty()->isVehicle()) {
        menuCheckLockVehicles->setCheck(value);
    } else if (AC->getTagProperty()->isPerson()) {
        menuCheckLockPersons->setCheck(value);
    } else if (AC->getTagProperty()->isPlanPersonTrip()) {
        menuCheckLockPersonTrips->setCheck(value);
    } else if (AC->getTagProperty()->isPlanWalk()) {
        menuCheckLockWalks->setCheck(value);
    } else if (AC->getTagProperty()->isPlanRide()) {
        menuCheckLockRides->setCheck(value);
    } else if (AC->getTagProperty()->isContainer()) {
        menuCheckLockContainers->setCheck(value);
    } else if (AC->getTagProperty()->isPlanTransport()) {
        menuCheckLockTransports->setCheck(value);
    } else if (AC->getTagProperty()->isPlanTranship()) {
        menuCheckLockTranships->setCheck(value);
    } else if (AC->getTagProperty()->isVehicleStop() ||
               AC->getTagProperty()->isPlanStopPerson() ||
               AC->getTagProperty()->isPlanStopContainer()) {
        menuCheckLockStops->setCheck(value);
    } else if (AC->getTagProperty()->getTag() == GNE_TAG_EDGEREL_SINGLE) {
        menuCheckLockEdgeDatas->setCheck(value);
    } else if (AC->getTagProperty()->getTag() == SUMO_TAG_EDGEREL) {
        menuCheckLockEdgeRelDatas->setCheck(value);
    } else if (AC->getTagProperty()->getTag() == SUMO_TAG_TAZREL) {
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

GNEApplicationWindowHelper::ProcessingMenuCommands::ProcessingMenuCommands(GNEApplicationWindow* applicationWindow) :
    myApplicationWindow(applicationWindow) {
}


void
GNEApplicationWindowHelper::ProcessingMenuCommands::buildProcessingMenuCommands(FXMenuPane* processingMenu) {
    // build network processing menu commands
    computeNetwork = GUIDesigns::buildFXMenuCommandShortcut(processingMenu,
                     TL("Compute Junctions"), "F5", TL("Compute junction shape and logic."),
                     GUIIconSubSys::getIcon(GUIIcon::COMPUTEJUNCTIONS), myApplicationWindow, MID_HOTKEY_F5_COMPUTE_NETWORK_DEMAND);
    computeNetworkVolatile = GUIDesigns::buildFXMenuCommandShortcut(processingMenu,
                             TL("Compute Junctions with volatile options"), "Shift+F5", TL("Compute junction shape and logic using volatile junctions."),
                             GUIIconSubSys::getIcon(GUIIcon::COMPUTEJUNCTIONS), myApplicationWindow, MID_HOTKEY_SHIFT_F5_COMPUTEJUNCTIONS_VOLATILE);
    cleanJunctions = GUIDesigns::buildFXMenuCommandShortcut(processingMenu,
                     TL("Clean Junctions"), "F6", TL("Remove solitary junctions."),
                     GUIIconSubSys::getIcon(GUIIcon::CLEANJUNCTIONS), myApplicationWindow, MID_HOTKEY_F6_CLEAN_SOLITARYJUNCTIONS_UNUSEDROUTES);
    joinJunctions = GUIDesigns::buildFXMenuCommandShortcut(processingMenu,
                    TL("Join Selected Junctions"), "F7", TL("Join selected junctions into a single junction."),
                    GUIIconSubSys::getIcon(GUIIcon::JOINJUNCTIONS), myApplicationWindow, MID_HOTKEY_F7_JOIN_SELECTEDJUNCTIONS_ROUTES);
    clearInvalidCrossings = GUIDesigns::buildFXMenuCommandShortcut(processingMenu,
                            TL("Clean invalid crossings"), "F8", TL("Clear invalid crossings."),
                            GUIIconSubSys::getIcon(GUIIcon::CLEANJUNCTIONS), myApplicationWindow, MID_HOTKEY_F8_CLEANINVALID_CROSSINGS_DEMANDELEMENTS);
    // add separator for checkBox
    mySeparatorCheckBox = new FXMenuSeparator(processingMenu);
    // add checkBox for recomputing in data mode
    menuCheckRecomputeDataMode = GUIDesigns::buildFXMenuCheckboxIcon(processingMenu,
                                 TL("Recompute Network in Data Mode"), "", "",
                                 GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDATA), myApplicationWindow, MID_GNE_TOGGLE_COMPUTE_NETWORK_DATA);
    // set default value
    menuCheckRecomputeDataMode->setCheck(myApplicationWindow->getApp()->reg().readBoolEntry("NETEDIT", "RecomputeData", true));
    // build demand  processing menu commands
    computeDemand = GUIDesigns::buildFXMenuCommandShortcut(processingMenu,
                    TL("Compute demand"), "F5", TL("Computes demand elements."),
                    GUIIconSubSys::getIcon(GUIIcon::COMPUTEDEMAND), myApplicationWindow, MID_HOTKEY_F5_COMPUTE_NETWORK_DEMAND);
    cleanRoutes = GUIDesigns::buildFXMenuCommandShortcut(processingMenu,
                  TL("Clean routes"), "F6", TL("Removes routes without vehicles."),
                  GUIIconSubSys::getIcon(GUIIcon::CLEANROUTES), myApplicationWindow, MID_HOTKEY_F6_CLEAN_SOLITARYJUNCTIONS_UNUSEDROUTES);
    joinRoutes = GUIDesigns::buildFXMenuCommandShortcut(processingMenu,
                 TL("Join routes"), "F7", TL("Joins routes with the same edges."),
                 GUIIconSubSys::getIcon(GUIIcon::JOINROUTES), myApplicationWindow, MID_HOTKEY_F7_JOIN_SELECTEDJUNCTIONS_ROUTES);
    adjustPersonPlans = GUIDesigns::buildFXMenuCommandShortcut(processingMenu,
                        TL("Adjust person plans"), "Shift+F7", TL("Adjust person plans (start/end positions, arrival positions, etc.)"),
                        GUIIconSubSys::getIcon(GUIIcon::ADJUSTPERSONPLANS), myApplicationWindow, MID_HOTKEY_SHIFT_F7_ADJUST_PERSON_PLANS);
    clearInvalidDemandElements = GUIDesigns::buildFXMenuCommandShortcut(processingMenu,
                                 TL("Clean invalid route elements"), "F8", TL("Clear elements with an invalid path (routes, Trips, Flows...)."),
                                 GUIIconSubSys::getIcon(GUIIcon::CLEANJUNCTIONS), myApplicationWindow, MID_HOTKEY_F8_CLEANINVALID_CROSSINGS_DEMANDELEMENTS);
    // add separator
    myOptionsSeparator = new FXMenuSeparator(processingMenu);
    // create optionmenus
    optionMenus = GUIDesigns::buildFXMenuCommandShortcut(processingMenu,
                  TL("Sumo options"), "Shift+F10", TL("Configure sumo Options."),
                  GUIIconSubSys::getIcon(GUIIcon::SUMO_MINI), myApplicationWindow, MID_HOTKEY_SHIFT_F10_SUMOOPTIONSMENU);
    // add separator
    myOptionsSeparator = new FXMenuSeparator(processingMenu);
    // create optionmenus
    optionMenus = GUIDesigns::buildFXMenuCommandShortcut(processingMenu,
                  TL("Options"), "F10", TL("Configure netedit Options."),
                  GUIIconSubSys::getIcon(GUIIcon::OPTIONS), myApplicationWindow, MID_HOTKEY_F10_OPTIONSMENU);
}


void
GNEApplicationWindowHelper::ProcessingMenuCommands::showNetworkProcessingMenuCommands() {
    // first enable menu commands
    computeNetwork->enable();
    computeNetworkVolatile->enable();
    cleanJunctions->enable();
    joinJunctions->enable();
    clearInvalidCrossings->enable();
    mySeparatorCheckBox->enable();
    menuCheckRecomputeDataMode->enable();
    // now show it
    computeNetwork->show();
    computeNetworkVolatile->show();
    cleanJunctions->show();
    joinJunctions->show();
    clearInvalidCrossings->show();
    mySeparatorCheckBox->show();
    menuCheckRecomputeDataMode->show();
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
    myOptionsSeparator->show();
}


void
GNEApplicationWindowHelper::ProcessingMenuCommands::hideSeparator() {
    myOptionsSeparator->hide();
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::LocateMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::LocateMenuCommands::LocateMenuCommands(GNEApplicationWindow* applicationWindow) :
    myApplicationWindow(applicationWindow) {
}


void
GNEApplicationWindowHelper::LocateMenuCommands::buildLocateMenuCommands(FXMenuPane* locateMenu) {
    // build locate menu commands
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           TL("&Junctions"), "Shift+J", TL("Open a dialog for locating a Junction."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEJUNCTION), myApplicationWindow, MID_HOTKEY_SHIFT_J_LOCATEJUNCTION);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           TL("&Edges"), "Shift+E", TL("Open a dialog for locating an Edge."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEEDGE), myApplicationWindow, MID_HOTKEY_SHIFT_E_LOCATEEDGE);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           TL("&WalkingAreas"), "Shift+W", TL("Open a dialog for locating a Walking Area."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEWALKINGAREA), myApplicationWindow, MID_HOTKEY_SHIFT_W_LOCATEWALKINGAREA);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           TL("&Vehicles"), "Shift+V", TL("Open a dialog for locating a Vehicle."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEVEHICLE), myApplicationWindow, MID_HOTKEY_SHIFT_V_LOCATEVEHICLE);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           TL("&Persons"), "Shift+P", TL("Open a dialog for locating a Person."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEPERSON), myApplicationWindow, MID_HOTKEY_SHIFT_P_LOCATEPERSON);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           TL("&Containers"), "Shift+C", TL("Open a dialog for locating a Container."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATECONTAINER), myApplicationWindow, MID_HOTKEY_SHIFT_C_LOCATECONTAINER);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           TL("&Route"), "Shift+R", TL("Open a dialog for locating a Route."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEROUTE), myApplicationWindow, MID_HOTKEY_SHIFT_R_LOCATEROUTE);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           TL("&Stops"), "Shift+S", TL("Open a dialog for locating a Stop."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATESTOP), myApplicationWindow, MID_HOTKEY_SHIFT_S_LOCATESTOP);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           TL("&TLS"), "Shift+T", TL("Open a dialog for locating a Traffic Light."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATETLS), myApplicationWindow, MID_HOTKEY_SHIFT_T_LOCATETLS);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           TL("&Additional"), "Shift+A", TL("Open a dialog for locating an Additional Structure."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEADD), myApplicationWindow, MID_HOTKEY_SHIFT_A_LOCATEADDITIONAL);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           TL("P&oI"), "Shift+O", TL("Open a dialog for locating a Point of Interest."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEPOI), myApplicationWindow, MID_HOTKEY_SHIFT_O_LOCATEPOI);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           TL("Po&lygon"), "Shift+L", TL("Open a dialog for locating a Polygon."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEPOLY), myApplicationWindow, MID_HOTKEY_SHIFT_L_LOCATEPOLY);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::ToolsMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::ToolsMenuCommands::ToolsMenuCommands(GNEApplicationWindow* applicationWindow) :
    myApplicationWindow(applicationWindow) {
}


GNEApplicationWindowHelper::ToolsMenuCommands::~ToolsMenuCommands() {
    // delete all tools
    for (const auto& tool : myPythonTools) {
        delete tool;
    }
    // delete dialogs
    if (myPythonToolDialog) {
        delete myPythonToolDialog;
    }
}


void
GNEApplicationWindowHelper::ToolsMenuCommands::buildTools(FXMenuPane* toolsMenu,
        const std::map<std::string, FXMenuPane*>& menuPaneToolMaps) {
    // build tools from template
    // the templateTools vector is imported from templates.h (which only exists in the cmake-build folder)
    for (const auto& templateTool : templateTools) {
        if (templateTool.name == "netdiff") {
            myPythonTools.push_back(new GNENetDiffTool(myApplicationWindow, templateTool.pythonPath,
                                    menuPaneToolMaps.at(templateTool.subfolder)));
        } else if (menuPaneToolMaps.count(templateTool.subfolder) > 0) {
            myPythonTools.push_back(new GNEPythonTool(myApplicationWindow, templateTool.pythonPath,
                                    templateTool.templateStr, menuPaneToolMaps.at(templateTool.subfolder)));
        } else {
            myPythonTools.push_back(new GNEPythonTool(myApplicationWindow, templateTool.pythonPath,
                                    templateTool.templateStr, toolsMenu));
        }
    }
}


long
GNEApplicationWindowHelper::ToolsMenuCommands::showTool(FXObject* menuCommand) {
    // iterate over all tools and find menu command
    for (const auto& tool : myPythonTools) {
        if (tool->getMenuCommand() == menuCommand) {
            if (myPythonToolDialog) {
                delete myPythonToolDialog;
            }
            myPythonToolDialog = new GNEPythonToolDialog(myApplicationWindow, tool);
            return 1;
        }
    }
    return 0;
}


long
GNEApplicationWindowHelper::ToolsMenuCommands::runToolDialog(FXObject* menuCommand) const {
    // iterate over all tools and find menu command
    for (const auto& tool : myPythonTools) {
        if (tool->getMenuCommand() == menuCommand) {
            GNERunPythonToolDialog(myApplicationWindow, tool);
            return 1;
        }
    }
    return 0;
}


long
GNEApplicationWindowHelper::ToolsMenuCommands::postProcessing(FXObject* menuCommand) const {
    // iterate over all tools and find menu command
    for (const auto& tool : myPythonTools) {
        if (tool->getMenuCommand() == menuCommand) {
            // execute postProcessing
            tool->postProcessing();
            return 1;
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::WindowsMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::WindowsMenuCommands::WindowsMenuCommands(GNEApplicationWindow* applicationWindow) :
    myApplicationWindow(applicationWindow) {
}


void
GNEApplicationWindowHelper::WindowsMenuCommands::buildWindowsMenuCommands(FXMenuPane* windowsMenu,
        FXStatusBar* statusbar, GUIMessageWindow* messageWindow) {
    // build windows menu commands
    GUIDesigns::buildFXMenuCheckbox(windowsMenu,
                                    TL("Show Status Line"), TL("Toggle this Status Bar on/off."),
                                    statusbar, FXWindow::ID_TOGGLESHOWN);
    GUIDesigns::buildFXMenuCheckbox(windowsMenu,
                                    TL("Show Message Window"), TL("Toggle the Message Window on/off."),
                                    messageWindow, FXWindow::ID_TOGGLESHOWN);
    new FXMenuSeparator(windowsMenu);
    GUIDesigns::buildFXMenuCommandShortcut(windowsMenu,
                                           TL("Clear Message Window"), "", TL("Clear the Message Window."),
                                           GUIIconSubSys::getIcon(GUIIcon::CLEARMESSAGEWINDOW), myApplicationWindow, MID_CLEARMESSAGEWINDOW);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::WindowsMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::HelpMenuCommands::HelpMenuCommands(GNEApplicationWindow* applicationWindow) :
    myApplicationWindow(applicationWindow) {
}


void
GNEApplicationWindowHelper::HelpMenuCommands::buildHelpMenuCommands(FXMenuPane* helpMenu) {
    // build windows menu commands
    GUIDesigns::buildFXMenuCommandShortcut(helpMenu, TL("Online Documentation"), "F1", TL("Open Online documentation."),
                                           nullptr, myApplicationWindow, MID_HOTKEY_F1_ONLINEDOCUMENTATION);
    new FXMenuSeparator(helpMenu);
    GUIDesigns::buildFXMenuCommandShortcut(helpMenu, TL("Changelog"), "", TL("Open Changelog."),
                                           nullptr, myApplicationWindow, MID_CHANGELOG);
    GUIDesigns::buildFXMenuCommandShortcut(helpMenu, TL("Hotkeys"), "", TL("Open Hotkeys."),
                                           nullptr, myApplicationWindow, MID_HOTKEYS);
    GUIDesigns::buildFXMenuCommandShortcut(helpMenu, TL("Tutorial"), "", TL("Open Tutorial."),
                                           nullptr, myApplicationWindow, MID_TUTORIAL);
    GUIDesigns::buildFXMenuCommandShortcut(helpMenu, TL("Feedback"), "", TL("Open feedback channels."),
                                           nullptr, myApplicationWindow, MID_FEEDBACK);
    new FXMenuSeparator(helpMenu);
    GUIDesigns::buildFXMenuCommandShortcut(helpMenu, TL("About"), "F12", TL("About netedit."),
                                           GUIIconSubSys::getIcon(GUIIcon::NETEDIT_MINI), myApplicationWindow, MID_HOTKEY_F12_ABOUT);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::NetworkCheckableButtons - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::SupermodeCommands::SupermodeCommands(GNEApplicationWindow* applicationWindow) :
    myApplicationWindow(applicationWindow) {
}


void
GNEApplicationWindowHelper::SupermodeCommands::showSupermodeCommands() {
    networkMode->show();
    demandMode->show();
    dataMode->show();
}


void
GNEApplicationWindowHelper::SupermodeCommands::setDefaultView() {
    showSupermodeCommands();
}


void
GNEApplicationWindowHelper::SupermodeCommands::setJuPedSimView() {
    dataMode->hide();
}


void
GNEApplicationWindowHelper::SupermodeCommands::buildSupermodeCommands(FXMenuPane* editMenu) {
    // build supermode menu commands
    networkMode = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
                  TL("&Network"), "F2", TL("Select network mode."),
                  GUIIconSubSys::getIcon(GUIIcon::SUPERMODENETWORK), myApplicationWindow, MID_HOTKEY_F2_SUPERMODE_NETWORK);
    demandMode = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
                 TL("&Demand"), "F3", TL("Select demand mode."),
                 GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDEMAND), myApplicationWindow, MID_HOTKEY_F3_SUPERMODE_DEMAND);
    dataMode = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
               TL("&Data"), "F4", TL("Select data mode."),
               GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDATA), myApplicationWindow, MID_HOTKEY_F4_SUPERMODE_DATA);
}

// ---------------------------------------------------------------------------
// GNESumoConfigHandler - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::GNESumoConfigHandler::GNESumoConfigHandler(GNEApplicationWindow* applicationWindow, const std::string& file) :
    myApplicationWindow(applicationWindow),
    myFile(file) {
}


bool
GNEApplicationWindowHelper::GNESumoConfigHandler::loadSumoConfig() {
    // get options
    auto& neteditOptions = OptionsCont::getOptions();
    // reset options
    myApplicationWindow->getSumoOptions().resetDefault();
    neteditOptions.resetDefault();
    // make all options writables
    myApplicationWindow->getSumoOptions().resetWritable();
    neteditOptions.resetWritable();
    // build parser
    XERCES_CPP_NAMESPACE::SAXParser parser;
    parser.setValidationScheme(XERCES_CPP_NAMESPACE::SAXParser::Val_Never);
    parser.setDisableDefaultEntityResolution(true);
    // start the parsing
    OptionsLoader handler(myApplicationWindow->getSumoOptions());
    try {
        parser.setDocumentHandler(&handler);
        parser.setErrorHandler(&handler);
        parser.parse(StringUtils::transcodeToLocal(myFile).c_str());
        if (handler.errorOccurred()) {
            WRITE_ERROR(TL("Could not load SUMO configuration '") + myFile + "'.");
            return false;
        }
    } catch (const XERCES_CPP_NAMESPACE::XMLException& e) {
        WRITE_ERROR(TL("Could not load SUMO configuration '") + myFile + "':\n " + StringUtils::transcode(e.getMessage()));
        return false;
    }
    // relocate files
    myApplicationWindow->getSumoOptions().relocateFiles(myFile);
    // set files
    myApplicationWindow->getFileBucketHandler()->setDefaultFilenameFile(FileBucket::Type::SUMOCONFIG, myFile, true);
    if (myApplicationWindow->getFileBucketHandler()->getDefaultFilename(FileBucket::Type::NETEDITCONFIG).empty()) {
        const auto neteditConfig = StringUtils::replace(myFile, ".sumocfg", ".netecfg");
        myApplicationWindow->getFileBucketHandler()->setDefaultFilenameFile(FileBucket::Type::NETEDITCONFIG, neteditConfig, true);
    }
    myApplicationWindow->getFileBucketHandler()->setDefaultFilenameFile(FileBucket::Type::NETWORK, myApplicationWindow->getSumoOptions().getString("net-file"), true);
    return true;
}

// ---------------------------------------------------------------------------
// GNENeteditConfigHandler - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::GNENeteditConfigHandler::GNENeteditConfigHandler(const std::string& file) :
    myFile(file) {
}


bool
GNEApplicationWindowHelper::GNENeteditConfigHandler::loadNeteditConfig() {
    // get options
    auto& neteditOptions = OptionsCont::getOptions();
    // before reset defaults, save ignore route and demand elements (because usually are loaded trought console)
    const bool ignoreAdditionalElements = neteditOptions.getBool("ignore.additionalelements");
    const bool ignoreRouteElements = neteditOptions.getBool("ignore.routeelements");
    // reset options
    neteditOptions.resetDefault();
    // make all options writables
    neteditOptions.resetWritable();
    // build parser
    XERCES_CPP_NAMESPACE::SAXParser parser;
    parser.setValidationScheme(XERCES_CPP_NAMESPACE::SAXParser::Val_Never);
    parser.setDisableDefaultEntityResolution(true);
    // start the parsing
    OptionsLoader handler(neteditOptions);
    try {
        parser.setDocumentHandler(&handler);
        parser.setErrorHandler(&handler);
        parser.parse(StringUtils::transcodeToLocal(myFile).c_str());
        if (handler.errorOccurred()) {
            WRITE_ERROR(TL("Could not load netedit configuration '") + myFile + "'.");
            return false;
        }
    } catch (const XERCES_CPP_NAMESPACE::XMLException& e) {
        WRITE_ERROR(TL("Could not load netedit configuration '") + myFile + "':\n " + StringUtils::transcode(e.getMessage()));
        return false;
    }
    // relocate files
    neteditOptions.relocateFiles(myFile);
    // check if we have loaded a netedit config or a netconvert config
    if (neteditOptions.getString("configuration-file").find(".netccfg") != std::string::npos) {
        const auto newConfiguration = StringUtils::replace(neteditOptions.getString("configuration-file"), ".netccfg", ".netecfg");
        neteditOptions.resetWritable();
        neteditOptions.set("configuration-file", newConfiguration);
    }
    // restore ignores
    neteditOptions.resetWritable();
    if (ignoreAdditionalElements) {
        neteditOptions.set("ignore.additionalelements", GNEAttributeCarrier::TRUE_STR);
    } else {
        neteditOptions.resetDefault("ignore.additionalelements");
    }
    if (ignoreRouteElements) {
        neteditOptions.set("ignore.routeelements", GNEAttributeCarrier::TRUE_STR);
    } else {
        neteditOptions.resetDefault("ignore.routeelements");
    }
    return true;
}


// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::FileBucketHandler - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::FileBucketHandler::FileBucketHandler(OptionsCont& neteditOptions, OptionsCont& sumoOptions) :
    myNeteditOptions(neteditOptions),
    mySumoOptions(sumoOptions),
    myTypes({FileBucket::Type::SUMOCONFIG, FileBucket::Type::NETEDITCONFIG, FileBucket::Type::NETWORK,
            FileBucket::Type::DEMAND, FileBucket::Type::MEANDATA, FileBucket::Type::ADDITIONAL,
            FileBucket::Type::DATA}) {
    // create default buckets
    for (auto type : myTypes) {
        myBuckets[type].push_back(new FileBucket(type));
    }
}


GNEApplicationWindowHelper::FileBucketHandler::~FileBucketHandler() {
    // delete buckets
    for (auto& bucketMap : myBuckets) {
        for (auto& bucket : bucketMap.second) {
            delete bucket;
        }
    }
}


FileBucket*
GNEApplicationWindowHelper::FileBucketHandler::updateAC(const GNEAttributeCarrier* AC, const std::string& filename) {
    // check file properties
    if (AC->getTagProperty()->saveInParentFile()) {
        // elements with parent aren't saved in buckets
        return nullptr;
    } else if (AC->getTagProperty()->isNetworkElement()) {
        // network elements are saved in a single file
        return myBuckets.at(FileBucket::Type::NETWORK).front();
    } else {
        // iterate over all buckets to check if the given filename already exist
        for (auto& bucketMap : myBuckets) {
            // get default bucket (secure because first bucket always exist)
            auto defaultBucket = bucketMap.second.front();
            // check if this bucket type is compatible
            if (AC->getTagProperty()->isFileCompatible(defaultBucket->getType())) {
                // search bucket with this filename
                for (auto& bucket : bucketMap.second) {
                    if (bucket->getFilename() == filename) {
                        // update number of elements in buckets
                        AC->getFileBucket()->removeElement();
                        bucket->addElement();
                        // removed empty buckets
                        removeEmptyBuckets();
                        // return the new bucket
                        return bucket;
                    }
                }
            }
        }
        // if we didn't found a bucket whit the given filename, create new
        for (auto& bucketMap : myBuckets) {
            // this front() call is secure because every bucket group have always at least one default bucket)
            const auto bucketType = bucketMap.second.front()->getType();
            // check compatibility
            if (AC->getTagProperty()->isFileCompatible(bucketType)) {
                // create new bucket with the given filename
                auto bucket = new FileBucket(bucketType, filename);
                myBuckets.at(bucketType).push_back(bucket);
                // update filename in options because we have a new bucket
                updateOptions();
                // update number of elements in new bucket
                bucket->addElement();
                // return the new bucket
                return bucket;
            }
        }
        // the AC was not updated, throw error
        throw ProcessError(TLF("Element '% cannot be updateAC in bucket '%'", AC->getID(), filename));
    }
}


bool
GNEApplicationWindowHelper::FileBucketHandler::checkFilename(const GNEAttributeCarrier* AC, const std::string& filename) const {
    // check file properties
    if (AC->getTagProperty()->saveInParentFile()) {
        // elements with parent aren't saved in buckets
        return false;
    } else {
        // iterate over all buckets to check if exist a bucket with this filename
        for (auto& bucketMap : myBuckets) {
            for (auto& bucket : bucketMap.second) {
                if (bucket->getFilename() == filename) {
                    // check if the bucket is compatible with this file
                    return AC->getTagProperty()->isFileCompatible(bucket->getType());
                }
            }
        }
        // the file will be saved in a new bucket
        return true;
    }
}


FileBucket*
GNEApplicationWindowHelper::FileBucketHandler::getDefaultBucket(const FileBucket::Type type) const {
    return myBuckets.at(type).front();
}


FileBucket*
GNEApplicationWindowHelper::FileBucketHandler::getBucket(const FileBucket::Type type, const std::string filename, const bool create) {
    // iterate over all buckets to check if the given filename already exist
    for (auto& bucketMap : myBuckets) {
        for (auto& bucket : bucketMap.second) {
            if ((bucket->getFilename() == filename) && (bucket->getType() == type)) {
                return bucket;
            }
        }
    }
    // on this point, we need to check if create a new bucket
    if (create) {
        // if the default bucket is empty, but not the filename, update the default bucket
        if (getDefaultFilename(type).empty() && (filename.size() > 0)) {
            setDefaultFilenameFile(type, filename, true);
            return getDefaultBucket(type);
        } else {
            // create new bucket
            auto bucket = new FileBucket(type, filename);
            myBuckets.at(type).push_back(bucket);
            // update filename in options because we have a new bucket
            updateOptions();
            return bucket;
        }
    } else {
        return nullptr;
    }
}


const std::vector<FileBucket*>&
GNEApplicationWindowHelper::FileBucketHandler::getFileBuckets(const FileBucket::Type type) const {
    return myBuckets.at(type);
}


const std::string&
GNEApplicationWindowHelper::FileBucketHandler::getDefaultFilename(const FileBucket::Type type) const {
    return myBuckets.at(type).front()->getFilename();
}


void
GNEApplicationWindowHelper::FileBucketHandler::setDefaultFilenameFile(const FileBucket::Type type, const std::string& filename, const bool force) {
    if (myBuckets.at(type).front()->getFilename().empty() || force) {
        myBuckets.at(type).front()->setFilename(filename);
        // update filename in options
        updateOptions();
    }
}


bool
GNEApplicationWindowHelper::FileBucketHandler::isFilenameDefined(const FileBucket::Type type) const {
    return (myBuckets.at(type).front()->getFilename().size() > 0);
}


void
GNEApplicationWindowHelper::FileBucketHandler::resetDefaultFilenames() {
    for (const auto& bucketPair : myBuckets) {
        bucketPair.second.front()->setFilename("");
    }
    // also remove empty buckets
    removeEmptyBuckets();
    // update filename in options
    updateOptions();
}


std::string
GNEApplicationWindowHelper::FileBucketHandler::parseFilenames(const std::vector<FileBucket::Type> types) const {
    std::string result;
    // group all saving files in a single string separated with comma
    for (const auto& type : types) {
        for (const auto& bucket : myBuckets.at(type)) {
            if (bucket->getFilename().size() > 0) {
                result.append(bucket->getFilename() + ",");
            }
        }
    }
    // remove last ','
    if (result.size() > 0) {
        result.pop_back();
    }
    return result;
}


void
GNEApplicationWindowHelper::FileBucketHandler::removeEmptyBuckets() {
    bool bucketDeleted = false;
    // iterate over all buckets and remove empty buckets (except default buckets)
    for (auto type : myTypes) {
        size_t bucketIndex = 0;
        while (bucketIndex < myBuckets.at(type).size()) {
            auto bucket = myBuckets.at(type).at(bucketIndex);
            if (bucket->isEmpty() && (bucket->isDefaultBucket() == false)) {
                delete bucket;
                myBuckets.at(type).erase(myBuckets.at(type).begin() + bucketIndex);
                bucketDeleted = true;
            } else {
                bucketIndex++;
            }
        }
    }
    // check if update options
    if (bucketDeleted) {
        updateOptions();
    }
}


void
GNEApplicationWindowHelper::FileBucketHandler::updateOptions() {
    // get filenames
    const auto networkFile = parseFilenames({FileBucket::Type::NETWORK});
    const auto additional = parseFilenames({FileBucket::Type::ADDITIONAL});
    const auto demandFile = parseFilenames({FileBucket::Type::DEMAND});
    const auto data = parseFilenames({FileBucket::Type::DATA});
    const auto meanData = parseFilenames({FileBucket::Type::MEANDATA});
    const auto additionalMeanData = parseFilenames({FileBucket::Type::ADDITIONAL, FileBucket::Type::MEANDATA});
    // set default filename depending of type
    myNeteditOptions.resetWritable();
    mySumoOptions.resetWritable();
    // network file (common)
    if (networkFile.size() > 0) {
        myNeteditOptions.set("net-file", networkFile);
        mySumoOptions.set("net-file", networkFile);
    } else {
        myNeteditOptions.resetDefault("net-file");
        mySumoOptions.resetDefault("net-file");
    }
    // additional file (only netedit)
    if (additional.size() > 0) {
        myNeteditOptions.set("additional-files", additional);
    } else {
        myNeteditOptions.resetDefault("additional-files");
    }
    // demand file (common)
    if (demandFile.size() > 0) {
        myNeteditOptions.set("route-files", demandFile);
        mySumoOptions.set("route-files", demandFile);
    } else {
        myNeteditOptions.resetDefault("route-files");
        mySumoOptions.resetDefault("route-files");
    }
    // data file (only netedit)
    if (data.size() > 0) {
        myNeteditOptions.set("data-files", data);
    } else {
        myNeteditOptions.resetDefault("data-files");
    }
    // meanData file (only netedit)
    if (meanData.size() > 0) {
        myNeteditOptions.set("meandata-files", meanData);
    } else {
        myNeteditOptions.resetDefault("meandata-files");
    }
    // additional + meanData files (only sumo)
    if (additionalMeanData.size() > 0) {
        mySumoOptions.set("additional-files", additionalMeanData);
    } else {
        mySumoOptions.resetDefault("additional-files");
    }
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper - methods
// ---------------------------------------------------------------------------

bool
GNEApplicationWindowHelper::toggleEditOptionsNetwork(GNEViewNet* viewNet, const MFXCheckableButton* menuCheck, FXObject* obj, FXSelector sel) {
    // finally function correspond to visibleMenuCommands[numericalKeyPressed]
    if (menuCheck == viewNet->getNetworkViewOptions().menuCheckToggleGrid) {
        // Call manually onCmdToggleShowGrid
        viewNet->onCmdToggleShowGrid(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckToggleDrawJunctionShape) {
        // Call manually onCmdToggleDrawJunctionShape
        viewNet->onCmdToggleDrawJunctionShape(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckDrawSpreadVehicles) {
        // Call manually onCmdToggleDrawSpreadVehicles
        viewNet->onCmdToggleDrawSpreadVehicles(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckShowDemandElements) {
        // Call manually onCmdToggleShowDemandElementsNetwork
        viewNet->onCmdToggleShowDemandElementsNetwork(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckSelectEdges) {
        // Call manually onCmdToggleSelectEdges
        viewNet->onCmdToggleSelectEdges(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckShowConnections) {
        // Call manually onCmdToggleShowConnections
        viewNet->onCmdToggleShowConnections(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckHideConnections) {
        // Call manually onCmdToggleHideConnections
        viewNet->onCmdToggleHideConnections(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckShowAdditionalSubElements) {
        // Call manually onCmdToggleShowAdditionalSubElements
        viewNet->onCmdToggleShowAdditionalSubElements(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckShowTAZElements) {
        // Call manually onCmdToggleShowTAZElements
        viewNet->onCmdToggleShowTAZElements(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckExtendSelection) {
        // Call manually onCmdToggleExtendSelection
        viewNet->onCmdToggleExtendSelection(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckChangeAllPhases) {
        // Call manually onCmdToggleChangeAllPhases
        viewNet->onCmdToggleChangeAllPhases(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckMergeAutomatically) {
        // Call manually onCmdToggleMergeAutomatically
        viewNet->onCmdToggleMergeAutomatically(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckMoveElevation) {
        // Call manually onCmdToggleMoveElevation
        viewNet->onCmdToggleMoveElevation(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckChainEdges) {
        // Call manually onCmdToggleChainEdges
        viewNet->onCmdToggleChainEdges(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckAutoOppositeEdge) {
        // Call manually onCmdToggleAutoOppositeEdge
        viewNet->onCmdToggleAutoOppositeEdge(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckShowJunctionBubble) {
        // Call manually onCmdToggleShowJunctionBubble
        viewNet->onCmdToggleShowJunctionBubbles(obj, sel, nullptr);
    } else {
        return false;
    }
    return true;
}


bool
GNEApplicationWindowHelper::toggleEditOptionsDemand(GNEViewNet* viewNet, const MFXCheckableButton* menuCheck, FXObject* obj, FXSelector sel) {
    if (menuCheck == viewNet->getDemandViewOptions().menuCheckToggleGrid) {
        // Call manually onCmdToggleShowGrid
        viewNet->onCmdToggleShowGrid(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckToggleDrawJunctionShape) {
        // Call manually onCmdToggleDrawJunctionShape
        viewNet->onCmdToggleDrawJunctionShape(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckDrawSpreadVehicles) {
        // Call manually onCmdToggleDrawSpreadVehicles
        viewNet->onCmdToggleDrawSpreadVehicles(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckHideShapes) {
        // Call manually onCmdToggleHideShapes
        viewNet->onCmdToggleHideShapes(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckShowAllTrips) {
        // Call manually onCmdToggleShowTrips
        viewNet->onCmdToggleShowTrips(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckShowAllPersonPlans) {
        // Call manually onCmdToggleShowAllPersonPlans
        viewNet->onCmdToggleShowAllPersonPlans(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckLockPerson) {
        // Call manually onCmdToggleLockPerson
        viewNet->onCmdToggleLockPerson(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckShowAllContainerPlans) {
        // Call manually onCmdToggleShowAllContainerPlans
        viewNet->onCmdToggleShowAllContainerPlans(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckLockContainer) {
        // Call manually onCmdToggleLockContainer
        viewNet->onCmdToggleLockContainer(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckHideNonInspectedDemandElements) {
        // Call manually onCmdToggleHideNonInspecteDemandElements
        viewNet->onCmdToggleHideNonInspecteDemandElements(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckShowOverlappedRoutes) {
        // Call manually onCmdToggleHideNonInspecteDemandElements
        viewNet->onCmdToggleShowOverlappedRoutes(obj, sel, nullptr);
    } else {
        return false;
    }
    return true;
}


bool
GNEApplicationWindowHelper::toggleEditOptionsData(GNEViewNet* viewNet, const MFXCheckableButton* menuCheck, FXObject* obj, FXSelector sel) {
    if (menuCheck == viewNet->getDataViewOptions().menuCheckToggleDrawJunctionShape) {
        // Call manually onCmdToggleDrawJunctionShape
        viewNet->onCmdToggleDrawJunctionShape(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDataViewOptions().menuCheckShowAdditionals) {
        // Call manually onCmdToggleShowAdditionals
        viewNet->onCmdToggleShowAdditionals(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDataViewOptions().menuCheckShowShapes) {
        // Call manually onCmdToggleShowShapes
        viewNet->onCmdToggleShowShapes(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDataViewOptions().menuCheckShowDemandElements) {
        // Call manually onCmdToggleShowDemandElementsData
        viewNet->onCmdToggleShowDemandElementsData(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDataViewOptions().menuCheckToggleTAZRelDrawing) {
        // Call manually onCmdToggleTAZRelDrawing
        viewNet->onCmdToggleTAZRelDrawing(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDataViewOptions().menuCheckToggleTAZDrawFill) {
        // Call manually onCmdToggleTAZDrawFill
        viewNet->onCmdToggleTAZDrawFill(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDataViewOptions().menuCheckToggleTAZRelOnlyFrom) {
        // Call manually onCmdToggleTAZRelOnlyFrom
        viewNet->onCmdToggleTAZRelOnlyFrom(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDataViewOptions().menuCheckToggleTAZRelOnlyTo) {
        // Call manually onCmdToggleTAZRelOnlyTo
        viewNet->onCmdToggleTAZRelOnlyTo(obj, sel, nullptr);
    } else {
        return false;
    }
    return true;
}


bool
GNEApplicationWindowHelper::stringEndsWith(const std::string& str, const std::string& suffix) {
    if (str.length() < suffix.length()) {
        return false;
    } else {
        return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
    }
}

/****************************************************************************/
