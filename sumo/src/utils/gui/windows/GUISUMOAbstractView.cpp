//---------------------------------------------------------------------------//
//                        GUISUMOAbstractView.cpp -
//  The base class for a view
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.4  2005/05/04 09:24:43  dkrajzew
// entries for viewport definition added; popups now popup faster
//
// Revision 1.3  2004/12/15 09:20:19  dkrajzew
// made guisim independent of giant/netedit
//
// Revision 1.2  2004/12/12 17:23:59  agaubatz
// Editor Tool Widgets included
//
// Revision 1.1  2004/11/23 10:38:32  dkrajzew
// debugging
//
// Revision 1.2  2004/10/29 06:01:56  dksumo
// renamed boundery to boundary
//
// Revision 1.1  2004/10/22 12:50:57  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.21  2004/08/02 11:54:52  dkrajzew
// added the possibility to take snapshots
//
// Revision 1.20  2004/07/02 08:32:10  dkrajzew
// changes due to the global object selection applied; some debugging (on zoom)
//
// Revision 1.19  2004/06/17 13:06:55  dkrajzew
// Polygon visualisation added
//
// Revision 1.18  2004/04/02 11:11:24  dkrajzew
// visualisation whether an item is selected added
//
// Revision 1.17  2004/03/19 12:54:08  dkrajzew
// porting to FOX
//
// Revision 1.16  2004/02/16 13:55:18  dkrajzew
// tried to patch a sometimes occuring visualisation bug
//
// Revision 1.15  2004/02/05 16:29:30  dkrajzew
// memory leaks removed
//
// Revision 1.14  2003/12/12 12:34:35  dkrajzew
// unwished simulation shutdown bug removed
//
// Revision 1.13  2003/11/12 14:07:46  dkrajzew
// clean up after recent changes
//
// Revision 1.12  2003/11/11 08:40:03  dkrajzew
// consequent position2D instead of two doubles implemented
//
// Revision 1.11  2003/09/23 14:25:13  dkrajzew
// possibility to visualise detectors using different geometry complexities
//  added
//
// Revision 1.10  2003/09/05 14:54:06  dkrajzew
// implementations of artefact drawers moved to folder "drawerimpl"
//
// Revision 1.9  2003/08/15 12:19:16  dkrajzew
// legend display patched
//
// Revision 1.8  2003/07/30 08:52:16  dkrajzew
// further work on visualisation of all geometrical objects
//
// Revision 1.7  2003/07/22 14:56:46  dkrajzew
// changes due to new detector handling
//
// Revision 1.6  2003/07/18 12:29:28  dkrajzew
// removed some warnings
//
// Revision 1.5  2003/07/16 15:18:22  dkrajzew
// new interfaces for drawing classes; junction drawer interface added
//
// Revision 1.4  2003/06/18 11:04:53  dkrajzew
// new error processing adapted
//
// Revision 1.3  2003/06/06 10:28:45  dkrajzew
// new subfolder holding popup-menus was added due to link-dependencies
//  under linux; GUIGLObjectPopupMenu*-classes were moved to "popup"
//
// Revision 1.2  2003/06/05 06:26:16  dkrajzew
// first tries to build under linux: warnings removed; Makefiles added
//
// Revision 1.1  2003/05/20 09:25:13  dkrajzew
// new view hierarchy; some debugging done
//
// Revision 1.12  2003/04/16 09:50:04  dkrajzew
// centering of the network debugged; additional parameter of maximum display
//  size added
//
// Revision 1.11  2003/04/14 08:24:57  dkrajzew
// unneeded display switch and zooming option removed; new glo-objct concept
//  implemented; comments added
//
// Revision 1.10  2003/04/07 10:15:16  dkrajzew
// glut reinserted
//
// Revision 1.9  2003/04/04 15:13:20  roessel
// Commented out #include <glut.h>
// Added #include <qcursor.h>
//
// Revision 1.8  2003/04/04 08:37:51  dkrajzew
// view centering now applies net size; closing problems debugged;
//  comments added; tootip button added
//
// Revision 1.7  2003/04/02 11:50:28  dkrajzew
// a working tool tip implemented
//
// Revision 1.6  2003/03/20 16:17:52  dkrajzew
// windows eol removed
//
// Revision 1.5  2003/03/12 16:55:19  dkrajzew
// centering of objects debugged
//
// Revision 1.3  2003/03/03 15:10:20  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:34:14  dkrajzew
// files updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <iostream>
#include <utility>
#include <cmath>
#include <cassert>
#include <utils/gfx/RGBColor.h>
#include <utils/convert/ToString.h>
#include <utils/gui/globjects/GUIGLObjectToolTip.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include "GUIDanielPerspectiveChanger.h"
#include "GUISUMOAbstractView.h"
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/foxtools/MFXCheckableButton.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/glutils/GLHelper.h>
#include "GUIMainWindow.h"
#include "GUIGlChildWindow.h"
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include "GUIDialog_EditViewport.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * some definitions (debugging only)
 * ======================================================================= */
