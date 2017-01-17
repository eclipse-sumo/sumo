/****************************************************************************/
/// @file    GUIMEInductLoop.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 21.07.2005
/// @version $Id$
///
// The gui-version of the MEInductLoop
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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


#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/geom/PositionVector.h>
#include <guisim/GUILane.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <microsim/logging/FuncBinding_IntParam.h>
#include <microsim/logging/FunctionBinding.h>
#include <mesosim/MEInductLoop.h>
#include <mesosim/MESegment.h>
#include "GUIMEInductLoop.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUIMEInductLoop-methods
 * ----------------------------------------------------------------------- */
GUIMEInductLoop::GUIMEInductLoop(const std::string& id, MESegment* s,
                                 SUMOReal position, const std::string& vTypes)
    : MEInductLoop(id, s, position, vTypes) {}


GUIMEInductLoop::~GUIMEInductLoop() {}


GUIDetectorWrapper*
GUIMEInductLoop::buildDetectorGUIRepresentation() {
    return new MyWrapper(*this, myPosition);
}


/* -------------------------------------------------------------------------
 * GUIMEInductLoop::MyWrapper-methods
 * ----------------------------------------------------------------------- */
GUIMEInductLoop::MyWrapper::MyWrapper(GUIMEInductLoop& detector, SUMOReal pos)
    : GUIDetectorWrapper("induct loop", detector.getID()),
      myDetector(detector), myPosition(pos) {
    const MSLane* lane = detector.mySegment->getEdge().getLanes()[0];
    myFGPosition = lane->geometryPositionAtOffset(pos);
    myBoundary.add(myFGPosition.x() + (SUMOReal) 5.5, myFGPosition.y() + (SUMOReal) 5.5);
    myBoundary.add(myFGPosition.x() - (SUMOReal) 5.5, myFGPosition.y() - (SUMOReal) 5.5);
    myFGRotation = -lane->getShape().rotationDegreeAtOffset(pos);
}


GUIMEInductLoop::MyWrapper::~MyWrapper() {}


Boundary
GUIMEInductLoop::MyWrapper::getCenteringBoundary() const {
    Boundary b(myBoundary);
    b.grow(20);
    return b;
}



GUIParameterTableWindow*
GUIMEInductLoop::MyWrapper::getParameterWindow(GUIMainWindow& app,
        GUISUMOAbstractView& /* parent */) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 2);
    // add items
    /*
    ret->mkItem("flow [veh/h]", true,
        new FuncBinding_IntParam<GUIMEInductLoop, SUMOReal>(
            &(getLoop()), &GUIMEInductLoop::getFlow, 1));
    ret->mkItem("mean speed [m/s]", true,
        new FuncBinding_IntParam<GUIMEInductLoop, SUMOReal>(
            &(getLoop()), &GUIMEInductLoop::getMeanSpeed, 1));
    ret->mkItem("occupancy [%]", true,
        new FuncBinding_IntParam<GUIMEInductLoop, SUMOReal>(
            &(getLoop()), &GUIMEInductLoop::getOccupancy, 1));
    ret->mkItem("mean vehicle length [m]", true,
        new FuncBinding_IntParam<GUIMEInductLoop, SUMOReal>(
            &(getLoop()), &GUIMEInductLoop::getMeanVehicleLength, 1));
    ret->mkItem("empty time [s]", true,
        new FunctionBinding<GUIMEInductLoop, SUMOReal>(
            &(getLoop()), &GUIMEInductLoop::getTimestepsSinceLastDetection));
            */
    //
    ret->mkItem("position [m]", false, myPosition);
    ret->mkItem("lane", false, myDetector.mySegment->getID());
    // close building
    ret->closeBuilding();
    return ret;
}


void
GUIMEInductLoop::MyWrapper::drawGL(const GUIVisualizationSettings& s) const {
    glPushName(getGlID());
    glPolygonOffset(0, -2);
    SUMOReal width = (SUMOReal) 2.0 * s.scale;
    glLineWidth(1.0);
    const SUMOReal exaggeration = s.addSize.getExaggeration(s);
    // shape
    glColor3d(1, 1, 0);
    glPushMatrix();
    glTranslated(myFGPosition.x(), myFGPosition.y(), getType());
    glRotated(myFGRotation, 0, 0, 1);
    glScaled(exaggeration, exaggeration, exaggeration);
    glBegin(GL_QUADS);
    glVertex2d(0 - 1.0, 2);
    glVertex2d(-1.0, -2);
    glVertex2d(1.0, -2);
    glVertex2d(1.0, 2);
    glEnd();
    glBegin(GL_LINES);
    // without the substracted offsets, lines are partially longer
    //  than the boxes
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


GUIMEInductLoop&
GUIMEInductLoop::MyWrapper::getLoop() {
    return myDetector;
}


/****************************************************************************/

