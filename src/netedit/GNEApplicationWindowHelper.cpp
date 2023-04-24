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
/// @file    GNEApplicationWindowHelper.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
///
// The main window of Netedit (adapted from GUIApplicationWindow)
/****************************************************************************/

#include <netedit/tools/GNENetDiffTool.h>
#include <netedit/dialogs/tools/GNEPythonToolDialog.h>
#include <netedit/dialogs/tools/GNENetgenerateDialog.h>
#include <netedit/dialogs/tools/GNERunPythonToolDialog.h>
#include <netedit/dialogs/tools/GNERunNetgenerateDialog.h>
#include <netedit/elements/GNEAttributeCarrier.h>
#include <netedit/templates.h>
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

#include <regex>

#include "GNEApplicationWindow.h"
#include "GNEViewNet.h"
#include "GNENet.h"

// ===========================================================================
// GNEApplicationWindowHelper::ToolbarsGrip method definitions
// ===========================================================================

GNEApplicationWindowHelper::ToolbarsGrip::ToolbarsGrip(GNEApplicationWindow* GNEApp) :
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindowHelper::ToolbarsGrip::buildMenuToolbarsGrip() {
    // build menu bar (for File, edit, processing...) using specify design
    myPythonToolBarShellMenu = new FXToolBarShell(myGNEApp, GUIDesignToolBar);
    menu = new FXMenuBar(myGNEApp->getTopDock(), myPythonToolBarShellMenu, GUIDesignToolbarMenuBarNetedit);
    // declare toolbar grip for menu bar
    new FXToolBarGrip(menu, menu, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
}


void
GNEApplicationWindowHelper::ToolbarsGrip::buildViewParentToolbarsGrips() {
    // build toolbar shells
    myPythonToolBarShellSuperModes = new FXToolBarShell(myGNEApp, GUIDesignToolBar);
    myPythonToolBarShellSaveElements = new FXToolBarShell(myGNEApp, GUIDesignToolBar);
    myPythonToolBarShellNavigation = new FXToolBarShell(myGNEApp, GUIDesignToolBar);
    myPythonToolBarShellModes = new FXToolBarShell(myGNEApp, GUIDesignToolBar);
    myPythonToolBarShellIntervalBar = new FXToolBarShell(myGNEApp, GUIDesignToolBar);
    // build menu bars
    superModes = new FXMenuBar(myGNEApp->getTopDock(), myPythonToolBarShellSuperModes, GUIDesignToolBarRaisedSame);
    if (OptionsCont::getOptions().getBool("gui-testing")) {
        saveElements = new FXMenuBar(myGNEApp->getTopDock(), myPythonToolBarShellSaveElements, GUIDesignToolBarRaisedNext);
        navigation = new FXMenuBar(myGNEApp->getTopDock(), myPythonToolBarShellNavigation, GUIDesignToolBarRaisedSame);
        modes = new FXMenuBar(myGNEApp->getTopDock(), myPythonToolBarShellModes, GUIDesignToolBarRaisedNext);
    } else {
        saveElements = new FXMenuBar(myGNEApp->getTopDock(), myPythonToolBarShellSaveElements, GUIDesignToolBarRaisedNext);
        navigation = new FXMenuBar(myGNEApp->getTopDock(), myPythonToolBarShellNavigation, GUIDesignToolBarRaisedSame);
        modes = new FXMenuBar(myGNEApp->getTopDock(), myPythonToolBarShellModes, GUIDesignToolBarRaisedSame);
    }
    intervalBar = new FXMenuBar(myGNEApp->getTopDock(), myPythonToolBarShellIntervalBar, GUIDesignToolBarRaisedNext);
    // build FXToolBarGrip
    new FXToolBarGrip(superModes, superModes, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
    new FXToolBarGrip(saveElements, saveElements, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
    new FXToolBarGrip(navigation, navigation, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
    new FXToolBarGrip(modes, modes, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
    new FXToolBarGrip(intervalBar, intervalBar, FXMenuBar::ID_TOOLBARGRIP, GUIDesignToolBarGrip);
    // create menu bars
    superModes->create();
    saveElements->create();
    navigation->create();
    modes->create();
    intervalBar->create();
    // create toolbar shells
    myPythonToolBarShellSuperModes->create();
    myPythonToolBarShellSaveElements->create();
    myPythonToolBarShellNavigation->create();
    myPythonToolBarShellModes->create();
    myPythonToolBarShellIntervalBar->create();
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
    delete myPythonToolBarShellSuperModes;
    delete myPythonToolBarShellSaveElements;
    delete myPythonToolBarShellNavigation;
    delete myPythonToolBarShellModes;
    delete myPythonToolBarShellIntervalBar;
    // recalc top dop after deleting elements
    myGNEApp->getTopDock()->recalc();
}

// ===========================================================================
// GNEApplicationWindowHelper::MenuBarFile method definitions
// ===========================================================================

GNEApplicationWindowHelper::MenuBarFile::MenuBarFile(GNEApplicationWindow* GNEApp) :
    myRecentNetworks(GNEApp->getApp(), TL("nets")),
    myRecentConfigs(GNEApp->getApp(), TL("configs")),
    myGNEApp(GNEApp) {
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
    myRecentNetworks.setTarget(myGNEApp);
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
    myRecentConfigs.setTarget(myGNEApp);
    myRecentConfigs.setSelector(MID_RECENTFILE);
    new FXMenuCascade(fileMenu, TL("Recent Configs"), nullptr, fileMenuRecentConfigFiles);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::FileMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::FileMenuCommands::FileMenuCommands(GNEApplicationWindow* GNEApp) :
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindowHelper::FileMenuCommands::buildFileMenuCommands(FXMenuPane* fileMenu, FXMenuPane* fileMenuNeteditConfig,
        FXMenuPane* fileMenuSumoConfig, FXMenuPane* fileMenuTLS, FXMenuPane* fileMenuEdgeTypes, FXMenuPane* fileMenuAdditionals,
        FXMenuPane* fileMenuDemandElements, FXMenuPane* fileMenuDataElements, FXMenuPane* fileMenuMeanDataElements) {

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("&New Network"), "Ctrl+N", TL("Create a new network."),
                                           GUIIconSubSys::getIcon(GUIIcon::NEW_NET), myGNEApp, MID_HOTKEY_CTRL_N_OPENNETWORK_NEWNETWORK);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("New Window"), "Ctrl+Shift+N", TL("Open a new netedit window."),
                                           nullptr, myGNEApp, MID_HOTKEY_CTRL_SHIFT_N_NEWWINDOW);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Generate Network"), "", TL("Create network using netgenerate."),
                                           GUIIconSubSys::getIcon(GUIIcon::NETGENERATE), myGNEApp, MID_GNE_NETGENERATE);

    // load sumo/netedit configs
    new FXMenuSeparator(fileMenu);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Load Netedit config..."), "Ctrl+E", TL("Load a netedit configuration file."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_NETEDITCONFIG), myGNEApp, MID_HOTKEY_CTRL_E_EDITSELECTION_LOADNETEDITCONFIG);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Load Sumo config..."), "Ctrl+M", TL("Load a SUMO configuration file."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_SUMOCONFIG), myGNEApp, MID_HOTKEY_CTRL_M_OPENSUMOCONFIG);

    // load networks
    new FXMenuSeparator(fileMenu);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("&Open Network..."), "Ctrl+O", TL("Open a SUMO network."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_NET), myGNEApp, MID_HOTKEY_CTRL_O_OPENSIMULATION_OPENNETWORK);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Open Netconvert Configura&tion..."), "Ctrl+Shift+O", TL("Open a configuration file with NETCONVERT options."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN), myGNEApp, MID_HOTKEY_CTRL_SHIFT_O_OPENNETCONVERTFILE);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Import &Foreign Network..."), "", TL("Import a foreign network such as OSM."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_NET), myGNEApp, MID_GNE_TOOLBARFILE_OPENFOREIGN);

    // save networks
    new FXMenuSeparator(fileMenu);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("&Save Network"), "Ctrl+S", TL("Save the network."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_NETWORKELEMENTS), myGNEApp, MID_HOTKEY_CTRL_S_STOPSIMULATION_SAVENETWORK);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Save Net&work As..."), "", TL("Save the network to another file."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_NETWORKELEMENTS), myGNEApp, MID_GNE_TOOLBARFILE_SAVENETWORK_AS);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Save Plain XM&L..."), "Ctrl+L", TL("Save a plain XML representation of the network."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE), myGNEApp, MID_HOTKEY_CTRL_L_SAVEASPLAINXML);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Save &Joined Junctions..."), "", TL("Save log of joined junctions (allows reproduction of joins)."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE), myGNEApp, MID_GNE_SAVEJOINEDJUNCTIONS);

    // reload
    new FXMenuSeparator(fileMenu);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("&Reload"), "Ctrl+R", TL("Reload the network."),
                                           GUIIconSubSys::getIcon(GUIIcon::RELOAD), myGNEApp, MID_HOTKEY_CTRL_R_RELOAD);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Reload Network"), "", TL("Reload the network."),
                                           GUIIconSubSys::getIcon(GUIIcon::RELOAD), myGNEApp, MID_GNE_TOOLBARFILE_RELOADNETWORK);

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

    // close network
    new FXMenuSeparator(fileMenu);
    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Close"), "Ctrl+W", TL("Close the network."),
                                           GUIIconSubSys::getIcon(GUIIcon::CLOSE), myGNEApp, MID_HOTKEY_CTRL_W_CLOSESIMULATION);
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
GNEApplicationWindowHelper::FileMenuCommands::buildNeteditConfigSection(FXMenuPane* menuPane) {
    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save Netedit Config"), "Ctrl+Shift+E", TL("Save netedit configuration file."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_NETEDITCONFIG), myGNEApp, MID_HOTKEY_CTRL_SHIFT_E_SAVENETEDITCONFIG);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save Netedit Config As..."), "", TL("Save netedit configuration in a new file."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_NETEDITCONFIG), myGNEApp, MID_GNE_TOOLBARFILE_SAVENETEDITCONFIG_AS);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Reload Netedit Config"), "", TL("Reload netedit configuration."),
                                           GUIIconSubSys::getIcon(GUIIcon::RELOAD), myGNEApp, MID_GNE_TOOLBARFILE_RELOAD_NETEDITCONFIG);
}


