/****************************************************************************/
/// @file    GUIUserIO.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2006-10-12
/// @version $Id$
///
// Some OS-dependant functions to ease cliboard manipulation
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

#include "GUIUserIO.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


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

/****************************************************************************/

