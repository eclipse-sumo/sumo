/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GUISUMOAbstractView.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @author  Andreas Gaubatz
/// @date    Sept 2002
///
// The base class for a view
/****************************************************************************/
#include <config.h>

#include <iostream>
#include <utility>
#include <cmath>
#include <cassert>
#include <limits>
#include <fxkeys.h>
#ifdef HAVE_GL2PS
#include <gl2ps.h>
#endif
#include <utils/foxtools/MFXSingleEventThread.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/foxtools/MFXImageHelper.h>
#include <utils/common/RGBColor.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SysUtils.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/globjects/GUICursorDialog.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/shapes/PointOfInterest.h>
#include <utils/gui/globjects/GUIPointOfInterest.h>
#include <utils/gui/globjects/GUIPolygon.h>
#include <utils/gui/windows/GUIDialog_ViewSettings.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>
#include <foreign/fontstash/fontstash.h>
#include <utils/gui/cursors/GUICursorSubSys.h>
#include <utils/options/OptionsCont.h>

#include <unordered_set>

#include "GUISUMOAbstractView.h"
#include "GUIMainWindow.h"
#include "GUIGlChildWindow.h"
#include "GUIDanielPerspectiveChanger.h"
#include "GUIDialog_EditViewport.h"

#ifdef HAVE_GDAL
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4435 5219 5220)
#endif
#if __GNUC__ > 3
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
#include <gdal_priv.h>
#if __GNUC__ > 3
#pragma GCC diagnostic pop
#endif
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif


// ===========================================================================
// debug constants
// ===========================================================================
//#define DEBUG_SNAPSHOT

// ===========================================================================
// static members
// ===========================================================================

const double GUISUMOAbstractView::SENSITIVITY = 0.1; // meters


// ===========================================================================
// member method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUISUMOAbstractView - FOX callback mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUISUMOAbstractView) GUISUMOAbstractViewMap[] = {
    FXMAPFUNC(SEL_CONFIGURE,            0,               GUISUMOAbstractView::onConfigure),
    FXMAPFUNC(SEL_PAINT,                0,               GUISUMOAbstractView::onPaint),
    FXMAPFUNC(SEL_LEFTBUTTONPRESS,      0,               GUISUMOAbstractView::onLeftBtnPress),
    FXMAPFUNC(SEL_LEFTBUTTONRELEASE,    0,               GUISUMOAbstractView::onLeftBtnRelease),
    FXMAPFUNC(SEL_MIDDLEBUTTONPRESS,    0,               GUISUMOAbstractView::onMiddleBtnPress),
    FXMAPFUNC(SEL_MIDDLEBUTTONRELEASE,  0,               GUISUMOAbstractView::onMiddleBtnRelease),
    FXMAPFUNC(SEL_RIGHTBUTTONPRESS,     0,               GUISUMOAbstractView::onRightBtnPress),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,   0,               GUISUMOAbstractView::onRightBtnRelease),
    FXMAPFUNC(SEL_DOUBLECLICKED,        0,               GUISUMOAbstractView::onDoubleClicked),
    FXMAPFUNC(SEL_MOUSEWHEEL,           0,               GUISUMOAbstractView::onMouseWheel),
    FXMAPFUNC(SEL_MOTION,               0,               GUISUMOAbstractView::onMouseMove),
    FXMAPFUNC(SEL_LEAVE,                0,               GUISUMOAbstractView::onMouseLeft),
    FXMAPFUNC(SEL_KEYPRESS,             0,               GUISUMOAbstractView::onKeyPress),
    FXMAPFUNC(SEL_KEYRELEASE,           0,               GUISUMOAbstractView::onKeyRelease),
    FXMAPFUNC(SEL_COMMAND, MID_CLOSE_LANE,               GUISUMOAbstractView::onCmdCloseLane),
    FXMAPFUNC(SEL_COMMAND, MID_CLOSE_EDGE,               GUISUMOAbstractView::onCmdCloseEdge),
    FXMAPFUNC(SEL_COMMAND, MID_ADD_REROUTER,             GUISUMOAbstractView::onCmdAddRerouter),
    FXMAPFUNC(SEL_COMMAND, MID_REACHABILITY,             GUISUMOAbstractView::onCmdShowReachability),
    FXMAPFUNC(SEL_COMMAND, MID_REACHABILITY,             GUISUMOAbstractView::onCmdShowReachability),
    FXMAPFUNC(SEL_CHANGED,  MID_SIMPLE_VIEW_COLORCHANGE, GUISUMOAbstractView::onVisualizationChange),
};


FXIMPLEMENT_ABSTRACT(GUISUMOAbstractView, FXGLCanvas, GUISUMOAbstractViewMap, ARRAYNUMBER(GUISUMOAbstractViewMap))


/* -------------------------------------------------------------------------
 * GUISUMOAbstractView - methods
 * ----------------------------------------------------------------------- */
GUISUMOAbstractView::GUISUMOAbstractView(FXComposite* p, GUIMainWindow& app, GUIGlChildWindow* parent, const SUMORTree& grid, FXGLVisual* glVis, FXGLCanvas* share) :
    FXGLCanvas(p, glVis, share, p, MID_GLCANVAS, LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0),
    myApp(&app),
    myGlChildWindowParent(parent),
    myGrid(&grid),
    myMouseHotspotX(app.getDefaultCursor()->getHotX()),
    myMouseHotspotY(app.getDefaultCursor()->getHotY()),
    myWindowCursorPositionX(getWidth() / 2),
    myWindowCursorPositionY(getHeight() / 2) {
    setTarget(this);
    enable();
    flags |= FLAG_ENABLED;
    myChanger = new GUIDanielPerspectiveChanger(*this, *myGrid);
    myVisualizationSettings = &gSchemeStorage.getDefault();
    myVisualizationSettings->gaming = myApp->isGaming();
    gSchemeStorage.setViewport(this);
    myDecals = gSchemeStorage.getDecals();
}


GUISUMOAbstractView::~GUISUMOAbstractView() {
    gSchemeStorage.setDefault(myVisualizationSettings->name);
    gSchemeStorage.saveViewport(myChanger->getXPos(), myChanger->getYPos(), myChanger->getZPos(), myChanger->getRotation());
    gSchemeStorage.saveDecals(myDecals);
    delete myPopup;
    delete myChanger;
    delete myGUIDialogEditViewport;
    delete myGUIDialogViewSettings;
    // cleanup decals
    for (auto& decal : myDecals) {
        delete decal.image;
    }
    // remove all elements
    for (auto& additional : myAdditionallyDrawn) {
        additional.first->removeActiveAddVisualisation(this, ~0);
    }
}


bool
GUISUMOAbstractView::isInEditMode() {
    return myInEditMode;
}


GUIPerspectiveChanger&
GUISUMOAbstractView::getChanger() const {
    return *myChanger;
}


void
GUISUMOAbstractView::updateToolTip() {
    if (myGlChildWindowParent->getGUIMainWindowParent()->getStaticTooltipView()->isStaticToolTipEnabled()) {
        update();
    }
}


Position
GUISUMOAbstractView::getPositionInformation() const {
    return screenPos2NetPos(myWindowCursorPositionX, myWindowCursorPositionY);
}


Position
GUISUMOAbstractView::snapToActiveGrid(const Position& pos, bool snapXY) const {
    Position result = pos;
    if (myVisualizationSettings->showGrid) {
        if (snapXY) {
            const double xRest = std::fmod(pos.x(), myVisualizationSettings->gridXSize) + (pos.x() < 0 ? myVisualizationSettings->gridXSize : 0);
            const double yRest = std::fmod(pos.y(), myVisualizationSettings->gridYSize) + (pos.y() < 0 ? myVisualizationSettings->gridYSize : 0);
            result.setx(pos.x() - xRest + (xRest < myVisualizationSettings->gridXSize * 0.5 ? 0 : myVisualizationSettings->gridXSize));
            result.sety(pos.y() - yRest + (yRest < myVisualizationSettings->gridYSize * 0.5 ? 0 : myVisualizationSettings->gridYSize));
        } else {
            // snapZToActiveGrid uses grid Y Size
            const double zRest = std::fmod(pos.z(), myVisualizationSettings->gridYSize) + (pos.z() < 0 ? myVisualizationSettings->gridYSize : 0);
            result.setz(pos.z() - zRest + (zRest < myVisualizationSettings->gridYSize * 0.5 ? 0 : myVisualizationSettings->gridYSize));
        }
    }
    return result;
}


Position
GUISUMOAbstractView::screenPos2NetPos(int x, int y) const {
    Boundary bound = myChanger->getViewport();
    double xNet = bound.xmin() + bound.getWidth() * x / getWidth();
    // cursor origin is in the top-left corner
    double yNet = bound.ymin() + bound.getHeight() * (getHeight() - y) / getHeight();
    // rotate around the viewport center
    if (myChanger->getRotation() != 0) {
        return Position(xNet, yNet).rotateAround2D(-DEG2RAD(myChanger->getRotation()), bound.getCenter());
    } else {
        return Position(xNet, yNet);
    }
}