void
GNEApplicationWindowHelper::FileMenuCommands::buildSumoConfigSection(FXMenuPane* menuPane) {
    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save Sumo Config"), "Ctrl+Shift+S", TL("Save sumo configuration file."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_SUMOCONFIG), myGNEApp, MID_HOTKEY_CTRL_SHIFT_S_SAVESUMOCONFIG);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save Sumo Config As..."), "", TL("Save sumo configuration in a new file."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_SUMOCONFIG), myGNEApp, MID_GNE_TOOLBARFILE_SAVESUMOCONFIG_AS);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Reload Sumo Config"), "", TL("Reload sumo configuration."),
                                           GUIIconSubSys::getIcon(GUIIcon::RELOAD), myGNEApp, MID_GNE_TOOLBARFILE_RELOAD_SUMOCONFIG);
}


void
GNEApplicationWindowHelper::FileMenuCommands::buildTrafficLightSection(FXMenuPane* menuPane) {
    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Load TLS Programs..."), "Ctrl+K", TL("Load programs for traffic lights in the current net."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_TLSPROGRAMS), myGNEApp, MID_HOTKEY_CTRL_K_OPENTLSPROGRAMS);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save TLS Programs"), "Ctrl+Shift+K", TL("Save all traffic light programs of the current network."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE), myGNEApp, MID_HOTKEY_CTRL_SHIFT_K_SAVETLS);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save TLS Programs As..."), "", TL("Save all traffic light programs of the current network to another file."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE), myGNEApp, MID_GNE_TOOLBARFILE_SAVETLSPROGRAMS_AS);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Reload TLS Programs"), "", TL("Reload TLS Programs."),
                                           GUIIconSubSys::getIcon(GUIIcon::RELOAD), myGNEApp, MID_GNE_TOOLBARFILE_RELOAD_TLSPROGRAMS);
}


void
GNEApplicationWindowHelper::FileMenuCommands::buildEdgeTypeSection(FXMenuPane* menuPane) {
    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Load Edge Types..."), "Ctrl+H", TL("Load edge types in the current net."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_TLSPROGRAMS), myGNEApp, MID_HOTKEY_CTRL_H_APPSETTINGS_OPENEDGETYPES);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save Edge Types"), "Ctrl+Shift+H", TL("Save edge types of the current net."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE), myGNEApp, MID_HOTKEY_CTRL_SHIFT_H_SAVEEDGETYPES);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save Edge Types As..."), "", TL("Save edge types of the current net to another file."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE), myGNEApp, MID_GNE_TOOLBARFILE_SAVEEDGETYPES_AS);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Reload edge types"), "", TL("Reload edge types."),
                                           GUIIconSubSys::getIcon(GUIIcon::RELOAD), myGNEApp, MID_GNE_TOOLBARFILE_RELOAD_EDGETYPES);
}


void
GNEApplicationWindowHelper::FileMenuCommands::buildAdditionalSection(FXMenuPane* menuPane) {
    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Load Additionals..."), "Ctrl+A", TL("Load additionals and shapes."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_ADDITIONALS), myGNEApp, MID_HOTKEY_CTRL_A_STARTSIMULATION_OPENADDITIONALS);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save Additionals"), "Ctrl+Shift+A", TL("Save additionals and shapes."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_ADDITIONALELEMENTS), myGNEApp, MID_HOTKEY_CTRL_SHIFT_A_SAVEADDITIONALS);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save Additionals As..."), "", TL("Save additional elements to another file."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_ADDITIONALELEMENTS), myGNEApp, MID_GNE_TOOLBARFILE_SAVEADDITIONALS_AS);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Reload Additionals"), "", TL("Reload additionals."),
                                           GUIIconSubSys::getIcon(GUIIcon::RELOAD), myGNEApp, MID_GNE_TOOLBARFILE_RELOAD_ADDITIONALS);
}


void
GNEApplicationWindowHelper::FileMenuCommands::buildDemandSection(FXMenuPane* menuPane) {
    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Load Demand Elements..."), "Ctrl+D", TL("Load demand elements."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_ADDITIONALS), myGNEApp, MID_HOTKEY_CTRL_D_SINGLESIMULATIONSTEP_OPENDEMANDELEMENTS);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save Demand Elements"), "Ctrl+Shift+D", TL("Save demand elements."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_DEMANDELEMENTS), myGNEApp, MID_HOTKEY_CTRL_SHIFT_D_SAVEDEMANDELEMENTS);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save Demand Elements As..."), "", TL("Save demand elements to another file."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_DEMANDELEMENTS), myGNEApp, MID_GNE_TOOLBARFILE_SAVEDEMAND_AS);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Reload Demand Elements"), "", TL("Reload demand elements."),
                                           GUIIconSubSys::getIcon(GUIIcon::RELOAD), myGNEApp, MID_GNE_TOOLBARFILE_RELOAD_DEMANDELEMENTS);
}


void
GNEApplicationWindowHelper::FileMenuCommands::buildDataSection(FXMenuPane* menuPane) {
    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Load Data Elements..."), "Ctrl+B", TL("Load data elements."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_ADDITIONALS), myGNEApp, MID_HOTKEY_CTRL_B_EDITBREAKPOINT_OPENDATAELEMENTS);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save Data Elements"), "Ctrl+Shift+B", TL("Save data elements."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_DATAELEMENTS), myGNEApp, MID_HOTKEY_CTRL_SHIFT_B_SAVEDATAELEMENTS);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save Data Elements As..."), "", TL("Save data elements to another file."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_DATAELEMENTS), myGNEApp, MID_GNE_TOOLBARFILE_SAVEDATA_AS);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Reload Data Elements"), "", TL("Reload data elements."),
                                           GUIIconSubSys::getIcon(GUIIcon::RELOAD), myGNEApp, MID_GNE_TOOLBARFILE_RELOAD_DATAELEMENTS);
}


