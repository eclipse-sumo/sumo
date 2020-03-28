/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNEGenericData.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// A abstract class for data sets
/****************************************************************************/
#include <config.h>

#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/data/GNEGenericData.h>
#include <netedit/frames/common/GNESelectorFrame.h>
#include <netedit/frames/data/GNEEdgeDataFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>

#include "GNEGenericData.h"
#include "GNEDataInterval.h"
#include "GNEDataSet.h"


// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEGenericData - methods
// ---------------------------------------------------------------------------

GNEGenericData::GNEGenericData(const SumoXMLTag tag, const GUIGlObjectType type, GNEDataInterval* dataIntervalParent,
        const std::map<std::string, std::string>& parameters,
        const std::vector<GNEEdge*>& edgeParents,
        const std::vector<GNELane*>& laneParents,
        const std::vector<GNEShape*>& shapeParents,
        const std::vector<GNEAdditional*>& additionalParents,
        const std::vector<GNEDemandElement*>& demandElementParents,
        const std::vector<GNEGenericData*>& genericDataParents,
        const std::vector<GNEEdge*>& edgeChildren,
        const std::vector<GNELane*>& laneChildren,
        const std::vector<GNEShape*>& shapeChildren,
        const std::vector<GNEAdditional*>& additionalChildren,
        const std::vector<GNEDemandElement*>& demandElementChildren,
        const std::vector<GNEGenericData*>& genericDataChildren) :
    GUIGlObject(type, dataIntervalParent->getID()),
    GNEAttributeCarrier(tag),
    Parameterised(ParameterisedAttrType::DOUBLE, parameters),
    GNEHierarchicalParentElements(this, edgeParents, laneParents, shapeParents, additionalParents, demandElementParents, genericDataParents),
    GNEHierarchicalChildElements(this, edgeChildren, laneChildren, shapeChildren, additionalChildren, demandElementChildren, genericDataChildren),
    myDataIntervalParent(dataIntervalParent) {
}


GNEGenericData::~GNEGenericData() {}


const std::string&
GNEGenericData::getID() const {
    return getMicrosimID();
}

GNEDataInterval*
GNEGenericData::getDataIntervalParent() const {
    return myDataIntervalParent;
}


const RGBColor&
GNEGenericData::getColor() const {
    // first check if we're in supermode demand
    if (myDataIntervalParent->getViewNet()->getEditModes().isCurrentSupermodeData()) {
        // special case for edgeDatas
        if (myTagProperty.getTag() == SUMO_TAG_MEANDATA_EDGE) {
            // obtain pointer to edge data frame (only for code legibly)
            const GNEEdgeDataFrame* edgeDataFrame = myDataIntervalParent->getViewNet()->getViewParent()->getEdgeDataFrame();
            // check if we have to filter generic data
            if (edgeDataFrame->shown()) {
                // get interval
                const GNEDataInterval* dataInterval = edgeDataFrame->getIntervalSelector()->getDataInterval();
                // get filtered attribute (can be empty)
                const std::string filteredAttribute = edgeDataFrame->getAttributeSelector()->getFilteredAttribute();
                // check interval
                if (dataInterval && (dataInterval == myDataIntervalParent) && (filteredAttribute.size() > 0)) {
                    // get maximum and minimum value
                    const double minimumValue = dataInterval->getMinimumGenericDataChildAttribute(filteredAttribute);
                    const double maximumValue = dataInterval->getMaximunGenericDataChildAttribute(filteredAttribute);
                    const double colorValue = getParametersMap().count(filteredAttribute) > 0 ? parse<double>(getParametersMap().at(filteredAttribute)) : 0;
                    // return scaled color
                    return edgeDataFrame->getAttributeSelector()->getScaledColor(minimumValue, maximumValue, colorValue);
                }
            }
        }
    }
    // return specific color
    return getSpecificColor();
}


bool
GNEGenericData::isGenericDataVisible() const {
    // first check if we're in supermode demand
    if (myDataIntervalParent->getViewNet()->getEditModes().isCurrentSupermodeData()) {
        // check if we're in common mode
        if ((myDataIntervalParent->getViewNet()->getEditModes().dataEditMode == DataEditMode::DATA_INSPECT) ||
            (myDataIntervalParent->getViewNet()->getEditModes().dataEditMode == DataEditMode::DATA_DELETE) ||
            (myDataIntervalParent->getViewNet()->getEditModes().dataEditMode == DataEditMode::DATA_SELECT)) {
            // obtain dataset, begin, end and attribute
            const std::string genericDataType = myDataIntervalParent->getViewNet()->getIntervalBar().getGenericDataTypeStr();
            const std::string dataSet = myDataIntervalParent->getViewNet()->getIntervalBar().getDataSetStr();
            const std::string begin = myDataIntervalParent->getViewNet()->getIntervalBar().getBeginStr();
            const std::string end = myDataIntervalParent->getViewNet()->getIntervalBar().getEndStr();
            const std::string attribute = myDataIntervalParent->getViewNet()->getIntervalBar().getAttributeStr();
            // chek genericData Type
            if (!genericDataType.empty() && (myTagProperty.getTagStr() != genericDataType)) {
                return false;
            }
            // chek data set
            if (!dataSet.empty() && myDataIntervalParent->getDataSetParent()->getID() != dataSet) {
                return false;
            }
            // chek begin
            if (!begin.empty() && parse<double>(begin) < myDataIntervalParent->getAttributeDouble(SUMO_ATTR_BEGIN)) {
                return false;
            }
            // chek end
            if (!end.empty() && parse<double>(end) > myDataIntervalParent->getAttributeDouble(SUMO_ATTR_END)) {
                return false;
            }
            // check attribute
            if (!attribute.empty() && getParametersMap().count(attribute) == 0) {
                return false;
            }
            // all checks ok, then return true
            return true;
        } else {
            // return specific function output
            return isVisible();
        }
    } else {
        // no supermode data
        return false;
    }
}


