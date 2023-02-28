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
/// @file    GNEEdgeData.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// class for edge data
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNEUndoList.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/data/GNEEdgeDataFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEEdgeData.h"
#include "GNEDataInterval.h"


// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEEdgeData - methods
// ---------------------------------------------------------------------------

GNEEdgeData::GNEEdgeData(GNEDataInterval* dataIntervalParent, GNEEdge* edgeParent, const Parameterised::Map& parameters) :
    GNEGenericData(GNE_TAG_EDGEREL_SINGLE, GUIIconSubSys::getIcon(GUIIcon::EDGEDATA), GLO_EDGEDATA, dataIntervalParent, parameters,
{}, {edgeParent}, {}, {}, {}, {}) {
}


GNEEdgeData::~GNEEdgeData() {}


RGBColor
GNEEdgeData::setColor(const GUIVisualizationSettings& s) const {
    // set default color
    RGBColor col = RGBColor::RED;
    if (isAttributeCarrierSelected()) {
        col = s.colorSettings.selectedEdgeDataColor;
    } else if (s.dataColorer.getScheme().getName() == GUIVisualizationSettings::SCHEME_NAME_DATA_ATTRIBUTE_NUMERICAL) {
        // user defined rainbow
        const double val = getColorValue(s, s.dataColorer.getActive());
        col = s.dataColorer.getScheme().getColor(val);
    } else if (myNet->getViewNet()->getEditModes().dataEditMode == DataEditMode::DATA_EDGEDATA) {
        // get selected data interval and filtered attribute
        const GNEDataInterval* dataInterval = myNet->getViewNet()->getViewParent()->getEdgeDataFrame()->getIntervalSelector()->getDataInterval();
        const std::string filteredAttribute = myNet->getViewNet()->getViewParent()->getEdgeDataFrame()->getAttributeSelector()->getFilteredAttribute();
        // continue if there is a selected data interval and filtered attribute
        if (dataInterval && (filteredAttribute.size() > 0)) {
            if (dataInterval->getSpecificAttributeColors().at(myTagProperty.getTag()).exist(filteredAttribute)) {
                // obtain minimum and maximum value
                const double minValue = dataInterval->getSpecificAttributeColors().at(myTagProperty.getTag()).getMinValue(filteredAttribute);
                const double maxValue = dataInterval->getSpecificAttributeColors().at(myTagProperty.getTag()).getMaxValue(filteredAttribute);
                // get value
                const double value = parse<double>(getParameter(filteredAttribute, "0"));
                // return color
                col = GNEViewNetHelper::getRainbowScaledColor(minValue, maxValue, value);
            }
        }
    }
    return col;
}


double
GNEEdgeData::getColorValue(const GUIVisualizationSettings& s, int activeScheme) const {
    switch (activeScheme) {
        case 0:
            return 0;
        case 1:
            return isAttributeCarrierSelected();
        case 2:
            return 0; // setfunctional color const GNEAdditional* TAZA = getParentAdditionals().front();
        case 3:
            return 0; // setfunctional color const GNEAdditional* TAZA = getParentAdditionals().back();
        case 4:
            // by numerical attribute value
            try {
                if (knowsParameter(s.relDataAttr)) {
                    return StringUtils::toDouble(getParameter(s.relDataAttr, "-1"));
                } else {
                    return GUIVisualizationSettings::MISSING_DATA;
                }
            } catch (NumberFormatException&) {
                return GUIVisualizationSettings::MISSING_DATA;
            }
    }
    return 0;
}