void
GNEApplicationWindowHelper::FileMenuCommands::buildMeanDataSection(FXMenuPane* menuPane) {
    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Load MeanDatas..."), "", TL("Load meanDatas and shapes."),
                                           GUIIconSubSys::getIcon(GUIIcon::OPEN_MEANDATAS), myGNEApp, MID_GNE_TOOLBARFILE_OPENMEANDATAS);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save MeanDatas"), "", TL("Save meanDatas and shapes."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_MEANDATAELEMENTS), myGNEApp, MID_HOTKEY_CTRL_SHIFT_M_SAVEMEANDATAS);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Save MeanDatas As..."), "Ctrl+Shift+M", TL("Save meanData elements to another file."),
                                           GUIIconSubSys::getIcon(GUIIcon::SAVE_MEANDATAELEMENTS), myGNEApp, MID_GNE_TOOLBARFILE_SAVEMEANDATAS_AS);

    new FXMenuSeparator(menuPane);

    GUIDesigns::buildFXMenuCommandShortcut(menuPane,
                                           TL("Reload MeanDatas"), "", TL("Reload meanDatas."),
                                           GUIIconSubSys::getIcon(GUIIcon::RELOAD), myGNEApp, MID_GNE_TOOLBARFILE_RELOAD_MEANDATAS);
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
                  TL("&Inspect"), "I", TL("Inspect elements and change their attributes."),
                  GUIIconSubSys::getIcon(GUIIcon::MODEINSPECT), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_I_MODE_INSPECT);
    deleteMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                 TL("&Delete"), "D", TL("Delete elements."),
                 GUIIconSubSys::getIcon(GUIIcon::MODEDELETE), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_D_MODE_SINGLESIMULATIONSTEP_DELETE);
    selectMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                 TL("&Select"), "S", TL("Select elements."),
                 GUIIconSubSys::getIcon(GUIIcon::MODESELECT), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_S_MODE_STOPSIMULATION_SELECT);
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
               TL("&Move"), "M", TL("Move elements."),
               GUIIconSubSys::getIcon(GUIIcon::MODEMOVE), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_M_MODE_MOVE_MEANDATA);
    createEdgeMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                     TL("&Edge"), "E", TL("Create junction and edges."),
                     GUIIconSubSys::getIcon(GUIIcon::MODECREATEEDGE), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_E_MODE_EDGE_EDGEDATA);
    connectMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                  TL("&Connection"), "C", TL("Modes connections between lanes."),
                  GUIIconSubSys::getIcon(GUIIcon::MODECONNECTION), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_C_MODE_CONNECT_PERSONPLAN);
    prohibitionMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                      TL("Pro&hibition"), "H", TL("Modes connection prohibitions."),
                      GUIIconSubSys::getIcon(GUIIcon::MODEPROHIBITION), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_H_MODE_PROHIBITION_CONTAINERPLAN);
    TLSMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
              "&Traffic light", "T", TL("Modes traffic lights over junctions."),
              GUIIconSubSys::getIcon(GUIIcon::MODETLS), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_T_MODE_TLS_TYPE);
    additionalMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                     TL("&Additional"), "A", TL("Create additional elements."),
                     GUIIconSubSys::getIcon(GUIIcon::MODEADDITIONAL), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_A_MODE_STARTSIMULATION_ADDITIONALSTOP);
    crossingMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                   TL("C&rossing"), "R", TL("Create crossings between edges."),
                   GUIIconSubSys::getIcon(GUIIcon::MODECROSSING), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_R_MODE_CROSSING_ROUTE_EDGERELDATA);
    TAZMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
              TL("TA&Z"), "Z", TL("Create Traffic Assignment Zones."),
              GUIIconSubSys::getIcon(GUIIcon::MODETAZ), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_Z_MODE_TAZ_TAZREL);
    shapeMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                "&POI-Poly", "P", TL("Create Points-Of-Interest and polygons."),
                GUIIconSubSys::getIcon(GUIIcon::MODESHAPE), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_P_MODE_POLYGON_PERSON);
    wireMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
               TL("&Wire"), "W", TL("Create wires."),
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
               TL("&Move"), "M", TL("Move elements."),
               GUIIconSubSys::getIcon(GUIIcon::MODEMOVE), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_M_MODE_MOVE_MEANDATA);
    routeMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                TL("Route"), "R", TL("Create Routes."),
                GUIIconSubSys::getIcon(GUIIcon::MODEROUTE), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_R_MODE_CROSSING_ROUTE_EDGERELDATA);
    vehicleMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                  TL("Vehicle"), "V", TL("Create vehicles."),
                  GUIIconSubSys::getIcon(GUIIcon::MODEVEHICLE), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_V_MODE_VEHICLE);
    typeMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
               TL("Type"), "T", TL("Create types (vehicles, person an containers)."),
               GUIIconSubSys::getIcon(GUIIcon::MODETYPE), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_T_MODE_TLS_TYPE);
    stopMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
               TL("Stop"), "A", TL("Create stops."),
               GUIIconSubSys::getIcon(GUIIcon::MODESTOP), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_A_MODE_STARTSIMULATION_ADDITIONALSTOP);
    personMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                 TL("Person"), "P", TL("Create persons."),
                 GUIIconSubSys::getIcon(GUIIcon::MODEPERSON), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_P_MODE_POLYGON_PERSON);
    personPlanMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                     TL("Person plan"), "C", TL("Create person plans."),
                     GUIIconSubSys::getIcon(GUIIcon::MODEPERSONPLAN), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_C_MODE_CONNECT_PERSONPLAN);
    containerMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                    TL("Container"), "G", TL("Create containers."),
                    GUIIconSubSys::getIcon(GUIIcon::MODECONTAINER), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_G_MODE_CONTAINER);
    containerPlanMode = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                        TL("Container plan"), "H", TL("Create container plans."),
                        GUIIconSubSys::getIcon(GUIIcon::MODECONTAINERPLAN), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_H_MODE_PROHIBITION_CONTAINERPLAN);
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
               GUIIconSubSys::getIcon(GUIIcon::MODEEDGEDATA), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_E_MODE_EDGE_EDGEDATA);
    edgeRelData = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                  TL("Edge&Relation"), "R", TL("Create edgeRelation elements."),
                  GUIIconSubSys::getIcon(GUIIcon::MODEEDGERELDATA), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_R_MODE_CROSSING_ROUTE_EDGERELDATA);
    TAZRelData = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
                 TL("TA&ZRelation"), "Z", TL("Create TAZRelation elements."),
                 GUIIconSubSys::getIcon(GUIIcon::MODETAZRELDATA), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_Z_MODE_TAZ_TAZREL);
    meanData = GUIDesigns::buildFXMenuCommandShortcut(modesMenu,
               TL("&MeanData"), "M", TL("Create MeanData edge/lanes."),
               GUIIconSubSys::getIcon(GUIIcon::MODEMEANDATA), myModesMenuCommandsParent->myGNEApp, MID_HOTKEY_M_MODE_MOVE_MEANDATA);
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
                          TL("Show grid"), "Ctrl+G or Alt+1", "",
                          GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_TOGGLEGRID),
                          myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_TOGGLEGRID);

    menuCheckToggleDrawJunctionShape = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                       TL("Hide junction shape"), "Ctrl+J or Alt+2", "",
                                       GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_TOGGLEDRAWJUNCTIONSHAPE),
                                       myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE);

    menuCheckDrawSpreadVehicles = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                  TL("Draw vehicles spread in lane or in depart position"), "Alt+3", "",
                                  GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_SPREADVEHICLE),
                                  myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_DRAWSPREADVEHICLES);

    menuCheckShowDemandElements = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                  TL("Show demand elements"), "Alt+4", "",
                                  GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_SHOWDEMANDELEMENTS),
                                  myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_SHOWDEMANDELEMENTS);

    menuCheckSelectEdges = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                           TL("Clicking should target lanes"), "Alt+5", "",
                           GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_SELECTEDGES),
                           myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_SELECTEDGES);

    menuCheckShowConnections = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                               TL("Show connections over junctions"), "Alt+6", "",
                               GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_SHOWCONNECTIONS),
                               myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_SHOWCONNECTIONS);

    menuCheckHideConnections = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                               TL("Hide connections"), "Alt+7", "",
                               GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_HIDECONNECTIONS),
                               myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_HIDECONNECTIONS);

    menuCheckShowAdditionalSubElements = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                         TL("Show sub-additional elements"), "Alt+8", "",
                                         GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_SHOWSUBADDITIONALS),
                                         myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_SHOWSUBADDITIONALS);

    menuCheckShowTAZElements = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                               TL("Show TAZ elements"), "Alt+9", "",
                               GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_SHOWTAZELEMENTS),
                               myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_SHOWTAZELEMENTS);

    menuCheckExtendSelection = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                               TL("Selecting multiple edges automatically select their junctions"), "Alt+10", "",
                               GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_AUTOSELECTJUNCTIONS),
                               myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_EXTENDSELECTION);

    menuCheckChangeAllPhases = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                               TL("Clicking should apply state changes to all phases of TLS plan"), "Alt+5", "",
                               GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_APPLYTOALLPHASES),
                               myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_CHANGEALLPHASES);

    menuCheckWarnAboutMerge = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                              TL("Ask for confirmation before merging junction"), "Alt+5", "",
                              GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_ASKFORMERGE),
                              myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_ASKFORMERGE);

    menuCheckShowJunctionBubble = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                  TL("Show bubbles over junctions shapes"), "Alt+6", "",
                                  GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_BUBBLES),
                                  myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_SHOWBUBBLES);

    menuCheckMoveElevation = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                             TL("Apply mouse movement to elevation"), "Alt+7", "",
                             GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_ELEVATION),
                             myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_MOVEELEVATION);

    menuCheckChainEdges = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                          TL("Create consecutive edges"), "Alt+5", "",
                          GUIIconSubSys::getIcon(GUIIcon::NETWORKMODE_CHECKBOX_CHAIN),
                          myGNEApp, MID_GNE_NETWORKVIEWOPTIONS_CHAINEDGES);

    menuCheckAutoOppositeEdge = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                TL("Create an edge in the opposite direction"), "Alt+6", "",
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
    if (menuCheckWarnAboutMerge->shown()) {
        menuCheckWarnAboutMerge->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckShowJunctionBubble->shown()) {
        menuCheckShowJunctionBubble->setAccelText(("Alt+" + toString(index)).c_str());
        index++;
    }
    if (menuCheckMoveElevation->shown()) {
        menuCheckMoveElevation->setAccelText(("Alt+" + toString(index)).c_str());
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
                          TL("Show grid"), "Ctrl+G or Alt+1", "",
                          GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_TOGGLEGRID),
                          myGNEApp, MID_GNE_DEMANDVIEWOPTIONS_SHOWGRID);

    menuCheckToggleDrawJunctionShape = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                       TL("Hide junction shape"), "Ctrl+J or Alt+2", "",
                                       GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_TOGGLEDRAWJUNCTIONSHAPE),
                                       myGNEApp, MID_GNE_DEMANDVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE);

    menuCheckDrawSpreadVehicles = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                  TL("Draw vehicles spread/depart position"), "Alt+3", "",
                                  GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_SPREADVEHICLE),
                                  myGNEApp, MID_GNE_DEMANDVIEWOPTIONS_DRAWSPREADVEHICLES);

    menuCheckHideShapes = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                          TL("Show shapes"), "Alt+4", "",
                          GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_HIDESHAPES),
                          myGNEApp, MID_GNE_DEMANDVIEWOPTIONS_HIDESHAPES);

    menuCheckShowAllTrips = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                            TL("Show all trips"), "Alt+5", "",
                            GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_SHOWTRIPS),
                            myGNEApp, MID_GNE_DEMANDVIEWOPTIONS_SHOWTRIPS);

    menuCheckShowAllPersonPlans = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                  TL("Show all person plans"), "Alt+6", "",
                                  GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_SHOWPERSONPLANS),
                                  myGNEApp, MID_GNE_DEMANDVIEWOPTIONS_SHOWALLPERSONPLANS);

    menuCheckLockPerson = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                          TL("Lock selected person"), "Alt+7", "",
                          GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_LOCKPERSON),
                          myGNEApp, MID_GNE_DEMANDVIEWOPTIONS_LOCKPERSON);

    menuCheckShowAllContainerPlans = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                     TL("Show all container plans"), "Alt+6", "",
                                     GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_SHOWCONTAINERPLANS),
                                     myGNEApp, MID_GNE_DEMANDVIEWOPTIONS_SHOWALLCONTAINERPLANS);

    menuCheckLockContainer = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                             TL("Lock selected container"), "Alt+7", "",
                             GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_LOCKCONTAINER),
                             myGNEApp, MID_GNE_DEMANDVIEWOPTIONS_LOCKCONTAINER);

    menuCheckHideNonInspectedDemandElements = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
            TL("Show non-inspected demand elements"), "Alt+8", "",
            GUIIconSubSys::getIcon(GUIIcon::DEMANDMODE_CHECKBOX_HIDENONINSPECTEDDEMANDELEMENTS),
            myGNEApp, MID_GNE_DEMANDVIEWOPTIONS_HIDENONINSPECTED);

    menuCheckShowOverlappedRoutes = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                    TL("Show number of overlapped routes"), "Alt+9", "",
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

