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

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIGlobalViewObjectsHandler.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/globjects/GUIPointOfInterest.h>
#include <utils/options/OptionsCont.h>
#include <utils/xml/NamespaceIDs.h>

#include "GNEPOI.h"
#include "GNEAdditionalHandler.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEPOI::GNEPOI(SumoXMLTag tag, GNENet* net) :
    GNEAdditional("", net, "", tag, ""),
    Shape(""),
    GNEMoveElementLaneSingle(this),
    GNEMoveElementView(this) {
}


GNEPOI::GNEPOI(const std::string& id, GNENet* net, const std::string& filename, const std::string& type, const RGBColor& color, const Position& pos,
               const bool geo, POIIcon icon, const double layer, const double angle, const std::string& imgFile, const double width,
               const double height, const std::string& name, const Parameterised::Map& parameters) :
    GNEAdditional(id, net, filename, geo ? GNE_TAG_POIGEO : SUMO_TAG_POI, name),
    Shape(id, type, color, layer, angle, imgFile, ""),
    GNEMoveElementLaneSingle(this),
    GNEMoveElementView(this, pos, width, height, 0),
    Parameterised(parameters),
    myPOIIcon(icon) {
    // update position depending of GEO
    if (geo) {
        Position cartesian = myPosOverView;
        GeoConvHelper::getFinal().x2cartesian_const(cartesian);
        myPosOverView = cartesian;
    }
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEPOI::GNEPOI(const std::string& id, GNENet* net, const std::string& filename, const std::string& type, const RGBColor& color, GNELane* lane, const double posOverLane,
               const bool friendlyPos, const double posLat, POIIcon icon, const double layer, const double angle, const std::string& imgFile, const double width,
               const double height, const std::string& name, const Parameterised::Map& parameters) :
    GNEAdditional(id, net, filename, GNE_TAG_POILANE, ""),
    Shape(id, type, color, layer, angle, imgFile, name),
    GNEMoveElementLaneSingle(this, lane, posOverLane, friendlyPos),
    GNEMoveElementView(this, Position(0, 0), width, height, 0),
    Parameterised(parameters),
    myPosLat(posLat),
    myPOIIcon(icon) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEPOI::~GNEPOI() {}


GNEMoveElement*
GNEPOI::getMoveElement() {
    if (getTagProperty()->getTag() == GNE_TAG_POILANE) {
        return static_cast<GNEMoveElementLaneSingle*>(this);
    } else {
        return static_cast<GNEMoveElementView*>(this);
    }
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
    POIBaseObject->addStringAttribute(SUMO_ATTR_ICON, SUMOXMLDefinitions::POIIcons.getString(myPOIIcon));
    POIBaseObject->addDoubleAttribute(SUMO_ATTR_LAYER, getShapeLayer());
    POIBaseObject->addStringAttribute(SUMO_ATTR_IMGFILE, getShapeImgFile());
    POIBaseObject->addDoubleAttribute(SUMO_ATTR_WIDTH, myWidth);
    POIBaseObject->addDoubleAttribute(SUMO_ATTR_HEIGHT, myHeight);
    POIBaseObject->addDoubleAttribute(SUMO_ATTR_ANGLE, getShapeNaviDegree());
    POIBaseObject->addStringAttribute(SUMO_ATTR_NAME, myAdditionalName);
    return POIBaseObject;
}


void
GNEPOI::writeAdditional(OutputDevice& device) const {
    device.openTag(SUMO_TAG_POI);
    // ID
    device.writeAttr(SUMO_ATTR_ID, StringUtils::escapeXML(getID()));
    // name (note: Use additional name instead shape name)
    if (myAdditionalName.size() > 0) {
        device.writeAttr(SUMO_ATTR_NAME, myAdditionalName);
    }
    // specific of poi lanes
    if (getTagProperty()->getTag() == GNE_TAG_POILANE) {
        // write move attributes
        GNEMoveElementLaneSingle::writeMoveAttributes(device);
        // write specific attributes
        if (myPosLat != 0) {
            device.writeAttr(SUMO_ATTR_POSITION_LAT, myPosLat);
        }
    } else {
        // specific of POI geos
        if (getTagProperty()->getTag() == GNE_TAG_POIGEO) {
            device.setPrecision(gPrecisionGeo);
            device.writeAttr(SUMO_ATTR_LON, getAttributeDouble(SUMO_ATTR_LON));
            device.writeAttr(SUMO_ATTR_LAT, getAttributeDouble(SUMO_ATTR_LAT));
            device.setPrecision();
        } else {
            device.writeAttr(SUMO_ATTR_X, myPosOverView.x());
            device.writeAttr(SUMO_ATTR_Y, myPosOverView.y());
        }
        // z
        if (myPosOverView.z() != 0) {
            device.writeAttr(SUMO_ATTR_Z, myPosOverView.z());
        }
    }
    // write shape attributes
    writeShapeAttributes(device, RGBColor::RED, Shape::DEFAULT_LAYER_POI);
    // width
    if (myWidth != Shape::DEFAULT_IMG_WIDTH) {
        device.writeAttr(SUMO_ATTR_WIDTH, myWidth);
    }
    // height
    if (myHeight != Shape::DEFAULT_IMG_HEIGHT) {
        device.writeAttr(SUMO_ATTR_HEIGHT, myHeight);
    }
    // Icon
    if (myPOIIcon != POIIcon::NONE) {
        device.writeAttr(SUMO_ATTR_ICON, SUMOXMLDefinitions::POIIcons.getString(myPOIIcon));
    }
    // params
    writeParams(device);
    device.closeTag();
}


bool
GNEPOI::isAdditionalValid() const {
    // only for POIS over lanes
    if (getParentLanes().size() == 0) {
        return true;
    } else if (myFriendlyPos) {
        // with friendly position enabled position is "always fixed"
        return true;
    } else {
        return fabs(myPosOverLane) <= getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
    }
}


std::string
GNEPOI::getAdditionalProblem() const {
    // only for POIS over lanes
    if (getTagProperty()->getTag() == GNE_TAG_POILANE) {
        // obtain final length
        const double len = getParentLanes().front()->getParentEdge()->getNBEdge()->getFinalLength();
        // check if detector has a problem
        if (GNEAdditionalHandler::checkLanePosition(myPosOverLane, 0, len, myFriendlyPos)) {
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
    if (getTagProperty()->getTag() == GNE_TAG_POILANE) {
        // set fixed position
        setAttribute(SUMO_ATTR_POSITION, toString(getFixedPositionOverLane()), myNet->getViewNet()->getUndoList());
    }
}


void
GNEPOI::updateGeometry() {
    // check if update width and height shapes
    if ((myWidth > 0) && (myHeight > 0)) {
        // calculate shape length
        myShapeHeight.clear();
        myShapeHeight.push_back(Position(0, myHeight * -0.5));
        myShapeHeight.push_back(Position(0, myHeight * 0.5));
        // move
        myShapeHeight.add(myPosOverView);
        // calculate shape width
        PositionVector leftShape = myShapeHeight;
        leftShape.move2side(myWidth * -0.5);
        PositionVector rightShape = myShapeHeight;
        rightShape.move2side(myWidth * 0.5);
        myShapeWidth = {leftShape.getCentroid(), rightShape.getCentroid()};
    }
    // set additional geometry
    if (getParentLanes().size() > 0) {
        myAdditionalGeometry.updateGeometry(getParentLanes().front()->getLaneShape(), getFixedPositionOverLane(), myPosLat);
    } else {
        myAdditionalGeometry.updateSinglePosGeometry(myPosOverView, 0);
    }
}


Position
GNEPOI::getPositionInView() const {
    return myAdditionalGeometry.getShape().getPolygonCenter();
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
    // update geometry
    updateGeometry();
    // reset boundary
    myAdditionalBoundary.reset();
    // add center
    myAdditionalBoundary.add(myPosOverView);
    // add width
    for (const auto& pos : myShapeWidth) {
        myAdditionalBoundary.add(pos);
    }
    // add height
    for (const auto& pos : myShapeHeight) {
        myAdditionalBoundary.add(pos);
    }
    // grow boundary
    myAdditionalBoundary.grow(5);
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
    if (getTagProperty()->getTag() == GNE_TAG_POILANE) {
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
        if (getTagProperty()->getTag() == GNE_TAG_POILANE) {
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
        if (s.checkDrawPOI(myWidth, myHeight, d, isAttributeCarrierSelected())) {
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
            return getParentLanes().front()->getID();
        case SUMO_ATTR_POSITION:
            if (getTagProperty()->getTag() == GNE_TAG_POILANE) {
                return toString(myPosOverLane);
            } else {
                return toString(myPosOverView);
            }
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(myFriendlyPos);
        case SUMO_ATTR_POSITION_LAT:
            return toString(myPosLat);
        case SUMO_ATTR_LON:
            if (GeoConvHelper::getFinal().getProjString() != "!") {
                return toString(getAttributeDouble(key), 8);
            } else {
                return TL("No geo-conversion defined");
            }
        case SUMO_ATTR_LAT:
            if (GeoConvHelper::getFinal().getProjString() != "!") {
                return toString(getAttributeDouble(key), 8);
            } else {
                return TL("No geo-conversion defined");
            }
        case SUMO_ATTR_TYPE:
            return getShapeType();
        case SUMO_ATTR_ICON:
            return SUMOXMLDefinitions::POIIcons.getString(myPOIIcon);
        case SUMO_ATTR_LAYER:
            return toString(getShapeLayer());
        case SUMO_ATTR_IMGFILE:
            return getShapeImgFile();
        case SUMO_ATTR_WIDTH:
            return toString(myWidth);
        case SUMO_ATTR_HEIGHT:
            return toString(myHeight);
        case SUMO_ATTR_ANGLE:
            return toString(getShapeNaviDegree());
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case GNE_ATTR_SHIFTLANEINDEX:
            return "";
        default:
            return getCommonAttribute(this, key);
    }
}


double
GNEPOI::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_POSITION:
            if (getTagProperty()->getTag() == GNE_TAG_POILANE) {
                return myPosOverLane;
            } else {
                throw InvalidArgument(getTagStr() + " attribute '" + toString(key) + "' not allowed");
            }
        case SUMO_ATTR_POSITION_LAT:
            return myPosLat;
        case SUMO_ATTR_LON:
            if (GeoConvHelper::getFinal().getProjString() != "!") {
                // calculate geo position
                Position GEOPosition = myPosOverView;
                GeoConvHelper::getFinal().cartesian2geo(GEOPosition);
                // return lon
                return GEOPosition.x();
            } else {
                throw InvalidArgument(getTagStr() + " attribute '" + toString(key) + "' not allowed");
            }
        case SUMO_ATTR_LAT:
            if (GeoConvHelper::getFinal().getProjString() != "!") {
                // calculate geo position
                Position GEOPosition = myPosOverView;
                GeoConvHelper::getFinal().cartesian2geo(GEOPosition);
                // return lat
                return GEOPosition.y();
            } else {
                throw InvalidArgument(getTagStr() + " attribute '" + toString(key) + "' not allowed");
            }
        case SUMO_ATTR_LAYER:
            return getShapeLayer();
        case SUMO_ATTR_WIDTH:
            return myWidth;
        case SUMO_ATTR_HEIGHT:
            return myHeight;
        case SUMO_ATTR_ANGLE:
            return getShapeNaviDegree();
        default:
            throw InvalidArgument(getTagStr() + " attribute '" + toString(key) + "' not allowed");
    }
}


const Parameterised::Map&
GNEPOI::getACParametersMap() const {
    return getParametersMap();
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
            if (getTagProperty()->getTag() == GNE_TAG_POILANE) {
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
        const double exaggeration = getExaggeration(s);
        const auto position = getPositionInView();
        // push matrix
        GLHelper::pushMatrix();
        // set POI color
        GUIPointOfInterest::setPOIColor(s, getShapeColor(), this, drawUsingSelectColor());
        // add extra offset z provided by icon to avoid overlapping
        if (myDrawInFront) {
            glTranslated(position.x(), position.y(), GLO_FRONTELEMENT + (double)myPOIIcon);
        } else {
            glTranslated(position.x(), position.y(), s.poiUseCustomLayer ? s.poiCustomLayer : getShapeLayer() + (double)myPOIIcon);
        }
        glRotated(-getShapeNaviDegree(), 0, 0, 1);
        // check if has to be drawn as a circle or with an image
        if (getShapeImgFile() != DEFAULT_IMG_FILE) {
            int textureID = GUITexturesHelper::getTextureID(getShapeImgFile());
            if (textureID > 0) {
                GUITexturesHelper::drawTexturedBox(textureID,
                                                   myWidth * 0.5 * exaggeration, myHeight * 0.5 * exaggeration,
                                                   myWidth * 0.5 * exaggeration,  myHeight * 0.5 * exaggeration);
            } else {
                // draw box
                GLHelper::drawRectangle(Position(0, 0), myWidth * exaggeration, myHeight * exaggeration);
            }
        } else {
            // fallback if no image is defined
            GLHelper::drawFilledCircle(std::max(myWidth, myHeight) * 0.5 * exaggeration, s.poiDetail);
            // check if draw polygon
            if (myPOIIcon != POIIcon::NONE) {
                // translate
                glTranslated(0, 0, 0.1);
                // rotate
                glRotated(180, 0, 0, 1);
                // draw texture
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getPOITexture(myPOIIcon), exaggeration * 0.8);
            }
        }
        // pop matrix
        GLHelper::popMatrix();
        if (!s.drawForRectangleSelection) {
            const Position namePos = position;
            drawName(namePos, s.scale, s.poiName, s.angle);
            if (s.poiType.show(this)) {
                const Position p = namePos + Position(0, -0.6 * s.poiType.size / s.scale);
                GLHelper::drawTextSettings(s.poiType, getShapeType(), p, s.scale, s.angle);
            }
            if (s.poiText.show(this)) {
                GLHelper::pushMatrix();
                glTranslated(namePos.x(), namePos.y(), 0);
                std::string value = getParameter(s.poiTextParam, "");
                if (value != "") {
                    auto lines = StringTokenizer(value, StringTokenizer::NEWLINE).getVector();
                    glRotated(-s.angle, 0, 0, 1);
                    glTranslated(0, 0.7 * s.poiText.scaledSize(s.scale) * (double)lines.size(), 0);
                    glRotated(s.angle, 0, 0, 1);
                    // FONS_ALIGN_LEFT = 1
                    // FONS_ALIGN_CENTER = 2
                    // FONS_ALIGN_MIDDLE = 16
                    const int align = (lines.size() > 1 ? 1 : 2) | 16;
                    for (std::string& line : lines) {
                        GLHelper::drawTextSettings(s.poiText, line, Position(0, 0), s.scale, s.angle, GLO_MAX, align);
                        glRotated(-s.angle, 0, 0, 1);
                        glTranslated(0, -0.7 * s.poiText.scaledSize(s.scale), 0);
                        glRotated(s.angle, 0, 0, 1);
                    }
                }
                GLHelper::popMatrix();
            }
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
        const auto parentEdgeBoundary = (getTagProperty()->getTag() == GNE_TAG_POILANE) ? getParentLanes().front()->getParentEdge() : nullptr;
        if (getShapeImgFile().empty()) {
            myAdditionalContour.calculateContourCircleShape(s, d, this, getPositionInView(), std::max(myWidth, myHeight) * 0.5, getShapeLayer(), exaggeration, parentEdgeBoundary);
        } else {
            myAdditionalContour.calculateContourRectangleShape(s, d, this, getPositionInView(), myHeight * 0.5, myWidth * 0.5, getShapeLayer(), 0, 0, getShapeNaviDegree(), exaggeration, parentEdgeBoundary);
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
            replaceAdditionalParentLanes(value);
            // update boundary (except for template)
            if (getID().size() > 0) {
                updateCenteringBoundary(myTagProperty->getTag() != GNE_TAG_POILANE);
            }
            break;
        case SUMO_ATTR_POSITION: {
            if (myTagProperty->getTag() == GNE_TAG_POILANE) {
                myPosOverLane = parse<double>(value);
            } else {
                myPosOverView = parse<Position>(value);
            }
            // update boundary (except for template)
            if (getID().size() > 0) {
                updateCenteringBoundary(myTagProperty->getTag() != GNE_TAG_POILANE);
            }
            break;
        }
        case SUMO_ATTR_FRIENDLY_POS:
            myFriendlyPos = parse<bool>(value);
            break;
        case SUMO_ATTR_POSITION_LAT:
            myPosLat = parse<double>(value);
            // update boundary (except for template)
            if (getID().size() > 0) {
                updateCenteringBoundary(myTagProperty->getTag() != GNE_TAG_POILANE);
            }
            break;
        case SUMO_ATTR_LON: {
            // parse geo attributes
            Position pos(parse<double>(value), getAttributeDouble(SUMO_ATTR_LAT));
            // transform to cartesian
            GeoConvHelper::getFinal().x2cartesian_const(pos);
            // update view position
            myPosOverView = pos;
            // update boundary (except for template)
            if (getID().size() > 0) {
                updateCenteringBoundary(myTagProperty->getTag() != GNE_TAG_POILANE);
            }
            break;
        }
        case SUMO_ATTR_LAT: {
            // parse geo attributes
            Position pos(getAttributeDouble(SUMO_ATTR_LON), parse<double>(value));
            // transform to cartesian
            GeoConvHelper::getFinal().x2cartesian_const(pos);
            // update view position
            myPosOverView = pos;
            // update boundary (except for template)
            if (getID().size() > 0) {
                updateCenteringBoundary(myTagProperty->getTag() != GNE_TAG_POILANE);
            }
            break;
        }
        case SUMO_ATTR_TYPE:
            setShapeType(value);
            break;
        case SUMO_ATTR_ICON:
            SUMOXMLDefinitions::POIIcons.get(value);
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
            myWidth = parse<double>(value);
            // update boundary (except for template)
            if (getID().size() > 0) {
                updateCenteringBoundary(myTagProperty->getTag() != GNE_TAG_POILANE);
            }
            break;
        case SUMO_ATTR_HEIGHT:
            // set new height
            myHeight = parse<double>(value);
            // update boundary (except for template)
            if (getID().size() > 0) {
                updateCenteringBoundary(myTagProperty->getTag() != GNE_TAG_POILANE);
            }
            break;
        case SUMO_ATTR_ANGLE:
            setShapeNaviDegree(parse<double>(value));
            // update boundary (except for template)
            if (getID().size() > 0) {
                updateCenteringBoundary(myTagProperty->getTag() != GNE_TAG_POILANE);
            }
            break;
        case SUMO_ATTR_NAME:
            myAdditionalName = value;
            break;
        case GNE_ATTR_SHIFTLANEINDEX:
            shiftLaneIndex();
            break;
        default:
            return setCommonAttribute(this, key, value);
    }
}

/****************************************************************************/