bool
GNEEdgeData::isGenericDataVisible() const {
    // get current data edit mode
    DataEditMode dataMode = myNet->getViewNet()->getEditModes().dataEditMode;
    // check if we have to filter generic data
    if ((dataMode == DataEditMode::DATA_INSPECT) || (dataMode == DataEditMode::DATA_DELETE) || (dataMode == DataEditMode::DATA_SELECT)) {
        return isVisibleInspectDeleteSelect();
    } else if (myDataIntervalParent->getNet()->getViewNet()->getViewParent()->getEdgeDataFrame()->shown()) {
        // get selected data interval and filtered attribute
        const GNEDataInterval* dataInterval = myNet->getViewNet()->getViewParent()->getEdgeDataFrame()->getIntervalSelector()->getDataInterval();
        const std::string filteredAttribute = myNet->getViewNet()->getViewParent()->getEdgeDataFrame()->getAttributeSelector()->getFilteredAttribute();
        // check interval
        if ((dataInterval != nullptr) && (dataInterval != myDataIntervalParent)) {
            return false;
        }
        // check attribute
        if ((filteredAttribute.size() > 0) && (getParametersMap().count(filteredAttribute) == 0)) {
            return false;
        }
        // all checks ok, then return true
        return true;
    } else {
        // GNEEdgeDataFrame hidden, then return false
        return false;
    }
}


void
GNEEdgeData::updateGeometry() {
    // calculate path
    myNet->getPathManager()->calculateConsecutivePathEdges(this, SVC_IGNORING, getParentEdges());
}


Position
GNEEdgeData::getPositionInView() const {
    return getParentEdges().front()->getPositionInView();
}


void
GNEEdgeData::writeGenericData(OutputDevice& device) const {
    // open device (don't use SUMO_TAG_MEANDATA_EDGE)
    device.openTag(SUMO_TAG_EDGE);
    // write edge ID
    device.writeAttr(SUMO_ATTR_ID, getParentEdges().front()->getID());
    // iterate over attributes
    for (const auto& attribute : getParametersMap()) {
        // write attribute (don't use writeParams)
        device.writeAttr(attribute.first, attribute.second);
    }
    // close device
    device.closeTag();
}


bool
GNEEdgeData::isGenericDataValid() const {
    return true;
}


std::string
GNEEdgeData::getGenericDataProblem() const {
    return "";
}


void
GNEEdgeData::fixGenericDataProblem() {
    throw InvalidArgument(getTagStr() + " cannot fix any problem");
}


void
GNEEdgeData::drawGL(const GUIVisualizationSettings& /*s*/) const {
    // Nothing to draw
}


void
GNEEdgeData::computePathElement() {
    // nothing to compute
}


