/****************************************************************************/
/// @file    GUIUserIO.cpp
/// @author  Daniel Krajzewicz
/// @date    2006-10-12
/// @version $Id$
///
// Some OS-dependant functions to ease cliboard manipulation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
GUIUserIO::copyToClipboard(const FXApp &app, const std::string &text) throw() {
    FXDragType types[] = {FXWindow::stringType, FXWindow::textType};
    if (app.getActiveWindow()->acquireClipboard(types,2)) {
        clipped = text;
    }
}

/****************************************************************************/

