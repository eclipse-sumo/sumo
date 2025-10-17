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
/// @file    GNEEdgeRelData.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// class for edge relation data
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNESegment.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/data/GNEEdgeRelDataFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEEdgeRelData.h"
#include "GNEDataInterval.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEEdgeRelData::GNEEdgeRelData(GNENet* net) :
    GNEGenericData(SUMO_TAG_EDGEREL, net) {
}


GNEEdgeRelData::GNEEdgeRelData(GNEDataInterval* dataIntervalParent, GNEEdge* fromEdge, GNEEdge* toEdge,
                               const Parameterised::Map& parameters) :
    GNEGenericData(SUMO_TAG_EDGEREL, dataIntervalParent, parameters) {
    // set parents
    setParents<GNEEdge*>({fromEdge, toEdge});
}


GNEEdgeRelData::~GNEEdgeRelData() {}


RGBColor
GNEEdgeRelData::setColor(const GUIVisualizationSettings& s) const {
    // set default color
    RGBColor col = RGBColor::GREEN;
    if (isAttributeCarrierSelected()) {
        col = s.colorSettings.selectedEdgeDataColor;
    } else if (s.dataColorer.getScheme().getName() == GUIVisualizationSettings::SCHEME_NAME_DATA_ATTRIBUTE_NUMERICAL) {
        // user defined rainbow
        double val = getColorValue(s, s.dataColorer.getActive());
        col = s.dataColorer.getScheme().getColor(val);
    } else if (myNet->getViewNet()->getEditModes().dataEditMode == DataEditMode::DATA_EDGERELDATA) {
        // get selected data interval and filtered attribute
        const GNEDataInterval* dataInterval = myNet->getViewNet()->getViewParent()->getEdgeRelDataFrame()->getIntervalSelector()->getDataInterval();
        const std::string filteredAttribute = myNet->getViewNet()->getViewParent()->getEdgeRelDataFrame()->getAttributeSelector()->getFilteredAttribute();
        // continue if there is a selected data interval and filtered attribute
        if (dataInterval && (filteredAttribute.size() > 0)) {
            // obtain minimum and maximum value
            const double minValue = dataInterval->getSpecificAttributeColors().at(myTagProperty->getTag()).getMinValue(filteredAttribute);
            const double maxValue = dataInterval->getSpecificAttributeColors().at(myTagProperty->getTag()).getMaxValue(filteredAttribute);
            // get value
            const double value = parse<double>(getParameter(filteredAttribute, "0"));
            col = GNEViewNetHelper::getRainbowScaledColor(minValue, maxValue, value);
        }
    }
    return col;
}


double
GNEEdgeRelData::getColorValue(const GUIVisualizationSettings& s, int activeScheme) const {
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
                if (hasParameter(s.relDataAttr)) {
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
GNEEdgeRelData::isGenericDataVisible() const {
    // obtain pointer to edge data frame (only for code legibly)
    const GNEEdgeRelDataFrame* edgeRelDataFrame = myNet->getViewNet()->getViewParent()->getEdgeRelDataFrame();
    // get current data edit mode
    DataEditMode dataMode = myNet->getViewNet()->getEditModes().dataEditMode;
    // check if we have to filter generic data
    if ((dataMode == DataEditMode::DATA_INSPECT) || (dataMode == DataEditMode::DATA_DELETE) || (dataMode == DataEditMode::DATA_SELECT)) {
        return isVisibleInspectDeleteSelect();
    } else if (edgeRelDataFrame->shown()) {
        // check interval
        if ((edgeRelDataFrame->getIntervalSelector()->getDataInterval() != nullptr) &&
                (edgeRelDataFrame->getIntervalSelector()->getDataInterval() != myDataIntervalParent)) {
            return false;
        }
        // check attribute
        if ((edgeRelDataFrame->getAttributeSelector()->getFilteredAttribute().size() > 0) &&
                (getParametersMap().count(edgeRelDataFrame->getAttributeSelector()->getFilteredAttribute()) == 0)) {
            return false;
        }
        // all checks ok, then return true
        return true;
    } else {
        // GNEEdgeRelDataFrame hidden, then return false
        return false;
    }
}


void
GNEEdgeRelData::updateGeometry() {
    // just compute path
    computePathElement();
}


void
GNEEdgeRelData::drawGL(const GUIVisualizationSettings& /*s*/) const {
    // Nothing to draw
}


void
GNEEdgeRelData::computePathElement() {
    // calculate path
    myNet->getDataPathManager()->calculateConsecutivePathEdges(this, SVC_IGNORING, getParentEdges());
}


void
GNEEdgeRelData::drawLanePartialGL(const GUIVisualizationSettings& s, const GNESegment* segment, const double offsetFront) const {
    // get color
    const auto color = setColor(s);
    if (segment->getLane() && (color.alpha() != 0) && myNet->getViewNet()->getEditModes().isCurrentSupermodeData()) {
        // get detail level
        const auto d = s.getDetailLevel(1);
        // draw over all edge's lanes
        for (const auto& laneEdge : segment->getLane()->getParentEdge()->getChildLanes()) {
            // get lane width
            const double laneWidth = s.addSize.getExaggeration(s, laneEdge) * s.edgeRelWidthExaggeration *
                                     (laneEdge->getParentEdge()->getNBEdge()->getLaneWidth(laneEdge->getIndex()) * 0.5);
            // Add a draw matrix
            GLHelper::pushMatrix();
            // Start with the drawing of the area translating matrix to origin
            drawInLayer(GLO_EDGERELDATA, offsetFront);
            GLHelper::setColor(RGBColor::BLACK);
            // draw box lines


            GLHelper::drawBoxLines(laneEdge->getLaneShape(), laneEdge->getShapeRotations(),
                                   laneEdge->getShapeLengths(), laneWidth);
            // translate to top
            glTranslated(0, 0, 0.01);
            GLHelper::setColor(color);
            // draw internal box lines
            GLHelper::drawBoxLines(laneEdge->getLaneShape(), laneEdge->getShapeRotations(),
                                   laneEdge->getShapeLengths(), (laneWidth - 0.1));
            // Pop last matrix
            GLHelper::popMatrix();
            // draw lock icon
            GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), getPositionInView(), 1);
            // draw filtered attribute
            if (getParentEdges().front()->getChildLanes().front() == laneEdge) {
                drawFilteredAttribute(s, laneEdge->getLaneShape(),
                                      myNet->getViewNet()->getViewParent()->getEdgeRelDataFrame()->getAttributeSelector()->getFilteredAttribute(),
                                      myNet->getViewNet()->getViewParent()->getEdgeRelDataFrame()->getIntervalSelector()->getDataInterval());
            }
            // draw dotted contour
            segment->getContour()->drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
        }
        // draw dotted contour
        if (getParentEdges().front() == segment->getLane()->getParentEdge()) {
            segment->getContour()->calculateContourEdge(s, d, getParentEdges().front(), this, getType(), true, false);
        }
        if (getParentEdges().back() == segment->getLane()->getParentEdge()) {
            segment->getContour()->calculateContourEdge(s, d, getParentEdges().back(), this, getType(), false, true);
        }
        // check if add this path element to redraw buffer
        if (!gViewObjectsHandler.isPathElementMarkForRedraw(this) && segment->getContour()->checkDrawPathContour(s, d, this)) {
            gViewObjectsHandler.addToRedrawPathElements(this);
        }

    }
}


void
GNEEdgeRelData::drawJunctionPartialGL(const GUIVisualizationSettings& s, const GNESegment* segment, const double /*offsetFront*/) const {
    // get color
    const auto color = setColor(s);
    if ((color.alpha() != 0) && myNet->getViewNet()->getEditModes().isCurrentSupermodeData()) {
        // get detail level
        const auto d = s.getDetailLevel(1);
        // get flag for only draw contour

        // finish

        // draw dotted contour
        if (segment->getPreviousLane() && segment->getNextLane()) {
            segment->getContour()->calculateContourEdges(s, d, segment->getPreviousLane()->getParentEdge(), segment->getNextLane()->getParentEdge());
        }
        // check if add this path element to redraw buffer
        if (!gViewObjectsHandler.isPathElementMarkForRedraw(this) && segment->getContour()->checkDrawPathContour(s, d, this)) {
            gViewObjectsHandler.addToRedrawPathElements(this);
        }
    }
}


