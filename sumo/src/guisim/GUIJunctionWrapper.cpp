/****************************************************************************/
/// @file    GUIJunctionWrapper.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @author  Andreas Gaubatz
/// @date    Mon, 1 Jul 2003
/// @version $Id$
///
// }
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#ifdef HAVE_OSG
#include <osg/Geometry>
#endif
#include <microsim/MSLane.h>
#include <microsim/MSJunction.h>
#include <utils/geom/Position.h>
#include <microsim/MSNet.h>
#include <microsim/MSInternalJunction.h>
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
#include <utils/gui/globjects/GLIncludes.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUIJunctionWrapper::GUIJunctionWrapper(MSJunction& junction)
    : GUIGlObject(GLO_JUNCTION, junction.getID()),
      myJunction(junction) {
    if (myJunction.getShape().size() == 0) {
        Position pos = myJunction.getPosition();
        myBoundary = Boundary(pos.x() - 1., pos.y() - 1., pos.x() + 1., pos.y() + 1.);
    } else {
        myBoundary = myJunction.getShape().getBoxBoundary();
    }
    myMaxSize = MAX2(myBoundary.getWidth(), myBoundary.getHeight());
#ifdef HAVE_INTERNAL_LANES
    myIsInner = dynamic_cast<MSInternalJunction*>(&myJunction) != 0;
#else
    myIsInner = false;
#endif
}


GUIJunctionWrapper::~GUIJunctionWrapper() {}


GUIGLObjectPopupMenu*
GUIJunctionWrapper::getPopUpMenu(GUIMainWindow& app,
                                 GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    return ret;
}


GUIParameterTableWindow*
GUIJunctionWrapper::getParameterWindow(GUIMainWindow& /*app*/,
                                       GUISUMOAbstractView&) {
    return 0;
}


Boundary
GUIJunctionWrapper::getCenteringBoundary() const {
    Boundary b = myBoundary;
    b.grow(20);
    return b;
}


void
GUIJunctionWrapper::drawGL(const GUIVisualizationSettings& s) const {
    // check whether it is not too small
    if (s.scale * myMaxSize < 1.) {
        return;
    }
    if (!myIsInner) {
        glPushName(getGlID());
        glPushMatrix();
        const SUMOReal colorValue = static_cast<SUMOReal>(s.junctionColorer.getActive() == 1 && gSelected.isSelected(getType(), getGlID()));
        GLHelper::setColor(s.junctionColorer.getScheme().getColor(colorValue));
        glTranslated(0, 0, getType());
        if (s.scale * myMaxSize < 40.) {
            GLHelper::drawFilledPoly(myJunction.getShape(), true);
        } else {
            GLHelper::drawFilledPolyTesselated(myJunction.getShape(), true);
        }
        glPopMatrix();
    }
    if (myIsInner) {
        drawName(myJunction.getPosition(), s.scale, s.internalJunctionName);
    } else {
        drawName(myJunction.getPosition(), s.scale, s.junctionName);
    }
    glPopName();
}


#ifdef HAVE_OSG
void
GUIJunctionWrapper::updateColor(const GUIVisualizationSettings& s) {
    const SUMOReal colorValue = static_cast<SUMOReal>(s.junctionColorer.getActive() == 1 && gSelected.isSelected(getType(), getGlID()));
    const RGBColor& col = s.junctionColorer.getScheme().getColor(colorValue);
    osg::Vec4ubArray* colors = dynamic_cast<osg::Vec4ubArray*>(myGeom->getColorArray());
    (*colors)[0].set(col.red(), col.green(), col.blue(), col.alpha());
    myGeom->setColorArray(colors);
}
#endif


/****************************************************************************/

