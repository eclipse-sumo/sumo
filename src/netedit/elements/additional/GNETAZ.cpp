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
#include <utils/gui/div/GUIGlobalPostDrawing.h>
#include <utils/gui/div/GUIGlobalPostDrawing.h>

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
    GNEAdditional("", net, GLO_TAZ, SUMO_TAG_TAZ, GUIIconSubSys::getIcon(GUIIcon::TAZ), "", {}, {}, {}, {}, {}, {}),
              TesselatedPolygon("", "", RGBColor::BLACK, {}, false, false, 1, Shape::DEFAULT_LAYER, Shape::DEFAULT_ANGLE, Shape::DEFAULT_IMG_FILE, Shape::DEFAULT_RELATIVEPATH, ""),
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
               const RGBColor& color, const std::string& name, const Parameterised::Map& parameters) :
    GNEAdditional(id, net, GLO_TAZ, SUMO_TAG_TAZ, GUIIconSubSys::getIcon(GUIIcon::TAZ), "", {}, {}, {}, {}, {}, {}),
TesselatedPolygon(id, "", color, shape, false, fill, 1, Shape::DEFAULT_LAYER, Shape::DEFAULT_ANGLE, Shape::DEFAULT_IMG_FILE, Shape::DEFAULT_RELATIVEPATH, name, parameters),
myTAZCenter(center),
myMaxWeightSource(0),
myMinWeightSource(0),
myAverageWeightSource(0),
myMaxWeightSink(0),
myMinWeightSink(0),
myAverageWeightSink(0) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
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


void
GNETAZ::writeAdditional(OutputDevice& device) const {
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
    // sort all Source/Sinks by ID
    std::map<std::pair<std::string, SumoXMLTag>, GNEAdditional*> sortedSourceSinks;
    for (const auto& sourceSink : getChildAdditionals()) {
        sortedSourceSinks[std::make_pair(sourceSink->getAttribute(SUMO_ATTR_EDGE), sourceSink->getTagProperty().getTag())] = sourceSink;
    }
    // write all TAZ Source/sinks
    for (const auto& sortedSourceSink : sortedSourceSinks) {
        sortedSourceSink.second->writeAdditional(device);
    }
    // write params
    writeParams(device);
    // close TAZ tag
    device.closeTag();
}


bool
GNETAZ::isAdditionalValid() const {
    return true;
}


std::string
GNETAZ::getAdditionalProblem() const {
    return "";
}


void
GNETAZ::fixAdditionalProblem() {
    // nothing to fix
}


void
GNETAZ::updateGeometry() {
    // just update geometry
    myAdditionalGeometry.updateGeometry(myShape);
    // update geometry of TAZRelDatas
    for (const auto& TAZRelData : getChildGenericDatas()) {
        TAZRelData->updateGeometry();
    }
    myTesselation.clear();
}


Position
GNETAZ::getPositionInView() const {
    return myShape.getCentroid();
}


double
GNETAZ::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.polySize.getExaggeration(s, this);
}


void
GNETAZ::updateCenteringBoundary(const bool updateGrid) {
    // Remove object from net
    if (updateGrid) {
        myNet->removeGLObjectFromGrid(this);
        for (const auto& TAZRelData : getChildGenericDatas()) {
            myNet->removeGLObjectFromGrid(TAZRelData);
        }
    }
    // use shape as boundary
    myAdditionalBoundary = myShape.getBoxBoundary();
    // add center
    if (myTAZCenter != Position::INVALID) {
        myAdditionalBoundary.add(myTAZCenter);
    }
    // grow boundary
    myAdditionalBoundary.grow(10);
    // add object into net
    if (updateGrid) {
        myNet->addGLObjectIntoGrid(this);
        for (const auto& TAZRelData : getChildGenericDatas()) {
            TAZRelData->updateGeometry();
            myNet->addGLObjectIntoGrid(TAZRelData);
        }
    }
}


