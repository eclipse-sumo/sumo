/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
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
#include <netedit/frames/GNEAdditionalFrame.h>
#include <netedit/frames/GNEConnectorFrame.h>
#include <netedit/frames/GNECrossingFrame.h>
#include <netedit/frames/GNEDeleteFrame.h>
#include <netedit/frames/GNEInspectorFrame.h>
#include <netedit/frames/GNEPolygonFrame.h>
#include <netedit/frames/GNEProhibitionFrame.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <netedit/frames/GNETAZFrame.h>
#include <netedit/frames/GNETLSEditorFrame.h>
#include <netedit/frames/GNECreateEdgeFrame.h>
#include <netedit/frames/GNERouteFrame.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/netelements/GNECrossing.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNELane.h>
#include <utils/gui/cursors/GUICursorSubSys.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIDanielPerspectiveChanger.h>
#include <utils/gui/windows/GUIDialog_ViewSettings.h>
#include <utils/gui/windows/GUIMainWindow.h>
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
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SETSUPERMODE_NETWORK,            GNEViewNet::onCmdSetSupermode),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SETSUPERMODE_DEMAND,             GNEViewNet::onCmdSetSupermode),
    // Shortcuts
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SHORTCUT_E,                      GNEViewNet::onCmdSetNetworkMode),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SHORTCUT_M,                      GNEViewNet::onCmdSetNetworkMode),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SHORTCUT_D,                      GNEViewNet::onCmdSetNetworkMode),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SHORTCUT_I,                      GNEViewNet::onCmdSetNetworkMode),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SHORTCUT_S,                      GNEViewNet::onCmdSetNetworkMode),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SHORTCUT_C,                      GNEViewNet::onCmdSetNetworkMode),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SHORTCUT_T,                      GNEViewNet::onCmdSetNetworkMode),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SHORTCUT_A,                      GNEViewNet::onCmdSetNetworkMode),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SHORTCUT_R,                      GNEViewNet::onCmdSetNetworkMode),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SHORTCUT_Z,                      GNEViewNet::onCmdSetNetworkMode),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SHORTCUT_P,                      GNEViewNet::onCmdSetNetworkMode),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SHORTCUT_W,                      GNEViewNet::onCmdSetNetworkMode),
    // Viewnet
    FXMAPFUNC(SEL_COMMAND, MID_GNE_VIEWNET_SHOW_CONNECTIONS,        GNEViewNet::onCmdToogleShowConnection),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_VIEWNET_SELECT_EDGES,            GNEViewNet::onCmdToogleSelectEdges),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_VIEWNET_SHOW_BUBBLES,            GNEViewNet::onCmdToogleShowBubbles),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_VIEWNET_MOVE_ELEVATION,          GNEViewNet::onCmdToogleMoveElevation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_VIEWNET_SHOW_GRID,               GNEViewNet::onCmdShowGrid),
    // select elements
    FXMAPFUNC(SEL_COMMAND, MID_ADDSELECT,                           GNEViewNet::onCmdAddSelected),
    FXMAPFUNC(SEL_COMMAND, MID_REMOVESELECT,                        GNEViewNet::onCmdRemoveSelected),
    // Junctions
    FXMAPFUNC(SEL_COMMAND, MID_GNE_JUNCTION_EDIT_SHAPE,             GNEViewNet::onCmdEditJunctionShape),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_JUNCTION_RESET_SHAPE,            GNEViewNet::onCmdResetJunctionShape),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_JUNCTION_REPLACE,                GNEViewNet::onCmdReplaceJunction),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_JUNCTION_SPLIT,                  GNEViewNet::onCmdSplitJunction),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_JUNCTION_CLEAR_CONNECTIONS,      GNEViewNet::onCmdClearConnections),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_JUNCTION_RESET_CONNECTIONS,      GNEViewNet::onCmdResetConnections),
    // Connections
    FXMAPFUNC(SEL_COMMAND, MID_GNE_CONNECTION_EDIT_SHAPE,           GNEViewNet::onCmdEditConnectionShape),
    // Crossings
    FXMAPFUNC(SEL_COMMAND, MID_GNE_CROSSING_EDIT_SHAPE,             GNEViewNet::onCmdEditCrossingShape),
    // Edges
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_SPLIT,                      GNEViewNet::onCmdSplitEdge),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_SPLIT_BIDI,                 GNEViewNet::onCmdSplitEdgeBidi),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_REVERSE,                    GNEViewNet::onCmdReverseEdge),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_ADD_REVERSE,                GNEViewNet::onCmdAddReversedEdge),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_EDIT_ENDPOINT,              GNEViewNet::onCmdEditEdgeEndpoint),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_RESET_ENDPOINT,             GNEViewNet::onCmdResetEdgeEndpoint),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_STRAIGHTEN,                 GNEViewNet::onCmdStraightenEdges),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_SMOOTH,                     GNEViewNet::onCmdSmoothEdges),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_STRAIGHTEN_ELEVATION,       GNEViewNet::onCmdStraightenEdgesElevation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_SMOOTH_ELEVATION,           GNEViewNet::onCmdSmoothEdgesElevation),
    // Lanes
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_DUPLICATE,                  GNEViewNet::onCmdDuplicateLane),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_RESET_CUSTOMSHAPE,          GNEViewNet::onCmdResetLaneCustomShape),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_TRANSFORM_SIDEWALK,         GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_TRANSFORM_BIKE,             GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_TRANSFORM_BUS,              GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_TRANSFORM_GREENVERGE,       GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_ADD_SIDEWALK,               GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_ADD_BIKE,                   GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_ADD_BUS,                    GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_ADD_GREENVERGE,             GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_REMOVE_SIDEWALK,            GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_REMOVE_BIKE,                GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_REMOVE_BUS,                 GNEViewNet::onCmdLaneOperation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_REMOVE_GREENVERGE,          GNEViewNet::onCmdLaneOperation),
    // Additionals
    FXMAPFUNC(SEL_COMMAND, MID_OPEN_ADDITIONAL_DIALOG,              GNEViewNet::onCmdOpenAdditionalDialog),
    // Polygons
    FXMAPFUNC(SEL_COMMAND, MID_GNE_POLYGON_SIMPLIFY_SHAPE,          GNEViewNet::onCmdSimplifyShape),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_POLYGON_CLOSE,                   GNEViewNet::onCmdClosePolygon),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_POLYGON_OPEN,                    GNEViewNet::onCmdOpenPolygon),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_POLYGON_SET_FIRST_POINT,         GNEViewNet::onCmdSetFirstGeometryPoint),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_POLYGON_DELETE_GEOMETRY_POINT,   GNEViewNet::onCmdDeleteGeometryPoint),
    // POIs
    FXMAPFUNC(SEL_COMMAND, MID_GNE_POI_TRANSFORM,                   GNEViewNet::onCmdTransformPOI),
};

// Object implementation
FXIMPLEMENT(GNEViewNet, GUISUMOAbstractView, GNEViewNetMap, ARRAYNUMBER(GNEViewNetMap))

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEViewNet::ObjectsUnderCursor - methods
// ---------------------------------------------------------------------------

GNEViewNet::ObjectsUnderCursor::ObjectsUnderCursor() {}


void
GNEViewNet::ObjectsUnderCursor::updateObjectUnderCursor(const std::vector<GUIGlObject*> &GUIGlObjects, GNEPoly* editedPolyShape) {
    // first clear all containers
    myAttributeCarriers.clear();
    myNetElements.clear();
    myAdditionals.clear();
    myShapes.clear();
    myJunctions.clear();
    myEdges.clear();
    myLanes.clear();
    myCrossings.clear();
    myConnections.clear();
    myTAZs.clear();
    myPOIs.clear();
    myPolys.clear();
    // set GUIGlObject
    sortGUIGlObjectsByAltitude(GUIGlObjects);
    // iterate over GUIGlObjects
    for (const auto &i : myGUIGlObjects) {
        // only continue if isn't GLO_NETELEMENT (0)
        if (i->getType() != GLO_NETELEMENT) {
            // cast attribute carrier from glObject
            myAttributeCarriers.push_back(dynamic_cast<GNEAttributeCarrier*>(i));
            // only continue if attributeCarrier isn't nullptr;
            if (myAttributeCarriers.back()) {
                // If we're editing a shape, ignore rest of elements (including other polygons)
                if (editedPolyShape != nullptr) {
                    if (myAttributeCarriers.back() == editedPolyShape) {
                        // cast Poly from attribute carrier
                        myPolys.push_back(dynamic_cast<GNEPoly*>(myAttributeCarriers.back()));
                    }
                } else {
                    // obtain tag property (only for improve code legibility)
                    const auto& tagValue = myAttributeCarriers.back()->getTagProperty();
                    // check if attributeCarrier can be casted into netElement, additional or shape
                    if (tagValue.isNetElement()) {
                        // cast netElement from attribute carrier
                        myNetElements.push_back(dynamic_cast<GNENetElement*>(myAttributeCarriers.back()));
                    } else if (tagValue.isAdditional()) {
                        // cast additional element from attribute carrier
                        myAdditionals.push_back(dynamic_cast<GNEAdditional*>(myAttributeCarriers.back()));
                    } else if (tagValue.isShape()) {
                        // cast shape element from attribute carrier
                        myShapes.push_back(dynamic_cast<GNEShape*>(myAttributeCarriers.back()));
                    } else if (tagValue.isTAZ()) {
                        // cast TAZ element from attribute carrier
                        myTAZs.push_back(dynamic_cast<GNETAZ*>(myAttributeCarriers.back()));
                    }
                    // now set specify AC type
                    switch (i->getType()) {
                        case GLO_JUNCTION:
                            myJunctions.push_back(dynamic_cast<GNEJunction*>(myAttributeCarriers.back()));
                            break;
                        case GLO_EDGE: {
                            // fisrt obtain Edge
                            GNEEdge *edge = dynamic_cast<GNEEdge*>(myAttributeCarriers.back());
                            // check if edge parent is already inserted in myEdges (for example, due clicking over Geometry Points)
                            if (std::find(myEdges.begin(), myEdges.end(), edge) == myEdges.end()) {
                                myEdges.push_back(edge);
                            }
                            break;
                        }
                        case GLO_LANE: {
                            myLanes.push_back(dynamic_cast<GNELane*>(myAttributeCarriers.back()));
                            // check if edge's lane parent is already inserted in myEdges (for example, due clicking over Geometry Points)
                            if (std::find(myEdges.begin(), myEdges.end(), &myLanes.back()->getParentEdge()) == myEdges.end()) {
                                myEdges.push_back(&myLanes.back()->getParentEdge());
                            }
                            break;
                        }
                        case GLO_CROSSING:
                            myCrossings.push_back(dynamic_cast<GNECrossing*>(myAttributeCarriers.back()));
                            break;
                        case GLO_CONNECTION:
                            myConnections.push_back(dynamic_cast<GNEConnection*>(myAttributeCarriers.back()));
                            break;
                        case GLO_POI:
                            myPOIs.push_back(dynamic_cast<GNEPOI*>(myAttributeCarriers.back()));
                            break;
                        case GLO_POLYGON:
                            myPolys.push_back(dynamic_cast<GNEPoly*>(myAttributeCarriers.back()));
                            break;
                        default:
                            break;
                    }
                }
            } else {
                myAttributeCarriers.pop_back();
            }
        }
    }
     // write information in debug mode
    WRITE_DEBUG("ObjectsUnderCursor: GUIGlObjects: " + toString(GUIGlObjects.size()) + 
                ", AttributeCarriers: " + toString(myAttributeCarriers.size()) + 
                ", NetElements: " + toString(myNetElements.size()) + 
                ", Additionals: " + toString(myAdditionals.size()) + 
                ", Shapes: " + toString(myShapes.size()) + 
                ", Junctions: " + toString(myJunctions.size()) + 
                ", Edges: " + toString(myEdges.size()) + 
                ", Lanes: " + toString(myLanes.size()) + 
                ", Crossings: " + toString(myCrossings.size()) + 
                ", Connections: " + toString(myConnections.size()) + 
                ", TAZs: " + toString(myTAZs.size()) + 
                ", POIs: " + toString(myPOIs.size()) + 
                ", Polys: " + toString(myPolys.size()));
}


void
GNEViewNet::ObjectsUnderCursor::swapLane2Edge() {
    // clear some containers
    myGUIGlObjects.clear();
    myAttributeCarriers.clear();
    myNetElements.clear();
    // fill containers using edges
    for (const auto &i : myEdges) {
        myGUIGlObjects.push_back(i);
        myAttributeCarriers.push_back(i);
        myNetElements.push_back(i);
    }
    // write information for debug
    WRITE_DEBUG("ObjectsUnderCursor: swapped Lanes to edges")
}


void 
GNEViewNet::ObjectsUnderCursor::setCreatedJunction(GNEJunction* junction) {
    if (myJunctions.size() > 0) {
        myJunctions.front() = junction;
    } else {
        myJunctions.push_back(junction);
    }
}


GUIGlID 
GNEViewNet::ObjectsUnderCursor::getGlIDFront() const {
    if (myGUIGlObjects.size() > 0) {
        return myGUIGlObjects.front()->getGlID();
    } else {
        return 0;
    }
}


GUIGlObjectType 
GNEViewNet::ObjectsUnderCursor::getGlTypeFront() const {
    if (myGUIGlObjects.size() > 0) {
        return myGUIGlObjects.front()->getType();
    } else {
        return GLO_NETWORK;
    }
}


GNEAttributeCarrier* 
GNEViewNet::ObjectsUnderCursor::getAttributeCarrierFront() const {
    if (myAttributeCarriers.size() > 0) {
        return myAttributeCarriers.front();
    } else {
        return nullptr;
    }
}


GNENetElement* 
GNEViewNet::ObjectsUnderCursor::getNetElementFront() const {
    if (myNetElements.size() > 0) {
        return myNetElements.front();
    } else {
        return nullptr;
    }
}


GNEAdditional* 
GNEViewNet::ObjectsUnderCursor::getAdditionalFront() const {
    if (myAdditionals.size() > 0) {
        return myAdditionals.front();
    } else {
        return nullptr;
    }
}


GNEShape* 
GNEViewNet::ObjectsUnderCursor::getShapeFront() const {
    if (myShapes.size() > 0) {
        return myShapes.front();
    } else {
        return nullptr;
    }
}