void
GUISUMOAbstractView::addDecals(const std::vector<Decal>& decals) {
    myDecals.insert(myDecals.end(), decals.begin(), decals.end());
}


void
GUISUMOAbstractView::updatePositionInformationLabel() const {
    Position pos = getPositionInformation();
    // set cartesian position
    myApp->getCartesianLabel()->setText(("x:" + toString(pos.x()) + ", y:" + toString(pos.y())).c_str());
    // set geo position
    GeoConvHelper::getFinal().cartesian2geo(pos);
    if (GeoConvHelper::getFinal().usingGeoProjection()) {
        myApp->getGeoLabel()->setText(("lat:" + toString(pos.y(), gPrecisionGeo) + ", lon:" + toString(pos.x(), gPrecisionGeo)).c_str());
    } else {
        myApp->getGeoLabel()->setText(TL("(No projection defined)"));
    }
    // if enabled, set test position
    if (myApp->getTestFrame()) {
        if (OptionsCont::getOptions().getBool("gui-testing")) {
            myApp->getTestFrame()->show();
            // adjust cursor position (24,25) to show exactly the same position as in function netedit.leftClick(match, X, Y)
            myApp->getTestLabel()->setText(("Test: x:" + toString(getWindowCursorPosition().x() - 24.0) + " y:" + toString(getWindowCursorPosition().y() - 25.0)).c_str());
        } else {
            myApp->getTestFrame()->hide();
        }
    }
}


int
GUISUMOAbstractView::doPaintGL(int /*mode*/, const Boundary& /*boundary*/) {
    return 0;
}


void
GUISUMOAbstractView::doInit() {
}


Boundary
GUISUMOAbstractView::getVisibleBoundary() const {
    return myChanger->getViewport();
}


bool
GUISUMOAbstractView::is3DView() const {
    return false;
}


void GUISUMOAbstractView::zoom2Pos(Position& /* camera */, Position& /* lookAt */, double /* zoom */) {
}


void
GUISUMOAbstractView::paintGL() {
    // reset debug counters
    GLHelper::resetMatrixCounter();
    GLHelper::resetVertexCounter();
    if (getWidth() == 0 || getHeight() == 0) {
        return;
    }
    const long start = SysUtils::getCurrentMillis();

    if (getTrackedID() != GUIGlObject::INVALID_ID) {
        centerTo(getTrackedID(), false);
    }
    // draw
    glClearColor(
        myVisualizationSettings->backgroundColor.red() / 255.f,
        myVisualizationSettings->backgroundColor.green() / 255.f,
        myVisualizationSettings->backgroundColor.blue() / 255.f,
        myVisualizationSettings->backgroundColor.alpha() / 255.f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (myVisualizationSettings->dither) {
        glEnable(GL_DITHER);
    } else {
        glDisable(GL_DITHER);
    }
    glEnable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);

    Boundary bound = applyGLTransform();
    doPaintGL(GL_RENDER, bound);
    GLHelper::checkCounterMatrix();
    GLHelper::checkCounterName();
    displayLegends();
    const long end = SysUtils::getCurrentMillis();
    myFrameDrawTime = end - start;
    if (myVisualizationSettings->fps) {
        drawFPS();
    }
    // check if show tooltip
    if (myGlChildWindowParent->getGUIMainWindowParent()->getStaticTooltipView()->isStaticToolTipEnabled()) {
        showToolTipFor(getToolTipID());
    } else {
        myGlChildWindowParent->getGUIMainWindowParent()->getStaticTooltipView()->hideStaticToolTip();
    }
    swapBuffers();
}


long
GUISUMOAbstractView::onCmdCloseLane(FXObject*, FXSelector, void*) {
    return 1;
}


long
GUISUMOAbstractView::onCmdCloseEdge(FXObject*, FXSelector, void*) {
    return 1;
}


long
GUISUMOAbstractView::onCmdAddRerouter(FXObject*, FXSelector, void*) {
    return 1;
}


long
GUISUMOAbstractView::onCmdShowReachability(FXObject*, FXSelector, void*) {
    return 1;
}


long
GUISUMOAbstractView::onVisualizationChange(FXObject*, FXSelector, void*) {
    return 1;
}


GUILane*
GUISUMOAbstractView::getLaneUnderCursor() {
    return nullptr;
}


GUIGlID
GUISUMOAbstractView::getToolTipID() {
    return getObjectUnderCursor();
}


GUIGlID
GUISUMOAbstractView::getObjectUnderCursor() {
    return getObjectAtPosition(getPositionInformation());
}


std::vector<GUIGlID>
GUISUMOAbstractView::getObjectsUnderCursor() {
    return getObjectsAtPosition(getPositionInformation(), SENSITIVITY);
}



std::vector<GUIGlObject*>
GUISUMOAbstractView::getGUIGlObjectsUnderCursor() {
    return getGUIGlObjectsAtPosition(getPositionInformation(), SENSITIVITY);
}


std::vector<GUIGlObject*>
GUISUMOAbstractView::getGUIGlObjectsUnderSnappedCursor() {
    return getGUIGlObjectsAtPosition(snapToActiveGrid(getPositionInformation()), SENSITIVITY);
}


GUIGlID
GUISUMOAbstractView::getObjectAtPosition(Position pos) {
    // calculate a boundary for the given position
    Boundary positionBoundary;
    positionBoundary.add(pos);
    positionBoundary.grow(SENSITIVITY);
    const std::vector<GUIGlID> ids = getObjectsInBoundary(positionBoundary);
    // Interpret results
    int idMax = 0;
    double maxLayer = -std::numeric_limits<double>::max();
    // iterate over obtained GUIGlIDs
    for (const auto& i : ids) {
        // obtain GUIGlObject
        GUIGlObject* o = GUIGlObjectStorage::gIDStorage.getObjectBlocking(i);
        // check that GUIGlObject exist
        if (o == nullptr) {
            continue;
        }
        // check that GUIGlObject isn't the network
        if (o->getGlID() == 0) {
            continue;
        }
        //std::cout << "point selection hit " << o->getMicrosimID() << "\n";
        double layer = o->getClickPriority();
        // check whether the current object is above a previous one
        if (layer > maxLayer) {
            idMax = i;
            maxLayer = layer;
        }
        // unblock object
        GUIGlObjectStorage::gIDStorage.unblockObject(i);
    }
    return idMax;
}


std::vector<GUIGlID>
GUISUMOAbstractView::getObjectsAtPosition(Position pos, double radius) {
    // declare result vector
    std::vector<GUIGlID> result;
    // calculate boundary
    Boundary selection;
    selection.add(pos);
    selection.grow(radius);
    // obtain GUIGlID of objects in boundary
    const std::vector<GUIGlID> ids = getObjectsInBoundary(selection);
    // iterate over obtained GUIGlIDs
    for (const auto& i : ids) {
        // obtain GUIGlObject
        GUIGlObject* o = GUIGlObjectStorage::gIDStorage.getObjectBlocking(i);
        // check that GUIGlObject exist
        if (o == nullptr) {
            continue;
        }
        // check that GUIGlObject isn't the network
        if (o->getGlID() == 0) {
            continue;
        }
        //std::cout << "point selection hit " << o->getMicrosimID() << "\n";
        GUIGlObjectType type = o->getType();
        // avoid network
        if (type != GLO_NETWORK) {
            result.push_back(i);
        }
        // unblock object
        GUIGlObjectStorage::gIDStorage.unblockObject(i);
    }
    return result;
}


std::vector<GUIGlObject*>
GUISUMOAbstractView::getGUIGlObjectsAtPosition(Position pos, double radius) {
    // declare result vector
    std::vector<GUIGlObject*> result;
    // calculate boundary
    Boundary selection;
    selection.add(pos);
    selection.grow(radius);
    // obtain GUIGlID of objects in boundary
    const std::vector<GUIGlID> ids = getObjectsInBoundary(selection);
    // iterate over obtained GUIGlIDs
    for (const auto& i : ids) {
        // obtain GUIGlObject
        GUIGlObject* o = GUIGlObjectStorage::gIDStorage.getObjectBlocking(i);
        // check that GUIGlObject exist
        if (o == nullptr) {
            continue;
        }
        // check that GUIGlObject isn't the network
        if (o->getGlID() == 0) {
            continue;
        }
        result.push_back(o);
        // unblock object
        GUIGlObjectStorage::gIDStorage.unblockObject(i);
    }
    return result;
}


