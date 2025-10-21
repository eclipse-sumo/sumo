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
// A class for visualizing and editing POIS in netedit
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/elements/moving/GNEMoveElementLaneSingle.h>
#include <netedit/elements/moving/GNEMoveElementViewResizable.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/globjects/GUIPointOfInterest.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/xml/NamespaceIDs.h>

#include "GNEPOI.h"

// ===========================================================================
// method definitions
// ===========================================================================

GNEPOI::GNEPOI(SumoXMLTag tag, GNENet* net) :
    Shape(""),
    GNEAdditional("", net, "", tag, ""),
    myMoveElementLaneSingle(new GNEMoveElementLaneSingle(this, SUMO_ATTR_POSITION, myPosOverLane, myFriendlyPos, GNEMoveElementLaneSingle::PositionType::SINGLE)),
    myMoveElementViewResizable(new GNEMoveElementViewResizable(this, (tag == GNE_TAG_POIGEO) ? GNEMoveElementView::AttributesFormat::GEO : GNEMoveElementView::AttributesFormat::CARTESIAN,
                               GNEMoveElementViewResizable::ResizingFormat::WIDTH_HEIGHT, SUMO_ATTR_POSITION, myPosOverView, myWidth, myHeight)) {
}


GNEPOI::GNEPOI(const std::string& id, GNENet* net, const std::string& filename, const std::string& type, const RGBColor& color, const Position& pos,
               const bool geo, POIIcon icon, const double layer, const double angle, const std::string& imgFile, const double width,
               const double height, const std::string& name, const Parameterised::Map& parameters) :
    Shape(id, type, color, layer, angle, imgFile, ""),
    GNEAdditional(id, net, filename, geo ? GNE_TAG_POIGEO : SUMO_TAG_POI, name),
    Parameterised(parameters),
    myPosOverView(pos),
    myWidth(width),
    myHeight(height),
    myPOIIcon(icon),
    myMoveElementLaneSingle(new GNEMoveElementLaneSingle(this, SUMO_ATTR_POSITION, myPosOverLane, myFriendlyPos, GNEMoveElementLaneSingle::PositionType::SINGLE)),
    myMoveElementViewResizable(new GNEMoveElementViewResizable(this, geo ? GNEMoveElementView::AttributesFormat::GEO : GNEMoveElementView::AttributesFormat::CARTESIAN,
                               GNEMoveElementViewResizable::ResizingFormat::WIDTH_HEIGHT, SUMO_ATTR_POSITION, myPosOverView, myWidth, myHeight)) {
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
    Shape(id, type, color, layer, angle, imgFile, name),
    GNEAdditional(id, net, filename, GNE_TAG_POILANE, ""),
    Parameterised(parameters),
    myPosOverLane(posOverLane),
    myFriendlyPos(friendlyPos),
    myWidth(width),
    myHeight(height),
    myPosLat(posLat),
    myPOIIcon(icon),
    myMoveElementLaneSingle(new GNEMoveElementLaneSingle(this, SUMO_ATTR_POSITION, myPosOverLane, myFriendlyPos, GNEMoveElementLaneSingle::PositionType::SINGLE)),
    myMoveElementViewResizable(new GNEMoveElementViewResizable(this, GNEMoveElementView::AttributesFormat::POSITION, GNEMoveElementViewResizable::ResizingFormat::WIDTH_HEIGHT,
                               SUMO_ATTR_POSITION, myPosOverView, myWidth, myHeight)) {
    // set parents
    setParent<GNELane*>(lane);
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEPOI::~GNEPOI() {
    delete myMoveElementLaneSingle;
    delete myMoveElementViewResizable;
}


GNEMoveElement*
GNEPOI::getMoveElement() const {
    if (getTagProperty()->getTag() == GNE_TAG_POILANE) {
        return myMoveElementLaneSingle;
    } else {
        return myMoveElementViewResizable;
    }
}


Parameterised*
GNEPOI::getParameters() {
    return this;
}


const Parameterised*
GNEPOI::getParameters() const {
    return this;
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
    // write common additional attributes
    writeAdditionalAttributes(device);
    // specific of poi lanes
    if (getTagProperty()->getTag() == GNE_TAG_POILANE) {
        // write move attributes
        myMoveElementLaneSingle->writeMoveAttributes(device);
        // write specific attributes
        if (myPosLat != 0) {
            device.writeAttr(SUMO_ATTR_POSITION_LAT, myPosLat);
        }
    } else {
        // write move attributes
        myMoveElementViewResizable->writeMoveAttributes(device);
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
    if (getTagProperty()->getTag() == GNE_TAG_POILANE) {
        // only movement problems
        return myMoveElementLaneSingle->isMoveElementValid();
    } else {
        return true;
    }
}


std::string
GNEPOI::getAdditionalProblem() const {
    // only for POIS over lanes
    if (getTagProperty()->getTag() == GNE_TAG_POILANE) {
        // only movement problems
        return myMoveElementLaneSingle->getMovingProblem();
    } else {
        return "";
    }
}


void
GNEPOI::fixAdditionalProblem() {
    // only for POIS over lanes
    if (getTagProperty()->getTag() == GNE_TAG_POILANE) {
        // only movement problems
        myMoveElementLaneSingle->fixMovingProblem();
    }
}


void
GNEPOI::updateGeometry() {
    // check if update width and height shapes
    if ((myWidth > 0) && (myHeight > 0)) {
        // calculate shape length
        myMoveElementViewResizable->myShapeHeight.clear();
        myMoveElementViewResizable->myShapeHeight.push_back(Position(0, myHeight * -0.5));
        myMoveElementViewResizable->myShapeHeight.push_back(Position(0, myHeight * 0.5));
        // move
        myMoveElementViewResizable->myShapeHeight.add(myPosOverView);
        // calculate shape width
        PositionVector leftShape = myMoveElementViewResizable->myShapeHeight;
        leftShape.move2side(myWidth * -0.5);
        PositionVector rightShape = myMoveElementViewResizable->myShapeHeight;
        rightShape.move2side(myWidth * 0.5);
        myMoveElementViewResizable->myShapeWidth = {leftShape.getCentroid(), rightShape.getCentroid()};
    }
    // set additional geometry
    if (getParentLanes().size() > 0) {
        myAdditionalGeometry.updateGeometry(getParentLanes().front()->getLaneShape(),
                                            myMoveElementLaneSingle->getFixedPositionOverLane(true), myPosLat);
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
    for (const auto& pos : myMoveElementViewResizable->myShapeWidth) {
        myAdditionalBoundary.add(pos);
    }
    // add height
    for (const auto& pos : myMoveElementViewResizable->myShapeHeight) {
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
                if ((myMoveElementViewResizable->myShapeHeight.front().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared) ||
                        (myMoveElementViewResizable->myShapeHeight.back().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared)) {
                    myMoveElementViewResizable->myMovingContourUp.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidthSmall, true);
                    myMoveElementViewResizable->myMovingContourDown.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidthSmall, true);
                } else if ((myMoveElementViewResizable->myShapeWidth.front().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared) ||
                           (myMoveElementViewResizable->myShapeWidth.back().distanceSquaredTo2D(mousePosition) <= snapRadiusSquared)) {
                    myMoveElementViewResizable->myMovingContourLeft.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidthSmall, true);
                    myMoveElementViewResizable->myMovingContourRight.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidthSmall, true);
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
            if (getTagProperty()->getTag() == GNE_TAG_POILANE) {
                return myMoveElementLaneSingle->getMovingAttribute(key);
            } else {
                return myMoveElementViewResizable->getMovingAttribute(key);
            }
    }
}


double
GNEPOI::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
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
            if (getTagProperty()->getTag() == GNE_TAG_POILANE) {
                return myMoveElementLaneSingle->getMovingAttributeDouble(key);
            } else {
                return myMoveElementViewResizable->getMovingAttributeDouble(key);
            }
    }
}


Position
GNEPOI::getAttributePosition(SumoXMLAttr key) const {
    if (getTagProperty()->getTag() == GNE_TAG_POILANE) {
        return myMoveElementLaneSingle->getMovingAttributePosition(key);
    } else {
        return myMoveElementViewResizable->getMovingAttributePosition(key);
    }
}


PositionVector
GNEPOI::getAttributePositionVector(SumoXMLAttr key) const {
    return getCommonAttributePositionVector(key);
}


void
GNEPOI::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_COLOR:
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
            if (getTagProperty()->getTag() == GNE_TAG_POILANE) {
                return myMoveElementLaneSingle->setMovingAttribute(key, value, undoList);
            } else {
                return myMoveElementViewResizable->setMovingAttribute(key, value, undoList);
            }
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
            if (getTagProperty()->getTag() == GNE_TAG_POILANE) {
                return myMoveElementLaneSingle->isMovingAttributeValid(key, value);
            } else {
                return myMoveElementViewResizable->isMovingAttributeValid(key, value);
            }
            break;
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
                                                   myWidth * 0.5 * exaggeration, myHeight * 0.5 * exaggeration);
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
            if (myMoveElementViewResizable->myShapeHeight.size() > 0) {
                drawUpGeometryPoint(s, d, myMoveElementViewResizable->myShapeHeight.front(), 180, RGBColor::ORANGE);
                drawDownGeometryPoint(s, d, myMoveElementViewResizable->myShapeHeight.back(), 180, RGBColor::ORANGE);
            }
            if (myMoveElementViewResizable->myShapeWidth.size() > 0) {
                drawLeftGeometryPoint(s, d, myMoveElementViewResizable->myShapeWidth.back(), -90, RGBColor::ORANGE);
                drawRightGeometryPoint(s, d, myMoveElementViewResizable->myShapeWidth.front(), -90, RGBColor::ORANGE);
            }
        }
    }
}


void
GNEPOI::calculatePOIContour(const GUIVisualizationSettings& s, const GUIVisualizationSettings::Detail d,
                            const double exaggeration, const bool movingGeometryPoints) const {
    // check if we're calculating the contour or the moving geometry points
    if (movingGeometryPoints) {
        myMoveElementViewResizable->myMovingContourUp.calculateContourCircleShape(s, d, this, myMoveElementViewResizable->myShapeHeight.front(), s.neteditSizeSettings.additionalGeometryPointRadius,
                getShapeLayer(), exaggeration, nullptr);
        myMoveElementViewResizable->myMovingContourDown.calculateContourCircleShape(s, d, this, myMoveElementViewResizable->myShapeHeight.back(), s.neteditSizeSettings.additionalGeometryPointRadius,
                getShapeLayer(), exaggeration, nullptr);
        myMoveElementViewResizable->myMovingContourLeft.calculateContourCircleShape(s, d, this, myMoveElementViewResizable->myShapeWidth.front(), s.neteditSizeSettings.additionalGeometryPointRadius,
                getShapeLayer(), exaggeration, nullptr);
        myMoveElementViewResizable->myMovingContourRight.calculateContourCircleShape(s, d, this, myMoveElementViewResizable->myShapeWidth.back(), s.neteditSizeSettings.additionalGeometryPointRadius,
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
            break;
        case SUMO_ATTR_POSITION: {
            if (myTagProperty->getTag() == GNE_TAG_POILANE) {
                myPosOverLane = parse<double>(value);
            } else {
                myPosOverView = parse<Position>(value);
            }
            break;
        }
        case SUMO_ATTR_POSITION_LAT:
            myPosLat = parse<double>(value);
            break;
        case SUMO_ATTR_LON: {
            // parse geo attributes
            Position pos(parse<double>(value), getAttributeDouble(SUMO_ATTR_LAT));
            // transform to cartesian
            GeoConvHelper::getFinal().x2cartesian_const(pos);
            // update view position
            myPosOverView = pos;
            break;
        }
        case SUMO_ATTR_LAT: {
            // parse geo attributes
            Position pos(getAttributeDouble(SUMO_ATTR_LON), parse<double>(value));
            // transform to cartesian
            GeoConvHelper::getFinal().x2cartesian_const(pos);
            // update view position
            myPosOverView = pos;
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
            break;
        case SUMO_ATTR_HEIGHT:
            // set new height
            myHeight = parse<double>(value);
            break;
        case SUMO_ATTR_ANGLE:
            setShapeNaviDegree(parse<double>(value));
            break;
        case SUMO_ATTR_NAME:
            myAdditionalName = value;
            break;
        case GNE_ATTR_SHIFTLANEINDEX:
            shiftLaneIndex();
            break;
        default:
            if (getTagProperty()->getTag() == GNE_TAG_POILANE) {
                return myMoveElementLaneSingle->setMovingAttribute(key, value);
            } else {
                return myMoveElementViewResizable->setMovingAttribute(key, value);
            }
            break;
    }
    // update boundary (except for template)
    if (getID().size() > 0) {
        updateCenteringBoundary(myTagProperty->getTag() != GNE_TAG_POILANE);
    }
}

/****************************************************************************/