void
GNEEdgeData::drawPartialGL(const GUIVisualizationSettings& s, const GNELane* lane, const GNEPathManager::Segment* /*segment*/, const double offsetFront) const {
    // get color
    const auto color = setColor(s);
    if ((color.alpha() != 0) && myNet->getViewNet()->getEditModes().isCurrentSupermodeData()) {
        // get flag for only draw contour
        const bool onlyDrawContour = !isGenericDataVisible();
        // Start drawing adding an gl identificator
        if (!onlyDrawContour) {
            GLHelper::pushName(getGlID());
        }
        // draw over all edge's lanes
        for (const auto& laneEdge : lane->getParentEdge()->getLanes()) {
            // get lane width
            const double laneWidth = s.addSize.getExaggeration(s, laneEdge) * s.edgeRelWidthExaggeration *
                                     (laneEdge->getParentEdge()->getNBEdge()->getLaneWidth(laneEdge->getIndex()) * 0.5);
            // Add a draw matrix
            GLHelper::pushMatrix();
            // Start with the drawing of the area translating matrix to origin
            myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_EDGEDATA, offsetFront);
            GLHelper::setColor(RGBColor::BLACK);
            // draw box lines
            GUIGeometry::drawLaneGeometry(s, myNet->getViewNet()->getPositionInformation(),
                                          laneEdge->getLaneShape(), laneEdge->getShapeRotations(),
                                          laneEdge->getShapeLengths(), {}, laneWidth, onlyDrawContour);
            // translate to top
            glTranslated(0, 0, 0.01);
            GLHelper::setColor(color);
            // draw interne box lines
            GUIGeometry::drawLaneGeometry(s, myNet->getViewNet()->getPositionInformation(),
                                          laneEdge->getLaneShape(), laneEdge->getShapeRotations(),
                                          laneEdge->getShapeLengths(), {}, (laneWidth - 0.1), onlyDrawContour);
            // Pop last matrix
            GLHelper::popMatrix();
            // draw lock icon
            GNEViewNetHelper::LockIcon::drawLockIcon(this, getType(), getPositionInView(), 1);
            // check if mouse is over element
            for (const auto& laneEdgeParent : laneEdge->getParentEdge()->getLanes()) {
                // get lane drawing constants
                GNELane::LaneDrawingConstants laneDrawingConstants(s, laneEdgeParent);
                mouseWithinGeometry(laneEdgeParent->getLaneShape(), laneDrawingConstants.halfWidth * s.edgeRelWidthExaggeration);
            }
            // draw filtered attribute
            if (getParentEdges().front()->getLanes().front() == laneEdge) {
                drawFilteredAttribute(s, laneEdge->getLaneShape(),
                                      myNet->getViewNet()->getViewParent()->getEdgeDataFrame()->getAttributeSelector()->getFilteredAttribute(),
                                      myNet->getViewNet()->getViewParent()->getEdgeDataFrame()->getIntervalSelector()->getDataInterval());
            }
            // inspect contour
            if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
                GNEEdge::drawDottedContourEdge(s, GUIDottedGeometry::DottedContourType::INSPECT,
                                               laneEdge->getParentEdge(), true, true, s.edgeRelWidthExaggeration);
            }
            // front contour
            if (myNet->getViewNet()->getFrontAttributeCarrier() == this) {
                GNEEdge::drawDottedContourEdge(s, GUIDottedGeometry::DottedContourType::FRONT,
                                               laneEdge->getParentEdge(), true, true, s.edgeRelWidthExaggeration);
            }
            // delete contour
            if (myNet->getViewNet()->drawDeleteContour(this, this)) {
                GNEEdge::drawDottedContourEdge(s, GUIDottedGeometry::DottedContourType::REMOVE,
                                               laneEdge->getParentEdge(), true, true, s.edgeRelWidthExaggeration);
            }
            // select contour
            if (myNet->getViewNet()->drawSelectContour(this, this)) {
                GNEEdge::drawDottedContourEdge(s, GUIDottedGeometry::DottedContourType::SELECT,
                                               laneEdge->getParentEdge(), true, true, s.edgeRelWidthExaggeration);
            }
        }
        // Pop name
        if (!onlyDrawContour) {
            GLHelper::popName();
        }
    }
}


void
GNEEdgeData::drawPartialGL(const GUIVisualizationSettings& /*s*/, const GNELane* /*fromLane*/, const GNELane* /*toLane*/, const GNEPathManager::Segment* /*segment*/, const double /*offsetFront*/) const {
    // EdgeDatas don't use drawPartialGL over junction
}


GNELane*
GNEEdgeData::getFirstPathLane() const {
    /* temporal */
    return nullptr;
}


GNELane*
GNEEdgeData::getLastPathLane() const {
    /* temporal */
    return nullptr;
}


Boundary
GNEEdgeData::getCenteringBoundary() const {
    return getParentEdges().front()->getCenteringBoundary();
}


std::string
GNEEdgeData::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getPartialID() + getParentEdges().front()->getID();
        case GNE_ATTR_DATASET:
            return myDataIntervalParent->getDataSetParent()->getID();
        case SUMO_ATTR_BEGIN:
            return myDataIntervalParent->getAttribute(SUMO_ATTR_BEGIN);
        case SUMO_ATTR_END:
            return myDataIntervalParent->getAttribute(SUMO_ATTR_END);
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNEEdgeData::getAttributeDouble(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
}


void
GNEEdgeData::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEEdgeData::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areAttributesValid(value, true);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool GNEEdgeData::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return false;
        default:
            return true;
    }
}


std::string
GNEEdgeData::getPopUpID() const {
    return getTagStr();
}


std::string
GNEEdgeData::getHierarchyName() const {
    return getTagStr() + ": " + getParentEdges().front()->getID();
}


void
GNEEdgeData::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_PARAMETERS:
            setParametersStr(value);
            // update attribute colors
            myDataIntervalParent->getDataSetParent()->updateAttributeColors();
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/
