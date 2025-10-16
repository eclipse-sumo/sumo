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
/// @file    GNELaneAreaDetector.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
///
//
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNESegment.h>
#include <netedit/GNETagProperties.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/gui/div/GLHelper.h>

#include "GNELaneAreaDetector.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNELaneAreaDetector::GNELaneAreaDetector(SumoXMLTag tag, GNENet* net) :
    GNEDetector(net, tag),
    myMoveElementLaneDouble(new GNEMoveElementLaneDouble(this, SUMO_ATTR_POSITION, myStartPosOverLane,
                            SUMO_ATTR_ENDPOS, myEndPosPosOverLane, myFriendlyPosition)) {
}


GNELaneAreaDetector::GNELaneAreaDetector(const std::string& id, GNENet* net, const std::string& filename, GNELane* lane, const double pos, const double length, const SUMOTime freq,
        const std::string& trafficLight, const std::string& outputFilename, const std::vector<std::string>& vehicleTypes, const std::vector<std::string>& nextEdges,
        const std::string& detectPersons, const std::string& name, const SUMOTime timeThreshold, const double speedThreshold, const double jamThreshold, const bool friendlyPos,
        const bool show, const Parameterised::Map& parameters) :
    GNEDetector(id, net, filename, SUMO_TAG_LANE_AREA_DETECTOR, freq, outputFilename, vehicleTypes, nextEdges, detectPersons, name, parameters),
    myStartPosOverLane(pos),
    myEndPosPosOverLane(pos + length),
    myFriendlyPosition(friendlyPos),
    myTimeThreshold(timeThreshold),
    mySpeedThreshold(speedThreshold),
    myJamThreshold(jamThreshold),
    myTrafficLight(trafficLight),
    myShow(show),
    myMoveElementLaneDouble(new GNEMoveElementLaneDouble(this, SUMO_ATTR_POSITION, myStartPosOverLane, SUMO_ATTR_ENDPOS, myEndPosPosOverLane, myFriendlyPosition)) {
    // set parents
    setParent<GNELane*>(lane);
}


GNELaneAreaDetector::GNELaneAreaDetector(const std::string& id, GNENet* net, const std::string& filename, std::vector<GNELane*> lanes, const double pos, const double endPos, const SUMOTime freq,
        const std::string& trafficLight, const std::string& outputFilename, const std::vector<std::string>& vehicleTypes, const std::vector<std::string>& nextEdges,
        const std::string& detectPersons, const std::string& name, const SUMOTime timeThreshold, const double speedThreshold, const double jamThreshold, const bool friendlyPos, const bool show,
        const Parameterised::Map& parameters) :
    GNEDetector(id, net, filename, GNE_TAG_MULTI_LANE_AREA_DETECTOR, freq, outputFilename, vehicleTypes, nextEdges,
                detectPersons, name, parameters),
    myStartPosOverLane(pos),
    myEndPosPosOverLane(endPos),
    myFriendlyPosition(friendlyPos),
    myTimeThreshold(timeThreshold),
    mySpeedThreshold(speedThreshold),
    myJamThreshold(jamThreshold),
    myTrafficLight(trafficLight),
    myShow(show),
    myMoveElementLaneDouble(new GNEMoveElementLaneDouble(this, SUMO_ATTR_POSITION, myStartPosOverLane, SUMO_ATTR_ENDPOS, myEndPosPosOverLane, myFriendlyPosition)) {
    // set parents
    setParents<GNELane*>(lanes);
}


GNELaneAreaDetector::~GNELaneAreaDetector() {
    delete myMoveElementLaneDouble;
}


GNEMoveElement*
GNELaneAreaDetector::getMoveElement() const {
    return myMoveElementLaneDouble;
}


Parameterised*
GNELaneAreaDetector::getParameters() {
    return this;
}