GNEApplicationWindowHelper::EditMenuCommands::DataViewOptions::DataViewOptions(GNEApplicationWindow* GNEApp) :
    menuCheckToggleDrawJunctionShape(nullptr),
    menuCheckShowAdditionals(nullptr),
    menuCheckShowShapes(nullptr),
    menuCheckShowDemandElements(nullptr),
    menuCheckToggleTAZRelDrawing(nullptr),
    menuCheckToggleTAZDrawFill(nullptr),
    menuCheckToggleTAZRelOnlyFrom(nullptr),
    menuCheckToggleTAZRelOnlyTo(nullptr),
    separator(nullptr),
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindowHelper::EditMenuCommands::DataViewOptions::buildDataViewOptionsMenuChecks(FXMenuPane* editMenu) {
    // create menu checks
    menuCheckToggleDrawJunctionShape = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                       TL("Hide junction shape"), "Ctrl+J or Alt+1", "",
                                       GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_TOGGLEDRAWJUNCTIONSHAPE),
                                       myGNEApp, MID_GNE_DEMANDVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE);

    menuCheckShowAdditionals = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                               TL("Show additionals"), "Alt+2", "",
                               GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_SHOWADDITIONALS),
                               myGNEApp, MID_GNE_DATAVIEWOPTIONS_SHOWADDITIONALS);

    menuCheckShowShapes = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                          TL("Show shapes"), "Alt+3", "",
                          GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_SHOWSHAPES),
                          myGNEApp, MID_GNE_DATAVIEWOPTIONS_SHOWSHAPES);

    menuCheckShowDemandElements = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                  TL("Show demand elements"), "Alt+4", "",
                                  GUIIconSubSys::getIcon(GUIIcon::COMMONMODE_CHECKBOX_SHOWDEMANDELEMENTS),
                                  myGNEApp, MID_GNE_DATAVIEWOPTIONS_SHOWDEMANDELEMENTS);

    menuCheckToggleTAZRelDrawing = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                   TL("Draw TAZRel from center"), "Alt+5", "",
                                   GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_TAZRELDRAWING),
                                   myGNEApp, MID_GNE_DATAVIEWOPTIONS_TAZRELDRAWING);

    menuCheckToggleTAZDrawFill = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                 TL("Draw TAZ fill"), "Alt+6", "",
                                 GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_TAZDRAWFILL),
                                 myGNEApp, MID_GNE_DATAVIEWOPTIONS_TAZDRAWFILL);

    menuCheckToggleTAZRelOnlyFrom = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                    TL("Only draw TAZRel from"), "Alt+6", "",
                                    GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_TAZRELONLYFROM),
                                    myGNEApp, MID_GNE_DATAVIEWOPTIONS_TAZRELONLYFROM);

    menuCheckToggleTAZRelOnlyTo = GUIDesigns::buildFXMenuCheckboxIcon(editMenu,
                                  TL("Only draw TAZRel to"), "Alt+7", "",
                                  GUIIconSubSys::getIcon(GUIIcon::DATAMODE_CHECKBOX_TAZRELONLYTO),
                                  myGNEApp, MID_GNE_DATAVIEWOPTIONS_TAZRELONLYTO);
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
                     TL("Undo"), "Ctrl+Z", TL("Undo the last change."),
                     GUIIconSubSys::getIcon(GUIIcon::UNDO), myGNEApp, MID_HOTKEY_CTRL_Z_UNDO);
    redoLastChange = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
                     TL("Redo"), "Ctrl+Y", TL("Redo the last change."),
                     GUIIconSubSys::getIcon(GUIIcon::REDO), myGNEApp, MID_HOTKEY_CTRL_Y_REDO);
    openUndolistDialog = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
                         TL("Show undo/redo history"), "", TL("Open undo/redo history dialog."),
                         GUIIconSubSys::getIcon(GUIIcon::UNDOLIST), myGNEApp, MID_GNE_UNDOLISTDIALOG);
}


void
GNEApplicationWindowHelper::EditMenuCommands::buildViewMenuCommands(FXMenuPane* editMenu) {
    // build rest of menu commands
    editViewScheme = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
                     TL("Edit Visualisation"), "F9", TL("Opens a dialog for editing visualization settings."),
                     GUIIconSubSys::getIcon(GUIIcon::COLORWHEEL), myGNEApp, MID_HOTKEY_F9_EDIT_VIEWSCHEME);
    editViewPort = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
                   TL("Edit Viewport"), "Ctrl+I", TL("Opens a dialog for editing viewing area, zoom and rotation."),
                   GUIIconSubSys::getIcon(GUIIcon::EDITVIEWPORT), myGNEApp, MID_HOTKEY_CTRL_I_EDITVIEWPORT);
}


void
GNEApplicationWindowHelper::EditMenuCommands::buildFrontElementMenuCommand(FXMenuPane* editMenu) {
    // add clear front element
    clearFrontElement = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
                        TL("Clear front element"), "F11", TL("Clear current front element"),
                        GUIIconSubSys::getIcon(GUIIcon::FRONTELEMENT), myGNEApp, MID_HOTKEY_F11_FRONTELEMENT);
}


void
GNEApplicationWindowHelper::EditMenuCommands::buildOpenSUMOMenuCommands(FXMenuPane* editMenu) {
    // add open in sumo options
    loadAdditionalsInSUMOGUI = new FXMenuCheck(editMenu,
            (TL("Load additionals in sumo-gui") + std::string("\t\t") + TL("Load additionals in sumo-gui.")).c_str(),
            myGNEApp, MID_GNE_TOOLBAREDIT_LOADADDITIONALS);
    loadAdditionalsInSUMOGUI->setCheck(TRUE);
    loadDemandInSUMOGUI = new FXMenuCheck(editMenu,
                                          (TL("Load demand in sumo-gui") + std::string("\t\t") + TL("Load demand in sumo-gui.")).c_str(),
                                          myGNEApp, MID_GNE_TOOLBAREDIT_LOADDEMAND);
    loadDemandInSUMOGUI->setCheck(TRUE);
    openInSUMOGUI = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
                    TL("Open in sumo-gui"), "Ctrl+T", TL("Opens the sumo-gui application with the current network."),
                    GUIIconSubSys::getIcon(GUIIcon::SUMO_MINI), myGNEApp, MID_HOTKEY_CTRL_T_OPENNETEDIT_OPENSUMO);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::LockMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::LockMenuCommands::LockMenuCommands(GNEApplicationWindow* GNEApp) :
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindowHelper::LockMenuCommands::buildLockMenuCommands(FXMenuPane* fileMenu) {
    // newtork
    menuCheckLockJunction = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                            TL("&Junctions"), "", "",
                            GUIIconSubSys::getIcon(GUIIcon::JUNCTION),
                            myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockEdges = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                         TL("&Edges"), "", "",
                         GUIIconSubSys::getIcon(GUIIcon::EDGE),
                         myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockLanes = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                         TL("&Lanes"), "", "",
                         GUIIconSubSys::getIcon(GUIIcon::LANE),
                         myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockConnections = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                               TL("&Connections"), "", "",
                               GUIIconSubSys::getIcon(GUIIcon::CONNECTION),
                               myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockCrossings = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                             TL("C&rossings"), "", "",
                             GUIIconSubSys::getIcon(GUIIcon::CROSSING),
                             myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockWalkingAreas = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                                TL("Walkin&gAreas"), "", "",
                                GUIIconSubSys::getIcon(GUIIcon::WALKINGAREA),
                                myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockAdditionals = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                               TL("&Additionals"), "", "",
                               GUIIconSubSys::getIcon(GUIIcon::BUSSTOP),
                               myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockTAZs = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                        TL("&TAZs"), "", "",
                        GUIIconSubSys::getIcon(GUIIcon::TAZ),
                        myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockWires = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                         TL("&Wires"), "", "",
                         GUIIconSubSys::getIcon(GUIIcon::OVERHEADWIRE),
                         myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockPolygons = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                            TL("&Polygons"), "", "",
                            GUIIconSubSys::getIcon(GUIIcon::POLY),
                            myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockPOIs = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                        TL("P&OIs"), "", "",
                        GUIIconSubSys::getIcon(GUIIcon::POI),
                        myGNEApp, MID_GNE_LOCK_ELEMENT);
    // demand
    menuCheckLockRoutes = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                          TL("&Routes"), "", "",
                          GUIIconSubSys::getIcon(GUIIcon::ROUTE),
                          myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockVehicles = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                            TL("&Vehicles"), "", "",
                            GUIIconSubSys::getIcon(GUIIcon::VEHICLE),
                            myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockPersons = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                           TL("&Persons"), "", "",
                           GUIIconSubSys::getIcon(GUIIcon::PERSON),
                           myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockPersonTrip = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                              TL("Person&Trips"), "", "",
                              GUIIconSubSys::getIcon(GUIIcon::PERSONTRIP_FROMTO),
                              myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockWalk = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                        TL("&Walks"), "", "",
                        GUIIconSubSys::getIcon(GUIIcon::WALK_FROMTO),
                        myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockRides = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                         TL("Ri&des"), "", "",
                         GUIIconSubSys::getIcon(GUIIcon::RIDE_FROMTO),
                         myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockContainers = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                              TL("&Containers"), "", "",
                              GUIIconSubSys::getIcon(GUIIcon::CONTAINER),
                              myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockTransports = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                              TL("Tra&nsports"), "", "",
                              GUIIconSubSys::getIcon(GUIIcon::TRANSPORT_FROMTO),
                              myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockTranships = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                             TL("Trans&hips"), "", "",
                             GUIIconSubSys::getIcon(GUIIcon::TRANSHIP_FROMTO),
                             myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockStops = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                         TL("Stop&s"), "", "",
                         GUIIconSubSys::getIcon(GUIIcon::STOPELEMENT),
                         myGNEApp, MID_GNE_LOCK_ELEMENT);
    // data
    menuCheckLockEdgeDatas = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                             TL("&EdgeDatas"), "", "",
                             GUIIconSubSys::getIcon(GUIIcon::EDGEDATA),
                             myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockEdgeRelDatas = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                                TL("E&dgeRelDatas"), "", "",
                                GUIIconSubSys::getIcon(GUIIcon::EDGERELDATA),
                                myGNEApp, MID_GNE_LOCK_ELEMENT);

    menuCheckLockEdgeTAZRels = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                               TL("Edge&TAZRel"), "", "",
                               GUIIconSubSys::getIcon(GUIIcon::TAZRELDATA),
                               myGNEApp, MID_GNE_LOCK_ELEMENT);

    // separator
    new FXMenuSeparator(fileMenu);

    menuCheckLockSelectedElements = GUIDesigns::buildFXMenuCheckboxIcon(fileMenu,
                                    TL("Lock selected elements"), "", TL("selected elements"),
                                    GUIIconSubSys::getIcon(GUIIcon::LOCK_SELECTED), myGNEApp, MID_GNE_LOCK_SELECTEDELEMENTS);

    // separator
    new FXMenuSeparator(fileMenu);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Lock all elements"), "", TL("all elements"),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCK), myGNEApp, MID_GNE_LOCK_ALLELEMENTS);

    GUIDesigns::buildFXMenuCommandShortcut(fileMenu,
                                           TL("Unlock all elements"), "", TL("Unlock all elements"),
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
    menuCheckLockWalkingAreas->remHotKey(parseHotKey('g'));
    menuCheckLockAdditionals->remHotKey(parseHotKey('a'));
    menuCheckLockTAZs->remHotKey(parseHotKey('t'));
    menuCheckLockWires->remHotKey(parseHotKey('w'));
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
    menuCheckLockWalkingAreas->enable();
    menuCheckLockAdditionals->enable();
    menuCheckLockTAZs->enable();
    menuCheckLockWires->enable();
    menuCheckLockPolygons->enable();
    menuCheckLockPOIs->enable();
    // set accels
    menuCheckLockJunction->addHotKey(parseHotKey('j'));
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
    // now show it
    menuCheckLockJunction->show();
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
}