GNELane*
GNEEdgeRelData::getFirstPathLane() const {
    /* temporal */
    return nullptr;
}


GNELane*
GNEEdgeRelData::getLastPathLane() const {
    /* temporal */
    return nullptr;
}


Position
GNEEdgeRelData::getPositionInView() const {
    return getParentEdges().front()->getPositionInView();
}


void
GNEEdgeRelData::writeGenericData(OutputDevice& device) const {
    // open device
    device.openTag(SUMO_TAG_EDGEREL);
    // write from
    device.writeAttr(SUMO_ATTR_FROM, getParentEdges().front()->getID());
    // write to
    device.writeAttr(SUMO_ATTR_TO, getParentEdges().back()->getID());
    // iterate over attributes
    for (const auto& attribute : getParametersMap()) {
        // write attribute (don't use writeParams)
        device.writeAttr(attribute.first, attribute.second);
    }
    // close device
    device.closeTag();
}


bool
GNEEdgeRelData::isGenericDataValid() const {
    return true;
}


std::string
GNEEdgeRelData::getGenericDataProblem() const {
    return "";
}


void
GNEEdgeRelData::fixGenericDataProblem() {
    throw InvalidArgument(getTagStr() + " cannot fix any problem");
}


Boundary
GNEEdgeRelData::getCenteringBoundary() const {
    return getParentEdges().front()->getCenteringBoundary();
}


std::string
GNEEdgeRelData::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getPartialID() + getParentEdges().front()->getID();
        case SUMO_ATTR_FROM:
            return getParentEdges().front()->getID();
        case SUMO_ATTR_TO:
            return getParentEdges().back()->getID();
        case GNE_ATTR_DATASET:
            return myDataIntervalParent->getDataSetParent()->getID();
        case SUMO_ATTR_BEGIN:
            return myDataIntervalParent->getAttribute(SUMO_ATTR_BEGIN);
        case SUMO_ATTR_END:
            return myDataIntervalParent->getAttribute(SUMO_ATTR_END);
        default:
            return getCommonAttribute(key);
    }
}


double
GNEEdgeRelData::getAttributeDouble(SumoXMLAttr key) const {
    return getCommonAttributeDouble(key);
}


void
GNEEdgeRelData::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            setCommonAttribute(key, value, undoList);
            break;
    }
}


bool
GNEEdgeRelData::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_FROM:
            return SUMOXMLDefinitions::isValidNetID(value) && (myNet->getAttributeCarriers()->retrieveEdge(value, false) != nullptr) &&
                   (value != getParentEdges().back()->getID());
        case SUMO_ATTR_TO:
            return SUMOXMLDefinitions::isValidNetID(value) && (myNet->getAttributeCarriers()->retrieveEdge(value, false) != nullptr) &&
                   (value != getParentEdges().front()->getID());
        default:
            return isCommonAttributeValid(key, value);
    }
}


bool GNEEdgeRelData::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return false;
        default:
            return true;
    }
}


std::string
GNEEdgeRelData::getPopUpID() const {
    return getTagStr();
}


std::string
GNEEdgeRelData::getHierarchyName() const {
    return getTagStr() + ": " + getParentEdges().front()->getID() + "->" + getParentEdges().back()->getID();
}


void
GNEEdgeRelData::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_FROM: {
            // change first edge
            replaceFirstParentEdge(value);
            break;
        }
        case SUMO_ATTR_TO: {
            // change last edge
            replaceLastParentEdge(value);
            break;
        }
        default:
            setCommonAttribute(key, value);
            if (!isTemplate()) {
                myDataIntervalParent->getDataSetParent()->updateAttributeColors();
            }
            break;
    }
    // mark interval toolbar for update
    myNet->getViewNet()->getIntervalBar().markForUpdate();
}

/****************************************************************************/
