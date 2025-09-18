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
/// @file    GNEPOI.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2017
///
// A class for visualizing and editing POIS in netedit (adapted from
// GUIPointOfInterest and NLHandler)
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/globjects/GUIPointOfInterest.h>
#include <utils/gui/div/GUIGlobalViewObjectsHandler.h>
#include <utils/options/OptionsCont.h>
#include <utils/xml/NamespaceIDs.h>

#include "GNEPOI.h"
#include "GNEAdditionalHandler.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEPOI::GNEPOI(SumoXMLTag tag, GNENet* net) :
    PointOfInterest("", "", RGBColor::BLACK, Position(0, 0), false, "", 0, false, 0, SUMOXMLDefinitions::POIIcons.getString(POIIcon::NONE),
                    0, 0, "", 0, 0, "", Parameterised::Map()),
    GNEAdditional("", net, "", tag, "") {
}


GNEPOI::GNEPOI(const std::string& id, GNENet* net, const std::string& filename, const std::string& type, const RGBColor& color, const double xLon, const double yLat,
               const bool geo, const std::string& icon, const double layer, const double angle, const std::string& imgFile, const double width, const double height,
               const std::string& name, const Parameterised::Map& parameters) :
    PointOfInterest(id, type, color, Position(xLon, yLat), geo, "", 0, false, 0, icon, layer, angle, imgFile, width, height, name, parameters),
    GNEAdditional(id, net, filename, geo ? GNE_TAG_POIGEO : SUMO_TAG_POI, "") {
    // update position depending of GEO
    if (geo) {
        Position cartesian(x(), y());
        GeoConvHelper::getFinal().x2cartesian_const(cartesian);
        set(cartesian.x(), cartesian.y());
    }
    // update geometry (needed for adjust myShapeWidth and myShapeHeight)
    updateGeometry();
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEPOI::GNEPOI(const std::string& id, GNENet* net, const std::string& filename, const std::string& type, const RGBColor& color, GNELane* lane, const double posOverLane,
               const bool friendlyPos, const double posLat, const std::string& icon, const double layer, const double angle, const std::string& imgFile, const double width,
               const double height, const std::string& name, const Parameterised::Map& parameters) :
    PointOfInterest(id, type, color, Position(), false, lane->getID(), posOverLane, friendlyPos, posLat, icon, layer, angle, imgFile, width, height, name, parameters),
    GNEAdditional(id, net, filename, GNE_TAG_POILANE, "") {
    // set parents
    setParent<GNELane*>(lane);
    // update geometry (needed for adjust myShapeWidth and myShapeHeight)
    updateGeometry();
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEPOI::~GNEPOI() {}


GNEMoveOperation*
GNEPOI::getMoveOperation() {
    if (drawMovingGeometryPoints()) {
        // get snap radius
        const double snapRadius = myNet->getViewNet()->getVisualisationSettings().neteditSizeSettings.additionalGeometryPointRadius;
        const double snapRadiusSquared = snapRadius * snapRadius;
        // get mouse position
        const Position mousePosition = myNet->getViewNet()->getPositionInformation();
        // check if we're editing width or height
        if ((myShapeWidth.size() == 0) || (myShapeHeight.size() == 0)) {
            return nullptr;
        } else if (myShapeHeight.front().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared) {
            // edit height
            return new GNEMoveOperation(this, myShapeHeight, true, GNEMoveOperation::OperationType::HEIGHT);
        } else if (myShapeHeight.back().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared) {
            // edit height
            return new GNEMoveOperation(this, myShapeHeight, false, GNEMoveOperation::OperationType::HEIGHT);
        } else if (myShapeWidth.front().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared) {
            // edit width
            return new GNEMoveOperation(this, myShapeWidth, true, GNEMoveOperation::OperationType::WIDTH);
        } else if (myShapeWidth.back().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared) {
            // edit width
            return new GNEMoveOperation(this, myShapeWidth, false, GNEMoveOperation::OperationType::WIDTH);
        } else {
            return nullptr;
        }
    } else if (getTagProperty()->getTag() == GNE_TAG_POILANE) {
        // return move operation for POI placed over lane
        return new GNEMoveOperation(this, getParentLanes().front(), myPosOverLane,
                                    myNet->getViewNet()->getViewParent()->getMoveFrame()->getCommonMoveOptions()->getAllowChangeLane());
    } else {
        // return move operation for a position in view
        return new GNEMoveOperation(this, *this);
    }
}


void
GNEPOI::removeGeometryPoint(const Position /*clickedPosition*/, GNEUndoList* /*undoList*/) {
    // nothing to remove
}


std::string
GNEPOI::generateChildID(SumoXMLTag /*childTag*/) {
    return "";
}


CommonXMLStructure::SumoBaseObject*
GNEPOI::getSumoBaseObject() const {
    CommonXMLStructure::SumoBaseObject* POIBaseObject = new CommonXMLStructure::SumoBaseObject(nullptr);
    POIBaseObject->setTag(SUMO_TAG_POI);
    // fill attributes
    POIBaseObject->addStringAttribute(SUMO_ATTR_ID, myID);
    POIBaseObject->addColorAttribute(SUMO_ATTR_COLOR, getShapeColor());
    POIBaseObject->addStringAttribute(SUMO_ATTR_TYPE, getShapeType());
    POIBaseObject->addStringAttribute(SUMO_ATTR_ICON, getIconStr());
    POIBaseObject->addDoubleAttribute(SUMO_ATTR_LAYER, getShapeLayer());
    POIBaseObject->addStringAttribute(SUMO_ATTR_IMGFILE, getShapeImgFile());
    POIBaseObject->addDoubleAttribute(SUMO_ATTR_WIDTH, getWidth());
    POIBaseObject->addDoubleAttribute(SUMO_ATTR_HEIGHT, getHeight());
    POIBaseObject->addDoubleAttribute(SUMO_ATTR_ANGLE, getShapeNaviDegree());
    POIBaseObject->addStringAttribute(SUMO_ATTR_NAME, getShapeName());
    return POIBaseObject;
}


void
GNEPOI::writeAdditional(OutputDevice& device) const {
    if (getParentLanes().size() > 0) {
        // obtain fixed position over lane
        double fixedPositionOverLane = myPosOverLane > getParentLanes().at(0)->getLaneShape().length() ? getParentLanes().at(0)->getLaneShape().length() : myPosOverLane < 0 ? 0 : myPosOverLane;
        // write POILane using POI::writeXML
        writeXML(device, false, 0, getParentLanes().at(0)->getID(), fixedPositionOverLane, myFriendlyPos, myPosLat);
    } else {
        writeXML(device, myGeo);
    }
}


bool
GNEPOI::isAdditionalValid() const {
    // only for POIS over lanes
    if (getParentLanes().size() == 0) {
        return true;
    } else if (getFriendlyPos()) {
        // with friendly position enabled position is "always fixed"
        return true;
    } else {
        return fabs(myPosOverLane) <= getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    }
}


std::string
GNEPOI::getAdditionalProblem() const {
    // only for POIS over lanes
    if (getParentLanes().size() > 0) {
        // obtain final length
        const double len = getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
        // check if detector has a problem
        if (GNEAdditionalHandler::checkLanePosition(myPosOverLane, 0, len, getFriendlyPos())) {
            return "";
        } else {
            // declare variable for error position
            std::string errorPosition;
            // check positions over lane
            if (myPosOverLane < 0) {
                errorPosition = (toString(SUMO_ATTR_POSITION) + " < 0");
            }
            if (myPosOverLane > len) {
                errorPosition = (toString(SUMO_ATTR_POSITION) + TL(" > lanes's length"));
            }
            return errorPosition;
        }
    } else {
        return "";
    }
}


void
GNEPOI::fixAdditionalProblem() {
    // only for POIS over lanes
    if (getParentLanes().size() > 0) {
        // declare new position
        double newPositionOverLane = myPosOverLane;
        // declare new length (but unsed in this context)
        double length = 0;
        // fix pos and length with fixLanePosition
        GNEAdditionalHandler::fixLanePosition(newPositionOverLane, length, getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength());
        // set new position
        setAttribute(SUMO_ATTR_POSITION, toString(newPositionOverLane), myNet->getViewNet()->getUndoList());
    }
}


void
GNEPOI::updateGeometry() {
    // set position
    if (getParentLanes().size() > 0) {
        // obtain fixed position over lane
        double fixedPositionOverLane = myPosOverLane > getParentLanes().at(0)->getLaneShapeLength() ? getParentLanes().at(0)->getLaneShapeLength() : myPosOverLane < 0 ? 0 : myPosOverLane;
        // set new position regarding to lane
        set(getParentLanes().at(0)->getLaneShape().positionAtOffset(fixedPositionOverLane * getParentLanes().at(0)->getLengthGeometryFactor(), -myPosLat));
    }
    // check if update width and height shapes
    if ((getWidth() > 0) && (getHeight() > 0)) {
        // calculate shape length
        myShapeHeight.clear();
        myShapeHeight.push_back(Position(0, getHeight() * -0.5));
        myShapeHeight.push_back(Position(0, getHeight() * 0.5));
        // move
        myShapeHeight.add(*this);
        // calculate shape width
        PositionVector leftShape = myShapeHeight;
        leftShape.move2side(getWidth() * -0.5);
        PositionVector rightShape = myShapeHeight;
        rightShape.move2side(getWidth() * 0.5);
        myShapeWidth = {leftShape.getCentroid(), rightShape.getCentroid()};
    }
}


Position
GNEPOI::getPositionInView() const {
    return *this;
}


double
GNEPOI::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.poiSize.getExaggeration(s, this);
}


void
GNEPOI::updateCenteringBoundary(const bool updateGrid) {
    // Remove object from net
    if (updateGrid) {
        myNet->removeGLObjectFromGrid(this);
    }
    // reset boundary
    myAdditionalBoundary.reset();
    // add position (this POI)
    myAdditionalBoundary.add(*this);
    // grow boundary
    myAdditionalBoundary.grow(5 + std::max(getWidth() * 0.5, getHeight() * 0.5));
    // add object into net
    if (updateGrid) {
        myNet->addGLObjectIntoGrid(this);
    }
}


void
GNEPOI::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // nothing to split
}


