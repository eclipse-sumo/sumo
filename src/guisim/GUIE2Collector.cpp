/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GUIE2Collector.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Okt 2003
///
// The gui-version of the MSE2Collector
/****************************************************************************/
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/geom/PositionVector.h>
#include "GUIE2Collector.h"
#include <utils/gui/div/GLHelper.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <microsim/logging/FunctionBinding.h>
#include "GUIEdge.h"
#include <utils/gui/globjects/GLIncludes.h>


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUIE2Collector-methods
 * ----------------------------------------------------------------------- */
GUIE2Collector::GUIE2Collector(const std::string& id, DetectorUsage usage,
                               MSLane* lane, double startPos, double endPos, double detLength,
                               SUMOTime haltingTimeThreshold, double haltingSpeedThreshold,
                               double jamDistThreshold,
                               const std::string name, const std::string& vTypes,
                               const std::string& nextEdges,
                               int detectPersons, bool showDetector)
    : MSE2Collector(id, usage, lane, startPos, endPos, detLength, haltingTimeThreshold,
                    haltingSpeedThreshold, jamDistThreshold, name, vTypes, nextEdges, detectPersons),
      myShow(showDetector) {}

GUIE2Collector::GUIE2Collector(const std::string& id, DetectorUsage usage,
                               std::vector<MSLane*> lanes, double startPos, double endPos,
                               SUMOTime haltingTimeThreshold, double haltingSpeedThreshold,
                               double jamDistThreshold,
                               const std::string name, const std::string& vTypes,
                               const std::string& nextEdges,
                               int detectPersons, bool showDetector)
    : MSE2Collector(id, usage, lanes, startPos, endPos, haltingTimeThreshold,
                    haltingSpeedThreshold, jamDistThreshold, name, vTypes, nextEdges, detectPersons),
      myShow(showDetector) {}

GUIE2Collector::~GUIE2Collector() {}


GUIDetectorWrapper*
GUIE2Collector::buildDetectorGUIRepresentation() {
    return new MyWrapper(*this);
}

// -------------------------------------------------------------------------
// GUIE2Collector::MyWrapper-methods
// -------------------------------------------------------------------------

GUIE2Collector::MyWrapper::MyWrapper(GUIE2Collector& detector) :
    GUIDetectorWrapper(GLO_E2DETECTOR, detector.getID(), GUIIconSubSys::getIcon(GUIIcon::E2)),
    myDetector(detector) {
    mySupportsOverride = true;
    // collect detector shape into one vector (v)
    const std::vector<MSLane*> lanes = detector.getLanes();
    for (std::vector<MSLane*>::const_iterator li = lanes.begin(); li != lanes.end(); ++li) {
        PositionVector shape = (*li)->getShape();
        double start = (li == lanes.begin() ? lanes.front()->interpolateLanePosToGeometryPos(detector.getStartPos()) : 0);
        double end = (li + 1 == lanes.end() ? lanes.back()->interpolateLanePosToGeometryPos(detector.getEndPos()) : shape.length());
        shape = shape.getSubpart(start, end);
        myFullGeometry.insert(myFullGeometry.end(), shape.begin(), shape.end());
    }
    //
    myShapeRotations.reserve(myFullGeometry.size() - 1);
    myShapeLengths.reserve(myFullGeometry.size() - 1);
    int e = (int) myFullGeometry.size() - 1;
    for (int i = 0; i < e; ++i) {
        const Position& f = myFullGeometry[i];
        const Position& s = myFullGeometry[i + 1];
        myShapeLengths.push_back(f.distanceTo(s));
        myShapeRotations.push_back((double) atan2((s.x() - f.x()), (f.y() - s.y())) * (double) 180.0 / (double) M_PI);
    }
    //
    myBoundary = myFullGeometry.getBoxBoundary();
}


GUIE2Collector::MyWrapper::~MyWrapper() {}


double
GUIE2Collector::MyWrapper::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.addSize.getExaggeration(s, this);
}


Boundary
GUIE2Collector::MyWrapper::getCenteringBoundary() const {
    Boundary b(myBoundary);
    b.grow(20);
    return b;
}