std::vector<GUIGlID>
GUISUMOAbstractView::getObjectsInBoundary(Boundary bound) {
    const int NB_HITS_MAX = 1024 * 1024;
    // Prepare the selection mode
    static GUIGlID hits[NB_HITS_MAX];
    static GLint nb_hits = 0;
    glSelectBuffer(NB_HITS_MAX, hits);
    glInitNames();

    myVisualizationSettings->scale = m2p(SUMO_const_laneWidth);
    Boundary oldViewPort = myChanger->getViewport(false); // backup the actual viewPort
    myChanger->setViewport(bound);
    bound = applyGLTransform(false);
    // enable draw for selecting (to draw objects with less details)
    myVisualizationSettings->drawForRectangleSelection = true;
    int hits2 = doPaintGL(GL_SELECT, bound);
    // reset flags
    myVisualizationSettings->drawForRectangleSelection = false;
    // Get the results
    nb_hits = glRenderMode(GL_RENDER);
    if (nb_hits == -1) {
        myApp->setStatusBarText("Selection in boundary failed. Try to select fewer than " + toString(hits2) + " items");
    }
    std::vector<GUIGlID> result;
    GLuint numNames;
    GLuint* ptr = hits;
    for (int i = 0; i < nb_hits; ++i) {
        numNames = *ptr;
        ptr += 3;
        for (int j = 0; j < (int)numNames; j++) {
            result.push_back(*ptr);
            ptr++;
        }
    }
    // switch viewport back to normal
    myChanger->setViewport(oldViewPort);
    return result;
}


std::vector<GUIGlObject*>
GUISUMOAbstractView::filterInternalLanes(const std::vector<GUIGlObject*>& objects) const {
    // count number of internal lanes
    size_t internalLanes = 0;
    for (const auto& object : objects) {
        if ((object->getType() == GLO_LANE) && (object->getMicrosimID().find(':') != std::string::npos)) {
            internalLanes++;
        }
    }
    // if all objects are internal lanes, return it all
    if (objects.size() == internalLanes || !myVisualizationSettings->drawJunctionShape) {
        return objects;
    }
    // in other case filter internal lanes
    std::vector<GUIGlObject*> filteredObjects;
    for (const auto& object : objects) {
        if ((object->getType() == GLO_LANE) && (object->getMicrosimID().find(':') != std::string::npos)) {
            continue;
        }
        filteredObjects.push_back(object);
    }
    return filteredObjects;
}


bool
GUISUMOAbstractView::showToolTipFor(const GUIGlID idToolTip) {
    if (idToolTip != GUIGlObject::INVALID_ID) {
        const GUIGlObject* object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(idToolTip);
        if (object != nullptr) {
            myGlChildWindowParent->getGUIMainWindowParent()->getStaticTooltipView()->showStaticToolTip(object->getFullName().c_str());
            return true;
        }
    }
    // nothing to show
    myGlChildWindowParent->getGUIMainWindowParent()->getStaticTooltipView()->hideStaticToolTip();
    return false;
}


void
GUISUMOAbstractView::paintGLGrid() const {
    // obtain minimum grid
    const double minimumSizeGrid = (myVisualizationSettings->gridXSize < myVisualizationSettings->gridYSize) ? myVisualizationSettings->gridXSize : myVisualizationSettings->gridYSize;
    // Check if the distance is enough to draw grid
    if (myVisualizationSettings->scale * myVisualizationSettings->addSize.getExaggeration(*myVisualizationSettings, nullptr) >= (25 / minimumSizeGrid)) {
        glEnable(GL_DEPTH_TEST);
        glLineWidth(1);
        // get multiplication values (2 is the margin)
        const int multXmin = (int)(myChanger->getViewport().xmin() / myVisualizationSettings->gridXSize) - 2;
        const int multYmin = (int)(myChanger->getViewport().ymin() / myVisualizationSettings->gridYSize) - 2;
        const int multXmax = (int)(myChanger->getViewport().xmax() / myVisualizationSettings->gridXSize) + 2;
        const int multYmax = (int)(myChanger->getViewport().ymax() / myVisualizationSettings->gridYSize) + 2;
        // obtain references
        const double xmin = myVisualizationSettings->gridXSize * multXmin;
        const double ymin = myVisualizationSettings->gridYSize * multYmin;
        const double xmax = myVisualizationSettings->gridXSize * multXmax;
        const double ymax = myVisualizationSettings->gridYSize * multYmax;
        double xp = xmin;
        double yp = ymin;
        // move drawing matrix
        glTranslated(0, 0, .55);
        glColor3d(0.5, 0.5, 0.5);
        // draw horizontal lines
        glBegin(GL_LINES);
        while (yp <= ymax) {
            glVertex2d(xmin, yp);
            glVertex2d(xmax, yp);
            yp += myVisualizationSettings->gridYSize;
        }
        // draw vertical lines
        while (xp <= xmax) {
            glVertex2d(xp, ymin);
            glVertex2d(xp, ymax);
            xp += myVisualizationSettings->gridXSize;
        }
        glEnd();
        glTranslated(0, 0, -.55);
    }
}


void
GUISUMOAbstractView::displayLegend() {
    // compute the scale bar length
    int length = 1;
    const std::string text("10000000000");
    int noDigits = 1;
    int pixelSize = (int) m2p((double) length);
    while (pixelSize <= 20) {
        length *= 10;
        noDigits++;
        if (noDigits > (int)text.length()) {
            return;
        }
        pixelSize = (int) m2p((double) length);
    }
    glLineWidth(1.0);

    glMatrixMode(GL_PROJECTION);
    GLHelper::pushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    GLHelper::pushMatrix();
    glLoadIdentity();

    // draw the scale bar
    const double z = -1;
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    GLHelper::pushMatrix();
    glTranslated(0, 0, z);

    double len = (double) pixelSize / (double)(getWidth() - 1) * (double) 2.0;
    glColor3d(0, 0, 0);
    double o = double(15) / double(getHeight());
    double o2 = o + o;
    double oo = double(5) / double(getHeight());
    glBegin(GL_LINES);
    // vertical
    glVertex2d(-.98, -1. + o);
    glVertex2d(-.98 + len, -1. + o);
    // tick at begin
    glVertex2d(-.98, -1. + o);
    glVertex2d(-.98, -1. + o2);
    // tick at end
    glVertex2d(-.98 + len, -1. + o);
    glVertex2d(-.98 + len, -1. + o2);
    glEnd();
    GLHelper::popMatrix();

    const double fontHeight = 0.1 * 300. / getHeight();
    const double fontWidth = 0.1 * 300. / getWidth();
    // draw 0
    GLHelper::drawText("0", Position(-.99, -0.99 + o2 + oo), z, fontHeight, RGBColor::BLACK, 0, FONS_ALIGN_LEFT, fontWidth);

    // draw current scale
    GLHelper::drawText((text.substr(0, noDigits) + "m").c_str(), Position(-.99 + len, -0.99 + o2 + oo), z, fontHeight, RGBColor::BLACK, 0, FONS_ALIGN_LEFT, fontWidth);

    // restore matrices
    glMatrixMode(GL_PROJECTION);
    GLHelper::popMatrix();
    glMatrixMode(GL_MODELVIEW);
    GLHelper::popMatrix();
}

void
GUISUMOAbstractView::displayLegends() {
    if (myVisualizationSettings->showSizeLegend) {
        displayLegend();
    }
    if (myVisualizationSettings->showColorLegend) {
        displayColorLegend(myVisualizationSettings->getLaneEdgeScheme(), false);
    }
    if (myVisualizationSettings->showVehicleColorLegend) {
        displayColorLegend(myVisualizationSettings->vehicleColorer.getScheme(), true);
    }
}

void
GUISUMOAbstractView::displayColorLegend(const GUIColorScheme& scheme, bool leftSide) {
    // compute the scale bar length
    glLineWidth(1.0);
    glMatrixMode(GL_PROJECTION);
    GLHelper::pushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    GLHelper::pushMatrix();
    glLoadIdentity();

    const double z = -1;
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    GLHelper::pushMatrix();
    glTranslated(0, 0, z);

    const bool fixed = scheme.isFixed();
    const int numColors = (int)scheme.getColors().size();

    // vertical
    double right = 0.98;
    double left = 0.95;
    double textX = left - 0.01;
    double textDir = 1;
    FONSalign textAlign = FONS_ALIGN_RIGHT;
    const double top = -0.7;
    const double bot = 0.9;
    const double dy = (top - bot) / numColors;
    const double bot2 = fixed ? bot : bot + dy / 2;
    // legend placement
    if (leftSide) {
        right = -right;
        left = -left;
        std::swap(right, left);
        textX = right + 0.01;
        textDir *= -1;
        textAlign = FONS_ALIGN_LEFT;
    }
    // draw black boundary around legend colors
    glColor3d(0, 0, 0);
    glBegin(GL_LINES);
    glVertex2d(right, top);
    glVertex2d(right, bot2);
    glVertex2d(left, bot2);
    glVertex2d(left, top);
    glVertex2d(right, top);
    glVertex2d(left, top);
    glVertex2d(right, bot2);
    glVertex2d(left, bot2);
    glEnd();

    const double fontHeight = 0.20 * 300. / getHeight();
    const double fontWidth = 0.20 * 300. / getWidth();

    const int fadeSteps = fixed ? 1 : 10;
    double colorStep = dy / fadeSteps;
    for (int i = 0; i < numColors; i++) {
        RGBColor col = scheme.getColors()[i];
        const double topi = top - i * dy;
        //const double boti = top - (i + 1) * dy;
        //std::cout << " col=" << scheme.getColors()[i] << " i=" << i << " topi=" << topi << " boti=" << boti << "\n";
        if (i + 1 < numColors) {
            // fade
            RGBColor col2 = scheme.getColors()[i + 1];
            double thresh2 = scheme.getThresholds()[i + 1];
            if (!fixed && thresh2 == GUIVisualizationSettings::MISSING_DATA) {
                // draw scale end before missing data
                GLHelper::setColor(col);
                glBegin(GL_QUADS);
                glVertex2d(left, topi);
                glVertex2d(right, topi);
                glVertex2d(right, topi - 5 * colorStep);
                glVertex2d(left, topi - 5 * colorStep);
                glEnd();
                glColor3d(0, 0, 0);
                glBegin(GL_LINES);
                glVertex2d(right, topi - 10 * colorStep);
                glVertex2d(left, topi - 10 * colorStep);
                glEnd();
                glBegin(GL_LINES);
                glVertex2d(right, topi - 5 * colorStep);
                glVertex2d(left, topi - 5 * colorStep);
                glEnd();
            } else {
                // fade colors
                for (double j = 0.0; j < fadeSteps; j++) {
                    GLHelper::setColor(RGBColor::interpolate(col, col2, j / fadeSteps));
                    glBegin(GL_QUADS);
                    glVertex2d(left, topi - j * colorStep);
                    glVertex2d(right, topi - j * colorStep);
                    glVertex2d(right, topi - (j + 1) * colorStep);
                    glVertex2d(left, topi - (j + 1) * colorStep);
                    glEnd();
                }
            }
        } else {
            GLHelper::setColor(col);
            glBegin(GL_QUADS);
            glVertex2d(left, topi);
            glVertex2d(right, topi);
            glVertex2d(right, bot2);
            glVertex2d(left, bot2);
            glEnd();
        }

        const double threshold = scheme.getThresholds()[i];
        std::string name = scheme.getNames()[i];
        std::string text = fixed || threshold == GUIVisualizationSettings::MISSING_DATA ? name : toString(threshold);

        const double bgShift = 0.0;
        const double textShift = 0.01;
        const double textXShift = -0.005;

        GLHelper::setColor(RGBColor::WHITE);
        glTranslated(0, 0, 0.1);
        glBegin(GL_QUADS);
        glVertex2d(textX, topi + fontHeight * bgShift);
        glVertex2d(textX - textDir * fontWidth * (double)text.size() / 2.1, topi + fontHeight * bgShift);
        glVertex2d(textX - textDir * fontWidth * (double)text.size() / 2.1, topi + fontHeight * (0.8 + bgShift));
        glVertex2d(textX, topi + fontHeight * (0.8 + bgShift));
        glEnd();
        glTranslated(0, 0, -0.1);
        GLHelper::drawText(text, Position(textX + textDir * textXShift, topi + textShift), 0, fontHeight, RGBColor::BLACK, 0, textAlign, fontWidth);
    }
    // draw scheme name
    std::string name = scheme.getName();
    if (StringUtils::startsWith(name, "by ")) {
        name = name.substr(3);
    }
    const double topN = -0.8;
    const double bgShift = 0.0;
    GLHelper::setColor(RGBColor::WHITE);
    glTranslated(0, 0, 0.1);
    glBegin(GL_QUADS);
    glVertex2d(textX + textDir * 0.04,                                                   topN + fontHeight * bgShift - 0.01);
    glVertex2d(textX + textDir * 0.04 - textDir * fontWidth * (double)name.size() / 2.3, topN + fontHeight * bgShift - 0.01);
    glVertex2d(textX + textDir * 0.04 - textDir * fontWidth * (double)name.size() / 2.3, topN + fontHeight * (0.8 + bgShift));
    glVertex2d(textX + textDir * 0.04,                                                   topN + fontHeight * (0.8 + bgShift));
    glEnd();
    glTranslated(0, 0, -0.1);
    GLHelper::drawText(name, Position(textX + textDir * 0.04, topN), 0, fontHeight, RGBColor::BLACK, 0, textAlign, fontWidth);

    GLHelper::popMatrix();
    // restore matrices
    glMatrixMode(GL_PROJECTION);
    GLHelper::popMatrix();
    glMatrixMode(GL_MODELVIEW);
    GLHelper::popMatrix();
}


double
GUISUMOAbstractView::getFPS() const {
    return 1000.0 / MAX2((long)1, myFrameDrawTime);
}


GUIGlChildWindow*
GUISUMOAbstractView::getGUIGlChildWindow() {
    return myGlChildWindowParent;
}


void
GUISUMOAbstractView::drawFPS() {
    glMatrixMode(GL_PROJECTION);
    GLHelper::pushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    GLHelper::pushMatrix();
    glLoadIdentity();
    const double fontHeight = 0.2 * 300. / getHeight();
    const double fontWidth = 0.2 * 300. / getWidth();
    GLHelper::drawText(toString((int)getFPS()) + " FPS", Position(0.82, 0.88), -1, fontHeight, RGBColor::RED, 0, FONS_ALIGN_LEFT, fontWidth);
#ifdef CHECK_ELEMENTCOUNTER
    GLHelper::drawText(toString(GLHelper::getMatrixCounter()) + " matrix", Position(0.82, 0.79), -1, fontHeight, RGBColor::RED, 0, FONS_ALIGN_LEFT, fontWidth);
    GLHelper::drawText(toString(GLHelper::getVertexCounter()) + " vertex", Position(0.82, 0.71), -1, fontHeight, RGBColor::RED, 0, FONS_ALIGN_LEFT, fontWidth);
#endif
    // restore matrices
    glMatrixMode(GL_PROJECTION);
    GLHelper::popMatrix();
    glMatrixMode(GL_MODELVIEW);
    GLHelper::popMatrix();
}


double
GUISUMOAbstractView::m2p(double meter) const {
    return  meter * getWidth() / myChanger->getViewport().getWidth();
}


double
GUISUMOAbstractView::p2m(double pixel) const {
    return pixel * myChanger->getViewport().getWidth() / getWidth();
}


void
GUISUMOAbstractView::recenterView() {
    myChanger->setViewport(*myGrid);
}


void
GUISUMOAbstractView::centerTo(GUIGlID id, bool applyZoom, double zoomDist) {
    GUIGlObject* o = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
    if (o != nullptr && dynamic_cast<GUIGlObject*>(o) != nullptr) {
        const Boundary& b = o->getCenteringBoundary();
        if (b.getCenter() != Position::INVALID) {
            if (applyZoom && zoomDist < 0) {
                myChanger->setViewport(b);
                update(); // only update when centering onto an object once
            } else {
                // called during tracking. update is triggered somewhere else
                myChanger->centerTo(b.getCenter(), zoomDist, applyZoom);
                updatePositionInformationLabel();
            }
        }
    }
    GUIGlObjectStorage::gIDStorage.unblockObject(id);
}


void
GUISUMOAbstractView::centerTo(const Position& pos, bool applyZoom, double zoomDist) {
    // called during tracking. update is triggered somewhere else
    myChanger->centerTo(pos, zoomDist, applyZoom);
    updatePositionInformationLabel();
}


void
GUISUMOAbstractView::centerTo(const Boundary& bound) {
    myChanger->setViewport(bound);
    update();
}


GUIMainWindow*
GUISUMOAbstractView::getMainWindow() const {
    return myApp;
}


Position
GUISUMOAbstractView::getWindowCursorPosition() const {
    return Position(myWindowCursorPositionX, myWindowCursorPositionY);
}


void
GUISUMOAbstractView::setWindowCursorPosition(FXint x, FXint y) {
    myWindowCursorPositionX = x + myMouseHotspotX;
    myWindowCursorPositionY = y + myMouseHotspotY;
}


FXbool
GUISUMOAbstractView::makeCurrent() {
    FXbool ret = FXGLCanvas::makeCurrent();
    return ret;
}


long
GUISUMOAbstractView::onConfigure(FXObject*, FXSelector, void*) {
    if (makeCurrent()) {
        glViewport(0, 0, getWidth() - 1, getHeight() - 1);
        glClearColor(
            myVisualizationSettings->backgroundColor.red() / 255.f,
            myVisualizationSettings->backgroundColor.green() / 255.f,
            myVisualizationSettings->backgroundColor.blue() / 255.f,
            myVisualizationSettings->backgroundColor.alpha() / 255.f);
        doInit();
        myAmInitialised = true;
        makeNonCurrent();
        checkSnapshots();
    }
    return 1;
}


