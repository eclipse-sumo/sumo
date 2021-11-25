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
/// @file    GNETAZRelData.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// class for TAZ relation data
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/frames/data/GNETAZRelDataFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNETAZRelData.h"
#include "GNEDataInterval.h"


// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNETAZRelData - methods
// ---------------------------------------------------------------------------

GNETAZRelData::GNETAZRelData(GNEDataInterval* dataIntervalParent, GNETAZElement* fromTAZ, GNETAZElement* toTAZ,
                             const std::map<std::string, std::string>& parameters) :
    GNEGenericData(SUMO_TAG_TAZREL, GLO_TAZRELDATA, dataIntervalParent, parameters,
{}, {}, {}, {}, {}, {fromTAZ, toTAZ}, {}, {}),
myLastWidth(0) {
    // update geometry
    updateGeometry();
}


GNETAZRelData::GNETAZRelData(GNEDataInterval* dataIntervalParent, GNETAZElement* TAZ,
                             const std::map<std::string, std::string>& parameters) :
    GNEGenericData(SUMO_TAG_TAZREL, GLO_TAZRELDATA, dataIntervalParent, parameters,
{}, {}, {}, {}, {}, {TAZ}, {}, {}),
myLastWidth(0) {
    // update geometry
    updateGeometry();
}


GNETAZRelData::~GNETAZRelData() {}


const RGBColor&
GNETAZRelData::getColor() const {
    return myColor;
}


double
GNETAZRelData::getColorValue(const GUIVisualizationSettings& s, int activeScheme) const {
    switch (activeScheme) {
        case 0:
            return 0;
        case 1:
            return isAttributeCarrierSelected();
        case 2:
            return 0; // setfunctional color const GNETAZElement* TAZA = getParentTAZElements().front();
        case 3:
            return 0; // setfunctional color const GNETAZElement* TAZA = getParentTAZElements().back();
        case 4:
            // by numerical attribute value
            try {
                return StringUtils::toDouble(getParameter(s.relDataAttr, "-1"));
            } catch (NumberFormatException&) {
                return -1;
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
        return isVisibleInspectDeleteSelect();
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
    // get both TAZs
    const GNETAZElement* TAZA = getParentTAZElements().front();
    const GNETAZElement* TAZB = getParentTAZElements().back();
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
        ring.add(TAZA->getTAZElementShape().front());
        myTAZRelGeometry.updateGeometry(ring);
        // move ringCenter to center
        ringCenter.add(TAZA->getAttributePosition(SUMO_ATTR_CENTER));
        myTAZRelGeometryCenter.updateGeometry(ringCenter);
    } else {
        // calculate line betwen to TAZ centers
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
        Position posA = TAZA->getTAZElementShape().positionAtOffset2D(TAZA->getTAZElementShape().nearest_offset_to_point2D(middlePoint));
        Position posB = TAZB->getTAZElementShape().positionAtOffset2D(TAZB->getTAZElementShape().nearest_offset_to_point2D(middlePoint));
        // check positions
        if (posA == Position::INVALID) {
            posA = TAZA->getTAZElementShape().front();
        }
        if (posB == Position::INVALID) {
            posB = TAZB->getTAZElementShape().front();
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
}


Position
GNETAZRelData::getPositionInView() const {
    return getParentTAZElements().front()->getAttributePosition(SUMO_ATTR_CENTER);
}


void
GNETAZRelData::writeGenericData(OutputDevice& device) const {
    // open device
    device.openTag(SUMO_TAG_TAZREL);
    // write from
    device.writeAttr(SUMO_ATTR_FROM, getParentTAZElements().front()->getID());
    // write to
    device.writeAttr(SUMO_ATTR_TO, getParentTAZElements().back()->getID());
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
    // draw TAZRels
    if (drawTAZRel()) {
        // get flag for only draw contour
        const bool onlyDrawContour = !isGenericDataVisible();
        // push name (needed for getGUIGlObjectsUnderCursor(...)
        if (!onlyDrawContour) {
            GLHelper::pushName(getGlID());
        }
        // push matrix
        GLHelper::pushMatrix();
        // translate to front
        myNet->getViewNet()->drawTranslateFrontAttributeCarrier(this, GLO_TAZ + 1);
        // set color
        double val = getColorValue(s, s.dataColorer.getActive());
        myColor = s.dataColorer.getScheme().getColor(val);
        GLHelper::setColor(myColor);
        // check if update lastWidth
        const double width = onlyDrawContour ? 0.1 :  0.5 * s.tazRelWidthExaggeration;
        if (width != myLastWidth) {
            myLastWidth = width;
            // cast object (check this, is ugly)
            GNETAZRelData* TAZRelData = const_cast<GNETAZRelData*>(this);
            myNet->removeGLObjectFromGrid(TAZRelData);
            TAZRelData->updateGeometry();
            myNet->addGLObjectIntoGrid(TAZRelData);
        }
        // draw geometry
        if (onlyDrawContour) {
            // draw depending of TAZRelDrawing
            if (myNet->getViewNet()->getDataViewOptions().TAZRelDrawing()) {
                GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), myTAZRelGeometryCenter, width);
            } else {
                GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), myTAZRelGeometry, width);
            }
        } else {
            // draw depending of TAZRelDrawing
            const GUIGeometry& geom = (myNet->getViewNet()->getDataViewOptions().TAZRelDrawing()
                                       ? myTAZRelGeometryCenter : myTAZRelGeometry);
            GUIGeometry::drawGeometry(s, myNet->getViewNet()->getPositionInformation(), geom, width);
            GLHelper::drawTriangleAtEnd(
                *(geom.getShape().end() - 2),
                *(geom.getShape().end() - 1),
                1.5 + width, 1.5 + width, 0.5 + width);
        }
        // pop matrix
        GLHelper::popMatrix();
        // pop name
        if (!onlyDrawContour) {
            GLHelper::popName();
        }
        // check if dotted contours has to be drawn
        if (myNet->getViewNet()->isAttributeCarrierInspected(this)) {
            if (myNet->getViewNet()->getDataViewOptions().TAZRelDrawing()) {
                GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::INSPECT, s, myTAZRelGeometryCenter.getShape(), 0.5, 1, 1, 1);
            } else {
                GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::INSPECT, s, myTAZRelGeometry.getShape(), 0.5, 1, 1, 1);
            }
        }
        if (myNet->getViewNet()->getFrontAttributeCarrier() == this) {
            if (myNet->getViewNet()->getDataViewOptions().TAZRelDrawing()) {
                GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::FRONT, s, myTAZRelGeometryCenter.getShape(), 0.5, 1, 1, 1);
            } else {
                GUIDottedGeometry::drawDottedContourShape(GUIDottedGeometry::DottedContourType::FRONT, s, myTAZRelGeometry.getShape(), 0.5, 1, 1, 1);
            }
        }
    }
}


