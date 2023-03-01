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
/// @file    GNEGenericData.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// A abstract class for data sets
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/data/GNEGenericData.h>
#include <netedit/frames/common/GNESelectorFrame.h>
#include <netedit/frames/data/GNEEdgeDataFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEGenericData.h"
#include "GNEDataInterval.h"


// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEGenericData - methods
// ---------------------------------------------------------------------------

GNEGenericData::GNEGenericData(const SumoXMLTag tag, FXIcon* icon, const GUIGlObjectType type, GNEDataInterval* dataIntervalParent,
                               const Parameterised::Map& parameters,
                               const std::vector<GNEJunction*>& junctionParents,
                               const std::vector<GNEEdge*>& edgeParents,
                               const std::vector<GNELane*>& laneParents,
                               const std::vector<GNEAdditional*>& additionalParents,
                               const std::vector<GNEDemandElement*>& demandElementParents,
                               const std::vector<GNEGenericData*>& genericDataParents) :
    GNEPathManager::PathElement(type, dataIntervalParent->getID(), icon, GNEPathManager::PathElement::Options::DATA_ELEMENT),
    Parameterised(parameters),
    GNEHierarchicalElement(dataIntervalParent->getNet(), tag, junctionParents, edgeParents, laneParents, additionalParents, demandElementParents, genericDataParents),
    myDataIntervalParent(dataIntervalParent) {
}


GNEGenericData::~GNEGenericData() {}


GUIGlObject*
GNEGenericData::getGUIGlObject() {
    return this;
}


GNEDataInterval*
GNEGenericData::getDataIntervalParent() const {
    return myDataIntervalParent;
}


