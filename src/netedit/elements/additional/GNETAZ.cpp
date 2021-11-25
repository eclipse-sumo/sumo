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
/// @file    GNETAZ.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2018
///
//
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <netedit/frames/data/GNETAZRelDataFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/globjects/GUIPolygon.h>

#include "GNETAZ.h"


// ===========================================================================
// static members
// ===========================================================================
const double GNETAZ::myHintSize = 0.8;
const double GNETAZ::myHintSizeSquared = 0.64;


// ===========================================================================
// member method definitions
// ===========================================================================

GNETAZ::GNETAZ(GNENet* net) :
    GNETAZElement("", net, GLO_TAZ, SUMO_TAG_TAZ,
        {}, {}, {}, {}, {}, {}, {}, {},
    std::map<std::string, std::string>()),
    SUMOPolygon("", "", RGBColor::BLACK, {}, false, false, 1, Shape::DEFAULT_LAYER, Shape::DEFAULT_ANGLE, Shape::DEFAULT_IMG_FILE, Shape::DEFAULT_RELATIVEPATH, ""),
    myMaxWeightSource(0),
    myMinWeightSource(0),
    myAverageWeightSource(0),
    myMaxWeightSink(0),
    myMinWeightSink(0),
    myAverageWeightSink(0) {
    // reset default values
    resetDefaultValues();
}


GNETAZ::GNETAZ(const std::string& id, GNENet* net, const PositionVector& shape, const Position& center, const bool fill,
               const RGBColor& color, const std::string& name, const std::map<std::string, std::string>& parameters) :
    GNETAZElement(id, net, GLO_TAZ, SUMO_TAG_TAZ,
        {}, {}, {}, {}, {}, {}, {}, {},
    parameters),
    SUMOPolygon(id, "", color, shape, false, fill, 1, Shape::DEFAULT_LAYER, Shape::DEFAULT_ANGLE, Shape::DEFAULT_IMG_FILE, Shape::DEFAULT_RELATIVEPATH, name),
    myTAZCenter(center),
    myMaxWeightSource(0),
    myMinWeightSource(0),
    myAverageWeightSource(0),
    myMaxWeightSink(0),
    myMinWeightSink(0),
    myAverageWeightSink(0) {
    // update geometry
    updateGeometry();
}


GNETAZ::~GNETAZ() {}


GNEMoveOperation*
GNETAZ::getMoveOperation() {
    // get snap radius
    const double snap_radius = myNet->getViewNet()->getVisualisationSettings().neteditSizeSettings.polygonGeometryPointRadius;
    // check if we're moving center or shape
    if (myTAZCenter.distanceSquaredTo2D(myNet->getViewNet()->getPositionInformation()) < (snap_radius * snap_radius)) {
        // move entire shape
        return new GNEMoveOperation(this, myTAZCenter);
    } else if (myNet->getViewNet()->getViewParent()->getMoveFrame()->getNetworkModeOptions()->getMoveWholePolygons()) {
        // move entire shape
        return new GNEMoveOperation(this, myShape);
    } else {
        // calculate move shape operation
        return calculateMoveShapeOperation(myShape, myNet->getViewNet()->getPositionInformation(), snap_radius, true);
    }
}


int
GNETAZ::getVertexIndex(Position pos, bool snapToGrid) {
    // check if position has to be snapped to grid
    if (snapToGrid) {
        pos = myNet->getViewNet()->snapToActiveGrid(pos);
    }
    // first check if vertex already exists
    for (const auto& shapePosition : myShape) {
        if (shapePosition.distanceTo2D(pos) < myNet->getViewNet()->getVisualisationSettings().neteditSizeSettings.polygonGeometryPointRadius) {
            return myShape.indexOfClosest(shapePosition);
        }
    }
    return -1;
}


