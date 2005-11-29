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
// Revision 1.10  2005/11/29 13:34:47  dkrajzew
// viewport debugged
//
// Revision 1.9  2005/11/09 06:46:34  dkrajzew
// added cursor position output (unfinished)
//
// Revision 1.8  2005/10/07 11:46:08  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.7  2005/09/23 06:11:14  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.6  2005/09/15 12:20:19  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/07/12 12:52:07  dkrajzew
// code style adapted
//
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
// consequent position2D instead of two SUMOReals implemented
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <iostream>
#include <utility>
#include <cmath>
#include <cassert>
#include <utils/gfx/RGBColor.h>
#include <utils/common/ToString.h>
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
#include <utils/glutils/polyfonts.h>
#include <utils/shapes/PointOfInterest.h>
#include <utils/shapes/ShapeContainer.h>
#include <utils/gui/globjects/GUIPointOfInterest.h>
#include <utils/gui/globjects/GUIPolygon2D.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


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


GUISUMOAbstractView::ViewSettings::ViewSettings(SUMOReal x, SUMOReal y,
                                                SUMOReal xoff, SUMOReal yoff)
    : myX(x), myY(y), myXOff(xoff), myYOff(yoff)
{
}


GUISUMOAbstractView::ViewSettings::~ViewSettings()
{
}


bool
GUISUMOAbstractView::ViewSettings::differ(SUMOReal x, SUMOReal y,
                                          SUMOReal xoff, SUMOReal yoff)
{
    return myX!=x || myY!=y || myXOff!=xoff || myXOff!=yoff;
}