void
GNEGenericData::drawAttribute(const PositionVector& shape) const {
    if ((myTagProperty.getTag() == GNE_TAG_EDGEREL_SINGLE) && (shape.length() > 0)) {
        // obtain pointer to edge data frame (only for code legibly)
        const GNEEdgeDataFrame* edgeDataFrame = myDataIntervalParent->getNet()->getViewNet()->getViewParent()->getEdgeDataFrame();
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
                GLHelper::pushMatrix();
                GLHelper::drawText(value, centerPosition, GLO_MAX, 2, RGBColor::BLUE);
                // pop draw matrix
                GLHelper::popMatrix();
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


GUIGLObjectPopupMenu*
GNEGenericData::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    // build header
    buildPopupHeader(ret, app);
    // build menu command for center button and copy cursor position to clipboard
    buildCenterPopupEntry(ret);
    buildPositionCopyEntry(ret, app);
    // buld menu commands for names
    GUIDesigns::buildFXMenuCommand(ret, TL("Copy ") + getTagStr() + TL(" name to clipboard"), nullptr, ret, MID_COPY_NAME);
    GUIDesigns::buildFXMenuCommand(ret, TL("Copy ") + getTagStr() + TL(" typed name to clipboard"), nullptr, ret, MID_COPY_TYPED_NAME);
    new FXMenuSeparator(ret);
    // build selection and show parameters menu
    myDataIntervalParent->getNet()->getViewNet()->buildSelectionACPopupEntry(ret, this);
    buildShowParamsPopupEntry(ret);
    // show option to open additional dialog
    if (myTagProperty.hasDialog()) {
        GUIDesigns::buildFXMenuCommand(ret, (TL("Open ") + getTagStr() + TL(" Dialog")).c_str(), getACIcon(), &parent, MID_OPEN_ADDITIONAL_DIALOG);
        new FXMenuSeparator(ret);
    } else {
        GUIDesigns::buildFXMenuCommand(ret, (TL("Cursor position in view: ") + toString(getPositionInView().x()) + "," + toString(getPositionInView().y())).c_str(), nullptr, nullptr, 0);
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
GNEGenericData::deleteGLObject() {
    myNet->deleteGenericData(this, myNet->getViewNet()->getUndoList());
}


void
GNEGenericData::selectGLObject() {
    if (isAttributeCarrierSelected()) {
        unselectAttributeCarrier();
    } else {
        selectAttributeCarrier();
    }
    // update information label
    myNet->getViewNet()->getViewParent()->getSelectorFrame()->getSelectionInformation()->updateInformationLabel();
}


void
GNEGenericData::updateGLObject() {
    updateGeometry();
}


bool
GNEGenericData::isPathElementSelected() const {
    return mySelected;
}


double
GNEGenericData::getPathElementDepartValue() const {
    return 0;
}


Position
GNEGenericData::getPathElementDepartPos() const {
    return Position();
}


double
GNEGenericData::getPathElementArrivalValue() const {
    return 0;
}


Position
GNEGenericData::getPathElementArrivalPos() const {
    return Position();
}


const Parameterised::Map&
GNEGenericData::getACParametersMap() const {
    return getParametersMap();
}

// ---------------------------------------------------------------------------
// GNEGenericData - protected methods
// ---------------------------------------------------------------------------

void
GNEGenericData::drawFilteredAttribute(const GUIVisualizationSettings& s, const PositionVector& laneShape, const std::string& attribute, const GNEDataInterval* dataIntervalParent) const {
    if ((myDataIntervalParent == dataIntervalParent) && (getParametersMap().count(attribute) > 0)) {
        const Position pos = laneShape.positionAtOffset2D(laneShape.length2D() * 0.5);
        const double rot = laneShape.rotationDegreeAtOffset(laneShape.length2D() * 0.5);
        // Add a draw matrix for details
        GLHelper::pushMatrix();
        // draw value
        GLHelper::drawText(getParameter(attribute), pos, GLO_MAX - 1, 2, RGBColor::BLACK, s.getTextAngle(rot + 90));
        // pop draw matrix
        GLHelper::popMatrix();
    }
}


bool
GNEGenericData::isVisibleInspectDeleteSelect() const {
    // get toolbar
    const GNEViewNetHelper::IntervalBar& toolBar = myNet->getViewNet()->getIntervalBar();
    // declare flag
    bool draw = true;
    // check filter by generic data type
    if ((toolBar.getGenericDataType() != SUMO_TAG_NOTHING) && (toolBar.getGenericDataType() != myTagProperty.getTag())) {
        draw = false;
    }
    // check filter by data set
    if (toolBar.getDataSet() && (toolBar.getDataSet() != myDataIntervalParent->getDataSetParent())) {
        draw = false;
    }
    // check filter by begin
    if ((toolBar.getBegin() != INVALID_DOUBLE) && (toolBar.getBegin() > myDataIntervalParent->getAttributeDouble(SUMO_ATTR_BEGIN))) {
        draw = false;
    }
    // check filter by end
    if ((toolBar.getEnd() != INVALID_DOUBLE) && (toolBar.getEnd() < myDataIntervalParent->getAttributeDouble(SUMO_ATTR_END))) {
        draw = false;
    }
    // check filter by attribute
    if ((toolBar.getParameter().size() > 0) && (getParametersMap().count(toolBar.getParameter()) == 0)) {
        draw = false;
    }
    // return flag
    return draw;
}

void
GNEGenericData::replaceFirstParentEdge(const std::string& value) {
    std::vector<GNEEdge*> parentEdges = getParentEdges();
    parentEdges[0] = myNet->getAttributeCarriers()->retrieveEdge(value);
    // replace parent edges
    replaceParentElements(this, parentEdges);
}


void
GNEGenericData::replaceLastParentEdge(const std::string& value) {
    std::vector<GNEEdge*> parentEdges = getParentEdges();
    parentEdges[(int)parentEdges.size() - 1] = myNet->getAttributeCarriers()->retrieveEdge(value);
    // replace parent edges
    replaceParentElements(this, parentEdges);
}


void
GNEGenericData::replaceParentTAZElement(const int index, const std::string& value) {
    std::vector<GNEAdditional*> parentTAZElements = getParentAdditionals();
    auto TAZ = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TAZ, value);
    // continue depending of index and number of TAZs
    if (index == 0) {
        if (parentTAZElements.size() == 2) {
            if (parentTAZElements.at(1)->getID() == value) {
                parentTAZElements = {TAZ};
            } else {
                parentTAZElements[0] = TAZ;
            }
        } else if (parentTAZElements.at(0) != TAZ) {
            parentTAZElements = {TAZ, parentTAZElements.at(0)};
        }
    } else if (index == 1) {
        if (parentTAZElements.size() == 2) {
            if (parentTAZElements.at(0)->getID() == value) {
                parentTAZElements = {TAZ};
            } else {
                parentTAZElements[1] = TAZ;
            }
        } else if (parentTAZElements.at(0) != TAZ) {
            parentTAZElements = {parentTAZElements.at(0), TAZ};
        }
    } else {
        throw ProcessError(TL("Invalid index"));
    }
    // replace parent TAZElements
    replaceParentElements(this, parentTAZElements);
}


std::string
GNEGenericData::getPartialID() const {
    return getDataIntervalParent()->getDataSetParent()->getID() + "[" +
           getDataIntervalParent()->getAttribute(SUMO_ATTR_BEGIN) + "," +
           getDataIntervalParent()->getAttribute(SUMO_ATTR_END) + "]:";
}

/****************************************************************************/