void
GNEApplicationWindowHelper::LockMenuCommands::hideNetworkLockMenuCommands() {
    // first disable menu commands
    menuCheckLockJunction->disable();
    menuCheckLockEdges->disable();
    menuCheckLockLanes->disable();
    menuCheckLockConnections->disable();
    menuCheckLockCrossings->disable();
    menuCheckLockWalkingAreas->disable();
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
    menuCheckLockWalkingAreas->hide();
    menuCheckLockAdditionals->hide();
    menuCheckLockTAZs->hide();
    menuCheckLockWires->hide();
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
    menuCheckLockWalkingAreas->setCheck(TRUE);
    menuCheckLockAdditionals->setCheck(TRUE);
    menuCheckLockTAZs->setCheck(TRUE);
    menuCheckLockWires->setCheck(TRUE);
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
    menuCheckLockWalkingAreas->setCheck(FALSE);
    menuCheckLockAdditionals->setCheck(FALSE);
    menuCheckLockTAZs->setCheck(FALSE);
    menuCheckLockWires->setCheck(FALSE);
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
    } else if (AC->getTagProperty().getTag() == SUMO_TAG_WALKINGAREA) {
        menuCheckLockWalkingAreas->setCheck(value);
    } else if (AC->getTagProperty().isAdditionalElement()) {
        menuCheckLockAdditionals->setCheck(value);
    } else if (AC->getTagProperty().isTAZElement()) {
        menuCheckLockTAZs->setCheck(value);
    } else if (AC->getTagProperty().isWireElement()) {
        menuCheckLockWires->setCheck(value);
    } else if (AC->getTagProperty().getTag() == SUMO_TAG_POLY) {
        menuCheckLockPolygons->setCheck(value);
    } else if (AC->getTagProperty().isShapeElement() && (AC->getTagProperty().getTag() != SUMO_TAG_POLY)) {
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
    } else if (AC->getTagProperty().getTag() == GNE_TAG_EDGEREL_SINGLE) {
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
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindowHelper::ProcessingMenuCommands::buildProcessingMenuCommands(FXMenuPane* processingMenu) {
    // build network processing menu commands
    computeNetwork = GUIDesigns::buildFXMenuCommandShortcut(processingMenu,
                     TL("Compute Junctions"), "F5", TL("Compute junction shape and logic."),
                     GUIIconSubSys::getIcon(GUIIcon::COMPUTEJUNCTIONS), myGNEApp, MID_HOTKEY_F5_COMPUTE_NETWORK_DEMAND);
    computeNetworkVolatile = GUIDesigns::buildFXMenuCommandShortcut(processingMenu,
                             TL("Compute Junctions with volatile options"), "Shift+F5", TL("Compute junction shape and logic using volatile junctions."),
                             GUIIconSubSys::getIcon(GUIIcon::COMPUTEJUNCTIONS), myGNEApp, MID_HOTKEY_SHIFT_F5_COMPUTEJUNCTIONS_VOLATILE);
    cleanJunctions = GUIDesigns::buildFXMenuCommandShortcut(processingMenu,
                     TL("Clean Junctions"), "F6", TL("Remove solitary junctions."),
                     GUIIconSubSys::getIcon(GUIIcon::CLEANJUNCTIONS), myGNEApp, MID_HOTKEY_F6_CLEAN_SOLITARYJUNCTIONS_UNUSEDROUTES);
    joinJunctions = GUIDesigns::buildFXMenuCommandShortcut(processingMenu,
                    TL("Join Selected Junctions"), "F7", TL("Join selected junctions into a single junction."),
                    GUIIconSubSys::getIcon(GUIIcon::JOINJUNCTIONS), myGNEApp, MID_HOTKEY_F7_JOIN_SELECTEDJUNCTIONS_ROUTES);
    clearInvalidCrossings = GUIDesigns::buildFXMenuCommandShortcut(processingMenu,
                            TL("Clean invalid crossings"), "F8", TL("Clear invalid crossings."),
                            GUIIconSubSys::getIcon(GUIIcon::CLEANJUNCTIONS), myGNEApp, MID_HOTKEY_F8_CLEANINVALID_CROSSINGS_DEMANDELEMENTS);
    // add separator for checkBox
    mySeparatorCheckBox = new FXMenuSeparator(processingMenu);
    // add checkBox for recomputing in data mode
    menuCheckRecomputeDataMode = GUIDesigns::buildFXMenuCheckboxIcon(processingMenu,
                                 TL("Recompute Network in Data Mode"), "", "",
                                 GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDATA), myGNEApp, MID_GNE_TOGGLE_COMPUTE_NETWORK_DATA);
    // set default value
    menuCheckRecomputeDataMode->setCheck(myGNEApp->getApp()->reg().readBoolEntry("netedit", "RecomputeData", true));
    // build demand  processing menu commands
    computeDemand = GUIDesigns::buildFXMenuCommandShortcut(processingMenu,
                    TL("Compute demand"), "F5", TL("Computes demand elements."),
                    GUIIconSubSys::getIcon(GUIIcon::COMPUTEDEMAND), myGNEApp, MID_HOTKEY_F5_COMPUTE_NETWORK_DEMAND);
    cleanRoutes = GUIDesigns::buildFXMenuCommandShortcut(processingMenu,
                  TL("Clean routes"), "F6", TL("Removes routes without vehicles."),
                  GUIIconSubSys::getIcon(GUIIcon::CLEANROUTES), myGNEApp, MID_HOTKEY_F6_CLEAN_SOLITARYJUNCTIONS_UNUSEDROUTES);
    joinRoutes = GUIDesigns::buildFXMenuCommandShortcut(processingMenu,
                 TL("Join routes"), "F7", TL("Joins routes with the same edges."),
                 GUIIconSubSys::getIcon(GUIIcon::JOINROUTES), myGNEApp, MID_HOTKEY_F7_JOIN_SELECTEDJUNCTIONS_ROUTES);
    adjustPersonPlans = GUIDesigns::buildFXMenuCommandShortcut(processingMenu,
                        TL("Adjust person plans"), "Shift+F7", TL("Adjust person plans (start/end positions, arrival positions, etc.)"),
                        GUIIconSubSys::getIcon(GUIIcon::ADJUSTPERSONPLANS), myGNEApp, MID_HOTKEY_SHIFT_F7_ADJUST_PERSON_PLANS);
    clearInvalidDemandElements = GUIDesigns::buildFXMenuCommandShortcut(processingMenu,
                                 TL("Clean invalid route elements"), "F8", TL("Clear elements with an invalid path (routes, Trips, Flows...)."),
                                 GUIIconSubSys::getIcon(GUIIcon::CLEANJUNCTIONS), myGNEApp, MID_HOTKEY_F8_CLEANINVALID_CROSSINGS_DEMANDELEMENTS);
    // add separator
    myOptionsSeparator = new FXMenuSeparator(processingMenu);
    // create optionmenus
    optionMenus = GUIDesigns::buildFXMenuCommandShortcut(processingMenu,
                  TL("Sumo options"), "Shift+F10", TL("Configure sumo Options."),
                  GUIIconSubSys::getIcon(GUIIcon::SUMO_MINI), myGNEApp, MID_HOTKEY_SHIFT_F10_SUMOOPTIONSMENU);
    // add separator
    myOptionsSeparator = new FXMenuSeparator(processingMenu);
    // create optionmenus
    optionMenus = GUIDesigns::buildFXMenuCommandShortcut(processingMenu,
                  TL("Options"), "F10", TL("Configure netedit Options."),
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

GNEApplicationWindowHelper::LocateMenuCommands::LocateMenuCommands(GNEApplicationWindow* GNEApp) :
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindowHelper::LocateMenuCommands::buildLocateMenuCommands(FXMenuPane* locateMenu) {
    // build locate menu commands
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           TL("&Junctions"), "Shift+J", TL("Open a dialog for locating a Junction."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEJUNCTION), myGNEApp, MID_HOTKEY_SHIFT_J_LOCATEJUNCTION);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           TL("&Edges"), "Shift+E", TL("Open a dialog for locating an Edge."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEEDGE), myGNEApp, MID_HOTKEY_SHIFT_E_LOCATEEDGE);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           TL("&WalkingAreas"), "Shift+W", TL("Open a dialog for locating a Walking Area."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEWALKINGAREA), myGNEApp, MID_HOTKEY_SHIFT_W_LOCATEWALKINGAREA);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           TL("&Vehicles"), "Shift+V", TL("Open a dialog for locating a Vehicle."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEVEHICLE), myGNEApp, MID_HOTKEY_SHIFT_V_LOCATEVEHICLE);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           TL("&Persons"), "Shift+P", TL("Open a dialog for locating a Person."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEPERSON), myGNEApp, MID_HOTKEY_SHIFT_P_LOCATEPERSON);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           TL("&Route"), "Shift+R", TL("Open a dialog for locating a Route."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEROUTE), myGNEApp, MID_HOTKEY_SHIFT_R_LOCATEROUTE);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           TL("&Stops"), "Shift+S", TL("Open a dialog for locating a Stop."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATESTOP), myGNEApp, MID_HOTKEY_SHIFT_S_LOCATESTOP);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           TL("&TLS"), "Shift+T", TL("Open a dialog for locating a Traffic Light."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATETLS), myGNEApp, MID_HOTKEY_SHIFT_T_LOCATETLS);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           TL("&Additional"), "Shift+A", TL("Open a dialog for locating an Additional Structure."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEADD), myGNEApp, MID_HOTKEY_SHIFT_A_LOCATEADDITIONAL);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           TL("P&oI"), "Shift+O", TL("Open a dialog for locating a Point of Interest."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEPOI), myGNEApp, MID_HOTKEY_SHIFT_O_LOCATEPOI);
    GUIDesigns::buildFXMenuCommandShortcut(locateMenu,
                                           TL("Po&lygon"), "Shift+L", TL("Open a dialog for locating a Polygon."),
                                           GUIIconSubSys::getIcon(GUIIcon::LOCATEPOLY), myGNEApp, MID_HOTKEY_SHIFT_L_LOCATEPOLY);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::ToolsMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::ToolsMenuCommands::ToolsMenuCommands(GNEApplicationWindow* GNEApp) :
    myGNEApp(GNEApp) {
}


