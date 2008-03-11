/****************************************************************************/
/// @file    GUIPolygon2D.cpp
/// @author  Daniel Krajzewicz
/// @date    June 2006
/// @version $Id$
///
// The GUI-version of a polygon
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "GUIPolygon2D.h"
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
GUIPolygon2D::GUIPolygon2D(GUIGlObjectStorage &idStorage,
                           int layer,
                           const std::string name, const std::string type,
                           const RGBColor &color,
                           const Position2DVector &Pos,
                           bool fill) throw()
        : Polygon2D(name, type, color, Pos, fill),
        GUIGlObject(idStorage, "poly:"+name), myLayer(layer)
{}


GUIPolygon2D::~GUIPolygon2D() throw()
{}



GUIGLObjectPopupMenu *
GUIPolygon2D::getPopUpMenu(GUIMainWindow &app,
                           GUISUMOAbstractView &parent) throw()
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
                                 GUISUMOAbstractView &) throw()
{
    return 0;
}


GUIGlObjectType
GUIPolygon2D::getType() const throw()
{
    return GLO_SHAPE;
}


const std::string &
GUIPolygon2D::microsimID() const throw()
{
    return myName;
}


Boundary
GUIPolygon2D::getCenteringBoundary() const throw()
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



/****************************************************************************/