long
GUISUMOAbstractView::onPaint(FXObject*, FXSelector, void*) {
    if (!isEnabled() || !myAmInitialised) {
        return 1;
    }
    if (makeCurrent()) {
        paintGL();
        makeNonCurrent();
    }
    return 1;
}


const Position&
GUISUMOAbstractView::getPopupPosition() const {
    return myPopupPosition;
}


void
GUISUMOAbstractView::destroyPopup() {
    if (myPopup != nullptr) {
        myPopup->removePopupFromObject();
        delete myPopup;
        myPopupPosition.set(0, 0);
        myPopup = nullptr;
        myCurrentObjectsDialog.clear();
    }
}


void
GUISUMOAbstractView::replacePopup(GUIGLObjectPopupMenu* popUp) {
    // use the same position of old popUp
    popUp->move(myPopup->getX(), myPopup->getY());
    // delete and replace popup
    myPopup->removePopupFromObject();
    delete myPopup;
    myPopup = popUp;
    // create and show popUp
    myPopup->create();
    myPopup->show();
    myChanger->onRightBtnRelease(nullptr);
    setFocus();
}


long
GUISUMOAbstractView::onLeftBtnPress(FXObject*, FXSelector, void* ptr) {
    destroyPopup();
    setFocus();
    FXEvent* e = (FXEvent*) ptr;
    // check whether the selection-mode is activated
    if ((e->state & CONTROLMASK) != 0) {
        // toggle selection of object under cursor
        if (makeCurrent()) {
            int id = getObjectUnderCursor();
            if (id != 0) {
                gSelected.toggleSelection(id);
            }
            makeNonCurrent();
            if (id != 0) {
                // possibly, the selection-coloring is used,
                //  so we should update the screen again...
                update();
            }
        }
    }
    if ((e->state & SHIFTMASK) != 0) {
        // track vehicle or person under cursor
        if (makeCurrent()) {
            int id = getObjectUnderCursor();
            if (id != 0) {
                GUIGlObject* o = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
                if (o != nullptr) {
                    if (o->getType() == GLO_VEHICLE || o->getType() == GLO_PERSON) {
                        startTrack(id);
                    } else if (o->getType() == GLO_REROUTER_EDGE) {
                        o->onLeftBtnPress(ptr);
                        update();
                    }
                }
            }
            makeNonCurrent();
        }
    }
    myChanger->onLeftBtnPress(ptr);
    grab();
    // Check there are double click
    if (e->click_count == 2) {
        handle(this, FXSEL(SEL_DOUBLECLICKED, 0), ptr);
    }
    return 1;
}


long
GUISUMOAbstractView::onLeftBtnRelease(FXObject*, FXSelector, void* ptr) {
    destroyPopup();
    myChanger->onLeftBtnRelease(ptr);
    if (myApp->isGaming()) {
        onGamingClick(getPositionInformation());
    }
    ungrab();
    return 1;
}


long
GUISUMOAbstractView::onMiddleBtnPress(FXObject*, FXSelector, void* ptr) {
    destroyPopup();
    setFocus();
    if (!myApp->isGaming()) {
        myChanger->onMiddleBtnPress(ptr);
    }
    grab();
    // enable panning
    myPanning = true;
    // set cursors
    setDefaultCursor(GUICursorSubSys::getCursor(GUICursor::MOVEVIEW));
    setDragCursor(GUICursorSubSys::getCursor(GUICursor::MOVEVIEW));
    return 1;
}


long
GUISUMOAbstractView::onMiddleBtnRelease(FXObject*, FXSelector, void* ptr) {
    destroyPopup();
    if (!myApp->isGaming()) {
        myChanger->onMiddleBtnRelease(ptr);
    }
    ungrab();
    // disable panning
    myPanning = false;
    // restore cursors
    setDefaultCursor(GUICursorSubSys::getCursor(GUICursor::DEFAULT));
    setDragCursor(GUICursorSubSys::getCursor(GUICursor::DEFAULT));
    return 1;
}


long
GUISUMOAbstractView::onRightBtnPress(FXObject*, FXSelector, void* ptr) {
    destroyPopup();
    if (!myApp->isGaming()) {
        myChanger->onRightBtnPress(ptr);
    }
    grab();
    return 1;
}


long
GUISUMOAbstractView::onRightBtnRelease(FXObject* o, FXSelector sel, void* ptr) {
    destroyPopup();
    onMouseMove(o, sel, ptr);
    if (!myChanger->onRightBtnRelease(ptr) && !myApp->isGaming()) {
        openObjectDialogAtCursor((FXEvent*)ptr);
    }
    if (myApp->isGaming()) {
        onGamingRightClick(getPositionInformation());
    }
    ungrab();
    return 1;
}


long
GUISUMOAbstractView::onDoubleClicked(FXObject*, FXSelector, void*) {
    return 1;
}


long
GUISUMOAbstractView::onMouseWheel(FXObject*, FXSelector, void* ptr) {
    if (!myApp->isGaming()) {
        myChanger->onMouseWheel(ptr);
        // upddate viewport
        if (myGUIDialogEditViewport != nullptr) {
            myGUIDialogEditViewport->setValues(myChanger->getZoom(),
                                               myChanger->getXPos(), myChanger->getYPos(),
                                               myChanger->getRotation());
        }
        updatePositionInformationLabel();
    }
    return 1;
}


long
GUISUMOAbstractView::onMouseMove(FXObject*, FXSelector, void* ptr) {
    // check if popup exist
    if (myPopup) {
        // check if handle front element
        if (myPopupPosition == getPositionInformation()) {
            myPopupPosition = Position::INVALID;
            myPopup->handle(this, FXSEL(SEL_COMMAND, MID_CURSORDIALOG_FRONT), nullptr);
            destroyPopup();
        } else if (!myPopup->shown()) {
            destroyPopup();
        }
    }
    if (myPopup == nullptr) {
        if (myGUIDialogEditViewport == nullptr || !myGUIDialogEditViewport->haveGrabbed()) {
            myChanger->onMouseMove(ptr);
        }
        if (myGUIDialogEditViewport != nullptr) {
            myGUIDialogEditViewport->setValues(myChanger->getZoom(),
                                               myChanger->getXPos(), myChanger->getYPos(),
                                               myChanger->getRotation());
        }
        updatePositionInformationLabel();
    }
    return 1;
}


long
GUISUMOAbstractView::onMouseLeft(FXObject*, FXSelector, void* /*data*/) {
    return 1;
}

std::vector<GUIGlObject*>
GUISUMOAbstractView::filterContextObjects(const std::vector<GUIGlObject*>& objects) {
    // assume input is sorted with ComparatorClickPriority
    std::vector<GUIGlObject*> result;
    for (GUIGlObject* o : objects) {
        if (o->getClickPriority() != GUIGlObject::INVALID_PRIORITY && (result.empty() || result.back() != o)) {
            result.push_back(o);
        }
    }
    return result;
}


void
GUISUMOAbstractView::openObjectDialogAtCursor(const FXEvent* ev) {
    // release the mouse grab
    ungrab();
    // check if alt key is pressed
    const bool altKeyPressed = ((ev->state & ALTMASK) != 0);
    // check if SUMO is enabled, initialised and Make OpenGL context current
    if (isEnabled() && myAmInitialised && makeCurrent()) {
        auto objectsUnderCursor = getGUIGlObjectsUnderCursor();
        if (objectsUnderCursor.empty()) {
            myPopup = GUIGlObjectStorage::gIDStorage.getNetObject()->getPopUpMenu(*myApp, *this);
        } else {
            std::sort(objectsUnderCursor.begin(), objectsUnderCursor.end(), ComparatorClickPriority());
            std::vector<GUIGlObject*> filtered = filterContextObjects(objectsUnderCursor);
            if (filtered.size() > 1 && (altKeyPressed
                                        || filtered[0]->getClickPriority() == filtered[1]->getClickPriority())) {
                // open dialog for picking among objects (without duplicates)
                myPopup = new GUICursorDialog(GUIGLObjectPopupMenu::PopupType::PROPERTIES, this, filtered);
            } else {
                myPopup = objectsUnderCursor.front()->getPopUpMenu(*myApp, *this);
            }
        }
        openPopupDialog();
        makeNonCurrent();
    }
}