#define DEBUG_OUT cout


/* =========================================================================
 * member method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * GUISUMOAbstractView::ViewSettings - methods
 * ----------------------------------------------------------------------- */
GUISUMOAbstractView::ViewSettings::ViewSettings()
    : myX(-1), myY(-1), myXOff(-1), myYOff(-1)
{
}


GUISUMOAbstractView::ViewSettings::ViewSettings(double x, double y,
                                                double xoff, double yoff)
    : myX(x), myY(y), myXOff(xoff), myYOff(yoff)
{
}


GUISUMOAbstractView::ViewSettings::~ViewSettings()
{
}


bool
GUISUMOAbstractView::ViewSettings::differ(double x, double y,
                                          double xoff, double yoff)
{
    return myX!=x || myY!=y || myXOff!=xoff || myXOff!=yoff;
}


void
GUISUMOAbstractView::ViewSettings::set(double x, double y,
                                       double xoff, double yoff)
{
    myX = x;
    myY = y;
    myXOff = xoff;
    myYOff = yoff;
}


/* -------------------------------------------------------------------------
 * GUISUMOAbstractView - FOX callback mapping
 * ----------------------------------------------------------------------- */
FXDEFMAP(GUISUMOAbstractView) GUISUMOAbstractViewMap[]={
    FXMAPFUNC(SEL_CONFIGURE,           0,                 GUISUMOAbstractView::onConfigure),
    FXMAPFUNC(SEL_PAINT,               0,                 GUISUMOAbstractView::onPaint),
    FXMAPFUNC(SEL_LEFTBUTTONPRESS,     0,                 GUISUMOAbstractView::onLeftBtnPress),
    FXMAPFUNC(SEL_LEFTBUTTONRELEASE,   0,                 GUISUMOAbstractView::onLeftBtnRelease),
    FXMAPFUNC(SEL_RIGHTBUTTONPRESS,    0,                 GUISUMOAbstractView::onRightBtnPress),
    FXMAPFUNC(SEL_RIGHTBUTTONRELEASE,  0,                 GUISUMOAbstractView::onRightBtnRelease),
    FXMAPFUNC(SEL_MOTION,              0,                 GUISUMOAbstractView::onMouseMove),
//    FXMAPFUNC(SEL_TIMEOUT,             ID_RMOUSETIMEOUT,  GUISUMOAbstractView::onRightMouseTimeOut),
    FXMAPFUNC(SEL_COMMAND,             MID_SIMSTEP,       GUISUMOAbstractView::onSimStep),
    FXMAPFUNC(SEL_KEYPRESS,            0,                 GUISUMOAbstractView::onKeyPress),
    FXMAPFUNC(SEL_KEYRELEASE,          0,                 GUISUMOAbstractView::onKeyRelease),
    FXMAPFUNC(SEL_COMMAND,             MID_EDITVIEWPORT,  GUISUMOAbstractView::onCmdEditViewport),

};

  // Macro for the GLTestApp class hierarchy implementation
