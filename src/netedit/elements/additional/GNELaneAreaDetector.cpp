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
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNESegment.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_Connection.h>
#include <netedit/elements/network/GNEConnection.h>
#include <netedit/frames/network/GNETLSEditorFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalViewObjectsHandler.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/xml/NamespaceIDs.h>

#include "GNELaneAreaDetector.h"
#include "GNEAdditionalHandler.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNELaneAreaDetector::GNELaneAreaDetector(SumoXMLTag tag, GNENet* net) :
    GNEDetector(net, GLO_E2DETECTOR, tag, GUIIcon::E2) {
}


GNELaneAreaDetector::GNELaneAreaDetector(const std::string& id, GNENet* net, const std::string& filename, GNELane* lane, double pos, double length, const SUMOTime freq,
        const std::string& trafficLight, const std::string& outputFilename, const std::vector<std::string>& vehicleTypes, const std::vector<std::string>& nextEdges,
        const std::string& detectPersons, const std::string& name, const SUMOTime timeThreshold, double speedThreshold, const double jamThreshold, const bool friendlyPos,
        const bool show, const Parameterised::Map& parameters) :
    GNEDetector(id, net, filename, GLO_E2DETECTOR, SUMO_TAG_LANE_AREA_DETECTOR, GUIIcon::E2, pos, freq, lane, outputFilename, vehicleTypes, nextEdges,
                detectPersons, name, friendlyPos, parameters),
    myEndPositionOverLane(pos + length),
    myTimeThreshold(timeThreshold),
    mySpeedThreshold(speedThreshold),
    myJamThreshold(jamThreshold),
    myTrafficLight(trafficLight),
    myShow(show) {
}


GNELaneAreaDetector::GNELaneAreaDetector(const std::string& id, GNENet* net, const std::string& filename, std::vector<GNELane*> lanes, double pos, double endPos, const SUMOTime freq,
        const std::string& trafficLight, const std::string& outputFilename, const std::vector<std::string>& vehicleTypes, const std::vector<std::string>& nextEdges,
        const std::string& detectPersons, const std::string& name, const SUMOTime timeThreshold, double speedThreshold, const double jamThreshold, const bool friendlyPos, const bool show,
        const Parameterised::Map& parameters) :
    GNEDetector(id, net, filename, GLO_E2DETECTOR, GNE_TAG_MULTI_LANE_AREA_DETECTOR, GUIIcon::E2, pos, freq, lanes, outputFilename, vehicleTypes, nextEdges,
                detectPersons, name, friendlyPos, parameters),
    myEndPositionOverLane(endPos),
    myTimeThreshold(timeThreshold),
    mySpeedThreshold(speedThreshold),
    myJamThreshold(jamThreshold),
    myTrafficLight(trafficLight),
    myShow(show) {
}


GNELaneAreaDetector::~GNELaneAreaDetector() {
}


