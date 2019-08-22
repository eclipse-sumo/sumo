/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEViewNet.cpp
/// @author  Jakob Erdmann
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2011
/// @version $Id$
///
// A view on the network being edited (adapted from GUIViewTraffic)
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================

#include <netedit/additionals/GNEPOI.h>
#include <netedit/additionals/GNEPoly.h>
#include <netedit/additionals/GNETAZ.h>
#include <netedit/demandelements/GNEDemandElement.h>
#include <netedit/frames/GNEAdditionalFrame.h>
#include <netedit/frames/GNEConnectorFrame.h>
#include <netedit/frames/GNECreateEdgeFrame.h>
#include <netedit/frames/GNECrossingFrame.h>
#include <netedit/frames/GNEDeleteFrame.h>
#include <netedit/frames/GNEInspectorFrame.h>
#include <netedit/frames/GNEPolygonFrame.h>
#include <netedit/frames/GNEProhibitionFrame.h>
#include <netedit/frames/GNERouteFrame.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <netedit/frames/GNETAZFrame.h>
#include <netedit/frames/GNETLSEditorFrame.h>
#include <netedit/frames/GNEVehicleFrame.h>
#include <netedit/frames/GNEVehicleTypeFrame.h>
#include <netedit/frames/GNEStopFrame.h>
#include <netedit/frames/GNEPersonTypeFrame.h>
#include <netedit/frames/GNEPersonFrame.h>
#include <netedit/frames/GNEPersonPlanFrame.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/netelements/GNECrossing.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNELane.h>
#include <utils/gui/cursors/GUICursorSubSys.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIDanielPerspectiveChanger.h>
#include <utils/gui/windows/GUIDialog_ViewSettings.h>
#include <utils/options/OptionsCont.h>

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
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_F3_SUPERMODE_NETWORK,                 GNEViewNet::onCmdSetSupermode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_F4_SUPERMODE_DEMAND,                  GNEViewNet::onCmdSetSupermode),
    // Modes
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_E_EDGEMODE,                           GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_M_MOVEMODE,                           GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_D_DELETEMODE,                         GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_I_INSPECTMODE,                        GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_S_SELECTMODE,                         GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_C_CONNECTMODE_PERSONPLANMODE,         GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_T_TLSMODE_VTYPEMODE,                  GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_A_ADDITIONALMODE_STOPMODE,            GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_R_CROSSINGMODE_ROUTEMODE,             GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_Z_TAZMODE,                            GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_P_POLYGONMODE_PERSONMODE,             GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_V_VEHICLEMODE,                        GNEViewNet::onCmdSetMode),
    FXMAPFUNC(SEL_COMMAND, MID_HOTKEY_W_PROHIBITIONMODE_PERSONTYPEMODE,     GNEViewNet::onCmdSetMode),
    // Common view options
    FXMAPFUNC(SEL_COMMAND, MID_GNE_COMMONVIEWOPTIONS_SHOWGRID,              GNEViewNet::onCmdToogleShowGrid),
    // Network view options
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_SHOWDEMANDELEMENTS,   GNEViewNet::onCmdToogleShowDemandElements),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_SELECTEDGES,          GNEViewNet::onCmdToogleSelectEdges),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_SHOWCONNECTIONS,      GNEViewNet::onCmdToogleShowConnections),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_SHOWCONNECTIONS,      GNEViewNet::onCmdToogleHideConnections),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_EXTENDSELECTION,      GNEViewNet::onCmdToogleExtendSelection),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_CHANGEALLPHASES,      GNEViewNet::onCmdToogleChangeAllPhases),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_ASKFORMERGE,          GNEViewNet::onCmdToogleWarnAboutMerge),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_SHOWBUBBLES,          GNEViewNet::onCmdToogleShowJunctionBubbles),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_MOVEELEVATION,        GNEViewNet::onCmdToogleMoveElevation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_CHAINEDGES,           GNEViewNet::onCmdToogleChainEdges),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_AUTOOPPOSITEEDGES,    GNEViewNet::onCmdToogleAutoOppositeEdge),
    // Demand view options
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_HIDENONINSPECTED,      GNEViewNet::onCmdToogleHideNonInspecteDemandElements),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_HIDESHAPES,            GNEViewNet::onCmdToogleHideShapes),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_SHOWALLPERSONPLANS,    GNEViewNet::onCmdToogleShowAllPersonPlans),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_LOCKPERSON,            GNEViewNet::onCmdToogleLockPerson),
    // Select elements
    FXMAPFUNC(SEL_COMMAND, MID_ADDSELECT,                                   GNEViewNet::onCmdAddSelected),
    FXMAPFUNC(SEL_COMMAND, MID_REMOVESELECT,                                GNEViewNet::onCmdRemoveSelected),
    // Junctions
    FXMAPFUNC(SEL_COMMAND, MID_GNE_JUNCTION_EDIT_SHAPE,                     GNEViewNet::onCmdEditJunctionShape),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_JUNCTION_RESET_SHAPE,                    GNEViewNet::onCmdResetJunctionShape),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_JUNCTION_REPLACE,                        GNEViewNet::onCmdReplaceJunction),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_JUNCTION_SPLIT,                          GNEViewNet::onCmdSplitJunction),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_JUNCTION_SPLIT_RECONNECT,                GNEViewNet::onCmdSplitJunctionReconnect),
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
    // Lanes
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_DUPLICATE,                          GNEViewNet::onCmdDuplicateLane),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_RESET_CUSTOMSHAPE,                  GNEViewNet::onCmdResetLaneCustomShape),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_TRANSFORM_SIDEWALK,                 GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_TRANSFORM_BIKE,                     GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_TRANSFORM_BUS,                      GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_TRANSFORM_GREENVERGE,               GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_ADD_SIDEWALK,                       GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_ADD_BIKE,                           GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_ADD_BUS,                            GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_ADD_GREENVERGE,                     GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_REMOVE_SIDEWALK,                    GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_REMOVE_BIKE,                        GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_REMOVE_BUS,                         GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_REMOVE_GREENVERGE,                  GNEViewNet::onCmdLaneOperation),
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
};

