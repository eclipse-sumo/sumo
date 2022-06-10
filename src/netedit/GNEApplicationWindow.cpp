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
/// @file    GNEApplicationWindow.cpp
/// @author  Pablo Alvarez Lopez
/// @date    mar 2020
///
// Functions from main window of NETEDIT
/****************************************************************************/
#include <netbuild/NBFrame.h>
#include <netedit/dialogs/GNEAbout.h>
#include <netedit/dialogs/GNEUndoListDialog.h>
#include <netedit/elements/network/GNEEdgeType.h>
#include <netedit/elements/network/GNELaneType.h>
#include <netedit/elements/GNEGeneralHandler.h>
#include <netedit/elements/data/GNEDataHandler.h>
#include <netedit/elements/GNEGeneralHandler.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <netedit/frames/common/GNESelectorFrame.h>
#include <netedit/frames/network/GNECreateEdgeFrame.h>
#include <netedit/frames/network/GNETAZFrame.h>
#include <netedit/frames/network/GNETLSEditorFrame.h>
#include <netedit/changes/GNEChange_EdgeType.h>
#include <netimport/NIFrame.h>
#include <netimport/NIXMLTypesHandler.h>
#include <netimport/NITypeLoader.h>
#include <netwrite/NWFrame.h>
#include <utils/common/SystemFrame.h>
#include <utils/foxtools/FXLinkLabel.h>
#include <utils/gui/cursors/GUICursorSubSys.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIDialog_GLChosenEditor.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIUserIO.h>
#include <utils/gui/events/GUIEvent_Message.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include <utils/gui/settings/GUISettingsHandler.h>
#include <utils/gui/shortcuts/GUIShortcutsSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIDialog_Options.h>
#include <utils/gui/windows/GUIPerspectiveChanger.h>
#include <utils/options/OptionsCont.h>
#include <utils/foxtools/FXMenuCheckIcon.h>
#include <utils/xml/XMLSubSys.h>

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
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_SHIFT_N_NEWWINDOW,                  GNEApplicationWindow::onCmdNewWindow),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_N_NEWNETWORK,                       GNEApplicationWindow::onCmdNewNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_N_NEWNETWORK,                       GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_OPEN_NETWORK,                                   GNEApplicationWindow::onCmdOpenNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_OPEN_NETWORK,                                   GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_OPEN_CONFIG,                                    GNEApplicationWindow::onCmdOpenConfiguration),
    FXMAPFUNC(SEL_UPDATE,   MID_OPEN_CONFIG,                                    GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_OPENFOREIGN,                    GNEApplicationWindow::onCmdOpenForeign),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_OPENFOREIGN,                    GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_RECENTFILE,                                     GNEApplicationWindow::onCmdOpenRecent),
    FXMAPFUNC(SEL_UPDATE,   MID_RECENTFILE,                                     GNEApplicationWindow::onUpdOpen),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_R_RELOAD,                           GNEApplicationWindow::onCmdReload),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_R_RELOAD,                           GNEApplicationWindow::onUpdReload),
    // network
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SAVEALLELEMENTS,                            GNEApplicationWindow::onCmdSaveAllElements),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_SAVEALLELEMENTS,                            GNEApplicationWindow::onUpdSaveNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_S_STOPSIMULATION_SAVENETWORK,       GNEApplicationWindow::onCmdSaveNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_S_STOPSIMULATION_SAVENETWORK,       GNEApplicationWindow::onUpdSaveNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_SHIFT_S_SAVENETWORK_AS,             GNEApplicationWindow::onCmdSaveAsNetwork),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_SHIFT_S_SAVENETWORK_AS,             GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_L_SAVEASPLAINXML,                   GNEApplicationWindow::onCmdSaveAsPlainXML),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_L_SAVEASPLAINXML,                   GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SAVEJOINEDJUNCTIONS,                        GNEApplicationWindow::onCmdSaveJoined),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_SAVEJOINEDJUNCTIONS,                        GNEApplicationWindow::onUpdNeedsNetwork),
    // TLS
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_K_OPENTLSPROGRAMS,                  GNEApplicationWindow::onCmdOpenTLSPrograms),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_K_OPENTLSPROGRAMS,                  GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_RELOAD_TLSPROGRAMS,             GNEApplicationWindow::onCmdReloadTLSPrograms),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_RELOAD_TLSPROGRAMS,             GNEApplicationWindow::onUpdReloadTLSPrograms),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_SHIFT_K_SAVETLS,                    GNEApplicationWindow::onCmdSaveTLSPrograms),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_SHIFT_K_SAVETLS,                    GNEApplicationWindow::onUpdSaveTLSPrograms),
    // edge types
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_H_OPENEDGETYPES,                    GNEApplicationWindow::onCmdOpenEdgeTypes),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_H_OPENEDGETYPES,                    GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_RELOAD_EDGETYPES,               GNEApplicationWindow::onCmdReloadEdgeTypes),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_RELOAD_EDGETYPES,               GNEApplicationWindow::onUpdReloadEdgeTypes),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_SHIFT_H_SAVEEDGETYPES,              GNEApplicationWindow::onCmdSaveEdgeTypes),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_SHIFT_H_SAVEEDGETYPES,              GNEApplicationWindow::onUpdSaveEdgeTypes),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVEEDGETYPES_AS,               GNEApplicationWindow::onCmdSaveEdgeTypesAs),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_SAVEEDGETYPES_AS,               GNEApplicationWindow::onUpdSaveEdgeTypesAs),
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
    // other
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARFILE_SAVETLSPROGRAMS_AS,             GNEApplicationWindow::onCmdSaveTLSProgramsAs),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARFILE_SAVETLSPROGRAMS_AS,             GNEApplicationWindow::onUpdSaveTLSPrograms),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_W_CLOSESIMULATION,                  GNEApplicationWindow::onCmdClose),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_W_CLOSESIMULATION,                  GNEApplicationWindow::onUpdNeedsNetwork),

    // Toolbar supermode
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F2_SUPERMODE_NETWORK,    GNEApplicationWindow::onCmdSetSuperMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F3_SUPERMODE_DEMAND,     GNEApplicationWindow::onCmdSetSuperMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F4_SUPERMODE_DATA,       GNEApplicationWindow::onCmdSetSuperMode),

    // Toolbar modes
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_A_MODE_ADDITIONAL_STOP,              GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_C_MODE_CONNECT_PERSONPLAN,           GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_D_MODE_DELETE,                       GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_E_MODE_EDGE_EDGEDATA,                GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_G_MODE_CONTAINER,                    GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_H_MODE_PROHIBITION_CONTAINERPLAN,    GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_I_MODE_INSPECT,                      GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_M_MODE_MOVE,                         GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_P_MODE_POLYGON_PERSON,               GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_R_MODE_CROSSING_ROUTE_EDGERELDATA,   GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_S_MODE_SELECT,                       GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_T_MODE_TLS_TYPE,                     GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_V_MODE_VEHICLE,                      GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_W_MODE_WIRE,                         GNEApplicationWindow::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_Z_MODE_TAZ_TAZREL,                   GNEApplicationWindow::onCmdSetMode),

    // Toolbar edit
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_Z_UNDO,                         GNEApplicationWindow::onCmdUndo),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_Z_UNDO,                         GNEApplicationWindow::onUpdUndo),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_Y_REDO,                         GNEApplicationWindow::onCmdRedo),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_Y_REDO,                         GNEApplicationWindow::onUpdRedo),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_UNDOLISTDIALOG,                         GNEApplicationWindow::onCmdOpenUndoListDialog),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_UNDOLISTDIALOG,                         GNEApplicationWindow::onUpdOpenUndoListDialog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBAREDIT_COMPUTEPATHMANAGER,         GNEApplicationWindow::onCmdComputePathManager),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBAREDIT_COMPUTEPATHMANAGER,         GNEApplicationWindow::onUpdComputePathManager),
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
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F9_EDIT_VIEWSCHEME,                  GNEApplicationWindow::onCmdEditViewScheme),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_F9_EDIT_VIEWSCHEME,                  GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_I_EDITVIEWPORT,                 GNEApplicationWindow::onCmdEditViewport),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_I_EDITVIEWPORT,                 GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_G_GAMINGMODE_TOGGLEGRID,        GNEApplicationWindow::onCmdToggleGrid),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_G_GAMINGMODE_TOGGLEGRID,        GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_J_TOOGLEDRAWJUNCTIONSHAPE,      GNEApplicationWindow::onCmdToggleDrawJunctionShape),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_J_TOOGLEDRAWJUNCTIONSHAPE,      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F11_FRONTELEMENT,                    GNEApplicationWindow::onCmdSetFrontElement),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_F11_FRONTELEMENT,                    GNEApplicationWindow::onUpdNeedsFrontElement),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBAREDIT_LOADADDITIONALS,            GNEApplicationWindow::onCmdLoadAdditionalsInSUMOGUI),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBAREDIT_LOADADDITIONALS,            GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBAREDIT_LOADDEMAND,                 GNEApplicationWindow::onCmdLoadDemandInSUMOGUI),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBAREDIT_LOADDEMAND,                 GNEApplicationWindow::onUpdNeedsNetwork),

    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_T_OPENSUMONETEDIT,              GNEApplicationWindow::onCmdOpenSUMOGUI),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_CTRL_T_OPENSUMONETEDIT,              GNEApplicationWindow::onUpdNeedsNetwork),
    /* Prepared for #6042
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_X_CUT,                          GNEApplicationWindow::onCmdCut),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_C_COPY,                         GNEApplicationWindow::onCmdCopy),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_CTRL_V_PASTE,                        GNEApplicationWindow::onCmdPaste),
    */

    // toolbar lock
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_LOCK_ELEMENT,                           GNEApplicationWindow::onCmdLockElements),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_LOCK_ALLELEMENTS,                       GNEApplicationWindow::onCmdLockAllElements),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_UNLOCK_ALLELEMENTS,                     GNEApplicationWindow::onCmdUnlockAllElements),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_LOCK_SELECTEDELEMENTS,                  GNEApplicationWindow::onCmdLockSelectElements),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_LOCK_MENUTITLE,                         GNEApplicationWindow::onUpdLockMenuTitle),

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
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F10_OPTIONSMENU,                             GNEApplicationWindow::onCmdOptions),

    // Toolbar locate
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEJUNCTION,     GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEJUNCTION,     GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEEDGE,         GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEEDGE,         GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEVEHICLE,      GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEVEHICLE,      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEPERSON,       GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEPERSON,       GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEROUTE,        GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEROUTE,        GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATESTOP,         GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATESTOP,         GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATETLS,          GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATETLS,          GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEADD,          GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEADD,          GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEPOI,          GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEPOI,          GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_LOCATEPOLY,         GNEApplicationWindow::onCmdLocate),
    FXMAPFUNC(SEL_UPDATE,   MID_LOCATEPOLY,         GNEApplicationWindow::onUpdNeedsNetwork),

    // toolbar tools
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_TOOLBARTOOLS_NETDIFF,   GNEApplicationWindow::onCmdRunNetDiff),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_TOOLBARTOOLS_NETDIFF,   GNEApplicationWindow::onUpdNeedsNetwork),

    // toolbar windows
    FXMAPFUNC(SEL_COMMAND,  MID_CLEARMESSAGEWINDOW,     GNEApplicationWindow::onCmdClearMsgWindow),

    // toolbar help
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F1_ONLINEDOCUMENTATION,  GNEApplicationWindow::onCmdHelp),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_F12_ABOUT,               GNEApplicationWindow::onCmdAbout),

    // alt + <number>
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_0_TOGGLEEDITOPTION,      GNEApplicationWindow::onCmdToggleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_0_TOGGLEEDITOPTION,      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_1_TOGGLEEDITOPTION,      GNEApplicationWindow::onCmdToggleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_1_TOGGLEEDITOPTION,      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_2_TOGGLEEDITOPTION,      GNEApplicationWindow::onCmdToggleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_2_TOGGLEEDITOPTION,      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_3_TOGGLEEDITOPTION,      GNEApplicationWindow::onCmdToggleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_3_TOGGLEEDITOPTION,      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_4_TOGGLEEDITOPTION,      GNEApplicationWindow::onCmdToggleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_4_TOGGLEEDITOPTION,      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_5_TOGGLEEDITOPTION,      GNEApplicationWindow::onCmdToggleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_5_TOGGLEEDITOPTION,      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_6_TOGGLEEDITOPTION,      GNEApplicationWindow::onCmdToggleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_6_TOGGLEEDITOPTION,      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_7_TOGGLEEDITOPTION,      GNEApplicationWindow::onCmdToggleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_7_TOGGLEEDITOPTION,      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_8_TOGGLEEDITOPTION,      GNEApplicationWindow::onCmdToggleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_8_TOGGLEEDITOPTION,      GNEApplicationWindow::onUpdNeedsNetwork),
    FXMAPFUNC(SEL_COMMAND,  MID_HOTKEY_ALT_9_TOGGLEEDITOPTION,      GNEApplicationWindow::onCmdToggleEditOptions),
    FXMAPFUNC(SEL_UPDATE,   MID_HOTKEY_ALT_9_TOGGLEEDITOPTION,      GNEApplicationWindow::onUpdNeedsNetwork),

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
    FXMAPFUNC(SEL_COMMAND,              MID_HOTKEY_CTRL_SHIFT_T_FORCESAVENETEWORK,          GNEApplicationWindow::onCmdForceSaveNetwork),
    FXMAPFUNC(SEL_COMMAND,              MID_HOTKEY_CTRL_SHIFT_U_FORCESAVEADDITIONALS,       GNEApplicationWindow::onCmdForceSaveAdditionals),
    FXMAPFUNC(SEL_COMMAND,              MID_HOTKEY_CTRL_SHIFT_V_FORCESAVEDEMANDELEMENTS,    GNEApplicationWindow::onCmdForceSaveDemandElements),
    FXMAPFUNC(SEL_COMMAND,              MID_HOTKEY_CTRL_SHIFT_W_FORCESAVEDATAELEMENTS,      GNEApplicationWindow::onCmdForceSaveDataElements),
    FXMAPFUNC(SEL_COMMAND,              MID_HOTKEY_SHIFT_F12_FOCUSUPPERELEMENT,             GNEApplicationWindow::onCmdFocusFrame),
    FXMAPFUNC(SEL_UPDATE,               MID_GNE_MODESMENUTITLE,                             GNEApplicationWindow::onUpdRequireViewNet),
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
}


