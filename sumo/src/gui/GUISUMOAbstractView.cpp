//---------------------------------------------------------------------------//
//                        GUISUMOAbstractView.cpp -
//  A view on the simulation; this view is a microscopic one
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
// Revision 1.12  2003/11/11 08:40:03  dkrajzew
// consequent position2D instead of two doubles implemented
//
// Revision 1.11  2003/09/23 14:25:13  dkrajzew
// possibility to visualise detectors using different geometry complexities added
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
// new subfolder holding popup-menus was added due to link-dependencies under linux; QGLObjectPopupMenu*-classes were moved to "popup"
//
// Revision 1.2  2003/06/05 06:26:16  dkrajzew
// first tries to build under linux: warnings removed; Makefiles added
//
// Revision 1.1  2003/05/20 09:25:13  dkrajzew
// new view hierarchy; some debugging done
//
// Revision 1.12  2003/04/16 09:50:04  dkrajzew
// centering of the network debugged; additional parameter of maximum display size added
//
// Revision 1.11  2003/04/14 08:24:57  dkrajzew
// unneeded display switch and zooming option removed; new glo-objct concept implemented; comments added
//
// Revision 1.10  2003/04/07 10:15:16  dkrajzew
// glut reinserted
//
// Revision 1.9  2003/04/04 15:13:20  roessel
// Commented out #include <glut.h>
// Added #include <qcursor.h>
//
// Revision 1.8  2003/04/04 08:37:51  dkrajzew
// view centering now applies net size; closing problems debugged; comments added; tootip button added
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
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <iostream>
#include <utility>
#include <cmath>
#include <guisim/GUINet.h>
#include <guisim/GUIEdge.h>
#include <guisim/GUILane.h>
#include <guisim/GUIVehicle.h>
#include <guisim/GUINetWrapper.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <qgl.h>
#include <qevent.h>
#include <qpainter.h>
#include <qtoolbar.h>
#include <qdialog.h>
#include <qcombobox.h>
#include <qpixmap.h>
#include <qcursor.h>
#include <qpopupmenu.h>
#include <qgl.h>
#include <utils/gfx/RGBColor.h>
#include <utils/convert/ToString.h>
#include "QGUIToggleButton.h"
#include "QGUIImageField.h"
#include "QGLObjectToolTip.h"
#include "GUIChooser.h"
#include "GUISUMOViewParent.h"
#include "GUIDanielPerspectiveChanger.h"
#include <guisim/GUIDetectorWrapper.h>
#include "GUISUMOAbstractView.h"
#include "popup/QGLObjectPopupMenu.h"
#include "GUIApplicationWindow.h"

//#include "icons/view_traffic/colour_lane.xpm"
//#include "icons/view_traffic/colour_vehicle.xpm"
//#include "icons/view_traffic/show_grid.xpm"
//#include "icons/view_traffic/show_tooltips.xpm"

#ifndef WIN32
#include "GUISUMOAbstractView.moc"
#endif


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


GUISUMOAbstractView::GUISUMOAbstractView(GUIApplicationWindow&app,
                                         GUISUMOViewParent &parent,
                                         GUINet &net)
    : QGLWidget(&parent, ""),
    _app(app),
    _parent(parent),
    _net(net),
    _showGrid(false),
    _changer(0),
    _useToolTips(false),
    _noDrawing(0),
    _mouseHotspotX(cursor().hotSpot().x()),
    _mouseHotspotY(cursor().hotSpot().y()),
    _popup(0)
{
    // set window sizes
    setMinimumSize(100, 30);
    setBaseSize(_parent.getMaxGLWidth(), _parent.getMaxGLHeight());
    setMaximumSize(_parent.getMaxGLWidth(), _parent.getMaxGLHeight());
    // compute the net scale
    double nw = _net.getBoundery().getWidth();
    double nh = _net.getBoundery().getHeight();
    _netScale = (nw < nh ? nh : nw);
    // show the middle at the beginning
    _changer = new GUIDanielPerspectiveChanger(*this);
    _changer->setNetSizes((size_t) nw, (size_t) nh);
    _toolTip = new QGLObjectToolTip(this);
    setMouseTracking(TRUE);
}


GUISUMOAbstractView::~GUISUMOAbstractView()
{
}


void
GUISUMOAbstractView::toggleShowGrid()
{
    _showGrid = !_showGrid;
    update();
}

void
GUISUMOAbstractView::toggleToolTips()
{
    _useToolTips = !_useToolTips;
    update();
}



