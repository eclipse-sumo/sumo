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
// Revision 1.13  2004/03/19 12:54:07  dkrajzew
// porting to FOX
//
// Revision 1.12  2003/11/11 08:40:03  dkrajzew
// consequent position2D instead of two doubles implemented
//
// Revision 1.11  2003/08/20 11:58:04  dkrajzew
// cleaned up a bit
//
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
/* =========================================================================
 * included modules
 * ======================================================================= */
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
GUIDanielPerspectiveChanger::GUIDanielPerspectiveChanger(
            GUISUMOAbstractView &callBack)
    : GUIPerspectiveChanger(callBack),
    myViewCenter(0, 0), _rotation(0), _zoom(100),
    _mouseButtonState(MOUSEBTN_NONE)
{
}


GUIDanielPerspectiveChanger::~GUIDanielPerspectiveChanger()
{
}


void
GUIDanielPerspectiveChanger::move(int xdiff, int ydiff)
{
    myViewCenter.add(-_callback.p2m(xdiff), _callback.p2m(ydiff));
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


double
GUIDanielPerspectiveChanger::getRotation() const
{
    return _rotation;
}


double
GUIDanielPerspectiveChanger::getXPos() const
{
    return myViewCenter.x();
}


double
GUIDanielPerspectiveChanger::getYPos() const
{
    return myViewCenter.y();
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
    myViewCenter.set(0, 0);
    _zoom = 100;
    _changed = true;
}



void
GUIDanielPerspectiveChanger::centerTo(const Boundery &netBoundery,
                                      const Position2D &pos, double radius)
{
    myViewCenter.set(pos);
    myViewCenter.sub(netBoundery.getCenter());
    myViewCenter.mul(-1.0);
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
    myViewCenter.set(bound.getCenter());
    myViewCenter.sub(netBoundery.getCenter());
    myViewCenter.mul(-1.0);
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


long
GUIDanielPerspectiveChanger::onLeftBtnPress(FXObject*,FXSelector,void*data)
{
    FXEvent* e = (FXEvent*) data;
    _mouseButtonState =
        (MouseState) ((int) _mouseButtonState | (int) MOUSEBTN_LEFT);
    _mouseXPosition = e->win_x;
    _mouseYPosition = e->win_y;
    return 1;
}


long
GUIDanielPerspectiveChanger::onLeftBtnRelease(FXObject*,FXSelector,void*data)
{
    FXEvent* e = (FXEvent*) data;
    _mouseButtonState =
        (MouseState) ((int) _mouseButtonState & (255-(int) MOUSEBTN_LEFT));
    _mouseXPosition = e->win_x;
    _mouseYPosition = e->win_y;
    return 1;
}


long
GUIDanielPerspectiveChanger::onRightBtnPress(FXObject*,FXSelector,void*data)
{
    FXEvent* e = (FXEvent*) data;
    _mouseButtonState =
        (MouseState) ((int) _mouseButtonState | (int) MOUSEBTN_RIGHT);
    _mouseXPosition = e->win_x;
    _mouseYPosition = e->win_y;
    return 1;
}


long
GUIDanielPerspectiveChanger::onRightBtnRelease(FXObject*,FXSelector,void*data)
{
    _mouseButtonState =
        (MouseState) ((int) _mouseButtonState & (255-(int) MOUSEBTN_RIGHT));
    if(data!=0) {
        FXEvent* e = (FXEvent*) data;
        _mouseXPosition = e->win_x;
        _mouseYPosition = e->win_y;
    }
    return 1;
}


long
GUIDanielPerspectiveChanger::onMouseMove(FXObject*,FXSelector,void*data)
{
    FXEvent* e = (FXEvent*) data;
    _callback.setTooltipPosition(e->win_x, e->win_y, e->root_x, e->root_y);
    int xdiff = _mouseXPosition - e->win_x;
    int ydiff = _mouseYPosition - e->win_y;
    switch(_mouseButtonState) {
    case MOUSEBTN_LEFT:
        move(xdiff, ydiff);
        break;
    case MOUSEBTN_RIGHT:
        zoom(ydiff);
        rotate(xdiff);
        break;
    default:
        _callback.updateToolTip();
        break;
    }
    _mouseXPosition = e->win_x;
    _mouseYPosition = e->win_y;
    return 1;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:




