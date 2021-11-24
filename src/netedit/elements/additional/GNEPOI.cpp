/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
#include <string>
#include <netedit/GNENet.h>
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
#include <utils/gui/images/GUITexturesHelper.h>

#include "GNEPOI.h"


// ===========================================================================
// method definitions
// ===========================================================================

GNEPOI::GNEPOI(SumoXMLTag tag, GNENet* net) :
    PointOfInterest("", "", RGBColor::BLACK, Position(0, 0), false, "", 0, false, 0, 0, 0, "", false, 0, 0, "", std::map<std::string, std::string>()),
    GNEShape("", net, GLO_POI, tag, {}, {}, {}, {}, {}, {}, {}, {}) {
    // reset default values
    resetDefaultValues();
}


GNEPOI::GNEPOI(GNENet* net, const std::string& id, const std::string& type, const RGBColor& color, const double xLon,
               const double yLat, const bool geo, const double layer, const double angle, const std::string& imgFile,
               const bool relativePath, const double width, const double height, const std::string& name,
               const std::map<std::string, std::string>& parameters) :
    PointOfInterest(id, type, color, Position(xLon, yLat), geo, "", 0, false, 0, layer, angle, imgFile, relativePath, width, height, name, parameters),
    GNEShape(id, net, GLO_POI, geo ? GNE_TAG_POIGEO : SUMO_TAG_POI, {}, {}, {}, {}, {}, {}, {}, {}) {
    // update position depending of GEO
    if (geo) {
        Position cartesian(x(), y());
        GeoConvHelper::getFinal().x2cartesian_const(cartesian);
        set(cartesian.x(), cartesian.y());
    }
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEPOI::GNEPOI(GNENet* net, const std::string& id, const std::string& type, const RGBColor& color, GNELane* lane, const double posOverLane,
               const bool friendlyPos, const double posLat, const double layer, const double angle, const std::string& imgFile, const bool relativePath, const double width,
               const double height, const std::string& name, const std::map<std::string, std::string>& parameters) :
    PointOfInterest(id, type, color, Position(), false, lane->getID(), posOverLane, friendlyPos, posLat, layer, angle, imgFile, relativePath, width, height, name, parameters),
    GNEShape(id, net, GLO_POI, GNE_TAG_POILANE,
        {}, {}, {lane}, {}, {}, {}, {}, {}
    ) {
    // update geometry (needed for POILanes)
    updateGeometry();
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEPOI::~GNEPOI() {}


GNEMoveOperation*
GNEPOI::getMoveOperation() {
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork() &&
            (myNet->getViewNet()->getEditModes().networkEditMode == NetworkEditMode::NETWORK_MOVE) &&
            myNet->getViewNet()->getMouseButtonKeyPressed().shiftKeyPressed()) {
        // get snap radius
        const double snap_radius = myNet->getViewNet()->getVisualisationSettings().neteditSizeSettings.additionalGeometryPointRadius;
        // get mouse position
        const Position mousePosition = myNet->getViewNet()->getPositionInformation();
        // check if we're editing width or height
        if ((myShapeWidth.size() == 0) || (myShapeHeight.size() == 0)) {
            return nullptr;
        } else if (myShapeHeight.front().distanceSquaredTo2D(mousePosition) <= (snap_radius * snap_radius)) {
            // edit height
            return new GNEMoveOperation(this, myShapeHeight, true, GNEMoveOperation::OperationType::HEIGHT);
        } else if (myShapeHeight.back().distanceSquaredTo2D(mousePosition) <= (snap_radius * snap_radius)) {
            // edit height
            return new GNEMoveOperation(this, myShapeHeight, false, GNEMoveOperation::OperationType::HEIGHT);
        } else if (myShapeWidth.front().distanceSquaredTo2D(mousePosition) <= (snap_radius * snap_radius)) {
            // edit width
            return new GNEMoveOperation(this, myShapeWidth, true, GNEMoveOperation::OperationType::WIDTH);
        } else if (myShapeWidth.back().distanceSquaredTo2D(mousePosition) <= (snap_radius * snap_radius)) {
            // edit width
            return new GNEMoveOperation(this, myShapeWidth, false, GNEMoveOperation::OperationType::WIDTH);
        } else {
            return nullptr;
        }
    } else if (getTagProperty().getTag() == GNE_TAG_POILANE) {
        // return move operation for POI placed over lane
        return new GNEMoveOperation(this, getParentLanes().front(), myPosOverLane,
                                    myNet->getViewNet()->getViewParent()->getMoveFrame()->getCommonModeOptions()->getAllowChangeLane());
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


void
GNEPOI::setParameter(const std::string& key, const std::string& value) {
    Parameterised::setParameter(key, value);
}


CommonXMLStructure::SumoBaseObject*
GNEPOI::getSumoBaseObject() const {
    CommonXMLStructure::SumoBaseObject* POIBaseObject = new CommonXMLStructure::SumoBaseObject(nullptr);
    POIBaseObject->setTag(SUMO_TAG_POI);
    // fill attributes
    POIBaseObject->addStringAttribute(SUMO_ATTR_ID, myID);
    POIBaseObject->addColorAttribute(SUMO_ATTR_COLOR, getShapeColor());
    POIBaseObject->addStringAttribute(SUMO_ATTR_TYPE, getShapeType());
    POIBaseObject->addDoubleAttribute(SUMO_ATTR_LAYER, getShapeLayer());
    POIBaseObject->addStringAttribute(SUMO_ATTR_IMGFILE, getShapeImgFile());
    POIBaseObject->addDoubleAttribute(SUMO_ATTR_WIDTH, getWidth());
    POIBaseObject->addDoubleAttribute(SUMO_ATTR_HEIGHT, getHeight());
    POIBaseObject->addDoubleAttribute(SUMO_ATTR_ANGLE, getShapeNaviDegree());
    POIBaseObject->addStringAttribute(SUMO_ATTR_NAME, getShapeName());
    POIBaseObject->addBoolAttribute(SUMO_ATTR_RELATIVEPATH, getShapeRelativePath());
    return POIBaseObject;
}


void
GNEPOI::writeShape(OutputDevice& device) {
    if (getParentLanes().size() > 0) {
        // obtain fixed position over lane
        double fixedPositionOverLane = myPosOverLane > getParentLanes().at(0)->getLaneShape().length() ? getParentLanes().at(0)->getLaneShape().length() : myPosOverLane < 0 ? 0 : myPosOverLane;
        // write POILane using POI::writeXML
        writeXML(device, false, 0, getParentLanes().at(0)->getID(), fixedPositionOverLane, myFriendlyPos, myPosLat);
    } else {
        writeXML(device, myGeo);
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
        // calculate shape lenght
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
    myBoundary.reset();
    // add position (this POI)
    myBoundary.add(*this);
    // grow boundary
    myBoundary.grow(10 + std::max(getWidth() * 0.5, getHeight() * 0.5));
    // add object into net
    if (updateGrid) {
        myNet->addGLObjectIntoGrid(this);
    }
}


GUIGlID
GNEPOI::getGlID() const {
    return GUIGlObject::getGlID();
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
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    // build selection and show parameters menu
    myNet->getViewNet()->buildSelectionACPopupEntry(ret, this);
    buildShowParamsPopupEntry(ret);
    // continue depending of lane number
    if (getParentLanes().size() > 0) {
        // add option for convert to GNEPOI
        GUIDesigns::buildFXMenuCommand(ret, "Release from " + toString(SUMO_TAG_LANE), GUIIconSubSys::getIcon(GUIIcon::LANE), &parent, MID_GNE_POI_TRANSFORM);
        return ret;
    } else {
        // add option for convert to GNEPOI
        GUIDesigns::buildFXMenuCommand(ret, "Attach to nearest " + toString(SUMO_TAG_LANE), GUIIconSubSys::getIcon(GUIIcon::LANE), &parent, MID_GNE_POI_TRANSFORM);
    }
    return ret;
}


GUIParameterTableWindow*
GNEPOI::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& /*parent*/) {
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this);
    // add items
    ret->mkItem("type", false, getShapeType());
    ret->mkItem("layer", false, getShapeLayer());
    ret->closeBuilding(this);
    return ret;
}


void
GNEPOI::drawGL(const GUIVisualizationSettings& s) const {
    // first check if POI can be drawn
    if (myNet->getViewNet()->getDemandViewOptions().showShapes() && myNet->getViewNet()->getDataViewOptions().showShapes()) {
        // check if boundary has to be drawn
        if (s.drawBoundaries) {
            GLHelper::drawBoundary(myBoundary);
        }
        // check if POI can be drawn
        if (GUIPointOfInterest::checkDraw(s, this)) {
            // obtain POIExaggeration
            const double POIExaggeration = getExaggeration(s);
            // push name (needed for getGUIGlObjectsUnderCursor(...)
            GLHelper::pushName(getGlID());
            // draw inner polygon
            if (myNet->getViewNet()->getFrontAttributeCarrier() == this) {
                GUIPointOfInterest::drawInnerPOI(s, this, this, drawUsingSelectColor(), GLO_DOTTEDCONTOUR_FRONT,
                                                 myShapeWidth.length2D(), myShapeHeight.length2D());
            } else {
                GUIPointOfInterest::drawInnerPOI(s, this, this, drawUsingSelectColor(), getShapeLayer(),
                                                 myShapeWidth.length2D(), myShapeHeight.length2D());
            }
            // draw an orange square mode if there is an image(see #4036)
            if (!getShapeImgFile().empty() && myNet->getViewNet()->getTestingMode().isTestingEnabled()) {
                // Add a draw matrix for drawing logo
                GLHelper::pushMatrix();
                glTranslated(x(), y(), getType() + 0.01);
                GLHelper::setColor(RGBColor::ORANGE);
                GLHelper::drawBoxLine(Position(0, 1), 0, 2, 1);
                GLHelper::popMatrix();
            }
            // draw geometry points
            GNEAdditional::drawUpGeometryPoint(myNet->getViewNet(), myShapeHeight.front(), 180, RGBColor::ORANGE);
            GNEAdditional::drawDownGeometryPoint(myNet->getViewNet(), myShapeHeight.back(), 180, RGBColor::ORANGE);
            GNEAdditional::drawLeftGeometryPoint(myNet->getViewNet(), myShapeWidth.back(), -90, RGBColor::ORANGE);
            GNEAdditional::drawRightGeometryPoint(myNet->getViewNet(), myShapeWidth.front(), -90, RGBColor::ORANGE);
            // pop name
            GLHelper::popName();
            // draw lock icon
            GNEViewNetHelper::LockIcon::drawLockIcon(this, getType(), getPositionInView(), POIExaggeration);
            // check if dotted contour has to be drawn
            if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                if (getShapeImgFile().empty()) {
                    GUIDottedGeometry::drawDottedContourCircle(GUIDottedGeometry::DottedContourType::INSPECT, s, *this, 1.3, POIExaggeration);
                } else {
                    GUIDottedGeometry::drawDottedSquaredShape(GUIDottedGeometry::DottedContourType::INSPECT, s, *this, getHeight() * 0.5, getWidth() * 0.5, 0, 0, getShapeNaviDegree(), POIExaggeration);
                }
            }
            // check if front dotted contour has to be drawn
            if ((myNet->getViewNet()->getFrontAttributeCarrier() == this)) {
                if (getShapeImgFile().empty()) {
                    GUIDottedGeometry::drawDottedContourCircle(GUIDottedGeometry::DottedContourType::FRONT, s, *this, 1.3, POIExaggeration);
                } else {
                    GUIDottedGeometry::drawDottedSquaredShape(GUIDottedGeometry::DottedContourType::FRONT, s, *this, getHeight() * 0.5, getWidth() * 0.5, 0, 0, getShapeNaviDegree(), POIExaggeration);
                }
            }
        }
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
        case SUMO_ATTR_LON: {
            // calculate geo position
            Position GEOPosition(x(), y());
            GeoConvHelper::getFinal().cartesian2geo(GEOPosition);
            // return lon
            return toString(GEOPosition.x());
        }
        case SUMO_ATTR_LAT: {
            // calculate geo position
            Position GEOPosition(x(), y());
            GeoConvHelper::getFinal().cartesian2geo(GEOPosition);
            // return lat
            return toString(GEOPosition.y());
        }
        case SUMO_ATTR_TYPE:
            return getShapeType();
        case SUMO_ATTR_LAYER:
            if (getShapeLayer() == Shape::DEFAULT_LAYER_POI) {
                return "default";
            } else {
                return toString(getShapeLayer());
            }
        case SUMO_ATTR_IMGFILE:
            return getShapeImgFile();
        case SUMO_ATTR_RELATIVEPATH:
            return toString(getShapeRelativePath());
        case SUMO_ATTR_WIDTH:
            return toString(getWidth());
        case SUMO_ATTR_HEIGHT:
            return toString(getHeight());
        case SUMO_ATTR_ANGLE:
            return toString(getShapeNaviDegree());
        case SUMO_ATTR_NAME:
            return getShapeName();
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        case GNE_ATTR_SHIFTLANEINDEX:
            return "";
        default:
            throw InvalidArgument(getTagStr() + " attribute '" + toString(key) + "' not allowed");
    }
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
        case SUMO_ATTR_LAYER:
        case SUMO_ATTR_IMGFILE:
        case SUMO_ATTR_RELATIVEPATH:
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_HEIGHT:
        case SUMO_ATTR_ANGLE:
        case SUMO_ATTR_NAME:
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
GNEPOI::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            if (SUMOXMLDefinitions::isValidTypeID(value)) {
                if (myTagProperty.getTag() == SUMO_TAG_POI) {
                    // POI
                    return (myNet->getAttributeCarriers()->retrieveShape(GNE_TAG_POILANE, value, false) == nullptr) &&
                           (myNet->getAttributeCarriers()->retrieveShape(GNE_TAG_POIGEO, value, false) == nullptr);
                } else if (myTagProperty.getTag() == GNE_TAG_POILANE) {
                    // POILane
                    return (myNet->getAttributeCarriers()->retrieveShape(SUMO_TAG_POI, value, false) == nullptr) &&
                           (myNet->getAttributeCarriers()->retrieveShape(GNE_TAG_POIGEO, value, false) == nullptr);
                } else if (myTagProperty.getTag() == GNE_TAG_POIGEO) {
                    // POI Geo
                    return (myNet->getAttributeCarriers()->retrieveShape(SUMO_TAG_POI, value, false) == nullptr) &&
                           (myNet->getAttributeCarriers()->retrieveShape(GNE_TAG_POILANE, value, false) == nullptr);
                } else {
                    // invalid POI tag
                    return false;
                }
            } else {
                // invalid id
                return false;
            }
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
        case SUMO_ATTR_LAYER:
            if (value == "default") {
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
        case SUMO_ATTR_RELATIVEPATH:
            return canParse<bool>(value);
        case SUMO_ATTR_WIDTH:
            return canParse<double>(value) && (parse<double>(value) > 0);
        case SUMO_ATTR_HEIGHT:
            return canParse<double>(value) && (parse<double>(value) > 0);
        case SUMO_ATTR_ANGLE:
            return canParse<double>(value);
        case SUMO_ATTR_NAME:
            return SUMOXMLDefinitions::isValidAttribute(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEPOI::isAttributeEnabled(SumoXMLAttr /* key */) const {
    // check if we're in supermode Network
    if (myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) {
        return true;
    } else {
        return false;
    }
}


const std::map<std::string, std::string>&
GNEPOI::getACParametersMap() const {
    return getParametersMap();
}

// ===========================================================================
// private
// ===========================================================================

void
GNEPOI::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID: {
            // update microsimID
            setMicrosimID(value);
            // set named ID
            myID = value;
            break;
        }
        case SUMO_ATTR_COLOR:
            setShapeColor(parse<RGBColor>(value));
            break;
        case SUMO_ATTR_LANE:
            myLane = value;
            replaceShapeParentLanes(value);
            break;
        case SUMO_ATTR_POSITION: {
            if (getParentLanes().size() > 0) {
                if (canParse<double>(value)) {
                    myPosOverLane = parse<double>(value);
                }
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
        case SUMO_ATTR_LAYER:
            if (value == "default") {
                setShapeLayer(Shape::DEFAULT_LAYER_POI);
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
        case SUMO_ATTR_RELATIVEPATH:
            setShapeRelativePath(parse<bool>(value));
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
            throw InvalidArgument(getTagStr() + " attribute '" + toString(key) + "' not allowed");
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
        if (getTagProperty().getTag() == GNE_TAG_POILANE) {
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
        undoList->begin(myTagProperty.getGUIIcon(), "height of " + getTagStr());
        setAttribute(SUMO_ATTR_HEIGHT, toString(moveResult.shapeToUpdate.length2D()), undoList);
        undoList->end();
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::WIDTH) {
        undoList->begin(myTagProperty.getGUIIcon(), "width of " + getTagStr());
        setAttribute(SUMO_ATTR_WIDTH, toString(moveResult.shapeToUpdate.length2D()), undoList);
        undoList->end();
    } else {
        undoList->begin(GUIIcon::POI, "position of " + getTagStr());
        if (getTagProperty().getTag() == GNE_TAG_POILANE) {
            undoList->changeAttribute(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(moveResult.newFirstPos)));
        } else {
            undoList->changeAttribute(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(moveResult.shapeToUpdate.front())));
        }
        undoList->end();
    }
}

/****************************************************************************/