GUIParameterTableWindow*
GUIE2Collector::MyWrapper::getParameterWindow(GUIMainWindow& app,
        GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this);
    // add items
    // parameter
    ret->mkItem(TL("name"), false, myDetector.getName());
    ret->mkItem(TL("length [m]"), false, myDetector.getLength());
    ret->mkItem(TL("position [m]"), false, myDetector.getStartPos());
    ret->mkItem(TL("lane"), false, myDetector.getLane()->getID());
    if (myDetector.isTyped()) {
        ret->mkItem(TL("vTypes"), false, toString(myDetector.getVehicleTypes()));
    }
    // values
    ret->mkItem(TL("vehicles [#]"), true,
                new FunctionBinding<MSE2Collector, int>(&myDetector, &MSE2Collector::getCurrentVehicleNumber));
    ret->mkItem(TL("occupancy [%]"), true,
                new FunctionBinding<MSE2Collector, double>(&myDetector, &MSE2Collector::getCurrentOccupancy));
    ret->mkItem(TL("mean speed [m/s]"), true,
                new FunctionBinding<MSE2Collector, double>(&myDetector, &MSE2Collector::getCurrentMeanSpeed));
    ret->mkItem(TL("mean vehicle length [m]"), true,
                new FunctionBinding<MSE2Collector, double>(&myDetector, &MSE2Collector::getCurrentMeanLength));
    ret->mkItem(TL("jam number [#]"), true,
                new FunctionBinding<MSE2Collector, int>(&myDetector, &MSE2Collector::getCurrentJamNumber));
    ret->mkItem(TL("max jam length [veh]"), true,
                new FunctionBinding<MSE2Collector, int>(&myDetector, &MSE2Collector::getCurrentMaxJamLengthInVehicles));
    ret->mkItem(TL("max jam length [m]"), true,
                new FunctionBinding<MSE2Collector, double>(&myDetector, &MSE2Collector::getCurrentMaxJamLengthInMeters));
    ret->mkItem(TL("jam length sum [veh]"), true,
                new FunctionBinding<MSE2Collector, int>(&myDetector, &MSE2Collector::getCurrentJamLengthInVehicles));
    ret->mkItem(TL("jam length sum [m]"), true,
                new FunctionBinding<MSE2Collector, double>(&myDetector, &MSE2Collector::getCurrentJamLengthInMeters));
    ret->mkItem(TL("started halts [#]"), true,
                new FunctionBinding<MSE2Collector, int>(&myDetector, &MSE2Collector::getCurrentStartedHalts));
    ret->mkItem(TL("interval seen vehicles [#]"), true,
                new FunctionBinding<MSE2Collector, int>(&myDetector, &MSE2Collector::getIntervalVehicleNumber));
    ret->mkItem(TL("interval speed [m/s]"), true,
                new FunctionBinding<MSE2Collector, double>(&myDetector, &MSE2Collector::getIntervalMeanSpeed));
    ret->mkItem(TL("interval occupancy [%]"), true,
                new FunctionBinding<MSE2Collector, double>(&myDetector, &MSE2Collector::getIntervalOccupancy));
    ret->mkItem(TL("interval max jam length [m]"), true,
                new FunctionBinding<MSE2Collector, double>(&myDetector, &MSE2Collector::getIntervalMaxJamLengthInMeters));
    ret->mkItem(TL("last interval seen vehicles [#]"), true,
                new FunctionBinding<MSE2Collector, int>(&myDetector, &MSE2Collector::getLastIntervalVehicleNumber));
    ret->mkItem(TL("last interval speed [m/s]"), true,
                new FunctionBinding<MSE2Collector, double>(&myDetector, &MSE2Collector::getLastIntervalMeanSpeed));
    ret->mkItem(TL("last interval occupancy [%]"), true,
                new FunctionBinding<MSE2Collector, double>(&myDetector, &MSE2Collector::getLastIntervalOccupancy));
    ret->mkItem(TL("last interval max jam length [m]"), true,
                new FunctionBinding<MSE2Collector, double>(&myDetector, &MSE2Collector::getLastIntervalMaxJamLengthInMeters));


    // close building
    ret->closeBuilding(&myDetector);
    return ret;
}


void
GUIE2Collector::MyWrapper::drawGL(const GUIVisualizationSettings& s) const {
    if (!myDetector.myShow) {
        return;
    }
    GLHelper::pushName(getGlID());
    GLHelper::pushMatrix();
    glTranslated(0, 0, GLO_JUNCTION + 0.4); // do not draw on top of linkRules
    double dwidth = 1;
    const double exaggeration = getExaggeration(s);
    if (exaggeration > 0) {
        if (haveOverride()) {
            glColor3d(1, 0, 1);
        } else if (myDetector.getUsageType() == DU_TL_CONTROL) {
            glColor3d(0, (double) .6, (double) .8);
        } else {
            glColor3d(0, (double) .8, (double) .8);
        }
        if (myDetector.getUsageType() == DU_TL_CONTROL) {
            dwidth = (double) 0.3;
        }
        double width = (double) 2.0 * s.scale;
        if (width * exaggeration > 1.0) {
            GLHelper::drawBoxLines(myFullGeometry, myShapeRotations, myShapeLengths, dwidth * exaggeration);
        } else {
            int e = (int) myFullGeometry.size() - 1;
            for (int i = 0; i < e; ++i) {
                GLHelper::drawLine(myFullGeometry[i], myShapeRotations[i], myShapeLengths[i]);
            }
        }
    }
    GLHelper::popMatrix();
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    GLHelper::popName();
}


GUIE2Collector&
GUIE2Collector::MyWrapper::getDetector() {
    return myDetector;
}

bool
GUIE2Collector::MyWrapper::haveOverride() const {
    return myDetector.getOverrideVehNumber() >= 0;
}


void
GUIE2Collector::MyWrapper::toggleOverride() const {
    if (haveOverride()) {
        myDetector.overrideVehicleNumber(-1);
    } else {
        myDetector.overrideVehicleNumber(1);
    }
}


/****************************************************************************/
