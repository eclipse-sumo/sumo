/***************************************************************************
                          GUIPolygon2D.cpp
    The GUI-version of a polygon
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

#include <string>
#include "GUIPolygon2D.h"
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>

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
GUIPolygon2D::GUIPolygon2D(GUIGlObjectStorage &idStorage,
                           int layer,
                           const std::string name, const std::string type,
                           const RGBColor &color,
                           const Position2DVector &Pos,
                           bool fill)
    : Polygon2D(name, type, color, Pos, fill),
    GUIGlObject(idStorage, "poly:"+name), myLayer(layer)
{
}


GUIPolygon2D::~GUIPolygon2D()
{
}



GUIGLObjectPopupMenu *
GUIPolygon2D::getPopUpMenu(GUIMainWindow &app,
                                 GUISUMOAbstractView &parent)
{
    GUIGLObjectPopupMenu *ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret, false);
    return ret;
}


GUIParameterTableWindow *
GUIPolygon2D::getParameterWindow(GUIMainWindow &,
                                       GUISUMOAbstractView &)
{
    return 0;
}


GUIGlObjectType
GUIPolygon2D::getType() const
{
    return GLO_SHAPE;
}


const std::string &
GUIPolygon2D::microsimID() const
{
    return myName;
}


bool
GUIPolygon2D::active() const
{
    return true;
}


Boundary
GUIPolygon2D::getCenteringBoundary() const
{
    Boundary b;
    b.add(myPos.getBoxBoundary());
    b.grow(10);
    return b;
}


int
GUIPolygon2D::getLayer() const
{
    return myLayer;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

