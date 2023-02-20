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
/// @file    GUIPersistentWindowPos.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Thu, 11.03.2004
///
// Editor for the list of chosen objects
/****************************************************************************/
#include <config.h>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <utils/common/MsgHandler.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/foxtools/MFXUtils.h>
#include "GUIPersistentWindowPos.h"
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/images/GUIIconSubSys.h>



// ===========================================================================
// method definitions
// ===========================================================================

GUIPersistentWindowPos::GUIPersistentWindowPos(FXWindow* parent, const std::string& name, bool storeSize,
        int x, int y,
        int width, int height,
        int minSize, int minTitlebarHeight) :
    myParent(parent),
    myWindowName(name),
    myStoreSize(storeSize),
    myDefaultX(x),
    myDefaultY(y),
    myDefaultWidth(width),
    myDefaultHeight(height),
    myMinSize(minSize),
    myMinTitlebarHeight(minTitlebarHeight)
{}

GUIPersistentWindowPos::GUIPersistentWindowPos() :
    myParent(nullptr)
{ }

GUIPersistentWindowPos::~GUIPersistentWindowPos() {
    saveWindowPos();
}


void
GUIPersistentWindowPos::saveWindowPos() {
    if (myParent != nullptr) {
        FXRegistry& reg = myParent->getApp()->reg();
        reg.writeIntEntry(myWindowName.c_str(), "x", myParent->getX());
        reg.writeIntEntry(myWindowName.c_str(), "y", myParent->getY());
        if (myStoreSize) {
            reg.writeIntEntry(myWindowName.c_str(), "width", myParent->getWidth());
            reg.writeIntEntry(myWindowName.c_str(), "height", myParent->getHeight());
        }
    }
}

void
GUIPersistentWindowPos::loadWindowPos() {
    if (myParent != nullptr) {
        FXRegistry& reg = myParent->getApp()->reg();
        // ensure window is visible after switching screen resolutions
        myParent->setX(MAX2(0, MIN2(reg.readIntEntry(myWindowName.c_str(), "x", myDefaultX),
                                    myParent->getApp()->getRootWindow()->getWidth() - myMinSize)));
        myParent->setY(MAX2(myMinTitlebarHeight,
                            MIN2(reg.readIntEntry(myWindowName.c_str(), "y", myDefaultY),
                                 myParent->getApp()->getRootWindow()->getHeight() - myMinSize)));
        if (myStoreSize) {
            myParent->setWidth(MAX2(reg.readIntEntry(myWindowName.c_str(), "width", myDefaultWidth), myMinSize));
            myParent->setHeight(MAX2(reg.readIntEntry(myWindowName.c_str(), "height", myDefaultHeight), myMinSize));
        }
    }
}

/****************************************************************************/
