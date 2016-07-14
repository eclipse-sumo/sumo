/****************************************************************************/
/// @file    GNEConnection.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
/// @version $Id$
///
// A class for visualizing connections between lanes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
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

#include <string>
#include <iostream>
#include <utility>
#include <time.h>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/common/ToString.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEConnection.h"
#include "GNEJunction.h"
#include "GNEEdge.h"
#include "GNELane.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS



// ===========================================================================
// static member definitions
// ===========================================================================
int NUM_POINTS = 5;

// ===========================================================================
// method definitions
// ===========================================================================

GNEConnection::GNEConnection(GNEEdge &from, int fromLane, GNEEdge &to, int toLane, bool pass, bool keepClear, SUMOReal contPos, bool uncontrolled, int tlIndex) :
    GNENetElement(from.getNet(), from.getID() + "_" + toString(fromLane) + "_" + to.getID() + "_" + toString(toLane) , GLO_JUNCTION, SUMO_TAG_CONNECTION),
    myNBConnection(from.getNBEdge(), fromLane, to.getNBEdge(), toLane, tlIndex), 
    myConnection(fromLane, to.getNBEdge(), toLane),
    myFromEdge(from),
    myToEdge(to),
    myJunction(from.getNBEdge()->getToNode()),
    myPass(pass),
    myKeepClear(keepClear),
    myContPos(contPos),
    myUncontrolled(uncontrolled) {
    // Update geometry
    updateGeometry();
}

GNEConnection::~GNEConnection() {}


void
GNEConnection::updateGeometry() {
    // Get shape of connection
    myShape = myJunction->computeInternalLaneShape(myFromEdge.getNBEdge(), myConnection, NUM_POINTS);
    int segments = (int) myShape.size() - 1;
    if (segments >= 0) {
        myShapeRotations.reserve(segments);
        myShapeLengths.reserve(segments);
        for (int i = 0; i < segments; ++i) {
            const Position& f = myShape[i];
            const Position& s = myShape[i + 1];
            myShapeLengths.push_back(f.distanceTo2D(s));
            myShapeRotations.push_back((SUMOReal) atan2((s.x() - f.x()), (f.y() - s.y())) * (SUMOReal) 180.0 / (SUMOReal) PI);
        }
    }
}


Boundary 
GNEConnection::getBoundary() const {
    return Boundary();
}


GNEEdge&
GNEConnection::getEdgeFrom() const {
    return myFromEdge;
}


GNEEdge&
GNEConnection::getEdgeTo() const {
    return myToEdge;
}


GNELane* 
GNEConnection::getFromLane() const {
    return myFromEdge.getLanes().at(myConnection.fromLane);
}


GNELane* 
GNEConnection::getToLane() const {
    return myToEdge.getLanes().at(myConnection.toLane);
}


int
GNEConnection::getFromLaneIndex() const {
    return myConnection.fromLane;
}


int
GNEConnection::getToLaneIndex() const {
    return myConnection.toLane;
}


bool
GNEConnection::getPass() {
    return myPass;
}


bool
GNEConnection::getKeepClear() {
    return myKeepClear;
}


SUMOReal
GNEConnection::getContPos() {
    return myContPos;
}


bool
GNEConnection::getUncontrolled() {
    return myUncontrolled;
}


const NBConnection& 
GNEConnection::getNBConnection() const {
    return myNBConnection;
}


const NBEdge::Connection&
GNEConnection::getNBEdgeConnection() const {
    return myConnection;
}

void
GNEConnection::setPass(bool pass) {
    myPass = pass;
}


void
GNEConnection::setKeepClear(bool keepClear) {
    myKeepClear = keepClear;
}


void
GNEConnection::setContPos(SUMOReal contPos) {
    myContPos = contPos;
}


void
GNEConnection::setUncontrolled(bool uncontrolled) {
    myUncontrolled = uncontrolled ;
}


GUIGLObjectPopupMenu*
GNEConnection::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    // Currently ignored before implementation to avoid warnings
    UNUSED_PARAMETER(app);
    UNUSED_PARAMETER(parent);
    return NULL;
}


GUIParameterTableWindow*
GNEConnection::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    // Currently ignored before implementation to avoid warnings
    UNUSED_PARAMETER(app);
    UNUSED_PARAMETER(parent);
    return NULL;
}


Boundary
GNEConnection::getCenteringBoundary() const {
    return Boundary();
}


void
GNEConnection::drawGL(const GUIVisualizationSettings& s) const {
    glPushMatrix();
    glPushName(getGlID());
    glTranslated(0, 0, GLO_JUNCTION + 0.1); // must draw on top of junction
//    GLHelper::setColor(colorForLinksState(myState));
    // draw lane
    // check whether it is not too small
    if (s.scale < 1.) {
        GLHelper::drawLine(myShape);
    } else {
        GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, 0.2);
    }
    glPopName();
    glPopMatrix();
}


std::string
GNEConnection::getAttribute(SumoXMLAttr key) const {
    // Currently ignored before implementation to avoid warnings
    UNUSED_PARAMETER(key);
    return "";
}


void
GNEConnection::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    // Currently ignored before implementation to avoid warnings
    UNUSED_PARAMETER(key);
    UNUSED_PARAMETER(value);
    UNUSED_PARAMETER(undoList);
}


bool
GNEConnection::isValid(SumoXMLAttr key, const std::string& value) {
    // Currently ignored before implementation to avoid warnings
    UNUSED_PARAMETER(key);
    UNUSED_PARAMETER(value);
    return false;
}


void
GNEConnection::setAttribute(SumoXMLAttr key, const std::string& value) {
    // Currently ignored before implementation to avoid warnings
    UNUSED_PARAMETER(key);
    UNUSED_PARAMETER(value);
}


/****************************************************************************/
