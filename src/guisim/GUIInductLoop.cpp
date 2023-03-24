/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GUIInductLoop.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Aug 2003
///
// The gui-version of the MSInductLoop, together with the according
/****************************************************************************/
#include <config.h>

#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/logging/FuncBinding_IntParam.h>
#include <microsim/logging/CastingFunctionBinding_Param.h>
#include <microsim/MSLane.h>
#include <microsim/output/MSInductLoop.h>
#include "GUIEdge.h"
#include "Command_Hotkey_InductionLoop.h"
#include "GUIInductLoop.h"


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUIInductLoop-methods
 * ----------------------------------------------------------------------- */
GUIInductLoop::GUIInductLoop(const std::string& id, MSLane* const lane,
                             double position, double length,
                             std::string name, const std::string& vTypes,
                             const std::string& nextEdges,
                             int detectPersons, bool show) :
    MSInductLoop(id, lane, position, length, name, vTypes, nextEdges, detectPersons, true),
    myWrapper(nullptr),
    myShow(show) {
}


GUIInductLoop::~GUIInductLoop() {}


GUIDetectorWrapper*
GUIInductLoop::buildDetectorGUIRepresentation() {
    if (knowsParameter("hotkey")) {
        Command_Hotkey_InductionLoop::registerHotkey(getParameter("hotkey"), this);
    }
    // caller (GUINet) takes responsibility for pointer
    myWrapper = new MyWrapper(*this, myPosition);
    return myWrapper;
}


void
GUIInductLoop::setSpecialColor(const RGBColor* color) {
    if (myWrapper != nullptr) {
        myWrapper->setSpecialColor(color);
    }
}


// -------------------------------------------------------------------------
// GUIInductLoop::MyWrapper-methods
// -------------------------------------------------------------------------

GUIInductLoop::MyWrapper::MyWrapper(GUIInductLoop& detector, double pos) :
    GUIDetectorWrapper(GLO_E1DETECTOR, detector.getID(), GUIIconSubSys::getIcon(GUIIcon::E1)),
    myDetector(detector), myPosition(pos),
    myHaveLength(myPosition != detector.getEndPosition()),
    mySpecialColor(nullptr) {
    mySupportsOverride = true;
    myFGPosition = detector.getLane()->geometryPositionAtOffset(pos);
    myBoundary.add(myFGPosition.x() + (double) 5.5, myFGPosition.y() + (double) 5.5);
    myBoundary.add(myFGPosition.x() - (double) 5.5, myFGPosition.y() - (double) 5.5);
    myFGRotation = -detector.getLane()->getShape().rotationDegreeAtOffset(pos);

    if (myHaveLength) {
        const MSLane& lane = *detector.getLane();
        const double endPos = detector.getEndPosition();
        myFGShape = lane.getShape();
        myFGShape = myFGShape.getSubpart(
                        lane.interpolateLanePosToGeometryPos(pos),
                        lane.interpolateLanePosToGeometryPos(endPos));
        myFGShapeRotations.reserve(myFGShape.size() - 1);
        myFGShapeLengths.reserve(myFGShape.size() - 1);
        int e = (int) myFGShape.size() - 1;
        for (int i = 0; i < e; ++i) {
            const Position& f = myFGShape[i];
            const Position& s = myFGShape[i + 1];
            myFGShapeLengths.push_back(f.distanceTo(s));
            myFGShapeRotations.push_back((double) atan2((s.x() - f.x()), (f.y() - s.y())) * (double) 180.0 / (double) PI);
        }
        myOutline.push_back(lane.geometryPositionAtOffset(pos, -1));
        myOutline.push_back(lane.geometryPositionAtOffset(pos, 1));
        myOutline.push_back(lane.geometryPositionAtOffset(endPos, 1));
        myOutline.push_back(lane.geometryPositionAtOffset(endPos, -1));
        myIndicators.push_back(lane.geometryPositionAtOffset(pos, -1.7));
        myIndicators.push_back(lane.geometryPositionAtOffset(pos, 1.7));
        myIndicators.push_back(lane.geometryPositionAtOffset(endPos, 1.7));
        myIndicators.push_back(lane.geometryPositionAtOffset(endPos, -1.7));
    }
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
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this);
    // add items
    // parameter
    ret->mkItem("name", false, myDetector.getName());
    ret->mkItem("position [m]", false, myPosition);
    if (myDetector.getEndPosition() != myPosition) {
        ret->mkItem("end position [m]", false, myDetector.getEndPosition());
    }
    ret->mkItem("lane", false, myDetector.getLane()->getID());
    // values
    ret->mkItem("entered vehicles [#]", true,
                new FuncBinding_IntParam<GUIInductLoop, double>(&myDetector, &GUIInductLoop::getEnteredNumber, 0));
    ret->mkItem("speed [m/s]", true,
                new FuncBinding_IntParam<GUIInductLoop, double>(&myDetector, &GUIInductLoop::getSpeed, 0));
    ret->mkItem("occupancy [%]", true,
                new FunctionBinding<GUIInductLoop, double>(&myDetector, &GUIInductLoop::getOccupancy));
    ret->mkItem("vehicle length [m]", true,
                new FuncBinding_IntParam<GUIInductLoop, double>(&myDetector, &GUIInductLoop::getVehicleLength, 0));
    ret->mkItem("empty time [s]", true,
                new FunctionBinding<GUIInductLoop, double>(&myDetector, &GUIInductLoop::getTimeSinceLastDetection));
    ret->mkItem("occupied time [s]", true,
                new FunctionBinding<GUIInductLoop, double>(&myDetector, &GUIInductLoop::getOccupancyTime));
    ret->mkItem("interval entered vehicles [#]", true,
                new CastingFunctionBinding_Param<GUIInductLoop, double, int, bool>(&myDetector, &GUIInductLoop::getIntervalVehicleNumber, false));
    ret->mkItem("interval speed [m/s]", true,
                new CastingFunctionBinding_Param<GUIInductLoop, double, double, bool>(&myDetector, &GUIInductLoop::getIntervalMeanSpeed, false));
    ret->mkItem("interval occupancy [%]", true,
                new CastingFunctionBinding_Param<GUIInductLoop, double, double, bool>(&myDetector, &GUIInductLoop::getIntervalOccupancy, false));
    ret->mkItem("last interval entered vehicles [#]", true,
                new CastingFunctionBinding_Param<GUIInductLoop, double, int, bool>(&myDetector, &GUIInductLoop::getIntervalVehicleNumber, true));
    ret->mkItem("last interval speed [m/s]", true,
                new CastingFunctionBinding_Param<GUIInductLoop, double, double, bool>(&myDetector, &GUIInductLoop::getIntervalMeanSpeed, true));
    ret->mkItem("last interval occupancy [%]", true,
                new CastingFunctionBinding_Param<GUIInductLoop, double, double, bool>(&myDetector, &GUIInductLoop::getIntervalOccupancy, true));
    // close building
    ret->closeBuilding(&myDetector);
    return ret;
}


