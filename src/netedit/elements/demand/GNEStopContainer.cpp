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
/// @file    GNEStopContainer.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2021
///
// Representation of container stops in NETEDIT
/****************************************************************************/
#include <cmath>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/changes/GNEChange_EnableAttribute.h>
#include <netedit/frames/common/GNEMoveFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/vehicle/SUMORouteHandler.h>

#include "GNEStopContainer.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEStopContainer::GNEStopContainer(SumoXMLTag tag, GNENet* net) :
    GNEDemandElement("", net, GLO_STOP_CONTAINER, tag, GNEPathManager::PathElement::Options::DEMAND_ELEMENT,
        {}, {}, {}, {}, {}, {}, {}, {}) {
    // reset default values
    resetDefaultValues();
}


GNEStopContainer::GNEStopContainer(GNENet* net, GNEDemandElement* containerParent, GNEAdditional* stoppingPlace, const SUMOVehicleParameter::Stop& stopParameter) :
    GNEDemandElement(containerParent, net, GLO_STOP_CONTAINER, GNE_TAG_STOPCONTAINER_CONTAINERSTOP, GNEPathManager::PathElement::Options::DEMAND_ELEMENT,
        {}, {}, {}, {stoppingPlace}, {}, {}, {containerParent}, {}),
    SUMOVehicleParameter::Stop(stopParameter) {
}


GNEStopContainer::GNEStopContainer(GNENet* net, GNEDemandElement* containerParent, GNEEdge* edge, const SUMOVehicleParameter::Stop& stopParameter) :
    GNEDemandElement(containerParent, net, GLO_STOP_CONTAINER, GNE_TAG_STOPCONTAINER_EDGE, GNEPathManager::PathElement::Options::DEMAND_ELEMENT,
        {}, {edge}, {}, {}, {}, {}, {containerParent}, {}),
    SUMOVehicleParameter::Stop(stopParameter) {
}


GNEStopContainer::~GNEStopContainer() {}


GNEMoveOperation*
GNEStopContainer::getMoveOperation() {
    if (myTagProperty.getTag() == GNE_TAG_STOPCONTAINER_EDGE) {
        // return move operation for additional placed over shape
        return new GNEMoveOperation(this, getParentEdges().front()->getLanes().front(), endPos,
                                    myNet->getViewNet()->getViewParent()->getMoveFrame()->getCommonModeOptions()->getAllowChangeLane());
    } else {
        return nullptr;
    }
}


std::string
GNEStopContainer::getBegin() const {
    return "";
}


void
GNEStopContainer::writeDemandElement(OutputDevice& device) const {
    write(device);
}


bool
GNEStopContainer::isDemandElementValid() const {
    // get lane
    const GNELane* firstLane = getFirstAllowedLane();
    // only Stops placed over lanes can be invalid
    if (myTagProperty.getTag() != SUMO_TAG_STOP_LANE) {
        return true;
    } else if (friendlyPos) {
        // with friendly position enabled position are "always fixed"
        return true;
    } else if (firstLane != nullptr) {
        // obtain lane length
        const double laneLength = getParentEdges().front()->getNBEdge()->getFinalLength() * firstLane->getLengthGeometryFactor();
        // declare end pos fixed
        const double endPosFixed = (endPos < 0) ? (endPos + laneLength) : endPos;
        // check values
        return (endPosFixed <= getParentEdges().front()->getNBEdge()->getFinalLength()) && (endPosFixed > 0);
    } else {
        return false;
    }
}


std::string
GNEStopContainer::getDemandElementProblem() const {
    if (friendlyPos) {
        return "";
    } else {
        // obtain lane length
        const double laneLength = getParentEdges().front()->getNBEdge()->getFinalLength();
        // declare end pos fixed
        const double endPosFixed = (endPos < 0) ? (endPos + laneLength) : endPos;
        // check positions over lane
        if (endPosFixed < 0) {
            return (toString(SUMO_ATTR_ENDPOS) + " < 0");
        } else if (endPosFixed > getParentEdges().front()->getNBEdge()->getFinalLength()) {
            return (toString(SUMO_ATTR_ENDPOS) + " > lanes's length");
        } else {
            return "";
        }
    }
}


