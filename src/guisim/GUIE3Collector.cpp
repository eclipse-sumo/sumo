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
/// @file    GUIE3Collector.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Jan 2004
///
// The gui-version of a MSE3Collector
/****************************************************************************/
#include <config.h>

#include "GUIE3Collector.h"
#include "GUIEdge.h"
#include <utils/common/MsgHandler.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/div/GLHelper.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/MSLane.h>
#include <utils/gui/globjects/GLIncludes.h>


// ===========================================================================
// method definitions
// ===========================================================================

// -------------------------------------------------------------------------
// GUIE3Collector::MyWrapper-methods
// -------------------------------------------------------------------------

GUIE3Collector::MyWrapper::MyWrapper(GUIE3Collector& detector) :
    GUIDetectorWrapper(GLO_E3DETECTOR, detector.getID(), GUIIconSubSys::getIcon(GUIIcon::E3)),
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
        new GUIParameterTableWindow(app, *this);
    // add items
    // values
    ret->mkItem(TL("name"), false, myDetector.myName);
    if (myDetector.isTyped()) {
        ret->mkItem(TL("vTypes"), false, toString(myDetector.getVehicleTypes()));
    }
    ret->mkItem(TL("vehicles within [#]"), true,
                new FunctionBinding<MSE3Collector, int>(&myDetector, &MSE3Collector::getVehiclesWithin));
    ret->mkItem(TL("mean speed [m/s]"), true,
                new FunctionBinding<MSE3Collector, double>(&myDetector, &MSE3Collector::getCurrentMeanSpeed));
    ret->mkItem(TL("haltings [#]"), true,
                new FunctionBinding<MSE3Collector, int>(&myDetector, &MSE3Collector::getCurrentHaltingNumber));
    ret->mkItem(TL("last interval mean travel time [s]"), true,
                new FunctionBinding<MSE3Collector, double>(&myDetector, &MSE3Collector::getLastIntervalMeanTravelTime));
    ret->mkItem(TL("last interval mean haltings [#]"), true,
                new FunctionBinding<MSE3Collector, double>(&myDetector, &MSE3Collector::getLastIntervalMeanHaltsPerVehicle));
    ret->mkItem(TL("last interval mean time loss [s]"), true,
                new FunctionBinding<MSE3Collector, double>(&myDetector, &MSE3Collector::getLastIntervalMeanTimeLoss));
    ret->mkItem(TL("last interval mean vehicle count [#]"), true,
                new FunctionBinding<MSE3Collector, int>(&myDetector, &MSE3Collector::getLastIntervalVehicleSum));

    // close building
    ret->closeBuilding(&myDetector);
    return ret;
}


void
GUIE3Collector::MyWrapper::drawGL(const GUIVisualizationSettings& s) const {
    GLHelper::pushName(getGlID());
    GLHelper::pushMatrix();
    glTranslated(0, 0, GLO_JUNCTION + 0.4); // do not draw on top of linkRules
    typedef std::vector<SingleCrossingDefinition> CrossingDefinitions;
    CrossingDefinitions::const_iterator i;
    GLHelper::setColor(s.detectorSettings.E3EntryColor);
    const double exaggeration = getExaggeration(s);
    for (i = myEntryDefinitions.begin(); i != myEntryDefinitions.end(); ++i) {
        drawSingleCrossing((*i).myFGPosition, (*i).myFGRotation, exaggeration);
    }
    GLHelper::setColor(s.detectorSettings.E3ExitColor);
    for (i = myExitDefinitions.begin(); i != myExitDefinitions.end(); ++i) {
        drawSingleCrossing((*i).myFGPosition, (*i).myFGRotation, exaggeration);
    }
    GLHelper::popMatrix();
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    GLHelper::popName();
}


void
GUIE3Collector::MyWrapper::drawSingleCrossing(const Position& pos,
        double rot, double upscale) const {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    GLHelper::pushMatrix();
    glTranslated(pos.x(), pos.y(), 0);
    glRotated(rot, 0, 0, 1);
    glScaled(upscale, upscale, 1);
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
    GLHelper::drawTriangleAtEnd(Position(0, 4), Position(0, 1), (double) 1, (double) .25);
    glTranslated(-3, 0, 0);
    GLHelper::drawBoxLine(Position(0, 4), 0, 2, .05);
    GLHelper::drawTriangleAtEnd(Position(0, 4), Position(0, 1), (double) 1, (double) .25);
    GLHelper::popMatrix();
}


double
GUIE3Collector::MyWrapper::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.addSize.getExaggeration(s, this);
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
                               double haltingSpeedThreshold,
                               SUMOTime haltingTimeThreshold,
                               const std::string name, const std::string& vTypes,
                               const std::string& nextEdges,
                               int detectPersons,
                               bool openEntry, bool expectArrival):
    MSE3Collector(id, entries,  exits, haltingSpeedThreshold, haltingTimeThreshold, name, vTypes, nextEdges, detectPersons, openEntry, expectArrival)
{}


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