void
GNELaneAreaDetector::writeAdditional(OutputDevice& device) const {
    device.openTag(SUMO_TAG_LANE_AREA_DETECTOR);
    device.writeAttr(SUMO_ATTR_ID, getID());
    // continue depending of E2 type
    if (myTagProperty->getTag() == SUMO_TAG_LANE_AREA_DETECTOR) {
        device.writeAttr(SUMO_ATTR_LANE, getParentLanes().front()->getID());
        device.writeAttr(SUMO_ATTR_POSITION, myPositionOverLane);
        device.writeAttr(SUMO_ATTR_LENGTH, toString(myEndPositionOverLane - myPositionOverLane));
    } else {
        device.writeAttr(SUMO_ATTR_LANES, getAttribute(SUMO_ATTR_LANES));
        device.writeAttr(SUMO_ATTR_POSITION, myPositionOverLane);
        device.writeAttr(SUMO_ATTR_ENDPOS, myEndPositionOverLane);
    }
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
                // if a connection between "from" lane and "to" lane of connection is found, change myE2valid to true again
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
    if (myTagProperty->getTag() == GNE_TAG_MULTI_LANE_AREA_DETECTOR) {
        // compute path
        computePathElement();
    } else {
        // Cut shape using as delimitators fixed start position and fixed end position
        myAdditionalGeometry.updateGeometry(getParentLanes().front()->getLaneShape(), getStartGeometryPositionOverLane(), getEndGeometryPositionOverLane(), myMoveElementLateralOffset);
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
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (s.checkDrawAdditional(d, isAttributeCarrierSelected())) {
            // draw E2
            drawE2(s, d, E2Exaggeration);
            // draw lock icon
            GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), myAdditionalGeometry.getShape().getCentroid(), E2Exaggeration);
            // Draw additional ID
            drawAdditionalID(s);
            // draw additional name
            drawAdditionalName(s);
            // draw dotted contour
            myAdditionalContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
        }
        // calculate contour and draw dotted geometry
        myAdditionalContour.calculateContourExtrudedShape(s, d, this, myAdditionalGeometry.getShape(), getType(), s.detectorSettings.E2Width,
                E2Exaggeration, true, true, 0, nullptr, getParentLanes().front()->getParentEdge());
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
        const bool movingGeometryPoints = drawMovingGeometryPoints(false);
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
            if (!drawMovingGeometryPoints(false)) {
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
        case SUMO_ATTR_LANES:
            return parseIDs(getParentLanes());
        case SUMO_ATTR_STARTPOS:
            return toString(myPositionOverLane);
        case SUMO_ATTR_ENDPOS:
            return toString(myEndPositionOverLane);
        case SUMO_ATTR_TLID:
            return myTrafficLight;
        case SUMO_ATTR_LENGTH:
            return toString(myEndPositionOverLane - myPositionOverLane);
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
        case SUMO_ATTR_LENGTH:
            return (myEndPositionOverLane - myPositionOverLane);
        case SUMO_ATTR_ENDPOS:
            return myEndPositionOverLane;
        default:
            return getDetectorAttributeDouble(key);
    }
}


