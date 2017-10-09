/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
/// @file    GNEDetector.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
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
#include <utils/geom/PositionVector.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
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
GNEDetector::setFilename(const std::string& filename) {
    myFilename = filename;
}

double GNEDetector::getAbsolutePositionOverLane() const {
    return myPositionOverLane * myLane->getLaneParametricLength();
}


void GNEDetector::moveGeometry(const Position&, const Position &offset) {
    // First we need to change the absolute new position to a relative position
    double lenghtDifference = 0;
    if (myLane->getLaneShapeLength() > 0) {
        lenghtDifference = myLane->getLaneParametricLength() / myLane->getLaneShapeLength();
    }
    double relativePos = offset.x() / myLane->getLaneParametricLength() * lenghtDifference;
    // change relative position position over lane
    myPositionOverLane += relativePos;
    // Update geometry
    updateGeometry();
}


void GNEDetector::commitGeometryMoving(const Position& oldPos, GNEUndoList* undoList) {
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


/****************************************************************************/
