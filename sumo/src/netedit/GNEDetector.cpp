/****************************************************************************/
/// @file    GNEDetector.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
//
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

#include <string>
#include <iostream>
#include <utility>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/geom/PositionVector.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>

#include "GNEDetector.h"
#include "GNELane.h"
#include "GNEChange_Attribute.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEViewNet.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEDetector::GNEDetector(const std::string& id, GNEViewNet* viewNet, SumoXMLTag tag, GUIIcon icon, GNELane* lane, double pos, double freq, const std::string& filename, bool friendlyPos) :
    GNEAdditional(id, viewNet, tag, icon),
    myPositionOverLane(pos / lane->getLaneParametricLength()),
    myFreq(freq),
    myFilename(filename),
    myFriendlyPosition(friendlyPos) {
    // This additional belongs to a Lane
    myLane = lane;
}


GNEDetector::~GNEDetector() {
}


double
GNEDetector::getFrequency() const {
    return myFreq;
}


const std::string&
GNEDetector::getFilename() const {
    return myFilename;
}


void
GNEDetector::setFrequency(const double freq) {
    if (freq > 0) {
        myFreq = freq;
    } else {
        throw InvalidArgument("Frequency '" + toString(freq) + "' not allowed. Must be greater than 0");
    }
}


void
GNEDetector::setFilename(const std::string &filename) {
    myFilename = filename;
}

double GNEDetector::getAbsolutePositionOverLane() const {
    return myPositionOverLane * myLane->getLaneParametricLength();
}


void GNEDetector::moveGeometry(const Position & newPosition) {
    // First we need to change the absolute new position to a relative position
    double lenghtDifference = 0;
    if (myLane->getLaneShapeLength() > 0) {
        lenghtDifference = myLane->getLaneParametricLength() / myLane->getLaneShapeLength();
    }
    double relativePos = newPosition.x() / myLane->getLaneParametricLength() * lenghtDifference;
    // change relative position position over lane
    myPositionOverLane += relativePos;
    // Update geometry
    updateGeometry();
}


void GNEDetector::commitGeometryMoving(const Position & oldPos, GNEUndoList * undoList) {
    undoList->p_begin("position of " + toString(getTag()));
    undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(getAbsolutePositionOverLane()), true, toString(oldPos.x())));
    undoList->p_end();
    // Refresh element
    myViewNet->getNet()->refreshAdditional(this);
}

Position GNEDetector::getPositionInView() const {
    return myLane->getShape().positionAtOffset(myPositionOverLane * myLane->getShape().length());
}

const std::string&
GNEDetector::getParentName() const {
    return myLane->getMicrosimID();
}


void
GNEDetector::drawDetectorIcon(const int GNELogoID, double sizex, double sizey) const {
    // Add a draw matrix
    glPushMatrix();
    // Traslate to center
    glTranslated(myShape.getLineCenter().x(), myShape.getLineCenter().y(), getType() + 0.1);
    // Set color
    glColor3d(1, 1, 1);
    // Set first rotation
    glRotated(myBlockIconRotation, 0, 0, -1);
    // Set second rotation
    glRotated(180, 0, 0, 1);
    //Traslate offset
    glTranslated(myDetectorLogoOffset.x(), myDetectorLogoOffset.y(), 0);
    // Draw detector logo
    GUITexturesHelper::drawTexturedBox(GNELogoID, sizex, sizey, (-1 * sizex), (-1 * sizey));
    // Pop detector logo matrix
    glPopMatrix();
}

/****************************************************************************/
