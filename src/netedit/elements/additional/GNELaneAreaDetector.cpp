/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNELaneAreaDetector.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
//
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_Connection.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/frames/network/GNETLSEditorFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/div/GUIGlobalPostDrawing.h>

#include "GNELaneAreaDetector.h"
#include "GNEAdditionalHandler.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNELaneAreaDetector::GNELaneAreaDetector(SumoXMLTag tag, GNENet* net) :
    GNEDetector("", net, GLO_E2DETECTOR, tag, GUIIconSubSys::getIcon(GUIIcon::E2),
                0, 0, {}, "", {}, "", false, Parameterised::Map()),
            myEndPositionOverLane(0),
            myTimeThreshold(0),
            mySpeedThreshold(0),
myJamThreshold(0) {
    // reset default values
    resetDefaultValues();
}


GNELaneAreaDetector::GNELaneAreaDetector(const std::string& id, GNELane* lane, GNENet* net, double pos, double length, const SUMOTime freq,
        const std::string& trafficLight, const std::string& filename, const std::vector<std::string>& vehicleTypes, const std::string& name,
        SUMOTime timeThreshold, double speedThreshold, double jamThreshold, bool friendlyPos, const Parameterised::Map& parameters) :
    GNEDetector(id, net, GLO_E2DETECTOR, SUMO_TAG_LANE_AREA_DETECTOR, GUIIconSubSys::getIcon(GUIIcon::E2),
                pos, freq, {
    lane
}, filename, vehicleTypes, name, friendlyPos, parameters),
myEndPositionOverLane(pos + length),
myTimeThreshold(timeThreshold),
mySpeedThreshold(speedThreshold),
myJamThreshold(jamThreshold),
myTrafficLight(trafficLight) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNELaneAreaDetector::GNELaneAreaDetector(const std::string& id, std::vector<GNELane*> lanes, GNENet* net, double pos, double endPos, const SUMOTime freq,
        const std::string& trafficLight, const std::string& filename, const std::vector<std::string>& vehicleTypes, const std::string& name, SUMOTime timeThreshold,
        double speedThreshold, double jamThreshold, bool friendlyPos, const Parameterised::Map& parameters) :
    GNEDetector(id, net, GLO_E2DETECTOR, GNE_TAG_MULTI_LANE_AREA_DETECTOR, GUIIconSubSys::getIcon(GUIIcon::E2),
                pos, freq, lanes, filename, vehicleTypes, name, friendlyPos, parameters),
    myEndPositionOverLane(endPos),
    myTimeThreshold(timeThreshold),
    mySpeedThreshold(speedThreshold),
    myJamThreshold(jamThreshold),
    myTrafficLight(trafficLight) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNELaneAreaDetector::~GNELaneAreaDetector() {
}