void
GNEStopContainer::fixDemandElementProblem() {
    //
}


SUMOVehicleClass
GNEStopContainer::getVClass() const {
    return getParentDemandElements().front()->getVClass();
}


const RGBColor&
GNEStopContainer::getColor() const {
    return myNet->getViewNet()->getVisualisationSettings().colorSettings.stopContainerColor;
}


void
GNEStopContainer::updateGeometry() {
    // only update Stops over edges
    if (getParentAdditionals().size() > 0) {
        // get stopContainer shape
        const PositionVector& stopContainerShape = getParentAdditionals().front()->getAdditionalGeometry().getShape();
        // update demand element geometry using both positions
        myDemandElementGeometry.updateGeometry(stopContainerShape, stopContainerShape.length2D() - 0.6, stopContainerShape.length2D(), 0);
    } else {
        // get front and back lane
        const GNELane* frontLane = getParentEdges().front()->getLanes().front();
        const GNELane* backLane = getParentEdges().front()->getLanes().back();
        // get lane drawing constants
        GNELane::LaneDrawingConstants laneDrawingConstantsFront(myNet->getViewNet()->getVisualisationSettings(), frontLane);
        GNELane::LaneDrawingConstants laneDrawingConstantBack(myNet->getViewNet()->getVisualisationSettings(), backLane);
        // calculate front position
        const Position frontPosition = frontLane->getLaneShape().positionAtOffset2D(getAttributeDouble(SUMO_ATTR_ARRIVALPOS), laneDrawingConstantsFront.halfWidth);
        // calulate length between both shapes
        const double length = backLane->getLaneShape().distance2D(frontPosition, true);
        // calculate back position
        const Position backPosition = frontLane->getLaneShape().positionAtOffset2D(getAttributeDouble(SUMO_ATTR_ARRIVALPOS), (length + laneDrawingConstantBack.halfWidth - laneDrawingConstantsFront.halfWidth) * -1);
        // update demand element geometry using both positions
        myDemandElementGeometry.updateGeometry({frontPosition, backPosition});
    }
}


Position
GNEStopContainer::getPositionInView() const {
    // check if is placed over a stopContainer
    if (getParentAdditionals().size() > 0) {
        return getParentAdditionals().front()->getPositionInView();
    } else {
        // get lane
        const GNELane* containerLane = getParentEdges().front()->getLaneByAllowedVClass(SVC_PEDESTRIAN);
        // get position over lane shape
        if (endPos <= 0) {
            return containerLane->getLaneShape().front();
        } else if (endPos >= containerLane->getLaneShape().length2D()) {
            return containerLane->getLaneShape().back();
        } else {
            return containerLane->getLaneShape().positionAtOffset2D(endPos);
        }
    }
}


std::string
GNEStopContainer::getParentName() const {
    // get lane
    const GNELane* const firstLane = getFirstAllowedLane();
    if (getParentDemandElements().size() > 0) {
        return getParentDemandElements().front()->getID();
    } else if (getParentAdditionals().size() > 0) {
        return getParentAdditionals().front()->getID();
    } else if (firstLane != nullptr) {
        return firstLane->getID();
    } else {
        throw ProcessError("Invalid parent");
    }
}


double
GNEStopContainer::getExaggeration(const GUIVisualizationSettings& s) const {
    return s.addSize.getExaggeration(s, this);
}


Boundary
GNEStopContainer::getCenteringBoundary() const {
    // Return Boundary depending if myMovingGeometryBoundary is initialised (important for move geometry)
    if (getParentAdditionals().size() > 0) {
        return getParentAdditionals().at(0)->getCenteringBoundary();
    } else {
        Boundary b;
        b.add(getPositionInView());
        b.grow(20);
        return b;
    }
}


void
GNEStopContainer::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


