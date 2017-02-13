/****************************************************************************/
/// @file    GUIManipulator.cpp
/// @author  Daniel Krajzewicz
/// @date    Jun 2004
/// @version $Id$
///
// Abstract GUI manipulation class
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

#include <fx.h>
#include <string>
#include <gui/GUIApplicationWindow.h>
#include "GUIManipulator.h"
#include <gui/GUIGlobals.h>
#include <utils/gui/div/GUIDesigns.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


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

