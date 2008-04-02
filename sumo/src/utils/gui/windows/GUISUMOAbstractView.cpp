/****************************************************************************/
/// @file    GUISUMOAbstractView.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The base class for a view
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include <utils/common/RGBColor.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/gui/globjects/GUIGLObjectToolTip.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include "GUIDanielPerspectiveChanger.h"
#include "GUISUMOAbstractView.h"
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include "GUIMainWindow.h"
#include "GUIGlChildWindow.h"
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include "GUIDialog_EditViewport.h"
#include <foreign/polyfonts/polyfonts.h>
#include <utils/shapes/PointOfInterest.h>
#include <utils/shapes/ShapeContainer.h>
#include <utils/gui/globjects/GUIPointOfInterest.h>
#include <utils/gui/globjects/GUIPolygon2D.h>
#include <utils/gui/windows/GUIDialog_ViewSettings.h>
#include <utils/geom/GeoConvHelper.h>


#ifdef WIN32
#include <windows.h>
#else
#define APIENTRY
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUISUMOAbstractView::ViewportSettings - methods
 * ----------------------------------------------------------------------- */
GUISUMOAbstractView::ViewportSettings::ViewportSettings()
        : myXMin(-1), myYMin(-1), myXMax(-1), myYMax(-1)
{}


GUISUMOAbstractView::ViewportSettings::ViewportSettings(SUMOReal xmin,
        SUMOReal ymin,
        SUMOReal xmax,
        SUMOReal ymax)
        : myXMin(xmin), myYMin(ymin), myXMax(xmax), myYMax(ymax)
{}


GUISUMOAbstractView::ViewportSettings::~ViewportSettings()
{}


bool
GUISUMOAbstractView::ViewportSettings::differ(SUMOReal xmin, SUMOReal ymin,
        SUMOReal xmax, SUMOReal ymax)
{
    return myXMin!=xmin || myYMin!=ymin || myXMax!=xmax || myYMax!=ymax;
}


void
GUISUMOAbstractView::ViewportSettings::set(SUMOReal xmin, SUMOReal ymin,
        SUMOReal xmax, SUMOReal ymax)
{
    myXMin = xmin;
    myYMin = ymin;
    myXMax = xmax;
    myYMax = ymax;
}


/* -------------------------------------------------------------------------
 * GUISUMOAbstractView - FOX callback mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUISUMOAbstractView) GUISUMOAbstractViewMap[]= {
    FXMAPFUNC(SEL_CONFIGURE,           0,                 GUISUMOAbstractView::onConfigure),
    FXMAPFUNC(SEL_PAINT,               0,                 GUISUMOAbstractView::onPaint),
    FXMAPFUNC(SEL_LEFTBUTTONPRESS,     0,                 GUISUMOAbstractView::onLeftBtnPress),
    FXMAPFUNC(SEL_LEFTBUTTONRELEASE,   0,                 GUISUMOAbstractView::onLeftBtnRelease),
    FXMAPFUNC(SEL_RIGHTBUTTONPRESS,    0,                 GUISUMOAbstractView::onRightBtnPress),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,  0,                 GUISUMOAbstractView::onRightBtnRelease),
    FXMAPFUNC(SEL_MOTION,              0,                 GUISUMOAbstractView::onMouseMove),
    FXMAPFUNC(SEL_LEAVE,               0,                 GUISUMOAbstractView::onMouseLeft),
    FXMAPFUNC(SEL_COMMAND,             MID_SIMSTEP,       GUISUMOAbstractView::onSimStep),
    FXMAPFUNC(SEL_KEYPRESS,            0,                 GUISUMOAbstractView::onKeyPress),
    FXMAPFUNC(SEL_KEYRELEASE,          0,                 GUISUMOAbstractView::onKeyRelease),

};


FXIMPLEMENT_ABSTRACT(GUISUMOAbstractView,FXGLCanvas,GUISUMOAbstractViewMap,ARRAYNUMBER(GUISUMOAbstractViewMap))


/* -------------------------------------------------------------------------
 * GUISUMOAbstractView - methods
 * ----------------------------------------------------------------------- */
GUISUMOAbstractView::GUISUMOAbstractView(FXComposite *p,
        GUIMainWindow &app,
        GUIGlChildWindow *parent,
        const GUIGrid &grid,
        FXGLVisual *glVis)
        : FXGLCanvas(p, glVis, p,
                     MID_GLCANVAS, LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y,
                     0, 0, 300, 200),
        myApp(&app),
        myParent(parent),
        myGrid(&((GUIGrid&) grid)),
        myChanger(0),
        myMouseHotspotX(app.getDefaultCursor()->getHotX()),
        myMouseHotspotY(app.getDefaultCursor()->getHotY()),
        myPopup(0),
        myAmInitialised(false),
        myViewportChooser(0), myVisualizationChanger(0),
        myUseToolTips(false)
{
    flags|=FLAG_ENABLED;
    myInEditMode=false;
    // compute the net scale
    SUMOReal nw = myGrid->getBoundary().getWidth();
    SUMOReal nh = myGrid->getBoundary().getHeight();
    myNetScale = (nw < nh ? nh : nw);
    // show the middle at the beginning
    myChanger = new GUIDanielPerspectiveChanger(*this);
    myChanger->setNetSizes((size_t) nw, (size_t) nh);
    myToolTip = new GUIGLObjectToolTip(myApp);
}