void
GNEStopContainer::drawGL(const GUIVisualizationSettings& s) const {
    // check if stop can be drawn
    if (drawPersonPlan()) {
        // Obtain exaggeration of the draw
        const double exaggeration = getExaggeration(s);
        // declare stop color
        const RGBColor stopColor = drawUsingSelectColor() ? s.colorSettings.selectedContainerPlanColor : s.colorSettings.stopColor;
        // Start drawing adding an gl identificator
        GLHelper::pushName(getGlID());
        // Add layer matrix matrix
        GLHelper::pushMatrix();
        // translate to front
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType());
        // check if draw stopContainer over stopContainer oder over lane
        if (getParentAdditionals().size() > 0) {
            drawStopContainerOverStopContainer(s, exaggeration, stopColor);
        } else {
            drawStopContainerOverLane(s, exaggeration, stopColor);
        }
        // pop layer matrix
        GLHelper::popMatrix();
        // Pop name
        GLHelper::popName();
        // draw lock icon
        GNEViewNetHelper::LockIcon::drawLockIcon(this, getType(), getPositionInView(), exaggeration);
        // check if dotted contours has to be drawn
        if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
            GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::INSPECT, s, myDemandElementGeometry.getShape(), 0.3,
                    exaggeration, true, true);
        }
        if (myNet->getViewNet()->getFrontAttributeCarrier() == this) {
            GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::FRONT, s, myDemandElementGeometry.getShape(), 0.3,
                    exaggeration, true, true);
        }
        // draw container parent if this stop if their first container plan child
        if ((getParentDemandElements().size() == 1) && getParentDemandElements().front()->getChildDemandElements().front() == this) {
            getParentDemandElements().front()->drawGL(s);
        }
    }
}


void
GNEStopContainer::computePathElement() {
    // only update geometry
    updateGeometry();
}


void
GNEStopContainer::drawPartialGL(const GUIVisualizationSettings& /*s*/, const GNELane* /*lane*/, const GNEPathManager::Segment* /*segment*/, const double /*offsetFront*/) const {
    // stopContainers don't use drawPartialGL
}


void
GNEStopContainer::drawPartialGL(const GUIVisualizationSettings& /*s*/, const GNELane* /*fromLane*/, const GNELane* /*toLane*/, const GNEPathManager::Segment* /*segment*/, const double /*offsetFront*/) const {
    // stopContainers don't use drawPartialGL
}


GNELane*
GNEStopContainer::getFirstPathLane() const {
    // check if stop is placed over a stopContainer
    if (getParentAdditionals().size() > 0) {
        return getParentAdditionals().front()->getParentLanes().front();
    } else {
        return getParentEdges().front()->getLaneByAllowedVClass(SVC_PEDESTRIAN);
    }
}


GNELane*
GNEStopContainer::getLastPathLane() const {
    // first and last path lane are the same
    return getFirstPathLane();
}


std::string
GNEStopContainer::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getParentDemandElements().front()->getID();
        case SUMO_ATTR_DURATION:
            if (parametersSet & STOP_DURATION_SET) {
                return time2string(duration);
            } else {
                return "";
            }
        case SUMO_ATTR_UNTIL:
            if (parametersSet & STOP_UNTIL_SET) {
                return time2string(until);
            } else {
                return "";
            }
        case SUMO_ATTR_ACTTYPE:
            return actType;
        // specific of Stops over stoppingPlaces
        case SUMO_ATTR_CONTAINER_STOP:
            return getParentAdditionals().front()->getID();
        // specific of stops over edges/lanes
        case SUMO_ATTR_EDGE:
            return getParentEdges().front()->getID();
        case SUMO_ATTR_ENDPOS:
            return toString(endPos);
        case SUMO_ATTR_FRIENDLY_POS:
            return toString(friendlyPos);
        //
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARENT:
            return getParentDemandElements().front()->getID();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a attribute of type '" + toString(key) + "'");
    }
}


double
GNEStopContainer::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        // we use SUMO_ATTR_ARRIVALPOS instead SUMO_ATTR_ENDPOS due it's a container plan
        case SUMO_ATTR_ARRIVALPOS:
            if (getParentAdditionals().size() > 0) {
                return getParentAdditionals().front()->getAttributeDouble(SUMO_ATTR_ENDPOS);
            } else {
                return endPos;
            }
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


