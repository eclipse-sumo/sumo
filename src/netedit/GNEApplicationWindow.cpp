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
/// @file    GNEApplicationWindow.cpp
/// @author  Pablo Alvarez Lopez
/// @date    mar 2020
///
// Functions from main window of NETEDIT
/****************************************************************************/
#include <netbuild/NBFrame.h>
#include <netedit/dialogs/GNEAbout.h>
#include <netedit/dialogs/GNEUndoListDialog.h>
#include <netedit/dialogs/GNEOverwriteElementsDialog.h>
#include <netedit/dialogs/tools/GNEToolNetDiff.h>
#include <netedit/elements/network/GNECrossing.h>
#include <netedit/elements/network/GNEEdgeType.h>
#include <netedit/elements/network/GNELaneType.h>
#include <netedit/elements/GNEGeneralHandler.h>
#include <netedit/elements/data/GNEDataHandler.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <netedit/frames/network/GNECreateEdgeFrame.h>
#include <netedit/frames/network/GNETAZFrame.h>
#include <netedit/frames/network/GNETLSEditorFrame.h>
#include <netedit/changes/GNEChange_EdgeType.h>
#include <netedit/dialogs/GNEFixNetworkElements.h>
#include <netimport/NIFrame.h>
#include <netimport/NIXMLTypesHandler.h>
#include <netimport/NITypeLoader.h>
#include <netwrite/NWFrame.h>
#include <utils/common/SystemFrame.h>
#include <utils/foxtools/MFXLinkLabel.h>
#include <utils/gui/cursors/GUICursorSubSys.h>
#include <utils/gui/globjects/GUISaveDialog.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIDialog_GLChosenEditor.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIUserIO.h>
#include <utils/gui/events/GUIEvent_Message.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include <utils/gui/settings/GUISettingsHandler.h>
#include <utils/gui/shortcuts/GUIShortcutsSubSys.h>
#include <utils/gui/windows/GUIDialog_Options.h>
#include <utils/gui/windows/GUIPerspectiveChanger.h>
#include <utils/foxtools/MFXMenuCheckIcon.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/handlers/TemplateHandler.h>
#include <xercesc/parsers/SAXParser.hpp>
#include <netedit/templates.h>

#include "GNEApplicationWindow.h"
#include "GNEEvent_NetworkLoaded.h"
#include "GNELoadThread.h"
#include "GNENet.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNEViewParent.h"

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

// ===========================================================================
// FOX-declarations
// ===========================================================================

FXDEFMAP(GNEApplicationWindow) GNEApplicationWindowMap[] = {
    // quit calls
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_Q_CLOSE,    GNEApplicationWindow::onCmdQuit),
    FXMAPFUNC(SEL_SIGNAL,   MID_HOTKEY_CTRL_Q_CLOSE,    GNEApplicationWindow::onCmdQuit),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_F4_CLOSE,    GNEApplicationWindow::onCmdQuit),
    FXMAPFUNC(SEL_CLOSE,    MID_WINDOW,                 GNEApplicationWindow::onCmdQuit),
    // toolbar file
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_N_OPENNETWORK_NEWNETWORK,   GNEApplicationWindow::onCmdNewNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_N_OPENNETWORK_NEWNETWORK,   GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_SHIFT_N_NEWWINDOW,          GNEApplicationWindow::onCmdNewWindow),
    FXMAPFUNC(SEL_COMMAND,  MID_RECENTFILE,                             GNEApplicationWindow::onCmdOpenRecent),
    FXMAPFUNC(SEL_UPDATE,   MID_RECENTFILE,                             GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_R_RELOAD,                   GNEApplicationWindow::onCmdSmartReload),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_R_RELOAD,                   GNEApplicationWindow::onUpdSmartReload),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_RELOADNETWORK,          GNEApplicationWindow::onCmdReloadNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_RELOADNETWORK,          GNEApplicationWindow::onUpdReloadNetwork),
    // network
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_O_OPENSIMULATION_OPENNETWORK,   GNEApplicationWindow::onCmdOpenNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_O_OPENSIMULATION_OPENNETWORK,   GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_SHIFT_O_OPENNETCONVERTFILE,     GNEApplicationWindow::onCmdOpenNetconvertConfig),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_SHIFT_O_OPENNETCONVERTFILE,     GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_OPENFOREIGN,                GNEApplicationWindow::onCmdOpenForeign),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_OPENFOREIGN,                GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_S_STOPSIMULATION_SAVENETWORK,   GNEApplicationWindow::onCmdSaveNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_S_STOPSIMULATION_SAVENETWORK,   GNEApplicationWindow::onUpdSaveNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVENETWORK_AS,             GNEApplicationWindow::onCmdSaveNetworkAs),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_SAVENETWORK_AS,             GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_L_SAVEASPLAINXML,               GNEApplicationWindow::onCmdSavePlainXMLAs),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_L_SAVEASPLAINXML,               GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SAVEJOINEDJUNCTIONS,                    GNEApplicationWindow::onCmdSaveJoinedJunctionsAs),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_SAVEJOINEDJUNCTIONS,                    GNEApplicationWindow::onUpdNeedsNetwork),
    // NETEDITConfig
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_E_EDITSELECTION_LOADNETEDITCONFIG,  GNEApplicationWindow::onCmdOpenNETEDITConfig),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_RELOAD_NETEDITCONFIG,           GNEApplicationWindow::onCmdReloadNETEDITConfig),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_RELOAD_NETEDITCONFIG,           GNEApplicationWindow::onUpdReloadNETEDITConfig),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_SHIFT_E_SAVENETEDITCONFIG,          GNEApplicationWindow::onCmdSaveNETEDITConfig),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_SHIFT_E_SAVENETEDITCONFIG,          GNEApplicationWindow::onUpdSaveNETEDITConfig),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVENETEDITCONFIG_AS,           GNEApplicationWindow::onCmdSaveNETEDITConfigAs),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_SAVENETEDITCONFIG_AS,           GNEApplicationWindow::onUpdNeedsNetwork),
    // SUMOConfig
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_M_OPENSUMOCONFIG,       GNEApplicationWindow::onCmdOpenSUMOConfig),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_RELOAD_SUMOCONFIG,  GNEApplicationWindow::onCmdReloadSUMOConfig),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_RELOAD_SUMOCONFIG,  GNEApplicationWindow::onUpdReloadSUMOConfig),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_SHIFT_S_SAVESUMOCONFIG, GNEApplicationWindow::onCmdSaveSUMOConfig),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_SHIFT_S_SAVESUMOCONFIG, GNEApplicationWindow::onUpdSaveSUMOConfig),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVESUMOCONFIG_AS,  GNEApplicationWindow::onCmdSaveSUMOConfigAs),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_SAVESUMOCONFIG_AS,  GNEApplicationWindow::onUpdNeedsNetwork),
    // TLS
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_K_OPENTLSPROGRAMS,      GNEApplicationWindow::onCmdOpenTLSPrograms),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_K_OPENTLSPROGRAMS,      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_RELOAD_TLSPROGRAMS, GNEApplicationWindow::onCmdReloadTLSPrograms),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_RELOAD_TLSPROGRAMS, GNEApplicationWindow::onUpdReloadTLSPrograms),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_SHIFT_K_SAVETLS,        GNEApplicationWindow::onCmdSaveTLSPrograms),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_SHIFT_K_SAVETLS,        GNEApplicationWindow::onUpdSaveTLSPrograms),
    // edge types
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_H_APPSETTINGS_OPENEDGETYPES,    GNEApplicationWindow::onCmdOpenEdgeTypes),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_H_APPSETTINGS_OPENEDGETYPES,    GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_RELOAD_EDGETYPES,           GNEApplicationWindow::onCmdReloadEdgeTypes),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_RELOAD_EDGETYPES,           GNEApplicationWindow::onUpdReloadEdgeTypes),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_SHIFT_H_SAVEEDGETYPES,          GNEApplicationWindow::onCmdSaveEdgeTypes),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_SHIFT_H_SAVEEDGETYPES,          GNEApplicationWindow::onUpdSaveEdgeTypes),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVEEDGETYPES_AS,           GNEApplicationWindow::onCmdSaveEdgeTypesAs),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_SAVEEDGETYPES_AS,           GNEApplicationWindow::onUpdSaveEdgeTypesAs),
    // additionals
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_A_STARTSIMULATION_OPENADDITIONALS,  GNEApplicationWindow::onCmdOpenAdditionals),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_A_STARTSIMULATION_OPENADDITIONALS,  GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_RELOAD_ADDITIONALS,             GNEApplicationWindow::onCmdReloadAdditionals),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_RELOAD_ADDITIONALS,             GNEApplicationWindow::onUpdReloadAdditionals),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_SHIFT_A_SAVEADDITIONALS,            GNEApplicationWindow::onCmdSaveAdditionals),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_SHIFT_A_SAVEADDITIONALS,            GNEApplicationWindow::onUpdSaveAdditionals),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVEADDITIONALS_AS,             GNEApplicationWindow::onCmdSaveAdditionalsAs),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_SAVEADDITIONALS_AS,             GNEApplicationWindow::onUpdSaveAdditionalsAs),
    // demand elements
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_D_SINGLESIMULATIONSTEP_OPENDEMANDELEMENTS,  GNEApplicationWindow::onCmdOpenDemandElements),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_D_SINGLESIMULATIONSTEP_OPENDEMANDELEMENTS,  GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_RELOAD_DEMANDELEMENTS,                  GNEApplicationWindow::onCmdReloadDemandElements),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_RELOAD_DEMANDELEMENTS,                  GNEApplicationWindow::onUpdReloadDemandElements),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_SHIFT_D_SAVEDEMANDELEMENTS,                 GNEApplicationWindow::onCmdSaveDemandElements),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_SHIFT_D_SAVEDEMANDELEMENTS,                 GNEApplicationWindow::onUpdSaveDemandElements),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVEDEMAND_AS,                          GNEApplicationWindow::onCmdSaveDemandElementsAs),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_SAVEDEMAND_AS,                          GNEApplicationWindow::onUpdSaveDemandElementsAs),
    // data elements
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_B_EDITBREAKPOINT_OPENDATAELEMENTS,  GNEApplicationWindow::onCmdOpenDataElements),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_B_EDITBREAKPOINT_OPENDATAELEMENTS,  GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_RELOAD_DATAELEMENTS,            GNEApplicationWindow::onCmdReloadDataElements),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_RELOAD_DATAELEMENTS,            GNEApplicationWindow::onUpdReloadDataElements),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_SHIFT_B_SAVEDATAELEMENTS,           GNEApplicationWindow::onCmdSaveDataElements),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_SHIFT_B_SAVEDATAELEMENTS,           GNEApplicationWindow::onUpdSaveDataElements),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVEDATA_AS,                    GNEApplicationWindow::onCmdSaveDataElementsAs),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_SAVEDATA_AS,                    GNEApplicationWindow::onUpdSaveDataElementsAs),
    // meanDatas
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_OPENMEANDATAS,      GNEApplicationWindow::onCmdOpenMeanDatas),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_OPENMEANDATAS,      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_RELOAD_MEANDATAS,   GNEApplicationWindow::onCmdReloadMeanDatas),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_RELOAD_MEANDATAS,   GNEApplicationWindow::onUpdReloadMeanDatas),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_SHIFT_M_SAVEMEANDATAS,  GNEApplicationWindow::onCmdSaveMeanDatas),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_SHIFT_M_SAVEMEANDATAS,  GNEApplicationWindow::onUpdSaveMeanDatas),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVEMEANDATAS_AS,   GNEApplicationWindow::onCmdSaveMeanDatasAs),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_SAVEMEANDATAS_AS,   GNEApplicationWindow::onUpdSaveMeanDatasAs),
    // other
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVETLSPROGRAMS_AS, GNEApplicationWindow::onCmdSaveTLSProgramsAs),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_SAVETLSPROGRAMS_AS, GNEApplicationWindow::onUpdSaveTLSPrograms),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_W_CLOSESIMULATION,      GNEApplicationWindow::onCmdClose),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_W_CLOSESIMULATION,      GNEApplicationWindow::onUpdNeedsNetwork),
    // Toolbar supermode
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F2_SUPERMODE_NETWORK,    GNEApplicationWindow::onCmdSetSuperMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F3_SUPERMODE_DEMAND,     GNEApplicationWindow::onCmdSetSuperMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F4_SUPERMODE_DATA,       GNEApplicationWindow::onCmdSetSuperMode),
    // Toolbar modes
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_A_MODE_STARTSIMULATION_ADDITIONALSTOP,   GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_C_MODE_CONNECT_PERSONPLAN,               GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_D_MODE_SINGLESIMULATIONSTEP_DELETE,      GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_E_MODE_EDGE_EDGEDATA,                    GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_G_MODE_CONTAINER,                        GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_H_MODE_PROHIBITION_CONTAINERPLAN,        GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_I_MODE_INSPECT,                          GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_M_MODE_MOVE_MEANDATA,                    GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_P_MODE_POLYGON_PERSON,                   GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_R_MODE_CROSSING_ROUTE_EDGERELDATA,       GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_S_MODE_STOPSIMULATION_SELECT,            GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_T_MODE_TLS_TYPE,                         GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_V_MODE_VEHICLE,                          GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_W_MODE_WIRE,                             GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_Z_MODE_TAZ_TAZREL,                       GNEApplicationWindow::onCmdSetMode),
    // Toolbar edit
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_Z_UNDO,                     GNEApplicationWindow::onCmdUndo),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_Z_UNDO,                     GNEApplicationWindow::onUpdUndo),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_Y_REDO,                     GNEApplicationWindow::onCmdRedo),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_Y_REDO,                     GNEApplicationWindow::onUpdRedo),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_UNDOLISTDIALOG,                     GNEApplicationWindow::onCmdOpenUndoListDialog),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_UNDOLISTDIALOG,                     GNEApplicationWindow::onUpdOpenUndoListDialog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBAREDIT_COMPUTEPATHMANAGER,     GNEApplicationWindow::onCmdComputePathManager),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBAREDIT_COMPUTEPATHMANAGER,     GNEApplicationWindow::onUpdComputePathManager),
    // Network view options
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_TOGGLEGRID,               GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_NETWORKVIEWOPTIONS_TOGGLEGRID,               GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE,  GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_NETWORKVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE,  GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_DRAWSPREADVEHICLES,       GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_NETWORKVIEWOPTIONS_DRAWSPREADVEHICLES,       GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_SHOWDEMANDELEMENTS,       GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_NETWORKVIEWOPTIONS_SHOWDEMANDELEMENTS,       GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_SELECTEDGES,              GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_NETWORKVIEWOPTIONS_SELECTEDGES,              GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_SHOWCONNECTIONS,          GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_NETWORKVIEWOPTIONS_SHOWCONNECTIONS,          GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_HIDECONNECTIONS,          GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_NETWORKVIEWOPTIONS_HIDECONNECTIONS,          GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_SHOWSUBADDITIONALS,       GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_NETWORKVIEWOPTIONS_SHOWSUBADDITIONALS,       GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_SHOWTAZELEMENTS,          GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_NETWORKVIEWOPTIONS_SHOWTAZELEMENTS,          GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_EXTENDSELECTION,          GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_NETWORKVIEWOPTIONS_EXTENDSELECTION,          GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_CHANGEALLPHASES,          GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_NETWORKVIEWOPTIONS_CHANGEALLPHASES,          GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_ASKFORMERGE,              GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_NETWORKVIEWOPTIONS_ASKFORMERGE,              GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_SHOWBUBBLES,              GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_NETWORKVIEWOPTIONS_SHOWBUBBLES,              GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_MOVEELEVATION,            GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_NETWORKVIEWOPTIONS_MOVEELEVATION,            GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_CHAINEDGES,               GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_NETWORKVIEWOPTIONS_CHAINEDGES,               GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_AUTOOPPOSITEEDGES,        GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_NETWORKVIEWOPTIONS_AUTOOPPOSITEEDGES,        GNEApplicationWindow::onUpdToggleViewOption),
    // Demand view options
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_SHOWGRID,                  GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_DEMANDVIEWOPTIONS_SHOWGRID,                  GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE,   GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_DEMANDVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE,   GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_DRAWSPREADVEHICLES,        GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_DEMANDVIEWOPTIONS_DRAWSPREADVEHICLES,        GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_HIDENONINSPECTED,          GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_DEMANDVIEWOPTIONS_HIDENONINSPECTED,          GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_HIDESHAPES,                GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_DEMANDVIEWOPTIONS_HIDESHAPES,                GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_SHOWTRIPS,                 GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_DEMANDVIEWOPTIONS_SHOWTRIPS,                 GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_SHOWALLPERSONPLANS,        GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_DEMANDVIEWOPTIONS_SHOWALLPERSONPLANS,        GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_LOCKPERSON,                GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_DEMANDVIEWOPTIONS_LOCKPERSON,                GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_SHOWALLCONTAINERPLANS,     GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_DEMANDVIEWOPTIONS_SHOWALLCONTAINERPLANS,     GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_LOCKCONTAINER,             GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_DEMANDVIEWOPTIONS_LOCKCONTAINER,             GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_SHOWOVERLAPPEDROUTES,      GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_DEMANDVIEWOPTIONS_SHOWOVERLAPPEDROUTES,      GNEApplicationWindow::onUpdToggleViewOption),
    // Data view options
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DATAVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE,     GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_DATAVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE,     GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DATAVIEWOPTIONS_SHOWADDITIONALS,             GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_DATAVIEWOPTIONS_SHOWADDITIONALS,             GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DATAVIEWOPTIONS_SHOWSHAPES,                  GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_DATAVIEWOPTIONS_SHOWSHAPES,                  GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DATAVIEWOPTIONS_SHOWDEMANDELEMENTS,          GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_DATAVIEWOPTIONS_SHOWDEMANDELEMENTS,          GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DATAVIEWOPTIONS_TAZRELDRAWING,               GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_DATAVIEWOPTIONS_TAZRELDRAWING,               GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DATAVIEWOPTIONS_TAZDRAWFILL,                 GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_DATAVIEWOPTIONS_TAZDRAWFILL,                 GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DATAVIEWOPTIONS_TAZRELONLYFROM,              GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_DATAVIEWOPTIONS_TAZRELONLYFROM,              GNEApplicationWindow::onUpdToggleViewOption),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DATAVIEWOPTIONS_TAZRELONLYTO,                GNEApplicationWindow::onCmdToggleViewOption),
    FXMAPFUNC(SEL_UPDATE,  MID_GNE_DATAVIEWOPTIONS_TAZRELONLYTO,                GNEApplicationWindow::onUpdToggleViewOption),
    // view
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F9_EDIT_VIEWSCHEME,              GNEApplicationWindow::onCmdEditViewScheme),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_F9_EDIT_VIEWSCHEME,              GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_I_EDITVIEWPORT,             GNEApplicationWindow::onCmdEditViewport),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_I_EDITVIEWPORT,             GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_G_GAMINGMODE_TOGGLEGRID,    GNEApplicationWindow::onCmdToggleGrid),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_G_GAMINGMODE_TOGGLEGRID,    GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_J_TOGGLEDRAWJUNCTIONSHAPE,  GNEApplicationWindow::onCmdToggleDrawJunctionShape),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_J_TOGGLEDRAWJUNCTIONSHAPE,  GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F11_FRONTELEMENT,                GNEApplicationWindow::onCmdSetFrontElement),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_F11_FRONTELEMENT,                GNEApplicationWindow::onUpdNeedsFrontElement),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBAREDIT_LOADADDITIONALS,        GNEApplicationWindow::onCmdLoadAdditionalsInSUMOGUI),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBAREDIT_LOADADDITIONALS,        GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBAREDIT_LOADDEMAND,             GNEApplicationWindow::onCmdLoadDemandInSUMOGUI),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBAREDIT_LOADDEMAND,             GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_T_OPENNETEDIT_OPENSUMO,     GNEApplicationWindow::onCmdOpenSUMOGUI),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_T_OPENNETEDIT_OPENSUMO,     GNEApplicationWindow::onUpdNeedsNetworkElement),
    /* Prepared for #6042
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_X_CUT,      GNEApplicationWindow::onCmdCut),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_C_COPY,     GNEApplicationWindow::onCmdCopy),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_V_PASTE,    GNEApplicationWindow::onCmdPaste),
    */
    // toolbar lock
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_LOCK_ELEMENT,           GNEApplicationWindow::onCmdLockElements),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_LOCK_ALLELEMENTS,       GNEApplicationWindow::onCmdLockAllElements),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_UNLOCK_ALLELEMENTS,     GNEApplicationWindow::onCmdUnlockAllElements),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_LOCK_SELECTEDELEMENTS,  GNEApplicationWindow::onCmdLockSelectElements),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_LOCK_MENUTITLE,         GNEApplicationWindow::onUpdLockMenuTitle),
    // Toolbar processing
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F5_COMPUTE_NETWORK_DEMAND,                   GNEApplicationWindow::onCmdProcessButton),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_F5_COMPUTE_NETWORK_DEMAND,                   GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_F5_COMPUTEJUNCTIONS_VOLATILE,          GNEApplicationWindow::onCmdProcessButton),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_SHIFT_F5_COMPUTEJUNCTIONS_VOLATILE,          GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F6_CLEAN_SOLITARYJUNCTIONS_UNUSEDROUTES,     GNEApplicationWindow::onCmdProcessButton),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_F6_CLEAN_SOLITARYJUNCTIONS_UNUSEDROUTES,     GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F7_JOIN_SELECTEDJUNCTIONS_ROUTES,            GNEApplicationWindow::onCmdProcessButton),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_F7_JOIN_SELECTEDJUNCTIONS_ROUTES,            GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_F7_ADJUST_PERSON_PLANS,                GNEApplicationWindow::onCmdProcessButton),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_SHIFT_F7_ADJUST_PERSON_PLANS,                GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F8_CLEANINVALID_CROSSINGS_DEMANDELEMENTS,    GNEApplicationWindow::onCmdProcessButton),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_F8_CLEANINVALID_CROSSINGS_DEMANDELEMENTS,    GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOGGLE_COMPUTE_NETWORK_DATA,                    GNEApplicationWindow::onCmdToggleComputeNetworkData),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_F10_SUMOOPTIONSMENU,                   GNEApplicationWindow::onCmdOpenSUMOOptionsDialog),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_SHIFT_F10_SUMOOPTIONSMENU,                   GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F10_OPTIONSMENU,                             GNEApplicationWindow::onCmdOpenOptionsDialog),
    // Toolbar locate
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_J_LOCATEJUNCTION,      GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_SHIFT_J_LOCATEJUNCTION,      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_E_LOCATEEDGE,          GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_SHIFT_E_LOCATEEDGE,          GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_W_LOCATEWALKINGAREA,   GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_SHIFT_W_LOCATEWALKINGAREA,   GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_V_LOCATEVEHICLE,       GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_SHIFT_V_LOCATEVEHICLE,       GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_P_LOCATEPERSON,        GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_SHIFT_P_LOCATEPERSON,        GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_R_LOCATEROUTE,         GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_SHIFT_R_LOCATEROUTE,         GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_S_LOCATESTOP,          GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_SHIFT_S_LOCATESTOP,          GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_T_LOCATETLS,           GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_SHIFT_T_LOCATETLS,           GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_A_LOCATEADDITIONAL,    GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_SHIFT_A_LOCATEADDITIONAL,    GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_O_LOCATEPOI,           GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_SHIFT_O_LOCATEPOI,           GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_L_LOCATEPOLY,          GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_SHIFT_L_LOCATEPOLY,          GNEApplicationWindow::onUpdNeedsNetwork),
    // toolbar tools
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARTOOLS_NETDIFF,   GNEApplicationWindow::onCmdToolNetDiff),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARTOOLS_NETDIFF,   GNEApplicationWindow::onUpdToolNetDiff),
    // toolbar windows
    FXMAPFUNC(SEL_COMMAND,  MID_CLEARMESSAGEWINDOW,     GNEApplicationWindow::onCmdClearMsgWindow),
    // toolbar help
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F1_ONLINEDOCUMENTATION,  GNEApplicationWindow::onCmdHelp),
    FXMAPFUNC(SEL_COMMAND,  MID_CHANGELOG,                      GNEApplicationWindow::onCmdChangelog),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEYS,                        GNEApplicationWindow::onCmdHotkeys),
    FXMAPFUNC(SEL_COMMAND,  MID_TUTORIAL,                       GNEApplicationWindow::onCmdTutorial),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F12_ABOUT,               GNEApplicationWindow::onCmdAbout),
    // alt + <number>
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_0_TOGGLEEDITOPTION,  GNEApplicationWindow::onCmdToggleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_0_TOGGLEEDITOPTION,  GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_1_TOGGLEEDITOPTION,  GNEApplicationWindow::onCmdToggleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_1_TOGGLEEDITOPTION,  GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_2_TOGGLEEDITOPTION,  GNEApplicationWindow::onCmdToggleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_2_TOGGLEEDITOPTION,  GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_3_TOGGLEEDITOPTION,  GNEApplicationWindow::onCmdToggleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_3_TOGGLEEDITOPTION,  GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_4_TOGGLEEDITOPTION,  GNEApplicationWindow::onCmdToggleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_4_TOGGLEEDITOPTION,  GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_5_TOGGLEEDITOPTION,  GNEApplicationWindow::onCmdToggleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_5_TOGGLEEDITOPTION,  GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_6_TOGGLEEDITOPTION,  GNEApplicationWindow::onCmdToggleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_6_TOGGLEEDITOPTION,  GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_7_TOGGLEEDITOPTION,  GNEApplicationWindow::onCmdToggleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_7_TOGGLEEDITOPTION,  GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_8_TOGGLEEDITOPTION,  GNEApplicationWindow::onCmdToggleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_8_TOGGLEEDITOPTION,  GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_9_TOGGLEEDITOPTION,  GNEApplicationWindow::onCmdToggleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_9_TOGGLEEDITOPTION,  GNEApplicationWindow::onUpdNeedsNetwork),
    // key events
    FXMAPFUNC(SEL_KEYPRESS,     0,                      GNEApplicationWindow::onKeyPress),
    FXMAPFUNC(SEL_KEYRELEASE,   0,                      GNEApplicationWindow::onKeyRelease),
    FXMAPFUNC(SEL_COMMAND,      MID_HOTKEY_ESC,         GNEApplicationWindow::onCmdAbort),
    FXMAPFUNC(SEL_COMMAND,      MID_HOTKEY_DEL,         GNEApplicationWindow::onCmdDel),
    FXMAPFUNC(SEL_COMMAND,      MID_HOTKEY_ENTER,       GNEApplicationWindow::onCmdEnter),
    FXMAPFUNC(SEL_COMMAND,      MID_HOTKEY_BACKSPACE,   GNEApplicationWindow::onCmdBackspace),
    // threads events
    FXMAPFUNC(FXEX::SEL_THREAD_EVENT,   ID_LOADTHREAD_EVENT,    GNEApplicationWindow::onLoadThreadEvent),
    FXMAPFUNC(FXEX::SEL_THREAD,         ID_LOADTHREAD_EVENT,    GNEApplicationWindow::onLoadThreadEvent),
    // Edge template functions
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_F1_TEMPLATE_SET,       GNEApplicationWindow::onCmdSetTemplate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_F2_TEMPLATE_COPY,      GNEApplicationWindow::onCmdCopyTemplate),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_SHIFT_F3_TEMPLATE_CLEAR,     GNEApplicationWindow::onCmdClearTemplate),
    // Other
    FXMAPFUNC(SEL_CLIPBOARD_REQUEST,    0,                                                  GNEApplicationWindow::onClipboardRequest),
    FXMAPFUNC(SEL_COMMAND,              MID_HOTKEY_CTRL_SHIFT_W_FORCESAVENETEWORK,          GNEApplicationWindow::onCmdForceSaveNetwork),
    FXMAPFUNC(SEL_COMMAND,              MID_HOTKEY_CTRL_SHIFT_X_FORCESAVEADDITIONALS,       GNEApplicationWindow::onCmdForceSaveAdditionals),
    FXMAPFUNC(SEL_COMMAND,              MID_HOTKEY_CTRL_SHIFT_Y_FORCESAVEDEMANDELEMENTS,    GNEApplicationWindow::onCmdForceSaveDemandElements),
    FXMAPFUNC(SEL_COMMAND,              MID_HOTKEY_CTRL_SHIFT_Z_FORCESAVEDATAELEMENTS,      GNEApplicationWindow::onCmdForceSaveDataElements),
    FXMAPFUNC(SEL_COMMAND,              MID_HOTKEY_SHIFT_F12_FOCUSUPPERELEMENT,             GNEApplicationWindow::onCmdFocusFrame),
    FXMAPFUNC(SEL_UPDATE,               MID_GNE_MODESMENUTITLE,                             GNEApplicationWindow::onUpdRequireViewNet),
    FXMAPFUNC(SEL_UPDATE,               MID_GNE_RECOMPUTINGNEEDED,                          GNEApplicationWindow::onUpdRequireRecomputing),
};

