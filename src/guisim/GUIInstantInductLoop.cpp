/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2003-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GUIInstantInductLoop.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Aug 2003
///
// The gui-version of the MSInstantInductLoop
/****************************************************************************/
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/geom/PositionVector.h>
#include "GUIInstantInductLoop.h"
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/output/MSInstantInductLoop.h>
#include <microsim/MSLane.h>
#include "GUIEdge.h"
#include <utils/gui/globjects/GLIncludes.h>


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUIInstantInductLoop-methods
 * ----------------------------------------------------------------------- */
GUIInstantInductLoop::GUIInstantInductLoop(const std::string& id, OutputDevice& od,
        MSLane* const lane, double positionInMeters,
        const std::string name, const std::string& vTypes,
        const std::string& nextEdges) :
    MSInstantInductLoop(id, od, lane, positionInMeters, name, vTypes, nextEdges) {}


GUIInstantInductLoop::~GUIInstantInductLoop() {}


GUIDetectorWrapper*
GUIInstantInductLoop::buildDetectorGUIRepresentation() {
    return new MyWrapper(*this, myPosition);
}

// -------------------------------------------------------------------------
// GUIInstantInductLoop::MyWrapper-methods
// -------------------------------------------------------------------------

GUIInstantInductLoop::MyWrapper::MyWrapper(GUIInstantInductLoop& detector, double pos) :
    GUIDetectorWrapper(GLO_E1DETECTOR_INSTANT, detector.getID(), GUIIconSubSys::getIcon(GUIIcon::E1INSTANT)),
    myDetector(detector), myPosition(pos) {
    myFGPosition = detector.getLane()->geometryPositionAtOffset(pos);
    myBoundary.add(myFGPosition.x() + (double) 5.5, myFGPosition.y() + (double) 5.5);
    myBoundary.add(myFGPosition.x() - (double) 5.5, myFGPosition.y() - (double) 5.5);
    myFGRotation = -detector.getLane()->getShape().rotationDegreeAtOffset(pos);
}


GUIInstantInductLoop::MyWrapper::~MyWrapper() {}


double
GUIInstantInductLoop::MyWrapper::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.addSize.getExaggeration(s, this);
}


Boundary
GUIInstantInductLoop::MyWrapper::getCenteringBoundary() const {
    Boundary b(myBoundary);
    b.grow(20);
    return b;
}



GUIParameterTableWindow*
GUIInstantInductLoop::MyWrapper::getParameterWindow(GUIMainWindow& app,
        GUISUMOAbstractView& /*parent !!! recheck this - never needed?*/) {
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this);
    // add items
    // parameter
    ret->mkItem(TL("name"), false, myDetector.myName);
    ret->mkItem(TL("position [m]"), false, myPosition);
    ret->mkItem(TL("lane"), false, myDetector.getLane()->getID());
    if (myDetector.isTyped()) {
        ret->mkItem(TL("vTypes"), false, toString(myDetector.getVehicleTypes()));
    }
    // values
    // close building
    ret->closeBuilding(&myDetector);
    return ret;
}


void
GUIInstantInductLoop::MyWrapper::drawGL(const GUIVisualizationSettings& s) const {
    GLHelper::pushName(getGlID());
    double width = (double) 2.0 * s.scale;
    glLineWidth(1.0);
    const double exaggeration = getExaggeration(s);
    // shape
    glColor3d(1, 0, 1);
    GLHelper::pushMatrix();
    glTranslated(0, 0, GLO_JUNCTION + 0.4); // do not draw on top of linkRules
    glTranslated(myFGPosition.x(), myFGPosition.y(), 0);
    glRotated(myFGRotation, 0, 0, 1);
    glScaled(exaggeration, exaggeration, 1);
    glBegin(GL_QUADS);
    glVertex2d(0 - 1.0, 2);
    glVertex2d(-1.0, -2);
    glVertex2d(1.0, -2);
    glVertex2d(1.0, 2);
    glEnd();
    glTranslated(0, 0, .01);
    glBegin(GL_LINES);
    glVertex2d(0, 2 - .1);
    glVertex2d(0, -2 + .1);
    glEnd();

    // outline
    if (width * exaggeration > 1) {
        glColor3d(1, 1, 1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin(GL_QUADS);
        glVertex2d(0 - 1.0, 2);
        glVertex2d(-1.0, -2);
        glVertex2d(1.0, -2);
        glVertex2d(1.0, 2);
        glEnd();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // position indicator
    if (width * exaggeration > 1) {
        glRotated(90, 0, 0, -1);
        glColor3d(1, 1, 1);
        glBegin(GL_LINES);
        glVertex2d(0, 1.7);
        glVertex2d(0, -1.7);
        glEnd();
    }
    GLHelper::popMatrix();
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    GLHelper::popName();
}


GUIInstantInductLoop&
GUIInstantInductLoop::MyWrapper::getLoop() {
    return myDetector;
}


/****************************************************************************/
