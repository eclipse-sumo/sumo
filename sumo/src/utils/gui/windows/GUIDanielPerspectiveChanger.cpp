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
// Revision 1.5  2005/10/07 11:46:08  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/23 06:11:14  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.3  2005/09/15 12:20:19  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/05/04 09:23:41  dkrajzew
// entries for viewport definition added; popups now popup faster
//
// Revision 1.1  2004/11/23 10:38:32  dkrajzew
// debugging
//
// Revision 1.2  2004/10/29 06:01:55  dksumo
// renamed boundery to boundary
//
// Revision 1.1  2004/10/22 12:50:56  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.13  2004/03/19 12:54:07  dkrajzew
// porting to FOX
//
// Revision 1.12  2003/11/11 08:40:03  dkrajzew
// consequent position2D instead of two SUMOReals implemented
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
// centering of the network debugged; additional parameter of maximum
//  display size added
//
// Revision 1.5  2003/04/04 08:37:49  dkrajzew
// view centering now applies net size; closing problems debugged;
//  comments added; tootip button added
//
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

#include <utils/geom/Boundary.h>
#include <utils/geom/Position2D.h>
#include "GUIPerspectiveChanger.h"
#include "GUIDanielPerspectiveChanger.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


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
    _mouseButtonState(MOUSEBTN_NONE), myMoveOnRightClick(false)
{
}


GUIDanielPerspectiveChanger::~GUIDanielPerspectiveChanger()
{
}


void
GUIDanielPerspectiveChanger::move(int xdiff, int ydiff)
{
    myViewCenter.add((SUMOReal) -_callback.p2m((SUMOReal) xdiff), (SUMOReal) _callback.p2m((SUMOReal) ydiff));
    _changed = true;
    _callback.update();
}


void
GUIDanielPerspectiveChanger::zoom(int diff)
{
    SUMOReal zoom = (SUMOReal) _zoom
        + (SUMOReal) diff /(SUMOReal)  100.0 * (SUMOReal) _zoom;
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
        _rotation += (SUMOReal) diff / (SUMOReal) 10.0;
        _changed = true;
        _callback.update();
    }
}


SUMOReal
GUIDanielPerspectiveChanger::getRotation() const
{
    return _rotation;
}


SUMOReal
GUIDanielPerspectiveChanger::getXPos() const
{
    return myViewCenter.x();
}


SUMOReal
GUIDanielPerspectiveChanger::getYPos() const
{
    return myViewCenter.y();
}


SUMOReal
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
GUIDanielPerspectiveChanger::centerTo(const Boundary &netBoundary,
                                      const Position2D &pos, SUMOReal radius)
{
    myViewCenter.set(pos);
    myViewCenter.sub(netBoundary.getCenter());
    myViewCenter.mul(-1.0);
    _zoom =
        netBoundary.getWidth() < netBoundary.getHeight() ?
        (SUMOReal) 25.0 * (SUMOReal) netBoundary.getWidth() / radius :
        (SUMOReal) 25.0 * (SUMOReal) netBoundary.getHeight() / radius;
    _changed = true;
}


void
GUIDanielPerspectiveChanger::centerTo(const Boundary &netBoundary,
                                      Boundary bound)
{
    myViewCenter.set(bound.getCenter());
    myViewCenter.sub(netBoundary.getCenter());
    myViewCenter.mul(-1.0);
    _zoom =
        bound.getWidth() > bound.getHeight() ?
        (SUMOReal) 100.0 * (SUMOReal) netBoundary.getWidth() / (SUMOReal) bound.getWidth() :
        (SUMOReal) 100.0 * (SUMOReal) netBoundary.getHeight() / (SUMOReal) bound.getHeight();
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


void
GUIDanielPerspectiveChanger::onLeftBtnPress(void*data)
{
    FXEvent* e = (FXEvent*) data;
    _mouseButtonState =
        (MouseState) ((int) _mouseButtonState | (int) MOUSEBTN_LEFT);
    _mouseXPosition = e->win_x;
    _mouseYPosition = e->win_y;
}


void
GUIDanielPerspectiveChanger::onLeftBtnRelease(void*data)
{
    FXEvent* e = (FXEvent*) data;
    _mouseButtonState =
        (MouseState) ((int) _mouseButtonState & (255-(int) MOUSEBTN_LEFT));
    _mouseXPosition = e->win_x;
    _mouseYPosition = e->win_y;
}


void
GUIDanielPerspectiveChanger::onRightBtnPress(void*data)
{
    FXEvent* e = (FXEvent*) data;
    _mouseButtonState =
        (MouseState) ((int) _mouseButtonState | (int) MOUSEBTN_RIGHT);
    _mouseXPosition = e->win_x;
    _mouseYPosition = e->win_y;
    myMoveOnRightClick = true;
}


bool
GUIDanielPerspectiveChanger::onRightBtnRelease(void*data)
{
    _mouseButtonState =
        (MouseState) ((int) _mouseButtonState & (255-(int) MOUSEBTN_RIGHT));
    if(data!=0) {
        FXEvent* e = (FXEvent*) data;
        _mouseXPosition = e->win_x;
        _mouseYPosition = e->win_y;
    }
    if(myMoveOnRightClick) {
        myMoveOnRightClick = false;
        return true;
    }
    return false;
}


void
GUIDanielPerspectiveChanger::onMouseMove(void*data)
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
        if(abs(xdiff)>0||abs(ydiff)>0) {
            myMoveOnRightClick = false;
        }
        break;
    default:
        _callback.updateToolTip();
        break;
    }
    _mouseXPosition = e->win_x;
    _mouseYPosition = e->win_y;
}


void
GUIDanielPerspectiveChanger::setViewport(SUMOReal zoom,
                                         SUMOReal xPos, SUMOReal yPos)
{
    _zoom = zoom;
    myViewCenter.set(xPos, yPos);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:




