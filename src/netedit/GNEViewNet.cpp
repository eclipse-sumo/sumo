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
/// @file    GNEViewNet.cpp
/// @author  Jakob Erdmann
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2011
///
// A view on the network being edited (adapted from GUIViewTraffic)
/****************************************************************************/
#include <netbuild/NBEdgeCont.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/dialogs/GNEGeometryPointDialog.h>
#include <netedit/elements/additional/GNEPOI.h>
#include <netedit/elements/additional/GNEPoly.h>
#include <netedit/elements/additional/GNETAZ.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/elements/network/GNECrossing.h>
#include <netedit/frames/common/GNEDeleteFrame.h>
#include <netedit/frames/common/GNESelectorFrame.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <netedit/frames/data/GNEEdgeDataFrame.h>
#include <netedit/frames/data/GNEEdgeRelDataFrame.h>
#include <netedit/frames/data/GNETAZRelDataFrame.h>
#include <netedit/frames/demand/GNEContainerFrame.h>
#include <netedit/frames/demand/GNEContainerPlanFrame.h>
#include <netedit/frames/demand/GNEPersonFrame.h>
#include <netedit/frames/demand/GNEPersonPlanFrame.h>
#include <netedit/frames/demand/GNEPersonTypeFrame.h>
#include <netedit/frames/demand/GNERouteFrame.h>
#include <netedit/frames/demand/GNEStopFrame.h>
#include <netedit/frames/demand/GNEVehicleFrame.h>
#include <netedit/frames/demand/GNEVehicleTypeFrame.h>
#include <netedit/frames/network/GNEAdditionalFrame.h>
#include <netedit/frames/network/GNEConnectorFrame.h>
#include <netedit/frames/network/GNECreateEdgeFrame.h>
#include <netedit/frames/network/GNECrossingFrame.h>
#include <netedit/frames/network/GNEPolygonFrame.h>
#include <netedit/frames/network/GNEProhibitionFrame.h>
#include <netedit/frames/network/GNETAZFrame.h>
#include <netedit/frames/network/GNETLSEditorFrame.h>
#include <utils/foxtools/FXMenuCheckIcon.h>
#include <utils/gui/cursors/GUICursorSubSys.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIDanielPerspectiveChanger.h>
#include <utils/gui/windows/GUIDialog_ViewSettings.h>
#include <utils/options/OptionsCont.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNENet.h"
#include "GNEUndoList.h"
#include "GNEViewNet.h"
#include "GNEViewParent.h"
#include "GNEApplicationWindow.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEViewNet) GNEViewNetMap[] = {
    // Super Modes
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_F2_SUPERMODE_NETWORK,                 GNEViewNet::onCmdSetSupermode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_F3_SUPERMODE_DEMAND,                  GNEViewNet::onCmdSetSupermode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_F4_SUPERMODE_DATA,                    GNEViewNet::onCmdSetSupermode),
    // Modes
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_G_MODE_CONTAINER,                     GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_H_MODE_CONTAINERDATA,                 GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_A_MODES_ADDITIONAL_STOP,              GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_C_MODES_CONNECT_PERSONPLAN,           GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_D_MODES_DELETE,                       GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_E_MODES_EDGE_EDGEDATA,                GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_I_MODES_INSPECT,                      GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_M_MODES_MOVE,                         GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_P_MODES_POLYGON_PERSON,               GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_R_MODES_CROSSING_ROUTE_EDGERELDATA,   GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_S_MODES_SELECT,                       GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_T_MODES_TLS_VTYPE,                    GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_V_MODES_VEHICLE,                      GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_W_MODES_PROHIBITION_PERSONTYPE,       GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_Z_MODES_TAZ_TAZREL,                   GNEViewNet::onCmdSetMode),
    // Network view options
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_TOGGLEGRID,           GNEViewNet::onCmdToggleShowGrid),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_DRAWSPREADVEHICLES,   GNEViewNet::onCmdToggleDrawSpreadVehicles),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_SHOWDEMANDELEMENTS,   GNEViewNet::onCmdToggleShowDemandElementsNetwork),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_SELECTEDGES,          GNEViewNet::onCmdToggleSelectEdges),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_SHOWCONNECTIONS,      GNEViewNet::onCmdToggleShowConnections),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_SHOWCONNECTIONS,      GNEViewNet::onCmdToggleHideConnections),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_EXTENDSELECTION,      GNEViewNet::onCmdToggleExtendSelection),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_CHANGEALLPHASES,      GNEViewNet::onCmdToggleChangeAllPhases),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_ASKFORMERGE,          GNEViewNet::onCmdToggleWarnAboutMerge),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_SHOWBUBBLES,          GNEViewNet::onCmdToggleShowJunctionBubbles),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_MOVEELEVATION,        GNEViewNet::onCmdToggleMoveElevation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_CHAINEDGES,           GNEViewNet::onCmdToggleChainEdges),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_AUTOOPPOSITEEDGES,    GNEViewNet::onCmdToggleAutoOppositeEdge),
    // Demand view options
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_SHOWGRID,              GNEViewNet::onCmdToggleShowGrid),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_DRAWSPREADVEHICLES,    GNEViewNet::onCmdToggleDrawSpreadVehicles),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_HIDENONINSPECTED,      GNEViewNet::onCmdToggleHideNonInspecteDemandElements),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_HIDESHAPES,            GNEViewNet::onCmdToggleHideShapes),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_SHOWTRIPS,             GNEViewNet::onCmdToggleShowTrips),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_SHOWALLPERSONPLANS,    GNEViewNet::onCmdToggleShowAllPersonPlans),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_LOCKPERSON,            GNEViewNet::onCmdToggleLockPerson),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_SHOWALLCONTAINERPLANS, GNEViewNet::onCmdToggleShowAllContainerPlans),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_LOCKCONTAINER,         GNEViewNet::onCmdToggleLockContainer),
    // Data view options
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DATAVIEWOPTIONS_SHOWADDITIONALS,         GNEViewNet::onCmdToggleShowAdditionals),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DATAVIEWOPTIONS_SHOWSHAPES,              GNEViewNet::onCmdToggleShowShapes),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DATAVIEWOPTIONS_SHOWDEMANDELEMENTS,      GNEViewNet::onCmdToggleShowDemandElementsData),
    // Select elements
    FXMAPFUNC(SEL_COMMAND, MID_ADDSELECT,                                   GNEViewNet::onCmdAddSelected),
    FXMAPFUNC(SEL_COMMAND, MID_REMOVESELECT,                                GNEViewNet::onCmdRemoveSelected),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_ADDSELECT_EDGE,                          GNEViewNet::onCmdAddEdgeSelected),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_REMOVESELECT_EDGE,                       GNEViewNet::onCmdRemoveEdgeSelected),
    // Junctions
    FXMAPFUNC(SEL_COMMAND, MID_GNE_JUNCTION_EDIT_SHAPE,                     GNEViewNet::onCmdEditJunctionShape),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_JUNCTION_RESET_SHAPE,                    GNEViewNet::onCmdResetJunctionShape),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_JUNCTION_REPLACE,                        GNEViewNet::onCmdReplaceJunction),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_JUNCTION_SPLIT,                          GNEViewNet::onCmdSplitJunction),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_JUNCTION_SPLIT_RECONNECT,                GNEViewNet::onCmdSplitJunctionReconnect),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_JUNCTION_SELECT_ROUNDABOUT,              GNEViewNet::onCmdSelectRoundabout),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_JUNCTION_CONVERT_ROUNDABOUT,             GNEViewNet::onCmdConvertRoundabout),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_JUNCTION_CLEAR_CONNECTIONS,              GNEViewNet::onCmdClearConnections),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_JUNCTION_RESET_CONNECTIONS,              GNEViewNet::onCmdResetConnections),
    // Connections
    FXMAPFUNC(SEL_COMMAND, MID_GNE_CONNECTION_EDIT_SHAPE,                   GNEViewNet::onCmdEditConnectionShape),
    // Crossings
    FXMAPFUNC(SEL_COMMAND, MID_GNE_CROSSING_EDIT_SHAPE,                     GNEViewNet::onCmdEditCrossingShape),
    // Edges
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_SPLIT,                              GNEViewNet::onCmdSplitEdge),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_SPLIT_BIDI,                         GNEViewNet::onCmdSplitEdgeBidi),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_REVERSE,                            GNEViewNet::onCmdReverseEdge),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_ADD_REVERSE,                        GNEViewNet::onCmdAddReversedEdge),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_EDIT_ENDPOINT,                      GNEViewNet::onCmdEditEdgeEndpoint),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_RESET_ENDPOINT,                     GNEViewNet::onCmdResetEdgeEndpoint),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_STRAIGHTEN,                         GNEViewNet::onCmdStraightenEdges),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_SMOOTH,                             GNEViewNet::onCmdSmoothEdges),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_STRAIGHTEN_ELEVATION,               GNEViewNet::onCmdStraightenEdgesElevation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_SMOOTH_ELEVATION,                   GNEViewNet::onCmdSmoothEdgesElevation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_RESET_LENGTH,                       GNEViewNet::onCmdResetLength),
    // Lanes
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_DUPLICATE,                          GNEViewNet::onCmdDuplicateLane),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_RESET_CUSTOMSHAPE,                  GNEViewNet::onCmdResetLaneCustomShape),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_RESET_OPPOSITELANE,                 GNEViewNet::onCmdResetOppositeLane),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_TRANSFORM_SIDEWALK,                 GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_TRANSFORM_BIKE,                     GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_TRANSFORM_BUS,                      GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_TRANSFORM_GREENVERGE,               GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_ADD_SIDEWALK,                       GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_ADD_BIKE,                           GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_ADD_BUS,                            GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_ADD_GREENVERGE_FRONT,               GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_ADD_GREENVERGE_BACK,                GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_REMOVE_SIDEWALK,                    GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_REMOVE_BIKE,                        GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_REMOVE_BUS,                         GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_REMOVE_GREENVERGE,                  GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_REACHABILITY,                                GNEViewNet::onCmdLaneReachability),
    // Additionals
    FXMAPFUNC(SEL_COMMAND, MID_OPEN_ADDITIONAL_DIALOG,                      GNEViewNet::onCmdOpenAdditionalDialog),
    // Polygons
    FXMAPFUNC(SEL_COMMAND, MID_GNE_POLYGON_SIMPLIFY_SHAPE,                  GNEViewNet::onCmdSimplifyShape),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_POLYGON_CLOSE,                           GNEViewNet::onCmdClosePolygon),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_POLYGON_OPEN,                            GNEViewNet::onCmdOpenPolygon),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_POLYGON_SET_FIRST_POINT,                 GNEViewNet::onCmdSetFirstGeometryPoint),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_POLYGON_DELETE_GEOMETRY_POINT,           GNEViewNet::onCmdDeleteGeometryPoint),
    // POIs
    FXMAPFUNC(SEL_COMMAND, MID_GNE_POI_TRANSFORM,                           GNEViewNet::onCmdTransformPOI),
    // Geometry Points
    FXMAPFUNC(SEL_COMMAND, MID_GNE_CUSTOM_GEOMETRYPOINT,                    GNEViewNet::onCmdSetCustomGeometryPoint),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_RESET_GEOMETRYPOINT,                     GNEViewNet::onCmdResetEndPoints),
    // IntervalBar
    FXMAPFUNC(SEL_COMMAND, MID_GNE_INTERVALBAR_GENERICDATATYPE,             GNEViewNet::onCmdIntervalBarGenericDataType),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_INTERVALBAR_DATASET,                     GNEViewNet::onCmdIntervalBarDataSet),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_INTERVALBAR_LIMITED,                     GNEViewNet::onCmdIntervalBarLimit),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_INTERVALBAR_BEGIN,                       GNEViewNet::onCmdIntervalBarSetBegin),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_INTERVALBAR_END,                         GNEViewNet::onCmdIntervalBarSetEnd),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_INTERVALBAR_ATTRIBUTE,                   GNEViewNet::onCmdIntervalBarSetAttribute)
};

// Object implementation
FXIMPLEMENT(GNEViewNet, GUISUMOAbstractView, GNEViewNetMap, ARRAYNUMBER(GNEViewNetMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEViewNet::GNEViewNet(FXComposite* tmpParent, FXComposite* actualParent, GUIMainWindow& app,
                       GNEViewParent* viewParent, GNENet* net, GNEUndoList* undoList,
                       FXGLVisual* glVis, FXGLCanvas* share) :
    GUISUMOAbstractView(tmpParent, app, viewParent, net->getGrid(), glVis, share),
    myEditModes(this),
    myTestingMode(this),
    myObjectsUnderCursor(this),
    myCommonCheckableButtons(this),
    myNetworkCheckableButtons(this),
    myDemandCheckableButtons(this),
    myDataCheckableButtons(this),
    myNetworkViewOptions(this),
    myDemandViewOptions(this),
    myDataViewOptions(this),
    myIntervalBar(this),
    myMoveSingleElementValues(this),
    myMoveMultipleElementValues(this),
    myVehicleOptions(this),
    myVehicleTypeOptions(this),
    mySaveElements(this),
    mySelectingArea(this),
    myEditNetworkElementShapes(this),
    myLockManager(this),
    myViewParent(viewParent),
    myNet(net),
    myCurrentFrame(nullptr),
    myUndoList(undoList),
    myFrontAttributeCarrier(nullptr) {
    // view must be the final member of actualParent
    reparent(actualParent);
    // Build edit modes
    buildEditModeControls();
    // Mark undo list
    myUndoList->mark();
    // set this net in Net
    myNet->setViewNet(this);
    // set drag delay
    ((GUIDanielPerspectiveChanger*)myChanger)->setDragDelay(100000000); // 100 milliseconds
    // Reset textures
    GUITextureSubSys::resetTextures();
    // init testing mode
    myTestingMode.initTestingMode();
}


GNEViewNet::~GNEViewNet() {}


void
GNEViewNet::doInit() {}


void
GNEViewNet::buildViewToolBars(GUIGlChildWindow* v) {
    // build coloring tools
    {
        for (auto it_names : gSchemeStorage.getNames()) {
            v->getColoringSchemesCombo()->appendItem(it_names.c_str());
            if (it_names == myVisualizationSettings->name) {
                v->getColoringSchemesCombo()->setCurrentItem(v->getColoringSchemesCombo()->getNumItems() - 1);
            }
        }
        v->getColoringSchemesCombo()->setNumVisible(MAX2(5, (int)gSchemeStorage.getNames().size() + 1));
    }
    // for junctions
    new FXButton(v->getLocatorPopup(),
                 "\tLocate Junctions\tLocate a junction within the network. (Shift+J)",
                 GUIIconSubSys::getIcon(GUIIcon::LOCATEJUNCTION), v, MID_LOCATEJUNCTION,
                 ICON_ABOVE_TEXT | FRAME_THICK | FRAME_RAISED);
    // for edges
    new FXButton(v->getLocatorPopup(),
                 "\tLocate Edges\tLocate an edge within the network. (Shift+E)",
                 GUIIconSubSys::getIcon(GUIIcon::LOCATEEDGE), v, MID_LOCATEEDGE,
                 ICON_ABOVE_TEXT | FRAME_THICK | FRAME_RAISED);

    // for vehicles
    new FXButton(v->getLocatorPopup(),
                 "\tLocate Vehicles\tLocate a vehicle within the network. (Shift+V)",
                 GUIIconSubSys::getIcon(GUIIcon::LOCATEVEHICLE), v, MID_LOCATEVEHICLE,
                 ICON_ABOVE_TEXT | FRAME_THICK | FRAME_RAISED);

    // for person
    new FXButton(v->getLocatorPopup(),
                 "\tLocate Persons\tLocate a person within the network. (Shift+P)",
                 GUIIconSubSys::getIcon(GUIIcon::LOCATEPERSON), v, MID_LOCATEPERSON,
                 ICON_ABOVE_TEXT | FRAME_THICK | FRAME_RAISED);

    // for routes
    new FXButton(v->getLocatorPopup(),
                 "\tLocate Route\tLocate a route within the network. (Shift+R)",
                 GUIIconSubSys::getIcon(GUIIcon::LOCATEROUTE), v, MID_LOCATEROUTE,
                 ICON_ABOVE_TEXT | FRAME_THICK | FRAME_RAISED);

    // for routes
    new FXButton(v->getLocatorPopup(),
                 "\tLocate Stops\tLocate a stop within the network. (Shift+S)",
                 GUIIconSubSys::getIcon(GUIIcon::LOCATESTOP), v, MID_LOCATESTOP,
                 ICON_ABOVE_TEXT | FRAME_THICK | FRAME_RAISED);

    // for persons (currently unused)
    /*
    new FXButton(v->getLocatorPopup(),
                 "\tLocate Vehicle\tLocate a person within the network.",
                 GUIIconSubSys::getIcon(GUIIcon::LOCATEPERSON), &v, MID_LOCATEPERSON,
                 ICON_ABOVE_TEXT | FRAME_THICK | FRAME_RAISED);
    */

    // for tls
    new FXButton(v->getLocatorPopup(),
                 "\tLocate TLS\tLocate a tls within the network. (Shift+T)",
                 GUIIconSubSys::getIcon(GUIIcon::LOCATETLS), v, MID_LOCATETLS,
                 ICON_ABOVE_TEXT | FRAME_THICK | FRAME_RAISED);
    // for additional stuff
    new FXButton(v->getLocatorPopup(),
                 "\tLocate Additional\tLocate an additional structure within the network. (Shift+A)",
                 GUIIconSubSys::getIcon(GUIIcon::LOCATEADD), v, MID_LOCATEADD,
                 ICON_ABOVE_TEXT | FRAME_THICK | FRAME_RAISED);
    // for pois
    new FXButton(v->getLocatorPopup(),
                 "\tLocate PoI\tLocate a PoI within the network. (Shift+O)",
                 GUIIconSubSys::getIcon(GUIIcon::LOCATEPOI), v, MID_LOCATEPOI,
                 ICON_ABOVE_TEXT | FRAME_THICK | FRAME_RAISED);
    // for polygons
    new FXButton(v->getLocatorPopup(),
                 "\tLocate Polygon\tLocate a Polygon within the network. (Shift+L)",
                 GUIIconSubSys::getIcon(GUIIcon::LOCATEPOLY), v, MID_LOCATEPOLY,
                 ICON_ABOVE_TEXT | FRAME_THICK | FRAME_RAISED);
}


void
GNEViewNet::updateViewNet() const {
    // this call is only used for breakpoints (to check when view is updated)
    GUISUMOAbstractView::update();
}


void
GNEViewNet::forceSupermodeNetwork() {
    myEditModes.setSupermode(Supermode::NETWORK, true);
}


std::set<std::pair<std::string, GNEAttributeCarrier*> >
GNEViewNet::getAttributeCarriersInBoundary(const Boundary& boundary, bool forceSelectEdges) {
    // use a SET of pairs to obtain IDs and Pointers to attribute carriers. We need this because certain ACs can be returned many times (example: Edges)
    // Note: a map cannot be used because there is different ACs with the same ID (example: Additionals)
    std::set<std::pair<std::string, GNEAttributeCarrier*> > result;
    // firstm make OpenGL context current prior to performing OpenGL commands
    if (makeCurrent()) {
        // obtain GUIGLIds of all objects in the given boundary (disabling drawForRectangleSelection)
        std::vector<GUIGlID> GLIds = getObjectsInBoundary(boundary, false);
        //  finish make OpenGL context current
        makeNonCurrent();
        // iterate over GUIGlIDs
        for (const auto& GLId : GLIds) {
            // avoid to select Net (i = 0)
            if (GLId != 0) {
                GNEAttributeCarrier* retrievedAC = myNet->retrieveAttributeCarrier(GLId);
                // in the case of a Lane, we need to change the retrieved lane to their the parent if myNetworkViewOptions.mySelectEdges is enabled
                if ((retrievedAC->getTagProperty().getTag() == SUMO_TAG_LANE) && (myNetworkViewOptions.selectEdges() || forceSelectEdges)) {
                    retrievedAC = dynamic_cast<GNELane*>(retrievedAC)->getParentEdge();
                }
                // make sure that AttributeCarrier can be selected
                if (retrievedAC->getTagProperty().isSelectable() && !myViewParent->getSelectorFrame()->getLockGLObjectTypes()->IsObjectTypeLocked(retrievedAC->getGUIGlObject()->getType())) {
                    result.insert(std::make_pair(retrievedAC->getID(), retrievedAC));
                }
            }
        }
    }
    return result;
}


const GNEViewNetHelper::ObjectsUnderCursor&
GNEViewNet::getObjectsUnderCursor() const {
    return myObjectsUnderCursor;
}


void
GNEViewNet::buildSelectionACPopupEntry(GUIGLObjectPopupMenu* ret, GNEAttributeCarrier* AC) {
    if (AC->isAttributeCarrierSelected()) {
        GUIDesigns::buildFXMenuCommand(ret, "Remove From Selected", GUIIconSubSys::getIcon(GUIIcon::FLAG_MINUS), this, MID_REMOVESELECT);
    } else {
        GUIDesigns::buildFXMenuCommand(ret, "Add To Selected", GUIIconSubSys::getIcon(GUIIcon::FLAG_PLUS), this, MID_ADDSELECT);
    }
    new FXMenuSeparator(ret);
}


bool
GNEViewNet::setColorScheme(const std::string& name) {
    if (!gSchemeStorage.contains(name)) {
        return false;
    }
    if (myVisualizationChanger != nullptr) {
        if (myVisualizationChanger->getCurrentScheme() != name) {
            myVisualizationChanger->setCurrentScheme(name);
        }
    }
    myVisualizationSettings = &gSchemeStorage.get(name.c_str());
    updateViewNet();
    return true;
}


void
GNEViewNet::openObjectDialogAtCursor() {
    // reimplemented from GUISUMOAbstractView due OverlappedInspection
    ungrab();
    // make network current
    if (isEnabled() && myAmInitialised && makeCurrent()) {
        // fill objects under cursor
        myObjectsUnderCursor.updateObjectUnderCursor(getGUIGlObjectsUnderCursor());
        // get GUIGLObject front
        GUIGlObject* o = myObjectsUnderCursor.getGUIGlObjectFront();
        // we need to check if we're inspecting a overlapping element
        if (myViewParent->getInspectorFrame()->getOverlappedInspection()->overlappedInspectionShown() &&
                myViewParent->getInspectorFrame()->getOverlappedInspection()->checkSavedPosition(getPositionInformation()) &&
                myInspectedAttributeCarriers.size() > 0) {
            o = dynamic_cast<GUIGlObject*>(myInspectedAttributeCarriers.front());
        }
        // if GlObject is null, use net
        if (o == nullptr) {
            o = myNet;
        }
        openObjectDialog(o);
        makeNonCurrent();
    }
}


void
GNEViewNet::saveVisualizationSettings() const {
    // first check if we have to save gui settings in a file (only used for testing purposes)
    OptionsCont& oc = OptionsCont::getOptions();
    if (oc.getString("gui-testing.setting-output").size() > 0) {
        try {
            // open output device
            OutputDevice& output = OutputDevice::getDevice(oc.getString("gui-testing.setting-output"));
            // save view settings
            output.openTag(SUMO_TAG_VIEWSETTINGS);
            myVisualizationSettings->save(output);
            // save viewport (zoom, X, Y and Z)
            output.openTag(SUMO_TAG_VIEWPORT);
            output.writeAttr(SUMO_ATTR_ZOOM, myChanger->getZoom());
            output.writeAttr(SUMO_ATTR_X, myChanger->getXPos());
            output.writeAttr(SUMO_ATTR_Y, myChanger->getYPos());
            output.writeAttr(SUMO_ATTR_ANGLE, myChanger->getRotation());
            output.closeTag();
            output.closeTag();
            // close output device
            output.close();
        } catch (...) {
            WRITE_ERROR("GUI-Settings cannot be saved in " + oc.getString("gui-testing.setting-output"));
        }
    }
}


const GNEViewNetHelper::EditModes&
GNEViewNet::getEditModes() const {
    return myEditModes;
}


const GNEViewNetHelper::TestingMode&
GNEViewNet::getTestingMode() const {
    return myTestingMode;
}


const GNEViewNetHelper::NetworkViewOptions&
GNEViewNet::getNetworkViewOptions() const {
    return myNetworkViewOptions;
}


const GNEViewNetHelper::DemandViewOptions&
GNEViewNet::getDemandViewOptions() const {
    return myDemandViewOptions;
}


const GNEViewNetHelper::DataViewOptions&
GNEViewNet::getDataViewOptions() const {
    return myDataViewOptions;
}


const GNEViewNetHelper::MouseButtonKeyPressed&
GNEViewNet::getMouseButtonKeyPressed() const {
    return myMouseButtonKeyPressed;
}


const GNEViewNetHelper::EditNetworkElementShapes&
GNEViewNet::getEditNetworkElementShapes() const {
    return myEditNetworkElementShapes;
}


void
GNEViewNet::buildColorRainbow(const GUIVisualizationSettings& s, GUIColorScheme& scheme, int active, GUIGlObjectType objectType,
                              bool hide, double hideThreshold) {
    assert(!scheme.isFixed());
    UNUSED_PARAMETER(s);
    double minValue = std::numeric_limits<double>::infinity();
    double maxValue = -std::numeric_limits<double>::infinity();
    // retrieve range
    if (objectType == GLO_LANE) {
        // XXX (see #3409) multi-colors are not currently handled. this is a quick hack
        if (active == 9) {
            active = 8; // segment height, fall back to start height
        } else if (active == 11) {
            active = 10; // segment incline, fall back to total incline
        }
        for (GNELane* lane : myNet->retrieveLanes()) {
            const double val = lane->getColorValue(s, active);
            if (val == s.MISSING_DATA) {
                continue;
            }
            minValue = MIN2(minValue, val);
            maxValue = MAX2(maxValue, val);
        }
    } else if (objectType == GLO_JUNCTION) {
        if (active == 3) {
            for (GNEJunction* junction : myNet->retrieveJunctions()) {
                minValue = MIN2(minValue, junction->getPositionInView().z());
                maxValue = MAX2(maxValue, junction->getPositionInView().z());
            }
        }
    }
    if (scheme.getName() == GUIVisualizationSettings::SCHEME_NAME_PERMISSION_CODE) {
        scheme.clear();
        // add threshold for every distinct value
        std::set<SVCPermissions> codes;
        for (GNELane* lane : myNet->retrieveLanes()) {
            codes.insert(lane->getParentEdge()->getNBEdge()->getPermissions(lane->getIndex()));
        }
        int step = MAX2(1, 360 / (int)codes.size());
        int hue = 0;
        for (SVCPermissions p : codes) {
            scheme.addColor(RGBColor::fromHSV(hue, 1, 1), p);
            hue = (hue + step) % 360;
        }
        return;
    }
    if (minValue != std::numeric_limits<double>::infinity()) {
        scheme.clear();
        // add new thresholds
        if (hide) {
            const double rawRange = maxValue - minValue;
            minValue = MAX2(hideThreshold + MIN2(1.0, rawRange / 100.0), minValue);
            scheme.addColor(RGBColor(204, 204, 204), hideThreshold);
        }
        double range = maxValue - minValue;
        scheme.addColor(RGBColor::RED, (minValue));
        scheme.addColor(RGBColor::ORANGE, (minValue + range * 1 / 6.0));
        scheme.addColor(RGBColor::YELLOW, (minValue + range * 2 / 6.0));
        scheme.addColor(RGBColor::GREEN, (minValue + range * 3 / 6.0));
        scheme.addColor(RGBColor::CYAN, (minValue + range * 4 / 6.0));
        scheme.addColor(RGBColor::BLUE, (minValue + range * 5 / 6.0));
        scheme.addColor(RGBColor::MAGENTA, (maxValue));
    }
}


void
GNEViewNet::setStatusBarText(const std::string& text) {
    myApp->setStatusBarText(text);
}


bool
GNEViewNet::autoSelectNodes() {
    return (myNetworkViewOptions.menuCheckExtendSelection->amChecked() != 0);
}


void
GNEViewNet::setSelectorFrameScale(double selectionScale) {
    myVisualizationSettings->selectorFrameScale = selectionScale;
}


bool
GNEViewNet::changeAllPhases() const {
    return (myNetworkViewOptions.menuCheckChangeAllPhases->amChecked() != 0);
}


bool
GNEViewNet::showJunctionAsBubbles() const {
    return (myEditModes.networkEditMode == NetworkEditMode::NETWORK_MOVE) && (myNetworkViewOptions.menuCheckShowJunctionBubble->amChecked());
}


bool
GNEViewNet::mergeJunctions(GNEJunction* movedJunction, GNEJunction* targetJunction) {
    if (movedJunction && targetJunction &&
            !movedJunction->isAttributeCarrierSelected() && !targetJunction->isAttributeCarrierSelected() &&
            (movedJunction != targetJunction)) {
        // optionally ask for confirmation
        if (myNetworkViewOptions.menuCheckWarnAboutMerge->amChecked()) {
            WRITE_DEBUG("Opening FXMessageBox 'merge junctions'");
            // open question box
            FXuint answer = FXMessageBox::question(this, MBOX_YES_NO,
                                                   "Confirm Junction Merger", "%s",
                                                   ("Do you wish to merge junctions '" + movedJunction->getMicrosimID() +
                                                    "' and '" + targetJunction->getMicrosimID() + "'?\n" +
                                                    "('" + movedJunction->getMicrosimID() +
                                                    "' will be eliminated and its roads added to '" +
                                                    targetJunction->getMicrosimID() + "')").c_str());
            if (answer != 1) { //1:yes, 2:no, 4:esc
                // write warning if netedit is running in testing mode
                if (answer == 2) {
                    WRITE_DEBUG("Closed FXMessageBox 'merge junctions' with 'No'");
                } else if (answer == 4) {
                    WRITE_DEBUG("Closed FXMessageBox 'merge junctions' with 'ESC'");
                }
                return false;
            } else {
                // write warning if netedit is running in testing mode
                WRITE_DEBUG("Closed FXMessageBox 'merge junctions' with 'Yes'");
            }
        }
        // merge moved and targed junctions
        myNet->mergeJunctions(movedJunction, targetJunction, myUndoList);
        return true;
    } else {
        return false;
    }
}


GNEViewNet::GNEViewNet() :
    myEditModes(this),
    myTestingMode(this),
    myObjectsUnderCursor(this),
    myCommonCheckableButtons(this),
    myNetworkCheckableButtons(this),
    myDemandCheckableButtons(this),
    myDataCheckableButtons(this),
    myNetworkViewOptions(this),
    myDemandViewOptions(this),
    myDataViewOptions(this),
    myIntervalBar(this),
    myMoveSingleElementValues(this),
    myMoveMultipleElementValues(this),
    myVehicleOptions(this),
    myVehicleTypeOptions(this),
    mySaveElements(this),
    mySelectingArea(this),
    myEditNetworkElementShapes(this),
    myLockManager(this),
    myViewParent(nullptr),
    myNet(nullptr),
    myCurrentFrame(nullptr),
    myUndoList(nullptr),
    myFrontAttributeCarrier(nullptr) {
}


std::vector<std::string>
GNEViewNet::getEdgeLaneParamKeys(bool edgeKeys) const {
    std::set<std::string> keys;
    for (const NBEdge* e : myNet->getEdgeCont().getAllEdges()) {
        if (edgeKeys) {
            for (const auto& item : e->getParametersMap()) {
                keys.insert(item.first);
            }
            for (const auto& con : e->getConnections()) {
                for (const auto& item : con.getParametersMap()) {
                    keys.insert(item.first);
                }
            }
        } else {
            for (const auto& lane : e->getLanes()) {
                int i = 0;
                for (const auto& item : lane.getParametersMap()) {
                    keys.insert(item.first);
                }
                for (const auto& con : e->getConnectionsFromLane(i)) {
                    for (const auto& item : con.getParametersMap()) {
                        keys.insert(item.first);
                    }
                }
                i++;
            }
        }
    }
    return std::vector<std::string>(keys.begin(), keys.end());
}


std::vector<std::string>
GNEViewNet::getEdgeDataAttrs() const {
    std::set<std::string> keys;
    /*
    for (const auto &edge : myNet->getAttributeCarriers()->getEdges()) {
        GNEGenericData* genericData = edge.second->getCurrentGenericDataElement();
        if (genericData != nullptr) {
            for (const auto &parameter : genericData->getParametersMap()) {
                keys.insert(parameter.first);
            }
        }
    }
    */
    return std::vector<std::string>(keys.begin(), keys.end());
}


int
GNEViewNet::doPaintGL(int mode, const Boundary& bound) {
    // init view settings
    if (!myVisualizationSettings->drawForPositionSelection && myVisualizationSettings->forceDrawForPositionSelection) {
        myVisualizationSettings->drawForPositionSelection = true;
    }
    if (!myVisualizationSettings->drawForRectangleSelection && myVisualizationSettings->forceDrawForRectangleSelection) {
        myVisualizationSettings->drawForRectangleSelection = true;
    }
    // set lefthand and laneIcons
    myVisualizationSettings->lefthand = OptionsCont::getOptions().getBool("lefthand");
    myVisualizationSettings->disableLaneIcons = OptionsCont::getOptions().getBool("disable-laneIcons");

    glRenderMode(mode);
    glMatrixMode(GL_MODELVIEW);
    GLHelper::pushMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    // visualize rectangular selection
    mySelectingArea.drawRectangleSelection(myVisualizationSettings->colorSettings.selectionColor);

    // compute lane width
    double lw = m2p(SUMO_const_laneWidth);
    // draw decals (if not in grabbing mode)
    if (!myVisualizationSettings->drawForRectangleSelection) {
        drawDecals();
        // depending of the visualizationSettings, enable or disable check box show grid
        if (myVisualizationSettings->showGrid) {
            // change show grid
            if (!myNetworkViewOptions.menuCheckToggleGrid->amChecked() ||
                    !myDemandViewOptions.menuCheckToggleGrid->amChecked()) {
                // change to true
                myNetworkViewOptions.menuCheckToggleGrid->setChecked(true);
                myDemandViewOptions.menuCheckToggleGrid->setChecked(true);
                // update show grid buttons
                myNetworkViewOptions.menuCheckToggleGrid->update();
                myNetworkViewOptions.menuCheckToggleGrid->update();
            }
            // draw grid only in network and demand mode
            if (myEditModes.isCurrentSupermodeNetwork() || myEditModes.isCurrentSupermodeDemand()) {
                paintGLGrid();
            }
        } else {
            // change show grid
            if (myNetworkViewOptions.menuCheckToggleGrid->amChecked() ||
                    myDemandViewOptions.menuCheckToggleGrid->amChecked()) {
                // change to false
                myNetworkViewOptions.menuCheckToggleGrid->setChecked(false);
                myDemandViewOptions.menuCheckToggleGrid->setChecked(false);
                // update show grid buttons
                myNetworkViewOptions.menuCheckToggleGrid->update();
                myNetworkViewOptions.menuCheckToggleGrid->update();
            }
        }
        // update show connections
        myNetworkViewOptions.menuCheckShowConnections->setChecked(myVisualizationSettings->showLane2Lane);
    }
    // draw temporal junction
    drawTemporalJunction();
    // draw temporal elements
    if (!myVisualizationSettings->drawForRectangleSelection) {
        drawTemporalDrawShape();
        drawLaneCandidates();
        // draw testing elements
        myTestingMode.drawTestingElements(myApp);
        // draw temporal E2 multilane detectors
        myViewParent->getAdditionalFrame()->getE2MultilaneLaneSelector()->drawTemporalE2Multilane(*myVisualizationSettings);
        // draw temporal trip/flow route
        myViewParent->getVehicleFrame()->getPathCreator()->drawTemporalRoute(*myVisualizationSettings);
        // draw temporal person plan route
        myViewParent->getPersonFrame()->getPathCreator()->drawTemporalRoute(*myVisualizationSettings);
        myViewParent->getPersonPlanFrame()->getPathCreator()->drawTemporalRoute(*myVisualizationSettings);
        // draw temporal route
        myViewParent->getRouteFrame()->getPathCreator()->drawTemporalRoute(*myVisualizationSettings);
        // draw temporal edgeRelPath
        myViewParent->getEdgeRelDataFrame()->getPathCreator()->drawTemporalRoute(*myVisualizationSettings);
    }
    // check menu checks of supermode demand
    if (myEditModes.isCurrentSupermodeDemand()) {
        // enable or disable menuCheckShowAllPersonPlans depending of there is a locked person
        if (myDemandViewOptions.getLockedPerson()) {
            myDemandViewOptions.menuCheckShowAllPersonPlans->disable();
        } else {
            myDemandViewOptions.menuCheckShowAllPersonPlans->enable();
        }
    }
    // clear pathDraw
    myNet->getPathManager()->getPathDraw()->clearPathDraw();
    // draw elements
    glLineWidth(1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    const float minB[2] = { (float)bound.xmin(), (float)bound.ymin() };
    const float maxB[2] = { (float)bound.xmax(), (float)bound.ymax() };
    myVisualizationSettings->scale = lw;
    glEnable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_POLYGON_OFFSET_LINE);
    // obtain objects included in minB and maxB
    int hits2 = myGrid->Search(minB, maxB, *myVisualizationSettings);
    // pop draw matrix
    GLHelper::popMatrix();
    return hits2;
}


long
GNEViewNet::onLeftBtnPress(FXObject*, FXSelector, void* eventData) {
    // set focus in view net
    setFocus();
    // update MouseButtonKeyPressed
    myMouseButtonKeyPressed.update(eventData);
    // interpret object under cursor
    if (makeCurrent()) {
        // fill objects under cursor
        myObjectsUnderCursor.updateObjectUnderCursor(getGUIGlObjectsUnderCursor());
        // process left button press function depending of supermode
        if (myEditModes.isCurrentSupermodeNetwork()) {
            processLeftButtonPressNetwork(eventData);
        } else if (myEditModes.isCurrentSupermodeDemand()) {
            processLeftButtonPressDemand(eventData);
        } else if (myEditModes.isCurrentSupermodeData()) {
            processLeftButtonPressData(eventData);
        }
        makeNonCurrent();
    }
    // update cursor
    updateCursor();
    // update view
    updateViewNet();
    return 1;
}


long
GNEViewNet::onLeftBtnRelease(FXObject* obj, FXSelector sel, void* eventData) {
    // process parent function
    GUISUMOAbstractView::onLeftBtnRelease(obj, sel, eventData);
    // update MouseButtonKeyPressed
    myMouseButtonKeyPressed.update(eventData);
    // interpret object under cursor
    if (makeCurrent()) {
        // fill objects under cursor
        myObjectsUnderCursor.updateObjectUnderCursor(getGUIGlObjectsUnderCursor());
        // process left button release function depending of supermode
        if (myEditModes.isCurrentSupermodeNetwork()) {
            processLeftButtonReleaseNetwork();
        } else if (myEditModes.isCurrentSupermodeDemand()) {
            processLeftButtonReleaseDemand();
        } else if (myEditModes.isCurrentSupermodeData()) {
            processLeftButtonReleaseData();
        }
        makeNonCurrent();
    }
    // update cursor
    updateCursor();
    // update view
    updateViewNet();
    return 1;
}


long
GNEViewNet::onRightBtnPress(FXObject* obj, FXSelector sel, void* eventData) {
    // update MouseButtonKeyPressed
    myMouseButtonKeyPressed.update(eventData);
    // update cursor
    updateCursor();
    if ((myEditModes.networkEditMode == NetworkEditMode::NETWORK_POLYGON) && myViewParent->getPolygonFrame()->getDrawingShapeModul()->isDrawing()) {
        // disable right button press during drawing polygon
        return 1;
    } else {
        return GUISUMOAbstractView::onRightBtnPress(obj, sel, eventData);
    }
}


long
GNEViewNet::onRightBtnRelease(FXObject* obj, FXSelector sel, void* eventData) {
    // update MouseButtonKeyPressed
    myMouseButtonKeyPressed.update(eventData);
    // update cursor
    updateCursor();
    // disable right button release during drawing polygon
    if ((myEditModes.networkEditMode == NetworkEditMode::NETWORK_POLYGON) && myViewParent->getPolygonFrame()->getDrawingShapeModul()->isDrawing()) {
        return 1;
    } else {
        return GUISUMOAbstractView::onRightBtnRelease(obj, sel, eventData);
    }
}


long
GNEViewNet::onMouseMove(FXObject* obj, FXSelector sel, void* eventData) {
    // process mouse move in GUISUMOAbstractView
    GUISUMOAbstractView::onMouseMove(obj, sel, eventData);
    // update MouseButtonKeyPressed
    myMouseButtonKeyPressed.update(eventData);
    // update cursor
    updateCursor();
    // process mouse move function depending of supermode
    if (myEditModes.isCurrentSupermodeNetwork()) {
        processMoveMouseNetwork(myMouseButtonKeyPressed.mouseLeftButtonPressed());
    } else if (myEditModes.isCurrentSupermodeDemand()) {
        processMoveMouseDemand(myMouseButtonKeyPressed.mouseLeftButtonPressed());
    } else if (myEditModes.isCurrentSupermodeData()) {
        processMoveMouseData(myMouseButtonKeyPressed.mouseLeftButtonPressed());
    }
    // update view
    updateViewNet();
    return 1;
}


long
GNEViewNet::onKeyPress(FXObject* o, FXSelector sel, void* eventData) {
    // update MouseButtonKeyPressed
    myMouseButtonKeyPressed.update(eventData);
    // update cursor
    updateCursor();
    // continue depending of current edit mode
    if (myEditModes.networkEditMode == NetworkEditMode::NETWORK_CREATE_EDGE) {
        // update viewNet (for temporal junction)
        updateViewNet();
    } else if ((myEditModes.networkEditMode == NetworkEditMode::NETWORK_POLYGON) && myViewParent->getPolygonFrame()->getDrawingShapeModul()->isDrawing()) {
        // change "delete last created point" depending of shift key
        myViewParent->getPolygonFrame()->getDrawingShapeModul()->setDeleteLastCreatedPoint(myMouseButtonKeyPressed.shiftKeyPressed());
        updateViewNet();
    } else if ((myEditModes.networkEditMode == NetworkEditMode::NETWORK_TAZ) && myViewParent->getTAZFrame()->getDrawingShapeModul()->isDrawing()) {
        // change "delete last created point" depending of shift key
        myViewParent->getTAZFrame()->getDrawingShapeModul()->setDeleteLastCreatedPoint(myMouseButtonKeyPressed.shiftKeyPressed());
        updateViewNet();
    }
    return GUISUMOAbstractView::onKeyPress(o, sel, eventData);
}


long
GNEViewNet::onKeyRelease(FXObject* o, FXSelector sel, void* eventData) {
    // update MouseButtonKeyPressed
    myMouseButtonKeyPressed.update(eventData);
    // update cursor
    updateCursor();
    // continue depending of current edit mode
    if (myEditModes.networkEditMode == NetworkEditMode::NETWORK_CREATE_EDGE) {
        // update viewNet (for temporal junction)
        updateViewNet();
    } else if ((myEditModes.networkEditMode == NetworkEditMode::NETWORK_POLYGON) && myViewParent->getPolygonFrame()->getDrawingShapeModul()->isDrawing()) {
        // change "delete last created point" depending of shift key
        myViewParent->getPolygonFrame()->getDrawingShapeModul()->setDeleteLastCreatedPoint(myMouseButtonKeyPressed.shiftKeyPressed());
        updateViewNet();
    } else if ((myEditModes.networkEditMode == NetworkEditMode::NETWORK_TAZ) && myViewParent->getTAZFrame()->getDrawingShapeModul()->isDrawing()) {
        // change "delete last created point" depending of shift key
        myViewParent->getTAZFrame()->getDrawingShapeModul()->setDeleteLastCreatedPoint(myMouseButtonKeyPressed.shiftKeyPressed());
        updateViewNet();
    }
    // check if selecting using rectangle has to be disabled
    if (mySelectingArea.selectingUsingRectangle && !myMouseButtonKeyPressed.shiftKeyPressed()) {
        mySelectingArea.selectingUsingRectangle = false;
        updateViewNet();
    }
    return GUISUMOAbstractView::onKeyRelease(o, sel, eventData);
}


void
GNEViewNet::abortOperation(bool clearSelection) {
    // steal focus from any text fields and place it over view net
    setFocus();
    // check what supermode is enabled
    if (myEditModes.isCurrentSupermodeNetwork()) {
        // abort operation depending of current mode
        if (myEditModes.networkEditMode == NetworkEditMode::NETWORK_CREATE_EDGE) {
            // abort edge creation in create edge frame
            myViewParent->getCreateEdgeFrame()->abortEdgeCreation();
        } else if (myEditModes.networkEditMode == NetworkEditMode::NETWORK_SELECT) {
            mySelectingArea.selectingUsingRectangle = false;
            // check if current selection has to be cleaned
            if (clearSelection) {
                myViewParent->getSelectorFrame()->clearCurrentSelection();
            }
        } else if (myEditModes.networkEditMode == NetworkEditMode::NETWORK_CONNECT) {
            // abort changes in Connector Frame
            myViewParent->getConnectorFrame()->getConnectionModifications()->onCmdCancelModifications(0, 0, 0);
        } else if (myEditModes.networkEditMode == NetworkEditMode::NETWORK_TLS) {
            myViewParent->getTLSEditorFrame()->onCmdCancel(nullptr, 0, nullptr);
        } else if (myEditModes.networkEditMode == NetworkEditMode::NETWORK_MOVE) {
            myEditNetworkElementShapes.stopEditCustomShape();
        } else if (myEditModes.networkEditMode == NetworkEditMode::NETWORK_POLYGON) {
            // abort current drawing
            myViewParent->getPolygonFrame()->getDrawingShapeModul()->abortDrawing();
        } else if (myEditModes.networkEditMode == NetworkEditMode::NETWORK_TAZ) {
            if (myViewParent->getTAZFrame()->getDrawingShapeModul()->isDrawing()) {
                // abort current drawing
                myViewParent->getTAZFrame()->getDrawingShapeModul()->abortDrawing();
            } else if (myViewParent->getTAZFrame()->getTAZCurrentModul()->getTAZ() != nullptr) {
                // finish current editing TAZ
                myViewParent->getTAZFrame()->getTAZCurrentModul()->setTAZ(nullptr);
            }
        } else if (myEditModes.networkEditMode == NetworkEditMode::NETWORK_PROHIBITION) {
            myViewParent->getProhibitionFrame()->onCmdCancel(nullptr, 0, nullptr);
        } else if (myEditModes.networkEditMode == NetworkEditMode::NETWORK_ADDITIONAL) {
            // abort select lanes
            myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->abortConsecutiveLaneSelector();
            // abort path
            myViewParent->getAdditionalFrame()->getE2MultilaneLaneSelector()->abortPathCreation();
        }
    } else if (myEditModes.isCurrentSupermodeDemand()) {
        // abort operation depending of current mode
        if (myEditModes.demandEditMode == DemandEditMode::DEMAND_SELECT) {
            mySelectingArea.selectingUsingRectangle = false;
            // check if current selection has to be cleaned
            if (clearSelection) {
                myViewParent->getSelectorFrame()->clearCurrentSelection();
            }
        } else if (myEditModes.demandEditMode == DemandEditMode::DEMAND_ROUTE) {
            myViewParent->getRouteFrame()->getPathCreator()->abortPathCreation();
        } else if (myEditModes.demandEditMode == DemandEditMode::DEMAND_VEHICLE) {
            myViewParent->getVehicleFrame()->getPathCreator()->abortPathCreation();
        } else if (myEditModes.demandEditMode == DemandEditMode::DEMAND_PERSON) {
            myViewParent->getPersonFrame()->getPathCreator()->abortPathCreation();
        } else if (myEditModes.demandEditMode == DemandEditMode::DEMAND_PERSONPLAN) {
            myViewParent->getPersonPlanFrame()->getPathCreator()->abortPathCreation();
        } else if (myEditModes.demandEditMode == DemandEditMode::DEMAND_CONTAINER) {
            myViewParent->getContainerFrame()->getPathCreator()->abortPathCreation();
        } else if (myEditModes.demandEditMode == DemandEditMode::DEMAND_CONTAINERPLAN) {
            myViewParent->getContainerPlanFrame()->getPathCreator()->abortPathCreation();
        }
    } else if (myEditModes.isCurrentSupermodeData()) {
        // abort operation depending of current mode
        if (myEditModes.demandEditMode == DemandEditMode::DEMAND_SELECT) {
            mySelectingArea.selectingUsingRectangle = false;
            // check if current selection has to be cleaned
            if (clearSelection) {
                myViewParent->getSelectorFrame()->clearCurrentSelection();
            }
        } else if (myEditModes.dataEditMode == DataEditMode::DATA_EDGERELDATA) {
            myViewParent->getEdgeRelDataFrame()->getPathCreator()->abortPathCreation();
        } else if (myEditModes.dataEditMode == DataEditMode::DATA_TAZRELDATA) {
            myViewParent->getTAZRelDataFrame()->clearTAZSelection();
        }
    }
    // abort undo list
    myUndoList->p_abort();
    // update view
    updateViewNet();
}


void
GNEViewNet::hotkeyDel() {
    // delete elements depending of current supermode
    if (myEditModes.isCurrentSupermodeNetwork()) {
        if ((myEditModes.networkEditMode == NetworkEditMode::NETWORK_CONNECT) || (myEditModes.networkEditMode == NetworkEditMode::NETWORK_TLS)) {
            setStatusBarText("Cannot delete in this mode");
        } else {
            myUndoList->p_begin("delete network selection");
            deleteSelectedConnections();
            deleteSelectedCrossings();
            deleteSelectedAdditionals();
            deleteSelectedLanes();
            deleteSelectedEdges();
            deleteSelectedJunctions();
            deleteSelectedShapes();
            deleteSelectedTAZElements();
            myUndoList->p_end();
        }
    } else if (myEditModes.isCurrentSupermodeDemand()) {
        myUndoList->p_begin("delete demand selection");
        deleteSelectedDemandElements();
        myUndoList->p_end();
    } else if (myEditModes.isCurrentSupermodeData()) {
        myUndoList->p_begin("delete data selection");
        deleteSelectedGenericDatas();
        myUndoList->p_end();
    }
    // update view
    updateViewNet();
}


void
GNEViewNet::hotkeyEnter() {
    // check what supermode is enabled
    if (myEditModes.isCurrentSupermodeNetwork()) {
        // abort operation depending of current mode
        if (myEditModes.networkEditMode == NetworkEditMode::NETWORK_CONNECT) {
            // Accept changes in Connector Frame
            myViewParent->getConnectorFrame()->getConnectionModifications()->onCmdSaveModifications(0, 0, 0);
        } else if (myEditModes.networkEditMode == NetworkEditMode::NETWORK_TLS) {
            myViewParent->getTLSEditorFrame()->onCmdOK(nullptr, 0, nullptr);
        } else if ((myEditModes.networkEditMode == NetworkEditMode::NETWORK_MOVE) && (myEditNetworkElementShapes.getEditedNetworkElement() != nullptr)) {
            myEditNetworkElementShapes.commitEditedShape();
        } else if (myEditModes.networkEditMode == NetworkEditMode::NETWORK_POLYGON) {
            if (myViewParent->getPolygonFrame()->getDrawingShapeModul()->isDrawing()) {
                // stop current drawing
                myViewParent->getPolygonFrame()->getDrawingShapeModul()->stopDrawing();
            } else {
                // start drawing
                myViewParent->getPolygonFrame()->getDrawingShapeModul()->startDrawing();
            }
        } else if (myEditModes.networkEditMode == NetworkEditMode::NETWORK_CROSSING) {
            myViewParent->getCrossingFrame()->createCrossingHotkey();
        } else if (myEditModes.networkEditMode == NetworkEditMode::NETWORK_TAZ) {
            if (myViewParent->getTAZFrame()->getDrawingShapeModul()->isDrawing()) {
                // stop current drawing
                myViewParent->getTAZFrame()->getDrawingShapeModul()->stopDrawing();
            } else if (myViewParent->getTAZFrame()->getTAZCurrentModul()->getTAZ() == nullptr) {
                // start drawing
                myViewParent->getTAZFrame()->getDrawingShapeModul()->startDrawing();
            } else if (myViewParent->getTAZFrame()->getTAZSaveChangesModul()->isChangesPending()) {
                // save pending changes
                myViewParent->getTAZFrame()->getTAZSaveChangesModul()->onCmdSaveChanges(0, 0, 0);
            }
        } else if (myEditModes.networkEditMode == NetworkEditMode::NETWORK_ADDITIONAL) {
            if (myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->isSelectingLanes()) {
                // stop select lanes to create additional
                myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->stopConsecutiveLaneSelector();
            }
            // create E2
            myViewParent->getAdditionalFrame()->getE2MultilaneLaneSelector()->createPath();
        }
    } else if (myEditModes.isCurrentSupermodeDemand()) {
        if (myEditModes.demandEditMode == DemandEditMode::DEMAND_ROUTE) {
            myViewParent->getRouteFrame()->getPathCreator()->createPath();
        } else if (myEditModes.demandEditMode == DemandEditMode::DEMAND_VEHICLE) {
            myViewParent->getVehicleFrame()->getPathCreator()->createPath();
        } else if (myEditModes.demandEditMode == DemandEditMode::DEMAND_PERSON) {
            myViewParent->getPersonFrame()->getPathCreator()->createPath();
        } else if (myEditModes.demandEditMode == DemandEditMode::DEMAND_PERSONPLAN) {
            myViewParent->getPersonPlanFrame()->getPathCreator()->createPath();
        } else if (myEditModes.demandEditMode == DemandEditMode::DEMAND_CONTAINER) {
            myViewParent->getContainerFrame()->getPathCreator()->createPath();
        } else if (myEditModes.demandEditMode == DemandEditMode::DEMAND_CONTAINERPLAN) {
            myViewParent->getContainerPlanFrame()->getPathCreator()->createPath();
        }
    } else if (myEditModes.isCurrentSupermodeData()) {
        if (myEditModes.dataEditMode == DataEditMode::DATA_EDGERELDATA) {
            myViewParent->getEdgeRelDataFrame()->getPathCreator()->createPath();
        }
    }
}


void
GNEViewNet::hotkeyBackSpace() {
    // check what supermode is enabled
    if (myEditModes.isCurrentSupermodeNetwork()) {
        if (myEditModes.networkEditMode == NetworkEditMode::NETWORK_ADDITIONAL) {
            myViewParent->getAdditionalFrame()->getE2MultilaneLaneSelector()->removeLastElement();
        }
    } else if (myEditModes.isCurrentSupermodeDemand()) {
        if (myEditModes.demandEditMode == DemandEditMode::DEMAND_ROUTE) {
            myViewParent->getRouteFrame()->getPathCreator()->removeLastElement();
        } else if (myEditModes.demandEditMode == DemandEditMode::DEMAND_VEHICLE) {
            myViewParent->getVehicleFrame()->getPathCreator()->removeLastElement();
        } else if (myEditModes.demandEditMode == DemandEditMode::DEMAND_PERSON) {
            myViewParent->getPersonFrame()->getPathCreator()->removeLastElement();
        } else if (myEditModes.demandEditMode == DemandEditMode::DEMAND_PERSONPLAN) {
            myViewParent->getPersonPlanFrame()->getPathCreator()->removeLastElement();
        } else if (myEditModes.demandEditMode == DemandEditMode::DEMAND_CONTAINER) {
            myViewParent->getContainerFrame()->getPathCreator()->removeLastElement();
        } else if (myEditModes.demandEditMode == DemandEditMode::DEMAND_CONTAINERPLAN) {
            myViewParent->getContainerPlanFrame()->getPathCreator()->removeLastElement();
        }
    } else if (myEditModes.isCurrentSupermodeData()) {
        if (myEditModes.dataEditMode == DataEditMode::DATA_EDGERELDATA) {
            myViewParent->getEdgeRelDataFrame()->getPathCreator()->removeLastElement();
        }
    }
}

void
GNEViewNet::hotkeyFocusFrame() {
    // if there is a visible frame, set focus over it. In other case, set focus over ViewNet
    if (myCurrentFrame != nullptr) {
        myCurrentFrame->focusUpperElement();
    } else {
        setFocus();
    }
}


GNEViewParent*
GNEViewNet::getViewParent() const {
    return myViewParent;
}


GNENet*
GNEViewNet::getNet() const {
    return myNet;
}


GNEUndoList*
GNEViewNet::getUndoList() const {
    return myUndoList;
}


GNEViewNetHelper::IntervalBar&
GNEViewNet::getIntervalBar() {
    return myIntervalBar;
}


const std::vector<GNEAttributeCarrier*>&
GNEViewNet::getInspectedAttributeCarriers() const {
    return myInspectedAttributeCarriers;
}


void
GNEViewNet::setInspectedAttributeCarriers(const std::vector<GNEAttributeCarrier*> ACs) {
    myInspectedAttributeCarriers = ACs;
}


bool
GNEViewNet::isAttributeCarrierInspected(const GNEAttributeCarrier* AC) const {
    if (myInspectedAttributeCarriers.empty()) {
        return false;
    } else {
        // search AC in myInspectedAttributeCarriers
        const auto it = std::find(myInspectedAttributeCarriers.begin(), myInspectedAttributeCarriers.end(), AC);
        if (it == myInspectedAttributeCarriers.end()) {
            return false;
        } else {
            return true;
        }
    }
}


void
GNEViewNet::removeFromAttributeCarrierInspected(const GNEAttributeCarrier* AC) {
    // search AC in myInspectedAttributeCarriers
    const auto it = std::find(myInspectedAttributeCarriers.begin(), myInspectedAttributeCarriers.end(), AC);
    if (it != myInspectedAttributeCarriers.end()) {
        myInspectedAttributeCarriers.erase(it);
        myViewParent->getInspectorFrame()->inspectMultisection(myInspectedAttributeCarriers);
    }
}


const GNEAttributeCarrier*
GNEViewNet::getFrontAttributeCarrier() const {
    return myFrontAttributeCarrier;
}


void
GNEViewNet::setFrontAttributeCarrier(const GNEAttributeCarrier* AC) {
    myFrontAttributeCarrier = AC;
    // update view
    updateViewNet();
}


void
GNEViewNet::drawTranslateFrontAttributeCarrier(const GNEAttributeCarrier* AC, double typeOrLayer, const double extraOffset) {
    if (myFrontAttributeCarrier == AC) {
        glTranslated(0, 0, GLO_DOTTEDCONTOUR_FRONT + extraOffset);
    } else {
        glTranslated(0, 0, typeOrLayer + extraOffset);
    }
}


bool
GNEViewNet::showLockIcon() const {
    return ((myEditModes.networkEditMode == NetworkEditMode::NETWORK_MOVE) ||
            (myEditModes.networkEditMode == NetworkEditMode::NETWORK_INSPECT) ||
            (myEditModes.networkEditMode == NetworkEditMode::NETWORK_ADDITIONAL));
}


GNEJunction*
GNEViewNet::getJunctionAtPopupPosition() {
    GNEJunction* junction = nullptr;
    if (makeCurrent()) {
        int id = getObjectAtPosition(getPopupPosition());
        GUIGlObject* pointed = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
        GUIGlObjectStorage::gIDStorage.unblockObject(id);
        if (pointed) {
            switch (pointed->getType()) {
                case GLO_JUNCTION:
                    junction = (GNEJunction*)pointed;
                    break;
                default:
                    break;
            }
        }
    }
    return junction;
}


GNEConnection*
GNEViewNet::getConnectionAtPopupPosition() {
    GNEConnection* connection = nullptr;
    if (makeCurrent()) {
        int id = getObjectAtPosition(getPopupPosition());
        GUIGlObject* pointed = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
        GUIGlObjectStorage::gIDStorage.unblockObject(id);
        if (pointed) {
            switch (pointed->getType()) {
                case GLO_CONNECTION:
                    connection = (GNEConnection*)pointed;
                    break;
                default:
                    break;
            }
        }
    }
    return connection;
}


GNECrossing*
GNEViewNet::getCrossingAtPopupPosition() {
    GNECrossing* crossing = nullptr;
    if (makeCurrent()) {
        int id = getObjectAtPosition(getPopupPosition());
        GUIGlObject* pointed = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
        GUIGlObjectStorage::gIDStorage.unblockObject(id);
        if (pointed) {
            switch (pointed->getType()) {
                case GLO_CROSSING:
                    crossing = (GNECrossing*)pointed;
                    break;
                default:
                    break;
            }
        }
    }
    return crossing;
}

GNEEdge*
GNEViewNet::getEdgeAtPopupPosition() {
    GNEEdge* edge = nullptr;
    if (makeCurrent()) {
        int id = getObjectAtPosition(getPopupPosition());
        GUIGlObject* pointed = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
        GUIGlObjectStorage::gIDStorage.unblockObject(id);
        if (pointed) {
            switch (pointed->getType()) {
                case GLO_EDGE:
                    edge = (GNEEdge*)pointed;
                    break;
                case GLO_LANE:
                    edge = (((GNELane*)pointed)->getParentEdge());
                    break;
                default:
                    break;
            }
        }
    }
    return edge;
}


GNELane*
GNEViewNet::getLaneAtPopupPosition() {
    GNELane* lane = nullptr;
    if (makeCurrent()) {
        int id = getObjectAtPosition(getPopupPosition());
        GUIGlObject* pointed = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
        GUIGlObjectStorage::gIDStorage.unblockObject(id);
        if (pointed) {
            if (pointed->getType() == GLO_LANE) {
                lane = (GNELane*)pointed;
            }
        }
    }
    return lane;
}


GNEAdditional*
GNEViewNet::getAdditionalAtPopupPosition() {
    if (makeCurrent()) {
        int id = getObjectAtPosition(getPopupPosition());
        GUIGlObject* pointed = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
        GUIGlObjectStorage::gIDStorage.unblockObject(id);
        if (pointed) {
            return dynamic_cast<GNEAdditional*>(pointed);
        }
    }
    return nullptr;
}


GNEPoly*
GNEViewNet::getPolygonAtPopupPosition() {
    if (makeCurrent()) {
        int id = getObjectAtPosition(getPopupPosition());
        GUIGlObject* pointed = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
        GUIGlObjectStorage::gIDStorage.unblockObject(id);
        if (pointed) {
            return dynamic_cast<GNEPoly*>(pointed);
        }
    }
    return nullptr;
}


GNEPOI*
GNEViewNet::getPOIAtPopupPosition() {
    if (makeCurrent()) {
        int id = getObjectAtPosition(getPopupPosition());
        GUIGlObject* pointed = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
        GUIGlObjectStorage::gIDStorage.unblockObject(id);
        if (pointed) {
            return dynamic_cast<GNEPOI*>(pointed);
        }
    }
    return nullptr;
}


GNETAZ*
GNEViewNet::getTAZAtPopupPosition() {
    if (makeCurrent()) {
        int id = getObjectAtPosition(getPopupPosition());
        GUIGlObject* pointed = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
        GUIGlObjectStorage::gIDStorage.unblockObject(id);
        if (pointed) {
            return dynamic_cast<GNETAZ*>(pointed);
        }
    }
    return nullptr;
}


long
GNEViewNet::onCmdSetSupermode(FXObject*, FXSelector sel, void*) {
    // check what network mode will be set
    switch (FXSELID(sel)) {
        case MID_HOTKEY_F2_SUPERMODE_NETWORK:
            myEditModes.setSupermode(Supermode::NETWORK, false);
            break;
        case MID_HOTKEY_F3_SUPERMODE_DEMAND:
            myEditModes.setSupermode(Supermode::DEMAND, false);
            break;
        case MID_HOTKEY_F4_SUPERMODE_DATA:
            myEditModes.setSupermode(Supermode::DATA, false);
            break;
        default:
            break;
    }
    return 1;
}

long
GNEViewNet::onCmdSetMode(FXObject*, FXSelector sel, void*) {
    if (myEditModes.isCurrentSupermodeNetwork()) {
        // check what network mode will be set
        switch (FXSELID(sel)) {
            case MID_HOTKEY_I_MODES_INSPECT:
                myEditModes.setNetworkEditMode(NetworkEditMode::NETWORK_INSPECT);
                break;
            case MID_HOTKEY_D_MODES_DELETE:
                myEditModes.setNetworkEditMode(NetworkEditMode::NETWORK_DELETE);
                break;
            case MID_HOTKEY_S_MODES_SELECT:
                myEditModes.setNetworkEditMode(NetworkEditMode::NETWORK_SELECT);
                break;
            case MID_HOTKEY_M_MODES_MOVE:
                myEditModes.setNetworkEditMode(NetworkEditMode::NETWORK_MOVE);
                break;
            case MID_HOTKEY_E_MODES_EDGE_EDGEDATA:
                myEditModes.setNetworkEditMode(NetworkEditMode::NETWORK_CREATE_EDGE);
                break;
            case MID_HOTKEY_C_MODES_CONNECT_PERSONPLAN:
                myEditModes.setNetworkEditMode(NetworkEditMode::NETWORK_CONNECT);
                break;
            case MID_HOTKEY_T_MODES_TLS_VTYPE:
                myEditModes.setNetworkEditMode(NetworkEditMode::NETWORK_TLS);
                break;
            case MID_HOTKEY_A_MODES_ADDITIONAL_STOP:
                myEditModes.setNetworkEditMode(NetworkEditMode::NETWORK_ADDITIONAL);
                break;
            case MID_HOTKEY_R_MODES_CROSSING_ROUTE_EDGERELDATA:
                myEditModes.setNetworkEditMode(NetworkEditMode::NETWORK_CROSSING);
                break;
            case MID_HOTKEY_Z_MODES_TAZ_TAZREL:
                myEditModes.setNetworkEditMode(NetworkEditMode::NETWORK_TAZ);
                break;
            case MID_HOTKEY_P_MODES_POLYGON_PERSON:
                myEditModes.setNetworkEditMode(NetworkEditMode::NETWORK_POLYGON);
                break;
            case MID_HOTKEY_W_MODES_PROHIBITION_PERSONTYPE:
                myEditModes.setNetworkEditMode(NetworkEditMode::NETWORK_PROHIBITION);
                break;
            default:
                break;
        }
    } else if (myEditModes.isCurrentSupermodeDemand()) {
        // check what demand mode will be set
        switch (FXSELID(sel)) {
            case MID_HOTKEY_G_MODE_CONTAINER:
                myEditModes.setDemandEditMode(DemandEditMode::DEMAND_CONTAINER);
                break;
            case MID_HOTKEY_H_MODE_CONTAINERDATA:
                myEditModes.setDemandEditMode(DemandEditMode::DEMAND_CONTAINERPLAN);
                break;
            case MID_HOTKEY_I_MODES_INSPECT:
                myEditModes.setDemandEditMode(DemandEditMode::DEMAND_INSPECT);
                break;
            case MID_HOTKEY_D_MODES_DELETE:
                myEditModes.setDemandEditMode(DemandEditMode::DEMAND_DELETE);
                break;
            case MID_HOTKEY_S_MODES_SELECT:
                myEditModes.setDemandEditMode(DemandEditMode::DEMAND_SELECT);
                break;
            case MID_HOTKEY_M_MODES_MOVE:
                myEditModes.setDemandEditMode(DemandEditMode::DEMAND_MOVE);
                break;
            case MID_HOTKEY_R_MODES_CROSSING_ROUTE_EDGERELDATA:
                myEditModes.setDemandEditMode(DemandEditMode::DEMAND_ROUTE);
                break;
            case MID_HOTKEY_V_MODES_VEHICLE:
                myEditModes.setDemandEditMode(DemandEditMode::DEMAND_VEHICLE);
                break;
            case MID_HOTKEY_T_MODES_TLS_VTYPE:
                myEditModes.setDemandEditMode(DemandEditMode::DEMAND_VEHICLETYPES);
                break;
            case MID_HOTKEY_A_MODES_ADDITIONAL_STOP:
                myEditModes.setDemandEditMode(DemandEditMode::DEMAND_STOP);
                break;
            case MID_HOTKEY_W_MODES_PROHIBITION_PERSONTYPE:
                myEditModes.setDemandEditMode(DemandEditMode::DEMAND_PERSONTYPES);
                break;
            case MID_HOTKEY_P_MODES_POLYGON_PERSON:
                myEditModes.setDemandEditMode(DemandEditMode::DEMAND_PERSON);
                break;
            case MID_HOTKEY_C_MODES_CONNECT_PERSONPLAN:
                myEditModes.setDemandEditMode(DemandEditMode::DEMAND_PERSONPLAN);
                break;
            default:
                break;
        }
    } else if (myEditModes.isCurrentSupermodeData()) {
        // check what demand mode will be set
        switch (FXSELID(sel)) {
            case MID_HOTKEY_I_MODES_INSPECT:
                myEditModes.setDataEditMode(DataEditMode::DATA_INSPECT);
                break;
            case MID_HOTKEY_D_MODES_DELETE:
                myEditModes.setDataEditMode(DataEditMode::DATA_DELETE);
                break;
            case MID_HOTKEY_S_MODES_SELECT:
                myEditModes.setDataEditMode(DataEditMode::DATA_SELECT);
                break;
            case MID_HOTKEY_E_MODES_EDGE_EDGEDATA:
                myEditModes.setDataEditMode(DataEditMode::DATA_EDGEDATA);
                break;
            case MID_HOTKEY_R_MODES_CROSSING_ROUTE_EDGERELDATA:
                myEditModes.setDataEditMode(DataEditMode::DATA_EDGERELDATA);
                break;
            case MID_HOTKEY_Z_MODES_TAZ_TAZREL:
                myEditModes.setDataEditMode(DataEditMode::DATA_TAZRELDATA);
                break;
        }
    }
    return 1;
}


long
GNEViewNet::onCmdSplitEdge(FXObject*, FXSelector, void*) {
    GNEEdge* edge = getEdgeAtPopupPosition();
    if (edge != nullptr) {
        myNet->splitEdge(edge, edge->getSplitPos(getPopupPosition()), myUndoList);
    }
    return 1;
}


long
GNEViewNet::onCmdSplitEdgeBidi(FXObject*, FXSelector, void*) {
    GNEEdge* edge = getEdgeAtPopupPosition();
    if (edge != nullptr) {
        // obtain reverse edge
        GNEEdge* reverseEdge = edge->getOppositeEdge();
        // check that reverse edge works
        if (reverseEdge != nullptr) {
            myNet->splitEdgesBidi(edge, reverseEdge, edge->getSplitPos(getPopupPosition()), myUndoList);
        }
    }
    return 1;
}


long
GNEViewNet::onCmdReverseEdge(FXObject*, FXSelector, void*) {
    GNEEdge* edge = getEdgeAtPopupPosition();
    if (edge != nullptr) {
        if (edge->isAttributeCarrierSelected()) {
            myUndoList->p_begin("Reverse selected " + toString(SUMO_TAG_EDGE) + "s");
            std::vector<GNEEdge*> edges = myNet->retrieveEdges(true);
            for (auto it : edges) {
                myNet->reverseEdge(it, myUndoList);
            }
            myUndoList->p_end();
        } else {
            myUndoList->p_begin("Reverse " + toString(SUMO_TAG_EDGE));
            myNet->reverseEdge(edge, myUndoList);
            myUndoList->p_end();
        }
    }
    return 1;
}


long
GNEViewNet::onCmdAddReversedEdge(FXObject*, FXSelector, void*) {
    GNEEdge* edge = getEdgeAtPopupPosition();
    if (edge != nullptr) {
        if (edge->isAttributeCarrierSelected()) {
            myUndoList->p_begin("Add Reverse edge for selected " + toString(SUMO_TAG_EDGE) + "s");
            std::vector<GNEEdge*> edges = myNet->retrieveEdges(true);
            for (auto it : edges) {
                myNet->addReversedEdge(it, myUndoList);
            }
            myUndoList->p_end();
        } else {
            myUndoList->p_begin("Add reverse " + toString(SUMO_TAG_EDGE));
            myNet->addReversedEdge(edge, myUndoList);
            myUndoList->p_end();
        }
    }
    return 1;
}


long
GNEViewNet::onCmdEditEdgeEndpoint(FXObject*, FXSelector, void*) {
    GNEEdge* edge = getEdgeAtPopupPosition();
    if (edge != nullptr) {
        // snap to active grid the Popup position
        edge->editEndpoint(getPopupPosition(), myUndoList);
    }
    return 1;
}


long
GNEViewNet::onCmdResetEdgeEndpoint(FXObject*, FXSelector, void*) {
    GNEEdge* edge = getEdgeAtPopupPosition();
    if (edge != nullptr) {
        // check if edge is selected
        if (edge->isAttributeCarrierSelected()) {
            // get all selected edges
            const auto selectedEdges = myNet->retrieveEdges(true);
            // begin operation
            myUndoList->p_begin("reset geometry points");
            // iterate over selected edges
            for (const auto& selectedEdge : selectedEdges) {
                // reset both end points
                selectedEdge->resetBothEndpoint(myUndoList);
            }
            // end operation
            myUndoList->p_end();
        } else {
            edge->resetEndpoint(getPopupPosition(), myUndoList);
        }
    }
    return 1;
}


long
GNEViewNet::onCmdStraightenEdges(FXObject*, FXSelector, void*) {
    GNEEdge* edge = getEdgeAtPopupPosition();
    if (edge != nullptr) {
        if (edge->isAttributeCarrierSelected()) {
            myUndoList->p_begin("straighten selected " + toString(SUMO_TAG_EDGE) + "s");
            std::vector<GNEEdge*> edges = myNet->retrieveEdges(true);
            for (auto it : edges) {
                it->setAttribute(SUMO_ATTR_SHAPE, "", myUndoList);
            }
            myUndoList->p_end();
        } else {

            myUndoList->p_begin("straighten " + toString(SUMO_TAG_EDGE));
            edge->setAttribute(SUMO_ATTR_SHAPE, "", myUndoList);
            myUndoList->p_end();
        }
    }
    return 1;
}


long
GNEViewNet::onCmdSmoothEdges(FXObject*, FXSelector, void*) {
    GNEEdge* edge = getEdgeAtPopupPosition();
    if (edge != nullptr) {
        if (edge->isAttributeCarrierSelected()) {
            myUndoList->p_begin("straighten elevation of selected " + toString(SUMO_TAG_EDGE) + "s");
            std::vector<GNEEdge*> edges = myNet->retrieveEdges(true);
            for (auto it : edges) {
                it->smooth(myUndoList);
            }
            myUndoList->p_end();
        } else {
            myUndoList->p_begin("straighten edge elevation");
            edge->smooth(myUndoList);
            myUndoList->p_end();
        }
    }
    return 1;
}


long
GNEViewNet::onCmdStraightenEdgesElevation(FXObject*, FXSelector, void*) {
    GNEEdge* edge = getEdgeAtPopupPosition();
    if (edge != nullptr) {
        if (edge->isAttributeCarrierSelected()) {
            myUndoList->p_begin("straighten elevation of selected " + toString(SUMO_TAG_EDGE) + "s");
            std::vector<GNEEdge*> edges = myNet->retrieveEdges(true);
            for (auto it : edges) {
                it->straightenElevation(myUndoList);
            }
            myUndoList->p_end();
        } else {
            myUndoList->p_begin("straighten edge elevation");
            edge->straightenElevation(myUndoList);
            myUndoList->p_end();
        }
    }
    return 1;
}


long
GNEViewNet::onCmdSmoothEdgesElevation(FXObject*, FXSelector, void*) {
    GNEEdge* edge = getEdgeAtPopupPosition();
    if (edge != nullptr) {
        if (edge->isAttributeCarrierSelected()) {
            myUndoList->p_begin("smooth elevation of selected " + toString(SUMO_TAG_EDGE) + "s");
            std::vector<GNEEdge*> edges = myNet->retrieveEdges(true);
            for (auto it : edges) {
                it->smoothElevation(myUndoList);
            }
            myUndoList->p_end();
        } else {
            myUndoList->p_begin("smooth edge elevation");
            edge->smoothElevation(myUndoList);
            myUndoList->p_end();
        }
    }
    return 1;
}


long
GNEViewNet::onCmdResetLength(FXObject*, FXSelector, void*) {
    GNEEdge* edge = getEdgeAtPopupPosition();
    if (edge != nullptr) {
        if (edge->isAttributeCarrierSelected()) {
            myUndoList->p_begin("reset edge lengths");
            std::vector<GNEEdge*> edges = myNet->retrieveEdges(true);
            for (auto it : edges) {
                it->setAttribute(SUMO_ATTR_LENGTH, "-1", myUndoList);
            }
            myUndoList->p_end();
        } else {
            edge->setAttribute(SUMO_ATTR_LENGTH, "-1", myUndoList);
        }
    }
    return 1;
}


long
GNEViewNet::onCmdSimplifyShape(FXObject*, FXSelector, void*) {
    // get polygon under mouse
    GNEPoly* polygonUnderMouse = getPolygonAtPopupPosition();
    // check polygon
    if (polygonUnderMouse) {
        // check if shape is selected
        if (polygonUnderMouse->isAttributeCarrierSelected()) {
            // get shapes
            const auto selectedShapes = myNet->retrieveShapes(true);
            // begin undo-list
            myNet->getViewNet()->getUndoList()->p_begin("simplify shapes");
            // iterate over shapes
            for (const auto& selectedShape : selectedShapes) {
                // check if shape is a poly
                if (selectedShape->getTagProperty().getTag() == SUMO_TAG_POLY) {
                    // simplify shape
                    dynamic_cast<GNEPoly*>(selectedShape)->simplifyShape();
                }
            }
            // end undo-list
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            polygonUnderMouse->simplifyShape();
        }
    }
    updateViewNet();
    return 1;
}


long
GNEViewNet::onCmdDeleteGeometryPoint(FXObject*, FXSelector, void*) {
    GNEPoly* polygonUnderMouse = getPolygonAtPopupPosition();
    if (polygonUnderMouse) {
        polygonUnderMouse->deleteGeometryPoint(getPopupPosition());
    }
    updateViewNet();
    return 1;
}


long
GNEViewNet::onCmdClosePolygon(FXObject*, FXSelector, void*) {
    // get polygon under mouse
    GNEPoly* polygonUnderMouse = getPolygonAtPopupPosition();
    // check polygon
    if (polygonUnderMouse) {
        // check if shape is selected
        if (polygonUnderMouse->isAttributeCarrierSelected()) {
            // get shapes
            const auto selectedShapes = myNet->retrieveShapes(true);
            // begin undo-list
            myNet->getViewNet()->getUndoList()->p_begin("close polygon shapes");
            // iterate over shapes
            for (const auto& selectedShape : selectedShapes) {
                // check if shape is a poly
                if (selectedShape->getTagProperty().getTag() == SUMO_TAG_POLY) {
                    // close polygon
                    dynamic_cast<GNEPoly*>(selectedShape)->closePolygon();
                }
            }
            // end undo-list
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            polygonUnderMouse->simplifyShape();
        }
    }
    updateViewNet();
    return 1;
}


long
GNEViewNet::onCmdOpenPolygon(FXObject*, FXSelector, void*) {
    // get polygon under mouse
    GNEPoly* polygonUnderMouse = getPolygonAtPopupPosition();
    // check polygon
    if (polygonUnderMouse) {
        // check if shape is selected
        if (polygonUnderMouse->isAttributeCarrierSelected()) {
            // get shapes
            const auto selectedShapes = myNet->retrieveShapes(true);
            // begin undo-list
            myNet->getViewNet()->getUndoList()->p_begin("open polygon shapes");
            // iterate over shapes
            for (const auto& selectedShape : selectedShapes) {
                // check if shape is a poly
                if (selectedShape->getTagProperty().getTag() == SUMO_TAG_POLY) {
                    // open polygon
                    dynamic_cast<GNEPoly*>(selectedShape)->openPolygon();
                }
            }
            // end undo-list
            myNet->getViewNet()->getUndoList()->p_end();
        } else {
            polygonUnderMouse->openPolygon();
        }
    }
    updateViewNet();
    return 1;
}


long
GNEViewNet::onCmdSetFirstGeometryPoint(FXObject*, FXSelector, void*) {
    GNEPoly* polygonUnderMouse = getPolygonAtPopupPosition();
    if (polygonUnderMouse) {
        polygonUnderMouse->changeFirstGeometryPoint(polygonUnderMouse->getVertexIndex(getPopupPosition(), false));
        updateViewNet();
    }

    return 1;
}


long
GNEViewNet::onCmdTransformPOI(FXObject*, FXSelector, void*) {
    // obtain POI at popup position
    GNEPOI* POI = getPOIAtPopupPosition();
    if (POI) {
        // check what type of POI will be transformed
        if (POI->getTagProperty().getTag() == SUMO_TAG_POI) {
            // obtain lanes around POI boundary
            std::vector<GUIGlID> GLIDs = getObjectsInBoundary(POI->getCenteringBoundary(), false);
            std::vector<GNELane*> lanes;
            for (const auto &GLID : GLIDs) {
                GNELane* lane = dynamic_cast<GNELane*>(GUIGlObjectStorage::gIDStorage.getObjectBlocking(GLID));
                if (lane) {
                    lanes.push_back(lane);
                }
            }
            if (lanes.empty()) {
                WRITE_WARNING("No lanes around " + toString(SUMO_TAG_POI) + " to attach it");
            } else {
                // obtain nearest lane to POI
                GNELane* nearestLane = lanes.front();
                double minorPosOverLane = nearestLane->getLaneShape().nearest_offset_to_point2D(POI->getPositionInView());
                double minorLateralOffset = nearestLane->getLaneShape().positionAtOffset(minorPosOverLane).distanceTo(POI->getPositionInView());
                for (const auto &lane : lanes) {
                    double posOverLane = lane->getLaneShape().nearest_offset_to_point2D(POI->getPositionInView());
                    double lateralOffset = lane->getLaneShape().positionAtOffset(posOverLane).distanceTo(POI->getPositionInView());
                    if (lateralOffset < minorLateralOffset) {
                        minorPosOverLane = posOverLane;
                        minorLateralOffset = lateralOffset;
                        nearestLane = lane;
                    }
                }
                // obtain values of POI
                std::string id = POI->getID();
                std::string type = POI->getShapeType();
                RGBColor color = POI->getShapeColor();
                Position pos = (*POI);
                double layer = POI->getShapeLayer();
                double angle = POI->getShapeNaviDegree();
                std::string imgFile = POI->getShapeImgFile();
                bool relativePath = POI->getShapeRelativePath();
                double POIWidth = POI->getWidth();      // double width -> C4458
                double POIHeight = POI->getHeight();    // double height -> C4458
                bool friendlyPos = POI->getFriendlyPos();
                // remove POI
                myUndoList->p_begin("attach POI into " + toString(SUMO_TAG_LANE));
                myNet->deleteShape(POI, myUndoList);
                // add POILane
                myNet->getAttributeCarriers()->addPOI(id, type, color, pos, false, nearestLane->getID(), minorPosOverLane, friendlyPos, 0, layer, angle, imgFile, relativePath, POIWidth, POIHeight);
                myUndoList->p_end();
            }
        } else {
            // obtain values of POILane
            std::string id = POI->getID();
            std::string type = POI->getShapeType();
            RGBColor color = POI->getShapeColor();
            Position pos = (*POI);
            double layer = POI->getShapeLayer();
            double angle = POI->getShapeNaviDegree();
            std::string imgFile = POI->getShapeImgFile();
            bool relativePath = POI->getShapeRelativePath();
            double POIWidth = POI->getWidth();      // double width -> C4458
            double POIWeight = POI->getHeight();    // double height -> C4458
            bool friendlyPos = POI->getFriendlyPos();
            // remove POI
            myUndoList->p_begin("release POI from " + toString(SUMO_TAG_LANE));
            myNet->deleteShape(POI, myUndoList);
            // add POI
            myNet->getAttributeCarriers()->addPOI(id, type, color, pos, false, "", 0, friendlyPos, 0, layer, angle, imgFile, relativePath, POIWidth, POIWeight);
            myUndoList->p_end();
        }
        // update view after transform
        updateViewNet();
    }
    return 1;
}


long
GNEViewNet::onCmdSetCustomGeometryPoint(FXObject*, FXSelector, void*) {
    // get element at popup position
    GNELane* lane = getLaneAtPopupPosition();
    GNEPoly* poly = getPolygonAtPopupPosition();
    GNETAZ* TAZ = getTAZAtPopupPosition();
    // check element
    if (lane != nullptr) {
        // make a copy of edge geometry
        PositionVector edgeGeometry = lane->getParentEdge()->getNBEdge()->getGeometry();
        // get index position
        const int index = edgeGeometry.indexOfClosest(getPositionInformation(), true);
        // get new position
        Position newPosition = edgeGeometry[index];
        // edit using GNEGeometryPointDialog
        GNEGeometryPointDialog(this, &newPosition);
        // now check position
        if (newPosition != edgeGeometry[index]) {
            // update new position
            edgeGeometry[index] = newPosition;
            // begin undo list
            myUndoList->p_begin("change edge Geometry Point position");
            // continue depending of index
            if (index == 0) {
                // change shape start
                myUndoList->p_add(new GNEChange_Attribute(lane->getParentEdge(), GNE_ATTR_SHAPE_START, toString(edgeGeometry.front())));
            } else if (index == ((int)edgeGeometry.size() - 1)) {
                // change shape end
                myUndoList->p_add(new GNEChange_Attribute(lane->getParentEdge(), GNE_ATTR_SHAPE_END, toString(edgeGeometry.back())));
            } else {
                // remove front and back geometry points
                edgeGeometry.pop_front();
                edgeGeometry.pop_back();
                // change shape
                myUndoList->p_add(new GNEChange_Attribute(lane->getParentEdge(), SUMO_ATTR_SHAPE, toString(edgeGeometry)));
            }
            // end undo list
            myUndoList->p_end();
        }
    } else if (poly != nullptr) {
        // make a copy of polygon geometry
        PositionVector polygonGeometry = poly->getShape();
        // get index position
        const int index = polygonGeometry.indexOfClosest(getPositionInformation(), true);
        // get new position
        Position newPosition = polygonGeometry[index];
        // edit using GNEGeometryPointDialog
        GNEGeometryPointDialog(this, &newPosition);
        // now check position
        if (newPosition != polygonGeometry[index]) {
            // update new position
            polygonGeometry[index] = newPosition;
            // begin undo list
            myUndoList->p_begin("change polygon Geometry Point position");
            // change shape
            myUndoList->p_add(new GNEChange_Attribute(poly, SUMO_ATTR_SHAPE, toString(polygonGeometry)));
            // end undo list
            myUndoList->p_end();
        }
    } else if (TAZ != nullptr) {
        // make a copy of TAZ geometry
        PositionVector TAZGeometry = TAZ->getTAZElementShape();
        // get index position
        const int index = TAZGeometry.indexOfClosest(getPositionInformation(), true);
        // get new position
        Position newPosition = TAZGeometry[index];
        // edit using GNEGeometryPointDialog
        GNEGeometryPointDialog(this, &newPosition);
        // now check position
        if (newPosition != TAZGeometry[index]) {
            // update new position
            TAZGeometry[index] = newPosition;
            // begin undo list
            myUndoList->p_begin("change TAZ Geometry Point position");
            // change shape
            myUndoList->p_add(new GNEChange_Attribute(TAZ, SUMO_ATTR_SHAPE, toString(TAZGeometry)));
            // end undo list
            myUndoList->p_end();
        }
    }
    return 1;
}


long
GNEViewNet::onCmdResetEndPoints(FXObject*, FXSelector, void*) {
    // get lane at popup position
    GNELane* laneAtPopupPosition = getLaneAtPopupPosition();
    // check element
    if (laneAtPopupPosition != nullptr) {
        // get parent edge
        GNEEdge* edge = laneAtPopupPosition->getParentEdge();
        // check if edge is selected
        if (edge->isAttributeCarrierSelected()) {
            // get selected edges
            auto selectedEdges = myNet->retrieveEdges(true);
            // begin undo list
            myUndoList->p_begin("reset end points of selected edges");
            // iterate over edges
            for (const auto& selectedEdge : selectedEdges) {
                // reset both end points
                selectedEdge->setAttribute(GNE_ATTR_SHAPE_START, "", myUndoList);
                selectedEdge->setAttribute(GNE_ATTR_SHAPE_END, "", myUndoList);
            }
            // end undo list
            myUndoList->p_end();
        } else {
            // begin undo list
            myUndoList->p_begin("reset end points of " + edge->getID());
            // reset both end points
            edge->setAttribute(GNE_ATTR_SHAPE_START, "", myUndoList);
            edge->setAttribute(GNE_ATTR_SHAPE_END, "", myUndoList);
            // end undo list
            myUndoList->p_end();
        }
    }
    return 1;
}


long
GNEViewNet::onCmdDuplicateLane(FXObject*, FXSelector, void*) {
    GNELane* laneAtPopupPosition = getLaneAtPopupPosition();
    if (laneAtPopupPosition != nullptr) {
        // when duplicating an unselected lane, keep all connections as they
        // are, otherwise recompute them
        if (laneAtPopupPosition->isAttributeCarrierSelected()) {
            myUndoList->p_begin("duplicate selected " + toString(SUMO_TAG_LANE) + "s");
            std::vector<GNELane*> lanes = myNet->retrieveLanes(true);
            for (const auto &lane : lanes) {
                myNet->duplicateLane(lane, myUndoList, true);
            }
            myUndoList->p_end();
        } else {
            myUndoList->p_begin("duplicate " + toString(SUMO_TAG_LANE));
            myNet->duplicateLane(laneAtPopupPosition, myUndoList, false);
            myUndoList->p_end();
        }
    }
    return 1;
}


long
GNEViewNet::onCmdResetLaneCustomShape(FXObject*, FXSelector, void*) {
    GNELane* laneAtPopupPosition = getLaneAtPopupPosition();
    if (laneAtPopupPosition != nullptr) {
        // when duplicating an unselected lane, keep all connections as they
        // are, otherwise recompute them
        if (laneAtPopupPosition->isAttributeCarrierSelected()) {
            myUndoList->p_begin("reset custom lane shapes");
            std::vector<GNELane*> lanes = myNet->retrieveLanes(true);
            for (const auto &lane : lanes) {
                lane->setAttribute(SUMO_ATTR_CUSTOMSHAPE, "", myUndoList);
            }
            myUndoList->p_end();
        } else {
            myUndoList->p_begin("reset custom lane shape");
            laneAtPopupPosition->setAttribute(SUMO_ATTR_CUSTOMSHAPE, "", myUndoList);
            myUndoList->p_end();
        }
    }
    return 1;
}


long
GNEViewNet::onCmdResetOppositeLane(FXObject*, FXSelector, void*) {
    GNELane* laneAtPopupPosition = getLaneAtPopupPosition();
    if (laneAtPopupPosition != nullptr) {
        // when duplicating an unselected lane, keep all connections as they
        // are, otherwise recompute them
        if (laneAtPopupPosition->isAttributeCarrierSelected()) {
        myUndoList->p_begin("reset opposite lanes");
            std::vector<GNELane*> lanes = myNet->retrieveLanes(true);
            for (const auto &lane : lanes) {
                lane->setAttribute(GNE_ATTR_OPPOSITE, "", myUndoList);
            }
            myUndoList->p_end();
        } else {
        myUndoList->p_begin("reset opposite lane");
        laneAtPopupPosition->setAttribute(GNE_ATTR_OPPOSITE, "", myUndoList);
        myUndoList->p_end();
        }
    }
    return 1;
}


long
GNEViewNet::onCmdLaneOperation(FXObject*, FXSelector sel, void*) {
    // check lane operation
    switch (FXSELID(sel)) {
        case MID_GNE_LANE_TRANSFORM_SIDEWALK:
            return restrictLane(SVC_PEDESTRIAN);
        case MID_GNE_LANE_TRANSFORM_BIKE:
            return restrictLane(SVC_BICYCLE);
        case MID_GNE_LANE_TRANSFORM_BUS:
            return restrictLane(SVC_BUS);
        case MID_GNE_LANE_TRANSFORM_GREENVERGE:
            return restrictLane(SVC_IGNORING);
        case MID_GNE_LANE_ADD_SIDEWALK:
            return addRestrictedLane(SVC_PEDESTRIAN, false);
        case MID_GNE_LANE_ADD_BIKE:
            return addRestrictedLane(SVC_BICYCLE, false);
        case MID_GNE_LANE_ADD_BUS:
            return addRestrictedLane(SVC_BUS, false);
        case MID_GNE_LANE_ADD_GREENVERGE_FRONT:
            return addRestrictedLane(SVC_IGNORING, true);
        case MID_GNE_LANE_ADD_GREENVERGE_BACK:
            return addRestrictedLane(SVC_IGNORING, false);
        case MID_GNE_LANE_REMOVE_SIDEWALK:
            return removeRestrictedLane(SVC_PEDESTRIAN);
        case MID_GNE_LANE_REMOVE_BIKE:
            return removeRestrictedLane(SVC_BICYCLE);
        case MID_GNE_LANE_REMOVE_BUS:
            return removeRestrictedLane(SVC_BUS);
        case MID_GNE_LANE_REMOVE_GREENVERGE:
            return removeRestrictedLane(SVC_IGNORING);
        default:
            return 0;
    }
}


long
GNEViewNet::onCmdLaneReachability(FXObject* menu, FXSelector, void*) {
    GNELane* laneAtPopupPosition = getLaneAtPopupPosition();
    if (laneAtPopupPosition != nullptr) {
        // obtain vClass
        const SUMOVehicleClass vClass = SumoVehicleClassStrings.get(dynamic_cast<FXMenuCommand*>(menu)->getText().text());
        // calculate reachability
        myNet->getPathManager()->getPathCalculator()->calculateReachability(vClass, laneAtPopupPosition->getParentEdge());
        // select all lanes with reachablility greather than 0
        myUndoList->p_begin("select lane reachability");
        for (const auto& edge : myNet->getAttributeCarriers()->getEdges()) {
            for (const auto& lane : edge.second->getLanes()) {
                if (lane->getReachability() >= 0) {
                    lane->setAttribute(GNE_ATTR_SELECTED, "true", myUndoList);
                }
            }
        }
        myUndoList->p_end();
    }
    // update viewNet
    updateViewNet();
    return 1;
}


long
GNEViewNet::onCmdOpenAdditionalDialog(FXObject*, FXSelector, void*) {
    // retrieve additional under cursor
    GNEAdditional* addtional = getAdditionalAtPopupPosition();
    // check if additional can open dialog
    if (addtional && addtional->getTagProperty().hasDialog()) {
        addtional->openAdditionalDialog();
    }
    return 1;
}


bool
GNEViewNet::restrictLane(SUMOVehicleClass vclass) {
    GNELane* laneAtPopupPosition = getLaneAtPopupPosition();
    if (laneAtPopupPosition != nullptr) {
        // Get selected lanes
        std::vector<GNELane*> lanes = myNet->retrieveLanes(true); ;
        // Declare map of edges and lanes
        std::map<GNEEdge*, GNELane*> mapOfEdgesAndLanes;
        // Iterate over selected lanes
        for (const auto &lane : lanes) {
            mapOfEdgesAndLanes[myNet->retrieveEdge(lane->getParentEdge()->getID())] = lane;
        }
        // Throw warning dialog if there hare multiple lanes selected in the same edge
        if (mapOfEdgesAndLanes.size() != lanes.size()) {
            FXMessageBox::information(getApp(), MBOX_OK,
                                      "Multiple lane in the same edge selected", "%s",
                                      ("There are selected lanes that belong to the same edge.\n Only one lane per edge will be restricted for " + toString(vclass) + ".").c_str());
        }
        // If we handeln a set of lanes
        if (mapOfEdgesAndLanes.size() > 0) {
            // declare counter for number of Sidewalks
            int counter = 0;
            // iterate over selected lanes
            for (auto i : mapOfEdgesAndLanes) {
                if (i.first->hasRestrictedLane(vclass)) {
                    counter++;
                }
            }
            // if all edges parent own a Sidewalk, stop function
            if (counter == (int)mapOfEdgesAndLanes.size()) {
                FXMessageBox::information(getApp(), MBOX_OK,
                                          ("Set vclass for " + toString(vclass) + " to selected lanes").c_str(), "%s",
                                          ("All lanes own already another lane in the same edge with a restriction for " + toString(vclass)).c_str());
                return 0;
            } else {
                WRITE_DEBUG("Opening FXMessageBox 'restrict lanes'");
                // Ask confirmation to user
                FXuint answer = FXMessageBox::question(getApp(), MBOX_YES_NO,
                                                       ("Set vclass for " + toString(vclass) + " to selected lanes").c_str(), "%s",
                                                       (toString(mapOfEdgesAndLanes.size() - counter) + " lanes will be restricted for " + toString(vclass) + ". continue?").c_str());
                if (answer != 1) { //1:yes, 2:no, 4:esc
                    // write warning if netedit is running in testing mode
                    if (answer == 2) {
                        WRITE_DEBUG("Closed FXMessageBox 'restrict lanes' with 'No'");
                    } else if (answer == 4) {
                        WRITE_DEBUG("Closed FXMessageBox 'restrict lanes' with 'ESC'");
                    }
                    return 0;
                } else {
                    // write warning if netedit is running in testing mode
                    WRITE_DEBUG("Closed FXMessageBox 'restrict lanes' with 'Yes'");
                }
            }
            // begin undo operation
            myUndoList->p_begin("restrict lanes to " + toString(vclass));
            // iterate over selected lanes
            for (std::map<GNEEdge*, GNELane*>::iterator i = mapOfEdgesAndLanes.begin(); i != mapOfEdgesAndLanes.end(); i++) {
                // Transform lane to Sidewalk
                myNet->restrictLane(vclass, i->second, myUndoList);
            }
            // end undo operation
            myUndoList->p_end();
        } else {
            // If only have a single lane, start undo/redo operation
            myUndoList->p_begin("restrict lane to " + toString(vclass));
            // Transform lane to Sidewalk
            myNet->restrictLane(vclass, laneAtPopupPosition, myUndoList);
            // end undo operation
            myUndoList->p_end();
        }
    }
    return 1;
}


bool
GNEViewNet::addRestrictedLane(SUMOVehicleClass vclass, const bool insertAtFront) {
    GNELane* laneAtPopupPosition = getLaneAtPopupPosition();
    if (laneAtPopupPosition != nullptr) {
        // Get selected edges
        std::vector<GNEEdge*> edges = myNet->retrieveEdges(true);
        // get selected lanes
        std::vector<GNELane*> lanes = myNet->retrieveLanes(true);
        // Declare set of edges
        std::set<GNEEdge*> setOfEdges;
        // Fill set of edges with vector of edges
        for (const auto &edge : edges) {
            setOfEdges.insert(edge);
        }
        // iterate over selected lanes
        for (const auto &lane : lanes) {
            // Insert pointer to edge into set of edges (To avoid duplicates)
            setOfEdges.insert(myNet->retrieveEdge(lane->getParentEdge()->getID()));
        }
        // If we handeln a set of edges
        if (setOfEdges.size() > 0) {
            // declare counter for number of restrictions
            int counter = 0;
            // iterate over set of edges
            for (std::set<GNEEdge*>::iterator it = setOfEdges.begin(); it != setOfEdges.end(); it++) {
                // update counter if edge has already a restricted lane of type "vclass"
                if ((*it)->hasRestrictedLane(vclass)) {
                    counter++;
                }
            }
            // if all lanes own a Sidewalk, stop function
            if (counter == (int)setOfEdges.size()) {
                FXMessageBox::information(getApp(), MBOX_OK,
                                          ("Add vclass for" + toString(vclass) + " to selected lanes").c_str(), "%s",
                                          ("All lanes own already another lane in the same edge with a restriction for " + toString(vclass)).c_str());
                return 0;
            } else {
                WRITE_DEBUG("Opening FXMessageBox 'restrict lanes'");
                // Ask confirmation to user
                FXuint answer = FXMessageBox::question(getApp(), MBOX_YES_NO,
                                                       ("Add vclass for " + toString(vclass) + " to selected lanes").c_str(), "%s",
                                                       (toString(setOfEdges.size() - counter) + " restrictions for " + toString(vclass) + " will be added. continue?").c_str());
                if (answer != 1) { //1:yes, 2:no, 4:esc
                    // write warning if netedit is running in testing mode
                    if (answer == 2) {
                        WRITE_DEBUG("Closed FXMessageBox 'restrict lanes' with 'No'");
                    } else if (answer == 4) {
                        WRITE_DEBUG("Closed FXMessageBox 'restrict lanes' with 'ESC'");
                    }
                    return 0;
                } else {
                    // write warning if netedit is running in testing mode
                    WRITE_DEBUG("Closed FXMessageBox 'restrict lanes' with 'Yes'");
                }
            }
            // begin undo operation
            myUndoList->p_begin("Add restrictions for " + toString(vclass));
            // iterate over set of edges
            for (const auto& edge : setOfEdges) {
                // add restricted lane (guess target)
                myNet->addRestrictedLane(vclass, edge, -1, myUndoList);
            }
            // end undo operation
            myUndoList->p_end();
        } else {
            // If only have a single lane, start undo/redo operation
            myUndoList->p_begin("Add vclass for " + toString(vclass));
            // Add restricted lane
            if (vclass == SVC_PEDESTRIAN) {
                // always add pedestrian lanes on the right
                myNet->addRestrictedLane(vclass, laneAtPopupPosition->getParentEdge(), 0, myUndoList);
            } else if (vclass == SVC_IGNORING) {
                if (insertAtFront) {
                    myNet->addGreenVergeLane(laneAtPopupPosition->getParentEdge(), laneAtPopupPosition->getIndex() + 1, myUndoList);
                } else {
                    myNet->addGreenVergeLane(laneAtPopupPosition->getParentEdge(), laneAtPopupPosition->getIndex(), myUndoList);
                }
            } else if (laneAtPopupPosition->getParentEdge()->getLanes().size() == 1) {
                // guess insertion position if there is only 1 lane
                myNet->addRestrictedLane(vclass, laneAtPopupPosition->getParentEdge(), -1, myUndoList);
            } else {
                myNet->addRestrictedLane(vclass, laneAtPopupPosition->getParentEdge(), laneAtPopupPosition->getIndex(), myUndoList);
            }
            // end undo/redo operation
            myUndoList->p_end();
        }
    }
    return 1;
}


bool
GNEViewNet::removeRestrictedLane(SUMOVehicleClass vclass) {
    GNELane* laneAtPopupPosition = getLaneAtPopupPosition();
    if (laneAtPopupPosition != nullptr) {
        // Get selected edges
        std::vector<GNEEdge*> edges = myNet->retrieveEdges(true);
        // get selected lanes
        std::vector<GNELane*> lanes = myNet->retrieveLanes(true);
        // Declare set of edges
        std::set<GNEEdge*> setOfEdges;
        // Fill set of edges with vector of edges
        for (const auto &edge : edges) {
            setOfEdges.insert(edge);
        }
        // iterate over selected lanes
        for (const auto &lane : lanes) {
            // Insert pointer to edge into set of edges (To avoid duplicates)
            setOfEdges.insert(myNet->retrieveEdge(lane->getParentEdge()->getID()));
        }
        // If we handeln a set of edges
        if (setOfEdges.size() > 0) {
            // declare counter for number of restrictions
            int counter = 0;
            // iterate over set of edges
            for (const auto & edge : setOfEdges) {
                // update counter if edge has already a restricted lane of type "vclass"
                if (edge->hasRestrictedLane(vclass)) {
                    counter++;
                }
            }
            // if all lanes don't own a Sidewalk, stop function
            if (counter == 0) {
                FXMessageBox::information(getApp(), MBOX_OK,
                                          ("Remove vclass for " + toString(vclass) + " to selected lanes").c_str(), "%s",
                                          ("Selected lanes and edges haven't a restriction for " + toString(vclass)).c_str());
                return 0;
            } else {
                WRITE_DEBUG("Opening FXMessageBox 'restrict lanes'");
                // Ask confirmation to user
                FXuint answer = FXMessageBox::question(getApp(), MBOX_YES_NO,
                                                       ("Remove vclass for " + toString(vclass) + " to selected lanes").c_str(), "%s",
                                                       (toString(counter) + " restrictions for " + toString(vclass) + " will be removed. continue?").c_str());
                if (answer != 1) { //1:yes, 2:no, 4:esc
                    // write warning if netedit is running in testing mode
                    if (answer == 2) {
                        WRITE_DEBUG("Closed FXMessageBox 'restrict lanes' with 'No'");
                    } else if (answer == 4) {
                        WRITE_DEBUG("Closed FXMessageBox 'restrict lanes' with 'ESC'");
                    }
                    return 0;
                } else {
                    // write warning if netedit is running in testing mode
                    WRITE_DEBUG("Closed FXMessageBox 'restrict lanes' with 'Yes'");
                }
            }
            // begin undo operation
            myUndoList->p_begin("Remove restrictions for " + toString(vclass));
            // iterate over set of edges
            for (const auto& edge : setOfEdges) {
                // add Sidewalk
                myNet->removeRestrictedLane(vclass, edge, myUndoList);
            }
            // end undo operation
            myUndoList->p_end();
        } else {
            // If only have a single lane, start undo/redo operation
            myUndoList->p_begin("Remove vclass for " + toString(vclass));
            // Remove Sidewalk
            myNet->removeRestrictedLane(vclass, laneAtPopupPosition->getParentEdge(), myUndoList);
            // end undo/redo operation
            myUndoList->p_end();
        }
    }
    return 1;
}


void
GNEViewNet::processClick(void* eventData) {
    FXEvent* evt = (FXEvent*)eventData;
    // process click
    destroyPopup();
    setFocus();
    myChanger->onLeftBtnPress(eventData);
    grab();
    // Check there are double click
    if (evt->click_count == 2) {
        handle(this, FXSEL(SEL_DOUBLECLICKED, 0), eventData);
    }
}


void
GNEViewNet::updateCursor() {
    // declare flags
    bool cursorMoveView = false;
    bool cursorInspect = false;
    bool cursorSelect = false;
    bool cursorMoveElement = false;
    bool cursorDelete = false;
    // continue depending of supermode
    if (myEditModes.isCurrentSupermodeNetwork()) {
        // move view
        if ((myEditModes.networkEditMode == NetworkEditMode::NETWORK_SELECT) ||
                (myEditModes.networkEditMode == NetworkEditMode::NETWORK_CREATE_EDGE) ||
                (myEditModes.networkEditMode == NetworkEditMode::NETWORK_ADDITIONAL) ||
                (myEditModes.networkEditMode == NetworkEditMode::NETWORK_POLYGON) ||
                (myEditModes.networkEditMode == NetworkEditMode::NETWORK_TAZ)) {
            cursorMoveView = true;
        }
        // specific mode
        if (myEditModes.networkEditMode == NetworkEditMode::NETWORK_INSPECT) {
            cursorInspect = true;
        } else if (myEditModes.networkEditMode == NetworkEditMode::NETWORK_SELECT) {
            cursorSelect = true;
        } else if (myEditModes.networkEditMode == NetworkEditMode::NETWORK_MOVE) {
            cursorMoveElement = true;
        } else if (myEditModes.networkEditMode == NetworkEditMode::NETWORK_DELETE) {
            cursorDelete = true;
        }
    } else if (myEditModes.isCurrentSupermodeDemand()) {
        // move view
        if ((myEditModes.demandEditMode == DemandEditMode::DEMAND_SELECT) ||
                (myEditModes.demandEditMode == DemandEditMode::DEMAND_VEHICLE) ||
                (myEditModes.demandEditMode == DemandEditMode::DEMAND_STOP)) {
            cursorMoveView = true;
        }
        // specific mode
        if (myEditModes.demandEditMode == DemandEditMode::DEMAND_INSPECT) {
            cursorInspect = true;
        } else if (myEditModes.demandEditMode == DemandEditMode::DEMAND_SELECT) {
            cursorSelect = true;
        } else if (myEditModes.demandEditMode == DemandEditMode::DEMAND_MOVE) {
            cursorMoveElement = true;
        } else if (myEditModes.demandEditMode == DemandEditMode::DEMAND_DELETE) {
            cursorDelete = true;
        }
    } else if (myEditModes.isCurrentSupermodeData()) {
        // move view
        if (myEditModes.dataEditMode == DataEditMode::DATA_SELECT) {
            cursorMoveView = true;
        }
        // specific mode
        if (myEditModes.dataEditMode == DataEditMode::DATA_INSPECT) {
            cursorInspect = true;
        } else if (myEditModes.dataEditMode == DataEditMode::DATA_SELECT) {
            cursorSelect = true;
        } else if (myEditModes.dataEditMode == DataEditMode::DATA_DELETE) {
            cursorDelete = true;
        }
    }
    // set cursor
    if (myMouseButtonKeyPressed.controlKeyPressed() && cursorMoveView) {
        // move view cursor if control key is pressed
        setDefaultCursor(GUICursorSubSys::getCursor(GUICursor::MOVEVIEW));
        setDragCursor(GUICursorSubSys::getCursor(GUICursor::MOVEVIEW));
    } else if (cursorInspect) {
        // special case for inspect lanes
        if (myNetworkViewOptions.selectEdges() && myMouseButtonKeyPressed.shiftKeyPressed() &&
                myEditModes.isCurrentSupermodeNetwork() && (myEditModes.networkEditMode == NetworkEditMode::NETWORK_INSPECT)) {
            // inspect lane cursor
            setDefaultCursor(GUICursorSubSys::getCursor(GUICursor::INSPECT_LANE));
            setDragCursor(GUICursorSubSys::getCursor(GUICursor::INSPECT_LANE));
        } else {
            // inspect cursor
            setDefaultCursor(GUICursorSubSys::getCursor(GUICursor::INSPECT));
            setDragCursor(GUICursorSubSys::getCursor(GUICursor::INSPECT));
        }
    } else if (cursorSelect) {
        // special case for select lanes
        if (myNetworkViewOptions.selectEdges() && myMouseButtonKeyPressed.shiftKeyPressed() &&
                myEditModes.isCurrentSupermodeNetwork() && (myEditModes.networkEditMode == NetworkEditMode::NETWORK_SELECT)) {
            // select lane cursor
            setDefaultCursor(GUICursorSubSys::getCursor(GUICursor::SELECT_LANE));
            setDragCursor(GUICursorSubSys::getCursor(GUICursor::SELECT_LANE));
        } else {
            // select cursor
            setDefaultCursor(GUICursorSubSys::getCursor(GUICursor::SELECT));
            setDragCursor(GUICursorSubSys::getCursor(GUICursor::SELECT));
        }
    } else if (cursorMoveElement) {
        // move cursor
        setDefaultCursor(GUICursorSubSys::getCursor(GUICursor::MOVEELEMENT));
        setDragCursor(GUICursorSubSys::getCursor(GUICursor::MOVEELEMENT));
    } else if (cursorDelete) {
        // delete cursor
        setDefaultCursor(GUICursorSubSys::getCursor(GUICursor::DELETE_CURSOR));
        setDragCursor(GUICursorSubSys::getCursor(GUICursor::DELETE_CURSOR));
    } else {
        // default cursor
        setDefaultCursor(GUICursorSubSys::getCursor(GUICursor::DEFAULT));
        setDragCursor(GUICursorSubSys::getCursor(GUICursor::DEFAULT));
    }
}


long
GNEViewNet::onCmdEditJunctionShape(FXObject*, FXSelector, void*) {
    // Obtain junction under mouse
    GNEJunction* junction = getJunctionAtPopupPosition();
    if (junction) {
        // check if network has to be updated
        if (junction->getNBNode()->getShape().size() == 0) {
            // recompute the whole network
            myNet->computeAndUpdate(OptionsCont::getOptions(), false);
        }
        // start edit custom shape
        myEditNetworkElementShapes.startEditCustomShape(junction);
    }
    // destroy pop-up and set focus in view net
    destroyPopup();
    setFocus();
    return 1;
}


long
GNEViewNet::onCmdResetJunctionShape(FXObject*, FXSelector, void*) {
    // Obtain junction under mouse
    GNEJunction* junction = getJunctionAtPopupPosition();
    if (junction) {
        // are, otherwise recompute them
        if (junction->isAttributeCarrierSelected()) {
            myUndoList->p_begin("reset custom junction shapes");
            std::vector<GNEJunction*> junctions = myNet->retrieveJunctions(true);
            for (auto it : junctions) {
                it->setAttribute(SUMO_ATTR_SHAPE, "", myUndoList);
            }
            myUndoList->p_end();
        } else {
            myUndoList->p_begin("reset custom junction shape");
            junction->setAttribute(SUMO_ATTR_SHAPE, "", myUndoList);
            myUndoList->p_end();
        }
    }
    // destroy pop-up and set focus in view net
    destroyPopup();
    setFocus();
    return 1;
}


long
GNEViewNet::onCmdReplaceJunction(FXObject*, FXSelector, void*) {
    GNEJunction* junction = getJunctionAtPopupPosition();
    if (junction != nullptr) {
        myNet->replaceJunctionByGeometry(junction, myUndoList);
        updateViewNet();
    }
    // destroy pop-up and set focus in view net
    destroyPopup();
    setFocus();
    return 1;
}


long
GNEViewNet::onCmdSplitJunction(FXObject*, FXSelector, void*) {
    GNEJunction* junction = getJunctionAtPopupPosition();
    if (junction != nullptr) {
        myNet->splitJunction(junction, false, myUndoList);
        updateViewNet();
    }
    // destroy pop-up and set focus in view net
    destroyPopup();
    setFocus();
    return 1;
}


long
GNEViewNet::onCmdSplitJunctionReconnect(FXObject*, FXSelector, void*) {
    GNEJunction* junction = getJunctionAtPopupPosition();
    if (junction != nullptr) {
        myNet->splitJunction(junction, true, myUndoList);
        updateViewNet();
    }
    // destroy pop-up and set focus in view net
    destroyPopup();
    setFocus();
    return 1;
}

long
GNEViewNet::onCmdSelectRoundabout(FXObject*, FXSelector, void*) {
    GNEJunction* junction = getJunctionAtPopupPosition();
    if (junction != nullptr) {
        myNet->selectRoundabout(junction, myUndoList);
        updateViewNet();
    }
    // destroy pop-up and set focus in view net
    destroyPopup();
    setFocus();
    return 1;
}

long
GNEViewNet::onCmdConvertRoundabout(FXObject*, FXSelector, void*) {
    GNEJunction* junction = getJunctionAtPopupPosition();
    if (junction != nullptr) {
        myNet->createRoundabout(junction, myUndoList);
        updateViewNet();
    }
    // destroy pop-up and set focus in view net
    destroyPopup();
    setFocus();
    return 1;
}


long
GNEViewNet::onCmdClearConnections(FXObject*, FXSelector, void*) {
    GNEJunction* junction = getJunctionAtPopupPosition();
    if (junction != nullptr) {
        // make sure we do not inspect the connection will it is being deleted
        if ((myInspectedAttributeCarriers.size() > 0) && (myInspectedAttributeCarriers.front()->getTagProperty().getTag() == SUMO_TAG_CONNECTION)) {
            myViewParent->getInspectorFrame()->clearInspectedAC();
        }
        // make sure that connections isn't the front attribute
        if (myFrontAttributeCarrier != nullptr && (myFrontAttributeCarrier->getTagProperty().getTag() == SUMO_TAG_CONNECTION)) {
            myFrontAttributeCarrier = nullptr;
        }
        // check if we're handling a selection
        if (junction->isAttributeCarrierSelected()) {
            std::vector<GNEJunction*> selectedJunction = myNet->retrieveJunctions(true);
            myUndoList->p_begin("clear connections of selected junctions");
            for (auto i : selectedJunction) {
                myNet->clearJunctionConnections(i, myUndoList);
            }
            myUndoList->p_end();
        } else {
            myNet->clearJunctionConnections(junction, myUndoList);
        }
        updateViewNet();
    }
    // destroy pop-up and set focus in view net
    destroyPopup();
    setFocus();
    return 1;
}


long
GNEViewNet::onCmdResetConnections(FXObject*, FXSelector, void*) {
    GNEJunction* junction = getJunctionAtPopupPosition();
    if (junction != nullptr) {
        // make sure we do not inspect the connection will it is being deleted
        if ((myInspectedAttributeCarriers.size() > 0) && myInspectedAttributeCarriers.front()->getTagProperty().getTag() == SUMO_TAG_CONNECTION) {
            myViewParent->getInspectorFrame()->clearInspectedAC();
        }
        // make sure that connections isn't the front attribute
        if (myFrontAttributeCarrier != nullptr && (myFrontAttributeCarrier->getTagProperty().getTag() == SUMO_TAG_CONNECTION)) {
            myFrontAttributeCarrier = nullptr;
        }
        // check if we're handling a selection
        if (junction->isAttributeCarrierSelected()) {
            std::vector<GNEJunction*> selectedJunction = myNet->retrieveJunctions(true);
            myUndoList->p_begin("reset connections of selected junctions");
            for (auto i : selectedJunction) {
                myNet->resetJunctionConnections(i, myUndoList);
            }
            myUndoList->p_end();
        } else {
            myNet->resetJunctionConnections(junction, myUndoList);
        }
        updateViewNet();
    }
    // destroy pop-up and set focus in view net
    destroyPopup();
    setFocus();
    return 1;
}


long
GNEViewNet::onCmdEditConnectionShape(FXObject*, FXSelector, void*) {
    // Obtain connection under mouse
    GNEConnection* connection = getConnectionAtPopupPosition();
    if (connection) {
        myEditNetworkElementShapes.startEditCustomShape(connection);
    }
    // destroy pop-up and update view Net
    destroyPopup();
    setFocus();
    return 1;
}


long
GNEViewNet::onCmdEditCrossingShape(FXObject*, FXSelector, void*) {
    // Obtain crossing under mouse
    GNECrossing* crossing = getCrossingAtPopupPosition();
    if (crossing) {
        // due crossings haven two shapes, check what has to be edited
        PositionVector shape = crossing->getNBCrossing()->customShape.size() > 0 ? crossing->getNBCrossing()->customShape : crossing->getNBCrossing()->shape;
        // check if network has to be updated
        if (crossing->getParentJunction()->getNBNode()->getShape().size() == 0) {
            // recompute the whole network
            myNet->computeAndUpdate(OptionsCont::getOptions(), false);
        }
        // start edit custom shape
        myEditNetworkElementShapes.startEditCustomShape(crossing);
    }
    // destroy pop-up and update view Net
    destroyPopup();
    setFocus();
    return 1;
}


long
GNEViewNet::onCmdToggleSelectEdges(FXObject*, FXSelector sel, void*) {
    // Toggle menuCheckSelectEdges
    if (myNetworkViewOptions.menuCheckSelectEdges->amChecked() == TRUE) {
        myNetworkViewOptions.menuCheckSelectEdges->setChecked(FALSE);
    } else {
        myNetworkViewOptions.menuCheckSelectEdges->setChecked(TRUE);
    }
    myNetworkViewOptions.menuCheckSelectEdges->update();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_SELECTEDGES)) {
        myNetworkViewOptions.menuCheckSelectEdges->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToggleShowConnections(FXObject*, FXSelector sel, void*) {
    // Toggle menuCheckShowConnections
    if (myNetworkViewOptions.menuCheckShowConnections->amChecked() == TRUE) {
        myNetworkViewOptions.menuCheckShowConnections->setChecked(FALSE);
    } else {
        myNetworkViewOptions.menuCheckShowConnections->setChecked(TRUE);
    }
    myNetworkViewOptions.menuCheckShowConnections->update();
    // if show was enabled, init GNEConnections
    if (myNetworkViewOptions.menuCheckShowConnections->amChecked() == TRUE) {
        getNet()->initGNEConnections();
    }
    // change flag "showLane2Lane" in myVisualizationSettings
    myVisualizationSettings->showLane2Lane = (myNetworkViewOptions.menuCheckShowConnections->amChecked() == TRUE);
    // Hide/show connections require recompute
    getNet()->requireRecompute();
    // Update viewnNet to show/hide conections
    updateViewNet();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_SHOWCONNECTIONS)) {
        myNetworkViewOptions.menuCheckShowConnections->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToggleHideConnections(FXObject*, FXSelector sel, void*) {
    // Toggle menuCheckHideConnections
    if (myNetworkViewOptions.menuCheckHideConnections->amChecked() == TRUE) {
        myNetworkViewOptions.menuCheckHideConnections->setChecked(FALSE);
    } else {
        myNetworkViewOptions.menuCheckHideConnections->setChecked(TRUE);
    }
    myNetworkViewOptions.menuCheckHideConnections->update();
    // Update viewnNet to show/hide conections
    updateViewNet();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_HIDECONNECTIONS)) {
        myNetworkViewOptions.menuCheckHideConnections->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToggleExtendSelection(FXObject*, FXSelector sel, void*) {
    // Toggle menuCheckExtendSelection
    if (myNetworkViewOptions.menuCheckExtendSelection->amChecked() == TRUE) {
        myNetworkViewOptions.menuCheckExtendSelection->setChecked(FALSE);
    } else {
        myNetworkViewOptions.menuCheckExtendSelection->setChecked(TRUE);
    }
    myNetworkViewOptions.menuCheckExtendSelection->update();
    // Only update view
    updateViewNet();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_EXTENDSELECTION)) {
        myNetworkViewOptions.menuCheckExtendSelection->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToggleChangeAllPhases(FXObject*, FXSelector sel, void*) {
    // Toggle menuCheckChangeAllPhases
    if (myNetworkViewOptions.menuCheckChangeAllPhases->amChecked() == TRUE) {
        myNetworkViewOptions.menuCheckChangeAllPhases->setChecked(FALSE);
    } else {
        myNetworkViewOptions.menuCheckChangeAllPhases->setChecked(TRUE);
    }
    myNetworkViewOptions.menuCheckChangeAllPhases->update();
    // Only update view
    updateViewNet();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_CHANGEALLPHASES)) {
        myNetworkViewOptions.menuCheckChangeAllPhases->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToggleShowGrid(FXObject*, FXSelector sel, void*) {
    // show or hidde grid depending of myNetworkViewOptions.menuCheckToggleGrid
    if (myVisualizationSettings->showGrid) {
        myVisualizationSettings->showGrid = false;
        myNetworkViewOptions.menuCheckToggleGrid->setChecked(false);
        myDemandViewOptions.menuCheckToggleGrid->setChecked(false);
    } else {
        myVisualizationSettings->showGrid = true;
        myNetworkViewOptions.menuCheckToggleGrid->setChecked(true);
        myDemandViewOptions.menuCheckToggleGrid->setChecked(true);
    }
    myNetworkViewOptions.menuCheckToggleGrid->update();
    myDemandViewOptions.menuCheckToggleGrid->update();
    // update view to show grid
    updateViewNet();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_TOGGLEGRID)) {
        myNetworkViewOptions.menuCheckToggleGrid->setFocus();
    } else if (sel == FXSEL(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_SHOWGRID)) {
        myDemandViewOptions.menuCheckToggleGrid->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToggleDrawSpreadVehicles(FXObject*, FXSelector sel, void*) {
    // Toggle menuCheckShowDemandElements
    if ((myNetworkViewOptions.menuCheckDrawSpreadVehicles->amChecked() == TRUE) ||
            (myDemandViewOptions.menuCheckDrawSpreadVehicles->amChecked() == TRUE)) {
        myNetworkViewOptions.menuCheckDrawSpreadVehicles->setChecked(FALSE);
        myDemandViewOptions.menuCheckDrawSpreadVehicles->setChecked(FALSE);
    } else {
        myNetworkViewOptions.menuCheckDrawSpreadVehicles->setChecked(TRUE);
        myDemandViewOptions.menuCheckDrawSpreadVehicles->setChecked(TRUE);
    }
    myNetworkViewOptions.menuCheckDrawSpreadVehicles->update();
    myDemandViewOptions.menuCheckDrawSpreadVehicles->update();
    // declare edge set
    std::set<GNEEdge*> edgesToUpdate;
    // compute vehicle geometry
    for (const auto& vehicle : myNet->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_VEHICLE)) {
        if (vehicle.second->getParentEdges().size() > 0) {
            edgesToUpdate.insert(vehicle.second->getParentEdges().front());
        } else if (vehicle.second->getChildDemandElements().size() > 0 && (vehicle.second->getChildDemandElements().front()->getTagProperty().getTag() == GNE_TAG_ROUTE_EMBEDDED)) {
            edgesToUpdate.insert(vehicle.second->getChildDemandElements().front()->getParentEdges().front());
        }
    }
    for (const auto& routeFlow : myNet->getAttributeCarriers()->getDemandElements().at(GNE_TAG_FLOW_ROUTE)) {
        if (routeFlow.second->getParentEdges().size() > 0) {
            edgesToUpdate.insert(routeFlow.second->getParentEdges().front());
        } else if (routeFlow.second->getChildDemandElements().size() > 0 && (routeFlow.second->getChildDemandElements().front()->getTagProperty().getTag() == GNE_TAG_ROUTE_EMBEDDED)) {
            edgesToUpdate.insert(routeFlow.second->getChildDemandElements().front()->getParentEdges().front());
        }
    }
    for (const auto& trip : myNet->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_TRIP)) {
        if (trip.second->getParentEdges().size() > 0) {
            edgesToUpdate.insert(trip.second->getParentEdges().front());
        }
    }
    for (const auto& flow : myNet->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_FLOW)) {
        if (flow.second->getParentEdges().size() > 0) {
            edgesToUpdate.insert(flow.second->getParentEdges().front());
        }
    }
    // update spread geometries of all edges
    for (const auto& edge : edgesToUpdate) {
        edge->updateVehicleSpreadGeometries();
    }
    // update view to show new vehicles positions
    updateViewNet();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_DRAWSPREADVEHICLES)) {
        myNetworkViewOptions.menuCheckDrawSpreadVehicles->setFocus();
    } else if (sel == FXSEL(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_DRAWSPREADVEHICLES)) {
        myDemandViewOptions.menuCheckDrawSpreadVehicles->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToggleWarnAboutMerge(FXObject*, FXSelector sel, void*) {
    // Toggle menuCheckWarnAboutMerge
    if (myNetworkViewOptions.menuCheckWarnAboutMerge->amChecked() == TRUE) {
        myNetworkViewOptions.menuCheckWarnAboutMerge->setChecked(FALSE);
    } else {
        myNetworkViewOptions.menuCheckWarnAboutMerge->setChecked(TRUE);
    }
    myNetworkViewOptions.menuCheckWarnAboutMerge->update();
    // Only update view
    updateViewNet();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_ASKFORMERGE)) {
        myNetworkViewOptions.menuCheckWarnAboutMerge->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToggleShowJunctionBubbles(FXObject*, FXSelector sel, void*) {
    // Toggle menuCheckShowJunctionBubble
    if (myNetworkViewOptions.menuCheckShowJunctionBubble->amChecked() == TRUE) {
        myNetworkViewOptions.menuCheckShowJunctionBubble->setChecked(FALSE);
    } else {
        myNetworkViewOptions.menuCheckShowJunctionBubble->setChecked(TRUE);
    }
    myNetworkViewOptions.menuCheckShowJunctionBubble->update();
    // Only update view
    updateViewNet();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_SHOWBUBBLES)) {
        myNetworkViewOptions.menuCheckShowJunctionBubble->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToggleMoveElevation(FXObject*, FXSelector sel, void*) {
    // Toggle menuCheckMoveElevation
    if (myNetworkViewOptions.menuCheckMoveElevation->amChecked() == TRUE) {
        myNetworkViewOptions.menuCheckMoveElevation->setChecked(FALSE);
    } else {
        myNetworkViewOptions.menuCheckMoveElevation->setChecked(TRUE);
    }
    myNetworkViewOptions.menuCheckMoveElevation->update();
    // Only update view
    updateViewNet();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_MOVEELEVATION)) {
        myNetworkViewOptions.menuCheckMoveElevation->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToggleChainEdges(FXObject*, FXSelector sel, void*) {
    // Toggle menuCheckMoveElevation
    if (myNetworkViewOptions.menuCheckChainEdges->amChecked() == TRUE) {
        myNetworkViewOptions.menuCheckChainEdges->setChecked(FALSE);
    } else {
        myNetworkViewOptions.menuCheckChainEdges->setChecked(TRUE);
    }
    myNetworkViewOptions.menuCheckChainEdges->update();
    // Only update view
    updateViewNet();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_CHAINEDGES)) {
        myNetworkViewOptions.menuCheckChainEdges->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToggleAutoOppositeEdge(FXObject*, FXSelector sel, void*) {
    // Toggle menuCheckAutoOppositeEdge
    if (myNetworkViewOptions.menuCheckAutoOppositeEdge->amChecked() == TRUE) {
        myNetworkViewOptions.menuCheckAutoOppositeEdge->setChecked(FALSE);
    } else {
        myNetworkViewOptions.menuCheckAutoOppositeEdge->setChecked(TRUE);
    }
    myNetworkViewOptions.menuCheckAutoOppositeEdge->update();
    // Only update view
    updateViewNet();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_AUTOOPPOSITEEDGES)) {
        myNetworkViewOptions.menuCheckAutoOppositeEdge->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToggleHideNonInspecteDemandElements(FXObject*, FXSelector sel, void*) {
    // Toggle menuCheckHideNonInspectedDemandElements
    if (myDemandViewOptions.menuCheckHideNonInspectedDemandElements->amChecked() == TRUE) {
        myDemandViewOptions.menuCheckHideNonInspectedDemandElements->setChecked(FALSE);
    } else {
        myDemandViewOptions.menuCheckHideNonInspectedDemandElements->setChecked(TRUE);
    }
    myDemandViewOptions.menuCheckHideNonInspectedDemandElements->update();
    // Only update view
    updateViewNet();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_HIDENONINSPECTED)) {
        myDemandViewOptions.menuCheckHideNonInspectedDemandElements->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToggleHideShapes(FXObject*, FXSelector sel, void*) {
    // Toggle menuCheckHideShapes
    if (myDemandViewOptions.menuCheckHideShapes->amChecked() == TRUE) {
        myDemandViewOptions.menuCheckHideShapes->setChecked(FALSE);
    } else {
        myDemandViewOptions.menuCheckHideShapes->setChecked(TRUE);
    }
    myDemandViewOptions.menuCheckHideShapes->update();
    // Only update view
    updateViewNet();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_HIDESHAPES)) {
        myDemandViewOptions.menuCheckHideShapes->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToggleShowTrips(FXObject*, FXSelector sel, void*) {
    // Toggle menuCheckHideShapes
    if (myDemandViewOptions.menuCheckShowAllTrips->amChecked() == TRUE) {
        myDemandViewOptions.menuCheckShowAllTrips->setChecked(FALSE);
    } else {
        myDemandViewOptions.menuCheckShowAllTrips->setChecked(TRUE);
    }
    myDemandViewOptions.menuCheckShowAllTrips->update();
    // Only update view
    updateViewNet();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_SHOWTRIPS)) {
        myDemandViewOptions.menuCheckShowAllTrips->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToggleShowAllPersonPlans(FXObject*, FXSelector sel, void*) {
    // Toggle menuCheckShowAllPersonPlans
    if (myDemandViewOptions.menuCheckShowAllPersonPlans->amChecked() == TRUE) {
        myDemandViewOptions.menuCheckShowAllPersonPlans->setChecked(FALSE);
    } else {
        myDemandViewOptions.menuCheckShowAllPersonPlans->setChecked(TRUE);
    }
    myDemandViewOptions.menuCheckShowAllPersonPlans->update();
    // Only update view
    updateViewNet();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_SHOWALLPERSONPLANS)) {
        myDemandViewOptions.menuCheckShowAllPersonPlans->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToggleLockPerson(FXObject*, FXSelector sel, void*) {
    // Toggle menuCheckLockPerson
    if (myDemandViewOptions.menuCheckLockPerson->amChecked() == TRUE) {
        myDemandViewOptions.menuCheckLockPerson->setChecked(FALSE);
    } else if ((myInspectedAttributeCarriers.size() > 0) && myInspectedAttributeCarriers.front()->getTagProperty().isPerson()) {
        myDemandViewOptions.menuCheckLockPerson->setChecked(TRUE);
    }
    myDemandViewOptions.menuCheckLockPerson->update();
    // lock or unlock current inspected person depending of menuCheckLockPerson value
    if (myDemandViewOptions.menuCheckLockPerson->amChecked()) {
        // obtan locked person or person plan
        const GNEDemandElement* personOrPersonPlan = dynamic_cast<const GNEDemandElement*>(myInspectedAttributeCarriers.front());
        if (personOrPersonPlan) {
            // lock person depending if casted demand element is either a person or a person plan
            if (personOrPersonPlan->getTagProperty().isPerson()) {
                myDemandViewOptions.lockPerson(personOrPersonPlan);
            } else {
                myDemandViewOptions.lockPerson(personOrPersonPlan->getParentDemandElements().front());
            }
        }
    } else {
        // unlock current person
        myDemandViewOptions.unlockPerson();
    }
    // update view
    updateViewNet();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_LOCKPERSON)) {
        myDemandViewOptions.menuCheckLockPerson->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToggleShowAllContainerPlans(FXObject*, FXSelector sel, void*) {
    // Toggle menuCheckShowAllContainerPlans
    if (myDemandViewOptions.menuCheckShowAllContainerPlans->amChecked() == TRUE) {
        myDemandViewOptions.menuCheckShowAllContainerPlans->setChecked(FALSE);
    } else {
        myDemandViewOptions.menuCheckShowAllContainerPlans->setChecked(TRUE);
    }
    myDemandViewOptions.menuCheckShowAllContainerPlans->update();
    // Only update view
    updateViewNet();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_SHOWALLCONTAINERPLANS)) {
        myDemandViewOptions.menuCheckShowAllContainerPlans->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToggleLockContainer(FXObject*, FXSelector sel, void*) {
    // Toggle menuCheckLockContainer
    if (myDemandViewOptions.menuCheckLockContainer->amChecked() == TRUE) {
        myDemandViewOptions.menuCheckLockContainer->setChecked(FALSE);
    } else if ((myInspectedAttributeCarriers.size() > 0) && myInspectedAttributeCarriers.front()->getTagProperty().isContainer()) {
        myDemandViewOptions.menuCheckLockContainer->setChecked(TRUE);
    }
    myDemandViewOptions.menuCheckLockContainer->update();
    // lock or unlock current inspected container depending of menuCheckLockContainer value
    if (myDemandViewOptions.menuCheckLockContainer->amChecked()) {
        // obtan locked container or container plan
        const GNEDemandElement* containerOrContainerPlan = dynamic_cast<const GNEDemandElement*>(myInspectedAttributeCarriers.front());
        if (containerOrContainerPlan) {
            // lock container depending if casted demand element is either a container or a container plan
            if (containerOrContainerPlan->getTagProperty().isContainer()) {
                myDemandViewOptions.lockContainer(containerOrContainerPlan);
            } else {
                myDemandViewOptions.lockContainer(containerOrContainerPlan->getParentDemandElements().front());
            }
        }
    } else {
        // unlock current container
        myDemandViewOptions.unlockContainer();
    }
    // update view
    updateViewNet();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_LOCKCONTAINER)) {
        myDemandViewOptions.menuCheckLockContainer->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToggleShowAdditionals(FXObject*, FXSelector sel, void*) {
    // Toggle menuCheckShowAdditionals
    if (myDataViewOptions.menuCheckShowAdditionals->amChecked() == TRUE) {
        myDataViewOptions.menuCheckShowAdditionals->setChecked(FALSE);
    } else {
        myDataViewOptions.menuCheckShowAdditionals->setChecked(TRUE);
    }
    myDataViewOptions.menuCheckShowAdditionals->update();
    // Only update view
    updateViewNet();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_DATAVIEWOPTIONS_SHOWADDITIONALS)) {
        myDataViewOptions.menuCheckShowAdditionals->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToggleShowShapes(FXObject*, FXSelector sel, void*) {
    // Toggle menuCheckShowShapes
    if (myDataViewOptions.menuCheckShowShapes->amChecked() == TRUE) {
        myDataViewOptions.menuCheckShowShapes->setChecked(FALSE);
    } else {
        myDataViewOptions.menuCheckShowShapes->setChecked(TRUE);
    }
    myDataViewOptions.menuCheckShowShapes->update();
    // Only update view
    updateViewNet();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_DATAVIEWOPTIONS_SHOWSHAPES)) {
        myDataViewOptions.menuCheckShowShapes->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToggleShowDemandElementsNetwork(FXObject*, FXSelector sel, void*) {
    // Toggle menuCheckShowDemandElements
    if (myNetworkViewOptions.menuCheckShowDemandElements->amChecked() == TRUE) {
        myNetworkViewOptions.menuCheckShowDemandElements->setChecked(FALSE);
    } else {
        myNetworkViewOptions.menuCheckShowDemandElements->setChecked(TRUE);
    }
    myNetworkViewOptions.menuCheckShowDemandElements->update();
    // compute demand elements
    myNet->computeDemandElements(myViewParent->getGNEAppWindows());
    // update view to show demand elements
    updateViewNet();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_SHOWDEMANDELEMENTS)) {
        myNetworkViewOptions.menuCheckShowDemandElements->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToggleShowDemandElementsData(FXObject*, FXSelector sel, void*) {
    // Toggle menuCheckShowDemandElements
    if (myDataViewOptions.menuCheckShowDemandElements->amChecked() == TRUE) {
        myDataViewOptions.menuCheckShowDemandElements->setChecked(FALSE);
    } else {
        myDataViewOptions.menuCheckShowDemandElements->setChecked(TRUE);
    }
    myDataViewOptions.menuCheckShowDemandElements->update();
    // compute demand elements
    myNet->computeDemandElements(myViewParent->getGNEAppWindows());
    // update view to show demand elements
    updateViewNet();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_DATAVIEWOPTIONS_SHOWDEMANDELEMENTS)) {
        myDataViewOptions.menuCheckShowDemandElements->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdIntervalBarGenericDataType(FXObject*, FXSelector, void*) {
    myIntervalBar.setGenericDataType();
    return 1;
}


long
GNEViewNet::onCmdIntervalBarDataSet(FXObject*, FXSelector, void*) {
    myIntervalBar.setDataSet();
    return 1;
}


long
GNEViewNet::onCmdIntervalBarLimit(FXObject*, FXSelector, void*) {
    myIntervalBar.setInterval();
    return 1;
}


long
GNEViewNet::onCmdIntervalBarSetBegin(FXObject*, FXSelector, void*) {
    myIntervalBar.setBegin();
    return 1;
}


long
GNEViewNet::onCmdIntervalBarSetEnd(FXObject*, FXSelector, void*) {
    myIntervalBar.setEnd();
    return 1;
}


long
GNEViewNet::onCmdIntervalBarSetAttribute(FXObject*, FXSelector, void*) {
    myIntervalBar.setAttribute();
    return 1;
}


long
GNEViewNet::onCmdAddSelected(FXObject*, FXSelector, void*) {
    // make GL current (To allow take objects in popup position)
    if (makeCurrent()) {
        int id = getObjectAtPosition(getPopupPosition());
        GNEAttributeCarrier* ACToselect = dynamic_cast <GNEAttributeCarrier*>(GUIGlObjectStorage::gIDStorage.getObjectBlocking(id));
        GUIGlObjectStorage::gIDStorage.unblockObject(id);
        // make sure that AC is selected before selecting
        if (ACToselect && !ACToselect->isAttributeCarrierSelected()) {
            ACToselect->selectAttributeCarrier();
        }
        // make non current
        makeNonCurrent();
    }
    return 1;
}


long
GNEViewNet::onCmdRemoveSelected(FXObject*, FXSelector, void*) {
    // make GL current (To allow take objects in popup position)
    if (makeCurrent()) {
        int id = getObjectAtPosition(getPopupPosition());
        GNEAttributeCarrier* ACToselect = dynamic_cast <GNEAttributeCarrier*>(GUIGlObjectStorage::gIDStorage.getObjectBlocking(id));
        GUIGlObjectStorage::gIDStorage.unblockObject(id);
        // make sure that AC is selected before unselecting
        if (ACToselect && ACToselect->isAttributeCarrierSelected()) {
            ACToselect->unselectAttributeCarrier();
        }
        // make non current
        makeNonCurrent();
    }
    return 1;
}


long
GNEViewNet::onCmdAddEdgeSelected(FXObject*, FXSelector, void*) {
    // make GL current (To allow take objects in popup position)
    if (makeCurrent()) {
        int id = getObjectAtPosition(getPopupPosition());
        // get lane
        GNELane* lane = dynamic_cast <GNELane*>(GUIGlObjectStorage::gIDStorage.getObjectBlocking(id));
        GUIGlObjectStorage::gIDStorage.unblockObject(id);
        // make sure that AC is selected before selecting
        if (lane && !lane->getParentEdge()->isAttributeCarrierSelected()) {
            lane->getParentEdge()->selectAttributeCarrier();
        }
        // make non current
        makeNonCurrent();
    }
    return 1;
}


long
GNEViewNet::onCmdRemoveEdgeSelected(FXObject*, FXSelector, void*) {
    // make GL current (To allow take objects in popup position)
    if (makeCurrent()) {
        int id = getObjectAtPosition(getPopupPosition());
        // get lane
        GNELane* lane = dynamic_cast <GNELane*>(GUIGlObjectStorage::gIDStorage.getObjectBlocking(id));
        GUIGlObjectStorage::gIDStorage.unblockObject(id);
        // make sure that AC is selected before unselecting
        if (lane && lane->getParentEdge()->isAttributeCarrierSelected()) {
            lane->getParentEdge()->unselectAttributeCarrier();
        }
        // make non current
        makeNonCurrent();
    }
    return 1;
}

// ===========================================================================
// private
// ===========================================================================

void
GNEViewNet::buildEditModeControls() {
    // first build supermode buttons
    myEditModes.buildSuperModeButtons();

    // build save buttons
    mySaveElements.buildSaveElementsButtons();

    // build menu checks for Common checkable buttons
    myCommonCheckableButtons.buildCommonCheckableButtons();

    // build menu checks for Network checkable buttons
    myNetworkCheckableButtons.buildNetworkCheckableButtons();

    // build menu checks for Demand checkable buttons
    myDemandCheckableButtons.buildDemandCheckableButtons();

    // build menu checks of view options Data
    myDataCheckableButtons.buildDataCheckableButtons();

    // Create Vertical separator
    new FXVerticalSeparator(myViewParent->getGNEAppWindows()->getToolbarsGrip().modes, GUIDesignVerticalSeparator);
    // XXX for some reason the vertial groove is not visible. adding more spacing to emphasize the separation
    new FXVerticalSeparator(myViewParent->getGNEAppWindows()->getToolbarsGrip().modes, GUIDesignVerticalSeparator);
    new FXVerticalSeparator(myViewParent->getGNEAppWindows()->getToolbarsGrip().modes, GUIDesignVerticalSeparator);

    // build menu checks of view options Network
    myNetworkViewOptions.buildNetworkViewOptionsMenuChecks();

    // build menu checks of view options Demand
    myDemandViewOptions.buildDemandViewOptionsMenuChecks();

    // build menu checks of view options Data
    myDataViewOptions.buildDataViewOptionsMenuChecks();

    // build interval bar
    myIntervalBar.buildIntervalBarElements();
}


void
GNEViewNet::updateNetworkModeSpecificControls() {
    // get menu checks
    const auto& menuChecks = myViewParent->getGNEAppWindows()->getEditMenuCommands().networkViewOptions;
    // hide all checkbox of view options Network
    myNetworkViewOptions.hideNetworkViewOptionsMenuChecks();
    // hide all checkbox of view options Demand
    myDemandViewOptions.hideDemandViewOptionsMenuChecks();
    // hide all checkbox of view options Data
    myDataViewOptions.hideDataViewOptionsMenuChecks();
    // disable all common edit modes
    myCommonCheckableButtons.disableCommonCheckableButtons();
    // disable all network edit modes
    myNetworkCheckableButtons.disableNetworkCheckableButtons();
    // disable all network edit modes
    myDataCheckableButtons.disableDataCheckableButtons();
    // hide interval bar
    myIntervalBar.hideIntervalBar();
    // hide all frames
    myViewParent->hideAllFrames();
    // hide all menuchecks
    myViewParent->getGNEAppWindows()->getEditMenuCommands().networkViewOptions.hideNetworkViewOptionsMenuChecks();
    myViewParent->getGNEAppWindows()->getEditMenuCommands().demandViewOptions.hideDemandViewOptionsMenuChecks();
    myViewParent->getGNEAppWindows()->getEditMenuCommands().dataViewOptions.hideDataViewOptionsMenuChecks();
    // In network mode, always show option "show grid", "draw spread vehicles" and "show demand elements"
    myNetworkViewOptions.menuCheckToggleGrid->show();
    myNetworkViewOptions.menuCheckDrawSpreadVehicles->show();
    myNetworkViewOptions.menuCheckShowDemandElements->show();
    menuChecks.menuCheckToggleGrid->show();
    menuChecks.menuCheckDrawSpreadVehicles->show();
    menuChecks.menuCheckShowDemandElements->show();
    // show separator
    menuChecks.separator->show();
    // enable selected controls
    switch (myEditModes.networkEditMode) {
        // common modes
        case NetworkEditMode::NETWORK_INSPECT:
            myViewParent->getInspectorFrame()->show();
            myViewParent->getInspectorFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getInspectorFrame();
            myCommonCheckableButtons.inspectButton->setChecked(true);
            // show view options
            myNetworkViewOptions.menuCheckSelectEdges->show();
            myNetworkViewOptions.menuCheckShowConnections->show();
            // show menu checks
            menuChecks.menuCheckSelectEdges->show();
            menuChecks.menuCheckShowConnections->show();
            // show
            break;
        case NetworkEditMode::NETWORK_DELETE:
            myViewParent->getDeleteFrame()->show();
            myViewParent->getDeleteFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getDeleteFrame();
            myCommonCheckableButtons.deleteButton->setChecked(true);
            myNetworkViewOptions.menuCheckShowConnections->show();
            // show view options
            myNetworkViewOptions.menuCheckSelectEdges->show();
            myNetworkViewOptions.menuCheckShowConnections->show();
            // show menu checks
            menuChecks.menuCheckSelectEdges->show();
            menuChecks.menuCheckShowConnections->show();
            break;
        case NetworkEditMode::NETWORK_SELECT:
            myViewParent->getSelectorFrame()->show();
            myViewParent->getSelectorFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getSelectorFrame();
            myCommonCheckableButtons.selectButton->setChecked(true);
            // show view options
            myNetworkViewOptions.menuCheckSelectEdges->show();
            myNetworkViewOptions.menuCheckShowConnections->show();
            myNetworkViewOptions.menuCheckExtendSelection->show();
            // show menu checks
            menuChecks.menuCheckSelectEdges->show();
            menuChecks.menuCheckShowConnections->show();
            menuChecks.menuCheckExtendSelection->show();
            break;
        // specific modes
        case NetworkEditMode::NETWORK_CREATE_EDGE:
            myViewParent->getCreateEdgeFrame()->show();
            myViewParent->getCreateEdgeFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getCreateEdgeFrame();
            myNetworkCheckableButtons.createEdgeButton->setChecked(true);
            // show view options
            myNetworkViewOptions.menuCheckChainEdges->show();
            myNetworkViewOptions.menuCheckAutoOppositeEdge->show();
            // show menu checks
            menuChecks.menuCheckChainEdges->show();
            menuChecks.menuCheckAutoOppositeEdge->show();
            break;
        case NetworkEditMode::NETWORK_MOVE:
            myViewParent->getMoveFrame()->show();
            myViewParent->getMoveFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getMoveFrame();
            myNetworkCheckableButtons.moveNetworkElementsButton->setChecked(true);
            // show view options
            myNetworkViewOptions.menuCheckWarnAboutMerge->show();
            myNetworkViewOptions.menuCheckShowJunctionBubble->show();
            myNetworkViewOptions.menuCheckMoveElevation->show();
            // show menu checks
            menuChecks.menuCheckWarnAboutMerge->show();
            menuChecks.menuCheckShowJunctionBubble->show();
            menuChecks.menuCheckMoveElevation->show();
            break;
        case NetworkEditMode::NETWORK_CONNECT:
            myViewParent->getConnectorFrame()->show();
            myViewParent->getConnectorFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getConnectorFrame();
            myNetworkCheckableButtons.connectionButton->setChecked(true);
            break;
        case NetworkEditMode::NETWORK_TLS:
            myViewParent->getTLSEditorFrame()->show();
            myViewParent->getTLSEditorFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getTLSEditorFrame();
            myNetworkCheckableButtons.trafficLightButton->setChecked(true);
            // show view options
            myNetworkViewOptions.menuCheckChangeAllPhases->show();
            // show menu checks
            menuChecks.menuCheckChangeAllPhases->show();
            break;
        case NetworkEditMode::NETWORK_ADDITIONAL:
            myViewParent->getAdditionalFrame()->show();
            myViewParent->getAdditionalFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getAdditionalFrame();
            myNetworkCheckableButtons.additionalButton->setChecked(true);
            break;
        case NetworkEditMode::NETWORK_CROSSING:
            myViewParent->getCrossingFrame()->show();
            myViewParent->getCrossingFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getCrossingFrame();
            myNetworkCheckableButtons.crossingButton->setChecked(true);
            break;
        case NetworkEditMode::NETWORK_TAZ:
            myViewParent->getTAZFrame()->show();
            myViewParent->getTAZFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getTAZFrame();
            myNetworkCheckableButtons.TAZButton->setChecked(true);
            break;
        case NetworkEditMode::NETWORK_POLYGON:
            myViewParent->getPolygonFrame()->show();
            myViewParent->getPolygonFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getPolygonFrame();
            myNetworkCheckableButtons.shapeButton->setChecked(true);
            break;
        case NetworkEditMode::NETWORK_PROHIBITION:
            myViewParent->getProhibitionFrame()->show();
            myViewParent->getProhibitionFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getProhibitionFrame();
            myNetworkCheckableButtons.prohibitionButton->setChecked(true);
            break;
        default:
            break;
    }
    // update common Network buttons
    myCommonCheckableButtons.updateCommonCheckableButtons();
    // Update Network buttons
    myNetworkCheckableButtons.updateNetworkCheckableButtons();
    // recalc toolbar
    myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->recalc();
    myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->repaint();
    // force repaint because different modes draw different things
    onPaint(nullptr, 0, nullptr);
    // finally update view
    updateViewNet();
}


void
GNEViewNet::updateDemandModeSpecificControls() {
    // get menu checks
    const auto& menuChecks = myViewParent->getGNEAppWindows()->getEditMenuCommands().demandViewOptions;
    // hide all checkbox of view options Network
    myNetworkViewOptions.hideNetworkViewOptionsMenuChecks();
    // hide all checkbox of view options Demand
    myDemandViewOptions.hideDemandViewOptionsMenuChecks();
    // hide all checkbox of view options Data
    myDataViewOptions.hideDataViewOptionsMenuChecks();
    // disable all common edit modes
    myCommonCheckableButtons.disableCommonCheckableButtons();
    // disable all Demand edit modes
    myDemandCheckableButtons.disableDemandCheckableButtons();
    // disable all network edit modes
    myDataCheckableButtons.disableDataCheckableButtons();
    // hide interval bar
    myIntervalBar.hideIntervalBar();
    // hide all frames
    myViewParent->hideAllFrames();
    // hide all menuchecks
    myViewParent->getGNEAppWindows()->getEditMenuCommands().networkViewOptions.hideNetworkViewOptionsMenuChecks();
    myViewParent->getGNEAppWindows()->getEditMenuCommands().demandViewOptions.hideDemandViewOptionsMenuChecks();
    myViewParent->getGNEAppWindows()->getEditMenuCommands().dataViewOptions.hideDataViewOptionsMenuChecks();
    // always show "hide shapes", "show grid", "draw spread vehicles" and show/lock persons and containers
    myDemandViewOptions.menuCheckToggleGrid->show();
    myDemandViewOptions.menuCheckDrawSpreadVehicles->show();
    myDemandViewOptions.menuCheckHideShapes->show();
    myDemandViewOptions.menuCheckShowAllTrips->show();
    myDemandViewOptions.menuCheckShowAllPersonPlans->show();
    myDemandViewOptions.menuCheckLockPerson->show();
    myDemandViewOptions.menuCheckShowAllContainerPlans->show();
    myDemandViewOptions.menuCheckLockContainer->show();
    menuChecks.menuCheckToggleGrid->show();
    menuChecks.menuCheckDrawSpreadVehicles->show();
    menuChecks.menuCheckHideShapes->show();
    menuChecks.menuCheckShowAllTrips->show();
    menuChecks.menuCheckShowAllPersonPlans->show();
    menuChecks.menuCheckLockPerson->show();
    menuChecks.menuCheckShowAllContainerPlans->show();
    menuChecks.menuCheckLockContainer->show();
    // show separator
    menuChecks.separator->show();
    // enable selected controls
    switch (myEditModes.demandEditMode) {
        // common modes
        case DemandEditMode::DEMAND_INSPECT:
            myViewParent->getInspectorFrame()->show();
            myViewParent->getInspectorFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getInspectorFrame();
            // set checkable button
            myCommonCheckableButtons.inspectButton->setChecked(true);
            // show view options
            myDemandViewOptions.menuCheckHideNonInspectedDemandElements->show();
            // show menu checks
            menuChecks.menuCheckHideNonInspectedDemandElements->show();
            break;
        case DemandEditMode::DEMAND_DELETE:
            myViewParent->getDeleteFrame()->show();
            myViewParent->getDeleteFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getDeleteFrame();
            // set checkable button
            myCommonCheckableButtons.deleteButton->setChecked(true);
            break;
        case DemandEditMode::DEMAND_SELECT:
            myViewParent->getSelectorFrame()->show();
            myViewParent->getSelectorFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getSelectorFrame();
            // set checkable button
            myCommonCheckableButtons.selectButton->setChecked(true);
            break;
        case DemandEditMode::DEMAND_MOVE:
            myViewParent->getMoveFrame()->show();
            myViewParent->getMoveFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getMoveFrame();
            // set checkable button
            myDemandCheckableButtons.moveDemandElementsButton->setChecked(true);
            break;
        // specific modes
        case DemandEditMode::DEMAND_ROUTE:
            myViewParent->getRouteFrame()->show();
            myViewParent->getRouteFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getRouteFrame();
            // set checkable button
            myDemandCheckableButtons.routeButton->setChecked(true);
            break;
        case DemandEditMode::DEMAND_VEHICLE:
            myViewParent->getVehicleFrame()->show();
            myViewParent->getVehicleFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getVehicleFrame();
            // set checkable button
            myDemandCheckableButtons.vehicleButton->setChecked(true);
            break;
        case DemandEditMode::DEMAND_VEHICLETYPES:
            myViewParent->getVehicleTypeFrame()->show();
            myViewParent->getVehicleTypeFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getVehicleTypeFrame();
            // set checkable button
            myDemandCheckableButtons.vehicleTypeButton->setChecked(true);
            break;
        case DemandEditMode::DEMAND_STOP:
            myViewParent->getStopFrame()->show();
            myViewParent->getStopFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getStopFrame();
            // set checkable button
            myDemandCheckableButtons.stopButton->setChecked(true);
            break;
        case DemandEditMode::DEMAND_PERSONTYPES:
            myViewParent->getPersonTypeFrame()->show();
            myViewParent->getPersonTypeFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getPersonTypeFrame();
            // set checkable button
            myDemandCheckableButtons.personTypeButton->setChecked(true);
            break;
        case DemandEditMode::DEMAND_PERSON:
            myViewParent->getPersonFrame()->show();
            myViewParent->getPersonFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getPersonFrame();
            // set checkable button
            myDemandCheckableButtons.personButton->setChecked(true);
            break;
        case DemandEditMode::DEMAND_PERSONPLAN:
            myViewParent->getPersonPlanFrame()->show();
            myViewParent->getPersonPlanFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getPersonPlanFrame();
            // set checkable button
            myDemandCheckableButtons.personPlanButton->setChecked(true);
            break;
        case DemandEditMode::DEMAND_CONTAINER:
            myViewParent->getContainerFrame()->show();
            myViewParent->getContainerFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getContainerFrame();
            // set checkable button
            myDemandCheckableButtons.containerButton->setChecked(true);
            break;
        case DemandEditMode::DEMAND_CONTAINERPLAN:
            myViewParent->getContainerPlanFrame()->show();
            myViewParent->getContainerPlanFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getContainerPlanFrame();
            // set checkable button
            myDemandCheckableButtons.containerPlanButton->setChecked(true);
            break;
        default:
            break;
    }
    // update common Network buttons
    myCommonCheckableButtons.updateCommonCheckableButtons();
    // Update Demand buttons
    myDemandCheckableButtons.updateDemandCheckableButtons();
    // recalc toolbar
    myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->recalc();
    myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->repaint();
    // force repaint because different modes draw different things
    onPaint(nullptr, 0, nullptr);
    // finally update view
    updateViewNet();
}


void
GNEViewNet::updateDataModeSpecificControls() {
    // get menu checks
    const auto& menuChecks = myViewParent->getGNEAppWindows()->getEditMenuCommands().dataViewOptions;
    // hide all checkbox of view options Network
    myNetworkViewOptions.hideNetworkViewOptionsMenuChecks();
    // hide all checkbox of view options Demand
    myDemandViewOptions.hideDemandViewOptionsMenuChecks();
    // hide all checkbox of view options Data
    myDataViewOptions.hideDataViewOptionsMenuChecks();
    // disable all common edit modes
    myCommonCheckableButtons.disableCommonCheckableButtons();
    // disable all Data edit modes
    myDataCheckableButtons.disableDataCheckableButtons();
    // show interval bar
    myIntervalBar.showIntervalBar();
    // hide all frames
    myViewParent->hideAllFrames();
    // hide all menuchecks
    myViewParent->getGNEAppWindows()->getEditMenuCommands().networkViewOptions.hideNetworkViewOptionsMenuChecks();
    myViewParent->getGNEAppWindows()->getEditMenuCommands().demandViewOptions.hideDemandViewOptionsMenuChecks();
    myViewParent->getGNEAppWindows()->getEditMenuCommands().dataViewOptions.hideDataViewOptionsMenuChecks();
    // In data mode, always show option "show demand elements" and "hide shapes"
    myDataViewOptions.menuCheckShowAdditionals->show();
    myDataViewOptions.menuCheckShowShapes->show();
    myDataViewOptions.menuCheckShowDemandElements->show();
    menuChecks.menuCheckShowAdditionals->show();
    menuChecks.menuCheckShowShapes->show();
    menuChecks.menuCheckShowDemandElements->show();
    // show separator
    menuChecks.separator->show();
    // enable selected controls
    switch (myEditModes.dataEditMode) {
        // common modes
        case DataEditMode::DATA_INSPECT:
            myViewParent->getInspectorFrame()->show();
            myViewParent->getInspectorFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getInspectorFrame();
            // set checkable button
            myCommonCheckableButtons.inspectButton->setChecked(true);
            // enable IntervalBar
            myIntervalBar.enableIntervalBar();
            break;
        case DataEditMode::DATA_DELETE:
            myViewParent->getDeleteFrame()->show();
            myViewParent->getDeleteFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getDeleteFrame();
            // set checkable button
            myCommonCheckableButtons.deleteButton->setChecked(true);
            // enable IntervalBar
            myIntervalBar.enableIntervalBar();
            break;
        case DataEditMode::DATA_SELECT:
            myViewParent->getSelectorFrame()->show();
            myViewParent->getSelectorFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getSelectorFrame();
            // set checkable button
            myCommonCheckableButtons.selectButton->setChecked(true);
            // enable IntervalBar
            myIntervalBar.enableIntervalBar();
            break;
        case DataEditMode::DATA_EDGEDATA:
            myViewParent->getEdgeDataFrame()->show();
            myViewParent->getEdgeDataFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getEdgeDataFrame();
            // set checkable button
            myDataCheckableButtons.edgeDataButton->setChecked(true);
            // disable IntervalBar
            myIntervalBar.disableIntervalBar();
            break;
        case DataEditMode::DATA_EDGERELDATA:
            myViewParent->getEdgeRelDataFrame()->show();
            myViewParent->getEdgeRelDataFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getEdgeRelDataFrame();
            // set checkable button
            myDataCheckableButtons.edgeRelDataButton->setChecked(true);
            // disable IntervalBar
            myIntervalBar.disableIntervalBar();
            break;
        case DataEditMode::DATA_TAZRELDATA:
            myViewParent->getTAZRelDataFrame()->show();
            myViewParent->getTAZRelDataFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getTAZRelDataFrame();
            // set checkable button
            myDataCheckableButtons.TAZRelDataButton->setChecked(true);
            // disable IntervalBar
            myIntervalBar.disableIntervalBar();
            break;
        default:
            break;
    }
    // update common Network buttons
    myCommonCheckableButtons.updateCommonCheckableButtons();
    // Update Data buttons
    myDataCheckableButtons.updateDataCheckableButtons();
    // recalc toolbar
    myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->recalc();
    myViewParent->getGNEAppWindows()->getToolbarsGrip().modes->repaint();
    // force repaint because different modes draw different things
    onPaint(nullptr, 0, nullptr);
    // finally update view
    updateViewNet();
}

void
GNEViewNet::deleteSelectedJunctions() {
    std::vector<GNEJunction*> junctions = myNet->retrieveJunctions(true);
    if (junctions.size() > 0) {
        std::string plural = junctions.size() == 1 ? ("") : ("s");
        myUndoList->p_begin("delete selected " + toString(SUMO_TAG_JUNCTION) + plural);
        for (auto i : junctions) {
            myNet->deleteJunction(i, myUndoList);
        }
        myUndoList->p_end();
    }
}


void
GNEViewNet::deleteSelectedLanes() {
    std::vector<GNELane*> lanes = myNet->retrieveLanes(true);
    if (lanes.size() > 0) {
        std::string plural = lanes.size() == 1 ? ("") : ("s");
        myUndoList->p_begin("delete selected " + toString(SUMO_TAG_LANE) + plural);
        for (auto i : lanes) {
            // when deleting multiple lanes, recompute connections
            myNet->deleteLane(i, myUndoList, true);
        }
        myUndoList->p_end();
    }
}


void
GNEViewNet::deleteSelectedEdges() {
    std::vector<GNEEdge*> edges = myNet->retrieveEdges(true);
    if (edges.size() > 0) {
        std::string plural = edges.size() == 1 ? ("") : ("s");
        myUndoList->p_begin("delete selected " + toString(SUMO_TAG_EDGE) + plural);
        for (auto i : edges) {
            // when deleting multiple edges, recompute connections
            myNet->deleteEdge(i, myUndoList, true);
        }
        myUndoList->p_end();
    }
}


void
GNEViewNet::deleteSelectedAdditionals() {
    const std::vector<GNEAdditional*> selectedAdditionals = myNet->retrieveAdditionals(true);
    if (selectedAdditionals.size() > 0) {
        std::string plural = selectedAdditionals.size() == 1 ? ("") : ("s");
        myUndoList->p_begin("delete selected additional" + plural);
        // do it in two phases: In the first phase remove slaves...
        for (const auto& selectedAdditional : selectedAdditionals) {
            if (selectedAdditional->getTagProperty().isSlave()) {
                myNet->deleteAdditional(selectedAdditional, myUndoList);
            }
        }
        // ... and in the second remove rest
        for (const auto& selectedAdditional : selectedAdditionals) {
            if (!selectedAdditional->getTagProperty().isSlave()) {
                myNet->deleteAdditional(selectedAdditional, myUndoList);
            }
        }
        myUndoList->p_end();
    }
}


void
GNEViewNet::deleteSelectedDemandElements() {
    std::vector<GNEDemandElement*> demandElements = myNet->retrieveDemandElements(true);
    if (demandElements.size() > 0) {
        std::string plural = demandElements.size() == 1 ? ("") : ("s");
        myUndoList->p_begin("delete selected demand elements" + plural);
        // declare vector for persons
        std::vector<GNEDemandElement*> persons;
        for (const auto& demandElement : demandElements) {
            // check if element is a personplan
            if (demandElement->getTagProperty().isPersonPlan()) {
                persons.push_back(demandElement->getParentDemandElements().front());
            }
            // due there are demand elements that are removed when their parent is removed, we need to check if yet exists before removing
            if (myNet->retrieveDemandElement(demandElement->getTagProperty().getTag(), demandElement->getID(), false) != nullptr) {
                myNet->deleteDemandElement(demandElement, myUndoList);
            }
        }
        // check if we have to remove empty persons
        for (const auto& person : persons) {
            // due person could be removed previously, check if exist
            if (person->getChildDemandElements().empty() && myNet->retrieveDemandElement(person->getTagProperty().getTag(), person->getID(), false) != nullptr) {
                myNet->deleteDemandElement(person, myUndoList);
            }
        }
        myUndoList->p_end();
    }
}


void
GNEViewNet::deleteSelectedGenericDatas() {
    std::vector<GNEGenericData*> genericDatas = myNet->retrieveGenericDatas(true);
    if (genericDatas.size() > 0) {
        std::string plural = genericDatas.size() == 1 ? ("") : ("s");
        myUndoList->p_begin("delete selected generic data" + plural);
        // iterate over generic datas
        for (const auto& genericData : genericDatas) {
            myNet->deleteGenericData(genericData, myUndoList);
        }
        myUndoList->p_end();
    }
}


void
GNEViewNet::deleteSelectedCrossings() {
    // obtain selected crossings
    std::vector<GNEJunction*> junctions = myNet->retrieveJunctions();
    std::vector<GNECrossing*> crossings;
    for (auto i : junctions) {
        for (auto j : i->getGNECrossings()) {
            if (j->isAttributeCarrierSelected()) {
                crossings.push_back(j);
            }
        }
    }
    // remove selected crossings
    if (crossings.size() > 0) {
        std::string plural = crossings.size() == 1 ? ("") : ("s");
        myUndoList->p_begin("delete selected " + toString(SUMO_TAG_CROSSING) + "s");
        for (auto i : crossings) {
            if (myNet->retrieveCrossing(i->getID(), false)) {
                myNet->deleteCrossing(i, myUndoList);
            }
        }
        myUndoList->p_end();
    }
}


void
GNEViewNet::deleteSelectedConnections() {
    // obtain selected connections
    std::vector<GNEEdge*> edges = myNet->retrieveEdges();
    std::vector<GNEConnection*> connections;
    for (auto i : edges) {
        for (auto j : i->getGNEConnections()) {
            if (j->isAttributeCarrierSelected()) {
                connections.push_back(j);
            }
        }
    }
    // remove selected connections
    if (connections.size() > 0) {
        std::string plural = connections.size() == 1 ? ("") : ("s");
        myUndoList->p_begin("delete selected " + toString(SUMO_TAG_CONNECTION) + plural);
        for (auto i : connections) {
            myNet->deleteConnection(i, myUndoList);
        }
        myUndoList->p_end();
    }
}


void
GNEViewNet::deleteSelectedShapes() {
    // obtain selected shapes
    std::vector<GNEShape*> selectedShapes = myNet->retrieveShapes(true);
    // remove it
    if (selectedShapes.size() > 0) {
        std::string plural = selectedShapes.size() == 1 ? ("") : ("s");
        myUndoList->p_begin("delete selected shape" + plural);
        for (auto i : selectedShapes) {
            myNet->deleteShape(i, myUndoList);
        }
        myUndoList->p_end();
    }
}


void
GNEViewNet::deleteSelectedTAZElements() {
    // obtain selected TAZ Elements
    std::vector<GNETAZElement*> selectedTAZElements = myNet->retrieveTAZElements(true);
    // remove it
    if (selectedTAZElements.size() > 0) {
        std::string plural = selectedTAZElements.size() == 1 ? ("") : ("s");
        myUndoList->p_begin("delete selected TAZ" + plural);
        for (auto i : selectedTAZElements) {
            myNet->deleteTAZElement(i, myUndoList);
        }
        myUndoList->p_end();
    }
}


void
GNEViewNet::updateControls() {
    switch (myEditModes.networkEditMode) {
        case NetworkEditMode::NETWORK_INSPECT:
            myViewParent->getInspectorFrame()->update();
            break;
        default:
            break;
    }
    // update view
    updateViewNet();
}

// ---------------------------------------------------------------------------
// Private methods
// ---------------------------------------------------------------------------

void
GNEViewNet::drawLaneCandidates() const {
    if (myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->isSelectingLanes()) {
        // draw first point
        if (myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLanes().size() > 0) {
            // Push draw matrix
            GLHelper::pushMatrix();
            // obtain first clicked point
            const Position& firstLanePoint = myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLanes().front().first->getLaneShape().positionAtOffset(
                                                 myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLanes().front().second);
            // must draw on top of other connections
            glTranslated(firstLanePoint.x(), firstLanePoint.y(), GLO_JUNCTION + 0.3);
            GLHelper::setColor(RGBColor::RED);
            // draw first point
            GLHelper::drawFilledCircle((double) 1.3, myVisualizationSettings->getCircleResolution());
            GLHelper::drawText("S", Position(), .1, 1.3, RGBColor::CYAN);
            // pop draw matrix
            GLHelper::popMatrix();
        }
        // draw connections between lanes
        if (myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLanes().size() > 1) {
            // iterate over all current selected lanes
            for (int i = 0; i < (int)myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLanes().size() - 1; i++) {
                // declare position vector for shape
                PositionVector shape;
                // declare vectors for shape rotation and lengths
                std::vector<double> shapeRotations, shapeLengths;
                // obtain GNELanes
                GNELane* from = myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLanes().at(i).first;
                GNELane* to = myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLanes().at(i + 1).first;
                // Push draw matrix
                GLHelper::pushMatrix();
                // must draw on top of other connections
                glTranslated(0, 0, GLO_JUNCTION + 0.2);
                // obtain connection shape
                shape = from->getParentEdge()->getNBEdge()->getConnection(from->getIndex(), to->getParentEdge()->getNBEdge(), to->getIndex()).shape;
                // set special color
                GLHelper::setColor(myVisualizationSettings->candidateColorSettings.possible);
                // Obtain lengths and shape rotations
                int segments = (int) shape.size() - 1;
                if (segments >= 0) {
                    shapeRotations.reserve(segments);
                    shapeLengths.reserve(segments);
                    for (int j = 0; j < segments; j++) {
                        shapeLengths.push_back(GNEGeometry::calculateLength(shape[j], shape[j + 1]));
                        shapeRotations.push_back(GNEGeometry::calculateRotation(shape[j], shape[j + 1]));
                    }
                }
                // draw a list of lines
                GLHelper::drawBoxLines(shape, shapeRotations, shapeLengths, 0.2);
                // pop draw matrix
                GLHelper::popMatrix();
            }
            // draw last point
            GLHelper::pushMatrix();
            // obtain last clicked point
            const Position& lastLanePoint = myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLanes().back().first->getLaneShape().positionAtOffset(
                                                myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLanes().back().second);
            // must draw on top of other connections
            glTranslated(lastLanePoint.x(), lastLanePoint.y(), GLO_JUNCTION + 0.3);
            GLHelper::setColor(RGBColor::RED);
            // draw last point
            GLHelper::drawFilledCircle((double) 1.3, 8);
            GLHelper::drawText("E", Position(), .1, 1.3, RGBColor::CYAN);
            // pop draw matrix
            GLHelper::popMatrix();
        }

    }
}


void
GNEViewNet::drawTemporalDrawShape() const {
    PositionVector temporalShape;
    bool deleteLastCreatedPoint = false;
    // obtain temporal shape and delete last created point flag
    if (myViewParent->getPolygonFrame()->getDrawingShapeModul()->isDrawing()) {
        temporalShape = myViewParent->getPolygonFrame()->getDrawingShapeModul()->getTemporalShape();
        deleteLastCreatedPoint = myViewParent->getPolygonFrame()->getDrawingShapeModul()->getDeleteLastCreatedPoint();
    } else if (myViewParent->getTAZFrame()->getDrawingShapeModul()->isDrawing()) {
        temporalShape = myViewParent->getTAZFrame()->getDrawingShapeModul()->getTemporalShape();
        deleteLastCreatedPoint = myViewParent->getTAZFrame()->getDrawingShapeModul()->getDeleteLastCreatedPoint();
    }
    // check if we're in drawing mode
    if (temporalShape.size() > 0) {
        // draw blue line with the current drawed shape
        GLHelper::pushMatrix();
        glLineWidth(2);
        glTranslated(0, 0, GLO_TEMPORALSHAPE);
        GLHelper::setColor(RGBColor::BLUE);
        GLHelper::drawLine(temporalShape);
        GLHelper::popMatrix();
        // draw red line from the last point of shape to the current mouse position
        GLHelper::pushMatrix();
        glLineWidth(2);
        glTranslated(0, 0, GLO_TEMPORALSHAPE);
        // draw last line depending if shift key (delete last created point) is pressed
        if (deleteLastCreatedPoint) {
            GLHelper::setColor(RGBColor::RED);
        } else {
            GLHelper::setColor(RGBColor::GREEN);
        }
        GLHelper::drawLine(temporalShape.back(), snapToActiveGrid(getPositionInformation()));
        GLHelper::popMatrix();
    }
}


void
GNEViewNet::drawTemporalJunction() const {
    // first check if we're in correct mode
    if (myEditModes.isCurrentSupermodeNetwork() && (myEditModes.networkEditMode == NetworkEditMode::NETWORK_CREATE_EDGE) &&
            !myMouseButtonKeyPressed.controlKeyPressed() &&
            !myMouseButtonKeyPressed.shiftKeyPressed() &&
            !myMouseButtonKeyPressed.altKeyPressed()) {
        // get mouse position
        const Position mousePosition = snapToActiveGrid(getPositionInformation());
        // get junction exaggeration
        const double junctionExaggeration = myVisualizationSettings->junctionSize.getExaggeration(myVisualizationSettings, nullptr, 4);
        // get buble color
        RGBColor bubbleColor = myVisualizationSettings->junctionColorer.getScheme().getColor(1);
        // change alpha
        bubbleColor.setAlpha(200);
        // push layer matrix
        GLHelper::pushMatrix();
        // translate to temporal shape layer
        glTranslated(0, 0, GLO_TEMPORALSHAPE);
        // push junction matrix
        GLHelper::pushMatrix();
        // move matrix junction center
        glTranslated(mousePosition.x(), mousePosition.y(), 0.1);
        // set color
        GLHelper::setColor(bubbleColor);
        // draw filled circle
        GLHelper::drawFilledCircle(myVisualizationSettings->neteditSizeSettings.junctionBubbleRadius * junctionExaggeration, myVisualizationSettings->getCircleResolution());
        // pop junction matrix
        GLHelper::popMatrix();
        // draw temporal edge
        if (myViewParent->getCreateEdgeFrame()->getJunctionSource() && (mousePosition.distanceSquaredTo(myViewParent->getCreateEdgeFrame()->getJunctionSource()->getPositionInView()) > 1)) {
            // set temporal edge color
            RGBColor temporalEdgeColor = RGBColor::BLACK;
            temporalEdgeColor.setAlpha(200);
            // declare temporal edge geometry
            GNEGeometry::Geometry temporalEdgeGeometery;
            // calculate geometry between source junction and mouse position
            PositionVector temporalEdge = {mousePosition, myViewParent->getCreateEdgeFrame()->getJunctionSource()->getPositionInView()};
            // move temporal edge 2 side
            temporalEdge.move2side(-1);
            // update geometry
            temporalEdgeGeometery.updateGeometry(temporalEdge);
            // push temporal edge matrix
            GLHelper::pushMatrix();
            // set color
            GLHelper::setColor(temporalEdgeColor);
            // draw temporal edge
            GNEGeometry::drawGeometry(this, temporalEdgeGeometery, 0.75);
            // check if we have to draw opposite edge
            if (myNetworkViewOptions.menuCheckAutoOppositeEdge->amChecked() == TRUE) {
                // move temporal edge to opposite edge
                temporalEdge.move2side(2);
                // update geometry
                temporalEdgeGeometery.updateGeometry(temporalEdge);
                // draw temporal edge
                GNEGeometry::drawGeometry(this, temporalEdgeGeometery, 0.75);
            }
            // pop temporal edge matrix
            GLHelper::popMatrix();
        }
        // pop layer matrix
        GLHelper::popMatrix();
    }
}


void
GNEViewNet::processLeftButtonPressNetwork(void* eventData) {
    // decide what to do based on mode
    switch (myEditModes.networkEditMode) {
        case NetworkEditMode::NETWORK_INSPECT: {
            // first swap lane to edges if mySelectEdges is enabled and shift key isn't pressed
            if (myNetworkViewOptions.selectEdges() && (myMouseButtonKeyPressed.shiftKeyPressed() == false)) {
                myObjectsUnderCursor.swapLane2Edge();
            }
            // process left click in Inspector Frame
            myViewParent->getInspectorFrame()->processNetworkSupermodeClick(getPositionInformation(), myObjectsUnderCursor);
            // process click
            processClick(eventData);
            break;
        }
        case NetworkEditMode::NETWORK_DELETE: {
            // first swap lane to edges if mySelectEdges is enabled and shift key isn't pressed
            if (myNetworkViewOptions.selectEdges() && (myMouseButtonKeyPressed.shiftKeyPressed() == false)) {
                myObjectsUnderCursor.swapLane2Edge();
            }
            // check that we have clicked over network element element
            if (myObjectsUnderCursor.getAttributeCarrierFront() &&
                    (myObjectsUnderCursor.getAttributeCarrierFront()->getTagProperty().isNetworkElement() ||
                     myObjectsUnderCursor.getAttributeCarrierFront()->getTagProperty().isAdditionalElement() ||
                     myObjectsUnderCursor.getAttributeCarrierFront()->getTagProperty().isShape() ||
                     myObjectsUnderCursor.getAttributeCarrierFront()->getTagProperty().isTAZElement())) {
                // now check if we want only delete geometry points
                if (myViewParent->getDeleteFrame()->getDeleteOptions()->deleteOnlyGeometryPoints()) {
                    // only remove geometry point
                    myViewParent->getDeleteFrame()->removeGeometryPoint(myObjectsUnderCursor);
                } else if (myObjectsUnderCursor.getAttributeCarrierFront()->isAttributeCarrierSelected()) {
                    // remove all selected attribute carriers
                    myViewParent->getDeleteFrame()->removeSelectedAttributeCarriers();
                } else {
                    // remove attribute carrier under cursor
                    myViewParent->getDeleteFrame()->removeAttributeCarrier(myObjectsUnderCursor);
                }
            } else {
                // process click
                processClick(eventData);
            }
            break;
        }
        case NetworkEditMode::NETWORK_SELECT:
            // first swap lane to edges if mySelectEdges is enabled and shift key isn't pressed
            if (myNetworkViewOptions.selectEdges() && (myMouseButtonKeyPressed.shiftKeyPressed() == false)) {
                myObjectsUnderCursor.swapLane2Edge();
            }
            // avoid to select if control key is pressed
            if (!myMouseButtonKeyPressed.controlKeyPressed()) {
                // check if a rect for selecting is being created
                if (myMouseButtonKeyPressed.shiftKeyPressed()) {
                    // begin rectangle selection
                    mySelectingArea.beginRectangleSelection();
                } else {
                    // first check that under cursor there is an attribute carrier, isn't a demand element and is selectable
                    if (myObjectsUnderCursor.getAttributeCarrierFront() && !myObjectsUnderCursor.getAttributeCarrierFront()->getTagProperty().isDemandElement()) {
                        // Check if this GLobject type is locked
                        if (!myViewParent->getSelectorFrame()->getLockGLObjectTypes()->IsObjectTypeLocked(myObjectsUnderCursor.getGlTypeFront())) {
                            // toggle networkElement selection
                            if (myObjectsUnderCursor.getAttributeCarrierFront()->isAttributeCarrierSelected()) {
                                myObjectsUnderCursor.getAttributeCarrierFront()->unselectAttributeCarrier();
                            } else {
                                myObjectsUnderCursor.getAttributeCarrierFront()->selectAttributeCarrier();
                            }
                        }
                    }
                    // process click
                    processClick(eventData);
                }
            } else {
                // process click
                processClick(eventData);
            }
            break;
        case NetworkEditMode::NETWORK_CREATE_EDGE: {
            // check what buttons are pressed
            if (myMouseButtonKeyPressed.shiftKeyPressed()) {
                // get edge under cursor
                GNEEdge* edge = myObjectsUnderCursor.getEdgeFront();
                if (edge) {
                    // obtain reverse edge
                    GNEEdge* reverseEdge = edge->getOppositeEdge();
                    // check if we're split one or both edges
                    if (myMouseButtonKeyPressed.altKeyPressed()) {
                        myNet->splitEdge(edge, edge->getSplitPos(getPositionInformation()), myUndoList);
                    } else if (reverseEdge) {
                        myNet->splitEdgesBidi(edge, reverseEdge, edge->getSplitPos(getPositionInformation()), myUndoList);
                    } else {
                        myNet->splitEdge(edge, edge->getSplitPos(getPositionInformation()), myUndoList);
                    }
                }
            } else if (!myMouseButtonKeyPressed.controlKeyPressed()) {
                // check if we have to update objects under snapped cursor
                if (myVisualizationSettings->showGrid) {
                    myViewParent->getCreateEdgeFrame()->updateObjectsUnderSnappedCursor(getGUIGlObjectsUnderSnappedCursor());
                } else {
                    myViewParent->getCreateEdgeFrame()->updateObjectsUnderSnappedCursor({});
                }
                // process left click in create edge frame Frame
                myViewParent->getCreateEdgeFrame()->processClick(getPositionInformation(),
                        myObjectsUnderCursor,
                        (myNetworkViewOptions.menuCheckAutoOppositeEdge->amChecked() == TRUE),
                        (myNetworkViewOptions.menuCheckChainEdges->amChecked() == TRUE));
            }
            // process click
            processClick(eventData);
            break;
        }
        case NetworkEditMode::NETWORK_MOVE: {
            // first swap lane to edges if mySelectEdges is enabled and shift key isn't pressed
            if (myNetworkViewOptions.selectEdges() && (myMouseButtonKeyPressed.shiftKeyPressed() == false)) {
                myObjectsUnderCursor.swapLane2Edge();
            }
            // check if we're editing a shape
            if (myEditNetworkElementShapes.getEditedNetworkElement()) {
                // check if we're removing a geometry point
                if (myMouseButtonKeyPressed.shiftKeyPressed()) {
                    // remove geometry point
                    if (myObjectsUnderCursor.getNetworkElementFront() == myEditNetworkElementShapes.getEditedNetworkElement()) {
                        myObjectsUnderCursor.getNetworkElementFront()->removeGeometryPoint(getPositionInformation(), myUndoList);
                    }
                } else if (!myMoveSingleElementValues.beginMoveNetworkElementShape()) {
                    // process click  if there isn't movable elements (to move camera using drag an drop)
                    processClick(eventData);
                }
            } else {
                // allways swap lane to edges in movement mode
                myObjectsUnderCursor.swapLane2Edge();
                // check that AC under cursor isn't a demand element
                if (myObjectsUnderCursor.getAttributeCarrierFront() && !myObjectsUnderCursor.getAttributeCarrierFront()->getTagProperty().isDemandElement()) {
                    // check if we're moving a set of selected items
                    if (myObjectsUnderCursor.getAttributeCarrierFront()->isAttributeCarrierSelected()) {
                        // move selected ACs
                        myMoveMultipleElementValues.beginMoveSelection();
                        // update view
                        updateViewNet();
                    } else if (!myMoveSingleElementValues.beginMoveSingleElementNetworkMode()) {
                        // process click  if there isn't movable elements (to move camera using drag an drop)
                        processClick(eventData);
                    }
                } else {
                    // process click  if there isn't movable elements (to move camera using drag an drop)
                    processClick(eventData);
                }
            }
            break;
        }
        case NetworkEditMode::NETWORK_CONNECT: {
            // check if we're clicked over a lane
            if (myObjectsUnderCursor.getLaneFront()) {
                // Handle laneclick (shift key may pass connections, Control key allow conflicts)
                myViewParent->getConnectorFrame()->handleLaneClick(myObjectsUnderCursor);
                updateViewNet();
            }
            // process click
            processClick(eventData);
            break;
        }
        case NetworkEditMode::NETWORK_TLS: {
            if (myObjectsUnderCursor.getJunctionFront()) {
                // edit TLS in TLSEditor frame
                myViewParent->getTLSEditorFrame()->editTLS(getPositionInformation(), myObjectsUnderCursor);
                updateViewNet();
            }
            // process click
            processClick(eventData);
            break;
        }
        case NetworkEditMode::NETWORK_ADDITIONAL: {
            // avoid create additionals if control key is pressed
            if (!myMouseButtonKeyPressed.controlKeyPressed()) {
                if (myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->isShown()) {
                    // check if we need to start select lanes
                    if (myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->isSelectingLanes()) {
                        // select getLaneFront() to create an additional with consecutive lanes
                        myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->addSelectedLane(myObjectsUnderCursor.getLaneFront(), snapToActiveGrid(getPositionInformation()));
                    } else if (myObjectsUnderCursor.getLaneFront()) {
                        myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->startConsecutiveLaneSelector(myObjectsUnderCursor.getLaneFront(), snapToActiveGrid(getPositionInformation()));
                    }
                } else if (myViewParent->getAdditionalFrame()->addAdditional(myObjectsUnderCursor)) {
                    // update view to show the new additional
                    updateViewNet();
                }
            }
            // process click
            processClick(eventData);
            break;
        }
        case NetworkEditMode::NETWORK_CROSSING: {
            // swap lanes to edges in crossingsMode
            myObjectsUnderCursor.swapLane2Edge();
            // call function addCrossing from crossing frame
            myViewParent->getCrossingFrame()->addCrossing(myObjectsUnderCursor);
            // process click
            processClick(eventData);
            break;
        }
        case NetworkEditMode::NETWORK_TAZ: {
            // swap lanes to edges in TAZ Mode
            myObjectsUnderCursor.swapLane2Edge();
            // avoid create TAZs if control key is pressed
            if (!myMouseButtonKeyPressed.controlKeyPressed()) {
                // check if we want to create a rect for selecting edges
                if (myMouseButtonKeyPressed.shiftKeyPressed() && (myViewParent->getTAZFrame()->getTAZCurrentModul()->getTAZ() != nullptr)) {
                    // begin rectangle selection
                    mySelectingArea.beginRectangleSelection();
                } else {
                    // check if process click was scuesfully
                    if (myViewParent->getTAZFrame()->processClick(snapToActiveGrid(getPositionInformation()), myObjectsUnderCursor)) {
                        // view net must be always update
                        updateViewNet();
                    }
                    // process click
                    processClick(eventData);
                }
            } else {
                // process click
                processClick(eventData);
            }
            break;
        }
        case NetworkEditMode::NETWORK_POLYGON: {
            // avoid create shapes if control key is pressed
            if (!myMouseButtonKeyPressed.controlKeyPressed()) {
                if (!myObjectsUnderCursor.getPOIFront()) {
                    // declare processClick flag
                    bool updateTemporalShape = false;
                    // process click
                    myViewParent->getPolygonFrame()->processClick(snapToActiveGrid(getPositionInformation()), myObjectsUnderCursor, updateTemporalShape);
                    // view net must be always update
                    updateViewNet();
                    // process click depending of the result of "process click"
                    if (!updateTemporalShape) {
                        // process click
                        processClick(eventData);
                    }
                }
            } else {
                // process click
                processClick(eventData);
            }
            break;
        }
        case NetworkEditMode::NETWORK_PROHIBITION: {
            if (myObjectsUnderCursor.getConnectionFront()) {
                // shift key may pass connections, Control key allow conflicts.
                myViewParent->getProhibitionFrame()->handleProhibitionClick(myObjectsUnderCursor);
                updateViewNet();
            }
            // process click
            processClick(eventData);
            break;
        }
        default: {
            // process click
            processClick(eventData);
        }
    }
}


void
GNEViewNet::processLeftButtonReleaseNetwork() {
    // check moved items
    if (myMoveMultipleElementValues.isMovingSelection()) {
        myMoveMultipleElementValues.finishMoveSelection();
    } else if (mySelectingArea.selectingUsingRectangle) {
        // check if we're creating a rectangle selection or we want only to select a lane
        if (mySelectingArea.startDrawing) {
            // check if we're selecting all type of elements o we only want a set of edges for TAZ
            if (myEditModes.networkEditMode == NetworkEditMode::NETWORK_SELECT) {
                mySelectingArea.processRectangleSelection();
            } else if (myEditModes.networkEditMode == NetworkEditMode::NETWORK_TAZ) {
                // process edge selection
                myViewParent->getTAZFrame()->processEdgeSelection(mySelectingArea.processEdgeRectangleSelection());
            }
        } else if (myMouseButtonKeyPressed.shiftKeyPressed()) {
            // obtain objects under cursor
            if (makeCurrent()) {
                // update objects under cursor again
                myObjectsUnderCursor.updateObjectUnderCursor(getGUIGlObjectsUnderCursor());
                makeNonCurrent();
            }
            // check if there is a lane in objects under cursor
            if (myObjectsUnderCursor.getLaneFront()) {
                // if we clicked over an lane with shift key pressed, select or unselect it
                if (myObjectsUnderCursor.getLaneFront()->isAttributeCarrierSelected()) {
                    myObjectsUnderCursor.getLaneFront()->unselectAttributeCarrier();
                } else {
                    myObjectsUnderCursor.getLaneFront()->selectAttributeCarrier();
                }
            }
        }
        // finish selection
        mySelectingArea.finishRectangleSelection();
    } else {
        // finish moving of single elements
        myMoveSingleElementValues.finishMoveSingleElement();
    }
}


void
GNEViewNet::processMoveMouseNetwork(const bool mouseLeftButtonPressed) {
    // change "delete last created point" depending if during movement shift key is pressed
    if ((myEditModes.networkEditMode == NetworkEditMode::NETWORK_POLYGON) && myViewParent->getPolygonFrame()->getDrawingShapeModul()->isDrawing()) {
        myViewParent->getPolygonFrame()->getDrawingShapeModul()->setDeleteLastCreatedPoint(myMouseButtonKeyPressed.shiftKeyPressed());
    } else if ((myEditModes.networkEditMode == NetworkEditMode::NETWORK_TAZ) && myViewParent->getTAZFrame()->getDrawingShapeModul()->isDrawing()) {
        myViewParent->getTAZFrame()->getDrawingShapeModul()->setDeleteLastCreatedPoint(myMouseButtonKeyPressed.shiftKeyPressed());
    }
    // check what type of additional is moved
    if (myMoveMultipleElementValues.isMovingSelection()) {
        // move entire selection
        myMoveMultipleElementValues.moveSelection(mouseLeftButtonPressed);
    } else if (mySelectingArea.selectingUsingRectangle) {
        // update selection corner of selecting area
        mySelectingArea.moveRectangleSelection();
    } else {
        // move single elements
        myMoveSingleElementValues.moveSingleElement(mouseLeftButtonPressed);
    }
}


void
GNEViewNet::processLeftButtonPressDemand(void* eventData) {
    // decide what to do based on mode
    switch (myEditModes.demandEditMode) {
        case DemandEditMode::DEMAND_INSPECT: {
            // process left click in Inspector Frame
            myViewParent->getInspectorFrame()->processDemandSupermodeClick(getPositionInformation(), myObjectsUnderCursor);
            // process click
            processClick(eventData);
            break;
        }
        case DemandEditMode::DEMAND_DELETE: {
            // check that we have clicked over an demand element
            if (myObjectsUnderCursor.getAttributeCarrierFront() && myObjectsUnderCursor.getAttributeCarrierFront()->getTagProperty().isDemandElement()) {
                // check if we are deleting a selection or an single attribute carrier
                if (myObjectsUnderCursor.getAttributeCarrierFront()->isAttributeCarrierSelected()) {
                    myViewParent->getDeleteFrame()->removeSelectedAttributeCarriers();
                } else {
                    myViewParent->getDeleteFrame()->removeAttributeCarrier(myObjectsUnderCursor);
                }
            } else {
                // process click
                processClick(eventData);
            }
            break;
        }
        case DemandEditMode::DEMAND_SELECT:
            // avoid to select if control key is pressed
            if (!myMouseButtonKeyPressed.controlKeyPressed()) {
                // check if a rect for selecting is being created
                if (myMouseButtonKeyPressed.shiftKeyPressed()) {
                    // begin rectangle selection
                    mySelectingArea.beginRectangleSelection();
                } else {
                    // first check that under cursor there is an attribute carrier, is demand element and is selectable
                    if (myObjectsUnderCursor.getAttributeCarrierFront() && myObjectsUnderCursor.getAttributeCarrierFront()->getTagProperty().isDemandElement()) {
                        // Check if this GLobject type is locked
                        if (!myViewParent->getSelectorFrame()->getLockGLObjectTypes()->IsObjectTypeLocked(myObjectsUnderCursor.getGlTypeFront())) {
                            // toggle networkElement selection
                            if (myObjectsUnderCursor.getAttributeCarrierFront()->isAttributeCarrierSelected()) {
                                myObjectsUnderCursor.getAttributeCarrierFront()->unselectAttributeCarrier();
                            } else {
                                myObjectsUnderCursor.getAttributeCarrierFront()->selectAttributeCarrier();
                            }
                        }
                    }
                    // process click
                    processClick(eventData);
                }
            } else {
                // process click
                processClick(eventData);
            }
            break;
        case DemandEditMode::DEMAND_MOVE: {
            // check that AC under cursor is a demand element
            if (myObjectsUnderCursor.getAttributeCarrierFront() &&
                    myObjectsUnderCursor.getAttributeCarrierFront()->getTagProperty().isDemandElement()) {
                // check if we're moving a set of selected items
                if (myObjectsUnderCursor.getAttributeCarrierFront()->isAttributeCarrierSelected()) {
                    // move selected ACs
                    myMoveMultipleElementValues.beginMoveSelection();
                    // update view
                    updateViewNet();
                } else if (!myMoveSingleElementValues.beginMoveSingleElementDemandMode()) {
                    // process click  if there isn't movable elements (to move camera using drag an drop)
                    processClick(eventData);
                }
            } else {
                // process click  if there isn't movable elements (to move camera using drag an drop)
                processClick(eventData);
            }
            break;
        }
        case DemandEditMode::DEMAND_ROUTE: {
            // check if we clicked over a lane
            if (myObjectsUnderCursor.getLaneFront()) {
                // Handle edge click
                myViewParent->getRouteFrame()->addEdgeRoute(myObjectsUnderCursor.getLaneFront()->getParentEdge(), myMouseButtonKeyPressed);
            }
            // process click
            processClick(eventData);
            break;
        }
        case DemandEditMode::DEMAND_VEHICLE: {
            // Handle click
            myViewParent->getVehicleFrame()->addVehicle(myObjectsUnderCursor, myMouseButtonKeyPressed);
            // process click
            processClick(eventData);
            break;
        }
        case DemandEditMode::DEMAND_STOP: {
            // Handle click
            myViewParent->getStopFrame()->addStop(myObjectsUnderCursor, myMouseButtonKeyPressed);
            // process click
            processClick(eventData);
            break;
        }
        case DemandEditMode::DEMAND_PERSON: {
            // Handle click
            myViewParent->getPersonFrame()->addPerson(myObjectsUnderCursor, myMouseButtonKeyPressed);
            // process click
            processClick(eventData);
            break;
        }
        case DemandEditMode::DEMAND_PERSONPLAN: {
            // Handle person plan click
            myViewParent->getPersonPlanFrame()->addPersonPlanElement(myObjectsUnderCursor, myMouseButtonKeyPressed);
            // process click
            processClick(eventData);
            break;
        }
        case DemandEditMode::DEMAND_CONTAINER: {
            // Handle click
            myViewParent->getContainerFrame()->addContainer(myObjectsUnderCursor, myMouseButtonKeyPressed);
            // process click
            processClick(eventData);
            break;
        }
        case DemandEditMode::DEMAND_CONTAINERPLAN: {
            // Handle container plan click
            myViewParent->getContainerPlanFrame()->addContainerPlanElement(myObjectsUnderCursor, myMouseButtonKeyPressed);
            // process click
            processClick(eventData);
            break;
        }
        default: {
            // process click
            processClick(eventData);
        }
    }
}


void
GNEViewNet::processLeftButtonReleaseDemand() {
    // check moved items
    if (myMoveMultipleElementValues.isMovingSelection()) {
        myMoveMultipleElementValues.finishMoveSelection();
    } else if (mySelectingArea.selectingUsingRectangle) {
        // check if we're creating a rectangle selection or we want only to select a lane
        if (mySelectingArea.startDrawing) {
            mySelectingArea.processRectangleSelection();
        }
        // finish selection
        mySelectingArea.finishRectangleSelection();
    } else {
        // finish moving of single elements
        myMoveSingleElementValues.finishMoveSingleElement();
    }
}


void
GNEViewNet::processMoveMouseDemand(const bool mouseLeftButtonPressed) {
    if (mySelectingArea.selectingUsingRectangle) {
        // update selection corner of selecting area
        mySelectingArea.moveRectangleSelection();
    } else {
        // move single elements
        myMoveSingleElementValues.moveSingleElement(mouseLeftButtonPressed);
    }
}


void
GNEViewNet::processLeftButtonPressData(void* eventData) {
    // decide what to do based on mode
    switch (myEditModes.dataEditMode) {
        case DataEditMode::DATA_INSPECT: {
            // process left click in Inspector Frame
            myViewParent->getInspectorFrame()->processDataSupermodeClick(getPositionInformation(), myObjectsUnderCursor);
            // process click
            processClick(eventData);
            break;
        }
        case DataEditMode::DATA_DELETE: {
            // check that we have clicked over an data element
            if (myObjectsUnderCursor.getAttributeCarrierFront() && myObjectsUnderCursor.getAttributeCarrierFront()->getTagProperty().isDataElement()) {
                // check if we are deleting a selection or an single attribute carrier
                if (myObjectsUnderCursor.getAttributeCarrierFront()->isAttributeCarrierSelected()) {
                    myViewParent->getDeleteFrame()->removeSelectedAttributeCarriers();
                } else {
                    myViewParent->getDeleteFrame()->removeAttributeCarrier(myObjectsUnderCursor);
                }
            } else {
                // process click
                processClick(eventData);
            }
            break;
        }
        case DataEditMode::DATA_SELECT:
            // avoid to select if control key is pressed
            if (!myMouseButtonKeyPressed.controlKeyPressed()) {
                // check if a rect for selecting is being created
                if (myMouseButtonKeyPressed.shiftKeyPressed()) {
                    // begin rectangle selection
                    mySelectingArea.beginRectangleSelection();
                } else {
                    // first check that under cursor there is an attribute carrier, is data element and is selectable
                    if (myObjectsUnderCursor.getAttributeCarrierFront() && myObjectsUnderCursor.getAttributeCarrierFront()->getTagProperty().isDataElement()) {
                        // Check if this GLobject type is locked
                        if (!myViewParent->getSelectorFrame()->getLockGLObjectTypes()->IsObjectTypeLocked(myObjectsUnderCursor.getGlTypeFront())) {
                            // toggle networkElement selection
                            if (myObjectsUnderCursor.getAttributeCarrierFront()->isAttributeCarrierSelected()) {
                                myObjectsUnderCursor.getAttributeCarrierFront()->unselectAttributeCarrier();
                            } else {
                                myObjectsUnderCursor.getAttributeCarrierFront()->selectAttributeCarrier();
                            }
                        }
                    }
                    // process click
                    processClick(eventData);
                }
            } else {
                // process click
                processClick(eventData);
            }
            break;
        case DataEditMode::DATA_EDGEDATA:
            // avoid create edgeData if control key is pressed
            if (!myMouseButtonKeyPressed.controlKeyPressed()) {
                if (myViewParent->getEdgeDataFrame()->addEdgeData(myObjectsUnderCursor, myMouseButtonKeyPressed)) {
                    // update view to show the new edge data
                    updateViewNet();
                }
            }
            // process click
            processClick(eventData);
            break;
        case DataEditMode::DATA_EDGERELDATA:
            // avoid create edgeData if control key is pressed
            if (!myMouseButtonKeyPressed.controlKeyPressed()) {
                if (myViewParent->getEdgeRelDataFrame()->addEdgeRelationData(myObjectsUnderCursor, myMouseButtonKeyPressed)) {
                    // update view to show the new edge data
                    updateViewNet();
                }
            }
            // process click
            processClick(eventData);
            break;
        case DataEditMode::DATA_TAZRELDATA:
            // avoid create TAZData if control key is pressed
            if (!myMouseButtonKeyPressed.controlKeyPressed()) {
                if (myViewParent->getTAZRelDataFrame()->addTAZRelationData(myObjectsUnderCursor, myMouseButtonKeyPressed)) {
                    // update view to show the new TAZ data
                    updateViewNet();
                }
            }
            // process click
            processClick(eventData);
            break;
        default: {
            // process click
            processClick(eventData);
        }
    }
}


void
GNEViewNet::processLeftButtonReleaseData() {
    // check moved items
    if (myMoveMultipleElementValues.isMovingSelection()) {
        myMoveMultipleElementValues.finishMoveSelection();
    } else if (mySelectingArea.selectingUsingRectangle) {
        // check if we're creating a rectangle selection or we want only to select a lane
        if (mySelectingArea.startDrawing) {
            mySelectingArea.processRectangleSelection();
        }
        // finish selection
        mySelectingArea.finishRectangleSelection();
    } else {
        // finish moving of single elements
        myMoveSingleElementValues.finishMoveSingleElement();
    }
}


void
GNEViewNet::processMoveMouseData(const bool mouseLeftButtonPressed) {
    if (mySelectingArea.selectingUsingRectangle) {
        // update selection corner of selecting area
        mySelectingArea.moveRectangleSelection();
    } else {
        // move single elements
        myMoveSingleElementValues.moveSingleElement(mouseLeftButtonPressed);
    }
}


/****************************************************************************/