GNEApplicationWindowHelper::ToolsMenuCommands::~ToolsMenuCommands() {
    // delete all tools
    for (const auto& tool : myPythonTools) {
        delete tool;
    }
    // delete dialogs
    delete myPythonToolDialog;
    delete myNetgenerateDialog;
    delete myRunPythonToolDialog;
    delete myRunNetgenerateDialog;
}


void
GNEApplicationWindowHelper::ToolsMenuCommands::buildTools(FXMenuPane* toolsMenu,
        const std::map<std::string, FXMenuPane*>& menuPaneToolMaps) {
    // build template tools
    for (const auto& templateTool : templateTools) {
        if (templateTool.name == "netdiff") {
            myPythonTools.push_back(new GNENetDiffTool(myGNEApp, templateTool.pythonPath,
                                    menuPaneToolMaps.at(templateTool.subfolder)));
        } else if (menuPaneToolMaps.count(templateTool.subfolder) > 0) {
            myPythonTools.push_back(new GNEPythonTool(myGNEApp, templateTool.pythonPath,
                                    templateTool.templateStr, menuPaneToolMaps.at(templateTool.subfolder)));
        } else {
            myPythonTools.push_back(new GNEPythonTool(myGNEApp, templateTool.pythonPath,
                                    templateTool.templateStr, toolsMenu));
        }
    }
    // build dialogs
    myPythonToolDialog = new GNEPythonToolDialog(myGNEApp);
    myNetgenerateDialog = new GNENetgenerateDialog(myGNEApp);
    myRunPythonToolDialog = new GNERunPythonToolDialog(myGNEApp);
    myRunNetgenerateDialog = new GNERunNetgenerateDialog(myGNEApp);
}


long
GNEApplicationWindowHelper::ToolsMenuCommands::showTool(FXObject* menuCommand) const {
    // iterate over all tools and find menu command
    for (const auto& tool : myPythonTools) {
        if (tool->getMenuCommand() == menuCommand) {
            myPythonToolDialog->openDialog(tool);
            return 1;
        }
    }
    return 0;
}


long
GNEApplicationWindowHelper::ToolsMenuCommands::showNetgenerateDialog() const {
    myNetgenerateDialog->openDialog();
    return 1;
}


long
GNEApplicationWindowHelper::ToolsMenuCommands::runToolDialog(FXObject* menuCommand) const {
    // iterate over all tools and find menu command
    for (const auto& tool : myPythonTools) {
        if (tool->getMenuCommand() == menuCommand) {
            myRunPythonToolDialog->runTool(tool);
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


long
GNEApplicationWindowHelper::ToolsMenuCommands::runNetgenerateDialog(const OptionsCont* netgenerateOptions) const {
    myRunNetgenerateDialog->run(netgenerateOptions);
    return 0;
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::WindowsMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::WindowsMenuCommands::WindowsMenuCommands(GNEApplicationWindow* GNEApp) :
    myGNEApp(GNEApp) {
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
                                           GUIIconSubSys::getIcon(GUIIcon::CLEARMESSAGEWINDOW), myGNEApp, MID_CLEARMESSAGEWINDOW);
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper::WindowsMenuCommands - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::HelpMenuCommands::HelpMenuCommands(GNEApplicationWindow* GNEApp) :
    myGNEApp(GNEApp) {
}


void
GNEApplicationWindowHelper::HelpMenuCommands::buildHelpMenuCommands(FXMenuPane* helpMenu) {
    // build windows menu commands
    GUIDesigns::buildFXMenuCommandShortcut(helpMenu, TL("&Online Documentation"), "F1", TL("Open Online documentation."),
                                           nullptr, myGNEApp, MID_HOTKEY_F1_ONLINEDOCUMENTATION);
    new FXMenuSeparator(helpMenu);
    GUIDesigns::buildFXMenuCommandShortcut(helpMenu, TL("&Changelog"), "", TL("Open Changelog."),
                                           nullptr, myGNEApp, MID_CHANGELOG);
    GUIDesigns::buildFXMenuCommandShortcut(helpMenu, TL("&Hotkeys"), "", TL("Open Hotkeys."),
                                           nullptr, myGNEApp, MID_HOTKEYS);
    GUIDesigns::buildFXMenuCommandShortcut(helpMenu, TL("&Tutorial"), "", TL("Open Tutorial."),
                                           nullptr, myGNEApp, MID_TUTORIAL);
    GUIDesigns::buildFXMenuCommandShortcut(helpMenu, TL("&Feedback"), "", TL("Open feedback channels."),
                                           nullptr, myGNEApp, MID_FEEDBACK);
    new FXMenuSeparator(helpMenu);
    GUIDesigns::buildFXMenuCommandShortcut(helpMenu, TL("&About"), "F12", TL("About netedit."),
                                           GUIIconSubSys::getIcon(GUIIcon::NETEDIT_MINI), myGNEApp, MID_HOTKEY_F12_ABOUT);
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
                  TL("&Network"), "F2", TL("Select network mode."),
                  GUIIconSubSys::getIcon(GUIIcon::SUPERMODENETWORK), myGNEApp, MID_HOTKEY_F2_SUPERMODE_NETWORK);
    demandMode = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
                 TL("&Demand"), "F3", TL("Select demand mode."),
                 GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDEMAND), myGNEApp, MID_HOTKEY_F3_SUPERMODE_DEMAND);
    dataMode = GUIDesigns::buildFXMenuCommandShortcut(editMenu,
               TL("&Data"), "F4", TL("Select data mode."),
               GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDATA), myGNEApp, MID_HOTKEY_F4_SUPERMODE_DATA);
}

// ---------------------------------------------------------------------------
// GNESumoConfigHandler - methods
// ---------------------------------------------------------------------------

GNEApplicationWindowHelper::GNESumoConfigHandler::GNESumoConfigHandler(OptionsCont& sumoOptions, const std::string& file) :
    mySumoOptions(sumoOptions),
    myFile(file) {
}


bool
GNEApplicationWindowHelper::GNESumoConfigHandler::loadSumoConfig() {
    // get options
    auto& neteditOptions = OptionsCont::getOptions();
    // make all options writables
    mySumoOptions.resetWritable();
    neteditOptions.resetWritable();
    // build parser
    XERCES_CPP_NAMESPACE::SAXParser parser;
    parser.setValidationScheme(XERCES_CPP_NAMESPACE::SAXParser::Val_Never);
    parser.setDisableDefaultEntityResolution(true);
    // start the parsing
    OptionsLoader handler(mySumoOptions);
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
    mySumoOptions.relocateFiles(myFile);
    // set loaded files in netedit options
    neteditOptions.set("sumocfg-file", myFile);
    neteditOptions.set("net-file", mySumoOptions.getString("net-file"));
    neteditOptions.set("additional-files", mySumoOptions.getString("additional-files"));
    neteditOptions.set("route-files", mySumoOptions.getString("route-files"));
    // check if we need to define the configuration file
    if (neteditOptions.getString("configuration-file").empty()) {
        const auto newConfiguration = StringUtils::replace(neteditOptions.getString("configuration-file"), ".sumocfg", ".neteditcfg");
        neteditOptions.resetWritable();
        neteditOptions.set("configuration-file", newConfiguration);
    }
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
        const auto newConfiguration = StringUtils::replace(neteditOptions.getString("configuration-file"), ".netccfg", ".neteditcfg");
        neteditOptions.resetWritable();
        neteditOptions.set("configuration-file", newConfiguration);
    }
    return true;
}

// ---------------------------------------------------------------------------
// GNEApplicationWindowHelper - methods
// ---------------------------------------------------------------------------

