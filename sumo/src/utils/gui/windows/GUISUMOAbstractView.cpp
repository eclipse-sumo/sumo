/****************************************************************************/
/// @file    GUISUMOAbstractView.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @author  Andreas Gaubatz
/// @date    Sept 2002
/// @version $Id$
///
// The base class for a view
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
#include <cassert>
#include <limits>
#include <fxkeys.h>
#include <foreign/polyfonts/polyfonts.h>
#include <foreign/gl2ps/gl2ps.h>
#include <utils/foxtools/FXSingleEventThread.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/foxtools/MFXImageHelper.h>
#include <utils/common/RGBColor.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
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
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GUISUMOAbstractView.h"
#include "GUIMainWindow.h"
#include "GUIGlChildWindow.h"
#include "GUIDanielPerspectiveChanger.h"
#include "GUIDialog_EditViewport.h"

#ifdef HAVE_GDAL
#include <gdal_priv.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUISUMOAbstractView - FOX callback mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUISUMOAbstractView) GUISUMOAbstractViewMap[] = {
    FXMAPFUNC(SEL_CONFIGURE,            0,      GUISUMOAbstractView::onConfigure),
    FXMAPFUNC(SEL_PAINT,                0,      GUISUMOAbstractView::onPaint),
    FXMAPFUNC(SEL_LEFTBUTTONPRESS,      0,      GUISUMOAbstractView::onLeftBtnPress),
    FXMAPFUNC(SEL_LEFTBUTTONRELEASE,    0,      GUISUMOAbstractView::onLeftBtnRelease),
    FXMAPFUNC(SEL_MIDDLEBUTTONPRESS,    0,      GUISUMOAbstractView::onMiddleBtnPress),
    FXMAPFUNC(SEL_MIDDLEBUTTONRELEASE,  0,      GUISUMOAbstractView::onMiddleBtnRelease),
    FXMAPFUNC(SEL_RIGHTBUTTONPRESS,     0,      GUISUMOAbstractView::onRightBtnPress),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,   0,      GUISUMOAbstractView::onRightBtnRelease),
    FXMAPFUNC(SEL_DOUBLECLICKED,        0,      GUISUMOAbstractView::onDoubleClicked),
    FXMAPFUNC(SEL_MOUSEWHEEL,           0,      GUISUMOAbstractView::onMouseWheel),
    FXMAPFUNC(SEL_MOTION,               0,      GUISUMOAbstractView::onMouseMove),
    FXMAPFUNC(SEL_LEAVE,                0,      GUISUMOAbstractView::onMouseLeft),
    FXMAPFUNC(SEL_KEYPRESS,             0,      GUISUMOAbstractView::onKeyPress),
    FXMAPFUNC(SEL_KEYRELEASE,           0,      GUISUMOAbstractView::onKeyRelease),

};


FXIMPLEMENT_ABSTRACT(GUISUMOAbstractView, FXGLCanvas, GUISUMOAbstractViewMap, ARRAYNUMBER(GUISUMOAbstractViewMap))


/* -------------------------------------------------------------------------
 * GUISUMOAbstractView - methods
 * ----------------------------------------------------------------------- */
GUISUMOAbstractView::GUISUMOAbstractView(FXComposite* p, GUIMainWindow& app, GUIGlChildWindow* parent, const SUMORTree& grid, FXGLVisual* glVis, FXGLCanvas* share) :
    FXGLCanvas(p, glVis, share, p, MID_GLCANVAS,
               LAYOUT_SIDE_TOP | LAYOUT_FILL_X | LAYOUT_FILL_Y, 0, 0, 0, 0),
    myApp(&app),
    myParent(parent),
    myGrid(&((SUMORTree&)grid)),
    myChanger(0),
    myMouseHotspotX(app.getDefaultCursor()->getHotX()),
    myMouseHotspotY(app.getDefaultCursor()->getHotY()),
    myPopup(0),
    myUseToolTips(false),
    myAmInitialised(false),
    myViewportChooser(0),
    myWindowCursorPositionX(getWidth() / 2),
    myWindowCursorPositionY(getHeight() / 2),
    myVisualizationChanger(0) {
    setTarget(this);
    enable();
    flags |= FLAG_ENABLED;
    myInEditMode = false;
    // show the middle at the beginning
    myChanger = new GUIDanielPerspectiveChanger(*this, *myGrid);
    myVisualizationSettings = &gSchemeStorage.getDefault();
    myVisualizationSettings->gaming = myApp->isGaming();
    gSchemeStorage.setViewport(this);
}