GUISUMOAbstractView::GUISUMOAbstractView(FXComposite *p,
        GUIMainWindow &app,
        GUIGlChildWindow *parent,
        const GUIGrid &grid,
        FXGLVisual *glVis, FXGLCanvas *share)
        : FXGLCanvas(p, glVis, share, p, MID_GLCANVAS,
                     LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0, 0, 300, 200),
        myApp(&app),
        myParent(parent),
        myGrid(&((GUIGrid&) grid)),
        myChanger(0),
        myMouseHotspotX(app.getDefaultCursor()->getHotX()),
        myMouseHotspotY(app.getDefaultCursor()->getHotY()),
        myPopup(0),
        myAmInitialised(false),
        myViewportChooser(0), myVisualizationChanger(0),
        myUseToolTips(false)
{
    flags|=FLAG_ENABLED;
    myInEditMode=false;
    // compute the net scale
    SUMOReal nw = myGrid->getBoundary().getWidth();
    SUMOReal nh = myGrid->getBoundary().getHeight();
    myNetScale = (nw < nh ? nh : nw);
    // show the middle at the beginning
    myChanger = new GUIDanielPerspectiveChanger(*this);
    myChanger->setNetSizes((size_t) nw, (size_t) nh);
    myToolTip = new GUIGLObjectToolTip(myApp);
}


GUISUMOAbstractView::~GUISUMOAbstractView()
{
    delete myChanger;
    delete myToolTip;
    delete myViewportChooser;
    delete myVisualizationChanger;
}


bool
GUISUMOAbstractView::isInEditMode()
{
    return myInEditMode;
}


void
GUISUMOAbstractView::updateToolTip()
{
    if (!myUseToolTips) {
        return;
    }
    update();
}


Position2D
GUISUMOAbstractView::getPositionInformation() const
{
    return getPositionInformation(myChanger->getMouseXPosition(), myChanger->getMouseYPosition());
}


Position2D
GUISUMOAbstractView::getPositionInformation(int mx, int my) const
{
    const Boundary &nb = myGrid->getBoundary();
    SUMOReal width = nb.getWidth();
    SUMOReal height = nb.getHeight();

    SUMOReal mzoom = myChanger->getZoom();
    // compute the offset
    SUMOReal cy = myChanger->getYPos();//cursorY;
    SUMOReal cx = myChanger->getXPos();//cursorY;
    // compute the visible area in horizontal direction
    SUMOReal mratioX;
    SUMOReal mratioY;
    SUMOReal xs = ((SUMOReal) myWidthInPixels / (SUMOReal) myApp->getMaxGLWidth())
                  / (myGrid->getBoundary().getWidth() / myNetScale) * myRatio;
    SUMOReal ys = ((SUMOReal) myHeightInPixels / (SUMOReal) myApp->getMaxGLHeight())
                  / (myGrid->getBoundary().getHeight() / myNetScale);
    if (xs<ys) {
        mratioX = 1;
        mratioY = ys/xs;
    } else {
        mratioY = 1;
        mratioX = xs/ys;
    }

    SUMOReal sxmin = nb.getCenter().x()
                     - mratioX * width * (SUMOReal) 100 / (mzoom) / (SUMOReal) 2. / (SUMOReal) .97;
    sxmin -= cx;
    SUMOReal sxmax = nb.getCenter().x()
                     + mratioX * width * (SUMOReal) 100 / (mzoom) / (SUMOReal) 2. / (SUMOReal) .97;
    sxmax -= cx;

    // compute the visible area in vertical direction
    SUMOReal symin = nb.getCenter().y()
                     - mratioY * height / mzoom * (SUMOReal) 100 / (SUMOReal) 2. / (SUMOReal) .97;
    symin -= cy;
    SUMOReal symax = nb.getCenter().y()
                     + mratioY * height / mzoom * (SUMOReal) 100 / (SUMOReal) 2. / (SUMOReal) .97;
    symax -= cy;

    SUMOReal sx, sy;
    if (width<height) {
        sx = sxmin
             + (sxmax-sxmin)
             * (SUMOReal) mx
             / (SUMOReal) myWidthInPixels;
        sy = symin
             + (symax-symin)
             * ((SUMOReal) myHeightInPixels - (SUMOReal) my)
             / (SUMOReal) myHeightInPixels;
    } else {
        sx = sxmin
             + (sxmax-sxmin)
             * (SUMOReal) mx
             / (SUMOReal) myWidthInPixels;
        sy = symin
             + (symax-symin)
             * ((SUMOReal) myHeightInPixels - (SUMOReal) my)
             / (SUMOReal) myHeightInPixels;
    }
    return Position2D(sx, sy);
}


void
GUISUMOAbstractView::updatePositionInformation() const
{
    if (true) {
        string text;
        Position2D pos = getPositionInformation();
        text = "x:" + toString(pos.x(), 3) + ", y:" + toString(pos.y(), 3);
        myApp->getCartesianLabel().setText(text.c_str());
        GeoConvHelper::cartesian2geo(pos);
        if (GeoConvHelper::usingGeoProjection()) {
            text = "lat:" + toString(pos.y(), 5) + ", lon:" + toString(pos.x(), 5);
        } else {
            text = "x:" + toString(pos.x(), 3) + ", y:" + toString(pos.y(), 3);
        }
        myApp->getGeoLabel().setText(text.c_str());
    }
}


