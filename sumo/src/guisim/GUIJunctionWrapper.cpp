/****************************************************************************/
/// @file    GUIJunctionWrapper.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 1 Jul 2003
/// @version $Id$
///
// }
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
#include <utility>
#include <microsim/MSLane.h>
#include <utils/geom/Position2D.h>
#include <microsim/MSNet.h>
#include <gui/GUIApplicationWindow.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include "GUIJunctionWrapper.h"
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GLHelper.h>
#include <foreign/polyfonts/polyfonts.h>

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
GUIJunctionWrapper::GUIJunctionWrapper(GUIGlObjectStorage &idStorage,
                                       MSJunction &junction,
                                       const Position2DVector &shape) throw()
        : GUIGlObject(idStorage, "junction:"+junction.getID()),
        myJunction(junction), myShape(shape)
{
    Boundary b = myShape.getBoxBoundary();
    myMaxSize = MAX2(b.getWidth(), b.getHeight());
}


GUIJunctionWrapper::~GUIJunctionWrapper() throw()
{}


GUIGLObjectPopupMenu *
GUIJunctionWrapper::getPopUpMenu(GUIMainWindow &app,
                                 GUISUMOAbstractView &parent) throw()
{
    GUIGLObjectPopupMenu *ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    return ret;
}


GUIParameterTableWindow *
GUIJunctionWrapper::getParameterWindow(GUIMainWindow &app,
                                       GUISUMOAbstractView &) throw()
{
    return 0;
}


const std::string &
GUIJunctionWrapper::getMicrosimID() const throw()
{
    return myJunction.getID();
}


Boundary
GUIJunctionWrapper::getBoundary() const
{
    Boundary boundary;
    int shapeLength = (int) myShape.size();
    for (int i=0; i<shapeLength; ++i) {
        const Position2D &pos = myShape[i];
        boundary.add(pos.x(), pos.y());
    }
    return boundary;
}


const Position2DVector &
GUIJunctionWrapper::getShape() const
{
    return myShape;
}


Boundary
GUIJunctionWrapper::getCenteringBoundary() const throw()
{
    Boundary b = getBoundary();
    b.grow(20);
    return b;
}

MSJunction &
GUIJunctionWrapper::getJunction() const
{
    return myJunction;
}

void
GUIJunctionWrapper::drawGL(const GUIVisualizationSettings &s) const throw()
{
    // check whether it is not too small
    if (s.scale*myMaxSize<1.) {
        return;
    }
    // (optional) set id
    if (s.needsGlID) {
        glPushName(getGlID());
    }
    glColor3f(0, 0, 0);
    glPolygonOffset(0, 1);
    GLHelper::drawFilledPoly(getShape(), true);
    // (optional) draw name
    if (s.drawJunctionName) {
        glPolygonOffset(0, -6);
        glPushMatrix();
        Position2D p = myJunction.getPosition();
        glTranslated(p.x(), p.y(), 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        pfSetPosition(0, 0);
        pfSetScale(s.junctionNameSize / s.scale);
        glColor3f(s.junctionNameColor.red(), s.junctionNameColor.green(), s.junctionNameColor.blue());
        SUMOReal w = pfdkGetStringWidth(getMicrosimID().c_str());
        glRotated(180, 1, 0, 0);
        glTranslated(-w/2., 0.4, 0);
        pfDrawString(getMicrosimID().c_str());
        glPopMatrix();
    }
    // (optional) clear id
    if (s.needsGlID) {
        glPopName();
    }
}


/****************************************************************************/

