/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GUIPerspectiveChanger.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// A class that allows to steer the visual output in dependence to user
/****************************************************************************/
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
    // reimplement in child
}


bool
GUIPerspectiveChanger::onLeftBtnRelease(void*) {
    // reimplement in child
    return false;
}


void
GUIPerspectiveChanger::onMiddleBtnPress(void*) {
    // reimplement in child
}


bool
GUIPerspectiveChanger::onMiddleBtnRelease(void*) {
    // reimplement in child
    return false;
}


void
GUIPerspectiveChanger::onRightBtnPress(void*) {
    // reimplement in child
}


bool
GUIPerspectiveChanger::onRightBtnRelease(void*) {
    // reimplement in child
    return false;
}

void
GUIPerspectiveChanger::onDoubleClicked(void*) {
    // reimplement in child
}


void
GUIPerspectiveChanger::onMouseWheel(void*) {
    // reimplement in child
}


void
GUIPerspectiveChanger::onMouseMove(void*) {
    // reimplement in child
}


long
GUIPerspectiveChanger::onKeyPress(void*) {
    // reimplement in child
    return 0;
}


long
GUIPerspectiveChanger::onKeyRelease(void*) {
    // reimplement in child
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