void
GUISUMOAbstractView::ViewSettings::set(SUMOReal x, SUMOReal y,
                                       SUMOReal xoff, SUMOReal yoff)
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
    FXMAPFUNC(SEL_COMMAND,             MID_EDITVIEW,      GUISUMOAbstractView::onCmdEditView),

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
    myAmInitialised(false),
    myViewportChooser(0)
{
    flags|=FLAG_ENABLED;
	_inEditMode=false;
    // compute the net scale
    SUMOReal nw = myGrid->getBoundary().getWidth();
    SUMOReal nh = myGrid->getBoundary().getHeight();
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
    myAmInitialised(false),
    myViewportChooser(0)
{
    flags|=FLAG_ENABLED;
	_inEditMode=false;
    // compute the net scale
    SUMOReal nw = myGrid->getBoundary().getWidth();
    SUMOReal nh = myGrid->getBoundary().getHeight();
    myNetScale = (nw < nh ? nh : nw);
    // show the middle at the beginning
    _changer = new GUIDanielPerspectiveChanger(*this);
    _changer->setNetSizes((size_t) nw, (size_t) nh);
    _toolTip = new GUIGLObjectToolTip(myApp);
}


GUISUMOAbstractView::~GUISUMOAbstractView()
{
    delete _changer;
    delete _toolTip;
    // just to quit cleanly on a failure
    if(_lock.locked()) {
        _lock.unlock();
    }
    delete myViewportChooser;
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
GUISUMOAbstractView::updatePositionInformation()
{
    if(false) {
        const Boundary &nb = myGrid->getBoundary();
        SUMOReal width = nb.getWidth();
        SUMOReal height = nb.getHeight();
        SUMOReal mzoom = _changer->getZoom();

        // compute the offset
        SUMOReal cy = _changer->getYPos();//cursorY;
        SUMOReal cx = _changer->getXPos();//cursorY;

        cout << cx << ", " << cy << endl;

        // compute the visible area in horizontal direction
        SUMOReal mratio = (SUMOReal) _widthInPixels / (SUMOReal) _heightInPixels;
        SUMOReal sxmin = nb.getCenter().x()
            - mratio * width * (SUMOReal) 100 / (mzoom) / (SUMOReal) 2. / (SUMOReal) .97;
        sxmin -= cx;
        SUMOReal sxmax = nb.getCenter().x()
            + mratio * width * (SUMOReal) 100 / (mzoom) / (SUMOReal) 2. / (SUMOReal) .97;
        sxmax -= cx;

        // compute the visible area in vertical direction
        SUMOReal symin = nb.getCenter().y() - height / mzoom * (SUMOReal) 100 / (SUMOReal) 2. / (SUMOReal) .97;
        symin += cy;
        SUMOReal symax = nb.getCenter().y() + height / mzoom * (SUMOReal) 100 / (SUMOReal) 2. / (SUMOReal) .97;
        symax += cy;

        SUMOReal sx = sxmin
            + (sxmax-sxmin)
            * (SUMOReal) _changer->getMouseXPosition()
            / (SUMOReal) _widthInPixels;
        SUMOReal sy = symin
            + (symax-symin)
            * ((SUMOReal) _heightInPixels - (SUMOReal) _changer->getMouseYPosition())
            / (SUMOReal) _heightInPixels;

        string text = "x:" + toString(sx) + ", y:" + toString(sy);
        myApp->setStatusBarText(text);
    }
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
    updatePositionInformation();
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
    SUMOReal scale = SUMOReal(getMaxGLWidth())/SUMOReal(SENSITIVITY);
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
    GUIGlObjectType prevType = (GUIGlObjectType) 0;
    for (int i=0; i<nb_hits; ++i) {
        assert (i*4+3<NB_HITS_MAX);
        unsigned int id = hits[i*4+3];
        GUIGlObject *o = gIDStorage.getObjectBlocking(id);
        if(o==0) {
            continue;
        }
        GUIGlObjectType type = o->getType();
        if((prevType<type)&&type!=0) {
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
    SUMOReal ypos = 0;
    SUMOReal xpos = 0;
    SUMOReal xend = (myGrid->getNoXCells())
        * myGrid->getXCellSize();
    SUMOReal yend = (myGrid->getNoYCells())
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
GUISUMOAbstractView::applyChanges(SUMOReal scale, size_t xoff, size_t yoff)
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
    SUMOReal xs = ((SUMOReal) _widthInPixels / (SUMOReal) myApp->getMaxGLWidth())
        / (myGrid->getBoundary().getWidth() / myNetScale) * _ratio;
    SUMOReal ys = ((SUMOReal) _heightInPixels / (SUMOReal) myApp->getMaxGLHeight())
        / (myGrid->getBoundary().getHeight() / myNetScale);
    if(xs<ys) {
        glScaled(xs, xs, 1);
        _addScl = xs;
    } else {
        glScaled(ys, ys, 1);
        _addScl = ys;
    }
    // initially, leave some room for the net
    glScaled((SUMOReal) 0.97, (SUMOReal) 0.97, (SUMOReal) 1);
    _addScl *= (SUMOReal) .97;

    // Apply the zoom and the scale
    SUMOReal zoom = (SUMOReal) (_changer->getZoom() / 100.0 * scale);
    glScaled(zoom, zoom, 0);
    // Translate to the middle of the net
    Position2D center = myGrid->getBoundary().getCenter();
    glTranslated(-center.x(), -center.y(), 0);
    // Translate in dependence to the view position applied by the user
    glTranslated(_changer->getXPos(), _changer->getYPos(), 0);
    // Translate to the mouse pointer, when wished
    if(xoff!=0||yoff!=0) {
        SUMOReal absX = (SUMOReal) (xoff-((SUMOReal) _widthInPixels/2.0));
        SUMOReal absY = (SUMOReal) (yoff-((SUMOReal) _heightInPixels/2.0));
        glTranslated(-p2m(absX), p2m(absY), 0);
    }
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // apply the widow size
    SUMOReal xf = (SUMOReal) -1.0 *
        ((SUMOReal) myApp->getMaxGLWidth() - (SUMOReal) _widthInPixels)
        / (SUMOReal) myApp->getMaxGLWidth();
    SUMOReal yf = (SUMOReal) -1.0 *
        ((SUMOReal) myApp->getMaxGLHeight() - (SUMOReal) _heightInPixels)
        / (SUMOReal) myApp->getMaxGLHeight();
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
        pixelSize = (size_t) m2p((SUMOReal) length);
        if(pixelSize>20) {
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

    /*
    GUITexturesHelper::getFontRenderer().SetActiveFont("std8");
    GUITexturesHelper::getFontRenderer().SetColor(0, 0, 0);
    */
    SUMOReal len = (SUMOReal) pixelSize / (SUMOReal) (myApp->getMaxGLWidth()-1) * (SUMOReal) 2.0;
    glColor3f(0, 0, 0);
    glBegin( GL_LINES );
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

    pfSetPosition((SUMOReal) (-.99+len), .96f);
    glRotated(180, 1, 0, 0);
    pfDrawString((text.substr(0, noDigits) + "m").c_str());
    glRotated(-180, 1, 0, 0);

    // restore matrices
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}


std::pair<SUMOReal, SUMOReal>
GUISUMOAbstractView::canvas2World(SUMOReal x, SUMOReal y)
{
    SUMOReal xret = (SUMOReal) (p2m(x-(myApp->getMaxGLWidth())/(SUMOReal) 2.0)-_changer->getXPos());
    SUMOReal yret = (SUMOReal) (p2m(y-(myApp->getMaxGLHeight())/(SUMOReal) 2.0)-_changer->getYPos());
    return std::pair<SUMOReal, SUMOReal>(xret, yret);
}


SUMOReal
GUISUMOAbstractView::m2p(SUMOReal meter)
{
    return (SUMOReal)
        (meter/myNetScale
        * (myApp->getMaxGLWidth()/_ratio)
        * _addScl * _changer->getZoom() / (SUMOReal) 100.0);
}


SUMOReal
GUISUMOAbstractView::p2m(SUMOReal pixel)
{
    return (SUMOReal)
        pixel * myNetScale /
        ((myApp->getMaxGLWidth()/_ratio) * _addScl *_changer->getZoom() / (SUMOReal) 100.0);
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
        _ratio = (SUMOReal) _widthInPixels / (SUMOReal) _heightInPixels;
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
    ungrab();
    return 1;
}


long
GUISUMOAbstractView::onMouseMove(FXObject *o,FXSelector sel,void *data)
{
    SUMOReal xpos = _changer->getXPos();
    SUMOReal ypos = _changer->getYPos();
    SUMOReal zoom = _changer->getZoom();
    if(myViewportChooser==0||!myViewportChooser->haveGrabbed()) {
        _changer->onMouseMove(data);
    }
    if(myViewportChooser!=0 &&
        (xpos!=_changer->getXPos()||ypos!=_changer->getYPos()||zoom!=_changer->getZoom()) ) {

        myViewportChooser->setValues(
            _changer->getZoom(), _changer->getXPos(), _changer->getYPos());

    }
    updatePositionInformation();
    return 1;
}


void
GUISUMOAbstractView::openObjectDialog()
{
    ungrab();
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
GUISUMOAbstractView::drawPolygon2D(const Polygon2D &polygon) const
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if(_useToolTips) {
        glPushName(static_cast<const GUIPolygon2D&>(polygon).getGlID());
    }
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
    if(_useToolTips) {
        glPopName();
    }
}


void
GUISUMOAbstractView::drawPOI2D(const PointOfInterest &p) const
{
    if(_useToolTips) {
        glPushName(static_cast<const GUIPointOfInterest&>(p).getGlID());
    }
    glColor3d(p.red(),p.green(),p.blue());
    glTranslated(p.x(), p.y(), 0);
    GLHelper::drawFilledCircle((SUMOReal) 1.3, 16);
    glTranslated(-p.x(), -p.y(), 0);
    if(_useToolTips) {
        glPopName();
    }
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
    if(myViewportChooser==0) {
        myViewportChooser =
            new GUIDialog_EditViewport(this, "Edit Viewport...",
                _changer->getZoom(), _changer->getXPos(), _changer->getYPos(),
                0, 0);
        myViewportChooser->create();
    }
    myViewportChooser->setOldValues(
        _changer->getZoom(), _changer->getXPos(), _changer->getYPos());
    myViewportChooser->show();
    return 1;
}


long
GUISUMOAbstractView::onCmdEditView(FXObject*,FXSelector,void*)
{
    return 1;
}


void
GUISUMOAbstractView::setViewport(SUMOReal zoom, SUMOReal xPos, SUMOReal yPos)
{
    _changer->setViewport(zoom, xPos, yPos);
    _changer->otherChange();
    update();
}


void
GUISUMOAbstractView::drawShapes(const ShapeContainer &sc) const
{
    {
        const std::vector<Polygon2D*> &pv = sc.getPolygonCont().buildAndGetStaticVector();
        std::vector<Polygon2D*>::const_iterator pi = pv.begin();
        for(; pi!=pv.end(); pi++) {
             drawPolygon2D(**pi);
        }
    }
    {
        const std::vector<PointOfInterest*> &pv = sc.getPOICont().buildAndGetStaticVector();
        std::vector<PointOfInterest*>::const_iterator pi = pv.begin();
        for(; pi!=pv.end(); pi++) {
             drawPOI2D(**pi);
        }
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


