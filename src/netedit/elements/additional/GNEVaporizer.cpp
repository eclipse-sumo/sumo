/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GNEVaporizer.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
///
//
/****************************************************************************/
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/div/GUIGlobalViewObjectsHandler.h>

#include "GNEVaporizer.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEVaporizer::GNEVaporizer(GNENet* net) :
    GNEAdditional("", net, GLO_VAPORIZER, SUMO_TAG_VAPORIZER, GUIIconSubSys::getIcon(GUIIcon::VAPORIZER), "",
{}, {}, {}, {}, {}, {}),
myBegin(0),
myEnd(0) {
    // reset default values
    resetDefaultValues();
}


GNEVaporizer::GNEVaporizer(GNENet* net, GNEEdge* edge, SUMOTime from, SUMOTime end, const std::string& name,
                           const Parameterised::Map& parameters) :
    GNEAdditional(edge->getID(), net, GLO_VAPORIZER, SUMO_TAG_VAPORIZER,  GUIIconSubSys::getIcon(GUIIcon::VAPORIZER), name,
{}, {edge}, {}, {}, {}, {}),
Parameterised(parameters),
myBegin(from),
myEnd(end) {
    // update centering boundary without updating grid
    updateCenteringBoundary(false);
}


GNEVaporizer::~GNEVaporizer() {
}


GNEMoveOperation*
GNEVaporizer::getMoveOperation() {
    // vaporizers cannot be moved
    return nullptr;
}


void
GNEVaporizer::writeAdditional(OutputDevice& device) const {
    device.openTag(getTagProperty().getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    if (!myAdditionalName.empty()) {
        device.writeAttr(SUMO_ATTR_NAME, StringUtils::escapeXML(myAdditionalName));
    }
    device.writeAttr(SUMO_ATTR_BEGIN, time2string(myBegin));
    device.writeAttr(SUMO_ATTR_END, time2string(myEnd));
    // write parameters (Always after children to avoid problems with additionals.xsd)
    writeParams(device);
    device.closeTag();
}


bool
GNEVaporizer::isAdditionalValid() const {
    return true;
}


std::string
GNEVaporizer::getAdditionalProblem() const {
    return "";
}


void
GNEVaporizer::fixAdditionalProblem() {
    // nothing to fix
}


bool
GNEVaporizer::checkDrawMoveContour() const {
    return false;
}


void
GNEVaporizer::updateGeometry() {
    // calculate perpendicular line
    calculatePerpendicularLine(3);
}


Position
GNEVaporizer::getPositionInView() const {
    return myAdditionalGeometry.getShape().getPolygonCenter();
}


void
GNEVaporizer::updateCenteringBoundary(const bool /*updateGrid*/) {
    // nothing to do
}


void
GNEVaporizer::splitEdgeGeometry(const double /*splitPosition*/, const GNENetworkElement* /*originalElement*/, const GNENetworkElement* /*newElement*/, GNEUndoList* /*undoList*/) {
    // geometry of this element cannot be splitted
}


std::string
GNEVaporizer::getParentName() const {
    return getParentEdges().front()->getID();
}


void
GNEVaporizer::drawGL(const GUIVisualizationSettings& s) const {
    // first check if additional has to be drawn
    if (myNet->getViewNet()->getDataViewOptions().showAdditionals()) {
        // Obtain exaggeration of the draw
        const double vaporizerExaggeration = getExaggeration(s);
        // get detail level
        const auto d = s.getDetailLevel(vaporizerExaggeration);
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (s.checkDrawAdditional(d, isAttributeCarrierSelected())) {
            // declare colors
            RGBColor vaporizerColor, centralLineColor;
            // set colors
            if (drawUsingSelectColor()) {
                vaporizerColor = s.colorSettings.selectedAdditionalColor;
                centralLineColor = vaporizerColor.changedBrightness(-32);
            } else {
                vaporizerColor = s.additionalSettings.vaporizerColor;
                centralLineColor = RGBColor::WHITE;
            }
            // draw parent and child lines
            drawParentChildLines(s, s.additionalSettings.connectionColor);
            // Add layer matrix matrix
            GLHelper::pushMatrix();
            // translate to front
            myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_VAPORIZER);
            // set base color
            GLHelper::setColor(vaporizerColor);
            // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
            GUIGeometry::drawGeometry(d, myAdditionalGeometry, 0.3 * vaporizerExaggeration);
            // move to front
            glTranslated(0, 0, .1);
            // set central color
            GLHelper::setColor(centralLineColor);
            // Draw the area using shape, shapeRotations, shapeLengths and value of exaggeration
            GUIGeometry::drawGeometry(d, myAdditionalGeometry, 0.05 * vaporizerExaggeration);
            // move to icon position and front
            glTranslated(myAdditionalGeometry.getShape().front().x(), myAdditionalGeometry.getShape().front().y(), .1);
            // rotate over lane
            GUIGeometry::rotateOverLane(myAdditionalGeometry.getShapeRotations().front() * -1);
            // Draw icon depending of level of detail
            if (d <= GUIVisualizationSettings::Detail::AdditionalDetails) {
                // set color
                glColor3d(1, 1, 1);
                // rotate texture
                glRotated(90, 0, 0, 1);
                // draw texture
                if (drawUsingSelectColor()) {
                    GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GUITexture::VAPORIZER_SELECTED), s.additionalSettings.vaporizerSize * vaporizerExaggeration);
                } else {
                    GUITexturesHelper::drawTexturedBox(GUITextureSubSys::getTexture(GUITexture::VAPORIZER), s.additionalSettings.vaporizerSize * vaporizerExaggeration);
                }
            } else {
                // set route probe color
                GLHelper::setColor(vaporizerColor);
                // just drawn a box
                GLHelper::drawBoxLine(Position(0, 0), 0, 2 * s.additionalSettings.vaporizerSize, s.additionalSettings.vaporizerSize * vaporizerExaggeration);
            }
            // pop layer matrix
            GLHelper::popMatrix();
            // draw additional name
            drawAdditionalName(s);
            // draw dotted contours
            myAdditionalContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
            mySymbolBaseContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidthSmall, true);
        }
        // calculate contours
        myAdditionalContour.calculateContourRectangleShape(s, d, this, myAdditionalGeometry.getShape().front(), s.additionalSettings.vaporizerSize,
                s.additionalSettings.vaporizerSize, getType(), 0, 0, 0, vaporizerExaggeration);
        mySymbolBaseContour.calculateContourExtrudedShape(s, d, this, myAdditionalGeometry.getShape(), getType(), 0.3, vaporizerExaggeration, true, true, 0);
    }
}