GUIGlID
GNEPOI::getGlID() const {
    return GUIGlObject::getGlID();
}


bool
GNEPOI::checkDrawMoveContour() const {
    // get edit modes
    const auto& editModes = myNet->getViewNet()->getEditModes();
    // check if we're in move mode
    if (!myNet->getViewNet()->isCurrentlyMovingElements() && editModes.isCurrentSupermodeNetwork() &&
            !myNet->getViewNet()->getEditNetworkElementShapes().getEditedNetworkElement() &&
            (editModes.networkEditMode == NetworkEditMode::NETWORK_MOVE) && myNet->getViewNet()->checkOverLockedElement(this, mySelected)) {
        // only move the first element
        return myNet->getViewNet()->getViewObjectsSelector().getGUIGlObjectFront() == this;
    } else {
        return false;
    }
}


std::string
GNEPOI::getParentName() const {
    if (getParentLanes().size() > 0) {
        return getParentLanes().front()->getID();
    } else {
        return myNet->getMicrosimID();
    }
}


GUIGLObjectPopupMenu*
GNEPOI::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    // create popup
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, this);
    // build common options
    buildPopUpMenuCommonOptions(ret, app, myNet->getViewNet(), myTagProperty->getTag(), mySelected);
    // specific of  non juPedSim polygons
    if (!myTagProperty->isJuPedSimElement()) {
        // continue depending of lane number
        if (getParentLanes().size() > 0) {
            // add option for convert to GNEPOI
            GUIDesigns::buildFXMenuCommand(ret, TL("Release from lane"), GUIIconSubSys::getIcon(GUIIcon::LANE), &parent, MID_GNE_POI_TRANSFORM);
            return ret;
        } else {
            // add option for convert to GNEPOI
            GUIDesigns::buildFXMenuCommand(ret, TL("Attach to nearest lane"), GUIIconSubSys::getIcon(GUIIcon::LANE), &parent, MID_GNE_POI_TRANSFORM);
        }
    }
    return ret;
}


void
GNEPOI::drawGL(const GUIVisualizationSettings& s) const {
    // first check if POI can be drawn
    if (myNet->getViewNet()->getDemandViewOptions().showShapes() &&
            myNet->getViewNet()->getDataViewOptions().showShapes()) {
        // draw boundaries
        GLHelper::drawBoundary(s, getCenteringBoundary());
        // obtain POIExaggeration
        const double POIExaggeration = getExaggeration(s);
        // get detail level
        const auto d = s.getDetailLevel(POIExaggeration);
        // check if draw moving geometry points (only if we have a defined image
        const bool movingGeometryPoints = getShapeImgFile().empty() ? false : drawMovingGeometryPoints();
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (s.checkDrawPOI(getWidth(), getHeight(), d, isAttributeCarrierSelected())) {
            // draw POI
            drawPOI(s, d, movingGeometryPoints);
            // draw lock icon
            GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), getPositionInView(), POIExaggeration);
            // draw dotted contours
            if (movingGeometryPoints) {
                // get snap radius
                const double snapRadius = myNet->getViewNet()->getVisualisationSettings().neteditSizeSettings.additionalGeometryPointRadius;
                const double snapRadiusSquared = snapRadius * snapRadius;
                // get mouse position
                const Position mousePosition = myNet->getViewNet()->getPositionInformation();
                // check if we're editing width or height
                if ((myShapeHeight.front().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared) ||
                        (myShapeHeight.back().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared)) {
                    myMovingContourUp.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidthSmall, true);
                    myMovingContourDown.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidthSmall, true);
                } else if ((myShapeWidth.front().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared) ||
                           (myShapeWidth.back().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared)) {
                    myMovingContourLeft.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidthSmall, true);
                    myMovingContourRight.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidthSmall, true);
                }
            } else {
                myAdditionalContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
            }
        }
        // calculate contour
        calculatePOIContour(s, d, POIExaggeration, movingGeometryPoints);
    }
}


