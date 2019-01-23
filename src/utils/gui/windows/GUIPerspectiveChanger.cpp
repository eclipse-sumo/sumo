/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "GUISUMOAbstractView.h"
#include "GUIPerspectiveChanger.h"


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
    double canvasRatio = (double)myCallback.getWidth() / myCallback.getHeight();
    double ratio = result.getWidth() / result.getHeight();
    if (ratio < canvasRatio) {
        result.growWidth(result.getWidth() * (canvasRatio / ratio - 1) / 2);
    } else {
        result.growHeight(result.getHeight() * (ratio / canvasRatio - 1) / 2);
    }
    return result;
}

/****************************************************************************/