FXIMPLEMENT(GUISUMOAbstractView,FXGLCanvas,GUISUMOAbstractViewMap,ARRAYNUMBER(GUISUMOAbstractViewMap))


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
    _parent(parent),
    myGrid(&((GUIGrid&) grid)),
    _showGrid(false),
    _changer(0),
    _useToolTips(false),
    _mouseHotspotX(app.getDefaultCursor()->getHotX()),
    _mouseHotspotY(app.getDefaultCursor()->getHotY()),
    _popup(0),
    myAmInitialised(false)
{
    flags|=FLAG_ENABLED;
	_inEditMode=false;
    // compute the net scale
    double nw = myGrid->getBoundary().getWidth();
    double nh = myGrid->getBoundary().getHeight();
    myNetScale = (nw < nh ? nh : nw);
    // show the middle at the beginning
    _changer = new GUIDanielPerspectiveChanger(*this);
    _changer->setNetSizes((size_t) nw, (size_t) nh);
    _toolTip = new GUIGLObjectToolTip(myApp);
}


GUISUMOAbstractView::GUISUMOAbstractView(FXComposite *p,
                                         GUIMainWindow &app,
                                         GUIGlChildWindow *parent,
                                         const GUIGrid &grid,
                                         FXGLVisual *glVis, FXGLCanvas *share)
    : FXGLCanvas(p, glVis, share, p, MID_GLCANVAS,
        LAYOUT_SIDE_TOP|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0, 0, 300, 200),
    myApp(&app),
    _parent(parent),
    myGrid(&((GUIGrid&) grid)),
    _showGrid(false),
    _changer(0),
    _useToolTips(false),
    _mouseHotspotX(app.getDefaultCursor()->getHotX()),
    _mouseHotspotY(app.getDefaultCursor()->getHotY()),
    _popup(0),
    myAmInitialised(false)
{
    flags|=FLAG_ENABLED;
	_inEditMode=false;
    // compute the net scale
    double nw = myGrid->getBoundary().getWidth();
    double nh = myGrid->getBoundary().getHeight();
    myNetScale = (nw < nh ? nh : nw);
    // show the middle at the beginning
    _changer = new GUIDanielPerspectiveChanger(*this);
    _changer->setNetSizes((size_t) nw, (size_t) nh);
    _toolTip = new GUIGLObjectToolTip(myApp);
}


GUISUMOAbstractView::~GUISUMOAbstractView()
{
//    getApp()->removeTimeout(this, ID_RMOUSETIMEOUT);
    delete _changer;
    delete _toolTip;
    // just to quit cleanly on a failure
    if(_lock.locked()) {
        _lock.unlock();
    }
}


bool
GUISUMOAbstractView::isInEditMode()
{
	return _inEditMode;
}


void
GUISUMOAbstractView::updateToolTip()
{
    if(!_useToolTips) {
        return;
    }
    _lock.lock();
    if(makeCurrent()) {
        // initialise the select mode
        unsigned int id = getObjectUnderCursor();
        showToolTipFor(id);
        makeNonCurrent();
    }
    // mark end-of-drawing
    _lock.unlock();
}


void
GUISUMOAbstractView::paintGL()
{
    _widthInPixels = getWidth();
    _heightInPixels = getHeight();
    if(_widthInPixels==0||_heightInPixels==0) {
        return;
    }
    // draw
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_POLYGON_SMOOTH);

    applyChanges(1.0, 0, 0);
    if(_showGrid) {
        paintGLGrid();
    }
    //


    doPaintGL(GL_RENDER, 1.0);
    if(_parent->showLegend()) {
        displayLegend();
    }
    // check whether the select mode /tooltips)
    //  shall be computed, too
    if(!_useToolTips) {
        glFlush();
        swapBuffers();
        return;
    }

    glFlush();
    swapBuffers();
    // get the object under the cursor
    unsigned int id = getObjectUnderCursor();
    showToolTipFor(id);
    glFlush();
    swapBuffers();
}


