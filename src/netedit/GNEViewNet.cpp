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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/xml/XMLSubSys.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBNode.h>

#include "GNEViewNet.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEJunction.h"
#include "GNEPOI.h"
#include "GNEPOILane.h"
#include "GNEApplicationWindow.h"
#include "GNEViewParent.h"
#include "GNENet.h"
#include "GNEUndoList.h"
#include "GNEInspectorFrame.h"
#include "GNESelectorFrame.h"
#include "GNEConnectorFrame.h"
#include "GNETLSEditorFrame.h"
#include "GNEAdditionalFrame.h"
#include "GNECrossingFrame.h"
#include "GNEPolygonFrame.h"
#include "GNEDeleteFrame.h"
#include "GNEAdditionalHandler.h"
#include "GNEPoly.h"
#include "GNECrossing.h"
#include "GNEAdditional.h"
#include "GNEAdditionalDialog.h"
#include "GNERerouter.h"
#include "GNEConnection.h"

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
    // Viewnet
    FXMAPFUNC(SEL_COMMAND, MID_GNE_VIEWNET_SHOW_CONNECTIONS,        GNEViewNet::onCmdToogleShowConnection),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_VIEWNET_SELECT_EDGES,            GNEViewNet::onCmdToogleSelectEdges),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_VIEWNET_SHOW_BUBBLES,            GNEViewNet::onCmdToogleShowBubbles),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_VIEWNET_SHOW_GRID,               GNEViewNet::onCmdShowGrid),
    // Junctions
    FXMAPFUNC(SEL_COMMAND, MID_GNE_JUNCTION_EDIT_SHAPE,             GNEViewNet::onCmdEditJunctionShape),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_JUNCTION_REPLACE,                GNEViewNet::onCmdReplaceJunction),
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
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_SET_ENDPOINT,               GNEViewNet::onCmdSetEdgeEndpoint),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_RESET_ENDPOINT,             GNEViewNet::onCmdResetEdgeEndpoint),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_STRAIGHTEN,                 GNEViewNet::onCmdStraightenEdges),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_SMOOTH,                     GNEViewNet::onCmdSmoothEdges),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_STRAIGHTEN_ELEVATION,       GNEViewNet::onCmdStraightenEdgesElevation),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_EDGE_SMOOTH_ELEVATION,           GNEViewNet::onCmdSmoothEdgesElevation),
    // Lanes
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_DUPLICATE,                  GNEViewNet::onCmdDuplicateLane),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_TRANSFORM_SIDEWALK,         GNEViewNet::onCmdRestrictLaneSidewalk),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_TRANSFORM_BIKE,             GNEViewNet::onCmdRestrictLaneBikelane),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_TRANSFORM_BUS,              GNEViewNet::onCmdRestrictLaneBuslane),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_ADD_SIDEWALK,               GNEViewNet::onCmdAddRestrictedLaneSidewalk),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_ADD_BIKE,                   GNEViewNet::onCmdAddRestrictedLaneBikelane),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_ADD_BUS,                    GNEViewNet::onCmdAddRestrictedLaneBuslane),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_REMOVE_SIDEWALK,            GNEViewNet::onCmdRemoveRestrictedLaneSidewalk),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_REMOVE_BIKE,                GNEViewNet::onCmdRemoveRestrictedLaneBikelane),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_LANE_REMOVE_BUS,                 GNEViewNet::onCmdRemoveRestrictedLaneBuslane),
    // addtionals
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
    myCreateEdgeSource(0),
    myJunctionToMove(0),
    myEdgeToMove(0),
    myPolyToMove(0),
    myPoiToMove(0),
    myPoiLaneToMove(0),
    myAdditionalToMove(0),
    myMovingIndexShape(-1),
    myMovingSelection(false),
    myAmInRectSelect(false),
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
    myEditModeNames(),
    myUndoList(undoList),
    myEditShapePoly(0),
    myTestingMode(OptionsCont::getOptions().getBool("gui-testing")) {
    // view must be the final member of actualParent
    reparent(actualParent);

    buildEditModeControls();
    myUndoList->mark();
    myNet->setViewNet(this);

    ((GUIDanielPerspectiveChanger*)myChanger)->setDragDelay(100000000); // 100 milliseconds

    if (myTestingMode && OptionsCont::getOptions().isSet("window-size")) {
        std::vector<std::string> windowSize = OptionsCont::getOptions().getStringVector("window-size");
        if (windowSize.size() == 2 && GNEAttributeCarrier::canParse<int>(windowSize[0]) && GNEAttributeCarrier::canParse<int>(windowSize[1])) {
            myTestingWidth = GNEAttributeCarrier::parse<int>(windowSize[0]);
            myTestingHeight = GNEAttributeCarrier::parse<int>(windowSize[1]);
        } else {
            WRITE_ERROR("Invalid windows size-format: " + toString(windowSize) + "for option'window-size'");
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
    if ((myEditShapePoly == NULL) && (element != NULL) && (shape.size() > 1)) {
        // save current edit mode before starting
        myPreviousEditMode = myEditMode;
        setEditModeFromHotkey(MID_GNE_SETMODE_MOVE);
        // add special GNEPoly fo edit shapes
        myEditShapePoly = myNet->addPolygonForEditShapes(element, shape, fill);
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
GNEViewNet::begingMoveSelection(GNEAttributeCarrier* originAC, const Position& originPosition) {
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
                // XXX this doesn't make sense. See #3708
                // a better solution would be to not move edge geometry when a suitable origin point could not be found
                //myOriginShapesMovedPartialShapes[i].inverted = (i->isInverted() != clickedEdge->isInverted());
                myOriginShapesMovedPartialShapes[i].inverted = true;
            }
            // obtain index shape of clicked edge and move it
            myOriginShapesMovedPartialShapes[clickedEdge].originalPosition = originPosition;
            myOriginShapesMovedPartialShapes[clickedEdge].index = clickedEdge->getVertexIndex(originPosition);
            myOriginShapesMovedPartialShapes[clickedEdge].inverted = false;
            // declare auxilar positionVector from Source To Destiny Junctions
            PositionVector segmentClickedEdge;
            // segmentA has two points, the first and last positions of edge
            segmentClickedEdge.push_back(clickedEdge->getGNEJunctionSource()->getPositionInView());
            segmentClickedEdge.push_back(clickedEdge->getGNEJunctionDestiny()->getPositionInView());
            // Obtain the offset (legA) over of segment regarding clicked position
            double offsetSegmentClickedEdge = segmentClickedEdge.nearest_offset_to_point2D(originPosition, false);
            double distanceToOffsetSegmentClickedEdge = segmentClickedEdge.positionAtOffset(offsetSegmentClickedEdge).distanceTo(originPosition);
            // check if direction of distanceToOffsetSegmentClickedEdge has to be changed
            if (!segmentClickedEdge.positionAtOffset(offsetSegmentClickedEdge, distanceToOffsetSegmentClickedEdge).almostSame(originPosition)) {
                distanceToOffsetSegmentClickedEdge *= -1;
            }
            // move index of rest of edges using offsetSegmentFSTDJ and distanceToOffsetSegmentFSTDJ as references
            for (auto i : noJunctionsSelected) {
                // don't move index of clicked edge, because was already moved
                if (i != clickedEdge) {
                    // calculate segment between first and las position of selected edge
                    PositionVector segmentSelectedEdge;
                    segmentSelectedEdge.push_back(i->getGNEJunctionSource()->getPositionInView());
                    segmentSelectedEdge.push_back(i->getGNEJunctionDestiny()->getPositionInView());
                    // get reference depending of this edge is the opposite edge of another alreaday inserted
                    if (myOriginShapesMovedPartialShapes[i].inverted) {
                        myOriginShapesMovedPartialShapes[i].originalPosition = segmentSelectedEdge.positionAtOffset(segmentSelectedEdge.length() - offsetSegmentClickedEdge, -1 * distanceToOffsetSegmentClickedEdge);
                    } else {
                        myOriginShapesMovedPartialShapes[i].originalPosition = segmentSelectedEdge.positionAtOffset(offsetSegmentClickedEdge, distanceToOffsetSegmentClickedEdge);
                    }
                    // obtain index to change
                    myOriginShapesMovedPartialShapes[i].index = i->getVertexIndex(myOriginShapesMovedPartialShapes[i].originalPosition);
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
    glRenderMode(mode);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    // visualize rectangular selection
    if (myAmInRectSelect) {
        glPushMatrix();
        glTranslated(0, 0, GLO_MAX - 1);
        GLHelper::setColor(GNENet::selectionColor);
        glLineWidth(2);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin(GL_QUADS);
        glVertex2d(mySelCorner1.x(), mySelCorner1.y());
        glVertex2d(mySelCorner1.x(), mySelCorner2.y());
        glVertex2d(mySelCorner2.x(), mySelCorner2.y());
        glVertex2d(mySelCorner2.x(), mySelCorner1.y());
        glEnd();
        glPopMatrix();
    }

    // compute lane width
    double lw = m2p(SUMO_const_laneWidth);
    // draw decals (if not in grabbing mode)
    if (!myUseToolTips) {
        drawDecals();
        // depending of the visualizationSettings, enable or disable check box show grid
        if (myVisualizationSettings->showGrid) {
            myMenuCheckShowGrid->setCheck(true);
            paintGLGrid();
        } else {
            myMenuCheckShowGrid->setCheck(false);
        }
        myMenuCheckShowConnections->setCheck(myVisualizationSettings->showLane2Lane);
        if (myTestingMode) {
            if (myTestingWidth > 0 && (getWidth() != myTestingWidth || getHeight() != myTestingHeight)) {
                // only resize once to avoid flickering
                //std::cout << " before resize: view=" << getWidth() << ", " << getHeight() << " app=" << myApp->getWidth() << ", " << myApp->getHeight() << "\n";
                myApp->resize(myTestingWidth + myTestingWidth - getWidth(), myTestingHeight + myTestingHeight - getHeight());
                //std::cout << " directly after resize: view=" << getWidth() << ", " << getHeight() << " app=" << myApp->getWidth() << ", " << myApp->getHeight() << "\n";
                myTestingWidth = 0;
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

            // Reset textures due bug #2780. This solution is only provisional, and must be updated in the future
            GUITextureSubSys::resetTextures();
        }
    }

    // draw temporal shape of polygon during drawing
    if (myViewParent->getPolygonFrame()->getDrawingMode()->isDrawing()) {
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
            GLHelper::setColor(RGBColor::RED);
            GLHelper::drawLine(temporalDrawingShape.back(), snapToActiveGrid(getPositionInformation()));
            glPopMatrix();
        }
    }

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
    for (std::map<const GUIGlObject*, int>::iterator i = myAdditionallyDrawn.begin(); i != myAdditionallyDrawn.end(); ++i) {
        (i->first)->drawGLAdditional(this, *myVisualizationSettings);
    }

    glPopMatrix();
    return hits2;
}


long
GNEViewNet::onLeftBtnPress(FXObject*, FXSelector, void* eventData) {
    FXEvent* e = (FXEvent*) eventData;
    setFocus();
    // interpret object under curser
    if (makeCurrent()) {
        int id = getObjectUnderCursor();
        GUIGlObject* pointed = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
        GUIGlObjectStorage::gIDStorage.unblockObject(id);
        GNEJunction* pointed_junction = 0;
        GNELane* pointed_lane = 0;
        GNEEdge* pointed_edge = 0;
        GNEPOI* pointed_poi = 0;
        GNEPOILane* pointed_poiLane = 0;
        GNEPoly* pointed_poly = 0;
        GNECrossing* pointed_crossing = 0;
        GNEAdditional* pointed_additional = 0;
        GNEConnection* pointed_connection = 0;
        if (pointed) {
            // If we're editing a shape, ignore rest of elements (including other polygons)
            if (myEditShapePoly) {
                if (pointed == myEditShapePoly) {
                    pointed_poly = (GNEPoly*)pointed;
                }
            } else {
                switch (pointed->getType()) {
                    case GLO_JUNCTION:
                        pointed_junction = (GNEJunction*)pointed;
                        break;
                    case GLO_EDGE:
                        pointed_edge = (GNEEdge*)pointed;
                        break;
                    case GLO_LANE:
                        pointed_lane = (GNELane*)pointed;
                        pointed_edge = &(pointed_lane->getParentEdge());
                        break;
                    case GLO_POI:
                        if (dynamic_cast<GNEPOI*>(pointed)) {
                            pointed_poi = (GNEPOI*)pointed;
                        } else {
                            pointed_poiLane = (GNEPOILane*)pointed;
                        }
                        break;
                    case GLO_POLYGON:
                        pointed_poly = (GNEPoly*)pointed;
                        break;
                    case GLO_CROSSING:
                        pointed_crossing = (GNECrossing*)pointed;
                        break;
                    case GLO_ADDITIONAL:
                        pointed_additional = (GNEAdditional*)pointed;
                        break;
                    case GLO_CONNECTION:
                        pointed_connection = (GNEConnection*)pointed;
                        break;
                    default:
                        pointed = 0;
                        break;
                }
            }
        }

        // decide what to do based on mode
        switch (myEditMode) {
            case GNE_MODE_CREATE_EDGE: {
                if ((e->state & CONTROLMASK) == 0) {
                    // allow moving when control is held down
                    if (!myUndoList->hasCommandGroup()) {
                        myUndoList->p_begin("create new " + toString(SUMO_TAG_EDGE));
                    }
                    if (!pointed_junction) {
                        pointed_junction = myNet->createJunction(snapToActiveGrid(getPositionInformation()), myUndoList);
                    }
                    if (myCreateEdgeSource == 0) {
                        myCreateEdgeSource = pointed_junction;
                        myCreateEdgeSource->markAsCreateEdgeSource();
                        update();
                    } else {
                        if (myCreateEdgeSource != pointed_junction) {
                            // may fail to prevent double edges
                            GNEEdge* newEdge = myNet->createEdge(
                                                   myCreateEdgeSource, pointed_junction, myViewParent->getInspectorFrame()->getEdgeTemplate(), myUndoList);
                            if (newEdge) {
                                // create another edge, if create opposite edge is enabled
                                if (myAutoCreateOppositeEdge->getCheck()) {
                                    myNet->createEdge(pointed_junction, myCreateEdgeSource, myViewParent->getInspectorFrame()->getEdgeTemplate(), myUndoList, "-" + newEdge->getNBEdge()->getID());
                                }
                                myCreateEdgeSource->unMarkAsCreateEdgeSource();

                                if (myUndoList->hasCommandGroup()) {
                                    myUndoList->p_end();
                                } else {
                                    std::cout << "edge created without an open CommandGroup )-:\n";
                                }
                                if (myChainCreateEdge->getCheck()) {
                                    myCreateEdgeSource = pointed_junction;
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
                processClick(e, eventData);
                break;
            }
            case GNE_MODE_MOVE: {
                // first obtain moving reference (common for all)
                myMovingReference = snapToActiveGrid(getPositionInformation());
                // check what type of AC will be moved
                if (pointed_poly) {
                    // set Poly to move
                    myPolyToMove = pointed_poly;
                    // save original shape (needed for commit change)
                    myMovingOriginalShape = myPolyToMove->getShape();
                    // save clicked position as moving original position
                    myMovingOriginalPosition = getPositionInformation();
                    // obtain index of vertex to move if shape isn't blocked
                    if ((myPolyToMove->isShapeBlocked() == false) && (myPolyToMove->isMovementBlocked() == false)) {
                        // obtain index of vertex to move and moving reference
                        myMovingIndexShape = myPolyToMove->getVertexIndex(myMovingOriginalPosition);
                    } else {
                        myMovingIndexShape = -1;
                    }
                } else if (pointed_poi) {
                    myPoiToMove = pointed_poi;
                    // Save original Position of Element
                    myMovingOriginalPosition = myPoiToMove->getPositionInView();
                } else if (pointed_poiLane) {
                    myPoiLaneToMove = pointed_poiLane;
                    // Save original Position of Element
                    myMovingOriginalPosition = myPoiLaneToMove->getPositionInView();
                } else if (pointed_junction) {
                    if (gSelected.isSelected(GLO_JUNCTION, pointed_junction->getGlID())) {
                        begingMoveSelection(pointed_junction, getPositionInformation());
                    } else {
                        myJunctionToMove = pointed_junction;
                    }
                    // Save original Position of Element
                    myMovingOriginalPosition = pointed_junction->getPositionInView();
                } else if (pointed_edge) {
                    if (gSelected.isSelected(GLO_EDGE, pointed_edge->getGlID())) {
                        begingMoveSelection(pointed_edge, getPositionInformation());
                    } else {
                        myEdgeToMove = pointed_edge;
                        // save original shape (needed for commit change)
                        myMovingOriginalShape = myEdgeToMove->getNBEdge()->getInnerGeometry();
                        // obtain index of vertex to move and moving reference
                        myMovingIndexShape = myEdgeToMove->getVertexIndex(getPositionInformation());
                    }
                    myMovingOriginalPosition = getPositionInformation();
                } else if (pointed_additional) {
                    if (gSelected.isSelected(GLO_ADDITIONAL, pointed_additional->getGlID())) {
                        myMovingSelection = true;
                    } else {
                        myAdditionalToMove = pointed_additional;
                        // Save original Position of Element
                        myMovingOriginalPosition = myAdditionalToMove->getPositionInView();
                    }
                } else {
                    // process click
                    processClick(e, eventData);
                }
                update();
                break;
            }
            case GNE_MODE_DELETE: {
                // Check if Control key is pressed
                bool markElementMode = (((FXEvent*)eventData)->state & CONTROLMASK) != 0;
                // obtain attribute carrier related to pointed object (if exists)
                GNEAttributeCarrier* ac = pointed ? myNet->retrieveAttributeCarrier(pointed->getGlID()) : NULL;
                if ((pointed_lane != NULL) && mySelectEdges) {
                    ac = pointed_edge;
                }
                if (ac) {
                    // if pointed element is an attribute carrier, remove it or mark it
                    if (markElementMode) {
                        if (myViewParent->getDeleteFrame()->getMarkedAttributeCarrier() != ac) {
                            myViewParent->getDeleteFrame()->markAttributeCarrier(ac);
                        }
                    } else if (myViewParent->getDeleteFrame()->getMarkedAttributeCarrier() != NULL) {
                        myViewParent->getDeleteFrame()->markAttributeCarrier(NULL);
                    } else {
                        myViewParent->getDeleteFrame()->removeAttributeCarrier(ac);
                    }
                } else {
                    // process click
                    processClick(e, eventData);
                }
                break;
            }
            case GNE_MODE_INSPECT: {
                GNEAttributeCarrier* pointedAC = 0;
                GUIGlObject* pointedO = 0;
                if (pointed_junction) {
                    pointedAC = pointed_junction;
                    pointedO = pointed_junction;
                } else if (pointed_lane) { // implies pointed_edge
                    if (mySelectEdges) {
                        pointedAC = pointed_edge;
                        pointedO = pointed_edge;
                    } else {
                        pointedAC = pointed_lane;
                        pointedO = pointed_lane;
                    }
                } else if (pointed_edge) {
                    pointedAC = pointed_edge;
                    pointedO = pointed_edge;
                } else if (pointed_crossing) {
                    pointedAC = pointed_crossing;
                    pointedO = pointed_crossing;
                } else if (pointed_additional) {
                    pointedAC = pointed_additional;
                    pointedO = pointed_additional;
                } else if (pointed_connection) {
                    pointedAC = pointed_connection;
                    pointedO = pointed_connection;
                } else if (pointed_poly) {
                    pointedAC = pointed_poly;
                    pointedO = pointed_poly;
                } else if (pointed_poi) {
                    pointedAC = pointed_poi;
                    pointedO = pointed_poi;
                } else if (pointed_poiLane) {
                    pointedAC = pointed_poiLane;
                    pointedO = pointed_poiLane;
                }
                // obtain selected ACs
                std::vector<GNEAttributeCarrier*> selectedElements;
                std::vector<GNEAttributeCarrier*> selectedFilteredElements;
                if (pointedO && gSelected.isSelected(pointedO->getType(), pointedO->getGlID())) {
                    // retrieve selected Attribute Carriers
                    selectedElements = myNet->retrieveAttributeCarriers(gSelected.getSelected(pointedO->getType()), pointedO->getType());
                    // filter selected elements (example: if we have two E2 and one busStop selected, and user click over one E2,
                    // attribues of busstop musn't be shown
                    for (auto i : selectedElements) {
                        if (i->getTag() == pointedAC->getTag()) {
                            selectedFilteredElements.push_back(i);
                        }
                    }
                }
                // Inspect seleted ACs, or single clicked AC
                if (selectedFilteredElements.size() > 0) {
                    myViewParent->getInspectorFrame()->inspectMultisection(selectedFilteredElements);
                } else if (pointedAC != NULL) {
                    myViewParent->getInspectorFrame()->inspectElement(pointedAC);
                }
                // process click
                processClick(e, eventData);
                // focus upper element of inspector frame
                if ((selectedFilteredElements.size() > 0) || (pointedAC != NULL)) {
                    myViewParent->getInspectorFrame()->focusUpperElement();
                }
                update();
                break;
            }
            case GNE_MODE_SELECT:
                if ((pointed_lane != NULL) && mySelectEdges) {
                    if (!myViewParent->getSelectorFrame()->locked(GLO_EDGE)) {
                        gSelected.toggleSelection(pointed_edge->getGlID());
                    }
                } else if (pointed && !myViewParent->getSelectorFrame()->locked(pointed->getType())) {
                    gSelected.toggleSelection(pointed->getGlID());
                }

                myAmInRectSelect = (((FXEvent*)eventData)->state & SHIFTMASK) != 0;
                if (myAmInRectSelect) {
                    mySelCorner1 = getPositionInformation();
                    mySelCorner2 = getPositionInformation();
                } else {
                    // process click
                    processClick(e, eventData);
                }
                update();
                break;

            case GNE_MODE_CONNECT: {
                if (pointed_lane) {
                    const bool mayPass = (((FXEvent*)eventData)->state & SHIFTMASK) != 0;
                    const bool allowConflict = (((FXEvent*)eventData)->state & CONTROLMASK) != 0;
                    myViewParent->getConnectorFrame()->handleLaneClick(pointed_lane, mayPass, allowConflict, true);
                    update();
                }
                // process click
                processClick(e, eventData);
                break;
            }
            case GNE_MODE_TLS: {
                if (pointed_junction) {
                    myViewParent->getTLSEditorFrame()->editJunction(pointed_junction);
                    update();
                }
                // process click
                processClick(e, eventData);
                break;
            }
            case GNE_MODE_ADDITIONAL: {
                if (pointed_additional == NULL) {
                    GNENetElement* netElement = dynamic_cast<GNENetElement*>(pointed);
                    GNEAdditionalFrame::AddAdditionalResult result = myViewParent->getAdditionalFrame()->addAdditional(netElement, this);
                    // process click or update view depending of the result of "add additional"
                    if ((result == GNEAdditionalFrame::ADDADDITIONAL_SUCCESS) || (result == GNEAdditionalFrame::ADDADDITIONAL_INVALID_PARENT)) {
                        update();
                        // process click
                        processClick(e, eventData);
                    }
                }

                break;
            }
            case GNE_MODE_CROSSING: {
                if (pointed_crossing == NULL) {
                    GNENetElement* netElement = dynamic_cast<GNENetElement*>(pointed);
                    if (myViewParent->getCrossingFrame()->addCrossing(netElement)) {
                        update();
                    }
                }
                // process click
                processClick(e, eventData);
                break;
            }
            case GNE_MODE_POLYGON: {
                if (pointed_poi == NULL) {
                    GNEPolygonFrame::AddShapeResult result = myViewParent->getPolygonFrame()->processClick(snapToActiveGrid(getPositionInformation()), pointed_lane);
                    // view net must be always update
                    update();
                    // process clickw depending of the result of "add additional"
                    if ((result != GNEPolygonFrame::ADDSHAPE_NEWPOINT)) {
                        // process click
                        processClick(e, eventData);
                    }
                }
                break;
            }
            default: {
                // process click
                processClick(e, eventData);
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
    } else if (myPolyToMove) {
        myPolyToMove->commitShapeChange(myMovingOriginalShape, myUndoList);
        myPolyToMove = 0;
    } else if (myPoiToMove) {
        myPoiToMove->commitGeometryMoving(myMovingOriginalPosition, myUndoList);
        myPoiToMove = 0;
    } else if (myPoiLaneToMove) {
        myPoiLaneToMove->commitGeometryMoving(myMovingOriginalPosition, myUndoList);
        myPoiLaneToMove = 0;
    } else if (myJunctionToMove) {
        // position is already up to date but we must register with myUndoList
        if (!mergeJunctions(myJunctionToMove, myMovingOriginalPosition)) {
            myJunctionToMove->commitGeometryMoving(myMovingOriginalPosition, myUndoList);
        }
        myJunctionToMove = 0;
    } else if (myEdgeToMove) {
        myEdgeToMove->commitShapeChange(myMovingOriginalShape, myUndoList);
        myEdgeToMove = 0;
    } else if (myAdditionalToMove) {
        myAdditionalToMove->commitGeometryMoving(myMovingOriginalPosition, myUndoList);
        myAdditionalToMove = 0;
    } else if (myAmInRectSelect) {
        myAmInRectSelect = false;
        // shift held down on mouse-down and mouse-up
        if (((FXEvent*)eventData)->state & SHIFTMASK) {
            if (makeCurrent()) {
                Boundary b;
                b.add(mySelCorner1);
                b.add(mySelCorner2);
                myViewParent->getSelectorFrame()->handleIDs(getObjectsInBoundary(b), mySelectEdges);
                makeNonCurrent();
            }
        }
        update();
    }
    return 1;
}


long GNEViewNet::onRightBtnPress(FXObject* obj, FXSelector sel, void* eventData) {
    if ((myEditMode == GNE_MODE_POLYGON) && myViewParent->getPolygonFrame()->getDrawingMode()->isDrawing()) {
        // during drawing of a polygon, right click removes the last created point
        myViewParent->getPolygonFrame()->getDrawingMode()->removeLastPoint();
        // update view
        update();
        return 1;
    } else {
        return GUISUMOAbstractView::onRightBtnPress(obj, sel, eventData);
    }
}


long GNEViewNet::onRightBtnRelease(FXObject* obj, FXSelector sel, void* eventData) {
    if ((myEditMode == GNE_MODE_POLYGON) && myViewParent->getPolygonFrame()->getDrawingMode()->isDrawing()) {
        // during drawing of a polygon, right click removes the last created point
        return 1;
    } else {
        return GUISUMOAbstractView::onRightBtnRelease(obj, sel, eventData);
    }
}


long
GNEViewNet::onMouseMove(FXObject* obj, FXSelector sel, void* eventData) {
    GUISUMOAbstractView::onMouseMove(obj, sel, eventData);
    // in delete mode object under cursor must be checked in every mouse movement
    if (myEditMode == GNE_MODE_DELETE) {
        setFocus();
        // show object information in delete frame
        if (makeCurrent()) {
            // Update current label of delete frame
            myViewParent->getDeleteFrame()->updateCurrentLabel(myNet->retrieveAttributeCarrier(getObjectUnderCursor()));
        }
    } else {
        // calculate offset of movement depending of showGrid
        Position offsetMovement;
        if (myVisualizationSettings->showGrid) {
            offsetMovement = snapToActiveGrid(getPositionInformation()) - myMovingOriginalPosition;
            if (myMenuCheckMoveElevation->getCheck()) {
                const double dist = int((offsetMovement.y() + offsetMovement.x()) / myVisualizationSettings->gridXSize) * myVisualizationSettings->gridXSize;
                offsetMovement = Position(0, 0, dist / 10);
            }
        } else {
            offsetMovement = getPositionInformation() - myMovingReference;
            if (myMenuCheckMoveElevation->getCheck()) {
                offsetMovement = Position(0, 0, (offsetMovement.y() + offsetMovement.x()) / 10);
            }
        }
        // @note  #3521: Add checkBox to allow moving elements... has to behere implemented
        // check what type of additional is moved
        if (myMovingSelection) {
            moveSelection(offsetMovement);
        } else if (myPolyToMove) {
            // move shape's geometry without commiting changes
            if (myPolyToMove->isShapeBlocked()) {
                myPolyToMove->moveEntireShape(myMovingOriginalShape, offsetMovement);
            } else {
                myMovingIndexShape = myPolyToMove->moveVertexShape(myMovingIndexShape, myMovingOriginalPosition, offsetMovement);
            }
        } else if (myPoiToMove) {
            // Move POI's geometry without commiting changes
            myPoiToMove->moveGeometry(myMovingOriginalPosition, offsetMovement);
        } else if (myPoiLaneToMove) {
            // Move POILane's geometry without commiting changes
            myPoiLaneToMove->moveGeometry(myMovingOriginalPosition, offsetMovement);
        } else if (myJunctionToMove) {
            // Move Junction's geometry without commiting changes
            myJunctionToMove->moveGeometry(myMovingOriginalPosition, offsetMovement);
        } else if (myEdgeToMove) {
            // move edge's geometry without commiting changes
            myMovingIndexShape = myEdgeToMove->moveVertexShape(myMovingIndexShape, myMovingOriginalPosition, offsetMovement);
        } else if (myAdditionalToMove  && (myAdditionalToMove->isAdditionalBlocked() == false)) {
            // Move Additional geometry without commiting changes
            myAdditionalToMove->moveGeometry(myMovingOriginalPosition, offsetMovement);
        } else if (myAmInRectSelect) {
            mySelCorner2 = getPositionInformation();
        }
    }
    // update view
    update();
    return 1;
}


long
GNEViewNet::onKeyPress(FXObject* o, FXSelector sel, void* eventData) {
    return GUISUMOAbstractView::onKeyPress(o, sel, eventData);
}


long
GNEViewNet::onKeyRelease(FXObject* o, FXSelector sel, void* eventData) {
    if (myAmInRectSelect && ((((FXEvent*)eventData)->state & SHIFTMASK) == false)) {
        myAmInRectSelect = false;
        update();
    }
    return GUISUMOAbstractView::onKeyRelease(o, sel, eventData);
}


void
GNEViewNet::abortOperation(bool clearSelection) {
    // steal focus from any text fields
    setFocus();
    if (myCreateEdgeSource != NULL) {
        // remove current created edge source
        myCreateEdgeSource->unMarkAsCreateEdgeSource();
        myCreateEdgeSource = 0;
    } else if (myEditMode == GNE_MODE_SELECT) {
        myAmInRectSelect = false;
        if (clearSelection) {
            gSelected.clear();
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
            if (GNEAttributeCarrier::hasAttribute(myEditShapePoly->getShapeEditedElement()->getTag(), SUMO_ATTR_CUSTOMSHAPE)) {
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
    if (myCurrentFrame != NULL) {
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


GNEPOILane*
GNEViewNet::getPOILaneAtPopupPosition() {
    if (makeCurrent()) {
        int id = getObjectAtPosition(getPopupPosition());
        GUIGlObject* pointed = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
        GUIGlObjectStorage::gIDStorage.unblockObject(id);
        if (pointed) {
            return dynamic_cast<GNEPOILane*>(pointed);
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
GNEViewNet::onCmdSetEdgeEndpoint(FXObject*, FXSelector, void*) {
    GNEEdge* edge = getEdgeAtPopupPosition();
    if (edge != 0) {
        edge->setEndpoint(getPopupPosition(), myUndoList);
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
        if (gSelected.isSelected(GLO_EDGE, edge->getGlID())) {
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
        if (gSelected.isSelected(GLO_EDGE, edge->getGlID())) {
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
        if (gSelected.isSelected(GLO_EDGE, edge->getGlID())) {
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
        if (gSelected.isSelected(GLO_EDGE, edge->getGlID())) {
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
    // check what type of POI will be transformed
    GNEPOILane* POILane = getPOILaneAtPopupPosition();
    GNEPOI* POI = getPOIAtPopupPosition();
    if (POI) {
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
            WRITE_WARNING("No lanes around " + toString(SUMO_TAG_POILANE) + " to attach it");
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
            std::string type = POI->getType();
            RGBColor color = POI->getColor();
            Position pos = (*POI);
            double layer = POI->getLayer();
            double angle = POI->getNaviDegree();
            std::string imgFile = POI->getImgFile();
            double POIWidth = POI->getWidth();      // double width -> C4458
            double POIHeight = POI->getHeight();    // double height -> C4458
            // remove POI
            myUndoList->p_begin("attach POI into " + toString(SUMO_TAG_LANE));
            myNet->deleteShape(POI, myUndoList);
            // add POILane
            myNet->addPOI(id, type, color, pos, false, nearestLane->getID(), minorPosOverLane, 0, layer, angle, imgFile, POIWidth, POIHeight);
            myUndoList->p_end();
        }
    } else if (POILane) {
        // obtain values of POILane
        std::string id = POILane->getID();
        std::string type = POILane->getType();
        RGBColor color = POILane->getColor();
        Position pos = (*POILane);
        double layer = POILane->getLayer();
        double angle = POILane->getNaviDegree();
        std::string imgFile = POILane->getImgFile();
        double POIWidth = POILane->getWidth();      // double width -> C4458
        double POIWeight = POILane->getHeight();    // double height -> C4458
        // remove POILane
        myUndoList->p_begin("release POI from " + toString(SUMO_TAG_LANE));
        myNet->deleteShape(POILane, myUndoList);
        // add POI
        myNet->addPOI(id, type, color, pos, false, "", 0, 0, layer, angle, imgFile, POIWidth, POIWeight);
        myUndoList->p_end();
    }
    // update view after transform
    update();
    return 1;
}


long
GNEViewNet::onCmdDuplicateLane(FXObject*, FXSelector, void*) {
    GNELane* lane = getLaneAtPopupPosition();
    if (lane != 0) {
        // when duplicating an unselected lane, keep all connections as they
        // are, otherwise recompute them
        if (gSelected.isSelected(GLO_LANE, lane->getGlID())) {
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
GNEViewNet::onCmdOpenAdditionalDialog(FXObject*, FXSelector, void*) {
    // retrieve additional under cursor
    GNEAdditional* addtional = getAdditionalAtPopupPosition();
    // check if additional can open dialog
    if (addtional && GNEAttributeCarrier::canOpenDialog(addtional->getTag())) {
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
                if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                    WRITE_WARNING("Opening FXMessageBox 'restrict lanes'");
                }
                // Ask confirmation to user
                FXuint answer = FXMessageBox::question(getApp(), MBOX_YES_NO,
                                                       ("Set vclass for " + toString(vclass) + " to selected lanes").c_str(), "%s",
                                                       (toString(mapOfEdgesAndLanes.size() - counter) + " lanes will be restricted for " + toString(vclass) + ". continue?").c_str());
                if (answer != 1) { //1:yes, 2:no, 4:esc
                    // write warning if netedit is running in testing mode
                    if ((answer == 2) && (OptionsCont::getOptions().getBool("gui-testing-debug"))) {
                        WRITE_WARNING("Closed FXMessageBox 'restrict lanes' with 'No'");
                    } else if ((answer == 4) && (OptionsCont::getOptions().getBool("gui-testing-debug"))) {
                        WRITE_WARNING("Closed FXMessageBox 'restrict lanes' with 'ESC'");
                    }
                    return 0;
                } else {
                    // write warning if netedit is running in testing mode
                    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                        WRITE_WARNING("Closed FXMessageBox 'restrict lanes' with 'Yes'");
                    }
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
                if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                    WRITE_WARNING("Opening FXMessageBox 'restrict lanes'");
                }
                // Ask confirmation to user
                FXuint answer = FXMessageBox::question(getApp(), MBOX_YES_NO,
                                                       ("Add vclass for " + toString(vclass) + " to selected lanes").c_str(), "%s",
                                                       (toString(setOfEdges.size() - counter) + " restrictions for " + toString(vclass) + " will be added. continue?").c_str());
                if (answer != 1) { //1:yes, 2:no, 4:esc
                    // write warning if netedit is running in testing mode
                    if ((answer == 2) && (OptionsCont::getOptions().getBool("gui-testing-debug"))) {
                        WRITE_WARNING("Closed FXMessageBox 'restrict lanes' with 'No'");
                    } else if ((answer == 4) && (OptionsCont::getOptions().getBool("gui-testing-debug"))) {
                        WRITE_WARNING("Closed FXMessageBox 'restrict lanes' with 'ESC'");
                    }
                    return 0;
                } else {
                    // write warning if netedit is running in testing mode
                    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                        WRITE_WARNING("Closed FXMessageBox 'restrict lanes' with 'Yes'");
                    }
                }
            }
            // begin undo operation
            myUndoList->p_begin("Add restrictions for " + toString(vclass));
            // iterate over set of edges
            for (std::set<GNEEdge*>::iterator it = setOfEdges.begin(); it != setOfEdges.end(); it++) {
                // add Sidewalk
                myNet->addSRestrictedLane(vclass, *(*it), myUndoList);
            }
            // end undo operation
            myUndoList->p_end();
        } else {
            // If only have a single lane, start undo/redo operation
            myUndoList->p_begin("Add vclass for " + toString(vclass));
            // Add Sidewalk
            myNet->addSRestrictedLane(vclass, lane->getParentEdge(), myUndoList);
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
                if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                    WRITE_WARNING("Opening FXMessageBox 'restrict lanes'");
                }
                // Ask confirmation to user
                FXuint answer = FXMessageBox::question(getApp(), MBOX_YES_NO,
                                                       ("Remove vclass for " + toString(vclass) + " to selected lanes").c_str(), "%s",
                                                       (toString(counter) + " restrictions for " + toString(vclass) + " will be removed. continue?").c_str());
                if (answer != 1) { //1:yes, 2:no, 4:esc
                    // write warning if netedit is running in testing mode
                    if ((answer == 2) && (OptionsCont::getOptions().getBool("gui-testing-debug"))) {
                        WRITE_WARNING("Closed FXMessageBox 'restrict lanes' with 'No'");
                    } else if ((answer == 4) && (OptionsCont::getOptions().getBool("gui-testing-debug"))) {
                        WRITE_WARNING("Closed FXMessageBox 'restrict lanes' with 'ESC'");
                    }
                    return 0;
                } else {
                    // write warning if netedit is running in testing mode
                    if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                        WRITE_WARNING("Closed FXMessageBox 'restrict lanes' with 'Yes'");
                    }
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
        junction->getNBNode()->computeNodeShape(-1);
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
GNEViewNet::onCmdClearConnections(FXObject*, FXSelector, void*) {
    GNEJunction* junction = getJunctionAtPopupPosition();
    if (junction != 0) {
        // check if we're handling a selection
        if (gSelected.isSelected(GLO_JUNCTION, junction->getGlID())) {
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
        if (gSelected.isSelected(GLO_JUNCTION, junction->getGlID())) {
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
    setStatusBarText("");
    abortOperation(false);
    // stop editing of custom shapes
    stopEditCustomShape();

    if (mode == myEditMode) {
        setStatusBarText("Mode already selected");
        if (myCurrentFrame != NULL) {
            myCurrentFrame->focusUpperElement();
        }
    } else {
        myEditMode = mode;
        switch (mode) {
            case GNE_MODE_CONNECT:
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
    myEditModeTrafficLight = new MFXCheckableButton(false, myToolbar, "\tset traffic light mode\tMode for edit traffic lights over junctions.",
            GUIIconSubSys::getIcon(ICON_MODETLS), this, MID_GNE_SETMODE_TLS, GUIDesignButtonToolbarCheckable);
    myEditModeAdditional = new MFXCheckableButton(false, myToolbar, "\tset additional mode\tMode for adding additional elements.",
            GUIIconSubSys::getIcon(ICON_MODEADDITIONAL), this, MID_GNE_SETMODE_ADDITIONAL, GUIDesignButtonToolbarCheckable);
    myEditModeCrossing = new MFXCheckableButton(false, myToolbar, "\tset crossing mode\tMode for creating crossings between edges.",
            GUIIconSubSys::getIcon(ICON_MODECROSSING), this, MID_GNE_SETMODE_CROSSING, GUIDesignButtonToolbarCheckable);
    myEditModePolygon = new MFXCheckableButton(false, myToolbar, "\tset polygon mode\tMode for creating polygons and POIs.",
            GUIIconSubSys::getIcon(ICON_MODEPOLYGON), this, MID_GNE_SETMODE_POLYGON, GUIDesignButtonToolbarCheckable);

    // @ToDo add here new FXToolBarGrip(myNavigationToolBar, NULL, 0, GUIDesignToolbarGrip);

    // initialize mode specific controls
    myChainCreateEdge = new FXMenuCheck(myToolbar, ("Chain\t\tCreate consecutive " + toString(SUMO_TAG_EDGE) + "s with a single click (hit ESC to cancel chain).").c_str(), this, 0);
    myChainCreateEdge->setCheck(false);

    myAutoCreateOppositeEdge = new FXMenuCheck(myToolbar, ("Two-way\t\tAutomatically create an " + toString(SUMO_TAG_EDGE) + " in the opposite direction").c_str(), this, 0);
    myAutoCreateOppositeEdge->setCheck(false);

    myMenuCheckSelectEdges = new FXMenuCheck(myToolbar, ("Select edges\t\tToggle whether clicking should select " + toString(SUMO_TAG_EDGE) + "s or " + toString(SUMO_TAG_LANE) + "s").c_str(), this, MID_GNE_VIEWNET_SELECT_EDGES);
    myMenuCheckSelectEdges->setCheck(true);

    myMenuCheckShowConnections = new FXMenuCheck(myToolbar, ("Show " + toString(SUMO_TAG_CONNECTION) + "s\t\tToggle show " + toString(SUMO_TAG_CONNECTION) + "s over " + toString(SUMO_TAG_JUNCTION) + "s").c_str(), this, MID_GNE_VIEWNET_SHOW_CONNECTIONS);
    myMenuCheckShowConnections->setCheck(myVisualizationSettings->showLane2Lane);

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
            getViewParent()->getAdditionalFrame()->removeAdditional(i);
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
            if (gSelected.isSelected(GLO_CROSSING, j->getGlID())) {
                crossings.push_back(j);
            }
        }
    }
    // remove selected crossings
    if (crossings.size() > 0) {
        std::string plural = crossings.size() == 1 ? ("") : ("s");
        myUndoList->p_begin("delete selected " + toString(SUMO_TAG_CROSSING) + "s");
        for (auto i : crossings) {
            myNet->deleteCrossing(i, myUndoList);
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
            if (gSelected.isSelected(GLO_CONNECTION, j->getGlID())) {
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
GNEViewNet::deleteSelectedShapes(SumoXMLTag shapeTag) {
    // obtain selected shapes
    std::vector<GNEShape*> selectedShapes = myNet->retrieveShapes(shapeTag, true);
    // remove it
    if (selectedShapes.size() > 0) {
        std::string plural = selectedShapes.size() == 1 ? ("") : ("s");
        myUndoList->p_begin("delete selected " + toString(shapeTag) + plural);
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
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Opening FXMessageBox 'merge junctions'");
            }
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
                if ((answer == 2) && (OptionsCont::getOptions().getBool("gui-testing-debug"))) {
                    WRITE_WARNING("Closed FXMessageBox 'merge junctions' with 'No'");
                } else if ((answer == 4) && (OptionsCont::getOptions().getBool("gui-testing-debug"))) {
                    WRITE_WARNING("Closed FXMessageBox 'merge junctions' with 'ESC'");
                }
                return false;
            } else {
                // write warning if netedit is running in testing mode
                if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                    WRITE_WARNING("Closed FXMessageBox 'merge junctions' with 'Yes'");
                }
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
/****************************************************************************/
