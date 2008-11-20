/****************************************************************************/
/// @file    GUIPointOfInterest.cpp
/// @author  Daniel Krajzewicz
/// @date    June 2006
/// @version $Id$
///
// The GUI-version of a point of interest
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

#include "GUIPointOfInterest.h"
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIVisualizationSettings.h>
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
GUIPointOfInterest::GUIPointOfInterest(GUIGlObjectStorage &idStorage,
                                       int layer,
                                       const std::string &id,
                                       const std::string &type,
                                       const Position2D &p,
                                       const RGBColor &c) throw()
        : PointOfInterest(id, type, p, c),
        GUIGlObject_AbstractAdd(idStorage, "poi:"+id, GLO_SHAPE), myLayer(layer)
{}


GUIPointOfInterest::~GUIPointOfInterest() throw()
{}


GUIGLObjectPopupMenu *
GUIPointOfInterest::getPopUpMenu(GUIMainWindow &app,
                                 GUISUMOAbstractView &parent) throw()
{

    GUIGLObjectPopupMenu *ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app, false);
    FXString t(myType.c_str());
    new FXMenuCommand(ret, "(" + t + ")", 0, 0, 0);
    new FXMenuSeparator(ret);
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
                                       GUISUMOAbstractView &) throw()
{
    return 0;
}


const std::string &
GUIPointOfInterest::getMicrosimID() const throw()
{
    return getID();
}


Boundary
GUIPointOfInterest::getCenteringBoundary() const throw()
{
    Boundary b;
    b.add(x(), y());
    b.grow(10);
    return b;
}


void
GUIPointOfInterest::drawGL(const GUIVisualizationSettings &s) const throw()
{
    if (s.scale*(1.3/3.0)<s.minPOISize) {
        return;
    }
    if (getLayer()==0) {
        glPolygonOffset(0, -3);
    } else if (getLayer()>0) {
        glPolygonOffset(0, -5-getLayer());
    } else {
        glPolygonOffset(0, -getLayer()+1);
    }
    // (optional) set id
    if (s.needsGlID) {
        glPushName(getGlID());
    }
    glColor3d(red(),green(),blue());
    glTranslated(x(), y(), 0);
    GLHelper::drawFilledCircle((SUMOReal) 1.3*s.poiExaggeration, 16);
    if (s.drawPOIName) {
        glColor3f(s.poiNameColor.red(), s.poiNameColor.green(), s.poiNameColor.blue());
        glPushMatrix();
        glTranslated((SUMOReal) 1.32*s.poiExaggeration, (SUMOReal) 1.32*s.poiExaggeration, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        pfSetPosition(0, 0);
        pfSetScale(s.poiNameSize / s.scale);
        glRotated(180, 1, 0, 0);
        pfDrawString(getID().c_str());
        glPopMatrix();
    }
    glTranslated(-x(), -y(), 0);
    // (optional) clear id
    if (s.needsGlID) {
        glPopName();
    }
}


int
GUIPointOfInterest::getLayer() const
{
    return myLayer;
}



/****************************************************************************/