void
GUISUMOAbstractView::mouseMoveEvent ( QMouseEvent *e )
{
    killTimers();
    _changer->mouseMoveEvent(e);
    QGLWidget::mouseMoveEvent(e);
}


void
GUISUMOAbstractView::mousePressEvent ( QMouseEvent *e )
{
    delete _popup;
    _popup = 0;
    if(_useToolTips) {
        _toolTip->hide();
    }
    killTimers();
    _changer->mousePressEvent(e);
    _timer = startTimer(250);
    assert(_timer!=0);
    _timerReason = e->button();
    QGLWidget::mousePressEvent(e);
}


void
GUISUMOAbstractView::mouseReleaseEvent ( QMouseEvent *e )
{
    _changer->mouseReleaseEvent(e);
    delete _popup;
    _popup = 0;
    killTimers();
    if(_useToolTips) {
        _toolTip->hide();
    }
    QGLWidget::mouseReleaseEvent(e);
}



bool
GUISUMOAbstractView::event( QEvent *e )
{
    if(e->type()==QEvent::MouseButtonDblClick) {
        DEBUG_OUT << "doppelt" << endl;
    }
    return QGLWidget::event(e);
}


void
GUISUMOAbstractView::initializeGL()
{
    _lock.lock();
    _widthInPixels = _parent.getMaxGLWidth();
    _heightInPixels = _parent.getMaxGLHeight();
    _ratio = (double) _widthInPixels / (double) _heightInPixels;
    glViewport( 0, 0, _parent.getMaxGLWidth()-1, _parent.getMaxGLHeight()-1 );
    glClearColor( 1.0, 1.0, 1.0, 1 );
    _changer->otherChange();
    myFontRenderer.add(_app.myFonts.get("std8"));
    doInit();
    _lock.unlock();
}


void
GUISUMOAbstractView::resizeGL( int width, int height )
{
    _lock.lock();
    _widthInPixels = width;
    _heightInPixels = height;
    _changer->applyCanvasSize(width, height);
    _lock.unlock();
}


void
GUISUMOAbstractView::updateToolTip()
{
    if(!_useToolTips) {
        return;
    }
    makeCurrent();
    _lock.lock();
    _noDrawing++;
    // ...and return when drawing is already being done
    if(_noDrawing>1) {
        _noDrawing--;
        _lock.unlock();
        return;
    }
    // initialise the select mode
    unsigned int id = getObjectUnderCursor();
    showToolTipFor(id);
    // mark end-of-drawing
    _noDrawing--;
    _lock.unlock();
}

void
GUISUMOAbstractView::paintGL()
{
    _lock.lock();
    // return if the canvas is not visible
    if(!isVisible()) {
		_lock.unlock();
        return;
    }
    _noDrawing++;
    // ...and return when drawing is already being done
    if(_noDrawing>1) {
        _noDrawing--;
        _lock.unlock();
        return;
    }
    _widthInPixels = width();
    _heightInPixels = height();
    // draw
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    applyChanges(1.0, 0, 0);
    if(_showGrid) {
        paintGLGrid();
    }
    doPaintGL(GL_RENDER, 1.0);
    if(_parent.showLegend()) {
        displayLegend();
    }
    // check whether the select mode /tooltips)
    //  shall be computed, too
    if(!_useToolTips) {
        _noDrawing--;
        glFlush();
        swapBuffers();
        _lock.unlock();
        return;
    }

    glFlush();
    swapBuffers();
    // get the object under the cursor
    unsigned int id = getObjectUnderCursor();
    showToolTipFor(id);
    _noDrawing--;
    glFlush();
    swapBuffers();
    _lock.unlock();
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
    //  Vehicles should have a greater id
    //  than the previously allocated lanes
    unsigned int idMax = 0;
    for (int i=0; i<nb_hits; ++i) {
        assert (i*4+3<NB_HITS_MAX);
        if (hits[i*4+3] > idMax) {
            idMax = hits[i*4+3];
            assert (i*4+3<NB_HITS_MAX);
        }
    }
    return idMax;
}