void
GNELaneAreaDetector::writeAdditional(OutputDevice& device) const {
    device.openTag(SUMO_TAG_LANE_AREA_DETECTOR);
    device.writeAttr(SUMO_ATTR_ID, getID());
    if (!myAdditionalName.empty()) {
        device.writeAttr(SUMO_ATTR_NAME, StringUtils::escapeXML(myAdditionalName));
    }
    // continue depending of E2 type
    if (myTagProperty.getTag() == SUMO_TAG_LANE_AREA_DETECTOR) {
        device.writeAttr(SUMO_ATTR_LANE, getParentLanes().front()->getID());
        device.writeAttr(SUMO_ATTR_POSITION, myPositionOverLane);
        device.writeAttr(SUMO_ATTR_LENGTH, toString(myEndPositionOverLane - myPositionOverLane));
    } else {
        device.writeAttr(SUMO_ATTR_LANES, getAttribute(SUMO_ATTR_LANES));
        device.writeAttr(SUMO_ATTR_POSITION, myPositionOverLane);
        device.writeAttr(SUMO_ATTR_ENDPOS, myEndPositionOverLane);
    }
    if (myTrafficLight.size() > 0) {
        device.writeAttr(SUMO_ATTR_TLID, myTrafficLight);
    }
    if (getAttribute(SUMO_ATTR_PERIOD).size() > 0) {
        device.writeAttr(SUMO_ATTR_PERIOD, time2string(myPeriod));
    }
    if (myFilename.size() > 0) {
        device.writeAttr(SUMO_ATTR_FILE, myFilename);
    }
    if (myVehicleTypes.size() > 0) {
        device.writeAttr(SUMO_ATTR_VTYPES, myVehicleTypes);
    }
    if (getAttribute(SUMO_ATTR_HALTING_TIME_THRESHOLD) != myTagProperty.getDefaultValue(SUMO_ATTR_HALTING_TIME_THRESHOLD)) {
        device.writeAttr(SUMO_ATTR_HALTING_TIME_THRESHOLD, mySpeedThreshold);
    }
    if (getAttribute(SUMO_ATTR_HALTING_SPEED_THRESHOLD) != myTagProperty.getDefaultValue(SUMO_ATTR_HALTING_SPEED_THRESHOLD)) {
        device.writeAttr(SUMO_ATTR_HALTING_SPEED_THRESHOLD, mySpeedThreshold);
    }
    if (getAttribute(SUMO_ATTR_JAM_DIST_THRESHOLD) != myTagProperty.getDefaultValue(SUMO_ATTR_JAM_DIST_THRESHOLD)) {
        device.writeAttr(SUMO_ATTR_JAM_DIST_THRESHOLD, mySpeedThreshold);
    }
    if (myFriendlyPosition) {
        device.writeAttr(SUMO_ATTR_FRIENDLY_POS, true);
    }
    // write parameters (Always after children to avoid problems with additionals.xsd)
    writeParams(device);
    device.closeTag();
}


bool
GNELaneAreaDetector::isAdditionalValid() const {
    if (getParentLanes().size() == 1) {
        // with friendly position enabled position are "always fixed"
        if (myFriendlyPosition) {
            return true;
        } else {
            return (myPositionOverLane >= 0) && (myEndPositionOverLane <= getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength());
        }
    } else {
        // first check if there is connection between all consecutive lanes
        if (areLaneConnected(getParentLanes())) {
            // with friendly position enabled position are "always fixed"
            if (myFriendlyPosition) {
                return true;
            } else {
                return (myPositionOverLane >= 0) &&
                       (myEndPositionOverLane >= 0) &&
                       (myPositionOverLane <= getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength()) &&
                       (myEndPositionOverLane <= getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength());
            }
        } else {
            return false;
        }
    }
}


std::string
GNELaneAreaDetector::getAdditionalProblem() const {
    // declare variable for error position
    std::string errorFirstLanePosition, separator, errorLastLanePosition;
    if (getParentLanes().size() == 1) {
        // check positions over lane
        if (myPositionOverLane < 0) {
            errorFirstLanePosition = (toString(SUMO_ATTR_POSITION) + " < 0");
        }
        if (myPositionOverLane > getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength()) {
            errorFirstLanePosition = (toString(SUMO_ATTR_POSITION) + TL(" > lanes's length"));
        }
    } else {
        // abort if lanes aren't consecutives
        if (!areLaneConsecutives(getParentLanes())) {
            return TL("lanes aren't consecutives");
        }
        // abort if lanes aren't connected
        if (!areLaneConnected(getParentLanes())) {
            return TL("lanes aren't connected");
        }
        // check positions over first lane
        if (myPositionOverLane < 0) {
            errorFirstLanePosition = (toString(SUMO_ATTR_POSITION) + " < 0");
        }
        if (myPositionOverLane > getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength()) {
            errorFirstLanePosition = (toString(SUMO_ATTR_POSITION) + TL(" > lanes's length"));
        }
        // check positions over last lane
        if (myEndPositionOverLane < 0) {
            errorLastLanePosition = (toString(SUMO_ATTR_ENDPOS) + " < 0");
        }
        if (myEndPositionOverLane > getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength()) {
            errorLastLanePosition = (toString(SUMO_ATTR_ENDPOS) + TL(" > lanes's length"));
        }
    }
    // check separator
    if ((errorFirstLanePosition.size() > 0) && (errorLastLanePosition.size() > 0)) {
        separator = TL(" and ");
    }
    // return error message
    return errorFirstLanePosition + separator + errorLastLanePosition;
}


