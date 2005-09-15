//---------------------------------------------------------------------------//
//                        GUIGlObject.cpp -
//  Base class for all objects that may be displayed within the openGL-gui
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
// Revision 1.2  2005/09/15 12:19:44  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2004/11/23 10:38:31  dkrajzew
// debugging
//
// Revision 1.1  2004/10/22 12:50:50  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.11  2004/03/19 12:54:07  dkrajzew
// porting to FOX
//
// Revision 1.10  2003/11/18 14:28:14  dkrajzew
// debugged and completed lane merging detectors
//
// Revision 1.9  2003/11/12 14:07:46  dkrajzew
// clean up after recent changes
//
// Revision 1.8  2003/07/30 08:52:16  dkrajzew
// further work on visualisation of all geometrical objects
//
// Revision 1.7  2003/07/22 14:56:46  dkrajzew
// changes due to new detector handling
//
// Revision 1.6  2003/06/06 10:28:45  dkrajzew
// new subfolder holding popup-menus was added due to link-dependencies
//  under linux; QGLObjectPopupMenu*-classes were moved to "popup"
//
// Revision 1.5  2003/06/05 11:37:30  dkrajzew
// class templates applied
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <stack>
#include <utils/convert/ToString.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include "GUIGlObject.h"
#include "GUIGlObjectStorage.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIGlObject::GUIGlObject(GUIGlObjectStorage &idStorage,
                         std::string fullName)
    : myFullName(fullName)
{
    idStorage.registerObject(this);
}


GUIGlObject::GUIGlObject(GUIGlObjectStorage &idStorage,
                         std::string fullName, size_t glID)
    : myFullName(fullName)
{
    idStorage.registerObject(this, glID);
}


GUIGlObject::~GUIGlObject()
{
}


const std::string &
GUIGlObject::getFullName() const
{
    return myFullName;
}


size_t
GUIGlObject::getGlID() const
{
    return myGlID;
}


void
GUIGlObject::setGlID(size_t id)
{
    myGlID = id;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