void
GUIInductLoop::MyWrapper::drawGL(const GUIVisualizationSettings& s) const {
    if (!myDetector.isVisible()) {
        return;
    }
    GLHelper::pushName(getGlID());
    double width = (double) 2.0 * s.scale;
    glLineWidth(1.0);
    const double exaggeration = getExaggeration(s);
    glColor3d(1, 1, 0);
    if (myHaveLength) {
        GLHelper::pushMatrix();
        glTranslated(0, 0, GLO_JUNCTION + 0.4); // do not draw on top of linkRules
        GLHelper::drawBoxLines(myFGShape, myFGShapeRotations, myFGShapeLengths, MIN2(1.0, exaggeration), 0, 0);
        if (width * exaggeration > 1) {

            // outline
            setOutlineColor();
            glTranslated(0, 0, .01);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glBegin(GL_QUADS);
            for (const Position& p : myOutline) {
                glVertex2d(p.x(), p.y());
            }
            glEnd();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            // position indicator
            glBegin(GL_LINES);
            glVertex2d(myIndicators[0].x(), myIndicators[0].y());
            glVertex2d(myIndicators[1].x(), myIndicators[1].y());
            glEnd();
            glBegin(GL_LINES);
            glVertex2d(myIndicators[2].x(), myIndicators[2].y());
            glVertex2d(myIndicators[3].x(), myIndicators[3].y());
            glEnd();

            // jammed actuated-tls detector, draw crossed-out:
            if (mySpecialColor != nullptr  && *mySpecialColor == RGBColor::ORANGE) {
                glBegin(GL_LINES);
                glVertex2d(myOutline[0].x(), myOutline[0].y());
                glVertex2d(myOutline[2].x(), myOutline[2].y());
                glEnd();
                glBegin(GL_LINES);
                glVertex2d(myOutline[1].x(), myOutline[1].y());
                glVertex2d(myOutline[3].x(), myOutline[3].y());
                glEnd();
            }
        }
        GLHelper::popMatrix();
    } else {
        // classic shape
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
        setOutlineColor();

        if (width * exaggeration > 1) {
            // outline
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glBegin(GL_QUADS);
            glVertex2d(0 - 1.0, 2);
            glVertex2d(-1.0, -2);
            glVertex2d(1.0, -2);
            glVertex2d(1.0, 2);
            glEnd();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            // position indicator
            glRotated(90, 0, 0, -1);
            glBegin(GL_LINES);
            glVertex2d(0, 1.7);
            glVertex2d(0, -1.7);
            glEnd();

            // jammed actuated-tls detector, draw crossed-out:
            if (mySpecialColor != nullptr  && *mySpecialColor == RGBColor::ORANGE) {
                glBegin(GL_LINES);
                glVertex2d(-1.0, 2);
                glVertex2d(1.0, -2);
                glEnd();
                glBegin(GL_LINES);
                glVertex2d(-1.0, -2);
                glVertex2d(1.0, 2);
                glEnd();
            }
        }
        GLHelper::popMatrix();
    }
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    GLHelper::popName();
}


void
GUIInductLoop::MyWrapper::setOutlineColor() const {
    if (haveOverride()) {
        glColor3d(1, 0, 1);
    } else if (mySpecialColor == nullptr) {
        glColor3d(1, 1, 1);
    } else {
        GLHelper::setColor(*mySpecialColor);
    }
}

bool
GUIInductLoop::MyWrapper::haveOverride() const {
    return myDetector.getOverrideTime() >= 0;
}


void
GUIInductLoop::MyWrapper::toggleOverride() const {
    if (haveOverride()) {
        myDetector.overrideTimeSinceDetection(-1);
    } else {
        myDetector.overrideTimeSinceDetection(0);
    }
}

/****************************************************************************/