void
GNETAZRelData::computePathElement() {
    // nothing to compute
}


void
GNETAZRelData::drawPartialGL(const GUIVisualizationSettings& /*s*/, const GNELane* /*lane*/, const GNEPathManager::Segment* /*segment*/, const double /*offsetFront*/) const {
    // nothing to draw
}


void
GNETAZRelData::drawPartialGL(const GUIVisualizationSettings& /*s*/, const GNELane* /*fromLane*/, const GNELane* /*toLane*/, const GNEPathManager::Segment* /*segment*/, const double /*offsetFront*/) const {
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


double
GNETAZRelData::getExaggeration(const GUIVisualizationSettings& /*s*/) const {
    return 1;
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
            return getParentTAZElements().front()->getID();
        case SUMO_ATTR_FROM:
            return getParentTAZElements().front()->getID();
        case SUMO_ATTR_TO:
            return getParentTAZElements().back()->getID();
        case GNE_ATTR_DATASET:
            return myDataIntervalParent->getDataSetParent()->getID();
        case GNE_ATTR_SELECTED:
            return toString(isAttributeCarrierSelected());
        case GNE_ATTR_PARAMETERS:
            return getParametersStr();
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


double
GNETAZRelData::getAttributeDouble(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a double attribute of type '" + toString(key) + "'");
}


void
GNETAZRelData::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            undoList->changeAttribute(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNETAZRelData::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
            return SUMOXMLDefinitions::isValidNetID(value) &&
                   (myNet->getAttributeCarriers()->retrieveTAZElement(SUMO_TAG_TAZ, value, false) != nullptr);
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNETAZRelData::enableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    // Nothing to enable
}


void
GNETAZRelData::disableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    // Nothing to disable enable
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
    return getTagStr() + ": " + getParentTAZElements().front()->getID() + "->" + getParentTAZElements().back()->getID();
}


bool
GNETAZRelData::drawTAZRel() const {
    // first check supermode
    if (!myNet->getViewNet()->getEditModes().isCurrentSupermodeData()) {
        return false;
    }
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
    // check if we're inspecting a TAZ
    if ((myNet->getViewNet()->getEditModes().dataEditMode == DataEditMode::DATA_INSPECT) &&
            (myNet->getViewNet()->getInspectedAttributeCarriers().size() == 1) &&
            (myNet->getViewNet()->getInspectedAttributeCarriers().front()->getTagProperty().getTag() == SUMO_TAG_TAZ)) {
        // get TAZ
        const auto TAZ = myNet->getViewNet()->getInspectedAttributeCarriers().front();
        // ignore TAZRels with one TAZParent
        if (getParentTAZElements().size() == 2) {
            if ((getParentTAZElements().front() == TAZ)  &&
                    myNet->getViewNet()->getDataViewOptions().TAZRelOnlyFrom()) {
                return true;
            } else if ((getParentTAZElements().back() == TAZ)  &&
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
            // remove from grid
            myNet->removeGLObjectFromGrid(this);
            // check number of parent TAZ elements
            if ((getParentTAZElements().size() > 1) &&
                    (value == getParentTAZElements().at(1)->getID())) {
                // reset second TAZ
                replaceSecondParentTAZElement(SUMO_TAG_TAZ, "");
            }
            // change first TAZ
            replaceFirstParentTAZElement(SUMO_TAG_TAZ, value);
            // update geometry
            updateGeometry();
            // add into grid again
            myNet->addGLObjectIntoGrid(this);
            break;
        }
        case SUMO_ATTR_TO: {
            // remove from grid
            myNet->removeGLObjectFromGrid(this);
            if (value == getParentTAZElements().front()->getID()) {
                replaceSecondParentTAZElement(SUMO_TAG_TAZ, "");
            } else {
                // change second TAZ
                replaceSecondParentTAZElement(SUMO_TAG_TAZ, value);
            }
            // update geometry
            updateGeometry();
            // add into grid again
            myNet->addGLObjectIntoGrid(this);
            break;
        }
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


void
GNETAZRelData::toogleAttribute(SumoXMLAttr /*key*/, const bool /*value*/) {
    throw InvalidArgument("Nothing to enable");
}

/****************************************************************************/
