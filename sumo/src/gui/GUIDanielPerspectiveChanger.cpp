//---------------------------------------------------------------------------//
//                        GUIDanielPerspectiveChanger.cpp -
//  A class that allows to steer the visual output in dependence to
//      user interaction
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
// $Log$
// Revision 1.10  2003/07/18 12:29:28  dkrajzew
// removed some warnings
//
// Revision 1.9  2003/06/18 11:27:54  dkrajzew
// some functions commented out removed
//
// Revision 1.8  2003/06/05 06:26:16  dkrajzew
// first tries to build under linux: warnings removed; Makefiles added
//
// Revision 1.7  2003/05/20 09:23:54  dkrajzew
// some statistics added; some debugging done
//
// Revision 1.6  2003/04/16 09:50:04  dkrajzew
// centering of the network debugged; additional parameter of maximum display size added
//
// Revision 1.5  2003/04/04 08:37:49  dkrajzew
// view centering now applies net size; closing problems debugged; comments added; tootip button added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <qevent.h>
#include <qnamespace.h>
#include <utils/geom/Boundery.h>
#include <utils/geom/Position2D.h>
#include "GUIViewTraffic.h"
#include "GUIPerspectiveChanger.h"
#include "GUIDanielPerspectiveChanger.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIDanielPerspectiveChanger::GUIDanielPerspectiveChanger(GUISUMOAbstractView &callBack)
    : GUIPerspectiveChanger(callBack),
    _xpos(0), _ypos(0), _rotation(0), _zoom(100), _mouseButtonState(Qt::NoButton)
{
}


GUIDanielPerspectiveChanger::~GUIDanielPerspectiveChanger()
{
}


void
GUIDanielPerspectiveChanger::mouseMoveEvent ( QMouseEvent *e )
{
    _callback.setTooltipPosition(e->x(), e->y(), e->globalX(), e->globalY());
    int xdiff = _mouseXPosition - e->x();
    int ydiff = _mouseYPosition - e->y();
    switch(_mouseButtonState) {
    case Qt::LeftButton:
        move(xdiff, ydiff);
        break;
    case Qt::RightButton:
        zoom(ydiff);
        rotate(xdiff);
        break;
    default:
        _callback.updateToolTip();
        break;
    }
    _mouseXPosition = e->x();
    _mouseYPosition = e->y();
}


void
GUIDanielPerspectiveChanger::move(int xdiff, int ydiff)
{
    _xpos -= _callback.p2m(xdiff);
    _ypos += _callback.p2m(ydiff);
    _changed = true;
    _callback.update();
}


void
GUIDanielPerspectiveChanger::zoom(int diff)
{
    double zoom = _zoom
        + (double) diff / 100.0 * _zoom;
    if(zoom>0.01&&zoom<10000000.0) {
        _zoom = zoom;
        _changed = true;
        _callback.update();
    }
}


void
GUIDanielPerspectiveChanger::rotate(int diff)
{
    if(_callback.allowRotation()) {
        _rotation += (double) diff / 10.0;
        _changed = true;
        _callback.update();
    }
}

void
GUIDanielPerspectiveChanger::mousePressEvent ( QMouseEvent *e )
{
    _mouseButtonState = e->button();
    _mouseXPosition = e->x();
    _mouseYPosition = e->y();
}


void
GUIDanielPerspectiveChanger::mouseReleaseEvent ( QMouseEvent * )
{
    _mouseButtonState = Qt::NoButton;
}


double
GUIDanielPerspectiveChanger::getRotation() const
{
    return _rotation;
}


double
GUIDanielPerspectiveChanger::getXPos() const
{
    return _xpos;
}


double
GUIDanielPerspectiveChanger::getYPos() const
{
    return _ypos;
}


double
GUIDanielPerspectiveChanger::getZoom() const
{
    return _zoom;
}


void
GUIDanielPerspectiveChanger::recenterView()
{
    _rotation = 0;
    _xpos = 0;
    _ypos = 0;
    _zoom = 100;
    _changed = true;
}



void
GUIDanielPerspectiveChanger::centerTo(const Boundery &netBoundery,
                                      const Position2D &pos, double radius)
{
    _xpos = -(pos.x() - netBoundery.getCenter().first);
    _ypos = -(pos.y() - netBoundery.getCenter().second);
    _zoom =
        netBoundery.getWidth() < netBoundery.getHeight() ?
        25.0 * netBoundery.getWidth() / radius :
        25.0 * netBoundery.getHeight() / radius;
    _changed = true;
}


void
GUIDanielPerspectiveChanger::centerTo(const Boundery &netBoundery,
                                      Boundery bound)
{
//    bound.flipY();
    _xpos = -(bound.getCenter().first
        - netBoundery.getCenter().first);
    _ypos = -(bound.getCenter().second
        - netBoundery.getCenter().second);
    _zoom =
        bound.getWidth() > bound.getHeight() ?
        100.0 * netBoundery.getWidth() / bound.getWidth() :
        100.0 * netBoundery.getHeight() / bound.getHeight();
    _changed = true;
}



int
GUIDanielPerspectiveChanger::getMouseXPosition() const
{
    return _mouseXPosition;
}


int
GUIDanielPerspectiveChanger::getMouseYPosition() const
{
    return _mouseYPosition;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIDanielPerspectiveChanger.icc"
//#endif

// Local Variables:
// mode:C++
// End:




