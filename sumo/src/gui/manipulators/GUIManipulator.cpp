//---------------------------------------------------------------------------//
//                        GUIManipulator.cpp -
//  Abstract GUI manipulation class
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Jun 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.1  2004/07/02 08:24:33  dkrajzew
// possibility to manipulate vss in the gui added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <fx.h>
#include <string>
#include <gui/GUIGlObject_AAManipulatable.h>
#include <gui/GUIApplicationWindow.h>
#include "GUIManipulator.h"
#include <gui/GUIGlobals.h>


/* =========================================================================
 * FOX callback mapping
 * ======================================================================= */
/*
FXDEFMAP(GUIManipulator) GUIManipulatorMap[]=
{
    FXMAPFUNC(SEL_COMMAND,  MID_SETTINGS_OK, GUIManipulator::onCmdClose),
};

FXIMPLEMENT(GUIManipulator, FXDialogBox, GUIManipulatorMap, ARRAYNUMBER(GUIManipulatorMap))
*/
FXIMPLEMENT(GUIManipulator, FXDialogBox, NULL, 0)


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIManipulator::GUIManipulator(GUIApplicationWindow &app,
                               const std::string &name,
                               GUIGlObject_AAManipulatable &o,
                               int xpos, int ypos)
    : FXDialogBox(&app, name.c_str(), DECOR_ALL),
    myObject(&o)
{
}


GUIManipulator::~GUIManipulator()
{
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

