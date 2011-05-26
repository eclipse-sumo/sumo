/****************************************************************************/
/// @file    GUIPointOfInterest.cpp
/// @author  Daniel Krajzewicz
/// @date    June 2006
/// @version $Id$
///
// The GUI-version of a point of interest
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include "GUIPointOfInterest.h"
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>
#include <utils/gui/div/GLHelper.h>
#include <foreign/polyfonts/polyfonts.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUIPointOfInterest::GUIPointOfInterest(int layer,
                                       const std::string &id,
                                       const std::string &type,
                                       const Position &p,
                                       const RGBColor &c) throw() : 
    PointOfInterest(id, type, p, c),
    GUIGlObject_AbstractAdd("poi", GLO_SHAPE, id),
    myLayer(layer) {}


GUIPointOfInterest::~GUIPointOfInterest() throw() {}


GUIGLObjectPopupMenu *
GUIPointOfInterest::getPopUpMenu(GUIMainWindow &app,
                                 GUISUMOAbstractView &parent) throw() {

    GUIGLObjectPopupMenu *ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app, false);
    FXString t(myType.c_str());
    new FXMenuCommand(ret, "(" + t + ")", 0, 0, 0);
    new FXMenuSeparator(ret);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    return ret;
}


GUIParameterTableWindow *
GUIPointOfInterest::getParameterWindow(GUIMainWindow &,
                                       GUISUMOAbstractView &) throw() {
    return 0;
}


Boundary
GUIPointOfInterest::getCenteringBoundary() const throw() {
    Boundary b;
    b.add(x(), y());
    b.grow(10);
    return b;
}


void
GUIPointOfInterest::drawGL(const GUIVisualizationSettings &s) const throw() {
    if (s.scale*(1.3/3.0)<s.minPOISize) {
        return;
    }
    glPushName(getGlID());
    glPushMatrix();
    glColor3d(red(),green(),blue());
    glTranslated(x(), y(), getLayer());
    GLHelper::drawFilledCircle((SUMOReal) 1.3*s.poiExaggeration, 16);
    glPopMatrix();
    if (s.drawPOIName) {
        drawName(Position(x() + 1.32*s.poiExaggeration, y() + 1.32*s.poiExaggeration),
                s.poiNameSize / s.scale, s.poiNameColor);
    }
    glPopName();
}


int
GUIPointOfInterest::getLayer() const {
    return myLayer;
}



/****************************************************************************/