void
GNETAZ::removeGeometryPoint(const Position clickedPosition, GNEUndoList* undoList) {
    // get original shape
    PositionVector shape = myShape;
    // check shape size
    if (shape.size() > 3) {
        // obtain index
        int index = shape.indexOfClosest(clickedPosition);
        // get last index
        const int lastIndex = ((int)shape.size() - 1);
        // get snap radius
        const double snap_radius = myNet->getViewNet()->getVisualisationSettings().neteditSizeSettings.polygonGeometryPointRadius;
        // check if we have to create a new index
        if ((index != -1) && shape[index].distanceSquaredTo2D(clickedPosition) < (snap_radius * snap_radius)) {
            // check if we're deleting the first point
            if ((index == 0) || (index == lastIndex)) {
                // remove both geometry point
                shape.erase(shape.begin() + lastIndex);
                shape.erase(shape.begin());
                // close shape
                shape.closePolygon();
            } else {
                // remove geometry point
                shape.erase(shape.begin() + index);
            }
            // commit new shape
            undoList->begin(GUIIcon::TAZ, "remove geometry point of " + getTagStr());
            undoList->changeAttribute(new GNEChange_Attribute(this, SUMO_ATTR_SHAPE, toString(shape)));
            undoList->end();
        }
    }
}


const PositionVector&
GNETAZ::getTAZElementShape() const {
    return myShape;
}


void
GNETAZ::writeTAZElement(OutputDevice& device) const {
    // first open TAZ tag
    device.openTag(SUMO_TAG_TAZ);
    // write TAZ attributes
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_SHAPE, myShape);
    if (myTAZCenter != myShape.getCentroid()) {
        device.writeAttr(SUMO_ATTR_CENTER, myTAZCenter);
    }
    if (myFill) {
        device.writeAttr(SUMO_ATTR_FILL, true);
    }
    if (getShapeName().size() > 0) {
        device.writeAttr(SUMO_ATTR_NAME, getShapeName());
    }
    device.writeAttr(SUMO_ATTR_COLOR, getShapeColor());
    // write all TAZ Source/sinks
    for (const auto& sourceSink : getChildTAZElements()) {
        sourceSink->writeTAZElement(device);
    }
    // write params
    GNETAZElement::writeParams(device);
    // close TAZ tag
    device.closeTag();
}


void
GNETAZ::updateGeometry() {
    // just update geometry
    myTAZGeometry.updateGeometry(myShape);
    // update geometry of TAZRelDatas
    for (const auto& TAZRelData : getChildGenericDatas()) {
        TAZRelData->updateGeometry();
    }
}


Position
GNETAZ::getPositionInView() const {
    return myShape.getCentroid();
}


double
GNETAZ::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.polySize.getExaggeration(s, this);
}


Boundary
GNETAZ::getCenteringBoundary() const {
    // Return Boundary depending if myMovingGeometryBoundary is initialised (important for move geometry)
    if (myMovingGeometryBoundary.isInitialised()) {
        return myMovingGeometryBoundary;
    } else if (myShape.size() > 0) {
        Boundary b = myShape.getBoxBoundary();
        // add center
        b.add(myTAZCenter);
        b.grow(40);
        return b;
    } else {
        return Boundary(-0.1, -0.1, 0.1, 0.1);
    }
}


std::string
GNETAZ::getParentName() const {
    return myNet->getMicrosimID();
}


GUIGLObjectPopupMenu*
GNETAZ::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    // build selection and show parameters menu
    myNet->getViewNet()->buildSelectionACPopupEntry(ret, this);
    buildShowParamsPopupEntry(ret);
    // create a extra FXMenuCommand if mouse is over a vertex
    const int index = getVertexIndex(myNet->getViewNet()->getPositionInformation(), false);
    if (index != -1) {
        // check if we're in network mode
        if (myNet->getViewNet()->getEditModes().networkEditMode == NetworkEditMode::NETWORK_MOVE) {
            GUIDesigns::buildFXMenuCommand(ret, "Set custom Geometry Point", nullptr, &parent, MID_GNE_CUSTOM_GEOMETRYPOINT);
        }
    }
    return ret;
}