void
GUISUMOAbstractView::openObjectDialog(const std::vector<GUIGlObject*>& objects, const bool filter) {
    if (objects.size() > 0) {
        // create cursor popup dialog
        if (objects.size() == 1) {
            myCurrentObjectsDialog = objects;
        } else if (filter) {
            // declare filtered objects
            std::vector<GUIGlObject*> filteredGLObjects;
            // fill filtered objects
            for (const auto& glObject : objects) {
                // compare type with first element type
                if (glObject->getType() == objects.front()->getType()) {
                    filteredGLObjects.push_back(glObject);
                }
            }
            myCurrentObjectsDialog = filteredGLObjects;
        } else {
            myCurrentObjectsDialog = objects;
        }
        if (myCurrentObjectsDialog.size() > 1) {
            myPopup = new GUICursorDialog(GUIGLObjectPopupMenu::PopupType::PROPERTIES, this, myCurrentObjectsDialog);
        } else {
            myPopup = myCurrentObjectsDialog.front()->getPopUpMenu(*myApp, *this);
        }
        // open popup dialog
        openPopupDialog();
    }
}


long
GUISUMOAbstractView::onKeyPress(FXObject* o, FXSelector sel, void* ptr) {
    const FXEvent* e = (FXEvent*) ptr;
    if (e->state & ALTMASK) {
        myVisualizationSettings->altKeyPressed = true;
        // update view (for polygon layers)
        update();
    } else {
        myVisualizationSettings->altKeyPressed = false;
    }
    // check if process canvas or popup
    if (myPopup != nullptr) {
        return myPopup->onKeyPress(o, sel, ptr);
    } else {
        if (e->state & CONTROLMASK) {
            if (e->code == FX::KEY_Page_Up) {
                myVisualizationSettings->gridXSize *= 2;
                myVisualizationSettings->gridYSize *= 2;
                update();
                return 1;
            } else if (e->code == FX::KEY_Page_Down) {
                myVisualizationSettings->gridXSize /= 2;
                myVisualizationSettings->gridYSize /= 2;
                update();
                return 1;
            }
        }
        FXGLCanvas::onKeyPress(o, sel, ptr);
        return myChanger->onKeyPress(ptr);
    }
}


long
GUISUMOAbstractView::onKeyRelease(FXObject* o, FXSelector sel, void* ptr) {
    const FXEvent* e = (FXEvent*) ptr;
    if ((e->state & ALTMASK) == 0) {
        myVisualizationSettings->altKeyPressed = false;
        // update view (for polygon layers)
        update();
    }
    // check if process canvas or popup
    if (myPopup != nullptr) {
        return myPopup->onKeyRelease(o, sel, ptr);
    } else {
        FXGLCanvas::onKeyRelease(o, sel, ptr);
        return myChanger->onKeyRelease(ptr);
    }
}

// ------------ Dealing with snapshots

void
GUISUMOAbstractView::addSnapshot(SUMOTime time, const std::string& file, const int w, const int h) {
#ifdef DEBUG_SNAPSHOT
    std::cout << "add snapshot time=" << time << " file=" << file << "\n";
#endif
    FXMutexLock lock(mySnapshotsMutex);
    mySnapshots[time].push_back(std::make_tuple(file, w, h));
}


std::string
GUISUMOAbstractView::makeSnapshot(const std::string& destFile, const int w, const int h) {
    if (w >= 0) {
        resize(w, h);
        repaint();
    }
    std::string errorMessage;
    FXString ext = FXPath::extension(destFile.c_str());
    const bool useGL2PS = ext == "ps" || ext == "eps" || ext == "pdf" || ext == "svg" || ext == "tex" || ext == "pgf";
#ifdef HAVE_FFMPEG
    const bool useVideo = destFile == "" || ext == "h264" || ext == "hevc" || ext == "mp4";
#endif
    for (int i = 0; i < 10 && !makeCurrent(); ++i) {
        MFXSingleEventThread::sleep(100);
    }
    // draw
    glClearColor(
        myVisualizationSettings->backgroundColor.red() / 255.f,
        myVisualizationSettings->backgroundColor.green() / 255.f,
        myVisualizationSettings->backgroundColor.blue() / 255.f,
        myVisualizationSettings->backgroundColor.alpha() / 255.f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (myVisualizationSettings->dither) {
        glEnable(GL_DITHER);
    } else {
        glDisable(GL_DITHER);
    }
    glEnable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);

    applyGLTransform();

    if (useGL2PS) {
#ifdef HAVE_GL2PS
        GLint format = GL2PS_PS;
        if (ext == "ps") {
            format = GL2PS_PS;
        } else if (ext == "eps") {
            format = GL2PS_EPS;
        } else if (ext == "pdf") {
            format = GL2PS_PDF;
        } else if (ext == "tex") {
            format = GL2PS_TEX;
        } else if (ext == "svg") {
            format = GL2PS_SVG;
        } else if (ext == "pgf") {
            format = GL2PS_PGF;
        } else {
            return "Could not save '" + destFile + "'.\n Unrecognized format '" + std::string(ext.text()) + "'.";
        }
        FILE* fp = fopen(destFile.c_str(), "wb");
        if (fp == 0) {
            return "Could not save '" + destFile + "'.\n Could not open file for writing";
        }
        GLHelper::setGL2PS();
        GLint buffsize = 0, state = GL2PS_OVERFLOW;
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        while (state == GL2PS_OVERFLOW) {
            buffsize += 1024 * 1024;
            gl2psBeginPage(destFile.c_str(), "sumo-gui; https://sumo.dlr.de", viewport, format, GL2PS_SIMPLE_SORT,
                           GL2PS_DRAW_BACKGROUND | GL2PS_USE_CURRENT_VIEWPORT,
                           GL_RGBA, 0, NULL, 0, 0, 0, buffsize, fp, "out.eps");
            glMatrixMode(GL_MODELVIEW);
            GLHelper::pushMatrix();
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_ALPHA_TEST);
            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
            // draw decals (if not in grabbing mode)

            drawDecals();
            if (myVisualizationSettings->showGrid) {
                paintGLGrid();
            }

            glLineWidth(1);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            Boundary viewPort = myChanger->getViewport();
            const float minB[2] = { (float)viewPort.xmin(), (float)viewPort.ymin() };
            const float maxB[2] = { (float)viewPort.xmax(), (float)viewPort.ymax() };
            myVisualizationSettings->scale = m2p(SUMO_const_laneWidth);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glEnable(GL_POLYGON_OFFSET_LINE);
            myGrid->Search(minB, maxB, *myVisualizationSettings);

            displayLegends();
            state = gl2psEndPage();
            glFinish();
        }
        GLHelper::setGL2PS(false);
        fclose(fp);
#else
        return "Could not save '" + destFile + "', gl2ps was not enabled at compile time.";
#endif
    } else {
        doPaintGL(GL_RENDER, myChanger->getViewport());
        displayLegends();
        swapBuffers();
        glFinish();
        FXColor* buf;
        FXMALLOC(&buf, FXColor, getWidth()*getHeight());
        // read from the back buffer
        glReadBuffer(GL_BACK);
        // Read the pixels
        glReadPixels(0, 0, getWidth(), getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)buf);
        makeNonCurrent();
        update();
        // mirror
        int mwidth = getWidth();
        int mheight = getHeight();
        FXColor* paa = buf;
        FXColor* pbb = buf + mwidth * (mheight - 1);
        do {
            FXColor* pa = paa;
            paa += mwidth;
            FXColor* pb = pbb;
            pbb -= mwidth;
            do {
                FXColor t = *pa;
                *pa++ = *pb;
                *pb++ = t;
            } while (pa < paa);
        } while (paa < pbb);
        try {
#ifdef HAVE_FFMPEG
            if (useVideo) {
                try {
                    saveFrame(destFile, buf);
                    errorMessage = "video";
                } catch (std::runtime_error& err) {
                    errorMessage = err.what();
                }
            } else
#endif
                if (!MFXImageHelper::saveImage(destFile, getWidth(), getHeight(), buf)) {
                    errorMessage = "Could not save '" + destFile + "'.";
                }
        } catch (InvalidArgument& e) {
            errorMessage = "Could not save '" + destFile + "'.\n" + e.what();
        }
        FXFREE(&buf);
    }
    return errorMessage;
}


void
GUISUMOAbstractView::saveFrame(const std::string& destFile, FXColor* buf) {
    UNUSED_PARAMETER(destFile);
    UNUSED_PARAMETER(buf);
}


void
GUISUMOAbstractView::checkSnapshots() {
    const SUMOTime time = getCurrentTimeStep() - DELTA_T;
#ifdef DEBUG_SNAPSHOT
    std::cout << "check snapshots time=" << time << " registeredTimes=" << mySnapshots.size() << "\n";
#endif
    FXMutexLock lock(mySnapshotsMutex);
    const auto snapIt = mySnapshots.find(time);
    if (snapIt == mySnapshots.end()) {
        return;
    }
    std::vector<std::tuple<std::string, int, int> > files = snapIt->second;
    lock.unlock();
    // decouple map access and painting to avoid deadlock
    for (const auto& entry : files) {
#ifdef DEBUG_SNAPSHOT
        std::cout << "make snapshot time=" << time << " file=" << file << "\n";
#endif
        const std::string& error = makeSnapshot(std::get<0>(entry), std::get<1>(entry), std::get<2>(entry));
        if (error != "" && error != "video") {
            WRITE_WARNING(error);
        }
    }
    // synchronization with a waiting run thread
    lock.lock();
    mySnapshots.erase(time);
    mySnapshotCondition.signal();
#ifdef DEBUG_SNAPSHOT
    std::cout << "  files=" << toString(files) << " myApplicationSnapshots=" << joinToString(*myApplicationSnapshots, ",") << "\n";
#endif
}