void
GNELaneAreaDetector::writeAdditional(OutputDevice& device) const {
    device.openTag(SUMO_TAG_LANE_AREA_DETECTOR);
    // write common additional attributes
    writeAdditionalAttributes(device);
    // write move atributes
    myMoveElementLaneDouble->writeMoveAttributes(device, (myTagProperty->getTag() == SUMO_TAG_LANE_AREA_DETECTOR));
    // write common detector parameters
    writeDetectorValues(device);
    // write specific attributes
    if (myTrafficLight.size() > 0) {
        device.writeAttr(SUMO_ATTR_TLID, myTrafficLight);
    }
    if (myTimeThreshold != myTagProperty->getDefaultTimeValue(SUMO_ATTR_HALTING_TIME_THRESHOLD)) {
        device.writeAttr(SUMO_ATTR_HALTING_TIME_THRESHOLD, time2string(myTimeThreshold));
    }
    if (mySpeedThreshold != myTagProperty->getDefaultDoubleValue(SUMO_ATTR_HALTING_SPEED_THRESHOLD)) {
        device.writeAttr(SUMO_ATTR_HALTING_SPEED_THRESHOLD, mySpeedThreshold);
    }
    if (myJamThreshold != myTagProperty->getDefaultDoubleValue(SUMO_ATTR_JAM_DIST_THRESHOLD)) {
        device.writeAttr(SUMO_ATTR_JAM_DIST_THRESHOLD, myJamThreshold);
    }
    if (myShow != myTagProperty->getDefaultBoolValue(SUMO_ATTR_SHOW_DETECTOR)) {
        device.writeAttr(SUMO_ATTR_SHOW_DETECTOR, myShow);
    }
    // write parameters (Always after children to avoid problems with additionals.xsd)
    writeParams(device);
    device.closeTag();
}


bool
GNELaneAreaDetector::isAdditionalValid() const {
    // only movement problems
    return myMoveElementLaneDouble->isMoveElementValid();
}


std::string
GNELaneAreaDetector::getAdditionalProblem() const {
    // only movement problems
    return myMoveElementLaneDouble->getMovingProblem();
}


void
GNELaneAreaDetector::fixAdditionalProblem() {
    // only movement problems
    return myMoveElementLaneDouble->fixMovingProblem();
}


void
GNELaneAreaDetector::updateGeometry() {
    // check E2 detector
    if (myTagProperty->getTag() == GNE_TAG_MULTI_LANE_AREA_DETECTOR) {
        // compute path
        computePathElement();
    } else {
        // Cut shape using as delimitators fixed start position and fixed end position
        myAdditionalGeometry.updateGeometry(getParentLanes().front()->getLaneShape(),
                                            myMoveElementLaneDouble->getStartFixedPositionOverLane(true),
                                            myMoveElementLaneDouble->getEndFixedPositionOverLane(true),
                                            myMoveElementLaneDouble->myMovingLateralOffset);
    }
}


void
GNELaneAreaDetector::drawGL(const GUIVisualizationSettings& s) const {
    // check drawing conditions
    if ((myTagProperty->getTag() == SUMO_TAG_LANE_AREA_DETECTOR) &&
            myNet->getViewNet()->getDataViewOptions().showAdditionals() &&
            !myNet->getViewNet()->selectingDetectorsTLSMode()) {
        // Obtain exaggeration of the draw
        const double E2Exaggeration = getExaggeration(s);
        // get detail level
        const auto d = s.getDetailLevel(E2Exaggeration);
        // check if draw geometry points
        const bool movingGeometryPoints = drawMovingGeometryPoints();
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (s.checkDrawAdditional(d, isAttributeCarrierSelected())) {
            // draw E2
            drawE2(s, d, E2Exaggeration, movingGeometryPoints);
            // draw lock icon
            GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), myAdditionalGeometry.getShape().getCentroid(), E2Exaggeration);
            // Draw additional ID
            drawAdditionalID(s);
            // draw additional name
            drawAdditionalName(s);
            // check if draw geometry points
            if (movingGeometryPoints) {
                myAdditionalContour.drawDottedContourGeometryPoints(s, d, this, myAdditionalGeometry.getShape(), s.neteditSizeSettings.additionalGeometryPointRadius,
                        1, s.dottedContourSettings.segmentWidthSmall);
            } else {
                myAdditionalContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
            }
        }
        // check if we're calculating the contour or the moving geometry points
        if (movingGeometryPoints) {
            if (myStartPosOverLane != INVALID_DOUBLE) {
                myAdditionalContour.calculateContourFirstGeometryPoint(s, d, this, myAdditionalGeometry.getShape(),
                        getType(), s.neteditSizeSettings.additionalGeometryPointRadius, 1);
            }
            if (movingGeometryPoints && (myEndPosPosOverLane != INVALID_DOUBLE)) {
                myAdditionalContour.calculateContourLastGeometryPoint(s, d, this, myAdditionalGeometry.getShape(),
                        getType(), s.neteditSizeSettings.additionalGeometryPointRadius, 1);
            }
        } else {
            // don't exaggerate contour
            myAdditionalContour.calculateContourExtrudedShape(s, d, this, myAdditionalGeometry.getShape(), getType(), s.detectorSettings.E2Width,
                    E2Exaggeration, true, true, 0, nullptr, getParentLanes().front()->getParentEdge());
        }
    }
}


