/****************************************************************************/
/// @file    GUIManipulator.cpp
/// @author  Daniel Krajzewicz
/// @date    Jun 2004
/// @version $Id: $
///
// Abstract GUI manipulation class
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <string>
#include <gui/GUIApplicationWindow.h>
#include "GUIManipulator.h"
#include <gui/GUIGlobals.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXIMPLEMENT(GUIManipulator, FXDialogBox, NULL, 0)


// ===========================================================================
// method definitions
// ===========================================================================
GUIManipulator::GUIManipulator(GUIMainWindow &app,
                               const std::string &name,
                               int xpos, int ypos)
        : FXDialogBox(&app, name.c_str(), DECOR_CLOSE|DECOR_TITLE, xpos, ypos, 0, 0)
{}


GUIManipulator::~GUIManipulator()
{}



/****************************************************************************/

