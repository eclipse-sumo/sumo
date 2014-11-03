/****************************************************************************/
/// @file    GUIDanielPerspectiveChanger.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A class that allows to steer the visual output in dependence to
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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

#include <utils/geom/Boundary.h>
#include <utils/geom/Position.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include "GUIPerspectiveChanger.h"
#include "GUIDanielPerspectiveChanger.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUIDanielPerspectiveChanger::GUIDanielPerspectiveChanger(
    GUISUMOAbstractView& callBack, const Boundary& viewPort) :
    GUIPerspectiveChanger(callBack, viewPort),
    myOrigWidth(viewPort.getWidth()),
    myOrigHeight(viewPort.getHeight()),
    myRotation(0),
    myMouseButtonState(MOUSEBTN_NONE),
    myMoveOnClick(false),
    myDragDelay(0) {}


GUIDanielPerspectiveChanger::~GUIDanielPerspectiveChanger() {}


void
GUIDanielPerspectiveChanger::move(int xdiff, int ydiff) {
    myViewPort.moveby(myCallback.p2m(xdiff), -myCallback.p2m(ydiff));
    myCallback.update();
}


void
GUIDanielPerspectiveChanger::zoom(SUMOReal factor) {
    if (myCallback.getApp()->reg().readIntEntry("gui", "zoomAtCenter", 1)) {
        myZoomBase = myViewPort.getCenter();
    }
    if (factor > 0) {
        myViewPort = Boundary(
                         myZoomBase.x() - (myZoomBase.x() - myViewPort.xmin()) / factor,
                         myZoomBase.y() - (myZoomBase.y() - myViewPort.ymin()) / factor,
                         myZoomBase.x() - (myZoomBase.x() - myViewPort.xmax()) / factor,
                         myZoomBase.y() - (myZoomBase.y() - myViewPort.ymax()) / factor);
        myCallback.update();
    }
}


void
GUIDanielPerspectiveChanger::rotate(int /* diff */) {
    /*
    if (myCallback.allowRotation()) {
        myRotation += (SUMOReal) diff / (SUMOReal) 10.0;
        myCallback.update();
    }
    */
}


SUMOReal
GUIDanielPerspectiveChanger::getRotation() const {
    return myRotation;
}


SUMOReal
GUIDanielPerspectiveChanger::getXPos() const {
    return myViewPort.getCenter().x();
}


SUMOReal
GUIDanielPerspectiveChanger::getYPos() const {
    return myViewPort.getCenter().y();
}


SUMOReal
GUIDanielPerspectiveChanger::getZoom() const {
    return myOrigWidth / myViewPort.getWidth() * 100;
}


void
GUIDanielPerspectiveChanger::centerTo(const Position& pos, SUMOReal radius,
                                      bool applyZoom) {
    if (applyZoom) {
        myViewPort = Boundary();
        myViewPort.add(pos);
        myViewPort.grow(radius);
    } else {
        myViewPort.moveby(pos.x() - getXPos(), pos.y() - getYPos());
    }
}


void
GUIDanielPerspectiveChanger::onLeftBtnPress(void* data) {
    myMouseButtonState |= MOUSEBTN_LEFT;
    FXEvent* e = (FXEvent*) data;
    myMouseXPosition = e->win_x;
    myMouseYPosition = e->win_y;
    myMoveOnClick = false;
    myMouseDownTime = FXThread::time();
}


bool
GUIDanielPerspectiveChanger::onLeftBtnRelease(void* data) {
    myMouseButtonState &= !MOUSEBTN_LEFT;
    FXEvent* e = (FXEvent*) data;
    myMouseXPosition = e->win_x;
    myMouseYPosition = e->win_y;
    return myMoveOnClick;
}


void
GUIDanielPerspectiveChanger::onRightBtnPress(void* data) {
    myMouseButtonState |= MOUSEBTN_RIGHT;
    FXEvent* e = (FXEvent*) data;
    myMouseXPosition = e->win_x;
    myMouseYPosition = e->win_y;
    myMoveOnClick = false;
    myMouseDownTime = FXThread::time();
    myZoomBase = myCallback.getPositionInformation();
}


bool
GUIDanielPerspectiveChanger::onRightBtnRelease(void* data) {
    myMouseButtonState &= !MOUSEBTN_RIGHT;
    if (data != 0) {
        FXEvent* e = (FXEvent*) data;
        myMouseXPosition = e->win_x;
        myMouseYPosition = e->win_y;
    }
    return myMoveOnClick;
}


void
GUIDanielPerspectiveChanger::onMouseWheel(void* data) {
    FXEvent* e = (FXEvent*) data;
    SUMOReal diff = 0.1;
    if (e->state & CONTROLMASK) {
        diff /= 2;
    } else if (e->state & SHIFTMASK) {
        diff *= 2;
    }
    if (e->code < 0) {
        diff = -diff;
    }
    myZoomBase = myCallback.getPositionInformation();
    zoom(1.0 + diff);
    myCallback.updateToolTip();
}


void
GUIDanielPerspectiveChanger::onMouseMove(void* data) {
    FXEvent* e = (FXEvent*) data;
    myCallback.setWindowCursorPosition(e->win_x, e->win_y);
    const int xdiff = myMouseXPosition - e->win_x;
    const int ydiff = myMouseYPosition - e->win_y;
    const bool moved = xdiff != 0 || ydiff != 0;
    const bool pastDelay = !gSchemeStorage.getDefault().gaming && FXThread::time() > (myMouseDownTime + myDragDelay);
    switch (myMouseButtonState) {
        case MOUSEBTN_LEFT:
            if (pastDelay) {
                move(xdiff, ydiff);
                if (moved) {
                    myMoveOnClick = true;
                }
            }
            break;
        case MOUSEBTN_RIGHT:
            if (pastDelay) {
                zoom(1 + 10.0 * ydiff / myCallback.getWidth());
                rotate(xdiff);
                if (moved) {
                    myMoveOnClick = true;
                }
            }
            break;
        default:
            if (moved) {
                myCallback.updateToolTip();
            }
            break;
    }
    myMouseXPosition = e->win_x;
    myMouseYPosition = e->win_y;
}


void
GUIDanielPerspectiveChanger::setViewport(SUMOReal zoom,
        SUMOReal xPos, SUMOReal yPos) {
    const SUMOReal zoomFactor = zoom / 50; // /100 to normalize, *2 because growth is added on both sides
    myViewPort = Boundary();
    myViewPort.add(Position(xPos, yPos));
    myViewPort.growHeight(myOrigHeight / zoomFactor);
    myViewPort.growWidth(myOrigWidth / zoomFactor);
    myCallback.update();
}


void
GUIDanielPerspectiveChanger::changeCanvassLeft(int change) {
    myViewPort = Boundary(
                     myViewPort.xmin() - myCallback.p2m(change),
                     myViewPort.ymin(),
                     myViewPort.xmax(),
                     myViewPort.ymax());
}

/****************************************************************************/
