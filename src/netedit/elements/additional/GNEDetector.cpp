/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEDetector.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
//
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNESegment.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/gui/div/GLHelper.h>

#include "GNEDetector.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDetector::GNEDetector(GNENet* net, SumoXMLTag tag) :
    GNEAdditional("", net, "", tag, "") {
}


GNEDetector::GNEDetector(const std::string& id, GNENet* net, const std::string& filename, SumoXMLTag tag,
                         const SUMOTime period, const std::string& outputFilename, const std::vector<std::string>& vehicleTypes,
                         const std::vector<std::string>& nextEdges, const std::string& detectPersons, const std::string& name,
                         const Parameterised::Map& parameters) :
    GNEAdditional(id, net, filename, tag, name),
    Parameterised(parameters),
    myPeriod(period),
    myOutputFilename(outputFilename),
    myVehicleTypes(vehicleTypes),
    myNextEdges(nextEdges),
    myDetectPersons(detectPersons) {
    // update output filename
    if (outputFilename.empty()) {
        myOutputFilename = id + ".xml";
    }
}


GNEDetector::GNEDetector(GNEAdditional* additionalParent, SumoXMLTag tag, const SUMOTime period,
                         const std::string& outputFilename, const std::string& name,
                         const Parameterised::Map& parameters) :
    GNEAdditional(additionalParent, tag, name),
    Parameterised(parameters),
    myPeriod(period),
    myOutputFilename(outputFilename) {
    // set parents
    setParent<GNEAdditional*>(additionalParent);
}


GNEDetector::~GNEDetector() {}


Parameterised*
GNEDetector::getParameters() {
    return this;
}


const Parameterised*
GNEDetector::getParameters() const {
    return this;
}


bool
GNEDetector::checkDrawMoveContour() const {
    // get edit modes
    const auto& editModes = myNet->getViewNet()->getEditModes();
    // check if we're in move mode
    if (!myNet->getViewNet()->isCurrentlyMovingElements() && editModes.isCurrentSupermodeNetwork() &&
            !myNet->getViewNet()->getEditNetworkElementShapes().getEditedNetworkElement() &&
            (editModes.networkEditMode == NetworkEditMode::NETWORK_MOVE) && myNet->getViewNet()->checkOverLockedElement(this, mySelected)) {
        // only move the first element
        if (myNet->getViewNet()->getViewObjectsSelector().getGUIGlObjectFront() == this) {
            // special case for multiple lane area detectors
            if (myTagProperty->getTag() == GNE_TAG_MULTI_LANE_AREA_DETECTOR) {
                auto segment = gViewObjectsHandler.getSelectedSegment(this);
                if (segment && segment->getJunction()) {
                    return false;
                } else if (segment && segment->getLane()) {
                    // ensure that is the first or the last lane
                    if (segment->getLaneIndex() == 0) {
                        return true;
                    } else if (segment->getLaneIndex() == ((int)getParentLanes().size() - 1)) {
                        return true;
                    }
                } else {
                    // this is the start or end point
                    return true;
                }
            } else {
                return true;
            }
        }
    }
    return false;
}


Position
GNEDetector::getPositionInView() const {
    return myAdditionalGeometry.getShape().getPolygonCenter();
}


void
GNEDetector::updateCenteringBoundary(const bool /*updateGrid*/) {
    // nothing to update
}


void
GNEDetector::splitEdgeGeometry(const double splitPosition, const GNENetworkElement* originalElement,
                               const GNENetworkElement* newElement, GNEUndoList* undoList) {
    // only split geometry of E2 multilane detectors
    if (myTagProperty->getTag() == GNE_TAG_MULTI_LANE_AREA_DETECTOR) {
        // obtain new list of E2 lanes
        std::string newE2Lanes = getNewListOfParents(originalElement, newElement);
        // update E2 Lanes
        if (newE2Lanes.size() > 0) {
            setAttribute(SUMO_ATTR_LANES, newE2Lanes, undoList);
        }
    } else if (splitPosition < getAttributeDouble(SUMO_ATTR_POSITION)) {
        // change lane
        setAttribute(SUMO_ATTR_LANE, newElement->getID(), undoList);
        // now adjust start position
        setAttribute(SUMO_ATTR_POSITION, toString(getAttributeDouble(SUMO_ATTR_POSITION) - splitPosition), undoList);
    }
}


std::string
GNEDetector::getParentName() const {
    return getParentLanes().front()->getID();
}


