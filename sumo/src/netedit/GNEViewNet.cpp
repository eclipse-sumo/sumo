/****************************************************************************/
/// @file    GNEViewNet.cpp
/// @author  Jakob Erdmann
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2011
/// @version $Id$
///
// A view on the network being edited (adapted from GUIViewTraffic)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
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
#include <foreign/polyfonts/polyfonts.h>
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
#include <utils/gui/windows/GUIDialog_ViewSettings.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/xml/XMLSubSys.h>

#include "GNEViewNet.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEJunction.h"
#include "GNEPOI.h"
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
#include "GNEDeleteFrame.h"
#include "GNEAdditionalHandler.h"
#include "GNEPoly.h"
#include "GNECrossing.h"
#include "GNEAdditional.h"
#include "GNEAdditionalDialog.h"
#include "GNERerouter.h"
#include "GNEConnection.h"


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNEViewNet) GNEViewNetMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_CREATE_EDGE,        GNEViewNet::onCmdSetModeCreateEdge),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_MOVE,               GNEViewNet::onCmdSetModeMove),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_DELETE,             GNEViewNet::onCmdSetModeDelete),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_INSPECT,            GNEViewNet::onCmdSetModeInspect),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_SELECT,             GNEViewNet::onCmdSetModeSelect),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_CONNECT,            GNEViewNet::onCmdSetModeConnect),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_TLS,                GNEViewNet::onCmdSetModeTLS),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONAL,         GNEViewNet::onCmdSetModeAdditional),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_CROSSING,           GNEViewNet::onCmdSetModeCrossing),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SPLIT_EDGE,              GNEViewNet::onCmdSplitEdge),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SPLIT_EDGE_BIDI,         GNEViewNet::onCmdSplitEdgeBidi),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_REVERSE_EDGE,            GNEViewNet::onCmdReverseEdge),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_ADD_REVERSE_EDGE,        GNEViewNet::onCmdAddReversedEdge),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SET_EDGE_ENDPOINT,       GNEViewNet::onCmdSetEdgeEndpoint),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_RESET_EDGE_ENDPOINT,     GNEViewNet::onCmdResetEdgeEndpoint),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_STRAIGHTEN,              GNEViewNet::onCmdStraightenEdges),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SIMPLIFY_SHAPE,          GNEViewNet::onCmdSimplifyShape),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DELETE_GEOMETRY,         GNEViewNet::onCmdDeleteGeometry),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_DUPLICATE_LANE,          GNEViewNet::onCmdDuplicateLane),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_TRANSFORM_LANE_SIDEWALK, GNEViewNet::onCmdRestrictLaneSidewalk),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_TRANSFORM_LANE_BIKE,     GNEViewNet::onCmdRestrictLaneBikelane),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_TRANSFORM_LANE_BUS,      GNEViewNet::onCmdRestrictLaneBuslane),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_REVERT_TRANSFORMATION,   GNEViewNet::onCmdRevertRestriction),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_ADD_LANE_SIDEWALK,       GNEViewNet::onCmdAddRestrictedLaneSidewalk),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_ADD_LANE_BIKE,           GNEViewNet::onCmdAddRestrictedLaneBikelane),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_ADD_LANE_BUS,            GNEViewNet::onCmdAddRestrictedLaneBuslane),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_REMOVE_LANE_SIDEWALK,    GNEViewNet::onCmdRemoveRestrictedLaneSidewalk),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_REMOVE_LANE_BIKE,        GNEViewNet::onCmdRemoveRestrictedLaneBikelane),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_REMOVE_LANE_BUS,         GNEViewNet::onCmdRemoveRestrictedLaneBuslane),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NODE_SHAPE,              GNEViewNet::onCmdNodeShape),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_NODE_REPLACE,            GNEViewNet::onCmdNodeReplace),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SHOW_CONNECTIONS,        GNEViewNet::onCmdToogleShowConnection),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SHOW_BUBBLES,            GNEViewNet::onCmdToogleShowBubbles)
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
    myPreviousEditMode(GNE_MODE_MOVE),
    myShowConnectionActivated(false),
    myCreateEdgeSource(0),
    myJunctionToMove(0),
    myEdgeToMove(0),
    myPolyToMove(0),
    myPoiToMove(0),
    myAdditionalToMove(0),
    myMoveSelection(false),
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
    myEditModeNames(),
    myUndoList(undoList),
    myCurrentPoly(0),
    myTestingMode(OptionsCont::getOptions().getBool("gui-testing")) {
    // view must be the final member of actualParent
    reparent(actualParent);

    buildEditModeControls();
    myUndoList->mark();
    myNet->setViewNet(this);

    ((GUIDanielPerspectiveChanger*)myChanger)->setDragDelay(100000000); // 100 milliseconds

    // init color schemes
    GUIColorer laneColorer;
    GUIColorScheme scheme = GUIColorScheme("uniform", RGBColor::BLACK, "road", true);
    scheme.addColor(RGBColor::GREY, 1, "Sidewalk");
    scheme.addColor(RGBColor(192, 66, 44), 2, "bike lane");
    scheme.addColor(RGBColor(200, 255, 200), 3, "green verge");
    scheme.addColor(RGBColor(150, 200, 200), 4, "waterway");
    scheme.addColor(RGBColor(92, 92, 92), 5, "no passenger"); // paths, service roads etc
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by selection (lane-/streetwise)", RGBColor(128, 128, 128, 255), "unselected", true);
    scheme.addColor(RGBColor(0, 80, 180, 255), 1, "selected");
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by permission code", RGBColor(240, 240, 240), "nobody");
    scheme.addColor(RGBColor(10, 10, 10), (SUMOReal)SVC_PASSENGER, "passenger");
    scheme.addColor(RGBColor(128, 128, 128), (SUMOReal)SVC_PEDESTRIAN, "pedestrian");
    scheme.addColor(RGBColor(80, 80, 80), (SUMOReal)(SVC_PEDESTRIAN | SVC_DELIVERY), "pedestrian_delivery");
    scheme.addColor(RGBColor(192, 66, 44), (SUMOReal)SVC_BICYCLE, "bicycle");
    scheme.addColor(RGBColor(40, 100, 40), (SUMOReal)SVC_BUS, "bus");
    scheme.addColor(RGBColor(166, 147, 26), (SUMOReal)SVC_TAXI, "taxi");
    scheme.addColor(RGBColor::BLACK, (SUMOReal)(SVCAll & ~SVC_NON_ROAD), "normal_road");
    scheme.addColor(RGBColor::BLACK, (SUMOReal)(SVCAll & ~(SVC_PEDESTRIAN | SVC_NON_ROAD)), "disallow_pedestrian");
    scheme.addColor(RGBColor(255, 206, 0), (SUMOReal)(SVCAll & ~(SVC_PEDESTRIAN | SVC_BICYCLE | SVC_MOPED | SVC_NON_ROAD)), "motorway");
    scheme.addColor(RGBColor(150, 200, 200), (SUMOReal)SVC_SHIP, "waterway");
    scheme.addColor(RGBColor::GREEN, (SUMOReal)SVCAll, "all");
    laneColorer.addScheme(scheme);

    scheme = GUIColorScheme("by allowed speed (lanewise)", RGBColor::RED);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)(30 / 3.6));
    scheme.addColor(RGBColor::GREEN, (SUMOReal)(55 / 3.6));
    scheme.addColor(RGBColor::CYAN, (SUMOReal)(80 / 3.6));
    scheme.addColor(RGBColor::BLUE, (SUMOReal)(120 / 3.6));
    scheme.addColor(RGBColor::MAGENTA, (SUMOReal)(150 / 3.6));
    laneColorer.addScheme(scheme);

    scheme = GUIColorScheme("by lane number (streetwise)", RGBColor::RED);
    scheme.addColor(RGBColor::BLUE, (SUMOReal)5);
    laneColorer.addScheme(scheme);

    scheme = GUIColorScheme("by given length/geometrical length", RGBColor::BLACK);
    scheme.addColor(RGBColor::RED, (SUMOReal)0.25);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)0.5);
    scheme.addColor(RGBColor(179, 179, 179, 255), (SUMOReal)1.0);
    scheme.addColor(RGBColor::GREEN, (SUMOReal)2.0);
    scheme.addColor(RGBColor::BLUE, (SUMOReal)4.0);
    laneColorer.addScheme(scheme);
    laneColorer.addScheme(GUIColorScheme("by angle", RGBColor::YELLOW, "", true));

    scheme = GUIColorScheme("by priority", RGBColor::YELLOW);
    scheme.addColor(RGBColor::RED, (SUMOReal) - 20);
    scheme.addColor(RGBColor::GREEN, (SUMOReal)20);
    scheme.setAllowsNegativeValues(true);
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by height at start", RGBColor::RED);
    scheme.addColor(RGBColor::BLUE, (SUMOReal) - 10);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)50);
    scheme.addColor(RGBColor::GREEN, (SUMOReal)100);
    scheme.addColor(RGBColor::MAGENTA, (SUMOReal)200);
    scheme.setAllowsNegativeValues(true);
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by height at segment start", RGBColor::RED);
    scheme.addColor(RGBColor::BLUE, (SUMOReal) - 10);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal)50);
    scheme.addColor(RGBColor::GREEN, (SUMOReal)100);
    scheme.addColor(RGBColor::MAGENTA, (SUMOReal)200);
    scheme.setAllowsNegativeValues(true);
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by inclination", RGBColor::GREY);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal) .1);
    scheme.addColor(RGBColor::RED, (SUMOReal) .3);
    scheme.addColor(RGBColor::GREEN, (SUMOReal) - .1);
    scheme.addColor(RGBColor::BLUE, (SUMOReal) - .3);
    scheme.setAllowsNegativeValues(true);
    laneColorer.addScheme(scheme);
    scheme = GUIColorScheme("by segment inclination", RGBColor::GREY);
    scheme.addColor(RGBColor::YELLOW, (SUMOReal) .1);
    scheme.addColor(RGBColor::RED, (SUMOReal) .3);
    scheme.addColor(RGBColor::GREEN, (SUMOReal) - .1);
    scheme.addColor(RGBColor::BLUE, (SUMOReal) - .3);
    scheme.setAllowsNegativeValues(true);
    laneColorer.addScheme(scheme);

    myVisualizationSettings->laneColorer = laneColorer;

    GUIColorer junctionColorer;
    scheme = GUIColorScheme("uniform", RGBColor(102, 0, 0), "", true);
    scheme.addColor(RGBColor(204, 0, 0), 1, "shape not computed");
    scheme.addColor(RGBColor(153, 0, 0), 2, "geometry points");
    junctionColorer.addScheme(scheme);
    scheme = GUIColorScheme("by selection", RGBColor(128, 128, 128, 255), "unselected", true);
    scheme.addColor(RGBColor(0, 80, 180, 255), 1, "selected");
    junctionColorer.addScheme(scheme);
    scheme = GUIColorScheme("by type", RGBColor::GREEN, "traffic_light", true);
    scheme.addColor(RGBColor(0, 128, 0), 1, "traffic_light_unregulated");
    scheme.addColor(RGBColor::YELLOW, 2, "priority");
    scheme.addColor(RGBColor::RED, 3, "priority_stop");
    scheme.addColor(RGBColor::BLUE, 4, "right_before_left");
    scheme.addColor(RGBColor::CYAN, 5, "allway_stop");
    scheme.addColor(RGBColor::GREY, 6, "district");
    scheme.addColor(RGBColor::MAGENTA, 7, "unregulated");
    scheme.addColor(RGBColor::BLACK, 8, "dead_end");
    scheme.addColor(RGBColor::ORANGE, 9, "rail_signal");
    scheme.addColor(RGBColor(192, 128, 64), 10, "zipper");
    scheme.addColor(RGBColor(192, 255, 192), 11, "traffic_light_right_on_red");
    scheme.addColor(RGBColor(128, 0, 128), 12, "rail_crossing"); // dark purple
    junctionColorer.addScheme(scheme);
    myVisualizationSettings->junctionColorer = junctionColorer;

    if (myTestingMode && OptionsCont::getOptions().isSet("window-size")) {
        std::vector<std::string> windowSize = OptionsCont::getOptions().getStringVector("window-size");
        assert(windowSize.size() == 2);
        myTestingWidth = TplConvert::_str2int(windowSize[0]);
        myTestingHeight = TplConvert::_str2int(windowSize[1]);
    }
}


