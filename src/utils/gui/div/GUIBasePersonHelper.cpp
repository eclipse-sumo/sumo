/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIBasePersonHelper.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2018
/// @version $Id$
///
// Functions used in GUIPerson and GNEPerson
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <microsim/pedestrians/MSPModel_Striping.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/images/GUITexturesHelper.h>

#include "GLHelper.h"
#include "GUIBasePersonHelper.h"

// ===========================================================================
// method definitions
// ===========================================================================

void
GUIBasePersonHelper::drawAction_drawAsTriangle(const double angle, const double lenght, const double width) {
    // draw triangle pointing forward
    glRotated(RAD2DEG(angle + M_PI / 2.), 0, 0, 1);
    glScaled(lenght, width, 1);
    glBegin(GL_TRIANGLES);
    glVertex2d(0., 0.);
    glVertex2d(1, -0.5);
    glVertex2d(1, 0.5);
    glEnd();
    // draw a smaller triangle to indicate facing
    GLHelper::setColor(GLHelper::getColor().changedBrightness(-64));
    glTranslated(0, 0, .045);
    glBegin(GL_TRIANGLES);
    glVertex2d(0., 0.);
    glVertex2d(0.5, -0.25);
    glVertex2d(0.5, 0.25);
    glEnd();
    glTranslated(0, 0, -.045);
}


void
GUIBasePersonHelper::drawAction_drawAsCircle(const double lenght, const double width) {
    glScaled(lenght, width, 1);
    GLHelper::drawFilledCircle(0.8);
}


void
GUIBasePersonHelper::drawAction_drawAsPoly(const double angle, const double lenght, const double width) {
    // draw pedestrian shape
    glRotated(GeomHelper::naviDegree(angle) - 180, 0, 0, -1);
    glScaled(lenght, width, 1);
    RGBColor lighter = GLHelper::getColor().changedBrightness(51);
    glTranslated(0, 0, .045);
    // head
    glScaled(1, 0.5, 1.);
    GLHelper::drawFilledCircle(0.5);
    // nose
    glBegin(GL_TRIANGLES);
    glVertex2d(0.0, -0.2);
    glVertex2d(0.0, 0.2);
    glVertex2d(-0.6, 0.0);
    glEnd();
    glTranslated(0, 0, -.045);
    // body
    glScaled(0.9, 2.0, 1);
    glTranslated(0, 0, .04);
    GLHelper::setColor(lighter);
    GLHelper::drawFilledCircle(0.5);
    glTranslated(0, 0, -.04);
}


void
GUIBasePersonHelper::drawAction_drawAsImage(const double angle, const double lenght, const double width, const std::string& file,
        const SUMOVehicleShape guiShape, const double exaggeration) {
    // first check if filename isn't empty
    if (file != "") {
        if (guiShape == SVS_PEDESTRIAN) {
            glRotated(RAD2DEG(angle + M_PI / 2.), 0, 0, 1);
        }
        int textureID = GUITexturesHelper::getTextureID(file);
        if (textureID > 0) {
            const double halfLength = lenght / 2.0 * exaggeration;
            const double halfWidth = width / 2.0 * exaggeration;
            GUITexturesHelper::drawTexturedBox(textureID, -halfWidth, -halfLength, halfWidth, halfLength);
        }
    } else {
        // fallback if no image is defined
        drawAction_drawAsPoly(angle, lenght, width);
    }
}

/****************************************************************************/
