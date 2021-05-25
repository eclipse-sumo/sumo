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

GNEPOI::GNEPOI(GNENet* net, const std::string& id, const std::string& type, const RGBColor& color, const Position& pos,
        bool geo, double layer, double angle, const std::string& imgFile, bool relativePath, double width, double height, 
        const std::string &name, const std::map<std::string, std::string> &parameters, bool movementBlocked) :
    PointOfInterest(id, type, color, pos, geo, "", 0, 0, layer, angle, imgFile, relativePath, width, height),
    GNEShape(id, net, GLO_POI, SUMO_TAG_POI, 
        {}, {}, {}, {}, {}, {}, {}, {},
        parameters, movementBlocked) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
    // set GEO Position
    myGEOPosition = pos;
    GeoConvHelper::getFinal().cartesian2geo(myGEOPosition);
}


GNEPOI::GNEPOI(GNENet* net, const std::string& id, const std::string& type, const RGBColor& color, double layer, double angle, 
        const std::string& imgFile, bool relativePath, GNELane* lane, double posOverLane, double posLat, double width, 
        double height, const std::string &name, const std::map<std::string, std::string> &parameters, bool movementBlocked) :
    PointOfInterest(id, type, color, Position(), false, lane->getID(), posOverLane, posLat, layer, angle, imgFile, relativePath, width, height),
    GNEShape(id, net, GLO_POI, SUMO_TAG_POILANE, 
        {}, {}, {lane}, {}, {}, {}, {}, {},
        parameters, movementBlocked) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEPOI::~GNEPOI() {}


GNEMoveOperation*
GNEPOI::getMoveOperation(const double /* shapeOffset */) {
    if (myBlockMovement) {
        return nullptr;
    } else if (getTagProperty().getTag() == SUMO_TAG_POILANE) {
        // return move operation for POI placed over lane
        return new GNEMoveOperation(this, getParentLanes().front(), {myPosOverLane},
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
GNEPOI::generateChildID(SumoXMLTag childTag) {
    int counter = (int)myNet->getAttributeCarriers()->getShapes().at(SUMO_TAG_POI).size();
    while (myNet->retrieveShape(SUMO_TAG_POI, getID() + toString(childTag) + toString(counter), false) != nullptr) {
        counter++;
    }
    return (getID() + toString(childTag) + toString(counter));
}


void
GNEPOI::setParameter(const std::string& key, const std::string& value) {
    Parameterised::setParameter(key, value);
}


void
GNEPOI::writeShape(OutputDevice& device) {
    if (getParentLanes().size() > 0) {
        // obtain fixed position over lane
        double fixedPositionOverLane = myPosOverLane > getParentLanes().at(0)->getLaneShape().length() ? getParentLanes().at(0)->getLaneShape().length() : myPosOverLane < 0 ? 0 : myPosOverLane;
        // write POILane using POI::writeXML
        writeXML(device, false, 0, getParentLanes().at(0)->getID(), fixedPositionOverLane, myPosLat);
    } else {
        writeXML(device, myGeo);
    }
}


void
GNEPOI::updateGeometry() {
    if (getParentLanes().size() > 0) {
        // obtain fixed position over lane
        double fixedPositionOverLane = myPosOverLane > getParentLanes().at(0)->getLaneShapeLength() ? getParentLanes().at(0)->getLaneShapeLength() : myPosOverLane < 0 ? 0 : myPosOverLane;
        // set new position regarding to lane
        set(getParentLanes().at(0)->getLaneShape().positionAtOffset(fixedPositionOverLane * getParentLanes().at(0)->getLengthGeometryFactor(), -myPosLat));
    }
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
    myBoundary.grow(10 + std::max(getWidth(), getHeight()));
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
            const double POIExaggeration = s.poiSize.getExaggeration(s, this);
            // push name (needed for getGUIGlObjectsUnderCursor(...)
            glPushName(getGlID());
            // draw inner polygon
            if (myNet->getViewNet()->getFrontAttributeCarrier() == this) {
                GUIPointOfInterest::drawInnerPOI(s, this, this, drawUsingSelectColor(), GLO_DOTTEDCONTOUR_FRONT);
            } else {
                GUIPointOfInterest::drawInnerPOI(s, this, this, drawUsingSelectColor(), getShapeLayer());
            }
            // draw an orange square mode if there is an image(see #4036)
            if (!getShapeImgFile().empty() && myNet->getViewNet()->getTestingMode().isTestingEnabled()) {
                // Add a draw matrix for drawing logo
                glPushMatrix();
                glTranslated(x(), y(), getType() + 0.01);
                GLHelper::setColor(RGBColor::ORANGE);
                GLHelper::drawBoxLine(Position(0, 1), 0, 2, 1);
                glPopMatrix();
            }
            // check if dotted contour has to be drawn
            if (s.drawDottedContour() || myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                if (getShapeImgFile().empty()) {
                    GNEGeometry::drawDottedContourCircle(GNEGeometry::DottedContourType::INSPECT, s, *this, 1.3, POIExaggeration);
                } else {
                    GNEGeometry::drawDottedSquaredShape(GNEGeometry::DottedContourType::INSPECT, s, *this, getWidth(), getHeight(), 0, 0, getShapeNaviDegree(), POIExaggeration);
                }
            }
            // check if front dotted contour has to be drawn
            if (s.drawDottedContour() || (myNet->getViewNet()->getFrontAttributeCarrier() == this)) {
                if (getShapeImgFile().empty()) {
                    GNEGeometry::drawDottedContourCircle(GNEGeometry::DottedContourType::FRONT, s, *this, 1.3, POIExaggeration);
                } else {
                    GNEGeometry::drawDottedSquaredShape(GNEGeometry::DottedContourType::FRONT, s, *this, getWidth(), getHeight(), 0, 0, getShapeNaviDegree(), POIExaggeration);
                }
            }
            // pop name
            glPopName();
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
        case SUMO_ATTR_POSITION_LAT:
            return toString(myPosLat);
        case SUMO_ATTR_GEOPOSITION:
            return toString(myGEOPosition, gPrecisionGeo);
        case SUMO_ATTR_GEO:
            return toString(myGeo);
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
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlockMovement);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " attribute '" + toString(key) + "' not allowed");
    }
}


void
GNEPOI::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_COLOR:
        case SUMO_ATTR_LANE:
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_POSITION_LAT:
        case SUMO_ATTR_GEOPOSITION:
        case SUMO_ATTR_GEO:
        case SUMO_ATTR_TYPE:
        case SUMO_ATTR_LAYER:
        case SUMO_ATTR_IMGFILE:
        case SUMO_ATTR_RELATIVEPATH:
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_HEIGHT:
        case SUMO_ATTR_ANGLE:
        case GNE_ATTR_BLOCK_MOVEMENT:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEPOI::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return SUMOXMLDefinitions::isValidTypeID(value) &&
                   (myNet->retrieveShape(SUMO_TAG_POI, value, false) == nullptr);
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case SUMO_ATTR_LANE:
            return (myNet->retrieveLane(value, false) != nullptr);
        case SUMO_ATTR_POSITION:
            if (getParentLanes().size() > 0) {
                return canParse<double>(value);
            } else {
                return canParse<Position>(value);
            }
        case SUMO_ATTR_POSITION_LAT:
            return canParse<double>(value);
        case SUMO_ATTR_GEOPOSITION: {
            return canParse<Position>(value);
        }
        case SUMO_ATTR_GEO:
            return canParse<bool>(value);
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
        case GNE_ATTR_BLOCK_MOVEMENT:
            return canParse<bool>(value);
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
            // note: getAttributeCarriers().updateID doesn't change Microsim ID in GNEShapes
            myNet->getAttributeCarriers()->updateID(this, value);
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
                // set GEO Position
                myGEOPosition.setx(this->x());
                myGEOPosition.sety(this->y());
                myGEOPosition.setz(this->z());
                GeoConvHelper::getFinal().cartesian2geo(myGEOPosition);
            }
            // update centering boundary
            updateCenteringBoundary(true);
            break;
        }
        case SUMO_ATTR_POSITION_LAT:
            myPosLat = parse<double>(value);
            // update centering boundary
            updateCenteringBoundary(true);
            break;
        case SUMO_ATTR_GEOPOSITION: {
            // set new position
            myGEOPosition = parse<Position>(value);
            // set cartesian Position
            set(myGEOPosition);
            GeoConvHelper::getFinal().x2cartesian_const(*this);
            // update centering boundary
            updateCenteringBoundary(true);
            break;
        }
        case SUMO_ATTR_GEO:
            myGeo = parse<bool>(value);
            break;
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
            myNet->removeGLObjectFromGrid(this);
            setShapeImgFile(value);
            // all textures must be refresh
            GUITexturesHelper::clearTextures();
            // add object into grid again
            myNet->addGLObjectIntoGrid(this);
            break;
        case SUMO_ATTR_RELATIVEPATH:
            setShapeRelativePath(parse<bool>(value));
            break;
        case SUMO_ATTR_WIDTH:
            // set new width
            setWidth(parse<double>(value));
            // update centering boundary
            updateCenteringBoundary(true);
            break;
        case SUMO_ATTR_HEIGHT:
            // set new height
            setHeight(parse<double>(value));
            // update centering boundary
            updateCenteringBoundary(true);
            break;
        case SUMO_ATTR_ANGLE:
            setShapeNaviDegree(parse<double>(value));
            break;
        case GNE_ATTR_BLOCK_MOVEMENT:
            myBlockMovement = parse<bool>(value);
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
        default:
            throw InvalidArgument(getTagStr() + " attribute '" + toString(key) + "' not allowed");
    }
}


void
GNEPOI::setMoveShape(const GNEMoveResult& moveResult) {
    // set geometry
    if (getTagProperty().getTag() == SUMO_TAG_POILANE) {
        myPosOverLane = moveResult.shapeToUpdate.front().x();
    } else {
        set(moveResult.shapeToUpdate.front());
    }
    updateGeometry();
}


void
GNEPOI::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    undoList->p_begin("position of " + getTagStr());
    if (getTagProperty().getTag() == SUMO_TAG_POILANE) {
        undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(moveResult.shapeToUpdate.front().x())));
    } else {
        undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(moveResult.shapeToUpdate.front())));
    }
    undoList->p_end();
}

/****************************************************************************/