// Object implementation
FXIMPLEMENT(GNEApplicationWindow, FXMainWindow, GNEApplicationWindowMap, ARRAYNUMBER(GNEApplicationWindowMap))


// ===========================================================================
// GNEApplicationWindow method definitions
// ===========================================================================

GNEApplicationWindow::GNEApplicationWindow(FXApp* a, const std::string& configPattern) :
    GUIMainWindow(a),
    myUndoList(new GNEUndoList(this)),
    myConfigPattern(configPattern),
    myToolbarsGrip(this),
    myMenuBarFile(this),
    myFileMenuCommands(this),
    myModesMenuCommands(this),
    myEditMenuCommands(this),
    myLockMenuCommands(this),
    myProcessingMenuCommands(this),
    myLocateMenuCommands(this),
    myToolsMenuCommands(this),
    myWindowsMenuCommands(this),
    mySupermodeCommands(this),
    myTitlePrefix("NETEDIT " VERSION_STRING) {
    // init icons
    GUIIconSubSys::initIcons(a);
    // init Textures
    GUITextureSubSys::initTextures(a);
    // init cursors
    GUICursorSubSys::initCursors(a);
    // create undoList dialog (after initCursors)
    myUndoListDialog = new GNEUndoListDialog(this);
    a->setTooltipTime(1000000000);
    a->setTooltipPause(1000000000);
    // set SUMO Options descriptions
    mySUMOOptions.setApplicationDescription(TL("A microscopic, multi-modal traffic simulation."));
    mySUMOOptions.setApplicationName("sumo", "Eclipse SUMO sumo Version " VERSION_STRING);
    // parse sumo options
    TemplateHandler::parseTemplate(mySUMOOptions, sumoTemplate);
}

void
GNEApplicationWindow::dependentBuild() {
    // do this not twice
    if (myHadDependentBuild) {
        WRITE_ERROR(TL("DEBUG: GNEApplicationWindow::dependentBuild called twice"));
        return;
    }
    myHadDependentBuild = true;
    if (gLanguage == "C") {
        FXRegistry reg("SUMO GUI", "sumo-gui");
        reg.read();
        gLanguage = reg.readStringEntry("gui", "language", "C");
        MsgHandler::setupI18n(gLanguage);
    }
    setTarget(this);
    setSelector(MID_WINDOW);
    // build toolbar menu
    getToolbarsGrip().buildMenuToolbarsGrip();
    // build the thread - io
    myLoadThreadEvent.setTarget(this);
    myLoadThreadEvent.setSelector(ID_LOADTHREAD_EVENT);
    // build the status bar
    myStatusbar = new FXStatusBar(this, GUIDesignStatusBar);
    // build geo coordinates label
    auto requireRecomputingFrame = new FXHorizontalFrame(myStatusbar, GUIDesignHorizontalFrameStatusBar);
    myRequireRecomputingButton = new MFXButtonTooltip(requireRecomputingFrame, myStaticTooltipMenu,
            TL("Recomputing\t\tRecomputing is needed"), nullptr, this, MID_GNE_RECOMPUTINGNEEDED, GUIDesignButtonStatusBarFixed);
    // build geo coordinates label
    myGeoFrame = new FXHorizontalFrame(myStatusbar, GUIDesignHorizontalFrameStatusBar);
    myGeoCoordinate = new FXLabel(myGeoFrame, TL("N/A\t\tOriginal coordinate (before coordinate transformation in netconvert)"), nullptr, GUIDesignLabelStatusBar);
    // build cartesian coordinates label
    myCartesianFrame = new FXHorizontalFrame(myStatusbar, GUIDesignHorizontalFrameStatusBar);
    myCartesianCoordinate = new FXLabel(myCartesianFrame, TL("N/A\t\tNetwork coordinate"), nullptr, GUIDesignLabelStatusBar);
    // build test coordinates label (only if gui-testing is enabled)
    if (OptionsCont::getOptions().getBool("gui-testing")) {
        myTestFrame = new FXHorizontalFrame(myStatusbar, GUIDesignHorizontalFrameStatusBar);
        myTestCoordinate = new FXLabel(myTestFrame, TL("N/A\t\tTest coordinate"), nullptr, GUIDesignLabelStatusBar);
    }
    // make the window a mdi-window
    myMainSplitter = new FXSplitter(this, GUIDesignSplitter | SPLITTER_VERTICAL | SPLITTER_REVERSED);
    myMDIClient = new FXMDIClient(myMainSplitter, GUIDesignSplitterMDI);
    myMDIMenu = new FXMDIMenu(this, myMDIClient);
    // build the message window
    myMessageWindow = new GUIMessageWindow(myMainSplitter, this);
    myMainSplitter->setSplit(1, 65);
    // fill menu and tool bar
    fillMenuBar();
    // build additional threads
    myLoadThread = new GNELoadThread(this, myEvents, myLoadThreadEvent);
    // set the status bar
    myStatusbar->getStatusLine()->setText(TL("Ready."));
    // set the caption
    setTitle(myTitlePrefix);
    // set Netedit ICON
    setIcon(GUIIconSubSys::getIcon(GUIIcon::NETEDIT));
    setMiniIcon(GUIIconSubSys::getIcon(GUIIcon::NETEDIT_MINI));
    // build NETEDIT Accelerators (hotkeys)
    GUIShortcutsSubSys::buildAccelerators(getAccelTable(), this, false);
}


void
GNEApplicationWindow::create() {
    // set windows size and position
    setWindowSizeAndPos();
    // set current folder
    gCurrentFolder = getApp()->reg().readStringEntry("SETTINGS", "basedir", "");
    // Create main window
    FXMainWindow::create();
    // create menu panes
    myFileMenu->create();
    myModesMenu->create();
    myEditMenu->create();
    myFileMenuNETEDITConfig->create();
    myFileMenuSUMOConfig->create();
    myFileMenuTLS->create();
    myFileMenuEdgeTypes->create();
    myFileMenuAdditionals->create();
    myFileMenuDemandElements->create();
    myFileMenuDataElements->create();
    myFileMenuMeanDataElements->create();
    myWindowMenu->create();
    myHelpMenu->create();

    FXint textWidth = getApp()->getNormalFont()->getTextWidth("8", 1) * 22;
    myCartesianFrame->setWidth(textWidth);
    myGeoFrame->setWidth(textWidth);

    // fill online maps
    if (myOnlineMaps.empty()) {
        myOnlineMaps["GeoHack"] = "https://geohack.toolforge.org/geohack.php?params=%lat;%lon_scale:1000";
        myOnlineMaps["GoogleSat"] = "https://www.google.com/maps?ll=%lat,%lon&t=h&z=18";
        myOnlineMaps["OSM"] = "https://www.openstreetmap.org/?mlat=%lat&mlon=%lon&zoom=18&layers=M";
    }

    show(PLACEMENT_DEFAULT);
    if (!OptionsCont::getOptions().isSet("window-size")) {
        if (getApp()->reg().readIntEntry("SETTINGS", "maximized", 0) == 1) {
            maximize();
        }
    }

}


GNEApplicationWindow::~GNEApplicationWindow() {
    closeAllWindows();
    // Close icons
    GUIIconSubSys::close();
    // Close gifs (Textures)
    GUITextureSubSys::close();
    GUICursorSubSys::close();
    // delete visuals
    delete myGLVisual;
    // must delete menus to avoid segfault on removing accelerators
    // (http://www.fox-toolkit.net/faq#TOC-What-happens-when-the-application-s)
    delete myFileMenuNETEDITConfig;
    delete myFileMenuSUMOConfig;
    delete myFileMenuTLS;
    delete myFileMenuEdgeTypes;
    delete myFileMenuAdditionals;
    delete myFileMenuDemandElements;
    delete myFileMenuMeanDataElements;
    delete myFileMenuDataElements;
    delete myFileMenuRecentNetworks;
    delete myFileMenuRecentConfigs;
    delete myFileMenu;
    delete myModesMenu;
    delete myEditMenu;
    delete myLockMenu;
    delete myProcessingMenu;
    delete myLocatorMenu;
    delete myToolsMenu;
    delete myWindowMenu;
    delete myHelpMenu;
    // Delete load thread
    delete myLoadThread;
    // drop all events
    while (!myEvents.empty()) {
        // get the next event
        GUIEvent* e = myEvents.top();
        myEvents.pop();
        delete e;
    }
    // delete undoList and dialog
    delete myUndoList;
    delete myUndoListDialog;
}


long
GNEApplicationWindow::onCmdQuit(FXObject*, FXSelector, void*) {
    if (continueWithUnsavedChanges("quit")) {
        storeWindowSizeAndPos();
        getApp()->reg().writeStringEntry("SETTINGS", "basedir", gCurrentFolder.text());
        if (isMaximized()) {
            getApp()->reg().writeIntEntry("SETTINGS", "maximized", 1);
        } else {
            getApp()->reg().writeIntEntry("SETTINGS", "maximized", 0);
        }
        getApp()->exit(0);
    }
    return 1;
}


long
GNEApplicationWindow::onCmdEditChosen(FXObject*, FXSelector, void*) {
    GUIDialog_GLChosenEditor* chooser =
        new GUIDialog_GLChosenEditor(this, &gSelected);
    chooser->create();
    chooser->show();
    return 1;
}


