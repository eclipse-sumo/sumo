/****************************************************************************/
/// @file    GUIInstantInductLoop.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Aug 2003
/// @version $Id$
///
// The gui-version of the MSInstantInductLoop
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2003-2014 DLR (http://www.dlr.de/) and contributors
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

#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/geom/PositionVector.h>
#include "GUIInstantInductLoop.h"
#include <utils/gui/div/GLHelper.h>
#include <utils/geom/Line.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/output/MSInstantInductLoop.h>
#include <microsim/MSLane.h>
#include "GUIEdge.h"
#include <utils/gui/globjects/GLIncludes.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUIInstantInductLoop-methods
 * ----------------------------------------------------------------------- */
GUIInstantInductLoop::GUIInstantInductLoop(const std::string& id, OutputDevice& od,
        MSLane* const lane, SUMOReal positionInMeters)
    : MSInstantInductLoop(id, od, lane, positionInMeters) {}


GUIInstantInductLoop::~GUIInstantInductLoop() {}


GUIDetectorWrapper*
GUIInstantInductLoop::buildDetectorGUIRepresentation() {
    return new MyWrapper(*this, myPosition);
}


/* -------------------------------------------------------------------------
 * GUIInstantInductLoop::MyWrapper-methods
 * ----------------------------------------------------------------------- */
GUIInstantInductLoop::MyWrapper::MyWrapper(GUIInstantInductLoop& detector, SUMOReal pos)
    : GUIDetectorWrapper("instant induct loop", detector.getID()),
      myDetector(detector), myPosition(pos) {
    myFGPosition = detector.getLane()->geometryPositionAtOffset(pos);
    myBoundary.add(myFGPosition.x() + (SUMOReal) 5.5, myFGPosition.y() + (SUMOReal) 5.5);
    myBoundary.add(myFGPosition.x() - (SUMOReal) 5.5, myFGPosition.y() - (SUMOReal) 5.5);
    myFGRotation = -detector.getLane()->getShape().rotationDegreeAtOffset(pos);
}


GUIInstantInductLoop::MyWrapper::~MyWrapper() {}


Boundary
GUIInstantInductLoop::MyWrapper::getCenteringBoundary() const {
    Boundary b(myBoundary);
    b.grow(20);
    return b;
}



GUIParameterTableWindow*
GUIInstantInductLoop::MyWrapper::getParameterWindow(GUIMainWindow& app,
        GUISUMOAbstractView& /*parent !!! recheck this - never needed?*/) {
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this, 7);
    // add items
    // parameter
    ret->mkItem("position [m]", false, myPosition);
    ret->mkItem("lane", false, myDetector.getLane()->getID());
    // values
    // close building
    ret->closeBuilding();
    return ret;
}


void
GUIInstantInductLoop::MyWrapper::drawGL(const GUIVisualizationSettings& s) const {
    glPushName(getGlID());
    SUMOReal width = (SUMOReal) 2.0 * s.scale;
    glLineWidth(1.0);
    const SUMOReal exaggeration = s.addSize.getExaggeration(s);
    // shape
    glColor3d(1, 0, 1);
    glPushMatrix();
    glTranslated(0, 0, getType());
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
        glVertex2f(0 - 1.0, 2);
        glVertex2f(-1.0, -2);
        glVertex2f(1.0, -2);
        glVertex2f(1.0, 2);
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
    glPopMatrix();
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    glPopName();
}


GUIInstantInductLoop&
GUIInstantInductLoop::MyWrapper::getLoop() {
    return myDetector;
}



/****************************************************************************/