void
GNELaneAreaDetector::fixAdditionalProblem() {
    if (getParentLanes().size() == 1) {
        // obtain position and length
        double newPositionOverLane = myPositionOverLane;
        double newLength = (myEndPositionOverLane - myPositionOverLane);
        // fix pos and length using fixE2DetectorPosition
        GNEAdditionalHandler::fixLanePosition(newPositionOverLane, newLength, getParentLanes().at(0)->getParentEdge()->getNBEdge()->getFinalLength());
        // set new position and length
        setAttribute(SUMO_ATTR_POSITION, toString(newPositionOverLane), myNet->getViewNet()->getUndoList());
        setAttribute(SUMO_ATTR_LENGTH, toString(newLength), myNet->getViewNet()->getUndoList());
    } else {
        if (!areLaneConsecutives(getParentLanes())) {
            // build connections between all consecutive lanes
            bool foundConnection = true;
            int i = 0;
            // iterate over all lanes, and stop if myE2valid is false
            while (i < ((int)getParentLanes().size() - 1)) {
                // change foundConnection to false
                foundConnection = false;
                // if a connection betwen "from" lane and "to" lane of connection is found, change myE2valid to true again
                for (const auto& connection : getParentLanes().at(i)->getParentEdge()->getGNEConnections()) {
                    if ((connection->getLaneFrom() == getParentLanes().at(i)) && (connection->getLaneTo() == getParentLanes().at(i + 1))) {
                        foundConnection = true;
                    }
                }
                // if connection wasn't found
                if (!foundConnection) {
                    // create new connection manually
                    NBEdge::Connection newCon(getParentLanes().at(i)->getIndex(), getParentLanes().at(i + 1)->getParentEdge()->getNBEdge(), getParentLanes().at(i + 1)->getIndex());
                    // allow to undo creation of new lane
                    myNet->getViewNet()->getUndoList()->add(new GNEChange_Connection(getParentLanes().at(i)->getParentEdge(), newCon, false, true), true);
                }
                // update lane iterator
                i++;
            }
        } else {
            // declare new positions
            double newPositionOverLane = myPositionOverLane;
            double newEndPositionOverLane = myEndPositionOverLane;
            // fix pos and length checkAndFixDetectorPosition
            GNEAdditionalHandler::fixMultiLanePosition(
                newPositionOverLane, getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength(),
                newEndPositionOverLane, getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength());
            // set new position and endPosition
            setAttribute(SUMO_ATTR_POSITION, toString(newPositionOverLane), myNet->getViewNet()->getUndoList());
            setAttribute(SUMO_ATTR_ENDPOS, toString(newEndPositionOverLane), myNet->getViewNet()->getUndoList());
        }
    }
}


void
GNELaneAreaDetector::updateGeometry() {
    // check E2 detector
    if (myTagProperty.getTag() == GNE_TAG_MULTI_LANE_AREA_DETECTOR) {
        // compute path
        computePathElement();
    } else {
        // Cut shape using as delimitators fixed start position and fixed end position
        myAdditionalGeometry.updateGeometry(getParentLanes().front()->getLaneShape(), getStartGeometryPositionOverLane(), getEndGeometryPositionOverLane(), myMoveElementLateralOffset);
        // update centering boundary without updating grid
        updateCenteringBoundary(false);
    }
}


void
GNELaneAreaDetector::drawGL(const GUIVisualizationSettings& s) const {
    // check if additional has to be drawn
    if ((myTagProperty.getTag() == SUMO_TAG_LANE_AREA_DETECTOR) && myNet->getViewNet()->getDataViewOptions().showAdditionals() &&
            !myNet->getViewNet()->selectingDetectorsTLSMode()) {
        // Obtain exaggeration of the draw
        const double E2Exaggeration = getExaggeration(s);
        // check exaggeration
        if (s.drawAdditionals(E2Exaggeration)) {
            // declare color
            RGBColor E2Color, textColor;
            // set color
            if (drawUsingSelectColor()) {
                E2Color = s.colorSettings.selectedAdditionalColor;
                textColor = E2Color.changedBrightness(-32);
            } else if (areLaneConsecutives(getParentLanes())) {
                E2Color = s.detectorSettings.E2Color;
                textColor = RGBColor::BLACK;
            }
            // avoid draw invisible elements
            if (E2Color.alpha() != 0) {
                // draw parent and child lines
                drawParentChildLines(s, s.additionalSettings.connectionColor);
                // Start drawing adding an gl identificator
                GLHelper::pushName(getGlID());
                // push layer matrix
                GLHelper::pushMatrix();
                // translate to front
                myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_E2DETECTOR);
                // set color
                GLHelper::setColor(E2Color);
                // draw geometry
                GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), myAdditionalGeometry, s.detectorSettings.E2Width * E2Exaggeration);
                // arrow
                if (myAdditionalGeometry.getShape().size() > 1) {
                    glTranslated(0, 0, 0.1);
                    GLHelper::drawTriangleAtEnd(myAdditionalGeometry.getShape()[-2], myAdditionalGeometry.getShape()[-1], (double) 0.5, (double) 1, 0.5);
                }
                // Check if the distance is enought to draw details
                if (s.drawDetail(s.detailSettings.detectorDetails, E2Exaggeration)) {
                    // draw E2 Logo
                    drawE2DetectorLogo(s, E2Exaggeration, "E2", textColor);
                }
                // draw geometry points
                drawLeftGeometryPoint(myNet->getViewNet(), myAdditionalGeometry.getShape().front(), myAdditionalGeometry.getShapeRotations().front(), E2Color);
                drawRightGeometryPoint(myNet->getViewNet(), myAdditionalGeometry.getShape().back(), myAdditionalGeometry.getShapeRotations().back(), E2Color);
                // pop layer matrix
                GLHelper::popMatrix();
                // Pop name
                GLHelper::popName();
                // draw lock icon
                GNEViewNetHelper::LockIcon::drawLockIcon(this, getType(), myAdditionalGeometry.getShape().getCentroid(), E2Exaggeration);
            }
            // check if mouse is over element
            mouseWithinGeometry(myAdditionalGeometry.getShape(), s.detectorSettings.E2Width);
            // inspect contour
            if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::INSPECT, myAdditionalGeometry.getShape(), s.detectorSettings.E2Width,
                        E2Exaggeration, true, true);
            }
            // front element contour
            if (myNet->getViewNet()->getFrontAttributeCarrier() == this) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::FRONT, myAdditionalGeometry.getShape(), s.detectorSettings.E2Width,
                        E2Exaggeration, true, true);
            }
            // delete contour
            if (myNet->getViewNet()->drawDeleteContour(this, this)) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::REMOVE, myAdditionalGeometry.getShape(), s.detectorSettings.E2Width,
                        E2Exaggeration, true, true);
            }
            // select contour
            if (myNet->getViewNet()->drawSelectContour(this, this)) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::SELECT, myAdditionalGeometry.getShape(), s.detectorSettings.E2Width,
                        E2Exaggeration, true, true);
            }
        }
        // Draw additional ID
        drawAdditionalID(s);
        // draw additional name
        drawAdditionalName(s);
    }
}


void
GNELaneAreaDetector::computePathElement() {
    // calculate path
    myNet->getPathManager()->calculateConsecutivePathLanes(this, getParentLanes());
}


