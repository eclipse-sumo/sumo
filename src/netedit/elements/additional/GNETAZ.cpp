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

GNETAZ::GNETAZ(const std::string& id, GNENet* net, PositionVector shape, RGBColor color, const std::string& name,
               const std::map<std::string, std::string>& parameters, bool blockMovement) :
    GNETAZElement(id, net, GLO_TAZ, SUMO_TAG_TAZ,
{}, {}, {}, {}, {}, {}, {}, {},
parameters, blockMovement),
            SUMOPolygon(id, name, color, shape, false, false, 1),
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
GNETAZ::getMoveOperation(const double shapeOffset) {
    // edit depending if shape is blocked
    if (myBlockMovement) {
        // nothing to move
        return nullptr;
    } else if (myNet->getViewNet()->getViewParent()->getMoveFrame()->getNetworkModeOptions()->getMoveWholePolygons()) {
        // move entire shape
        return new GNEMoveOperation(this, myShape);
    } else {
        // declare shape to move
        PositionVector shapeToMove = myShape;
        // first check if in the given shapeOffset there is a geometry point
        const Position positionAtOffset = shapeToMove.positionAtOffset2D(shapeOffset);
        // check if position is valid
        if (positionAtOffset == Position::INVALID) {
            return nullptr;
        } else {
            // obtain index
            const int index = myShape.indexOfClosest(positionAtOffset);
            // declare new index
            int newIndex = index;
            // get snap radius
            const double snap_radius = myNet->getViewNet()->getVisualisationSettings().neteditSizeSettings.polygonGeometryPointRadius;
            // check if we have to create a new index
            if (positionAtOffset.distanceSquaredTo2D(shapeToMove[index]) > (snap_radius * snap_radius)) {
                newIndex = shapeToMove.insertAtClosest(positionAtOffset, true);
            }
            // get last index
            const int lastIndex = ((int)shapeToMove.size() - 1);
            // return move operation for edit shape
            if (myShape.isClosed() && ((index == 0) || (index == lastIndex))) {
                return new GNEMoveOperation(this, myShape, {0, lastIndex}, shapeToMove, {0, lastIndex});
            } else {
                return new GNEMoveOperation(this, myShape, {index}, shapeToMove, {newIndex});
            }
        }
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
            undoList->p_begin("remove geometry point of " + getTagStr());
            undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_SHAPE, toString(shape)));
            undoList->p_end();
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
}


Position
GNETAZ::getPositionInView() const {
    return myShape.getCentroid();
}