void
GNELaneAreaDetector::computePathElement() {
    // calculate path
    myNet->getNetworkPathManager()->calculateConsecutivePathLanes(this, getParentLanes());
}


void
GNELaneAreaDetector::drawLanePartialGL(const GUIVisualizationSettings& s, const GNESegment* segment, const double offsetFront) const {
    // check if E2 can be drawn
    if (segment->getLane() && (myTagProperty->getTag() == GNE_TAG_MULTI_LANE_AREA_DETECTOR) &&
            myNet->getViewNet()->getDataViewOptions().showAdditionals() && !myNet->getViewNet()->selectingDetectorsTLSMode()) {
        const bool movingGeometryPoints = drawMovingGeometryPoints();
        // Obtain exaggeration of the draw
        const double E2Exaggeration = getExaggeration(s);
        // get detail level
        const auto d = s.getDetailLevel(E2Exaggeration);
        // calculate startPos
        const double geometryDepartPos = getAttributeDouble(SUMO_ATTR_POSITION);
        // get endPos
        const double geometryEndPos = getAttributeDouble(SUMO_ATTR_ENDPOS);
        // declare path geometry
        GUIGeometry E2Geometry;
        // update pathGeometry depending of first and last segment
        if (segment->isFirstSegment() && segment->isLastSegment()) {
            E2Geometry.updateGeometry(segment->getLane()->getLaneGeometry().getShape(),
                                      geometryDepartPos,
                                      Position::INVALID,
                                      geometryEndPos,
                                      Position::INVALID);
        } else if (segment->isFirstSegment()) {
            E2Geometry.updateGeometry(segment->getLane()->getLaneGeometry().getShape(),
                                      geometryDepartPos,
                                      Position::INVALID,
                                      -1,
                                      Position::INVALID);
        } else if (segment->isLastSegment()) {
            E2Geometry.updateGeometry(segment->getLane()->getLaneGeometry().getShape(),
                                      -1,
                                      Position::INVALID,
                                      geometryEndPos,
                                      Position::INVALID);
        } else {
            E2Geometry = segment->getLane()->getLaneGeometry();
        }
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (s.checkDrawAdditional(d, isAttributeCarrierSelected())) {
            // draw E2 partial
            drawE2PartialLane(s, d, segment, offsetFront, E2Geometry, E2Exaggeration, movingGeometryPoints);
            // draw additional ID
            drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
            // draw dotted contour
            if (movingGeometryPoints) {
                // get mouse position
                const Position mousePosition = myNet->getViewNet()->getPositionInformation();
                // get snap radius
                const double snap_radius = myNet->getViewNet()->getVisualisationSettings().neteditSizeSettings.additionalGeometryPointRadius;
                if (segment->getFromContour() && E2Geometry.getShape().front().distanceSquaredTo2D(mousePosition) <= (snap_radius * snap_radius)) {
                    segment->getFromContour()->drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidthSmall, true);
                } else if (segment->getToContour() && E2Geometry.getShape().back().distanceSquaredTo2D(mousePosition) <= (snap_radius * snap_radius)) {
                    segment->getToContour()->drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidthSmall, true);
                }
            } else {
                segment->getContour()->drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
            }
        }
        // calculate contour and draw dotted geometry
        segment->getContour()->calculateContourExtrudedShape(s, d, this, E2Geometry.getShape(), getType(), s.detectorSettings.E2Width,
                E2Exaggeration, segment->isFirstSegment(), segment->isLastSegment(), 0, segment, segment->getLane()->getParentEdge());
        // check if create from-to contours
        if (segment->getFromContour()) {
            segment->getFromContour()->calculateContourCircleShape(s, d, this, E2Geometry.getShape().front(),
                    s.neteditSizeSettings.additionalGeometryPointRadius, getType(), E2Exaggeration, segment->getLane()->getParentEdge());
        } else if (segment->getToContour()) {
            segment->getToContour()->calculateContourCircleShape(s, d, this, E2Geometry.getShape().back(),
                    s.neteditSizeSettings.additionalGeometryPointRadius, getType(), E2Exaggeration, segment->getLane()->getParentEdge());
        }
        // check if add this path element to redraw buffer
        if (!gViewObjectsHandler.isPathElementMarkForRedraw(this) && segment->getContour()->checkDrawPathContour(s, d, this)) {
            gViewObjectsHandler.addToRedrawPathElements(this);
        }
    }
}


void
GNELaneAreaDetector::drawJunctionPartialGL(const GUIVisualizationSettings& s, const GNESegment* segment, const double offsetFront) const {
    // check if E2 can be drawn
    if ((myTagProperty->getTag() == GNE_TAG_MULTI_LANE_AREA_DETECTOR) && segment->getPreviousLane() && segment->getNextLane() &&
            myNet->getViewNet()->getDataViewOptions().showAdditionals() && !myNet->getViewNet()->selectingDetectorsTLSMode()) {
        // Obtain exaggeration of the draw
        const double E2Exaggeration = getExaggeration(s);
        // get detail level
        const auto d = s.getDetailLevel(E2Exaggeration);
        // get flag for show only contour
        const bool onlyContour = myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork() ? myNet->getViewNet()->getNetworkViewOptions().showConnections() : false;
        // check if connection to next lane exist
        const bool connectionExist = segment->getPreviousLane()->getLane2laneConnections().exist(segment->getNextLane());
        // get geometry
        const GUIGeometry& E2Geometry = connectionExist ? segment->getPreviousLane()->getLane2laneConnections().getLane2laneGeometry(segment->getNextLane()) :
                                        GUIGeometry({segment->getPreviousLane()->getLaneShape().back(), segment->getNextLane()->getLaneShape().front()});
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (s.checkDrawAdditional(d, isAttributeCarrierSelected())) {
            // draw E2 partial
            drawE2PartialJunction(s, d, onlyContour, offsetFront, E2Geometry, E2Exaggeration);
            // draw dotted contour
            if (!drawMovingGeometryPoints()) {
                segment->getContour()->drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
            }
        }
        // calculate contour
        segment->getContour()->calculateContourExtrudedShape(s, d, this, E2Geometry.getShape(), getType(), s.detectorSettings.E2Width, E2Exaggeration,
                false, false, 0, segment, segment->getJunction());
        // check if add this path element to redraw buffer
        if (!gViewObjectsHandler.isPathElementMarkForRedraw(this) && segment->getContour()->checkDrawPathContour(s, d, this)) {
            gViewObjectsHandler.addToRedrawPathElements(this);
        }
    }
}


std::string
GNELaneAreaDetector::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_POSITION:
            return toString(myStartPosOverLane);
        case SUMO_ATTR_ENDPOS:
            return toString(myEndPosPosOverLane);
        case SUMO_ATTR_TLID:
            return myTrafficLight;
        case SUMO_ATTR_LENGTH:
            return toString(myEndPosPosOverLane - myStartPosOverLane);
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return time2string(myTimeThreshold);
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            return toString(mySpeedThreshold);
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
            return toString(myJamThreshold);
        case SUMO_ATTR_SHOW_DETECTOR:
            return toString(myShow);
        default:
            return getDetectorAttribute(key);
    }
}


