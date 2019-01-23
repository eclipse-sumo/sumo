/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIUserIO.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2006-10-12
/// @version $Id$
///
// Some OS-dependant functions to ease cliboard manipulation
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "GUIUserIO.h"


// ===========================================================================
// static member definitions
// ===========================================================================
std::string GUIUserIO::clipped = "";


// ===========================================================================
// method definitions
// ===========================================================================
void
GUIUserIO::copyToClipboard(const FXApp& app, const std::string& text) {
    FXDragType types[] = {FXWindow::stringType, FXWindow::textType};
    if (app.getActiveWindow()->acquireClipboard(types, 2)) {
        clipped = text;
    }
}

std::string
GUIUserIO::copyFromClipboard(const FXApp& app) {
    FXString string;
    if (app.getActiveWindow()->getDNDData(FROM_CLIPBOARD, FXWindow::utf8Type, string)) {
        return string.text();
    };
    return string.text();
}

/****************************************************************************/