void
GNELaneAreaDetector::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_LANES:
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
        case SUMO_ATTR_LANES:
            if (value.empty()) {
                return false;
            } else {
                return canParse<std::vector<GNELane*> >(myNet, value, true);
            }
        case SUMO_ATTR_ENDPOS:
            return canParse<double>(value);
        case SUMO_ATTR_TLID:
            // temporal
            return SUMOXMLDefinitions::isValidNetID(value);
        case SUMO_ATTR_LENGTH:
            return (canParse<double>(value) && (parse<double>(value) >= 0));
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return canParse<SUMOTime>(value);
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
                            const double exaggeration) const {
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
    // draw geometry points
    drawLeftGeometryPoint(s, d, myAdditionalGeometry.getShape().front(), myAdditionalGeometry.getShapeRotations().front(), E2Color);
    drawRightGeometryPoint(s, d, myAdditionalGeometry.getShape().back(), myAdditionalGeometry.getShapeRotations().back(), E2Color);
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
        if (segment->isFirstSegment() && segment->isLastSegment()) {
            drawLeftGeometryPoint(s, d, geometry.getShape().front(),  geometry.getShapeRotations().front(), E2Color, true);
            drawRightGeometryPoint(s, d, geometry.getShape().back(), geometry.getShapeRotations().back(), E2Color, true);
        } else if (segment->isFirstSegment()) {
            drawLeftGeometryPoint(s, d, geometry.getShape().front(), geometry.getShapeRotations().front(), E2Color, true);
        } else if (segment->isLastSegment()) {
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
        case SUMO_ATTR_LANES:
            replaceAdditionalParentLanes(value);
            break;
        case SUMO_ATTR_ENDPOS:
            myEndPositionOverLane = parse<double>(value);
            // update geometry (except for template)
            if (getParentLanes().size() > 0) {
                updateGeometry();
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


void
GNELaneAreaDetector::setMoveShape(const GNEMoveResult& moveResult) {
    if ((moveResult.operationType == GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_FIRST) ||
            (moveResult.operationType == GNEMoveOperation::OperationType::MULTIPLE_LANES_MOVE_FIRST)) {
        // change only start position
        myPositionOverLane = moveResult.newFirstPos;
    } else if ((moveResult.operationType == GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_LAST) ||
               (moveResult.operationType == GNEMoveOperation::OperationType::MULTIPLE_LANES_MOVE_LAST)) {
        // change only end position
        myEndPositionOverLane = moveResult.newFirstPos;
    } else {
        if (moveResult.operationType == GNEMoveOperation::OperationType::MULTIPLE_LANES_MOVE_BOTH_FIRST) {
            const auto difference = moveResult.newFirstPos - myPositionOverLane;
            // change start position
            myPositionOverLane = moveResult.newFirstPos;
            myEndPositionOverLane += difference;
        } else if (moveResult.operationType == GNEMoveOperation::OperationType::MULTIPLE_LANES_MOVE_BOTH_LAST) {
            const auto difference = moveResult.newFirstPos - myEndPositionOverLane;
            // change end position
            myPositionOverLane += difference;
            myEndPositionOverLane = moveResult.newFirstPos;
        }
        // end position over lane
        if (myPositionOverLane < 0) {
            myPositionOverLane = 0;
        } else if (myPositionOverLane > getParentLanes().front()->getLaneShapeLength()) {
            myPositionOverLane = getParentLanes().front()->getLaneShapeLength();
        }
        // adjust position over lane
        if (myEndPositionOverLane < 0) {
            myEndPositionOverLane = 0;
        } else if (myEndPositionOverLane > getParentLanes().back()->getLaneShapeLength()) {
            myEndPositionOverLane = getParentLanes().back()->getLaneShapeLength();
        }
    }
    // update geometry
    updateGeometry();
}


void
GNELaneAreaDetector::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // begin change attribute
    undoList->begin(this, "position of " + getTagStr());
    // set attributes depending of operation type
    if ((moveResult.operationType == GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_FIRST) ||
            (moveResult.operationType == GNEMoveOperation::OperationType::MULTIPLE_LANES_MOVE_FIRST)) {
        // set only start position
        setAttribute(SUMO_ATTR_POSITION, toString(moveResult.newFirstPos), undoList);
    } else if ((moveResult.operationType == GNEMoveOperation::OperationType::SINGLE_LANE_MOVE_LAST) ||
               (moveResult.operationType == GNEMoveOperation::OperationType::MULTIPLE_LANES_MOVE_LAST)) {
        // set only end position
        setAttribute(SUMO_ATTR_ENDPOS, toString(moveResult.newFirstPos), undoList);
    } else {
        double startPos = myPositionOverLane;
        double endPos = myEndPositionOverLane;
        // set positions
        if (moveResult.operationType == GNEMoveOperation::OperationType::MULTIPLE_LANES_MOVE_BOTH_FIRST) {
            const auto difference = moveResult.newFirstPos - myPositionOverLane;
            // change start position
            startPos = moveResult.newFirstPos;
            endPos += difference;
        } else if (moveResult.operationType == GNEMoveOperation::OperationType::MULTIPLE_LANES_MOVE_BOTH_LAST) {
            const auto difference = moveResult.newFirstPos - myEndPositionOverLane;
            // change end position
            startPos += difference;
            endPos = moveResult.newFirstPos;
        }
        // end position over lane
        if (startPos < 0) {
            startPos = 0;
        } else if (startPos > getParentLanes().front()->getLaneShapeLength()) {
            startPos = getParentLanes().front()->getLaneShapeLength();
        }
        // adjust position over lane
        if (endPos < 0) {
            endPos = 0;
        } else if (endPos > getParentLanes().back()->getLaneShapeLength()) {
            endPos = getParentLanes().back()->getLaneShapeLength();
        }
        // set only end position
        setAttribute(SUMO_ATTR_POSITION, toString(startPos), undoList);
        setAttribute(SUMO_ATTR_ENDPOS, toString(endPos), undoList);
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