void
GUISUMOAbstractView::paintGL()
{
    myWidthInPixels = getWidth();
    myHeightInPixels = getHeight();
    if (myWidthInPixels==0||myHeightInPixels==0) {
        return;
    }

    if (getTrackedID()>0) {
        GUIGlObject *o = gIDStorage.getObjectBlocking(getTrackedID());
        if (o!=0) {
            Boundary b = o->getCenteringBoundary();
            //b.grow(20);
            myChanger->centerTo(myGrid->getBoundary(), b, false);
        }
    }

    unsigned int id = 0;
    if (myUseToolTips) {
        id = getObjectUnderCursor();
    }

    // draw
    glClearColor(
        myVisualizationSettings->backgroundColor.red(),
        myVisualizationSettings->backgroundColor.green(),
        myVisualizationSettings->backgroundColor.blue(),
        1);
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

    applyChanges(1.0, 0, 0);
    if (myVisualizationSettings->showGrid) {
        paintGLGrid();
    }
    //
    doPaintGL(GL_RENDER, 1.0);
    if (myVisualizationSettings->showSizeLegend) {
        displayLegend();
    }
    // check whether the select mode /tooltips)
    //  shall be computed, too
    glFlush();
    swapBuffers();
    if (myUseToolTips) {
        showToolTipFor(id);
    }
}


unsigned int
GUISUMOAbstractView::getObjectUnderCursor()
{
    int xpos = myToolTipX+myMouseHotspotX;
    int ypos = myToolTipY+myMouseHotspotY;
    if (xpos<0||xpos>=myWidthInPixels) {
        return 0;
    }
    if (ypos<0||ypos>=myHeightInPixels) {
        return 0;
    }


    const int SENSITIVITY = 4;
    const int NB_HITS_MAX = 1000;
    // Prepare the selection mode
    static GLuint hits[NB_HITS_MAX];
    static GLint nb_hits = 0;
    glSelectBuffer(NB_HITS_MAX, hits);
    glInitNames();
    // compute new scale
    SUMOReal scale = SUMOReal(getMaxGLWidth())/SUMOReal(SENSITIVITY);
    applyChanges(scale, myToolTipX+myMouseHotspotX, myToolTipY+myMouseHotspotY);
    // paint in select mode
    bool tmp = myUseToolTips;
    myUseToolTips = true;
    doPaintGL(GL_SELECT, scale);
    myUseToolTips = tmp;
    // Get the results
    nb_hits = glRenderMode(GL_RENDER);
    if (nb_hits==0) {
        return 0;
    }
    // Interpret results
    unsigned int idMax = 0;
    int prevLayer = -1000;
    for (int i=0; i<nb_hits; ++i) {
        assert(i*4+3<NB_HITS_MAX);
        unsigned int id = hits[i*4+3];
        GUIGlObject *o = gIDStorage.getObjectBlocking(id);
        if (o==0) {
            continue;
        }
        GUIGlObjectType type = o->getType();
        if (type!=0) {
            int clayer = (int) type;
            // determine an "abstract" layer for shapes
            //  this "layer" resembles the layer of the shape
            //  taking into account the stac of other objects
            if (type==GLO_SHAPE) {
                if (dynamic_cast<GUIPolygon2D*>(o)!=0) {
                    if (dynamic_cast<GUIPolygon2D*>(o)->getLayer()>0) {
                        clayer = GLO_MAX + dynamic_cast<GUIPolygon2D*>(o)->getLayer();
                    }
                    if (dynamic_cast<GUIPolygon2D*>(o)->getLayer()<0) {
                        clayer = dynamic_cast<GUIPolygon2D*>(o)->getLayer();
                    }
                }
                if (dynamic_cast<GUIPointOfInterest*>(o)!=0) {
                    if (dynamic_cast<GUIPointOfInterest*>(o)->getLayer()>0) {
                        clayer = GLO_MAX + dynamic_cast<GUIPointOfInterest*>(o)->getLayer();
                    }
                    if (dynamic_cast<GUIPointOfInterest*>(o)->getLayer()<0) {
                        clayer = dynamic_cast<GUIPointOfInterest*>(o)->getLayer();
                    }
                }
            }
            // check whether the current object is above a previous one
            if (prevLayer==-1000||prevLayer<clayer) {
                idMax = id;
                prevLayer = clayer;
            }
        }
        gIDStorage.unblockObject(id);
        assert(i*4+3<NB_HITS_MAX);
    }
    return idMax;
}


void
GUISUMOAbstractView::showToolTipFor(unsigned int id)
{
    if (id!=0) {
        GUIGlObject *object = gIDStorage.getObjectBlocking(id);
        myToolTip->setObjectTip(object, myMouseX, myMouseY);
        if (object!=0) {
            gIDStorage.unblockObject(id);
        }
    } else {
        myToolTip->hide();
    }
}