void
GNEApplicationWindow::dependentBuild() {
    // do this not twice
    if (myHadDependentBuild) {
        WRITE_ERROR("DEBUG: GNEApplicationWindow::dependentBuild called twice");
        return;
    }
    myHadDependentBuild = true;
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
    myGeoFrame = new FXHorizontalFrame(myStatusbar, GUIDesignHorizontalFrameStatusBar);
    myGeoCoordinate = new FXLabel(myGeoFrame, "N/A\t\tOriginal coordinate (before coordinate transformation in netconvert)", nullptr, LAYOUT_CENTER_Y);
    // build cartesian coordinates label
    myCartesianFrame = new FXHorizontalFrame(myStatusbar, GUIDesignHorizontalFrameStatusBar);
    myCartesianCoordinate = new FXLabel(myCartesianFrame, "N/A\t\tNetwork coordinate", nullptr, LAYOUT_CENTER_Y);
    // build test coordinates label (only if gui-testing is enabled)
    if (OptionsCont::getOptions().getBool("gui-testing")) {
        myTestFrame = new FXHorizontalFrame(myStatusbar, GUIDesignHorizontalFrameStatusBar);
        myTestCoordinate = new FXLabel(myTestFrame, "N/A\t\tTest coordinate", nullptr, LAYOUT_CENTER_Y);
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
    myLoadThread = new GNELoadThread(getApp(), this, myEvents, myLoadThreadEvent);
    // set the status bar
    myStatusbar->getStatusLine()->setText("Ready.");
    // set the caption
    setTitle(myTitlePrefix);
    // set Netedit ICON
    setIcon(GUIIconSubSys::getIcon(GUIIcon::NETEDIT));
    setMiniIcon(GUIIconSubSys::getIcon(GUIIcon::NETEDIT_MINI));
    // build NETEDIT Accelerators (hotkeys)
    GUIShortcutsSubSys::buildNETEDITAccelerators(this);
}


void
GNEApplicationWindow::create() {
    setWindowSizeAndPos();
    gCurrentFolder = getApp()->reg().readStringEntry("SETTINGS", "basedir", "");
    FXMainWindow::create();
    myFileMenu->create();
    myModesMenu->create();
    myEditMenu->create();
    myFileMenuTLS->create();
    myFileMenuEdgeTypes->create();
    myFileMenuAdditionals->create();
    myFileMenuDemandElements->create();
    myFileMenuDataElements->create();
    //mySettingsMenu->create();
    myWindowMenu->create();
    myHelpMenu->create();

    FXint textWidth = getApp()->getNormalFont()->getTextWidth("8", 1) * 22;
    myCartesianFrame->setWidth(textWidth);
    myGeoFrame->setWidth(textWidth);

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
    // delete visuals
    delete myGLVisual;
    // must delete menus to avoid segfault on removing accelerators
    // (http://www.fox-toolkit.net/faq#TOC-What-happens-when-the-application-s)
    delete myFileMenuTLS;
    delete myFileMenuEdgeTypes;
    delete myFileMenuAdditionals;
    delete myFileMenuDemandElements;
    delete myFileMenuDataElements;
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
    // first check that current edited Net can be closed (und therefore the undo-list cleared, see #5753)
    if (myViewNet && !onCmdClose(0, 0, 0)) {
        return 1;
    } else {
        OptionsCont& oc = OptionsCont::getOptions();
        GNELoadThread::fillOptions(oc);
        GNELoadThread::setDefaultOptions(oc);
        loadConfigOrNet("", true, false, true, true);
        return 1;
    }
}


long
GNEApplicationWindow::onCmdOpenConfiguration(FXObject*, FXSelector, void*) {
    // first check that current edited Net can be closed (und therefore the undo-list cleared, see #5753)
    if (myViewNet && !onCmdClose(0, 0, 0)) {
        return 1;
    } else {
        // get the new file name
        FXFileDialog opendialog(this, "Open Netconvert Configuration");
        opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::OPEN_CONFIG));
        opendialog.setSelectMode(SELECTFILE_EXISTING);
        opendialog.setPatternList(myConfigPattern.c_str());
        if (gCurrentFolder.length() != 0) {
            opendialog.setDirectory(gCurrentFolder);
        }
        if (opendialog.execute()) {
            gCurrentFolder = opendialog.getDirectory();
            std::string file = opendialog.getFilename().text();
            // load config
            loadConfigOrNet(file, false);
            // add it into recent configs
            myMenuBarFile.myRecentNetsAndConfigs.appendFile(file.c_str());
        }
        return 1;
    }
}


long
GNEApplicationWindow::onCmdOpenNetwork(FXObject*, FXSelector, void*) {
    // get the new file name
    FXFileDialog opendialog(this, "Open Network");
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::OPEN_NET));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("SUMO nets (*.net.xml)\nAll files (*)");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        // get file
        const std::string file = opendialog.getFilename().text();
        // check if file isn't empty first
        if (!file.empty()) {
            // check if current edited Net can be closed(und therefore the undo - list cleared, see #5753)
            if (myViewNet) {
                // call close
                onCmdClose(0, 0, 0);
                // if after close there is myViewNet yet, abort
                if (myViewNet) {
                    return 0;
                }
            }
            // set current folder
            gCurrentFolder = opendialog.getDirectory();
            // load network
            loadConfigOrNet(file, true);
            // add it into recent nets
            myMenuBarFile.myRecentNetsAndConfigs.appendFile(file.c_str());
            // when a net is loaded, save additionals and TLSPrograms are disabled
            disableSaveAdditionalsMenu();
            myFileMenuCommands.saveTLSPrograms->disable();
            myFileMenuCommands.saveEdgeTypes->disable();
        }
    }
    return 0;
}


long
GNEApplicationWindow::onCmdOpenForeign(FXObject*, FXSelector, void*) {
    // first check that current edited Net can be closed (und therefore the undo-list cleared, see #5753)
    if (myViewNet && !onCmdClose(0, 0, 0)) {
        return 1;
    } else {
        // get the new file name
        FXFileDialog opendialog(this, "Import Foreign Network");
        opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::OPEN_NET));
        opendialog.setSelectMode(SELECTFILE_EXISTING);
        FXString osmPattern("OSM net (*.osm.xml,*.osm)");
        opendialog.setPatternText(0, osmPattern);
        if (gCurrentFolder.length() != 0) {
            opendialog.setDirectory(gCurrentFolder);
        }
        if (opendialog.execute()) {
            gCurrentFolder = opendialog.getDirectory();
            std::string file = opendialog.getFilename().text();

            OptionsCont& oc = OptionsCont::getOptions();
            GNELoadThread::fillOptions(oc);
            if (osmPattern.contains(opendialog.getPattern())) {
                // recommended osm options
                // https://sumo.dlr.de/wiki/Networks/Import/OpenStreetMap#Recommended_NETCONVERT_Options
                oc.set("osm-files", file);
                oc.set("geometry.remove", "true");
                oc.set("ramps.guess", "true");
                oc.set("junctions.join", "true");
                oc.set("tls.guess-signals", "true");
                oc.set("tls.discard-simple", "true");
            } else {
                throw ProcessError("Attempted to import unknown file format '" + file + "'.");
            }

            GUIDialog_Options* wizard =
                new GUIDialog_Options(this, "Select Import Options", getWidth(), getHeight());

            if (wizard->execute()) {
                NIFrame::checkOptions(); // needed to set projection parameters
                loadConfigOrNet("", false, false, false);
            }
        }
        return 1;
    }
}