GNEJunction* 
GNEViewNet::ObjectsUnderCursor::getJunctionFront() const {
    if (myJunctions.size() > 0) {
        return myJunctions.front();
    } else {
        return nullptr;
    }
}


GNEEdge* 
GNEViewNet::ObjectsUnderCursor::getEdgeFront() const {
    if (myEdges.size() > 0) {
        return myEdges.front();
    } else {
        return nullptr;
    }
}


GNELane* 
GNEViewNet::ObjectsUnderCursor::getLaneFront() const {
    if (myLanes.size() > 0) {
        return myLanes.front();
    } else {
        return nullptr;
    }
}


GNECrossing* 
GNEViewNet::ObjectsUnderCursor::getCrossingFront() const {
    if (myCrossings.size() > 0) {
        return myCrossings.front();
    } else {
        return nullptr;
    }
}


GNEConnection* 
GNEViewNet::ObjectsUnderCursor::getConnectionFront() const {
    if (myConnections.size() > 0) {
        return myConnections.front();
    } else {
        return nullptr;
    }
}


GNETAZ* 
GNEViewNet::ObjectsUnderCursor::getTAZFront() const {
    if (myTAZs.size() > 0) {
        return myTAZs.front();
    } else {
        return nullptr;
    }
}


GNEPOI* 
GNEViewNet::ObjectsUnderCursor::getPOIFront() const {
    if (myPOIs.size() > 0) {
        return myPOIs.front();
    } else {
        return nullptr;
    }
}


GNEPoly* 
GNEViewNet::ObjectsUnderCursor::getPolyFront() const {
    if (myPolys.size() > 0) {
        return myPolys.front();
    } else {
        return nullptr;
    }
}


const std::vector<GNEAttributeCarrier*> &
GNEViewNet::ObjectsUnderCursor::getClickedAttributeCarriers() const {
    return myAttributeCarriers;
}


void
GNEViewNet::ObjectsUnderCursor::sortGUIGlObjectsByAltitude(const std::vector<GUIGlObject*> &GUIGlObjects) {
    // first clear myGUIGlObjects
    myGUIGlObjects.clear();
    // declare a map to save sorted GUIGlObjects
    std::map<GUIGlObjectType, std::vector<GUIGlObject*> > mySortedGUIGlObjects;
    for (const auto &i : GUIGlObjects) {
        mySortedGUIGlObjects[i->getType()].push_back(i);
    }
    // move sorted GUIGlObjects into myGUIGlObjects using a reverse iterator
    for (std::map<GUIGlObjectType, std::vector<GUIGlObject*> >::reverse_iterator i = mySortedGUIGlObjects.rbegin(); i != mySortedGUIGlObjects.rend(); i++) {
        for (const auto &j : i->second) {
            myGUIGlObjects.push_back(j);
        }
    }
}

// ---------------------------------------------------------------------------
// GNEViewNet::keyPressed - methods
// ---------------------------------------------------------------------------

GNEViewNet::KeyPressed::KeyPressed() :
    myEventInfo(nullptr) {
}


void
GNEViewNet::KeyPressed::update(void *eventData) {
    myEventInfo = (FXEvent*) eventData;
}


bool
GNEViewNet::KeyPressed::shiftKeyPressed() const {
    if (myEventInfo) {
        return (myEventInfo->state & SHIFTMASK) != 0;
    } else {
        return false;
    }
}


bool
GNEViewNet::KeyPressed::controlKeyPressed() const {
    if (myEventInfo) {
        return (myEventInfo->state & CONTROLMASK) != 0;
    } else {
        return false;
    }
}

// ---------------------------------------------------------------------------
// GNEViewNet - methods
// ---------------------------------------------------------------------------