void
GUISUMOAbstractView::paintGLGrid()
{
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glLineWidth(1);

    SUMOReal xmin = myGrid->getBoundary().xmin();
    SUMOReal ymin = myGrid->getBoundary().ymin();
    SUMOReal ypos = ymin;
    SUMOReal xpos = xmin;
    SUMOReal xend = myGrid->getBoundary().xmax();//(myGrid->getNoXCells()) * myGrid->getXCellSize() + myGrid->getBoundary().xmin();
    SUMOReal yend = myGrid->getBoundary().ymax();//(myGrid->getNoYCells()) * myGrid->getYCellSize() + myGrid->getBoundary().ymin();

    /*
    // draw boxes
    {
        glBegin(GL_QUADS);
        SUMOReal yb = ymin;
        for (int yr=0; yr<myGrid->getNoYCells(); yr++) {
            SUMOReal ye = yb + myVisualizationSettings->gridYSize;
            SUMOReal xb = xmin;
            for (int xr=0; xr<myGrid->getNoXCells(); xr++) {
                SUMOReal xe = xb + myVisualizationSettings->gridXSize;
                switch (myGrid->getPaintState(xr, yr)) {
                case GUIGrid::GPS_NOT_DRAWN:
                    continue;
                case GUIGrid::GPS_FULL_DRAWN:
                    glColor3f((SUMOReal) 0.7, (SUMOReal) 0.7, (SUMOReal) 0.7);
                    break;
                case GUIGrid::GPS_ADD_DRAWN:
                    glColor3f((SUMOReal) 0.8, (SUMOReal) 0.8, (SUMOReal) 0.8);
                    break;
                default:
                    throw 1;
                }
                glVertex2d(xb+1, yb+1);
                glVertex2d(xe-1, yb+1);
                glVertex2d(xe-1, ye-1);
                glVertex2d(xb+1, ye-1);
                xb = xe;
            }
            yb = ye;
        }
        glEnd();
    }
    */
    // draw horizontal lines
    glColor3f(0.5, 0.5, 0.5);
    {
        glBegin(GL_LINES);
        for (; ypos<yend;) {
            glVertex2d(xmin, ypos);
            glVertex2d(xend, ypos);
            ypos += myVisualizationSettings->gridYSize;
        }
        // draw vertical lines
        for (; xpos<xend;) {
            glVertex2d(xpos, ymin);
            glVertex2d(xpos, yend);
            xpos += myVisualizationSettings->gridXSize;
        }
        glEnd();
    }
}


void
GUISUMOAbstractView::applyChanges(SUMOReal scale, size_t xoff, size_t yoff)
{
    myWidthInPixels = getWidth();
    myHeightInPixels = getHeight();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // rotate first;
    //  This is quite unchecked, so it's meaning and position is quite
    //  unclear
    glRotated(myChanger->getRotation(), 0, 0, 1);
    // Fit the view's size to the size of the net
    glScaled(2.0/myNetScale, 2.0/myNetScale, 0);
    // apply ratio between window width and height
    glScaled(1/myRatio, 1, 0);
    // initially (zoom=100), the net shall be completely visible on the screen
    SUMOReal xs = ((SUMOReal) myWidthInPixels / (SUMOReal) myApp->getMaxGLWidth())
                  / (myGrid->getBoundary().getWidth() / myNetScale) * myRatio;
    SUMOReal ys = ((SUMOReal) myHeightInPixels / (SUMOReal) myApp->getMaxGLHeight())
                  / (myGrid->getBoundary().getHeight() / myNetScale);
    if (xs<ys) {
        glScaled(xs, xs, 1);
        myAddScl = xs;
    } else {
        glScaled(ys, ys, 1);
        myAddScl = ys;
    }
    // initially, leave some room for the net
    glScaled((SUMOReal) 0.97, (SUMOReal) 0.97, (SUMOReal) 1);
    myAddScl *= (SUMOReal) .97;

    // Apply the zoom and the scale
    SUMOReal zoom = (SUMOReal)(myChanger->getZoom() / 100.0 * scale);
    glScaled(zoom, zoom, 0);
    // Translate to the middle of the net
    Position2D center = myGrid->getBoundary().getCenter();
    glTranslated(-center.x(), -center.y(), 0);
    // Translate in dependence to the view position applied by the user
    glTranslated(myChanger->getXPos(), myChanger->getYPos(), 0);
    // Translate to the mouse pointer, when wished
    if (xoff!=0||yoff!=0) {
        SUMOReal absX = (SUMOReal)(xoff-((SUMOReal) myWidthInPixels/2.0));
        SUMOReal absY = (SUMOReal)(yoff-((SUMOReal) myHeightInPixels/2.0));
        glTranslated(-p2m(absX), p2m(absY), 0);
    }
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // apply the widow size
    SUMOReal xf = (SUMOReal) -1.0 *
                  ((SUMOReal) myApp->getMaxGLWidth() - (SUMOReal) myWidthInPixels)
                  / (SUMOReal) myApp->getMaxGLWidth();
    SUMOReal yf = (SUMOReal) -1.0 *
                  ((SUMOReal) myApp->getMaxGLHeight() - (SUMOReal) myHeightInPixels)
                  / (SUMOReal) myApp->getMaxGLHeight();
    glTranslated(xf, yf, 0);
    myChanger->applied();
}


