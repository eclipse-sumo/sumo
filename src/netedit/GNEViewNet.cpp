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
#include <config.h>

#include <iostream>
#include <utility>
#include <cmath>
#include <limits>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/foxtools/MFXImageHelper.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/common/RGBColor.h>
#include <utils/options/OptionsCont.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIDanielPerspectiveChanger.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/windows/GUIDialog_ViewSettings.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/xml/XMLSubSys.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBNode.h>
#include <netedit/frames/GNEInspectorFrame.h>
#include <netedit/frames/GNESelectorFrame.h>
#include <netedit/frames/GNEConnectorFrame.h>
#include <netedit/frames/GNETLSEditorFrame.h>
#include <netedit/frames/GNEAdditionalFrame.h>
#include <netedit/frames/GNECrossingFrame.h>
#include <netedit/frames/GNEPolygonFrame.h>
#include <netedit/frames/GNEDeleteFrame.h>
#include <netedit/frames/GNEProhibitionFrame.h>
#include <netedit/netelements/GNECrossing.h>
#include <netedit/dialogs/GNEAdditionalDialog.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/additionals/GNEPOI.h>
#include <netedit/additionals/GNEAdditionalHandler.h>
#include <netedit/additionals/GNEPoly.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/additionals/GNERerouter.h>

#include "GNEViewNet.h"
#include "GNEApplicationWindow.h"
#include "GNEViewParent.h"
#include "GNENet.h"
#include "GNEUndoList.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEViewNet) GNEViewNetMap[] = {
    // Modes
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SETMODE_CREATE_EDGE,             GNEViewNet::onCmdSetModeCreateEdge),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SETMODE_MOVE,                    GNEViewNet::onCmdSetModeMove),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SETMODE_DELETE,                  GNEViewNet::onCmdSetModeDelete),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SETMODE_INSPECT,                 GNEViewNet::onCmdSetModeInspect),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SETMODE_SELECT,                  GNEViewNet::onCmdSetModeSelect),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SETMODE_CONNECT,                 GNEViewNet::onCmdSetModeConnect),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SETMODE_TLS,                     GNEViewNet::onCmdSetModeTLS),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SETMODE_ADDITIONAL,              GNEViewNet::onCmdSetModeAdditional),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SETMODE_CROSSING,                GNEViewNet::onCmdSetModeCrossing),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SETMODE_POLYGON,                 GNEViewNet::onCmdSetModePolygon),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SETMODE_PROHIBITION,             GNEViewNet::onCmdSetModeProhibition),
    // Viewnet
    FXMAPFUNC(SEL_COMMAND, MID_GNE_VIEWNET_SHOW_CONNECTIONS,        GNEViewNet::onCmdToogleShowConnection),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_VIEWNET_SELECT_EDGES,            GNEViewNet::onCmdToogleSelectEdges),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_VIEWNET_SHOW_BUBBLES,            GNEViewNet::onCmdToogleShowBubbles),
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
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_TRANSFORM_SIDEWALK,         GNEViewNet::onCmdRestrictLaneSidewalk),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_TRANSFORM_BIKE,             GNEViewNet::onCmdRestrictLaneBikelane),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_TRANSFORM_BUS,              GNEViewNet::onCmdRestrictLaneBuslane),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_TRANSFORM_GREENVERGE,       GNEViewNet::onCmdRestrictLaneGreenVerge),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_ADD_SIDEWALK,               GNEViewNet::onCmdAddRestrictedLaneSidewalk),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_ADD_BIKE,                   GNEViewNet::onCmdAddRestrictedLaneBikelane),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_ADD_BUS,                    GNEViewNet::onCmdAddRestrictedLaneBuslane),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_ADD_GREENVERGE,             GNEViewNet::onCmdAddRestrictedLaneGreenVerge),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_REMOVE_SIDEWALK,            GNEViewNet::onCmdRemoveRestrictedLaneSidewalk),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_REMOVE_BIKE,                GNEViewNet::onCmdRemoveRestrictedLaneBikelane),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_REMOVE_BUS,                 GNEViewNet::onCmdRemoveRestrictedLaneBuslane),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_REMOVE_GREENVERGE,          GNEViewNet::onCmdRemoveRestrictedLaneGreenVerge),
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

GNEViewNet::GNEViewNet(FXComposite* tmpParent, FXComposite* actualParent, GUIMainWindow& app,
                       GNEViewParent* viewParent, GNENet* net, GNEUndoList* undoList,
                       FXGLVisual* glVis, FXGLCanvas* share, FXToolBar* toolBar) :
    GUISUMOAbstractView(tmpParent, app, viewParent, net->getVisualisationSpeedUp(), glVis, share),
    myViewParent(viewParent),
    myNet(net),
    myEditMode(GNE_MODE_MOVE),
    myCurrentFrame(0),
    myShowConnections(false),
    mySelectEdges(true),
    myShiftKeyPressed(false),
    myCreateEdgeSource(0),
    myMovingSelection(false),
    myToolbar(toolBar),
    myEditModeCreateEdge(0),
    myEditModeMove(0),
    myEditModeDelete(0),
    myEditModeInspect(0),
    myEditModeSelect(0),
    myEditModeConnection(0),
    myEditModeTrafficLight(0),
    myEditModeAdditional(0),
    myEditModeCrossing(0),
    myEditModePolygon(0),
    myEditModeProhibition(0),
    myEditModeNames(),
    myUndoList(undoList),
    myEditShapePoly(0) {
    // view must be the final member of actualParent
    reparent(actualParent);

    buildEditModeControls();
    myUndoList->mark();
    myNet->setViewNet(this);

    ((GUIDanielPerspectiveChanger*)myChanger)->setDragDelay(100000000); // 100 milliseconds

    // Reset textures
    GUITextureSubSys::resetTextures();

    if (myTestingMode.testingEnabled && OptionsCont::getOptions().isSet("window-size")) {
        std::vector<std::string> windowSize = OptionsCont::getOptions().getStringVector("window-size");
        if (windowSize.size() == 2 && GNEAttributeCarrier::canParse<int>(windowSize[0]) && GNEAttributeCarrier::canParse<int>(windowSize[1])) {
            myTestingMode.testingWidth = GNEAttributeCarrier::parse<int>(windowSize[0]);
            myTestingMode.testingHeight = GNEAttributeCarrier::parse<int>(windowSize[1]);
        } else {
            WRITE_ERROR("Invalid windows size-format: " + toString(windowSize) + "for option 'window-size'");
        }
    }
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
    if (myVisualizationChanger != 0) {
        if (myVisualizationChanger->getCurrentScheme() != name) {
            myVisualizationChanger->setCurrentScheme(name);
        }
    }
    myVisualizationSettings = &gSchemeStorage.get(name.c_str());
    update();
    return true;
}


