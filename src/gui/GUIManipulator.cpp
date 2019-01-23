/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIManipulator.cpp
/// @author  Daniel Krajzewicz
/// @date    Jun 2004
/// @version $Id$
///
// Abstract GUI manipulation class
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fx.h>
#include <string>
#include <gui/GUIApplicationWindow.h>
#include "GUIManipulator.h"
#include <gui/GUIGlobals.h>
#include <utils/gui/div/GUIDesigns.h>


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXIMPLEMENT(GUIManipulator, FXDialogBox, NULL, 0)


// ===========================================================================
// method definitions
// ===========================================================================
GUIManipulator::GUIManipulator(GUIMainWindow& app, const std::string& name, int xpos, int ypos) :
    FXDialogBox(&app, name.c_str(), GUIDesignDialogBox, xpos, ypos, 0, 0) {}


GUIManipulator::~GUIManipulator() {}


/****************************************************************************/