long
GNEApplicationWindow::onCmdNewNetwork(FXObject*, FXSelector, void*) {
    // check if close current simulation
    if (onCmdClose(0, 0, 0) == 1) {
        // create new network
        createNewNetwork();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdOpenNetconvertConfig(FXObject*, FXSelector, void*) {
    // get netconvert filename
    const auto netconvertFile = GNEApplicationWindowHelper::openNetconvertFileDialog(this);
    // continue depending of netconvertFile
    if (!netconvertFile.empty() && (onCmdClose(0, 0, 0) == 1)) {
        // load configuration
        loadConfiguration(netconvertFile);
    }
    return 1;
}


long
GNEApplicationWindow::onCmdOpenNetwork(FXObject*, FXSelector, void*) {
    // get netconvert filename
    const auto networkFile = GNEApplicationWindowHelper::openNetworkFileDialog(this, false);
    // continue depending of netconvertFile
    if (!networkFile.empty() && (onCmdClose(0, 0, 0) == 1)) {
        // load network
        loadNetwork(networkFile);
    }
    return 0;
}


long
GNEApplicationWindow::onCmdOpenForeign(FXObject*, FXSelector, void*) {
    // get netconvert filename
    const auto OSMFile = GNEApplicationWindowHelper::openOSMFileDialog(this);
    // continue depending of netconvertFile
    if (!OSMFile.empty() && (onCmdClose(0, 0, 0) == 1)) {
        loadOSM(OSMFile);
    }
    return 1;
}


long
GNEApplicationWindow::onCmdOpenNETEDITConfig(FXObject*, FXSelector, void*) {
    auto& neteditOptions = OptionsCont::getOptions();
    // get netconvert filename
    const auto neteditConfigFile = GNEApplicationWindowHelper::openNETEDITConfigFileDialog(this, false);
    // continue depending of netconvertFile
    if (!neteditConfigFile.empty() && (onCmdClose(0, 0, 0) == 1)) {
        // reset netedit options
        myLoadThread->fillOptions(neteditOptions);
        myLoadThread->setDefaultOptions(neteditOptions);
        // set netedit configuration file to load
        neteditOptions.resetWritable();
        neteditOptions.set("configuration-file", neteditConfigFile);
        // run load thread
        myLoadThread->loadNetworkOrConfig();
        // update view
        update();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdOpenSUMOConfig(FXObject*, FXSelector, void*) {
    auto& neteditOptions = OptionsCont::getOptions();
    // get netconvert filename
    const auto sumoConfigFile = GNEApplicationWindowHelper::openSUMOConfigFileDialog(this, false);
    // continue depending of netconvertFile
    if (!sumoConfigFile.empty() && (onCmdClose(0, 0, 0) == 1)) {
        // reset options
        myLoadThread->fillOptions(neteditOptions);
        myLoadThread->setDefaultOptions(neteditOptions);
        // set sumo configuration file to load
        neteditOptions.resetWritable();
        neteditOptions.set("sumocfg-file", sumoConfigFile);
        // run load thread
        myLoadThread->loadNetworkOrConfig();
        // update view
        update();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdReloadNETEDITConfig(FXObject*, FXSelector, void*) {
    // check if close current simulation
    if (onCmdClose(0, 0, 0) == 1) {
        auto& neteditOptions = OptionsCont::getOptions();
        // get existent configuration file
        const auto neteditConfigFile = neteditOptions.getString("configuration-file");
        // reset options
        myLoadThread->fillOptions(neteditOptions);
        myLoadThread->setDefaultOptions(neteditOptions);
        // set configuration file to load
        neteditOptions.resetWritable();
        neteditOptions.set("configuration-file", neteditConfigFile);
        // run load thread
        myLoadThread->loadNetworkOrConfig();
        // update view
        update();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdReloadSUMOConfig(FXObject*, FXSelector, void*) {
    auto& neteditOptions = OptionsCont::getOptions();
    // check if close current simulation
    if (onCmdClose(0, 0, 0) == 1) {
        const auto sumoConfigFile = neteditOptions.getString("sumocfg-file");
        // reset options
        myLoadThread->fillOptions(neteditOptions);
        myLoadThread->setDefaultOptions(neteditOptions);
        // set configuration file to load
        neteditOptions.resetWritable();
        neteditOptions.set("sumocfg-file", sumoConfigFile);
        // run load thread
        myLoadThread->loadNetworkOrConfig();
        // update view
        update();
    }
    return 1;
}


long
GNEApplicationWindow::onUpdReloadNETEDITConfig(FXObject* sender, FXSelector, void*) {
    // check if file exist
    if (myViewNet && !OptionsCont::getOptions().getString("configuration-file").empty()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
}


long
GNEApplicationWindow::onUpdReloadSUMOConfig(FXObject* sender, FXSelector, void*) {
    // check if file exist
    if (myViewNet && !OptionsCont::getOptions().getString("sumocfg-file").empty()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
}


long
GNEApplicationWindow::onCmdOpenTLSPrograms(FXObject*, FXSelector, void*) {
    auto& neteditOptions = OptionsCont::getOptions();
    // get netconvert filename
    const auto TLSFile = GNEApplicationWindowHelper::openTLSFileDialog(this, false);
    // continue depending of netconvertFile
    if (!TLSFile.empty()) {
        // set file to load
        neteditOptions.resetWritable();
        neteditOptions.set("tls-file", TLSFile);
        // Run parser
        myUndoList->begin(Supermode::NETWORK, GUIIcon::MODETLS, "loading TLS Programs from '" + TLSFile + "'");
        myNet->computeNetwork(this);
        if (myNet->getViewNet()->getViewParent()->getTLSEditorFrame()->parseTLSPrograms(TLSFile) == false) {
            // Abort undo/redo
            myUndoList->abortAllChangeGroups();
        } else {
            // commit undo/redo operation
            myUndoList->end();
            update();
        }
    } else {
        // write debug information
        WRITE_DEBUG("Cancel TLSProgram dialog");
    }
    return 1;
}


long
GNEApplicationWindow::onCmdReloadTLSPrograms(FXObject*, FXSelector, void*) {
    // get option container
    auto& neteditOptions = OptionsCont::getOptions();
    // Run parser
    myUndoList->begin(Supermode::NETWORK, GUIIcon::MODETLS, "loading TLS Programs from '" + neteditOptions.getString("tls-file") + "'");
    myNet->computeNetwork(this);
    if (myNet->getViewNet()->getViewParent()->getTLSEditorFrame()->parseTLSPrograms(neteditOptions.getString("tls-file")) == false) {
        // Abort undo/redo
        myUndoList->abortAllChangeGroups();
    } else {
        // commit undo/redo operation
        myUndoList->end();
        update();
    }
    return 1;
}


long
GNEApplicationWindow::onUpdReloadTLSPrograms(FXObject* sender, FXSelector, void*) {
    // check if file exist
    if (myViewNet && OptionsCont::getOptions().getString("tls-file").empty()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    }
}


long
GNEApplicationWindow::onCmdOpenEdgeTypes(FXObject*, FXSelector, void*) {
    auto& neteditOptions = OptionsCont::getOptions();
    // get netconvert filename
    const auto edgeTypeFile = GNEApplicationWindowHelper::openEdgeTypeFileDialog(this, false);
    // continue depending of netconvertFile
    if (!edgeTypeFile.empty()) {
        // set file to load
        neteditOptions.resetWritable();
        neteditOptions.set("edgetypes-file", edgeTypeFile);
        // declare type container
        NBTypeCont typeContainerAux;
        // declare type handler
        NIXMLTypesHandler handler(typeContainerAux);
        // load edge types
        NITypeLoader::load(handler, {edgeTypeFile}, "types");
        // write information
        WRITE_MESSAGE(TL("Loaded edge types from '") + toString(typeContainerAux.size()) + "'");
        // now create GNETypes based on typeContainerAux
        myViewNet->getUndoList()->begin(Supermode::NETWORK, GUIIcon::EDGE, "load edgeTypes");
        // iterate over typeContainerAux
        for (const auto& auxEdgeType : typeContainerAux) {
            // create new edge type
            GNEEdgeType* edgeType = new GNEEdgeType(myNet, auxEdgeType.first, auxEdgeType.second);
            // add lane types
            for (const auto& laneType : auxEdgeType.second->laneTypeDefinitions) {
                edgeType->addLaneType(new GNELaneType(edgeType, laneType));
            }
            // add it using undoList
            myViewNet->getUndoList()->add(new GNEChange_EdgeType(edgeType, true), true);

        }
        // end undo list
        myViewNet->getUndoList()->end();
        // refresh edge type selector
        myViewNet->getViewParent()->getCreateEdgeFrame()->getEdgeTypeSelector()->refreshEdgeTypeSelector();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdReloadEdgeTypes(FXObject*, FXSelector, void*) {
    // declare type container
    NBTypeCont typeContainerAux;
    // declare type handler
    NIXMLTypesHandler handler(typeContainerAux);
    // load edge types
    NITypeLoader::load(handler, {OptionsCont::getOptions().getString("edgetypes-file")}, "types");
    // write information
    WRITE_MESSAGE(TL("Reloaded edge types from '") + toString(typeContainerAux.size()) + ".");
    // now create GNETypes based on typeContainerAux
    myViewNet->getUndoList()->begin(Supermode::NETWORK, GUIIcon::EDGE, "load edgeTypes");
    // iterate over typeContainerAux
    for (const auto& auxEdgeType : typeContainerAux) {
        // create new edge type
        GNEEdgeType* edgeType = new GNEEdgeType(myNet, auxEdgeType.first, auxEdgeType.second);
        // add lane types
        for (const auto& laneType : auxEdgeType.second->laneTypeDefinitions) {
            edgeType->addLaneType(new GNELaneType(edgeType, laneType));
        }
        // add it using undoList
        myViewNet->getUndoList()->add(new GNEChange_EdgeType(edgeType, true), true);

    }
    // end undo list
    myViewNet->getUndoList()->end();
    // refresh edge type selector
    myViewNet->getViewParent()->getCreateEdgeFrame()->getEdgeTypeSelector()->refreshEdgeTypeSelector();
    return 0;
}


long
GNEApplicationWindow::onUpdReloadEdgeTypes(FXObject* sender, FXSelector, void*) {
    // check if file exist
    if (myViewNet && OptionsCont::getOptions().getString("edgetypes-file").empty()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    }
}


long
GNEApplicationWindow::onCmdSmartReload(FXObject*, FXSelector, void*) {
    auto& neteditOptions = OptionsCont::getOptions();
    // check if close current file
    if (onCmdClose(0, 0, 0) == 1) {
        // store size, position and viewport
        storeWindowSizeAndPos();
        gSchemeStorage.saveViewport(0, 0, -1, 0); // recenter view
        // set flag
        myAmLoading = true;
        // get files
        const auto neteditConfig = neteditOptions.getString("configuration-file");
        const auto sumoConfig = neteditOptions.getString("sumocfg-file");
        const auto networkFile = neteditOptions.getString("net-file");
        // fill (reset) all options
        myLoadThread->fillOptions(neteditOptions);
        // set default options defined in GNELoadThread::setDefaultOptions(...)
        myLoadThread->setDefaultOptions(neteditOptions);
        // set file to load
        neteditOptions.resetWritable();
        if (neteditConfig.size() > 0) {
            neteditOptions.set("configuration-file", neteditConfig);
            // set status bar
            setStatusBarText("Reloading netedit config file '" + neteditConfig + "'");
        } else if (sumoConfig.size() > 0) {
            neteditOptions.set("sumocfg-file", sumoConfig);
            // set status bar
            setStatusBarText("Reloading sumo config file '" + sumoConfig + "'");
        } else if (networkFile.size() > 0) {
            neteditOptions.set("net-file", networkFile);
            // set status bar
            setStatusBarText("Reloading network file '" + networkFile + "'");
        } else {
            // nothing to load
            return 0;
        }
        // load network
        myLoadThread->loadNetworkOrConfig();
    }
    return 1;
}


long
GNEApplicationWindow::onUpdSmartReload(FXObject* sender, FXSelector, void*) {
    auto& neteditOptions = OptionsCont::getOptions();
    // declare strings
    FXString neteditConfig = TL("&Reload Netedit config");
    FXString sumoConfig = TL("&Reload Sumo config");
    FXString netFile = TL("&Reload Network");
    FXString reload = TL("&Reload");
    // check options
    if (myNet == nullptr) {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
        sender->handle(this, FXSEL(SEL_COMMAND, ID_SETSTRINGVALUE), &reload);
    } else if (neteditOptions.getString("configuration-file").size() > 0) {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
        sender->handle(this, FXSEL(SEL_COMMAND, ID_SETSTRINGVALUE), &neteditConfig);
    } else if (neteditOptions.getString("sumocfg-file").size() > 0) {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
        sender->handle(this, FXSEL(SEL_COMMAND, ID_SETSTRINGVALUE), &sumoConfig);
    } else if (neteditOptions.getString("net-file").size() > 0) {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
        sender->handle(this, FXSEL(SEL_COMMAND, ID_SETSTRINGVALUE), &netFile);
    } else {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
        sender->handle(this, FXSEL(SEL_COMMAND, ID_SETSTRINGVALUE), &reload);
    }
    return 1;
}


long
GNEApplicationWindow::onCmdReloadNetwork(FXObject*, FXSelector, void*) {
    auto& neteditOptions = OptionsCont::getOptions();
    // check if close current file
    if (onCmdClose(0, 0, 0) == 1) {
        // store size, position and viewport
        storeWindowSizeAndPos();
        gSchemeStorage.saveViewport(0, 0, -1, 0); // recenter view
        // set flag
        myAmLoading = true;
        // get network
        const auto networkFile = neteditOptions.getString("net-file");
        // fill (reset) all options
        myLoadThread->fillOptions(neteditOptions);
        // set default options defined in GNELoadThread::setDefaultOptions(...)
        myLoadThread->setDefaultOptions(neteditOptions);
        // set file to load
        neteditOptions.resetWritable();
        neteditOptions.set("net-file", networkFile);
        // set status bar
        setStatusBarText("Reloading network file '" + networkFile + "'");
        // loaad network
        myLoadThread->loadNetworkOrConfig();
    }
    return 1;
}


long
GNEApplicationWindow::onUpdReloadNetwork(FXObject* sender, FXSelector, void*) {
    auto& neteditOptions = OptionsCont::getOptions();
    if (myNet == nullptr) {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
        sender->handle(this, FXSEL(SEL_COMMAND, ID_HIDE), nullptr);
    } else if ((neteditOptions.getString("net-file").size() > 0) && 
        ((neteditOptions.getString("configuration-file").size() > 0) || (neteditOptions.getString("sumocfg-file").size() > 0))) {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
        sender->handle(this, FXSEL(SEL_COMMAND, ID_SHOW), nullptr);
    } else {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
        sender->handle(this, FXSEL(SEL_COMMAND, ID_HIDE), nullptr);
    }
    return 1;
}


long
GNEApplicationWindow::onCmdOpenRecent(FXObject*, FXSelector, void* fileData) {
    // first check that current edited Net can be closed (und therefore the undo-list cleared, see #5753)
    if (myAmLoading) {
        myStatusbar->getStatusLine()->setText(TL("Already loading!"));
    } else if (onCmdClose(0, 0, 0) == 1) {
        // get filedata
        const std::string recentFile = ((const char*)fileData);
        // check if we're loading a network o a config
        if ((recentFile.find(".neteditcfg") != std::string::npos) || 
            (recentFile.find(".sumocfg") != std::string::npos) ||
            (recentFile.find(".netccfg") != std::string::npos)) {
            // load config
            loadConfiguration(recentFile);
        } else {
            // load network
            loadNetwork(recentFile);
        }
    }
    return 1;
}


long
GNEApplicationWindow::onCmdClose(FXObject*, FXSelector, void*) {
    if (myViewNet == nullptr) {
        return 1;
    } else if (continueWithUnsavedChanges("close")) {
        closeAllWindows();
        // add a separator to the log
        myMessageWindow->addSeparator();
        // hide all Supermode, Network and demand commands
        mySupermodeCommands.hideSupermodeCommands();
        myModesMenuCommands.networkMenuCommands.hideNetworkMenuCommands();
        myModesMenuCommands.demandMenuCommands.hideDemandMenuCommands();
        myModesMenuCommands.dataMenuCommands.hideDataMenuCommands();
        // hide view options
        myEditMenuCommands.networkViewOptions.hideNetworkViewOptionsMenuChecks();
        myEditMenuCommands.demandViewOptions.hideDemandViewOptionsMenuChecks();
        myEditMenuCommands.dataViewOptions.hideDataViewOptionsMenuChecks();
        return 1;
    } else {
        return 0;
    }
}


long
GNEApplicationWindow::onCmdLocate(FXObject*, FXSelector sel, void*) {
    if (myMDIClient->numChildren() > 0) {
        GNEViewParent* w = dynamic_cast<GNEViewParent*>(myMDIClient->getActiveChild());
        if (w != nullptr) {
            w->onCmdLocate(nullptr, sel, nullptr);
        }
    }
    return 1;
}


long
GNEApplicationWindow::onCmdToolNetDiff(FXObject*, FXSelector, void*) {
/*
    // open a modal NetDiff tool dialog
    GNEToolNetDiff(this);  // NOSONAR
        const auto &neteditOptions = OptionsCont::getOptions();
        // check that currently there is a View
        if (myViewNet == nullptr) {
            return 0;
        }
    #ifdef WIN32
        // check that python folder is defined in PATH
        const char* path = getenv("PATH");
        if ((strstr(path, "Python") == nullptr) && (strstr(path, "python") == nullptr)) {
            WRITE_ERROR(TL("Python folder must be defined in PATH"));
            return 0;
        }
    #endif
        // check that SUMO_HOME is defined
        const char* sumoPath = getenv("SUMO_HOME");
        if (sumoPath == nullptr) {
            WRITE_ERROR(TL("Enviroment variable SUMO_HOME must be defined"));
            return 0;
        }
        // get netdiff path
        const std::string netDiff = std::string(sumoPath) + "/tools/net/netdiff.py";
        if (!FileHelpers::isReadable(netDiff)) {
            WRITE_ERROR("netdiff.py cannot be found in path '" + netDiff + "'");
            return 0;
        }
        // check if network is saved
        if (!myViewNet->getNet()->isNetSaved()) {
            // save network
            onCmdSaveNetwork(nullptr, 0, nullptr);
            if (!myViewNet->getNet()->isNetSaved()) {
                return 0;
            }
        }
        // get the second network to ddiff
        opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::OPEN_NET));
        opendialog.setSelectMode(SELECTFILE_EXISTING);
        opendialog.setPatternList("SUMO nets (*.net.xml,*.net.xml.gz)\nAll files (*)");
        if (gCurrentFolder.length() != 0) {
            opendialog.setDirectory(gCurrentFolder);
        }
        if (opendialog.execute()) {
            // get file
            const std::string secondNet = opendialog.getFilename().text();
            // check if secondNet isn't empty
            if (secondNet.empty()) {
                return 0;
            }
            // extract folder
            const std::string secondNetFolder = getFolder(secondNet).text();
            // declare python command
            std::string cmd = "cd " + secondNetFolder + TL("&") +  // folder to save diff files (the same of second net)
                              "python " + netDiff +                           // netdiff.py
                              " " + neteditOptions.getString("net-file") +             // netA (current)
                              " " + secondNet +                               // net B
                              " diff";                                        // netdiff options
            // start in background
    #ifndef WIN32
            cmd = cmd + " &";
    #else
            // see "help start" for the parameters
            cmd = "start /B \"\" " + cmd;
    #endif
            WRITE_MESSAGE(TL("Running " + cmd + ".");
            // yay! fun with dangerous commands... Never use this over the internet
            SysUtils::runHiddenCommand(cmd);
        }
        */
    return 1;
}


long
GNEApplicationWindow::onUpdToolNetDiff(FXObject* sender, FXSelector, void*) {
    // currently disabled
    sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    return 1;
}


long
GNEApplicationWindow::onUpdOpen(FXObject* sender, FXSelector, void*) {
    sender->handle(this, myAmLoading ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    return 1;
}


long
GNEApplicationWindow::onCmdClearMsgWindow(FXObject*, FXSelector, void*) {
    myMessageWindow->clear();
    return 1;
}


long
GNEApplicationWindow::onCmdLoadAdditionalsInSUMOGUI(FXObject*, FXSelector, void*) {
    // write warning if netedit is running in testing mode
    WRITE_DEBUG("Toggle load additionals in sumo-gui");
    return 1;
}


long
GNEApplicationWindow::onCmdLoadDemandInSUMOGUI(FXObject*, FXSelector, void*) {
    // write warning if netedit is running in testing mode
    WRITE_DEBUG("Toggle load demand in sumo-gui");
    return 1;
}


long
GNEApplicationWindow::onCmdAbout(FXObject*, FXSelector, void*) {
    // write warning if netedit is running in testing mode
    WRITE_DEBUG("Opening about dialog");
    // create and open about dialog
    GNEAbout* about = new GNEAbout(this);
    about->create();
    about->show(PLACEMENT_OWNER);
    // write warning if netedit is running in testing mode
    WRITE_DEBUG("Closed about dialog");
    return 1;
}


long GNEApplicationWindow::onClipboardRequest(FXObject*, FXSelector, void* ptr) {
    FXEvent* event = (FXEvent*)ptr;
    FXString string = GUIUserIO::clipped.c_str();
    setDNDData(FROM_CLIPBOARD, event->target, string);
    return 1;
}


long
GNEApplicationWindow::onLoadThreadEvent(FXObject*, FXSelector, void*) {
    eventOccurred();
    return 1;
}


void
GNEApplicationWindow::eventOccurred() {
    while (!myEvents.empty()) {
        // get the next event
        GUIEvent* e = myEvents.top();
        myEvents.pop();
        // process
        switch (e->getOwnType()) {
            case GUIEventType::SIMULATION_LOADED:
                handleEvent_NetworkLoaded(e);
                break;
            case GUIEventType::MESSAGE_OCCURRED:
            case GUIEventType::WARNING_OCCURRED:
            case GUIEventType::ERROR_OCCURRED:
            case GUIEventType::DEBUG_OCCURRED:
            case GUIEventType::GLDEBUG_OCCURRED:
                handleEvent_Message(e);
                break;
            default:
                break;
        }
        delete e;
    }
}


void
GNEApplicationWindow::handleEvent_NetworkLoaded(GUIEvent* e) {
    myAmLoading = false;
    GNEEvent_NetworkLoaded* ec = static_cast<GNEEvent_NetworkLoaded*>(e);
    // check whether the loading was successfull
    if (ec->net == nullptr) {
        // report failure
        setStatusBarText("Loading of '" + ec->file + "' failed!");
    } else {
        // set new Net
        myNet = ec->net;
        // report success
        setStatusBarText("'" + ec->file + "' loaded.");
        setWindowSizeAndPos();
        // build viewparent toolbar grips before creating view parent
        getToolbarsGrip().buildViewParentToolbarsGrips();
        // initialise NETEDIT View
        GNEViewParent* viewParent = new GNEViewParent(myMDIClient, myMDIMenu, "NETEDIT VIEW", this, nullptr, myNet, myUndoList, nullptr, MDI_TRACKING, 10, 10, 300, 200);
        // create it maximized
        viewParent->maximize();
        // mark it as Active child
        myMDIClient->setActiveChild(viewParent);
        // cast pointer myViewNet
        myViewNet = dynamic_cast<GNEViewNet*>(viewParent->getView());
        // set settings in view
        if (viewParent->getView() && ec->settingsFile != "") {
            GUISettingsHandler settings(ec->settingsFile, true, true);
            settings.addSettings(viewParent->getView());
            viewParent->getView()->addDecals(settings.getDecals());
            settings.applyViewport(viewParent->getView());
            settings.setSnapshots(viewParent->getView());
        }
        // set network name on the caption
        setTitle(MFXUtils::getTitleText(myTitlePrefix, ec->file.c_str()));
        // force supermode network
        if (myViewNet) {
            myViewNet->forceSupermodeNetwork();
        }
        if (myViewNet && ec->viewportFromRegistry) {
            Position off;
            off.set(getApp()->reg().readRealEntry("viewport", "x"), getApp()->reg().readRealEntry("viewport", "y"), getApp()->reg().readRealEntry("viewport", "z"));
            Position p(off.x(), off.y(), 0);
            myViewNet->setViewportFromToRot(off, p, 0);
        }
    }
    myMessageWindow->registerMsgHandlers();
    // load elements
    loadElements();
    // load data elements
    loadDataElements();
    // after loading net shouldn't be saved
    if (myNet) {
        myNet->getSavingStatus()->networkSaved();
    }
    // update app
    update();
    // restore focus
    setFocus();
}


void
GNEApplicationWindow::handleEvent_Message(GUIEvent* e) {
    GUIEvent_Message* ec = static_cast<GUIEvent_Message*>(e);
    myMessageWindow->appendMsg(ec->getOwnType(), ec->getMsg());
}

// ---------------------------------------------------------------------------
// private methods
// ---------------------------------------------------------------------------

void
GNEApplicationWindow::fillMenuBar() {
    // build file menu
    myFileMenu = new FXMenuPane(this, LAYOUT_FIX_HEIGHT);
    GUIDesigns::buildFXMenuTitle(myToolbarsGrip.menu, TL("&File"), nullptr, myFileMenu);
    myFileMenuNETEDITConfig = new FXMenuPane(this);
    myFileMenuSUMOConfig = new FXMenuPane(this);
    myFileMenuTLS = new FXMenuPane(this);
    myFileMenuEdgeTypes = new FXMenuPane(this);
    myFileMenuAdditionals = new FXMenuPane(this);
    myFileMenuDemandElements = new FXMenuPane(this);
    myFileMenuDataElements = new FXMenuPane(this);
    myFileMenuMeanDataElements = new FXMenuPane(this);
    myFileMenuRecentNetworks = new FXMenuPane(this);
    myFileMenuRecentConfigs = new FXMenuPane(this);
    myFileMenuCommands.buildFileMenuCommands(myFileMenu, myFileMenuNETEDITConfig, myFileMenuSUMOConfig,
            myFileMenuTLS, myFileMenuEdgeTypes, myFileMenuAdditionals, myFileMenuDemandElements,
            myFileMenuDataElements, myFileMenuMeanDataElements);
    // add separator for recent files
    new FXMenuSeparator(myFileMenu);
    // build recent files
    myMenuBarFile.buildRecentNetworkFiles(myFileMenu, myFileMenuRecentNetworks);
    myMenuBarFile.buildRecentConfigFiles(myFileMenu, myFileMenuRecentConfigs);
    new FXMenuSeparator(myFileMenu);
    GUIDesigns::buildFXMenuCommandShortcut(myFileMenu,
                                           TL("&Quit"), "Ctrl+Q", TL("Quit the Application."),
                                           nullptr, this, MID_HOTKEY_CTRL_Q_CLOSE);
    // build modes menu
    myModesMenu = new FXMenuPane(this);
    myModesMenuTitle = GUIDesigns::buildFXMenuTitle(myToolbarsGrip.menu, TL("&Modes"), nullptr, myModesMenu);
    myModesMenuTitle->setTarget(this);
    myModesMenuTitle->setSelector(MID_GNE_MODESMENUTITLE);
    // build Supermode commands and hide it
    mySupermodeCommands.buildSupermodeCommands(myModesMenu);
    mySupermodeCommands.hideSupermodeCommands();
    // build separator between supermodes y modes
    new FXMenuSeparator(myModesMenu);
    // build modes menu commands
    myModesMenuCommands.buildModesMenuCommands(myModesMenu);
    // build edit menu
    myEditMenu = new FXMenuPane(this);
    GUIDesigns::buildFXMenuTitle(myToolbarsGrip.menu, TL("&Edit"), nullptr, myEditMenu);
    // build edit menu commands
    myEditMenuCommands.buildUndoRedoMenuCommands(myEditMenu);
    // build separator
    new FXMenuSeparator(myEditMenu);
    // build view options
    myEditMenuCommands.networkViewOptions.buildNetworkViewOptionsMenuChecks(myEditMenu);
    myEditMenuCommands.demandViewOptions.buildDemandViewOptionsMenuChecks(myEditMenu);
    myEditMenuCommands.dataViewOptions.buildDataViewOptionsMenuChecks(myEditMenu);
    // hide view options
    myEditMenuCommands.networkViewOptions.hideNetworkViewOptionsMenuChecks();
    myEditMenuCommands.demandViewOptions.hideDemandViewOptionsMenuChecks();
    myEditMenuCommands.dataViewOptions.hideDataViewOptionsMenuChecks();
    // build view menu commands
    myEditMenuCommands.buildViewMenuCommands(myEditMenu);
    // build separator
    new FXMenuSeparator(myEditMenu);
    // build front element menu commands
    myEditMenuCommands.buildFrontElementMenuCommand(myEditMenu);
    // build separator
    new FXMenuSeparator(myEditMenu);
    // build open in sumo menu commands
    myEditMenuCommands.buildOpenSUMOMenuCommands(myEditMenu);
    // build lock menu
    myLockMenu = new FXMenuPane(this);
    myLockMenuTitle = GUIDesigns::buildFXMenuTitle(myToolbarsGrip.menu, TL("L&ock"), nullptr, myLockMenu);
    myLockMenuTitle->setTarget(this);
    myLockMenuTitle->setSelector(MID_GNE_LOCK_MENUTITLE);
    // build lock menu commands
    myLockMenuCommands.buildLockMenuCommands(myLockMenu);
    // build processing menu (trigger netbuild computations)
    myProcessingMenu = new FXMenuPane(this);
    GUIDesigns::buildFXMenuTitle(myToolbarsGrip.menu, TL("&Processing"), nullptr, myProcessingMenu);
    myProcessingMenuCommands.buildProcessingMenuCommands(myProcessingMenu);
    // build locate menu
    myLocatorMenu = new FXMenuPane(this);
    GUIDesigns::buildFXMenuTitle(myToolbarsGrip.menu, TL("&Locate"), nullptr, myLocatorMenu);
    myLocateMenuCommands.buildLocateMenuCommands(myLocatorMenu);
    // build tools menu
    myToolsMenu = new FXMenuPane(this);
    GUIDesigns::buildFXMenuTitle(myToolbarsGrip.menu, TL("&Tools"), nullptr, myToolsMenu);
    myToolsMenuCommands.buildToolsMenuCommands(myToolsMenu);
    // build windows menu
    myWindowMenu = new FXMenuPane(this);
    GUIDesigns::buildFXMenuTitle(myToolbarsGrip.menu, TL("&Window"), nullptr, myWindowMenu);
    myWindowsMenuCommands.buildWindowsMenuCommands(myWindowMenu, myStatusbar, myMessageWindow);
    // build help menu
    myHelpMenu = new FXMenuPane(this);
    GUIDesigns::buildFXMenuTitle(myToolbarsGrip.menu, TL("&Help"), nullptr, myHelpMenu);
    // build help menu commands
    GUIDesigns::buildFXMenuCommandShortcut(myHelpMenu, TL("&Online Documentation"), "F1", TL("Open Online documentation."),
                                           nullptr, this, MID_HOTKEY_F1_ONLINEDOCUMENTATION);
    new FXMenuSeparator(myHelpMenu);
    GUIDesigns::buildFXMenuCommandShortcut(myHelpMenu, TL("&Changelog"), "", TL("Open Changelog."),
                                           nullptr, this, MID_CHANGELOG);
    GUIDesigns::buildFXMenuCommandShortcut(myHelpMenu, TL("&Hotkeys"), "", TL("Open Hotkeys."),
                                           nullptr, this, MID_HOTKEYS);
    GUIDesigns::buildFXMenuCommandShortcut(myHelpMenu, TL("&Tutorial"), "", TL("Open Tutorial."),
                                           nullptr, this, MID_TUTORIAL);
    new FXMenuSeparator(myHelpMenu);
    GUIDesigns::buildFXMenuCommandShortcut(myHelpMenu, TL("&About"), "F12", TL("About netedit."),
                                           GUIIconSubSys::getIcon(GUIIcon::NETEDIT_MINI), this, MID_HOTKEY_F12_ABOUT);
}


FXGLCanvas*
GNEApplicationWindow::getBuildGLCanvas() const {
    // NETEDIT uses only a single View, then return nullptr
    return nullptr;
}


SUMOTime
GNEApplicationWindow::getCurrentSimTime() const {
    return 0;
}


double
GNEApplicationWindow::getTrackerInterval() const {
    return 1;
}


GNEUndoList*
GNEApplicationWindow::getUndoList() {
    return myUndoList;
}


GNEUndoListDialog*
GNEApplicationWindow::getUndoListDialog() {
    return myUndoListDialog;
}


GNEViewNet*
GNEApplicationWindow::getViewNet() {
    return myViewNet;
}


GNEApplicationWindowHelper::ToolbarsGrip&
GNEApplicationWindow::getToolbarsGrip() {
    return myToolbarsGrip;
}


void
GNEApplicationWindow::updateRecomputingLabel() {
    if (myViewNet && myViewNet->getNet()) {
        // show
        myRequireRecomputingButton->show();
        // set label depending of recomputing
        if (myNet->getAttributeCarriers()->getJunctions().empty() || myNet->isNetRecomputed()) {
            myRequireRecomputingButton->setText("");
            myRequireRecomputingButton->setTipText("Network computed");
            myRequireRecomputingButton->setIcon(GUIIconSubSys::getIcon(GUIIcon::OK));
            myRequireRecomputingButton->setBackColor(FXRGBA(240, 255, 205, 255));
        } else {
            myRequireRecomputingButton->setText(TL("Press F5"));
            myRequireRecomputingButton->setTipText("Network requires recomputing");
            myRequireRecomputingButton->setIcon(GUIIconSubSys::getIcon(GUIIcon::WARNING));
            myRequireRecomputingButton->setBackColor(FXRGBA(253, 255, 206, 255));
        }
    } else {
        // hide
        myRequireRecomputingButton->hide();
    }
}


void
GNEApplicationWindow::closeAllWindows() {
    // check if view has to be saved
    if (myViewNet) {
        myViewNet->saveVisualizationSettings();
    }
    // lock tracker
    myTrackerLock.lock();
    // remove trackers and other external windows
    while (!myGLWindows.empty()) {
        delete myGLWindows.front();
    }
    myViewNet = nullptr;
    for (FXMainWindow* const window : myTrackerWindows) {
        window->destroy();
        delete window;
    }
    myTrackerWindows.clear();
    // reset the caption
    setTitle(myTitlePrefix);
    // unlock tracker
    myTrackerLock.unlock();
    // remove coordinate information
    myGeoCoordinate->setText(TL("N/A"));
    myCartesianCoordinate->setText(TL("N/A"));
    if (myTestCoordinate) {
        myTestCoordinate->setText(TL("N/A"));
    }
    // check if net can be deleted
    if (myNet != nullptr) {
        delete myNet;
        myNet = nullptr;
        GeoConvHelper::resetLoaded();
    }
    myMessageWindow->unregisterMsgHandlers();
    // Reset textures
    GUITextureSubSys::resetTextures();
    // reset fonts
    GLHelper::resetFont();
}


FXCursor*
GNEApplicationWindow::getDefaultCursor() {
    return getApp()->getDefaultCursor(DEF_ARROW_CURSOR);
}


void
GNEApplicationWindow::loadOptionOnStartup() {
    auto& neteditOptions = OptionsCont::getOptions();
    if (neteditOptions.getBool("new")) {
        createNewNetwork();
    } else {
        // set flag
        myAmLoading = true;
        // set status bar
        setStatusBarText("Loading console arguments");
        // load console arguments
        myLoadThread->loadNetworkOrConfig();
        // add it into recent networks and configs
        if (neteditOptions.getString("net-file").size() > 0) {
            myMenuBarFile.myRecentNetworks.appendFile(neteditOptions.getString("net-file").c_str());
        }
        if (neteditOptions.getString("configuration-file").size() > 0) {
            myMenuBarFile.myRecentNetworks.appendFile(neteditOptions.getString("net-file").c_str());
        }
    }
}


void
GNEApplicationWindow::createNewNetwork() {
    auto& neteditOptions = OptionsCont::getOptions();
    // save windows size and position
    storeWindowSizeAndPos();
    // enable loading flag and disable reloading flag
    myAmLoading = true;
    // recenter view
    gSchemeStorage.saveViewport(0, 0, -1, 0);
    // fill (reset) all options
    GNELoadThread::fillOptions(neteditOptions);
    // set default options defined in GNELoadThread::setDefaultOptions(...)
    GNELoadThread::setDefaultOptions(neteditOptions);
    // update status bar
    setStatusBarText("Creating new network.");
    // create new network
    myLoadThread->newNetwork();
    // update window
    update();
}


void
GNEApplicationWindow::loadNetwork(const std::string &networkFile) {
    auto& neteditOptions = OptionsCont::getOptions();
    // store size, position and viewport
    storeWindowSizeAndPos();
    gSchemeStorage.saveViewport(0, 0, -1, 0); // recenter view
    // set flag
    myAmLoading = true;
    // fill (reset) all options
    myLoadThread->fillOptions(neteditOptions);
    // set default options defined in GNELoadThread::setDefaultOptions(...)
    myLoadThread->setDefaultOptions(neteditOptions);
    // set file to load
    neteditOptions.resetWritable();
    neteditOptions.set("net-file", networkFile);
    // set status bar
    setStatusBarText("Loading network file '" + networkFile + "'");
    // load network
    myLoadThread->loadNetworkOrConfig();
    // add it into recent nets
    myMenuBarFile.myRecentNetworks.appendFile(networkFile.c_str());
}


void
GNEApplicationWindow::loadConfiguration(const std::string &configurationFile) {
    auto& neteditOptions = OptionsCont::getOptions();
    // store size, position and viewport
    storeWindowSizeAndPos();
    gSchemeStorage.saveViewport(0, 0, -1, 0); // recenter view
    // set flag
    myAmLoading = true;
    // fill (reset) all options
    myLoadThread->fillOptions(neteditOptions);
    // set default options defined in GNELoadThread::setDefaultOptions(...)
    myLoadThread->setDefaultOptions(neteditOptions);
    // set file to load
    neteditOptions.resetWritable();
    neteditOptions.set("configuration-file", configurationFile);
    // set status bar
    setStatusBarText("Loading configuration file '" + configurationFile + "'");
    // load config
    myLoadThread->loadNetworkOrConfig();
    // add it into recent configs
    myMenuBarFile.myRecentNetworks.appendFile(configurationFile.c_str());
}


void
GNEApplicationWindow::loadOSM(const std::string &OSMFile) {
    auto& neteditOptions = OptionsCont::getOptions();
    // store size, position and viewport
    storeWindowSizeAndPos();
    gSchemeStorage.saveViewport(0, 0, -1, 0); // recenter view
    // set flag
    myAmLoading = true;
    // fill (reset) all options
    myLoadThread->fillOptions(neteditOptions);
    // set default options defined in GNELoadThread::setDefaultOptions(...)
    myLoadThread->setDefaultOptions(neteditOptions);
    // recommended osm options
    // https://sumo.dlr.de/wiki/Networks/Import/OpenStreetMap#Recommended_NETCONVERT_Options
    neteditOptions.set("osm-files", OSMFile);
    neteditOptions.set("geometry.remove", "true");
    neteditOptions.set("ramps.guess", "true");
    neteditOptions.set("junctions.join", "true");
    neteditOptions.set("tls.guess-signals", "true");
    neteditOptions.set("tls.discard-simple", "true");
    // open wizard dialog
    GUIDialog_Options* wizard =
        new GUIDialog_Options(this, OptionsCont::getOptions(), TL("Select Import Options"), getWidth(), getHeight());
    if (wizard->execute()) {
        NIFrame::checkOptions(); // needed to set projection parameters
        // set file to load
        neteditOptions.resetWritable();
        neteditOptions.set("configuration-file", OSMFile);
        // set status bar
        setStatusBarText("Loading OSM file '" + OSMFile + "'");
        // load config
        myLoadThread->loadNetworkOrConfig();
    }
    // after load, mark network saved
    myNet->getSavingStatus()->networkSaved();
}

void
GNEApplicationWindow::setStatusBarText(const std::string& statusBarText) {
    myStatusbar->getStatusLine()->setText(statusBarText.c_str());
    myStatusbar->getStatusLine()->setNormalText(statusBarText.c_str());
}


long
GNEApplicationWindow::computeJunctionWithVolatileOptions() {
    // declare variable to save FXMessageBox outputs.
    FXuint answer = 0;
    // write warning if netedit is running in testing mode
    WRITE_DEBUG("Opening FXMessageBox 'Volatile Recomputing'");
    // open question dialog box
    answer = FXMessageBox::question(myNet->getViewNet()->getApp(), MBOX_YES_NO, TL("Recompute with volatile options"),
                                    "Changes produced in the net due a recomputing with volatile options cannot be undone. Continue?");
    if (answer != 1) { //1:yes, 2:no, 4:esc
        // write warning if netedit is running in testing mode
        if (answer == 2) {
            WRITE_DEBUG("Closed FXMessageBox 'Volatile Recomputing' with 'No'");
        } else if (answer == 4) {
            WRITE_DEBUG("Closed FXMessageBox 'Volatile Recomputing' with 'ESC'");
        }
        // abort recompute with volatile options
        return 0;
    } else {
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Closed FXMessageBox 'Volatile Recomputing' with 'Yes'");
        // save additionals
        onCmdSaveAdditionals(nullptr, 0, nullptr);
        // save demand elements
        onCmdSaveDemandElements(nullptr, 0, nullptr);
        // save data elements
        onCmdSaveDataElements(nullptr, 0, nullptr);
        // save mean datas
        onCmdSaveMeanDatas(nullptr, 0, nullptr);
        // compute with volatile options
        myNet->computeNetwork(this, true, true);
        updateControls();
        return 1;
    }
}

bool
GNEApplicationWindow::consoleOptionsLoaded() {
    if (myConsoleOptionsLoaded) {
        myConsoleOptionsLoaded = false;
        return true;
    } else {
        return false;
    }
}


long
GNEApplicationWindow::onCmdSetSuperMode(FXObject* sender, FXSelector sel, void* ptr) {
    // check that currently there is a View
    if (myViewNet) {
        myViewNet->onCmdSetSupermode(sender, sel, ptr);
    }
    return 1;
}


long
GNEApplicationWindow::onCmdSetMode(FXObject* sender, FXSelector sel, void* ptr) {
    // check that currently there is a View
    if (myViewNet) {
        myViewNet->onCmdSetMode(sender, sel, ptr);
    }
    return 1;
}


long
GNEApplicationWindow::onCmdLockElements(FXObject*, FXSelector, void*) {
    if (myViewNet) {
        myViewNet->getLockManager().updateFlags();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdLockAllElements(FXObject*, FXSelector, void*) {
    // lock all
    myLockMenuCommands.lockAll();
    myViewNet->getLockManager().updateFlags();
    return 1;
}


long
GNEApplicationWindow::onCmdUnlockAllElements(FXObject*, FXSelector, void*) {
    // unlock all
    myLockMenuCommands.unlockAll();
    myViewNet->getLockManager().updateFlags();
    return 1;
}


long
GNEApplicationWindow::onCmdLockSelectElements(FXObject*, FXSelector, void*) {
    if (myViewNet) {
        myViewNet->update();
    }
    return 1;
}


long
GNEApplicationWindow::onUpdLockMenuTitle(FXObject*, FXSelector, void*) {
    if (myViewNet) {
        if (myViewNet->getEditModes().isCurrentSupermodeNetwork()) {
            // supermode network
            if ((myViewNet->getEditModes().networkEditMode == NetworkEditMode::NETWORK_INSPECT) ||
                    (myViewNet->getEditModes().networkEditMode == NetworkEditMode::NETWORK_SELECT) ||
                    (myViewNet->getEditModes().networkEditMode == NetworkEditMode::NETWORK_DELETE) ||
                    (myViewNet->getEditModes().networkEditMode == NetworkEditMode::NETWORK_CONNECT) ||
                    (myViewNet->getEditModes().networkEditMode == NetworkEditMode::NETWORK_MOVE)) {
                myLockMenuTitle->enable();
            } else {
                myLockMenuTitle->disable();
            }
        } else if (myViewNet->getEditModes().isCurrentSupermodeDemand()) {
            // supermode demand
            if ((myViewNet->getEditModes().demandEditMode == DemandEditMode::DEMAND_INSPECT) ||
                    (myViewNet->getEditModes().demandEditMode == DemandEditMode::DEMAND_SELECT) ||
                    (myViewNet->getEditModes().demandEditMode == DemandEditMode::DEMAND_DELETE) ||
                    (myViewNet->getEditModes().demandEditMode == DemandEditMode::DEMAND_MOVE)) {
                myLockMenuTitle->enable();
            } else {
                myLockMenuTitle->disable();
            }
        } else if (myViewNet->getEditModes().isCurrentSupermodeData()) {
            // supermode data
            if ((myViewNet->getEditModes().dataEditMode == DataEditMode::DATA_INSPECT) ||
                    (myViewNet->getEditModes().dataEditMode == DataEditMode::DATA_SELECT) ||
                    (myViewNet->getEditModes().dataEditMode == DataEditMode::DATA_DELETE)) {
                myLockMenuTitle->enable();
            } else {
                myLockMenuTitle->disable();
            }
        } else {
            myLockMenuTitle->disable();
        }
    } else {
        myLockMenuTitle->disable();
    }
    return 1;
}

long
GNEApplicationWindow::onCmdProcessButton(FXObject*, FXSelector sel, void*) {
    // first check if there is a view
    if (myViewNet) {
        // process depending of supermode
        if (myViewNet->getEditModes().isCurrentSupermodeNetwork()) {
            // check what FXMenuCommand was called
            switch (FXSELID(sel)) {
                case MID_HOTKEY_F5_COMPUTE_NETWORK_DEMAND:
                    // show extra information for tests
                    WRITE_DEBUG("Key F5 (Compute) pressed");
                    myNet->computeNetwork(this, true, false);
                    updateControls();
                    break;
                case MID_HOTKEY_SHIFT_F5_COMPUTEJUNCTIONS_VOLATILE:
                    // show extra information for tests
                    WRITE_DEBUG("Keys Shift + F5 (Compute with volatile options) pressed");
                    computeJunctionWithVolatileOptions();
                    break;
                case MID_HOTKEY_F6_CLEAN_SOLITARYJUNCTIONS_UNUSEDROUTES:
                    // show extra information for tests
                    WRITE_DEBUG("Key F6 (Clean junction) pressed");
                    myNet->removeSolitaryJunctions(myUndoList);
                    break;
                case MID_HOTKEY_F7_JOIN_SELECTEDJUNCTIONS_ROUTES:
                    // show extra information for tests
                    WRITE_DEBUG("Key F7 (Join junctions) pressed");
                    myNet->joinSelectedJunctions(myUndoList);
                    break;
                case MID_HOTKEY_F8_CLEANINVALID_CROSSINGS_DEMANDELEMENTS:
                    // show extra information for tests
                    WRITE_DEBUG("Key F8 (Clean invalid crossings) pressed");
                    myNet->cleanInvalidCrossings(myUndoList);
                    break;
                default:
                    break;
            }
        } else if (myViewNet->getEditModes().isCurrentSupermodeDemand()) {
            // check what FXMenuCommand was called
            switch (FXSELID(sel)) {
                case MID_HOTKEY_F5_COMPUTE_NETWORK_DEMAND:
                    // show extra information for tests
                    WRITE_DEBUG("Key F5 (Compute) pressed");
                    myNet->computeDemandElements(this);
                    updateControls();
                    break;
                case MID_HOTKEY_F6_CLEAN_SOLITARYJUNCTIONS_UNUSEDROUTES:
                    // show extra information for tests
                    WRITE_DEBUG("Key F6 (RemoveUnusedRoutes) pressed");
                    myNet->cleanUnusedRoutes(myUndoList);
                    break;
                case MID_HOTKEY_F7_JOIN_SELECTEDJUNCTIONS_ROUTES:
                    // show extra information for tests
                    WRITE_DEBUG("Key F7 (JoinRoutes) pressed");
                    myNet->joinRoutes(myUndoList);
                    break;
                case MID_HOTKEY_SHIFT_F7_ADJUST_PERSON_PLANS:
                    // show extra information for tests
                    WRITE_DEBUG("Key F7 (AdjustPersonPlans) pressed");
                    myNet->adjustPersonPlans(myUndoList);
                    break;
                case MID_HOTKEY_F8_CLEANINVALID_CROSSINGS_DEMANDELEMENTS:
                    // show extra information for tests
                    WRITE_DEBUG("Key F8 (CleanInvalidDemandElements) pressed");
                    myNet->cleanInvalidDemandElements(myUndoList);
                    break;
                default:
                    break;
            }
        }
    }
    return 1;
}


long
GNEApplicationWindow::onCmdNewWindow(FXObject*, FXSelector, void*) {
    FXRegistry reg("SUMO netedit", "netedit");
    std::string netedit = "netedit";
    const char* sumoPath = getenv("SUMO_HOME");
    if (sumoPath != nullptr) {
        std::string newPath = std::string(sumoPath) + "/bin/netedit";
        if (FileHelpers::isReadable(newPath) || FileHelpers::isReadable(newPath + ".exe")) {
            netedit = "\"" + newPath + "\"";
        }
    }
    std::string cmd = netedit;
    // start in background
#ifndef WIN32
    cmd = cmd + " &";
#else
    // see "help start" for the parameters
    cmd = "start /B \"\" " + cmd;
#endif
    WRITE_MESSAGE(TL("Running ") + cmd + ".");
    // yay! fun with dangerous commands... Never use this over the internet
    SysUtils::runHiddenCommand(cmd);
    return 1;
}


long
GNEApplicationWindow::onCmdOpenSUMOGUI(FXObject* obj, FXSelector sel, void* ptr) {
    // get option container
    auto& neteditOptions = OptionsCont::getOptions();
    // input parameters
    std::string inputParameters;
    // if we have only a network, then load directly without creating a SUMOConfig
    if ((myNet->getAttributeCarriers()->getNumberOfAdditionals() == 0 && myNet->getAttributeCarriers()->getNumberOfDemandElements() == 0) ||
        ((myEditMenuCommands.loadAdditionalsInSUMOGUI->getCheck() == FALSE) && (myEditMenuCommands.loadDemandInSUMOGUI->getCheck() == FALSE))) {
        // force save network
        if (onCmdSaveNetwork(obj, sel, ptr) == 0) {
            // network wasn't saved, then stop
            return 0;
        }
        inputParameters = " --registry-viewport -n \"" + neteditOptions.getString("net-file") + "\"";
        // write info
        WRITE_MESSAGE(TL("Loading network '") + neteditOptions.getString("net-file") + TL("' in SUMO-GUI"));
    } else {
        // force save SUMOConfig
        if (onCmdSaveSUMOConfig(obj, sel, ptr) == 0) {
            // SUMOConfig wasn't saved, then stop
            return 0;
        }
        inputParameters = " --registry-viewport -c \"" + neteditOptions.getString("sumocfg-file") + "\"";
        // write info
        WRITE_MESSAGE(TL("Loading sumo config '") + neteditOptions.getString("sumocfg-file") + TL("' in SUMO-GUI"));
    }
    // save current viewport in registry
    FXRegistry reg("SUMO GUI", "sumo-gui");
    reg.read();
    reg.writeRealEntry("viewport", "x", myViewNet->getChanger().getXPos());
    reg.writeRealEntry("viewport", "y", myViewNet->getChanger().getYPos());
    reg.writeRealEntry("viewport", "z", myViewNet->getChanger().getZPos());
    reg.write();
    // declare executable
    std::string sumoGuiExecutable = "sumo-gui";
    // if SUMO_HOME is defined, update executable
    const char* sumoHome = getenv("SUMO_HOME");
    if (sumoHome != nullptr) {
        std::string newPath = std::string(sumoHome) + "/bin/sumo-gui";
        if (FileHelpers::isReadable(newPath) || FileHelpers::isReadable(newPath + ".exe")) {
            sumoGuiExecutable = "\"" + newPath + "\"";
        }
    }
    // declare command
    std::string cmd = sumoGuiExecutable + inputParameters;
    // start in background
#ifndef WIN32
    cmd = cmd + " &";
#else
    // see "help start" for the parameters
    cmd = "start /B \"\" " + cmd;
#endif
    WRITE_MESSAGE(TL("Running ") + cmd + ".");
    // yay! fun with dangerous commands... Never use this over the internet
    SysUtils::runHiddenCommand(cmd);
    return 1;
}


long
GNEApplicationWindow::onCmdAbort(FXObject*, FXSelector, void*) {
    // check that view exists
    if (myViewNet) {
        // show extra information for tests
        WRITE_DEBUG("Key ESC (abort) pressed");
        // first check if we're selecting a subset of edges in TAZ Frame
        if (myViewNet->getViewParent()->getTAZFrame()->getTAZSelectionStatisticsModule()->getEdgeAndTAZChildrenSelected().size() > 0) {
            // show extra information for tests
            WRITE_DEBUG("Cleaning current selected edges");
            // clear current selection
            myViewNet->getViewParent()->getTAZFrame()->getTAZSelectionStatisticsModule()->clearSelectedEdges();
        } else if (myViewNet->getViewParent()->getInspectorFrame()->shown()) {
            // check if stop select parent
            if (myViewNet->getViewParent()->getInspectorFrame()->getNeteditAttributesEditor()->isSelectingParent()) {
                // show extra information for tests
                WRITE_DEBUG("Stop select new parent");
                // and stop select paretn
                myViewNet->getViewParent()->getInspectorFrame()->getNeteditAttributesEditor()->stopSelectParent();
            } else {
                // show extra information for tests
                WRITE_DEBUG("Cleaning inspected elements");
                // clear inspected elements
                myViewNet->getViewParent()->getInspectorFrame()->inspectSingleElement(nullptr);
            }
        } else {
            // abort current operation
            myViewNet->abortOperation();
        }
    }
    return 1;
}


long
GNEApplicationWindow::onCmdDel(FXObject*, FXSelector, void*) {
    // check that view exists
    if (myViewNet) {
        // show extra information for tests
        WRITE_DEBUG("Key DEL (delete) pressed");
        myViewNet->hotkeyDel();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdEnter(FXObject*, FXSelector, void*) {
    // check that view exists
    if (myViewNet) {
        // show extra information for tests
        WRITE_DEBUG("Key ENTER pressed");
        myViewNet->hotkeyEnter();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdBackspace(FXObject*, FXSelector, void*) {
    // check that view exists
    if (myViewNet) {
        // show extra information for tests
        WRITE_DEBUG("Key BACKSPACE pressed");
        myViewNet->hotkeyBackSpace();
    }
    return 1;
}

long
GNEApplicationWindow::onCmdForceSaveNetwork(FXObject* /*sender*/, FXSelector /*sel*/, void* /*ptr*/) {
    // check that view exists
    if (myViewNet) {
        myViewNet->getNet()->getSavingStatus()->requireSaveNetwork();
        myViewNet->update();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdForceSaveAdditionals(FXObject* /*sender*/, FXSelector /*sel*/, void* /*ptr*/) {
    // check that view exists
    if (myViewNet) {
        myViewNet->getNet()->getSavingStatus()->requireSaveAdditionals();
        update();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdForceSaveDemandElements(FXObject* /*sender*/, FXSelector /*sel*/, void* /*ptr*/) {
    // check that view exists
    if (myViewNet) {
        myViewNet->getNet()->getSavingStatus()->requireSaveDemandElements();
        update();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdForceSaveDataElements(FXObject* /*sender*/, FXSelector /*sel*/, void* /*ptr*/) {
    // check that view exists
    if (myViewNet) {
        myViewNet->getNet()->getSavingStatus()->requireSaveDataElements();
        update();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdFocusFrame(FXObject*, FXSelector, void*) {
    // check that view exists
    if (myViewNet) {
        myViewNet->hotkeyFocusFrame();
    }
    return 1;
}


long
GNEApplicationWindow::onUpdRequireViewNet(FXObject* sender, FXSelector, void*) {
    // enable or disable sender element depending of viewNet
    sender->handle(this, myViewNet ? FXSEL(SEL_COMMAND, ID_ENABLE) : FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    return 1;
}


long
GNEApplicationWindow::onUpdRequireRecomputing(FXObject*, FXSelector, void*) {
    updateRecomputingLabel();
    return 1;
}


long
GNEApplicationWindow::onCmdEditViewport(FXObject*, FXSelector, void*) {
    // check that view exists
    if (myViewNet) {
        myViewNet->showViewportEditor();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdEditViewScheme(FXObject*, FXSelector, void*) {
    // check that view exists
    if (myViewNet) {
        myViewNet->showViewschemeEditor();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdToggleGrid(FXObject* sender, FXSelector sel, void* ptr) {
    // check that view exists
    if (myViewNet) {
        // show debug info
        if (myViewNet->getVisualisationSettings().showGrid) {
            // show extra information for tests
            WRITE_DEBUG("Disabled grid through Ctrl+g hotkey");
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled grid through Ctrl+g hotkey");
        }
        // Call manually toggle grid function
        myViewNet->onCmdToggleShowGrid(sender, sel, ptr);
    }
    return 1;
}


long
GNEApplicationWindow::onCmdToggleDrawJunctionShape(FXObject* sender, FXSelector sel, void* ptr) {
    // check that view exists
    if (myViewNet) {
        // show debug info
        if (myViewNet->getVisualisationSettings().drawJunctionShape) {
            // show extra information for tests
            WRITE_DEBUG("Disabled draw junction shape through Ctrl+j hotkey");
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled draw junction shape through Ctrl+j hotkey");
        }
        // Call manually toggle junction shape function
        myViewNet->onCmdToggleDrawJunctionShape(sender, sel, ptr);
    }
    return 1;
}


long
GNEApplicationWindow::onCmdSetFrontElement(FXObject*, FXSelector, void*) {
    if (myViewNet) {
        if (myViewNet->getViewParent()->getInspectorFrame()->shown()) {
            // get inspected AC
            GNEAttributeCarrier* inspectedAC = (myViewNet->getInspectedAttributeCarriers().size() == 1) ? myViewNet->getInspectedAttributeCarriers().front() : nullptr;
            // set or clear front attribute
            if (myViewNet->getFrontAttributeCarrier() == inspectedAC) {
                myViewNet->setFrontAttributeCarrier(nullptr);
            } else {
                myViewNet->setFrontAttributeCarrier(inspectedAC);
            }
            myViewNet->getViewParent()->getInspectorFrame()->getNeteditAttributesEditor()->refreshNeteditAttributesEditor(true);
        } else {
            myViewNet->setFrontAttributeCarrier(nullptr);
        }
        update();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdToggleEditOptions(FXObject* sender, FXSelector sel, void* /* ptr */) {
    // first check that we have a ViewNet
    if (myViewNet) {
        // first check what selector was called
        int numericalKeyPressed = sel - FXSEL(SEL_COMMAND, MID_HOTKEY_ALT_0_TOGGLEEDITOPTION) - 1;
        // check that numericalKeyPressed is valid
        if ((numericalKeyPressed < 0) || (numericalKeyPressed > 10)) {
            return 1;
        }
        // declare a vector in which save visible menu commands
        std::vector<MFXCheckableButton*> visibleMenuCommands;
        // get common, network and demand visible menu commands
        myViewNet->getNetworkViewOptions().getVisibleNetworkMenuCommands(visibleMenuCommands);
        myViewNet->getDemandViewOptions().getVisibleDemandMenuCommands(visibleMenuCommands);
        myViewNet->getDataViewOptions().getVisibleDataMenuCommands(visibleMenuCommands);
        // now check that numericalKeyPressed isn't greather than visible view options
        if (numericalKeyPressed >= (int)visibleMenuCommands.size()) {
            return 1;
        }
        // toggle edit options
        if (GNEApplicationWindowHelper::toggleEditOptionsNetwork(myViewNet,
                visibleMenuCommands.at(numericalKeyPressed), numericalKeyPressed, sender, sel)) {
            return 1;
        } else if (GNEApplicationWindowHelper::toggleEditOptionsDemand(myViewNet,
                   visibleMenuCommands.at(numericalKeyPressed), numericalKeyPressed, sender, sel)) {
            return 1;
        } else if (GNEApplicationWindowHelper::toggleEditOptionsData(myViewNet,
                   visibleMenuCommands.at(numericalKeyPressed), numericalKeyPressed, sender, sel)) {
            return 1;
        }
    }
    return 1;
}


long
GNEApplicationWindow::onCmdHelp(FXObject*, FXSelector, void*) {
    MFXLinkLabel::fxexecute("https://sumo.dlr.de/docs/netedit.html");
    return 1;
}


long
GNEApplicationWindow::onCmdChangelog(FXObject*, FXSelector, void*) {
    // update in every version
    MFXLinkLabel::fxexecute("https://sumo.dlr.de/docs/ChangeLog.html");
    return 1;
}


long
GNEApplicationWindow::onCmdHotkeys(FXObject*, FXSelector, void*) {
    MFXLinkLabel::fxexecute("https://sumo.dlr.de/docs/sumo-gui.html#keyboard_shortcuts");
    return 1;
}


long
GNEApplicationWindow::onCmdToggleComputeNetworkData(FXObject*, FXSelector, void*) {
    if (myProcessingMenuCommands.menuCheckRecomputeDataMode->getCheck() == TRUE) {
        return getApp()->reg().writeBoolEntry("NETEDIT", "RecomputeData", true);
    } else {
        return getApp()->reg().writeBoolEntry("NETEDIT", "RecomputeData", false);
    }
}


long
GNEApplicationWindow::onCmdTutorial(FXObject*, FXSelector, void*) {
    MFXLinkLabel::fxexecute("https://sumo.dlr.de/docs/Tutorials/index.html");
    return 1;
}


long
GNEApplicationWindow::onCmdOpenOptionsDialog(FXObject*, FXSelector, void*) {
    GUIDialog_Options* wizard = new GUIDialog_Options(this, OptionsCont::getOptions(), TL("Configure Options"), getWidth(), getHeight());
    if (wizard->execute()) {
        NIFrame::checkOptions(); // needed to set projection parameters
        NBFrame::checkOptions();
        NWFrame::checkOptions();
        SystemFrame::checkOptions(); // needed to set precision
    }
    return 1;
}


long
GNEApplicationWindow::onCmdOpenSUMOOptionsDialog(FXObject*, FXSelector, void*) {
    GUIDialog_Options* wizard = new GUIDialog_Options(this, mySUMOOptions, TL("SUMO Options"), getWidth(), getHeight());
    wizard->execute();
    return 1;
}


long
GNEApplicationWindow::onCmdUndo(FXObject*, FXSelector, void*) {
    WRITE_DEBUG("Keys Ctrl+Z (Undo) pressed");
    // Check conditions
    if (myViewNet == nullptr) {
        return 0;
    } else if (!myEditMenuCommands.undoLastChange->isEnabled()) {
        return 0;
    } else {
        // check supermode (currently ignore supermode data)
        if ((myViewNet->getUndoList()->getUndoSupermode() != Supermode::DATA) &&
                (myViewNet->getUndoList()->getUndoSupermode() != myViewNet->getEditModes().currentSupermode)) {
            // abort if user doesn't press "yes"
            if (!myViewNet->aksChangeSupermode(TL("Undo"), myViewNet->getUndoList()->getUndoSupermode())) {
                return 0;
            }
        }
        myViewNet->getUndoList()->undo();
        // update current show frame after undo
        if (myViewNet->getViewParent()->getCurrentShownFrame()) {
            myViewNet->getViewParent()->getCurrentShownFrame()->updateFrameAfterUndoRedo();
        }
        // update manually undo/redo menu commands (see #6005)
        onUpdUndo(myEditMenuCommands.undoLastChange, 0, 0);
        onUpdRedo(myEditMenuCommands.redoLastChange, 0, 0);
        // update toolbar undo-redo buttons
        myViewNet->getViewParent()->updateUndoRedoButtons();
        return 1;
    }
}


long
GNEApplicationWindow::onCmdRedo(FXObject*, FXSelector, void*) {
    WRITE_DEBUG("Keys Ctrl+Y (Redo) pressed");
    // Check conditions
    if (myViewNet == nullptr) {
        return 0;
    } else if (!myEditMenuCommands.redoLastChange->isEnabled()) {
        return 0;
    } else {
        // check supermode (currently ignore supermode data)
        if ((myViewNet->getUndoList()->getRedoSupermode() != Supermode::DATA) &&
                (myViewNet->getUndoList()->getRedoSupermode() != myViewNet->getEditModes().currentSupermode)) {
            // abort if user doesn't press "yes"
            if (!myViewNet->aksChangeSupermode(TL("Redo"), myViewNet->getUndoList()->getRedoSupermode())) {
                return 0;
            }
        }
        myViewNet->getUndoList()->redo();
        // update current show frame after redo
        if (myViewNet->getViewParent()->getCurrentShownFrame()) {
            myViewNet->getViewParent()->getCurrentShownFrame()->updateFrameAfterUndoRedo();
        }
        // update manually undo/redo menu commands (see #6005)
        onUpdUndo(myEditMenuCommands.undoLastChange, 0, 0);
        onUpdRedo(myEditMenuCommands.redoLastChange, 0, 0);
        // update toolbar undo-redo buttons
        myViewNet->getViewParent()->updateUndoRedoButtons();
        return 1;
    }
}


long
GNEApplicationWindow::onCmdOpenUndoListDialog(FXObject*, FXSelector, void*) {
    // avoid open two dialogs
    if (myUndoListDialog->shown()) {
        myUndoListDialog->setFocus();
    } else {
        myUndoListDialog->show();
    }
    return 1;
}


long
GNEApplicationWindow::onUpdOpenUndoListDialog(FXObject* sender, FXSelector, void*) {
    // check if net exist and there is something to undo/redo
    if (myNet && (myEditMenuCommands.undoLastChange->isEnabled() || myEditMenuCommands.redoLastChange->isEnabled())) {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
        if (myUndoListDialog->shown()) {
            myUndoListDialog->hide();
        }
    }
    return 1;
}


long
GNEApplicationWindow::onCmdComputePathManager(FXObject*, FXSelector, void*) {
    // first check viewNet
    if (myViewNet && !myViewNet->getNet()->getPathManager()->getPathCalculator()->isPathCalculatorUpdated()) {
        // update path calculator
        myViewNet->getNet()->getPathManager()->getPathCalculator()->updatePathCalculator();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdCut(FXObject*, FXSelector, void*) {
    WRITE_DEBUG("Key Ctrl+X (Cut) pressed");
    // Prepared for #6042
    return 1;
}


long
GNEApplicationWindow::onCmdCopy(FXObject*, FXSelector, void*) {
    WRITE_DEBUG("Key Ctrl+C (Copy) pressed");
    // Prepared for #6042
    return 1;
}


long
GNEApplicationWindow::onCmdPaste(FXObject*, FXSelector, void*) {
    WRITE_DEBUG("Key Ctrl+V (Paste) pressed");
    // Prepared for #6042
    return 1;
}


long
GNEApplicationWindow::onCmdSetTemplate(FXObject*, FXSelector, void*) {
    WRITE_DEBUG("Key Ctrl+F1 (Set Template) pressed");
    // first check if myViewNet exist
    if (myViewNet) {
        // call set template in inspector frame
        myViewNet->getViewParent()->getInspectorFrame()->getTemplateEditor()->setTemplate();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdCopyTemplate(FXObject*, FXSelector, void*) {
    WRITE_DEBUG("Key Ctrl+F2 (Copy Template) pressed");
    // first check if myViewNet exist
    if (myViewNet) {
        // call copy template in inspector frame
        myViewNet->getViewParent()->getInspectorFrame()->getTemplateEditor()->copyTemplate();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdClearTemplate(FXObject*, FXSelector, void*) {
    WRITE_DEBUG("Key Ctrl+F3 (Clear Template) pressed");
    // first check if myViewNet exist
    if (myViewNet) {
        // call clear template in inspector frame
        myViewNet->getViewParent()->getInspectorFrame()->getTemplateEditor()->clearTemplate();
    }
    return 1;
}


long
GNEApplicationWindow::onUpdNeedsNetwork(FXObject* sender, FXSelector, void*) {
    // check if net exist
    if (myNet) {
        myFileMenuCommands.enableMenuCascades();
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        myFileMenuCommands.disableMenuCascades();
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
}


long
GNEApplicationWindow::onUpdNeedsNetworkElement(FXObject* sender, FXSelector, void*) {
    // check if at least there is one edge in the network
    if (myNet && (myNet->getAttributeCarriers()->getEdges().size() > 0)) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
}


long
GNEApplicationWindow::onUpdNeedsFrontElement(FXObject* sender, FXSelector, void*) {
    // check if net, viewnet and front attribute exist
    if (myNet && myViewNet && myViewNet->getFrontAttributeCarrier()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
}


long
GNEApplicationWindow::onUpdSaveNetwork(FXObject* sender, FXSelector, void*) {
    return sender->handle(this, ((myNet == nullptr) || myNet->getSavingStatus()->isNetworkSaved()) ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
}


long
GNEApplicationWindow::onUpdSaveAdditionals(FXObject* sender, FXSelector, void*) {
    if (myNet == nullptr) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else if (myNet->getViewNet()->getViewParent()->getTAZFrame()->getTAZSaveChangesModule()->isChangesPending()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else {
        return sender->handle(this, myNet->getSavingStatus()->isAdditionalsSaved() ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    }
}


long
GNEApplicationWindow::onUpdSaveAdditionalsAs(FXObject* sender, FXSelector, void*) {
    return sender->handle(this, ((myNet == nullptr) || (myNet->getAttributeCarriers()->getNumberOfAdditionals() == 0)) ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
}


long
GNEApplicationWindow::onUpdSaveDemandElements(FXObject* sender, FXSelector, void*) {
    return sender->handle(this, ((myNet == nullptr) || myNet->getSavingStatus()->isDemandElementsSaved()) ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
}


long
GNEApplicationWindow::onUpdSaveDemandElementsAs(FXObject* sender, FXSelector, void*) {
    return sender->handle(this, ((myNet == nullptr) || (myNet->getAttributeCarriers()->getNumberOfDemandElements() == 0)) ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
}


long
GNEApplicationWindow::onUpdSaveDataElements(FXObject* sender, FXSelector, void*) {
    return sender->handle(this, ((myNet == nullptr) || myNet->getSavingStatus()->isDataElementsSaved()) ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
}


long
GNEApplicationWindow::onUpdSaveDataElementsAs(FXObject* sender, FXSelector, void*) {
    return sender->handle(this, ((myNet == nullptr) || (myNet->getAttributeCarriers()->getDataSets().size() == 0)) ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
}


long
GNEApplicationWindow::onUpdSaveMeanDatas(FXObject* sender, FXSelector, void*) {
    if (myNet == nullptr) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else {
        return sender->handle(this, myNet->getSavingStatus()->isMeanDatasSaved() ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    }
}


long
GNEApplicationWindow::onUpdSaveMeanDatasAs(FXObject* sender, FXSelector, void*) {
    return sender->handle(this, ((myNet == nullptr) || (myNet->getAttributeCarriers()->getNumberOfMeanDatas() == 0)) ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
}


long
GNEApplicationWindow::onUpdUndo(FXObject* sender, FXSelector sel, void* ptr) {
    return myUndoList->onUpdUndo(sender, sel, ptr);
}


long
GNEApplicationWindow::onUpdRedo(FXObject* sender, FXSelector sel, void* ptr) {
    return myUndoList->onUpdRedo(sender, sel, ptr);
}


long
GNEApplicationWindow::onUpdComputePathManager(FXObject* sender, FXSelector /*sel*/, void* /*ptr*/) {
    // first check viewNet
    if (myViewNet) {
        // check supermode network
        if (myViewNet->getEditModes().isCurrentSupermodeNetwork()) {
            // disable
            return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
        } else if (myViewNet->getNet()->getPathManager()->getPathCalculator()->isPathCalculatorUpdated()) {
            // disable
            return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
        } else {
            // enable
            return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
        }
    } else {
        // disable
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
}


long
GNEApplicationWindow::onCmdToggleViewOption(FXObject* sender, FXSelector sel, void* ptr) {
    // check viewNet
    if (myViewNet) {
        // continue depending of selector
        switch (FXSELID(sel)) {
            // Network
            case MID_GNE_NETWORKVIEWOPTIONS_TOGGLEGRID:
                return myViewNet->onCmdToggleShowGrid(sender, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE:
                return myViewNet->onCmdToggleDrawJunctionShape(sender, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_DRAWSPREADVEHICLES:
                return myViewNet->onCmdToggleDrawSpreadVehicles(sender, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_SHOWDEMANDELEMENTS:
                return myViewNet->onCmdToggleShowDemandElementsNetwork(sender, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_SELECTEDGES:
                return myViewNet->onCmdToggleSelectEdges(sender, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_SHOWCONNECTIONS:
                return myViewNet->onCmdToggleShowConnections(sender, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_HIDECONNECTIONS:
                return myViewNet->onCmdToggleHideConnections(sender, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_SHOWSUBADDITIONALS:
                return myViewNet->onCmdToggleShowAdditionalSubElements(sender, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_SHOWTAZELEMENTS:
                return myViewNet->onCmdToggleShowTAZElements(sender, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_EXTENDSELECTION:
                return myViewNet->onCmdToggleExtendSelection(sender, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_CHANGEALLPHASES:
                return myViewNet->onCmdToggleChangeAllPhases(sender, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_ASKFORMERGE:
                return myViewNet->onCmdToggleWarnAboutMerge(sender, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_SHOWBUBBLES:
                return myViewNet->onCmdToggleShowJunctionBubbles(sender, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_MOVEELEVATION:
                return myViewNet->onCmdToggleMoveElevation(sender, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_CHAINEDGES:
                return myViewNet->onCmdToggleChainEdges(sender, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_AUTOOPPOSITEEDGES:
                return myViewNet->onCmdToggleAutoOppositeEdge(sender, sel, ptr);
            // Demand
            case MID_GNE_DEMANDVIEWOPTIONS_SHOWGRID:
                return myViewNet->onCmdToggleShowGrid(sender, sel, ptr);
            case MID_GNE_DEMANDVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE:
                return myViewNet->onCmdToggleDrawJunctionShape(sender, sel, ptr);
            case MID_GNE_DEMANDVIEWOPTIONS_DRAWSPREADVEHICLES:
                return myViewNet->onCmdToggleDrawSpreadVehicles(sender, sel, ptr);
            case MID_GNE_DEMANDVIEWOPTIONS_HIDENONINSPECTED:
                return myViewNet->onCmdToggleHideNonInspecteDemandElements(sender, sel, ptr);
            case MID_GNE_DEMANDVIEWOPTIONS_HIDESHAPES:
                return myViewNet->onCmdToggleHideShapes(sender, sel, ptr);
            case MID_GNE_DEMANDVIEWOPTIONS_SHOWTRIPS:
                return myViewNet->onCmdToggleShowTrips(sender, sel, ptr);
            case MID_GNE_DEMANDVIEWOPTIONS_SHOWALLPERSONPLANS:
                return myViewNet->onCmdToggleShowAllPersonPlans(sender, sel, ptr);
            case MID_GNE_DEMANDVIEWOPTIONS_LOCKPERSON:
                return myViewNet->onCmdToggleLockPerson(sender, sel, ptr);
            case MID_GNE_DEMANDVIEWOPTIONS_SHOWALLCONTAINERPLANS:
                return myViewNet->onCmdToggleShowAllContainerPlans(sender, sel, ptr);
            case MID_GNE_DEMANDVIEWOPTIONS_LOCKCONTAINER:
                return myViewNet->onCmdToggleLockContainer(sender, sel, ptr);
            case MID_GNE_DEMANDVIEWOPTIONS_SHOWOVERLAPPEDROUTES:
                return myViewNet->onCmdToggleShowOverlappedRoutes(sender, sel, ptr);
            // Data
            case MID_GNE_DATAVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE:
                return myViewNet->onCmdToggleDrawJunctionShape(sender, sel, ptr);
            case MID_GNE_DATAVIEWOPTIONS_SHOWADDITIONALS:
                return myViewNet->onCmdToggleShowAdditionals(sender, sel, ptr);
            case MID_GNE_DATAVIEWOPTIONS_SHOWSHAPES:
                return myViewNet->onCmdToggleShowShapes(sender, sel, ptr);
            case MID_GNE_DATAVIEWOPTIONS_SHOWDEMANDELEMENTS:
                return myViewNet->onCmdToggleShowDemandElementsData(sender, sel, ptr);
            case MID_GNE_DATAVIEWOPTIONS_TAZRELDRAWING:
                return myViewNet->onCmdToggleTAZRelDrawing(sender, sel, ptr);
            case MID_GNE_DATAVIEWOPTIONS_TAZDRAWFILL:
                return myViewNet->onCmdToggleTAZRelDrawing(sender, sel, ptr);
            case MID_GNE_DATAVIEWOPTIONS_TAZRELONLYFROM:
                return myViewNet->onCmdToggleTAZRelOnlyFrom(sender, sel, ptr);
            case MID_GNE_DATAVIEWOPTIONS_TAZRELONLYTO:
                return myViewNet->onCmdToggleTAZRelOnlyTo(sender, sel, ptr);
            default:
                return 0;
        }
    } else {
        return 0;
    }
}


long
GNEApplicationWindow::onUpdToggleViewOption(FXObject* sender, FXSelector sel, void* /*ptr*/) {
    // get menuCheck
    MFXMenuCheckIcon* menuCheck = dynamic_cast<MFXMenuCheckIcon*>(sender);
    // check viewNet
    if (myViewNet && menuCheck) {
        // continue depending of selector
        switch (FXSELID(sel)) {
            // Network
            case MID_GNE_NETWORKVIEWOPTIONS_TOGGLEGRID:
                if (myViewNet->getNetworkViewOptions().menuCheckToggleGrid->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_NETWORKVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE:
                if (myViewNet->getNetworkViewOptions().menuCheckToggleDrawJunctionShape->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_NETWORKVIEWOPTIONS_DRAWSPREADVEHICLES:
                if (myViewNet->getNetworkViewOptions().menuCheckDrawSpreadVehicles->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_NETWORKVIEWOPTIONS_SHOWDEMANDELEMENTS:
                if (myViewNet->getNetworkViewOptions().menuCheckShowDemandElements->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_NETWORKVIEWOPTIONS_SELECTEDGES:
                if (myViewNet->getNetworkViewOptions().menuCheckSelectEdges->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_NETWORKVIEWOPTIONS_SHOWCONNECTIONS:
                if (myViewNet->getNetworkViewOptions().menuCheckShowConnections->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_NETWORKVIEWOPTIONS_HIDECONNECTIONS:
                if (myViewNet->getNetworkViewOptions().menuCheckHideConnections->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_NETWORKVIEWOPTIONS_SHOWSUBADDITIONALS:
                if (myViewNet->getNetworkViewOptions().menuCheckShowAdditionalSubElements->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_NETWORKVIEWOPTIONS_SHOWTAZELEMENTS:
                if (myViewNet->getNetworkViewOptions().menuCheckShowTAZElements->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_NETWORKVIEWOPTIONS_EXTENDSELECTION:
                if (myViewNet->getNetworkViewOptions().menuCheckExtendSelection->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_NETWORKVIEWOPTIONS_CHANGEALLPHASES:
                if (myViewNet->getNetworkViewOptions().menuCheckChangeAllPhases->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_NETWORKVIEWOPTIONS_ASKFORMERGE:
                if (myViewNet->getNetworkViewOptions().menuCheckWarnAboutMerge->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_NETWORKVIEWOPTIONS_SHOWBUBBLES:
                if (myViewNet->getNetworkViewOptions().menuCheckShowJunctionBubble->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_NETWORKVIEWOPTIONS_MOVEELEVATION:
                if (myViewNet->getNetworkViewOptions().menuCheckMoveElevation->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_NETWORKVIEWOPTIONS_CHAINEDGES:
                if (myViewNet->getNetworkViewOptions().menuCheckChainEdges->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_NETWORKVIEWOPTIONS_AUTOOPPOSITEEDGES:
                if (myViewNet->getNetworkViewOptions().menuCheckAutoOppositeEdge->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            // Demand
            case MID_GNE_DEMANDVIEWOPTIONS_SHOWGRID:
                if (myViewNet->getDemandViewOptions().menuCheckToggleGrid->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_DEMANDVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE:
                if (myViewNet->getDemandViewOptions().menuCheckToggleDrawJunctionShape->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_DEMANDVIEWOPTIONS_DRAWSPREADVEHICLES:
                if (myViewNet->getDemandViewOptions().menuCheckDrawSpreadVehicles->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_DEMANDVIEWOPTIONS_HIDESHAPES:
                if (myViewNet->getDemandViewOptions().menuCheckHideShapes->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_DEMANDVIEWOPTIONS_SHOWTRIPS:
                if (myViewNet->getDemandViewOptions().menuCheckShowAllTrips->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_DEMANDVIEWOPTIONS_SHOWALLPERSONPLANS:
                if (myViewNet->getDemandViewOptions().menuCheckShowAllPersonPlans->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_DEMANDVIEWOPTIONS_LOCKPERSON:
                if (myViewNet->getDemandViewOptions().menuCheckLockPerson->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                // special case for lock persons
                if (myViewNet->getDemandViewOptions().menuCheckLockPerson->isEnabled()) {
                    menuCheck->enable();
                } else {
                    menuCheck->disable();
                }
                break;
            case MID_GNE_DEMANDVIEWOPTIONS_SHOWALLCONTAINERPLANS:
                if (myViewNet->getDemandViewOptions().menuCheckShowAllContainerPlans->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_DEMANDVIEWOPTIONS_LOCKCONTAINER:
                if (myViewNet->getDemandViewOptions().menuCheckLockContainer->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                // special case for lock containers
                if (myViewNet->getDemandViewOptions().menuCheckLockContainer->isEnabled()) {
                    menuCheck->enable();
                } else {
                    menuCheck->disable();
                }
                break;
            case MID_GNE_DEMANDVIEWOPTIONS_HIDENONINSPECTED:
                if (myViewNet->getDemandViewOptions().menuCheckHideNonInspectedDemandElements->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_DEMANDVIEWOPTIONS_SHOWOVERLAPPEDROUTES:
                if (myViewNet->getDemandViewOptions().menuCheckShowOverlappedRoutes->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            // Data
            case MID_GNE_DATAVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE:
                if (myViewNet->getDataViewOptions().menuCheckToggleDrawJunctionShape->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_DATAVIEWOPTIONS_SHOWADDITIONALS:
                if (myViewNet->getDataViewOptions().menuCheckShowAdditionals->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_DATAVIEWOPTIONS_SHOWSHAPES:
                if (myViewNet->getDataViewOptions().menuCheckShowShapes->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_DATAVIEWOPTIONS_SHOWDEMANDELEMENTS:
                if (myViewNet->getDataViewOptions().menuCheckShowDemandElements->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_DATAVIEWOPTIONS_TAZRELDRAWING:
                if (myViewNet->getDataViewOptions().menuCheckToggleTAZRelDrawing->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_DATAVIEWOPTIONS_TAZDRAWFILL:
                if (myViewNet->getDataViewOptions().menuCheckToggleTAZDrawFill->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;

            case MID_GNE_DATAVIEWOPTIONS_TAZRELONLYFROM:
                if (myViewNet->getDataViewOptions().menuCheckToggleTAZRelOnlyFrom->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_DATAVIEWOPTIONS_TAZRELONLYTO:
                if (myViewNet->getDataViewOptions().menuCheckToggleTAZRelOnlyTo->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            default:
                break;
        }
    }
    return 0;
}


long
GNEApplicationWindow::onCmdSaveNetwork(FXObject* sender, FXSelector sel, void* ptr) {
    auto& neteditOptions = OptionsCont::getOptions();
    // function onCmdSaveNetworkAs must be executed if this is the first save
    if (neteditOptions.getString("net-file").empty()) {
        return onCmdSaveNetworkAs(sender, sel, ptr);
    } else {
        // se net file in SUMO options
        mySUMOOptions.resetWritable();
        mySUMOOptions.set("net-file", neteditOptions.getString("net-file"));
        // begin save network
        getApp()->beginWaitCursor();
        try {
            // obtain invalid networkElements (currently only edges or crossings
            std::vector<GNENetworkElement*> invalidNetworkElements;
            // iterate over crossings and edges
            for (const auto& edge : myViewNet->getNet()->getAttributeCarriers()->getEdges()) {
                if (edge.second->isNetworkElementValid() == false) {
                    invalidNetworkElements.push_back(edge.second);
                }
            }
            for (const auto& crossing : myViewNet->getNet()->getAttributeCarriers()->getCrossings()) {
                if (crossing->isNetworkElementValid() == false) {
                    invalidNetworkElements.push_back(crossing);
                }
            }
            // if there are invalid network elements, open GNEFixNetworkElements
            if (invalidNetworkElements.size() > 0) {
                // 0 -> Canceled Saving, with or without selecting invalid network elements
                // 1 -> Invalid network elements fixed, friendlyPos enabled, or saved with invalid positions
                GNEFixNetworkElements fixNetworkElementsDialog(myViewNet, invalidNetworkElements);
                if (fixNetworkElementsDialog.execute() == 0) {
                    // show debug information
                    WRITE_DEBUG("network elements saving aborted");
                    // stop
                    return 1;
                } else {
                    // Save network
                    myNet->saveNetwork();
                    // show debug information
                    WRITE_DEBUG("network elements saved after dialog");
                }
            } else {
                // Save network
                myNet->saveNetwork();
                // show debug information
                WRITE_DEBUG("network elements saved");
            }
        } catch (IOError& e) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Opening FXMessageBox 'error saving network'");
            // open error message box
            FXMessageBox::error(this, MBOX_OK, TL("Saving Network failed!"), "%s", e.what());
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox 'error saving network' with 'OK'");
        }
        // write info
        WRITE_MESSAGE(TL("Network saved in '") + neteditOptions.getString("net-file") + "'.");
        // After saving a net successfully, add it into Recent Nets list.
        myMenuBarFile.myRecentNetworks.appendFile(neteditOptions.getString("net-file").c_str());
        myMessageWindow->addSeparator();
        // end save network
        getApp()->endWaitCursor();
        // update view
        myViewNet->updateViewNet();
        // set focus again in net
        myViewNet->setFocus();
        return 1;
    }
}


long
GNEApplicationWindow::onCmdSaveNetworkAs(FXObject*, FXSelector, void*) {
    // get network file file
    const auto networkFile = GNEApplicationWindowHelper::openNetworkFileDialog(this, true);
    if (networkFile.size() > 0) {
        // set ouput file in NETEDIT configs
        auto& neteditOptions = OptionsCont::getOptions();
        neteditOptions.resetWritable();
        neteditOptions.set("net-file", networkFile);
        // update NETEDIT title with the network name
        setTitle(MFXUtils::getTitleText(myTitlePrefix, networkFile.c_str()));
        // save network
        onCmdSaveNetwork(nullptr, 0, nullptr);
    }
    return 1;
}


long
GNEApplicationWindow::onCmdSavePlainXMLAs(FXObject*, FXSelector, void*) {
    // get neteditConfig filename
    auto plainXMLFile = GNEApplicationWindowHelper::savePlainXMLFileDialog(this);
    // Remove extension
    if (plainXMLFile.size() > 0) {
        plainXMLFile.pop_back();
    }
    // continue depending of file
    if (plainXMLFile.size() > 0) {
        // start saving plain XML
        getApp()->beginWaitCursor();
        try {
            myNet->savePlain(plainXMLFile);
        } catch (IOError& e) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Opening FXMessageBox 'Error saving plainXML'");
            // open message box
            FXMessageBox::error(this, MBOX_OK, TL("Saving plain xml failed!"), "%s", e.what());
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox 'Error saving plainXML' with 'OK'");
        }
        // end saving plain XML
        getApp()->endWaitCursor();
        // write info
        WRITE_MESSAGE(TL("Plain XML saved with prefix '") + plainXMLFile + "'");
        // restore focus
        setFocus();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdSaveJoinedJunctionsAs(FXObject*, FXSelector, void*) {
    // get neteditConfig filename
    const auto joinedJunctionsFile = GNEApplicationWindowHelper::saveJoinedJunctionsFileDialog(this);
    // continue depending of file
    if (joinedJunctionsFile.size() > 0) {
        getApp()->beginWaitCursor();
        try {
            myNet->saveJoined(joinedJunctionsFile);
        } catch (IOError& e) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Opening FXMessageBox 'error saving joined'");
            // opening error message
            FXMessageBox::error(this, MBOX_OK, TL("Saving joined junctions failed!"), "%s", e.what());
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox 'error saving joined' with 'OK'");
        }
        getApp()->endWaitCursor();
        // write info
        WRITE_MESSAGE(TL("Joined junctions saved to '") + joinedJunctionsFile + "'");
        // restore focus
        setFocus();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdSaveNETEDITConfig(FXObject*, FXSelector, void*) {
    // obtain NETEDIT option container
    auto& neteditOptions = OptionsCont::getOptions();
    neteditOptions.resetWritable();
    // Check if configuration file was already set at start of netedit or with a previous save
    if (neteditOptions.getString("configuration-file").empty()) {
        return onCmdSaveNETEDITConfigAs(nullptr, 0, nullptr);
    } else {
        const auto neteditConfigFile = neteditOptions.getString("configuration-file");
        // get config file without extension
        const auto patterFile = StringUtils::replace(neteditConfigFile, ".neteditcfg", "");
        // save all element giving automatic names
        if (!myNet->getSavingStatus()->isNetworkSaved()) {
            if (neteditOptions.getString("net-file").empty()) {
                neteditOptions.set("net-file", patterFile + ".net.xml");
            }
            onCmdSaveNetwork(nullptr, 0, nullptr);
        }
        if (!myNet->getSavingStatus()->isAdditionalsSaved()) {
            if (neteditOptions.getString("additional-files").empty()) {
                neteditOptions.set("additional-files", patterFile + ".add.xml");
            }
            onCmdSaveAdditionals(nullptr, 0, nullptr);
        }
        if (!myNet->getSavingStatus()->isDemandElementsSaved()) {
            if (neteditOptions.getString("route-files").empty()) {
                neteditOptions.set("route-files", patterFile + ".rou.xml");
            }
            onCmdSaveDemandElements(nullptr, 0, nullptr);
        }
        if (!myNet->getSavingStatus()->isDataElementsSaved()) {
            if (neteditOptions.getString("data-files").empty()) {
                neteditOptions.set("data-files", patterFile + ".dat.xml");
            }
            onCmdSaveDataElements(nullptr, 0, nullptr);
        }
        if (!myNet->getSavingStatus()->isMeanDatasSaved()) {
            if (neteditOptions.getString("meandata-files").empty()) {
                neteditOptions.set("meandata-files", patterFile + ".med.add.xml");
            }
            onCmdSaveMeanDatas(nullptr, 0, nullptr);
        }
        std::ofstream out(StringUtils::transcodeToLocal(neteditConfigFile));
        if (out.good()) {
            // write NETEDIT config
            neteditOptions.writeConfiguration(out, true, false, false, neteditConfigFile, true);
            // write info
            WRITE_MESSAGE(TL("NETEDIT configuration saved in '") + neteditConfigFile + "'");
            // config saved
            myNet->getSavingStatus()->NETEDITConfigSaved();
            // After saving a config successfully, add it into recent configs
            myMenuBarFile.myRecentConfigs.appendFile(neteditOptions.getString("configuration-file").c_str());
        } else {
            WRITE_MESSAGE(TL("Could not save NETEDIT configuration in '") + neteditConfigFile + "'");
        }
        out.close();
        return 1;
    }
}


long
GNEApplicationWindow::onCmdSaveNETEDITConfigAs(FXObject* sender, FXSelector sel, void* ptr) {
    auto& neteditOptions = OptionsCont::getOptions();
    // get neteditConfig filename
    const auto neteditConfigFile = GNEApplicationWindowHelper::openNETEDITConfigFileDialog(this, true);
    // continue depending of file
    if (!neteditConfigFile.empty()) {
        neteditOptions.resetWritable();
        neteditOptions.set("configuration-file", neteditConfigFile);
        // continue saving netedit config
        return onCmdSaveNETEDITConfig(sender, sel, ptr);
    } else {
        return 0;
    }
}


long
GNEApplicationWindow::onUpdSaveNETEDITConfig(FXObject* sender, FXSelector, void*) {
    // check if enable or disable save NETEDIT config button
    if (myNet == nullptr) {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else if (OptionsCont::getOptions().getString("configuration-file").empty()) {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else if (!myNet->getSavingStatus()->isNETEDITConfigSaved()) {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
    // check if eenable/disable save individual files
    if (myNet) {
        if (myNet->getSavingStatus()->isAdditionalsSaved() && myNet->getSavingStatus()->isDemandElementsSaved() && 
            myNet->getSavingStatus()->isDataElementsSaved() && myNet->getSavingStatus()->isMeanDatasSaved()) {
            myViewNet->getSaveElements().setSaveIndividualFiles(false);
        } else {
            myViewNet->getSaveElements().setSaveIndividualFiles(true);
        }
    }
    return 1;
}


long
GNEApplicationWindow::onCmdSaveSUMOConfig(FXObject* sender, FXSelector sel, void* ptr) {
    // obtain NETEDIT option container
    auto& neteditOptions = OptionsCont::getOptions();
    // reset containers
    neteditOptions.resetWritable();
    mySUMOOptions.resetWritable();
    // Check if configuration file was already set at start of netedit or with a previous save
    if (neteditOptions.getString("sumocfg-file").empty()) {
        return onCmdSaveSUMOConfigAs(sender, sel, ptr);
    } else {
        // check if ignore additionals and demand elements (only used open SUMO-GUI from NETEDIT)
        const FXSelector openSUMO = FXSEL(SEL_COMMAND, MID_HOTKEY_CTRL_T_OPENNETEDIT_OPENSUMO);
        const bool ignoreAdditionals = (sel == openSUMO)? (myEditMenuCommands.loadAdditionalsInSUMOGUI->getCheck() == FALSE) : false;
        const bool ignoreDemandElements = (sel == openSUMO)? (myEditMenuCommands.loadDemandInSUMOGUI->getCheck() == FALSE) : false;
        // get SUMOConfig file
        const auto sumoConfigFile = neteditOptions.getString("sumocfg-file");
        // get config file without extension
        const auto patterFile = StringUtils::replace(sumoConfigFile, ".sumocfg", "");
        // save all element giving automatic names
        if (!myNet->getSavingStatus()->isNetworkSaved()) {
            if (neteditOptions.getString("net-file").empty()) {
                neteditOptions.set("net-file", patterFile + ".net.xml");
            }
            onCmdSaveNetwork(nullptr, 0, nullptr);
        }
        if (!myNet->getSavingStatus()->isAdditionalsSaved()) {
            if (neteditOptions.getString("additional-files").empty()) {
                neteditOptions.set("additional-files", patterFile + ".add.xml");
            }
            onCmdSaveAdditionals(nullptr, 0, nullptr);
        }
        if (!myNet->getSavingStatus()->isDemandElementsSaved()) {
            if (neteditOptions.getString("route-files").empty()) {
                neteditOptions.set("route-files", patterFile + ".rou.xml");
            }
            onCmdSaveDemandElements(nullptr, 0, nullptr);
        }
        if (!myNet->getSavingStatus()->isMeanDatasSaved()) {
            if (neteditOptions.getString("meandata-files").empty()) {
                neteditOptions.set("meandata-files", patterFile + ".med.add.xml");
            }
            onCmdSaveMeanDatas(nullptr, 0, nullptr);
        }
        // set input in sumo options
        setInputInSUMOOptions(ignoreAdditionals, ignoreDemandElements);
        // if we have trips or flow over junctions, add option junction-taz
        if ((myNet->getAttributeCarriers()->getDemandElements().at(GNE_TAG_TRIP_JUNCTIONS).size() > 0) ||
                (myNet->getAttributeCarriers()->getDemandElements().at(GNE_TAG_FLOW_JUNCTIONS).size() > 0)) {
            mySUMOOptions.set("junction-taz", "true");
        }
        std::ofstream out(StringUtils::transcodeToLocal(sumoConfigFile));
        if (out.good()) {
            // write SUMO config
            mySUMOOptions.writeConfiguration(out, true, false, false, sumoConfigFile, true);
            // write info
            WRITE_MESSAGE(TL("SUMO configuration saved in '") + sumoConfigFile + "'");
            // if ignoreAdditionals or ignoreDemandElements is enabled, don't mark SUMOConfig as saved
            if (!ignoreAdditionals && !ignoreDemandElements) {
                myNet->getSavingStatus()->SUMOConfigSaved();
            }
            // After saving a config successfully, add it into recent configs
            myMenuBarFile.myRecentConfigs.appendFile(neteditOptions.getString("sumocfg-file").c_str());
        } else {
            WRITE_MESSAGE(TL("Could not save SUMO configuration in '") + sumoConfigFile + "'");
        }
        out.close();
        return 1;
    }
}


long
GNEApplicationWindow::onCmdSaveSUMOConfigAs(FXObject* sender, FXSelector sel, void* ptr) {
    auto& neteditOptions = OptionsCont::getOptions();
    // get sumoConfig filename
    const auto sumoConfigFile = GNEApplicationWindowHelper::openSUMOConfigFileDialog(this, true);
    // continue depending of file
    if (!sumoConfigFile.empty()) {
        // save file in NETEDIT options
        neteditOptions.resetWritable();
        neteditOptions.set("sumocfg-file", sumoConfigFile);
        // continue saving SUMO Config
        return onCmdSaveSUMOConfig(sender, sel, ptr);
    } else {
        return 0;
    }
}


long
GNEApplicationWindow::onUpdSaveSUMOConfig(FXObject* sender, FXSelector, void*) {
    if (myNet == nullptr) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else if (OptionsCont::getOptions().getString("sumocfg-file").empty()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else if (!myNet->getSavingStatus()->isSUMOConfigSaved()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
}


long
GNEApplicationWindow::onCmdSaveTLSPrograms(FXObject* obj, FXSelector sel, void* ptr) {
    // get option container
    auto& neteditOptions = OptionsCont::getOptions();
    // Check if TLS Programs file was already set at start of netedit or with a previous save
    if (neteditOptions.getString("tls-file").empty()) {
        return onCmdSaveTLSProgramsAs(obj, sel, ptr);
    } else {
        // Start saving TLS Programs
        getApp()->beginWaitCursor();
        try {
            myNet->computeNetwork(this, true); // GNEChange_TLS does not triggere GNENet:requireRecompute
            myNet->saveTLSPrograms(neteditOptions.getString("tls-file"));
            // write info
            WRITE_MESSAGE(TL("TLS Programs saved in '") + neteditOptions.getString("tls-file") + "'");
        } catch (IOError& e) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Opening FXMessageBox 'error saving TLS Programs'");
            // open error message box
            FXMessageBox::error(this, MBOX_OK, TL("Saving TLS Programs failed!"), "%s", e.what());
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox 'error saving TLS Programs' with 'OK'");
        }
        myMessageWindow->addSeparator();
        getApp()->endWaitCursor();
        // restore focus
        setFocus();
    }
    return 1;
}


long
GNEApplicationWindow::onUpdSaveTLSPrograms(FXObject* sender, FXSelector, void*) {
    if (myNet == nullptr) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else {
        // check if there is at least one TLS
        for (const auto &junction : myNet->getAttributeCarriers()->getJunctions()) {
            if (junction.second->getNBNode()->getControllingTLS().size() > 0) {
                return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
            }
        }
        // no TLS, then disable
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
}


long
GNEApplicationWindow::onCmdSaveEdgeTypes(FXObject* obj, FXSelector sel, void* ptr) {
    // get option container
    auto& neteditOptions = OptionsCont::getOptions();
    // Check if edgeType file was already set at start of netedit or with a previous save
    if (neteditOptions.getString("edgetypes-file").empty()) {
        return onCmdSaveTLSProgramsAs(obj, sel, ptr);
    } else {
        // Start saving edgeTypes
        getApp()->beginWaitCursor();
        try {
            myNet->saveEdgeTypes(neteditOptions.getString("edgetypes-file"));
            // write info
            WRITE_MESSAGE(TL("EdgeType saved in '") + neteditOptions.getString("edgetypes-file") + "'");
        } catch (IOError& e) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Opening FXMessageBox 'error saving edgeTypes'");
            // open error message box
            FXMessageBox::error(this, MBOX_OK, TL("Saving edgeTypes failed!"), "%s", e.what());
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox 'error saving edgeTypes' with 'OK'");
        }
        myMessageWindow->addSeparator();
        getApp()->endWaitCursor();
        // restore focus
        setFocus();
    }
    return 1;
}


long
GNEApplicationWindow::onUpdSaveEdgeTypes(FXObject* sender, FXSelector, void*) {
    // check if net exist and there are edge types
    if (myNet && (myNet->getAttributeCarriers()->getEdgeTypes().size() > 0)) {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
    return 1;
}


long
GNEApplicationWindow::onCmdSaveTLSProgramsAs(FXObject*, FXSelector, void*) {
    // get option container
    auto& neteditOptions = OptionsCont::getOptions();
    // get TLS file
    const auto TLSFile = GNEApplicationWindowHelper::openTLSFileDialog(this, true);
    // check tat file is valid
    if (TLSFile != "") {
        // change value of "tls-file"
        neteditOptions.resetWritable();
        neteditOptions.set("tls-file", TLSFile);
        // save TLS Programs
        return onCmdSaveTLSPrograms(nullptr, 0, nullptr);
    } else {
        return 1;
    }
}


long
GNEApplicationWindow::onCmdSaveEdgeTypesAs(FXObject*, FXSelector, void*) {
    // get option container
    auto& neteditOptions = OptionsCont::getOptions();
    // get network file file
    const auto edgeTypesFile = GNEApplicationWindowHelper::openEdgeTypeFileDialog(this, true);
    // check tat file is valid
    if (edgeTypesFile.size() > 0) {
        // change value of "edgetypes-file"
        neteditOptions.resetWritable();
        neteditOptions.set("edgetypes-file", edgeTypesFile);
        // save edgeTypes
        return onCmdSaveEdgeTypes(nullptr, 0, nullptr);
    } else {
        return 1;
    }
}


long
GNEApplicationWindow::onUpdSaveEdgeTypesAs(FXObject* sender, FXSelector, void*) {
    // check if net exist and there are edge types
    if (myNet && (myNet->getAttributeCarriers()->getEdgeTypes().size() > 0)) {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
    return 1;
}


long
GNEApplicationWindow::onCmdOpenAdditionals(FXObject*, FXSelector, void*) {
    auto &neteditOptions = OptionsCont::getOptions();
    // get file
    const auto additionalFile = GNEApplicationWindowHelper::openAdditionalFileDialog(this, false);
    // check file
    if (!additionalFile.empty()) {
        // declare overwrite flag
        bool overwriteElements = false;
        // check if open question dialog box
        if (additionalFile == neteditOptions.getString("additional-files")) {
            // open overwrite dialog
            GNEOverwriteElementsDialog overwriteDialog(this, "additional");
            // continue depending of result
            if (overwriteDialog.getResult() == GNEOverwriteElementsDialog::Result::CANCEL) {
                // abort load
                return 0;
            } else if (overwriteDialog.getResult() == GNEOverwriteElementsDialog::Result::OVERWRITE) {
                // enable overwriteElements
                overwriteElements = true;
            }
        }
        // flag for save current saving status
        const auto previouslySaved = myNet->getSavingStatus()->isAdditionalsSaved();
        // disable validation for additionals
        XMLSubSys::setValidation("never", "auto", "auto");
        // Create additional handler
        GNEGeneralHandler generalHandler(myNet, additionalFile, true, overwriteElements);
        // begin undoList operation
        myUndoList->begin(Supermode::NETWORK, GUIIcon::SUPERMODENETWORK, TL("load additionals from '") + additionalFile + "'");
        // Run parser
        if (!generalHandler.parse()) {
            // write error
            WRITE_ERROR(TL("Loading of additional file failed: ") + additionalFile);
        } else {
            // change value of "additional-files"
            neteditOptions.resetWritable();
            neteditOptions.set("additional-files", additionalFile);
            // write info
            WRITE_MESSAGE(TL("Loading of additional file sucessfully: ") + additionalFile);
            // enable save if there is errors loading additionals
            if (previouslySaved && !generalHandler.isErrorCreatingElement()) {
                myNet->getSavingStatus()->additionalsSaved();
            }
        }
        // end undoList operation
        myUndoList->end();
        // restore validation for additionals
        XMLSubSys::setValidation("auto", "auto", "auto");
        // update view
        update();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdReloadAdditionals(FXObject*, FXSelector, void*) {
    // get additionalFile
    const std::string additionalFile = OptionsCont::getOptions().getString("additional-files");
    // disable validation for additionals
    XMLSubSys::setValidation("never", "auto", "auto");
    // Create general handler
    GNEGeneralHandler generalHandler(myNet, additionalFile, true, true);
    // begin undoList operation
    myUndoList->begin(Supermode::DEMAND, GUIIcon::SUPERMODENETWORK, TL("reloading additionals from '") + additionalFile + "'");
    // clear additionals
    myNet->clearAdditionalElements(myUndoList);
    // Run parser
    if (!generalHandler.parse()) {
        WRITE_ERROR(TL("Reloading of additional file failed: ") + additionalFile);
    }
    // end undoList operation
    myUndoList->end();
    // restore validation for additionals
    XMLSubSys::setValidation("auto", "auto", "auto");
    // update view
    update();
    return 1;
}


long
GNEApplicationWindow::onUpdReloadAdditionals(FXObject* sender, FXSelector, void*) {
    // check if file exist
    if (myViewNet && OptionsCont::getOptions().getString("additional-files").empty()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    }
}


long
GNEApplicationWindow::onCmdSaveAdditionals(FXObject* sender, FXSelector sel, void* ptr) {
    // get option container
    auto& neteditOptions = OptionsCont::getOptions();
    // check saving conditions
    if (myNet->getSavingStatus()->isAdditionalsSaved()) {
        // nothing to save
        return 1;
    } else if (neteditOptions.getString("additional-files").empty()) {
        // choose file to save
        return onCmdSaveAdditionalsAs(sender, sel, ptr);
    } else {
        // Start saving additionals
        getApp()->beginWaitCursor();
        // compute before saving (for detectors positions)
        myNet->computeNetwork(this);
        // save additionals
        myNet->saveAdditionals();
        // show info
        WRITE_MESSAGE(TL("Additionals saved in '") + neteditOptions.getString("additional-files") + "'");
        // end saving additionals
        getApp()->endWaitCursor();
        // restore focus
        setFocus();
        return 1;
    }
}


long
GNEApplicationWindow::onCmdSaveAdditionalsAs(FXObject*, FXSelector, void*) {
    // get option container
    auto& neteditOptions = OptionsCont::getOptions();
    // declare current folder
    FXString currentFolder = gCurrentFolder;
    // set current folder
    if (neteditOptions.getString("configuration-file").size() > 0) {
        currentFolder = getFolder(neteditOptions.getString("configuration-file"));
    } else if (neteditOptions.getString("net-file").size() > 0) {
        currentFolder = getFolder(neteditOptions.getString("net-file"));
    }
    // get additional file
    const auto additionalFile = GNEApplicationWindowHelper::openAdditionalFileDialog(this, true);
    // check that file is valid
    if (additionalFile.size() > 0) {
        // reset writtable flag
        neteditOptions.resetWritable();
        // change value of "additional-files"
        neteditOptions.set("additional-files", additionalFile);
        // enable save additionals
        myNet->getSavingStatus()->requireSaveAdditionals();
        // save additionals
        return onCmdSaveAdditionals(nullptr, 0, nullptr);
    } else {
        return 1;
    }
}


long
GNEApplicationWindow::onCmdOpenDemandElements(FXObject*, FXSelector, void*) {
    // get option container
    auto& neteditOptions = OptionsCont::getOptions();
    // get file
    const auto routeFile = GNEApplicationWindowHelper::openRouteFileDialog(this, false);
    // check file
    if (!routeFile.empty()) {
        // declare overwrite flag
        bool overwriteElements = false;
        // check if open question dialog box
        if (routeFile == neteditOptions.getString("route-files")) {
            // open overwrite dialog
            GNEOverwriteElementsDialog overwriteDialog(this, "route");
            // continue depending of result
            if (overwriteDialog.getResult() == GNEOverwriteElementsDialog::Result::CANCEL) {
                // abort load
                return 0;
            } else if (overwriteDialog.getResult() == GNEOverwriteElementsDialog::Result::OVERWRITE) {
                // enable overwriteElements
                overwriteElements = true;
            }
        }
        // save previous demand element status saving
        const auto previouslySaved = myNet->getSavingStatus()->isDemandElementsSaved();
        // disable validation for additionals
        XMLSubSys::setValidation("never", "auto", "auto");
        // Create generic handler
        GNEGeneralHandler handler(myNet, routeFile, true, overwriteElements);
        // begin undoList operation
        myUndoList->begin(Supermode::DEMAND, GUIIcon::SUPERMODEDEMAND, TL("loading demand elements from '") + routeFile + "'");
        // Run parser for additionals
        if (!handler.parse()) {
            // write error
            WRITE_ERROR(TL("Loading of route file failed: ") + routeFile);
        } else {
            // change value of "route-files"
            neteditOptions.resetWritable();
            neteditOptions.set("route-files", routeFile);
            // show info
            WRITE_MESSAGE(TL("Loading of route file sucessfully: ") + routeFile);
            // enable demand elements if there is an error creating element
            if (previouslySaved && !handler.isErrorCreatingElement()) {
                myNet->getSavingStatus()->demandElementsSaved();
            }
        }
        // end undoList operation
        myUndoList->end();
        // restore validation
        XMLSubSys::setValidation("auto", "auto", "auto");
        // update view
        update();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdReloadDemandElements(FXObject*, FXSelector, void*) {
    // get file
    const std::string routeFile = OptionsCont::getOptions().getString("route-files");
    // disable validation for additionals
    XMLSubSys::setValidation("never", "auto", "auto");
    // Create handler
    GNEGeneralHandler handler(myNet, routeFile, true, true);
    // begin undoList operation
    myUndoList->begin(Supermode::DEMAND, GUIIcon::SUPERMODEDEMAND, "reloading demand elements from '" + routeFile + "'");
    // clear demand elements
    myNet->clearDemandElements(myUndoList);
    // Run parser for additionals
    if (!handler.parse()) {
        WRITE_ERROR(TL("Reloading of route file failed: ") + routeFile);
    }
    // end undoList operation and update view
    myUndoList->end();
    update();
    // restore validation for demand
    XMLSubSys::setValidation("auto", "auto", "auto");
    return 1;
}


long
GNEApplicationWindow::onUpdReloadDemandElements(FXObject* sender, FXSelector, void*) {
    // check if file exist
    if (myViewNet && OptionsCont::getOptions().getString("route-files").empty()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    }
}


long
GNEApplicationWindow::onCmdSaveDemandElements(FXObject* sender, FXSelector sel, void* ptr) {
    // get option container
    auto& neteditOptions = OptionsCont::getOptions();
    // check if requiere save demand elements
    if (myNet->getSavingStatus()->isDemandElementsSaved()) {
        // nothing to save
        return 1;
    } else if (neteditOptions.getString("route-files").empty()) {
        return onCmdSaveDemandElementsAs(sender, sel, ptr);
    } else {
        // Start saving demand elements
        getApp()->beginWaitCursor();
        // compute before saving
        myNet->computeNetwork(this);
        // save demand elements
        myNet->saveDemandElements();
        // show info
        WRITE_MESSAGE(TL("Demand elements saved in '") + neteditOptions.getString("route-files") + "'");
        // end saving demand elements
        getApp()->endWaitCursor();
        // restore focus
        setFocus();
        return 1;
    }
}


long
GNEApplicationWindow::onCmdSaveDemandElementsAs(FXObject* sender, FXSelector sel, void* ptr) {
    // get option container
    auto& neteditOptions = OptionsCont::getOptions();
    // declare current folder
    FXString currentFolder = gCurrentFolder;
    // set current folder
    if (neteditOptions.getString("configuration-file").size() > 0) {
        currentFolder = getFolder(neteditOptions.getString("configuration-file"));
    } else if (neteditOptions.getString("net-file").size() > 0) {
        currentFolder = getFolder(neteditOptions.getString("net-file"));
    }
    // get route file
    const auto routeFile = GNEApplicationWindowHelper::openRouteFileDialog(this, true);
    // check that file is correct
    if (routeFile != "") {
        // reset writtable flag
        neteditOptions.resetWritable();
        // change value of "route-files"
        neteditOptions.set("route-files", routeFile);
        // requiere save demand elements
        myNet->getSavingStatus()->requireSaveDemandElements();
        // save demand elements
        return onCmdSaveDemandElements(sender, sel, ptr);
    } else {
        return 1;
    }
}


long
GNEApplicationWindow::onCmdOpenDataElements(FXObject*, FXSelector, void*) {
    // get option container
    auto& neteditOptions = OptionsCont::getOptions();
    // get file
    const auto dataFile = GNEApplicationWindowHelper::openDataFileDialog(this, false);
    // check file
    if (dataFile.empty()) {
        // declare overwrite flag
        bool overwriteElements = false;
        // check if open question dialog box
        if (dataFile == neteditOptions.getString("data-files")) {
            // open overwrite dialog
            GNEOverwriteElementsDialog overwriteDialog(this, "data");
            // continue depending of result
            if (overwriteDialog.getResult() == GNEOverwriteElementsDialog::Result::CANCEL) {
                // abort load
                return 0;
            } else if (overwriteDialog.getResult() == GNEOverwriteElementsDialog::Result::OVERWRITE) {
                // enable overwriteElements
                overwriteElements = true;
            }
        }
        // save previous demand element status saving
        const auto previouslySaved = myNet->getSavingStatus()->isDataElementsSaved();
        // disable update data
        myViewNet->getNet()->disableUpdateData();
        // disable validation for data elements
        XMLSubSys::setValidation("never", "auto", "auto");
        // Create data handler
        GNEDataHandler dataHandler(myNet, dataFile, true, overwriteElements);
        // begin undoList operation
        myUndoList->begin(Supermode::DATA, GUIIcon::SUPERMODEDATA, TL("loading data elements from '") + dataFile + "'");
        // Run data parser
        if (!dataHandler.parse()) {
            // write error
            WRITE_ERROR(TL("Loading of data file failed: ") + dataFile);
        } else {
            // change value of "data-files"
            neteditOptions.resetWritable();
            neteditOptions.set("data-files", dataFile);
            // show info
            WRITE_MESSAGE(TL("Loading of data file sucessfully: ") + dataFile);
            // enable demand elements if there is an error creating element
            if (previouslySaved && !dataHandler.isErrorCreatingElement()) {
                myNet->getSavingStatus()->dataElementsSaved();
            }
        }
        // end undoList operation
        myUndoList->end();
        // enable update data
        myViewNet->getNet()->enableUpdateData();
        // restore validation for data
        XMLSubSys::setValidation("auto", "auto", "auto");
        // update
        update();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdReloadDataElements(FXObject*, FXSelector, void*) {
    // get file
    const std::string dataFile = OptionsCont::getOptions().getString("data-files");
    // disable update data
    myViewNet->getNet()->disableUpdateData();
    // disable validation for additionals
    XMLSubSys::setValidation("never", "auto", "auto");
    // Create additional handler
    GNEDataHandler dataHandler(myNet, dataFile, true, false);
    // begin undoList operation
    myUndoList->begin(Supermode::DATA, GUIIcon::SUPERMODEDATA, TL("reloading data elements from '") + dataFile + "'");
    // clear data elements
    myNet->clearDemandElements(myUndoList);
    // Run data parser
    if (!dataHandler.parse()) {
        WRITE_ERROR(TL("Reloading of data file failed: ") + dataFile);
    }
    // restore validation for data
    XMLSubSys::setValidation("auto", "auto", "auto");
    // end undoList operation and update view
    myUndoList->end();
    // enable update data
    myViewNet->getNet()->enableUpdateData();
    // update
    update();
    return 1;
}


long
GNEApplicationWindow::onUpdReloadDataElements(FXObject* sender, FXSelector, void*) {
    // check if file exist
    if (myViewNet && OptionsCont::getOptions().getString("data-files").empty()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    }
}


long
GNEApplicationWindow::onCmdSaveDataElements(FXObject* sender, FXSelector sel, void* ptr) {
    // get option container
    auto& neteditOptions = OptionsCont::getOptions();
    // check conditions
    if (myNet->getSavingStatus()->isDataElementsSaved()) {
        // nothing to save
        return 1;
    } else if (neteditOptions.getString("data-files").empty()) {
        return onCmdSaveDataElementsAs(sender, sel, ptr);
    } else {
        // Start saving data elements
        getApp()->beginWaitCursor();
        // save data elements
        myNet->saveDataElements();
        // write einfo
        WRITE_MESSAGE(TL("Data elements saved in '") + neteditOptions.getString("data-files") + "'");
        // end saving
        getApp()->endWaitCursor();
        // restore focus
        setFocus();
        return 1;
    }
}


long
GNEApplicationWindow::onCmdSaveDataElementsAs(FXObject* sender, FXSelector sel, void* ptr) {
    // get option container
    auto& neteditOptions = OptionsCont::getOptions();
    // declare current folder
    FXString currentFolder = gCurrentFolder;
    // set current folder
    if (neteditOptions.getString("configuration-file").size() > 0) {
        currentFolder = getFolder(neteditOptions.getString("configuration-file"));
    } else if (neteditOptions.getString("net-file").size() > 0) {
        currentFolder = getFolder(neteditOptions.getString("net-file"));
    }
    // get data file
    const auto dataFile = GNEApplicationWindowHelper::openDataFileDialog(this, true);
    // check that file is correct
    if (dataFile != "") {
        // reset writtable flag
        neteditOptions.resetWritable();
        // change value of "data-files"
        neteditOptions.set("data-files", dataFile);
        // mark data elements as unsaved
        myNet->getSavingStatus()->requireSaveDataElements();
        // save data elements
        return onCmdSaveDataElements(sender, sel, ptr);
    } else {
        return 1;
    }
}


long
GNEApplicationWindow::onCmdOpenMeanDatas(FXObject*, FXSelector, void*) {
    auto &neteditOptions = OptionsCont::getOptions();
    // get file
    const auto meanDataFile = GNEApplicationWindowHelper::openMeanDataDialog(this, false);
    // check file
    if (!meanDataFile.empty()) {
        // declare overwrite flag
        bool overwriteElements = false;
        // check if open question dialog box
        if (meanDataFile == neteditOptions.getString("meandata-files")) {
            // open overwrite dialog
            GNEOverwriteElementsDialog overwriteDialog(this, "meanData");
            // continue depending of result
            if (overwriteDialog.getResult() == GNEOverwriteElementsDialog::Result::CANCEL) {
                // abort load
                return 0;
            } else if (overwriteDialog.getResult() == GNEOverwriteElementsDialog::Result::OVERWRITE) {
                // enable overwriteElements
                overwriteElements = true;
            }
        }
        // save previous demand element status saving
        const auto previouslySaved = myNet->getSavingStatus()->isMeanDatasSaved();
        // disable validation for meanDatas
        XMLSubSys::setValidation("never", "auto", "auto");
        // Create meanData handler
        GNEGeneralHandler generalHandler(myNet, meanDataFile, true, overwriteElements);
        // begin undoList operation
        myUndoList->begin(Supermode::DATA, GUIIcon::SUPERMODEDATA, TL("load meanDatas from '") + meanDataFile + "'");
        // Run parser
        if (!generalHandler.parse()) {
            // write error
            WRITE_ERROR(TL("Loading of meandata file failed: ") + meanDataFile);
        } else {
            // change value of "meandata-files"
            neteditOptions.resetWritable();
            neteditOptions.set("meandata-files", meanDataFile);
            // show info
            WRITE_MESSAGE(TL("Loading of meandata file sucessfully: ") + meanDataFile);
            // enable demand elements if there is an error creating element
            if (previouslySaved && !generalHandler.isErrorCreatingElement()) {
                myNet->getSavingStatus()->meanDatasSaved();
            }
        }
        // end undoList operation
        myUndoList->end();        
        // restore validation for meanDatas
        XMLSubSys::setValidation("auto", "auto", "auto");
        // update view
        update();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdReloadMeanDatas(FXObject*, FXSelector, void*) {
    // get file
    const std::string meanDataFile = OptionsCont::getOptions().getString("meandata-files");
    // disable validation for meanDatas
    XMLSubSys::setValidation("never", "auto", "auto");
    // Create general handler
    GNEGeneralHandler generalHandler(myNet, meanDataFile, true, true);
    // begin undoList operation
    myUndoList->begin(Supermode::DEMAND, GUIIcon::SUPERMODENETWORK, TL("reloading meanDatas from '") + meanDataFile + "'");
    // clear meanDatas
    myNet->clearMeanDataElements(myUndoList);
    // Run parser
    if (!generalHandler.parse()) {
        WRITE_MESSAGE(TL("Loading of meandata file sucessfully: ") + meanDataFile);
    }
    // end undoList operation and update view
    myUndoList->end();
    update();
    // restore validation for meanDatas
    XMLSubSys::setValidation("auto", "auto", "auto");
    return 1;
}


long
GNEApplicationWindow::onUpdReloadMeanDatas(FXObject* sender, FXSelector, void*) {
    // check if file exist
    if (myViewNet && OptionsCont::getOptions().getString("meandata-files").empty()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    }
}


long
GNEApplicationWindow::onCmdSaveMeanDatas(FXObject* sender, FXSelector sel, void* ptr) {
    // get option container
    auto& neteditOptions = OptionsCont::getOptions();
    // check conditions
    if (myNet->getSavingStatus()->isMeanDatasSaved()) {
        // nothing to save
        return 1;
    } else if (neteditOptions.getString("meandata-files").empty()) {
        return onCmdSaveMeanDatasAs(sender, sel, ptr);
    } else {
        // Start saving meanDatas
        getApp()->beginWaitCursor();
        // save mean datas
        myNet->saveMeanDatas();
        // write info
        WRITE_MESSAGE(TL("MeanDatas saved in '") + neteditOptions.getString("meandata-files") + "'");
        // end saving
        getApp()->endWaitCursor();
        // restore focus
        setFocus();
        return 1;
    }
}


long
GNEApplicationWindow::onCmdSaveMeanDatasAs(FXObject* sender, FXSelector sel, void* ptr) {
    // get option container
    auto& neteditOptions = OptionsCont::getOptions();
    // declare current folder
    FXString currentFolder = gCurrentFolder;
    // set current folder
    if (neteditOptions.getString("configuration-file").size() > 0) {
        currentFolder = getFolder(neteditOptions.getString("configuration-file"));
    } else if (neteditOptions.getString("net-file").size() > 0) {
        currentFolder = getFolder(neteditOptions.getString("net-file"));
    }
    // get meanData file
    const auto meanDataFile = GNEApplicationWindowHelper::openMeanDataDialog(this, true);
    // check that file is valid
    if (meanDataFile != "") {
        // reset writtable flag
        neteditOptions.resetWritable();
        // change value of "meandata-files"
        neteditOptions.set("meandata-files", meanDataFile);
        // mark mean datas as unsaved
        myNet->getSavingStatus()->requireSaveMeanDatas();
        // save meanDatas
        return onCmdSaveMeanDatas(sender, sel, ptr);
    } else {
        return 1;
    }
}


bool
GNEApplicationWindow::continueWithUnsavedChanges(const std::string& operation) {
    FXuint answer = 0;
    if (myNet && !myNet->getSavingStatus()->isNetworkSaved()) {
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Opening FXMessageBox 'Confirm " + operation + " network'");
        // open question box
        answer = GUISaveDialog::question(getApp(), ("Confirm " + operation + " Network").c_str(), "%s",
                                         ("You have unsaved changes in the network.\nDo you wish to " + operation + " and discard all changes?").c_str());
        // restore focus to view net
        myViewNet->setFocus();
        // if user close dialog box, check additionals and demand elements
        if (answer == GUISaveDialog::CLICKED_DISCARD) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox 'Confirm " + operation + " network' with 'Quit'");
            if (continueWithUnsavedAdditionalChanges(operation) &&
                    continueWithUnsavedDemandElementChanges(operation) &&
                    continueWithUnsavedDataElementChanges(operation)) {
                // clear undo list
                clearUndoList();
                return true;
            } else {
                return false;
            }
        } else if (answer == GUISaveDialog::CLICKED_SAVE) {
            // save network
            onCmdSaveNetwork(nullptr, 0, nullptr);
            // check
            if (continueWithUnsavedAdditionalChanges(operation) &&
                    continueWithUnsavedDemandElementChanges(operation) &&
                    continueWithUnsavedDataElementChanges(operation)) {
                // clear undo list
                clearUndoList();
                return true;
            } else {
                return false;
            }
        } else {
            // write warning if netedit is running in testing mode
            if (answer == 2) {
                WRITE_DEBUG("Closed FXMessageBox 'Confirm " + operation + " network' with 'No'");
            } else if (answer == 4) {
                WRITE_DEBUG("Closed FXMessageBox 'Confirm " + operation + " network' with 'ESC'");
            }
            // return false to stop closing/reloading
            return false;
        }
    } else {
        if (continueWithUnsavedAdditionalChanges(operation) &&
                continueWithUnsavedDemandElementChanges(operation) &&
                continueWithUnsavedDataElementChanges(operation)) {
            // clear undo list
            clearUndoList();
            return true;
        } else {
            // return false to stop closing/reloading
            return false;
        }
    }
}


bool
GNEApplicationWindow::continueWithUnsavedAdditionalChanges(const std::string& operation) {
    // Check if there are non saved additionals
    if (myNet && !myNet->getSavingStatus()->isAdditionalsSaved()) {
        WRITE_DEBUG("Opening FXMessageBox 'Save additionals before " + operation + "'");
        // open question box
        FXuint answer = GUISaveDialog::question(getApp(),
                                                ("Save additionals before " + operation).c_str(), "%s",
                                                ("You have unsaved additionals. Do you wish to " + operation + " and discard all changes?").c_str());
        // restore focus to view net
        myViewNet->setFocus();
        // if answer was affirmative, but there was an error during saving additionals, return false to stop closing/reloading
        if (answer == GUISaveDialog::CLICKED_DISCARD) {
            WRITE_DEBUG("Closed FXMessageBox 'Save additionals before " + operation + "' with 'Quit'");
            // nothing to save, return true
            return true;
        } else if (answer == GUISaveDialog::CLICKED_SAVE) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox 'Save additionals before " + operation + "' with 'Yes'");
            if (onCmdSaveAdditionals(nullptr, 0, nullptr) == 1) {
                // additionals successfully saved
                return true;
            } else {
                // error saving additionals, abort saving
                return false;
            }
        } else {
            // write warning if netedit is running in testing mode
            if (answer == 2) {
                WRITE_DEBUG("Closed FXMessageBox 'Save additionals before " + operation + "' with 'No'");
            } else if (answer == 4) {
                WRITE_DEBUG("Closed FXMessageBox 'Save additionals before " + operation + "' with 'ESC'");
            }
            // abort saving
            return false;
        }
    } else {
        // nothing to save, return true
        return true;
    }
}


bool
GNEApplicationWindow::continueWithUnsavedDemandElementChanges(const std::string& operation) {
    // Check if there are non saved demand elements
    if (myNet && !myNet->getSavingStatus()->isDemandElementsSaved()) {
        WRITE_DEBUG("Opening FXMessageBox 'Save demand elements before " + operation + "'");
        // open question box
        FXuint answer = GUISaveDialog::question(getApp(), ("Save demand elements before " + operation).c_str(), "%s",
                                                ("You have unsaved demand elements.\nDo you wish to " + operation + " and discard all changes?").c_str());
        // restore focus to view net
        myViewNet->setFocus();
        // if answer was affirmative, but there was an error during saving demand elements, return false to stop closing/reloading
        if (answer == GUISaveDialog::CLICKED_DISCARD) {
            WRITE_DEBUG("Closed FXMessageBox 'Save demand elements before " + operation + "' with 'Quit'");
            // nothing to save, return true
            return true;
        } else if (answer == GUISaveDialog::CLICKED_SAVE) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox 'Save demand elements before " + operation + "' with 'Yes'");
            if (onCmdSaveDemandElements(nullptr, 0, nullptr) == 1) {
                // demand elements successfully saved
                return true;
            } else {
                // error saving demand elements, abort saving
                return false;
            }
        } else {
            // write warning if netedit is running in testing mode
            if (answer == 2) {
                WRITE_DEBUG("Closed FXMessageBox 'Save demand elements before " + operation + "' with 'No'");
            } else if (answer == 4) {
                WRITE_DEBUG("Closed FXMessageBox 'Save demand elements before " + operation + "' with 'ESC'");
            }
            // abort saving
            return false;
        }
    } else {
        // nothing to save, return true
        return true;
    }
}


bool
GNEApplicationWindow::continueWithUnsavedDataElementChanges(const std::string& operation) {
    // Check if there are non saved data elements
    if (myNet && !myNet->getSavingStatus()->isDataElementsSaved()) {
        WRITE_DEBUG("Opening FXMessageBox 'Save data elements before " + operation + "'");
        // open question box
        FXuint answer = GUISaveDialog::question(getApp(), ("Save data elements before " + operation).c_str(), "%s",
                                                ("You have unsaved data elements.\nDo you wish to " + operation + " and discard all changes?").c_str());
        // restore focus to view net
        myViewNet->setFocus();
        // if answer was affirmative, but there was an error during saving data elements, return false to stop closing/reloading
        if (answer == GUISaveDialog::CLICKED_DISCARD) {
            WRITE_DEBUG("Closed FXMessageBox 'Save data elements before " + operation + "' with 'Quit'");
            // nothing to save, return true
            return true;
        } else if (answer == GUISaveDialog::CLICKED_SAVE) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox 'Save data elements before " + operation + "' with 'Yes'");
            if (onCmdSaveDataElements(nullptr, 0, nullptr) == 1) {
                // data elements successfully saved
                return true;
            } else {
                // error saving data elements, abort saving
                return false;
            }
        } else {
            // write warning if netedit is running in testing mode
            if (answer == 2) {
                WRITE_DEBUG("Closed FXMessageBox 'Save data elements before " + operation + "' with 'No'");
            } else if (answer == 4) {
                WRITE_DEBUG("Closed FXMessageBox 'Save data elements before " + operation + "' with 'ESC'");
            }
            // abort saving
            return false;
        }
    } else {
        // nothing to save, return true
        return true;
    }
}


void
GNEApplicationWindow::setInputInSUMOOptions(const bool ignoreAdditionals, const bool ignoreRoutes) {
    // obtain NETEDIT option container
    auto& neteditOptions = OptionsCont::getOptions();
    mySUMOOptions.resetWritable();
    // set network
    mySUMOOptions.set("net-file", neteditOptions.getString("net-file"));
    // set routes
    if (ignoreRoutes) {
        mySUMOOptions.set("route-files", "");
    } else {
        mySUMOOptions.set("route-files", neteditOptions.getString("route-files"));
    }
    // set SUMOOptions depending of additionalFiles and meanData files
    if (ignoreAdditionals) {
        mySUMOOptions.set("additional-files", neteditOptions.getString("meandata-files"));
    } else {
        if ((neteditOptions.getString("additional-files").size() > 0) && (neteditOptions.getString("meandata-files").size())) {
            mySUMOOptions.set("additional-files", neteditOptions.getString("additional-files") + "," + neteditOptions.getString("meandata-files"));
        } else if (neteditOptions.getString("additional-files").size() > 0) {
            mySUMOOptions.set("additional-files", neteditOptions.getString("additional-files"));
        } else if (neteditOptions.getString("meandata-files").size() > 0) {
            mySUMOOptions.set("additional-files", neteditOptions.getString("meandata-files"));
        } else {
            mySUMOOptions.set("additional-files", "");
        }
    }
}


FXString
GNEApplicationWindow::getFolder(const std::string& folder) const {
    // declare folder
    std::string newFolder = folder;
    // declare stop flag
    bool stop = false;
    // continue while stop is false
    while (!stop) {
        if (newFolder.empty()) {
            // new folder empty, then stop
            stop = true;
        } else if ((newFolder.back() == '\'') || (newFolder.back() == '\\') ||
                   (newFolder.back() == '/') /* || (newFolder.back() == '//') */) {
            // removed file, then stop
            stop = true;
        } else {
            newFolder.pop_back();
        }
    }
    // if is empty, return gCurrentFolder
    if (newFolder.empty()) {
        return gCurrentFolder;
    }
    return FXString(newFolder.c_str());
}


void
GNEApplicationWindow::updateControls() {
    // check that view exists
    if (myViewNet) {
        myViewNet->updateControls();
    }
}


void
GNEApplicationWindow::updateSuperModeMenuCommands(const Supermode supermode) {
    // remove lock hotkeys
    myLockMenuCommands.removeHotkeys();
    // check supermode
    if (supermode == Supermode::NETWORK) {
        // menu commands
        myModesMenuCommands.networkMenuCommands.showNetworkMenuCommands();
        myModesMenuCommands.demandMenuCommands.hideDemandMenuCommands();
        myModesMenuCommands.dataMenuCommands.hideDataMenuCommands();
        // lock
        myLockMenuCommands.showNetworkLockMenuCommands();
        myLockMenuCommands.hideDemandLockMenuCommands();
        myLockMenuCommands.hideDataLockMenuCommands();
        // processing
        myProcessingMenuCommands.showNetworkProcessingMenuCommands();
        myProcessingMenuCommands.hideDemandProcessingMenuCommands();
        myProcessingMenuCommands.hideDataProcessingMenuCommands();
        myProcessingMenuCommands.showSeparator();
    } else if (supermode == Supermode::DEMAND) {
        // menu commands
        myModesMenuCommands.networkMenuCommands.hideNetworkMenuCommands();
        myModesMenuCommands.demandMenuCommands.showDemandMenuCommands();
        myModesMenuCommands.dataMenuCommands.hideDataMenuCommands();
        // lock
        myLockMenuCommands.hideNetworkLockMenuCommands();
        myLockMenuCommands.showDemandLockMenuCommands();
        myLockMenuCommands.hideDataLockMenuCommands();
        // processing
        myProcessingMenuCommands.hideNetworkProcessingMenuCommands();
        myProcessingMenuCommands.showDemandProcessingMenuCommands();
        myProcessingMenuCommands.hideDataProcessingMenuCommands();
        myProcessingMenuCommands.showSeparator();
    } else if (supermode == Supermode::DATA) {
        // menu commands
        myModesMenuCommands.networkMenuCommands.hideNetworkMenuCommands();
        myModesMenuCommands.demandMenuCommands.hideDemandMenuCommands();
        myModesMenuCommands.dataMenuCommands.showDataMenuCommands();
        // lock
        myLockMenuCommands.hideNetworkLockMenuCommands();
        myLockMenuCommands.hideDemandLockMenuCommands();
        myLockMenuCommands.showDataLockMenuCommands();
        // processing
        myProcessingMenuCommands.hideNetworkProcessingMenuCommands();
        myProcessingMenuCommands.hideDemandProcessingMenuCommands();
        myProcessingMenuCommands.showDataProcessingMenuCommands();
        myProcessingMenuCommands.hideSeparator();
    } else {
        // menu commands
        myModesMenuCommands.networkMenuCommands.hideNetworkMenuCommands();
        myModesMenuCommands.demandMenuCommands.hideDemandMenuCommands();
        myModesMenuCommands.dataMenuCommands.hideDataMenuCommands();
        myProcessingMenuCommands.hideSeparator();
        // lock
        myLockMenuCommands.hideNetworkLockMenuCommands();
        myLockMenuCommands.hideDemandLockMenuCommands();
        myLockMenuCommands.hideDataLockMenuCommands();
        // processing
        myProcessingMenuCommands.hideNetworkProcessingMenuCommands();
        myProcessingMenuCommands.hideDemandProcessingMenuCommands();
        myProcessingMenuCommands.hideDataProcessingMenuCommands();
    }
}


void
GNEApplicationWindow::disableUndoRedo(const std::string& reason) {
    myUndoRedoListEnabled = reason;
}


void
GNEApplicationWindow::enableUndoRedo() {
    myUndoRedoListEnabled.clear();
}


const std::string&
GNEApplicationWindow::isUndoRedoEnabled() const {
    return myUndoRedoListEnabled;
}


GNEApplicationWindowHelper::EditMenuCommands&
GNEApplicationWindow::getEditMenuCommands() {
    return myEditMenuCommands;
}


GNEApplicationWindowHelper::LockMenuCommands&
GNEApplicationWindow::getLockMenuCommands() {
    return myLockMenuCommands;
}


void
GNEApplicationWindow::clearUndoList() {
    if (myViewNet) {
        // destroy Popup (to avoid crashes)
        myViewNet->destroyPopup();
    }
    // clear undo list and return true to continue with closing/reload
    myUndoList->clear();
}


const GNEApplicationWindowHelper::ProcessingMenuCommands&
GNEApplicationWindow::getProcessingMenuCommands() const {
    return myProcessingMenuCommands;
}


OptionsCont&
GNEApplicationWindow::getSUMOOptions() {
    return mySUMOOptions;
}


void
GNEApplicationWindow::loadElements() {
    // get option container
    auto& neteditOptions = OptionsCont::getOptions();
    // get additional files
    StringVector files;
    for (const auto &additionalFiles : neteditOptions.getStringVector("additional-files")) {
        files.push_back(additionalFiles);
    }
    for (const auto &routeFiles : neteditOptions.getStringVector("route-files")) {
        files.push_back(routeFiles);
    }
    for (const auto &meanDataFiles : neteditOptions.getStringVector("meandata-files")) {
        files.push_back(meanDataFiles);
    }
    // clear options
    neteditOptions.resetWritable();
    neteditOptions.set("additional-files", "");
    neteditOptions.set("route-files", "");
    neteditOptions.set("meandata-files", "");
    // continue depending of network and additional files
    if (myNet && (files.size() > 0)) {
        // begin undolist
        myUndoList->begin(Supermode::NETWORK, GUIIcon::SUPERMODENETWORK, "loading elements from '" + toString(files) + "'");
        // iterate over every additional file
        for (const auto& file : files) {
            WRITE_MESSAGE(TL("loading additionals and meandatas from '") + file + "'");
            // declare general handler
            GNEGeneralHandler handler(myNet, file, true, false);
            // disable validation for additionals
            XMLSubSys::setValidation("never", "auto", "auto");
            // Run parser
            if (!handler.parse()) {
                WRITE_ERROR("Loading of " + file + " failed.");
            }
            // set netedit options
            neteditOptions.resetWritable();
            if (handler.isAdditionalFile() && neteditOptions.getString("additional-files").empty()) {
                neteditOptions.set("additional-files", file);
            } else if (handler.isRouteFile() && neteditOptions.getString("route-files").empty()) {
                neteditOptions.set("route-files", file);
            } else if (handler.isMeanDataFile() && neteditOptions.getString("meandata-files").empty()) {
                neteditOptions.set("meandata-files", file);
            }
            // set additionals in SUMOConfig
            setInputInSUMOOptions(false, false);
            // disable validation for additionals
            XMLSubSys::setValidation("auto", "auto", "auto");
        }
        // end undo list
        myUndoList->end();
    }
}


void
GNEApplicationWindow::loadDataElements() {
    // get option container
    auto& neteditOptions = OptionsCont::getOptions();
    // get data files
    const auto dataFiles = neteditOptions.getStringVector("data-files");
    // continue depending of network and data files
    if (myNet && (dataFiles.size() > 0)) {
        // disable update data
        myViewNet->getNet()->disableUpdateData();
        // begin undolist
        myUndoList->begin(Supermode::DATA, GUIIcon::SUPERMODEDATA, "loading data elements from '" + toString(dataFiles) + "'");
        // iterate over every data file
        for (const auto& dataFile : dataFiles) {
            WRITE_MESSAGE(TL("Loading data elements from '") + dataFile + "'");
            GNEDataHandler dataHandler(myNet, dataFile, true, false);
            // disable validation for data elements
            XMLSubSys::setValidation("never", "auto", "auto");
            if (!dataHandler.parse()) {
                WRITE_ERROR("Loading of " + dataFile + " failed.");
            }
            // set first dataElementsFiles as default file
            neteditOptions.resetWritable();
            neteditOptions.set("data-files", dataFile);
            // disable validation for data elements
            XMLSubSys::setValidation("auto", "auto", "auto");
        }
        // end undolist
        myUndoList->end();
        // enable update data
        myViewNet->getNet()->enableUpdateData();
    }
}

// ---------------------------------------------------------------------------
// GNEApplicationWindow - protected methods
// ---------------------------------------------------------------------------

GNEApplicationWindow::GNEApplicationWindow() :
    myToolbarsGrip(this),
    myMenuBarFile(this),
    myFileMenuCommands(this),
    myModesMenuCommands(this),
    myEditMenuCommands(this),
    myLockMenuCommands(nullptr),
    myProcessingMenuCommands(this),
    myLocateMenuCommands(this),
    myToolsMenuCommands(this),
    myWindowsMenuCommands(this),
    mySupermodeCommands(this) {
}


long
GNEApplicationWindow::onKeyPress(FXObject* o, FXSelector sel, void* eventData) {
    const long handled = FXMainWindow::onKeyPress(o, sel, eventData);
    if (handled == 0 && myMDIClient->numChildren() > 0) {
        GNEViewParent* w = dynamic_cast<GNEViewParent*>(myMDIClient->getActiveChild());
        if (w != nullptr) {
            w->onKeyPress(nullptr, sel, eventData);
        }
    }
    return 0;
}


long
GNEApplicationWindow::onKeyRelease(FXObject* o, FXSelector sel, void* eventData) {
    const long handled = FXMainWindow::onKeyRelease(o, sel, eventData);
    if (handled == 0 && myMDIClient->numChildren() > 0) {
        GNEViewParent* w = dynamic_cast<GNEViewParent*>(myMDIClient->getActiveChild());
        if (w != nullptr) {
            w->onKeyRelease(nullptr, sel, eventData);
        }
    }
    return 0;
}


/****************************************************************************/
