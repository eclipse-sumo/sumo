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
/// @file    GNEPersonStop.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2020
///
// Representation of person stops in NETEDIT
/****************************************************************************/
#include <cmath>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_EnableAttribute.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/images/GUITextureSubSys.h>
#include <utils/vehicle/SUMORouteHandler.h>

#include "GNEPersonStop.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEPersonStop::GNEPersonStop(GNENet* net, GNEDemandElement* personParent, GNEAdditional* stoppingPlace, const SUMOVehicleParameter::Stop& stopParameter) :
    GNEDemandElement(personParent, net, GLO_PERSONSTOP, GNE_TAG_PERSONSTOP_BUSSTOP,
        {}, {}, {}, {stoppingPlace}, {}, {}, {personParent}, {}),
    SUMOVehicleParameter::Stop(stopParameter) {
}


GNEPersonStop::GNEPersonStop(GNENet* net, GNEDemandElement* personParent, GNEEdge* edge, const SUMOVehicleParameter::Stop& stopParameter) :
    GNEDemandElement(personParent, net, GLO_PERSONSTOP, GNE_TAG_PERSONSTOP_EDGE,
        {}, {edge}, {}, {}, {}, {}, {personParent}, {}),
    SUMOVehicleParameter::Stop(stopParameter) {
}


GNEPersonStop::~GNEPersonStop() {}


GNEMoveOperation* 
GNEPersonStop::getMoveOperation(const double /*shapeOffset*/) {
    if (myTagProperty.getTag() == GNE_TAG_PERSONSTOP_EDGE) {
        // return move operation for additional placed over shape
        return new GNEMoveOperation(this, getParentEdges().front()->getLanes().front(), {endPos});
    }
    else {
        return nullptr;
    }
}


std::string
GNEPersonStop::getBegin() const {
    return "";
}


void
GNEPersonStop::writeDemandElement(OutputDevice& device) const {
    write(device);
}


bool
GNEPersonStop::isDemandElementValid() const {
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
        const double endPosFixed = (endPos < 0)? (endPos + laneLength) : endPos;
        // check values
        return (endPosFixed <= getParentEdges().front()->getNBEdge()->getFinalLength()) && (endPosFixed > 0);
    } else {
        return false;
    }
}