Boundary
GNETAZ::getCenteringBoundary() const {
    // Return Boundary depending if myMovingGeometryBoundary is initialised (important for move geometry)
    if (myMovingGeometryBoundary.isInitialised()) {
        return myMovingGeometryBoundary;
    } else if (myShape.size() > 0) {
        Boundary b = myShape.getBoxBoundary();
        b.grow(20);
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
    if (myNet->getViewNet()->getDemandViewOptions().showShapes() &&
            myNet->getViewNet()->getDataViewOptions().showShapes() &&
            GUIPolygon::checkDraw(s, this, this)) {
        // Obtain constants
        const double TAZExaggeration = s.polySize.getExaggeration(s, (GNETAZElement*)this);
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
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_TAZ);
        // check if we're drawing a polygon or a polyline
        if (getFill()) {
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
                GUIPolygon::drawInnerPolygon(s, this, this, myTAZGeometry.getShape(), 0, drawUsingSelectColor());
            }
        } else {
            // push matrix
            GLHelper::pushMatrix();
            // set color
            GLHelper::setColor(color);
            // draw geometry (polyline)
            GNEGeometry::drawGeometry(myNet->getViewNet(), myTAZGeometry, s.neteditSizeSettings.polylineWidth * TAZExaggeration);
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
            GNEGeometry::drawGeometry(myNet->getViewNet(), myTAZGeometry, s.neteditSizeSettings.polygonContourWidth * TAZExaggeration);
            // pop contour matrix
            GLHelper::popMatrix();
            // draw shape points only in Network supemode
            if (s.drawMovingGeometryPoint(TAZExaggeration, s.neteditSizeSettings.polygonGeometryPointRadius) && myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) {
                // draw geometry points
                GNEGeometry::drawGeometryPoints(s, myNet->getViewNet(), myTAZGeometry.getShape(), darkerColor, invertedColor, s.neteditSizeSettings.polygonGeometryPointRadius, TAZExaggeration);
                // draw moving hint points
                if (myBlockMovement == false) {
                    GNEGeometry::drawMovingHint(s, myNet->getViewNet(), myTAZGeometry.getShape(), invertedColor, s.neteditSizeSettings.polygonGeometryPointRadius, TAZExaggeration);
                }
            }
        }
        // check if dotted contour has to be drawn
        if (s.drawDottedContour() || myNet->getViewNet()->isAttributeCarrierInspected(this)) {
            // draw depending if is closed
            if (getFill() || myTAZGeometry.getShape().isClosed()) {
                GNEGeometry::drawDottedContourClosedShape(GNEGeometry::DottedContourType::INSPECT, s, myTAZGeometry.getShape(), 1);
            } else {
                GNEGeometry::drawDottedContourShape(GNEGeometry::DottedContourType::INSPECT, s, myTAZGeometry.getShape(), s.neteditSizeSettings.polylineWidth, TAZExaggeration);
            }
        }
        // check if front dotted contour has to be drawn
        if (s.drawDottedContour() || (myNet->getViewNet()->getFrontAttributeCarrier() == this)) {
            // draw depending if is closed
            if (getFill() || myTAZGeometry.getShape().isClosed()) {
                GNEGeometry::drawDottedContourClosedShape(GNEGeometry::DottedContourType::FRONT, s, myTAZGeometry.getShape(), 1);
            } else {
                GNEGeometry::drawDottedContourShape(GNEGeometry::DottedContourType::FRONT, s, myTAZGeometry.getShape(), s.neteditSizeSettings.polylineWidth, TAZExaggeration);
            }
        }
        // pop layer matrix
        GLHelper::popMatrix();
        // pop name
        GLHelper::popName();
        /* temporal */
        // draw TAZRel datas
        for (const auto& TAZRel : getChildGenericDatas()) {
            // only draw for the first TAZ
            if ((TAZRel->getTagProperty().getTag() == SUMO_TAG_TAZREL) && (TAZRel->getParentTAZElements().front() == this)) {
                // push name (needed for getGUIGlObjectsUnderCursor(...)
                GLHelper::pushName(TAZRel->getGlID());
                // push matrix
                GLHelper::pushMatrix();
                // set custom line width
                glLineWidth(3);
                GLHelper::setColor(TAZRel->getColor());
                // draw line between two TAZs
                GLHelper::drawLine(TAZRel->getParentTAZElements().front()->getPositionInView(), TAZRel->getParentTAZElements().back()->getPositionInView());
                //restore line width
                glLineWidth(1);
                // pop matrix
                GLHelper::popMatrix();
                // pop name
                GLHelper::popName();
            }
        }
        // get name position
        const Position& namePos = myTAZGeometry.getShape().getPolygonCenter();
        // draw name
        drawName(namePos, s.scale, s.polyName, s.angle);
        // check if draw poly type
        if (s.polyType.show) {
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
        case SUMO_ATTR_COLOR:
            return toString(getShapeColor());
        case SUMO_ATTR_NAME:
            return getShapeName();
        case SUMO_ATTR_FILL:
            return toString(myFill);
        case SUMO_ATTR_EDGES: {
            std::vector<std::string> edgeIDs;
            for (auto i : getChildAdditionals()) {
                edgeIDs.push_back(i->getAttribute(SUMO_ATTR_EDGE));
            }
            return toString(edgeIDs);
        }
        case GNE_ATTR_BLOCK_MOVEMENT:
            return toString(myBlockMovement);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return GNETAZElement::getParametersStr();
        case GNE_ATTR_MIN_SOURCE:
            return toString(myMinWeightSource);
        case GNE_ATTR_MIN_SINK:
            return toString(myMinWeightSink);
        case GNE_ATTR_MAX_SOURCE:
            return toString(myMaxWeightSource);
        case GNE_ATTR_MAX_SINK:
            return toString(myMaxWeightSink);
        case GNE_ATTR_AVERAGE_SOURCE:
            return toString(myAverageWeightSource);
        case GNE_ATTR_AVERAGE_SINK:
            return toString(myAverageWeightSink);
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


void
GNETAZ::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_SHAPE:
        case SUMO_ATTR_COLOR:
        case SUMO_ATTR_NAME:
        case SUMO_ATTR_FILL:
        case SUMO_ATTR_EDGES:
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
GNETAZ::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return isValidTAZElementID(value);
        case SUMO_ATTR_SHAPE:
            return canParse<PositionVector>(value);
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
GNETAZ::updateParentAdditional() {
    // reset all stadistic variables
    myMaxWeightSource = 0;
    myMinWeightSource = -1;
    myAverageWeightSource = 0;
    myMaxWeightSink = 0;
    myMinWeightSink = -1;
    myAverageWeightSink = 0;
    // declare an extra variables for saving number of children
    int numberOfSources = 0;
    int numberOfSinks = 0;
    // iterate over child additional
    for (const auto& additional : getChildAdditionals()) {
        if (additional->getTagProperty().getTag() == SUMO_TAG_TAZSOURCE) {
            double weight = additional->getAttributeDouble(SUMO_ATTR_WEIGHT);
            // check max Weight
            if (myMaxWeightSource < weight) {
                myMaxWeightSource = weight;
            }
            // check min Weight
            if ((myMinWeightSource == -1) || (weight < myMinWeightSource)) {
                myMinWeightSource = weight;
            }
            // update Average
            myAverageWeightSource += weight;
            // update number of sources
            numberOfSources++;
        } else if (additional->getTagProperty().getTag() == SUMO_TAG_TAZSINK) {
            double weight = additional->getAttributeDouble(SUMO_ATTR_WEIGHT);
            // check max Weight
            if (myMaxWeightSink < weight) {
                myMaxWeightSink = weight;
            }
            // check min Weight
            if ((myMinWeightSink == -1) || (weight < myMinWeightSink)) {
                myMinWeightSink = weight;
            }
            // update Average
            myAverageWeightSink += weight;
            // update number of sinks
            numberOfSinks++;
        }
    }
    // calculate average
    myAverageWeightSource /= numberOfSources;
    myAverageWeightSink /= numberOfSinks;
}

// ===========================================================================
// private
// ===========================================================================

void
GNETAZ::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            myNet->getAttributeCarriers()->updateID(this, value);
            break;
        case SUMO_ATTR_SHAPE:
            myNet->removeGLObjectFromGrid(this);
            myShape = parse<PositionVector>(value);
            // always close shape
            if ((myShape.size() > 1) && (myShape.front() != myShape.back())) {
                myShape.push_back(myShape.front());
            }
            myNet->addGLObjectIntoGrid(this);
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
            GNETAZElement::setParametersStr(value);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}



void
GNETAZ::setMoveShape(const GNEMoveResult& moveResult) {
    // update new shape
    myShape = moveResult.shapeToUpdate;
    // update geometry
    myTAZGeometry.updateGeometry(myShape);
}


void
GNETAZ::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    // commit new shape
    undoList->p_begin("moving " + toString(SUMO_ATTR_SHAPE) + " of " + getTagStr());
    undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_SHAPE, toString(moveResult.shapeToUpdate)));
    undoList->p_end();
}

/****************************************************************************/
