/***************************************************************************
                          GUIPointOfInterest.cpp
    The GUI-version of a point of interest
                             -------------------
    project              : SUMO - Simulation of Urban MObility
    begin                : June 2006
    copyright            : (C) 2006 by Daniel Krajzewicz
    organisation         : IVF/DLR http://ivf.dlr.de
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.9  2006/11/22 13:06:46  dkrajzew
// patching problems on choosing an object when using shapes within different layers
//
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

#include "GUIPointOfInterest.h"
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIPointOfInterest::GUIPointOfInterest(GUIGlObjectStorage &idStorage,
                                       int layer,
                                       const std::string &id,
                                       const std::string &type,
                                       const Position2D &p,
                                       const RGBColor &c)
    : PointOfInterest(id, type, p, c),
    GUIGlObject(idStorage, "poi:"+id), myLayer(layer)
{
}


GUIPointOfInterest::~GUIPointOfInterest()
{
}


GUIGLObjectPopupMenu *
GUIPointOfInterest::getPopUpMenu(GUIMainWindow &app,
                                 GUISUMOAbstractView &parent)
{

    GUIGLObjectPopupMenu *ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    // manipulation
    new FXMenuCommand(ret, "Rename ", GUIIconSubSys::getIcon(ICON_MANIP), ret, MID_RENAME);
    new FXMenuCommand(ret, "Move To ", GUIIconSubSys::getIcon(ICON_MANIP), ret, MID_MOVETO);
    new FXMenuCommand(ret, "Change Color ", GUIIconSubSys::getIcon(ICON_MANIP), ret, MID_CHANGECOL);
    new FXMenuCommand(ret, "Change Type", GUIIconSubSys::getIcon(ICON_MANIP), ret, MID_CHANGETYPE);
    new FXMenuCommand(ret, "Delete", GUIIconSubSys::getIcon(ICON_CLOSING), ret, MID_DELETE);
    new FXMenuSeparator(ret);
    //
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret, false);
    return ret;
}


GUIParameterTableWindow *
GUIPointOfInterest::getParameterWindow(GUIMainWindow &,
                                       GUISUMOAbstractView &)
{
    return 0;
}


GUIGlObjectType
GUIPointOfInterest::getType() const
{
    return GLO_SHAPE;
}


const std::string &
GUIPointOfInterest::microsimID() const
{
    return getID();
}


bool
GUIPointOfInterest::active() const
{
    return true;
}


Boundary
GUIPointOfInterest::getCenteringBoundary() const
{
    Boundary b;
    b.add(x(), y());
    b.grow(10);
    return b;
}


int
GUIPointOfInterest::getLayer() const
{
    return myLayer;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