void
GUISUMOAbstractView::displayLegend(bool /*flip !!!*/)
{
    // compute the scale bar length
    size_t length = 1;
    const string text("10000000000");
    size_t noDigits = 1;
    size_t pixelSize = 0;
    while (true) {
        pixelSize = (size_t) m2p((SUMOReal) length);
        if (pixelSize>20) {
            break;
        }
        length *= 10;
        noDigits++;
    }
    SUMOReal lineWidth = 1.0;
    glLineWidth((SUMOReal) lineWidth);

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
    glDisable(GL_DEPTH_TEST);

    SUMOReal len = (SUMOReal) pixelSize / (SUMOReal)(myApp->getMaxGLWidth()-1) * (SUMOReal) 2.0;
    glColor3f(0, 0, 0);
    glBegin(GL_LINES);
    // vertical
    glVertex2d(-.98, -.98);
    glVertex2d(-.98+len, -.98);
    // tick at begin
    glVertex2d(-.98, -.98);
    glVertex2d(-.98, -.97);
    // tick at end
    glVertex2d(-.98+len, -.98);
    glVertex2d(-.98+len, -.97);
    glEnd();

    pfSetPosition(-0.99f, 0.96f);
    pfSetScale(0.03f);
    glRotated(180, 1, 0, 0);
    pfDrawString("0m");
    glRotated(-180, 1, 0, 0);

    pfSetPosition((SUMOReal)(-.99+len), .96f);
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
GUISUMOAbstractView::m2p(SUMOReal meter)
{
    return (SUMOReal)
           (meter/myNetScale
            *(myApp->getMaxGLWidth()/myRatio)
            * myAddScl * myChanger->getZoom() / (SUMOReal) 100.0);
}


SUMOReal
GUISUMOAbstractView::p2m(SUMOReal pixel)
{
    return (SUMOReal)
           pixel * myNetScale /
           ((myApp->getMaxGLWidth()/myRatio) * myAddScl *myChanger->getZoom() / (SUMOReal) 100.0);
}


void
GUISUMOAbstractView::recenterView()
{
    myChanger->recenterView();
}


void
GUISUMOAbstractView::centerTo(GUIGlObject *o)
{
    centerTo(o->getCenteringBoundary());
    myChanger->otherChange();
    update();
}


void
GUISUMOAbstractView::centerTo(Boundary bound)
{
    myChanger->centerTo(myGrid->getBoundary(), bound);
}

/*
bool
GUISUMOAbstractView::allowRotation() const
{
    return myParent->allowRotation();
}
*/

void
GUISUMOAbstractView::setTooltipPosition(FXint x, FXint y,
                                        FXint mouseX, FXint mouseY)
{
    myToolTipX = x;
    myToolTipY = y;
    myMouseX = mouseX;
    myMouseY = mouseY;
}


FXbool
GUISUMOAbstractView::makeCurrent()
{
    FXbool ret = FXGLCanvas::makeCurrent();
    return ret;
}



int
GUISUMOAbstractView::getMaxGLWidth() const
{
    return myApp->getMaxGLWidth();
}

int
GUISUMOAbstractView::getMaxGLHeight() const
{
    return myApp->getMaxGLHeight();
}


void
GUISUMOAbstractView::clearUsetable(size_t *edges2Show,
                                   size_t edges2ShowSize)
{
    memset(edges2Show, 0, sizeof(size_t)*edges2ShowSize);
}


long
GUISUMOAbstractView::onConfigure(FXObject*,FXSelector,void*)
{
    if (makeCurrent()) {
        myWidthInPixels = myApp->getMaxGLWidth();
        myHeightInPixels = myApp->getMaxGLHeight();
        myRatio = (SUMOReal) myWidthInPixels / (SUMOReal) myHeightInPixels;
        glViewport(0, 0, myApp->getMaxGLWidth()-1, myApp->getMaxGLHeight()-1);
        glClearColor(
            myVisualizationSettings->backgroundColor.red(),
            myVisualizationSettings->backgroundColor.green(),
            myVisualizationSettings->backgroundColor.blue(),
            1);
        myChanger->applyCanvasSize(width, height);
        doInit();
        myAmInitialised = true;
        makeNonCurrent();
    }
    return 1;
}


long
GUISUMOAbstractView::onPaint(FXObject*,FXSelector,void*)
{
    if (!isEnabled()||!myAmInitialised) {
        return 1;
    }
    if (makeCurrent()) {
        paintGL();
        makeNonCurrent();
    }
    return 1;
}


long
GUISUMOAbstractView::onLeftBtnPress(FXObject *,FXSelector ,void *data)
{
    delete myPopup;
    myPopup = 0;
    FXEvent *e = (FXEvent*) data;
    // check whether the selection-mode is activated
    if (e->state&ALTMASK) {
        // try to get the object-id if so
        if (makeCurrent()) {
            unsigned int id = getObjectUnderCursor();
            if (id!=0) {
                gSelected.toggleSelection(id);
            }
            makeNonCurrent();
            if (id!=0) {
                // possibly, the selection-colouring is used,
                //  so we should update the screen again...
                update();
            }
        }
    }
    myChanger->onLeftBtnPress(data);
    grab();
    return 1;
}


long
GUISUMOAbstractView::onLeftBtnRelease(FXObject *,FXSelector ,void *data)
{
    delete myPopup;
    myPopup = 0;
    myChanger->onLeftBtnRelease(data);
    ungrab();
    return 1;
}


long
GUISUMOAbstractView::onRightBtnPress(FXObject *,FXSelector ,void *data)
{
    delete myPopup;
    myPopup = 0;
    myChanger->onRightBtnPress(data);
    grab();
    return 1;
}


long
GUISUMOAbstractView::onRightBtnRelease(FXObject *,FXSelector ,void *data)
{
    delete myPopup;
    myPopup = 0;
    if (myChanger->onRightBtnRelease(data)) {
        openObjectDialog();
    }
    ungrab();
    return 1;
}


long
GUISUMOAbstractView::onMouseMove(FXObject *,FXSelector ,void *data)
{
    SUMOReal xpos = myChanger->getXPos();
    SUMOReal ypos = myChanger->getYPos();
    SUMOReal zoom = myChanger->getZoom();
    if (myViewportChooser==0||!myViewportChooser->haveGrabbed()) {
        myChanger->onMouseMove(data);
    }
    if (myViewportChooser!=0 &&
            (xpos!=myChanger->getXPos()||ypos!=myChanger->getYPos()||zoom!=myChanger->getZoom())) {

        myViewportChooser->setValues(
            myChanger->getZoom(), myChanger->getXPos(), myChanger->getYPos());

    }
    updatePositionInformation();
    return 1;
}


long
GUISUMOAbstractView::onMouseLeft(FXObject *,FXSelector ,void *data)
{
    if (myViewportChooser==0||!myViewportChooser->haveGrabbed()) {
//        myChanger->onMouseLeft();
        myToolTip->setObjectTip(0, -1, -1);
    }
    return 1;
}


void
GUISUMOAbstractView::openObjectDialog()
{
    ungrab();
    if (!isEnabled()||!myAmInitialised) {
        return;
    }
    if (makeCurrent()) {
        // initialise the select mode
        unsigned int id = getObjectUnderCursor();
        GUIGlObject *o = 0;
        if (id!=0) {
            o = gIDStorage.getObjectBlocking(id);
        } else {
            o = gNetWrapper;
        }
        if (o!=0) {
            myPopup = o->getPopUpMenu(*myApp, *this);
            myPopup->setX(myMouseX);
            myPopup->setY(myMouseY);
            myPopup->create();
            myPopup->show();
            myChanger->onRightBtnRelease(0);
        }
        makeNonCurrent();
    }
}


long
GUISUMOAbstractView::onKeyPress(FXObject *o,FXSelector sel,void *data)
{
    FXEvent *e = (FXEvent*) data;
    if ((e->state&ALTMASK)!=0) {
        setDefaultCursor(getApp()->getDefaultCursor(DEF_CROSSHAIR_CURSOR));
        grabKeyboard();
    }
    return FXGLCanvas::onKeyPress(o, sel, data);
}


long
GUISUMOAbstractView::onKeyRelease(FXObject *o,FXSelector sel,void *data)
{
    FXEvent *e = (FXEvent*) data;
    if ((e->state&ALTMASK)==0) {
        ungrabKeyboard();
        setDefaultCursor(getApp()->getDefaultCursor(DEF_ARROW_CURSOR));
    }
    return FXGLCanvas::onKeyRelease(o, sel, data);
}


/*
long
GUISUMOAbstractView::onCmdShowGrid(FXObject*sender,FXSelector,void*)
{
    MFXCheckableButton *button = static_cast<MFXCheckableButton*>(sender);
    button->setChecked(!button->amChecked());
    myShowGrid = button->amChecked();
    update();
    return 1;
}
*/

long
GUISUMOAbstractView::onSimStep(FXObject*,FXSelector,void*)
{
    update();
    return 1;
}

void APIENTRY beginCallback(GLenum which)
{
    glBegin(which);
}

void APIENTRY errorCallback(GLenum errorCode)
{
    const GLubyte *estring;

    estring = gluErrorString(errorCode);
    fprintf(stderr, "Tessellation Error: %s\n", estring);
    exit(0);
}

void APIENTRY endCallback(void)
{
    glEnd();
}

void APIENTRY vertexCallback(GLvoid *vertex)
{
    const GLdouble *pointer;

    pointer = (GLdouble *) vertex;
    glVertex3dv((GLdouble *) vertex);
}

/*  combineCallback is used to create a new vertex when edges
 *  intersect.  coordinate location is trivial to calculate,
 *  but weight[4] may be used to average color, normal, or texture
 *  coordinate data.  In this program, color is weighted.
 */
void APIENTRY combineCallback(GLdouble coords[3],
                              GLdouble *vertex_data[4],
                              GLfloat weight[4], GLdouble **dataOut)
{
    GLdouble *vertex;

    vertex = (GLdouble *) malloc(7 * sizeof(GLdouble));

    vertex[0] = coords[0];
    vertex[1] = coords[1];
    vertex[2] = coords[2];
    *dataOut = vertex;
}

double glvert[6];

void
GUISUMOAbstractView::drawPolygon2D(const Polygon2D &polygon) const
{
    if (polygon.fill()) {
        if (polygon.getPosition2DVector().size()<3) {
            return;
        }
    } else {
        if (polygon.getPosition2DVector().size()<2) {
            return;
        }
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    myPolyDrawLock.lock();
    if (myUseToolTips) {
        glPushName(static_cast<const GUIPolygon2D&>(polygon).getGlID());
    }
    RGBColor color = polygon.getColor();
    glColor3d(color.red(), color.green(), color.blue());
    if (polygon.fill()) {
        double *points = new double[polygon.getPosition2DVector().size()*3];
        GLUtesselator *tobj = gluNewTess();
        gluTessCallback(tobj, GLU_TESS_VERTEX, (GLvoid(APIENTRY*)()) &glVertex3dv);
        gluTessCallback(tobj, GLU_TESS_BEGIN, (GLvoid(APIENTRY*)()) &beginCallback);
        gluTessCallback(tobj, GLU_TESS_END, (GLvoid(APIENTRY*)()) &endCallback);
        //gluTessCallback(tobj, GLU_TESS_ERROR, (GLvoid (APIENTRY*) ()) &errorCallback);
        gluTessCallback(tobj, GLU_TESS_COMBINE, (GLvoid(APIENTRY*)()) &combineCallback);
        gluTessProperty(tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);
        gluTessBeginPolygon(tobj, NULL);
        gluTessBeginContour(tobj);
        for (size_t i=0; i!=polygon.getPosition2DVector().size(); ++i) {
            points[3*i]  = polygon.getPosition2DVector()[(int) i].x();
            points[3*i+1]  = polygon.getPosition2DVector()[(int) i].y();
            points[3*i+2]  = 0;
            glvert[0] = polygon.getPosition2DVector()[(int) i].x();
            glvert[1] = polygon.getPosition2DVector()[(int) i].y();
            glvert[2] = 0;
            glvert[3] = 1;
            glvert[4] = 1;
            glvert[5] = 1;
            gluTessVertex(tobj, points+3*i, points+3*i) ;
        }
        gluTessEndContour(tobj);

        gluTessEndPolygon(tobj);
        gluDeleteTess(tobj);
        delete[] points;
    } else {
        GLHelper::drawBoxLines(polygon.getPosition2DVector(), 1.);
    }

    if (myUseToolTips) {
        glPopName();
    }
    myPolyDrawLock.unlock();
}


void
GUISUMOAbstractView::drawPOI2D(const PointOfInterest &p, SUMOReal width) const
{
    if (myUseToolTips) {
        glPushName(static_cast<const GUIPointOfInterest&>(p).getGlID());
    }
    glColor3d(p.red(),p.green(),p.blue());
    glTranslated(p.x(), p.y(), 0);
    GLHelper::drawFilledCircle((SUMOReal) 1.3*myVisualizationSettings->poiExaggeration, 16);
    if (myVisualizationSettings->drawPOIName) {
        glColor3f(myVisualizationSettings->poiNameColor.red(), myVisualizationSettings->poiNameColor.green(), myVisualizationSettings->poiNameColor.blue());
        glPushMatrix();
        glTranslated((SUMOReal) 1.32*myVisualizationSettings->poiExaggeration, (SUMOReal) 1.32*myVisualizationSettings->poiExaggeration, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        pfSetPosition(0, 0);
        pfSetScale(myVisualizationSettings->poiNameSize / width);
        glRotated(180, 1, 0, 0);
        pfDrawString(static_cast<const GUIPointOfInterest&>(p).getID().c_str());
        glPopMatrix();
    }
    glTranslated(-p.x(), -p.y(), 0);
    if (myUseToolTips) {
        glPopName();
    }
}


FXColor *
GUISUMOAbstractView::getSnapshot()
{
    makeCurrent();
    // draw
    // draw
    glClearColor(
        myVisualizationSettings->backgroundColor.red(),
        myVisualizationSettings->backgroundColor.green(),
        myVisualizationSettings->backgroundColor.blue(),
        1);
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

    applyChanges(1.0, 0, 0);
    if (myVisualizationSettings->showGrid) {
        paintGLGrid();
    }
    doPaintGL(GL_RENDER, 1.0);
    if (myVisualizationSettings->showSizeLegend) {
        displayLegend();
    }

    glFlush();
    swapBuffers();
    glFinish();
    FXColor *buf;
    FXMALLOC(&buf, FXColor, getWidth()*getHeight());
    // read from the back buffer
    glReadBuffer(GL_BACK);
    // Read the pixels
    glReadPixels(0, 0, getWidth(), getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)buf);
    // !!! flip orientation
    makeNonCurrent();
    update();
    // mirror
    size_t mwidth = getWidth();
    size_t mheight = getHeight();
    FXColor *paa = buf;
    FXColor *pbb = buf + mwidth * (mheight-1);
    do {
        FXColor *pa=paa;
        paa+=mwidth;
        FXColor *pb=pbb;
        pbb-=mwidth;
        do {
            FXColor t=*pa;
            *pa++=*pb;
            *pb++=t;
        } while (pa<paa);
    } while (paa<pbb);
    return buf;
}


void
GUISUMOAbstractView::showViewportEditor()
{
    if (myViewportChooser==0) {
        myViewportChooser =
            new GUIDialog_EditViewport(this, "Edit Viewport...",
                                       myChanger->getZoom(), myChanger->getXPos(), myChanger->getYPos(),
                                       0, 0);
        myViewportChooser->create();
    }
    myViewportChooser->setOldValues(
        myChanger->getZoom(), myChanger->getXPos(), myChanger->getYPos());
    myViewportChooser->show();
}


void
GUISUMOAbstractView::setViewport(SUMOReal zoom, SUMOReal xPos, SUMOReal yPos)
{
    myChanger->setViewport(zoom, xPos, yPos);
    myChanger->otherChange();
    update();
}


void
GUISUMOAbstractView::showToolTips(bool val)
{
    myUseToolTips = val;
}


void
GUISUMOAbstractView::drawShapesLayer(const ShapeContainer &sc, int layer, SUMOReal width)
{
    const std::map<std::string, Polygon2D*> &pol = sc.getPolygonCont(layer).getMyMap();
    for (std::map<std::string, Polygon2D*>::const_iterator i=pol.begin(); i!=pol.end(); ++i) {
        drawPolygon2D(*((*i).second));
    }
    const std::map<std::string, PointOfInterest*> &poi = sc.getPOICont(layer).getMyMap();
    for (std::map<std::string, PointOfInterest*>::const_iterator i=poi.begin(); i!=poi.end(); ++i) {
        drawPOI2D(*((*i).second), width);
    }
}


void
GUISUMOAbstractView::drawShapes(const ShapeContainer &sc, int maxLayer, SUMOReal width)
{
    if (maxLayer<=0&&sc.getMinLayer()<=0) {
        for (int i=sc.getMinLayer(); i<=0; i++) {
            drawShapesLayer(sc, i, width);
        }
    }
    if (maxLayer>0&&sc.getMaxLayer()>0) {
        for (int i=0; i<=sc.getMaxLayer(); i++) {
            drawShapesLayer(sc, i, width);
        }
    }
}


SUMOReal
GUISUMOAbstractView::getGridWidth() const
{
    return myGrid->getBoundary().getWidth();
}


SUMOReal
GUISUMOAbstractView::getGridHeight() const
{
    return myGrid->getBoundary().getHeight();
}


void
GUISUMOAbstractView::rename(GUIGlObject *)
{}

void
GUISUMOAbstractView::moveTo(GUIGlObject *)
{}

void
GUISUMOAbstractView::changeCol(GUIGlObject *)
{}

void
GUISUMOAbstractView::changeTyp(GUIGlObject *)
{}

void
GUISUMOAbstractView::deleteObj(GUIGlObject *)
{}


FXComboBox &
GUISUMOAbstractView::getColoringSchemesCombo()
{
    return myParent->getColoringSchemesCombo();
}


bool
GUISUMOAbstractView::VisualizationSettings::operator==(const GUISUMOAbstractView::VisualizationSettings &v2)
{
    if (antialiase!=v2.antialiase) return false;
    if (dither!=v2.dither) return false;
    if (backgroundColor!=v2.backgroundColor) return false;
    if (showBackgroundDecals!=v2.showBackgroundDecals) return false;
    if (showGrid!=v2.showGrid) return false;
    if (gridXSize!=v2.gridXSize) return false;
    if (gridYSize!=v2.gridYSize) return false;

    if (laneEdgeMode!=v2.laneEdgeMode) return false;
    if (laneColorings!=v2.laneColorings) return false;
    if (laneShowBorders!=v2.laneShowBorders) return false;
    if (showLinkDecals!=v2.showLinkDecals) return false;
    if (laneEdgeExaggMode!=v2.laneEdgeExaggMode) return false;
    if (minExagg!=v2.minExagg) return false;
    if (maxExagg!=v2.maxExagg) return false;
    if (showRails!=v2.showRails) return false;
    if (drawEdgeName!=v2.drawEdgeName) return false;
    if (edgeNameSize!=v2.edgeNameSize) return false;
    if (edgeNameColor!=v2.edgeNameColor) return false;

    if (vehicleMode!=v2.vehicleMode) return false;
    if (minVehicleSize!=v2.minVehicleSize) return false;
    if (vehicleExaggeration!=v2.vehicleExaggeration) return false;
    if (vehicleColorings!=v2.vehicleColorings) return false;
    if (showBlinker!=v2.showBlinker) return false;
    if (drawcC2CRadius!=v2.drawcC2CRadius) return false;
    if (drawLaneChangePreference!=v2.drawLaneChangePreference) return false;
    if (drawVehicleName!=v2.drawVehicleName) return false;
    if (vehicleNameSize!=v2.vehicleNameSize) return false;
    if (vehicleNameColor!=v2.vehicleNameColor) return false;

    if (junctionMode!=v2.junctionMode) return false;
    if (drawLinkTLIndex!=v2.drawLinkTLIndex) return false;
    if (drawLinkJunctionIndex!=v2.drawLinkJunctionIndex) return false;
    if (drawJunctionName!=v2.drawJunctionName) return false;
    if (junctionNameSize!=v2.junctionNameSize) return false;
    if (junctionNameColor!=v2.junctionNameColor) return false;

    if (showLane2Lane!=v2.showLane2Lane) return false;

    if (addMode!=v2.addMode) return false;
    if (minAddSize!=v2.minAddSize) return false;
    if (addExaggeration!=v2.addExaggeration) return false;
    if (drawAddName!=v2.drawAddName) return false;
    if (addNameSize!=v2.addNameSize) return false;

    if (minPOISize!=v2.minPOISize) return false;
    if (poiExaggeration!=v2.poiExaggeration) return false;
    if (drawPOIName!=v2.drawPOIName) return false;
    if (poiNameSize!=v2.poiNameSize) return false;
    if (poiNameColor!=v2.poiNameColor) return false;

    if (showSizeLegend!=v2.showSizeLegend) return false;

    return true;
}



/****************************************************************************/

