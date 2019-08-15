/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNECalibrator.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/dialogs/GNECalibratorDialog.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNECalibrator.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNECalibrator::GNECalibrator(const std::string& id, GNEViewNet* viewNet, GNEEdge* edge, double pos, SUMOTime frequency, const std::string& name, const std::string& output, const std::string& routeprobe) :
    GNEAdditional(id, viewNet, GLO_CALIBRATOR, SUMO_TAG_CALIBRATOR, name, false, {
    edge
}, {}, {}, {}, {}, {}, {}, {}, {}, {}),
myPositionOverLane(pos),
myFrequency(frequency),
myOutput(output),
myRouteProbe(routeprobe) {
}


GNECalibrator::GNECalibrator(const std::string& id, GNEViewNet* viewNet, GNELane* lane, double pos, SUMOTime frequency, const std::string& name, const std::string& output, const std::string& routeprobe) :
    GNEAdditional(id, viewNet, GLO_CALIBRATOR, SUMO_TAG_LANECALIBRATOR, name, false, {}, {lane}, {}, {}, {}, {}, {}, {}, {}, {}),
myPositionOverLane(pos),
myFrequency(frequency),
myOutput(output),
myRouteProbe(routeprobe) {
}


GNECalibrator::~GNECalibrator() {}


void
GNECalibrator::moveGeometry(const Position&) {
    // This additional cannot be moved
}


void
GNECalibrator::commitGeometryMoving(GNEUndoList*) {
    // This additional cannot be moved
}


void
GNECalibrator::updateGeometry() {
    // Clear all containers
    myGeometry.clearGeometry();
    // get shape depending of we have a edge or a lane
    if (getLaneParents().size() > 0) {
        // Get shape of lane parent
        myGeometry.shape.push_back(getLaneParents().front()->getGeometry().shape.positionAtOffset(myPositionOverLane));
        // Save rotation (angle) of the vector constructed by points f and s
        myGeometry.shapeRotations.push_back(getLaneParents().front()->getGeometry().shape.rotationDegreeAtOffset(myPositionOverLane) * -1);
    } else if (getEdgeParents().size() > 0) {
        for (auto i : getEdgeParents().front()->getLanes()) {
            // Get shape of lane parent
            myGeometry.shape.push_back(i->getGeometry().shape.positionAtOffset(myPositionOverLane));
            // Save rotation (angle) of the vector constructed by points f and s
            myGeometry.shapeRotations.push_back(getEdgeParents().front()->getLanes().at(0)->getGeometry().shape.rotationDegreeAtOffset(myPositionOverLane) * -1);
        }
    } else {
        throw ProcessError("Both myEdge and myLane aren't defined");
    }
}


Position
GNECalibrator::getPositionInView() const {
    PositionVector shape = (getLaneParents().size() > 0) ? getLaneParents().front()->getGeometry().shape : getEdgeParents().front()->getLanes().at(0)->getGeometry().shape;
    if (myPositionOverLane < 0) {
        return shape.front();
    } else if (myPositionOverLane > shape.length()) {
        return shape.back();
    } else {
        return shape.positionAtOffset(myPositionOverLane);
    }
}


Boundary
GNECalibrator::getCenteringBoundary() const {
    return myGeometry.shape.getBoxBoundary().grow(10);
}


std::string
GNECalibrator::getParentName() const {
    // get parent name depending of we have a edge or a lane
    if (getLaneParents().size() > 0) {
        return getLaneParents().front()->getMicrosimID();
    } else if (getEdgeParents().size() > 0) {
        return getEdgeParents().front()->getLanes().at(0)->getMicrosimID();
    } else {
        throw ProcessError("Both myEdge and myLane aren't defined");
    }
}