GUISUMOAbstractView::~GUISUMOAbstractView() {
    gSchemeStorage.setDefault(myVisualizationSettings->name);
    gSchemeStorage.saveViewport(myChanger->getXPos(), myChanger->getYPos(), myChanger->getZPos());
    delete myPopup;
    delete myChanger;
    delete myViewportChooser;
    delete myVisualizationChanger;
    // cleanup decals
    for (std::vector<GUISUMOAbstractView::Decal>::iterator it = myDecals.begin(); it != myDecals.end(); ++it) {
        delete it->image;
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
    if (!myUseToolTips) {
        return;
    }
    update();
}


Position
GUISUMOAbstractView::getPositionInformation() const {
    return screenPos2NetPos(myWindowCursorPositionX, myWindowCursorPositionY);
}


Position
GUISUMOAbstractView::screenPos2NetPos(int x, int y) const {
    Boundary bound = myChanger->getViewport();
    SUMOReal xNet = bound.xmin() + bound.getWidth() * x / getWidth();
    // cursor origin is in the top-left corner
    SUMOReal yNet = bound.ymin() + bound.getHeight() * (getHeight() - y) / getHeight();
    return Position(xNet, yNet);
}


void
GUISUMOAbstractView::addDecals(const std::vector<Decal>& decals) {
    myDecals.insert(myDecals.end(), decals.begin(), decals.end());
}


GUIVisualizationSettings*
GUISUMOAbstractView::getVisualisationSettings() {
    return myVisualizationSettings;
}


void
GUISUMOAbstractView::updatePositionInformation() const {
    Position pos = getPositionInformation();
    std::string text = "x:" + toString(pos.x()) + ", y:" + toString(pos.y());
    myApp->getCartesianLabel().setText(text.c_str());
    GeoConvHelper::getFinal().cartesian2geo(pos);
    if (GeoConvHelper::getFinal().usingGeoProjection()) {
        text = "lat:" + toString(pos.y(), gPrecisionGeo) + ", lon:" + toString(pos.x(), gPrecisionGeo);
    } else {
        text = "x:" + toString(pos.x()) + ", y:" + toString(pos.y());
    }
    myApp->getGeoLabel().setText(text.c_str());
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


void
GUISUMOAbstractView::paintGL() {
    if (getWidth() == 0 || getHeight() == 0) {
        return;
    }

    if (getTrackedID() != GUIGlObject::INVALID_ID) {
        centerTo(getTrackedID(), false);
    }

    GUIGlID id = GUIGlObject::INVALID_ID;
    if (myUseToolTips) {
        id = getObjectUnderCursor();
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
    if (myVisualizationSettings->antialiase) {
        glEnable(GL_BLEND);
        glEnable(GL_POLYGON_SMOOTH);
        glEnable(GL_LINE_SMOOTH);
    } else {
        glDisable(GL_BLEND);
        glDisable(GL_POLYGON_SMOOTH);
        glDisable(GL_LINE_SMOOTH);
    }

    applyGLTransform();
    doPaintGL(GL_RENDER, myChanger->getViewport());
    if (myVisualizationSettings->showSizeLegend) {
        displayLegend();
    }
    // check whether the select mode /tooltips)
    //  shall be computed, too
    if (myUseToolTips && id != GUIGlObject::INVALID_ID) {
        showToolTipFor(id);
    }
    swapBuffers();
}


GUIGlID
GUISUMOAbstractView::getObjectUnderCursor() {
    return getObjectAtPosition(getPositionInformation());
}


GUIGlID
GUISUMOAbstractView::getObjectAtPosition(Position pos) {
    const SUMOReal SENSITIVITY = 0.1; // meters
    Boundary selection;
    selection.add(pos);
    selection.grow(SENSITIVITY);
    const std::vector<GUIGlID> ids = getObjectsInBoundary(selection);
    // Interpret results
    int idMax = 0;
    SUMOReal maxLayer = -std::numeric_limits<SUMOReal>::max();
    for (std::vector<GUIGlID>::const_iterator it = ids.begin(); it != ids.end(); it++) {
        GUIGlID id = *it;
        GUIGlObject* o = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
        if (o == 0) {
            continue;
        }
        if (o->getGlID() == 0) {
            continue;
        }
        //std::cout << "point selection hit " << o->getMicrosimID() << "\n";
        GUIGlObjectType type = o->getType();
        if (type != 0) {
            SUMOReal layer = (SUMOReal)type;
            // determine an "abstract" layer for shapes
            //  this "layer" resembles the layer of the shape
            //  taking into account the stac of other objects
            if (type == GLO_POI || type == GLO_POLYGON) {
                layer = dynamic_cast<Shape*>(o)->getLayer();
            }
            if (type == GLO_LANE && GUIVisualizationSettings::UseMesoSim) {
                // do not select lanes in meso mode
                continue;
            }
            // check whether the current object is above a previous one
            if (layer > maxLayer) {
                idMax = id;
                maxLayer = layer;
            }
        }
        GUIGlObjectStorage::gIDStorage.unblockObject(id);
    }
    return idMax;
}


std::vector<GUIGlID>
GUISUMOAbstractView::getObjectsAtPosition(Position pos, SUMOReal radius) {
    Boundary selection;
    selection.add(pos);
    selection.grow(radius);
    const std::vector<GUIGlID> ids = getObjectsInBoundary(selection);
    std::vector<GUIGlID> result;
    // Interpret results
    for (std::vector<GUIGlID>::const_iterator it = ids.begin(); it != ids.end(); it++) {
        GUIGlID id = *it;
        GUIGlObject* o = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
        if (o == 0) {
            continue;
        }
        if (o->getGlID() == 0) {
            continue;
        }
        //std::cout << "point selection hit " << o->getMicrosimID() << "\n";
        GUIGlObjectType type = o->getType();
        if (type != 0) {
            result.push_back(id);
        }
        GUIGlObjectStorage::gIDStorage.unblockObject(id);
    }
    return result;
}


std::vector<GUIGlID>
GUISUMOAbstractView::getObjectsInBoundary(const Boundary& bound) {
    const int NB_HITS_MAX = 1024 * 1024;
    // Prepare the selection mode
    static GUIGlID hits[NB_HITS_MAX];
    static GLint nb_hits = 0;
    glSelectBuffer(NB_HITS_MAX, hits);
    glInitNames();

    Boundary oldViewPort = myChanger->getViewport(false); // backup the actual viewPort
    myChanger->setViewport(bound);
    applyGLTransform(false);

    // paint in select mode
    myVisualizationSettings->drawForSelecting = true;
    int hits2 = doPaintGL(GL_SELECT, bound);
    myVisualizationSettings->drawForSelecting = false;
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


void
GUISUMOAbstractView::showToolTipFor(const GUIGlID id) {
    if (id != 0) {
        GUIGlObject* object = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
        if (object != 0) {
            Position pos = getPositionInformation();
            pos.add(0, p2m(15));
            GLHelper::drawTextBox(object->getFullName(), pos, GLO_MAX - 1, p2m(20), RGBColor::BLACK, RGBColor(255, 179, 0, 255));
            GUIGlObjectStorage::gIDStorage.unblockObject(id);
        }
    }
}


void
GUISUMOAbstractView::paintGLGrid() {
    glEnable(GL_DEPTH_TEST);
    glLineWidth(1);

    SUMOReal xmin = myGrid->xmin();
    SUMOReal ymin = myGrid->ymin();
    SUMOReal ypos = ymin;
    SUMOReal xpos = xmin;
    SUMOReal xend = myGrid->xmax();
    SUMOReal yend = myGrid->ymax();

    glTranslated(0, 0, .55);
    glColor3d(0.5, 0.5, 0.5);
    // draw horizontal lines
    glBegin(GL_LINES);
    for (; ypos < yend;) {
        glVertex2d(xmin, ypos);
        glVertex2d(xend, ypos);
        ypos += myVisualizationSettings->gridYSize;
    }
    // draw vertical lines
    for (; xpos < xend;) {
        glVertex2d(xpos, ymin);
        glVertex2d(xpos, yend);
        xpos += myVisualizationSettings->gridXSize;
    }
    glEnd();
    glTranslated(0, 0, -.55);
}


void
GUISUMOAbstractView::displayLegend() {
    // compute the scale bar length
    int length = 1;
    const std::string text("10000000000");
    int noDigits = 1;
    int pixelSize = (int) m2p((SUMOReal) length);
    while (pixelSize <= 20) {
        length *= 10;
        noDigits++;
        if (noDigits > (int)text.length()) {
            return;
        }
        pixelSize = (int) m2p((SUMOReal) length);
    }
    glLineWidth(1.0);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // draw the scale bar
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    SUMOReal len = (SUMOReal) pixelSize / (SUMOReal)(getWidth() - 1) * (SUMOReal) 2.0;
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

    SUMOReal w = SUMOReal(35) / SUMOReal(getWidth());
    SUMOReal h = SUMOReal(35) / SUMOReal(getHeight());
    pfSetPosition(SUMOReal(-0.99), SUMOReal(1. - o2 - oo));
    pfSetScaleXY(w, h);
    glRotated(180, 1, 0, 0);
    pfDrawString("0m");
    glRotated(-180, 1, 0, 0);

    pfSetPosition(SUMOReal(-.99 + len), SUMOReal(1. - o2 - oo));
    glRotated(180, 1, 0, 0);
    pfDrawString((text.substr(0, noDigits) + "m").c_str());
    glRotated(-180, 1, 0, 0);

    // restore matrices
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}


SUMOReal
GUISUMOAbstractView::m2p(SUMOReal meter) const {
    return  meter * getWidth() / myChanger->getViewport().getWidth();
}


SUMOReal
GUISUMOAbstractView::p2m(SUMOReal pixel) const {
    return pixel * myChanger->getViewport().getWidth() / getWidth();
}


void
GUISUMOAbstractView::recenterView() {
    myChanger->setViewport(*myGrid);
}


void
GUISUMOAbstractView::centerTo(GUIGlID id, bool applyZoom, SUMOReal zoomDist) {
    GUIGlObject* o = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
    if (o != 0 && dynamic_cast<GUIGlObject*>(o) != 0) {
        if (applyZoom && zoomDist < 0) {
            myChanger->setViewport(o->getCenteringBoundary());
            update(); // only update when centering onto an object once
        } else {
            // called during tracking. update is triggered somewhere else
            myChanger->centerTo(o->getCenteringBoundary().getCenter(), zoomDist, applyZoom);
            updatePositionInformation();
        }
    }
    GUIGlObjectStorage::gIDStorage.unblockObject(id);
}


void
GUISUMOAbstractView::centerTo(const Boundary& bound) {
    myChanger->setViewport(bound);
    update();
}

/*
bool
GUISUMOAbstractView::allowRotation() const
{
    return myParent->allowRotation();
}
*/

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


void
GUISUMOAbstractView::destroyPopup() {
    delete myPopup;
    myPopup = 0;
}


long
GUISUMOAbstractView::onLeftBtnPress(FXObject*, FXSelector , void* data) {
    destroyPopup();
    setFocus();
    FXEvent* e = (FXEvent*) data;
    // check whether the selection-mode is activated
    if ((e->state & CONTROLMASK) != 0) {
        // try to get the object-id if so
        if (makeCurrent()) {
            int id = getObjectUnderCursor();
            if (id != 0) {
                gSelected.toggleSelection(id);
            }
            makeNonCurrent();
            if (id != 0) {
                // possibly, the selection-colouring is used,
                //  so we should update the screen again...
                update();
            }
        }
    }
    myChanger->onLeftBtnPress(data);
    grab();
    // Check there are double click
    if (e->click_count == 2) {
        handle(this, FXSEL(SEL_DOUBLECLICKED, 0), data);
    }
    return 1;
}


long
GUISUMOAbstractView::onLeftBtnRelease(FXObject*, FXSelector , void* data) {
    destroyPopup();
    myChanger->onLeftBtnRelease(data);
    if (myApp->isGaming()) {
        onGamingClick(getPositionInformation());
    }
    ungrab();
    return 1;
}


long
GUISUMOAbstractView::onMiddleBtnPress(FXObject*, FXSelector, void*) {
    return 1;
}


long
GUISUMOAbstractView::onMiddleBtnRelease(FXObject*, FXSelector, void*) {
    return 1;
}


long
GUISUMOAbstractView::onRightBtnPress(FXObject*, FXSelector , void* data) {
    destroyPopup();
    myChanger->onRightBtnPress(data);
    grab();
    return 1;
}


long
GUISUMOAbstractView::onRightBtnRelease(FXObject* o, FXSelector sel, void* data) {
    destroyPopup();
    onMouseMove(o, sel, data);
    if (!myChanger->onRightBtnRelease(data) && !myApp->isGaming()) {
        openObjectDialog();
    }
    ungrab();
    return 1;
}


long
GUISUMOAbstractView::onDoubleClicked(FXObject*, FXSelector, void*) {
    return 1;
}


long
GUISUMOAbstractView::onMouseWheel(FXObject*, FXSelector , void* data) {
    if (!myApp->isGaming()) {
        myChanger->onMouseWheel(data);
        updatePositionInformation();
    }
    return 1;
}


long
GUISUMOAbstractView::onMouseMove(FXObject*, FXSelector , void* data) {
    if (myViewportChooser == 0 || !myViewportChooser->haveGrabbed()) {
        myChanger->onMouseMove(data);
    }
    if (myViewportChooser != 0) {
        myViewportChooser->setValues(myChanger->getZoom(), myChanger->getXPos(), myChanger->getYPos());
    }
    updatePositionInformation();
    return 1;
}


long
GUISUMOAbstractView::onMouseLeft(FXObject*, FXSelector , void* /*data*/) {
    return 1;
}


void
GUISUMOAbstractView::openObjectDialog() {
    ungrab();
    if (!isEnabled() || !myAmInitialised) {
        return;
    }
    if (makeCurrent()) {
        // initialise the select mode
        int id = getObjectUnderCursor();
        GUIGlObject* o = 0;
        if (id != 0) {
            o = GUIGlObjectStorage::gIDStorage.getObjectBlocking(id);
        } else {
            o = GUIGlObjectStorage::gIDStorage.getNetObject();
        }
        if (o != 0) {
            myPopup = o->getPopUpMenu(*myApp, *this);
            int x, y;
            FXuint b;
            myApp->getCursorPosition(x, y, b);
            myPopup->setX(x + myApp->getX());
            myPopup->setY(y + myApp->getY());
            myPopup->create();
            myPopup->show();
            myChanger->onRightBtnRelease(0);
            GUIGlObjectStorage::gIDStorage.unblockObject(id);
        }
        makeNonCurrent();
    }
}


long
GUISUMOAbstractView::onKeyPress(FXObject* o, FXSelector sel, void* data) {
    FXGLCanvas::onKeyPress(o, sel, data);
    return myChanger->onKeyPress(data);
}


long
GUISUMOAbstractView::onKeyRelease(FXObject* o, FXSelector sel, void* data) {
    FXGLCanvas::onKeyRelease(o, sel, data);
    return myChanger->onKeyRelease(data);
}


// ------------ Dealing with snapshots
void
GUISUMOAbstractView::setSnapshots(std::map<SUMOTime, std::string> snaps) {
    mySnapshots.insert(snaps.begin(), snaps.end());
}


std::string
GUISUMOAbstractView::makeSnapshot(const std::string& destFile) {
    std::string errorMessage;
    FXString ext = FXPath::extension(destFile.c_str());
    const bool useGL2PS = ext == "ps" || ext == "eps" || ext == "pdf" || ext == "svg" || ext == "tex" || ext == "pgf";
#ifdef HAVE_FFMPEG
    const bool useVideo = destFile == "" || ext == "h264" || ext == "hevc";
#endif
    for (int i = 0; i < 10 && !makeCurrent(); ++i) {
        FXSingleEventThread::sleep(100);
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
    if (myVisualizationSettings->antialiase) {
        glEnable(GL_BLEND);
        glEnable(GL_POLYGON_SMOOTH);
        glEnable(GL_LINE_SMOOTH);
    } else {
        glDisable(GL_BLEND);
        glDisable(GL_POLYGON_SMOOTH);
        glDisable(GL_LINE_SMOOTH);
    }

    applyGLTransform();

    if (useGL2PS) {
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
        GLint buffsize = 0, state = GL2PS_OVERFLOW;
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        while (state == GL2PS_OVERFLOW) {
            buffsize += 1024 * 1024;
            gl2psBeginPage(destFile.c_str(), "sumo-gui; http://sumo.dlr.de", viewport, format, GL2PS_SIMPLE_SORT,
                           GL2PS_DRAW_BACKGROUND | GL2PS_USE_CURRENT_VIEWPORT,
                           GL_RGBA, 0, NULL, 0, 0, 0, buffsize, fp, "out.eps");
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_ALPHA_TEST);
            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
            // compute lane width
            // draw decals (if not in grabbing mode)
            if (!myUseToolTips) {
                drawDecals();
                if (myVisualizationSettings->showGrid) {
                    paintGLGrid();
                }
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

            if (myVisualizationSettings->showSizeLegend) {
                displayLegend();
            }
            state = gl2psEndPage();
            glFinish();
        }
        fclose(fp);
    } else {
        doPaintGL(GL_RENDER, myChanger->getViewport());
        if (myVisualizationSettings->showSizeLegend) {
            displayLegend();
        }
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
    std::map<SUMOTime, std::string>::iterator snapIt = mySnapshots.find(getCurrentTimeStep());
    if (snapIt != mySnapshots.end()) {
        std::string error = makeSnapshot(snapIt->second);
        if (error != "") {
            WRITE_WARNING(error);
        }
    }
}


SUMOTime
GUISUMOAbstractView::getCurrentTimeStep() const {
    return 0;
}


void
GUISUMOAbstractView::showViewschemeEditor() {
    if (myVisualizationChanger == 0) {
        myVisualizationChanger =
            new GUIDialog_ViewSettings(
            this, myVisualizationSettings,
            &myDecals, &myDecalsLock);
        myVisualizationChanger->create();
    } else {
        myVisualizationChanger->setCurrent(myVisualizationSettings);
    }
    myVisualizationChanger->show();
}


GUIDialog_EditViewport*
GUISUMOAbstractView::getViewportEditor() {
    if (myViewportChooser == 0) {
        myViewportChooser =
            new GUIDialog_EditViewport(this, "Edit Viewport...", 0, 0);
        myViewportChooser->create();
    }
    myViewportChooser->setValues(myChanger->getZoom(), myChanger->getXPos(), myChanger->getYPos());
    return myViewportChooser;
}


void
GUISUMOAbstractView::showViewportEditor() {
    getViewportEditor(); // make sure it exists;
    Position p(myChanger->getXPos(), myChanger->getYPos(), myChanger->getZPos());
    myViewportChooser->setOldValues(p, Position::INVALID);
    myViewportChooser->show();
}


void
GUISUMOAbstractView::setViewportFromTo(const Position& lookFrom, const Position& /* lookAt */) {
    myChanger->setViewportFrom(lookFrom.x(), lookFrom.y(), lookFrom.z());
    update();
}


void
GUISUMOAbstractView::copyViewportTo(GUISUMOAbstractView* view) {
    // look straight down
    view->setViewportFromTo(Position(myChanger->getXPos(), myChanger->getYPos(), myChanger->getZPos()),
                            Position(myChanger->getXPos(), myChanger->getYPos(), 0));
}


void
GUISUMOAbstractView::showToolTips(bool val) {
    myUseToolTips = val;
}


bool
GUISUMOAbstractView::setColorScheme(const std::string&) {
    return true;
}


GUIVisualizationSettings*
GUISUMOAbstractView::getVisualisationSettings() const {
    return myVisualizationSettings;
}


void
GUISUMOAbstractView::remove(GUIDialog_EditViewport*) {
    myViewportChooser = 0;
}


void
GUISUMOAbstractView::remove(GUIDialog_ViewSettings*) {
    myVisualizationChanger = 0;
}


SUMOReal
GUISUMOAbstractView::getGridWidth() const {
    return myGrid->getWidth();
}


SUMOReal
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


FXComboBox&
GUISUMOAbstractView::getColoringSchemesCombo() {
    return myParent->getColoringSchemesCombo();
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
                WRITE_WARNING("Could not convert coordinates in " + d.filename + ".");
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
        WRITE_WARNING("Could not allocate memory for " + d.filename + ".");
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
            WRITE_MESSAGE("Unknown color band in " + d.filename + ", maybe fox can parse it.");
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
    return 0;
}


void
GUISUMOAbstractView::drawDecals() {
    glPushName(0);
    myDecalsLock.lock();
    for (std::vector<GUISUMOAbstractView::Decal>::iterator l = myDecals.begin(); l != myDecals.end(); ++l) {
        GUISUMOAbstractView::Decal& d = *l;
        if (d.skip2D) {
            continue;
        }
        if (!d.initialised) {
            try {
                FXImage* img = checkGDALImage(d);
                if (img == 0) {
                    img = MFXImageHelper::loadImage(getApp(), d.filename);
                }
                MFXImageHelper::scalePower2(img, GUITexturesHelper::getMaxTextureSize());
                d.glID = GUITexturesHelper::add(img);
                d.initialised = true;
                d.image = img;
            } catch (InvalidArgument& e) {
                WRITE_ERROR("Could not load '" + d.filename + "'.\n" + e.what());
                d.skip2D = true;
            }
        }
        glPushMatrix();
        if (d.screenRelative) {
            Position center = screenPos2NetPos((int)d.centerX, (int)d.centerY);
            glTranslated(center.x(), center.y(), d.layer);
        } else {
            glTranslated(d.centerX, d.centerY, d.layer);
        }
        glRotated(d.rot, 0, 0, 1);
        glColor3d(1, 1, 1);
        SUMOReal halfWidth = d.width / 2.;
        SUMOReal halfHeight = d.height / 2.;
        if (d.screenRelative) {
            halfWidth = p2m(halfWidth);
            halfHeight = p2m(halfHeight);
        }
        GUITexturesHelper::drawTexturedBox(d.glID, -halfWidth, -halfHeight, halfWidth, halfHeight);
        glPopMatrix();
    }
    myDecalsLock.unlock();
    glPopName();
}


// ------------ Additional visualisations
bool
GUISUMOAbstractView::addAdditionalGLVisualisation(const GUIGlObject* const which) {
    if (myAdditionallyDrawn.find(which) == myAdditionallyDrawn.end()) {
        myAdditionallyDrawn[which] = 1;
    } else {
        myAdditionallyDrawn[which] = myAdditionallyDrawn[which] + 1;
    }
    update();
    return true;
}


bool
GUISUMOAbstractView::removeAdditionalGLVisualisation(const GUIGlObject* const which) {
    if (getTrackedID() == which->getGlID()) {
        stopTrack();
    }
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


void
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
    SUMOReal scaleX = (SUMOReal)getWidth() / bound.getWidth();
    SUMOReal scaleY = (SUMOReal)getHeight() / bound.getHeight();
    glScaled(scaleX, scaleY, 1);
    glTranslated(-bound.xmin(), -bound.ymin(), 0);
}


SUMOReal
GUISUMOAbstractView::getDelay() const {
    return myApp->getDelay();
}


void
GUISUMOAbstractView::setDelay(SUMOReal delay) {
    myApp->setDelay(delay);
}


GUISUMOAbstractView::Decal::Decal() :
    filename(),
    centerX(0),
    centerY(0),
    centerZ(0),
    width(0),
    height(0),
    altitude(0),
    rot(0),
    tilt(0),
    roll(0),
    layer(0),
    initialised(false),
    skip2D(false),
    screenRelative(false),
    glID(-1),
    image(0) {
}


/****************************************************************************/