void
GNETAZ::drawGL(const GUIVisualizationSettings& s) const {
    // check if boundary has to be drawn
    if (s.drawBoundaries) {
        GLHelper::drawBoundary(getCenteringBoundary());
    }
    // first check if poly can be drawn
    if (myNet->getViewNet()->getDemandViewOptions().showShapes() && GUIPolygon::checkDraw(s, this, this)) {
        // check if draw start und end
        const bool drawExtremeSymbols = myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork() &&
                                        myNet->getViewNet()->getEditModes().networkEditMode == NetworkEditMode::NETWORK_MOVE;
        // Obtain constants
        const double TAZExaggeration = getExaggeration(s);
        const Position mousePosition = myNet->getViewNet()->getPositionInformation();
        // get colors
        const RGBColor color = isAttributeCarrierSelected() ? s.colorSettings.selectionColor : getShapeColor();
        const RGBColor invertedColor = color.invertedColor();
        const RGBColor darkerColor = color.changedBrightness(-32);
        // push name (needed for getGUIGlObjectsUnderCursor(...)
        GLHelper::pushName(GNETAZElement::getGlID());
        // push layer matrix
        GLHelper::pushMatrix();
        // translate to front
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getShapeLayer());
        // check if we're drawing a polygon or a polyline
        if (getFill() || myNet->getViewNet()->getDataViewOptions().TAZDrawFill()) {
            if (s.drawForPositionSelection) {
                // check if mouse is within geometry
                if (myTAZGeometry.getShape().around(mousePosition)) {
                    // push matrix
                    GLHelper::pushMatrix();
                    // move to mouse position
                    glTranslated(mousePosition.x(), mousePosition.y(), 0);
                    // set color
                    GLHelper::setColor(color);
                    // draw circle
                    GLHelper::drawFilledCircle(1, s.getCircleResolution());
                    // pop matrix
                    GLHelper::popMatrix();
                }
            } else {
                // draw inner polygon
                const int alphaOverride = myNet->getViewNet()->getDataViewOptions().TAZDrawFill() ? 128 : -1;
                GUIPolygon::drawInnerPolygon(s, this, this, myTAZGeometry.getShape(), true, 0, drawUsingSelectColor(), alphaOverride);
            }
        } else {
            // push matrix
            GLHelper::pushMatrix();
            // set color
            GLHelper::setColor(color);
            // draw geometry (polyline)
            GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), myTAZGeometry, s.neteditSizeSettings.polylineWidth * TAZExaggeration);
            // pop matrix
            GLHelper::popMatrix();
        }
        // draw contour if shape isn't blocked
        if (!myNet->getViewNet()->getViewParent()->getMoveFrame()->getNetworkModeOptions()->getMoveWholePolygons()) {
            // push contour matrix
            GLHelper::pushMatrix();
            // translate to front
            glTranslated(0, 0, 0.1);
            // set color
            GLHelper::setColor(darkerColor);
            // draw polygon contour
            GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), myTAZGeometry, s.neteditSizeSettings.polygonContourWidth * TAZExaggeration);
            // pop contour matrix
            GLHelper::popMatrix();
            // draw shape points only in Network supemode
            if (s.drawMovingGeometryPoint(TAZExaggeration, s.neteditSizeSettings.polygonGeometryPointRadius) && myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) {
                // check move mode flag
                const bool moveMode = (myNet->getViewNet()->getEditModes().networkEditMode == NetworkEditMode::NETWORK_MOVE);
                // draw geometry points
                GUIGeometry::drawGeometryPoints(s, myNet->getViewNet()->getPositionInformation(), myTAZGeometry.getShape(), darkerColor, invertedColor,
                                                s.neteditSizeSettings.polygonGeometryPointRadius * (moveMode ? 1 : 0.5), TAZExaggeration,
                                                myNet->getViewNet()->getNetworkViewOptions().editingElevation(), drawExtremeSymbols);
                // draw moving hint points
                if (!myNet->getViewNet()->getLockManager().isObjectLocked(GLO_TAZ, isAttributeCarrierSelected()) && moveMode) {
                    GUIGeometry::drawMovingHint(s, myNet->getViewNet()->getPositionInformation(), myTAZGeometry.getShape(), invertedColor,
                                                s.neteditSizeSettings.polygonGeometryPointRadius, TAZExaggeration);
                }
            }
        }
        // draw center
        const double centerRadius = s.neteditSizeSettings.polygonGeometryPointRadius * TAZExaggeration;
        // push center matrix
        GLHelper::pushMatrix();
        // move to vertex
        glTranslated(myTAZCenter.x(), myTAZCenter.y(), 0.3);
        // set color
        GLHelper::setColor(darkerColor);
        // draw circle
        GLHelper::drawFilledCircle(centerRadius, s.getCircleResolution());
        // move to front
        glTranslated(0, 0, 0.1);
        // set color
        GLHelper::setColor(color);
        // draw circle
        GLHelper::drawFilledCircle(centerRadius * 0.8, s.getCircleResolution());
        // pop center matrix
        GLHelper::popMatrix();
        // draw dotted contours
        drawDottedContours(s, TAZExaggeration);
        // pop layer matrix
        GLHelper::popMatrix();
        // pop name
        GLHelper::popName();
        // draw lock icon
        GNEViewNetHelper::LockIcon::drawLockIcon(this, getType(), getPositionInView(), TAZExaggeration);
        // get name position
        const Position& namePos = myTAZGeometry.getShape().getPolygonCenter();
        // draw name
        drawName(myTAZCenter, s.scale, s.polyName, s.angle);
        // check if draw poly type
        if (s.polyType.show(this)) {
            const Position p = namePos + Position(0, -0.6 * s.polyType.size / s.scale);
            GLHelper::drawTextSettings(s.polyType, getShapeType(), p, s.scale, s.angle);
        }
    }
}


