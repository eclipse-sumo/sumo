/****************************************************************************/
/// @file    GUIInductLoop.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Aug 2003
/// @version $Id$
///
// The gui-version of the MSInductLoop, together with the according
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include "GUIInductLoop.h"
#include <utils/gui/div/GLHelper.h>
#include <utils/geom/Line.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/MSLane.h>
#include <microsim/output/MSInductLoop.h>
#include "GUIEdge.h"
#include <utils/gui/globjects/GLIncludes.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUIInductLoop-methods
 * ----------------------------------------------------------------------- */
GUIInductLoop::GUIInductLoop(const std::string& id, MSLane* const lane,
                             SUMOReal position, bool splitByType)
    : MSInductLoop(id, lane, position, splitByType) {}


GUIInductLoop::~GUIInductLoop() {}


GUIDetectorWrapper*
GUIInductLoop::buildDetectorGUIRepresentation() {
    return new MyWrapper(*this, myPosition);
}


void
GUIInductLoop::reset() {
    AbstractMutex::ScopedLocker locker(myLock);
    MSInductLoop::reset();
}


void
GUIInductLoop::enterDetectorByMove(SUMOVehicle& veh, SUMOReal entryTimestep) {
    AbstractMutex::ScopedLocker locker(myLock);
    MSInductLoop::enterDetectorByMove(veh, entryTimestep);
}

void
GUIInductLoop::leaveDetectorByMove(SUMOVehicle& veh, SUMOReal leaveTimestep) {
    AbstractMutex::ScopedLocker locker(myLock);
    MSInductLoop::leaveDetectorByMove(veh, leaveTimestep);
}

void
GUIInductLoop::leaveDetectorByLaneChange(SUMOVehicle& veh, SUMOReal lastPos) {
    AbstractMutex::ScopedLocker locker(myLock);
    MSInductLoop::leaveDetectorByLaneChange(veh, lastPos);
}


std::vector<MSInductLoop::VehicleData>
GUIInductLoop::collectVehiclesOnDet(SUMOTime t) const {
    AbstractMutex::ScopedLocker locker(myLock);
    return MSInductLoop::collectVehiclesOnDet(t);
}


/* -------------------------------------------------------------------------
 * GUIInductLoop::MyWrapper-methods
 * ----------------------------------------------------------------------- */
GUIInductLoop::MyWrapper::MyWrapper(GUIInductLoop& detector, SUMOReal pos)
    : GUIDetectorWrapper("induct loop", detector.getID()),
      myDetector(detector), myPosition(pos) {
    myFGPosition = detector.getLane()->geometryPositionAtOffset(pos);
    myBoundary.add(myFGPosition.x() + (SUMOReal) 5.5, myFGPosition.y() + (SUMOReal) 5.5);
    myBoundary.add(myFGPosition.x() - (SUMOReal) 5.5, myFGPosition.y() - (SUMOReal) 5.5);
    myFGRotation = -detector.getLane()->getShape().rotationDegreeAtOffset(pos);
}


GUIInductLoop::MyWrapper::~MyWrapper() {}


Boundary
GUIInductLoop::MyWrapper::getCenteringBoundary() const {
    Boundary b(myBoundary);
    b.grow(20);
    return b;
}



GUIParameterTableWindow*
GUIInductLoop::MyWrapper::getParameterWindow(GUIMainWindow& app,
        GUISUMOAbstractView& /*parent !!! recheck this - never needed?*/) {
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this, 7);
    // add items
    // parameter
    ret->mkItem("position [m]", false, myPosition);
    ret->mkItem("lane", false, myDetector.getLane()->getID());
    // values
    ret->mkItem("passed vehicles [#]", true,
                new FunctionBinding<GUIInductLoop, unsigned int>(&myDetector, &GUIInductLoop::getCurrentPassedNumber));
    ret->mkItem("speed [m/s]", true,
                new FunctionBinding<GUIInductLoop, SUMOReal>(&myDetector, &GUIInductLoop::getCurrentSpeed));
    ret->mkItem("occupancy [%]", true,
                new FunctionBinding<GUIInductLoop, SUMOReal>(&myDetector, &GUIInductLoop::getCurrentOccupancy));
    ret->mkItem("vehicle length [m]", true,
                new FunctionBinding<GUIInductLoop, SUMOReal>(&myDetector, &GUIInductLoop::getCurrentLength));
    ret->mkItem("empty time [s]", true,
                new FunctionBinding<GUIInductLoop, SUMOReal>(&(getLoop()), &GUIInductLoop::getTimestepsSinceLastDetection));
    // close building
    ret->closeBuilding();
    return ret;
}


void
GUIInductLoop::MyWrapper::drawGL(const GUIVisualizationSettings& s) const {
    glPushName(getGlID());
    SUMOReal width = (SUMOReal) 2.0 * s.scale;
    glLineWidth(1.0);
    const SUMOReal exaggeration = s.addSize.getExaggeration(s);
    // shape
    glColor3d(1, 1, 0);
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


GUIInductLoop&
GUIInductLoop::MyWrapper::getLoop() {
    return myDetector;
}



/****************************************************************************/