std::string
GNEPersonStop::getDemandElementProblem() const {
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
GNEPersonStop::fixDemandElementProblem() {
    //
}


SUMOVehicleClass
GNEPersonStop::getVClass() const {
    return getParentDemandElements().front()->getVClass();
}


const RGBColor&
GNEPersonStop::getColor() const {
    return myNet->getViewNet()->getVisualisationSettings().colorSettings.personStops;
}


void
GNEPersonStop::updateGeometry() {
    // only update Stops over lanes, because other uses the geometry of stopping place parent
    if (getParentEdges().size() > 0) {
        // get lanes
        const GNELane* frontLane = getParentEdges().front()->getLanes().front();
        const GNELane* backLane = getParentEdges().front()->getLanes().back();
        // get lane drawing constants
        GNELane::LaneDrawingConstants laneDrawingConstantsFront(myNet->getViewNet()->getVisualisationSettings(), frontLane);
        GNELane::LaneDrawingConstants laneDrawingConstantBack(myNet->getViewNet()->getVisualisationSettings(), backLane);
        // calculate front position
        const Position frontPosition = frontLane->getLaneShape().positionAtOffset2D(endPos, laneDrawingConstantsFront.halfWidth);
        // calulate length between both shapes
        const double length = backLane->getLaneShape().distance2D(frontPosition, true);
        // calculate back position
        const Position backPosition = frontLane->getLaneShape().positionAtOffset2D(endPos, (length + laneDrawingConstantBack.halfWidth - laneDrawingConstantsFront.halfWidth) * -1);
        // update demand element geometry using both positions
        myDemandElementGeometry.updateGeometry({frontPosition, backPosition});
    } else if (getParentAdditionals().size() > 0) {
        // update geometry using geometry of additional (busStop)
        myDemandElementGeometry.updateGeometry(getParentAdditionals().at(0));
    }
}


void
GNEPersonStop::computePath() {
    // nothing to compute
}


Position
GNEPersonStop::getPositionInView() const {
    // get lane
    const GNELane* const firstLane = getFirstAllowedLane();
    if (firstLane != nullptr) {
        if (firstLane->getLaneShape().length2D() > 1) {
            return firstLane->getLaneShape().positionAtOffset2D(1);
        } else if (firstLane->getLaneShape().size() > 0) {
            return firstLane->getLaneShape().front();
        } else {
            return Position(0, 0);
        }
    } else if (getParentDemandElements().size() > 0) {
        return getParentDemandElements().front()->getPositionInView();
    } else {
        throw ProcessError("Invalid Stop parent");
    }
}


std::string
GNEPersonStop::getParentName() const {
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


Boundary
GNEPersonStop::getCenteringBoundary() const {
    // Return Boundary depending if myMovingGeometryBoundary is initialised (important for move geometry)
    if (getParentAdditionals().size() > 0) {
        return getParentAdditionals().at(0)->getCenteringBoundary();
/*
    } else if (myMovingGeometryBoundary.isInitialised()) {
        return myMovingGeometryBoundary;
*/
    } else if (myDemandElementGeometry.getShape().size() > 0) {
        Boundary b = myDemandElementGeometry.getShape().getBoxBoundary();
        b.grow(20);
        return b;
    } else {
        return Boundary(-0.1, -0.1, 0.1, 0.1);
    }
}


void
GNEPersonStop::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


void
GNEPersonStop::drawGL(const GUIVisualizationSettings& s) const {
    // check if stop can be drawn
    if (drawPersonPlan()) {
        // Obtain exaggeration of the draw
        const double exaggeration = s.addSize.getExaggeration(s, this);
        // declare value to save stop color
        const RGBColor stopColor = drawUsingSelectColor()? s.colorSettings.selectedPersonPlanColor : s.colorSettings.stops;
        const RGBColor centralLineColor = drawUsingSelectColor() ? stopColor.changedBrightness(-32) : RGBColor::WHITE;
        // Start drawing adding an gl identificator
        glPushName(getGlID());
        // Add layer matrix matrix
        glPushMatrix();
        // translate to front
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, getType());
        // set base color
        GLHelper::setColor(stopColor);
        // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
        GNEGeometry::drawGeometry(myNet->getViewNet(), myDemandElementGeometry, 0.3 * exaggeration);
        // move to front
        glTranslated(0, 0, .1);
        // set central color
        GLHelper::setColor(centralLineColor);
        // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
        GNEGeometry::drawGeometry(myNet->getViewNet(), myDemandElementGeometry, 0.05 * exaggeration);
        // move to icon position and front
        glTranslated(myDemandElementGeometry.getShape().front().x(), myDemandElementGeometry.getShape().front().y(), .1);
        // rotate over lane
        GNEGeometry::rotateOverLane((myDemandElementGeometry.getShapeRotations().front() * -1) + 90);
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
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GUITexture::PERSONSTOP_SELECTED), s.additionalSettings.vaporizerSize * exaggeration);
            } else {
                GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GUITexture::PERSONSTOP), s.additionalSettings.vaporizerSize * exaggeration);
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
        // pop layer matrix
        glPopMatrix();
        // Pop name
        glPopName();
        // check if dotted contours has to be drawn
        if (s.drawDottedContour() || myNet->getViewNet()->isAttributeCarrierInspected(this)) {
            GNEGeometry::drawDottedContourShape(GNEGeometry::DottedContourType::INSPECT, s, myDemandElementGeometry.getShape(), 0.3, exaggeration);
        }
        if (s.drawDottedContour() || myNet->getViewNet()->getFrontAttributeCarrier() == this) {
            GNEGeometry::drawDottedContourShape(GNEGeometry::DottedContourType::FRONT, s, myDemandElementGeometry.getShape(), 0.3, exaggeration);
        }
        // draw person parent if this stop if their first person plan child
        if ((getParentDemandElements().size() == 1) && getParentDemandElements().front()->getChildDemandElements().front() == this) {
            getParentDemandElements().front()->drawGL(s);
        }
    }
}