std::string
GNETAZ::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getID();
        case SUMO_ATTR_SHAPE:
            return toString(myShape);
        case SUMO_ATTR_CENTER:
            if (myTAZCenter == myShape.getCentroid()) {
                return "";
            } else {
                return toString(myTAZCenter);
            }
        case SUMO_ATTR_COLOR:
            return toString(getShapeColor());
        case SUMO_ATTR_NAME:
            return getShapeName();
        case SUMO_ATTR_FILL:
            return toString(myFill);
        case SUMO_ATTR_EDGES: {
            std::vector<std::string> edgeIDs;
            for (const auto& TAZChild : getChildAdditionals()) {
                edgeIDs.push_back(TAZChild->getAttribute(SUMO_ATTR_EDGE));
            }
            return toString(edgeIDs);
        }
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return GNETAZElement::getParametersStr();
        case GNE_ATTR_MIN_SOURCE:
            if (myMinWeightSource == INVALID_DOUBLE) {
                return "undefined";
            } else {
                return toString(myMinWeightSource);
            }
        case GNE_ATTR_MIN_SINK:
            if (myMinWeightSink == INVALID_DOUBLE) {
                return "undefined";
            } else {
                return toString(myMinWeightSink);
            }
        case GNE_ATTR_MAX_SOURCE:
            if (myMaxWeightSource == INVALID_DOUBLE) {
                return "undefined";
            } else {
                return toString(myMaxWeightSource);
            }
        case GNE_ATTR_MAX_SINK:
            if (myMaxWeightSink == INVALID_DOUBLE) {
                return "undefined";
            } else {
                return toString(myMaxWeightSink);
            }
        case GNE_ATTR_AVERAGE_SOURCE:
            if (myAverageWeightSource == INVALID_DOUBLE) {
                return "undefined";
            } else {
                return toString(myAverageWeightSource);
            }
        case GNE_ATTR_AVERAGE_SINK:
            if (myAverageWeightSink == INVALID_DOUBLE) {
                return "undefined";
            } else {
                return toString(myAverageWeightSink);
            }
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNETAZ::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case GNE_ATTR_MIN_SOURCE:
            return myMinWeightSource;
        case GNE_ATTR_MIN_SINK:
            return myMinWeightSink;
        case GNE_ATTR_MAX_SOURCE:
            return myMaxWeightSource;
        case GNE_ATTR_MAX_SINK:
            return myMaxWeightSink;
        case GNE_ATTR_AVERAGE_SOURCE:
            return myAverageWeightSource;
        case GNE_ATTR_AVERAGE_SINK:
            return myAverageWeightSink;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
    }
}