PositionVector GNEDetector::getAttributePositionVector(SumoXMLAttr key) const {
    return getCommonAttributePositionVector(key);
}


std::string
GNEDetector::getPopUpID() const {
    return getTagStr() + ": " + getID();
}


std::string
GNEDetector::getHierarchyName() const {
    return getTagStr();
}


std::string
GNEDetector::getDetectorAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_PERIOD:
            if (myPeriod == SUMOTime_MAX_PERIOD) {
                return "";
            } else {
                return time2string(myPeriod);
            }
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case SUMO_ATTR_FILE:
            return myOutputFilename;
        case SUMO_ATTR_VTYPES:
            return toString(myVehicleTypes);
        case SUMO_ATTR_NEXT_EDGES:
            return toString(myNextEdges);
        case SUMO_ATTR_DETECT_PERSONS:
            return toString(myDetectPersons);
        case GNE_ATTR_SHIFTLANEINDEX:
            return "";
        default:
            return getMoveElement()->getMovingAttribute(key);
    }
}


double
GNEDetector::getDetectorAttributeDouble(SumoXMLAttr key) const {
    return getMoveElement()->getMovingAttributeDouble(key);
}


Position
GNEDetector::getDetectorAttributePosition(SumoXMLAttr key) const {
    return getMoveElement()->getMovingAttributePosition(key);
}


void
GNEDetector::setDetectorAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_PERIOD:
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_FILE:
        case SUMO_ATTR_VTYPES:
        case SUMO_ATTR_NEXT_EDGES:
        case SUMO_ATTR_DETECT_PERSONS:
        case GNE_ATTR_SHIFTLANEINDEX:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            getMoveElement()->setMovingAttribute(key, value, undoList);
            break;
    }
}



bool
GNEDetector::isDetectorValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidDetectorID(value);
        case SUMO_ATTR_PERIOD:
            if (value.empty()) {
                return true;
            } else {
                return (canParse<double>(value) && (parse<double>(value) >= 0));
            }
        case SUMO_ATTR_NAME:
            return SUMOXMLDefinitions::isValidAttribute(value);
        case SUMO_ATTR_FILE:
            return SUMOXMLDefinitions::isValidFilename(value);
        case SUMO_ATTR_VTYPES:
            if (value.empty()) {
                return true;
            } else {
                return SUMOXMLDefinitions::isValidListOfTypeID(value);
            }
        case SUMO_ATTR_NEXT_EDGES:
            if (value.empty()) {
                return true;
            } else {
                return SUMOXMLDefinitions::isValidListOfNetIDs(value);
            }
        case SUMO_ATTR_DETECT_PERSONS:
            if (value.empty()) {
                return true;
            } else {
                return SUMOXMLDefinitions::PersonModeValues.hasString(value);
            }
        default:
            return getMoveElement()->isMovingAttributeValid(key, value);
    }
}


void
GNEDetector::writeDetectorValues(OutputDevice& device) const {
    if ((myPeriod > 0) && (myPeriod != SUMOTime_MAX_PERIOD)) {
        device.writeAttr(SUMO_ATTR_PERIOD, time2string(myPeriod));
    }
    if (myOutputFilename.size() > 0) {
        device.writeAttr(SUMO_ATTR_FILE, myOutputFilename);
    }
    if (myVehicleTypes.size() > 0) {
        device.writeAttr(SUMO_ATTR_VTYPES, myVehicleTypes);
    }
    if (myNextEdges.size() > 0) {
        device.writeAttr(SUMO_ATTR_NEXT_EDGES, myNextEdges);
    }
    if ((myDetectPersons.size() > 0) && (myDetectPersons != SUMOXMLDefinitions::PersonModeValues.getString(PersonMode::NONE))) {
        device.writeAttr(SUMO_ATTR_DETECT_PERSONS, myDetectPersons);
    }
}


void
GNEDetector::setDetectorAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            // update microsimID
            setAdditionalID(value);
            break;
        case SUMO_ATTR_PERIOD:
            if (value.empty()) {
                myPeriod = SUMOTime_MAX_PERIOD;
            } else {
                myPeriod = string2time(value);
            }
            break;
        case SUMO_ATTR_FILE:
            myOutputFilename = value;
            break;
        case SUMO_ATTR_NAME:
            myAdditionalName = value;
            break;
        case SUMO_ATTR_VTYPES:
            myVehicleTypes = parse<std::vector<std::string> >(value);
            break;
        case SUMO_ATTR_NEXT_EDGES:
            myNextEdges = parse<std::vector<std::string> >(value);
            break;
        case SUMO_ATTR_DETECT_PERSONS:
            myDetectPersons = value;
            break;
        case GNE_ATTR_SHIFTLANEINDEX:
            shiftLaneIndex();
            break;
        default:
            getMoveElement()->setMovingAttribute(key, value);
            break;
    }
}