long
GNEApplicationWindow::onCmdOpenTLSPrograms(FXObject*, FXSelector, void*) {
    // write debug information
    WRITE_DEBUG("Open TLSProgram dialog");
    // get the shape file name
    FXFileDialog opendialog(this, "Open TLS Programs file");
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::MODETLS));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("TLSProgram files (*.xml)\nAll files (*)");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        // close additional dialog
        WRITE_DEBUG("Close TLSProgram dialog");
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();
        // Run parser
        myUndoList->begin(Supermode::NETWORK, GUIIcon::MODETLS, "loading TLS Programs from '" + file + "'");
        myNet->computeNetwork(this);
        if (myNet->getViewNet()->getViewParent()->getTLSEditorFrame()->parseTLSPrograms(file) == false) {
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
    // Run parser
    myUndoList->begin(Supermode::NETWORK, GUIIcon::MODETLS, "loading TLS Programs from '" + OptionsCont::getOptions().getString("TLSPrograms-output") + "'");
    myNet->computeNetwork(this);
    if (myNet->getViewNet()->getViewParent()->getTLSEditorFrame()->parseTLSPrograms(OptionsCont::getOptions().getString("TLSPrograms-output")) == false) {
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
GNEApplicationWindow::onUpdReloadTLSPrograms(FXObject*, FXSelector, void*) {
    // check if file exist
    if (myViewNet && OptionsCont::getOptions().getString("TLSPrograms-output").empty()) {
        return myFileMenuCommands.reloadTLSPrograms->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else {
        return myFileMenuCommands.reloadTLSPrograms->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    }
}


long
GNEApplicationWindow::onCmdOpenEdgeTypes(FXObject*, FXSelector, void*) {
    // open dialog
    FXFileDialog opendialog(this, "Load edgeType file");
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::MODECREATEEDGE));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("*.xml");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        // declare type container
        NBTypeCont typeContainerAux;
        // declare type handler
        NIXMLTypesHandler* handler = new NIXMLTypesHandler(typeContainerAux);
        // load edge types
        NITypeLoader::load(handler, {opendialog.getFilename().text()}, "types");
        // write information
        WRITE_MESSAGE("Loaded " + toString(typeContainerAux.size()) + " edge types");
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
    return 0;
}


long
GNEApplicationWindow::onCmdReloadEdgeTypes(FXObject*, FXSelector, void*) {
    // declare type container
    NBTypeCont typeContainerAux;
    // declare type handler
    NIXMLTypesHandler* handler = new NIXMLTypesHandler(typeContainerAux);
    // load edge types
    NITypeLoader::load(handler, {OptionsCont::getOptions().getString("edgeTypes-output")}, "types");
    // write information
    WRITE_MESSAGE("Loaded " + toString(typeContainerAux.size()) + " edge types");
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
GNEApplicationWindow::onUpdReloadEdgeTypes(FXObject*, FXSelector, void*) {
    // check if file exist
    if (myViewNet && OptionsCont::getOptions().getString("edgeTypes-output").empty()) {
        return myFileMenuCommands.reloadEdgeTypes->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else {
        return myFileMenuCommands.reloadEdgeTypes->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    }
}


long
GNEApplicationWindow::onCmdOpenRecent(FXObject*, FXSelector, void* fileData) {
    // first check that current edited Net can be closed (und therefore the undo-list cleared, see #5753)
    if (myViewNet && !onCmdClose(0, 0, 0)) {
        return 1;
    } else if (myAmLoading) {
        myStatusbar->getStatusLine()->setText("Already loading!");
        return 1;
    } else {
        // get filedata
        std::string file((const char*)fileData);
        // check if we're loading a network or a config (.netccfg for configs)
        if (file.find(".netccfg") != std::string::npos) {
            // load config
            loadConfigOrNet(file, false);
        } else {
            // load network
            loadConfigOrNet(file, true);
        }
        return 1;
    }
}


long
GNEApplicationWindow::onCmdReload(FXObject*, FXSelector, void*) {
    // first check that current edited Net can be closed (und therefore the undo-list cleared, see #5753)
    if (myViewNet) {
        // check if current network can be closed
        if (continueWithUnsavedChanges("reload")) {
            closeAllWindows();
            // disable save additionals and TLS menu
            disableSaveAdditionalsMenu();
            myFileMenuCommands.saveTLSPrograms->disable();
            // disable toolbargrip modes
            myToolbarsGrip.menu->disable();
            // hide all Supermode, Network and demand commands
            mySupermodeCommands.hideSupermodeCommands();
            myModesMenuCommands.networkMenuCommands.hideNetworkMenuCommands();
            myModesMenuCommands.demandMenuCommands.hideDemandMenuCommands();
            myModesMenuCommands.dataMenuCommands.hideDataMenuCommands();
            // hide view options
            myEditMenuCommands.networkViewOptions.hideNetworkViewOptionsMenuChecks();
            myEditMenuCommands.demandViewOptions.hideDemandViewOptionsMenuChecks();
            myEditMenuCommands.dataViewOptions.hideDataViewOptionsMenuChecks();
        } else {
            // abort reloading (because "cancel button" was pressed)
            return 1;
        }
    }
    // @note. If another network has been load during this session, it might not be desirable to set useStartupOptions
    loadConfigOrNet(OptionsCont::getOptions().getString("sumo-net-file"), true, true);
    return 1;
}


long
GNEApplicationWindow::onCmdClose(FXObject*, FXSelector, void*) {
    if (continueWithUnsavedChanges("close")) {
        closeAllWindows();
        // add a separator to the log
        myMessageWindow->addSeparator();
        // disable save additionals and TLS menu
        disableSaveAdditionalsMenu();
        myFileMenuCommands.saveTLSPrograms->disable();
        // hide all Supermode, Network and demand commands
        mySupermodeCommands.hideSupermodeCommands();
        myModesMenuCommands.networkMenuCommands.hideNetworkMenuCommands();
        myModesMenuCommands.demandMenuCommands.hideDemandMenuCommands();
        myModesMenuCommands.dataMenuCommands.hideDataMenuCommands();
        // hide view options
        myEditMenuCommands.networkViewOptions.hideNetworkViewOptionsMenuChecks();
        myEditMenuCommands.demandViewOptions.hideDemandViewOptionsMenuChecks();
        myEditMenuCommands.dataViewOptions.hideDataViewOptionsMenuChecks();
    }
    return 1;
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
GNEApplicationWindow::onCmdRunNetDiff(FXObject*, FXSelector, void*) {
    OptionsCont& oc = OptionsCont::getOptions();
    // check that currently there is a View
    if (myViewNet == nullptr) {
        return 0;
    }
#ifdef WIN32
    // check that python folder is defined in PATH
    const char* path = getenv("PATH");
    if ((strstr(path, "Python") == nullptr) && (strstr(path, "python") == nullptr)) {
        WRITE_ERROR("Python folder must be defined in PATH");
        return 0;
    }
#endif
    // check that SUMO_HOME is defined
    const char* sumoPath = getenv("SUMO_HOME");
    if (sumoPath == nullptr) {
        WRITE_ERROR("Enviroment variable SUMO_HOME must be defined");
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
    FXFileDialog opendialog(this, "Open diff Network");
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::OPEN_NET));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("SUMO nets (*.net.xml)\nAll files (*)");
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
        std::string cmd = "cd " + secondNetFolder + "&" +  // folder to save diff files (the same of second net)
                          "python " + netDiff +                           // netdiff.py
                          " " + oc.getString("output-file") +             // netA (current)
                          " " + secondNet +                               // net B
                          " diff";                                        // netdiff options
        // start in background
#ifndef WIN32
        cmd = cmd + " &";
#else
        // see "help start" for the parameters
        cmd = "start /B \"\" " + cmd;
#endif
        WRITE_MESSAGE("Running " + cmd + ".");
        // yay! fun with dangerous commands... Never use this over the internet
        SysUtils::runHiddenCommand(cmd);
    }
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
    OptionsCont& oc = OptionsCont::getOptions();
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
        GNEViewParent* viewParent = new GNEViewParent(myMDIClient, myMDIMenu, "NETEDIT VIEW", this, nullptr, myNet, ec->isNewNet, myUndoList, nullptr, MDI_TRACKING, 10, 10, 300, 200);
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
    getApp()->endWaitCursor();
    myMessageWindow->registerMsgHandlers();
    // check if additionals/shapes has to be loaded at start
    if (oc.isSet("additional-files") && !oc.getString("additional-files").empty() && myNet) {
        // obtain vector of additional files
        std::vector<std::string> additionalFiles = oc.getStringVector("additional-files");
        // begin undolist
        myUndoList->begin(Supermode::NETWORK, GUIIcon::SUPERMODENETWORK, "loading additionals and shapes from '" + toString(additionalFiles) + "'");
        // iterate over every additional file
        for (const auto& additionalFile : additionalFiles) {
            WRITE_MESSAGE("loading additionals and shapes from '" + additionalFile + "'");
            // declare general handler
            GNEGeneralHandler generalHandler(myNet, additionalFile, true);
            // disable validation for additionals
            XMLSubSys::setValidation("never", "auto", "auto");
            // Run parser
            if (!generalHandler.parse()) {
                WRITE_ERROR("Loading of " + additionalFile + " failed.");
            }
            // disable validation for additionals
            XMLSubSys::setValidation("auto", "auto", "auto");
        }
        // end undo list
        myUndoList->end();
        // disable save additionals (because additionals were loaded through console)
        myNet->requireSaveAdditionals(false);
    }
    // check if demand elements has to be loaded at start
    if (oc.isSet("route-files") && !oc.getString("route-files").empty() && myNet) {
        // obtain vector of route files
        std::vector<std::string> demandElementsFiles = oc.getStringVector("route-files");
        // begin undolist
        myUndoList->begin(Supermode::DEMAND, GUIIcon::SUPERMODEDEMAND, "loading demand elements from '" + toString(demandElementsFiles) + "'");
        // iterate over every route file
        for (const auto& demandElementsFile : demandElementsFiles) {
            WRITE_MESSAGE("Loading demand elements from '" + demandElementsFile + "'");
            GNEGeneralHandler handler(myNet, demandElementsFile, true);
            // disable validation for demand elements
            XMLSubSys::setValidation("never", "auto", "auto");
            if (!handler.parse()) {
                WRITE_ERROR("Loading of " + demandElementsFile + " failed.");
            }
            // disable validation for demand elements
            XMLSubSys::setValidation("auto", "auto", "auto");
        }
        // end undo list
        myUndoList->end();
        // disable save demand elements (because demand elements were loaded through console)
        myNet->requireSaveDemandElements(false);
    }
    // check if data elements has to be loaded at start
    if (oc.isSet("data-files") && !oc.getString("data-files").empty() && myNet) {
        // obtain vector of data files
        std::vector<std::string> dataElementsFiles = oc.getStringVector("data-files");
        // disable update data
        myViewNet->getNet()->disableUpdateData();
        // begin undolist
        myUndoList->begin(Supermode::DATA, GUIIcon::SUPERMODEDATA, "loading data elements from '" + toString(dataElementsFiles) + "'");
        // iterate over every data file
        for (const auto& dataElementsFile : dataElementsFiles) {
            WRITE_MESSAGE("Loading data elements from '" + dataElementsFile + "'");
            GNEDataHandler dataHandler(myNet, dataElementsFile, true);
            // disable validation for data elements
            XMLSubSys::setValidation("never", "auto", "auto");
            if (!dataHandler.parse()) {
                WRITE_ERROR("Loading of " + dataElementsFile + " failed.");
            }
            // disable validation for data elements
            XMLSubSys::setValidation("auto", "auto", "auto");
        }
        // end undolist
        myUndoList->end();
        // disable save data elements (because data elements were loaded through console)
        myNet->requireSaveDataElements(false);
        // enable update data
        myViewNet->getNet()->enableUpdateData();
    }
    // check if additionals output must be changed
    if (oc.isSet("additionals-output")) {
        // overwrite "additional-files" with value "additionals-output"
        oc.resetWritable();
        oc.set("additional-files", oc.getString("additionals-output"));
    }
    // check if demand elements output must be changed
    if (oc.isSet("demandelements-output")) {
        // overwrite "route-files" with value "demandelements-output"
        oc.resetWritable();
        oc.set("route-files", oc.getString("demandelements-output"));
    }
    // check if data elements output must be changed
    if (oc.isSet("dataelements-output")) {
        // overwrite "data-files" with value "dataelements-output"
        oc.resetWritable();
        oc.set("data-files", oc.getString("dataelements-output"));
    }
    // after loading net shouldn't be saved
    if (myNet) {
        myNet->requireSaveNet(false);
    }
    // write reload message
    if (myReloading) {
        WRITE_MESSAGE("Reload successfully");
        myReloading = false;
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
    GUIDesigns::buildFXMenuTitle(myToolbarsGrip.menu, "&File", nullptr, myFileMenu);
    myFileMenuTLS = new FXMenuPane(this);
    myFileMenuEdgeTypes = new FXMenuPane(this);
    myFileMenuAdditionals = new FXMenuPane(this);
    myFileMenuDemandElements = new FXMenuPane(this);
    myFileMenuDataElements = new FXMenuPane(this);
    myFileMenuCommands.buildFileMenuCommands(myFileMenu, myFileMenuTLS, myFileMenuEdgeTypes, myFileMenuAdditionals, myFileMenuDemandElements, myFileMenuDataElements);
    // build recent files
    myMenuBarFile.buildRecentFiles(myFileMenu);
    new FXMenuSeparator(myFileMenu);
    GUIDesigns::buildFXMenuCommandShortcut(myFileMenu,
                                           "&Quit", "Ctrl+Q", "Quit the Application.",
                                           nullptr, this, MID_HOTKEY_CTRL_Q_CLOSE);
    // build modes menu
    myModesMenu = new FXMenuPane(this);
    myModesMenuTitle = GUIDesigns::buildFXMenuTitle(myToolbarsGrip.menu, "&Modes", nullptr, myModesMenu);
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
    GUIDesigns::buildFXMenuTitle(myToolbarsGrip.menu, "&Edit", nullptr, myEditMenu);
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
    // build separator
    new FXMenuSeparator(myEditMenu);
    // build open in sumo menu commands
    myEditMenuCommands.buildOpenSUMOMenuCommands(myEditMenu);
    // build lock menu
    myLockMenu = new FXMenuPane(this);
    myLockMenuTitle = GUIDesigns::buildFXMenuTitle(myToolbarsGrip.menu, "L&ock", nullptr, myLockMenu);
    myLockMenuTitle->setTarget(this);
    myLockMenuTitle->setSelector(MID_GNE_LOCK_MENUTITLE);
    // build lock menu commands
    myLockMenuCommands.buildLockMenuCommands(myLockMenu);
    // build processing menu (trigger netbuild computations)
    myProcessingMenu = new FXMenuPane(this);
    GUIDesigns::buildFXMenuTitle(myToolbarsGrip.menu, "&Processing", nullptr, myProcessingMenu);
    myProcessingMenuCommands.buildProcessingMenuCommands(myProcessingMenu);
    // build locate menu
    myLocatorMenu = new FXMenuPane(this);
    GUIDesigns::buildFXMenuTitle(myToolbarsGrip.menu, "&Locate", nullptr, myLocatorMenu);
    myLocateMenuCommands.buildLocateMenuCommands(myLocatorMenu);
    // build tools menu
    myToolsMenu = new FXMenuPane(this);
    GUIDesigns::buildFXMenuTitle(myToolbarsGrip.menu, "&Tools", nullptr, myToolsMenu);
    myToolsMenuCommands.buildToolsMenuCommands(myToolsMenu);
    // build windows menu
    myWindowMenu = new FXMenuPane(this);
    GUIDesigns::buildFXMenuTitle(myToolbarsGrip.menu, "&Window", nullptr, myWindowMenu);
    myWindowsMenuCommands.buildWindowsMenuCommands(myWindowMenu, myStatusbar, myMessageWindow);
    // build help menu
    myHelpMenu = new FXMenuPane(this);
    GUIDesigns::buildFXMenuTitle(myToolbarsGrip.menu, "&Help", nullptr, myHelpMenu);
    // build help menu commands
    GUIDesigns::buildFXMenuCommandShortcut(myHelpMenu,
                                           "&Online Documentation", "F1", "Open Online documentation.",
                                           nullptr, this, MID_HOTKEY_F1_ONLINEDOCUMENTATION);
    GUIDesigns::buildFXMenuCommandShortcut(myHelpMenu,
                                           "&About", "F12", "About netedit.",
                                           GUIIconSubSys::getIcon(GUIIcon::NETEDIT_MINI), this, MID_HOTKEY_F12_ABOUT);
}


void
GNEApplicationWindow::loadConfigOrNet(const std::string file, bool isNet, bool isReload, bool useStartupOptions, bool newNet) {
    storeWindowSizeAndPos();
    getApp()->beginWaitCursor();
    myAmLoading = true;
    myReloading = isReload;
    closeAllWindows();
    if (isReload) {
        myLoadThread->start();
        setStatusBarText("Reloading.");
    } else {
        gSchemeStorage.saveViewport(0, 0, -1, 0); // recenter view
        myLoadThread->loadConfigOrNet(file, isNet, useStartupOptions, newNet);
        setStatusBarText("Loading '" + file + "'.");
    }
    // show supermode commands menu
    mySupermodeCommands.showSupermodeCommands();
    // show Network command menus (because Network is the default supermode)
    myModesMenuCommands.networkMenuCommands.showNetworkMenuCommands();
    // update window
    update();
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
    myGeoCoordinate->setText("N/A");
    myCartesianCoordinate->setText("N/A");
    if (myTestCoordinate) {
        myTestCoordinate->setText("N/A");
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
    // disable saving command
    disableSaveAdditionalsMenu();
}


FXCursor*
GNEApplicationWindow::getDefaultCursor() {
    return getApp()->getDefaultCursor(DEF_ARROW_CURSOR);
}


void
GNEApplicationWindow::loadOptionOnStartup() {
    OptionsCont& oc = OptionsCont::getOptions();
    // Disable normalization preserve the given network as far as possible
    oc.set("offset.disable-normalization", "true");
    loadConfigOrNet("", true, false, true, oc.getBool("new"));
}


void
GNEApplicationWindow::setStatusBarText(const std::string& statusBarText) {
    myStatusbar->getStatusLine()->setText(statusBarText.c_str());
    myStatusbar->getStatusLine()->setNormalText(statusBarText.c_str());
}


long
GNEApplicationWindow::computeJunctionWithVolatileOptions() {
    // obtain option container
    OptionsCont& oc = OptionsCont::getOptions();
    // declare variable to save FXMessageBox outputs.
    FXuint answer = 0;
    // declare string to save paths in wich additionals, shapes demand and data elements will be saved
    std::string additionalsSavePath = oc.getString("additional-files");
    std::string demandElementsSavePath = oc.getString("route-files");
    std::string dataElementsSavePath = oc.getString("data-files");
    // write warning if netedit is running in testing mode
    WRITE_DEBUG("Opening FXMessageBox 'Volatile Recomputing'");
    // open question dialog box
    answer = FXMessageBox::question(myNet->getViewNet()->getApp(), MBOX_YES_NO, "Recompute with volatile options",
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
        // Check if there are additionals in our net
        if (myNet->getAttributeCarriers()->getNumberOfAdditionals() > 0) {
            // ask user if want to save additionals if weren't saved previously
            if (oc.getString("additional-files") == "") {
                // write warning if netedit is running in testing mode
                WRITE_DEBUG("Opening FXMessageBox 'Save additionals before recomputing'");
                // open question dialog box
                answer = FXMessageBox::question(myNet->getViewNet()->getApp(), MBOX_YES_NO, "Save additionals before recomputing with volatile options",
                                                "Would you like to save additionals before recomputing?");
                if (answer != 1) { //1:yes, 2:no, 4:esc
                    // write warning if netedit is running in testing mode
                    if (answer == 2) {
                        WRITE_DEBUG("Closed FXMessageBox 'Save additionals before recomputing' with 'No'");
                    } else if (answer == 4) {
                        WRITE_DEBUG("Closed FXMessageBox 'Save additionals before recomputing' with 'ESC'");
                    }
                } else {
                    // write warning if netedit is running in testing mode
                    WRITE_DEBUG("Closed FXMessageBox 'Save additionals before recomputing' with 'Yes'");
                    // Open a dialog to set filename output
                    FXString file = MFXUtils::getFilename2Write(this,
                                    "Save demand element file", ".xml",
                                    GUIIconSubSys::getIcon(GUIIcon::MODETLS),
                                    gCurrentFolder).text();
                    // add xml extension
                    std::string fileWithExtension = FileHelpers::addExtension(file.text(), ".rou.xml");
                    // check that file is valid
                    if (fileWithExtension != "") {
                        // update additional files
                        oc.resetWritable();
                        oc.set("additional-files", fileWithExtension);
                        // set obtanied filename output into additionalsSavePath (can be "")
                        additionalsSavePath = oc.getString("additional-files");
                    }
                }
            }
            // Check if additional must be saved in a temporal directory, if user didn't define a directory for additionals
            if (oc.getString("additional-files") == "") {
                // Obtain temporal directory provided by FXSystem::getCurrentDirectory()
                additionalsSavePath = FXSystem::getTempDirectory().text() + std::string("/tmpAdditionalsNetedit.xml");
            }
            // Start saving additionals
            getApp()->beginWaitCursor();
            try {
                myNet->saveAdditionals(additionalsSavePath);
            } catch (IOError& e) {
                // write warning if netedit is running in testing mode
                WRITE_DEBUG("Opening FXMessageBox 'Error saving additionals before recomputing'");
                // open error message box
                FXMessageBox::error(this, MBOX_OK, "Saving additionals in temporal folder failed!", "%s", e.what());
                // write warning if netedit is running in testing mode
                WRITE_DEBUG("Closed FXMessageBox 'Error saving additionals before recomputing' with 'OK'");
            }
            // end saving additionals
            myMessageWindow->addSeparator();
            getApp()->endWaitCursor();
            // restore focus
            setFocus();
        } else {
            // clear additional path
            additionalsSavePath = "";
        }
        // Check if there are demand elements in our net
        if (myNet->getAttributeCarriers()->getNumberOfDemandElements() > 0) {
            // ask user if want to save demand elements if weren't saved previously
            if (oc.getString("route-files") == "") {
                // write warning if netedit is running in testing mode
                WRITE_DEBUG("Opening FXMessageBox 'Save demand elements before recomputing'");
                // open question dialog box
                answer = FXMessageBox::question(myNet->getViewNet()->getApp(), MBOX_YES_NO, "Save demand elements before recomputing with volatile options",
                                                "Would you like to save demand elements before recomputing?");
                if (answer != 1) { //1:yes, 2:no, 4:esc
                    // write warning if netedit is running in testing mode
                    if (answer == 2) {
                        WRITE_DEBUG("Closed FXMessageBox 'Save demand elements before recomputing' with 'No'");
                    } else if (answer == 4) {
                        WRITE_DEBUG("Closed FXMessageBox 'Save demand elements before recomputing' with 'ESC'");
                    }
                } else {
                    // write warning if netedit is running in testing mode
                    WRITE_DEBUG("Closed FXMessageBox 'Save demand elements before recomputing' with 'Yes'");
                    // Open a dialog to set filename output
                    FXString file = MFXUtils::getFilename2Write(this,
                                    "Save demand element file", ".rou.xml",
                                    GUIIconSubSys::getIcon(GUIIcon::MODETLS),
                                    gCurrentFolder).text();
                    // add xml extension
                    std::string fileWithExtension = FileHelpers::addExtension(file.text(), ".rou.xml");
                    // check that file is valid
                    if (fileWithExtension != "") {
                        // update route files
                        oc.resetWritable();
                        oc.set("route-files", fileWithExtension);
                        // set obtanied filename output into demand elementSavePath (can be "")
                        demandElementsSavePath = oc.getString("route-files");
                    }
                }
            }
            // Check if demand element must be saved in a temporal directory, if user didn't define a directory for demand elements
            if (oc.getString("route-files") == "") {
                // Obtain temporal directory provided by FXSystem::getCurrentDirectory()
                demandElementsSavePath = FXSystem::getTempDirectory().text() + std::string("/tmpDemandElementsNetedit.xml");
            }
            // Start saving demand elements
            getApp()->beginWaitCursor();
            try {
                myNet->saveDemandElements(demandElementsSavePath);
            } catch (IOError& e) {
                // write warning if netedit is running in testing mode
                WRITE_DEBUG("Opening FXMessageBox 'Error saving demand elements before recomputing'");
                // open error message box
                FXMessageBox::error(this, MBOX_OK, "Saving demand elements in temporal folder failed!", "%s", e.what());
                // write warning if netedit is running in testing mode
                WRITE_DEBUG("Closed FXMessageBox 'Error saving demand elements before recomputing' with 'OK'");
            }
            // end saving demand elements
            myMessageWindow->addSeparator();
            getApp()->endWaitCursor();
            // restore focus
            setFocus();
        } else {
            // clear demand element path
            demandElementsSavePath = "";
        }
        // Check if there are data elements in our net
        if (myNet->getAttributeCarriers()->getDataSets().size() > 0) {
            // ask user if want to save data elements if weren't saved previously
            if (oc.getString("data-files") == "") {
                // write warning if netedit is running in testing mode
                WRITE_DEBUG("Opening FXMessageBox 'Save data elements before recomputing'");
                // open question dialog box
                answer = FXMessageBox::question(myNet->getViewNet()->getApp(), MBOX_YES_NO, "Save data elements before recomputing with volatile options",
                                                "Would you like to save data elements before recomputing?");
                if (answer != 1) { //1:yes, 2:no, 4:esc
                    // write warning if netedit is running in testing mode
                    if (answer == 2) {
                        WRITE_DEBUG("Closed FXMessageBox 'Save data elements before recomputing' with 'No'");
                    } else if (answer == 4) {
                        WRITE_DEBUG("Closed FXMessageBox 'Save data elements before recomputing' with 'ESC'");
                    }
                } else {
                    // write warning if netedit is running in testing mode
                    WRITE_DEBUG("Closed FXMessageBox 'Save data elements before recomputing' with 'Yes'");
                    // Open a dialog to set filename output
                    FXString file = MFXUtils::getFilename2Write(this,
                                    "Save data element file", ".rou.xml",
                                    GUIIconSubSys::getIcon(GUIIcon::MODETLS),
                                    gCurrentFolder).text();
                    // add xml extension
                    std::string fileWithExtension = FileHelpers::addExtension(file.text(), ".rou.xml");
                    // check that file is valid
                    if (fileWithExtension != "") {
                        // update data files
                        oc.resetWritable();
                        oc.set("data-files", fileWithExtension);
                        // set obtanied filename output into data elementSavePath (can be "")
                        dataElementsSavePath = oc.getString("data-files");
                    }
                }
            }
            // Check if data element must be saved in a temporal directory, if user didn't define a directory for data elements
            if (oc.getString("data-files") == "") {
                // Obtain temporal directory provided by FXSystem::getCurrentDirectory()
                dataElementsSavePath = FXSystem::getTempDirectory().text() + std::string("/tmpDataElementsNetedit.xml");
            }
            // Start saving data elements
            getApp()->beginWaitCursor();
            try {
                myNet->saveDataElements(dataElementsSavePath);
            } catch (IOError& e) {
                // write warning if netedit is running in testing mode
                WRITE_DEBUG("Opening FXMessageBox 'Error saving data elements before recomputing'");
                // open error message box
                FXMessageBox::error(this, MBOX_OK, "Saving data elements in temporal folder failed!", "%s", e.what());
                // write warning if netedit is running in testing mode
                WRITE_DEBUG("Closed FXMessageBox 'Error saving data elements before recomputing' with 'OK'");
            }
            // end saving data elements
            myMessageWindow->addSeparator();
            getApp()->endWaitCursor();
            // restore focus
            setFocus();
        } else {
            // clear data element path
            dataElementsSavePath = "";
        }
        // compute with volatile options
        myNet->computeNetwork(this, true, true, additionalsSavePath, demandElementsSavePath, dataElementsSavePath);
        updateControls();
        return 1;
    }
}


void
GNEApplicationWindow::enableSaveTLSProgramsMenu() {
    myFileMenuCommands.saveTLSPrograms->enable();
}


void
GNEApplicationWindow::enableSaveAdditionalsMenu() {
    myFileMenuCommands.saveAdditionals->enable();
    myFileMenuCommands.saveAdditionalsAs->enable();
}


void
GNEApplicationWindow::disableSaveAdditionalsMenu() {
    myFileMenuCommands.saveAdditionals->disable();
    myFileMenuCommands.saveAdditionalsAs->disable();
}


void
GNEApplicationWindow::enableSaveDemandElementsMenu() {
    myFileMenuCommands.saveDemandElements->disable();
    myFileMenuCommands.saveDemandElementsAs->disable();
}


void
GNEApplicationWindow::disableSaveDemandElementsMenu() {
    myFileMenuCommands.saveDemandElements->disable();
    myFileMenuCommands.saveDemandElementsAs->disable();
}


void
GNEApplicationWindow::enableSaveDataElementsMenu() {
    myFileMenuCommands.saveDataElements->disable();
    myFileMenuCommands.saveDataElementsAs->disable();
}


void
GNEApplicationWindow::disableSaveDataElementsMenu() {
    myFileMenuCommands.saveDataElements->disable();
    myFileMenuCommands.saveDataElementsAs->disable();
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
    WRITE_MESSAGE("Running " + cmd + ".");
    // yay! fun with dangerous commands... Never use this over the internet
    SysUtils::runHiddenCommand(cmd);
    return 1;
}


long
GNEApplicationWindow::onCmdOpenSUMOGUI(FXObject*, FXSelector, void*) {
    // check that currently there is a View
    if (myViewNet) {
        // first check if network is saved
        if (!myViewNet->getNet()->isNetSaved()) {
            // save network
            onCmdSaveNetwork(nullptr, 0, nullptr);
            if (!myViewNet->getNet()->isNetSaved()) {
                return 0;
            }
        }
        // now check if additionals must be loaded and are saved
        if ((myEditMenuCommands.loadAdditionalsInSUMOGUI->getCheck() == TRUE) &&
                (myViewNet->getNet()->getAttributeCarriers()->getNumberOfAdditionals() > 0)) {
            // save additionals
            onCmdSaveAdditionals(nullptr, 0, nullptr);
            // check if additionals were successfully saved. If not, abort
            if (!myViewNet->getNet()->isAdditionalsSaved()) {
                return 0;
            }
        }
        // finally check if demand elements must be loaded and are saved
        if ((myEditMenuCommands.loadDemandInSUMOGUI->getCheck() == TRUE) &&
                (myViewNet->getNet()->getAttributeCarriers()->getNumberOfDemandElements() > 0)) {
            // save additionals
            onCmdSaveDemandElements(nullptr, 0, nullptr);
            // check if demand elements were successfully saved. If not, abort
            if (!myViewNet->getNet()->isDemandElementsSaved()) {
                return 0;
            }
        }
        // obtain viewport
        FXRegistry reg("SUMO GUI", "sumo-gui");
        reg.read();
        reg.writeRealEntry("viewport", "x", myViewNet->getChanger().getXPos());
        reg.writeRealEntry("viewport", "y", myViewNet->getChanger().getYPos());
        reg.writeRealEntry("viewport", "z", myViewNet->getChanger().getZPos());
        reg.write();
        std::string sumogui = "sumo-gui";
        const char* sumoPath = getenv("SUMO_HOME");
        if (sumoPath != nullptr) {
            std::string newPath = std::string(sumoPath) + "/bin/sumo-gui";
            if (FileHelpers::isReadable(newPath) || FileHelpers::isReadable(newPath + ".exe")) {
                sumogui = "\"" + newPath + "\"";
            }
        }
        // declare comand
        std::string cmd = sumogui + " --registry-viewport" + " -n "  + "\"" + OptionsCont::getOptions().getString("output-file") + "\"";
        // obtainer options container
        OptionsCont& oc = OptionsCont::getOptions();
        // if load additionals is enabled, add it to command
        if ((myEditMenuCommands.loadAdditionalsInSUMOGUI->getCheck() == TRUE) && (oc.getString("additional-files").size() > 0)) {
            cmd += " -a \"" + oc.getString("additional-files") + "\"";
        }
        // if load demand is enabled, add it to command
        if ((myEditMenuCommands.loadDemandInSUMOGUI->getCheck() == TRUE) && (oc.getString("route-files").size() > 0)) {
            cmd += " -r \"" + oc.getString("route-files") + "\"";
        }
        // if we have trips or flow over junctions, add option junction-taz
        if ((myNet->getAttributeCarriers()->getDemandElements().at(GNE_TAG_TRIP_JUNCTIONS).size() > 0) ||
                (myNet->getAttributeCarriers()->getDemandElements().at(GNE_TAG_FLOW_JUNCTIONS).size() > 0)) {
            cmd += " --junction-taz";
        }
        // start in background
#ifndef WIN32
        cmd = cmd + " &";
#else
        // see "help start" for the parameters
        cmd = "start /B \"\" " + cmd;
#endif
        WRITE_MESSAGE("Running " + cmd + ".");
        // yay! fun with dangerous commands... Never use this over the internet
        SysUtils::runHiddenCommand(cmd);
    }
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
        myViewNet->getNet()->requireSaveNet(true);
        myViewNet->update();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdForceSaveAdditionals(FXObject* /*sender*/, FXSelector /*sel*/, void* /*ptr*/) {
    // check that view exists
    if (myViewNet) {
        myViewNet->getNet()->requireSaveAdditionals(true);
        update();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdForceSaveDemandElements(FXObject* /*sender*/, FXSelector /*sel*/, void* /*ptr*/) {
    // check that view exists
    if (myViewNet) {
        myViewNet->getNet()->requireSaveDemandElements(true);
        update();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdForceSaveDataElements(FXObject* /*sender*/, FXSelector /*sel*/, void* /*ptr*/) {
    // check that view exists
    if (myViewNet) {
        myViewNet->getNet()->requireSaveDataElements(true);
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
GNEApplicationWindow::onCmdToggleGrid(FXObject* obj, FXSelector sel, void* ptr) {
    // check that view exists
    if (myViewNet) {
        // show debug info
        if (myViewNet->getVisualisationSettings().showGrid) {
            // show extra information for tests
            WRITE_DEBUG("Disabled grid throught Ctrl+g hotkey");
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled grid throught Ctrl+g hotkey");
        }
        // Call manually toggle grid function
        myViewNet->onCmdToggleShowGrid(obj, sel, ptr);
    }
    return 1;
}


long
GNEApplicationWindow::onCmdToggleDrawJunctionShape(FXObject* obj, FXSelector sel, void* ptr) {
    // check that view exists
    if (myViewNet) {
        // show debug info
        if (myViewNet->getVisualisationSettings().drawJunctionShape) {
            // show extra information for tests
            WRITE_DEBUG("Disabled draw junction shape throught Ctrl+j hotkey");
        } else {
            // show extra information for tests
            WRITE_DEBUG("Enabled draw junction shape throught Ctrl+j hotkey");
        }
        // Call manually toggle junction shape function
        myViewNet->onCmdToggleDrawJunctionShape(obj, sel, ptr);
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
    }
    return 1;
}


long
GNEApplicationWindow::onCmdToggleEditOptions(FXObject* obj, FXSelector sel, void* /* ptr */) {
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
                visibleMenuCommands.at(numericalKeyPressed), numericalKeyPressed, obj, sel)) {
            return 1;
        } else if (GNEApplicationWindowHelper::toggleEditOptionsDemand(myViewNet,
                   visibleMenuCommands.at(numericalKeyPressed), numericalKeyPressed, obj, sel)) {
            return 1;
        } else if (GNEApplicationWindowHelper::toggleEditOptionsData(myViewNet,
                   visibleMenuCommands.at(numericalKeyPressed), numericalKeyPressed, obj, sel)) {
            return 1;
        }
    }
    return 1;
}


long
GNEApplicationWindow::onCmdHelp(FXObject*, FXSelector, void*) {
    FXLinkLabel::fxexecute("https://sumo.dlr.de/docs/netedit.html");
    return 1;
}


long
GNEApplicationWindow::onCmdOptions(FXObject*, FXSelector, void*) {
    GUIDialog_Options* wizard =
        new GUIDialog_Options(this, "Configure Options", getWidth(), getHeight());

    if (wizard->execute()) {
        NIFrame::checkOptions(); // needed to set projection parameters
        NBFrame::checkOptions();
        NWFrame::checkOptions();
        SystemFrame::checkOptions(); // needed to set precision
    }
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
            if (!myViewNet->aksChangeSupermode("Undo", myViewNet->getUndoList()->getUndoSupermode())) {
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
        // check if update undoRedo dialog
        if (myUndoListDialog->shown()) {
            myUndoListDialog->updateList();
        }
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
            if (!myViewNet->aksChangeSupermode("Redo", myViewNet->getUndoList()->getRedoSupermode())) {
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
        // check if update undoRedo dialog
        if (myUndoListDialog->shown()) {
            myUndoListDialog->updateList();
        }
        return 1;
    }
}


long
GNEApplicationWindow::onCmdOpenUndoListDialog(FXObject*, FXSelector, void*) {
    // avoid open two dialogs
    if (myUndoListDialog->shown()) {
        myUndoListDialog->setFocus();
    } else {
        myUndoListDialog->open();
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
GNEApplicationWindow::onCmdSaveAsNetwork(FXObject*, FXSelector, void*) {
    // declar extensions
    const std::string netExtension = ".net.xml";
    const std::string zipNetExtension = netExtension + ".gz";
    const std::string wildcard = (netExtension + "\n*" + zipNetExtension);
    // open dialog
    FXString file = MFXUtils::getFilename2Write(this,
                    "Save Network as", wildcard.c_str(),
                    GUIIconSubSys::getIcon(GUIIcon::SAVE),
                    gCurrentFolder);
    // get file with extension
    std::string fileWithExtension = file.text();
    // clear wildcard
    const size_t pos = fileWithExtension.find(wildcard);
    if (pos != std::string::npos){
        // If found then erase it from string
        fileWithExtension.erase(pos, wildcard.length());
    }
    // check xml extension
    if (!GNEApplicationWindowHelper::stringEndsWith(fileWithExtension, netExtension) && 
        !GNEApplicationWindowHelper::stringEndsWith(fileWithExtension, zipNetExtension)) {
        fileWithExtension = FileHelpers::addExtension(fileWithExtension, netExtension);
    }
    // check that file with extension is valid
    if (fileWithExtension != "") {
        OptionsCont& oc = OptionsCont::getOptions();
        oc.resetWritable();
        oc.set("output-file", fileWithExtension);
        setTitle(MFXUtils::getTitleText(myTitlePrefix, fileWithExtension.c_str()));
        onCmdSaveNetwork(nullptr, 0, nullptr);
    }
    return 1;
}


long
GNEApplicationWindow::onCmdSaveAsPlainXML(FXObject*, FXSelector, void*) {
    // obtain option container
    OptionsCont& oc = OptionsCont::getOptions();
    // declare current folder
    FXString currentFolder = gCurrentFolder;
    // check if there is a saved network
    if (oc.getString("output-file").size() > 0) {
        // extract folder
        currentFolder = getFolder(oc.getString("output-file"));
    }
    // open dialog
    FXString file = MFXUtils::getFilename2Write(this,
                    "Save plain-xml edge-file (other names will be deduced from this)", "",
                    GUIIconSubSys::getIcon(GUIIcon::SAVE),
                    currentFolder);
    // check that file is valid (note: in this case we don't need to use function FileHelpers::addExtension)
    if (file != "") {
        std::string prefix = file.text();
        // if the name of an edg.xml file was given, remove the suffix
        if (StringUtils::endsWith(prefix, ".edg.xml")) {
            prefix = prefix.substr(0, prefix.size() - 8);
        }
        if (StringUtils::endsWith(prefix, ".")) {
            prefix = prefix.substr(0, prefix.size() - 1);
        }
        getApp()->beginWaitCursor();
        try {
            myNet->savePlain(oc, prefix);
        } catch (IOError& e) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Opening FXMessageBox 'Error saving plainXML'");
            // open message box
            FXMessageBox::error(this, MBOX_OK, "Saving plain xml failed!", "%s", e.what());
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox 'Error saving plainXML' with 'OK'");
        }
        myMessageWindow->appendMsg(GUIEventType::MESSAGE_OCCURRED, "Plain XML saved with prefix '" + prefix + "'.\n");
        myMessageWindow->addSeparator();
        getApp()->endWaitCursor();
        // restore focus
        setFocus();
    }
    return 1;
}


long
GNEApplicationWindow::onCmdSaveJoined(FXObject*, FXSelector, void*) {
    // obtain option container
    OptionsCont& oc = OptionsCont::getOptions();
    // declare current folder
    FXString currentFolder = gCurrentFolder;
    // check if there is a saved network
    if (oc.getString("output-file").size() > 0) {
        // extract folder
        currentFolder = getFolder(oc.getString("output-file"));
    }
    // open dialog
    FXString file = MFXUtils::getFilename2Write(this,
                    "Save joined-junctions", ".nod.xml",
                    GUIIconSubSys::getIcon(GUIIcon::SAVE),
                    currentFolder);
    // add xml extension
    std::string fileWithExtension = FileHelpers::addExtension(file.text(), ".xml");
    // check that file with extension is valid
    if (fileWithExtension != "") {
        getApp()->beginWaitCursor();
        try {
            myNet->saveJoined(oc, fileWithExtension);
        } catch (IOError& e) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Opening FXMessageBox 'error saving joined'");
            // opening error message
            FXMessageBox::error(this, MBOX_OK, "Saving joined junctions failed!", "%s", e.what());
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox 'error saving joined' with 'OK'");
        }
        myMessageWindow->appendMsg(GUIEventType::MESSAGE_OCCURRED, "Joined junctions saved to '" + fileWithExtension + "'.\n");
        myMessageWindow->addSeparator();
        getApp()->endWaitCursor();
        // restore focus
        setFocus();
    }
    return 1;
}


long
GNEApplicationWindow::onUpdNeedsNetwork(FXObject* sender, FXSelector, void*) {
    // check if net exist
    if (myNet) {
        // enable menu cascades
        myFileMenuCommands.TLSMenuCascade->enable();
        myFileMenuCommands.edgeTypesMenuCascade->enable();
        myFileMenuCommands.additionalMenuCascade->enable();
        myFileMenuCommands.demandMenuCascade->enable();
        myFileMenuCommands.dataMenuCascade->enable();
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        // disable menu cascades
        myFileMenuCommands.TLSMenuCascade->disable();
        myFileMenuCommands.edgeTypesMenuCascade->disable();
        myFileMenuCommands.additionalMenuCascade->disable();
        myFileMenuCommands.demandMenuCascade->disable();
        myFileMenuCommands.dataMenuCascade->disable();
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
GNEApplicationWindow::onUpdReload(FXObject* sender, FXSelector, void*) {
    if ((myNet == nullptr) || !OptionsCont::getOptions().isSet("sumo-net-file")) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    }
}


long
GNEApplicationWindow::onUpdSaveAllElements(FXObject* sender, FXSelector, void*) {
    bool enable = false;
    if (myNet) {
        if (!myNet->isNetSaved()) {
            enable = true;
        }
        if (!myNet->isAdditionalsSaved()) {
            enable = true;
        }
        if (!myNet->isDemandElementsSaved()) {
            enable = true;
        }
        if (!myNet->isDataElementsSaved()) {
            enable = true;
        }
    }
    sender->handle(this, enable ? FXSEL(SEL_COMMAND, ID_ENABLE) : FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    return 1;
}


long
GNEApplicationWindow::onUpdSaveNetwork(FXObject* sender, FXSelector, void*) {
    return sender->handle(this, ((myNet == nullptr) || myNet->isNetSaved()) ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
}


long
GNEApplicationWindow::onUpdSaveAdditionals(FXObject* sender, FXSelector, void*) {
    if (myNet == nullptr) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else if (myNet->getViewNet()->getViewParent()->getTAZFrame()->getTAZSaveChangesModule()->isChangesPending()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else {
        return sender->handle(this, myNet->isAdditionalsSaved() ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    }
}


long
GNEApplicationWindow::onUpdSaveAdditionalsAs(FXObject* sender, FXSelector, void*) {
    return sender->handle(this, ((myNet == nullptr) || (myNet->getAttributeCarriers()->getNumberOfAdditionals() == 0)) ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
}


long
GNEApplicationWindow::onUpdSaveDemandElements(FXObject* sender, FXSelector, void*) {
    return sender->handle(this, ((myNet == nullptr) || myNet->isDemandElementsSaved()) ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
}


long
GNEApplicationWindow::onUpdSaveDemandElementsAs(FXObject* sender, FXSelector, void*) {
    return sender->handle(this, ((myNet == nullptr) || (myNet->getAttributeCarriers()->getNumberOfDemandElements() == 0)) ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
}


long
GNEApplicationWindow::onUpdSaveDataElements(FXObject* sender, FXSelector, void*) {
    return sender->handle(this, ((myNet == nullptr) || myNet->isDataElementsSaved()) ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
}


long
GNEApplicationWindow::onUpdSaveDataElementsAs(FXObject* sender, FXSelector, void*) {
    return sender->handle(this, ((myNet == nullptr) || (myNet->getAttributeCarriers()->getDataSets().size() == 0)) ? FXSEL(SEL_COMMAND, ID_DISABLE) : FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
}


long
GNEApplicationWindow::onUpdUndo(FXObject* obj, FXSelector sel, void* ptr) {
    return myUndoList->onUpdUndo(obj, sel, ptr);
}


long
GNEApplicationWindow::onUpdRedo(FXObject* obj, FXSelector sel, void* ptr) {
    return myUndoList->onUpdRedo(obj, sel, ptr);
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
GNEApplicationWindow::onCmdToggleViewOption(FXObject* obj, FXSelector sel, void* ptr) {
    // check viewNet
    if (myViewNet) {
        // continue depending of selector
        switch (FXSELID(sel)) {
            // Network
            case MID_GNE_NETWORKVIEWOPTIONS_TOGGLEGRID:
                return myViewNet->onCmdToggleShowGrid(obj, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE:
                return myViewNet->onCmdToggleDrawJunctionShape(obj, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_DRAWSPREADVEHICLES:
                return myViewNet->onCmdToggleDrawSpreadVehicles(obj, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_SHOWDEMANDELEMENTS:
                return myViewNet->onCmdToggleShowDemandElementsNetwork(obj, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_SELECTEDGES:
                return myViewNet->onCmdToggleSelectEdges(obj, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_SHOWCONNECTIONS:
                return myViewNet->onCmdToggleShowConnections(obj, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_HIDECONNECTIONS:
                return myViewNet->onCmdToggleHideConnections(obj, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_SHOWSUBADDITIONALS:
                return myViewNet->onCmdToggleShowAdditionalSubElements(obj, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_SHOWTAZELEMENTS:
                return myViewNet->onCmdToggleShowTAZElements(obj, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_EXTENDSELECTION:
                return myViewNet->onCmdToggleExtendSelection(obj, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_CHANGEALLPHASES:
                return myViewNet->onCmdToggleChangeAllPhases(obj, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_ASKFORMERGE:
                return myViewNet->onCmdToggleWarnAboutMerge(obj, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_SHOWBUBBLES:
                return myViewNet->onCmdToggleShowJunctionBubbles(obj, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_MOVEELEVATION:
                return myViewNet->onCmdToggleMoveElevation(obj, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_CHAINEDGES:
                return myViewNet->onCmdToggleChainEdges(obj, sel, ptr);
            case MID_GNE_NETWORKVIEWOPTIONS_AUTOOPPOSITEEDGES:
                return myViewNet->onCmdToggleAutoOppositeEdge(obj, sel, ptr);
            // Demand
            case MID_GNE_DEMANDVIEWOPTIONS_SHOWGRID:
                return myViewNet->onCmdToggleShowGrid(obj, sel, ptr);
            case MID_GNE_DEMANDVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE:
                return myViewNet->onCmdToggleDrawJunctionShape(obj, sel, ptr);
            case MID_GNE_DEMANDVIEWOPTIONS_DRAWSPREADVEHICLES:
                return myViewNet->onCmdToggleDrawSpreadVehicles(obj, sel, ptr);
            case MID_GNE_DEMANDVIEWOPTIONS_HIDENONINSPECTED:
                return myViewNet->onCmdToggleHideNonInspecteDemandElements(obj, sel, ptr);
            case MID_GNE_DEMANDVIEWOPTIONS_HIDESHAPES:
                return myViewNet->onCmdToggleHideShapes(obj, sel, ptr);
            case MID_GNE_DEMANDVIEWOPTIONS_SHOWTRIPS:
                return myViewNet->onCmdToggleShowTrips(obj, sel, ptr);
            case MID_GNE_DEMANDVIEWOPTIONS_SHOWALLPERSONPLANS:
                return myViewNet->onCmdToggleShowAllPersonPlans(obj, sel, ptr);
            case MID_GNE_DEMANDVIEWOPTIONS_LOCKPERSON:
                return myViewNet->onCmdToggleLockPerson(obj, sel, ptr);
            case MID_GNE_DEMANDVIEWOPTIONS_SHOWALLCONTAINERPLANS:
                return myViewNet->onCmdToggleShowAllContainerPlans(obj, sel, ptr);
            case MID_GNE_DEMANDVIEWOPTIONS_LOCKCONTAINER:
                return myViewNet->onCmdToggleLockContainer(obj, sel, ptr);
            case MID_GNE_DEMANDVIEWOPTIONS_SHOWOVERLAPPEDROUTES:
                return myViewNet->onCmdToggleShowOverlappedRoutes(obj, sel, ptr);
            // Data
            case MID_GNE_DATAVIEWOPTIONS_TOGGLEDRAWJUNCTIONSHAPE:
                return myViewNet->onCmdToggleDrawJunctionShape(obj, sel, ptr);
            case MID_GNE_DATAVIEWOPTIONS_SHOWADDITIONALS:
                return myViewNet->onCmdToggleShowAdditionals(obj, sel, ptr);
            case MID_GNE_DATAVIEWOPTIONS_SHOWSHAPES:
                return myViewNet->onCmdToggleShowShapes(obj, sel, ptr);
            case MID_GNE_DATAVIEWOPTIONS_SHOWDEMANDELEMENTS:
                return myViewNet->onCmdToggleShowDemandElementsData(obj, sel, ptr);
            case MID_GNE_DATAVIEWOPTIONS_TAZRELDRAWING:
                return myViewNet->onCmdToggleTAZRelDrawing(obj, sel, ptr);
            case MID_GNE_DATAVIEWOPTIONS_TAZDRAWFILL:
                return myViewNet->onCmdToggleTAZRelDrawing(obj, sel, ptr);
            case MID_GNE_DATAVIEWOPTIONS_TAZRELONLYFROM:
                return myViewNet->onCmdToggleTAZRelOnlyFrom(obj, sel, ptr);
            case MID_GNE_DATAVIEWOPTIONS_TAZRELONLYTO:
                return myViewNet->onCmdToggleTAZRelOnlyTo(obj, sel, ptr);
            default:
                return 0;
        }
    } else {
        return 0;
    }
}


long
GNEApplicationWindow::onUpdToggleViewOption(FXObject* obj, FXSelector sel, void* /*ptr*/) {
    // get menuCheck
    FXMenuCheckIcon* menuCheck = dynamic_cast<FXMenuCheckIcon*>(obj);
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
                if (myViewNet->getDataViewOptions().menuCheckToogleTAZRelDrawing->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_DATAVIEWOPTIONS_TAZDRAWFILL:
                if (myViewNet->getDataViewOptions().menuCheckToogleTAZDrawFill->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;

            case MID_GNE_DATAVIEWOPTIONS_TAZRELONLYFROM:
                if (myViewNet->getDataViewOptions().menuCheckToogleTAZRelOnlyFrom->amChecked()) {
                    menuCheck->setCheck(TRUE);
                } else {
                    menuCheck->setCheck(FALSE);
                }
                break;
            case MID_GNE_DATAVIEWOPTIONS_TAZRELONLYTO:
                if (myViewNet->getDataViewOptions().menuCheckToogleTAZRelOnlyTo->amChecked()) {
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
GNEApplicationWindow::onCmdSaveAllElements(FXObject*, FXSelector, void*) {
    // save all elements
    if (!myNet->isNetSaved()) {
        onCmdSaveNetwork(nullptr, 0, nullptr);
    }
    if (!myNet->isAdditionalsSaved()) {
        onCmdSaveAdditionals(nullptr, 0, nullptr);
    }
    if (!myNet->isDemandElementsSaved()) {
        onCmdSaveDemandElements(nullptr, 0, nullptr);
    }
    if (!myNet->isDataElementsSaved()) {
        onCmdSaveDataElements(nullptr, 0, nullptr);
    }
    return 1;
}


long
GNEApplicationWindow::onCmdSaveNetwork(FXObject*, FXSelector, void*) {
    OptionsCont& oc = OptionsCont::getOptions();
    // function onCmdSaveAsNetwork must be executed if this is the first save
    if (oc.getString("output-file") == "" || oc.isDefault("output-file")) {
        return onCmdSaveAsNetwork(nullptr, 0, nullptr);
    } else {
        getApp()->beginWaitCursor();
        try {
            myNet->save(oc);
        } catch (IOError& e) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Opening FXMessageBox 'error saving network'");
            // open error message box
            FXMessageBox::error(this, MBOX_OK, "Saving Network failed!", "%s", e.what());
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox 'error saving network' with 'OK'");
        }
        myMessageWindow->appendMsg(GUIEventType::MESSAGE_OCCURRED, "Network saved in " + oc.getString("output-file") + ".\n");
        // After saving a net successfully, add it into Recent Nets list.
        myMenuBarFile.myRecentNetsAndConfigs.appendFile(oc.getString("output-file").c_str());
        myMessageWindow->addSeparator();
        getApp()->endWaitCursor();
        // restore focus
        setFocus();
        return 1;
    }
}


long
GNEApplicationWindow::onCmdSaveTLSPrograms(FXObject*, FXSelector, void*) {
    // obtain option container
    OptionsCont& oc = OptionsCont::getOptions();
    // check if save additional menu is enabled
    if (myFileMenuCommands.saveTLSPrograms->isEnabled()) {
        // Check if TLS Programs file was already set at start of netedit or with a previous save
        if (oc.getString("TLSPrograms-output").empty()) {
            // declare current folder
            FXString currentFolder = gCurrentFolder;
            // check if there is a saved network
            if (oc.getString("output-file").size() > 0) {
                // extract folder
                currentFolder = getFolder(oc.getString("output-file"));
            }
            // open dialog
            FXString file = MFXUtils::getFilename2Write(this,
                            "Save TLS Programs", ".xml",
                            GUIIconSubSys::getIcon(GUIIcon::MODETLS),
                            currentFolder);
            // add xml extension
            std::string fileWithExtension = FileHelpers::addExtension(file.text(), ".xml");
            // check tat file is valid
            if (file == "") {
                // None TLS Programs file was selected, then stop function
                return 0;
            } else {
                // change value of "TLSPrograms-output"
                oc.resetWritable();
                oc.set("TLSPrograms-output", fileWithExtension);
            }
        }
        // Start saving TLS Programs
        getApp()->beginWaitCursor();
        try {
            myNet->computeNetwork(this, true); // GNEChange_TLS does not triggere GNENet:requireRecompute
            myNet->saveTLSPrograms(oc.getString("TLSPrograms-output"));
            myMessageWindow->appendMsg(GUIEventType::MESSAGE_OCCURRED, "TLS Programs saved in " + oc.getString("TLSPrograms-output") + ".\n");
            myFileMenuCommands.saveTLSPrograms->disable();
        } catch (IOError& e) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Opening FXMessageBox 'error saving TLS Programs'");
            // open error message box
            FXMessageBox::error(this, MBOX_OK, "Saving TLS Programs failed!", "%s", e.what());
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox 'error saving TLS Programs' with 'OK'");
        }
        myMessageWindow->addSeparator();
        getApp()->endWaitCursor();
        // restore focus
        setFocus();
        return 1;
    } else {
        return 0;
    }
}


long
GNEApplicationWindow::onUpdSaveTLSPrograms(FXObject* sender, FXSelector, void*) {
    // check if net exist and there is junctions
    if (myNet && (myNet->getAttributeCarriers()->getJunctions().size() > 0)) {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
    return 1;
}


long
GNEApplicationWindow::onCmdSaveEdgeTypes(FXObject*, FXSelector, void*) {
    // obtain option container
    OptionsCont& oc = OptionsCont::getOptions();
    // check if save additional menu is enabled
    if (myFileMenuCommands.saveEdgeTypes->isEnabled()) {
        // Check if edgeType file was already set at start of netedit or with a previous save
        if (oc.getString("edgeTypes-output").empty()) {
            // declare current folder
            FXString currentFolder = gCurrentFolder;
            // check if there is a saved network
            if (oc.getString("output-file").size() > 0) {
                // extract folder
                currentFolder = getFolder(oc.getString("output-file"));
            }
            // open dialog
            FXString file = MFXUtils::getFilename2Write(this,
                            "Save edgeType file", ".xml",
                            GUIIconSubSys::getIcon(GUIIcon::MODECREATEEDGE),
                            currentFolder);
            // add xml extension
            std::string fileWithExtension = FileHelpers::addExtension(file.text(), ".xml");
            // check tat file is valid
            if (file == "") {
                // None edgeType Programs file was selected, then stop function
                return 0;
            } else {
                // change value of "edgeTypes-output"
                oc.resetWritable();
                oc.set("edgeTypes-output", fileWithExtension);
            }
        }
        // Start saving edgeTypes
        getApp()->beginWaitCursor();
        try {
            myNet->saveEdgeTypes(oc.getString("edgeTypes-output"));
            myMessageWindow->appendMsg(GUIEventType::MESSAGE_OCCURRED, "EdgeType saved in " + oc.getString("edgeTypes-output") + ".\n");
            myFileMenuCommands.saveEdgeTypes->disable();
        } catch (IOError& e) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Opening FXMessageBox 'error saving edgeTypes'");
            // open error message box
            FXMessageBox::error(this, MBOX_OK, "Saving edgeTypes failed!", "%s", e.what());
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox 'error saving edgeTypes' with 'OK'");
        }
        myMessageWindow->addSeparator();
        getApp()->endWaitCursor();
        // restore focus
        setFocus();
        return 1;
    } else {
        return 0;
    }
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
    // obtain option container
    OptionsCont& oc = OptionsCont::getOptions();
    // declare current folder
    FXString currentFolder = gCurrentFolder;
    // check if there is a saved network
    if (oc.getString("output-file").size() > 0) {
        // extract folder
        currentFolder = getFolder(oc.getString("output-file"));
    }
    // Open window to select TLS Programs file
    FXString file = MFXUtils::getFilename2Write(this,
                    "Save TLS Programs as", ".xml",
                    GUIIconSubSys::getIcon(GUIIcon::MODETLS),
                    currentFolder);
    // add xml extension
    std::string fileWithExtension = FileHelpers::addExtension(file.text(), ".xml");
    // check tat file is valid
    if (fileWithExtension != "") {
        // change value of "TLSPrograms-output"
        OptionsCont::getOptions().resetWritable();
        OptionsCont::getOptions().set("TLSPrograms-output", fileWithExtension);
        // save TLS Programs
        return onCmdSaveTLSPrograms(nullptr, 0, nullptr);
    } else {
        return 1;
    }
}


long
GNEApplicationWindow::onCmdSaveEdgeTypesAs(FXObject*, FXSelector, void*) {
    // obtain option container
    OptionsCont& oc = OptionsCont::getOptions();
    // declare current folder
    FXString currentFolder = gCurrentFolder;
    // check if there is a saved network
    if (oc.getString("output-file").size() > 0) {
        // extract folder
        currentFolder = getFolder(oc.getString("output-file"));
    }
    // Open window to select edgeType file
    FXString file = MFXUtils::getFilename2Write(this,
                    "Save edgeType file as", ".xml",
                    GUIIconSubSys::getIcon(GUIIcon::MODECREATEEDGE),
                    currentFolder);
    // add xml extension
    std::string fileWithExtension = FileHelpers::addExtension(file.text(), ".xml");
    // check tat file is valid
    if (fileWithExtension != "") {
        // change value of "edgeTypes-output"
        OptionsCont::getOptions().set("edgeTypes-output", fileWithExtension);
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
    // write debug information
    WRITE_DEBUG("Open additional dialog");
    // get the Additional file name
    FXFileDialog opendialog(this, "Open Additionals file");
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::MODEADDITIONAL));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("XML files (*.xml)\nAll files (*)");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        // close additional dialog
        WRITE_DEBUG("Close additional dialog");
        // save previous status save
        const bool requireSaveAdditionals = !myNet->isAdditionalsSaved();
        const bool requireSaveDemandElements = !myNet->isDemandElementsSaved();
        const bool requireSaveDataElements = !myNet->isDataElementsSaved();
        // udpate current folder
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();
        // disable validation for additionals
        XMLSubSys::setValidation("never", "auto", "auto");
        // Create additional handler
        GNEGeneralHandler generalHandler(myNet, file, true);
        // begin undoList operation
        myUndoList->begin(Supermode::NETWORK, GUIIcon::SUPERMODENETWORK, "reloading additionals from '" + file + "'");
        // Run parser
        if (!generalHandler.parse()) {
            WRITE_ERROR("Loading of " + file + " failed.");
        }
        // end undoList operation and update view
        myUndoList->end();
        update();
        // restore validation for additionals
        XMLSubSys::setValidation("auto", "auto", "auto");
        // update require save additional elements
        myNet->requireSaveAdditionals(requireSaveAdditionals);
        myNet->requireSaveDemandElements(requireSaveDemandElements);
        myNet->requireSaveDataElements(requireSaveDataElements);
    } else {
        // write debug information
        WRITE_DEBUG("Cancel additional dialog");
    }
    return 1;
}


long
GNEApplicationWindow::onCmdReloadAdditionals(FXObject*, FXSelector, void*) {
    // get file
    const std::string file = OptionsCont::getOptions().getString("additional-files");
    // disable validation for additionals
    XMLSubSys::setValidation("never", "auto", "auto");
    // Create general handler
    GNEGeneralHandler generalHandler(myNet, file, true);
    // begin undoList operation
    myUndoList->begin(Supermode::DEMAND, GUIIcon::SUPERMODENETWORK, "reloading additionals from '" + file + "'");
    // clear additionals
    myNet->clearAdditionalElements(myUndoList);
    // Run parser
    if (!generalHandler.parse()) {
        WRITE_ERROR("Reloading of " + file + " failed.");
    }
    // end undoList operation and update view
    myUndoList->end();
    update();
    // restore validation for additionals
    XMLSubSys::setValidation("auto", "auto", "auto");
    return 1;
}


long
GNEApplicationWindow::onUpdReloadAdditionals(FXObject*, FXSelector, void*) {
    // check if file exist
    if (myViewNet && OptionsCont::getOptions().getString("additional-files").empty()) {
        return myFileMenuCommands.reloadAdditionals->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else {
        return myFileMenuCommands.reloadAdditionals->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    }
}


long
GNEApplicationWindow::onCmdSaveAdditionals(FXObject*, FXSelector, void*) {
    // obtain option container
    OptionsCont& oc = OptionsCont::getOptions();
    // check if save additional menu is enabled
    if (myFileMenuCommands.saveAdditionals->isEnabled()) {
        // Check if additionals file was already set at start of netedit or with a previous save
        if (oc.getString("additional-files").empty()) {
            // declare current folder
            FXString currentFolder = gCurrentFolder;
            // check if there is a saved network
            if (oc.getString("output-file").size() > 0) {
                // extract folder
                currentFolder = getFolder(oc.getString("output-file"));
            }
            // open dialog
            FXString file = MFXUtils::getFilename2Write(this,
                            "Save Additionals file", ".xml",
                            GUIIconSubSys::getIcon(GUIIcon::MODEADDITIONAL),
                            currentFolder);
            // add xml extension
            std::string fileWithExtension = FileHelpers::addExtension(file.text(), ".xml");
            // check tat file is valid
            if (fileWithExtension != "") {
                // change value of "additional-files"
                oc.resetWritable();
                oc.set("additional-files", fileWithExtension);
            } else {
                // None additionals file was selected, then stop function
                return 0;
            }
        }
        // Start saving additionals
        getApp()->beginWaitCursor();
        try {
            // compute before saving (for detectors positions)
            myNet->computeNetwork(this);
            myNet->saveAdditionals(oc.getString("additional-files"));
            myMessageWindow->appendMsg(GUIEventType::MESSAGE_OCCURRED, "Additionals saved in " + oc.getString("additional-files") + ".\n");
            myFileMenuCommands.saveAdditionals->disable();
        } catch (IOError& e) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Opening FXMessageBox 'error saving additionals'");
            // open error message box
            FXMessageBox::error(this, MBOX_OK, "Saving additionals failed!", "%s", e.what());
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox 'error saving additionals' with 'OK'");
        }
        myMessageWindow->addSeparator();
        getApp()->endWaitCursor();
        // restore focus
        setFocus();
        return 1;
    } else {
        return 0;
    }
}


long
GNEApplicationWindow::onCmdSaveAdditionalsAs(FXObject*, FXSelector, void*) {
    // obtain option container
    OptionsCont& oc = OptionsCont::getOptions();
    // declare current folder
    FXString currentFolder = gCurrentFolder;
    // check if there is a saved network
    if (oc.getString("output-file").size() > 0) {
        // extract folder
        currentFolder = getFolder(oc.getString("output-file"));
    }
    // Open window to select additional file
    FXString file = MFXUtils::getFilename2Write(this,
                    "Save Additionals file as", ".xml",
                    GUIIconSubSys::getIcon(GUIIcon::MODEADDITIONAL),
                    currentFolder);
    // add xml extension
    std::string fileWithExtension = FileHelpers::addExtension(file.text(), ".xml");
    // check tat file is valid
    if (fileWithExtension != "") {
        // reset writtable flag
        OptionsCont::getOptions().resetWritable();
        // change value of "additional-files"
        OptionsCont::getOptions().set("additional-files", fileWithExtension);
        // change flag of menu command for save additionals
        myFileMenuCommands.saveAdditionals->enable();
        // save additionals
        return onCmdSaveAdditionals(nullptr, 0, nullptr);
    } else {
        return 1;
    }
}


long
GNEApplicationWindow::onCmdOpenDemandElements(FXObject*, FXSelector, void*) {
    // write debug information
    WRITE_DEBUG("Open demand element dialog");
    // get the demand element file name
    FXFileDialog opendialog(this, "Open demand element file");
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDEMAND));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("XML files (*.xml)\nDemand files (*rou.xml)\nAll files (*)");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        // close additional dialog
        WRITE_DEBUG("Close demand element dialog");
        // save previous status save
        const bool requireSaveAdditionals = !myNet->isAdditionalsSaved();
        const bool requireSaveDemandElements = !myNet->isDemandElementsSaved();
        const bool requireSaveDataElements = !myNet->isDataElementsSaved();
        // udpate current folder
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();
        // disable validation for additionals
        XMLSubSys::setValidation("never", "auto", "auto");
        // Create generic handler
        GNEGeneralHandler handler(myNet, file, true);
        // begin undoList operation
        myUndoList->begin(Supermode::DEMAND, GUIIcon::SUPERMODEDEMAND, "loading demand elements from '" + file + "'");
        // Run parser for additionals
        if (!handler.parse()) {
            WRITE_ERROR("Loading of " + file + " failed.");
        }
        // end undoList operation and update view
        myUndoList->end();
        update();
        // restore validation for demand
        XMLSubSys::setValidation("auto", "auto", "auto");
        // update require save additional elements
        myNet->requireSaveAdditionals(requireSaveAdditionals);
        myNet->requireSaveDemandElements(requireSaveDemandElements);
        myNet->requireSaveDataElements(requireSaveDataElements);
    } else {
        // write debug information
        WRITE_DEBUG("Cancel demand element dialog");
    }
    return 1;
}


long
GNEApplicationWindow::onCmdReloadDemandElements(FXObject*, FXSelector, void*) {
    // get file
    const std::string file = OptionsCont::getOptions().getString("route-files");
    // disable validation for additionals
    XMLSubSys::setValidation("never", "auto", "auto");
    // Create handler
    GNEGeneralHandler handler(myNet, file, true);
    // begin undoList operation
    myUndoList->begin(Supermode::DEMAND, GUIIcon::SUPERMODEDEMAND, "reloading demand elements from '" + file + "'");
    // clear demand elements
    myNet->clearDemandElements(myUndoList);
    // Run parser for additionals
    if (!handler.parse()) {
        WRITE_ERROR("Reloading of " + file + " failed.");
    }
    // end undoList operation and update view
    myUndoList->end();
    update();
    // restore validation for demand
    XMLSubSys::setValidation("auto", "auto", "auto");
    return 1;
}


long
GNEApplicationWindow::onUpdReloadDemandElements(FXObject*, FXSelector, void*) {
    // check if file exist
    if (myViewNet && OptionsCont::getOptions().getString("route-files").empty()) {
        return myFileMenuCommands.reloadDemandElements->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else {
        return myFileMenuCommands.reloadDemandElements->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    }
}


long
GNEApplicationWindow::onCmdSaveDemandElements(FXObject*, FXSelector, void*) {
    // obtain option container
    OptionsCont& oc = OptionsCont::getOptions();
    // check if save demand element menu is enabled
    if (myFileMenuCommands.saveDemandElements->isEnabled()) {
        // Check if demand elements file was already set at start of netedit or with a previous save
        if (oc.getString("route-files").empty()) {
            // declare current folder
            FXString currentFolder = gCurrentFolder;
            // check if there is a saved network
            if (oc.getString("output-file").size() > 0) {
                // extract folder
                currentFolder = getFolder(oc.getString("output-file"));
            }
            // open dialog
            FXString file = MFXUtils::getFilename2Write(this,
                            "Save demand element file", ".rou.xml",
                            GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDEMAND),
                            currentFolder);
            // add xml extension
            std::string fileWithExtension = FileHelpers::addExtension(file.text(), ".rou.xml");
            // check tat file is valid
            if (fileWithExtension != "") {
                // change value of "route-files"
                oc.resetWritable();
                oc.set("route-files", fileWithExtension);
            } else {
                // None demand elements file was selected, then stop function
                return 0;
            }
        }
        // Start saving demand elements
        getApp()->beginWaitCursor();
        try {
            myNet->saveDemandElements(oc.getString("route-files"));
            myMessageWindow->appendMsg(GUIEventType::MESSAGE_OCCURRED, "Demand elements saved in " + oc.getString("route-files") + ".\n");
            myFileMenuCommands.saveDemandElements->disable();
        } catch (IOError& e) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Opening FXMessageBox 'error saving demand elements'");
            // open error message box
            FXMessageBox::error(this, MBOX_OK, "Saving demand elements failed!", "%s", e.what());
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox 'error saving demand elements' with 'OK'");
        }
        myMessageWindow->addSeparator();
        getApp()->endWaitCursor();
        // restore focus
        setFocus();
        return 1;
    } else {
        return 0;
    }
}


long
GNEApplicationWindow::onCmdSaveDemandElementsAs(FXObject*, FXSelector, void*) {
    // obtain option container
    OptionsCont& oc = OptionsCont::getOptions();
    // declare current folder
    FXString currentFolder = gCurrentFolder;
    // check if there is a saved network
    if (oc.getString("output-file").size() > 0) {
        // extract folder
        currentFolder = getFolder(oc.getString("output-file"));
    }
    // Open window to select additionasl file
    FXString file = MFXUtils::getFilename2Write(this,
                    "Save demand element file as", ".rou.xml",
                    GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDEMAND),
                    currentFolder);
    // add xml extension
    std::string fileWithExtension = FileHelpers::addExtension(file.text(), ".rou.xml");
    // check that file is correct
    if (fileWithExtension != "") {
        // reset writtable flag
        OptionsCont::getOptions().resetWritable();
        // change value of "route-files"
        OptionsCont::getOptions().set("route-files", fileWithExtension);
        // change flag of menu command for save demand elements
        myFileMenuCommands.saveDemandElements->enable();
        // save demand elements
        return onCmdSaveDemandElements(nullptr, 0, nullptr);
    } else {
        return 1;
    }
}


long
GNEApplicationWindow::onCmdOpenDataElements(FXObject*, FXSelector, void*) {
    // write debug information
    WRITE_DEBUG("Open data element dialog");
    // get the data element file name
    FXFileDialog opendialog(this, "Open data element file");
    opendialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDATA));
    opendialog.setSelectMode(SELECTFILE_EXISTING);
    opendialog.setPatternList("XML files (*.xml)\nData files (*dat.xml)\nAll files (*)");
    if (gCurrentFolder.length() != 0) {
        opendialog.setDirectory(gCurrentFolder);
    }
    if (opendialog.execute()) {
        // close additional dialog
        WRITE_DEBUG("Close data element dialog");
        // save previous status save
        const bool requireSaveAdditionals = !myNet->isAdditionalsSaved();
        const bool requireSaveDemandElements = !myNet->isDemandElementsSaved();
        const bool requireSaveDataElements = !myNet->isDataElementsSaved();
        // udpate current folder
        gCurrentFolder = opendialog.getDirectory();
        std::string file = opendialog.getFilename().text();
        // disable update data
        myViewNet->getNet()->disableUpdateData();
        // disable validation for data elements
        XMLSubSys::setValidation("never", "auto", "auto");
        // Create data handler
        GNEDataHandler dataHandler(myNet, file, true);
        // begin undoList operation
        myUndoList->begin(Supermode::DATA, GUIIcon::SUPERMODEDATA, "loading data elements from '" + file + "'");
        // Run data parser
        if (!dataHandler.parse()) {
            WRITE_ERROR("Loading of " + file + " failed.");
        }
        // end undoList operation and update view
        myUndoList->end();
        // enable update data
        myViewNet->getNet()->enableUpdateData();
        // update
        update();
        // restore validation for data
        XMLSubSys::setValidation("auto", "auto", "auto");
        // update require save additional elements
        myNet->requireSaveAdditionals(requireSaveAdditionals);
        myNet->requireSaveDemandElements(requireSaveDemandElements);
        myNet->requireSaveDataElements(requireSaveDataElements);
    } else {
        // write debug information
        WRITE_DEBUG("Cancel data element dialog");
    }
    return 1;
}


long
GNEApplicationWindow::onCmdReloadDataElements(FXObject*, FXSelector, void*) {
    // get file
    const std::string file = OptionsCont::getOptions().getString("data-files");
    // disable update data
    myViewNet->getNet()->disableUpdateData();
    // disable validation for additionals
    XMLSubSys::setValidation("never", "auto", "auto");
    // Create additional handler
    GNEDataHandler dataHandler(myNet, file, true);
    // begin undoList operation
    myUndoList->begin(Supermode::DATA, GUIIcon::SUPERMODEDATA, "reloading data elements from '" + file + "'");
    // clear data elements
    myNet->clearDemandElements(myUndoList);
    // Run data parser
    if (!dataHandler.parse()) {
        WRITE_ERROR("Reloading of " + file + " failed.");
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
GNEApplicationWindow::onUpdReloadDataElements(FXObject*, FXSelector, void*) {
    // check if file exist
    if (myViewNet && OptionsCont::getOptions().getString("data-files").empty()) {
        return myFileMenuCommands.reloadDataElements->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    } else {
        return myFileMenuCommands.reloadDataElements->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    }
}


long
GNEApplicationWindow::onCmdSaveDataElements(FXObject*, FXSelector, void*) {
    // obtain option container
    OptionsCont& oc = OptionsCont::getOptions();
    // check if save data element menu is enabled
    if (myFileMenuCommands.saveDataElements->isEnabled()) {
        // Check if data elements file was already set at start of netedit or with a previous save
        if (oc.getString("data-files").empty()) {
            // declare current folder
            FXString currentFolder = gCurrentFolder;
            // check if there is a saved network
            if (oc.getString("output-file").size() > 0) {
                // extract folder
                currentFolder = getFolder(oc.getString("output-file"));
            }
            // open dialog
            FXString file = MFXUtils::getFilename2Write(this,
                            "Save data element file", ".xml",
                            GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDATA),
                            currentFolder);
            // add xml extension
            std::string fileWithExtension = FileHelpers::addExtension(file.text(), ".xml");
            // check tat file is valid
            if (fileWithExtension != "") {
                // change value of "data-files"
                oc.resetWritable();
                oc.set("data-files", fileWithExtension);
            } else {
                // None data elements file was selected, then stop function
                return 0;
            }
        }
        // Start saving data elements
        getApp()->beginWaitCursor();
        try {
            myNet->saveDataElements(oc.getString("data-files"));
            myMessageWindow->appendMsg(GUIEventType::MESSAGE_OCCURRED, "Data elements saved in " + oc.getString("data-files") + ".\n");
            myFileMenuCommands.saveDataElements->disable();
        } catch (IOError& e) {
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Opening FXMessageBox 'error saving data elements'");
            // open error message box
            FXMessageBox::error(this, MBOX_OK, "Saving data elements failed!", "%s", e.what());
            // write warning if netedit is running in testing mode
            WRITE_DEBUG("Closed FXMessageBox 'error saving data elements' with 'OK'");
        }
        myMessageWindow->addSeparator();
        getApp()->endWaitCursor();
        // restore focus
        setFocus();
        return 1;
    } else {
        return 0;
    }
}


long
GNEApplicationWindow::onCmdSaveDataElementsAs(FXObject*, FXSelector, void*) {
    // obtain option container
    OptionsCont& oc = OptionsCont::getOptions();
    // declare current folder
    FXString currentFolder = gCurrentFolder;
    // check if there is a saved network
    if (oc.getString("output-file").size() > 0) {
        // extract folder
        currentFolder = getFolder(oc.getString("output-file"));
    }
    // Open window to select additionasl file
    FXString file = MFXUtils::getFilename2Write(this,
                    "Save data element file as", ".xml",
                    GUIIconSubSys::getIcon(GUIIcon::SUPERMODEDATA),
                    currentFolder);
    // add xml extension
    std::string fileWithExtension = FileHelpers::addExtension(file.text(), ".xml");
    // check that file is correct
    if (fileWithExtension != "") {
        // reset writtable flag
        OptionsCont::getOptions().resetWritable();
        // change value of "data-files"
        OptionsCont::getOptions().set("data-files", fileWithExtension);
        // change flag of menu command for save data elements
        myFileMenuCommands.saveDataElements->enable();
        // save data elements
        return onCmdSaveDataElements(nullptr, 0, nullptr);
    } else {
        return 1;
    }
}


bool
GNEApplicationWindow::continueWithUnsavedChanges(const std::string& operation) {
    FXuint answer = 0;
    if (myViewNet && myNet && !myNet->isNetSaved()) {
        // write warning if netedit is running in testing mode
        WRITE_DEBUG("Opening FXMessageBox 'Confirm " + operation + " network'");
        // open question box
        answer = FXMessageBox::question(getApp(), MBOX_QUIT_SAVE_CANCEL,
                                        ("Confirm " + operation + " Network").c_str(), "%s",
                                        ("You have unsaved changes in the network. Do you wish to " + operation + " and discard all changes?").c_str());
        // restore focus to view net
        myViewNet->setFocus();
        // if user close dialog box, check additionals and demand elements
        if (answer == MBOX_CLICKED_QUIT) {
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
        } else if (answer == MBOX_CLICKED_SAVE) {
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
    if (myViewNet && myFileMenuCommands.saveAdditionals->isEnabled()) {
        WRITE_DEBUG("Opening FXMessageBox 'Save additionals before " + operation + "'");
        // open question box
        FXuint answer = FXMessageBox::question(getApp(), MBOX_QUIT_SAVE_CANCEL,
                                               ("Save additionals before " + operation).c_str(), "%s",
                                               ("You have unsaved additionals. Do you wish to " + operation + " and discard all changes?").c_str());
        // restore focus to view net
        myViewNet->setFocus();
        // if answer was affirmative, but there was an error during saving additionals, return false to stop closing/reloading
        if (answer == MBOX_CLICKED_QUIT) {
            WRITE_DEBUG("Closed FXMessageBox 'Save additionals before " + operation + "' with 'Quit'");
            // nothing to save, return true
            return true;
        } else if (answer == MBOX_CLICKED_SAVE) {
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
    if (myViewNet && myFileMenuCommands.saveDemandElements->isEnabled()) {
        WRITE_DEBUG("Opening FXMessageBox 'Save demand elements before " + operation + "'");
        // open question box
        FXuint answer = FXMessageBox::question(getApp(), MBOX_QUIT_SAVE_CANCEL,
                                               ("Save demand elements before " + operation).c_str(), "%s",
                                               ("You have unsaved demand elements. Do you wish to " + operation + " and discard all changes?").c_str());
        // restore focus to view net
        myViewNet->setFocus();
        // if answer was affirmative, but there was an error during saving demand elements, return false to stop closing/reloading
        if (answer == MBOX_CLICKED_QUIT) {
            WRITE_DEBUG("Closed FXMessageBox 'Save demand elements before " + operation + "' with 'Quit'");
            // nothing to save, return true
            return true;
        } else if (answer == MBOX_CLICKED_SAVE) {
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
    if (myViewNet && myFileMenuCommands.saveDataElements->isEnabled()) {
        WRITE_DEBUG("Opening FXMessageBox 'Save data elements before " + operation + "'");
        // open question box
        FXuint answer = FXMessageBox::question(getApp(), MBOX_QUIT_SAVE_CANCEL,
                                               ("Save data elements before " + operation).c_str(), "%s",
                                               ("You have unsaved data elements. Do you wish to " + operation + " and discard all changes?").c_str());
        // restore focus to view net
        myViewNet->setFocus();
        // if answer was affirmative, but there was an error during saving data elements, return false to stop closing/reloading
        if (answer == MBOX_CLICKED_QUIT) {
            WRITE_DEBUG("Closed FXMessageBox 'Save data elements before " + operation + "' with 'Quit'");
            // nothing to save, return true
            return true;
        } else if (answer == MBOX_CLICKED_SAVE) {
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
