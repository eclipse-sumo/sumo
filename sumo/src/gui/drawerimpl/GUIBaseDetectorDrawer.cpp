//---------------------------------------------------------------------------//
//                        GUIBaseDetectorDrawer.cpp -
//  Base class for detector drawing
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Wed, 14.Jan 2004
//  copyright            : (C) 2003 by Daniel Krajzewicz
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
// Revision 1.7  2005/09/15 11:05:28  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/04/27 09:44:26  dkrajzew
// level3 warnings removed
//
// Revision 1.5  2004/11/23 10:05:21  dkrajzew
// removed some warnings and adapted the new class hierarchy
//
// Revision 1.4  2004/07/02 08:12:11  dkrajzew
// detector drawers now also draw other additional items
//
// Revision 1.3  2004/03/19 12:34:30  dkrajzew
// porting to FOX
//
// Revision 1.2  2004/02/16 13:54:39  dkrajzew
// tried to patch a sometimes occuring visualisation bug
//
// Revision 1.1  2004/01/26 06:39:41  dkrajzew
// visualisation of e3-detectors added; documentation added
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

#include <utils/gui/windows/GUISUMOAbstractView.h>
#include "GUIBaseDetectorDrawer.h"
#include <guisim/GUIDetectorWrapper.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIBaseDetectorDrawer::GUIBaseDetectorDrawer(
        const std::vector<GUIGlObject_AbstractAdd*> &additionals)
    : myAdditionals(additionals)
{
}


GUIBaseDetectorDrawer::~GUIBaseDetectorDrawer()
{
}


void
GUIBaseDetectorDrawer::drawGLDetectors(size_t *which,
                                       size_t maxDetectors,
                                       double scale)
{
    initStep();
    myDrawGLDetectors(which, maxDetectors, scale);
}


void
GUIBaseDetectorDrawer::initStep()
{
    glLineWidth(1);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