void
GNEDetector::drawE1Shape(const GUIVisualizationSettings::Detail d, const double exaggeration,
                         const RGBColor& mainColor, const RGBColor& secondColor) const {
    // push matrix
    GLHelper::pushMatrix();
    // set line width
    glLineWidth(1.0);
    // translate to center geometry
    glTranslated(myAdditionalGeometry.getShape().front().x(), myAdditionalGeometry.getShape().front().y(), 0);
    // rotate over lane
    GUIGeometry::rotateOverLane(myAdditionalGeometry.getShapeRotations().front() + 90);
    // scale
    glScaled(exaggeration, exaggeration, 1);
    // set main color
    GLHelper::setColor(mainColor);
    // begin draw square
    glBegin(GL_QUADS);
    // draw square
    glVertex2d(-1.0,  2);
    glVertex2d(-1.0, -2);
    glVertex2d(1.0, -2);
    glVertex2d(1.0,  2);
    // end draw square
    glEnd();
    // move top
    glTranslated(0, 0, .01);
    // begin draw line
    glBegin(GL_LINES);
    // draw lines
    glVertex2d(0, 2 - .1);
    glVertex2d(0, -2 + .1);
    // end draw line
    glEnd();
    // draw center only in draw in level 2
    if (d <= GUIVisualizationSettings::Detail::AdditionalDetails) {
        // set main color
        GLHelper::setColor(secondColor);
        // set polygon mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // begin draw square
        glBegin(GL_QUADS);
        // draw square
        glVertex2f(-1.0,  2);
        glVertex2f(-1.0, -2);
        glVertex2f(1.0, -2);
        glVertex2f(1.0,  2);
        // end draw square
        glEnd();
        // rotate 90 degrees
        glRotated(90, 0, 0, -1);
        //set polygon mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        // begin draw line
        glBegin(GL_LINES);
        // draw line
        glVertex2d(0, 1.7);
        glVertex2d(0, -1.7);
        // end draw line
        glEnd();
        //arrow
        glTranslated(2, 0, 0);
        GLHelper::setColor(mainColor);
        GLHelper::drawTriangleAtEnd(Position(0, 0), Position(0.5, 0), (double) 0.5, (double) 1);
    }
    // pop matrix
    GLHelper::popMatrix();
}


void
GNEDetector::drawE1DetectorLogo(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                const double exaggeration, const std::string& logo, const RGBColor& textColor) const {
    // only draw in level 2
    if (d <= GUIVisualizationSettings::Detail::Text) {
        // calculate position
        const Position pos = myAdditionalGeometry.getShape().front();
        // calculate rotation
        const double rot = s.getTextAngle(myAdditionalGeometry.getShapeRotations().front() + 90);
        // Start pushing matrix
        GLHelper::pushMatrix();
        // Traslate to position
        glTranslated(pos.x(), pos.y(), 0.1);
        // scale text
        glScaled(exaggeration, exaggeration, 1);
        // draw E1 logo
        GLHelper::drawText(logo + "     ", Position(), .1, 1.5, textColor, rot);
        // pop matrix
        GLHelper::popMatrix();
    }
}


void
GNEDetector::drawE2DetectorLogo(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                const double exaggeration, const std::string& logo, const RGBColor& textColor) const {
    // only draw in level 2
    if (d <= GUIVisualizationSettings::Detail::Text) {
        // calculate middle point
        const double middlePoint = (myAdditionalGeometry.getShape().length2D() * 0.5);
        // calculate position
        const Position pos = myAdditionalGeometry.getShape().positionAtOffset2D(middlePoint);
        // calculate rotation
        const double rot = s.getTextAngle(myAdditionalGeometry.getShape().rotationDegreeAtOffset(middlePoint) + 90);
        // Start pushing matrix
        GLHelper::pushMatrix();
        // Traslate to position
        glTranslated(pos.x(), pos.y(), 0.1);
        // scale text
        glScaled(exaggeration, exaggeration, 1);
        // draw E1 logo
        GLHelper::drawText(logo, Position(), .1, 1.5, textColor, rot);
        // pop matrix
        GLHelper::popMatrix();
    }
}

/****************************************************************************/