void
GUISUMOAbstractView::waitForSnapshots(const SUMOTime snapshotTime) {
    FXMutexLock lock(mySnapshotsMutex);
    if (mySnapshots.count(snapshotTime) > 0) {
        mySnapshotCondition.wait(mySnapshotsMutex);
    }
}


SUMOTime
GUISUMOAbstractView::getCurrentTimeStep() const {
    return 0;
}


void
GUISUMOAbstractView::showViewschemeEditor() {
    if (myGUIDialogViewSettings == nullptr) {
        myGUIDialogViewSettings = new GUIDialog_ViewSettings(this, myVisualizationSettings);
        myGUIDialogViewSettings->create();
    } else {
        myGUIDialogViewSettings->setCurrent(myVisualizationSettings);
    }
    setFocus();
    myGUIDialogViewSettings->show();
}


GUIDialog_EditViewport*
GUISUMOAbstractView::getViewportEditor() {
    if (myGUIDialogEditViewport == nullptr) {
        myGUIDialogEditViewport = new GUIDialog_EditViewport(this, TLC("Labels", "Edit Viewport"));
        myGUIDialogEditViewport->create();
    }
    updateViewportValues();
    return myGUIDialogEditViewport;
}


void GUISUMOAbstractView::updateViewportValues() {
    myGUIDialogEditViewport->setValues(myChanger->getZoom(),
                                       myChanger->getXPos(), myChanger->getYPos(),
                                       myChanger->getRotation());
}


void
GUISUMOAbstractView::showViewportEditor() {
    getViewportEditor(); // make sure it exists;
    Position p(myChanger->getXPos(), myChanger->getYPos(), myChanger->getZPos());
    myGUIDialogEditViewport->setOldValues(p, Position::INVALID, myChanger->getRotation());
    myGUIDialogEditViewport->show();
}


void
GUISUMOAbstractView::setViewportFromToRot(const Position& lookFrom, const Position& /* lookAt */, double rotation) {
    myChanger->setViewportFrom(lookFrom.x(), lookFrom.y(), lookFrom.z());
    myChanger->setRotation(rotation);
    update();
}


void
GUISUMOAbstractView::copyViewportTo(GUISUMOAbstractView* view) {
    // look straight down
    view->setViewportFromToRot(Position(myChanger->getXPos(), myChanger->getYPos(), myChanger->getZPos()),
                               Position(myChanger->getXPos(), myChanger->getYPos(), 0),
                               myChanger->getRotation());
}


bool
GUISUMOAbstractView::setColorScheme(const std::string&) {
    return true;
}


const GUIVisualizationSettings&
GUISUMOAbstractView::getVisualisationSettings() const {
    return *myVisualizationSettings;
}


GUIVisualizationSettings*
GUISUMOAbstractView::editVisualisationSettings() const {
    return myVisualizationSettings;
}


void
GUISUMOAbstractView::remove(GUIDialog_EditViewport*) {
    myGUIDialogEditViewport = nullptr;
}


void
GUISUMOAbstractView::remove(GUIDialog_ViewSettings*) {
    myGUIDialogViewSettings = nullptr;
}


double
GUISUMOAbstractView::getGridWidth() const {
    return myGrid->getWidth();
}


double
GUISUMOAbstractView::getGridHeight() const {
    return myGrid->getHeight();
}


void
GUISUMOAbstractView::startTrack(int /*id*/) {
}


void
GUISUMOAbstractView::stopTrack() {
}


GUIGlID
GUISUMOAbstractView::getTrackedID() const {
    return GUIGlObject::INVALID_ID;
}


void
GUISUMOAbstractView::onGamingClick(Position /*pos*/) {
}

void
GUISUMOAbstractView::onGamingRightClick(Position /*pos*/) {
}


std::vector<GUISUMOAbstractView::Decal>&
GUISUMOAbstractView::getDecals() {
    return myDecals;
}


FXMutex&
GUISUMOAbstractView::getDecalsLockMutex() {
    return myDecalsLockMutex;
}


MFXComboBoxIcon*
GUISUMOAbstractView::getColoringSchemesCombo() {
    return myGlChildWindowParent->getColoringSchemesCombo();
}


FXImage*
GUISUMOAbstractView::checkGDALImage(Decal& d) {
#ifdef HAVE_GDAL
    GDALAllRegister();
    GDALDataset* poDataset = (GDALDataset*)GDALOpen(d.filename.c_str(), GA_ReadOnly);
    if (poDataset == 0) {
        return 0;
    }
    const int xSize = poDataset->GetRasterXSize();
    const int ySize = poDataset->GetRasterYSize();
    // checking for geodata in the picture and try to adapt position and scale
    if (d.width <= 0.) {
        double adfGeoTransform[6];
        if (poDataset->GetGeoTransform(adfGeoTransform) == CE_None) {
            Position topLeft(adfGeoTransform[0], adfGeoTransform[3]);
            const double horizontalSize = xSize * adfGeoTransform[1];
            const double verticalSize = ySize * adfGeoTransform[5];
            Position bottomRight(topLeft.x() + horizontalSize, topLeft.y() + verticalSize);
            if (GeoConvHelper::getProcessing().x2cartesian(topLeft) && GeoConvHelper::getProcessing().x2cartesian(bottomRight)) {
                d.width = bottomRight.x() - topLeft.x();
                d.height = topLeft.y() - bottomRight.y();
                d.centerX = (topLeft.x() + bottomRight.x()) / 2;
                d.centerY = (topLeft.y() + bottomRight.y()) / 2;
                //WRITE_MESSAGE("proj: " + toString(poDataset->GetProjectionRef()) + " dim: " + toString(d.width) + "," + toString(d.height) + " center: " + toString(d.centerX) + "," + toString(d.centerY));
            } else {
                WRITE_WARNINGF(TL("Could not convert coordinates in %."), d.filename);
            }
        }
    }
#endif
    if (d.width <= 0.) {
        d.width = getGridWidth();
        d.height = getGridHeight();
    }

    // trying to read the picture
#ifdef HAVE_GDAL
    const int picSize = xSize * ySize;
    FXColor* result;
    if (!FXMALLOC(&result, FXColor, picSize)) {
        WRITE_WARNINGF("Could not allocate memory for %.", d.filename);
        return 0;
    }
    for (int j = 0; j < picSize; j++) {
        result[j] = FXRGB(0, 0, 0);
    }
    bool valid = true;
    for (int i = 1; i <= poDataset->GetRasterCount(); i++) {
        GDALRasterBand* poBand = poDataset->GetRasterBand(i);
        int shift = -1;
        if (poBand->GetColorInterpretation() == GCI_RedBand) {
            shift = 0;
        } else if (poBand->GetColorInterpretation() == GCI_GreenBand) {
            shift = 1;
        } else if (poBand->GetColorInterpretation() == GCI_BlueBand) {
            shift = 2;
        } else if (poBand->GetColorInterpretation() == GCI_AlphaBand) {
            shift = 3;
        } else {
            valid = false;
            break;
        }
        assert(xSize == poBand->GetXSize() && ySize == poBand->GetYSize());
        if (poBand->RasterIO(GF_Read, 0, 0, xSize, ySize, ((unsigned char*)result) + shift, xSize, ySize, GDT_Byte, 4, 4 * xSize) == CE_Failure) {
            valid = false;
            break;
        }
    }
    GDALClose(poDataset);
    if (valid) {
        return new FXImage(getApp(), result, IMAGE_OWNED | IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP, xSize, ySize);
    }
    FXFREE(&result);
#endif
    return nullptr;
}