void
GNELaneAreaDetector::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* lane, const GNEPathManager::Segment* segment, const double offsetFront) const {
    // calculate E2Detector width
    const double E2DetectorWidth = s.addSize.getExaggeration(s, lane);
    // check if E2 can be drawn
    if (s.drawAdditionals(E2DetectorWidth) && myNet->getViewNet()->getDataViewOptions().showAdditionals() &&
            !myNet->getViewNet()->selectingDetectorsTLSMode()) {
        // calculate startPos
        const double geometryDepartPos = getAttributeDouble(SUMO_ATTR_POSITION);
        // get endPos
        const double geometryEndPos = getAttributeDouble(SUMO_ATTR_ENDPOS);
        // declare path geometry
        GUIGeometry E2Geometry;
        // update pathGeometry depending of first and last segment
        if (segment->isFirstSegment() && segment->isLastSegment()) {
            E2Geometry.updateGeometry(lane->getLaneGeometry().getShape(),
                                      geometryDepartPos, geometryEndPos,      // extrem positions
                                      Position::INVALID, Position::INVALID);  // extra positions
        } else if (segment->isFirstSegment()) {
            E2Geometry.updateGeometry(lane->getLaneGeometry().getShape(),
                                      geometryDepartPos, -1,                  // extrem positions
                                      Position::INVALID, Position::INVALID);  // extra positions
        } else if (segment->isLastSegment()) {
            E2Geometry.updateGeometry(lane->getLaneGeometry().getShape(),
                                      -1, geometryEndPos,                     // extrem positions
                                      Position::INVALID, Position::INVALID);  // extra positions
        } else {
            E2Geometry = lane->getLaneGeometry();
        }
        // obtain color
        const RGBColor E2Color = drawUsingSelectColor() ? s.colorSettings.selectedAdditionalColor : s.detectorSettings.E2Color;
        // Start drawing adding an gl identificator
        GLHelper::pushName(getGlID());
        // push layer matrix
        GLHelper::pushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, getType() + offsetFront);
        // Set color
        GLHelper::setColor(E2Color);
        // draw geometry
        GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), E2Geometry, E2DetectorWidth);
        // draw geometry points
        if (segment->isFirstSegment() && segment->isLastSegment()) {
            drawLeftGeometryPoint(myNet->getViewNet(), E2Geometry.getShape().front(),  E2Geometry.getShapeRotations().front(), E2Color, true);
            drawRightGeometryPoint(myNet->getViewNet(), E2Geometry.getShape().back(), E2Geometry.getShapeRotations().back(), E2Color, true);
        } else if (segment->isFirstSegment()) {
            drawLeftGeometryPoint(myNet->getViewNet(), E2Geometry.getShape().front(), E2Geometry.getShapeRotations().front(), E2Color, true);
        } else if (segment->isLastSegment()) {
            drawRightGeometryPoint(myNet->getViewNet(), E2Geometry.getShape().back(), E2Geometry.getShapeRotations().back(), E2Color, true);
            // draw arrow
            if (E2Geometry.getShape().size() > 1) {
                glTranslated(0, 0, 0.1);
                GLHelper::drawTriangleAtEnd(E2Geometry.getShape()[-2], E2Geometry.getShape()[-1], (double) 0.5, (double) 1, 0.5);
            }
        }
        // Pop layer matrix
        GLHelper::popMatrix();
        // Pop name
        GLHelper::popName();
        // draw additional ID
        if (!s.drawForRectangleSelection) {
            drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
            // check if this is the label segment
            if (segment->isLabelSegment()) {
                // calculate middle point
                const double middlePoint = (E2Geometry.getShape().length2D() * 0.5);
                // calculate position
                const Position pos = E2Geometry.getShape().positionAtOffset2D(middlePoint);
                // calculate rotation
                const double rot = s.getTextAngle((E2Geometry.getShape().rotationDegreeAtOffset(middlePoint) * -1) + 90);
                // Start pushing matrix
                GLHelper::pushMatrix();
                // Traslate to position
                glTranslated(pos.x(), pos.y(), getType() + offsetFront + 0.1);
                // rotate
                glRotated(rot, 0, 0, 1);
                // move
                glTranslated(-1, 0, 0);
                // scale text
                glScaled(E2DetectorWidth, E2DetectorWidth, 1);
                // draw E1 logo
                GLHelper::drawText("E2 Multilane", Position(), .1, 1.5, RGBColor::BLACK);
                // pop matrix
                GLHelper::popMatrix();
            }
        }
        // declare trim geometry to draw
        const auto shape = (segment->isFirstSegment() || segment->isLastSegment()) ? E2Geometry.getShape() : lane->getLaneShape();
        // check if mouse is over element
        mouseWithinGeometry(shape, s.detectorSettings.E2Width);
        // inspect contour
        if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
            GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::INSPECT, shape, E2DetectorWidth, 1, segment->isFirstSegment(), segment->isLastSegment());
        }
        // dotted contour
        if (myNet->getViewNet()->getFrontAttributeCarrier() == this) {
            GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::FRONT, shape, E2DetectorWidth, 1, segment->isFirstSegment(), segment->isLastSegment());
        }
        // delete contour
        if (myNet->getViewNet()->drawDeleteContour(this, this)) {
            GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::REMOVE, shape, E2DetectorWidth, 1, segment->isFirstSegment(), segment->isLastSegment());
        }
        // select contour
        if (myNet->getViewNet()->drawSelectContour(this, this)) {
            GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::SELECT, shape, E2DetectorWidth, 1, segment->isFirstSegment(), segment->isLastSegment());
        }
    }
}