bool
GNEApplicationWindowHelper::toggleEditOptionsNetwork(GNEViewNet* viewNet, const MFXCheckableButton* menuCheck,
        const int numericalKeyPressed, FXObject* obj, FXSelector sel) {
    // finally function correspond to visibleMenuCommands[numericalKeyPressed]
    if (menuCheck == viewNet->getNetworkViewOptions().menuCheckToggleGrid) {
        // Toggle menuCheckToggleGrid
        if (viewNet->getNetworkViewOptions().menuCheckToggleGrid->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled toggle show grid through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled toggle show grid through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowGrid
        viewNet->onCmdToggleShowGrid(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckToggleDrawJunctionShape) {
        // Toggle menuCheckToggleDrawJunctionShape
        if (viewNet->getNetworkViewOptions().menuCheckToggleDrawJunctionShape->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled draw junction shape through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled draw junction shape through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleDrawJunctionShape
        viewNet->onCmdToggleDrawJunctionShape(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckDrawSpreadVehicles) {
        // Toggle menuCheckDrawSpreadVehicles
        if (viewNet->getNetworkViewOptions().menuCheckDrawSpreadVehicles->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled toggle draw spread vehicles through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled toggle spread vehicles through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleDrawSpreadVehicles
        viewNet->onCmdToggleDrawSpreadVehicles(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckShowDemandElements) {
        // Toggle menuCheckShowDemandElements
        if (viewNet->getNetworkViewOptions().menuCheckShowDemandElements->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled show demand elements through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled show demand elements through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowDemandElementsNetwork
        viewNet->onCmdToggleShowDemandElementsNetwork(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckSelectEdges) {
        // Toggle menuCheckSelectEdges
        if (viewNet->getNetworkViewOptions().menuCheckSelectEdges->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled select edges through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled select edges through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleSelectEdges
        viewNet->onCmdToggleSelectEdges(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckShowConnections) {
        // Toggle menuCheckShowConnections
        if (viewNet->getNetworkViewOptions().menuCheckShowConnections->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled show connections through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled show connections through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowConnections
        viewNet->onCmdToggleShowConnections(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckHideConnections) {
        // Toggle menuCheckHideConnections
        if (viewNet->getNetworkViewOptions().menuCheckHideConnections->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled hide connections through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled hide connections through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleHideConnections
        viewNet->onCmdToggleHideConnections(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckShowAdditionalSubElements) {
        // Toggle menuCheckShowAdditionalSubElements
        if (viewNet->getNetworkViewOptions().menuCheckShowAdditionalSubElements->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled show additional sub-elements through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled show additional sub-elements through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowAdditionalSubElements
        viewNet->onCmdToggleShowAdditionalSubElements(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckShowTAZElements) {
        // Toggle menuCheckShowTAZElements
        if (viewNet->getNetworkViewOptions().menuCheckShowTAZElements->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled show TAZ elements through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled show TAZ elements through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowTAZElements
        viewNet->onCmdToggleShowTAZElements(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckExtendSelection) {
        // Toggle menuCheckExtendSelection
        if (viewNet->getNetworkViewOptions().menuCheckExtendSelection->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled extend selection through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled extend selection through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleExtendSelection
        viewNet->onCmdToggleExtendSelection(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckChangeAllPhases) {
        // Toggle menuCheckChangeAllPhases
        if (viewNet->getNetworkViewOptions().menuCheckChangeAllPhases->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled change all phases through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled change all phases through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleChangeAllPhases
        viewNet->onCmdToggleChangeAllPhases(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckWarnAboutMerge) {
        // Toggle menuCheckWarnAboutMerge
        if (viewNet->getNetworkViewOptions().menuCheckWarnAboutMerge->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled warn about merge through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled warn about merge through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleWarnAboutMerge
        viewNet->onCmdToggleWarnAboutMerge(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckShowJunctionBubble) {
        // Toggle menuCheckShowJunctionBubble
        if (viewNet->getNetworkViewOptions().menuCheckShowJunctionBubble->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled show junction as bubble through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled show junction as bubble through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowJunctionBubble
        viewNet->onCmdToggleShowJunctionBubbles(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckMoveElevation) {
        // Toggle menuCheckMoveElevation
        if (viewNet->getNetworkViewOptions().menuCheckMoveElevation->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled move elevation through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled move elevation through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleMoveElevation
        viewNet->onCmdToggleMoveElevation(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckChainEdges) {
        // Toggle menuCheckChainEdges
        if (viewNet->getNetworkViewOptions().menuCheckChainEdges->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled chain edges through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled chain edges through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleChainEdges
        viewNet->onCmdToggleChainEdges(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getNetworkViewOptions().menuCheckAutoOppositeEdge) {
        // Toggle menuCheckAutoOppositeEdge
        if (viewNet->getNetworkViewOptions().menuCheckAutoOppositeEdge->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled auto opposite edge through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled auto opposite edge through alt + " + toString(numericalKeyPressed + 1));
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
            WRITE_DEBUG("Disabled toggle show grid through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled toggle show grid through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowGrid
        viewNet->onCmdToggleShowGrid(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckToggleDrawJunctionShape) {
        // Toggle menuCheckToggleDrawJunctionShape
        if (viewNet->getDemandViewOptions().menuCheckToggleDrawJunctionShape->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled draw junction shape through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled draw junction shape through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleDrawJunctionShape
        viewNet->onCmdToggleDrawJunctionShape(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckDrawSpreadVehicles) {
        // Toggle menuCheckDrawSpreadVehicles
        if (viewNet->getDemandViewOptions().menuCheckDrawSpreadVehicles->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled toggle draw spread vehicles through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled toggle spread vehicles through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleDrawSpreadVehicles
        viewNet->onCmdToggleDrawSpreadVehicles(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckHideShapes) {
        // Toggle menuCheckHideShapes
        if (viewNet->getDemandViewOptions().menuCheckHideShapes->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled hide shapes through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled hide shapes through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleHideShapes
        viewNet->onCmdToggleHideShapes(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckShowAllTrips) {
        // Toggle menuCheckShowAllTrips
        if (viewNet->getDemandViewOptions().menuCheckShowAllTrips->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled show all trips through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled show all trips through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowTrips
        viewNet->onCmdToggleShowTrips(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckShowAllPersonPlans) {
        // Toggle menuCheckShowAllPersonPlans
        if (viewNet->getDemandViewOptions().menuCheckShowAllPersonPlans->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled show all person plans through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled show all person plans through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowAllPersonPlans
        viewNet->onCmdToggleShowAllPersonPlans(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckLockPerson) {
        // Toggle menuCheckShowAllPersonPlans
        if (viewNet->getDemandViewOptions().menuCheckLockPerson->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled lock person plan through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled lock person plan through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleLockPerson
        viewNet->onCmdToggleLockPerson(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckShowAllContainerPlans) {
        // Toggle menuCheckShowAllContainerPlans
        if (viewNet->getDemandViewOptions().menuCheckShowAllContainerPlans->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled show all container plans through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled show all container plans through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowAllContainerPlans
        viewNet->onCmdToggleShowAllContainerPlans(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckLockContainer) {
        // Toggle menuCheckShowAllContainerPlans
        if (viewNet->getDemandViewOptions().menuCheckLockContainer->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled lock container plan through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled lock container plan through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleLockContainer
        viewNet->onCmdToggleLockContainer(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckHideNonInspectedDemandElements) {
        // Toggle menuCheckHideNonInspectedDemandElements
        if (viewNet->getDemandViewOptions().menuCheckHideNonInspectedDemandElements->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled hide non inspected demand elements through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled hide non inspected demand elements through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleHideNonInspecteDemandElements
        viewNet->onCmdToggleHideNonInspecteDemandElements(obj, sel, nullptr);

    } else if (menuCheck == viewNet->getDemandViewOptions().menuCheckShowOverlappedRoutes) {
        // Toggle menuCheckShowOverlappedRoutes
        if (viewNet->getDemandViewOptions().menuCheckShowOverlappedRoutes->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled show overlapped routes through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled show overlapped routes through alt + " + toString(numericalKeyPressed + 1));
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
            WRITE_DEBUG("Disabled draw junction shape through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled draw junction shape through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleDrawJunctionShape
        viewNet->onCmdToggleDrawJunctionShape(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDataViewOptions().menuCheckShowAdditionals) {
        // Toggle menuCheckHideShapes
        if (viewNet->getDataViewOptions().menuCheckShowAdditionals->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled show additionals through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled show shapes through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowAdditionals
        viewNet->onCmdToggleShowAdditionals(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDataViewOptions().menuCheckShowShapes) {
        // Toggle menuCheckHideShapes
        if (viewNet->getDataViewOptions().menuCheckShowShapes->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled show shapes through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled show shapes through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowShapes
        viewNet->onCmdToggleShowShapes(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDataViewOptions().menuCheckShowDemandElements) {
        // Toggle menuCheckShowDemandElements
        if (viewNet->getDataViewOptions().menuCheckShowDemandElements->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled show demand elements through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled show demand elements through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleShowDemandElementsData
        viewNet->onCmdToggleShowDemandElementsData(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDataViewOptions().menuCheckToggleTAZRelDrawing) {
        // Toggle menuCheckToggleTAZRelDrawing
        if (viewNet->getDataViewOptions().menuCheckToggleTAZRelDrawing->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled toggle TAXRel drawing through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled toggle TAXRel drawing through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleTAZRelDrawing
        viewNet->onCmdToggleTAZRelDrawing(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDataViewOptions().menuCheckToggleTAZDrawFill) {
        // Toggle menuCheckToggleTAZDrawFill
        if (viewNet->getDataViewOptions().menuCheckToggleTAZDrawFill->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled toggle TAZ draw fill through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled toggle TAZ draw fill through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleTAZDrawFill
        viewNet->onCmdToggleTAZDrawFill(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDataViewOptions().menuCheckToggleTAZRelOnlyFrom) {
        // Toggle menuCheckToggleTAZRelOnlyFrom
        if (viewNet->getDataViewOptions().menuCheckToggleTAZRelOnlyFrom->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled toggle TAZRel only from through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled toggle TAZRel only from through alt + " + toString(numericalKeyPressed + 1));
        }
        // Call manually onCmdToggleTAZRelOnlyFrom
        viewNet->onCmdToggleTAZRelOnlyFrom(obj, sel, nullptr);
    } else if (menuCheck == viewNet->getDataViewOptions().menuCheckToggleTAZRelOnlyTo) {
        // Toggle menuCheckToggleTAZRelOnlyTo
        if (viewNet->getDataViewOptions().menuCheckToggleTAZRelOnlyTo->amChecked() == TRUE) {
            // show extra information for tests
            WRITE_DEBUG("Disabled toggle TAZRel only to through alt + " + toString(numericalKeyPressed + 1));
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled toggle TAZRel only to through alt + " + toString(numericalKeyPressed + 1));
        }
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


std::string
GNEApplicationWindowHelper::openFileDialog(FXWindow* window, bool save, bool multi) {
    if (save) {
        return openFileDialog(window, TL("Save file as"), GUIIcon::SAVE,
                              TL("XML files (*.xml)") + std::string("\n") +
                              TL("All files (*)"), save, multi);
    } else {
        return openFileDialog(window, TL("Open file"), GUIIcon::OPEN_SUMOCONFIG,
                              TL("XML files (*.xml)") + std::string("\n") +
                              TL("All files (*)"), save, multi);
    }
}


std::string
GNEApplicationWindowHelper::openNetworkFileDialog(FXWindow* window, bool save, bool multi) {
    if (save) {
        return openFileDialog(window, TL("Save Network file as"), GUIIcon::SAVE_NETWORKELEMENTS,
                              TL("SUMO Network files (*.net.xml)") + std::string("\n") +
                              TL("SUMO Network files zipped (*.net.xml.gz)") + std::string("\n") +
                              TL("XML files (*.xml)\nAll files (*)"), save, multi);
    } else {
        return openFileDialog(window, TL("Open Network file"), GUIIcon::OPEN_NET,
                              TL("SUMO Network files (*.net.xml,*.net.xml.gz)") + std::string("\n") +
                              TL("XML files (*.xml)") + std::string("\n") +
                              TL("All files (*)"), save, multi);
    }
}


std::string
GNEApplicationWindowHelper::openNetconvertFileDialog(FXWindow* window) {
    return openFileDialog(window, TL("Open NetConvert file"), GUIIcon::OPEN_NET,
                          TL("Netconvert files (*.netccfg)") + std::string("\n") +
                          TL("XML files (*.xml)") + std::string("\n") +
                          TL("All files (*)"), false);
}


std::string
GNEApplicationWindowHelper::savePlainXMLFileDialog(FXWindow* window) {
    return openFileDialog(window, TL("Save plain XML as"), GUIIcon::SAVE, "", true);
}


std::string
GNEApplicationWindowHelper::saveJoinedJunctionsFileDialog(FXWindow* window) {
    return openFileDialog(window, TL("Save joined Junctions as"), GUIIcon::SAVE,
                          TL("Junction files (*.nod.xml)") + std::string("\n") +
                          TL("XML files (*.xml)") + std::string("\n") +
                          TL("All files (*)"), true);
}


std::string
GNEApplicationWindowHelper::saveToolLog(FXWindow* window) {
    return openFileDialog(window, TL("Save tool log"), GUIIcon::SAVE,
                          TL("Text file (*.txt)") + std::string("\n") +
                          TL("All files (*)"), true);
}


std::string
GNEApplicationWindowHelper::openOSMFileDialog(FXWindow* window) {
    return openFileDialog(window, TL("Open OSM file"), GUIIcon::OPEN_NET,
                          TL("OSM net (*.osm.xml,*.osm)"), false);
}


std::string
GNEApplicationWindowHelper::openNeteditConfigFileDialog(FXWindow* window, bool save) {
    if (save) {
        return openFileDialog(window, TL("Save netedit Config file as"), GUIIcon::SAVE_NETEDITCONFIG,
                              TL("Netedit Config files (*.neteditcfg)") + std::string("\n") +
                              TL("All files (*)"), save);
    } else {
        return openFileDialog(window, TL("Open netedit Config file"), GUIIcon::OPEN_NETEDITCONFIG,
                              TL("Netedit Config files (*.neteditcfg)") + std::string("\n") +
                              TL("All files (*)"), save);
    }
}


std::string
GNEApplicationWindowHelper::openSumoConfigFileDialog(FXWindow* window, bool save, bool multi) {
    if (save) {
        return openFileDialog(window, TL("Save SUMO Config file as"), GUIIcon::SAVE_SUMOCONFIG,
                              TL("SUMO Config files (*.sumocfg)") + std::string("\n") +
                              TL("All files (*)"), save, multi);
    } else {
        return openFileDialog(window, TL("Open SUMO Config file"), GUIIcon::OPEN_SUMOCONFIG,
                              TL("SUMO Config files (*.sumocfg)") + std::string("\n") +
                              TL("All files (*)"), save, multi);
    }
}


std::string
GNEApplicationWindowHelper::openTLSFileDialog(FXWindow* window, bool save) {
    if (save) {
        return openFileDialog(window, TL("Save TLS file as"), GUIIcon::SAVE_NETWORKELEMENTS,
                              TL("TLS files (*.tll.xml)") + std::string("\n") +
                              TL("XML files (*.xml)") + std::string("\n") +
                              TL("All files (*)"), save);
    } else {
        return openFileDialog(window, TL("Open TLS file"), GUIIcon::OPEN_TLSPROGRAMS,
                              TL("TLS files (*.tll.xml)") + std::string("\n") +
                              TL("XML files (*.xml)") + std::string("\n") +
                              TL("All files (*)"), save);
    }
}


std::string
GNEApplicationWindowHelper::openEdgeTypeFileDialog(FXWindow* window, bool save) {
    if (save) {
        return openFileDialog(window, TL("Save EdgeType file as"), GUIIcon::SAVE_NETWORKELEMENTS,
                              TL("EdgeType files (*.tll.xml)") + std::string("\n") +
                              TL("XML files (*.xml)") + std::string("\n") +
                              TL("All files (*)"), save);
    } else {
        return openFileDialog(window, TL("Open EdgeType file"), GUIIcon::OPEN_NET,
                              TL("EdgeType files (*.typ.xml)") + std::string("\n") +
                              TL("XML files (*.xml)") + std::string("\n") +
                              TL("All files (*)"), save);
    }
}


std::string
GNEApplicationWindowHelper::openAdditionalFileDialog(FXWindow* window, bool save, bool multi) {
    if (save) {
        return openFileDialog(window, TL("Save Additionals file as"), GUIIcon::SAVE_ADDITIONALELEMENTS,
                              TL("Additional files (*.add.xml)") + std::string("\n") +
                              TL("XML files (*.xml)") + std::string("\n") +
                              TL("All files (*)"), save, multi);
    } else {
        return openFileDialog(window, TL("Open Additionals file"), GUIIcon::MODEADDITIONAL,
                              TL("Additional files (*.add.xml)") + std::string("\n") +
                              TL("XML files (*.xml)") + std::string("\n") +
                              TL("All files (*)"), save, multi);
    }
}


std::string
GNEApplicationWindowHelper::openRouteFileDialog(FXWindow* window, bool save, bool multi) {
    if (save) {
        return openFileDialog(window, TL("Save Route file as"), GUIIcon::SAVE_DEMANDELEMENTS,
                              TL("Route files (*.rou.xml)") + std::string("\n") +
                              TL("XML files (*.xml)") + std::string("\n") +
                              TL("All files (*)"), save, multi);
    } else {
        return openFileDialog(window, TL("Open Route file"), GUIIcon::SUPERMODEDEMAND,
                              TL("Route files (*.rou.xml)") + std::string("\n") +
                              TL("XML files (*.xml)") + std::string("\n") +
                              TL("All files (*)"), save, multi);
    }
}


std::string
GNEApplicationWindowHelper::openDataFileDialog(FXWindow* window, bool save, bool multi) {
    if (save) {
        return openFileDialog(window, TL("Save Data file as"), GUIIcon::SAVE_DATAELEMENTS,
                              TL("Data files (*.dat.xml)") + std::string("\n") +
                              TL("XML files (*.xml)") + std::string("\n") +
                              TL("All files (*)"), save, multi);
    } else {
        return openFileDialog(window, TL("Open Data file"), GUIIcon::SUPERMODEDATA,
                              TL("Data files (*.dat.xml)") + std::string("\n") +
                              TL("XML files (*.xml)") + std::string("\n") +
                              TL("All files (*)"), save, multi);
    }
}


std::string
GNEApplicationWindowHelper::openMeanDataDialog(FXWindow* window, bool save, bool multi) {
    if (save) {
        return openFileDialog(window, TL("Save MeanData file as"), GUIIcon::SAVE_MEANDATAELEMENTS,
                              TL("Meandata files (*.add.xml)") + std::string("\n") +
                              TL("XML files (*.xml)") + std::string("\n") +
                              TL("All files (*)"), save, multi);
    } else {
        return openFileDialog(window, TL("Open MeanData file"), GUIIcon::MODEMEANDATA,
                              TL("Meandata files (*.add.xml)") + std::string("\n") +
                              TL("XML files (*.xml)") + std::string("\n") +
                              TL("All files (*)"), save, multi);
    }
}


std::string
GNEApplicationWindowHelper::openOptionFileDialog(FXWindow* window, bool save) {
    if (save) {
        return openFileDialog(window, TL("Save options file as"), GUIIcon::SAVE,
                              TL("XML files (*.xml)") + std::string("\n") +
                              TL("All files (*)"), save);
    } else {
        return openFileDialog(window, TL("Open options file"), GUIIcon::OPEN,
                              TL("XML files (*.xml)") + std::string("\n") +
                              TL("All files (*)"), save);
    }
}


std::string
GNEApplicationWindowHelper::openFileDialog(FXWindow* window, const std::string title,
        GUIIcon icon, const std::string patternList, bool save, bool multi) {
    // write title information
    WRITE_DEBUG(title);
    // configure open dialog
    FXFileDialog opendialog(window, title.c_str());
    // check if allow to create a new file, or select only existent files
    if (save) {
        opendialog.setSelectMode(SELECTFILE_ANY);
    } else {
        if (multi) { 
            opendialog.setSelectMode(SELECTFILE_MULTIPLE);
        } else {
            opendialog.setSelectMode(SELECTFILE_EXISTING);
        }
    }
    // set icon and pattern list
    opendialog.setIcon(GUIIconSubSys::getIcon(icon));
    opendialog.setPatternList(patternList.c_str());
    // set current folder
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    // open dialog
    if (opendialog.execute()) {
        // continue depending if we're loading or saving
        if (save) {
            // check if overwritte file
            if (MFXUtils::userPermitsOverwritingWhenFileExists(window, opendialog.getFilename())) {
                // close save dialog
                WRITE_DEBUG("Close save dialog sucesfully");
                // udpate current folder
                gCurrentFolder = opendialog.getDirectory();
                // assureExtension
                return MFXUtils::assureExtension(opendialog.getFilename(),
                        opendialog.getPatternText(opendialog.getCurrentPattern()).after('.').before(')')).text();
            } else {
                // close additional dialog
                WRITE_DEBUG("Abort overwritte file");
                // return empty file
                return "";
            }
        } else {
            // close load dialog
            WRITE_DEBUG("Close load dialog sucesfully");
            // udpate current folder
            gCurrentFolder = opendialog.getDirectory();
            // return file
            if (multi) {
                FXString* files = opendialog.getFilenames();
                std::string result;
                bool first = true;
                if (files != nullptr) {
                    for(int i=0; !files[i].empty(); i++){
                        if (first) {
                            first = false;
                        } else {
                            result += " ";
                        }
                        result += files[i].text();
                    }
                    delete [] files;

                } 
                return result;
            } else {
                return opendialog.getFilename().text();
            }
        }
    } else {
        // close additional dialog
        WRITE_DEBUG("Abort dialog");
        // return empty file
        return "";
    }
}

/****************************************************************************/