unsigned int
GUISUMOAbstractView::getObjectUnderCursor()
{
    const int SENSITIVITY = 4;
    const int NB_HITS_MAX = 1000;
    // Prepare the selection mode
    static GLuint hits[NB_HITS_MAX];
    static GLint nb_hits = 0;
    glSelectBuffer(NB_HITS_MAX, hits);
    glInitNames();
    // compute new scale
    double scale = double(getMaxGLWidth())/double(SENSITIVITY);
    applyChanges(scale, _toolTipX+_mouseHotspotX,
        _toolTipY+_mouseHotspotY);
    // paint in select mode
    bool tmp = _useToolTips;
    _useToolTips = true;
    doPaintGL(GL_SELECT, scale);
    _useToolTips = tmp;
    // Get the results
    nb_hits = glRenderMode(GL_RENDER);
    if(nb_hits==0) {
        return 0;
    }
    // Interpret results
    unsigned int idMax = 0;
    GUIGlObjectType prevType = GLO_MAX;
    for (int i=0; i<nb_hits; ++i) {
        assert (i*4+3<NB_HITS_MAX);
        unsigned int id = hits[i*4+3];
        GUIGlObject *o = gIDStorage.getObjectBlocking(id);
        if(o==0) {
            continue;
        }
        GUIGlObjectType type = o->getType();
        if((prevType>=type||idMax<id)&&type!=0) {
            idMax = id;
            prevType = type;
        }
        gIDStorage.unblockObject(id);
        assert (i*4+3<NB_HITS_MAX);
    }
    return idMax;
}