void
GNEPersonStop::drawPartialGL(const GUIVisualizationSettings& /*s*/, const GNELane* /*lane*/, const double /*offsetFront*/, const int /*options*/) const {
    // personStops don't use drawPartialGL
}


void
GNEPersonStop::drawPartialGL(const GUIVisualizationSettings& /*s*/, const GNELane* /*fromLane*/, const GNELane* /*toLane*/, const double /*offsetFront*/, const int /*options*/) const {
    // personStops don't use drawPartialGL
}


std::string
GNEPersonStop::getAttribute(SumoXMLAttr key) const {
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
        case SUMO_ATTR_BUS_STOP:
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
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEPersonStop::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ENDPOS:
            return endPos;
        case SUMO_ATTR_ARRIVALPOS:
            return 0;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEPersonStop::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_DURATION:
        case SUMO_ATTR_UNTIL:
        case SUMO_ATTR_EXTENSION:
        case SUMO_ATTR_ACTTYPE:
        // specific of Stops over stoppingPlaces
        case SUMO_ATTR_BUS_STOP:
        // specific of stops over edges/lanes
        case SUMO_ATTR_EDGE:
        case SUMO_ATTR_ENDPOS:
        case SUMO_ATTR_FRIENDLY_POS:
        //
        case GNE_ATTR_SELECTED:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEPersonStop::isValid(SumoXMLAttr key, const std::string& value) {
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
        case SUMO_ATTR_BUS_STOP:
            return (myNet->retrieveAdditional(SUMO_TAG_BUS_STOP, value, false) != nullptr);
        // specific of stops over edges/lanes
        case SUMO_ATTR_EDGE:
            if (myNet->retrieveEdge(value, false) != nullptr) {
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
GNEPersonStop::enableAttribute(SumoXMLAttr key, GNEUndoList* undoList) {
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
            undoList->p_add(new GNEChange_Attribute(this, key, myTagProperty.getAttributeProperties(key).getDefaultValue()));
            break;
        case SUMO_ATTR_UNTIL:
        case SUMO_ATTR_EXTENSION:
            undoList->p_add(new GNEChange_Attribute(this, key, myTagProperty.getAttributeProperties(key).getDefaultValue()));
            break;
        default:
            break;
    }
}


void
GNEPersonStop::disableAttribute(SumoXMLAttr key, GNEUndoList* undoList) {
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
GNEPersonStop::isAttributeEnabled(SumoXMLAttr key) const {
    if (key == SUMO_ATTR_FROM) {
        return (getParentDemandElements().at(0)->getPreviousChildDemandElement(this) == nullptr);
    } else {
        return true;
    }
}


std::string
GNEPersonStop::getPopUpID() const {
    return getTagStr();
}


std::string
GNEPersonStop::getHierarchyName() const {
    if (getParentAdditionals().size() > 0) {
        return "person stop: " + getParentAdditionals().front()->getTagStr();
    } else {
        return "person stop: lane";
    }
}


const std::map<std::string, std::string>&
GNEPersonStop::getACParametersMap() const {
    return getParametersMap();
}


const GNELane*
GNEPersonStop::getFirstAllowedLane() const {
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

// ===========================================================================
// private
// ===========================================================================

void
GNEPersonStop::setAttribute(SumoXMLAttr key, const std::string& value) {
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
        case SUMO_ATTR_BUS_STOP:
            replaceAdditionalParent(SUMO_TAG_BUS_STOP, value);
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
GNEPersonStop::setEnabledAttribute(const int enabledAttributes) {
    parametersSet = enabledAttributes;
}


void
GNEPersonStop::setMoveShape(const GNEMoveResult& moveResult) {
    // change endPos
    endPos = moveResult.shapeToUpdate.front().x();
    // update geometry
    updateGeometry();
}


void
GNEPersonStop::commitMoveShape(const GNEMoveResult& moveResult, GNEUndoList* undoList) {
    undoList->p_begin("endPos of " + getTagStr());
    // now adjust endPos position
    setAttribute(SUMO_ATTR_ENDPOS, toString(moveResult.shapeToUpdate.front().x()), undoList);
    undoList->p_end();
}

/****************************************************************************/