std::string
GNEPOI::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return myID;
        case SUMO_ATTR_COLOR:
            return toString(getShapeColor());
        case SUMO_ATTR_LANE:
            return myLane;
        case SUMO_ATTR_POSITION:
            if (getParentLanes().size() > 0) {
                return toString(myPosOverLane);
            } else {
                return toString(*this);
            }
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(getFriendlyPos());
        case SUMO_ATTR_POSITION_LAT:
            return toString(myPosLat);
        case SUMO_ATTR_LON:
            if (GeoConvHelper::getFinal().getProjString() != "!") {
                // calculate geo position
                Position GEOPosition(x(), y());
                GeoConvHelper::getFinal().cartesian2geo(GEOPosition);
                // return lon
                return toString(GEOPosition.x(), 8);
            } else {
                return TL("No geo-conversion defined");
            }
        case SUMO_ATTR_LAT:
            if (GeoConvHelper::getFinal().getProjString() != "!") {
                // calculate geo position
                Position GEOPosition(x(), y());
                GeoConvHelper::getFinal().cartesian2geo(GEOPosition);
                // return lat
                return toString(GEOPosition.y(), 8);
            } else {
                return TL("No geo-conversion defined");
            }
        case SUMO_ATTR_TYPE:
            return getShapeType();
        case SUMO_ATTR_ICON:
            return getIconStr();
        case SUMO_ATTR_LAYER:
            return toString(getShapeLayer());
        case SUMO_ATTR_IMGFILE:
            return getShapeImgFile();
        case SUMO_ATTR_WIDTH:
            return toString(getWidth());
        case SUMO_ATTR_HEIGHT:
            return toString(getHeight());
        case SUMO_ATTR_ANGLE:
            return toString(getShapeNaviDegree());
        case SUMO_ATTR_NAME:
            return getShapeName();
        case GNE_ATTR_SHIFTLANEINDEX:
            return "";
        default:
            return getCommonAttribute(this, key);
    }
}