void
GUISUMOAbstractView::showToolTipFor(unsigned int id)
{
    if(id!=0) {
        GUIGlObject *object = gIDStorage.getObjectBlocking(id);
        _toolTip->setObjectTip(object, _mouseX, _mouseY);
        if(object!=0) {
            gIDStorage.unblockObject(id);
        }
    } else {
        _toolTip->hide();
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
    glBegin( GL_LINES );

    glColor3f(0.5, 0.5, 0.5);
    double ypos = 0;
    double xpos = 0;
    double xend = (myGrid->getNoXCells())
        * myGrid->getXCellSize();
    double yend = (myGrid->getNoYCells())
        * myGrid->getYCellSize();
    for(int yr=0; yr<myGrid->getNoYCells()+1; yr++) {
        glVertex2d(0, ypos);
        glVertex2d(xend, ypos);
        ypos += myGrid->getYCellSize();
    }
    for(int xr=0; xr<myGrid->getNoXCells()+1; xr++) {
        glVertex2d(xpos, 0);
        glVertex2d(xpos, yend);
        xpos += myGrid->getXCellSize();
    }
    glEnd();
}


void
GUISUMOAbstractView::applyChanges(double scale, size_t xoff, size_t yoff)
{
    _widthInPixels = getWidth();
    _heightInPixels = getHeight();
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    // rotate first;
    //  This is quite unchecked, so it's meaning and position is quite
    //  unclear
    glRotated(_changer->getRotation(), 0, 0, 1);
    // Fit the view's size to the size of the net
    glScaled(2.0/myNetScale, 2.0/myNetScale, 0);
    // apply ratio between window width and height
    glScaled(1/_ratio, 1, 0);
    // initially (zoom=100), the net shall be completely visible on the screen
    double xs = ((double) _widthInPixels / (double) myApp->getMaxGLWidth())
        / (myGrid->getBoundary().getWidth() / myNetScale) * _ratio;
    double ys = ((double) _heightInPixels / (double) myApp->getMaxGLHeight())
        / (myGrid->getBoundary().getHeight() / myNetScale);
    if(xs<ys) {
        glScaled(xs, xs, 1);
        _addScl = xs;
    } else {
        glScaled(ys, ys, 1);
        _addScl = ys;
    }
    // initially, leave some room for the net
    glScaled(0.97, 0.97, 1);
    _addScl *= .97;

    // Apply the zoom and the scale
    double zoom = _changer->getZoom() / 100.0 * scale;
    glScaled(zoom, zoom, 0);
    // Translate to the middle of the net
    Position2D center = myGrid->getBoundary().getCenter();
    glTranslated(-center.x(), -center.y(), 0);
    // Translate in dependence to the view position applied by the user
    glTranslated(_changer->getXPos(), _changer->getYPos(), 0);
    // Translate to the mouse pointer, when wished
    if(xoff!=0||yoff!=0) {
        double absX = (double(xoff)-(double(_widthInPixels)/2.0));
        double absY = (double(yoff)-(double(_heightInPixels)/2.0));
        glTranslated(-p2m(absX), p2m(absY), 0);
    }
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // apply the widow size
    double xf = -1.0 *
        ((double) myApp->getMaxGLWidth() - (double) _widthInPixels)
        / (double) myApp->getMaxGLWidth();
    double yf = -1.0 *
        ((double) myApp->getMaxGLHeight() - (double) _heightInPixels)
        / (double) myApp->getMaxGLHeight();
    glTranslated(xf, yf, 0);
    _changer->applied();
}


void
GUISUMOAbstractView::displayLegend(bool flip)
{
    // compute the scale bar length
    size_t length = 1;
    const string text("10000000000");
    size_t noDigits = 1;
    size_t pixelSize = 0;
    while(true) {
        pixelSize = (size_t) m2p((double) length);
        if(pixelSize>20) {
            break;
        }
        length *= 10;
        noDigits++;
    }
    double lineWidth = 1.0;
    glLineWidth((float) lineWidth);

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

    GUITexturesHelper::getFontRenderer().SetActiveFont("std8");
    GUITexturesHelper::getFontRenderer().SetColor(0, 0, 0);
    double len = (double) pixelSize / (double) (myApp->getMaxGLWidth()-1) * 2.0;
    glColor3f(0, 0, 0);
    glBegin( GL_LINES );
    // vertical
    glVertex2d(-.98, -.98);
    glVertex2d(-.98+len, -.98);
    // tick at begin
    glVertex2d(-.98, -.98);
    glVertex2d(-.98, -.97);
    GUITexturesHelper::getFontRenderer().StringOut(
        (float) 10, (float) (myApp->getMaxGLHeight()-30), "0m");
    // tick at end
    glVertex2d(-.98+len, -.98);
    glVertex2d(-.98+len, -.97);
    GUITexturesHelper::getFontRenderer().StringOut(
        (float) 10+pixelSize, (float) (myApp->getMaxGLHeight()-30),
        text.substr(0, noDigits) + "m");
    glEnd();
    GUITexturesHelper::getFontRenderer().Draw(flip);

    // draw the current position
/*
    std::pair<double, double> c = canvas2World(_mouseX, _mouseY);
    string out = toString<double>(c.first) + ", " + toString<double>(c.second);
    StringOut(getWidth()-100, (double) (myApp->getMaxGLHeight()-20),
        out);
*/

    // restore matrices
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}


std::pair<double, double>
GUISUMOAbstractView::canvas2World(double x, double y)
{
    double xret = p2m(x-(myApp->getMaxGLWidth())/2.0)-_changer->getXPos();
    double yret = p2m(y-(myApp->getMaxGLHeight())/2.0)-_changer->getYPos();
    return std::pair<double, double>(xret, yret);
}


double
GUISUMOAbstractView::m2p(double meter)
{
    return
        (meter/myNetScale
        * (myApp->getMaxGLWidth()/_ratio)
        * _addScl * _changer->getZoom() / 100.0);
}


double
GUISUMOAbstractView::p2m(double pixel)
{
    return
        pixel * myNetScale /
        ((myApp->getMaxGLWidth()/_ratio) * _addScl *_changer->getZoom() / 100.0);
}


void
GUISUMOAbstractView::recenterView()
{
    _changer->recenterView();
}


void
GUISUMOAbstractView::centerTo(GUIGlObject *o)
{
    centerTo(o->getCenteringBoundary());
    _changer->otherChange();
    update();
}


void
GUISUMOAbstractView::centerTo(Boundary bound)
{
    _changer->centerTo(myGrid->getBoundary(), bound);
}


bool
GUISUMOAbstractView::allowRotation() const
{
    return _parent->allowRotation();
}


void
GUISUMOAbstractView::setTooltipPosition(size_t x, size_t y,
                                        size_t mouseX, size_t mouseY)
{
    _toolTipX = x;
    _toolTipY = y;
    _mouseX = mouseX;
    _mouseY = mouseY;
}


FXbool
GUISUMOAbstractView::makeCurrent()
{
    _lock.lock();
    FXbool ret = FXGLCanvas::makeCurrent();
    _lock.unlock();
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
    if(makeCurrent()) {
        _lock.lock();
        _widthInPixels = myApp->getMaxGLWidth();
        _heightInPixels = myApp->getMaxGLHeight();
        _ratio = (double) _widthInPixels / (double) _heightInPixels;
        glViewport( 0, 0, myApp->getMaxGLWidth()-1, myApp->getMaxGLHeight()-1 );
        glClearColor( 1.0, 1.0, 1.0, 1 );
        _changer->applyCanvasSize(width, height);
        doInit();
        myAmInitialised = true;
        makeNonCurrent();
        _lock.unlock();
    }
    return 1;
}


long
GUISUMOAbstractView::onPaint(FXObject*,FXSelector,void*)
{
    if(!isEnabled()||!myAmInitialised) {
        return 1;
    }
    if(_lock.locked()) {
        return 1;
    }
    _lock.lock();
    if(makeCurrent()) {
        paintGL();
        makeNonCurrent();
    }
    _lock.unlock();
    return 1;
}


long
GUISUMOAbstractView::onLeftBtnPress(FXObject *,FXSelector sel,void *data)
{
    delete _popup;
    _popup = 0;
    FXEvent *e = (FXEvent*) data;
    // check whether the selection-mode is activated
    if(e->state&ALTMASK) {
        _lock.lock();
        // try to get the object-id if so
        if(makeCurrent()) {
            unsigned int id = getObjectUnderCursor();
            if(id!=0) {
                gSelected.addObjectChecking(id, e->state&SHIFTMASK);
            }
            makeNonCurrent();
            if(id!=0) {
                // possibly, the selection-colouring is used,
                //  so we should update the screen again...
                update();
            }
        }
        _lock.unlock();
    }
    _changer->onLeftBtnPress(data);
    grab();
    return 1;
}


long
GUISUMOAbstractView::onLeftBtnRelease(FXObject *o,FXSelector sel,void *data)
{
    delete _popup;
    _popup = 0;
    _changer->onLeftBtnRelease(data);
    ungrab();
    return 1;
}


long
GUISUMOAbstractView::onRightBtnPress(FXObject *o,FXSelector sel,void *data)
{
    delete _popup;
    _popup = 0;
    _changer->onRightBtnPress(data);
//    getApp()->addTimeout(this, ID_RMOUSETIMEOUT, 500);
    grab();
    return 1;
}


long
GUISUMOAbstractView::onRightBtnRelease(FXObject *o,FXSelector sel,void *data)
{
    delete _popup;
    _popup = 0;
    if(_changer->onRightBtnRelease(data)) {
        openObjectDialog();
    }
//    getApp()->removeTimeout(this, ID_RMOUSETIMEOUT);
    ungrab();
    return 1;
}


long
GUISUMOAbstractView::onMouseMove(FXObject *o,FXSelector sel,void *data)
{
//    getApp()->removeTimeout(this, ID_RMOUSETIMEOUT);
    _changer->onMouseMove(data);
    return 1;
}


void
GUISUMOAbstractView::openObjectDialog()
{
    ungrab();
//    getApp()->removeTimeout(this, ID_RMOUSETIMEOUT);
    if(!isEnabled()||!myAmInitialised) {
        return;
    }
    _lock.lock();
    if(makeCurrent()) {
        // initialise the select mode
        unsigned int id = getObjectUnderCursor();
        GUIGlObject *o = 0;
        if(id!=0) {
            o = gIDStorage.getObjectBlocking(id);
        } else {
            o = gNetWrapper;
        }
        if(o!=0) {
            _popup = o->getPopUpMenu(*myApp, *this);
            _popup->setX(_mouseX);
            _popup->setY(_mouseY);
            _popup->create();
            _popup->show();
            _changer->onRightBtnRelease(0);
        }
        makeNonCurrent();
    }
    _lock.unlock();
}


long
GUISUMOAbstractView::onKeyPress(FXObject *o,FXSelector sel,void *data)
{
    FXEvent *e = (FXEvent*) data;
    if((e->state&ALTMASK)!=0) {
        setDefaultCursor(getApp()->getDefaultCursor(DEF_CROSSHAIR_CURSOR));
        grabKeyboard();
    }
    return FXGLCanvas::onKeyPress(o, sel, data);
}


long
GUISUMOAbstractView::onKeyRelease(FXObject *o,FXSelector sel,void *data)
{
    FXEvent *e = (FXEvent*) data;
    if((e->state&ALTMASK)==0) {
        ungrabKeyboard();
        setDefaultCursor(getApp()->getDefaultCursor(DEF_ARROW_CURSOR));
    }
    return FXGLCanvas::onKeyRelease(o, sel, data);
}



long
GUISUMOAbstractView::onCmdShowToolTips(FXObject*sender,FXSelector,void*)
{
    MFXCheckableButton *button = static_cast<MFXCheckableButton*>(sender);
    button->setChecked(!button->amChecked());
    _useToolTips = button->amChecked();
    update();
    return 1;
}


long
GUISUMOAbstractView::onCmdShowGrid(FXObject*sender,FXSelector,void*)
{
    MFXCheckableButton *button = static_cast<MFXCheckableButton*>(sender);
    button->setChecked(!button->amChecked());
    _showGrid = button->amChecked();
    update();
    return 1;
}


long
GUISUMOAbstractView::onSimStep(FXObject*sender,FXSelector,void*)
{
    update();
    return 1;
}


void
GUISUMOAbstractView::drawPolygon2D(Polygon2D &polygon)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_POLYGON);

    RGBColor color = polygon.getColor();
    glColor3d(color.red(),color.green(),color.blue());
    Position2DVector position2dV = polygon.getPosition2DVector();

    const Position2DVector::ContType &l = position2dV.getCont();
    for(Position2DVector::ContType ::const_iterator i=l.begin(); i!=l.end(); i++) {
        const Position2D &p = *i;
        glVertex2d(p.x(), p.y());
    }

    glEnd();
}