// Object implementation
FXIMPLEMENT(GNEViewNet, GUISUMOAbstractView, GNEViewNetMap, ARRAYNUMBER(GNEViewNetMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEViewNet::GNEViewNet(FXComposite* tmpParent, FXComposite* actualParent, GUIMainWindow& app,
                       GNEViewParent* viewParent, GNENet* net, GNEUndoList* undoList,
                       FXGLVisual* glVis, FXGLCanvas* share) :
    GUISUMOAbstractView(tmpParent, app, viewParent, net->getVisualisationSpeedUp(), glVis, share),
    myEditModes(this),
    myTestingMode(this),
    myCommonCheckableButtons(this),
    myNetworkCheckableButtons(this),
    myDemandCheckableButtons(this),
    myCommonViewOptions(this),
    myNetworkViewOptions(this),
    myDemandViewOptions(this),
    myMoveSingleElementValues(this),
    myMoveMultipleElementValues(this),
    myVehicleOptions(this),
    myVehicleTypeOptions(this),
    mySelectingArea(this),
    myEditShapes(this),
    myViewParent(viewParent),
    myNet(net),
    myCurrentFrame(nullptr),
    myUndoList(undoList) {
    // view must be the final member of actualParent
    reparent(actualParent);
    // Build edit modes
    buildEditModeControls();
    // Mark undo list
    myUndoList->mark();
    // set this viewNet in Net
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
GNEViewNet::buildViewToolBars(GUIGlChildWindow& cw) {
    // build coloring tools
    {
        for (auto it_names : gSchemeStorage.getNames()) {
            cw.getColoringSchemesCombo()->appendItem(it_names.c_str());
            if (it_names == myVisualizationSettings->name) {
                cw.getColoringSchemesCombo()->setCurrentItem(cw.getColoringSchemesCombo()->getNumItems() - 1);
            }
        }
        cw.getColoringSchemesCombo()->setNumVisible(MAX2(5, (int)gSchemeStorage.getNames().size() + 1));
    }
    // for junctions
    new FXButton(cw.getLocatorPopup(),
                 "\tLocate Junction\tLocate a junction within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATEJUNCTION), &cw, MID_LOCATEJUNCTION,
                 ICON_ABOVE_TEXT | FRAME_THICK | FRAME_RAISED);
    // for edges
    new FXButton(cw.getLocatorPopup(),
                 "\tLocate Street\tLocate a street within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATEEDGE), &cw, MID_LOCATEEDGE,
                 ICON_ABOVE_TEXT | FRAME_THICK | FRAME_RAISED);

    // for vehicles
    new FXButton(cw.getLocatorPopup(),
                 "\tLocate Vehicle\tLocate a vehicle within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATEVEHICLE), &cw, MID_LOCATEVEHICLE,
                 ICON_ABOVE_TEXT | FRAME_THICK | FRAME_RAISED);

    // for routes
    new FXButton(cw.getLocatorPopup(),
                 "\tLocate Route\tLocate a route within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATEROUTE), &cw, MID_LOCATEROUTE,
                 ICON_ABOVE_TEXT | FRAME_THICK | FRAME_RAISED);

    // for routes
    new FXButton(cw.getLocatorPopup(),
                 "\tLocate Stop\tLocate a stop within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATESTOP), &cw, MID_LOCATESTOP,
                 ICON_ABOVE_TEXT | FRAME_THICK | FRAME_RAISED);

    // for persons (currently unused)
    /*
    new FXButton(cw.getLocatorPopup(),
                 "\tLocate Vehicle\tLocate a person within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATEPERSON), &v, MID_LOCATEPERSON,
                 ICON_ABOVE_TEXT | FRAME_THICK | FRAME_RAISED);
    */

    // for tls
    new FXButton(cw.getLocatorPopup(),
                 "\tLocate TLS\tLocate a tls within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATETLS), &cw, MID_LOCATETLS,
                 ICON_ABOVE_TEXT | FRAME_THICK | FRAME_RAISED);
    // for additional stuff
    new FXButton(cw.getLocatorPopup(),
                 "\tLocate Additional\tLocate an additional structure within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATEADD), &cw, MID_LOCATEADD,
                 ICON_ABOVE_TEXT | FRAME_THICK | FRAME_RAISED);
    // for pois
    new FXButton(cw.getLocatorPopup(),
                 "\tLocate PoI\tLocate a PoI within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATEPOI), &cw, MID_LOCATEPOI,
                 ICON_ABOVE_TEXT | FRAME_THICK | FRAME_RAISED);
    // for polygons
    new FXButton(cw.getLocatorPopup(),
                 "\tLocate Polygon\tLocate a Polygon within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATEPOLY), &cw, MID_LOCATEPOLY,
                 ICON_ABOVE_TEXT | FRAME_THICK | FRAME_RAISED);
}


void
GNEViewNet::update() const {
    // this call is only used for breakpoints (to check when view is updated)
    GUISUMOAbstractView::update();
}


std::set<std::pair<std::string, GNEAttributeCarrier*> >
GNEViewNet::getAttributeCarriersInBoundary(const Boundary& boundary, bool forceSelectEdges) {
    // use a SET of pairs to obtain IDs and Pointers to attribute carriers. We need this because certain ACs can be returned many times (example: Edges)
    // Note: a map cannot be used because there is different ACs with the same ID (example: Additionals)
    std::set<std::pair<std::string, GNEAttributeCarrier*> > result;
    // firstm make OpenGL context current prior to performing OpenGL commands
    if (makeCurrent()) {
        // obtain GUIGLIds of all objects in the given boundary
        std::vector<GUIGlID> ids = getObjectsInBoundary(boundary);
        //  finish make OpenGL context current
        makeNonCurrent();
        // iterate over GUIGlIDs
        for (auto i : ids) {
            // avoid to select Net (i = 0)
            if (i != 0) {
                GNEAttributeCarrier* retrievedAC = myNet->retrieveAttributeCarrier(i);
                // in the case of a Lane, we need to change the retrieved lane to their the parent if myNetworkViewOptions.mySelectEdges is enabled
                if ((retrievedAC->getTagProperty().getTag() == SUMO_TAG_LANE) && (myNetworkViewOptions.selectEdges() || forceSelectEdges)) {
                    retrievedAC = &dynamic_cast<GNELane*>(retrievedAC)->getParentEdge();
                }
                // make sure that AttributeCarrier can be selected
                GUIGlObject* glObject = dynamic_cast<GUIGlObject*>(retrievedAC);
                if (glObject && !myViewParent->getSelectorFrame()->getLockGLObjectTypes()->IsObjectTypeLocked(glObject->getType())) {
                    result.insert(std::make_pair(retrievedAC->getID(), retrievedAC));
                }
            }
        }
    }
    return result;
}


void
GNEViewNet::buildSelectionACPopupEntry(GUIGLObjectPopupMenu* ret, GNEAttributeCarrier* AC) {
    if (AC->isAttributeCarrierSelected()) {
        new FXMenuCommand(ret, "Remove From Selected", GUIIconSubSys::getIcon(ICON_FLAG_MINUS), this, MID_REMOVESELECT);
    } else {
        new FXMenuCommand(ret, "Add To Selected", GUIIconSubSys::getIcon(ICON_FLAG_PLUS), this, MID_ADDSELECT);
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
    update();
    return true;
}


void
GNEViewNet::openObjectDialog() {
    // reimplemented from GUISUMOAbstractView due OverlappedInspection
    ungrab();
    if (!isEnabled() || !myAmInitialised) {
        return;
    }
    if (makeCurrent()) {
        // initialise the select mode
        int id = getObjectUnderCursor();
        GUIGlObject* o = nullptr;
        // we need to check if we're inspecting a overlapping element
        if (myViewParent->getInspectorFrame()->getOverlappedInspection()->overlappedInspectionShown() &&
                myViewParent->getInspectorFrame()->getOverlappedInspection()->checkSavedPosition(getPositionInformation()) &&
                myViewParent->getInspectorFrame()->getAttributesEditor()->getEditedACs().size() > 0) {
            o = dynamic_cast<GUIGlObject*>(myViewParent->getInspectorFrame()->getAttributesEditor()->getEditedACs().front());
        } else if (id != 0) {
            o = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
        } else {
            o = GUIGlObjectStorage::gIDStorage.getNetObject();
        }
        // check if open popup menu can be opened
        if (o != nullptr) {
            myPopup = o->getPopUpMenu(*myApp, *this);
            int x, y;
            FXuint b;
            myApp->getCursorPosition(x, y, b);
            myPopup->setX(x + myApp->getX());
            myPopup->setY(y + myApp->getY());
            myPopup->create();
            myPopup->show();
            myPopupPosition = getPositionInformation();
            myChanger->onRightBtnRelease(nullptr);
            GUIGlObjectStorage::gIDStorage.unblockObject(id);
            setFocus();
        }
        makeNonCurrent();
    }
}


const GNEViewNetHelper::EditModes&
GNEViewNet::getEditModes() const {
    return myEditModes;
}


const GNEViewNetHelper::CommonViewOptions&
GNEViewNet::getCommonViewOptions() const {
    return myCommonViewOptions;
}


const GNEViewNetHelper::NetworkViewOptions&
GNEViewNet::getNetworkViewOptions() const {
    return myNetworkViewOptions;
}


const GNEViewNetHelper::DemandViewOptions&
GNEViewNet::getDemandViewOptions() const {
    return myDemandViewOptions;
}


const GNEViewNetHelper::KeyPressed&
GNEViewNet::getKeyPressed() const {
    return myKeyPressed;
}


const GNEViewNetHelper::EditShapes&
GNEViewNet::getEditShapes() const {
    return myEditShapes;
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
    if (minValue != std::numeric_limits<double>::infinity()) {
        scheme.clear();
        // add new thresholds
        if (hide) {
            minValue = MAX2(hideThreshold + 1, minValue);
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
    return (myNetworkViewOptions.menuCheckExtendSelection->getCheck() != 0);
}


void
GNEViewNet::setSelectionScaling(double selectionScale) {
    myVisualizationSettings->selectionScale = selectionScale;
}


bool
GNEViewNet::changeAllPhases() const {
    return (myNetworkViewOptions.menuCheckChangeAllPhases->getCheck() != 0);
}


bool
GNEViewNet::showJunctionAsBubbles() const {
    return (myEditModes.networkEditMode == GNE_NMODE_MOVE) && (myNetworkViewOptions.menuCheckShowJunctionBubble->getCheck());
}


GNEViewNet::GNEViewNet() :
    myEditModes(this),
    myTestingMode(this),
    myCommonCheckableButtons(this),
    myNetworkCheckableButtons(this),
    myDemandCheckableButtons(this),
    myCommonViewOptions(this),
    myNetworkViewOptions(this),
    myDemandViewOptions(this),
    myMoveSingleElementValues(this),
    myMoveMultipleElementValues(this),
    myVehicleOptions(this),
    myVehicleTypeOptions(this),
    mySelectingArea(this),
    myEditShapes(this) {
}


int
GNEViewNet::doPaintGL(int mode, const Boundary& bound) {
    // init view settings
    if (!myVisualizationSettings->drawForSelecting && myVisualizationSettings->forceDrawForSelecting) {
        myVisualizationSettings->drawForSelecting = true;
    }
    glRenderMode(mode);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
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
    if (!myUseToolTips && !myVisualizationSettings->drawForSelecting) {
        drawDecals();
        // depending of the visualizationSettings, enable or disable check box show grid
        if (myVisualizationSettings->showGrid) {
            myCommonViewOptions.menuCheckShowGrid->setCheck(true);
            paintGLGrid();
        } else {
            myCommonViewOptions.menuCheckShowGrid->setCheck(false);
        }
        myNetworkViewOptions.menuCheckShowConnections->setCheck(myVisualizationSettings->showLane2Lane);
    }
    // draw temporal elements
    if (!myVisualizationSettings->drawForSelecting) {
        drawTemporalDrawShape();
        drawLaneCandidates();
        // draw testing elements
        myTestingMode.drawTestingElements(myApp);
        // draw temporal trip/flow route
        myViewParent->getVehicleFrame()->getEdgePathCreator()->drawTemporalRoute();
        // draw temporal person plan route
        myViewParent->getPersonFrame()->getEdgePathCreator()->drawTemporalRoute();
        myViewParent->getPersonPlanFrame()->getPersonPlanCreator()->drawTemporalRoute();
        // draw temporal non consecutive edge
        myViewParent->getRouteFrame()->drawTemporalRoute();
    }
    // check menu checks of supermode demand
    if (myEditModes.currentSupermode == GNE_SUPERMODE_DEMAND) {
        // enable or disable menuCheckShowAllPersonPlans depending of there is a locked person
        if (myDemandViewOptions.getLockedPerson()) {
            myDemandViewOptions.menuCheckShowAllPersonPlans->disable();
        } else {
            myDemandViewOptions.menuCheckShowAllPersonPlans->enable();
        }
        // check if menuCheckLockPerson must be enabled or disabled
        if (myDemandViewOptions.menuCheckLockPerson->getCheck() == FALSE) {
            // check if we're in inspector mode and we're inspecting exactly one element
            if ((myEditModes.demandEditMode == GNE_DMODE_INSPECT) && getDottedAC()) {
                // obtain tag property
                const GNEAttributeCarrier::TagProperties& tagProperty = getDottedAC()->getTagProperty();
                // enable menu check lock person if is either a person, a person plan or a person stop
                if (tagProperty.isPerson() || tagProperty.isPersonPlan() || tagProperty.isPersonStop()) {
                    myDemandViewOptions.menuCheckLockPerson->enable();
                } else {
                    myDemandViewOptions.menuCheckLockPerson->disable();
                }
            } else {
                myDemandViewOptions.menuCheckLockPerson->disable();
            }
        }
    }
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
    glPopMatrix();
    return hits2;
}


long
GNEViewNet::onLeftBtnPress(FXObject*, FXSelector, void* eventData) {
    // set focus in view net
    setFocus();
    // update keyPressed
    myKeyPressed.update(eventData);
    // interpret object under cursor
    if (makeCurrent()) {
        // fill objects under cursor
        myObjectsUnderCursor.updateObjectUnderCursor(getGUIGlObjectsUnderCursor(), myEditShapes.editedShapePoly);
        // process left button press function depending of supermode
        if (myEditModes.currentSupermode == GNE_SUPERMODE_NETWORK) {
            processLeftButtonPressNetwork(eventData);
        } else if (myEditModes.currentSupermode == GNE_SUPERMODE_DEMAND) {
            processLeftButtonPressDemand(eventData);
        }
        makeNonCurrent();
    }
    // update cursor
    updateCursor();
    // update view
    update();
    return 1;
}


long
GNEViewNet::onLeftBtnRelease(FXObject* obj, FXSelector sel, void* eventData) {
    // process parent function
    GUISUMOAbstractView::onLeftBtnRelease(obj, sel, eventData);
    // first update keyPressed
    myKeyPressed.update(eventData);
    // update cursor
    updateCursor();
    // process left button release function depending of supermode
    if (myEditModes.currentSupermode == GNE_SUPERMODE_NETWORK) {
        processLeftButtonReleaseNetwork();
    } else if (myEditModes.currentSupermode == GNE_SUPERMODE_DEMAND) {
        processLeftButtonReleaseDemand();
    }
    // update view
    update();
    return 1;
}


long
GNEViewNet::onRightBtnPress(FXObject* obj, FXSelector sel, void* eventData) {
    // update keyPressed
    myKeyPressed.update(eventData);
    // update cursor
    updateCursor();
    if ((myEditModes.networkEditMode == GNE_NMODE_POLYGON) && myViewParent->getPolygonFrame()->getDrawingShapeModul()->isDrawing()) {
        // disable right button press during drawing polygon
        return 1;
    } else {
        return GUISUMOAbstractView::onRightBtnPress(obj, sel, eventData);
    }
}


long
GNEViewNet::onRightBtnRelease(FXObject* obj, FXSelector sel, void* eventData) {
    // update keyPressed
    myKeyPressed.update(eventData);
    // update cursor
    updateCursor();
    // disable right button release during drawing polygon
    if ((myEditModes.networkEditMode == GNE_NMODE_POLYGON) && myViewParent->getPolygonFrame()->getDrawingShapeModul()->isDrawing()) {
        return 1;
    } else {
        return GUISUMOAbstractView::onRightBtnRelease(obj, sel, eventData);
    }
}


long
GNEViewNet::onMouseMove(FXObject* obj, FXSelector sel, void* eventData) {
    // process mouse move in GUISUMOAbstractView
    GUISUMOAbstractView::onMouseMove(obj, sel, eventData);
    // update keyPressed
    myKeyPressed.update(eventData);
    // update cursor
    updateCursor();
    // process mouse move function depending of supermode
    if (myEditModes.currentSupermode == GNE_SUPERMODE_NETWORK) {
        processMoveMouseNetwork();
    } else if (myEditModes.currentSupermode == GNE_SUPERMODE_DEMAND) {
        processMoveMouseDemand();
    }
    return 1;
}


long
GNEViewNet::onKeyPress(FXObject* o, FXSelector sel, void* eventData) {
    // update keyPressed
    myKeyPressed.update(eventData);
    // update cursor
    updateCursor();
    // change "delete last created point" depending of shift key
    if ((myEditModes.networkEditMode == GNE_NMODE_POLYGON) && myViewParent->getPolygonFrame()->getDrawingShapeModul()->isDrawing()) {
        myViewParent->getPolygonFrame()->getDrawingShapeModul()->setDeleteLastCreatedPoint(myKeyPressed.shiftKeyPressed());
        update();
    } else if ((myEditModes.networkEditMode == GNE_NMODE_TAZ) && myViewParent->getTAZFrame()->getDrawingShapeModul()->isDrawing()) {
        myViewParent->getTAZFrame()->getDrawingShapeModul()->setDeleteLastCreatedPoint(myKeyPressed.shiftKeyPressed());
        update();
    }
    return GUISUMOAbstractView::onKeyPress(o, sel, eventData);
}


long
GNEViewNet::onKeyRelease(FXObject* o, FXSelector sel, void* eventData) {
    // update keyPressed
    myKeyPressed.update(eventData);
    // update cursor
    updateCursor();
    // change "delete last created point" depending of shift key
    if ((myEditModes.networkEditMode == GNE_NMODE_POLYGON) && myViewParent->getPolygonFrame()->getDrawingShapeModul()->isDrawing()) {
        myViewParent->getPolygonFrame()->getDrawingShapeModul()->setDeleteLastCreatedPoint(myKeyPressed.shiftKeyPressed());
        update();
    }
    // check if selecting using rectangle has to be disabled
    if (mySelectingArea.selectingUsingRectangle && !myKeyPressed.shiftKeyPressed()) {
        mySelectingArea.selectingUsingRectangle = false;
        update();
    }
    return GUISUMOAbstractView::onKeyRelease(o, sel, eventData);
}


void
GNEViewNet::abortOperation(bool clearSelection) {
    // steal focus from any text fields and place it over view net
    setFocus();
    // check what supermode is enabled
    if (myEditModes.currentSupermode == GNE_SUPERMODE_NETWORK) {
        // abort operation depending of current mode
        if (myEditModes.networkEditMode == GNE_NMODE_CREATE_EDGE) {
            // abort edge creation in create edge frame
            myViewParent->getCreateEdgeFrame()->abortEdgeCreation();
        } else if (myEditModes.networkEditMode == GNE_NMODE_SELECT) {
            mySelectingArea.selectingUsingRectangle = false;
            // check if current selection has to be cleaned
            if (clearSelection) {
                myViewParent->getSelectorFrame()->clearCurrentSelection();
            }
        } else if (myEditModes.networkEditMode == GNE_NMODE_CONNECT) {
            // abort changes in Connector Frame
            myViewParent->getConnectorFrame()->getConnectionModifications()->onCmdCancelModifications(0, 0, 0);
        } else if (myEditModes.networkEditMode == GNE_NMODE_TLS) {
            myViewParent->getTLSEditorFrame()->onCmdCancel(nullptr, 0, nullptr);
        } else if (myEditModes.networkEditMode == GNE_NMODE_MOVE) {
            myEditShapes.stopEditCustomShape();
        } else if (myEditModes.networkEditMode == GNE_NMODE_POLYGON) {
            // abort current drawing
            myViewParent->getPolygonFrame()->getDrawingShapeModul()->abortDrawing();
        } else if (myEditModes.networkEditMode == GNE_NMODE_TAZ) {
            if (myViewParent->getTAZFrame()->getDrawingShapeModul()->isDrawing()) {
                // abort current drawing
                myViewParent->getPolygonFrame()->getDrawingShapeModul()->abortDrawing();
            } else if (myViewParent->getTAZFrame()->getTAZCurrentModul()->getTAZ() != nullptr) {
                // finish current editing TAZ
                myViewParent->getTAZFrame()->getTAZCurrentModul()->setTAZ(nullptr);
            }
        } else if (myEditModes.networkEditMode == GNE_NMODE_PROHIBITION) {
            myViewParent->getProhibitionFrame()->onCmdCancel(nullptr, 0, nullptr);
        } else if (myEditModes.networkEditMode == GNE_NMODE_ADDITIONAL) {
            // abort select lanes
            myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->abortConsecutiveLaneSelector();
        }
    } else if (myEditModes.currentSupermode == GNE_SUPERMODE_DEMAND) {
        // abort operation depending of current mode
        if (myEditModes.demandEditMode == GNE_DMODE_SELECT) {
            mySelectingArea.selectingUsingRectangle = false;
            // check if current selection has to be cleaned
            if (clearSelection) {
                myViewParent->getSelectorFrame()->clearCurrentSelection();
            }
        } else if (myEditModes.demandEditMode == GNE_DMODE_ROUTE) {
            myViewParent->getRouteFrame()->hotkeyEsc();
        } else if (myEditModes.demandEditMode == GNE_DMODE_VEHICLE) {
            myViewParent->getVehicleFrame()->getEdgePathCreator()->abortEdgePathCreation();
        } else if (myEditModes.demandEditMode == GNE_DMODE_PERSON) {
            myViewParent->getPersonFrame()->getEdgePathCreator()->abortEdgePathCreation();
        } else if (myEditModes.demandEditMode == GNE_DMODE_PERSONPLAN) {
            myViewParent->getPersonPlanFrame()->getPersonPlanCreator()->abortPersonPlanCreation();
        }
    }
    // abort undo list
    myUndoList->p_abort();
}


void
GNEViewNet::hotkeyDel() {
    if (myEditModes.networkEditMode == GNE_NMODE_CONNECT || myEditModes.networkEditMode == GNE_NMODE_TLS) {
        setStatusBarText("Cannot delete in this mode");
    } else {
        // delete elements depending of current supermode
        if (myEditModes.currentSupermode == GNE_SUPERMODE_NETWORK) {
            myUndoList->p_begin("delete network selection");
            deleteSelectedConnections();
            deleteSelectedCrossings();
            deleteSelectedAdditionals();
            deleteSelectedLanes();
            deleteSelectedEdges();
            deleteSelectedJunctions();
            deleteSelectedShapes();
            myUndoList->p_end();
        } else {
            myUndoList->p_begin("delete demand selection");
            deleteSelectedDemandElements();
            myUndoList->p_end();
        }
        // update view
        update();
    }
}


void
GNEViewNet::hotkeyEnter() {
    // check what supermode is enabled
    if (myEditModes.currentSupermode == GNE_SUPERMODE_NETWORK) {
        // abort operation depending of current mode
        if (myEditModes.networkEditMode == GNE_NMODE_CONNECT) {
            // Accept changes in Connector Frame
            myViewParent->getConnectorFrame()->getConnectionModifications()->onCmdSaveModifications(0, 0, 0);
        } else if (myEditModes.networkEditMode == GNE_NMODE_TLS) {
            myViewParent->getTLSEditorFrame()->onCmdOK(nullptr, 0, nullptr);
        } else if ((myEditModes.networkEditMode == GNE_NMODE_MOVE) && (myEditShapes.editedShapePoly != nullptr)) {
            myEditShapes.saveEditedShape();
        } else if (myEditModes.networkEditMode == GNE_NMODE_POLYGON) {
            if (myViewParent->getPolygonFrame()->getDrawingShapeModul()->isDrawing()) {
                // stop current drawing
                myViewParent->getPolygonFrame()->getDrawingShapeModul()->stopDrawing();
            } else {
                // start drawing
                myViewParent->getPolygonFrame()->getDrawingShapeModul()->startDrawing();
            }
        } else if (myEditModes.networkEditMode == GNE_NMODE_CROSSING) {
            myViewParent->getCrossingFrame()->createCrossingHotkey();
        } else if (myEditModes.networkEditMode == GNE_NMODE_TAZ) {
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
        } else if (myEditModes.networkEditMode == GNE_NMODE_ADDITIONAL) {
            if (myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->isSelectingLanes()) {
                // stop select lanes to create additional
                myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->stopConsecutiveLaneSelector();
            }
        }
    } else if (myEditModes.currentSupermode == GNE_SUPERMODE_DEMAND) {
        // abort operation depending of current mode
        if (myEditModes.demandEditMode == GNE_DMODE_ROUTE) {
            myViewParent->getRouteFrame()->hotkeyEnter();
        } else if (myEditModes.demandEditMode == GNE_DMODE_VEHICLE) {
            myViewParent->getVehicleFrame()->getEdgePathCreator()->finishEdgePathCreation();
        } else if (myEditModes.demandEditMode == GNE_DMODE_PERSON) {
            myViewParent->getPersonFrame()->getEdgePathCreator()->finishEdgePathCreation();
        } else if (myEditModes.demandEditMode == GNE_DMODE_PERSONPLAN) {
            myViewParent->getPersonPlanFrame()->getPersonPlanCreator()->finishPersonPlanCreation();
        }
    }
}


void
GNEViewNet::hotkeyBackSpace() {
    // Currently only used in Demand mode
    if (myEditModes.currentSupermode == GNE_SUPERMODE_DEMAND) {
        // abort operation depending of current mode
        if (myEditModes.demandEditMode == GNE_DMODE_ROUTE) {
            myViewParent->getRouteFrame()->hotkeyBackSpace();
        } else if (myEditModes.demandEditMode == GNE_DMODE_VEHICLE) {
            myViewParent->getVehicleFrame()->getEdgePathCreator()->removeLastInsertedElement();
        } else if (myEditModes.demandEditMode == GNE_DMODE_PERSON) {
            myViewParent->getPersonFrame()->getEdgePathCreator()->removeLastInsertedElement();
        } else if (myEditModes.demandEditMode == GNE_DMODE_PERSONPLAN) {
            myViewParent->getPersonPlanFrame()->getPersonPlanCreator()->removeLastAddedElement();
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


const GNEAttributeCarrier*
GNEViewNet::getDottedAC() const {
    return myDottedAC;
}


void
GNEViewNet::setDottedAC(const GNEAttributeCarrier* AC) {
    myDottedAC = AC;
}


bool
GNEViewNet::showLockIcon() const {
    return (myEditModes.networkEditMode == GNE_NMODE_MOVE || myEditModes.networkEditMode == GNE_NMODE_INSPECT || myEditModes.networkEditMode == GNE_NMODE_ADDITIONAL);
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
                    edge = &(((GNELane*)pointed)->getParentEdge());
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

long
GNEViewNet::onCmdSetSupermode(FXObject*, FXSelector sel, void*) {
    // check what network mode will be set
    switch (FXSELID(sel)) {
        case MID_HOTKEY_F3_SUPERMODE_NETWORK:
            myEditModes.setSupermode(GNE_SUPERMODE_NETWORK);
            break;
        case MID_HOTKEY_F4_SUPERMODE_DEMAND:
            myEditModes.setSupermode(GNE_SUPERMODE_DEMAND);
            break;
        default:
            break;
    }
    return 1;
}

long
GNEViewNet::onCmdSetMode(FXObject*, FXSelector sel, void*) {
    if (myEditModes.currentSupermode == GNE_SUPERMODE_NETWORK) {
        // check what network mode will be set
        switch (FXSELID(sel)) {
            case MID_HOTKEY_I_INSPECTMODE:
                myEditModes.setNetworkEditMode(GNE_NMODE_INSPECT);
                break;
            case MID_HOTKEY_D_DELETEMODE:
                myEditModes.setNetworkEditMode(GNE_NMODE_DELETE);
                break;
            case MID_HOTKEY_S_SELECTMODE:
                myEditModes.setNetworkEditMode(GNE_NMODE_SELECT);
                break;
            case MID_HOTKEY_M_MOVEMODE:
                myEditModes.setNetworkEditMode(GNE_NMODE_MOVE);
                break;
            case MID_HOTKEY_E_EDGEMODE:
                myEditModes.setNetworkEditMode(GNE_NMODE_CREATE_EDGE);
                break;
            case MID_HOTKEY_C_CONNECTMODE_PERSONPLANMODE:
                myEditModes.setNetworkEditMode(GNE_NMODE_CONNECT);
                break;
            case MID_HOTKEY_T_TLSMODE_VTYPEMODE:
                myEditModes.setNetworkEditMode(GNE_NMODE_TLS);
                break;
            case MID_HOTKEY_A_ADDITIONALMODE_STOPMODE:
                myEditModes.setNetworkEditMode(GNE_NMODE_ADDITIONAL);
                break;
            case MID_HOTKEY_R_CROSSINGMODE_ROUTEMODE:
                myEditModes.setNetworkEditMode(GNE_NMODE_CROSSING);
                break;
            case MID_HOTKEY_Z_TAZMODE:
                myEditModes.setNetworkEditMode(GNE_NMODE_TAZ);
                break;
            case MID_HOTKEY_P_POLYGONMODE_PERSONMODE:
                myEditModes.setNetworkEditMode(GNE_NMODE_POLYGON);
                break;
            case MID_HOTKEY_W_PROHIBITIONMODE_PERSONTYPEMODE:
                myEditModes.setNetworkEditMode(GNE_NMODE_PROHIBITION);
                break;
            default:
                break;
        }
    } else {
        // check what demand mode will be set
        switch (FXSELID(sel)) {
            case MID_HOTKEY_I_INSPECTMODE:
                myEditModes.setDemandEditMode(GNE_DMODE_INSPECT);
                break;
            case MID_HOTKEY_D_DELETEMODE:
                myEditModes.setDemandEditMode(GNE_DMODE_DELETE);
                break;
            case MID_HOTKEY_S_SELECTMODE:
                myEditModes.setDemandEditMode(GNE_DMODE_SELECT);
                break;
            case MID_HOTKEY_M_MOVEMODE:
                myEditModes.setDemandEditMode(GNE_DMODE_MOVE);
                break;
            case MID_HOTKEY_R_CROSSINGMODE_ROUTEMODE:
                myEditModes.setDemandEditMode(GNE_DMODE_ROUTE);
                break;
            case MID_HOTKEY_V_VEHICLEMODE:
                myEditModes.setDemandEditMode(GNE_DMODE_VEHICLE);
                break;
            case MID_HOTKEY_T_TLSMODE_VTYPEMODE:
                myEditModes.setDemandEditMode(GNE_DMODE_VEHICLETYPES);
                break;
            case MID_HOTKEY_A_ADDITIONALMODE_STOPMODE:
                myEditModes.setDemandEditMode(GNE_DMODE_STOP);
                break;
            case MID_HOTKEY_W_PROHIBITIONMODE_PERSONTYPEMODE:
                myEditModes.setDemandEditMode(GNE_DMODE_PERSONTYPES);
                break;
            case MID_HOTKEY_P_POLYGONMODE_PERSONMODE:
                myEditModes.setDemandEditMode(GNE_DMODE_PERSON);
                break;
            case MID_HOTKEY_C_CONNECTMODE_PERSONPLANMODE:
                myEditModes.setDemandEditMode(GNE_DMODE_PERSONPLAN);
                break;
            default:
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
        edge->resetEndpoint(getPopupPosition(), myUndoList);
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
GNEViewNet::onCmdSimplifyShape(FXObject*, FXSelector, void*) {
    if (myEditShapes.editedShapePoly != nullptr) {
        myEditShapes.editedShapePoly->simplifyShape(false);
        update();
    } else {
        GNEPoly* polygonUnderMouse = getPolygonAtPopupPosition();
        if (polygonUnderMouse) {
            polygonUnderMouse->simplifyShape();
        }
    }
    return 1;
}


long
GNEViewNet::onCmdDeleteGeometryPoint(FXObject*, FXSelector, void*) {
    if (myEditShapes.editedShapePoly != nullptr) {
        myEditShapes.editedShapePoly->deleteGeometryPoint(getPopupPosition(), false);
        update();
    } else {
        GNEPoly* polygonUnderMouse = getPolygonAtPopupPosition();
        if (polygonUnderMouse) {
            polygonUnderMouse->deleteGeometryPoint(getPopupPosition());
        }
    }
    return 1;
}


long
GNEViewNet::onCmdClosePolygon(FXObject*, FXSelector, void*) {
    if (myEditShapes.editedShapePoly != nullptr) {
        myEditShapes.editedShapePoly->closePolygon(false);
        update();
    } else {
        GNEPoly* polygonUnderMouse = getPolygonAtPopupPosition();
        if (polygonUnderMouse) {
            polygonUnderMouse->closePolygon();
        }
    }
    return 1;
}


long
GNEViewNet::onCmdOpenPolygon(FXObject*, FXSelector, void*) {
    if (myEditShapes.editedShapePoly != nullptr) {
        myEditShapes.editedShapePoly->openPolygon(false);
        update();
    } else {
        GNEPoly* polygonUnderMouse = getPolygonAtPopupPosition();
        if (polygonUnderMouse) {
            polygonUnderMouse->openPolygon();
        }
    }
    return 1;
}


long
GNEViewNet::onCmdSetFirstGeometryPoint(FXObject*, FXSelector, void*) {
    if (myEditShapes.editedShapePoly != nullptr) {
        myEditShapes.editedShapePoly->changeFirstGeometryPoint(myEditShapes.editedShapePoly->getVertexIndex(getPopupPosition(), false, false), false);
        update();
    } else {
        GNEPoly* polygonUnderMouse = getPolygonAtPopupPosition();
        if (polygonUnderMouse) {
            polygonUnderMouse->changeFirstGeometryPoint(polygonUnderMouse->getVertexIndex(getPopupPosition(), false, false));
        }
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
            std::vector<GUIGlID> GLIDs = getObjectsInBoundary(POI->getCenteringBoundary());
            std::vector<GNELane*> lanes;
            for (auto i : GLIDs) {
                GNELane* lane = dynamic_cast<GNELane*>(GUIGlObjectStorage::gIDStorage.getObjectBlocking(i));
                if (lane) {
                    lanes.push_back(lane);
                }
            }
            if (lanes.empty()) {
                WRITE_WARNING("No lanes around " + toString(SUMO_TAG_POI) + " to attach it");
            } else {
                // obtain nearest lane to POI
                GNELane* nearestLane = lanes.front();
                double minorPosOverLane = nearestLane->getGeometry().shape.nearest_offset_to_point2D(POI->getPositionInView());
                double minorLateralOffset = nearestLane->getGeometry().shape.positionAtOffset(minorPosOverLane).distanceTo(POI->getPositionInView());
                for (auto i : lanes) {
                    double posOverLane = i->getGeometry().shape.nearest_offset_to_point2D(POI->getPositionInView());
                    double lateralOffset = i->getGeometry().shape.positionAtOffset(posOverLane).distanceTo(POI->getPositionInView());
                    if (lateralOffset < minorLateralOffset) {
                        minorPosOverLane = posOverLane;
                        minorLateralOffset = lateralOffset;
                        nearestLane = i;
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
                // remove POI
                myUndoList->p_begin("attach POI into " + toString(SUMO_TAG_LANE));
                myNet->deleteShape(POI, myUndoList);
                // add POILane
                myNet->addPOI(id, type, color, pos, false, nearestLane->getID(), minorPosOverLane, 0, layer, angle, imgFile, relativePath, POIWidth, POIHeight);
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
            // remove POI
            myUndoList->p_begin("release POI from " + toString(SUMO_TAG_LANE));
            myNet->deleteShape(POI, myUndoList);
            // add POI
            myNet->addPOI(id, type, color, pos, false, "", 0, 0, layer, angle, imgFile, relativePath, POIWidth, POIWeight);
            myUndoList->p_end();
        }
        // update view after transform
        update();
    }
    return 1;
}


long
GNEViewNet::onCmdDuplicateLane(FXObject*, FXSelector, void*) {
    GNELane* lane = getLaneAtPopupPosition();
    if (lane != nullptr) {
        // when duplicating an unselected lane, keep all connections as they
        // are, otherwise recompute them
        if (lane->isAttributeCarrierSelected()) {
            myUndoList->p_begin("duplicate selected " + toString(SUMO_TAG_LANE) + "s");
            std::vector<GNELane*> lanes = myNet->retrieveLanes(true);
            for (auto it : lanes) {
                myNet->duplicateLane(it, myUndoList, true);
            }
            myUndoList->p_end();
        } else {
            myUndoList->p_begin("duplicate " + toString(SUMO_TAG_LANE));
            myNet->duplicateLane(lane, myUndoList, false);
            myUndoList->p_end();
        }
    }
    return 1;
}


long
GNEViewNet::onCmdResetLaneCustomShape(FXObject*, FXSelector, void*) {
    GNELane* lane = getLaneAtPopupPosition();
    if (lane != nullptr) {
        // when duplicating an unselected lane, keep all connections as they
        // are, otherwise recompute them
        if (lane->isAttributeCarrierSelected()) {
            myUndoList->p_begin("reset custom lane shapes");
            std::vector<GNELane*> lanes = myNet->retrieveLanes(true);
            for (auto it : lanes) {
                it->setAttribute(SUMO_ATTR_CUSTOMSHAPE, "", myUndoList);
            }
            myUndoList->p_end();
        } else {
            myUndoList->p_begin("reset custom lane shape");
            lane->setAttribute(SUMO_ATTR_CUSTOMSHAPE, "", myUndoList);
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
            return addRestrictedLane(SVC_PEDESTRIAN);
        case MID_GNE_LANE_ADD_BIKE:
            return addRestrictedLane(SVC_BICYCLE);
        case MID_GNE_LANE_ADD_BUS:
            return addRestrictedLane(SVC_BUS);
        case MID_GNE_LANE_ADD_GREENVERGE:
            return addRestrictedLane(SVC_IGNORING);
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
            break;
    }
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
    GNELane* lane = getLaneAtPopupPosition();
    if (lane != nullptr) {
        // Get selected lanes
        std::vector<GNELane*> lanes = myNet->retrieveLanes(true); ;
        // Declare map of edges and lanes
        std::map<GNEEdge*, GNELane*> mapOfEdgesAndLanes;
        // Iterate over selected lanes
        for (auto i : lanes) {
            mapOfEdgesAndLanes[myNet->retrieveEdge(i->getParentEdge().getID())] = i;
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
            myNet->restrictLane(vclass, lane, myUndoList);
            // end undo operation
            myUndoList->p_end();
        }
    }
    return 1;
}


bool
GNEViewNet::addRestrictedLane(SUMOVehicleClass vclass) {
    GNELane* lane = getLaneAtPopupPosition();
    if (lane != nullptr) {
        // Get selected edges
        std::vector<GNEEdge*> edges = myNet->retrieveEdges(true);
        // get selected lanes
        std::vector<GNELane*> lanes = myNet->retrieveLanes(true);
        // Declare set of edges
        std::set<GNEEdge*> setOfEdges;
        // Fill set of edges with vector of edges
        for (auto i : edges) {
            setOfEdges.insert(i);
        }
        // iterate over selected lanes
        for (auto it : lanes) {
            // Insert pointer to edge into set of edges (To avoid duplicates)
            setOfEdges.insert(myNet->retrieveEdge(it->getParentEdge().getID()));
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
            for (auto it : setOfEdges) {
                // add restricted lane (guess target)
                myNet->addRestrictedLane(vclass, *it, -1, myUndoList);
            }
            // end undo operation
            myUndoList->p_end();
        } else {
            // If only have a single lane, start undo/redo operation
            myUndoList->p_begin("Add vclass for " + toString(vclass));
            // Add restricted lane
            if (vclass == SVC_PEDESTRIAN) {
                // always add pedestrian lanes on the right
                myNet->addRestrictedLane(vclass, lane->getParentEdge(), 0, myUndoList);
            } else if (lane->getParentEdge().getLanes().size() == 1) {
                // guess insertion position if there is only 1 lane
                myNet->addRestrictedLane(vclass, lane->getParentEdge(), -1, myUndoList);
            } else {
                myNet->addRestrictedLane(vclass, lane->getParentEdge(), lane->getIndex(), myUndoList);
            }
            // end undo/redo operation
            myUndoList->p_end();
        }
    }
    return 1;
}


bool
GNEViewNet::removeRestrictedLane(SUMOVehicleClass vclass) {
    GNELane* lane = getLaneAtPopupPosition();
    if (lane != nullptr) {
        // Get selected edges
        std::vector<GNEEdge*> edges = myNet->retrieveEdges(true);
        // get selected lanes
        std::vector<GNELane*> lanes = myNet->retrieveLanes(true);
        // Declare set of edges
        std::set<GNEEdge*> setOfEdges;
        // Fill set of edges with vector of edges
        for (auto i : edges) {
            setOfEdges.insert(i);
        }
        // iterate over selected lanes
        for (auto it : lanes) {
            // Insert pointer to edge into set of edges (To avoid duplicates)
            setOfEdges.insert(myNet->retrieveEdge(it->getParentEdge().getID()));
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
            for (std::set<GNEEdge*>::iterator it = setOfEdges.begin(); it != setOfEdges.end(); it++) {
                // add Sidewalk
                myNet->removeRestrictedLane(vclass, *(*it), myUndoList);
            }
            // end undo operation
            myUndoList->p_end();
        } else {
            // If only have a single lane, start undo/redo operation
            myUndoList->p_begin("Remove vclass for " + toString(vclass));
            // Remove Sidewalk
            myNet->removeRestrictedLane(vclass, lane->getParentEdge(), myUndoList);
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
    // declare a flag for cursor move
    bool cursorMove = false;
    // check if in current mode/supermode cursor move can be shown
    if (myEditModes.currentSupermode == GNE_SUPERMODE_NETWORK) {
        if ((myEditModes.networkEditMode == GNE_NMODE_ADDITIONAL) ||
                (myEditModes.networkEditMode == GNE_NMODE_POLYGON) ||
                (myEditModes.networkEditMode == GNE_NMODE_TAZ)) {
            cursorMove = true;
        }
    } else if (myEditModes.currentSupermode == GNE_SUPERMODE_DEMAND) {
        if ((myEditModes.demandEditMode == GNE_DMODE_ROUTE) ||
                (myEditModes.demandEditMode == GNE_DMODE_VEHICLE) ||
                (myEditModes.demandEditMode == GNE_DMODE_STOP)) {
            cursorMove = true;
        }
    }
    // update cursor if control key is pressed
    if (myKeyPressed.controlKeyPressed() && cursorMove) {
        setDefaultCursor(GUICursorSubSys::getCursor(SUMOCURSOR_MOVE));
        setDragCursor(GUICursorSubSys::getCursor(SUMOCURSOR_MOVE));
    } else {
        setDefaultCursor(GUICursorSubSys::getCursor(SUMOCURSOR_DEFAULT));
        setDragCursor(GUICursorSubSys::getCursor(SUMOCURSOR_DEFAULT));
    }
}


long
GNEViewNet::onCmdEditJunctionShape(FXObject*, FXSelector, void*) {
    // Obtain junction under mouse
    GNEJunction* junction = getJunctionAtPopupPosition();
    if (junction) {
        if (!OptionsCont::getOptions().getBool("lefthand")) {
            // for lefthand networks, the shape is already computed in GNELoadThread::run()
            // computing it here does not work because the network needs to be
            // mirrored before and after
            junction->getNBNode()->computeNodeShape(-1);
        } else if (junction->getNBNode()->getShape().size() == 0) {
            // recompute the whole network
            myNet->computeAndUpdate(OptionsCont::getOptions(), false);
        }
        PositionVector nodeShape = junction->getNBNode()->getShape();
        nodeShape.closePolygon();
        myEditShapes.startEditCustomShape(junction, nodeShape, true);
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
        update();
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
        update();
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
        update();
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
        update();
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
        update();
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
        myEditShapes.startEditCustomShape(connection, connection->getGeometry().shape, false);
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
        myEditShapes.startEditCustomShape(crossing, shape, false);
    }
    // destroy pop-up and update view Net
    destroyPopup();
    setFocus();
    return 1;
}


long
GNEViewNet::onCmdToogleShowDemandElements(FXObject*, FXSelector sel, void*) {
    // update view to show demand elements
    update();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_SHOWDEMANDELEMENTS)) {
        myNetworkViewOptions.menuCheckShowDemandElements->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToogleSelectEdges(FXObject*, FXSelector sel, void*) {
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_SELECTEDGES)) {
        myNetworkViewOptions.menuCheckSelectEdges->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToogleShowConnections(FXObject*, FXSelector sel, void*) {
    // if show was enabled, init GNEConnections
    if (myNetworkViewOptions.menuCheckShowConnections->getCheck() == TRUE) {
        getNet()->initGNEConnections();
    }
    // change flag "showLane2Lane" in myVisualizationSettings
    myVisualizationSettings->showLane2Lane = (myNetworkViewOptions.menuCheckShowConnections->getCheck() == TRUE);
    // Hide/show connections requiere recompute
    getNet()->requireRecompute();
    // Update viewnNet to show/hide conections
    update();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_SHOWCONNECTIONS)) {
        myNetworkViewOptions.menuCheckShowConnections->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToogleHideConnections(FXObject*, FXSelector sel, void*) {
    // Update viewnNet to show/hide conections
    update();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_HIDECONNECTIONS)) {
        myNetworkViewOptions.menuCheckHideConnections->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToogleExtendSelection(FXObject*, FXSelector sel, void*) {
    // Only update view
    update();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_EXTENDSELECTION)) {
        myNetworkViewOptions.menuCheckExtendSelection->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToogleChangeAllPhases(FXObject*, FXSelector sel, void*) {
    // Only update view
    update();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_CHANGEALLPHASES)) {
        myNetworkViewOptions.menuCheckChangeAllPhases->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToogleShowGrid(FXObject*, FXSelector sel, void*) {
    // show or hidde grid depending of myNetworkViewOptions.menuCheckShowGrid
    if (myCommonViewOptions.menuCheckShowGrid->getCheck()) {
        myVisualizationSettings->showGrid = true;
    } else {
        myVisualizationSettings->showGrid = false;
    }
    // update view to show grid
    update();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_COMMONVIEWOPTIONS_SHOWGRID)) {
        myCommonViewOptions.menuCheckShowGrid->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToogleWarnAboutMerge(FXObject*, FXSelector sel, void*) {
    // Only update view
    update();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_ASKFORMERGE)) {
        myNetworkViewOptions.menuCheckWarnAboutMerge->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToogleShowJunctionBubbles(FXObject*, FXSelector sel, void*) {
    // Only update view
    update();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_SHOWBUBBLES)) {
        myNetworkViewOptions.menuCheckShowJunctionBubble->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToogleMoveElevation(FXObject*, FXSelector sel, void*) {
    // Only update view
    update();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_MOVEELEVATION)) {
        myNetworkViewOptions.menuCheckMoveElevation->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToogleChainEdges(FXObject*, FXSelector sel, void*) {
    // Only update view
    update();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_CHAINEDGES)) {
        myNetworkViewOptions.menuCheckChainEdges->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToogleAutoOppositeEdge(FXObject*, FXSelector sel, void*) {
    // Only update view
    update();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_NETWORKVIEWOPTIONS_AUTOOPPOSITEEDGES)) {
        myNetworkViewOptions.menuCheckAutoOppositeEdge->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToogleHideNonInspecteDemandElements(FXObject*, FXSelector sel, void*) {
    // Only update view
    update();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_HIDENONINSPECTED)) {
        myDemandViewOptions.menuCheckHideNonInspectedDemandElements->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToogleHideShapes(FXObject*, FXSelector sel, void*) {
    // Only update view
    update();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_HIDESHAPES)) {
        myDemandViewOptions.menuCheckHideShapes->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToogleShowAllPersonPlans(FXObject*, FXSelector sel, void*) {
    // Only update view
    update();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_SHOWALLPERSONPLANS)) {
        myDemandViewOptions.menuCheckShowAllPersonPlans->setFocus();
    }
    return 1;
}


long
GNEViewNet::onCmdToogleLockPerson(FXObject*, FXSelector sel, void*) {
    // lock or unlock current inspected person depending of menuCheckLockPerson value
    if (myDemandViewOptions.menuCheckLockPerson->getCheck()) {
        // obtan locked person or person plan
        const GNEDemandElement* personOrPersonPlan = dynamic_cast<const GNEDemandElement*>(getDottedAC());
        if (personOrPersonPlan) {
            // lock person depending if casted demand element is either a person or a person plan
            if (personOrPersonPlan->getTagProperty().isPerson()) {
                myDemandViewOptions.lockPerson(personOrPersonPlan);
                // change menuCheckLockPerson text
                myDemandViewOptions.menuCheckLockPerson->setText(("unlock " + personOrPersonPlan->getID()).c_str());
            } else {
                myDemandViewOptions.lockPerson(personOrPersonPlan->getDemandElementParents().front());
                // change menuCheckLockPerson text
                myDemandViewOptions.menuCheckLockPerson->setText(("unlock " + personOrPersonPlan->getDemandElementParents().front()->getID()).c_str());
            }
        }
    } else {
        // unlock current person
        myDemandViewOptions.unlockPerson();
        // change menuCheckLockPerson text
        myDemandViewOptions.menuCheckLockPerson->setText("lock person");
    }
    // update view
    update();
    // set focus in menu check again, if this function was called clicking over menu check instead using alt+<key number>
    if (sel == FXSEL(SEL_COMMAND, MID_GNE_DEMANDVIEWOPTIONS_LOCKPERSON)) {
        myDemandViewOptions.menuCheckLockPerson->setFocus();
    }
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

// ===========================================================================
// private
// ===========================================================================

void
GNEViewNet::buildEditModeControls() {
    // first build supermode buttons
    myEditModes.buildSuperModeButtons();

    // build menu checks for Common checkable buttons
    myCommonCheckableButtons.buildCommonCheckableButtons();

    // build menu checks for Network checkable buttons
    myNetworkCheckableButtons.buildNetworkCheckableButtons();

    // build menu checks for Demand checkable buttons
    myDemandCheckableButtons.buildDemandCheckableButtons();

    // build menu checks of view options Common
    myCommonViewOptions.buildCommonViewOptionsMenuChecks();

    // build menu checks of view options Network
    myNetworkViewOptions.buildNetworkViewOptionsMenuChecks();

    // build menu checks of view options Demand
    myDemandViewOptions.buildDemandViewOptionsMenuChecks();
}


void
GNEViewNet::updateNetworkModeSpecificControls() {
    // hide grid
    myCommonViewOptions.menuCheckShowGrid->setCheck(myVisualizationSettings->showGrid);
    // hide all checkbox of view options Common
    myCommonViewOptions.hideCommonViewOptionsMenuChecks();
    // hide all checkbox of view options Network
    myNetworkViewOptions.hideNetworkViewOptionsMenuChecks();
    // hide all checkbox of view options Demand
    myDemandViewOptions.hideDemandViewOptionsMenuChecks();
    // disable all common edit modes
    myCommonCheckableButtons.disableCommonCheckableButtons();
    // disable all network edit modes
    myNetworkCheckableButtons.disableNetworkCheckableButtons();
    // hide all frames
    myViewParent->hideAllFrames();
    // In network mode, always show option "show demand elements"
    myNetworkViewOptions.menuCheckShowDemandElements->show();
    // enable selected controls
    switch (myEditModes.networkEditMode) {
        // common modes
        case GNE_NMODE_INSPECT:
            myViewParent->getInspectorFrame()->show();
            myViewParent->getInspectorFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getInspectorFrame();
            myCommonCheckableButtons.inspectButton->setChecked(true);
            // show view options
            myNetworkViewOptions.menuCheckSelectEdges->show();
            myNetworkViewOptions.menuCheckShowConnections->show();
            // show toolbar grip of view options
            myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->show();
            break;
        case GNE_NMODE_DELETE:
            myViewParent->getDeleteFrame()->show();
            myViewParent->getDeleteFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getDeleteFrame();
            myCommonCheckableButtons.deleteButton->setChecked(true);
            myNetworkViewOptions.menuCheckShowConnections->show();
            // show view options
            myNetworkViewOptions.menuCheckSelectEdges->show();
            // show toolbar grip of view options
            myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->show();
            break;
        case GNE_NMODE_SELECT:
            myViewParent->getSelectorFrame()->show();
            myViewParent->getSelectorFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getSelectorFrame();
            myCommonCheckableButtons.selectButton->setChecked(true);
            // show view options
            myNetworkViewOptions.menuCheckSelectEdges->show();
            myNetworkViewOptions.menuCheckShowConnections->show();
            myNetworkViewOptions.menuCheckExtendSelection->show();
            // show toolbar grip of view options
            myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->show();
            break;
        // specific modes
        case GNE_NMODE_CREATE_EDGE:
            myNetworkViewOptions.menuCheckChainEdges->show();
            myNetworkViewOptions.menuCheckAutoOppositeEdge->show();
            myNetworkCheckableButtons.createEdgeButton->setChecked(true);
            // show view options
            myCommonViewOptions.menuCheckShowGrid->show();
            // show toolbar grip of view options
            myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->show();
            break;
        case GNE_NMODE_MOVE:
            myNetworkViewOptions.menuCheckWarnAboutMerge->show();
            myNetworkViewOptions.menuCheckShowJunctionBubble->show();
            myNetworkViewOptions.menuCheckMoveElevation->show();
            myCommonCheckableButtons.moveButton->setChecked(true);
            // show view options
            myCommonViewOptions.menuCheckShowGrid->show();
            // show toolbar grip of view options
            myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->show();
            break;
        case GNE_NMODE_CONNECT:
            myViewParent->getConnectorFrame()->show();
            myViewParent->getConnectorFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getConnectorFrame();
            myNetworkCheckableButtons.connectionButton->setChecked(true);
            // show view options
            myNetworkViewOptions.menuCheckHideConnections->show();
            // show toolbar grip of view options
            myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->show();
            break;
        case GNE_NMODE_TLS:
            myViewParent->getTLSEditorFrame()->show();
            myViewParent->getTLSEditorFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getTLSEditorFrame();
            myNetworkCheckableButtons.trafficLightButton->setChecked(true);
            // show view options
            myNetworkViewOptions.menuCheckChangeAllPhases->show();
            // show toolbar grip of view options
            myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->show();
            break;
        case GNE_NMODE_ADDITIONAL:
            myViewParent->getAdditionalFrame()->show();
            myViewParent->getAdditionalFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getAdditionalFrame();
            myNetworkCheckableButtons.additionalButton->setChecked(true);
            // show view options
            myCommonViewOptions.menuCheckShowGrid->show();
            // show toolbar grip of view options
            myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->show();
            break;
        case GNE_NMODE_CROSSING:
            myViewParent->getCrossingFrame()->show();
            myViewParent->getCrossingFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getCrossingFrame();
            myNetworkCheckableButtons.crossingButton->setChecked(true);
            // show view options
            myCommonViewOptions.menuCheckShowGrid->setCheck(false);
            // show toolbar grip of view options
            myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->show();
            break;
        case GNE_NMODE_TAZ:
            myViewParent->getTAZFrame()->show();
            myViewParent->getTAZFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getTAZFrame();
            myNetworkCheckableButtons.TAZButton->setChecked(true);
            // show view options
            myCommonViewOptions.menuCheckShowGrid->setCheck(false);
            // show toolbar grip of view options
            myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->show();
            break;
        case GNE_NMODE_POLYGON:
            myViewParent->getPolygonFrame()->show();
            myViewParent->getPolygonFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getPolygonFrame();
            myNetworkCheckableButtons.shapeButton->setChecked(true);
            // show view options
            myCommonViewOptions.menuCheckShowGrid->show();
            // show toolbar grip of view options
            myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->show();
            break;
        case GNE_NMODE_PROHIBITION:
            myViewParent->getProhibitionFrame()->show();
            myViewParent->getProhibitionFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getProhibitionFrame();
            myNetworkCheckableButtons.prohibitionButton->setChecked(true);
            // hide toolbar grip of view options
            myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->hide();
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
    // force repaint because different modes draw different things
    onPaint(nullptr, 0, nullptr);
    // finally update view
    update();
}


void
GNEViewNet::updateDemandModeSpecificControls() {
    // hide grid
    myCommonViewOptions.menuCheckShowGrid->setCheck(myVisualizationSettings->showGrid);
    // hide all checkbox of view options Network
    myNetworkViewOptions.hideNetworkViewOptionsMenuChecks();
    // hide all checkbox of view options Demand
    myDemandViewOptions.hideDemandViewOptionsMenuChecks();
    // disable all common edit modes
    myCommonCheckableButtons.disableCommonCheckableButtons();
    // disable all Demand edit modes
    myDemandCheckableButtons.disableDemandCheckableButtons();
    // hide all frames
    myViewParent->hideAllFrames();
    // enable selected controls
    switch (myEditModes.demandEditMode) {
        // common modes
        case GNE_DMODE_INSPECT:
            myViewParent->getInspectorFrame()->show();
            myViewParent->getInspectorFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getInspectorFrame();
            myCommonCheckableButtons.inspectButton->setChecked(true);
            // show view options
            myDemandViewOptions.menuCheckHideShapes->show();
            myDemandViewOptions.menuCheckHideNonInspectedDemandElements->show();
            myDemandViewOptions.menuCheckShowAllPersonPlans->show();
            myDemandViewOptions.menuCheckLockPerson->show();
            // show toolbar grip of view options
            myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->show();
            break;
        case GNE_DMODE_DELETE:
            myViewParent->getDeleteFrame()->show();
            myViewParent->getDeleteFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getDeleteFrame();
            myCommonCheckableButtons.deleteButton->setChecked(true);
            // show view options
            myDemandViewOptions.menuCheckHideShapes->show();
            myDemandViewOptions.menuCheckShowAllPersonPlans->show();
            myDemandViewOptions.menuCheckLockPerson->show();
            // show toolbar grip of view options
            myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->show();
            break;
        case GNE_DMODE_SELECT:
            myViewParent->getSelectorFrame()->show();
            myViewParent->getSelectorFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getSelectorFrame();
            myCommonCheckableButtons.selectButton->setChecked(true);
            // show view options
            myDemandViewOptions.menuCheckHideShapes->show();
            myDemandViewOptions.menuCheckShowAllPersonPlans->show();
            myDemandViewOptions.menuCheckLockPerson->show();
            // show toolbar grip of view options
            myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->show();
            break;
        case GNE_DMODE_MOVE:
            myCommonCheckableButtons.moveButton->setChecked(true);
            // show view options
            myDemandViewOptions.menuCheckHideShapes->show();
            myCommonViewOptions.menuCheckShowGrid->show();
            myDemandViewOptions.menuCheckShowAllPersonPlans->show();
            myDemandViewOptions.menuCheckLockPerson->show();
            // show toolbar grip of view options
            myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->show();
            break;
        // specific modes
        case GNE_DMODE_ROUTE:
            myViewParent->getRouteFrame()->show();
            myViewParent->getRouteFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getRouteFrame();
            myDemandCheckableButtons.routeButton->setChecked(true);
            // show view options
            myDemandViewOptions.menuCheckHideShapes->show();
            myDemandViewOptions.menuCheckShowAllPersonPlans->show();
            myDemandViewOptions.menuCheckLockPerson->show();
            // show toolbar grip of view options
            myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->show();
            break;
        case GNE_DMODE_VEHICLE:
            myViewParent->getVehicleFrame()->show();
            myViewParent->getVehicleFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getVehicleFrame();
            myDemandCheckableButtons.vehicleButton->setChecked(true);
            // show view options
            myDemandViewOptions.menuCheckHideShapes->show();
            myDemandViewOptions.menuCheckShowAllPersonPlans->show();
            myDemandViewOptions.menuCheckLockPerson->show();
            // show toolbar grip of view options
            myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->show();
            break;
        case GNE_DMODE_VEHICLETYPES:
            myViewParent->getVehicleTypeFrame()->show();
            myViewParent->getVehicleTypeFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getVehicleTypeFrame();
            myDemandCheckableButtons.vehicleTypeButton->setChecked(true);
            // show view options
            myDemandViewOptions.menuCheckHideShapes->show();
            myDemandViewOptions.menuCheckShowAllPersonPlans->show();
            myDemandViewOptions.menuCheckLockPerson->show();
            /// show toolbar grip of view options
            myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->show();
            break;
        case GNE_DMODE_STOP:
            myViewParent->getStopFrame()->show();
            myViewParent->getStopFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getStopFrame();
            myDemandCheckableButtons.stopButton->setChecked(true);
            // show view options
            myDemandViewOptions.menuCheckHideShapes->show();
            myDemandViewOptions.menuCheckShowAllPersonPlans->show();
            myDemandViewOptions.menuCheckLockPerson->show();
            // show toolbar grip of view options
            myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->show();
            break;
        case GNE_DMODE_PERSONTYPES:
            myViewParent->getPersonTypeFrame()->show();
            myViewParent->getPersonTypeFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getPersonTypeFrame();
            myDemandCheckableButtons.personTypeButton->setChecked(true);
            // show view options
            myDemandViewOptions.menuCheckHideShapes->show();
            myDemandViewOptions.menuCheckShowAllPersonPlans->show();
            myDemandViewOptions.menuCheckLockPerson->show();
            // show toolbar grip of view options
            myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->show();
            break;
        case GNE_DMODE_PERSON:
            myViewParent->getPersonFrame()->show();
            myViewParent->getPersonFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getPersonFrame();
            myDemandCheckableButtons.personButton->setChecked(true);
            // show view options
            myDemandViewOptions.menuCheckHideShapes->show();
            myDemandViewOptions.menuCheckShowAllPersonPlans->show();
            myDemandViewOptions.menuCheckLockPerson->show();
            // show toolbar grip of view options
            myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->show();
            break;
        case GNE_DMODE_PERSONPLAN:
            myViewParent->getPersonPlanFrame()->show();
            myViewParent->getPersonPlanFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getPersonPlanFrame();
            myDemandCheckableButtons.personPlanButton->setChecked(true);
            // show view options
            myDemandViewOptions.menuCheckHideShapes->show();
            myDemandViewOptions.menuCheckShowAllPersonPlans->show();
            myDemandViewOptions.menuCheckLockPerson->show();
            // show toolbar grip of view options
            myViewParent->getGNEAppWindows()->getToolbarsGrip().modeOptions->show();
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
    // force repaint because different modes draw different things
    onPaint(nullptr, 0, nullptr);
    // finally update view
    update();
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
    std::vector<GNEAdditional*> additionals = myNet->retrieveAdditionals(true);
    if (additionals.size() > 0) {
        std::string plural = additionals.size() == 1 ? ("") : ("s");
        myUndoList->p_begin("delete selected additional" + plural);
        for (auto i : additionals) {
            // due there are additionals that are removed when their parent is removed, we need to check if yet exists before removing
            if (myNet->retrieveAdditional(i->getTagProperty().getTag(), i->getID(), false) != nullptr) {
                myNet->deleteAdditional(i, myUndoList);
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
        for (auto i : demandElements) {
            // due there are demand elements that are removed when their parent is removed, we need to check if yet exists before removing
            if (myNet->retrieveDemandElement(i->getTagProperty().getTag(), i->getID(), false) != nullptr) {
                myNet->deleteDemandElement(i, myUndoList);
            }
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


bool
GNEViewNet::mergeJunctions(GNEJunction* moved, const Position& oldPos) {
    const Position& newPos = moved->getNBNode()->getPosition();
    GNEJunction* mergeTarget = nullptr;
    // try to find another junction to merge with
    if (makeCurrent()) {
        Boundary selection;
        selection.add(newPos);
        selection.grow(0.1);
        const std::vector<GUIGlID> ids = getObjectsInBoundary(selection);
        GUIGlObject* object = nullptr;
        for (auto it_ids : ids) {
            if (it_ids == 0) {
                continue;
            }
            object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(it_ids);
            if (!object) {
                throw ProcessError("Unkown object in selection (id=" + toString(it_ids) + ").");
            }
            if ((object->getType() == GLO_JUNCTION) && (it_ids != moved->getGlID())) {
                mergeTarget = dynamic_cast<GNEJunction*>(object);
            }
            GUIGlObjectStorage::gIDStorage.unblockObject(it_ids);
        }
    }
    if (mergeTarget) {
        // optionally ask for confirmation
        if (myNetworkViewOptions.menuCheckWarnAboutMerge->getCheck()) {
            WRITE_DEBUG("Opening FXMessageBox 'merge junctions'");
            // open question box
            FXuint answer = FXMessageBox::question(this, MBOX_YES_NO,
                                                   "Confirm Junction Merger", "%s",
                                                   ("Do you wish to merge junctions '" + moved->getMicrosimID() +
                                                    "' and '" + mergeTarget->getMicrosimID() + "'?\n" +
                                                    "('" + moved->getMicrosimID() +
                                                    "' will be eliminated and its roads added to '" +
                                                    mergeTarget->getMicrosimID() + "')").c_str());
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
        // restore previous position of junction moved
        moved->moveGeometry(oldPos, Position(0, 0));
        // finish geometry moving
        moved->endGeometryMoving();
        // merge moved and targed junctions
        myNet->mergeJunctions(moved, mergeTarget, myUndoList);
        return true;
    } else {
        return false;
    }
}


void
GNEViewNet::updateControls() {
    switch (myEditModes.networkEditMode) {
        case GNE_NMODE_INSPECT:
            myViewParent->getInspectorFrame()->update();
            break;
        default:
            break;
    }
    // update view
    update();
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
            glPushMatrix();
            // obtain first clicked point
            const Position& firstLanePoint = myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLanes().front().first->getGeometry().shape.positionAtOffset(
                                                 myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLanes().front().second);
            // must draw on top of other connections
            glTranslated(firstLanePoint.x(), firstLanePoint.y(), GLO_JUNCTION + 0.3);
            GLHelper::setColor(RGBColor::RED);
            // draw first point
            GLHelper::drawFilledCircle((double) 1.3, myVisualizationSettings->getCircleResolution());
            GLHelper::drawText("S", Position(), .1, 1.3, RGBColor::CYAN);
            // pop draw matrix
            glPopMatrix();
        }
        // draw connections between lanes
        if (myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLanes().size() > 1) {
            // iterate over all current selected lanes
            for (int i = 0; i < (int)myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLanes().size() - 1; i++) {
                // declare position vector for shape
                PositionVector shape;
                // declare vectors for shape rotation and lenghts
                std::vector<double> shapeRotations, shapeLengths;
                // obtain GNELanes
                GNELane* from = myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLanes().at(i).first;
                GNELane* to = myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLanes().at(i + 1).first;
                // Push draw matrix
                glPushMatrix();
                // must draw on top of other connections
                glTranslated(0, 0, GLO_JUNCTION + 0.2);
                // obtain connection shape
                shape = from->getParentEdge().getNBEdge()->getConnection(from->getIndex(), to->getParentEdge().getNBEdge(), to->getIndex()).shape;
                // set special color
                GLHelper::setColor(myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLaneColor());
                // Obtain lengths and shape rotations
                int segments = (int) shape.size() - 1;
                if (segments >= 0) {
                    shapeRotations.reserve(segments);
                    shapeLengths.reserve(segments);
                    for (int j = 0; j < segments; j++) {
                        const Position& f = shape[j];
                        const Position& s = shape[j + 1];
                        shapeLengths.push_back(f.distanceTo2D(s));
                        shapeRotations.push_back((double) atan2((s.x() - f.x()), (f.y() - s.y())) * (double) 180.0 / (double)M_PI);
                    }
                }
                // draw a list of lines
                GLHelper::drawBoxLines(shape, shapeRotations, shapeLengths, 0.2);
                // pop draw matrix
                glPopMatrix();
            }
            // draw last point
            glPushMatrix();
            // obtain last clicked point
            const Position& lastLanePoint = myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLanes().back().first->getGeometry().shape.positionAtOffset(
                                                myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLanes().back().second);
            // must draw on top of other connections
            glTranslated(lastLanePoint.x(), lastLanePoint.y(), GLO_JUNCTION + 0.3);
            GLHelper::setColor(RGBColor::RED);
            // draw last point
            GLHelper::drawFilledCircle((double) 1.3, 8);
            GLHelper::drawText("E", Position(), .1, 1.3, RGBColor::CYAN);
            // pop draw matrix
            glPopMatrix();
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
        glPushMatrix();
        glLineWidth(2);
        glTranslated(0, 0, GLO_MAX);
        GLHelper::setColor(RGBColor::BLUE);
        GLHelper::drawLine(temporalShape);
        glPopMatrix();
        // draw red line from the last point of shape to the current mouse position
        glPushMatrix();
        glLineWidth(2);
        glTranslated(0, 0, GLO_MAX);
        // draw last line depending if shift key (delete last created point) is pressed
        if (deleteLastCreatedPoint) {
            GLHelper::setColor(RGBColor::RED);
        } else {
            GLHelper::setColor(RGBColor::GREEN);
        }
        GLHelper::drawLine(temporalShape.back(), snapToActiveGrid(getPositionInformation()));
        glPopMatrix();
    }
}


void
GNEViewNet::processLeftButtonPressNetwork(void* eventData) {
    // decide what to do based on mode
    switch (myEditModes.networkEditMode) {
        case GNE_NMODE_INSPECT: {
            // process left click in Inspector Frame
            myViewParent->getInspectorFrame()->processNetworkSupermodeClick(getPositionInformation(), myObjectsUnderCursor);
            // process click
            processClick(eventData);
            break;
        }
        case GNE_NMODE_DELETE: {
            // check that we have clicked over an non-demand element
            if (myObjectsUnderCursor.getAttributeCarrierFront() && !myObjectsUnderCursor.getAttributeCarrierFront()->getTagProperty().isDemandElement()) {
                // change the selected attribute carrier if myNetworkViewOptions.mySelectEdges is enabled and clicked element is a getLaneFront()
                if (myNetworkViewOptions.selectEdges() && (myObjectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag() == SUMO_TAG_LANE) && !myKeyPressed.shiftKeyPressed()) {
                    myObjectsUnderCursor.swapLane2Edge();
                }
                // check if we are deleting a selection or an single attribute carrier
                if (myObjectsUnderCursor.getAttributeCarrierFront()->isAttributeCarrierSelected()) {
                    // before delete al selected attribute carriers, check if we clicked over a geometry point
                    if (myViewParent->getDeleteFrame()->getDeleteOptions()->deleteOnlyGeometryPoints() &&
                            (((myObjectsUnderCursor.getEdgeFront()) && (myObjectsUnderCursor.getEdgeFront()->getVertexIndex(getPositionInformation(), false, false) != -1))
                             || ((myObjectsUnderCursor.getPolyFront()) && (myObjectsUnderCursor.getPolyFront()->getVertexIndex(getPositionInformation(), false, false) != -1)))) {
                        myViewParent->getDeleteFrame()->removeAttributeCarrier(myObjectsUnderCursor);
                    } else {
                        myViewParent->getDeleteFrame()->removeSelectedAttributeCarriers();
                    }
                } else {
                    myViewParent->getDeleteFrame()->removeAttributeCarrier(myObjectsUnderCursor);
                }
            } else {
                // process click
                processClick(eventData);
            }
            break;
        }
        case GNE_NMODE_SELECT:
            // check if a rect for selecting is being created
            if (myKeyPressed.shiftKeyPressed()) {
                // begin rectangle selection
                mySelectingArea.beginRectangleSelection();
            } else {
                // first check that under cursor there is an attribute carrier, isn't a demand element and is selectable
                if (myObjectsUnderCursor.getAttributeCarrierFront() && !myObjectsUnderCursor.getAttributeCarrierFront()->getTagProperty().isDemandElement()) {
                    // change the selected attribute carrier if myNetworkViewOptions.mySelectEdges is enabled and clicked element is a getLaneFront()
                    if (myNetworkViewOptions.selectEdges() && (myObjectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag() == SUMO_TAG_LANE)) {
                        myObjectsUnderCursor.swapLane2Edge();
                    }
                    // Check if this GLobject type is locked
                    if (!myViewParent->getSelectorFrame()->getLockGLObjectTypes()->IsObjectTypeLocked(myObjectsUnderCursor.getGlTypeFront())) {
                        // toogle netElement selection
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
            break;
        case GNE_NMODE_CREATE_EDGE: {
            // make sure that Control key isn't pressed
            if (!myKeyPressed.controlKeyPressed()) {
                // process left click in create edge frame Frame
                myViewParent->getCreateEdgeFrame()->processClick(getPositionInformation(), myObjectsUnderCursor,
                        myNetworkViewOptions.menuCheckAutoOppositeEdge->getCheck() == TRUE,
                        myNetworkViewOptions.menuCheckChainEdges->getCheck() == TRUE);
            }
            // process click
            processClick(eventData);
            break;
        }
        case GNE_NMODE_MOVE: {
            // allways swap lane to edges in movement mode
            if (myObjectsUnderCursor.getLaneFront()) {
                myObjectsUnderCursor.swapLane2Edge();
            }
            // check that AC under cursor isn't a demand element
            if (myObjectsUnderCursor.getAttributeCarrierFront() && !myObjectsUnderCursor.getAttributeCarrierFront()->getTagProperty().isDemandElement()) {
                // check if we're moving a set of selected items
                if (myObjectsUnderCursor.getAttributeCarrierFront()->isAttributeCarrierSelected()) {
                    // move selected ACs
                    myMoveMultipleElementValues.beginMoveSelection(myObjectsUnderCursor.getAttributeCarrierFront());
                    // update view
                    update();
                } else if (!myMoveSingleElementValues.beginMoveSingleElementNetworkMode()) {
                    // process click  if there isn't movable elements (to move camera using drag an drop)
                    processClick(eventData);
                }
            } else {
                // process click  if there isn't movable elements (to move camera using drag an drop)
                processClick(eventData);
            }
            break;
        }
        case GNE_NMODE_CONNECT: {
            if (myObjectsUnderCursor.getLaneFront()) {
                // Handle laneclick (shift key may pass connections, Control key allow conflicts)
                myViewParent->getConnectorFrame()->handleLaneClick(myObjectsUnderCursor);
                update();
            }
            // process click
            processClick(eventData);
            break;
        }
        case GNE_NMODE_TLS: {
            if (myObjectsUnderCursor.getJunctionFront()) {
                myViewParent->getTLSEditorFrame()->editJunction(myObjectsUnderCursor.getJunctionFront());
                update();
            }
            // process click
            processClick(eventData);
            break;
        }
        case GNE_NMODE_ADDITIONAL: {
            // avoid create additionals if control key is pressed
            if (!myKeyPressed.controlKeyPressed()) {
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
                    update();
                }
            }
            // process click
            processClick(eventData);
            break;
        }
        case GNE_NMODE_CROSSING: {
            // swap lanes to edges in crossingsMode
            if (myObjectsUnderCursor.getLaneFront()) {
                myObjectsUnderCursor.swapLane2Edge();
            }
            // call function addCrossing from crossing frame
            myViewParent->getCrossingFrame()->addCrossing(myObjectsUnderCursor);
            // process click
            processClick(eventData);
            break;
        }
        case GNE_NMODE_TAZ: {
            // avoid create TAZs if control key is pressed
            if (!myKeyPressed.controlKeyPressed()) {
                // swap laness to edges in TAZ Mode
                if (myObjectsUnderCursor.getLaneFront()) {
                    myObjectsUnderCursor.swapLane2Edge();
                }
                // check if we want to create a rect for selecting edges
                if (myKeyPressed.shiftKeyPressed() && (myViewParent->getTAZFrame()->getTAZCurrentModul()->getTAZ() != nullptr)) {
                    // begin rectangle selection
                    mySelectingArea.beginRectangleSelection();
                } else {
                    // check if process click was scuesfully
                    if (myViewParent->getTAZFrame()->processClick(snapToActiveGrid(getPositionInformation()), myObjectsUnderCursor)) {
                        // view net must be always update
                        update();
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
        case GNE_NMODE_POLYGON: {
            // avoid create shapes if control key is pressed
            if (!myKeyPressed.controlKeyPressed()) {
                if (!myObjectsUnderCursor.getPOIFront()) {
                    GNEPolygonFrame::AddShapeResult result = myViewParent->getPolygonFrame()->processClick(snapToActiveGrid(getPositionInformation()), myObjectsUnderCursor);
                    // view net must be always update
                    update();
                    // process click depending of the result of "process click"
                    if ((result != GNEPolygonFrame::ADDSHAPE_UPDATEDTEMPORALSHAPE)) {
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
        case GNE_NMODE_PROHIBITION: {
            if (myObjectsUnderCursor.getConnectionFront()) {
                // shift key may pass connections, Control key allow conflicts.
                myViewParent->getProhibitionFrame()->handleProhibitionClick(myObjectsUnderCursor);
                update();
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
            if (myEditModes.networkEditMode == GNE_NMODE_SELECT) {
                mySelectingArea.processRectangleSelection();
            } else if (myEditModes.networkEditMode == GNE_NMODE_TAZ) {
                // process edge selection
                myViewParent->getTAZFrame()->processEdgeSelection(mySelectingArea.processEdgeRectangleSelection());
            }
        } else if (myKeyPressed.shiftKeyPressed()) {
            // obtain objects under cursor
            if (makeCurrent()) {
                // update objects under cursor again
                myObjectsUnderCursor.updateObjectUnderCursor(getGUIGlObjectsUnderCursor(), myEditShapes.editedShapePoly);
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
GNEViewNet::processMoveMouseNetwork() {
    // change "delete last created point" depending if during movement shift key is pressed
    if ((myEditModes.networkEditMode == GNE_NMODE_POLYGON) && myViewParent->getPolygonFrame()->getDrawingShapeModul()->isDrawing()) {
        myViewParent->getPolygonFrame()->getDrawingShapeModul()->setDeleteLastCreatedPoint(myKeyPressed.shiftKeyPressed());
    }
    // check what type of additional is moved
    if (myMoveMultipleElementValues.isMovingSelection()) {
        // move entire selection
        myMoveMultipleElementValues.moveSelection();
    } else if (mySelectingArea.selectingUsingRectangle) {
        // update selection corner of selecting area
        mySelectingArea.moveRectangleSelection();
    } else {
        // move single elements
        myMoveSingleElementValues.moveSingleElement();
    }
}


void
GNEViewNet::processLeftButtonPressDemand(void* eventData) {
    // decide what to do based on mode
    switch (myEditModes.demandEditMode) {
        case GNE_DMODE_INSPECT: {
            // process left click in Inspector Frame
            myViewParent->getInspectorFrame()->processDemandSupermodeClick(getPositionInformation(), myObjectsUnderCursor);
            // process click
            processClick(eventData);
            break;
        }
        case GNE_DMODE_DELETE: {
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
        case GNE_DMODE_SELECT:
            // check if a rect for selecting is being created
            if (myKeyPressed.shiftKeyPressed()) {
                // begin rectangle selection
                mySelectingArea.beginRectangleSelection();
            } else {
                // check if a rect for selecting is being created
                if (myKeyPressed.shiftKeyPressed()) {
                    // begin rectangle selection
                    mySelectingArea.beginRectangleSelection();
                } else {
                    // first check that under cursor there is an attribute carrier, is demand element and is selectable
                    if (myObjectsUnderCursor.getAttributeCarrierFront() && myObjectsUnderCursor.getAttributeCarrierFront()->getTagProperty().isDemandElement()) {
                        // Check if this GLobject type is locked
                        if (!myViewParent->getSelectorFrame()->getLockGLObjectTypes()->IsObjectTypeLocked(myObjectsUnderCursor.getGlTypeFront())) {
                            // toogle netElement selection
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
            }
            break;
        case GNE_DMODE_MOVE: {
            // check that AC under cursor is a demand element
            if (myObjectsUnderCursor.getAttributeCarrierFront() &&
                    myObjectsUnderCursor.getAttributeCarrierFront()->getTagProperty().isDemandElement()) {
                // check if we're moving a set of selected items
                if (myObjectsUnderCursor.getAttributeCarrierFront()->isAttributeCarrierSelected()) {
                    // move selected ACs
                    myMoveMultipleElementValues.beginMoveSelection(myObjectsUnderCursor.getAttributeCarrierFront());
                    // update view
                    update();
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
        case GNE_DMODE_ROUTE: {
            // check if we clicked over a lane and Control key isn't pressed
            if (myObjectsUnderCursor.getLaneFront() && !myKeyPressed.controlKeyPressed()) {
                // Handle edge click
                myViewParent->getRouteFrame()->handleEdgeClick(&myObjectsUnderCursor.getLaneFront()->getParentEdge());
            }
            // process click
            processClick(eventData);
            break;
        }
        case GNE_DMODE_VEHICLE: {
            // make sure that Control key isn't pressed
            if (!myKeyPressed.controlKeyPressed()) {
                // Handle click
                myViewParent->getVehicleFrame()->addVehicle(myObjectsUnderCursor);
            }
            // process click
            processClick(eventData);
            break;
        }
        case GNE_DMODE_STOP: {
            // make sure that Control key isn't pressed
            if (!myKeyPressed.controlKeyPressed()) {
                // Handle click
                myViewParent->getStopFrame()->addStop(myObjectsUnderCursor, myKeyPressed.shiftKeyPressed());
            }
            // process click
            processClick(eventData);
            break;
        }
        case GNE_DMODE_PERSON: {
            // make sure that Control key isn't pressed
            if (!myKeyPressed.controlKeyPressed()) {
                // Handle click
                myViewParent->getPersonFrame()->addPerson(myObjectsUnderCursor);
            }
            // process click
            processClick(eventData);
            break;
        }
        case GNE_DMODE_PERSONPLAN: {
            // make sure that Control key isn't pressed
            if (!myKeyPressed.controlKeyPressed()) {
                // Handle click
                myViewParent->getPersonPlanFrame()->addPersonPlan(myObjectsUnderCursor);
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
GNEViewNet::processMoveMouseDemand() {
    if (mySelectingArea.selectingUsingRectangle) {
        // update selection corner of selecting area
        mySelectingArea.moveRectangleSelection();
    } else {
        // move single elements
        myMoveSingleElementValues.moveSingleElement();
    }
}

/****************************************************************************/