double
GNEPOI::getAttributeDouble(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " attribute '" + toString(key) + "' not allowed");
}


const Parameterised::Map&
GNEPOI::getACParametersMap() const {
    return PointOfInterest::getParametersMap();
}


void
GNEPOI::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_COLOR:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_FRIENDLY_POS:
        case SUMO_ATTR_POSITION_LAT:
        case SUMO_ATTR_LON:
        case SUMO_ATTR_LAT:
        case SUMO_ATTR_TYPE:
        case SUMO_ATTR_ICON:
        case SUMO_ATTR_LAYER:
        case SUMO_ATTR_IMGFILE:
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_HEIGHT:
        case SUMO_ATTR_ANGLE:
        case SUMO_ATTR_NAME:
        case GNE_ATTR_SHIFTLANEINDEX:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setCommonAttribute(key, value, undoList);
            break;
    }
}


bool
GNEPOI::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidAdditionalID(NamespaceIDs::POIs, value);
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case SUMO_ATTR_LANE:
            return (myNet->getAttributeCarriers()->retrieveLane(value, false) != nullptr);
        case SUMO_ATTR_POSITION:
            if (getParentLanes().size() > 0) {
                return canParse<double>(value);
            } else {
                return canParse<Position>(value);
            }
        case SUMO_ATTR_FRIENDLY_POS:
            return canParse<bool>(value);
        case SUMO_ATTR_POSITION_LAT:
            return canParse<double>(value);
        case SUMO_ATTR_LON:
            return canParse<double>(value);
        case SUMO_ATTR_LAT:
            return canParse<double>(value);
        case SUMO_ATTR_TYPE:
            return true;
        case SUMO_ATTR_ICON:
            return SUMOXMLDefinitions::POIIcons.hasString(value);
        case SUMO_ATTR_LAYER:
            if (value.empty()) {
                return true;
            } else {
                return canParse<double>(value);
            }
        case SUMO_ATTR_IMGFILE:
            if (value == "") {
                return true;
            } else {
                // check that image can be loaded
                return GUITexturesHelper::getTextureID(value) != -1;
            }
        case SUMO_ATTR_WIDTH:
            return canParse<double>(value) && (parse<double>(value) > 0);
        case SUMO_ATTR_HEIGHT:
            return canParse<double>(value) && (parse<double>(value) > 0);
        case SUMO_ATTR_ANGLE:
            return canParse<double>(value);
        case SUMO_ATTR_NAME:
            return SUMOXMLDefinitions::isValidAttribute(value);
        default:
            return isCommonValid(key, value);
    }
}


bool
GNEPOI::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_POSITION:
            if (myTagProperty->getTag() == GNE_TAG_POIGEO) {
                return (GeoConvHelper::getFinal().getProjString() != "!");
            } else {
                return true;
            }
        case SUMO_ATTR_LON:
        case SUMO_ATTR_LAT:
            return (GeoConvHelper::getFinal().getProjString() != "!");
        default:
            return true;
    }
}


std::string
GNEPOI::getPopUpID() const {
    return getTagStr() + ": " + getID();
}


std::string
GNEPOI::getHierarchyName() const {
    return getTagStr();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEPOI::drawPOI(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                const bool movingGeometryPoints) const {
    if (GUIPointOfInterest::checkDraw(s, this)) {
        // draw inner polygon
        if (myDrawInFront) {
            GUIPointOfInterest::drawInnerPOI(s, this, this, drawUsingSelectColor(), GLO_FRONTELEMENT,
                                             myShapeWidth.length2D(), myShapeHeight.length2D());
        } else {
            GUIPointOfInterest::drawInnerPOI(s, this, this, drawUsingSelectColor(), s.poiUseCustomLayer ? s.poiCustomLayer : getShapeLayer(),
                                             myShapeWidth.length2D(), myShapeHeight.length2D());
        }
        // draw geometry points
        if (movingGeometryPoints) {
            if (myShapeHeight.size() > 0) {
                drawUpGeometryPoint(s, d, myShapeHeight.front(), 180, RGBColor::ORANGE);
                drawDownGeometryPoint(s, d, myShapeHeight.back(), 180, RGBColor::ORANGE);
            }
            if (myShapeWidth.size() > 0) {
                drawLeftGeometryPoint(s, d, myShapeWidth.back(), -90, RGBColor::ORANGE);
                drawRightGeometryPoint(s, d, myShapeWidth.front(), -90, RGBColor::ORANGE);
            }
        }
    }
}


void
GNEPOI::calculatePOIContour(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                            const double exaggeration, const bool movingGeometryPoints) const {
    // check if we're calculating the contour or the moving geometry points
    if (movingGeometryPoints) {
        myMovingContourUp.calculateContourCircleShape(s, d, this, myShapeHeight.front(), s.neteditSizeSettings.additionalGeometryPointRadius,
                getShapeLayer(), exaggeration, nullptr);
        myMovingContourDown.calculateContourCircleShape(s, d, this, myShapeHeight.back(), s.neteditSizeSettings.additionalGeometryPointRadius,
                getShapeLayer(), exaggeration, nullptr);
        myMovingContourLeft.calculateContourCircleShape(s, d, this, myShapeWidth.front(), s.neteditSizeSettings.additionalGeometryPointRadius,
                getShapeLayer(), exaggeration, nullptr);
        myMovingContourRight.calculateContourCircleShape(s, d, this, myShapeWidth.back(), s.neteditSizeSettings.additionalGeometryPointRadius,
                getShapeLayer(), exaggeration, nullptr);
    } else {
        const auto parentEdgeBoundary = (getParentLanes().size() > 0) ? getParentLanes().front()->getParentEdge() : nullptr;
        if (getShapeImgFile().empty()) {
            const double radius = getWidth() > getHeight() ? getWidth() : getHeight();
            myAdditionalContour.calculateContourCircleShape(s, d, this, *this, radius * 0.5, getShapeLayer(), exaggeration, parentEdgeBoundary);
        } else {
            myAdditionalContour.calculateContourRectangleShape(s, d, this, *this, getHeight() * 0.5, getWidth() * 0.5, getShapeLayer(), 0, 0, getShapeNaviDegree(), exaggeration, parentEdgeBoundary);
        }
    }
}


void
GNEPOI::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID: {
            // update microsimID
            setAdditionalID(value);
            // set named ID
            myID = value;
            break;
        }
        case SUMO_ATTR_COLOR:
            setShapeColor(parse<RGBColor>(value));
            break;
        case SUMO_ATTR_LANE:
            myLane = value;
            replaceAdditionalParentLanes(value);
            break;
        case SUMO_ATTR_POSITION: {
            if (myTagProperty->getTag() == GNE_TAG_POILANE) {
                myPosOverLane = parse<double>(value);
            } else {
                // set position
                set(parse<Position>(value));
            }
            // update centering boundary
            updateCenteringBoundary(true);
            // update geometry
            updateGeometry();
            break;
        }
        case SUMO_ATTR_FRIENDLY_POS:
            setFriendlyPos(parse<bool>(value));
            break;
        case SUMO_ATTR_POSITION_LAT:
            myPosLat = parse<double>(value);
            // update centering boundary
            updateCenteringBoundary(true);
            // update geometry
            updateGeometry();
            break;
        case SUMO_ATTR_LON: {
            // calculate cartesian
            Position cartesian(parse<double>(value), parse<double>(getAttribute(SUMO_ATTR_LAT)));
            GeoConvHelper::getFinal().x2cartesian_const(cartesian);
            // set cartesian
            set(cartesian);
            // update centering boundary
            updateCenteringBoundary(true);
            // update geometry
            updateGeometry();
            break;
        }
        case SUMO_ATTR_LAT: {
            // calculate cartesian
            Position cartesian(parse<double>(getAttribute(SUMO_ATTR_LON)), parse<double>(value));
            GeoConvHelper::getFinal().x2cartesian_const(cartesian);
            // set cartesian
            set(cartesian);
            // update centering boundary
            updateCenteringBoundary(true);
            // update geometry
            updateGeometry();
            break;
        }
        case SUMO_ATTR_TYPE:
            setShapeType(value);
            break;
        case SUMO_ATTR_ICON:
            setIcon(value);
            break;
        case SUMO_ATTR_LAYER:
            if (value.empty()) {
                setShapeLayer(myTagProperty->getDefaultDoubleValue(key));
            } else {
                setShapeLayer(parse<double>(value));
            }
            break;
        case SUMO_ATTR_IMGFILE:
            // first remove object from grid due img file affect to boundary
            if (getID().size() > 0) {
                myNet->removeGLObjectFromGrid(this);
            }
            setShapeImgFile(value);
            // all textures must be refresh
            GUITexturesHelper::clearTextures();
            // add object into grid again
            if (getID().size() > 0) {
                myNet->addGLObjectIntoGrid(this);
            }
            break;
        case SUMO_ATTR_WIDTH:
            // set new width
            setWidth(parse<double>(value));
            // update centering boundary and geometry (except for templates)
            if (getID().size() > 0) {
                updateCenteringBoundary(true);
                updateGeometry();
            }
            break;
        case SUMO_ATTR_HEIGHT:
            // set new height
            setHeight(parse<double>(value));
            // update centering boundary and geometry (except for templates)
            if (getID().size() > 0) {
                updateCenteringBoundary(true);
                updateGeometry();
            }
            break;
        case SUMO_ATTR_ANGLE:
            setShapeNaviDegree(parse<double>(value));
            break;
        case SUMO_ATTR_NAME:
            setShapeName(value);
            break;
        case GNE_ATTR_SHIFTLANEINDEX:
            shiftLaneIndex();
            break;
        default:
            return setCommonAttribute(this, key, value);
    }
}


void
GNEPOI::setMoveShape(const GNEMoveResult& moveResult) {
    // check what are being updated
    if (moveResult.operationType == GNEMoveOperation::OperationType::HEIGHT) {
        myShapeHeight = moveResult.shapeToUpdate;
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::WIDTH) {
        myShapeWidth = moveResult.shapeToUpdate;
    } else {
        if (getTagProperty()->getTag() == GNE_TAG_POILANE) {
            myPosOverLane = moveResult.newFirstPos;
        } else {
            set(moveResult.shapeToUpdate.front());
        }
        // update geometry
        updateGeometry();
    }
}


void
GNEPOI::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // check what are being updated
    if (moveResult.operationType == GNEMoveOperation::OperationType::HEIGHT) {
        undoList->begin(this, "height of " + getTagStr());
        setAttribute(SUMO_ATTR_HEIGHT, toString(moveResult.shapeToUpdate.length2D()), undoList);
        undoList->end();
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::WIDTH) {
        undoList->begin(this, "width of " + getTagStr());
        setAttribute(SUMO_ATTR_WIDTH, toString(moveResult.shapeToUpdate.length2D()), undoList);
        undoList->end();
    } else {
        undoList->begin(this, "position of " + getTagStr());
        if (getTagProperty()->getTag() == GNE_TAG_POILANE) {
            GNEChange_Attribute::changeAttribute(this, SUMO_ATTR_POSITION, toString(moveResult.newFirstPos), undoList);
        } else {
            GNEChange_Attribute::changeAttribute(this, SUMO_ATTR_POSITION, toString(moveResult.shapeToUpdate.front()), undoList);
        }
        undoList->end();
    }
}

/****************************************************************************/
