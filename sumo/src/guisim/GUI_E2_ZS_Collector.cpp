/****************************************************************************/
/// @file    GUI_E2_ZS_Collector.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Okt 2003
/// @version $Id$
///
// The gui-version of the MS_E2_ZS_Collector
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
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
#include "GUI_E2_ZS_Collector.h"
#include <utils/gui/div/GLHelper.h>
#include <utils/geom/Line.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <microsim/logging/FunctionBinding.h>
#include "GUIEdge.h"
#include <utils/gui/globjects/GLIncludes.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUI_E2_ZS_Collector-methods
 * ----------------------------------------------------------------------- */
GUI_E2_ZS_Collector::GUI_E2_ZS_Collector(const std::string& id, DetectorUsage usage,
        MSLane* const lane, SUMOReal startPos, SUMOReal detLength,
        SUMOTime haltingTimeThreshold, SUMOReal haltingSpeedThreshold,
        SUMOReal jamDistThreshold)
    : MSE2Collector(id, usage, lane, startPos, detLength, haltingTimeThreshold,
                    haltingSpeedThreshold, jamDistThreshold) {}



GUI_E2_ZS_Collector::~GUI_E2_ZS_Collector() {}


GUIDetectorWrapper*
GUI_E2_ZS_Collector::buildDetectorGUIRepresentation() {
    return new MyWrapper(*this);
}



/* -------------------------------------------------------------------------
 * GUI_E2_ZS_Collector::MyWrapper-methods
 * ----------------------------------------------------------------------- */
GUI_E2_ZS_Collector::MyWrapper::MyWrapper(GUI_E2_ZS_Collector& detector)
    : GUIDetectorWrapper("E2 detector", detector.getID()),
      myDetector(detector) {
    const PositionVector& v = detector.getLane()->getShape();
    Line l(v.front(), v.back());
    // build geometry
    myFullGeometry = v.getSubpart(detector.getStartPos(), detector.getEndPos());
    //
    myShapeRotations.reserve(myFullGeometry.size() - 1);
    myShapeLengths.reserve(myFullGeometry.size() - 1);
    int e = (int) myFullGeometry.size() - 1;
    for (int i = 0; i < e; ++i) {
        const Position& f = myFullGeometry[i];
        const Position& s = myFullGeometry[i + 1];
        myShapeLengths.push_back(f.distanceTo(s));
        myShapeRotations.push_back((SUMOReal) atan2((s.x() - f.x()), (f.y() - s.y())) * (SUMOReal) 180.0 / (SUMOReal) PI);
    }
    //
    myBoundary = myFullGeometry.getBoxBoundary();
}


GUI_E2_ZS_Collector::MyWrapper::~MyWrapper() {}


Boundary
GUI_E2_ZS_Collector::MyWrapper::getCenteringBoundary() const {
    Boundary b(myBoundary);
    b.grow(20);
    return b;
}


GUIParameterTableWindow*
GUI_E2_ZS_Collector::MyWrapper::getParameterWindow(GUIMainWindow& app,
        GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 13);
    // add items
    // parameter
    ret->mkItem("length [m]", false, myDetector.getEndPos() - myDetector.getStartPos());
    ret->mkItem("position [m]", false, myDetector.getStartPos());
    ret->mkItem("lane", false, myDetector.getLane()->getID());
    // values
    ret->mkItem("vehicles [#]", true,
                new FunctionBinding<MSE2Collector, unsigned>(&myDetector, &MSE2Collector::getCurrentVehicleNumber));
    ret->mkItem("occupancy [%]", true,
                new FunctionBinding<MSE2Collector, SUMOReal>(&myDetector, &MSE2Collector::getCurrentOccupancy));
    ret->mkItem("mean speed [m/s]", true,
                new FunctionBinding<MSE2Collector, SUMOReal>(&myDetector, &MSE2Collector::getCurrentMeanSpeed));
    ret->mkItem("mean vehicle length [m]", true,
                new FunctionBinding<MSE2Collector, SUMOReal>(&myDetector, &MSE2Collector::getCurrentMeanLength));
    ret->mkItem("jam number [#]", true,
                new FunctionBinding<MSE2Collector, unsigned>(&myDetector, &MSE2Collector::getCurrentJamNumber));
    ret->mkItem("max jam length [veh]", true,
                new FunctionBinding<MSE2Collector, unsigned>(&myDetector, &MSE2Collector::getCurrentMaxJamLengthInVehicles));
    ret->mkItem("max jam length [m]", true,
                new FunctionBinding<MSE2Collector, SUMOReal>(&myDetector, &MSE2Collector::getCurrentMaxJamLengthInMeters));
    ret->mkItem("jam length sum [veh]", true,
                new FunctionBinding<MSE2Collector, unsigned>(&myDetector, &MSE2Collector::getCurrentJamLengthInVehicles));
    ret->mkItem("jam length sum [m]", true,
                new FunctionBinding<MSE2Collector, SUMOReal>(&myDetector, &MSE2Collector::getCurrentJamLengthInMeters));
    ret->mkItem("started halts [#]", true,
                new FunctionBinding<MSE2Collector, unsigned>(&myDetector, &MSE2Collector::getCurrentStartedHalts));
    // close building
    ret->closeBuilding();
    return ret;
}


void
GUI_E2_ZS_Collector::MyWrapper::drawGL(const GUIVisualizationSettings& s) const {
    glPushName(getGlID());
    glPushMatrix();
    glTranslated(0, 0, getType());
    SUMOReal dwidth = 1;
    if (myDetector.getUsageType() == DU_TL_CONTROL) {
        dwidth = (SUMOReal) 0.3;
        glColor3d(0, (SUMOReal) .6, (SUMOReal) .8);
    } else {
        glColor3d(0, (SUMOReal) .8, (SUMOReal) .8);
    }
    SUMOReal width = 2; // !!!
    if (width * s.addExaggeration > 1.0) {
        glScaled(s.addExaggeration, s.addExaggeration, 1);
        GLHelper::drawBoxLines(myFullGeometry, myShapeRotations, myShapeLengths, dwidth);
    } else {
        int e = (int) myFullGeometry.size() - 1;
        for (int i = 0; i < e; ++i) {
            GLHelper::drawLine(myFullGeometry[i], myShapeRotations[i], myShapeLengths[i]);
        }
    }
    glPopMatrix();
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    glPopName();
}


GUI_E2_ZS_Collector&
GUI_E2_ZS_Collector::MyWrapper::getDetector() {
    return myDetector;
}



/****************************************************************************/