void
GNETAZ::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // Nothing to split
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
            GUIDesigns::buildFXMenuCommand(ret, TL("Set custom Geometry Point"), nullptr, &parent, MID_GNE_CUSTOM_GEOMETRYPOINT);
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
        const bool drawFill = (myNet->getViewNet()->getEditModes().isCurrentSupermodeData() && myNet->getViewNet()->getDataViewOptions().TAZDrawFill()) ? true : getFill();
        // get colors
        const RGBColor color = GUIPolygon::setColor(s, this, this, drawUsingSelectColor(), -1);
        const RGBColor invertedColor = color.invertedColor();
        const RGBColor darkerColor = color.changedBrightness(-32);
        // avoid draw invisible elements
        if (color.alpha() != 0) {
            // push name (needed for getGUIGlObjectsUnderCursor(...)
            GLHelper::pushName(GNEAdditional::getGlID());
            // push layer matrix
            GLHelper::pushMatrix();
            // translate to front
            myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getShapeLayer());
            // check if we're drawing a polygon or a polyline
            if (getFill() || myNet->getViewNet()->getDataViewOptions().TAZDrawFill()) {
                if (s.drawForPositionSelection) {
                    // check if mouse is within geometry
                    if (myAdditionalGeometry.getShape().around(mousePosition)) {
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
                    GUIPolygon::drawInnerPolygon(s, this, this, myAdditionalGeometry.getShape(), 0, drawFill, drawUsingSelectColor(), alphaOverride, true);
                }
            } else {
                // push matrix
                GLHelper::pushMatrix();
                // set color
                GLHelper::setColor(color);
                // draw geometry (polyline)
                GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), myAdditionalGeometry, s.neteditSizeSettings.polylineWidth * TAZExaggeration);
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
                GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), myAdditionalGeometry, s.neteditSizeSettings.polygonContourWidth * TAZExaggeration);
                // pop contour matrix
                GLHelper::popMatrix();
                // draw shape points only in Network supemode
                if (s.drawMovingGeometryPoint(TAZExaggeration, s.neteditSizeSettings.polygonGeometryPointRadius) && myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) {
                    // check move mode flag
                    const bool moveMode = (myNet->getViewNet()->getEditModes().networkEditMode == NetworkEditMode::NETWORK_MOVE);
                    // draw geometry points
                    GUIGeometry::drawGeometryPoints(s, myNet->getViewNet()->getPositionInformation(), myAdditionalGeometry.getShape(), darkerColor, invertedColor,
                                                    s.neteditSizeSettings.polygonGeometryPointRadius * (moveMode ? 1 : 0.5), TAZExaggeration,
                                                    myNet->getViewNet()->getNetworkViewOptions().editingElevation(), drawExtremeSymbols);
                    // draw moving hint points
                    if (!myNet->getViewNet()->getLockManager().isObjectLocked(GLO_TAZ, isAttributeCarrierSelected()) && moveMode) {
                        GUIGeometry::drawMovingHint(s, myNet->getViewNet()->getPositionInformation(), myAdditionalGeometry.getShape(), invertedColor,
                                                    s.neteditSizeSettings.polygonGeometryPointRadius, TAZExaggeration);
                    }
                }
            }
            // draw center
            const double centerRadius = s.neteditSizeSettings.polygonGeometryPointRadius * TAZExaggeration;
            // push center matrix
            GLHelper::pushMatrix();
            // move to vertex
            glTranslated(myTAZCenter.x(), myTAZCenter.y(), GLO_JUNCTION + 0.3);
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
            // pop layer matrix
            GLHelper::popMatrix();
            // pop name
            GLHelper::popName();
            // draw lock icon
            GNEViewNetHelper::LockIcon::drawLockIcon(this, getType(), getPositionInView(), TAZExaggeration);
        }
        // draw name
        drawName(myTAZCenter, s.scale, s.polyName, s.angle);
        // check if mouse is over element
        mouseWithinGeometry(myAdditionalGeometry.getShape());
        // draw dotted contours
        drawDottedContours(s, TAZExaggeration);
        // check if draw poly type
        if (s.polyType.show(this)) {
            const Position p = myAdditionalGeometry.getShape().getPolygonCenter() + Position(0, -0.6 * s.polyType.size / s.scale);
            GLHelper::drawTextSettings(s.polyType, getShapeType(), p, s.scale, s.angle);
        }
    }
}


std::string
GNETAZ::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
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
            return getParametersStr();
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