void
GNECalibrator::drawGL(const GUIVisualizationSettings& s) const {
    // get values
    const double exaggeration = s.addSize.getExaggeration(s, this);
    // begin draw
    glPushName(getGlID());
    glLineWidth(1.0);
    // iterate over every Calibrator symbol
    for (int i = 0; i < (int)myGeometry.shape.size(); ++i) {
        const Position& pos = myGeometry.shape[i];
        double rot = myGeometry.shapeRotations[i];
        glPushMatrix();
        glTranslated(pos.x(), pos.y(), getType());
        glRotated(rot, 0, 0, 1);
        glTranslated(0, 0, getType());
        glScaled(exaggeration, exaggeration, 1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        // set color
        if (drawUsingSelectColor()) {
            GLHelper::setColor(s.colorSettings.selectedAdditionalColor);
        } else {
            GLHelper::setColor(s.colorSettings.calibrator);
        }
        // base
        glBegin(GL_TRIANGLES);
        glVertex2d(0 - 1.4, 0);
        glVertex2d(0 - 1.4, 6);
        glVertex2d(0 + 1.4, 6);
        glVertex2d(0 + 1.4, 0);
        glVertex2d(0 - 1.4, 0);
        glVertex2d(0 + 1.4, 6);
        glEnd();
        // draw text if isn't being drawn for selecting
        if (!s.drawForSelecting && s.drawDetail(s.detailSettings.calibratorText, exaggeration)) {
            // set color depending of selection status
            RGBColor textColor = drawUsingSelectColor() ? s.colorSettings.selectionColor : RGBColor::BLACK;
            // draw "C"
            GLHelper::drawText("C", Position(0, 1.5), 0.1, 3, textColor, 180);
            // draw "edge" or "lane "
            if (getLaneParents().size() > 0) {
                GLHelper::drawText("lane", Position(0, 3), .1, 1, textColor, 180);
            } else if (getEdgeParents().size() > 0) {
                GLHelper::drawText("edge", Position(0, 3), .1, 1, textColor, 180);
            } else {
                throw ProcessError("Both myEdge and myLane aren't defined");
            }
        }
        glPopMatrix();
        // check if dotted contour has to be drawn
        if (myViewNet->getDottedAC() == this) {
            GLHelper::drawShapeDottedContourRectangle(s, getType(), pos, 2.8, 6, rot, 0, 3);
        }
    }
    // draw name
    drawName(getPositionInView(), s.scale, s.addName);
    // pop name
    glPopName();
}


void
GNECalibrator::openAdditionalDialog() {
    // Open calibrator dialog
    GNECalibratorDialog calibratorDialog(this);
}


std::string
GNECalibrator::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getAdditionalID();
        case SUMO_ATTR_EDGE:
            return getEdgeParents().front()->getID();
        case SUMO_ATTR_LANE:
            return getLaneParents().front()->getID();
        case SUMO_ATTR_POSITION:
            return toString(myPositionOverLane);
        case SUMO_ATTR_FREQUENCY:
            return time2string(myFrequency);
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case SUMO_ATTR_OUTPUT:
            return myOutput;
        case SUMO_ATTR_ROUTEPROBE:
            return myRouteProbe;
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_GENERIC:
            return getGenericParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNECalibrator::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_EDGE:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_FREQUENCY:
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_OUTPUT:
        case SUMO_ATTR_ROUTEPROBE:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_GENERIC:
            undoList->p_add(new GNEChange_Attribute(this, myViewNet->getNet(), key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }

}


bool
GNECalibrator::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(value);
        case SUMO_ATTR_EDGE:
            if (myViewNet->getNet()->retrieveEdge(value, false) != nullptr) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_LANE:
            if (myViewNet->getNet()->retrieveLane(value, false) != nullptr) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_POSITION:
            if (canParse<double>(value)) {
                // obtain position and check if is valid
                double newPosition = parse<double>(value);
                PositionVector shape = (getLaneParents().size() > 0) ? getLaneParents().front()->getGeometry().shape : getEdgeParents().front()->getLanes().at(0)->getGeometry().shape;
                if ((newPosition < 0) || (newPosition > shape.length())) {
                    return false;
                } else {
                    return true;
                }
            } else {
                return false;
            }
        case SUMO_ATTR_FREQUENCY:
            return canParse<SUMOTime>(value);
        case SUMO_ATTR_NAME:
            return SUMOXMLDefinitions::isValidAttribute(value);
        case SUMO_ATTR_OUTPUT:
            return SUMOXMLDefinitions::isValidFilename(value);
        case SUMO_ATTR_ROUTEPROBE:
            return SUMOXMLDefinitions::isValidNetID(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_GENERIC:
            return isGenericParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNECalibrator::getPopUpID() const {
    return getTagStr() + ": " + getID();
}


std::string
GNECalibrator::getHierarchyName() const {
    return getTagStr();
}

// ===========================================================================
// private
// ===========================================================================

void
GNECalibrator::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            changeAdditionalID(value);
            break;
        case SUMO_ATTR_EDGE:
            changeEdgeParents(this, value);
            break;
        case SUMO_ATTR_LANE:
            changeLaneParents(this, value);
            break;
        case SUMO_ATTR_POSITION:
            myPositionOverLane = parse<double>(value);
            break;
        case SUMO_ATTR_FREQUENCY:
            myFrequency = parse<SUMOTime>(value);
            break;
        case SUMO_ATTR_NAME:
            myAdditionalName = value;
            break;
        case SUMO_ATTR_OUTPUT:
            myOutput = value;
            break;
        case SUMO_ATTR_ROUTEPROBE:
            myRouteProbe = value;
            break;
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_GENERIC:
            setGenericParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