FXColor *
GUISUMOAbstractView::getSnapshot()
{
    makeCurrent();
    // draw
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_POLYGON_SMOOTH);
    applyChanges(1.0, 0, 0);
    glScaled(1.0, -1.0, 1);
    if(_showGrid) {
        paintGLGrid();
    }
    doPaintGL(GL_RENDER, 1.0);

    glFlush();
    swapBuffers();
    glFinish();
    FXColor *buf;
    FXMALLOC(&buf, FXColor, getWidth()*getHeight());
    // read from the back buffer
    glReadBuffer(GL_BACK);
    // Read the pixels
    glReadPixels(0, 0, getWidth(), getHeight(),
        GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)buf);
    makeNonCurrent();
    update();
    return buf;
}


FXPopup *
GUISUMOAbstractView::getLocatorPopup(GUIGlChildWindow &p)
{
    return myLocatorPopup;
}


long
GUISUMOAbstractView::onCmdEditViewport(FXObject*,FXSelector,void*)
{
    GUIDialog_EditViewport *chooser =
        new GUIDialog_EditViewport(this, "Edit Viewport...",
        _changer->getZoom(), _changer->getXPos(), _changer->getYPos(),
        0, 0);
    chooser->create();
    chooser->show();
    return 1;
}


void
GUISUMOAbstractView::setViewport(double zoom, double xPos, double yPos)
{
    _changer->setViewport(zoom, xPos, yPos);
    _changer->otherChange();
    update();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


