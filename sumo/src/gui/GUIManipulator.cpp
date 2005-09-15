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
// Revision 1.1  2005/09/15 11:05:28  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/09/09 12:49:59  dksumo
// complete code rework: debug_new and config added
//
// Revision 1.1  2005/08/01 12:56:40  dksumo
// structure of moanipulators changed
//
// Revision 1.2  2005/04/26 07:06:31  dksumo
// SUMOTime inserted; level3 warnings patched
//
// Revision 1.1  2004/10/22 12:49:09  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.1  2004/07/02 08:24:33  dkrajzew
// possibility to manipulate vss in the gui added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <fx.h>
#include <string>
#include <gui/GUIApplicationWindow.h>
#include "GUIManipulator.h"
#include <gui/GUIGlobals.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * FOX callback mapping
 * ======================================================================= */
FXIMPLEMENT(GUIManipulator, FXDialogBox, NULL, 0)


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIManipulator::GUIManipulator(GUIMainWindow &app,
                               const std::string &name,
                               int xpos, int ypos)
    : FXDialogBox(&app, name.c_str(), DECOR_CLOSE|DECOR_TITLE, xpos, ypos, 0, 0)
{
}


GUIManipulator::~GUIManipulator()
{
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