GNEViewNet::GNEViewNet(FXComposite* tmpParent, FXComposite* actualParent, GUIMainWindow& app,
                       GNEViewParent* viewParent, GNENet* net, GNEUndoList* undoList,
                       FXGLVisual* glVis, FXGLCanvas* share, FXToolBar* toolBar) :
    GUISUMOAbstractView(tmpParent, app, viewParent, net->getVisualisationSpeedUp(), glVis, share),
    myViewParent(viewParent),
    myNet(net),
    myCurrentFrame(nullptr),
    myCreateEdgeOptions(this),
    myMoveSingleElementValues(this), 
    myMoveMultipleElementValues(this), 
    mySelectingArea(this),
    myTestingMode(this), 
    myViewOptions(this),
    mySuperModes(this),
    myNetworkCheckableButtons(this),
    myDemandCheckableButtons(this),
    myToolbar(toolBar),
    myUndoList(undoList),
    myEditShapePoly(nullptr) {
    // view must be the final member of actualParent
    reparent(actualParent);
    // Build Network edit mode control
    buildNetworkEditModeControls();
    // Build Demand edit mode control
    buildDemandEditModeControls();
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


GNEViewNet::~GNEViewNet() { }


void
GNEViewNet::doInit() {}


void
GNEViewNet::buildViewToolBars(GUIGlChildWindow& cw) {
    // build coloring tools
    {
        for (auto it_names : gSchemeStorage.getNames()) {
            cw.getColoringSchemesCombo().appendItem(it_names.c_str());
            if (it_names == myVisualizationSettings->name) {
                cw.getColoringSchemesCombo().setCurrentItem(cw.getColoringSchemesCombo().getNumItems() - 1);
            }
        }
        cw.getColoringSchemesCombo().setNumVisible(MAX2(5, (int)gSchemeStorage.getNames().size() + 1));
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


std::set<std::pair<std::string, GNEAttributeCarrier*> >
GNEViewNet::getAttributeCarriersInBoundary(const Boundary &boundary, bool forceSelectEdges) {
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
                // in the case of a Lane, we need to change the retrieved lane to their the parent if myViewOptions.mySelectEdges is enabled
                if ((retrievedAC->getTagProperty().getTag() == SUMO_TAG_LANE) && (myViewOptions.selectEdges() || forceSelectEdges)) {
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
        if(myViewParent->getInspectorFrame()->getOverlappedInspection()->overlappedInspectionShown() &&
            myViewParent->getInspectorFrame()->getOverlappedInspection()->checkSavedPosition(getPositionInformation()) &&
            myViewParent->getInspectorFrame()->getInspectedACs().size() > 0) {
            o = dynamic_cast<GUIGlObject*>(myViewParent->getInspectorFrame()->getInspectedACs().front());
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


void
GNEViewNet::buildColorRainbow(const GUIVisualizationSettings& s, GUIColorScheme& scheme, int active, GUIGlObjectType objectType) {
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
GNEViewNet::selectEdges() const {
    return myViewOptions.selectEdges();
}


bool 
GNEViewNet::editingElevation() const {
    if (myCreateEdgeOptions.moveElevation->shown()) {
        return (myCreateEdgeOptions.moveElevation->getCheck() == TRUE);
    } else {
        return false;
    }
}


bool
GNEViewNet::showConnections() {
    if (mySuperModes.networkEditMode == GNE_NMODE_CONNECT) {
        return myViewOptions.menuCheckHideConnections->getCheck() == 0;
    } else if (mySuperModes.networkEditMode == GNE_NMODE_PROHIBITION) {
        return true;
    } else if (myViewOptions.menuCheckShowConnections->shown() == false) {
        return false;
    } else {
        return (myVisualizationSettings->showLane2Lane);
    }
}


bool
GNEViewNet::autoSelectNodes() {
    return (myViewOptions.menuCheckExtendSelection->getCheck() != 0);
}


void
GNEViewNet::setSelectionScaling(double selectionScale) {
    myVisualizationSettings->selectionScale = selectionScale;
}


bool
GNEViewNet::changeAllPhases() const {
    return (myViewOptions.menuCheckChangeAllPhases->getCheck() != 0);
}


bool
GNEViewNet::showJunctionAsBubbles() const {
    return (mySuperModes.networkEditMode == GNE_NMODE_MOVE) && (myCreateEdgeOptions.showJunctionBubble->getCheck());
}


void
GNEViewNet::startEditCustomShape(GNENetElement* element, const PositionVector& shape, bool fill) {
    if ((myEditShapePoly == nullptr) && (element != nullptr) && (shape.size() > 1)) {
        // save current edit mode before starting
        myPreviousNetworkEditMode = mySuperModes.networkEditMode;
        setEditModeFromHotkey(MID_GNE_SHORTCUT_M);
        // add special GNEPoly fo edit shapes
        // color is taken from junction color settings
        RGBColor col = getVisualisationSettings()->junctionColorer.getSchemes()[0].getColor(3);
        myEditShapePoly = myNet->addPolygonForEditShapes(element, shape, fill, col);
        // update view net to show the new myEditShapePoly
        update();
    }
}


void
GNEViewNet::stopEditCustomShape() {
    // stop edit shape junction deleting myEditShapePoly
    if (myEditShapePoly != nullptr) {
        myNet->removePolygonForEditShapes(myEditShapePoly);
        myEditShapePoly = nullptr;
        // restore previous edit mode
        if (mySuperModes.networkEditMode != myPreviousNetworkEditMode) {
            setNetworkEditMode(myPreviousNetworkEditMode);
        }
    }
}


GNEViewNet::GNEViewNet() : 
    myCreateEdgeOptions(this),
    myMoveSingleElementValues(this),
    myMoveMultipleElementValues(this),
    mySelectingArea(this),
    myTestingMode(this), 
    myViewOptions(this),
    mySuperModes(this),
    myNetworkCheckableButtons(this),
    myDemandCheckableButtons(this) {
}


int
GNEViewNet::doPaintGL(int mode, const Boundary& bound) {
    // init view settings
    // (uncomment the next line to check select mode)
    // myVisualizationSettings->drawForSelecting = true;
    glRenderMode(mode);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    // visualize rectangular selection
    mySelectingArea.drawRectangleSelection(myVisualizationSettings->selectionColor);

    // compute lane width
    double lw = m2p(SUMO_const_laneWidth);
    // draw decals (if not in grabbing mode)
    if (!myUseToolTips && !myVisualizationSettings->drawForSelecting) {
        drawDecals();
        // depending of the visualizationSettings, enable or disable check box show grid
        if (myVisualizationSettings->showGrid) {
            myViewOptions.menuCheckShowGrid->setCheck(true);
            paintGLGrid();
        } else {
            myViewOptions.menuCheckShowGrid->setCheck(false);
        }
        myViewOptions.menuCheckShowConnections->setCheck(myVisualizationSettings->showLane2Lane);

    }
    // draw temporal elements
    if (!myVisualizationSettings->drawForSelecting) {
        drawTemporalDrawShape();
        drawLaneCandidates();
        // draw testing elements
        myTestingMode.drawTestingElements(myApp);
    }
    // draw elements
    glLineWidth(1);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    const float minB[2] = { (float)bound.xmin(), (float)bound.ymin() };
    const float maxB[2] = { (float)bound.xmax(), (float)bound.ymax() };
    myVisualizationSettings->scale = lw;
    glEnable(GL_POLYGON_OFFSET_FILL);
    glEnable(GL_POLYGON_OFFSET_LINE);
    myVisualizationSettings->editMode = mySuperModes.networkEditMode;
    int hits2 = myGrid->Search(minB, maxB, *myVisualizationSettings);

    glTranslated(0, 0, GLO_ADDITIONAL);
    for (auto i : myAdditionallyDrawn) {
        i.first->drawGLAdditional(this, *myVisualizationSettings);
    }

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
        myObjectsUnderCursor.updateObjectUnderCursor(getGUIGlObjectsUnderCursor(), myEditShapePoly);
        // decide what to do based on mode
        switch (mySuperModes.networkEditMode) {
            case GNE_NMODE_CREATE_EDGE: {
                // make sure that Control key isn't pressed
                if (!myKeyPressed.controlKeyPressed()) {
                    // process left click in create edge frame Frame
                    myViewParent->getCreateEdgeFrame()->processClick(getPositionInformation(), myObjectsUnderCursor, myCreateEdgeOptions.autoOppositeEdge->getCheck(), myCreateEdgeOptions.chainEdges->getCheck());
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
                // check if we're moving a set of selected items
                if (myObjectsUnderCursor.getAttributeCarrierFront() && myObjectsUnderCursor.getAttributeCarrierFront()->isAttributeCarrierSelected()) {
                    // move selected ACs
                    myMoveMultipleElementValues.beginMoveSelection(myObjectsUnderCursor.getAttributeCarrierFront());
                    // update view
                    update();
                } else if (!myMoveSingleElementValues.beginMoveSingleElement()) {
                    // process click  if there isn't movable elements (to move camera using drag an drop)
                    processClick(eventData);
                }
                break;
            }
            case GNE_NMODE_DELETE: {
                if (myObjectsUnderCursor.getAttributeCarrierFront()) {
                    // change the selected attribute carrier if myViewOptions.mySelectEdges is enabled and clicked element is a getLaneFront()
                    if (myViewOptions.selectEdges() && (myObjectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag() == SUMO_TAG_LANE) && !myKeyPressed.shiftKeyPressed()) {
                        myObjectsUnderCursor.swapLane2Edge();
                    }
                    // check if we are deleting a selection or an single attribute carrier
                    if (myObjectsUnderCursor.getAttributeCarrierFront()->isAttributeCarrierSelected()) {
                        // before delete al selected attribute carriers, check if we clicked over a geometry point
                        if (myViewParent->getDeleteFrame()->getDeleteOptions()->deleteOnlyGeometryPoints() &&
                                (((myObjectsUnderCursor.getEdgeFront()) && (myObjectsUnderCursor.getEdgeFront()->getVertexIndex(getPositionInformation(), false, false) != -1)) 
                              || ((myObjectsUnderCursor.getPolyFront()) && (myObjectsUnderCursor.getPolyFront()->getVertexIndex(getPositionInformation(), false, false) != -1)))) {
                            myViewParent->getDeleteFrame()->removeAttributeCarrier(myObjectsUnderCursor.getAttributeCarrierFront());
                        } else {
                            myViewParent->getDeleteFrame()->removeSelectedAttributeCarriers();
                        }
                    } else {
                        myViewParent->getDeleteFrame()->removeAttributeCarrier(myObjectsUnderCursor.getAttributeCarrierFront());
                    }
                } else {
                    // process click
                    processClick(eventData);
                }
                break;
            }
            case GNE_NMODE_INSPECT: {
                // process left click in Inspector Frame
                myViewParent->getInspectorFrame()->processClick(getPositionInformation(), myObjectsUnderCursor);
                // process click
                processClick(eventData);
                break;
            }
            case GNE_NMODE_SELECT:
                // check if a rect for selecting is being created
                if (myKeyPressed.shiftKeyPressed()) {
                    // begin rectangle selection
                    mySelectingArea.beginRectangleSelection();
                } else {
                    // first check that under cursor there is an attribute carrier and is selectable
                    if (myObjectsUnderCursor.getAttributeCarrierFront()) {
                        // change the selected attribute carrier if myViewOptions.mySelectEdges is enabled and clicked element is a getLaneFront()
                        if (myViewOptions.selectEdges() && (myObjectsUnderCursor.getAttributeCarrierFront()->getTagProperty().getTag() == SUMO_TAG_LANE)) {
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
                if(!myKeyPressed.controlKeyPressed()) {
                    if(myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->isShown()) {
                        // check if we need to start select lanes
                        if(myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->isSelectingLanes()) {
                            // select getLaneFront() to create an additional with consecutive lanes
                            myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->addSelectedLane(myObjectsUnderCursor.getLaneFront(), snapToActiveGrid(getPositionInformation()));
                        } else if (myObjectsUnderCursor.getLaneFront()) {
                            myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->startConsecutiveLaneSelector(myObjectsUnderCursor.getLaneFront(), snapToActiveGrid(getPositionInformation()));
                        }
                    } else {
                        // call function addAdditional from additional frame
                        myViewParent->getAdditionalFrame()->addAdditional(myObjectsUnderCursor);
                    }
                    update();
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
                if(!myKeyPressed.controlKeyPressed()) {
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
                        if(myViewParent->getTAZFrame()->processClick(snapToActiveGrid(getPositionInformation()), myObjectsUnderCursor)) {
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
                if(!myKeyPressed.controlKeyPressed()) {
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
        makeNonCurrent();
    }
    // update cursor
    updateCursor();
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
    // check moved items
    if (myMoveMultipleElementValues.isMovingSelection()) {
        myMoveMultipleElementValues.finishMoveSelection();
    } else if (mySelectingArea.selectingUsingRectangle) {
        // check if we're creating a rectangle selection or we want only to select a lane
        if(mySelectingArea.startDrawing) {
            // check if we're selecting all type of elements o we only want a set of edges for TAZ
            if(mySuperModes.networkEditMode == GNE_NMODE_SELECT) { 
                mySelectingArea.processRectangleSelection();
            } else if(mySuperModes.networkEditMode == GNE_NMODE_TAZ) {  
                // process edge selection
                myViewParent->getTAZFrame()->processEdgeSelection(mySelectingArea.processEdgeRectangleSelection());
            }
        } else if(myKeyPressed.shiftKeyPressed()) {
            // obtain objects under cursor
            if (makeCurrent()) {
                // update objects under cursor again
                myObjectsUnderCursor.updateObjectUnderCursor(getGUIGlObjectsUnderCursor(), myEditShapePoly);
                makeNonCurrent();
            }
            // check if there is a lane in objects under cursor
            if(myObjectsUnderCursor.getLaneFront()) {
                // if we clicked over an lane with shift key pressed, select or unselect it
                if(myObjectsUnderCursor.getLaneFront()->isAttributeCarrierSelected()) {
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
    update();
    return 1;
}


long 
GNEViewNet::onRightBtnPress(FXObject* obj, FXSelector sel, void* eventData) {
    // update keyPressed
    myKeyPressed.update(eventData);
    // update cursor
    updateCursor();
    if ((mySuperModes.networkEditMode == GNE_NMODE_POLYGON) && myViewParent->getPolygonFrame()->getDrawingShapeModul()->isDrawing()) {
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
    if ((mySuperModes.networkEditMode == GNE_NMODE_POLYGON) && myViewParent->getPolygonFrame()->getDrawingShapeModul()->isDrawing()) {
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
    // change "delete last created point" depending if during movement shift key is pressed
    if ((mySuperModes.networkEditMode == GNE_NMODE_POLYGON) && myViewParent->getPolygonFrame()->getDrawingShapeModul()->isDrawing()) {
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
    // update view
    update();
    return 1;
}


long
GNEViewNet::onKeyPress(FXObject* o, FXSelector sel, void* eventData) {
    // update keyPressed
    myKeyPressed.update(eventData);
    // update cursor
    updateCursor();
    // change "delete last created point" depending of shift key
    if ((mySuperModes.networkEditMode == GNE_NMODE_POLYGON) && myViewParent->getPolygonFrame()->getDrawingShapeModul()->isDrawing()) {
        myViewParent->getPolygonFrame()->getDrawingShapeModul()->setDeleteLastCreatedPoint(myKeyPressed.shiftKeyPressed());
    } else if ((mySuperModes.networkEditMode == GNE_NMODE_TAZ) && myViewParent->getTAZFrame()->getDrawingShapeModul()->isDrawing()) {
        myViewParent->getTAZFrame()->getDrawingShapeModul()->setDeleteLastCreatedPoint(myKeyPressed.shiftKeyPressed());
    }
    update();
    return GUISUMOAbstractView::onKeyPress(o, sel, eventData);
}


long
GNEViewNet::onKeyRelease(FXObject* o, FXSelector sel, void* eventData) {
    // update keyPressed
    myKeyPressed.update(eventData);
    // update cursor
    updateCursor();
    // change "delete last created point" depending of shift key
    if ((mySuperModes.networkEditMode == GNE_NMODE_POLYGON) && myViewParent->getPolygonFrame()->getDrawingShapeModul()->isDrawing()) {
        myViewParent->getPolygonFrame()->getDrawingShapeModul()->setDeleteLastCreatedPoint(myKeyPressed.shiftKeyPressed());
    }
    // check if selecting using rectangle has to be disabled
    if (mySelectingArea.selectingUsingRectangle && !myKeyPressed.shiftKeyPressed()) {
        mySelectingArea.selectingUsingRectangle = false;
    }
    update();
    return GUISUMOAbstractView::onKeyRelease(o, sel, eventData);
}


void
GNEViewNet::abortOperation(bool clearSelection) {
    // steal focus from any text fields and place it over view net
    setFocus();
    // abort operation depending of current mode
    if (mySuperModes.networkEditMode == GNE_NMODE_CREATE_EDGE) {
        // abort edge creation in create edge frame
        myViewParent->getCreateEdgeFrame()->abortEdgeCreation();
    } else if (mySuperModes.networkEditMode == GNE_NMODE_SELECT) {
        mySelectingArea.selectingUsingRectangle = false;
        // check if current selection has to be cleaned
        if (clearSelection) {
            myViewParent->getSelectorFrame()->clearCurrentSelection();
        }
    } else if (mySuperModes.networkEditMode == GNE_NMODE_CONNECT) {
        // abort changes in Connector Frame
        myViewParent->getConnectorFrame()->getConnectionModifications()->onCmdCancelModifications(0, 0, 0);
    } else if (mySuperModes.networkEditMode == GNE_NMODE_TLS) {
        myViewParent->getTLSEditorFrame()->onCmdCancel(nullptr, 0, nullptr);
    } else if (mySuperModes.networkEditMode == GNE_NMODE_MOVE) {
        stopEditCustomShape();
    } else if (mySuperModes.networkEditMode == GNE_NMODE_POLYGON) {
        // abort current drawing
        myViewParent->getPolygonFrame()->getDrawingShapeModul()->abortDrawing();
    } else if (mySuperModes.networkEditMode == GNE_NMODE_TAZ) {
        if(myViewParent->getTAZFrame()->getDrawingShapeModul()->isDrawing()) {
            // abort current drawing
            myViewParent->getPolygonFrame()->getDrawingShapeModul()->abortDrawing();
        } else if (myViewParent->getTAZFrame()->getTAZCurrentModul()->getTAZ() != nullptr) {
            // finish current editing TAZ
            myViewParent->getTAZFrame()->getTAZCurrentModul()->setTAZ(nullptr);
        }
    } else if (mySuperModes.networkEditMode == GNE_NMODE_PROHIBITION) {
        myViewParent->getProhibitionFrame()->onCmdCancel(nullptr, 0, nullptr);
    } else if (mySuperModes.networkEditMode == GNE_NMODE_ADDITIONAL) {
        // abort select lanes
        myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->abortConsecutiveLaneSelector();
    }
    myUndoList->p_abort();
}


void
GNEViewNet::hotkeyDel() {
    if (mySuperModes.networkEditMode == GNE_NMODE_CONNECT || mySuperModes.networkEditMode == GNE_NMODE_TLS) {
        setStatusBarText("Cannot delete in this mode");
    } else {
        myUndoList->p_begin("delete selection");
        deleteSelectedConnections();
        deleteSelectedCrossings();
        deleteSelectedAdditionals();
        deleteSelectedLanes();
        deleteSelectedEdges();
        deleteSelectedJunctions();
        deleteSelectedShapes();
        myUndoList->p_end();
    }
}


void
GNEViewNet::hotkeyEnter() {
    if (mySuperModes.networkEditMode == GNE_NMODE_CONNECT) {
        // Accept changes in Connector Frame
        myViewParent->getConnectorFrame()->getConnectionModifications()->onCmdSaveModifications(0, 0, 0);
    } else if (mySuperModes.networkEditMode == GNE_NMODE_TLS) {
        myViewParent->getTLSEditorFrame()->onCmdOK(nullptr, 0, nullptr);
    } else if ((mySuperModes.networkEditMode == GNE_NMODE_MOVE) && (myEditShapePoly != nullptr)) {
        // save edited junction's shape
        if (myEditShapePoly != nullptr) {
            myUndoList->p_begin("custom " + myEditShapePoly->getShapeEditedElement()->getTagStr() + " shape");
            SumoXMLAttr attr = SUMO_ATTR_SHAPE;
            if (myEditShapePoly->getShapeEditedElement()->getTagProperty().hasAttribute(SUMO_ATTR_CUSTOMSHAPE)) {
                attr = SUMO_ATTR_CUSTOMSHAPE;
            }
            myEditShapePoly->getShapeEditedElement()->setAttribute(attr, toString(myEditShapePoly->getShape()), myUndoList);
            myUndoList->p_end();
            stopEditCustomShape();
            update();
        }
    } else if (mySuperModes.networkEditMode == GNE_NMODE_POLYGON) {
        if (myViewParent->getPolygonFrame()->getDrawingShapeModul()->isDrawing()) {
            // stop current drawing
            myViewParent->getPolygonFrame()->getDrawingShapeModul()->stopDrawing();
        } else {
            // start drawing
            myViewParent->getPolygonFrame()->getDrawingShapeModul()->startDrawing();
        }
    } else if (mySuperModes.networkEditMode == GNE_NMODE_CROSSING) {
        myViewParent->getCrossingFrame()->createCrossingHotkey();
    } else if (mySuperModes.networkEditMode == GNE_NMODE_TAZ) {
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
    } else if (mySuperModes.networkEditMode == GNE_NMODE_ADDITIONAL) {
        if (myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->isSelectingLanes()) {
            // stop select lanes to create additional
            myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->stopConsecutiveLaneSelector();
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


void
GNEViewNet::setEditModeFromHotkey(FXushort selid) {
    // first check what supermode is being edited
    if (mySuperModes.currentSupermode == GNE_SUPERMODE_NETWORK) {
        switch (selid) {
            case MID_GNE_SHORTCUT_E:
                setNetworkEditMode(GNE_NMODE_CREATE_EDGE);
                break;
            case MID_GNE_SHORTCUT_M:
                setNetworkEditMode(GNE_NMODE_MOVE);
                break;
            case MID_GNE_SHORTCUT_D:
                setNetworkEditMode(GNE_NMODE_DELETE);
                break;
            case MID_GNE_SHORTCUT_I:
                setNetworkEditMode(GNE_NMODE_INSPECT);
                break;
            case MID_GNE_SHORTCUT_S:
                setNetworkEditMode(GNE_NMODE_SELECT);
                break;
            case MID_GNE_SHORTCUT_C:
                setNetworkEditMode(GNE_NMODE_CONNECT);
                break;
            case MID_GNE_SHORTCUT_T:
                setNetworkEditMode(GNE_NMODE_TLS);
                break;
            case MID_GNE_SHORTCUT_A:
                setNetworkEditMode(GNE_NMODE_ADDITIONAL);
                break;
            case MID_GNE_SHORTCUT_R:
                setNetworkEditMode(GNE_NMODE_CROSSING);
                break;
            case MID_GNE_SHORTCUT_Z:
                setNetworkEditMode(GNE_NMODE_TAZ);
                break;
            case MID_GNE_SHORTCUT_P:
                setNetworkEditMode(GNE_NMODE_POLYGON);
                break;
            case MID_GNE_SHORTCUT_W:
                setNetworkEditMode(GNE_NMODE_PROHIBITION);
                break;
            default:
                // Shortcut wasn't assigned in this Supermode
                break;
        }
    } else if (mySuperModes.currentSupermode == GNE_SUPERMODE_DEMAND) {
        switch (selid) {
            case MID_GNE_SHORTCUT_R:
                setDemandEditMode(GNE_DMODE_ROUTES);
                break;
            default:
                // Shortcut wasn't assigned in this Supermode
                break;
        }
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


NetworkEditMode
GNEViewNet::getCurrentNetworkEditMode() const {
    return mySuperModes.networkEditMode;
}


DemandEditMode
GNEViewNet::getCurrentDemandEditMode() const {
    return mySuperModes.demandEditMode;
}


const GNEViewNet::KeyPressed &
GNEViewNet::getKeyPressed() const {
    return myKeyPressed;
}


FXMenuCheck*
GNEViewNet::getMenuCheckShowGrid() const {
    return myViewOptions.menuCheckShowGrid;
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
    return (mySuperModes.networkEditMode == GNE_NMODE_MOVE || mySuperModes.networkEditMode == GNE_NMODE_INSPECT || mySuperModes.networkEditMode == GNE_NMODE_ADDITIONAL);
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
        case MID_GNE_SETSUPERMODE_NETWORK:
            setSupermode(GNE_SUPERMODE_NETWORK);
            break;
        case MID_GNE_SETSUPERMODE_DEMAND:
            setSupermode(GNE_SUPERMODE_DEMAND);
            break;
        default:
            break;
    }
    return 1;
}

long 
GNEViewNet::onCmdSetNetworkMode(FXObject*, FXSelector sel, void*) {
    // check what network mode will be set
    switch (FXSELID(sel)) {
        case MID_GNE_SHORTCUT_E:
            setNetworkEditMode(GNE_NMODE_CREATE_EDGE);
            break;
        case MID_GNE_SHORTCUT_M:
            setNetworkEditMode(GNE_NMODE_MOVE);
            break;
        case MID_GNE_SHORTCUT_D:
            setNetworkEditMode(GNE_NMODE_DELETE);
            break;
        case MID_GNE_SHORTCUT_I:
            setNetworkEditMode(GNE_NMODE_INSPECT);
            break;
        case MID_GNE_SHORTCUT_S:
            setNetworkEditMode(GNE_NMODE_SELECT);
            break;
        case MID_GNE_SHORTCUT_C:
            setNetworkEditMode(GNE_NMODE_CONNECT);
            break;
        case MID_GNE_SHORTCUT_T:
            setNetworkEditMode(GNE_NMODE_TLS);
            break;
        case MID_GNE_SHORTCUT_A:
            setNetworkEditMode(GNE_NMODE_ADDITIONAL);
            break;
        case MID_GNE_SHORTCUT_R:
            setNetworkEditMode(GNE_NMODE_CROSSING);
            break;
        case MID_GNE_SHORTCUT_Z:
            setNetworkEditMode(GNE_NMODE_TAZ);
            break;
        case MID_GNE_SHORTCUT_P:
            setNetworkEditMode(GNE_NMODE_POLYGON);
            break;
        case MID_GNE_SHORTCUT_W:
            setNetworkEditMode(GNE_NMODE_PROHIBITION);
            break;
        default:
            break;
    }
    return 1;
}


long
GNEViewNet::onCmdSetDemandMode(FXObject*, FXSelector sel, void*) {
    // check what demand mode will be set
    switch (FXSELID(sel)) {
        case MID_GNE_SHORTCUT_R:
            setDemandEditMode(GNE_DMODE_ROUTES);
            break;
        default:
            break;
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
    if (myEditShapePoly != nullptr) {
        myEditShapePoly->simplifyShape(false);
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
    if (myEditShapePoly != nullptr) {
        myEditShapePoly->deleteGeometryPoint(getPopupPosition(), false);
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
    if (myEditShapePoly != nullptr) {
        myEditShapePoly->closePolygon(false);
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
    if (myEditShapePoly != nullptr) {
        myEditShapePoly->openPolygon(false);
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
    if (myEditShapePoly != nullptr) {
        myEditShapePoly->changeFirstGeometryPoint(myEditShapePoly->getVertexIndex(getPopupPosition(), false, false), false);
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
                double minorPosOverLane = nearestLane->getShape().nearest_offset_to_point2D(POI->getPositionInView());
                double minorLateralOffset = nearestLane->getShape().positionAtOffset(minorPosOverLane).distanceTo(POI->getPositionInView());
                for (auto i : lanes) {
                    double posOverLane = i->getShape().nearest_offset_to_point2D(POI->getPositionInView());
                    double lateralOffset = i->getShape().positionAtOffset(posOverLane).distanceTo(POI->getPositionInView());
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
                // add restricted lane
                myNet->addRestrictedLane(vclass, *it, 0, myUndoList);
            }
            // end undo operation
            myUndoList->p_end();
        } else {
            // If only have a single lane, start undo/redo operation
            myUndoList->p_begin("Add vclass for " + toString(vclass));
            // Add restricted lane
            if (lane->getIndex() > 0) {
                myNet->addRestrictedLane(vclass, lane->getParentEdge(), lane->getIndex(), myUndoList);
            } else {
                myNet->addRestrictedLane(vclass, lane->getParentEdge(), 0, myUndoList);
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
    // update view
    update();
}


void 
GNEViewNet::updateCursor() {
    // update cursor if control key is pressed
    if(myKeyPressed.controlKeyPressed() && ((mySuperModes.networkEditMode == GNE_NMODE_ADDITIONAL) || (mySuperModes.networkEditMode == GNE_NMODE_POLYGON) || (mySuperModes.networkEditMode == GNE_NMODE_TAZ))) {
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
        startEditCustomShape(junction, nodeShape, true);
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
        myNet->splitJunction(junction, myUndoList);
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
        startEditCustomShape(connection, connection->getShape(), false);
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
        startEditCustomShape(crossing, shape, false);
    }
    // destroy pop-up and update view Net
    destroyPopup();
    setFocus();
    return 1;
}


long
GNEViewNet::onCmdToogleShowConnection(FXObject*, FXSelector, void*) {
    // if show was enabled, init GNEConnections
    if (myViewOptions.showConnections()) {
        getNet()->initGNEConnections();
    }
    // change flag "showLane2Lane" in myVisualizationSettings
    myVisualizationSettings->showLane2Lane = myViewOptions.showConnections();
    // Hide/show connections requiere recompute
    getNet()->requireRecompute();
    // Update viewnNet to show/hide conections
    update();
    return 1;
}


long
GNEViewNet::onCmdToogleSelectEdges(FXObject*, FXSelector, void*) {
    // nothing to do
    return 1;
}


long
GNEViewNet::onCmdToogleShowBubbles(FXObject*, FXSelector, void*) {
    // Only update view
    update();
    return 1;
}


long
GNEViewNet::onCmdToogleMoveElevation(FXObject*, FXSelector, void*) {
    // Only update view
    update();
    return 1;
}


long
GNEViewNet::onCmdAddSelected(FXObject*, FXSelector, void*) {
    if (makeCurrent()) {
        int id = getObjectAtPosition(getPopupPosition());
        GNEAttributeCarrier* ACToselect = dynamic_cast <GNEAttributeCarrier*>(GUIGlObjectStorage::gIDStorage.getObjectBlocking(id));
        GUIGlObjectStorage::gIDStorage.unblockObject(id);
        if (ACToselect && !ACToselect->isAttributeCarrierSelected()) {
            ACToselect->selectAttributeCarrier();
        }
    }
    return 1;
}


long
GNEViewNet::onCmdRemoveSelected(FXObject*, FXSelector, void*) {
    if (makeCurrent()) {
        int id = getObjectAtPosition(getPopupPosition());
        GNEAttributeCarrier* ACToselect = dynamic_cast <GNEAttributeCarrier*>(GUIGlObjectStorage::gIDStorage.getObjectBlocking(id));
        GUIGlObjectStorage::gIDStorage.unblockObject(id);
        if (ACToselect && ACToselect->isAttributeCarrierSelected()) {
            ACToselect->unselectAttributeCarrier();
        }
    }
    return 1;
}


long
GNEViewNet::onCmdShowGrid(FXObject*, FXSelector, void*) {
    // show or hidde grid depending of myViewOptions.menuCheckShowGrid
    if (myViewOptions.menuCheckShowGrid->getCheck()) {
        myVisualizationSettings->showGrid = true;
    } else {
        myVisualizationSettings->showGrid = false;
    }
    update();
    return 1;
}


// ===========================================================================
// private
// ===========================================================================

void 
GNEViewNet::setSupermode(Supermode supermode) {
    if (supermode == mySuperModes.currentSupermode) {
        setStatusBarText("Mode already selected");
        if (myCurrentFrame != nullptr) {
            myCurrentFrame->focusUpperElement();
        }
    } else {
        setStatusBarText("");
        // abort current operation
        abortOperation(false);
        // set super mode
        mySuperModes.currentSupermode = supermode;
        // set supermodes
        if (supermode == GNE_SUPERMODE_NETWORK) {
            // change buttons
            mySuperModes.networkButton->setChecked(true);
            mySuperModes.demandButton->setChecked(false);
            // show network buttons
            myNetworkCheckableButtons.showNetworkCheckableButtons();
            // hide demand buttons
            myDemandCheckableButtons.hideDemandCheckableButtons();
            // force update network mode
            setNetworkEditMode(mySuperModes.networkEditMode, true);
        } else if (supermode == GNE_SUPERMODE_DEMAND) {
            // for demand recompute network
            myNet->computeEverything((GNEApplicationWindow*)myApp);
            // change buttons
            mySuperModes.networkButton->setChecked(false);
            mySuperModes.demandButton->setChecked(true);
            // hide network buttons
            myNetworkCheckableButtons.hideNetworkCheckableButtons();
            // show demand buttons
            myDemandCheckableButtons.showDemandCheckableButtons();
            // force update demand mode
            setDemandEditMode(mySuperModes.demandEditMode, true);
        }
        // update buttons
        mySuperModes.networkButton->update();
        mySuperModes.demandButton->update();
        // update CommandButtons in GNEAppWindows
        myViewParent->getGNEAppWindows()->updateSuperModeMenuCommands(mySuperModes.currentSupermode);
    }
}


void
GNEViewNet::setNetworkEditMode(NetworkEditMode mode, bool force) {
    if ((mode == mySuperModes.networkEditMode) && !force) {
        setStatusBarText("Network mode already selected");
        if (myCurrentFrame != nullptr) {
            myCurrentFrame->focusUpperElement();
        }
    } else if (mySuperModes.networkEditMode == GNE_NMODE_TLS && !myViewParent->getTLSEditorFrame()->isTLSSaved()) {
        setStatusBarText("save modifications in TLS before change mode");
        myCurrentFrame->focusUpperElement();
    } else {
        setStatusBarText("");
        abortOperation(false);
        // stop editing of custom shapes
        stopEditCustomShape();
        // set edit mode
        mySuperModes.networkEditMode = mode;
        switch (mode) {
            case GNE_NMODE_CONNECT:
            case GNE_NMODE_PROHIBITION:
            case GNE_NMODE_TLS:
                // modes which depend on computed data
                myNet->computeEverything((GNEApplicationWindow*)myApp);
                break;
            default:
                break;
        }
        updateNetworkModeSpecificControls();
    }
}


void
GNEViewNet::setDemandEditMode(DemandEditMode mode, bool force) {
    if ((mode == mySuperModes.demandEditMode) && !force) {
        setStatusBarText("Demand mode already selected");
        if (myCurrentFrame != nullptr) {
            myCurrentFrame->focusUpperElement();
        }
    } else {
        /** **/
        updateDemandModeSpecificControls();
    }
}


void
GNEViewNet::buildNetworkEditModeControls() {
    // first build supermode buttons
    mySuperModes.buildSuperModeButtons();

    // build menu checks for Network checkable buttons
    myNetworkCheckableButtons.buildNetworkCheckableButtons();

    // build menu checks for Demand checkable buttons
    myDemandCheckableButtons.buildDemandCheckableButtons();

    // @ToDo add here new FXToolBarGrip(myNavigationToolBar, nullptr, 0, GUIDesignToolbarGrip);

    // initialize mode specific controls

    // build menu checks of create edges options
    myCreateEdgeOptions.buildCreateEdgeOptionMenuChecks();

    // build menu checks of view options
    myViewOptions.buildViewOptionsMenuChecks();
}


void
GNEViewNet::updateNetworkModeSpecificControls() {
    // hide grid
    myViewOptions.menuCheckShowGrid->setCheck(myVisualizationSettings->showGrid);
    // hide all checkbox of create edge
    myCreateEdgeOptions.hideCreateEdgeOptionMenuChecks();
    // hide all checkbox of view options
    myViewOptions.hideViewOptionsMenuChecks();
    // disable all network edit modes
    myNetworkCheckableButtons.disableNetworkCheckableButtons();
    // hide all frames
    myViewParent->hideAllFrames();
    // enable selected controls
    switch (mySuperModes.networkEditMode) {
        case GNE_NMODE_CREATE_EDGE:
            myCreateEdgeOptions.chainEdges->show();
            myCreateEdgeOptions.autoOppositeEdge->show();
            myNetworkCheckableButtons.createEdgeButton->setChecked(true);
            myViewOptions.menuCheckShowGrid->show();
            break;
        case GNE_NMODE_MOVE:
            myCreateEdgeOptions.warnAboutMerge->show();
            myCreateEdgeOptions.showJunctionBubble->show();
            myCreateEdgeOptions.moveElevation->show();
            myNetworkCheckableButtons.moveButton->setChecked(true);
            myViewOptions.menuCheckShowGrid->show();
            break;
        case GNE_NMODE_DELETE:
            myViewParent->getDeleteFrame()->show();
            myViewParent->getDeleteFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getDeleteFrame();
            myViewOptions.menuCheckShowConnections->show();
            myViewOptions.menuCheckSelectEdges->show();
            myNetworkCheckableButtons.deleteButton->setChecked(true);
            break;
        case GNE_NMODE_INSPECT:
            myViewParent->getInspectorFrame()->show();
            myViewParent->getInspectorFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getInspectorFrame();
            myViewOptions.menuCheckSelectEdges->show();
            myViewOptions.menuCheckShowConnections->show();
            myNetworkCheckableButtons.inspectButton->setChecked(true);
            break;
        case GNE_NMODE_SELECT:
            myViewParent->getSelectorFrame()->show();
            myViewParent->getSelectorFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getSelectorFrame();
            myViewOptions.menuCheckSelectEdges->show();
            myViewOptions.menuCheckShowConnections->show();
            myViewOptions.menuCheckExtendSelection->show();
            myNetworkCheckableButtons.selectButton->setChecked(true);
            break;
        case GNE_NMODE_CONNECT:
            myViewParent->getConnectorFrame()->show();
            myViewParent->getConnectorFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getConnectorFrame();
            myViewOptions.menuCheckHideConnections->show();
            myNetworkCheckableButtons.connectionButton->setChecked(true);
            break;
        case GNE_NMODE_TLS:
            myViewParent->getTLSEditorFrame()->show();
            myViewParent->getTLSEditorFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getTLSEditorFrame();
            myViewOptions.menuCheckChangeAllPhases->show();
            myNetworkCheckableButtons.trafficLightButton->setChecked(true);
            break;
        case GNE_NMODE_ADDITIONAL:
            myViewParent->getAdditionalFrame()->show();
            myViewParent->getAdditionalFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getAdditionalFrame();
            myNetworkCheckableButtons.additionalButton->setChecked(true);
            myViewOptions.menuCheckShowGrid->show();
            break;
        case GNE_NMODE_CROSSING:
            myViewParent->getCrossingFrame()->show();
            myViewParent->getCrossingFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getCrossingFrame();
            myNetworkCheckableButtons.crossingButton->setChecked(true);
            myViewOptions.menuCheckShowGrid->setCheck(false);
            break;
        case GNE_NMODE_TAZ:
            myViewParent->getTAZFrame()->show();
            myViewParent->getTAZFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getTAZFrame();
            myNetworkCheckableButtons.TAZButton->setChecked(true);
            myViewOptions.menuCheckShowGrid->setCheck(false);
            break;
        case GNE_NMODE_POLYGON:
            myViewParent->getPolygonFrame()->show();
            myViewParent->getPolygonFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getPolygonFrame();
            myNetworkCheckableButtons.shapeButton->setChecked(true);
            myViewOptions.menuCheckShowGrid->show();
            break;
        case GNE_NMODE_PROHIBITION:
            myViewParent->getProhibitionFrame()->show();
            myViewParent->getProhibitionFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getProhibitionFrame();
            myNetworkCheckableButtons.prohibitionButton->setChecked(true);
            break;
        default:
            break;
    }
    // Update Network buttons
    myNetworkCheckableButtons.updateNetworkCheckableButtons();
    // force repaint because different modes draw different things
    myToolbar->recalc();
    onPaint(nullptr, 0, nullptr);
    update();
}



void
GNEViewNet::buildDemandEditModeControls() {
    // initialize mode specific controls
}


void
GNEViewNet::updateDemandModeSpecificControls() {
    // hide grid
    myViewOptions.menuCheckShowGrid->setCheck(myVisualizationSettings->showGrid);
    // hide all checkbox of create edge
    myCreateEdgeOptions.hideCreateEdgeOptionMenuChecks();
    // hide all checkbox of view options
    myViewOptions.hideViewOptionsMenuChecks();
    // disable all Demand edit modes
    myDemandCheckableButtons.disableDemandCheckableButtons();
    // hide all frames
    myViewParent->hideAllFrames();
    // enable selected controls
    switch (mySuperModes.demandEditMode) {
        case GNE_DMODE_ROUTES:
            myViewParent->getRouteFrame()->show();
            myViewParent->getRouteFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getRouteFrame();
            myDemandCheckableButtons.routeButton->setChecked(true);
            break;
        default:
            break;
    }
    // Update Demand buttons
    myDemandCheckableButtons.updateDemandCheckableButtons();
    // force repaint because different modes draw different things
    myToolbar->recalc();
    onPaint(nullptr, 0, nullptr);
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
                getViewParent()->getAdditionalFrame()->removeAdditional(i);
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
        if (myCreateEdgeOptions.warnAboutMerge->getCheck()) {
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
    switch (mySuperModes.networkEditMode) {
        case GNE_NMODE_INSPECT:
            myViewParent->getInspectorFrame()->update();
            break;
        default:
            break;
    }
}

// ---------------------------------------------------------------------------
// GNEViewNet::MoveSingleElementValues - methods
// ---------------------------------------------------------------------------

GNEViewNet::MoveSingleElementValues::MoveSingleElementValues(GNEViewNet* viewNet) :
    movingIndexShape(-1),
    myViewNet(viewNet),
    myMovingStartPos(false),
    myMovingEndPos(false),
    myJunctionToMove(nullptr),
    myEdgeToMove(nullptr),
    myPolyToMove(nullptr),
    myPOIToMove(nullptr),
    myAdditionalToMove(nullptr),
    myTAZToMove(nullptr) {
}


bool 
GNEViewNet::MoveSingleElementValues::beginMoveSingleElement() {
    // first obtain moving reference (common for all)
    myRelativeClickedPosition = myViewNet->getPositionInformation();
    // check what type of AC will be moved
    if (myViewNet->myObjectsUnderCursor.getPolyFront()) {
        // calculate poly movement values (can be entire shape, single geometry points, altitude, etc.)
        return calculatePolyValues();
    } else if (myViewNet->myObjectsUnderCursor.getPOIFront()) {
        // set POI moved object
        myPOIToMove = myViewNet->myObjectsUnderCursor.getPOIFront();
        // Save original Position of POI in view
        originalPositionInView = myPOIToMove->getPositionInView();
        // there is moved items, then return true
        return true;
    } else if (myViewNet->myObjectsUnderCursor.getAdditionalFront()) {
        // set additionals moved object
        myAdditionalToMove = myViewNet->myObjectsUnderCursor.getAdditionalFront();
        // save current position of additional
        originalPositionInView = myAdditionalToMove->getPositionInView();
        // start additional geometry moving
        myAdditionalToMove->startGeometryMoving();
        // there is moved items, then return true
        return true;
    } else if(myViewNet->myObjectsUnderCursor.getTAZFront()) {
        // calculate TAZ movement values (can be entire shape or single geometry points)
        return calculateTAZValues();
    } else if (myViewNet->myObjectsUnderCursor.getJunctionFront()) {
        // set junction moved object
        myJunctionToMove = myViewNet->myObjectsUnderCursor.getJunctionFront();
        // Save original Position of Element in view
        originalPositionInView = myJunctionToMove->getPositionInView();
        // start junction geometry moving
        myJunctionToMove->startGeometryMoving();
        // there is moved items, then return true
        return true;
    } else if (myViewNet->myObjectsUnderCursor.getEdgeFront() || myViewNet->myObjectsUnderCursor.getLaneFront()) {
        // calculate Edge movement values (can be entire shape, single geometry points, altitude, etc.)
        return calculateEdgeValues();
    } else {
        // there isn't moved items, then return false
        return false;
    }
}


void
GNEViewNet::MoveSingleElementValues::moveSingleElement() {
    // calculate offsetMovement depending of current mouse position and relative clicked position
    // @note  #3521: Add checkBox to allow moving elements... has to be implemented and used here
    Position offsetMovement = myViewNet->getPositionInformation() - myViewNet->myMoveSingleElementValues.myRelativeClickedPosition;
    // calculate Z depending of moveElevation
    if (myViewNet->myCreateEdgeOptions.moveElevation->shown() && myViewNet->myCreateEdgeOptions.moveElevation->getCheck() == TRUE) {
        // reset offset X and Y and use Y for Z
        offsetMovement = Position(0, 0, offsetMovement.y());
    } else {
        // leave z empty (because in this case offset only actuates over X-Y)
        offsetMovement.setz(0);
    }
    // check what element will be moved
    if (myPolyToMove) {
        // move shape's geometry without commiting changes depending if polygon is blocked
        if (myPolyToMove->isPolygonBlocked()) {
            // move entire shape
            myPolyToMove->moveEntireShape(originalShapeBeforeMoving, offsetMovement);
        } else {
            // move only a certain Geometry Point
            movingIndexShape = myPolyToMove->moveVertexShape(movingIndexShape, originalPositionInView, offsetMovement);
        }
    } else if (myPOIToMove) {
        // Move POI's geometry without commiting changes
        myPOIToMove->moveGeometry(originalPositionInView, offsetMovement);
    } else if (myJunctionToMove) {
        // Move Junction's geometry without commiting changes
        myJunctionToMove->moveGeometry(originalPositionInView, offsetMovement);
    } else if (myEdgeToMove) {
        // check if we're moving the start or end position, or a geometry point
        if (myMovingStartPos) {
            myEdgeToMove->moveShapeStart(originalPositionInView, offsetMovement);
        } else if (myMovingEndPos) {
            myEdgeToMove->moveShapeEnd(originalPositionInView, offsetMovement);
        } else {
            // move edge's geometry without commiting changes
            movingIndexShape = myEdgeToMove->moveVertexShape(movingIndexShape, originalPositionInView, offsetMovement);
        }
    } else if (myAdditionalToMove && (myAdditionalToMove->isAdditionalBlocked() == false)) {
        // Move Additional geometry without commiting changes
        myAdditionalToMove->moveGeometry(offsetMovement);
    } else if (myTAZToMove) {
        /// move TAZ's geometry without commiting changes depending if polygon is blocked
        if (myTAZToMove->isShapeBlocked()) {
            // move entire shape
            myTAZToMove->moveEntireShape(originalShapeBeforeMoving, offsetMovement);
        } else {
            // move only a certain Geometry Point
            movingIndexShape = myTAZToMove->moveVertexShape(movingIndexShape, originalPositionInView, offsetMovement);
        }
    }
}


void 
GNEViewNet::MoveSingleElementValues::finishMoveSingleElement() {
    if (myPolyToMove) {
        myPolyToMove->commitShapeChange(originalShapeBeforeMoving, myViewNet->getUndoList());
        myPolyToMove = nullptr;
    } else if (myPOIToMove) {
        myPOIToMove->commitGeometryMoving(originalPositionInView, myViewNet->getUndoList());
        myPOIToMove = nullptr;
    } else if (myJunctionToMove) {
        // check if in the moved position there is another Junction and it will be merged
        if (!myViewNet->mergeJunctions(myJunctionToMove, originalPositionInView)) {
            myJunctionToMove->endGeometryMoving();
            // position is already up to date but we must register with myViewNet->getUndoList()
            myJunctionToMove->commitGeometryMoving(originalPositionInView, myViewNet->getUndoList());
        }
        myJunctionToMove = nullptr;
    } else if (myEdgeToMove) {
        // end geometry moving
        myEdgeToMove->endGeometryMoving();
        // commit change depending of what was moved
        if (myMovingStartPos) {
            myEdgeToMove->commitShapeStartChange(originalPositionInView, myViewNet->getUndoList());
            myMovingStartPos = false;
        } else if (myMovingEndPos) {
            myEdgeToMove->commitShapeEndChange(originalPositionInView, myViewNet->getUndoList());
            myMovingEndPos = false;
        } else {
            myEdgeToMove->commitShapeChange(originalShapeBeforeMoving, myViewNet->getUndoList());
        }
        myEdgeToMove = nullptr;
    } else if (myAdditionalToMove) {
        myAdditionalToMove->endGeometryMoving();
        myAdditionalToMove->commitGeometryMoving(myViewNet->getUndoList());
        myAdditionalToMove = nullptr;
    } else if (myTAZToMove) {
        myTAZToMove->commitShapeChange(originalShapeBeforeMoving, myViewNet->getUndoList());
        myTAZToMove = nullptr;
    }
}


bool 
GNEViewNet::MoveSingleElementValues::calculatePolyValues() {
    // set Poly to move
    myPolyToMove = myViewNet->myObjectsUnderCursor.getPolyFront();
    // now we have two cases: if we're editing the X-Y coordenade or the altitude (z)
    if (myViewNet->myCreateEdgeOptions.moveElevation->shown() && myViewNet->myCreateEdgeOptions.moveElevation->getCheck() == TRUE) {
        // check if in the clicked position a geometry point exist
        int existentIndex = myPolyToMove->getVertexIndex(myViewNet->getPositionInformation(), false, false);
        if (existentIndex != -1) {
            // save original shape (needed for commit change)
            myViewNet->myMoveSingleElementValues.originalShapeBeforeMoving = myPolyToMove->getShape();
            // obtain existent index
            myViewNet->myMoveSingleElementValues.movingIndexShape = existentIndex;
            myViewNet->myMoveSingleElementValues.originalPositionInView = myPolyToMove->getShape()[existentIndex];
            // poly values sucesfully calculated, then return true
            return true;
        } else {
            // stop poly moving
            myPolyToMove = nullptr;
            // poly values wasn't calculated, then return false
            return false;
        }
    } else {
        // save original shape (needed for commit change)
        myViewNet->myMoveSingleElementValues.originalShapeBeforeMoving = myPolyToMove->getShape();
        // save clicked position as moving original position
        myViewNet->myMoveSingleElementValues.originalPositionInView = myViewNet->getPositionInformation();
        // obtain index of vertex to move if shape isn't blocked
        if ((myPolyToMove->isPolygonBlocked() == false) && (myPolyToMove->isMovementBlocked() == false)) {
            // check if we want to remove a Geometry Point
            if (myViewNet->myKeyPressed.shiftKeyPressed()) {
                // check if we're clicked over a Geometry Point
                myViewNet->myMoveSingleElementValues.movingIndexShape = myPolyToMove->getVertexIndex(myViewNet->myMoveSingleElementValues.originalPositionInView, false, false);
                if (myViewNet->myMoveSingleElementValues.movingIndexShape != -1) {
                    myPolyToMove->deleteGeometryPoint(myViewNet->myMoveSingleElementValues.originalPositionInView);
                    // after removing Geomtery Point, reset PolyToMove
                    myPolyToMove = nullptr;
                    // poly values wasn't calculated, then return false
                    return false;
                }
                // poly values sucesfully calculated, then return true
                return true;
            } else {
                // obtain index of vertex to move and moving reference
                myViewNet->myMoveSingleElementValues.movingIndexShape = myPolyToMove->getVertexIndex(myViewNet->myMoveSingleElementValues.originalPositionInView, false, false);
                if (myViewNet->myMoveSingleElementValues.movingIndexShape == -1) {
                    // create new geometry point
                    myViewNet->myMoveSingleElementValues.movingIndexShape = myPolyToMove->getVertexIndex(myViewNet->myMoveSingleElementValues.originalPositionInView, true, true);
                }
                // poly values sucesfully calculated, then return true
                return true;
            }
        } else {
            myViewNet->myMoveSingleElementValues.movingIndexShape = -1;
            // poly values wasn't calculated, then return false
            return false;
        }
    }
}


bool 
GNEViewNet::MoveSingleElementValues::calculateEdgeValues() {
    if (myViewNet->myKeyPressed.shiftKeyPressed()) {
        // edit end point
        myViewNet->myObjectsUnderCursor.getEdgeFront()->editEndpoint(myViewNet->getPositionInformation(), myViewNet->myUndoList);
        // edge values wasn't calculated, then return false
        return false;
    } else {
        // assign clicked edge to edgeToMove
        myEdgeToMove = myViewNet->myObjectsUnderCursor.getEdgeFront();
        // check if we clicked over a start or end position
        if (myEdgeToMove->clickedOverShapeStart(myViewNet->getPositionInformation())) {
            // save start pos
            myViewNet->myMoveSingleElementValues.originalPositionInView = myEdgeToMove->getNBEdge()->getGeometry().front();
            myViewNet->myMoveSingleElementValues.myMovingStartPos = true;
            // start geometry moving
            myEdgeToMove->startGeometryMoving();
            // edge values sucesfully calculated, then return true
            return true;
        } else if (myEdgeToMove->clickedOverShapeEnd(myViewNet->getPositionInformation())) {
            // save end pos
            myViewNet->myMoveSingleElementValues.originalPositionInView = myEdgeToMove->getNBEdge()->getGeometry().back();
            myViewNet->myMoveSingleElementValues.myMovingEndPos = true;
            // start geometry moving
            myEdgeToMove->startGeometryMoving();
            // edge values sucesfully calculated, then return true
            return true;
        } else {
            // now we have two cases: if we're editing the X-Y coordenade or the altitude (z)
            if (myViewNet->myCreateEdgeOptions.moveElevation->shown() && myViewNet->myCreateEdgeOptions.moveElevation->getCheck() == TRUE) {
                // check if in the clicked position a geometry point exist
                int existentIndex = myEdgeToMove->getVertexIndex(myViewNet->getPositionInformation(), false, false);
                if (existentIndex != -1) {
                    myViewNet->myMoveSingleElementValues.movingIndexShape = existentIndex;
                    myViewNet->myMoveSingleElementValues.originalPositionInView = myEdgeToMove->getNBEdge()->getInnerGeometry()[existentIndex];
                    // start geometry moving
                    myEdgeToMove->startGeometryMoving();
                    // edge values sucesfully calculated, then return true
                    return true;
                } else {
                    // stop edge moving
                    myEdgeToMove = nullptr;
                    // edge values wasn't calculated, then return false
                    return false;
                }
            } else {
                // save original shape (needed for commit change)
                myViewNet->myMoveSingleElementValues.originalShapeBeforeMoving = myEdgeToMove->getNBEdge()->getInnerGeometry();
                // obtain index of vertex to move and moving reference
                myViewNet->myMoveSingleElementValues.movingIndexShape = myEdgeToMove->getVertexIndex(myViewNet->getPositionInformation(), false, false);
                // if index doesn't exist, create it snapping new edge to grid
                if (myViewNet->myMoveSingleElementValues.movingIndexShape == -1) {
                    myViewNet->myMoveSingleElementValues.movingIndexShape = myEdgeToMove->getVertexIndex(myViewNet->getPositionInformation(), true, true);
                }
                // make sure that myViewNet->myMoveSingleElementValues.movingIndexShape isn't -1
                if(myViewNet->myMoveSingleElementValues.movingIndexShape != -1) {
                    myViewNet->myMoveSingleElementValues.originalPositionInView = myEdgeToMove->getNBEdge()->getInnerGeometry()[myViewNet->myMoveSingleElementValues.movingIndexShape];
                    // start geometry moving
                    myEdgeToMove->startGeometryMoving();
                    // edge values sucesfully calculated, then return true
                    return true;
                } else {
                    // edge values wasn't calculated, then return false
                    return false;
                }
            }
        }
    }
}


bool 
GNEViewNet::MoveSingleElementValues::calculateTAZValues() {
    // set TAZ to move
    myTAZToMove = myViewNet->myObjectsUnderCursor.getTAZFront();
    // save original shape (needed for commit change)
    myViewNet->myMoveSingleElementValues.originalShapeBeforeMoving = myTAZToMove->getShape();
    // save clicked position as moving original position
    myViewNet->myMoveSingleElementValues.originalPositionInView = myViewNet->getPositionInformation();
    // obtain index of vertex to move if shape isn't blocked
    if ((myTAZToMove->isShapeBlocked() == false) && (myTAZToMove->isAdditionalBlocked() == false)) {
        // check if we want to remove a Geometry Point
        if (myViewNet->myKeyPressed.shiftKeyPressed()) {
            // check if we're clicked over a Geometry Point
            myViewNet->myMoveSingleElementValues.movingIndexShape = myTAZToMove->getVertexIndex(myViewNet->myMoveSingleElementValues.originalPositionInView, false, false);
            if (myViewNet->myMoveSingleElementValues.movingIndexShape != -1) {
                myTAZToMove->deleteGeometryPoint(myViewNet->myMoveSingleElementValues.originalPositionInView);
                // after removing Geomtery Point, reset TAZToMove
                myTAZToMove = nullptr;
                // TAZ values wasn't calculated, then return false
                return false;
            }
            // TAZ values sucesfully calculated, then return true
            return true;
        } else {
            // obtain index of vertex to move and moving reference
            myViewNet->myMoveSingleElementValues.movingIndexShape = myTAZToMove->getVertexIndex(myViewNet->myMoveSingleElementValues.originalPositionInView, false, false);
            if (myViewNet->myMoveSingleElementValues.movingIndexShape == -1) {
                // create new geometry point
                myViewNet->myMoveSingleElementValues.movingIndexShape = myTAZToMove->getVertexIndex(myViewNet->myMoveSingleElementValues.originalPositionInView, true, true);
            }
            // TAZ values sucesfully calculated, then return true
            return true;
        }
    } else {
        // abort moving index shape
        myViewNet->myMoveSingleElementValues.movingIndexShape = -1;
        // TAZ values wasn't calculated, then return false
        return false;
    }
}

// ---------------------------------------------------------------------------
// GNEViewNet::MoveMultipleElementValues - methods
// ---------------------------------------------------------------------------

GNEViewNet::MoveMultipleElementValues::MoveMultipleElementValues(GNEViewNet* viewNet) : 
    myViewNet(viewNet),
    myMovingSelection(false) {
}


void
GNEViewNet::MoveMultipleElementValues::beginMoveSelection(GNEAttributeCarrier* originAC) {
    // enable moving selection
    myMovingSelection = true;
    // save clicked position (to calculate offset)
    myClickedPosition = myViewNet->getPositionInformation();
    // obtain Junctions and edges selected
    std::vector<GNEJunction*> selectedJunctions = myViewNet->getNet()->retrieveJunctions(true);
    std::vector<GNEEdge*> selectedEdges = myViewNet->getNet()->retrieveEdges(true);
    // Junctions are always moved, then save position of current selected junctions (Needed when mouse is released)
    for (auto i : selectedJunctions) {
        // save junction position
        myMovedJunctionOriginPositions[i] = i->getPositionInView();
        // start geometry moving
        i->startGeometryMoving();
    }
    // make special movement depending of clicked AC
    if (originAC->getTagProperty().getTag() == SUMO_TAG_JUNCTION) {
        // if clicked element is a junction, move shapes of all selected edges
        for (auto i : selectedEdges) {
            // save entire edge geometry
            myMovedEdgesOriginShape[i] = i->getNBEdge()->getInnerGeometry();
            // start geometry moving
            i->startGeometryMoving();
        }
    } else if (originAC->getTagProperty().getTag() == SUMO_TAG_EDGE) {
        // obtain clicked edge
        GNEEdge* clickedEdge = dynamic_cast<GNEEdge*>(originAC);
        // if clicked edge has origin and destiny junction selected, move shapes of all selected edges
        if (myMovedJunctionOriginPositions.count(clickedEdge->getGNEJunctionSource()) > 0 &&
            myMovedJunctionOriginPositions.count(clickedEdge->getGNEJunctionDestiny()) > 0) {
            for (auto i : selectedEdges) {
                // save entire edge geometry
                myMovedEdgesOriginShape[i] = i->getNBEdge()->getInnerGeometry();
                // start geometry moving
                i->startGeometryMoving();
            }
        } else {
            // declare three groups for dividing edges
            std::vector<GNEEdge*> noJunctionsSelected;
            std::vector<GNEEdge*> originJunctionSelected;
            std::vector<GNEEdge*> destinyJunctionSelected;
            // divide selected edges into four groups, depending of the selection of their junctions
            for (auto i : selectedEdges) {
                bool originSelected = myMovedJunctionOriginPositions.count(i->getGNEJunctionSource()) > 0;
                bool destinySelected = myMovedJunctionOriginPositions.count(i->getGNEJunctionDestiny()) > 0;
                // bot junctions selected
                if (!originSelected && !destinySelected) {
                    noJunctionsSelected.push_back(i);
                } else if (originSelected && !destinySelected) {
                    originJunctionSelected.push_back(i);
                } else if (!originSelected && destinySelected) {
                    destinyJunctionSelected.push_back(i);
                } else if (!originSelected && !destinySelected) {
                    // save edge geometry
                    myMovedEdgesOriginShape[i] = i->getNBEdge()->getInnerGeometry();
                    // start geometry moving
                     i->startGeometryMoving();
                }
            }
            // save original shape of all noJunctionsSelected edges (needed for commit change)
            for (auto i : noJunctionsSelected) {
                myMovedEgdesGeometryPoints[i] = new MoveSingleElementValues(myViewNet);
                // save edge geometry
                myMovedEgdesGeometryPoints[i]->originalShapeBeforeMoving = i->getNBEdge()->getInnerGeometry();
                // start geometry moving
                i->startGeometryMoving();
            }
            // obtain index shape of clicked edge
            int index = clickedEdge->getVertexIndex(myViewNet->getPositionInformation(), true, true);
            // check that index is valid
            if (index < 0) {
                throw ProcessError("invalid shape index");
            }
            // save index and original position
            myMovedEgdesGeometryPoints[clickedEdge] = new MoveSingleElementValues(myViewNet);
            myMovedEgdesGeometryPoints[clickedEdge]->movingIndexShape = index;
            myMovedEgdesGeometryPoints[clickedEdge]->originalPositionInView = myViewNet->getPositionInformation();
            // start moving of clicked edge AFTER getting vertex Index
            clickedEdge->startGeometryMoving();
            // do the same for  the rest of noJunctionsSelected edges
            for (auto i : noJunctionsSelected) {
                if (i != clickedEdge) {
                    myMovedEgdesGeometryPoints[i] = new MoveSingleElementValues(myViewNet);
                    // save index and original position
                    myMovedEgdesGeometryPoints[i]->movingIndexShape = i->getVertexIndex(myViewNet->getPositionInformation(), true, true);
                    // set originalPosition depending if edge is opposite to clicked edge
                    if (i->getOppositeEdge() == clickedEdge) {
                        myMovedEgdesGeometryPoints[i]->originalPositionInView = myViewNet->getPositionInformation();
                    } else {
                        myMovedEgdesGeometryPoints[i]->originalPositionInView = i->getNBEdge()->getInnerGeometry()[myMovedEgdesGeometryPoints[i]->movingIndexShape];
                    }
                    // start moving of clicked edge AFTER getting vertex Index
                    i->startGeometryMoving();
                }
            }
        }
    }
}


void
GNEViewNet::MoveMultipleElementValues::moveSelection() {
    // calculate offset between current position and original position
    Position offsetMovement = myViewNet->getPositionInformation() - myClickedPosition;
    // calculate Z depending of Grid
    if (myViewNet->myCreateEdgeOptions.moveElevation->shown() && myViewNet->myCreateEdgeOptions.moveElevation->getCheck() == TRUE) {
        // reset offset X and Y and use Y for Z
        offsetMovement = Position(0, 0, offsetMovement.y());
    } else {
        // leave z empty (because in this case offset only actuates over X-Y)
        offsetMovement.setz(0);
    }
    // move selected junctions
    for (auto i : myMovedJunctionOriginPositions) {
        i.first->moveGeometry(i.second, offsetMovement);
    }
    // move entire edge shapes
    for (auto i : myMovedEdgesOriginShape) {
        i.first->moveEntireShape(i.second, offsetMovement);
    }
    // move partial shapes
    for (auto i : myMovedEgdesGeometryPoints) {
        i.first->moveVertexShape(i.second->movingIndexShape, i.second->originalPositionInView, offsetMovement);
    }
}


void
GNEViewNet::MoveMultipleElementValues::finishMoveSelection() {
    // begin undo list
    myViewNet->getUndoList()->p_begin("position of selected elements");
    // commit positions of moved junctions
    for (auto i : myMovedJunctionOriginPositions) {
        i.first->endGeometryMoving();
        i.first->commitGeometryMoving(i.second, myViewNet->getUndoList());
    }
    // commit shapes of entired moved edges
    for (auto i : myMovedEdgesOriginShape) {
        i.first->endGeometryMoving();
        i.first->commitShapeChange(i.second, myViewNet->getUndoList());
    }
    //commit shapes of partial moved shapes
    for (auto i : myMovedEgdesGeometryPoints) {
        i.first->endGeometryMoving();
        i.first->commitShapeChange(i.second->originalShapeBeforeMoving, myViewNet->getUndoList());
    }
    // end undo list
    myViewNet->getUndoList()->p_end();
    // stop moving selection
    myMovingSelection = false;
    // clear containers
    myMovedJunctionOriginPositions.clear();
    myMovedEdgesOriginShape.clear();
    // delete all movedEgdesGeometryPoints before clear container
    for (const auto &i : myMovedEgdesGeometryPoints) {
        delete i.second;
    }
    myMovedEgdesGeometryPoints.clear();
}


bool 
GNEViewNet::MoveMultipleElementValues::isMovingSelection() const {
    return myMovingSelection;
}

// ---------------------------------------------------------------------------
// GNEViewNet::SelectingArea - methods
// ---------------------------------------------------------------------------

GNEViewNet::SelectingArea::SelectingArea(GNEViewNet* viewNet) :
    selectingUsingRectangle(false),
    startDrawing(false), 
    myViewNet(viewNet) {
}


void 
GNEViewNet::SelectingArea::beginRectangleSelection() {
    selectingUsingRectangle = true;
    selectionCorner1 = myViewNet->getPositionInformation();
    selectionCorner2 = selectionCorner1;
}


void 
GNEViewNet::SelectingArea::moveRectangleSelection() {
    // start drawing
    startDrawing = true;
    // only update selection corner 2
    selectionCorner2 = myViewNet->getPositionInformation();
    // update status bar
    myViewNet->setStatusBarText("Selection width:" + toString(fabs(selectionCorner1.x() - selectionCorner2.x()))
        + " height:" + toString(fabs(selectionCorner1.y() - selectionCorner2.y()))
        + " diagonal:" + toString(selectionCorner1.distanceTo2D(selectionCorner2)));
}


void 
GNEViewNet::SelectingArea::finishRectangleSelection() {
    // finish rectangle selection
    selectingUsingRectangle = false;
    startDrawing = false;
}


void
GNEViewNet::SelectingArea::processRectangleSelection() {
    // shift held down on mouse-down and mouse-up and check that rectangle exist
    if ((abs(selectionCorner1.x() - selectionCorner2.x()) > 0.01) && 
        (abs(selectionCorner1.y() - selectionCorner2.y()) > 0.01) &&
        myViewNet->myKeyPressed.shiftKeyPressed()) {
        // create boundary between two corners
        Boundary rectangleBoundary;
        rectangleBoundary.add(selectionCorner1);
        rectangleBoundary.add(selectionCorner2);
        // process selection within boundary
        processBoundarySelection(rectangleBoundary);
    }
}


std::vector<GNEEdge*>
GNEViewNet::SelectingArea::processEdgeRectangleSelection() {
    // declare vector for selection
    std::vector<GNEEdge*> result;
    // shift held down on mouse-down and mouse-up and check that rectangle exist
    if ((abs(selectionCorner1.x() - selectionCorner2.x()) > 0.01) && 
        (abs(selectionCorner1.y() - selectionCorner2.y()) > 0.01) &&
        myViewNet->myKeyPressed.shiftKeyPressed()) {
        // create boundary between two corners
        Boundary rectangleBoundary;
        rectangleBoundary.add(selectionCorner1);
        rectangleBoundary.add(selectionCorner2);
        if (myViewNet->makeCurrent()) {
            // obtain all ACs in Rectangle BOundary
            std::set<std::pair<std::string, GNEAttributeCarrier*> > ACsInBoundary = myViewNet->getAttributeCarriersInBoundary(rectangleBoundary);
            // Filter ACs in Boundary and get only edges
            for (auto i : ACsInBoundary) {
                if(i.second->getTagProperty().getTag() == SUMO_TAG_EDGE) {
                    result.push_back(dynamic_cast<GNEEdge*>(i.second));
                }
            }
            myViewNet->makeNonCurrent();
        }
    }
    return result;
}


void 
GNEViewNet::SelectingArea::processShapeSelection(const PositionVector &shape) {
    processBoundarySelection(shape.getBoxBoundary());
}


void 
GNEViewNet::SelectingArea::drawRectangleSelection(const RGBColor& color) const {
    if (selectingUsingRectangle) {
        glPushMatrix();
        glTranslated(0, 0, GLO_MAX - 1);
        GLHelper::setColor(color);
        glLineWidth(2);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin(GL_QUADS);
        glVertex2d(selectionCorner1.x(), selectionCorner1.y());
        glVertex2d(selectionCorner1.x(), selectionCorner2.y());
        glVertex2d(selectionCorner2.x(), selectionCorner2.y());
        glVertex2d(selectionCorner2.x(), selectionCorner1.y());
        glEnd();
        glPopMatrix();
    }
}


void 
GNEViewNet::SelectingArea::processBoundarySelection(const Boundary &boundary) {
    if (myViewNet->makeCurrent()) {
        std::set<std::pair<std::string, GNEAttributeCarrier*> > ACsInBoundary = myViewNet->getAttributeCarriersInBoundary(boundary);
        // declare two sets of attribute carriers, one for select and another for unselect
        std::vector<GNEAttributeCarrier*> ACToSelect;
        std::vector<GNEAttributeCarrier*> ACToUnselect;
        // reserve memory (we assume that in the worst case we're going to insert all elements of ACsInBoundary
        ACToSelect.reserve(ACsInBoundary.size());
        ACToUnselect.reserve(ACsInBoundary.size());
        // in restrict AND replace mode all current selected attribute carriers will be unselected
        if ((myViewNet->myViewParent->getSelectorFrame()->getModificationModeModul()->getModificationMode() == GNESelectorFrame::ModificationMode::SET_RESTRICT) ||
                (myViewNet->myViewParent->getSelectorFrame()->getModificationModeModul()->getModificationMode() == GNESelectorFrame::ModificationMode::SET_REPLACE)) {
            for (auto i : myViewNet->myNet->getSelectedAttributeCarriers()) {
                ACToUnselect.push_back(i);
            }
        }
        // iterate over AtributeCarriers obtained of boundary an place it in ACToSelect or ACToUnselect
        for (auto i : ACsInBoundary) {
            switch (myViewNet->myViewParent->getSelectorFrame()->getModificationModeModul()->getModificationMode()) {
                case GNESelectorFrame::ModificationMode::SET_SUB:
                    ACToUnselect.push_back(i.second);
                    break;
                case GNESelectorFrame::ModificationMode::SET_RESTRICT:
                    if (std::find(ACToUnselect.begin(), ACToUnselect.end(), i.second) != ACToUnselect.end()) {
                        ACToSelect.push_back(i.second);
                    }
                    break;
                default:
                    ACToSelect.push_back(i.second);
                    break;
            }
        }
        // select junctions and their connections and crossings if Auto select junctions is enabled (note: only for "add mode")
        if (myViewNet->autoSelectNodes() && GNESelectorFrame::ModificationMode::SET_ADD) {
            std::vector<GNEEdge*> edgesToSelect;
            // iterate over ACToSelect and extract edges
            for (auto i : ACToSelect) {
                if (i->getTagProperty().getTag() == SUMO_TAG_EDGE) {
                    edgesToSelect.push_back(dynamic_cast<GNEEdge*>(i));
                }
            }
            // iterate over extracted edges
            for (auto i : edgesToSelect) {
                // select junction source and all their connections and crossings
                ACToSelect.push_back(i->getGNEJunctionSource());
                for (auto j : i->getGNEJunctionSource()->getGNEConnections()) {
                    ACToSelect.push_back(j);
                }
                for (auto j : i->getGNEJunctionSource()->getGNECrossings()) {
                    ACToSelect.push_back(j);
                }
                // select junction destiny and all their connections crossings
                ACToSelect.push_back(i->getGNEJunctionDestiny());
                for (auto j : i->getGNEJunctionDestiny()->getGNEConnections()) {
                    ACToSelect.push_back(j);
                }
                for (auto j : i->getGNEJunctionDestiny()->getGNECrossings()) {
                    ACToSelect.push_back(j);
                }
            }
        }
        // only continue if there is ACs to select or unselect
        if ((ACToSelect.size() + ACToUnselect.size()) > 0) {
            // first unselect AC of ACToUnselect and then selects AC of ACToSelect
            myViewNet->myUndoList->p_begin("selection using rectangle");
            for (auto i : ACToUnselect) {
                i->setAttribute(GNE_ATTR_SELECTED, "0", myViewNet->myUndoList);
            }
            for (auto i : ACToSelect) {
                if (i->getTagProperty().isSelectable()) {
                    i->setAttribute(GNE_ATTR_SELECTED, "1", myViewNet->myUndoList);
                }
            }
            myViewNet->myUndoList->p_end();
        }
        myViewNet->makeNonCurrent();
    }
}

// ---------------------------------------------------------------------------
// GNEViewNet::TestingMode - methods
// ---------------------------------------------------------------------------

GNEViewNet::TestingMode::TestingMode(GNEViewNet* viewNet) :
    myViewNet(viewNet),
    myTestingEnabled(OptionsCont::getOptions().getBool("gui-testing")),
    myTestingWidth(0),
    myTestingHeight(0) {
}


void
GNEViewNet::TestingMode::initTestingMode() {
    // first check if testing mode is enabled and window size is correct
    if (myTestingEnabled && OptionsCont::getOptions().isSet("window-size")) {
        std::vector<std::string> windowSize = OptionsCont::getOptions().getStringVector("window-size");
        // make sure that given windows size has exactly two valid int values
        if ((windowSize.size() == 2) && GNEAttributeCarrier::canParse<int>(windowSize[0]) && GNEAttributeCarrier::canParse<int>(windowSize[1])) {
            myTestingWidth = GNEAttributeCarrier::parse<int>(windowSize[0]);
            myTestingHeight = GNEAttributeCarrier::parse<int>(windowSize[1]);
        } else {
            WRITE_ERROR("Invalid windows size-format: " + toString(windowSize) + "for option 'window-size'");
        }
    }
}


void
GNEViewNet::TestingMode::drawTestingElements(GUIMainWindow* mainWindow) {
    // first check if testing mode is neabled
    if (myTestingEnabled) {
        // check if main windows has to be resized
        if (myTestingWidth > 0 && ((myViewNet->getWidth() != myTestingWidth) || (myViewNet->getHeight() != myTestingHeight))) {
            // only resize once to avoid flickering
            //std::cout << " before resize: view=" << getWidth() << ", " << getHeight() << " app=" << mainWindow->getWidth() << ", " << mainWindow->getHeight() << "\n";
            mainWindow->resize(myTestingWidth + myTestingWidth - myViewNet->getWidth(), myTestingHeight + myTestingHeight - myViewNet->getHeight());
            //std::cout << " directly after resize: view=" << getWidth() << ", " << getHeight() << " app=" << mainWindow->getWidth() << ", " << mainWindow->getHeight() << "\n";
            myTestingWidth = 0;
        }
        //std::cout << " fixed: view=" << getWidth() << ", " << getHeight() << " app=" << mainWindow->getWidth() << ", " << mainWindow->getHeight() << "\n";
        // draw pink square in the upper left corner on top of everything
        glPushMatrix();
        const double size = myViewNet->p2m(32);
        Position center = myViewNet->screenPos2NetPos(8, 8);
        GLHelper::setColor(RGBColor::MAGENTA);
        glTranslated(center.x(), center.y(), GLO_MAX - 1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBegin(GL_QUADS);
        glVertex2d(0, 0);
        glVertex2d(0, -size);
        glVertex2d(size, -size);
        glVertex2d(size, 0);
        glEnd();
        glPopMatrix();
        glPushMatrix();
        // show box with the current position relative to pink square
        Position posRelative = myViewNet->screenPos2NetPos(myViewNet->getWidth() - 40, myViewNet->getHeight() - 20);
        // adjust cursor position (24,25) to show exactly the same position as in function netedit.leftClick(match, X, Y)
        GLHelper::drawTextBox(toString(myViewNet->myWindowCursorPositionX - 24) + " " + toString(myViewNet->myWindowCursorPositionY - 25), posRelative, GLO_MAX - 1, myViewNet->p2m(20), RGBColor::BLACK, RGBColor::WHITE);
        glPopMatrix();
    }
}

// ---------------------------------------------------------------------------
// GNEViewNet::CreateEdgeOptions - methods
// ---------------------------------------------------------------------------

GNEViewNet::CreateEdgeOptions::CreateEdgeOptions(GNEViewNet* viewNet) : 
    myViewNet(viewNet) {
}


void
GNEViewNet::CreateEdgeOptions::buildCreateEdgeOptionMenuChecks() {
    chainEdges = new FXMenuCheck(myViewNet->myToolbar, ("Chain\t\tCreate consecutive " + toString(SUMO_TAG_EDGE) + "s with a single click (hit ESC to cancel chain).").c_str(), myViewNet, 0);
    chainEdges->setCheck(false);

    autoOppositeEdge = new FXMenuCheck(myViewNet->myToolbar, ("Two-way\t\tAutomatically create an " + toString(SUMO_TAG_EDGE) + " in the opposite direction").c_str(), myViewNet, 0);
    autoOppositeEdge->setCheck(false);

    warnAboutMerge = new FXMenuCheck(myViewNet->myToolbar, ("Ask for merge\t\tAsk for confirmation before merging " + toString(SUMO_TAG_JUNCTION) + ".").c_str(), myViewNet, 0);
    warnAboutMerge->setCheck(true);

    showJunctionBubble = new FXMenuCheck(myViewNet->myToolbar, ("Bubbles\t\tShow bubbles over " + toString(SUMO_TAG_JUNCTION) + "'s shapes.").c_str(), myViewNet, MID_GNE_VIEWNET_SHOW_BUBBLES);
    showJunctionBubble->setCheck(false);

    moveElevation = new FXMenuCheck(myViewNet->myToolbar, "Elevation\t\tApply mouse movement to elevation instead of x,y position", myViewNet, MID_GNE_VIEWNET_MOVE_ELEVATION);
    moveElevation->setCheck(false);
}


void
GNEViewNet::CreateEdgeOptions::hideCreateEdgeOptionMenuChecks() {
    chainEdges->hide();
    autoOppositeEdge->hide();
    warnAboutMerge->hide();
    showJunctionBubble->hide();
    moveElevation->hide();
}

// ---------------------------------------------------------------------------
// GNEViewNet::ViewOptions - methods
// ---------------------------------------------------------------------------

GNEViewNet::ViewOptions::ViewOptions(GNEViewNet* viewNet) :
    myViewNet(viewNet) {
}


void 
GNEViewNet::ViewOptions::buildViewOptionsMenuChecks() {
    menuCheckSelectEdges = new FXMenuCheck(myViewNet->myToolbar, ("Select edges\t\tToggle whether clicking should select " + toString(SUMO_TAG_EDGE) + "s or " + toString(SUMO_TAG_LANE) + "s").c_str(), myViewNet, MID_GNE_VIEWNET_SELECT_EDGES);
    menuCheckSelectEdges->setCheck(true);

    menuCheckShowConnections = new FXMenuCheck(myViewNet->myToolbar, ("Show " + toString(SUMO_TAG_CONNECTION) + "s\t\tToggle show " + toString(SUMO_TAG_CONNECTION) + "s over " + toString(SUMO_TAG_JUNCTION) + "s").c_str(), myViewNet, MID_GNE_VIEWNET_SHOW_CONNECTIONS);
    menuCheckShowConnections->setCheck(myViewNet->myVisualizationSettings->showLane2Lane);

    menuCheckHideConnections = new FXMenuCheck(myViewNet->myToolbar, "Hide connections\t\tHide connections", myViewNet, 0);
    menuCheckHideConnections->setCheck(false);

    menuCheckExtendSelection = new FXMenuCheck(myViewNet->myToolbar, ("Auto-select " + toString(SUMO_TAG_JUNCTION) + "s\t\tToggle whether selecting multiple " + toString(SUMO_TAG_EDGE) + "s should automatically select their " + toString(SUMO_TAG_JUNCTION) + "s").c_str(), myViewNet, 0);
    menuCheckExtendSelection->setCheck(false);
    
    menuCheckChangeAllPhases = new FXMenuCheck(myViewNet->myToolbar, ("Apply change to all phases\t\tToggle whether clicking should apply state changes to all phases of the current " + toString(SUMO_TAG_TRAFFIC_LIGHT) + " plan").c_str(), myViewNet, 0);
    menuCheckChangeAllPhases->setCheck(false);

    menuCheckShowGrid = new FXMenuCheck(myViewNet->myToolbar, "Grid\t\tshow grid and restrict movement to the grid (size defined in visualization options)", myViewNet, MID_GNE_VIEWNET_SHOW_GRID);
    menuCheckShowGrid->setCheck(false);
}

void 
GNEViewNet::ViewOptions::hideViewOptionsMenuChecks() {
    menuCheckSelectEdges->hide();
    menuCheckShowConnections->hide();
    menuCheckHideConnections->hide();
    menuCheckExtendSelection->hide();
    menuCheckChangeAllPhases->hide();
    menuCheckShowGrid->hide();
}

bool 
GNEViewNet::ViewOptions::selectEdges() const {
    if(menuCheckSelectEdges->shown()) {
        return (menuCheckSelectEdges->getCheck() == TRUE);
    } else {
        // by default, if menuCheckSelectEdges isn't shown, always select edges
        return true;
    }
}


bool 
GNEViewNet::ViewOptions::showConnections() const {
    return (menuCheckShowConnections->getCheck() == TRUE);
}

// ---------------------------------------------------------------------------
// GNEViewNet::SuperModes - methods
// ---------------------------------------------------------------------------

GNEViewNet::SuperModes::SuperModes(GNEViewNet* viewNet) :
    myViewNet(viewNet),
    currentSupermode(GNE_SUPERMODE_NONE),
    networkEditMode(GNE_NMODE_INSPECT),
    demandEditMode(GNE_DMODE_ROUTES),
    networkButton(nullptr),
    demandButton(nullptr) {
}


GNEViewNet::SuperModes::~SuperModes() {
    // destroy and delete vertical separator button
    myVerticalSeparator->destroy();
    delete myVerticalSeparator;
    // destroy and delete network button
    networkButton->destroy();
    delete networkButton;
    // destroy and delete demand button
    demandButton->destroy();
    delete demandButton;
    // recalc menu bar because there is removed elements
    myViewNet->getViewParent()->getGNEAppWindows()->getMenuBar()->recalc();
}


void
GNEViewNet::SuperModes::buildSuperModeButtons() {
    // Create Vertical separator
    myVerticalSeparator = new FXVerticalSeparator(myViewNet->getViewParent()->getGNEAppWindows()->getMenuBar(), GUIDesignVerticalSeparator);
    // create buttons
    networkButton = new MFXCheckableButton(false, myViewNet->getViewParent()->getGNEAppWindows()->getMenuBar(), "Network\t\tSet mode for edit network elements.",
        GUIIconSubSys::getIcon(ICON_SUPERMODENETWORK), myViewNet, MID_GNE_SETSUPERMODE_NETWORK, GUIDesignButtonToolbarSupermode);
    demandButton = new MFXCheckableButton(false, myViewNet->getViewParent()->getGNEAppWindows()->getMenuBar(), "Demand\t\tSet mode for edit traffic demand.",
        GUIIconSubSys::getIcon(ICON_SUPERMODEDEMAND), myViewNet, MID_GNE_SETSUPERMODE_DEMAND, GUIDesignButtonToolbarSupermode);
    // new elements has to be created manually (because MenuBar already exists)
    myVerticalSeparator->create();
    networkButton->create();
    demandButton->create();
    // recalc menu bar because there is new elements
    myViewNet->getViewParent()->getGNEAppWindows()->getMenuBar()->recalc();
}

// ---------------------------------------------------------------------------
// GNEViewNet::NetworkCheckableButtons - methods
// ---------------------------------------------------------------------------

GNEViewNet::NetworkCheckableButtons::NetworkCheckableButtons(GNEViewNet* viewNet) : 
    myViewNet(viewNet),
    createEdgeButton(nullptr),
    moveButton(nullptr),
    deleteButton(nullptr),
    inspectButton(nullptr),
    selectButton(nullptr),
    connectionButton(nullptr),
    trafficLightButton(nullptr),
    additionalButton(nullptr),
    crossingButton(nullptr),
    shapeButton(nullptr),
    prohibitionButton(nullptr) {
}


void
GNEViewNet::NetworkCheckableButtons::buildNetworkCheckableButtons() {
    createEdgeButton = new MFXCheckableButton(false, myViewNet->myToolbar, "\tset create edge mode\tMode for creating junction and edges.",
        GUIIconSubSys::getIcon(ICON_MODECREATEEDGE), myViewNet, MID_GNE_SHORTCUT_E, GUIDesignButtonToolbarCheckable);
    moveButton = new MFXCheckableButton(false, myViewNet->myToolbar, "\tset move mode\tMode for move elements.",
        GUIIconSubSys::getIcon(ICON_MODEMOVE), myViewNet, MID_GNE_SHORTCUT_M, GUIDesignButtonToolbarCheckable);
    deleteButton = new MFXCheckableButton(false, myViewNet->myToolbar, "\tset delete mode\tMode for delete elements.",
        GUIIconSubSys::getIcon(ICON_MODEDELETE), myViewNet, MID_GNE_SHORTCUT_D, GUIDesignButtonToolbarCheckable);
    inspectButton = new MFXCheckableButton(false, myViewNet->myToolbar, "\tset inspect mode\tMode for inspect elements and change their attributes.",
        GUIIconSubSys::getIcon(ICON_MODEINSPECT), myViewNet, MID_GNE_SHORTCUT_I, GUIDesignButtonToolbarCheckable);
    selectButton = new MFXCheckableButton(false, myViewNet->myToolbar, "\tset select mode\tMode for select elements.",
        GUIIconSubSys::getIcon(ICON_MODESELECT), myViewNet, MID_GNE_SHORTCUT_S, GUIDesignButtonToolbarCheckable);
    connectionButton = new MFXCheckableButton(false, myViewNet->myToolbar, "\tset connection mode\tMode for edit connections between lanes.",
        GUIIconSubSys::getIcon(ICON_MODECONNECTION), myViewNet, MID_GNE_SHORTCUT_C, GUIDesignButtonToolbarCheckable);
    prohibitionButton = new MFXCheckableButton(false, myViewNet->myToolbar, "\tset prohibition mode\tMode for editing connection prohibitions.",
        GUIIconSubSys::getIcon(ICON_MODEPROHIBITION), myViewNet, MID_GNE_SHORTCUT_W, GUIDesignButtonToolbarCheckable);
    trafficLightButton = new MFXCheckableButton(false, myViewNet->myToolbar, "\tset traffic light mode\tMode for edit traffic lights over junctions.",
        GUIIconSubSys::getIcon(ICON_MODETLS), myViewNet, MID_GNE_SHORTCUT_T, GUIDesignButtonToolbarCheckable);
    additionalButton = new MFXCheckableButton(false, myViewNet->myToolbar, "\tset additional mode\tMode for adding additional elements.",
        GUIIconSubSys::getIcon(ICON_MODEADDITIONAL), myViewNet, MID_GNE_SHORTCUT_A, GUIDesignButtonToolbarCheckable);
    crossingButton = new MFXCheckableButton(false, myViewNet->myToolbar, "\tset crossing mode\tMode for creating crossings between edges.",
        GUIIconSubSys::getIcon(ICON_MODECROSSING), myViewNet, MID_GNE_SHORTCUT_R, GUIDesignButtonToolbarCheckable);
    TAZButton = new MFXCheckableButton(false, myViewNet->myToolbar, "\tset TAZ mode\tMode for creating Traffic Assignment Zones.",
        GUIIconSubSys::getIcon(ICON_MODETAZ), myViewNet, MID_GNE_SHORTCUT_Z, GUIDesignButtonToolbarCheckable);
    shapeButton = new MFXCheckableButton(false, myViewNet->myToolbar, "\tset polygon mode\tMode for creating polygons and POIs.",
        GUIIconSubSys::getIcon(ICON_MODEPOLYGON), myViewNet, MID_GNE_SHORTCUT_P, GUIDesignButtonToolbarCheckable);
}


void 
GNEViewNet::NetworkCheckableButtons::showNetworkCheckableButtons() {
    createEdgeButton->show();
    moveButton->show();
    deleteButton->show();
    inspectButton->show();
    selectButton->show();
    connectionButton->show();
    trafficLightButton->show();
    additionalButton->show();
    crossingButton->show();
    TAZButton->show();
    shapeButton->show();
    prohibitionButton->show();
}


void 
GNEViewNet::NetworkCheckableButtons::hideNetworkCheckableButtons() {
    createEdgeButton->hide();
    moveButton->hide();
    deleteButton->hide();
    inspectButton->hide();
    selectButton->hide();
    connectionButton->hide();
    trafficLightButton->hide();
    additionalButton->hide();
    crossingButton->hide();
    TAZButton->hide();
    shapeButton->hide();
    prohibitionButton->hide();
}


void 
GNEViewNet::NetworkCheckableButtons::disableNetworkCheckableButtons() {
    createEdgeButton->setChecked(false);
    moveButton->setChecked(false);
    deleteButton->setChecked(false);
    inspectButton->setChecked(false);
    selectButton->setChecked(false);
    connectionButton->setChecked(false);
    trafficLightButton->setChecked(false);
    additionalButton->setChecked(false);
    crossingButton->setChecked(false);
    TAZButton->setChecked(false);
    shapeButton->setChecked(false);
    prohibitionButton->setChecked(false);
}


void 
GNEViewNet::NetworkCheckableButtons::updateNetworkCheckableButtons() {
    createEdgeButton->update();
    moveButton->update();
    deleteButton->update();
    inspectButton->update();
    selectButton->update();
    connectionButton->update();
    trafficLightButton->update();
    additionalButton->update();
    crossingButton->update();
    TAZButton->update();
    shapeButton->update();
    prohibitionButton->update();
}

// ---------------------------------------------------------------------------
// GNEViewNet::DemandCheckableButtons - methods
// ---------------------------------------------------------------------------

GNEViewNet::DemandCheckableButtons::DemandCheckableButtons(GNEViewNet* viewNet) : 
    myViewNet(viewNet),
    routeButton(nullptr) {
}


void 
GNEViewNet::DemandCheckableButtons::buildDemandCheckableButtons() {
    routeButton = new MFXCheckableButton(false, myViewNet->myToolbar, "\tcreate route mode\tMode for creating routes.",
        GUIIconSubSys::getIcon(ICON_MODEROUTE), myViewNet, MID_GNE_SHORTCUT_R, GUIDesignButtonToolbarCheckable);
}


void 
GNEViewNet::DemandCheckableButtons::showDemandCheckableButtons() {
    routeButton->show();
}


void 
GNEViewNet::DemandCheckableButtons::hideDemandCheckableButtons() {
    routeButton->hide();
}


void 
GNEViewNet::DemandCheckableButtons::disableDemandCheckableButtons() {
    routeButton->setChecked(false);
}


void 
GNEViewNet::DemandCheckableButtons::updateDemandCheckableButtons() {
    routeButton->update();
}

// ---------------------------------------------------------------------------
// Private methods
// ---------------------------------------------------------------------------

void 
GNEViewNet::drawLaneCandidates() const {
    if (myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->isSelectingLanes()) {
        // draw first point
        if(myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLanes().size() > 0) {
            // Push draw matrix
            glPushMatrix();
            // obtain first clicked point
            const Position &firstLanePoint = myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLanes().front().first->getShape().positionAtOffset(
                                             myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLanes().front().second);
            // must draw on top of other connections
            glTranslated(firstLanePoint.x(), firstLanePoint.y(), GLO_JUNCTION + 0.3);
            GLHelper::setColor(RGBColor::RED);
            // draw first point
            GLHelper::drawFilledCircle((double) 1.3, 8);
            GLHelper::drawText("S", Position(), .1, 1.3, RGBColor::CYAN);
            // pop draw matrix
            glPopMatrix();
        }
        // draw connections between lanes
        if(myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLanes().size() > 1) {
            // iterate over all current selected lanes
            for (int i = 0; i < (int)myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLanes().size() - 1; i++) {
                // declare position vector for shape
                PositionVector shape;
                // declare vectors for shape rotation and lenghts
                std::vector<double> shapeRotations, shapeLengths;
                // obtain GNELanes
                GNELane* from = myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLanes().at(i).first;
                GNELane* to = myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLanes().at(i+1).first;
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
            const Position &lastLanePoint = myViewParent->getAdditionalFrame()->getConsecutiveLaneSelector()->getSelectedLanes().back().first->getShape().positionAtOffset(
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
    if(myViewParent->getPolygonFrame()->getDrawingShapeModul()->isDrawing()) {
        temporalShape = myViewParent->getPolygonFrame()->getDrawingShapeModul()->getTemporalShape();
        deleteLastCreatedPoint = myViewParent->getPolygonFrame()->getDrawingShapeModul()->getDeleteLastCreatedPoint();
    } else if(myViewParent->getTAZFrame()->getDrawingShapeModul()->isDrawing()) {
        temporalShape = myViewParent->getTAZFrame()->getDrawingShapeModul()->getTemporalShape();
        deleteLastCreatedPoint = myViewParent->getTAZFrame()->getDrawingShapeModul()->getDeleteLastCreatedPoint();
    }
    // check if we're in drawing mode
    if(temporalShape.size() > 0) {
        // draw blue line with the current drawed shape
        glPushMatrix();
        glLineWidth(2);
        GLHelper::setColor(RGBColor::BLUE);
        GLHelper::drawLine(temporalShape);
        glPopMatrix();
        // draw red line from the last point of shape to the current mouse position
        glPushMatrix();
        glLineWidth(2);
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

/****************************************************************************/