Position
GNEStopContainer::getAttributePosition(SumoXMLAttr key) const {
    switch (key) {
        // we use SUMO_ATTR_ARRIVALPOS instead SUMO_ATTR_ENDPOS due it's a container plan
        case SUMO_ATTR_ARRIVALPOS: {
            if (getParentAdditionals().size() > 0) {
                // return first position of stopContainer
                return getParentAdditionals().front()->getAdditionalGeometry().getShape().front();
            } else {
                // get lane shape
                const PositionVector& laneShape = getLastPathLane()->getLaneShape();
                // continue depending of arrival position
                if (endPos == 0) {
                    return laneShape.front();
                } else if ((endPos == -1) || (endPos >= laneShape.length2D())) {
                    return laneShape.back();
                } else {
                    return laneShape.positionAtOffset2D(endPos);
                }
            }
        }
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a position attribute of type '" + toString(key) + "'");
    }
}


void
GNEStopContainer::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_DURATION:
        case SUMO_ATTR_UNTIL:
        case SUMO_ATTR_EXTENSION:
        case SUMO_ATTR_ACTTYPE:
        case GNE_ATTR_SELECTED:
        case SUMO_ATTR_FRIENDLY_POS:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            break;
        case SUMO_ATTR_EDGE: {
            // get next containerPlan
            GNEDemandElement* nextContainerPlan = getParentDemandElements().at(0)->getNextChildDemandElement(this);
            // continue depending of nextContainerPlan
            if (nextContainerPlan) {
                undoList->begin(myTagProperty.getGUIIcon(), "Change from attribute of next containerPlan");
                nextContainerPlan->setAttribute(SUMO_ATTR_FROM, value, undoList);
                undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
                undoList->end();
            } else {
                undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            }
            break;
        }
        case SUMO_ATTR_CONTAINER_STOP: {
            // get next container plan
            GNEDemandElement* nextContainerPlan = getParentDemandElements().at(0)->getNextChildDemandElement(this);
            // continue depending of nextContainerPlan
            if (nextContainerPlan) {
                // obtain stopContainer
                const GNEAdditional* stopContainer = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, value);
                // change from attribute using edge ID
                undoList->begin(myTagProperty.getGUIIcon(), "Change from attribute of next containerPlan");
                nextContainerPlan->setAttribute(SUMO_ATTR_FROM, stopContainer->getParentLanes().front()->getParentEdge()->getID(), undoList);
                undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
                undoList->end();
            } else {
                undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            }
            break;
        }
        case SUMO_ATTR_ENDPOS: {
            // get previous container plan
            GNEDemandElement* previousContainerPlan = getParentDemandElements().at(0)->getPreviousChildDemandElement(this);
            // check if leave presonStop connected is enabled
            if (myNet->getViewNet()->getViewParent()->getMoveFrame()->getDemandModeOptions()->getLeaveStopPersonsConnected() &&
                    previousContainerPlan && previousContainerPlan->getTagProperty().hasAttribute(SUMO_ATTR_ARRIVALPOS)) {
                // change from attribute using edge ID
                undoList->begin(myTagProperty.getGUIIcon(), "Change arrivalPos attribute of previous containerPlan");
                previousContainerPlan->setAttribute(SUMO_ATTR_ARRIVALPOS, value, undoList);
                undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
                undoList->end();
            } else {
                undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            }
            break;
        }
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEStopContainer::isValid(SumoXMLAttr key, const std::string& value) {
    // declare string error
    std::string error;
    switch (key) {
        case SUMO_ATTR_DURATION:
        case SUMO_ATTR_UNTIL:
        case SUMO_ATTR_EXTENSION:
            if (canParse<SUMOTime>(value)) {
                return parse<SUMOTime>(value) >= 0;
            } else {
                return false;
            }
        case SUMO_ATTR_ACTTYPE:
            return true;
        // specific of Stops over stoppingPlaces
        case SUMO_ATTR_CONTAINER_STOP:
            return (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_CONTAINER_STOP, value, false) != nullptr);
        // specific of stops over edges/lanes
        case SUMO_ATTR_EDGE:
            if (myNet->getAttributeCarriers()->retrieveEdge(value, false) != nullptr) {
                return true;
            } else {
                return false;
            }
        case SUMO_ATTR_ENDPOS:
            return canParse<double>(value) && fabs(parse<double>(value)) < getParentEdges().front()->getNBEdge()->getFinalLength();
        case SUMO_ATTR_FRIENDLY_POS:
            return canParse<bool>(value);
        //
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEStopContainer::enableAttribute(SumoXMLAttr key, GNEUndoList* undoList) {
    // obtain a copy of parameter sets
    int newParametersSet = parametersSet;
    // modify parametersSetCopy depending of attr
    switch (key) {
        case SUMO_ATTR_DURATION:
            newParametersSet |= STOP_DURATION_SET;
            break;
        case SUMO_ATTR_UNTIL:
            newParametersSet |= STOP_UNTIL_SET;
            break;
        default:
            break;
    }
    // add GNEChange_EnableAttribute
    undoList->add(new GNEChange_EnableAttribute(this, parametersSet, newParametersSet), true);
    // modify parametersSetCopy depending of attr
    switch (key) {
        case SUMO_ATTR_DURATION:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, myTagProperty.getAttributeProperties(key).getDefaultValue()));
            break;
        case SUMO_ATTR_UNTIL:
        case SUMO_ATTR_EXTENSION:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, myTagProperty.getAttributeProperties(key).getDefaultValue()));
            break;
        default:
            break;
    }
}