void 
GNEGenericData::drawAttribute(const PositionVector& shape) const {
    if ((myTagProperty.getTag() == SUMO_TAG_MEANDATA_EDGE) && (shape.length() > 0)) {
        // obtain pointer to edge data frame (only for code legibly)
        const GNEEdgeDataFrame* edgeDataFrame = myDataIntervalParent->getViewNet()->getViewParent()->getEdgeDataFrame();
        // check if we have to filter generic data
        if (edgeDataFrame->shown()) {
            // check attribute
            if ((edgeDataFrame->getAttributeSelector()->getFilteredAttribute().size() > 0) &&
                (getParametersMap().count(edgeDataFrame->getAttributeSelector()->getFilteredAttribute()) > 0)) {
                // get value
                const std::string value = getParametersMap().at(edgeDataFrame->getAttributeSelector()->getFilteredAttribute());
                // calculate center position
                const Position centerPosition = shape.positionAtOffset2D(shape.length2D() / 2);
                // Add a draw matrix
                glPushMatrix();
                GLHelper::drawText(value, centerPosition, GLO_MAX, 2, RGBColor::BLUE);
                // pop draw matrix
                glPopMatrix();
            }
        }
    }
}


bool
GNEGenericData::isGenericDataValid() const {
    return true;
}


std::string
GNEGenericData::getGenericDataProblem() const {
    return "";
}


void
GNEGenericData::fixGenericDataProblem() {
    throw InvalidArgument(getTagStr() + " cannot fix any problem");
}


GNEViewNet*
GNEGenericData::getViewNet() const {
    return myDataIntervalParent->getViewNet();
}


GUIGLObjectPopupMenu* 
GNEGenericData::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    // build header
    buildPopupHeader(ret, app);
    // build menu command for center button and copy cursor position to clipboard
    buildCenterPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    // buld menu commands for names
    new FXMenuCommand(ret, ("Copy " + getTagStr() + " name to clipboard").c_str(), nullptr, ret, MID_COPY_NAME);
    new FXMenuCommand(ret, ("Copy " + getTagStr() + " typed name to clipboard").c_str(), nullptr, ret, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(ret);
    // build selection and show parameters menu
    myDataIntervalParent->getViewNet()->buildSelectionACPopupEntry(ret, this);
    buildShowParamsPopupEntry(ret);
    // show option to open additional dialog
    if (myTagProperty.hasDialog()) {
        new FXMenuCommand(ret, ("Open " + getTagStr() + " Dialog").c_str(), getIcon(), &parent, MID_OPEN_ADDITIONAL_DIALOG);
        new FXMenuSeparator(ret);
    } else {
        new FXMenuCommand(ret, ("Cursor position in view: " + toString(getPositionInView().x()) + "," + toString(getPositionInView().y())).c_str(), nullptr, nullptr, 0);
    }
    return ret;
}


GUIParameterTableWindow* 
GNEGenericData::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& /* parent */) {
    // Create table
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this);
    // Iterate over attributes
    for (const auto& tagProperty : myTagProperty) {
        // Add attribute and set it dynamic if aren't unique
        if (tagProperty.isUnique()) {
            ret->mkItem(tagProperty.getAttrStr().c_str(), false, getAttribute(tagProperty.getAttr()));
        } else {
            ret->mkItem(tagProperty.getAttrStr().c_str(), true, getAttribute(tagProperty.getAttr()));
        }
    }
    // close building
    ret->closeBuilding();
    return ret;
}


void 
GNEGenericData::drawGL(const GUIVisualizationSettings& /*s*/) const {
    // currently unused
}


void
GNEGenericData::selectAttributeCarrier(bool changeFlag) {
    gSelected.select(getGlID());
    // add object into list of selected objects
    myDataIntervalParent->getViewNet()->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->addedLockedObject(getType());
    if (changeFlag) {
        mySelected = true;
    }
}


void
GNEGenericData::unselectAttributeCarrier(bool changeFlag) {
    gSelected.deselect(getGlID());
    // remove object of list of selected objects
    myDataIntervalParent->getViewNet()->getViewParent()->getSelectorFrame()->getLockGLObjectTypes()->removeLockedObject(getType());
    if (changeFlag) {
        mySelected = false;
    }
}


bool
GNEGenericData::isAttributeCarrierSelected() const {
    return mySelected;
}


bool
GNEGenericData::drawUsingSelectColor() const {
    if (mySelected && (myDataIntervalParent->getViewNet()->getEditModes().isCurrentSupermodeDemand())) {
        return true;
    } else {
        return false;
    }
}


std::string
GNEGenericData::generateChildID(SumoXMLTag /*childTag*/) {
    return "";
}

/****************************************************************************/