const Parameterised::Map&
GNETAZ::getACParametersMap() const {
    return getParametersMap();
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
                   (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TAZ, value, false) == nullptr) &&
                   (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_POLY, value, false) == nullptr);
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
            return areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
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
    for (const auto& TAZChild : getChildAdditionals()) {
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
    // get TAZRelDataFrame
    const auto TAZRelDataFrame = myNet->getViewNet()->getViewParent()->getTAZRelDataFrame();
    // dotted contour for inspect
    if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
        GUIDottedGeometry::drawDottedContourClosedShape(s, GUIDottedGeometry::DottedContourType::INSPECT, myAdditionalGeometry.getShape(), 1);
    }
    // dotted contour for front
    if (myNet->getViewNet()->getFrontAttributeCarrier() == this) {
        GUIDottedGeometry::drawDottedContourClosedShape(s, GUIDottedGeometry::DottedContourType::FRONT, myAdditionalGeometry.getShape(), 1);
    }
    // delete contour
    if (myNet->getViewNet()->drawDeleteContour(this, this)) {
        GUIDottedGeometry::drawDottedContourClosedShape(s, GUIDottedGeometry::DottedContourType::REMOVE, myAdditionalGeometry.getShape(), 1);
    }
    // select contour
    if (myNet->getViewNet()->drawSelectContour(this, this)) {
        GUIDottedGeometry::drawDottedContourClosedShape(s, GUIDottedGeometry::DottedContourType::SELECT, myAdditionalGeometry.getShape(), 1);
    }
    // dotted contour for first TAZ
    if ((myNet->getViewNet()->getViewParent()->getTAZRelDataFrame()->getFirstTAZ() == this)) {
        GUIDottedGeometry::drawDottedContourClosedShape(s, GUIDottedGeometry::DottedContourType::FROMTAZ, myAdditionalGeometry.getShape(), 1, s.neteditSizeSettings.polylineWidth * TAZExaggeration);
    }
    // dotted contour for second TAZ
    if ((myNet->getViewNet()->getViewParent()->getTAZRelDataFrame()->getSecondTAZ() == this)) {
        GUIDottedGeometry::drawDottedContourClosedShape(s, GUIDottedGeometry::DottedContourType::TOTAZ, myAdditionalGeometry.getShape(), 1, s.neteditSizeSettings.polylineWidth * TAZExaggeration);
    }
    // now check if mouse is over TAZ
    if (TAZRelDataFrame->shown() && (gPostDrawing.markedTAZ == nullptr) && ((TAZRelDataFrame->getFirstTAZ() == nullptr) || (TAZRelDataFrame->getSecondTAZ() == nullptr))) {
        // get dotted contour type
        const auto dottedContourType = (TAZRelDataFrame->getFirstTAZ() == nullptr) ? GUIDottedGeometry::DottedContourType::FROMTAZ : GUIDottedGeometry::DottedContourType::TOTAZ;
        // draw depending if is closed
        if (getFill() || myNet->getViewNet()->getDataViewOptions().TAZDrawFill()) {
            if (myAdditionalGeometry.getShape().around(myNet->getViewNet()->getPositionInformation())) {
                GUIDottedGeometry::drawDottedContourClosedShape(s, dottedContourType, myAdditionalGeometry.getShape(), 1);
            }
        } else {
            // scale shape
            auto scaledShape = myAdditionalGeometry.getShape();
            scaledShape.scaleAbsolute(1);
            // check if mouse is around scaled shape
            if ((scaledShape.around(myNet->getViewNet()->getPositionInformation()) && (scaledShape.distance2D(myNet->getViewNet()->getPositionInformation()) <= 1.3)) ||
                    (myAdditionalGeometry.getShape().around(myNet->getViewNet()->getPositionInformation()) && (myAdditionalGeometry.getShape().distance2D(myNet->getViewNet()->getPositionInformation()) <= 1))) {
                GUIDottedGeometry::drawDottedContourClosedShape(s, dottedContourType, myAdditionalGeometry.getShape(), 1);
            }
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
            // set new shape
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
            // update geometry
            updateGeometry();
            // update centering boundary
            if (!isTemplate()) {
                updateCenteringBoundary(true);
            }
            break;
        }
        case SUMO_ATTR_CENTER:
            if (value.empty()) {
                myTAZCenter = myShape.getCentroid();
            } else {
                myTAZCenter = parse<Position>(value);
            }
            // update geometry
            updateGeometry();
            // update centering boundary
            if (!isTemplate()) {
                updateCenteringBoundary(true);
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
            setParametersStr(value);
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
        myAdditionalGeometry.updateGeometry(myShape);
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
        myAdditionalGeometry.updateGeometry(myShape);
    }
    myTesselation.clear();
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
