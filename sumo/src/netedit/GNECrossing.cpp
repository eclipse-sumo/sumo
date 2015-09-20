/****************************************************************************/
/// @file    GNECrossing.cpp
/// @author  Jakob Erdmann
/// @date    June 2011
/// @version $Id: GNECrossing.cpp 18830 2015-09-11 08:57:34Z behrisch $
///
// A class for visualizing Inner Lanes (used when editing traffic lights)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
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
#include <iostream>
#include <utility>
#include <time.h>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/common/ToString.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNECrossing.h"
#include "GNEJunction.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS



// ===========================================================================
// static member definitions
// ===========================================================================


// ===========================================================================
// method definitions
// ===========================================================================
GNECrossing::GNECrossing(GNEJunction& parentJunction, const std::string& id) :
    GUIGlObject(GLO_EDGE, id),
    myParentJunction(parentJunction),
    myCrossing(parentJunction.getNBNode()->getCrossing(id)),
    myShape(myCrossing.shape)
{
    int segments = (int) myShape.size() - 1;
    if (segments >= 0) {
        myShapeRotations.reserve(segments);
        myShapeLengths.reserve(segments);
        for (int i = 0; i < segments; ++i) {
            const Position& f = myShape[i];
            const Position& s = myShape[i + 1];
            myShapeLengths.push_back(f.distanceTo2D(s));
            myShapeRotations.push_back((SUMOReal) atan2((s.x() - f.x()), (f.y() - s.y())) * (SUMOReal) 180.0 / (SUMOReal) PI);
        }
    }
}


GNECrossing::~GNECrossing() {}


void
GNECrossing::drawGL(const GUIVisualizationSettings& s) const {
    glPushMatrix();
    glPushName(getGlID());
    glTranslated(0, 0, GLO_JUNCTION + 0.1); // must draw on top of junction

    if (myCrossing.priority) {
        glColor3d(0.9, 0.9, 0.9);
    } else {
        glColor3d(0.1, 0.1, 0.1);
    }
    glTranslated(0, 0, .2);
    // @todo: duplicate eliminate duplicate code with GNELane::drawCrossties(0.5, 1.0, myCrossing.width * 0.5);
    {
        SUMOReal length = 0.5;
        SUMOReal spacing = 1.0;
        SUMOReal halfWidth = myCrossing.width * 0.5;
        glPushMatrix();
        // draw on top of of the white area between the rails
        glTranslated(0, 0, 0.1);
        int e = (int) myShape.size() - 1;
        for (int i = 0; i < e; ++i) {
            glPushMatrix();
            glTranslated(myShape[i].x(), myShape[i].y(), 0.0);
            glRotated(myShapeRotations[i], 0, 0, 1);
            for (SUMOReal t = 0; t < myShapeLengths[i]; t += spacing) {
                glBegin(GL_QUADS);
                glVertex2d(-halfWidth, -t);
                glVertex2d(-halfWidth, -t - length);
                glVertex2d(halfWidth, -t - length);
                glVertex2d(halfWidth, -t);
                glEnd();
            }
            glPopMatrix();
        }
        glPopMatrix();
    }


    glTranslated(0, 0, -.2);
    glPopName();
    glPopMatrix();
}


GUIGLObjectPopupMenu*
GNECrossing::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    myPopup = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(myPopup, app);
    return myPopup;
}


GUIParameterTableWindow*
GNECrossing::getParameterWindow(GUIMainWindow& app,
                                    GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 2);
    // add items
    // close building
    ret->closeBuilding();
    return ret;
}


Boundary
GNECrossing::getCenteringBoundary() const {
    Boundary b = myShape.getBoxBoundary();
    b.grow(10);
    return b;
}


/****************************************************************************/
