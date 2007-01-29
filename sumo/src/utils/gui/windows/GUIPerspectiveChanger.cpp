//---------------------------------------------------------------------------//
//                        GUIPerspectiveChanger.cpp -
//  A class that allows to steer the visual output in dependence to user
//      interaction
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
// Revision 1.6  2006/11/16 12:30:55  dkrajzew
// warnings removed
//
// Revision 1.5  2006/07/06 05:54:11  dkrajzew
// refactoring
//
// Revision 1.4  2005/10/07 11:46:08  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.3  2005/09/15 12:20:19  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/05/04 09:24:43  dkrajzew
// entries for viewport definition added; popups now popup faster
//
// Revision 1.1  2004/11/23 10:38:32  dkrajzew
// debugging
//
// Revision 1.1  2004/10/22 12:50:56  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.5  2004/03/19 12:54:08  dkrajzew
// porting to FOX
//
// Revision 1.4  2003/05/20 09:23:54  dkrajzew
// some statistics added; some debugging done
//
// Revision 1.3  2003/04/04 08:37:50  dkrajzew
// view centering now applies net size; closing problems debugged;
//  comments added; tootip button added
//
// Revision 1.2  2003/02/07 10:34:14  dkrajzew
// files updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "GUISUMOAbstractView.h"
#include "GUIPerspectiveChanger.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIPerspectiveChanger::GUIPerspectiveChanger(GUISUMOAbstractView &callBack)
    : myCallback(callBack), myHaveChanged(true)
{
}


GUIPerspectiveChanger::~GUIPerspectiveChanger()
{
}


bool
GUIPerspectiveChanger::changed() const
{
    return myHaveChanged;
}


void
GUIPerspectiveChanger::otherChange()
{
    myHaveChanged = true;
}


void
GUIPerspectiveChanger::applied()
{
    myHaveChanged = false;
}


void
GUIPerspectiveChanger::setNetSizes(size_t width, size_t height)
{
    myNetWidth = width;
    myNetHeight = height;
}


void
GUIPerspectiveChanger::applyCanvasSize(size_t width, size_t height)
{
    myCanvasWidth = width;
    myCanvasHeight = height;
    otherChange();
}


void
GUIPerspectiveChanger::onLeftBtnPress(void*)
{
}


void
GUIPerspectiveChanger::onLeftBtnRelease(void*)
{
}


void
GUIPerspectiveChanger::onRightBtnPress(void*)
{
}


bool
GUIPerspectiveChanger::onRightBtnRelease(void*)
{
    return false;
}


void
GUIPerspectiveChanger::onMouseMove(void *)
{
}


int
GUIPerspectiveChanger::getMouseXPosition() const
{
    return myMouseXPosition;
}


int
GUIPerspectiveChanger::getMouseYPosition() const
{
    return myMouseYPosition;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


