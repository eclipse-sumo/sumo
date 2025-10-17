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
/// @file    GNETAZRelData.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// class for TAZ relation data
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/elements/additional/GNETAZ.h>
#include <netedit/frames/data/GNETAZRelDataFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalViewObjectsHandler.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNETAZRelData.h"
#include "GNEDataInterval.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNETAZRelData::GNETAZRelData(GNENet* net) :
    GNEGenericData(SUMO_TAG_TAZREL, net),
    myLastWidth(0) {
}


GNETAZRelData::GNETAZRelData(GNEDataInterval* dataIntervalParent, GNEAdditional* fromTAZ, GNEAdditional* toTAZ,
                             const Parameterised::Map& parameters) :
    GNEGenericData(SUMO_TAG_TAZREL, dataIntervalParent, parameters),
    myLastWidth(0) {
    // set parents
    setParents<GNEAdditional*>({fromTAZ, toTAZ});
}


GNETAZRelData::GNETAZRelData(GNEDataInterval* dataIntervalParent, GNEAdditional* TAZ,
                             const Parameterised::Map& parameters) :
    GNEGenericData(SUMO_TAG_TAZREL, dataIntervalParent, parameters),
    myLastWidth(0) {
    // set parents
    setParent<GNEAdditional*>(TAZ);
}


GNETAZRelData::~GNETAZRelData() {}


RGBColor
GNETAZRelData::setColor(const GUIVisualizationSettings& s) const {
    RGBColor color;
    if (isAttributeCarrierSelected()) {
        color = s.colorSettings.selectedEdgeDataColor;
    } else {
        if (!setFunctionalColor(s.dataColorer.getActive(), color)) {
            double val = getColorValue(s, s.dataColorer.getActive());
            color = s.dataColorer.getScheme().getColor(val);
        }
    }
    return color;
}


double
GNETAZRelData::getColorValue(const GUIVisualizationSettings& s, int activeScheme) const {
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


double
GNETAZRelData::getScaleValue(const GUIVisualizationSettings& s, int activeScheme) const {
    switch (activeScheme) {
        case 0: // uniform
            return 0;
        case 1: // selection
            return isAttributeCarrierSelected();
        case 2: // by numerical attribute value
            try {
                if (hasParameter(s.relDataScaleAttr)) {
                    return StringUtils::toDouble(getParameter(s.relDataScaleAttr, "-1"));
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
GNETAZRelData::isGenericDataVisible() const {
    // obtain pointer to TAZ data frame (only for code legibly)
    const GNETAZRelDataFrame* TAZRelDataFrame = myNet->getViewNet()->getViewParent()->getTAZRelDataFrame();
    // get current data edit mode
    DataEditMode dataMode = myNet->getViewNet()->getEditModes().dataEditMode;
    // check if we have to filter generic data
    if ((dataMode == DataEditMode::DATA_INSPECT) || (dataMode == DataEditMode::DATA_DELETE) || (dataMode == DataEditMode::DATA_SELECT)) {
        return /*isVisibleInspectDeleteSelect()*/ true;
    } else if (TAZRelDataFrame->shown()) {
        // check interval
        if ((TAZRelDataFrame->getIntervalSelector()->getDataInterval() != nullptr) &&
                (TAZRelDataFrame->getIntervalSelector()->getDataInterval() != myDataIntervalParent)) {
            return false;
        }
        // check attribute
        if ((TAZRelDataFrame->getAttributeSelector()->getFilteredAttribute().size() > 0) &&
                (getParametersMap().count(TAZRelDataFrame->getAttributeSelector()->getFilteredAttribute()) == 0)) {
            return false;
        }
        // all checks ok, then return true
        return true;
    } else {
        // GNETAZRelDataFrame hidden, then return false
        return false;
    }
}


void
GNETAZRelData::updateGeometry() {
    // remove from grid
    myNet->removeGLObjectFromGrid(this);
    // get both TAZs
    const GNEAdditional* TAZA = getParentAdditionals().front();
    const GNEAdditional* TAZB = getParentAdditionals().back();
    // check if this is the same TAZ
    if (TAZA == TAZB) {
        // declare ring
        PositionVector ring;
        // declare first point
        std::pair<double, double> p1 = GLHelper::getCircleCoords().at(GLHelper::angleLookup(0));
        // add 8 segments
        for (int i = 0; i <= 8; ++i) {
            const std::pair<double, double>& p2 = GLHelper::getCircleCoords().at(GLHelper::angleLookup(0 + i * 45));
            // make al line between 0,0 and p2
            PositionVector line = {Position(), Position(p2.first, p2.second)};
            // extrapolate
            line.extrapolate(3, false, true);
            // add line back to ring
            ring.push_back(line.back());
            // update p1
            p1 = p2;
        }
        // make a copy of ring
        PositionVector ringCenter = ring;
        // move ring to first geometry point
        ring.add(TAZA->getAdditionalGeometry().getShape().front());
        myTAZRelGeometry.updateGeometry(ring);
        // move ringCenter to center
        ringCenter.add(TAZA->getAttributePosition(SUMO_ATTR_CENTER));
        myTAZRelGeometryCenter.updateGeometry(ringCenter);
    } else {
        // calculate line between to TAZ centers
        PositionVector line = {TAZA->getAttributePosition(SUMO_ATTR_CENTER), TAZB->getAttributePosition(SUMO_ATTR_CENTER)};
        // check line
        if (line.length() < 1) {
            line = {TAZA->getAttributePosition(SUMO_ATTR_CENTER) - 0.5, TAZB->getAttributePosition(SUMO_ATTR_CENTER) + 0.5};
        }
        // add offset to line
        line.move2side(0.5 + myLastWidth);
        // calculate middle point
        const Position middlePoint = line.getLineCenter();
        // get closest points to middlePoint
        Position posA = TAZA->getAdditionalGeometry().getShape().positionAtOffset2D(TAZA->getAdditionalGeometry().getShape().nearest_offset_to_point2D(middlePoint));
        Position posB = TAZB->getAdditionalGeometry().getShape().positionAtOffset2D(TAZB->getAdditionalGeometry().getShape().nearest_offset_to_point2D(middlePoint));
        // check positions
        if (posA == Position::INVALID) {
            posA = TAZA->getAdditionalGeometry().getShape().front();
        }
        if (posB == Position::INVALID) {
            posB = TAZB->getAdditionalGeometry().getShape().front();
        }
        // update geometry
        if (posA.distanceTo(posB) < 1) {
            myTAZRelGeometry.updateGeometry({posA - 0.5, posB + 0.5});
        } else {
            myTAZRelGeometry.updateGeometry({posA, posB});
        }
        // update center geometry
        myTAZRelGeometryCenter.updateGeometry(line);
    }
    // add into grid again
    myNet->addGLObjectIntoGrid(this);
}


Position
GNETAZRelData::getPositionInView() const {
    return getParentAdditionals().front()->getAttributePosition(SUMO_ATTR_CENTER);
}


void
GNETAZRelData::writeGenericData(OutputDevice& device) const {
    // open device
    device.openTag(SUMO_TAG_TAZREL);
    // write from
    device.writeAttr(SUMO_ATTR_FROM, getParentAdditionals().front()->getID());
    // write to
    device.writeAttr(SUMO_ATTR_TO, getParentAdditionals().back()->getID());
    // iterate over attributes
    for (const auto& attribute : getParametersMap()) {
        // write attribute (don't use writeParams)
        device.writeAttr(attribute.first, attribute.second);
    }
    // close device
    device.closeTag();
}


bool
GNETAZRelData::isGenericDataValid() const {
    return true;
}


std::string
GNETAZRelData::getGenericDataProblem() const {
    return "";
}


void
GNETAZRelData::fixGenericDataProblem() {
    throw InvalidArgument(getTagStr() + " cannot fix any problem");
}


void
GNETAZRelData::drawGL(const GUIVisualizationSettings& s) const {
    // draw boundaries
    GLHelper::drawBoundary(s, getCenteringBoundary());
    // draw TAZRels
    if (drawTAZRel()) {
        // get detail level
        const auto d = s.getDetailLevel(1);
        // draw geometry only if we'rent in drawForObjectUnderCursor mode
        if (!s.drawForViewObjectsHandler) {
            const auto& color = setColor(s);
            // get flag for only draw contour
            const bool onlyDrawContour = !isGenericDataVisible();
            // push matrix
            GLHelper::pushMatrix();
            // translate to front
            drawInLayer(GLO_TAZ + 1);
            GLHelper::setColor(color);
            // check if update lastWidth
            const double width = (onlyDrawContour ? 0.1 :  0.5 * s.tazRelWidthExaggeration
                                  * s.dataScaler.getScheme().getColor(getScaleValue(s, s.dataScaler.getActive())));
            if (width != myLastWidth) {
                myLastWidth = width;
            }
            // draw geometry
            if (onlyDrawContour) {
                // draw depending of TAZRelDrawing
                if (myNet->getViewNet()->getDataViewOptions().TAZRelDrawing()) {
                    GUIGeometry::drawGeometry(d, myTAZRelGeometryCenter, width);
                } else {
                    GUIGeometry::drawGeometry(d, myTAZRelGeometry, width);
                }
            } else {
                // draw depending of TAZRelDrawing
                const GUIGeometry& geom = (myNet->getViewNet()->getDataViewOptions().TAZRelDrawing()
                                           ? myTAZRelGeometryCenter : myTAZRelGeometry);
                GUIGeometry::drawGeometry(d, geom, width);
                GLHelper::drawTriangleAtEnd(
                    *(geom.getShape().end() - 2),
                    *(geom.getShape().end() - 1),
                    1.5 + width, 1.5 + width, 0.5 + width);
            }
            // pop matrix
            GLHelper::popMatrix();
            // draw dotted contour
            myTAZRelDataContour.drawDottedContours(s, d, this, s.dottedContourSettings.segmentWidth, true);
        }
        if (myNet->getViewNet()->getDataViewOptions().TAZRelDrawing()) {
            // calculate contour and draw dotted geometry
            myTAZRelDataContour.calculateContourExtrudedShape(s, d, this, myTAZRelGeometryCenter.getShape(), getType(),
                    0.5, 1, true, true, 0, nullptr, nullptr);
        } else {
            // calculate contour and draw dotted geometry
            myTAZRelDataContour.calculateContourExtrudedShape(s, d, this, myTAZRelGeometry.getShape(), getType(),
                    0.5, 1, true, true, 0, nullptr, nullptr);
        }
    }
}


bool
GNETAZRelData::setFunctionalColor(int activeScheme, RGBColor& col) const {
    switch (activeScheme) {
        case 2: { // origin taz
            const GNETAZ* from = dynamic_cast<const GNETAZ*>(getParentAdditionals().front());
            col = from->getShapeColor();
            return true;
        }
        case 3: { // destination taz
            const GNETAZ* to = dynamic_cast<const GNETAZ*>(getParentAdditionals().back());
            col = to->getShapeColor();
            return true;
        }
        default:
            return false;
    }
}

void
GNETAZRelData::computePathElement() {
    // nothing to compute
}


void
GNETAZRelData::drawLanePartialGL(const GUIVisualizationSettings& /*s*/, const GNESegment* /*segment*/, const double /*offsetFront*/) const {
    // nothing to draw
}


void
GNETAZRelData::drawJunctionPartialGL(const GUIVisualizationSettings& /*s*/, const GNESegment* /*segment*/, const double /*offsetFront*/) const {
    // nothing to draw
}


GNELane*
GNETAZRelData::getFirstPathLane() const {
    return nullptr;
}


GNELane*
GNETAZRelData::getLastPathLane() const {
    return nullptr;
}


Boundary
GNETAZRelData::getCenteringBoundary() const {
    Boundary b;
    // add two shapes
    b.add(myTAZRelGeometry.getShape().getBoxBoundary());
    b.add(myTAZRelGeometryCenter.getShape().getBoxBoundary());
    b.grow(20);
    return b;
}


std::string
GNETAZRelData::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            if (getParentAdditionals().size() == 1) {
                return getPartialID() + getParentAdditionals().front()->getID();
            } else {
                return getPartialID() + (getParentAdditionals().front()->getID() + "->" + getParentAdditionals().back()->getID());
            }
        case SUMO_ATTR_FROM:
            return getParentAdditionals().front()->getID();
        case SUMO_ATTR_TO:
            return getParentAdditionals().back()->getID();
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
GNETAZRelData::getAttributeDouble(SumoXMLAttr key) const {
    return getCommonAttributeDouble(key);
}


void
GNETAZRelData::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
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
GNETAZRelData::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
            return SUMOXMLDefinitions::isValidNetID(value) &&
                   (myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TAZ, value, false) != nullptr);
        default:
            return isCommonAttributeValid(key, value);
    }
}


bool GNETAZRelData::isAttributeEnabled(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return false;
        default:
            return true;
    }
}


std::string
GNETAZRelData::getPopUpID() const {
    return getTagStr();
}


std::string
GNETAZRelData::getHierarchyName() const {
    if (getParentAdditionals().size() == 1) {
        return getTagStr() + ": " + getParentAdditionals().front()->getID();
    } else {
        return getTagStr() + ": " + getParentAdditionals().front()->getID() + "->" + getParentAdditionals().back()->getID();
    }
}


bool
GNETAZRelData::drawTAZRel() const {
    const auto& inspectedElements = myNet->getViewNet()->getInspectedElements();
    // first check supermode
    if (!myNet->getViewNet()->getEditModes().isCurrentSupermodeData()) {
        return false;
    }
    // check TAZRelFrame
    if (myNet->getViewNet()->getViewParent()->getTAZRelDataFrame()->shown()) {
        // check dataSet
        const GNEDataSet* dataSet = myNet->getViewNet()->getViewParent()->getTAZRelDataFrame()->getDataSetSelector()->getDataSet();
        if (dataSet && (myDataIntervalParent->getDataSetParent() != dataSet)) {
            return false;
        }
        // check interval
        const GNEDataInterval* dataInterval = myNet->getViewNet()->getViewParent()->getTAZRelDataFrame()->getIntervalSelector()->getDataInterval();
        if (dataInterval && (myDataIntervalParent != dataInterval)) {
            return false;
        }
    }
    // check if both draw TAZRel checkBox are disabled
    if (!myNet->getViewNet()->getDataViewOptions().TAZRelOnlyFrom() && !myNet->getViewNet()->getDataViewOptions().TAZRelOnlyTo()) {
        return false;
    }
    // check if we're inspecting a TAZ
    if ((myNet->getViewNet()->getEditModes().dataEditMode == DataEditMode::DATA_INSPECT) &&
            inspectedElements.isInspectingSingleElement() && (inspectedElements.getFirstAC()->getTagProperty()->getTag() == SUMO_TAG_TAZ)) {
        // ignore TAZRels with one TAZParent
        if (getParentAdditionals().size() == 2) {
            if ((getParentAdditionals().front() == inspectedElements.getFirstAC())  &&
                    myNet->getViewNet()->getDataViewOptions().TAZRelOnlyFrom()) {
                return true;
            } else if ((getParentAdditionals().back() == inspectedElements.getFirstAC())  &&
                       myNet->getViewNet()->getDataViewOptions().TAZRelOnlyTo()) {
                return true;
            } else {
                return false;
            }
        }
    }
    return true;
}


void
GNETAZRelData::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_FROM: {
            // replace first TAZ Parent
            replaceParentTAZElement(0, value);
            // update geometry
            updateGeometry();
            break;
        }
        case SUMO_ATTR_TO: {
            // replace second TAZ Parent
            replaceParentTAZElement(1, value);
            // update geometry
            updateGeometry();
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