void
GUISUMOAbstractView::drawDecals() {
    GLHelper::pushName(0);
    myDecalsLockMutex.lock();
    for (auto& decal : myDecals) {
        if (decal.skip2D || decal.filename.empty()) {
            continue;
        }
        if (!decal.initialised) {
            try {
                FXImage* img = checkGDALImage(decal);
                if (img == nullptr) {
                    img = MFXImageHelper::loadImage(getApp(), decal.filename);
                }
                MFXImageHelper::scalePower2(img, GUITexturesHelper::getMaxTextureSize());
                decal.glID = GUITexturesHelper::add(img);
                decal.initialised = true;
                decal.image = img;
            } catch (InvalidArgument& e) {
                WRITE_ERROR("Could not load '" + decal.filename + "'.\n" + e.what());
                decal.skip2D = true;
            }
        }
        GLHelper::pushMatrix();
        if (decal.screenRelative) {
            Position center = screenPos2NetPos((int)decal.centerX, (int)decal.centerY);
            glTranslated(center.x(), center.y(), decal.layer);
        } else {
            glTranslated(decal.centerX, decal.centerY, decal.layer);
        }
        glRotated(decal.rot, 0, 0, 1);
        glColor3d(1, 1, 1);
        double halfWidth = decal.width / 2.;
        double halfHeight = decal.height / 2.;
        if (decal.screenRelative) {
            halfWidth = p2m(halfWidth);
            halfHeight = p2m(halfHeight);
        }
        GUITexturesHelper::drawTexturedBox(decal.glID, -halfWidth, -halfHeight, halfWidth, halfHeight);
        GLHelper::popMatrix();
    }
    myDecalsLockMutex.unlock();
    GLHelper::popName();
}


void
GUISUMOAbstractView::openPopupDialog() {
    int x, y;
    FXuint b;
    myApp->getCursorPosition(x, y, b);
    int appX = myApp->getX();
    int popX = x + appX;
    int popY = y + myApp->getY();
    myPopup->setX(popX);
    myPopup->setY(popY);
    myPopup->create();
    myPopup->show();
    // TODO: try to stay on screen even on a right secondary screen in multi-monitor setup
    const int rootWidth = getApp()->getRootWindow()->getWidth();
    const int rootHeight = getApp()->getRootWindow()->getHeight();
    if (popX <= rootWidth) {
        const int maxX = (appX < 0) ? 0 : rootWidth;
        popX = MIN2(popX, maxX - myPopup->getWidth() - 10);
    }
    popY = MIN2(popY, rootHeight - myPopup->getHeight() - 50);
    myPopup->move(popX, popY);
    myPopupPosition = getPositionInformation();
    myChanger->onRightBtnRelease(nullptr);
    setFocus();
}

// ------------ Additional visualisations

bool
GUISUMOAbstractView::addAdditionalGLVisualisation(GUIGlObject* const which) {
    if (myAdditionallyDrawn.find(which) == myAdditionallyDrawn.end()) {
        myAdditionallyDrawn[which] = 1;
    } else {
        myAdditionallyDrawn[which] = myAdditionallyDrawn[which] + 1;
    }
    update();
    return true;
}


bool
GUISUMOAbstractView::removeAdditionalGLVisualisation(GUIGlObject* const which) {
    if (myAdditionallyDrawn.find(which) == myAdditionallyDrawn.end()) {
        return false;
    }
    int cnt = myAdditionallyDrawn[which];
    if (cnt == 1) {
        myAdditionallyDrawn.erase(which);
    } else {
        myAdditionallyDrawn[which] = myAdditionallyDrawn[which] - 1;
    }
    update();
    return true;
}


bool
GUISUMOAbstractView::isAdditionalGLVisualisationEnabled(GUIGlObject* const which) const {
    if (myAdditionallyDrawn.find(which) == myAdditionallyDrawn.end()) {
        return false;
    } else {
        return true;
    }
}


Boundary
GUISUMOAbstractView::applyGLTransform(bool fixRatio) {
    Boundary bound = myChanger->getViewport(fixRatio);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // as a rough rule, each GLObject is drawn at z = -GUIGlObjectType
    // thus, objects with a higher value will be closer (drawn on top)
    // // @todo last param should be 0 after modifying all glDraw methods
    glOrtho(0, getWidth(), 0, getHeight(), -GLO_MAX - 1, GLO_MAX + 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    double scaleX = (double)getWidth() / bound.getWidth();
    double scaleY = (double)getHeight() / bound.getHeight();
    glScaled(scaleX, scaleY, 1);
    glTranslated(-bound.xmin(), -bound.ymin(), 0);
    // rotate around the center of the screen
    //double angle = -90;
    if (myChanger->getRotation() != 0) {
        glTranslated(bound.getCenter().x(), bound.getCenter().y(), 0);
        glRotated(myChanger->getRotation(), 0, 0, 1);
        glTranslated(-bound.getCenter().x(), -bound.getCenter().y(), 0);
        Boundary rotBound;
        double rad = -DEG2RAD(myChanger->getRotation());
        rotBound.add(Position(bound.xmin(), bound.ymin()).rotateAround2D(rad, bound.getCenter()));
        rotBound.add(Position(bound.xmin(), bound.ymax()).rotateAround2D(rad, bound.getCenter()));
        rotBound.add(Position(bound.xmax(), bound.ymin()).rotateAround2D(rad, bound.getCenter()));
        rotBound.add(Position(bound.xmax(), bound.ymax()).rotateAround2D(rad, bound.getCenter()));
        bound = rotBound;
    }
    myVisualizationSettings->angle = myChanger->getRotation();
    return bound;
}


double
GUISUMOAbstractView::getDelay() const {
    return myApp->getDelay();
}


void
GUISUMOAbstractView::setDelay(double delay) {
    myApp->setDelay(delay);
}


void
GUISUMOAbstractView::setBreakpoints(const std::vector<SUMOTime>& breakpoints) {
    myApp->setBreakpoints(breakpoints);
}


void
GUISUMOAbstractView::buildMinMaxRainbow(const GUIVisualizationSettings& s, GUIColorScheme& scheme,
                                        const GUIVisualizationRainbowSettings& rs, double minValue, double maxValue, bool hasMissingData) {
    if (rs.hideMin && rs.hideMax && minValue == std::numeric_limits<double>::infinity()) {
        minValue = rs.minThreshold;
        maxValue = rs.maxThreshold;
    }
    if (rs.fixRange) {
        if (rs.hideMin) {
            minValue = rs.minThreshold;
        }
        if (rs.hideMax) {
            maxValue = rs.maxThreshold;
        }
    }
    if (minValue != std::numeric_limits<double>::infinity()) {
        scheme.clear();
        // add new thresholds
        if (scheme.getName() == GUIVisualizationSettings::SCHEME_NAME_EDGEDATA_NUMERICAL
                || scheme.getName() == GUIVisualizationSettings::SCHEME_NAME_EDGE_PARAM_NUMERICAL
                || scheme.getName() == GUIVisualizationSettings::SCHEME_NAME_LANE_PARAM_NUMERICAL
                || scheme.getName() == GUIVisualizationSettings::SCHEME_NAME_DATA_ATTRIBUTE_NUMERICAL
                || scheme.getName() == GUIVisualizationSettings::SCHEME_NAME_PARAM_NUMERICAL
                || hasMissingData) {
            scheme.addColor(s.COL_MISSING_DATA, s.MISSING_DATA, "missing data");
        }
        if (rs.hideMin && !rs.fixRange) {
            const double rawRange = maxValue - minValue;
            minValue = MAX2(rs.minThreshold + MIN2(1.0, rawRange / 100.0), minValue);
            scheme.addColor(RGBColor(204, 204, 204), rs.minThreshold);
        }
        if (rs.hideMax && !rs.fixRange) {
            const double rawRange = maxValue - minValue;
            maxValue = MIN2(rs.maxThreshold - MIN2(1.0, rawRange / 100.0), maxValue);
            scheme.addColor(RGBColor(204, 204, 204), rs.maxThreshold);
        }
        const double range = maxValue - minValue;
        scheme.addColor(rs.colors.front(), minValue);
        const int steps = (int)rs.colors.size() - 1;
        if (rs.setNeutral) {
            const int steps1 = steps / 2;
            const int steps2 = steps - steps1;
            const double range1 = rs.neutralThreshold - minValue;
            const double range2 = maxValue - rs.neutralThreshold;
            for (int i = 1; i < steps1; i++) {
                scheme.addColor(rs.colors[i], (minValue + range1 * i / steps1));
            }
            scheme.addColor(rs.colors[steps1], rs.neutralThreshold);
            for (int i = 1; i < steps2; i++) {
                scheme.addColor(rs.colors[steps1 + i], (rs.neutralThreshold + range2 * i / steps2));
            }
        } else {
            for (int i = 1; i < steps; i++) {
                scheme.addColor(rs.colors[i], (minValue + range * i / steps));
            }
        }
        scheme.addColor(rs.colors.back(), maxValue);
    }
}


GUISUMOAbstractView::LayerObject::LayerObject(double layer, GUIGlObject* object) :
    myGLObject(object) {
    first = layer;
    second.first = object->getType();
    second.second = object->getMicrosimID();
}


GUISUMOAbstractView::LayerObject::LayerObject(GUIGlObject* object) :
    myGLObject(object) {
    first = object->getType();
    second.first = object->getType();
    second.second = object->getMicrosimID();
}


GUIGlObject*
GUISUMOAbstractView::LayerObject::getGLObject() const {
    return myGLObject;
}

/****************************************************************************/
