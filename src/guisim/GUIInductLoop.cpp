/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/MSLane.h>
#include <microsim/output/MSInductLoop.h>
#include "GUIEdge.h"
#include <utils/gui/globjects/GLIncludes.h>


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUIInductLoop-methods
 * ----------------------------------------------------------------------- */
GUIInductLoop::GUIInductLoop(const std::string& id, MSLane* const lane,
                             double position, const std::string& vTypes)
    : MSInductLoop(id, lane, position, vTypes) {}


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
GUIInductLoop::enterDetectorByMove(SUMOVehicle& veh, double entryTimestep) {
    AbstractMutex::ScopedLocker locker(myLock);
    MSInductLoop::enterDetectorByMove(veh, entryTimestep);
}

void
GUIInductLoop::leaveDetectorByMove(SUMOVehicle& veh, double leaveTimestep) {
    AbstractMutex::ScopedLocker locker(myLock);
    MSInductLoop::leaveDetectorByMove(veh, leaveTimestep);
}

void
GUIInductLoop::leaveDetectorByLaneChange(SUMOVehicle& veh, double lastPos) {
    AbstractMutex::ScopedLocker locker(myLock);
    MSInductLoop::leaveDetectorByLaneChange(veh, lastPos);
}


std::vector<MSInductLoop::VehicleData>
GUIInductLoop::collectVehiclesOnDet(SUMOTime t, bool leaveTime) const {
    AbstractMutex::ScopedLocker locker(myLock);
    return MSInductLoop::collectVehiclesOnDet(t, leaveTime);
}


/* -------------------------------------------------------------------------
 * GUIInductLoop::MyWrapper-methods
 * ----------------------------------------------------------------------- */
GUIInductLoop::MyWrapper::MyWrapper(GUIInductLoop& detector, double pos)
    : GUIDetectorWrapper("induct loop", detector.getID()),
      myDetector(detector), myPosition(pos) {
    myFGPosition = detector.getLane()->geometryPositionAtOffset(pos);
    myBoundary.add(myFGPosition.x() + (double) 5.5, myFGPosition.y() + (double) 5.5);
    myBoundary.add(myFGPosition.x() - (double) 5.5, myFGPosition.y() - (double) 5.5);
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
                new FunctionBinding<GUIInductLoop, int>(&myDetector, &GUIInductLoop::getCurrentPassedNumber));
    ret->mkItem("speed [m/s]", true,
                new FunctionBinding<GUIInductLoop, double>(&myDetector, &GUIInductLoop::getCurrentSpeed));
    ret->mkItem("occupancy [%]", true,
                new FunctionBinding<GUIInductLoop, double>(&myDetector, &GUIInductLoop::getCurrentOccupancy));
    ret->mkItem("vehicle length [m]", true,
                new FunctionBinding<GUIInductLoop, double>(&myDetector, &GUIInductLoop::getCurrentLength));
    ret->mkItem("empty time [s]", true,
                new FunctionBinding<GUIInductLoop, double>(&(getLoop()), &GUIInductLoop::getTimeSinceLastDetection));
    // close building
    ret->closeBuilding();
    return ret;
}


void
GUIInductLoop::MyWrapper::drawGL(const GUIVisualizationSettings& s) const {
    glPushName(getGlID());
    double width = (double) 2.0 * s.scale;
    glLineWidth(1.0);
    const double exaggeration = s.addSize.getExaggeration(s);
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