GNEViewNet::~GNEViewNet() { }


void
GNEViewNet::doInit() {}


void
GNEViewNet::buildViewToolBars(GUIGlChildWindow& cw) {
    /*
    // build coloring tools
    {
        const std::vector<std::string> &names = gSchemeStorage.getNames();
        for (std::vector<std::string>::const_iterator i=names.begin(); i!=names.end(); ++i) {
            v.getColoringSchemesCombo().appendItem((*i).c_str());
            if ((*i) == myVisualizationSettings->name) {
                v.getColoringSchemesCombo().setCurrentItem(v.getColoringSchemesCombo().getNumItems()-1);
            }
        }
        v.getColoringSchemesCombo().setNumVisible(5);
    }
    */

    // locator button for junctions
    new FXButton(cw.getLocatorPopup(),
                 "\tLocate Junction\tLocate a junction within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATEJUNCTION), &cw, MID_LOCATEJUNCTION,
                 ICON_ABOVE_TEXT | FRAME_THICK | FRAME_RAISED);
    // locator button for edges
    new FXButton(cw.getLocatorPopup(),
                 "\tLocate Street\tLocate a street within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATEEDGE), &cw, MID_LOCATEEDGE,
                 ICON_ABOVE_TEXT | FRAME_THICK | FRAME_RAISED);
    // locator button for tls
    new FXButton(cw.getLocatorPopup(),
                 "\tLocate TLS\tLocate a traffic light within the network.",
                 GUIIconSubSys::getIcon(ICON_LOCATETLS), &cw, MID_LOCATETLS,
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
GNEViewNet::setStatusBarText(const std::string& text) {
    myApp->setStatusBarText(text);
}


bool
GNEViewNet::selectEdges() {
    return mySelectEdges->getCheck() != 0;
}


bool
GNEViewNet::showConnections() {
    if (myEditMode == GNE_MODE_CONNECT) {
        return true;
    } else if (myShowConnections->shown() == false) {
        return false;
    } else {
        return (myShowConnections->getCheck() == 1);
    }
}


bool
GNEViewNet::autoSelectNodes() {
    return myExtendToEdgeNodes->getCheck() != 0;
}


void
GNEViewNet::setSelectionScaling(SUMOReal selectionScale) {
    myVisualizationSettings->selectionScale = selectionScale;
}


bool
GNEViewNet::changeAllPhases() const {
    return myChangeAllPhases->getCheck() != FALSE;
}


bool
GNEViewNet::showJunctionAsBubbles() const {
    return (myEditMode == GNE_MODE_MOVE) && (myShowBubbleOverJunction->getCheck());
}


int
GNEViewNet::doPaintGL(int mode, const Boundary& bound) {
    // init view settings
    glRenderMode(mode);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
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
    SUMOReal lw = m2p(SUMO_const_laneWidth);
    // draw decals (if not in grabbing mode)
    if (!myUseToolTips) {
        drawDecals();
        if (myVisualizationSettings->showGrid) {
            paintGLGrid();
        }
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
            const SUMOReal size = p2m(32);
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
GNEViewNet::onLeftBtnPress(FXObject* obj, FXSelector sel, void* data) {
    FXEvent* e = (FXEvent*) data;
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
        GNEPoly* pointed_poly = 0;
        GNECrossing* pointed_crossing = 0;
        GNEAdditional* pointed_additional = 0;
        GNEConnection* pointed_connection = 0;
        if (pointed) {
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
                    pointed_poi = (GNEPOI*)pointed;
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

        // decide what to do based on mode
        switch (myEditMode) {
            case GNE_MODE_CREATE_EDGE: {
                if ((e->state & CONTROLMASK) == 0) {
                    // allow moving when control is held down
                    if (!myUndoList->hasCommandGroup()) {
                        myUndoList->p_begin("create new " + toString(SUMO_TAG_EDGE));
                    }
                    if (!pointed_junction) {
                        pointed_junction = myNet->createJunction(getPositionInformation(), myUndoList);
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
                                if (myAutoCreateOppositeEdge->getCheck()) {
                                    myNet->createEdge(
                                        pointed_junction, myCreateEdgeSource, myViewParent->getInspectorFrame()->getEdgeTemplate(), myUndoList, "-" + newEdge->getNBEdge()->getID());
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
                                setStatusBarText("An edge with the same geometry already exists!");
                            }
                        } else {
                            setStatusBarText("Start- and endpoint for an edge must be distinct!");
                        }
                        update();
                    }
                }
                GUISUMOAbstractView::onLeftBtnPress(obj, sel, data);
                break;
            }
            case GNE_MODE_MOVE: {
                if (pointed_poly) {
                    myPolyToMove = pointed_poly;
                    myMoveSrc = getPositionInformation();
                } else if (pointed_poi) {
                    myPoiToMove = pointed_poi;
                    myMoveSrc = getPositionInformation();
                } else if (pointed_junction) {
                    if (gSelected.isSelected(GLO_JUNCTION, pointed_junction->getGlID())) {
                        myMoveSelection = true;
                    } else {
                        myJunctionToMove = pointed_junction;
                    }
                    myMoveSrc = getPositionInformation();
                } else if (pointed_edge) {
                    if (gSelected.isSelected(GLO_EDGE, pointed_edge->getGlID())) {
                        myMoveSelection = true;
                    } else {
                        myEdgeToMove = pointed_edge;
                    }
                    myMoveSrc = getPositionInformation();
                } else if (pointed_additional) {
                    if (gSelected.isSelected(GLO_ADDITIONAL, pointed_additional->getGlID())) {
                        myMoveSelection = true;
                    } else {
                        // Only move additional if can be moved
                        if (pointed_additional->isAdditionalBlocked() == false) {
                            myAdditionalToMove = pointed_additional;
                            if (myAdditionalToMove->getLane()) {
                                if (GNEAttributeCarrier::hasAttribute(myAdditionalToMove->getTag(), SUMO_ATTR_STARTPOS)) {
                                    // Obtain start position
                                    SUMOReal startPos = GNEAttributeCarrier::parse<SUMOReal>(myAdditionalToMove->getAttribute(SUMO_ATTR_STARTPOS));
                                    if (GNEAttributeCarrier::hasAttribute(myAdditionalToMove->getTag(), SUMO_ATTR_ENDPOS)) {
                                        // Obtain end position
                                        SUMOReal endPos = GNEAttributeCarrier::parse<SUMOReal>(myAdditionalToMove->getAttribute(SUMO_ATTR_ENDPOS));
                                        // Save both values in myOldAdditionalPosition
                                        myOldAdditionalPosition.set(startPos, endPos);
                                    } else if (GNEAttributeCarrier::hasAttribute(myAdditionalToMove->getTag(), SUMO_ATTR_LENGTH)) {
                                        // Obtain length attribute
                                        SUMOReal length = GNEAttributeCarrier::parse<SUMOReal>(myAdditionalToMove->getAttribute(SUMO_ATTR_LENGTH));
                                        // Save both values in myOldAdditionalPosition
                                        myOldAdditionalPosition.set(startPos, length);
                                    } else {
                                        // Save only startpos in myOldAdditionalPosition
                                        myOldAdditionalPosition.set(startPos, 0);
                                    }
                                }
                                // Set myAdditionalMovingReference
                                myAdditionalMovingReference.set(pointed_additional->getLane()->getShape().nearest_offset_to_point2D(getPositionInformation(), false), 0, 0);
                            } else {
                                // Set myOldAdditionalPosition and myAdditionalMovingReference
                                myOldAdditionalPosition = getPositionInformation();
                                myAdditionalMovingReference = pointed_additional->getPositionInView() - getPositionInformation();
                            }
                        }
                    }
                } else {
                    GUISUMOAbstractView::onLeftBtnPress(obj, sel, data);
                }
                update();
                break;
            }
            case GNE_MODE_DELETE: {
                // Check if Control key is pressed
                bool markElementMode = (((FXEvent*)data)->state & CONTROLMASK) != 0;
                GNEAttributeCarrier* ac = dynamic_cast<GNEAttributeCarrier*>(pointed);
                if (ac) {
                    // if pointed element is an attribute carrier, remove it or mark it
                    if (markElementMode) {
                        if (myViewParent->getDeleteFrame()->getMarkedAttributeCarrier() != ac) {
                            myViewParent->getDeleteFrame()->markAttributeCarrier(ac);
                            myViewParent->getDeleteFrame()->showAttributeCarrierChilds(ac);
                        }
                    } else if (myViewParent->getDeleteFrame()->getMarkedAttributeCarrier() != NULL) {
                        myViewParent->getDeleteFrame()->markAttributeCarrier(NULL);
                        myViewParent->getDeleteFrame()->showAttributeCarrierChilds(ac);
                    } else {
                        myViewParent->getDeleteFrame()->removeAttributeCarrier(ac);
                    }
                } else {
                    GUISUMOAbstractView::onLeftBtnPress(obj, sel, data);
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
                    if (selectEdges()) {
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
                }
                // obtain selected ACs
                std::vector<GNEAttributeCarrier*> selected;
                if (pointedO && gSelected.isSelected(pointedO->getType(), pointedO->getGlID())) {
                    std::set<GUIGlID> selectedIDs = gSelected.getSelected(pointedO->getType());
                    selected = myNet->retrieveAttributeCarriers(selectedIDs, pointedO->getType());
                } else if (pointedAC != 0) {
                    selected.push_back(pointedAC);
                }
                // Inspect seleted ACs, or single clicked AC
                myViewParent->getInspectorFrame()->inspectMultisection(selected);
                GUISUMOAbstractView::onLeftBtnPress(obj, sel, data);
                update();
                break;
            }
            case GNE_MODE_SELECT:
                if (pointed_lane && selectEdges()) {
                    gSelected.toggleSelection(pointed_edge->getGlID());
                } else if (pointed) {
                    gSelected.toggleSelection(pointed->getGlID());
                }

                myAmInRectSelect = (((FXEvent*)data)->state & SHIFTMASK) != 0;
                if (myAmInRectSelect) {
                    mySelCorner1 = getPositionInformation();
                    mySelCorner2 = getPositionInformation();
                } else {
                    GUISUMOAbstractView::onLeftBtnPress(obj, sel, data);
                }
                update();
                break;

            case GNE_MODE_CONNECT: {
                if (pointed_lane) {
                    const bool mayPass = (((FXEvent*)data)->state & SHIFTMASK) != 0;
                    const bool allowConflict = (((FXEvent*)data)->state & CONTROLMASK) != 0;
                    myViewParent->getConnectorFrame()->handleLaneClick(pointed_lane, mayPass, allowConflict, true);
                    update();
                }
                GUISUMOAbstractView::onLeftBtnPress(obj, sel, data);
                break;
            }
            case GNE_MODE_TLS: {
                if (pointed_junction) {
                    myViewParent->getTLSEditorFrame()->editJunction(pointed_junction);
                    update();
                }
                GUISUMOAbstractView::onLeftBtnPress(obj, sel, data);
                break;
            }
            case GNE_MODE_ADDITIONAL: {
                if (pointed_additional == NULL) {
                    GNENetElement* netElement = dynamic_cast<GNENetElement*>(pointed);
                    if (myViewParent->getAdditionalFrame()->addAdditional(netElement, this)) {
                        update();
                    }
                }
                GUISUMOAbstractView::onLeftBtnPress(obj, sel, data);
                break;
            }
            case GNE_MODE_CROSSING: {
                if (pointed_crossing == NULL) {
                    GNENetElement* netElement = dynamic_cast<GNENetElement*>(pointed);
                    if (myViewParent->getCrossingFrame()->addCrossing(netElement)) {
                        update();
                    }
                }
                GUISUMOAbstractView::onLeftBtnPress(obj, sel, data);
                break;
            }
            default: {
                GUISUMOAbstractView::onLeftBtnPress(obj, sel, data);
            }
        }
        makeNonCurrent();
    }
    return 1;
}


long
GNEViewNet::onLeftBtnRelease(FXObject* obj, FXSelector sel, void* data) {
    GUISUMOAbstractView::onLeftBtnRelease(obj, sel, data);
    if (myPolyToMove) {
        myPolyToMove = 0;
    } else if (myPoiToMove) {
        myPoiToMove = 0;
    } else if (myJunctionToMove) {
        // position is already up to date but we must register with myUndoList
        if (!mergeJunctions(myJunctionToMove)) {
            myJunctionToMove->registerMove(myUndoList);
        }
        myJunctionToMove = 0;
    } else if (myEdgeToMove) {
        // shape is already up to date but we must register with myUndoList
        const std::string& newShape = myEdgeToMove->getAttribute(SUMO_ATTR_SHAPE);
        myEdgeToMove->setAttribute(SUMO_ATTR_SHAPE, newShape, myUndoList);
        myEdgeToMove = 0;
    } else if (myAdditionalToMove) {
        if (myAdditionalToMove->getLane()) {
            myAdditionalToMove->commmitAdditionalGeometryMoved(myOldAdditionalPosition, myUndoList);
            myAdditionalToMove = 0;
        } else {
            myAdditionalToMove->commmitAdditionalGeometryMoved(myOldAdditionalPosition + myAdditionalMovingReference, myUndoList);
            myAdditionalToMove = 0;
        }
    } else if (myMoveSelection) {
        // positions and shapes are already up to date but we must register with myUndoList
        myNet->finishMoveSelection(myUndoList);
        myMoveSelection = false;
    } else if (myAmInRectSelect) {
        myAmInRectSelect = false;
        // shift held down on mouse-down and mouse-up
        if (((FXEvent*)data)->state & SHIFTMASK) {
            if (makeCurrent()) {
                Boundary b;
                b.add(mySelCorner1);
                b.add(mySelCorner2);
                myViewParent->getSelectorFrame()->handleIDs(getObjectsInBoundary(b), selectEdges());
                makeNonCurrent();
            }
        }
        update();
    }
    return 1;
}


long
GNEViewNet::onDoubleClicked(FXObject*, FXSelector, void*) {
    // If current edit mode is INSPECT or ADDITIONAL
    if (myEditMode == GNE_MODE_INSPECT || myEditMode == GNE_MODE_ADDITIONAL) {
        setFocus();
        // interpret object under curser
        if (makeCurrent()) {
            int id = getObjectUnderCursor();
            GUIGlObject* pointed = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
            GUIGlObjectStorage::gIDStorage.unblockObject(id);
            GNEAdditional* pointed_additional = dynamic_cast<GNEAdditional*>(pointed);
            // If pointed element is an additional
            if (pointed_additional != NULL) {
                // If additional has a additional dialog, open it.
                pointed_additional->openAdditionalDialog();
            }
            makeNonCurrent();
        }
    }
    return 1;
}


long
GNEViewNet::onMouseMove(FXObject* obj, FXSelector sel, void* data) {
    GUISUMOAbstractView::onMouseMove(obj, sel, data);
    // in delete mode object under cursor must be checked in every mouse movement
    if (myEditMode == GNE_MODE_DELETE) {
        setFocus();
        // show object information in delete frame
        if (makeCurrent()) {
            // obtain ac of globjectID
            int glid = getObjectUnderCursor();
            GNEAttributeCarrier* ac = dynamic_cast<GNEAttributeCarrier*>(GUIGlObjectStorage::gIDStorage.getObjectBlocking(glid));
            GUIGlObjectStorage::gIDStorage.unblockObject(glid);
            // Update current label of delete frame
            myViewParent->getDeleteFrame()->updateCurrentLabel(ac);
            // show childs of object in GNEDeleteFrame if current there isn't a marked element
            if (myViewParent->getDeleteFrame()->getMarkedAttributeCarrier() == NULL) {
                myViewParent->getDeleteFrame()->showAttributeCarrierChilds(ac);
            }
        }
    } else {
        if (myPolyToMove) {
            myMoveSrc = myPolyToMove->moveGeometry(myMoveSrc, getPositionInformation());
        } else if (myPoiToMove) {
            myPoiToMove->move(getPositionInformation());
        } else if (myJunctionToMove) {
            myJunctionToMove->move(getPositionInformation());
        } else if (myEdgeToMove) {
            myMoveSrc = myEdgeToMove->moveGeometry(myMoveSrc, getPositionInformation());
        } else if (myAdditionalToMove) {
            // If additional is placed over lane, move it across it
            if (myAdditionalToMove->getLane()) {
                SUMOReal posOfMouseOverLane = myAdditionalToMove->getLane()->getShape().nearest_offset_to_point2D(getPositionInformation(), false);
                myAdditionalToMove->moveAdditionalGeometry(posOfMouseOverLane - myAdditionalMovingReference.x(), 0);
                myAdditionalMovingReference.set(posOfMouseOverLane, 0, 0);
            } else {
                // Calculate offset movement
                Position offsetPosition = getPositionInformation() - myOldAdditionalPosition;
                myAdditionalToMove->moveAdditionalGeometry(myOldAdditionalPosition + offsetPosition + myAdditionalMovingReference);
            }
            update();
        } else if (myMoveSelection) {
            Position moveTarget = getPositionInformation();
            myNet->moveSelection(myMoveSrc, moveTarget);
            myMoveSrc = moveTarget;
        } else if (myAmInRectSelect) {
            mySelCorner2 = getPositionInformation();
            update();
        }
    }
    return 1;
}


void
GNEViewNet::abortOperation(bool clearSelection) {
    setFocus(); // steal focus from any text fields
    if (myCreateEdgeSource) {
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
        removeCurrentPoly();
    }
    myUndoList->p_abort();
}


void
GNEViewNet::hotkeyDel() {
    if (myEditMode == GNE_MODE_CONNECT || myEditMode == GNE_MODE_TLS) {
        setStatusBarText("Cannot delete in this mode");
    } else {
        myUndoList->p_begin("delete selection");
        deleteSelectedJunctions();
        deleteSelectedEdges();
        myUndoList->p_end();
    }
}


void
GNEViewNet::hotkeyEnter() {
    if (myEditMode == GNE_MODE_CONNECT) {
        myViewParent->getConnectorFrame()->onCmdOK(0, 0, 0);
    } else if (myEditMode == GNE_MODE_TLS) {
        myViewParent->getTLSEditorFrame()->onCmdOK(0, 0, 0);
    } else if (myEditMode == GNE_MODE_MOVE && myCurrentPoly != 0) {
        if (myCurrentPoly->getEditedJunction() != 0) {
            myCurrentPoly->getEditedJunction()->setAttribute(SUMO_ATTR_SHAPE, toString(myCurrentPoly->getShape()), myUndoList);
            removeCurrentPoly();
            update();
        }
    }
}


void
GNEViewNet::setEditModeFromHotkey(FXushort selid) {
    switch (selid) {
        case MID_GNE_MODE_CREATE_EDGE:
            setEditMode(GNE_MODE_CREATE_EDGE);
            break;
        case MID_GNE_MODE_MOVE:
            setEditMode(GNE_MODE_MOVE);
            break;
        case MID_GNE_MODE_DELETE:
            setEditMode(GNE_MODE_DELETE);
            break;
        case MID_GNE_MODE_INSPECT:
            setEditMode(GNE_MODE_INSPECT);
            break;
        case MID_GNE_MODE_SELECT:
            setEditMode(GNE_MODE_SELECT);
            break;
        case MID_GNE_MODE_CONNECT:
            setEditMode(GNE_MODE_CONNECT);
            break;
        case MID_GNE_MODE_TLS:
            setEditMode(GNE_MODE_TLS);
            break;
        case MID_GNE_MODE_ADDITIONAL:
            setEditMode(GNE_MODE_ADDITIONAL);
            break;
        case MID_GNE_MODE_CROSSING:
            setEditMode(GNE_MODE_CROSSING);
            break;
        default:
            FXMessageBox::error(this, MBOX_OK, "invalid edit mode", "%s", "...");
            break;
    }
}


void
GNEViewNet::markPopupPosition() {
    myPopupSpot = getPositionInformation();
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
GNEViewNet::getJunctionAtCursorPosition(Position& /* pos */) {
    GNEJunction* junction = 0;
    if (makeCurrent()) {
        int id = getObjectAtPosition(myPopupSpot);
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




GNEEdge*
GNEViewNet::getEdgeAtCursorPosition(Position& /* pos */) {
    GNEEdge* edge = 0;
    if (makeCurrent()) {
        int id = getObjectAtPosition(myPopupSpot);
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
GNEViewNet::getLaneAtCurserPosition(Position& /* pos */) {
    GNELane* lane = 0;
    if (makeCurrent()) {
        int id = getObjectAtPosition(myPopupSpot);
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
GNEViewNet::getEdgesAtCursorPosition(Position& /* pos */) {
    std::set<GNEEdge*> result;
    if (makeCurrent()) {
        const std::vector<GUIGlID> ids = getObjectsAtPosition(myPopupSpot, 1.0);
        for (std::vector<GUIGlID>::const_iterator it = ids.begin(); it != ids.end(); ++it) {
            GUIGlObject* pointed = GUIGlObjectStorage::gIDStorage.getObjectBlocking(*it);
            GUIGlObjectStorage::gIDStorage.unblockObject(*it);
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
GNEViewNet::onCmdSplitEdge(FXObject*, FXSelector, void*) {
    GNEEdge* edge = getEdgeAtCursorPosition(myPopupSpot);
    if (edge != 0) {
        myNet->splitEdge(edge, edge->getSplitPos(myPopupSpot), myUndoList);
    }
    return 1;
}


long
GNEViewNet::onCmdSplitEdgeBidi(FXObject*, FXSelector, void*) {
    std::set<GNEEdge*> edges = getEdgesAtCursorPosition(myPopupSpot);
    if (edges.size() != 0) {
        myNet->splitEdgesBidi(edges, (*edges.begin())->getSplitPos(myPopupSpot), myUndoList);
    }
    return 1;
}


long
GNEViewNet::onCmdReverseEdge(FXObject*, FXSelector, void*) {
    GNEEdge* edge = getEdgeAtCursorPosition(myPopupSpot);
    if (edge != 0) {
        myNet->reverseEdge(edge, myUndoList);
    }
    return 1;
}


long
GNEViewNet::onCmdAddReversedEdge(FXObject*, FXSelector, void*) {
    GNEEdge* edge = getEdgeAtCursorPosition(myPopupSpot);
    if (edge != 0) {
        myNet->addReversedEdge(edge, myUndoList);
    }
    return 1;
}


long
GNEViewNet::onCmdSetEdgeEndpoint(FXObject*, FXSelector, void*) {
    GNEEdge* edge = getEdgeAtCursorPosition(myPopupSpot);
    if (edge != 0) {
        edge->setEndpoint(myPopupSpot, myUndoList);
    }
    return 1;
}


long
GNEViewNet::onCmdResetEdgeEndpoint(FXObject*, FXSelector, void*) {
    GNEEdge* edge = getEdgeAtCursorPosition(myPopupSpot);
    if (edge != 0) {
        edge->resetEndpoint(myPopupSpot, myUndoList);
    }
    return 1;
}


long
GNEViewNet::onCmdStraightenEdges(FXObject*, FXSelector, void*) {
    GNEEdge* edge = getEdgeAtCursorPosition(myPopupSpot);
    if (edge != 0) {
        if (gSelected.isSelected(GLO_EDGE, edge->getGlID())) {
            myUndoList->p_begin("straighten selected " + toString(SUMO_TAG_EDGE) + "s");
            std::vector<GNEEdge*> edges = myNet->retrieveEdges(true);
            for (std::vector<GNEEdge*>::iterator it = edges.begin(); it != edges.end(); it++) {
                (*it)->setAttribute(SUMO_ATTR_SHAPE, "", myUndoList);
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
GNEViewNet::onCmdSimplifyShape(FXObject*, FXSelector, void*) {
    if (myCurrentPoly != 0) {
        myCurrentPoly->simplifyShape();
        update();
    }
    return 1;
}


long
GNEViewNet::onCmdDeleteGeometry(FXObject*, FXSelector, void*) {
    if (myCurrentPoly != 0) {
        myCurrentPoly->deleteGeometryNear(myPopupSpot);
        update();
    }
    return 1;
}


long
GNEViewNet::onCmdDuplicateLane(FXObject*, FXSelector, void*) {
    GNELane* lane = getLaneAtCurserPosition(myPopupSpot);
    if (lane != 0) {
        if (gSelected.isSelected(GLO_LANE, lane->getGlID())) {
            myUndoList->p_begin("duplicate selected " + toString(SUMO_TAG_LANE) + "s");
            std::vector<GNELane*> lanes = myNet->retrieveLanes(true);
            for (std::vector<GNELane*>::iterator it = lanes.begin(); it != lanes.end(); it++) {
                myNet->duplicateLane(*it, myUndoList);
            }
            myUndoList->p_end();
        } else {
            myUndoList->p_begin("duplicate " + toString(SUMO_TAG_LANE));
            myNet->duplicateLane(lane, myUndoList);
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


bool
GNEViewNet::restrictLane(SUMOVehicleClass vclass) {
    GNELane* lane = getLaneAtCurserPosition(myPopupSpot);
    if (lane != 0) {
        // Get selected lanes
        std::vector<GNELane*> lanes = myNet->retrieveLanes(true); ;
        // Declare map of edges and lanes
        std::map<GNEEdge*, GNELane*> mapOfEdgesAndLanes;
        // Iterate over selected lanes
        for (std::vector<GNELane*>::iterator i = lanes.begin(); i != lanes.end(); i++) {
            mapOfEdgesAndLanes[myNet->retrieveEdge((*i)->getParentEdge().getID())] = (*i);
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
            for (std::map<GNEEdge*, GNELane*>::iterator i = mapOfEdgesAndLanes.begin(); i != mapOfEdgesAndLanes.end(); i++) {
                if (i->first->hasRestrictedLane(vclass)) {
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
                // Ask confirmation to user
                FXuint answer = FXMessageBox::question(getApp(), MBOX_YES_NO,
                                                       ("Set vclass for " + toString(vclass) + " to selected lanes").c_str(), "%s",
                                                       (toString(mapOfEdgesAndLanes.size() - counter) + " lanes will be restricted for " + toString(vclass) + ". continue?").c_str());
                if (answer != 1) { //1:yes, 2:no, 4:esc
                    return 0;
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
    GNELane* lane = getLaneAtCurserPosition(myPopupSpot);
    if (lane != 0) {
        // Get selected edges
        std::vector<GNEEdge*> edges = myNet->retrieveEdges(true);
        // get selected lanes
        std::vector<GNELane*> lanes = myNet->retrieveLanes(true);
        // Declare set of edges
        std::set<GNEEdge*> setOfEdges;
        // Fill set of edges with vector of edges
        for (std::vector<GNEEdge*>::iterator i = edges.begin(); i != edges.end(); i++) {
            setOfEdges.insert(*i);
        }
        // iterate over selected lanes
        for (std::vector<GNELane*>::iterator it = lanes.begin(); it != lanes.end(); it++) {
            // Insert pointer to edge into set of edges (To avoid duplicates)
            setOfEdges.insert(myNet->retrieveEdge((*it)->getParentEdge().getID()));
        }
        // If we handeln a set of edges
        if (setOfEdges.size() > 0) {
            // declare counter for number of restrictions
            int counter = 0;
            // iterate over set of edges
            for (std::set<GNEEdge*>::iterator it = setOfEdges.begin(); it != setOfEdges.end(); it++) {
                // update counter if edge has already a restricted lane of type "vclass"
                if ((*it)->hasRestrictedLane(vclass) == true) {
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
                // Ask confirmation to user
                FXuint answer = FXMessageBox::question(getApp(), MBOX_YES_NO,
                                                       ("Add vclass for " + toString(vclass) + " to selected lanes").c_str(), "%s",
                                                       (toString(setOfEdges.size() - counter) + " restrictions for " + toString(vclass) + " will be added. continue?").c_str());
                if (answer != 1) { //1:yes, 2:no, 4:esc
                    return 0;
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
    GNELane* lane = getLaneAtCurserPosition(myPopupSpot);
    if (lane != 0) {
        // Get selected edges
        std::vector<GNEEdge*> edges = myNet->retrieveEdges(true);
        // get selected lanes
        std::vector<GNELane*> lanes = myNet->retrieveLanes(true);
        // Declare set of edges
        std::set<GNEEdge*> setOfEdges;
        // Fill set of edges with vector of edges
        for (std::vector<GNEEdge*>::iterator i = edges.begin(); i != edges.end(); i++) {
            setOfEdges.insert(*i);
        }
        // iterate over selected lanes
        for (std::vector<GNELane*>::iterator it = lanes.begin(); it != lanes.end(); it++) {
            // Insert pointer to edge into set of edges (To avoid duplicates)
            setOfEdges.insert(myNet->retrieveEdge((*it)->getParentEdge().getID()));
        }
        // If we handeln a set of edges
        if (setOfEdges.size() > 0) {
            // declare counter for number of restrictions
            int counter = 0;
            // iterate over set of edges
            for (std::set<GNEEdge*>::iterator it = setOfEdges.begin(); it != setOfEdges.end(); it++) {
                // update counter if edge has already a restricted lane of type "vclass"
                if ((*it)->hasRestrictedLane(vclass) == true) {
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
                // Ask confirmation to user
                FXuint answer = FXMessageBox::question(getApp(), MBOX_YES_NO,
                                                       ("Remove vclass for " + toString(vclass) + " to selected lanes").c_str(), "%s",
                                                       (toString(counter) + " restrictions for " + toString(vclass) + " will be removed. continue?").c_str());
                if (answer != 1) { //1:yes, 2:no, 4:esc
                    return 0;
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


long
GNEViewNet::onCmdRevertRestriction(FXObject*, FXSelector, void*) {
    GNELane* lane = getLaneAtCurserPosition(myPopupSpot);
    if (lane != 0) {
        // Declare vector of lanes
        std::vector<GNELane*> lanes;
        // Check if we have a set of selected edges or lanes
        if (gSelected.isSelected(GLO_EDGE, lane->getParentEdge().getGlID())) {
            // Get selected edgeds
            std::vector<GNEEdge*> edges = myNet->retrieveEdges(true);
            // fill vector of lanes with the lanes of selected edges
            for (std::vector<GNEEdge*>::iterator i = edges.begin(); i != edges.end(); i++) {
                for (std::vector<GNELane*>::const_iterator j = (*i)->getLanes().begin(); j != (*i)->getLanes().end(); j++) {
                    lanes.push_back(*j);
                }
            }
        } else if (gSelected.isSelected(GLO_LANE, lane->getGlID())) {
            // get selected lanes
            lanes = myNet->retrieveLanes(true);
        }
        // If we handeln a set of lanes
        if (lanes.size() > 0) {
            // declare counter for number of Sidewalks
            int counter = 0;
            // iterate over selected lanes
            for (std::vector<GNELane*>::iterator it = lanes.begin(); it != lanes.end(); it++) {
                if (((*it)->isRestricted(SVC_PEDESTRIAN)) || ((*it)->isRestricted(SVC_BICYCLE)) || ((*it)->isRestricted(SVC_BUS))) {
                    counter++;
                }
            }
            // if none of selected lanes has a transformation, stop
            if (counter == 0) {
                FXMessageBox::information(getApp(), MBOX_OK,
                                          "Revert restriction", "%s",
                                          "None of selected lanes has a previous restriction");
                return 0;
            } else {
                // Ask confirmation to user
                FXuint answer = FXMessageBox::question(getApp(), MBOX_YES_NO,
                                                       "Revert restriction", "%s",
                                                       (toString(counter) + " restrictions of lanes lanes will be reverted. continue?").c_str());
                if (answer != 1) { //1:yes, 2:no, 4:esc
                    return 0;
                }
            }
            // begin undo operation
            myUndoList->p_begin("revert restrictions");
            // iterate over selected lanes
            for (std::vector<GNELane*>::iterator it = lanes.begin(); it != lanes.end(); it++) {
                // revert transformation
                myNet->revertLaneRestriction(*it, myUndoList);
            }
            // end undo operation
            myUndoList->p_end();
        } else {
            // If only have a single lane, start undo/redo operation
            myUndoList->p_begin("revert restriction");
            // revert transformation
            myNet->revertLaneRestriction(lane, myUndoList);
            // end undo operation
            myUndoList->p_end();
        }
    }
    return 1;
}


long
GNEViewNet::onCmdNodeShape(FXObject*, FXSelector, void*) {
    GNEJunction* junction = getJunctionAtCursorPosition(myPopupSpot);
    if (junction != 0) {
        if (myCurrentPoly == 0) {
            if (junction->getNBNode()->getShape().size() > 1) {
                setEditModeFromHotkey(MID_GNE_MODE_MOVE);
                //std::cout << junction->getNBNode()->getShape() << "\n";
                junction->getNBNode()->computeNodeShape(-1);
                if (myCurrentPoly != 0) {
                    myNet->getVisualisationSpeedUp().removeAdditionalGLObject(myCurrentPoly);
                    delete myCurrentPoly;
                    myCurrentPoly = 0;
                }
                PositionVector shape = junction->getNBNode()->getShape();
                shape.closePolygon();
                myCurrentPoly = new GNEPoly(myNet, junction, "junction_shape:" + junction->getMicrosimID(), "junction shape",
                                            shape, false, RGBColor::GREEN, GLO_POLYGON);
                myCurrentPoly->setLineWidth(0.3);
                myNet->getVisualisationSpeedUp().addAdditionalGLObject(myCurrentPoly);

                update();
            }
        } else {
            junction->setAttribute(SUMO_ATTR_SHAPE, toString(myCurrentPoly->getShape()), myUndoList);
            removeCurrentPoly();
            update();
        }
    }
    return 1;
}


void
GNEViewNet::removeCurrentPoly() {
    if (myCurrentPoly != 0) {
        myNet->getVisualisationSpeedUp().removeAdditionalGLObject(myCurrentPoly);
        delete myCurrentPoly;
        myCurrentPoly = 0;
    }
}


long
GNEViewNet::onCmdNodeReplace(FXObject*, FXSelector, void*) {
    GNEJunction* junction = getJunctionAtCursorPosition(myPopupSpot);
    if (junction != 0) {
        myNet->replaceJunctionByGeometry(junction, myUndoList);
        update();
    }
    return 1;
}


long
GNEViewNet::onCmdToogleShowConnection(FXObject*, FXSelector, void*) {
    if (!myShowConnectionActivated) {
        getNet()->initGNEConnections();
        myShowConnectionActivated = true;
    }
    // Update viewnNet to show/hide conections
    update();
    // Hide/show connections requiere recompute
    getNet()->requireRecompute();
    return 1;
}


long
GNEViewNet::onCmdToogleShowBubbles(FXObject*, FXSelector, void*) {
    // Update view net Shapes
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
    if (mode == myEditMode) {
        setStatusBarText("Mode already selected");
    } else {
        myPreviousEditMode = myEditMode;
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
    }
    updateModeSpecificControls();
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

    // initialize buttons for modes
    myEditModeCreateEdge = new MFXCheckableButton(false, myToolbar, "\tset create edge mode\tMode for creating junction and edges.",
            GUIIconSubSys::getIcon(ICON_MODECREATEEDGE), this, MID_GNE_MODE_CREATE_EDGE, GUIDesignButtonToolbarCheckable);
    myEditModeMove = new MFXCheckableButton(false, myToolbar, "\tset move mode\tMode for move elements.",
                                            GUIIconSubSys::getIcon(ICON_MODEMOVE), this, MID_GNE_MODE_MOVE, GUIDesignButtonToolbarCheckable);
    myEditModeDelete = new MFXCheckableButton(false, myToolbar, "\tset delete mode\tMode for delete elements.",
            GUIIconSubSys::getIcon(ICON_MODEDELETE), this, MID_GNE_MODE_DELETE, GUIDesignButtonToolbarCheckable);
    myEditModeInspect = new MFXCheckableButton(false, myToolbar, "\tset inspect mode\tMode for inspect elements and change their attributes.",
            GUIIconSubSys::getIcon(ICON_MODEINSPECT), this, MID_GNE_MODE_INSPECT, GUIDesignButtonToolbarCheckable);
    myEditModeSelect = new MFXCheckableButton(false, myToolbar, "\tset select mode\tMode for select elements.",
            GUIIconSubSys::getIcon(ICON_MODESELECT), this, MID_GNE_MODE_SELECT, GUIDesignButtonToolbarCheckable);
    myEditModeConnection = new MFXCheckableButton(false, myToolbar, "\tset connection mode\tMode for edit connections between lanes.",
            GUIIconSubSys::getIcon(ICON_MODECONNECTION), this, MID_GNE_MODE_CONNECT, GUIDesignButtonToolbarCheckable);
    myEditModeTrafficLight = new MFXCheckableButton(false, myToolbar, "\tset traffic light mode\tMode for edit traffic lights over junctions.",
            GUIIconSubSys::getIcon(ICON_MODETLS), this, MID_GNE_MODE_TLS, GUIDesignButtonToolbarCheckable);
    myEditModeAdditional = new MFXCheckableButton(false, myToolbar, "\tset additional mode\tMode for adding additional elements.",
            GUIIconSubSys::getIcon(ICON_MODEADDITIONAL), this, MID_GNE_MODE_ADDITIONAL, GUIDesignButtonToolbarCheckable);
    myEditModeCrossing = new MFXCheckableButton(false, myToolbar, "\tset crossing mode\tMode for creating crossings between edges.",
            GUIIconSubSys::getIcon(ICON_MODECROSSING), this, MID_GNE_MODE_CROSSING, GUIDesignButtonToolbarCheckable);

    // @ToDo add here new FXToolBarGrip(myNavigationToolBar, NULL, 0, GUIDesignToolbarGrip);

    // initialize mode specific controls
    myChainCreateEdge = new FXMenuCheck(myToolbar, "chain\t\tCreate consecutive edges with a single click (hit ESC to cancel chain).", this, 0);
    myAutoCreateOppositeEdge = new FXMenuCheck(myToolbar, "two-way\t\tAutomatically create an edge in the opposite direction", this, 0);
    mySelectEdges = new FXMenuCheck(myToolbar, "select edges\t\tToggle whether clicking should select edges or lanes", this, 0);
    mySelectEdges->setCheck();

    myShowConnections = new FXMenuCheck(myToolbar, "show connections\t\tToggle show connections over junctions", this, MID_GNE_SHOW_CONNECTIONS);
    myShowConnections->setCheck(false);

    myExtendToEdgeNodes = new FXMenuCheck(myToolbar, "auto-select junctions\t\tToggle whether selecting multiple edges should automatically select their junctions", this, 0);

    myWarnAboutMerge = new FXMenuCheck(myToolbar, "ask for merge\t\tAsk for confirmation before merging junctions.", this, 0);
    myWarnAboutMerge->setCheck(true);

    myShowBubbleOverJunction = new FXMenuCheck(myToolbar, "Show bubbles over junction \t\tShow bubbles over juntion's shapes.", this, MID_GNE_SHOW_BUBBLES);
    myShowBubbleOverJunction->setCheck(false);

    myChangeAllPhases = new FXMenuCheck(myToolbar, "apply change to all phases\t\tToggle whether clicking should apply state changes to all phases of the current traffic light plan", this, 0);
    myChangeAllPhases->setCheck(false);
}


void
GNEViewNet::updateModeSpecificControls() {
    // hide all controls (checkboxs)
    myChainCreateEdge->hide();
    myAutoCreateOppositeEdge->hide();
    mySelectEdges->hide();
    myShowConnections->hide();
    myExtendToEdgeNodes->hide();
    myChangeAllPhases->hide();
    myWarnAboutMerge->hide();
    myShowBubbleOverJunction->hide();
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
    // hide all Frames
    myViewParent->hideAllFrames();
    // enable selected controls
    switch (myEditMode) {
        case GNE_MODE_CREATE_EDGE:
            // show additionals checkboxs
            myChainCreateEdge->show();
            myAutoCreateOppositeEdge->show();
            // check create edge mode button
            myEditModeCreateEdge->setChecked(true);
            break;
        case GNE_MODE_MOVE:
            // show additionals checkboxs
            myWarnAboutMerge->show();
            myShowBubbleOverJunction->show();
            // check move mode button
            myEditModeMove->setChecked(true);
            break;
        case GNE_MODE_DELETE:
            // Show delete frame
            myViewParent->getDeleteFrame()->show();
            // show additionals checkboxs
            mySelectEdges->show();
            myShowConnections->show();
            // check delete mode button
            myEditModeDelete->setChecked(true);
            break;
        case GNE_MODE_INSPECT:
            // show inspector frame
            myViewParent->getInspectorFrame()->show();
            // show additionals checkboxs
            mySelectEdges->show();
            myShowConnections->show();
            // check inspect mode button
            myEditModeInspect->setChecked(true);
            break;
        case GNE_MODE_SELECT:
            // show selector frame
            myViewParent->getSelectorFrame()->show();
            // show additionals checkboxs
            mySelectEdges->show();
            myShowConnections->show();
            myExtendToEdgeNodes->show();
            // check select mode button
            myEditModeSelect->setChecked(true);
            break;
        case GNE_MODE_CONNECT:
            // show connector frame
            myViewParent->getConnectorFrame()->show();
            // check connection mode button
            myEditModeConnection->setChecked(true);
            break;
        case GNE_MODE_TLS:
            // show TLS Frame
            myViewParent->getTLSEditorFrame()->show();
            // show additionals checkboxs
            myChangeAllPhases->show();
            // check TLS mode button
            myEditModeTrafficLight->setChecked(true);
            break;
        case GNE_MODE_ADDITIONAL:
            // show additional frame
            myViewParent->getAdditionalFrame()->show();
            // check additional mode button
            myEditModeAdditional->setChecked(true);
            break;
        case GNE_MODE_CROSSING:
            // show crossing frame
            myViewParent->getCrossingFrame()->show();
            // check crossing mode button
            myEditModeCrossing->setChecked(true);
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
    // force repaint because different modes draw different things
    myToolbar->recalc();
    onPaint(0, 0, 0);
    update();
}


void
GNEViewNet::deleteSelectedJunctions() {
    myUndoList->p_begin("delete selected " + toString(SUMO_TAG_JUNCTION) + "s");
    std::vector<GNEJunction*> junctions = myNet->retrieveJunctions(true);
    for (std::vector<GNEJunction*>::iterator it = junctions.begin(); it != junctions.end(); it++) {
        myNet->deleteJunction(*it, myUndoList);
    }
    myUndoList->p_end();
}


void
GNEViewNet::deleteSelectedEdges() {
    if (mySelectEdges->getCheck()) {
        myUndoList->p_begin("delete selected " + toString(SUMO_TAG_EDGE) + "s");
        std::vector<GNEEdge*> edges = myNet->retrieveEdges(true);
        for (std::vector<GNEEdge*>::iterator it = edges.begin(); it != edges.end(); it++) {
            myNet->deleteEdge(*it, myUndoList);
        }
    } else {
        myUndoList->p_begin("delete selected " + toString(SUMO_TAG_LANE) + "s");
        std::vector<GNELane*> lanes = myNet->retrieveLanes(true);
        for (std::vector<GNELane*>::iterator it = lanes.begin(); it != lanes.end(); it++) {
            myNet->deleteLane(*it, myUndoList);
        }
    }
    myUndoList->p_end();
}


bool
GNEViewNet::mergeJunctions(GNEJunction* moved) {
    const Position& newPos = moved->getNBNode()->getPosition();
    GNEJunction* mergeTarget = 0;
    // try to find another junction to merge with
    if (makeCurrent()) {
        Boundary selection;
        selection.add(newPos);
        selection.grow(0.1);
        const std::vector<GUIGlID> ids = getObjectsInBoundary(selection);
        GUIGlObject* object = 0;
        for (std::vector<GUIGlID>::const_iterator it = ids.begin(); it != ids.end(); it++) {
            GUIGlID id = *it;
            if (id == 0) {
                continue;
            }
            object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
            if (!object) {
                throw ProcessError("Unkown object in selection (id=" + toString(id) + ").");
            }
            if (object->getType() == GLO_JUNCTION && id != moved->getGlID()) {
                mergeTarget = dynamic_cast<GNEJunction*>(object);
            }
            GUIGlObjectStorage::gIDStorage.unblockObject(id);
        }
    }
    if (mergeTarget) {
        // optionally ask for confirmation
        if (myWarnAboutMerge->getCheck()) {
            FXuint answer = FXMessageBox::question(this, MBOX_YES_NO,
                                                   "Confirm Junction Merger", "%s",
                                                   ("Do you wish to merge junctions '" + moved->getMicrosimID() +
                                                    "' and '" + mergeTarget->getMicrosimID() + "'?\n" +
                                                    "('" + moved->getMicrosimID() +
                                                    "' will be eliminated and its roads added to '" +
                                                    mergeTarget->getMicrosimID() + "')").c_str());
            if (answer != 1) { //1:yes, 2:no, 4:esc
                return false;
            }
        }
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