void
GNEViewNet::buildColorRainbow(GUIColorScheme& scheme, int active, GUIGlObjectType objectType) {
    assert(!scheme.isFixed());
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
            const double val = lane->getColorValue(active);
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
GNEViewNet::selectEdges() {
    return mySelectEdges;
}


bool
GNEViewNet::showConnections() {
    if (myEditMode == GNE_MODE_CONNECT) {
        return myMenuCheckHideConnections->getCheck() == 0;
    } else if (myEditMode == GNE_MODE_PROHIBITION) {
        return true;
    } else if (myMenuCheckShowConnections->shown() == false) {
        return false;
    } else {
        return (myVisualizationSettings->showLane2Lane);
    }
}


bool
GNEViewNet::autoSelectNodes() {
    return (myMenuCheckExtendToEdgeNodes->getCheck() != 0);
}


void
GNEViewNet::setSelectionScaling(double selectionScale) {
    myVisualizationSettings->selectionScale = selectionScale;
}


bool
GNEViewNet::changeAllPhases() const {
    return (myMenuCheckChangeAllPhases->getCheck() != 0);
}


bool
GNEViewNet::showJunctionAsBubbles() const {
    return (myEditMode == GNE_MODE_MOVE) && (myMenuCheckShowBubbleOverJunction->getCheck());
}


void
GNEViewNet::startEditCustomShape(GNENetElement* element, const PositionVector& shape, bool fill) {
    if ((myEditShapePoly == nullptr) && (element != nullptr) && (shape.size() > 1)) {
        // save current edit mode before starting
        myPreviousEditMode = myEditMode;
        setEditModeFromHotkey(MID_GNE_SETMODE_MOVE);
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
    if (myEditShapePoly != 0) {
        myNet->removePolygonForEditShapes(myEditShapePoly);
        myEditShapePoly = 0;
        // restore previous edit mode
        if (myEditMode != myPreviousEditMode) {
            setEditMode(myPreviousEditMode);
        }
    }
}


void
GNEViewNet::beginMoveSelection(GNEAttributeCarrier* originAC, const Position& originPosition) {
    // enable moving selection
    myMovingSelection = true;
    // obtain Junctions and edges selected
    std::vector<GNEJunction*> selectedJunctions = myNet->retrieveJunctions(true);
    std::vector<GNEEdge*> selectedEdges = myNet->retrieveEdges(true);
    // Junctions are always moved, then save position of current selected junctions (Needed when mouse is released)
    for (auto i : selectedJunctions) {
        myOriginPositionOfMovedJunctions[i] = i->getPositionInView();
    }
    // make special movement depending of clicked AC
    if (originAC->getTag() == SUMO_TAG_JUNCTION) {
        // if clicked element is a junction, move shapes of all selected edges
        for (auto i : selectedEdges) {
            myOriginShapesMovedEntireShapes[i] = i->getNBEdge()->getInnerGeometry();
        }
    } else if (originAC->getTag() == SUMO_TAG_EDGE) {
        // obtain clicked edge
        GNEEdge* clickedEdge = dynamic_cast<GNEEdge*>(originAC);
        // if clicked edge has origin and destiny junction selected, move shapes of all selected edges
        if ((myOriginPositionOfMovedJunctions.count(clickedEdge->getGNEJunctionSource()) > 0 &&
                myOriginPositionOfMovedJunctions.count(clickedEdge->getGNEJunctionDestiny()) > 0)) {
            for (auto i : selectedEdges) {
                myOriginShapesMovedEntireShapes[i] = i->getNBEdge()->getInnerGeometry();
            }
        } else {
            // declare three groups for dividing edges
            std::vector<GNEEdge*> noJunctionsSelected;
            std::vector<GNEEdge*> originJunctionSelected;
            std::vector<GNEEdge*> destinyJunctionSelected;
            // divide selected edges into four groups, depending of the selection of their junctions
            for (auto i : selectedEdges) {
                bool originSelected = myOriginPositionOfMovedJunctions.count(i->getGNEJunctionSource()) > 0;
                bool destinySelected = myOriginPositionOfMovedJunctions.count(i->getGNEJunctionDestiny()) > 0;
                // bot junctions selected
                if (!originSelected && !destinySelected) {
                    noJunctionsSelected.push_back(i);
                } else if (originSelected && !destinySelected) {
                    originJunctionSelected.push_back(i);
                } else if (!originSelected && destinySelected) {
                    destinyJunctionSelected.push_back(i);
                } else if (!originSelected && !destinySelected) {
                    myOriginShapesMovedEntireShapes[i] = i->getNBEdge()->getInnerGeometry();
                }
            }
            // saved old position of Edges which both junction isn't noJunctionsSelected
            for (auto i : noJunctionsSelected) {
                myOriginShapesMovedPartialShapes[i].originalShape = i->getNBEdge()->getInnerGeometry();
            }
            // obtain index shape of clicked edge and move it
            int index = clickedEdge->getVertexIndex(originPosition);
            assert(index >= 0);
            myOriginShapesMovedPartialShapes[clickedEdge].index = index;
            Position originPositionOnClicked = clickedEdge->getNBEdge()->getInnerGeometry()[index];
            myOriginShapesMovedPartialShapes[clickedEdge].originalPosition = originPositionOnClicked;
            // find/create point to move for other edges
            for (auto i : noJunctionsSelected) {
                if (i != clickedEdge) {
                    int index = i->getVertexIndex(originPositionOnClicked);
                    myOriginShapesMovedPartialShapes[i].index = index;
                    if (index >= 0) {
                        myOriginShapesMovedPartialShapes[i].originalPosition = i->getNBEdge()->getInnerGeometry()[index];
                    }
                }
            }
        }
    }
}


void
GNEViewNet::moveSelection(const Position& offset) {
    // move selected junctions
    for (auto i : myOriginPositionOfMovedJunctions) {
        i.first->moveGeometry(i.second, offset);
    }

    // move entire edge shapes
    for (auto i : myOriginShapesMovedEntireShapes) {
        i.first->moveEntireShape(i.second, offset);
    }

    // move partial shapes
    for (auto i : myOriginShapesMovedPartialShapes) {
        i.first->moveVertexShape(i.second.index, i.second.originalPosition, offset);
    }
}


void
GNEViewNet::finishMoveSelection() {
    myUndoList->p_begin("position of selected elements");
    // commit positions of moved junctions
    for (auto i : myOriginPositionOfMovedJunctions) {
        i.first->commitGeometryMoving(i.second, myUndoList);
    }
    myOriginPositionOfMovedJunctions.clear();

    // commit shapes of entired moved edges
    for (auto i : myOriginShapesMovedEntireShapes) {
        i.first->commitShapeChange(i.second, myUndoList);
    }
    myOriginShapesMovedEntireShapes.clear();

    //commit shapes of partial moved shapes
    for (auto i : myOriginShapesMovedPartialShapes) {
        i.first->commitShapeChange(i.second.originalShape, myUndoList);
    }
    myOriginShapesMovedPartialShapes.clear();

    myUndoList->p_end();
    myMovingSelection = false;
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
    if (mySelectingArea.selectingUsingRectangle) {
        glPushMatrix();
        glTranslated(0, 0, GLO_MAX - 1);
        GLHelper::setColor(GNENet::selectionColor);
        glLineWidth(2);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin(GL_QUADS);
        glVertex2d(mySelectingArea.selectionCorner1.x(), mySelectingArea.selectionCorner1.y());
        glVertex2d(mySelectingArea.selectionCorner1.x(), mySelectingArea.selectionCorner2.y());
        glVertex2d(mySelectingArea.selectionCorner2.x(), mySelectingArea.selectionCorner2.y());
        glVertex2d(mySelectingArea.selectionCorner2.x(), mySelectingArea.selectionCorner1.y());
        glEnd();
        glPopMatrix();
    }

    // compute lane width
    double lw = m2p(SUMO_const_laneWidth);
    // draw decals (if not in grabbing mode)
    if (!myUseToolTips && !myVisualizationSettings->drawForSelecting) {
        drawDecals();
        // depending of the visualizationSettings, enable or disable check box show grid
        if (myVisualizationSettings->showGrid) {
            myMenuCheckShowGrid->setCheck(true);
            paintGLGrid();
        } else {
            myMenuCheckShowGrid->setCheck(false);
        }
        myMenuCheckShowConnections->setCheck(myVisualizationSettings->showLane2Lane);
        if (myTestingMode.testingEnabled) {
            if (myTestingMode.testingWidth > 0 && (getWidth() != myTestingMode.testingWidth || getHeight() != myTestingMode.testingHeight)) {
                // only resize once to avoid flickering
                //std::cout << " before resize: view=" << getWidth() << ", " << getHeight() << " app=" << myApp->getWidth() << ", " << myApp->getHeight() << "\n";
                myApp->resize(myTestingMode.testingWidth + myTestingMode.testingWidth - getWidth(), myTestingMode.testingHeight + myTestingMode.testingHeight - getHeight());
                //std::cout << " directly after resize: view=" << getWidth() << ", " << getHeight() << " app=" << myApp->getWidth() << ", " << myApp->getHeight() << "\n";
                myTestingMode.testingWidth = 0;
            }
            //std::cout << " fixed: view=" << getWidth() << ", " << getHeight() << " app=" << myApp->getWidth() << ", " << myApp->getHeight() << "\n";
            // draw pink square in the upper left corner on top of everything
            glPushMatrix();
            const double size = p2m(32);
            Position center = screenPos2NetPos(8, 8);
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
            // show box with the current position relative to pink square
            Position posRelative = screenPos2NetPos(getWidth() - 40, getHeight() - 20);
            // adjust cursor position (24,25) to show exactly the same position as in function netedit.leftClick(match, X, Y)
            GLHelper::drawTextBox(toString(myWindowCursorPositionX - 24) + " " + toString(myWindowCursorPositionY - 25), posRelative, GLO_MAX - 1, p2m(20), RGBColor::BLACK, RGBColor::WHITE);
        }
    }

    // draw temporal shape of polygon during drawing
    if (!myVisualizationSettings->drawForSelecting && myViewParent->getPolygonFrame()->getDrawingMode()->isDrawing()) {
        const PositionVector& temporalDrawingShape = myViewParent->getPolygonFrame()->getDrawingMode()->getTemporalShape();
        // draw blue line with the current drawed shape
        glPushMatrix();
        glLineWidth(2);
        GLHelper::setColor(RGBColor::BLUE);
        GLHelper::drawLine(temporalDrawingShape);
        glPopMatrix();
        // draw red line from the last point of shape to the current mouse position
        if (temporalDrawingShape.size() > 0) {
            glPushMatrix();
            glLineWidth(2);
            // draw last line depending if shift key (delete last created point) is pressed
            if (myViewParent->getPolygonFrame()->getDrawingMode()->getDeleteLastCreatedPoint()) {
                GLHelper::setColor(RGBColor::RED);
            } else {
                GLHelper::setColor(RGBColor::GREEN);
            }
            GLHelper::drawLine(temporalDrawingShape.back(), snapToActiveGrid(getPositionInformation()));
            glPopMatrix();
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
    myVisualizationSettings->editMode = myEditMode;
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
    FXEvent* evt = (FXEvent*) eventData;
    setFocus();
    // interpret object under cursor
    if (makeCurrent()) {
        // first update objects under cursor
        myObjectsUnderCursor.updateObjectUnderCursor(getObjectUnderCursor(), myEditShapePoly, evt);
        // decide what to do based on mode
        switch (myEditMode) {
            case GNE_MODE_CREATE_EDGE: {
                // make sure that Control key isn't pressed
                if (!myObjectsUnderCursor.controlKeyPressed()) {
                    // allow moving when control is held down
                    if (!myUndoList->hasCommandGroup()) {
                        myUndoList->p_begin("create new " + toString(SUMO_TAG_EDGE));
                    }
                    if (!myObjectsUnderCursor.junction) {
                        myObjectsUnderCursor.junction = myNet->createJunction(snapToActiveGrid(getPositionInformation()), myUndoList);
                    }
                    if (myCreateEdgeSource == 0) {
                        myCreateEdgeSource = myObjectsUnderCursor.junction;
                        myCreateEdgeSource->markAsCreateEdgeSource();
                        update();
                    } else {
                        if (myCreateEdgeSource != myObjectsUnderCursor.junction) {
                            // may fail to prevent double edges
                            GNEEdge* newEdge = myNet->createEdge(
                                                   myCreateEdgeSource, myObjectsUnderCursor.junction, myViewParent->getInspectorFrame()->getTemplateEditor()->getEdgeTemplate(), myUndoList);
                            if (newEdge) {
                                // create another edge, if create opposite edge is enabled
                                if (myAutoCreateOppositeEdge->getCheck()) {
                                    myNet->createEdge(myObjectsUnderCursor.junction, myCreateEdgeSource, myViewParent->getInspectorFrame()->getTemplateEditor()->getEdgeTemplate(), myUndoList, "-" + newEdge->getNBEdge()->getID());
                                }
                                myCreateEdgeSource->unMarkAsCreateEdgeSource();

                                if (myUndoList->hasCommandGroup()) {
                                    myUndoList->p_end();
                                } else {
                                    std::cout << "edge created without an open CommandGroup )-:\n";
                                }
                                if (myChainCreateEdge->getCheck()) {
                                    myCreateEdgeSource = myObjectsUnderCursor.junction;
                                    myCreateEdgeSource->markAsCreateEdgeSource();
                                    myUndoList->p_begin("create new " + toString(SUMO_TAG_EDGE));
                                } else {
                                    myCreateEdgeSource = 0;
                                }
                            } else {
                                setStatusBarText("An " + toString(SUMO_TAG_EDGE) + " with the same geometry already exists!");
                            }
                        } else {
                            setStatusBarText("Start- and endpoint for an " + toString(SUMO_TAG_EDGE) + " must be distinct!");
                        }
                        update();
                    }
                }
                // process click
                processClick(evt, eventData);
                break;
            }
            case GNE_MODE_MOVE: {
                // first obtain moving reference (common for all)
                myMoveSingleElementValues.movingReference = snapToActiveGrid(getPositionInformation());
                // check what type of AC will be moved
                if (myObjectsUnderCursor.poly) {
                    // set Poly to move
                    myMovedItems.polyToMove = myObjectsUnderCursor.poly;
                    // save original shape (needed for commit change)
                    myMoveSingleElementValues.movingOriginalShape = myMovedItems.polyToMove->getShape();
                    // save clicked position as moving original position
                    myMoveSingleElementValues.movingOriginalPosition = getPositionInformation();
                    // obtain index of vertex to move if shape isn't blocked
                    if ((myMovedItems.polyToMove->isShapeBlocked() == false) && (myMovedItems.polyToMove->isMovementBlocked() == false)) {
                        // check if we want to remove a Geometry Point
                        if (myShiftKeyPressed) {
                            // check if we're clicked over a Geometry Point
                            myMoveSingleElementValues.movingIndexShape = myMovedItems.polyToMove->getVertexIndex(myMoveSingleElementValues.movingOriginalPosition, false);
                            if (myMoveSingleElementValues.movingIndexShape != -1) {
                                myMovedItems.polyToMove->deleteGeometryPoint(myMoveSingleElementValues.movingOriginalPosition);
                                // after removing Geomtery Point, reset PolyToMove
                                myMovedItems.polyToMove = nullptr;
                            }
                        } else {
                            // obtain index of vertex to move and moving reference
                            myMoveSingleElementValues.movingIndexShape = myMovedItems.polyToMove->getVertexIndex(myMoveSingleElementValues.movingOriginalPosition);
                        }
                    } else {
                        myMoveSingleElementValues.movingIndexShape = -1;
                    }
                } else if (myObjectsUnderCursor.poi) {
                    myMovedItems.poiToMove = myObjectsUnderCursor.poi;
                    // Save original Position of Element
                    myMoveSingleElementValues.movingOriginalPosition = myMovedItems.poiToMove->getPositionInView();
                } else if (myObjectsUnderCursor.junction) {
                    if (myObjectsUnderCursor.junction->isAttributeCarrierSelected()) {
                        beginMoveSelection(myObjectsUnderCursor.junction, getPositionInformation());
                    } else {
                        myMovedItems.junctionToMove = myObjectsUnderCursor.junction;
                        myMovedItems.junctionToMove->startGeometryMoving();
                    }
                    // Save original Position of Element
                    myMoveSingleElementValues.movingOriginalPosition = myObjectsUnderCursor.junction->getPositionInView();
                } else if (myObjectsUnderCursor.edge || myObjectsUnderCursor.lane) {
                    // allways swap lanes to edges in movement mode
                    if (myObjectsUnderCursor.lane) {
                        myObjectsUnderCursor.swapLane2Edge();
                    }
                    if (myObjectsUnderCursor.edge->isAttributeCarrierSelected()) {
                        beginMoveSelection(myObjectsUnderCursor.edge, getPositionInformation());
                    } else if (myObjectsUnderCursor.shiftKeyPressed()) {
                        myObjectsUnderCursor.edge->editEndpoint(getPositionInformation(), myUndoList);
                    } else {
                        myMovedItems.edgeToMove = myObjectsUnderCursor.edge;
                        if (myMovedItems.edgeToMove->clickedOverShapeStart(getPositionInformation())) {
                            myMoveSingleElementValues.movingOriginalPosition = myMovedItems.edgeToMove->getNBEdge()->getGeometry().front();
                            myMoveSingleElementValues.movingStartPos = true;
                        } else if (myMovedItems.edgeToMove->clickedOverShapeEnd(getPositionInformation())) {
                            myMoveSingleElementValues.movingOriginalPosition = myMovedItems.edgeToMove->getNBEdge()->getGeometry().back();
                            myMoveSingleElementValues.movingEndPos = true;
                        } else {
                            // save original shape (needed for commit change)
                            myMoveSingleElementValues.movingOriginalShape = myMovedItems.edgeToMove->getNBEdge()->getInnerGeometry();
                            // obtain index of vertex to move and moving reference
                            myMoveSingleElementValues.movingIndexShape = myMovedItems.edgeToMove->getVertexIndex(getPositionInformation());
                            myMoveSingleElementValues.movingOriginalPosition = getPositionInformation();
                            // start moving AFTER getvertex Index
                            myMovedItems.edgeToMove->startGeometryMoving();
                        }
                    }
                } else if (myObjectsUnderCursor.additional) {
                    if (myObjectsUnderCursor.additional->isAttributeCarrierSelected()) {
                        myMovingSelection = true;
                    } else {
                        myMovedItems.additionalToMove = myObjectsUnderCursor.additional;
                        // start geometry moving
                        myMovedItems.additionalToMove->startGeometryMoving();
                        // Save original Position of Element
                        myMoveSingleElementValues.movingOriginalPosition = myMovedItems.additionalToMove->getPositionInView();
                    }
                } else {
                    // process click
                    processClick(evt, eventData);
                }
                update();
                break;
            }
            case GNE_MODE_DELETE: {
                if (myObjectsUnderCursor.attributeCarrier) {
                    // change the selected attribute carrier if mySelectEdges is enabled and clicked element is a lane
                    if (mySelectEdges && (myObjectsUnderCursor.attributeCarrier->getTag() == SUMO_TAG_LANE)) {
                        myObjectsUnderCursor.swapLane2Edge();
                    }
                    // check if we are deleting a selection or an single attribute carrier
                    if (myObjectsUnderCursor.attributeCarrier->isAttributeCarrierSelected()) {
                        // before delete al selected attribute carriers, check if we clicked over a geometry point
                        if (myViewParent->getDeleteFrame()->getDeleteOptions()->deleteOnlyGeometryPoints() &&
                                ((myObjectsUnderCursor.edge && (myObjectsUnderCursor.edge->getVertexIndex(getPositionInformation(), false) != -1)) ||
                                 (myObjectsUnderCursor.poly && (myObjectsUnderCursor.poly->getVertexIndex(getPositionInformation(), false) != -1)))) {
                            myViewParent->getDeleteFrame()->removeAttributeCarrier(myObjectsUnderCursor.attributeCarrier);
                        } else {
                            myViewParent->getDeleteFrame()->removeSelectedAttributeCarriers();
                        }
                    } else {
                        myViewParent->getDeleteFrame()->removeAttributeCarrier(myObjectsUnderCursor.attributeCarrier);
                    }
                } else {
                    // process click
                    processClick(evt, eventData);
                }
                break;
            }
            case GNE_MODE_INSPECT: {
                if (myObjectsUnderCursor.attributeCarrier) {
                    // change the selected attribute carrier if mySelectEdges is enabled and clicked element is a lane
                    if (mySelectEdges && (myObjectsUnderCursor.attributeCarrier->getTag() == SUMO_TAG_LANE)) {
                        myObjectsUnderCursor.swapLane2Edge();
                    }
                    // if Control key is Pressed, select instead inspect element
                    if (myObjectsUnderCursor.controlKeyPressed()) {
                        // Check if this GLobject type is locked
                        if (!myViewParent->getSelectorFrame()->getLockGLObjectTypes()->IsObjectTypeLocked(myObjectsUnderCursor.glType)) {
                            // toogle netElement selection
                            if (myObjectsUnderCursor.attributeCarrier->isAttributeCarrierSelected()) {
                                myObjectsUnderCursor.attributeCarrier->unselectAttributeCarrier();
                            } else {
                                myObjectsUnderCursor.attributeCarrier->selectAttributeCarrier();
                            }
                        }
                    } else {
                        // inspect attribute carrier, or multiseletion if AC is selected
                        myViewParent->getInspectorFrame()->inspectElement(myObjectsUnderCursor.attributeCarrier);
                        // focus upper element of inspector frame
                        myViewParent->getInspectorFrame()->focusUpperElement();
                    }
                }
                // process click
                processClick(evt, eventData);
                update();
                break;
            }
            case GNE_MODE_SELECT:
                // first check that under cursor there is an attribute carrier and is selectable
                if (myObjectsUnderCursor.attributeCarrier) {
                    // change the selected attribute carrier if mySelectEdges is enabled and clicked element is a lane
                    if (mySelectEdges && (myObjectsUnderCursor.attributeCarrier->getTag() == SUMO_TAG_LANE)) {
                        myObjectsUnderCursor.swapLane2Edge();
                    }
                    // Check if this GLobject type is locked
                    if (!myViewParent->getSelectorFrame()->getLockGLObjectTypes()->IsObjectTypeLocked(myObjectsUnderCursor.glType)) {
                        // toogle netElement selection
                        if (myObjectsUnderCursor.attributeCarrier->isAttributeCarrierSelected()) {
                            myObjectsUnderCursor.attributeCarrier->unselectAttributeCarrier();
                        } else {
                            myObjectsUnderCursor.attributeCarrier->selectAttributeCarrier();
                        }
                        // update selector frame
                        if (myViewParent->getSelectorFrame()->shown()) {
                            myViewParent->getSelectorFrame()->getLockGLObjectTypes()->updateLockGLObjectTypes();
                        }
                    }
                }
                // check if a rect for selecting is being created
                if (myObjectsUnderCursor.shiftKeyPressed()) {
                    mySelectingArea.selectingUsingRectangle = true;
                    mySelectingArea.selectionCorner1 = getPositionInformation();
                    mySelectingArea.selectionCorner2 = getPositionInformation();
                } else {
                    // process click
                    processClick(evt, eventData);
                }
                update();
                break;
            case GNE_MODE_CONNECT: {
                if (myObjectsUnderCursor.lane) {
                    // shift key may pass connections, Control key allow conflicts.
                    myViewParent->getConnectorFrame()->handleLaneClick(myObjectsUnderCursor.lane, myObjectsUnderCursor.shiftKeyPressed(), myObjectsUnderCursor.controlKeyPressed(), true);
                    update();
                }
                // process click
                processClick(evt, eventData);
                break;
            }
            case GNE_MODE_TLS: {
                if (myObjectsUnderCursor.junction) {
                    myViewParent->getTLSEditorFrame()->editJunction(myObjectsUnderCursor.junction);
                    update();
                }
                // process click
                processClick(evt, eventData);
                break;
            }
            case GNE_MODE_ADDITIONAL: {
                GNEAdditionalFrame::AddAdditionalResult result = myViewParent->getAdditionalFrame()->addAdditional(myObjectsUnderCursor.netElement, myObjectsUnderCursor.additional);
                // process click or update view depending of the result of "add additional"
                if ((result == GNEAdditionalFrame::ADDADDITIONAL_SUCCESS) || (result == GNEAdditionalFrame::ADDADDITIONAL_INVALID_PARENT)) {
                    update();
                    // process click
                    processClick(evt, eventData);
                }
                break;
            }
            case GNE_MODE_CROSSING: {
                if (myObjectsUnderCursor.crossing == nullptr) {
                    if (myViewParent->getCrossingFrame()->addCrossing(myObjectsUnderCursor.netElement)) {
                        update();
                    }
                }
                // process click
                processClick(evt, eventData);
                break;
            }
            case GNE_MODE_POLYGON: {
                if (myObjectsUnderCursor.poi == nullptr) {
                    GNEPolygonFrame::AddShapeResult result = myViewParent->getPolygonFrame()->processClick(snapToActiveGrid(getPositionInformation()), myObjectsUnderCursor.lane);
                    // view net must be always update
                    update();
                    // process click depending of the result of "process click"
                    if ((result != GNEPolygonFrame::ADDSHAPE_UPDATEDTEMPORALSHAPE)) {
                        // process click
                        processClick(evt, eventData);
                    }
                }
                break;
            }
            case GNE_MODE_PROHIBITION: {
                if (myObjectsUnderCursor.connection) {
                    // shift key may pass connections, Control key allow conflicts.
                    myViewParent->getProhibitionFrame()->handleConnectionClick(myObjectsUnderCursor.connection, myObjectsUnderCursor.shiftKeyPressed(), myObjectsUnderCursor.controlKeyPressed(), true);
                    update();
                }
                // process click
                processClick(evt, eventData);
                break;
            }
            default: {
                // process click
                processClick(evt, eventData);
            }
        }
        makeNonCurrent();
    }
    return 1;
}


long
GNEViewNet::onLeftBtnRelease(FXObject* obj, FXSelector sel, void* eventData) {
    GUISUMOAbstractView::onLeftBtnRelease(obj, sel, eventData);
    if (myMovingSelection) {
        finishMoveSelection();
    } else if (myMovedItems.polyToMove) {
        myMovedItems.polyToMove->commitShapeChange(myMoveSingleElementValues.movingOriginalShape, myUndoList);
        myMovedItems.polyToMove = 0;
    } else if (myMovedItems.poiToMove) {
        myMovedItems.poiToMove->commitGeometryMoving(myMoveSingleElementValues.movingOriginalPosition, myUndoList);
        myMovedItems.poiToMove = 0;
    } else if (myMovedItems.junctionToMove) {
        // finish Junction movement
        myMovedItems.junctionToMove->endGeometryMoving();
        // position is already up to date but we must register with myUndoList
        if (!mergeJunctions(myMovedItems.junctionToMove, myMoveSingleElementValues.movingOriginalPosition)) {
            myMovedItems.junctionToMove->commitGeometryMoving(myMoveSingleElementValues.movingOriginalPosition, myUndoList);
        }
        myMovedItems.junctionToMove = 0;
    } else if (myMovedItems.edgeToMove) {
        if (myMoveSingleElementValues.movingStartPos) {
            myMovedItems.edgeToMove->commitShapeStartChange(myMoveSingleElementValues.movingOriginalPosition, myUndoList);
            myMoveSingleElementValues.movingStartPos = false;
        } else if (myMoveSingleElementValues.movingEndPos) {
            myMovedItems.edgeToMove->commitShapeEndChange(myMoveSingleElementValues.movingOriginalPosition, myUndoList);
            myMoveSingleElementValues.movingEndPos = false;
        } else {
            myMovedItems.edgeToMove->commitShapeChange(myMoveSingleElementValues.movingOriginalShape, myUndoList);
        }
        myMovedItems.edgeToMove = 0;
    } else if (myMovedItems.additionalToMove) {
        myMovedItems.additionalToMove->endGeometryMoving();
        myMovedItems.additionalToMove->commitGeometryMoving(myMoveSingleElementValues.movingOriginalPosition, myUndoList);
        myMovedItems.additionalToMove = 0;
    } else if (mySelectingArea.selectingUsingRectangle) {
        bool shiftKeyPressed = ((FXEvent*)eventData)->state & SHIFTMASK;
        mySelectingArea.processSelection(this, shiftKeyPressed);
        // update selector frame
        if (myViewParent->getSelectorFrame()->shown()) {
            myViewParent->getSelectorFrame()->getLockGLObjectTypes()->updateLockGLObjectTypes();
        }
    }
    update();
    return 1;
}


long GNEViewNet::onRightBtnPress(FXObject* obj, FXSelector sel, void* eventData) {
    // disable right button press during drawing polygon
    if ((myEditMode == GNE_MODE_POLYGON) && myViewParent->getPolygonFrame()->getDrawingMode()->isDrawing()) {
        return 1;
    } else {
        return GUISUMOAbstractView::onRightBtnPress(obj, sel, eventData);
    }
}


long GNEViewNet::onRightBtnRelease(FXObject* obj, FXSelector sel, void* eventData) {
    // disable right button release during drawing polygon
    if ((myEditMode == GNE_MODE_POLYGON) && myViewParent->getPolygonFrame()->getDrawingMode()->isDrawing()) {
        return 1;
    } else {
        return GUISUMOAbstractView::onRightBtnRelease(obj, sel, eventData);
    }
}


long
GNEViewNet::onMouseMove(FXObject* obj, FXSelector sel, void* eventData) {
    GUISUMOAbstractView::onMouseMove(obj, sel, eventData);
    // update shift key pressed
    myShiftKeyPressed = ((FXEvent*)eventData)->state & SHIFTMASK;
    // change "delete last created point" depending if during movement shift key is pressed
    if ((myEditMode == GNE_MODE_POLYGON) && myViewParent->getPolygonFrame()->getDrawingMode()->isDrawing()) {
        myViewParent->getPolygonFrame()->getDrawingMode()->setDeleteLastCreatedPoint(myShiftKeyPressed);
    }
    // calculate offset of movement depending of showGrid
    Position offsetMovement;
    if (myVisualizationSettings->showGrid) {
        offsetMovement = snapToActiveGrid(getPositionInformation()) - myMoveSingleElementValues.movingOriginalPosition;
        if (myMenuCheckMoveElevation->getCheck()) {
            const double dist = int((offsetMovement.y() + offsetMovement.x()) / myVisualizationSettings->gridXSize) * myVisualizationSettings->gridXSize;
            offsetMovement = Position(0, 0, dist / 10);
        }
    } else {
        offsetMovement = getPositionInformation() - myMoveSingleElementValues.movingReference;
        if (myMenuCheckMoveElevation->getCheck()) {
            offsetMovement = Position(0, 0, (offsetMovement.y() + offsetMovement.x()) / 10);
        }
    }
    // @note  #3521: Add checkBox to allow moving elements... has to behere implemented
    // check what type of additional is moved
    if (myMovingSelection) {
        moveSelection(offsetMovement);
    } else if (myMovedItems.polyToMove) {
        // move shape's geometry without commiting changes
        if (myMovedItems.polyToMove->isShapeBlocked()) {
            myMovedItems.polyToMove->moveEntireShape(myMoveSingleElementValues.movingOriginalShape, offsetMovement);
        } else {
            myMoveSingleElementValues.movingIndexShape = myMovedItems.polyToMove->moveVertexShape(myMoveSingleElementValues.movingIndexShape, myMoveSingleElementValues.movingOriginalPosition, offsetMovement);
        }
    } else if (myMovedItems.poiToMove) {
        // Move POI's geometry without commiting changes
        myMovedItems.poiToMove->moveGeometry(myMoveSingleElementValues.movingOriginalPosition, offsetMovement);
    } else if (myMovedItems.junctionToMove) {
        // Move Junction's geometry without commiting changes
        myMovedItems.junctionToMove->moveGeometry(myMoveSingleElementValues.movingOriginalPosition, offsetMovement);
    } else if (myMovedItems.edgeToMove) {
        if (myMoveSingleElementValues.movingStartPos) {
            myMovedItems.edgeToMove->moveShapeStart(myMoveSingleElementValues.movingOriginalPosition, offsetMovement);
        } else if (myMoveSingleElementValues.movingEndPos) {
            myMovedItems.edgeToMove->moveShapeEnd(myMoveSingleElementValues.movingOriginalPosition, offsetMovement);
        } else {
            // move edge's geometry without commiting changes
            myMoveSingleElementValues.movingIndexShape = myMovedItems.edgeToMove->moveVertexShape(myMoveSingleElementValues.movingIndexShape, myMoveSingleElementValues.movingOriginalPosition, offsetMovement);
        }
    } else if (myMovedItems.additionalToMove  && (myMovedItems.additionalToMove->isAdditionalBlocked() == false)) {
        // Move Additional geometry without commiting changes
        myMovedItems.additionalToMove->moveGeometry(myMoveSingleElementValues.movingOriginalPosition, offsetMovement);
    } else if (mySelectingArea.selectingUsingRectangle) {
        mySelectingArea.selectionCorner2 = getPositionInformation();
        setStatusBarText(mySelectingArea.reportDimensions());
    }
    // update view
    update();
    return 1;
}


long
GNEViewNet::onKeyPress(FXObject* o, FXSelector sel, void* eventData) {
    // update shift key pressed
    myShiftKeyPressed = ((FXEvent*)eventData)->state & SHIFTMASK;
    // change "delete last created point" depending of shift key
    if ((myEditMode == GNE_MODE_POLYGON) && myViewParent->getPolygonFrame()->getDrawingMode()->isDrawing()) {
        myViewParent->getPolygonFrame()->getDrawingMode()->setDeleteLastCreatedPoint(((FXEvent*)eventData)->state & SHIFTMASK);
    }
    update();
    return GUISUMOAbstractView::onKeyPress(o, sel, eventData);
}


long
GNEViewNet::onKeyRelease(FXObject* o, FXSelector sel, void* eventData) {
    // update shift key pressed
    myShiftKeyPressed = ((FXEvent*)eventData)->state & SHIFTMASK;
    // change "delete last created point" depending of shift key
    if ((myEditMode == GNE_MODE_POLYGON) && myViewParent->getPolygonFrame()->getDrawingMode()->isDrawing()) {
        myViewParent->getPolygonFrame()->getDrawingMode()->setDeleteLastCreatedPoint(myShiftKeyPressed);
    }
    // check if selecting using rectangle has to be disabled
    if (mySelectingArea.selectingUsingRectangle && !myShiftKeyPressed) {
        mySelectingArea.selectingUsingRectangle = false;
    }
    update();
    return GUISUMOAbstractView::onKeyRelease(o, sel, eventData);
}


void
GNEViewNet::abortOperation(bool clearSelection) {
    // steal focus from any text fields
    setFocus();
    if (myCreateEdgeSource != nullptr) {
        // remove current created edge source
        myCreateEdgeSource->unMarkAsCreateEdgeSource();
        myCreateEdgeSource = 0;
    } else if (myEditMode == GNE_MODE_SELECT) {
        mySelectingArea.selectingUsingRectangle = false;
        // check if current selection has to be cleaned
        if (clearSelection) {
            myViewParent->getSelectorFrame()->clearCurrentSelection();
        }
    } else if (myEditMode == GNE_MODE_CONNECT) {
        myViewParent->getConnectorFrame()->onCmdCancel(0, 0, 0);
    } else if (myEditMode == GNE_MODE_TLS) {
        myViewParent->getTLSEditorFrame()->onCmdCancel(0, 0, 0);
    } else if (myEditMode == GNE_MODE_MOVE) {
        stopEditCustomShape();
    } else if (myEditMode == GNE_MODE_POLYGON) {
        // abort current drawing
        myViewParent->getPolygonFrame()->getDrawingMode()->abortDrawing();
    } else if (myEditMode == GNE_MODE_PROHIBITION) {
        myViewParent->getProhibitionFrame()->onCmdCancel(0, 0, 0);
    }
    myUndoList->p_abort();
}


void
GNEViewNet::hotkeyDel() {
    if (myEditMode == GNE_MODE_CONNECT || myEditMode == GNE_MODE_TLS) {
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
    if (myEditMode == GNE_MODE_CONNECT) {
        myViewParent->getConnectorFrame()->onCmdOK(0, 0, 0);
    } else if (myEditMode == GNE_MODE_TLS) {
        myViewParent->getTLSEditorFrame()->onCmdOK(0, 0, 0);
    } else if ((myEditMode == GNE_MODE_MOVE) && (myEditShapePoly != 0)) {
        // save edited junction's shape
        if (myEditShapePoly != 0) {
            myUndoList->p_begin("custom " + toString(myEditShapePoly->getShapeEditedElement()->getTag()) + " shape");
            SumoXMLAttr attr = SUMO_ATTR_SHAPE;
            if (GNEAttributeCarrier::getTagProperties(myEditShapePoly->getShapeEditedElement()->getTag()).hasAttribute(SUMO_ATTR_CUSTOMSHAPE)) {
                attr = SUMO_ATTR_CUSTOMSHAPE;
            }
            myEditShapePoly->getShapeEditedElement()->setAttribute(attr, toString(myEditShapePoly->getShape()), myUndoList);
            myUndoList->p_end();
            stopEditCustomShape();
            update();
        }
    } else if (myEditMode == GNE_MODE_POLYGON) {
        if (myViewParent->getPolygonFrame()->getDrawingMode()->isDrawing()) {
            // stop current drawing
            myViewParent->getPolygonFrame()->getDrawingMode()->stopDrawing();
        } else {
            // start drawing
            myViewParent->getPolygonFrame()->getDrawingMode()->startDrawing();
        }
    } else if (myEditMode == GNE_MODE_CROSSING) {
        myViewParent->getCrossingFrame()->onCmdCreateCrossing(0, 0, 0);
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
    switch (selid) {
        case MID_GNE_SETMODE_CREATE_EDGE:
            setEditMode(GNE_MODE_CREATE_EDGE);
            break;
        case MID_GNE_SETMODE_MOVE:
            setEditMode(GNE_MODE_MOVE);
            break;
        case MID_GNE_SETMODE_DELETE:
            setEditMode(GNE_MODE_DELETE);
            break;
        case MID_GNE_SETMODE_INSPECT:
            setEditMode(GNE_MODE_INSPECT);
            break;
        case MID_GNE_SETMODE_SELECT:
            setEditMode(GNE_MODE_SELECT);
            break;
        case MID_GNE_SETMODE_CONNECT:
            setEditMode(GNE_MODE_CONNECT);
            break;
        case MID_GNE_SETMODE_TLS:
            setEditMode(GNE_MODE_TLS);
            break;
        case MID_GNE_SETMODE_ADDITIONAL:
            setEditMode(GNE_MODE_ADDITIONAL);
            break;
        case MID_GNE_SETMODE_CROSSING:
            setEditMode(GNE_MODE_CROSSING);
            break;
        case MID_GNE_SETMODE_POLYGON:
            setEditMode(GNE_MODE_POLYGON);
            break;
        case MID_GNE_SETMODE_PROHIBITION:
            setEditMode(GNE_MODE_PROHIBITION);
            break;
        default:
            throw ProcessError("invalid edit mode called by hotkey");
            break;
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


EditMode
GNEViewNet::getCurrentEditMode() const {
    return myEditMode;
}


FXMenuCheck*
GNEViewNet::getMenuCheckShowGrid() const {
    return myMenuCheckShowGrid;
}


const GNEAttributeCarrier*
GNEViewNet::getACUnderCursor() const {
    return myACUnderCursor;
}


void
GNEViewNet::setACUnderCursor(const GNEAttributeCarrier* AC) {
    myACUnderCursor = AC;
}


bool
GNEViewNet::showLockIcon() const {
    return (myEditMode == GNE_MODE_MOVE || myEditMode == GNE_MODE_INSPECT || myEditMode == GNE_MODE_ADDITIONAL);
}


GNEJunction*
GNEViewNet::getJunctionAtPopupPosition() {
    GNEJunction* junction = 0;
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
    GNEConnection* connection = 0;
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
    GNECrossing* crossing = 0;
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
    GNEEdge* edge = 0;
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
    GNELane* lane = 0;
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


std::set<GNEEdge*>
GNEViewNet::getEdgesAtPopupPosition() {
    std::set<GNEEdge*> result;
    if (makeCurrent()) {
        const std::vector<GUIGlID> ids = getObjectsAtPosition(getPopupPosition(), 1.0);
        for (auto it : ids) {
            GUIGlObject* pointed = GUIGlObjectStorage::gIDStorage.getObjectBlocking(it);
            GUIGlObjectStorage::gIDStorage.unblockObject(it);
            if (pointed) {
                switch (pointed->getType()) {
                    case GLO_EDGE:
                        result.insert((GNEEdge*)pointed);
                        break;
                    case GLO_LANE:
                        result.insert(&(((GNELane*)pointed)->getParentEdge()));
                        break;
                    default:
                        break;
                }
            }
        }
    }
    return result;
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
    return 0;
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
    return 0;
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
    return 0;
}


long
GNEViewNet::onCmdSetModeCreateEdge(FXObject*, FXSelector, void*) {
    setEditMode(GNE_MODE_CREATE_EDGE);
    return 1;
}


long
GNEViewNet::onCmdSetModeMove(FXObject*, FXSelector, void*) {
    setEditMode(GNE_MODE_MOVE);
    return 1;
}


long
GNEViewNet::onCmdSetModeDelete(FXObject*, FXSelector, void*) {
    setEditMode(GNE_MODE_DELETE);
    return 1;
}


long
GNEViewNet::onCmdSetModeInspect(FXObject*, FXSelector, void*) {
    setEditMode(GNE_MODE_INSPECT);
    return 1;
}


long
GNEViewNet::onCmdSetModeSelect(FXObject*, FXSelector, void*) {
    setEditMode(GNE_MODE_SELECT);
    return 1;
}


long
GNEViewNet::onCmdSetModeConnect(FXObject*, FXSelector, void*) {
    setEditMode(GNE_MODE_CONNECT);
    return 1;
}


long
GNEViewNet::onCmdSetModeTLS(FXObject*, FXSelector, void*) {
    setEditMode(GNE_MODE_TLS);
    return 1;
}


long
GNEViewNet::onCmdSetModeAdditional(FXObject*, FXSelector, void*) {
    setEditMode(GNE_MODE_ADDITIONAL);
    return 1;
}


long
GNEViewNet::onCmdSetModeCrossing(FXObject*, FXSelector, void*) {
    setEditMode(GNE_MODE_CROSSING);
    return 1;
}


long
GNEViewNet::onCmdSetModePolygon(FXObject*, FXSelector, void*) {
    setEditMode(GNE_MODE_POLYGON);
    return 1;
}

long
GNEViewNet::onCmdSetModeProhibition(FXObject*, FXSelector, void*) {
    setEditMode(GNE_MODE_PROHIBITION);
    return 1;
}

long
GNEViewNet::onCmdSplitEdge(FXObject*, FXSelector, void*) {
    GNEEdge* edge = getEdgeAtPopupPosition();
    if (edge != 0) {
        myNet->splitEdge(edge, edge->getSplitPos(getPopupPosition()), myUndoList);
    }
    return 1;
}


long
GNEViewNet::onCmdSplitEdgeBidi(FXObject*, FXSelector, void*) {
    std::set<GNEEdge*> edges = getEdgesAtPopupPosition();
    if (edges.size() != 0) {
        myNet->splitEdgesBidi(edges, (*edges.begin())->getSplitPos(getPopupPosition()), myUndoList);
    }
    return 1;
}


long
GNEViewNet::onCmdReverseEdge(FXObject*, FXSelector, void*) {
    GNEEdge* edge = getEdgeAtPopupPosition();
    if (edge != 0) {
        myNet->reverseEdge(edge, myUndoList);
    }
    return 1;
}


long
GNEViewNet::onCmdAddReversedEdge(FXObject*, FXSelector, void*) {
    GNEEdge* edge = getEdgeAtPopupPosition();
    if (edge != 0) {
        myNet->addReversedEdge(edge, myUndoList);
    }
    return 1;
}


long
GNEViewNet::onCmdEditEdgeEndpoint(FXObject*, FXSelector, void*) {
    GNEEdge* edge = getEdgeAtPopupPosition();
    if (edge != 0) {
        edge->editEndpoint(getPopupPosition(), myUndoList);
    }
    return 1;
}


long
GNEViewNet::onCmdResetEdgeEndpoint(FXObject*, FXSelector, void*) {
    GNEEdge* edge = getEdgeAtPopupPosition();
    if (edge != 0) {
        edge->resetEndpoint(getPopupPosition(), myUndoList);
    }
    return 1;
}


long
GNEViewNet::onCmdStraightenEdges(FXObject*, FXSelector, void*) {
    GNEEdge* edge = getEdgeAtPopupPosition();
    if (edge != 0) {
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
    if (edge != 0) {
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
    if (edge != 0) {
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
    if (edge != 0) {
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
    if (myEditShapePoly != 0) {
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
    if (myEditShapePoly != 0) {
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
    if (myEditShapePoly != 0) {
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
    if (myEditShapePoly != 0) {
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
    if (myEditShapePoly != 0) {
        myEditShapePoly->changeFirstGeometryPoint(myEditShapePoly->getVertexIndex(getPopupPosition(), false), false);
        update();
    } else {
        GNEPoly* polygonUnderMouse = getPolygonAtPopupPosition();
        if (polygonUnderMouse) {
            polygonUnderMouse->changeFirstGeometryPoint(polygonUnderMouse->getVertexIndex(getPopupPosition(), false));
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
        if (POI->getTag() == SUMO_TAG_POI) {
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
    if (lane != 0) {
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
    if (lane != 0) {
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
GNEViewNet::onCmdRestrictLaneSidewalk(FXObject*, FXSelector, void*) {
    return restrictLane(SVC_PEDESTRIAN);
}


long
GNEViewNet::onCmdRestrictLaneBikelane(FXObject*, FXSelector, void*) {
    return restrictLane(SVC_BICYCLE);
}


long
GNEViewNet::onCmdRestrictLaneBuslane(FXObject*, FXSelector, void*) {
    return restrictLane(SVC_BUS);
}


long
GNEViewNet::onCmdRestrictLaneGreenVerge(FXObject*, FXSelector, void*) {
    return restrictLane(SVC_IGNORING);
}


long
GNEViewNet::onCmdAddRestrictedLaneSidewalk(FXObject*, FXSelector, void*) {
    return addRestrictedLane(SVC_PEDESTRIAN);
}


long
GNEViewNet::onCmdAddRestrictedLaneBikelane(FXObject*, FXSelector, void*) {
    return addRestrictedLane(SVC_BICYCLE);
}


long
GNEViewNet::onCmdAddRestrictedLaneBuslane(FXObject*, FXSelector, void*) {
    return addRestrictedLane(SVC_BUS);
}


long
GNEViewNet::onCmdAddRestrictedLaneGreenVerge(FXObject*, FXSelector, void*) {
    return addRestrictedLane(SVC_IGNORING);
}


long
GNEViewNet::onCmdRemoveRestrictedLaneSidewalk(FXObject*, FXSelector, void*) {
    return removeRestrictedLane(SVC_PEDESTRIAN);
}


long
GNEViewNet::onCmdRemoveRestrictedLaneBikelane(FXObject*, FXSelector, void*) {
    return removeRestrictedLane(SVC_BICYCLE);
}


long
GNEViewNet::onCmdRemoveRestrictedLaneBuslane(FXObject*, FXSelector, void*) {
    return removeRestrictedLane(SVC_BUS);
}


long
GNEViewNet::onCmdRemoveRestrictedLaneGreenVerge(FXObject*, FXSelector, void*) {
    return removeRestrictedLane(SVC_IGNORING);
}


long
GNEViewNet::onCmdOpenAdditionalDialog(FXObject*, FXSelector, void*) {
    // retrieve additional under cursor
    GNEAdditional* addtional = getAdditionalAtPopupPosition();
    // check if additional can open dialog
    if (addtional && GNEAttributeCarrier::getTagProperties(addtional->getTag()).hasDialog()) {
        addtional->openAdditionalDialog();
    }
    return 1;
}


bool
GNEViewNet::restrictLane(SUMOVehicleClass vclass) {
    GNELane* lane = getLaneAtPopupPosition();
    if (lane != 0) {
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
    if (lane != 0) {
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
    if (lane != 0) {
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
GNEViewNet::processClick(FXEvent* e, void* eventData) {
    // process click
    destroyPopup();
    setFocus();
    myChanger->onLeftBtnPress(eventData);
    grab();
    // Check there are double click
    if (e->click_count == 2) {
        handle(this, FXSEL(SEL_DOUBLECLICKED, 0), eventData);
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
    if (junction != 0) {
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
    if (junction != 0) {
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
    if (junction != 0) {
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
    if (junction != 0) {
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
    if (!myShowConnections) {
        getNet()->initGNEConnections();
        myShowConnections = true;
    }
    myVisualizationSettings->showLane2Lane = myMenuCheckShowConnections->getCheck() == 1;
    // Update viewnNet to show/hide conections
    update();
    // Hide/show connections requiere recompute
    getNet()->requireRecompute();
    return 1;
}


long
GNEViewNet::onCmdToogleSelectEdges(FXObject*, FXSelector, void*) {
    if (myMenuCheckSelectEdges->getCheck()) {
        mySelectEdges = true;
    } else {
        mySelectEdges = false;
    }
    return 1;
}


long
GNEViewNet::onCmdToogleShowBubbles(FXObject*, FXSelector, void*) {
    // Update view net Shapes
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
    // show or hidde grid depending of myMenuCheckShowGrid
    if (myMenuCheckShowGrid->getCheck()) {
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
GNEViewNet::setEditMode(EditMode mode) {
    if (mode == myEditMode) {
        setStatusBarText("Mode already selected");
        if (myCurrentFrame != nullptr) {
            myCurrentFrame->focusUpperElement();
        }
    } else if (myEditMode == GNE_MODE_TLS && !myViewParent->getTLSEditorFrame()->isTLSSaved()) {
        setStatusBarText("save modifications in TLS before change mode");
        myCurrentFrame->focusUpperElement();
    } else {
        setStatusBarText("");
        abortOperation(false);
        // stop editing of custom shapes
        stopEditCustomShape();
        // set edit mode
        myEditMode = mode;
        switch (mode) {
            case GNE_MODE_CONNECT:
            case GNE_MODE_PROHIBITION:
            case GNE_MODE_TLS:
                // modes which depend on computed data
                myNet->computeEverything((GNEApplicationWindow*)myApp);
                break;
            default:
                break;
        }
        updateModeSpecificControls();
    }
}


void
GNEViewNet::buildEditModeControls() {
    // initialize mappings
    myEditModeNames.insert("(e) Create Edge", GNE_MODE_CREATE_EDGE);
    myEditModeNames.insert("(m) Move", GNE_MODE_MOVE);
    myEditModeNames.insert("(d) Delete", GNE_MODE_DELETE);
    myEditModeNames.insert("(i) Inspect", GNE_MODE_INSPECT);
    myEditModeNames.insert("(s) Select", GNE_MODE_SELECT);
    myEditModeNames.insert("(c) Connect", GNE_MODE_CONNECT);
    myEditModeNames.insert("(t) Traffic Lights", GNE_MODE_TLS);
    myEditModeNames.insert("(a) Additionals", GNE_MODE_ADDITIONAL);
    myEditModeNames.insert("(r) Crossings", GNE_MODE_CROSSING);
    myEditModeNames.insert("(p) Polygons", GNE_MODE_POLYGON);
    myEditModeNames.insert("(w) Prohibitions", GNE_MODE_PROHIBITION);

    // initialize buttons for modes
    myEditModeCreateEdge = new MFXCheckableButton(false, myToolbar, "\tset create edge mode\tMode for creating junction and edges.",
            GUIIconSubSys::getIcon(ICON_MODECREATEEDGE), this, MID_GNE_SETMODE_CREATE_EDGE, GUIDesignButtonToolbarCheckable);
    myEditModeMove = new MFXCheckableButton(false, myToolbar, "\tset move mode\tMode for move elements.",
                                            GUIIconSubSys::getIcon(ICON_MODEMOVE), this, MID_GNE_SETMODE_MOVE, GUIDesignButtonToolbarCheckable);
    myEditModeDelete = new MFXCheckableButton(false, myToolbar, "\tset delete mode\tMode for delete elements.",
            GUIIconSubSys::getIcon(ICON_MODEDELETE), this, MID_GNE_SETMODE_DELETE, GUIDesignButtonToolbarCheckable);
    myEditModeInspect = new MFXCheckableButton(false, myToolbar, "\tset inspect mode\tMode for inspect elements and change their attributes.",
            GUIIconSubSys::getIcon(ICON_MODEINSPECT), this, MID_GNE_SETMODE_INSPECT, GUIDesignButtonToolbarCheckable);
    myEditModeSelect = new MFXCheckableButton(false, myToolbar, "\tset select mode\tMode for select elements.",
            GUIIconSubSys::getIcon(ICON_MODESELECT), this, MID_GNE_SETMODE_SELECT, GUIDesignButtonToolbarCheckable);
    myEditModeConnection = new MFXCheckableButton(false, myToolbar, "\tset connection mode\tMode for edit connections between lanes.",
            GUIIconSubSys::getIcon(ICON_MODECONNECTION), this, MID_GNE_SETMODE_CONNECT, GUIDesignButtonToolbarCheckable);
    myEditModeProhibition = new MFXCheckableButton(false, myToolbar, "\tset prohibition mode\tMode for editing connection prohibitions.",
            GUIIconSubSys::getIcon(ICON_MODEPROHIBITION), this, MID_GNE_SETMODE_PROHIBITION, GUIDesignButtonToolbarCheckable);
    myEditModeTrafficLight = new MFXCheckableButton(false, myToolbar, "\tset traffic light mode\tMode for edit traffic lights over junctions.",
            GUIIconSubSys::getIcon(ICON_MODETLS), this, MID_GNE_SETMODE_TLS, GUIDesignButtonToolbarCheckable);
    myEditModeAdditional = new MFXCheckableButton(false, myToolbar, "\tset additional mode\tMode for adding additional elements.",
            GUIIconSubSys::getIcon(ICON_MODEADDITIONAL), this, MID_GNE_SETMODE_ADDITIONAL, GUIDesignButtonToolbarCheckable);
    myEditModeCrossing = new MFXCheckableButton(false, myToolbar, "\tset crossing mode\tMode for creating crossings between edges.",
            GUIIconSubSys::getIcon(ICON_MODECROSSING), this, MID_GNE_SETMODE_CROSSING, GUIDesignButtonToolbarCheckable);
    myEditModePolygon = new MFXCheckableButton(false, myToolbar, "\tset polygon mode\tMode for creating polygons and POIs.",
            GUIIconSubSys::getIcon(ICON_MODEPOLYGON), this, MID_GNE_SETMODE_POLYGON, GUIDesignButtonToolbarCheckable);

    // @ToDo add here new FXToolBarGrip(myNavigationToolBar, nullptr, 0, GUIDesignToolbarGrip);

    // initialize mode specific controls
    myChainCreateEdge = new FXMenuCheck(myToolbar, ("Chain\t\tCreate consecutive " + toString(SUMO_TAG_EDGE) + "s with a single click (hit ESC to cancel chain).").c_str(), this, 0);
    myChainCreateEdge->setCheck(false);

    myAutoCreateOppositeEdge = new FXMenuCheck(myToolbar, ("Two-way\t\tAutomatically create an " + toString(SUMO_TAG_EDGE) + " in the opposite direction").c_str(), this, 0);
    myAutoCreateOppositeEdge->setCheck(false);

    myMenuCheckSelectEdges = new FXMenuCheck(myToolbar, ("Select edges\t\tToggle whether clicking should select " + toString(SUMO_TAG_EDGE) + "s or " + toString(SUMO_TAG_LANE) + "s").c_str(), this, MID_GNE_VIEWNET_SELECT_EDGES);
    myMenuCheckSelectEdges->setCheck(true);

    myMenuCheckShowConnections = new FXMenuCheck(myToolbar, ("Show " + toString(SUMO_TAG_CONNECTION) + "s\t\tToggle show " + toString(SUMO_TAG_CONNECTION) + "s over " + toString(SUMO_TAG_JUNCTION) + "s").c_str(), this, MID_GNE_VIEWNET_SHOW_CONNECTIONS);
    myMenuCheckShowConnections->setCheck(myVisualizationSettings->showLane2Lane);

    myMenuCheckHideConnections = new FXMenuCheck(myToolbar,
            "Hide connections\t\tHide connections", this, 0);
    myMenuCheckHideConnections->setCheck(false);

    myMenuCheckExtendToEdgeNodes = new FXMenuCheck(myToolbar, ("Auto-select " + toString(SUMO_TAG_JUNCTION) + "s\t\tToggle whether selecting multiple " + toString(SUMO_TAG_EDGE) + "s should automatically select their " + toString(SUMO_TAG_JUNCTION) + "s").c_str(), this, 0);

    myMenuCheckWarnAboutMerge = new FXMenuCheck(myToolbar, ("Ask for merge\t\tAsk for confirmation before merging " + toString(SUMO_TAG_JUNCTION) + ".").c_str(), this, 0);
    myMenuCheckWarnAboutMerge->setCheck(true);

    myMenuCheckShowBubbleOverJunction = new FXMenuCheck(myToolbar, ("Bubbles\t\tShow bubbles over " + toString(SUMO_TAG_JUNCTION) + "'s shapes.").c_str(), this, MID_GNE_VIEWNET_SHOW_BUBBLES);
    myMenuCheckShowBubbleOverJunction->setCheck(false);

    myMenuCheckMoveElevation = new FXMenuCheck(myToolbar, "Elevation\t\tApply mouse movement to elevation instead of x,y position", this, MID_GNE_VIEWNET_MOVE_ELEVATION);
    myMenuCheckMoveElevation->setCheck(false);

    myMenuCheckChangeAllPhases = new FXMenuCheck(myToolbar, ("Apply change to all phases\t\tToggle whether clicking should apply state changes to all phases of the current " + toString(SUMO_TAG_TRAFFIC_LIGHT) + " plan").c_str(), this, 0);
    myMenuCheckChangeAllPhases->setCheck(false);

    myMenuCheckShowGrid = new FXMenuCheck(myToolbar, "Grid\t\tshow grid and restrict movement to the grid (size defined in visualization options)", this, MID_GNE_VIEWNET_SHOW_GRID);
    myMenuCheckShowGrid->setCheck(false);
}


void
GNEViewNet::updateModeSpecificControls() {
    // hide grid
    myMenuCheckShowGrid->setCheck(myVisualizationSettings->showGrid);
    // hide all controls (checkboxs)
    myChainCreateEdge->hide();
    myAutoCreateOppositeEdge->hide();
    myMenuCheckSelectEdges->hide();
    myMenuCheckShowConnections->hide();
    myMenuCheckHideConnections->hide();
    myMenuCheckExtendToEdgeNodes->hide();
    myMenuCheckChangeAllPhases->hide();
    myMenuCheckWarnAboutMerge->hide();
    myMenuCheckShowBubbleOverJunction->hide();
    myMenuCheckMoveElevation->hide();
    myMenuCheckShowGrid->hide();
    // unckeck all edit modes
    myEditModeCreateEdge->setChecked(false);
    myEditModeMove->setChecked(false);
    myEditModeDelete->setChecked(false);
    myEditModeInspect->setChecked(false);
    myEditModeSelect->setChecked(false);
    myEditModeConnection->setChecked(false);
    myEditModeTrafficLight->setChecked(false);
    myEditModeAdditional->setChecked(false);
    myEditModeCrossing->setChecked(false);
    myEditModePolygon->setChecked(false);
    myEditModeProhibition->setChecked(false);
    myViewParent->hideAllFrames();
    // enable selected controls
    switch (myEditMode) {
        case GNE_MODE_CREATE_EDGE:
            myChainCreateEdge->show();
            myAutoCreateOppositeEdge->show();
            myEditModeCreateEdge->setChecked(true);
            myMenuCheckShowGrid->show();
            break;
        case GNE_MODE_MOVE:
            myMenuCheckWarnAboutMerge->show();
            myMenuCheckShowBubbleOverJunction->show();
            myMenuCheckMoveElevation->show();
            myEditModeMove->setChecked(true);
            myMenuCheckShowGrid->show();
            break;
        case GNE_MODE_DELETE:
            myViewParent->getDeleteFrame()->show();
            myViewParent->getDeleteFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getDeleteFrame();
            myMenuCheckShowConnections->show();
            myMenuCheckSelectEdges->show();
            myEditModeDelete->setChecked(true);
            break;
        case GNE_MODE_INSPECT:
            myViewParent->getInspectorFrame()->show();
            myViewParent->getInspectorFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getInspectorFrame();
            myMenuCheckSelectEdges->show();
            myMenuCheckShowConnections->show();
            myEditModeInspect->setChecked(true);
            break;
        case GNE_MODE_SELECT:
            myViewParent->getSelectorFrame()->show();
            myViewParent->getSelectorFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getSelectorFrame();
            myMenuCheckSelectEdges->show();
            myMenuCheckShowConnections->show();
            myMenuCheckExtendToEdgeNodes->show();
            myEditModeSelect->setChecked(true);
            break;
        case GNE_MODE_CONNECT:
            myViewParent->getConnectorFrame()->show();
            myViewParent->getConnectorFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getConnectorFrame();
            myMenuCheckHideConnections->show();
            myEditModeConnection->setChecked(true);
            break;
        case GNE_MODE_TLS:
            myViewParent->getTLSEditorFrame()->show();
            myViewParent->getTLSEditorFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getTLSEditorFrame();
            myMenuCheckChangeAllPhases->show();
            myEditModeTrafficLight->setChecked(true);
            break;
        case GNE_MODE_ADDITIONAL:
            myViewParent->getAdditionalFrame()->show();
            myViewParent->getAdditionalFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getAdditionalFrame();
            myEditModeAdditional->setChecked(true);
            myMenuCheckShowGrid->show();
            break;
        case GNE_MODE_CROSSING:
            myViewParent->getCrossingFrame()->show();
            myViewParent->getCrossingFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getCrossingFrame();
            myEditModeCrossing->setChecked(true);
            myMenuCheckShowGrid->setCheck(false);
            break;
        case GNE_MODE_POLYGON:
            myViewParent->getPolygonFrame()->show();
            myViewParent->getPolygonFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getPolygonFrame();
            myEditModePolygon->setChecked(true);
            myMenuCheckShowGrid->show();
            break;
        case GNE_MODE_PROHIBITION:
            myViewParent->getProhibitionFrame()->show();
            myViewParent->getProhibitionFrame()->focusUpperElement();
            myCurrentFrame = myViewParent->getProhibitionFrame();
            myEditModeProhibition->setChecked(true);
            break;
        default:
            break;
    }
    // Update buttons
    myEditModeCreateEdge->update();
    myEditModeMove->update();
    myEditModeDelete->update();
    myEditModeInspect->update();
    myEditModeSelect->update();
    myEditModeConnection->update();
    myEditModeTrafficLight->update();
    myEditModeAdditional->update();
    myEditModeCrossing->update();
    myEditModePolygon->update();
    myEditModeProhibition->update();
    // force repaint because different modes draw different things
    myToolbar->recalc();
    onPaint(0, 0, 0);
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
            if (myNet->retrieveAdditional(i->getTag(), i->getID(), false) != nullptr) {
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
    GNEJunction* mergeTarget = 0;
    // try to find another junction to merge with
    if (makeCurrent()) {
        Boundary selection;
        selection.add(newPos);
        selection.grow(0.1);
        const std::vector<GUIGlID> ids = getObjectsInBoundary(selection);
        GUIGlObject* object = 0;
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
        if (myMenuCheckWarnAboutMerge->getCheck()) {
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
        // merge moved and targed junctions
        myNet->mergeJunctions(moved, mergeTarget, myUndoList);
        return true;
    } else {
        return false;
    }
}


void
GNEViewNet::updateControls() {
    switch (myEditMode) {
        case GNE_MODE_INSPECT:
            myViewParent->getInspectorFrame()->update();
            break;
        default:
            break;
    }
}

void
GNEViewNet::ObjectsUnderCursor::updateObjectUnderCursor(GUIGlID glIDObject, GNEPoly* editedPolyShape, FXEvent* ev) {
    // first reset all variables
    eventInfo = nullptr;
    glType = GLO_NETWORK;
    attributeCarrier = nullptr;
    netElement = nullptr;
    additional = nullptr;
    shape = nullptr;
    junction = nullptr;
    edge = nullptr;
    lane = nullptr;
    crossing = nullptr;
    connection = nullptr;
    poi = nullptr;
    poly = nullptr;
    // set event
    eventInfo = ev;
    // set GLID of object under cursor
    glID = glIDObject;
    // only continue if isn't 0
    if (glID == 0) {
        return;
    }
    // obtain glObject associated to these glID
    GUIGlObject* glObject = GUIGlObjectStorage::gIDStorage.getObjectBlocking(glID);
    GUIGlObjectStorage::gIDStorage.unblockObject(glID);
    // only continue if glObject isn't nullptr;
    if (glObject == nullptr) {
        return;
    }
    // set glType
    glType = glObject->getType();
    // cast attribute carrier from glObject
    attributeCarrier = dynamic_cast<GNEAttributeCarrier*>(glObject);
    // only continue if attributeCarrier isn't nullptr;
    if (attributeCarrier == nullptr) {
        return;
    }
    // If we're editing a shape, ignore rest of elements (including other polygons)
    if (editedPolyShape != nullptr) {
        if (attributeCarrier == editedPolyShape) {
            // cast Poly from attribute carrier
            poly = dynamic_cast<GNEPoly*>(attributeCarrier);
        }
    } else {
        // obtain tag property (only for improve code legibility)
        const auto& tagValue = GNEAttributeCarrier::getTagProperties(attributeCarrier->getTag());
        // check if attributeCarrier can be casted into netElement, additional or shape
        if (tagValue.isNetElement()) {
            // cast netElement from attribute carrier
            netElement = dynamic_cast<GNENetElement*>(attributeCarrier);
        } else if (tagValue.isAdditional()) {
            // cast additional element from attribute carrier
            additional = dynamic_cast<GNEAdditional*>(attributeCarrier);
        } else if (tagValue.isShape()) {
            // cast shape element from attribute carrier
            shape = dynamic_cast<GNEShape*>(attributeCarrier);
        } else {
            // element isn't specified, then stop
            return;
        }
        // now set specify AC type
        switch (glObject->getType()) {
            case GLO_JUNCTION:
                junction = dynamic_cast<GNEJunction*>(attributeCarrier);
                break;
            case GLO_EDGE:
                edge = dynamic_cast<GNEEdge*>(attributeCarrier);
                break;
            case GLO_LANE:
                lane = dynamic_cast<GNELane*>(attributeCarrier);
                break;
            case GLO_POI:
                poi = dynamic_cast<GNEPOI*>(attributeCarrier);
                break;
            case GLO_POLYGON:
                poly = dynamic_cast<GNEPoly*>(attributeCarrier);
                break;
            case GLO_CROSSING:
                crossing = dynamic_cast<GNECrossing*>(attributeCarrier);
                break;
            case GLO_CONNECTION:
                connection = dynamic_cast<GNEConnection*>(attributeCarrier);
                break;
            default:
                break;
        }
    }
}


void
GNEViewNet::ObjectsUnderCursor::swapLane2Edge() {
    if (lane) {
        edge = &lane->getParentEdge();
        netElement = edge;
        attributeCarrier = edge;
        glType = GLO_EDGE;
    }
}


bool
GNEViewNet::ObjectsUnderCursor::shiftKeyPressed() const {
    if (eventInfo) {
        return (eventInfo->state & SHIFTMASK) != 0;
    } else {
        return false;
    }
}


bool
GNEViewNet::ObjectsUnderCursor::controlKeyPressed() const {
    if (eventInfo) {
        return (eventInfo->state & CONTROLMASK) != 0;
    } else {
        return false;
    }
}


void
GNEViewNet::SelectingArea::processSelection(GNEViewNet* viewNet, bool shiftKeyPressed) {
    selectingUsingRectangle = false;
    // shift held down on mouse-down and mouse-up
    if (shiftKeyPressed) {
        if (viewNet->makeCurrent()) {
            Boundary b;
            b.add(selectionCorner1);
            b.add(selectionCorner2);
            std::vector<GUIGlID> ids = viewNet->getObjectsInBoundary(b);
            // use a set of pairs to obtain attribute carriers in rectangle sorted by ID (note: a map cannot be used because there is different ACs with the same ID
            std::set<std::pair<std::string, GNEAttributeCarrier*> > ACInRectangles;
            for (auto i : ids) {
                // avoid to select Net (i = 0)
                if (i != 0) {
                    GNEAttributeCarrier* retrievedAC = viewNet->myNet->retrieveAttributeCarrier(i);
                    // in the case of a Lane, we need to change the retrieved lane to their the parent if mySelectEdges is enabled
                    if ((retrievedAC->getTag() == SUMO_TAG_LANE) && viewNet->mySelectEdges) {
                        retrievedAC = &dynamic_cast<GNELane*>(retrievedAC)->getParentEdge();
                    }
                    // make sure that AttributeCarrier can be selected
                    GUIGlObject* glObject = dynamic_cast<GUIGlObject*>(retrievedAC);
                    if (glObject && !viewNet->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->IsObjectTypeLocked(glObject->getType())) {
                        ACInRectangles.insert(std::make_pair(retrievedAC->getID(), retrievedAC));
                    }
                }
            }
            // declare two sets of attribute carriers, one for select and another for unselect
            std::set<std::pair<std::string, GNEAttributeCarrier*> > ACToSelect;
            std::set<std::pair<std::string, GNEAttributeCarrier*> > ACToUnselect;
            // in restrict AND replace mode all current selected attribute carriers will be unselected
            if ((viewNet->myViewParent->getSelectorFrame()->getModificationModeModul()->getModificationMode() == GNESelectorFrame::ModificationMode::SET_RESTRICT) ||
                    (viewNet->myViewParent->getSelectorFrame()->getModificationModeModul()->getModificationMode() == GNESelectorFrame::ModificationMode::SET_REPLACE)) {
                for (auto i : viewNet->myNet->getSelectedAttributeCarriers()) {
                    ACToUnselect.insert(std::make_pair(i->getID(), i));
                }
            }
            // iterate over AtributeCarriers obtained of rectangle an place it in ACToSelect or ACToUnselect
            for (auto i : ACInRectangles) {
                switch (viewNet->myViewParent->getSelectorFrame()->getModificationModeModul()->getModificationMode()) {
                    case GNESelectorFrame::ModificationMode::SET_SUB:
                        ACToUnselect.insert(i);
                        break;
                    case GNESelectorFrame::ModificationMode::SET_RESTRICT:
                        if (ACToUnselect.find(i) != ACToUnselect.end()) {
                            ACToSelect.insert(i);
                        }
                        break;
                    default:
                        ACToSelect.insert(i);
                        break;
                }
            }
            // select junctions and their connections and crossings if Auto select junctions is enabled (note: only for "add mode")
            if (viewNet->autoSelectNodes() && GNESelectorFrame::ModificationMode::SET_ADD) {
                std::vector<GNEEdge*> edgesToSelect;
                // iterate over ACToSelect and extract edges
                for (auto i : ACToSelect) {
                    if (i.second->getTag() == SUMO_TAG_EDGE) {
                        edgesToSelect.push_back(dynamic_cast<GNEEdge*>(i.second));
                    }
                }
                // iterate over extracted edges
                for (auto i : edgesToSelect) {
                    // select junction source and all their connections and crossings
                    ACToSelect.insert(std::make_pair(i->getGNEJunctionSource()->getID(), i->getGNEJunctionSource()));
                    for (auto j : i->getGNEJunctionSource()->getGNEConnections()) {
                        ACToSelect.insert(std::make_pair(j->getID(), j));
                    }
                    for (auto j : i->getGNEJunctionSource()->getGNECrossings()) {
                        ACToSelect.insert(std::make_pair(j->getID(), j));
                    }
                    // select junction destiny and all their connections crossings
                    ACToSelect.insert(std::make_pair(i->getGNEJunctionDestiny()->getID(), i->getGNEJunctionDestiny()));
                    for (auto j : i->getGNEJunctionDestiny()->getGNEConnections()) {
                        ACToSelect.insert(std::make_pair(j->getID(), j));
                    }
                    for (auto j : i->getGNEJunctionDestiny()->getGNECrossings()) {
                        ACToSelect.insert(std::make_pair(j->getID(), j));
                    }
                }
            }
            // only continue if there is ACs to select or unselect
            if ((ACToSelect.size() + ACToUnselect.size()) > 0) {
                // first unselect AC of ACToUnselect and then selects AC of ACToSelect
                viewNet->myUndoList->p_begin("selection using rectangle");
                for (auto i : ACToUnselect) {
                    i.second->setAttribute(GNE_ATTR_SELECTED, "false", viewNet->myUndoList);
                }
                for (auto i : ACToSelect) {
                    if (GNEAttributeCarrier::getTagProperties(i.second->getTag()).isSelectable()) {
                        i.second->setAttribute(GNE_ATTR_SELECTED, "true", viewNet->myUndoList);
                    }
                }
                viewNet->myUndoList->p_end();
            }
            viewNet->makeNonCurrent();
        }
    }
}

std::string
GNEViewNet::SelectingArea::reportDimensions() {
    return ("Selection width:" + toString(fabs(selectionCorner1.x() - selectionCorner2.x()))
            + " height:" + toString(fabs(selectionCorner1.y() - selectionCorner2.y()))
            + " diagonal:" + toString(selectionCorner1.distanceTo2D(selectionCorner2)));
}

/****************************************************************************/