void
GNEStopContainer::disableAttribute(SumoXMLAttr key, GNEUndoList* undoList) {
    // obtain a copy of parameter sets
    int newParametersSet = parametersSet;
    // modify parametersSetCopy depending of attr
    switch (key) {
        case SUMO_ATTR_DURATION:
            newParametersSet &= ~STOP_DURATION_SET;
            break;
        case SUMO_ATTR_UNTIL:
            newParametersSet &= ~STOP_UNTIL_SET;
            break;
        case SUMO_ATTR_EXTENSION:
            newParametersSet &= ~STOP_EXTENSION_SET;
            break;
        default:
            break;
    }
    // add GNEChange_EnableAttribute
    undoList->add(new GNEChange_EnableAttribute(this, parametersSet, newParametersSet), true);
}


bool
GNEStopContainer::isAttributeEnabled(SumoXMLAttr key) const {
    if (key == SUMO_ATTR_FROM) {
        return (getParentDemandElements().at(0)->getPreviousChildDemandElement(this) == nullptr);
    } else {
        return true;
    }
}


std::string
GNEStopContainer::getPopUpID() const {
    return getTagStr();
}


std::string
GNEStopContainer::getHierarchyName() const {
    if (getParentAdditionals().size() > 0) {
        return "container stop: " + getParentAdditionals().front()->getTagStr();
    } else {
        return "container stop: lane";
    }
}


const std::map<std::string, std::string>&
GNEStopContainer::getACParametersMap() const {
    return getParametersMap();
}


const GNELane*
GNEStopContainer::getFirstAllowedLane() const {
    if (getParentEdges().empty()) {
        return nullptr;
    }
    for (const auto& pLane : getParentEdges().front()->getLanes()) {
        if (pLane->allowPedestrians()) {
            return pLane;
        }
    }
    return getParentEdges().front()->getLanes().front();
}


void
GNEStopContainer::drawStopContainerOverLane(const GUIVisualizationSettings& s, const double exaggeration, const RGBColor& stopColor) const {
    // declare central line color
    const RGBColor centralLineColor = drawUsingSelectColor() ? stopColor.changedBrightness(-32) : RGBColor::WHITE;
    // set base color
    GLHelper::setColor(stopColor);
    // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
    GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), myDemandElementGeometry, 0.3 * exaggeration);
    // move to front
    glTranslated(0, 0, .1);
    // set central color
    GLHelper::setColor(centralLineColor);
    // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
    GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), myDemandElementGeometry, 0.05 * exaggeration);
    // move to icon position and front
    glTranslated(myDemandElementGeometry.getShape().front().x(), myDemandElementGeometry.getShape().front().y(), .1);
    // rotate over lane
    GUIGeometry::rotateOverLane((myDemandElementGeometry.getShapeRotations().front() * -1) + 90);
    // move again
    glTranslated(0, s.additionalSettings.vaporizerSize * exaggeration, 0);
    // Draw icon depending of Route Probe is selected and if isn't being drawn for selecting
    if (!s.drawForRectangleSelection && s.drawDetail(s.detailSettings.laneTextures, exaggeration)) {
        // set color
        glColor3d(1, 1, 1);
        // rotate texture
        glRotated(180, 0, 0, 1);
        // draw texture
        if (drawUsingSelectColor()) {
            GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GUITexture::STOPCONTAINER_SELECTED), s.additionalSettings.vaporizerSize * exaggeration);
        } else {
            GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GUITexture::STOPCONTAINER), s.additionalSettings.vaporizerSize * exaggeration);
        }
    } else {
        // rotate
        glRotated(22.5, 0, 0, 1);
        // set stop color
        GLHelper::setColor(stopColor);
        // move matrix
        glTranslated(0, 0, 0);
        // draw filled circle
        GLHelper::drawFilledCircle(0.1 + s.additionalSettings.vaporizerSize, 8);
    }
}