void
GUISUMOAbstractView::showToolTipFor(unsigned int id)
{
    if(id!=0) {
        GUIGlObject *object = _net._idStorage.getObjectBlocking(id);
        _toolTip->setObjectTip(object, _mouseX, _mouseY);
        if(object!=0) {
            _net._idStorage.unblockObject(id);
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
    double xend = (_net._grid.getNoXCells())
        * _net._grid.getXCellSize();
    double yend = (_net._grid.getNoYCells())
        * _net._grid.getYCellSize();
    for(int yr=0; yr<_net._grid.getNoYCells()+1; yr++) {
        glVertex2f(0, ypos);
        glVertex2f(xend, ypos);
        ypos += _net._grid.getYCellSize();
    }
    for(int xr=0; xr<_net._grid.getNoXCells()+1; xr++) {
        glVertex2f(xpos, 0);
        glVertex2f(xpos, yend);
        xpos += _net._grid.getXCellSize();
    }
    glEnd();
}


void
GUISUMOAbstractView::applyChanges(double scale,
                             size_t xoff, size_t yoff)
{
    _widthInPixels = width();
    _heightInPixels = height();
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    // rotate first;
    //  This is quite unchecked, so it's meaning and position is quite
    //  unclear
    glRotated(_changer->getRotation(), 0, 0, 1);
    // Fit the view's size to the size of the net
    glScaled(
        2.0/_netScale,
        2.0/_netScale,
        0);
    // apply ratio between window width and height
    glScaled(1/_ratio, 1, 0);
    // initially (zoom=100), the net shall be completely visible on the screen
    double xs = ((double) _widthInPixels / (double) _parent.getMaxGLWidth())
        / (_net.getBoundery().getWidth() / _netScale) * _ratio;
    double ys = ((double) _heightInPixels / (double) _parent.getMaxGLHeight())
        / (_net.getBoundery().getHeight() / _netScale);
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
    Position2D center = _net.getBoundery().getCenter();
    glTranslated(-center.x(), -center.y(), 0);
    // Translate in dependence to the view position applied by the user
    glTranslated(_changer->getXPos(), _changer->getYPos(), 0);
    // Translate to the mouse pointer, when wished
    if(xoff!=0||yoff!=0) {
        double absX = (double(xoff)-(double(_widthInPixels)/2.0));
        double absY = (double(yoff)-(double(_heightInPixels)/2.0));
        glTranslated(
            -p2m(absX),
            p2m(absY),
            0);
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // apply the widow size
    double xf = -1.0 *
        ((double) _parent.getMaxGLWidth() - (double) _widthInPixels)
        / (double) _parent.getMaxGLWidth();
    double yf = -1.0 *
        ((double) _parent.getMaxGLHeight() - (double) _heightInPixels)
        / (double) _parent.getMaxGLHeight();
    glTranslated(xf, yf, 0);
    _changer->applied();
}


void
GUISUMOAbstractView::displayLegend()
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
//        text = text + string("0");
    }
    double lineWidth = 1.0;
    glLineWidth(lineWidth);

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

    myFontRenderer.SetColor(0, 0, 0);
    double len = (double) pixelSize / (double) (_parent.getMaxGLWidth()-1) * 2.0;
    glColor3f(0, 0, 0);
    glBegin( GL_LINES );
    // vertical
    glVertex2f(-.98, -.98);
    glVertex2f(-.98+len, -.98);
    // tick at begin
    glVertex2f(-.98, -.98);
    glVertex2f(-.98, -.97);
    myFontRenderer.StringOut(10, (double) (_parent.getMaxGLHeight()-30), "0m");
    // tick at end
    glVertex2f(-.98+len, -.98);
    glVertex2f(-.98+len, -.97);
    myFontRenderer.StringOut(10+pixelSize, (double) (_parent.getMaxGLHeight()-30),
        text.substr(0, noDigits) + "m");
    glEnd();
    myFontRenderer.Draw();

    // draw the current position
/*
    std::pair<double, double> c = canvas2World(_mouseX, _mouseY);
    string out = toString<double>(c.first) + ", " + toString<double>(c.second);
    myFontRenderer.StringOut(width()-100, (double) (_parent.getMaxGLHeight()-20),
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
    double xret = p2m(x-(_parent.getMaxGLWidth())/2.0)-_changer->getXPos();
    double yret = p2m(y-(_parent.getMaxGLHeight())/2.0)-_changer->getYPos();
    return std::pair<double, double>(xret, yret);
}


double
GUISUMOAbstractView::m2p(double meter)
{
    return
        (meter/_netScale
        * (_parent.getMaxGLWidth()/_ratio)
        * _addScl * _changer->getZoom() / 100.0);
}


double
GUISUMOAbstractView::p2m(double pixel)
{
    return
        pixel * _netScale /
        ((_parent.getMaxGLWidth()/_ratio) * _addScl *_changer->getZoom() / 100.0);
}


void
GUISUMOAbstractView::recenterView()
{
    _changer->recenterView();
}


void
GUISUMOAbstractView::centerTo(GUIGlObjectType type,
                              const std::string &name)
{
    switch(type) {
    case GLO_NETWORK:
        {
            centerTo(_net.getBoundery());
        }
        break;
    case GLO_JUNCTION:
        {
            Position2D pos = _net.getJunctionPosition(name);
            centerTo(pos, 50); // !!! the radius should be variable
        }
        break;
    case GLO_LANE:
        {
            Boundery bound =
                _net.getEdgeBoundery(name.substr(0, name.find_last_of("_")));
            bound.grow(20.0);
            centerTo(bound);
        }
        break;
    case GLO_EDGE:
        {
            Boundery bound = _net.getEdgeBoundery(name);
            bound.grow(20.0);
            centerTo(bound);
        }
        break;
    case GLO_VEHICLE:
        {
            if(_net.vehicleExists(name)) {
                Position2D pos = _net.getVehiclePosition(name);
                centerTo(pos, 20); // !!! another radius?
            }
        }
        break;
    case GLO_DETECTOR:
        {
            Position2D pos = _net.getDetectorPosition(name);
            centerTo(pos, 20); // !!! the radius should be variable
        }
        break;
    case GLO_EMITTER:
        {
            Position2D pos = _net.getEmitterPosition(name);
            centerTo(pos, 20); // !!! the radius should be variable
        }
        break;
    default:
        // should not happen
        throw 1;
    }
    _changer->otherChange();
    update();
}


void
GUISUMOAbstractView::centerTo(Position2D pos, double radius)
{
    _changer->centerTo(_net.getBoundery(), pos, radius);
}


void
GUISUMOAbstractView::centerTo(Boundery bound)
{
    _changer->centerTo(_net.getBoundery(), bound);
}


bool
GUISUMOAbstractView::allowRotation() const
{
    return _parent.allowRotation();
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


void
GUISUMOAbstractView::paintEvent ( QPaintEvent *e )
{
    // do not paint anything when the canvas is
    //  not visible
    if(!isVisible()) {
        return;
    }
/*    // mark that drawing is in process
    _noDrawing++;
    // ...and return when drawing is already
    //  being done
    if(_noDrawing>1) {
        _noDrawing--;
        return;
    }
    makeCurrent();
    glDraw();*/
    QGLWidget::paintEvent(e);
}



void
GUISUMOAbstractView::timerEvent ( QTimerEvent *e )
{
    killTimers();
    switch(_timerReason) {
    case Qt::LeftButton:
        DEBUG_OUT << "Left Button" << endl;
        break;
    case Qt::RightButton:
        {
            makeCurrent();
            _lock.lock();
            _noDrawing++;
            // ...and return when drawing is already being done
            if(_noDrawing>1) {
                _noDrawing--;
                _lock.unlock();
                return;
            }
            // initialise the select mode
            unsigned int id = getObjectUnderCursor();
            GUIGlObject *o = 0;
            if(id!=0) {
                o = _net._idStorage.getObjectBlocking(id);
            } else {
                o = _net.getWrapper();
            }
            if(o!=0) {
                _popup = o->getPopUpMenu(_app, *this);
                _popup->setGeometry(_mouseX, _mouseY,
                    _popup->width()+_mouseX, _popup->height()+_mouseY);
                _popup->show();
                _changer->mouseReleaseEvent(0);
            }
            _noDrawing--;
            _lock.unlock();
        }
        break;
    default:
        break;
    }
    // !!! what to do with e?
}


void
GUISUMOAbstractView::makeCurrent()
{
    _lock.lock();
    QGLWidget::makeCurrent();
	_lock.unlock();
}



int
GUISUMOAbstractView::getMaxGLWidth() const
{
    return _parent.getMaxGLWidth();
}

int
GUISUMOAbstractView::getMaxGLHeight() const
{
    return _parent.getMaxGLHeight();
}


void
GUISUMOAbstractView::clearUsetable(size_t *_edges2Show,
                                   size_t _edges2ShowSize)
{
    memset(_edges2Show, 0, sizeof(size_t)*_edges2ShowSize);
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUISUMOAbstractView.icc"
//#endif

// Local Variables:
// mode:C++
// End:


