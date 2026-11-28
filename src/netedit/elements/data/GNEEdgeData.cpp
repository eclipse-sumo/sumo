/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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

#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNESegment.h>
#include <netedit/GNEUndoList.h>
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

GNEEdgeData::GNEEdgeData(GNENet* net) :
    GNEGenericData(GNE_TAG_EDGEREL_SINGLE, net) {
}


GNEEdgeData::GNEEdgeData(GNEDataInterval* dataIntervalParent, GNEEdge* edge, const Parameterised::Map& parameters) :
    GNEGenericData(GNE_TAG_EDGEREL_SINGLE, dataIntervalParent, parameters) {
    // set parents
    setParent<GNEEdge*>(edge);
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
        const GNEDataInterval* dataInterval = myNet->getViewParent()->getEdgeDataFrame()->getIntervalSelector()->getDataInterval();
        const std::string filteredAttribute = myNet->getViewParent()->getEdgeDataFrame()->getAttributeSelector()->getFilteredAttribute();
        // continue if there is a selected data interval and filtered attribute
        if (dataInterval && (filteredAttribute.size() > 0)) {
            if (dataInterval->getSpecificAttributeColors().at(myTagProperty->getTag()).exist(filteredAttribute)) {
                // obtain minimum and maximum value
                const double minValue = dataInterval->getSpecificAttributeColors().at(myTagProperty->getTag()).getMinValue(filteredAttribute);
                const double maxValue = dataInterval->getSpecificAttributeColors().at(myTagProperty->getTag()).getMaxValue(filteredAttribute);
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
                if (hasParameter(s.relDataAttr)) {
                    return StringUtils::toDouble(getParameter(s.relDataAttr, "-1"));
                } else {
                    return GUIVisualizationSettings::MISSING_DATA;
                }
            } catch (NumberFormatException&) {
                return GUIVisualizationSettings::MISSING_DATA;
            }
        default:
            return 0;
    }
}


bool
GNEEdgeData::isGenericDataVisible() const {
    return getVisibility() == Visibility::FULL;
}


GNEEdgeData::Visibility
GNEEdgeData::getVisibility() const {
    const auto dataMode = myNet->getViewNet()->getEditModes().dataEditMode;
    if (dataMode == DataEditMode::DATA_INSPECT || dataMode == DataEditMode::DATA_DELETE || dataMode == DataEditMode::DATA_SELECT) {
        return isVisibleInspectDeleteSelect() ? Visibility::FULL : Visibility::HIDDEN;
    }
    const auto edgeDataFrame = myNet->getViewParent()->getEdgeDataFrame();
    if (!edgeDataFrame->shown()) {
        return Visibility::HIDDEN;
    }
    const auto dataSet = edgeDataFrame->getDataSetSelector()->getDataSet();
    if (dataSet && dataSet != myDataIntervalParent->getDataSetParent()) {
        return Visibility::HIDDEN;
    }
    const auto dataInterval = edgeDataFrame->getIntervalSelector()->getDataInterval();
    if (dataInterval && dataInterval != myDataIntervalParent) {
        return Visibility::HIDDEN;
    }
    const auto filteredAttribute = edgeDataFrame->getAttributeSelector()->getFilteredAttribute();
    if (filteredAttribute.empty() || hasParameter(filteredAttribute)) {
        return Visibility::FULL;
    }
    // Keep data without the selected attribute visible as outlines in the active interval.
    return dataInterval ? Visibility::OUTLINE : Visibility::HIDDEN;
}


void
GNEEdgeData::updateGeometry() {
    // calculate path
    myNet->getDataPathManager()->calculateConsecutivePathEdges(this, SVC_IGNORING, getParentEdges());
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
GNEEdgeData::drawLanePartialGL(const GUIVisualizationSettings& s, const GNESegment* segment, const double offsetFront) const {
    if (!segment->getLane() || !myNet->getViewNet()->getEditModes().isCurrentSupermodeData()) {
        return;
    }
    const auto visibility = getVisibility();
    if (visibility == Visibility::HIDDEN || (visibility == Visibility::OUTLINE && s.drawForViewObjectsHandler)) {
        return;
    }
    const auto color = setColor(s);
    if (color.alpha() == 0) {
        return;
    }
    const auto d = s.getDetailLevel(1);
    const auto edge = segment->getLane()->getParentEdge();
    if (s.drawForViewObjectsHandler) {
        segment->getContour()->calculateContourEdge(s, d, edge, this, getType(), true, true);
        return;
    }

    const auto& lanes = edge->getChildLanes();
    for (const auto& lane : lanes) {
        const auto drawing = lane->getDrawingConstants();
        const auto& geometry = lane->getLaneGeometry();
        const double width = drawing->getDrawingWidth();
        GLHelper::pushMatrix();
        drawInLayer(GLO_EDGEDATA, offsetFront);
        if (visibility == Visibility::OUTLINE) {
            GLHelper::setColor(color);
            GUIGeometry::drawContourGeometry(geometry, width, true);
        } else {
            GLHelper::setColor(RGBColor::BLACK);
            GUIGeometry::drawGeometry(drawing->getDetail(), geometry, width);
            glTranslated(0, 0, 0.01);
            GLHelper::setColor(color);
            GUIGeometry::drawGeometry(drawing->getDetail(), geometry, width - 0.1);
        }
        GLHelper::popMatrix();
    }

    if (visibility == Visibility::FULL) {
        GNEViewNetHelper::LockIcon::drawLockIcon(d, this, getType(), getPositionInView(), 1);
        const auto edgeDataFrame = myNet->getViewParent()->getEdgeDataFrame();
        drawFilteredAttribute(s, lanes.front()->getLaneShape(), edgeDataFrame->getAttributeSelector()->getFilteredAttribute(),
                              edgeDataFrame->getIntervalSelector()->getDataInterval());
        segment->getContour()->drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
    }
}


void
GNEEdgeData::drawJunctionPartialGL(const GUIVisualizationSettings& /*s*/, const GNESegment* /*segment*/, const double /*offsetFront*/) const {
    // EdgeDatas don't use drawJunctionPartialGL over junction
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
        default:
            return getCommonAttribute(key);
    }
}


double
GNEEdgeData::getAttributeDouble(SumoXMLAttr key) const {
    return getCommonAttributeDouble(key);
}


void
GNEEdgeData::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    setCommonAttribute(key, value, undoList);
}


bool
GNEEdgeData::isValid(SumoXMLAttr key, const std::string& value) {
    return isCommonAttributeValid(key, value);
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
    setCommonAttribute(key, value);
    if (!isTemplate()) {
        myDataIntervalParent->getDataSetParent()->updateAttributeColors();
    }
    // mark interval toolbar for update
    if (!isTemplate()) {
        myNet->getViewNet()->getIntervalBar().markForUpdate();
    }
}

/****************************************************************************/