std::string
GNEVaporizer::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_EDGE:
            return getMicrosimID();
        case SUMO_ATTR_BEGIN:
            return time2string(myBegin);
        case SUMO_ATTR_END:
            return time2string(myEnd);
        case SUMO_ATTR_NAME:
            return myAdditionalName;
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEVaporizer::getAttributeDouble(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_BEGIN:
            return STEPS2TIME(myBegin);
        case SUMO_ATTR_END:
            return STEPS2TIME(myEnd);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
    }
}


const Parameterised::Map&
GNEVaporizer::getACParametersMap() const {
    return getParametersMap();
}


void
GNEVaporizer::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_EDGE:
        case SUMO_ATTR_BEGIN:
        case SUMO_ATTR_END:
        case SUMO_ATTR_NAME:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEVaporizer::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_EDGE:
            if (myNet->getAttributeCarriers()->retrieveEdge(value, false) != nullptr) {
                return isValidAdditionalID(value);
            } else {
                return false;
            }
        case SUMO_ATTR_BEGIN:
            if (canParse<SUMOTime>(value)) {
                return (parse<SUMOTime>(value) <= myEnd);
            } else {
                return false;
            }
        case SUMO_ATTR_END:
            if (canParse<SUMOTime>(value)) {
                return (myBegin <= parse<SUMOTime>(value));
            } else {
                return false;
            }
        case SUMO_ATTR_NAME:
            return SUMOXMLDefinitions::isValidAttribute(value);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


std::string
GNEVaporizer::getPopUpID() const {
    return getTagStr();
}


std::string
GNEVaporizer::getHierarchyName() const {
    return getTagStr() + ": " + getAttribute(SUMO_ATTR_BEGIN) + " -> " + getAttribute(SUMO_ATTR_END);
}

// ===========================================================================
// private
// ===========================================================================

void
GNEVaporizer::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_EDGE:
            // update microsimID
            setAdditionalID(value);
            replaceAdditionalParentEdges(value);
            break;
        case SUMO_ATTR_BEGIN:
            myBegin = parse<SUMOTime>(value);
            break;
        case SUMO_ATTR_END:
            myEnd = parse<SUMOTime>(value);
            break;
        case SUMO_ATTR_NAME:
            myAdditionalName = value;
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
GNEVaporizer::setMoveShape(const GNEMoveResult& /*moveResult*/) {
    // nothing to do
}


void
GNEVaporizer::commitMoveShape(const GNEMoveResult& /*moveResult*/, GNEUndoList* /*undoList*/) {
    // nothing to do
}

/****************************************************************************/