void
GNELaneAreaDetector::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* fromLane, const GNELane* toLane, const GNEPathManager::Segment* /*segment*/, const double offsetFront) const {
    // calculate E2Detector width
    const double E2DetectorWidth = s.addSize.getExaggeration(s, fromLane);
    // check if E2 can be drawn
    if (s.drawAdditionals(E2DetectorWidth) && myNet->getViewNet()->getDataViewOptions().showAdditionals() && !myNet->getViewNet()->selectingDetectorsTLSMode()) {
        // get flag for show only contour
        const bool onlyContour = myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork() ? myNet->getViewNet()->getNetworkViewOptions().showConnections() : false;
        // Start drawing adding an gl identificator
        GLHelper::pushName(getGlID());
        // Add a draw matrix
        GLHelper::pushMatrix();
        // Start with the drawing of the area traslating matrix to origin
        glTranslated(0, 0, getType() + offsetFront);
        // Set color of the base
        if (drawUsingSelectColor()) {
            GLHelper::setColor(s.colorSettings.selectedAdditionalColor);
        } else {
            GLHelper::setColor(s.detectorSettings.E2Color);
        }
        // draw lane2lane
        if (fromLane->getLane2laneConnections().exist(toLane)) {
            // check if draw only contour
            if (onlyContour) {
                GUIGeometry::drawContourGeometry(fromLane->getLane2laneConnections().getLane2laneGeometry(toLane), E2DetectorWidth);
            } else {
                GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), fromLane->getLane2laneConnections().getLane2laneGeometry(toLane), E2DetectorWidth);
            }
        } else {
            // Set invalid person plan color
            GLHelper::setColor(RGBColor::RED);
            // calculate invalid geometry
            const GUIGeometry invalidGeometry({fromLane->getLaneShape().back(), toLane->getLaneShape().front()});
            // check if draw only contour
            if (onlyContour) {
                GUIGeometry::drawContourGeometry(invalidGeometry, (0.5 * E2DetectorWidth));
            } else {
                // draw invalid geometry
                GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), invalidGeometry, (0.5 * E2DetectorWidth));
            }
        }
        // Pop last matrix
        GLHelper::popMatrix();
        // Pop name
        GLHelper::popName();
        // continue with dotted contours
        if (fromLane->getLane2laneConnections().exist(toLane)) {
            // check if mouse is over element
            mouseWithinGeometry(fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(), s.detectorSettings.E2Width);
            // inspect contour
            if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::INSPECT, fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
                        E2DetectorWidth, 1, false, false);
            }
            // front contour
            if (myNet->getViewNet()->getFrontAttributeCarrier() == this) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::FRONT, fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
                        E2DetectorWidth, 1, false, false);
            }
            // delete contour
            if (myNet->getViewNet()->drawDeleteContour(this, this)) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::REMOVE, fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
                        E2DetectorWidth, 1, false, false);
            }
            // select contour
            if (myNet->getViewNet()->drawSelectContour(this, this)) {
                GUIDottedGeometry::drawDottedContourShape(s, GUIDottedGeometry::DottedContourType::SELECT, fromLane->getLane2laneConnections().getLane2laneGeometry(toLane).getShape(),
                        E2DetectorWidth, 1, false, false);
            }
        }
    }
}