Position
GNETAZ::getAttributePosition(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_CENTER:
            return myTAZCenter;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
    }
}


void
GNETAZ::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_SHAPE:
        case SUMO_ATTR_CENTER:
        case SUMO_ATTR_COLOR:
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_FILL:
        case SUMO_ATTR_EDGES:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNETAZ::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return SUMOXMLDefinitions::isValidAdditionalID(value) &&
                   (myNet->getAttributeCarriers()->retrieveTAZElement(SUMO_TAG_TAZ, value, false) == nullptr) &&
                   (myNet->getAttributeCarriers()->retrieveShape(SUMO_TAG_POLY, value, false) == nullptr);
        case SUMO_ATTR_SHAPE:
            if (value.empty()) {
                return false;
            } else {
                return canParse<PositionVector>(value);
            }
        case SUMO_ATTR_CENTER:
            if (value.empty()) {
                return true;
            } else {
                return canParse<Position>(value);
            }
        case SUMO_ATTR_COLOR:
            return canParse<RGBColor>(value);
        case SUMO_ATTR_NAME:
            return SUMOXMLDefinitions::isValidAttribute(value);
        case SUMO_ATTR_FILL:
            return canParse<bool>(value);
        case SUMO_ATTR_EDGES:
            if (value.empty()) {
                return true;
            } else {
                return SUMOXMLDefinitions::isValidListOfTypeID(value);
            }
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNETAZ::isAttributeEnabled(SumoXMLAttr /* key */) const {
    return true;
}


std::string
GNETAZ::getPopUpID() const {
    return getTagStr() + ":" + getID();
}


std::string
GNETAZ::getHierarchyName() const {
    return getTagStr();
}


