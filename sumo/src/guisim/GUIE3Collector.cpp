/****************************************************************************/
/// @file    GUIE3Collector.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Jan 2004
/// @version $Id$
///
// The gui-version of a MSE3Collector
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

#include "GUIE3Collector.h"
#include "GUIEdge.h"
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GLHelper.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/MSLane.h>
#include <utils/gui/globjects/GLIncludes.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * GUIE3Collector::MyWrapper-methods
 * ----------------------------------------------------------------------- */
GUIE3Collector::MyWrapper::MyWrapper(GUIE3Collector& detector)
    : GUIDetectorWrapper("E3 detector", detector.getID()),
      myDetector(detector) {
    const CrossSectionVector& entries = detector.getEntries();
    const CrossSectionVector& exits = detector.getExits();
    CrossSectionVectorConstIt i;
    for (i = entries.begin(); i != entries.end(); ++i) {
        SingleCrossingDefinition def = buildDefinition(*i);
        myBoundary.add(def.myFGPosition);
        myEntryDefinitions.push_back(def);
    }
    for (i = exits.begin(); i != exits.end(); ++i) {
        SingleCrossingDefinition def = buildDefinition(*i);
        myBoundary.add(def.myFGPosition);
        myExitDefinitions.push_back(def);
    }
}


GUIE3Collector::MyWrapper::~MyWrapper() {}


GUIE3Collector::MyWrapper::SingleCrossingDefinition
GUIE3Collector::MyWrapper::buildDefinition(const MSCrossSection& section) {
    SingleCrossingDefinition def;
    def.myFGPosition = section.myLane->geometryPositionAtOffset(section.myPosition);
    def.myFGRotation = -section.myLane->getShape().rotationDegreeAtOffset(section.myPosition);
    return def;
}


GUIParameterTableWindow*
GUIE3Collector::MyWrapper::getParameterWindow(GUIMainWindow& app,
        GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 3);
    // add items
    // values
    ret->mkItem("vehicles within [#]", true,
                new FunctionBinding<MSE3Collector, int>(&myDetector, &MSE3Collector::getVehiclesWithin));
    ret->mkItem("mean speed [m/s]", true,
                new FunctionBinding<MSE3Collector, SUMOReal>(&myDetector, &MSE3Collector::getCurrentMeanSpeed));
    ret->mkItem("haltings [#]", true,
                new FunctionBinding<MSE3Collector, int>(&myDetector, &MSE3Collector::getCurrentHaltingNumber));
    // close building
    ret->closeBuilding();
    return ret;
}


void
GUIE3Collector::MyWrapper::drawGL(const GUIVisualizationSettings& s) const {
    glPushName(getGlID());
    glPushMatrix();
    glTranslated(0, 0, getType());
    typedef std::vector<SingleCrossingDefinition> CrossingDefinitions;
    CrossingDefinitions::const_iterator i;
    glColor3d(0, .8, 0);
    const SUMOReal exaggeration = s.addSize.getExaggeration(s);
    for (i = myEntryDefinitions.begin(); i != myEntryDefinitions.end(); ++i) {
        drawSingleCrossing((*i).myFGPosition, (*i).myFGRotation, exaggeration);
    }
    glColor3d(.8, 0, 0);
    for (i = myExitDefinitions.begin(); i != myExitDefinitions.end(); ++i) {
        drawSingleCrossing((*i).myFGPosition, (*i).myFGRotation, exaggeration);
    }
    glPopMatrix();
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    glPopName();
}


void
GUIE3Collector::MyWrapper::drawSingleCrossing(const Position& pos,
        SUMOReal rot, SUMOReal upscale) const {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPushMatrix();
    glScaled(upscale, upscale, 1);
    glTranslated(pos.x(), pos.y(), 0);
    glRotated(rot, 0, 0, 1);
    glBegin(GL_LINES);
    glVertex2d(1.7, 0);
    glVertex2d(-1.7, 0);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2d(-1.7, .5);
    glVertex2d(-1.7, -.5);
    glVertex2d(1.7, -.5);
    glVertex2d(1.7, .5);
    glEnd();
    // arrows
    glTranslated(1.5, 0, 0);
    GLHelper::drawBoxLine(Position(0, 4), 0, 2, .05);
    GLHelper::drawTriangleAtEnd(Position(0, 4), Position(0, 1), (SUMOReal) 1, (SUMOReal) .25);
    glTranslated(-3, 0, 0);
    GLHelper::drawBoxLine(Position(0, 4), 0, 2, .05);
    GLHelper::drawTriangleAtEnd(Position(0, 4), Position(0, 1), (SUMOReal) 1, (SUMOReal) .25);
    glPopMatrix();
}


Boundary
GUIE3Collector::MyWrapper::getCenteringBoundary() const {
    Boundary b(myBoundary);
    b.grow(20);
    return b;
}


GUIE3Collector&
GUIE3Collector::MyWrapper::getDetector() {
    return myDetector;
}


/* -------------------------------------------------------------------------
 * GUIE3Collector-methods
 * ----------------------------------------------------------------------- */
GUIE3Collector::GUIE3Collector(const std::string& id,
                               const CrossSectionVector& entries,  const CrossSectionVector& exits,
                               SUMOReal haltingSpeedThreshold,
                               SUMOTime haltingTimeThreshold, const std::string& vTypes)
    : MSE3Collector(id, entries,  exits, haltingSpeedThreshold, haltingTimeThreshold, vTypes) {}


GUIE3Collector::~GUIE3Collector() {}


const CrossSectionVector&
GUIE3Collector::getEntries() const {
    return myEntries;
}


const CrossSectionVector&
GUIE3Collector::getExits() const {
    return myExits;
}



GUIDetectorWrapper*
GUIE3Collector::buildDetectorGUIRepresentation() {
    return new MyWrapper(*this);
}



/****************************************************************************/