std::string
GNELaneAreaDetector::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_LANES:
            return parseIDs(getParentLanes());
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_STARTPOS:
            return toString(myPositionOverLane);
        case SUMO_ATTR_ENDPOS:
            return toString(myEndPositionOverLane);
        case SUMO_ATTR_PERIOD:
            if (myPeriod == SUMOTime_MAX_PERIOD) {
                return "";
            } else {
                return time2string(myPeriod);
            }
        case SUMO_ATTR_TLID:
            return myTrafficLight;
        case SUMO_ATTR_LENGTH:
            return toString(myEndPositionOverLane - myPositionOverLane);
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case SUMO_ATTR_FILE:
            return myFilename;
        case SUMO_ATTR_VTYPES:
            return toString(myVehicleTypes);
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return time2string(myTimeThreshold);
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            return toString(mySpeedThreshold);
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
            return toString(myJamThreshold);
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(myFriendlyPosition);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        case GNE_ATTR_SHIFTLANEINDEX:
            return "";
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNELaneAreaDetector::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_POSITION:
            return myPositionOverLane;
        case SUMO_ATTR_LENGTH:
            return (myEndPositionOverLane - myPositionOverLane);
        case SUMO_ATTR_ENDPOS:
            return myEndPositionOverLane;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNELaneAreaDetector::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_LANES:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_ENDPOS:
        case SUMO_ATTR_PERIOD:
        case SUMO_ATTR_TLID:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_FILE:
        case SUMO_ATTR_VTYPES:
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
        case SUMO_ATTR_FRIENDLY_POS:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
        case GNE_ATTR_SHIFTLANEINDEX:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNELaneAreaDetector::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            if (value == getID()) {
                return true;
            } else if (isValidDetectorID(value)) {
                return (myNet->getAttributeCarriers()->retrieveAdditional(GNE_TAG_MULTI_LANE_AREA_DETECTOR, value, false) == nullptr) &&
                       (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_LANE_AREA_DETECTOR, value, false) == nullptr);
            } else {
                return false;
            }
        case SUMO_ATTR_LANE:
            if (value.empty()) {
                return false;
            } else {
                return canParse<std::vector<GNELane*> >(myNet, value, false);
            }
        case SUMO_ATTR_LANES:
            if (value.empty()) {
                return false;
            } else if (canParse<std::vector<GNELane*> >(myNet, value, false)) {
                // check if lanes are consecutives
                return lanesConsecutives(parse<std::vector<GNELane*> >(myNet, value));
            } else {
                return false;
            }
        case SUMO_ATTR_POSITION:
            return canParse<double>(value);
        case SUMO_ATTR_ENDPOS:
            return canParse<double>(value);
        case SUMO_ATTR_PERIOD:
            if (value.empty()) {
                return true;
            } else {
                return (canParse<double>(value) && (parse<double>(value) >= 0));
            }
        case SUMO_ATTR_TLID:
            // temporal
            return SUMOXMLDefinitions::isValidNetID(value);
        case SUMO_ATTR_LENGTH:
            return (canParse<double>(value) && (parse<double>(value) >= 0));
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
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return canParse<SUMOTime>(value);
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            return (canParse<double>(value) && (parse<double>(value) >= 0));
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
            return (canParse<double>(value) && (parse<double>(value) >= 0));
        case SUMO_ATTR_FRIENDLY_POS:
            return canParse<bool>(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNELaneAreaDetector::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            // update microsimID
            setMicrosimID(value);
            break;
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_LANES:
            replaceAdditionalParentLanes(value);
            break;
        case SUMO_ATTR_POSITION:
            myPositionOverLane = parse<double>(value);
            // update geometry (except for template)
            if (getParentLanes().size() > 0) {
                updateGeometry();
            }
            break;
        case SUMO_ATTR_ENDPOS:
            myEndPositionOverLane = parse<double>(value);
            // update geometry (except for template)
            if (getParentLanes().size() > 0) {
                updateGeometry();
            }
            break;
        case SUMO_ATTR_PERIOD:
            if (value.empty()) {
                myPeriod = SUMOTime_MAX_PERIOD;
            } else {
                myPeriod = string2time(value);
            }
            break;
        case SUMO_ATTR_TLID:
            myTrafficLight = value;
            break;
        case SUMO_ATTR_LENGTH:
            myEndPositionOverLane = (myPositionOverLane + parse<double>(value));
            // update geometry (except for template)
            if (getParentLanes().size() > 0) {
                updateGeometry();
            }
            break;
        case SUMO_ATTR_NAME:
            myAdditionalName = value;
            break;
        case SUMO_ATTR_FILE:
            myFilename = value;
            break;
        case SUMO_ATTR_VTYPES:
            myVehicleTypes = parse<std::vector<std::string> >(value);
            break;
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            myTimeThreshold = TIME2STEPS(parse<double>(value));
            break;
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            mySpeedThreshold = parse<double>(value);
            break;
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
            myJamThreshold = parse<double>(value);
            break;
        case SUMO_ATTR_FRIENDLY_POS:
            myFriendlyPosition = parse<bool>(value);
            break;
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_PARAMETERS:
            setParametersStr(value);
            break;
        case GNE_ATTR_SHIFTLANEINDEX:
            shiftLaneIndex();
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNELaneAreaDetector::setMoveShape(const GNEMoveResult& moveResult) {
    if ((moveResult.operationType == GNEMoveOperation::OperationType::ONE_LANE_MOVEFIRST) ||
            (moveResult.operationType == GNEMoveOperation::OperationType::TWO_LANES_MOVEFIRST)) {
        // change only start position
        myPositionOverLane = moveResult.newFirstPos;
    } else if ((moveResult.operationType == GNEMoveOperation::OperationType::ONE_LANE_MOVESECOND) ||
               (moveResult.operationType == GNEMoveOperation::OperationType::TWO_LANES_MOVESECOND)) {
        // change only end position
        myEndPositionOverLane = moveResult.newFirstPos;
    } else {
        // change both position
        myPositionOverLane = moveResult.newFirstPos;
        myEndPositionOverLane = moveResult.newSecondPos;
    }
    // update geometry
    updateGeometry();
}


void
GNELaneAreaDetector::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // begin change attribute
    undoList->begin(myTagProperty.getGUIIcon(), "position of " + getTagStr());
    // set attributes depending of operation type
    if ((moveResult.operationType == GNEMoveOperation::OperationType::ONE_LANE_MOVEFIRST) ||
            (moveResult.operationType == GNEMoveOperation::OperationType::TWO_LANES_MOVEFIRST)) {
        // set only start position
        setAttribute(SUMO_ATTR_POSITION, toString(moveResult.newFirstPos), undoList);
    } else if ((moveResult.operationType == GNEMoveOperation::OperationType::ONE_LANE_MOVESECOND) ||
               (moveResult.operationType == GNEMoveOperation::OperationType::TWO_LANES_MOVESECOND)) {
        // set only end position
        setAttribute(SUMO_ATTR_ENDPOS, toString(moveResult.newFirstPos), undoList);
    } else {
        // set both positions
        setAttribute(SUMO_ATTR_POSITION, toString(moveResult.newFirstPos), undoList);
        setAttribute(SUMO_ATTR_ENDPOS, toString(moveResult.newSecondPos), undoList);
    }
    // end change attribute
    undoList->end();
}


