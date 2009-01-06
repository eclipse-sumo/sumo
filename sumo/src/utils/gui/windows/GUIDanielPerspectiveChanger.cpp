/****************************************************************************/
/// @file    GUIDanielPerspectiveChanger.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A class that allows to steer the visual output in dependence to
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include <utils/geom/Boundary.h>
#include <utils/geom/Position2D.h>
#include "GUIPerspectiveChanger.h"
#include "GUIDanielPerspectiveChanger.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
GUIDanielPerspectiveChanger::GUIDanielPerspectiveChanger(
    GUISUMOAbstractView &callBack)
        : GUIPerspectiveChanger(callBack),
        myViewCenter(0, 0), myRotation(0), myZoom(100),
        myMouseButtonState(MOUSEBTN_NONE), myMoveOnRightClick(false)
{}


GUIDanielPerspectiveChanger::~GUIDanielPerspectiveChanger()
{}


void
GUIDanielPerspectiveChanger::move(int xdiff, int ydiff)
{
    myViewCenter.add((SUMOReal) -myCallback.p2m((SUMOReal) xdiff), (SUMOReal) myCallback.p2m((SUMOReal) ydiff));
    myHaveChanged = true;
    myCallback.update();
}


void
GUIDanielPerspectiveChanger::zoom(int diff)
{
    SUMOReal zoom = (SUMOReal) myZoom
                    + (SUMOReal) diff /(SUMOReal)  100.0 * (SUMOReal) myZoom;
    if (zoom>0.01&&zoom<10000000.0) {
        myZoom = zoom;
        myHaveChanged = true;
        myCallback.update();
    }
}


void
GUIDanielPerspectiveChanger::rotate(int diff)
{
    if (false) {//myCallback.allowRotation()) {
        myRotation += (SUMOReal) diff / (SUMOReal) 10.0;
        myHaveChanged = true;
        myCallback.update();
    }
}


SUMOReal
GUIDanielPerspectiveChanger::getRotation() const
{
    return myRotation;
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
    return myZoom;
}


void
GUIDanielPerspectiveChanger::recenterView()
{
    myRotation = 0;
    myViewCenter.set(0, 0);
    myZoom = 100;
    myHaveChanged = true;
}



void
GUIDanielPerspectiveChanger::centerTo(const Boundary &netBoundary,
                                      const Position2D &pos, SUMOReal radius,
                                      bool applyZoom)
{
    myViewCenter.set(pos);
    myViewCenter.sub(netBoundary.getCenter());
    myViewCenter.mul(-1.0);
    if (applyZoom) {
        myZoom =
            netBoundary.getWidth() < netBoundary.getHeight() ?
            (SUMOReal) 25.0 * (SUMOReal) netBoundary.getWidth() / radius :
            (SUMOReal) 25.0 * (SUMOReal) netBoundary.getHeight() / radius;
    }
    myHaveChanged = true;
}


void
GUIDanielPerspectiveChanger::centerTo(const Boundary &netBoundary,
                                      Boundary bound,
                                      bool applyZoom)
{
    myViewCenter.set(bound.getCenter());
    myViewCenter.sub(netBoundary.getCenter());
    myViewCenter.mul(-1.0);
    if (applyZoom) {
        myZoom =
            bound.getWidth() > bound.getHeight() ?
            (SUMOReal) 100.0 * (SUMOReal) netBoundary.getWidth() / (SUMOReal) bound.getWidth() :
            (SUMOReal) 100.0 * (SUMOReal) netBoundary.getHeight() / (SUMOReal) bound.getHeight();
    }
    myHaveChanged = true;
}



void
GUIDanielPerspectiveChanger::onLeftBtnPress(void*data)
{
    FXEvent* e = (FXEvent*) data;
    myMouseButtonState =
        (MouseState)((int) myMouseButtonState | (int) MOUSEBTN_LEFT);
    myMouseXPosition = e->win_x;
    myMouseYPosition = e->win_y;
}


void
GUIDanielPerspectiveChanger::onLeftBtnRelease(void*data)
{
    FXEvent* e = (FXEvent*) data;
    myMouseButtonState =
        (MouseState)((int) myMouseButtonState & (255-(int) MOUSEBTN_LEFT));
    myMouseXPosition = e->win_x;
    myMouseYPosition = e->win_y;
}


void
GUIDanielPerspectiveChanger::onRightBtnPress(void*data)
{
    FXEvent* e = (FXEvent*) data;
    myMouseButtonState =
        (MouseState)((int) myMouseButtonState | (int) MOUSEBTN_RIGHT);
    myMouseXPosition = e->win_x;
    myMouseYPosition = e->win_y;
    myMoveOnRightClick = true;
}


bool
GUIDanielPerspectiveChanger::onRightBtnRelease(void*data)
{
    myMouseButtonState =
        (MouseState)((int) myMouseButtonState & (255-(int) MOUSEBTN_RIGHT));
    if (data!=0) {
        FXEvent* e = (FXEvent*) data;
        myMouseXPosition = e->win_x;
        myMouseYPosition = e->win_y;
    }
    if (myMoveOnRightClick) {
        myMoveOnRightClick = false;
        return true;
    }
    return false;
}


void
GUIDanielPerspectiveChanger::onMouseMove(void*data)
{
    FXEvent* e = (FXEvent*) data;
    myCallback.setTooltipPosition(e->win_x, e->win_y, e->root_x, e->root_y);
    int xdiff = myMouseXPosition - e->win_x;
    int ydiff = myMouseYPosition - e->win_y;
    switch (myMouseButtonState) {
    case MOUSEBTN_LEFT:
        move(xdiff, ydiff);
        break;
    case MOUSEBTN_RIGHT:
        zoom(ydiff);
        rotate(xdiff);
        if (abs(xdiff)>0||abs(ydiff)>0) {
            myMoveOnRightClick = false;
        }
        break;
    default:
        if (xdiff!=0||ydiff!=0) {
            myCallback.updateToolTip();
        }
        break;
    }
    myMouseXPosition = e->win_x;
    myMouseYPosition = e->win_y;
}


void
GUIDanielPerspectiveChanger::setViewport(SUMOReal zoom,
        SUMOReal xPos, SUMOReal yPos)
{
    myZoom = zoom;
    myViewCenter.set(xPos, yPos);
}



/****************************************************************************/