void
GNETAZ::updateTAZStadistic() {
    // reset all stadistic variables
    myMaxWeightSource = INVALID_DOUBLE;
    myMinWeightSource = INVALID_DOUBLE;
    myAverageWeightSource = 0;
    myMaxWeightSink = INVALID_DOUBLE;
    myMinWeightSink = INVALID_DOUBLE;
    myAverageWeightSink = 0;
    // declare an extra variables for saving number of children
    int numberOfSources = 0;
    int numberOfSinks = 0;
    // iterate over child additional
    for (const auto& TAZChild : getChildTAZElements()) {
        if (TAZChild->getTagProperty().getTag() == SUMO_TAG_TAZSOURCE) {
            const double weight = TAZChild->getAttributeDouble(SUMO_ATTR_WEIGHT);
            // check max Weight
            if ((myMaxWeightSource == INVALID_DOUBLE) || (myMaxWeightSource < weight)) {
                myMaxWeightSource = weight;
            }
            // check min Weight
            if ((myMinWeightSource == INVALID_DOUBLE) || (weight < myMinWeightSource)) {
                myMinWeightSource = weight;
            }
            // update Average
            myAverageWeightSource += weight;
            // update number of sources
            numberOfSources++;
        } else if (TAZChild->getTagProperty().getTag() == SUMO_TAG_TAZSINK) {
            const double weight = TAZChild->getAttributeDouble(SUMO_ATTR_WEIGHT);
            // check max Weight
            if ((myMaxWeightSink == INVALID_DOUBLE) || myMaxWeightSink < weight) {
                myMaxWeightSink = weight;
            }
            // check min Weight
            if ((myMinWeightSink == INVALID_DOUBLE) || (weight < myMinWeightSink)) {
                myMinWeightSink = weight;
            }
            // update Average
            myAverageWeightSink += weight;
            // update number of sinks
            numberOfSinks++;
        }
    }
    // calculate average
    if (numberOfSources > 0) {
        myAverageWeightSource /= numberOfSources;
    }
    if (numberOfSinks > 0) {
        myAverageWeightSink /= numberOfSinks;
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNETAZ::drawDottedContours(const GUIVisualizationSettings& s, const double TAZExaggeration) const {
    // flag for draw fill
    const bool drawFill = getFill() || myNet->getViewNet()->getDataViewOptions().TAZDrawFill() || myTAZGeometry.getShape().isClosed();
    // dotted contour for inspect
    if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
        // draw depending if is closed
        if (drawFill) {
            GUIDottedGeometry::drawDottedContourClosedShape(GUIDottedGeometry::DottedContourType::INSPECT, s, myTAZGeometry.getShape(), 1);
        } else {
            GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::INSPECT, s, myTAZGeometry.getShape(), s.neteditSizeSettings.polylineWidth,
                    TAZExaggeration, 1, 1);
        }
    }
    // dotted contour for front
    if ((myNet->getViewNet()->getFrontAttributeCarrier() == this)) {
        // draw depending if is closed
        if (drawFill) {
            GUIDottedGeometry::drawDottedContourClosedShape(GUIDottedGeometry::DottedContourType::FRONT, s, myTAZGeometry.getShape(), 1);
        } else {
            GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::FRONT, s, myTAZGeometry.getShape(), s.neteditSizeSettings.polylineWidth,
                    TAZExaggeration, 1, 1);
        }
    }
    // dotted contour for first TAZ
    if ((myNet->getViewNet()->getViewParent()->getTAZRelDataFrame()->getFirstTAZ() == this)) {
        // draw depending if is closed
        if (drawFill) {
            GUIDottedGeometry::drawDottedContourClosedShape(GUIDottedGeometry::DottedContourType::GREEN, s, myTAZGeometry.getShape(), 1, s.neteditSizeSettings.polylineWidth * TAZExaggeration);
        } else {
            GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::GREEN, s, myTAZGeometry.getShape(), s.neteditSizeSettings.polylineWidth,
                    TAZExaggeration, 1, 1);
        }
    }
    // dotted contour for second TAZ
    if ((myNet->getViewNet()->getViewParent()->getTAZRelDataFrame()->getSecondTAZ() == this)) {
        // draw depending if is closed
        if (drawFill) {
            GUIDottedGeometry::drawDottedContourClosedShape(GUIDottedGeometry::DottedContourType::MAGENTA, s, myTAZGeometry.getShape(), 1, s.neteditSizeSettings.polylineWidth * TAZExaggeration);
        } else {
            GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::MAGENTA, s, myTAZGeometry.getShape(), s.neteditSizeSettings.polylineWidth,
                    TAZExaggeration, 1, 1);
        }
    }
}


