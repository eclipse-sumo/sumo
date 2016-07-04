/****************************************************************************/
/// @file    GNEConnection.cpp
/// @author  Pablo Álvarez López
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
#include "GNEEdge.h"
#include "GNELane.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS



// ===========================================================================
// static member definitions
// ===========================================================================


// ===========================================================================
// method definitions
// ===========================================================================

GNEConnection::GNEConnection(GNEEdge *edgeFrom, GNEEdge *edgeTo, GNELane *fromLane, GNELane *toLane, bool pass, bool keepClear, SUMOReal contPos, bool uncontrolled) :
    GNENetElement(NULL, "CHANGE", GLO_CONNECTION, SUMO_TAG_CONNECTION), 
    myEdgeFrom(edgeFrom),
    myEdgeTo(edgeTo),
    myFromLane(fromLane),
    myToLane(toLane),
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
}


GNEEdge *
GNEConnection::getEdgeFrom() {
    return myEdgeFrom;
}


GNEEdge *
GNEConnection::getEdgeTo() {
    return myEdgeTo;
}


GNELane *
GNEConnection::getFromLane() {
    return myFromLane;
}


GNELane *
GNEConnection::getToLane() {
    return myToLane;
}


int 
GNEConnection::getFromLaneIndex() {
    return myFromLane->getIndex();
}


int 
GNEConnection::getToLaneIndex() {
    return myToLane->getIndex();
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
GNEConnection::getCenteringBoundary() const{
    return Boundary();
}


void 
GNEConnection::drawGL(const GUIVisualizationSettings& s) const {
    // Currently ignored before implementation to avoid warnings
    UNUSED_PARAMETER(s);
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