double
GNELaneAreaDetector::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_POSITION:
            return myStartPosOverLane;
        case SUMO_ATTR_ENDPOS:
            return myEndPosPosOverLane;
        case SUMO_ATTR_LENGTH:
            return (myEndPosPosOverLane - myStartPosOverLane);
        default:
            return getDetectorAttributeDouble(key);
    }
}


Position
GNELaneAreaDetector::getAttributePosition(SumoXMLAttr key) const {
    return getDetectorAttributePosition(key);
}


PositionVector
GNELaneAreaDetector::getAttributePositionVector(SumoXMLAttr key) const {
    return getCommonAttributePositionVector(key);
}


void
GNELaneAreaDetector::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_ENDPOS:
        case SUMO_ATTR_TLID:
        case SUMO_ATTR_LENGTH:
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
        case SUMO_ATTR_SHOW_DETECTOR:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setDetectorAttribute(key, value, undoList);
            break;
    }
}


bool
GNELaneAreaDetector::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_ENDPOS:
            return canParse<double>(value);
        case SUMO_ATTR_TLID:
            // temporal
            return SUMOXMLDefinitions::isValidNetID(value);
        case SUMO_ATTR_LENGTH:
            return (canParse<double>(value) && (parse<double>(value) >= 0));
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return canParse<SUMOTime>(value) && (parse<SUMOTime>(value) >= 0);
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            return (canParse<double>(value) && (parse<double>(value) >= 0));
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
            return (canParse<double>(value) && (parse<double>(value) >= 0));
        case SUMO_ATTR_SHOW_DETECTOR:
            return canParse<bool>(value);
        default:
            return isDetectorValid(key, value);
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNELaneAreaDetector::drawE2(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                            const double exaggeration, const bool movingGeometryPoints) const {
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
    // draw parent and child lines
    drawParentChildLines(s, s.additionalSettings.connectionColor);
    // push layer matrix
    GLHelper::pushMatrix();
    // translate to front
    drawInLayer(GLO_E2DETECTOR);
    // set color
    GLHelper::setColor(E2Color);
    // draw geometry
    GUIGeometry::drawGeometry(d, myAdditionalGeometry, s.detectorSettings.E2Width * exaggeration);
    // draw arrow
    if (myAdditionalGeometry.getShape().size() > 1) {
        glTranslated(0, 0, 0.1);
        GLHelper::drawTriangleAtEnd(myAdditionalGeometry.getShape()[-2], myAdditionalGeometry.getShape()[-1], (double) 0.5, (double) 1, 0.5);
    }
    // draw E2 Logo
    drawE2DetectorLogo(s, d, exaggeration, "E2", textColor);
    // check if draw geometry points
    if (movingGeometryPoints) {
        drawLeftGeometryPoint(s, d, myAdditionalGeometry.getShape().front(), myAdditionalGeometry.getShapeRotations().front(), E2Color);
        drawRightGeometryPoint(s, d, myAdditionalGeometry.getShape().back(), myAdditionalGeometry.getShapeRotations().back(), E2Color);
    }
    // pop layer matrix
    GLHelper::popMatrix();
}


void
GNELaneAreaDetector::drawE2PartialLane(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                                       const GNESegment* segment, const double offsetFront,
                                       const GUIGeometry& geometry, const double exaggeration, const bool movingGeometryPoints) const {
    // obtain color
    const RGBColor E2Color = drawUsingSelectColor() ? s.colorSettings.selectedAdditionalColor : s.detectorSettings.E2Color;
    // push layer matrix
    GLHelper::pushMatrix();
    // Start with the drawing of the area traslating matrix to origin
    glTranslated(0, 0, getType() + offsetFront);
    // Set color
    GLHelper::setColor(E2Color);
    // draw geometry
    GUIGeometry::drawGeometry(d, geometry, s.detectorSettings.E2Width * exaggeration);
    // check if draw moving geometry points
    if (movingGeometryPoints) {
        if (segment->isFirstSegment()) {
            // calculate and draw left geometry point
            myAdditionalContour.calculateContourFirstGeometryPoint(s, d, this, geometry.getShape(),
                    getType(), s.neteditSizeSettings.additionalGeometryPointRadius, 1, true);
            drawLeftGeometryPoint(s, d, geometry.getShape().front(), geometry.getShapeRotations().front(), E2Color, true);
        }
        if (segment->isLastSegment()) {
            // calculate and draw right  geometry point
            myAdditionalContour.calculateContourLastGeometryPoint(s, d, this, geometry.getShape(),
                    getType(), s.neteditSizeSettings.additionalGeometryPointRadius, 1, true);
            drawRightGeometryPoint(s, d, geometry.getShape().back(), geometry.getShapeRotations().back(), E2Color, true);
            // draw arrow
            if (geometry.getShape().size() > 1) {
                glTranslated(0, 0, 0.1);
                GLHelper::drawTriangleAtEnd(geometry.getShape()[-2], geometry.getShape()[-1], (double) 0.5, (double) 1, 0.5);
            }
        }
    }
    // Pop layer matrix
    GLHelper::popMatrix();
    // check if this is the label segment
    if (segment->isLabelSegment()) {
        // calculate middle point
        const double middlePoint = (geometry.getShape().length2D() * 0.5);
        // calculate position
        const Position pos = geometry.getShape().positionAtOffset2D(middlePoint);
        // calculate rotation
        const double rot = s.getTextAngle((geometry.getShape().rotationDegreeAtOffset(middlePoint) * -1) + 90);
        // Start pushing matrix
        GLHelper::pushMatrix();
        // Traslate to position
        glTranslated(pos.x(), pos.y(), getType() + offsetFront + 0.1);
        // rotate
        glRotated(rot, 0, 0, 1);
        // move
        glTranslated(-1, 0, 0);
        // scale text
        glScaled(exaggeration, exaggeration, 1);
        // draw E1 logo
        GLHelper::drawText("E2 Multilane", Position(), .1, 1.5, RGBColor::BLACK);
        // pop matrix
        GLHelper::popMatrix();
    }

}


void
GNELaneAreaDetector::drawE2PartialJunction(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
        const bool onlyContour, const double offsetFront, const GUIGeometry& geometry,
        const double exaggeration) const {
    const bool invalid = geometry.getShape().length() == 2;
    const double width = s.detectorSettings.E2Width * exaggeration * (invalid ? 0.5 : 1);
    // Add a draw matrix
    GLHelper::pushMatrix();
    // Start with the drawing of the area traslating matrix to origin
    glTranslated(0, 0, getType() + offsetFront);
    // Set color of the base
    if (drawUsingSelectColor()) {
        GLHelper::setColor(s.colorSettings.selectedAdditionalColor);
    } else if (invalid) {
        GLHelper::setColor(RGBColor::RED);
    } else {
        GLHelper::setColor(s.detectorSettings.E2Color);
    }
    // check if draw only contour
    if (onlyContour) {
        GUIGeometry::drawContourGeometry(geometry, width);
    } else {
        GUIGeometry::drawGeometry(d, geometry, width);
    }
    // Pop last matrix
    GLHelper::popMatrix();
}


void
GNELaneAreaDetector::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_LANES:
            replaceAdditionalParentLanes(value);
            break;
        case SUMO_ATTR_POSITION:
            myStartPosOverLane = parse<double>(value);
            // update geometry (except for template)
            if (getParentLanes().size() > 0) {
                updateGeometry();
            }
            break;
        case SUMO_ATTR_ENDPOS:
            myEndPosPosOverLane = parse<double>(value);
            // update geometry (except for template)
            if (getParentLanes().size() > 0) {
                updateGeometry();
            }
            break;
        case SUMO_ATTR_TLID:
            myTrafficLight = value;
            break;
        case SUMO_ATTR_LENGTH:
            myEndPosPosOverLane = (myStartPosOverLane + parse<double>(value));
            // update geometry (except for template)
            if (getParentLanes().size() > 0) {
                updateGeometry();
            }
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
        case SUMO_ATTR_SHOW_DETECTOR:
            myShow = parse<bool>(value);
            break;
        default:
            setDetectorAttribute(key, value);
            break;
    }
}

/****************************************************************************/