double
GNELaneAreaDetector::getStartGeometryPositionOverLane() const {
    // get lane final and shape length
    const double laneLength = getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    // get startPosition
    double fixedPos = myPositionOverLane;
    // adjust fixedPos
    if (fixedPos < 0) {
        fixedPos += laneLength;
    }
    fixedPos *= getParentLanes().front()->getLengthGeometryFactor();
    // return depending of fixedPos
    if (fixedPos < 0) {
        return 0;
    } else if (fixedPos > (getParentLanes().front()->getLaneShapeLength() - POSITION_EPS)) {
        return (getParentLanes().front()->getLaneShapeLength() - POSITION_EPS);
    } else {
        return fixedPos;
    }
}


double
GNELaneAreaDetector::getEndGeometryPositionOverLane() const {
    // get lane final and shape length
    const double laneLength = getParentLanes().back()->getParentEdge()->getNBEdge()->getFinalLength();
    // get endPosition
    double fixedPos = myEndPositionOverLane;
    // adjust fixedPos
    if (fixedPos < 0) {
        fixedPos += laneLength;
    }
    fixedPos *= getParentLanes().back()->getLengthGeometryFactor();
    // return depending of fixedPos
    if (fixedPos < POSITION_EPS) {
        return POSITION_EPS;
    } else if (fixedPos > getParentLanes().back()->getLaneShapeLength()) {
        return getParentLanes().back()->getLaneShapeLength();
    } else {
        return fixedPos;
    }
}

/****************************************************************************/
