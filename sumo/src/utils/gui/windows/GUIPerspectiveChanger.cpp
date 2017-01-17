/****************************************************************************/
/// @file    GUIPerspectiveChanger.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A class that allows to steer the visual output in dependence to user
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

#include "GUISUMOAbstractView.h"
#include "GUIPerspectiveChanger.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUIPerspectiveChanger::GUIPerspectiveChanger(GUISUMOAbstractView& callBack, const Boundary& viewPort) :
    myCallback(callBack),
    myViewPort(viewPort) {
}


GUIPerspectiveChanger::~GUIPerspectiveChanger() {
}


void
GUIPerspectiveChanger::onLeftBtnPress(void*) {
}


bool
GUIPerspectiveChanger::onLeftBtnRelease(void*) {
    return false;
}


void
GUIPerspectiveChanger::onRightBtnPress(void*) {
}


bool
GUIPerspectiveChanger::onRightBtnRelease(void*) {
    return false;
}

void
GUIPerspectiveChanger::onDoubleClicked(void*) {
}


void
GUIPerspectiveChanger::onMouseWheel(void*) {
}


void
GUIPerspectiveChanger::onMouseMove(void*) {
}


long
GUIPerspectiveChanger::onKeyPress(void*) {
    return 0;
}


long
GUIPerspectiveChanger::onKeyRelease(void*) {
    return 0;
}


FXint
GUIPerspectiveChanger::getMouseXPosition() const {
    return myMouseXPosition;
}


FXint
GUIPerspectiveChanger::getMouseYPosition() const {
    return myMouseYPosition;
}


Boundary
GUIPerspectiveChanger::getViewport(bool fixRatio) {
    if (fixRatio) {
        return patchedViewPort();
    } else {
        return myViewPort;
    }
}


void
GUIPerspectiveChanger::setViewport(const Boundary& viewPort) {
    myViewPort = viewPort;
}


Boundary
GUIPerspectiveChanger::patchedViewPort() {
    // avoid division by zero
    if (myCallback.getHeight() == 0 ||
            myCallback.getWidth() == 0 ||
            myViewPort.getHeight() == 0 ||
            myViewPort.getWidth() == 0) {
        return myViewPort;
    }
    Boundary result = myViewPort;
    SUMOReal canvasRatio = (SUMOReal)myCallback.getWidth() / myCallback.getHeight();
    SUMOReal ratio = result.getWidth() / result.getHeight();
    if (ratio < canvasRatio) {
        result.growWidth(result.getWidth() * (canvasRatio / ratio - 1) / 2);
    } else {
        result.growHeight(result.getHeight() * (ratio / canvasRatio - 1) / 2);
    }
    return result;
}

/****************************************************************************/