void
GNEStopContainer::drawStopContainerOverStopContainer(const GUIVisualizationSettings& s, const double exaggeration, const RGBColor& stopColor) const {
    // set base color
    GLHelper::setColor(stopColor);
    // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
    GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), myDemandElementGeometry, s.stoppingPlaceSettings.containerStopWidth * exaggeration);
    // move to icon position and front
    glTranslated(myDemandElementGeometry.getShape().getLineCenter().x(), myDemandElementGeometry.getShape().getLineCenter().y(), .1);
    // rotate over lane
    GUIGeometry::rotateOverLane((myDemandElementGeometry.getShapeRotations().front() * -1) + 90);
    // move again
    glTranslated(s.stoppingPlaceSettings.containerStopWidth * exaggeration * -2, 0, 0);
    // Draw icon depending of Route Probe is selected and if isn't being drawn for selecting
    if (!s.drawForRectangleSelection && s.drawDetail(s.detailSettings.laneTextures, exaggeration)) {
        // set color
        glColor3d(1, 1, 1);
        // rotate texture
        glRotated(-90, 0, 0, 1);
        // draw texture
        if (drawUsingSelectColor()) {
            GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GUITexture::STOPCONTAINER_SELECTED), s.additionalSettings.vaporizerSize * exaggeration);
        } else {
            GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GUITexture::STOPCONTAINER), s.additionalSettings.vaporizerSize * exaggeration);
        }
    } else {
        // rotate
        glRotated(22.5, 0, 0, 1);
        // set stop color
        GLHelper::setColor(stopColor);
        // move matrix
        glTranslated(0, 0, 0);
        // draw filled circle
        GLHelper::drawFilledCircle(0.1 + s.additionalSettings.vaporizerSize, 8);
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEStopContainer::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_DURATION:
            if (value.empty()) {
                parametersSet &= ~STOP_DURATION_SET;
            } else {
                duration = string2time(value);
                parametersSet |= STOP_DURATION_SET;
            }
            break;
        case SUMO_ATTR_UNTIL:
            if (value.empty()) {
                parametersSet &= ~STOP_UNTIL_SET;
            } else {
                until = string2time(value);
                parametersSet |= STOP_UNTIL_SET;
            }
            break;
        case SUMO_ATTR_ACTTYPE:
            actType = value;
            break;
        // specific of Stops over stoppingPlaces
        case SUMO_ATTR_CONTAINER_STOP:
            replaceAdditionalParent(SUMO_TAG_CONTAINER_STOP, value);
            updateGeometry();
            break;
        // specific of Stops over edges/lanes
        case SUMO_ATTR_EDGE:
            replaceDemandParentEdges(value);
            updateGeometry();
            edge = value;
            break;
        case SUMO_ATTR_ENDPOS:
            endPos = parse<double>(value);
            updateGeometry();
            break;
        case SUMO_ATTR_FRIENDLY_POS:
            friendlyPos = parse<bool>(value);
            break;
        //
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEStopContainer::toogleAttribute(SumoXMLAttr key, const bool value) {
/*
    parametersSet = enabledAttributes;
*/
}


void
GNEStopContainer::setMoveShape(const GNEMoveResult& moveResult) {
    // change endPos
    endPos = moveResult.newFirstPos;
    // update geometry
    updateGeometry();
}


void
GNEStopContainer::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    undoList->begin(myTagProperty.getGUIIcon(), "endPos of " + getTagStr());
    // now adjust endPos position
    setAttribute(SUMO_ATTR_ENDPOS, toString(moveResult.newFirstPos), undoList);
    undoList->end();
}

/****************************************************************************/