void
GNETAZ::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            // update microsimID
            setMicrosimID(value);
            break;
        case SUMO_ATTR_SHAPE: {
            const bool updateCenter = (myTAZCenter == myShape.getCentroid());
            // remove TAZ and TAZRelDatas
            if (getID().size() > 0) {
                myNet->removeGLObjectFromGrid(this);
                for (const auto& TAZRelData : getChildGenericDatas()) {
                    myNet->removeGLObjectFromGrid(TAZRelData);
                }
            }
            myShape = parse<PositionVector>(value);
            // always close shape
            if ((myShape.size() > 1) && (myShape.front() != myShape.back())) {
                myShape.push_back(myShape.front());
            }
            // update center
            if (myShape.size() == 0) {
                myTAZCenter = Position(0, 0, 0);
            } else if (updateCenter) {
                myTAZCenter = myShape.getCentroid();
            }
            // add TAZ and TAZRelDatas
            if (getID().size() > 0) {
                myNet->addGLObjectIntoGrid(this);
                for (const auto& TAZRelData : getChildGenericDatas()) {
                    TAZRelData->updateGeometry();
                    myNet->addGLObjectIntoGrid(TAZRelData);
                }
            }
            break;
        }
        case SUMO_ATTR_CENTER:
            // remove TAZ and TAZRelDatas
            if (getID().size() > 0) {
                myNet->removeGLObjectFromGrid(this);
                for (const auto& TAZRelData : getChildGenericDatas()) {
                    myNet->removeGLObjectFromGrid(TAZRelData);
                }
            }
            if (value.empty()) {
                myTAZCenter = myShape.getCentroid();
            } else {
                myTAZCenter = parse<Position>(value);
            }
            // add TAZ and TAZRelDatas
            if (getID().size() > 0) {
                myNet->addGLObjectIntoGrid(this);
                for (const auto& TAZRelData : getChildGenericDatas()) {
                    TAZRelData->updateGeometry();
                    myNet->addGLObjectIntoGrid(TAZRelData);
                }
            }
            break;
        case SUMO_ATTR_COLOR:
            setShapeColor(parse<RGBColor>(value));
            break;
        case SUMO_ATTR_NAME:
            setShapeName(value);
            break;
        case SUMO_ATTR_FILL:
            myFill = parse<bool>(value);
            break;
        case SUMO_ATTR_EDGES:
            break;
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_PARAMETERS:
            GNETAZElement::setParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNETAZ::setMoveShape(const GNEMoveResult& moveResult) {
    if (moveResult.operationType == GNEMoveOperation::OperationType::POSITION) {
        // update new center
        myTAZCenter = moveResult.shapeToUpdate.front();
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::ENTIRE_SHAPE) {
        // update new shape and center
        myTAZCenter.add(moveResult.shapeToUpdate.getCentroid() - myShape.getCentroid());
        myShape = moveResult.shapeToUpdate;
        // update geometry
        myTAZGeometry.updateGeometry(myShape);
    } else {
        // get lastIndex
        const int lastIndex = (int)moveResult.shapeToUpdate.size() - 1;
        // update new shape
        myShape = moveResult.shapeToUpdate;
        // adjust first and last position
        if (moveResult.geometryPointsToMove.front() == 0) {
            myShape[lastIndex] = moveResult.shapeToUpdate[0];
        } else if (moveResult.geometryPointsToMove.front() == lastIndex) {
            myShape[0] = moveResult.shapeToUpdate[lastIndex];
        }
        myShape.closePolygon();
        // update geometry
        myTAZGeometry.updateGeometry(myShape);
    }
}


void
GNETAZ::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    if (moveResult.operationType == GNEMoveOperation::OperationType::POSITION) {
        // commit center
        undoList->begin(GUIIcon::TAZ, "moving " + toString(SUMO_ATTR_CENTER) + " of " + getTagStr());
        undoList->changeAttribute(new GNEChange_Attribute(this, SUMO_ATTR_CENTER, toString(moveResult.shapeToUpdate.front())));
        undoList->end();
    } else if (moveResult.operationType == GNEMoveOperation::OperationType::ENTIRE_SHAPE) {
        // calculate offset between old and new shape
        Position newCenter = myTAZCenter;
        newCenter.add(moveResult.shapeToUpdate.getCentroid() - myShape.getCentroid());
        // commit new shape and center
        undoList->begin(GUIIcon::TAZ, "moving " + toString(SUMO_ATTR_SHAPE) + " of " + getTagStr());
        undoList->changeAttribute(new GNEChange_Attribute(this, SUMO_ATTR_CENTER, toString(newCenter)));
        undoList->changeAttribute(new GNEChange_Attribute(this, SUMO_ATTR_SHAPE, toString(moveResult.shapeToUpdate)));
        undoList->end();
    } else {
        // get lastIndex
        const int lastIndex = (int)moveResult.shapeToUpdate.size() - 1;
        // close shapeToUpdate
        auto closedShape = moveResult.shapeToUpdate;
        // adjust first and last position
        if (moveResult.geometryPointsToMove.front() == 0) {
            closedShape[lastIndex] = moveResult.shapeToUpdate[0];
        } else if (moveResult.geometryPointsToMove.front() == lastIndex) {
            closedShape[0] = moveResult.shapeToUpdate[lastIndex];
        }
        // commit new shape
        undoList->begin(GUIIcon::TAZ, "moving " + toString(SUMO_ATTR_SHAPE) + " of " + getTagStr());
        undoList->changeAttribute(new GNEChange_Attribute(this, SUMO_ATTR_SHAPE, toString(closedShape)));
        undoList->end();
    }
}

/****************************************************************************/
