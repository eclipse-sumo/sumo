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
/// @file    GNEGenericData.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// A abstract class for generic datas
/****************************************************************************/

#include <netedit/GNENet.h>
#include <netedit/GNETagPropertiesDatabase.h>
#include <netedit/GNEViewParent.h>
#include <netedit/frames/common/GNESelectorFrame.h>
#include <netedit/frames/data/GNEEdgeDataFrame.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>

#include "GNEDataInterval.h"
#include "GNEGenericData.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNEGenericData::GNEGenericData(SumoXMLTag tag, GNENet* net) :
    GNEAttributeCarrier(tag, net, "", true),
    GUIGlObject(net->getTagPropertiesDatabase()->getTagProperty(tag, true)->getGLType(), "",
                GUIIconSubSys::getIcon(net->getTagPropertiesDatabase()->getTagProperty(tag, true)->getGUIIcon())),
    GNEPathElement(GNEPathElement::Options::DATA_ELEMENT),
    myDataIntervalParent(nullptr) {
}


GNEGenericData::GNEGenericData(const SumoXMLTag tag, GNEDataInterval* dataIntervalParent, const Parameterised::Map& parameters) :
    GNEAttributeCarrier(tag, dataIntervalParent->getNet(), dataIntervalParent->getFilename(), false),
    GUIGlObject(dataIntervalParent->getNet()->getTagPropertiesDatabase()->getTagProperty(tag, true)->getGLType(), dataIntervalParent->getID(),
                GUIIconSubSys::getIcon(dataIntervalParent->getNet()->getTagPropertiesDatabase()->getTagProperty(tag, true)->getGUIIcon())),
    GNEPathElement(GNEPathElement::Options::DATA_ELEMENT),
    Parameterised(parameters),
    myDataIntervalParent(dataIntervalParent) {
}


GNEGenericData::~GNEGenericData() {}


GNEHierarchicalElement*
GNEGenericData::getHierarchicalElement() {
    return this;
}


GNEMoveElement*
GNEGenericData::getMoveElement() const {
    return nullptr;
}


Parameterised*
GNEGenericData::getParameters() {
    return this;
}


const Parameterised*
GNEGenericData::getParameters() const {
    return this;
}


GUIGlObject*
GNEGenericData::getGUIGlObject() {
    return this;
}


const GUIGlObject*
GNEGenericData::getGUIGlObject() const {
    return this;
}


GNEDataInterval*
GNEGenericData::getDataIntervalParent() const {
    return myDataIntervalParent;
}


void
GNEGenericData::drawAttribute(const PositionVector& shape) const {
    if ((myTagProperty->getTag() == GNE_TAG_EDGEREL_SINGLE) && (shape.length() > 0)) {
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
GNEGenericData::checkDrawFromContour() const {
    return false;
}


bool
GNEGenericData::checkDrawToContour() const {
    return false;
}


bool
GNEGenericData::checkDrawRelatedContour() const {
    // check opened popup
    if (myNet->getViewNet()->getPopup()) {
        return myNet->getViewNet()->getPopup()->getGLObject() == this;
    }
    return false;
}


bool
GNEGenericData::checkDrawOverContour() const {
    return false;
}


bool
GNEGenericData::checkDrawDeleteContour() const {
    // get edit modes
    const auto& editModes = myNet->getViewNet()->getEditModes();
    // check if we're in delete mode
    if (editModes.isCurrentSupermodeData() && (editModes.dataEditMode == DataEditMode::DATA_DELETE)) {
        return myNet->getViewNet()->checkOverLockedElement(this, mySelected);
    } else {
        return false;
    }
}


bool
GNEGenericData::checkDrawDeleteContourSmall() const {
    return false;
}


bool
GNEGenericData::checkDrawSelectContour() const {
    // get edit modes
    const auto& editModes = myNet->getViewNet()->getEditModes();
    // check if we're in select mode
    if (editModes.isCurrentSupermodeData() && (editModes.dataEditMode == DataEditMode::DATA_SELECT)) {
        return myNet->getViewNet()->checkOverLockedElement(this, mySelected);
    } else {
        return false;
    }
}


bool
GNEGenericData::checkDrawMoveContour() const {
    return false;
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
    // create popup
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, this);
    // build common options
    buildPopUpMenuCommonOptions(ret, app, myNet->getViewNet(), myTagProperty->getTag(), mySelected);
    // show option to open additional dialog
    if (myTagProperty->hasDialog()) {
        GUIDesigns::buildFXMenuCommand(ret, (TLF("Open % Dialog", getTagStr())).c_str(), getACIcon(), &parent, MID_OPEN_ADDITIONAL_DIALOG);
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
    for (const auto& tagProperty : myTagProperty->getAttributeProperties()) {
        // Add attribute and set it dynamic if aren't unique
        if (tagProperty->isUnique()) {
            ret->mkItem(tagProperty->getAttrStr().c_str(), false, getAttribute(tagProperty->getAttr()));
        } else {
            ret->mkItem(tagProperty->getAttrStr().c_str(), true, getAttribute(tagProperty->getAttr()));
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


Position
GNEGenericData::getAttributePosition(SumoXMLAttr key) const {
    return getCommonAttributePosition(key);
}


PositionVector
GNEGenericData::getAttributePositionVector(SumoXMLAttr key) const {
    return getCommonAttributePositionVector(key);
}


bool
GNEGenericData::isPathElementSelected() const {
    return mySelected;
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
    if ((toolBar.getGenericDataType() != SUMO_TAG_NOTHING) && (toolBar.getGenericDataType() != myTagProperty->getTag())) {
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
    auto newEdge = myNet->getAttributeCarriers()->retrieveEdge(value);
    GNEHierarchicalElement::updateParent(this, 0, newEdge);
}


void
GNEGenericData::replaceLastParentEdge(const std::string& value) {
    auto newEdge = myNet->getAttributeCarriers()->retrieveEdge(value);
    GNEHierarchicalElement::updateParent(this, (int)getParentEdges().size() - 1, newEdge);
}


void
GNEGenericData::replaceParentTAZElement(const int index, const std::string& value) {
    std::vector<GNEAdditional*> newTAZs = getParentAdditionals();
    auto TAZ = myNet->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_TAZ, value);
    // continue depending of index and number of TAZs
    if (index == 0) {
        if (newTAZs.size() == 2) {
            if (newTAZs.at(1)->getID() == value) {
                newTAZs = {TAZ};
            } else {
                newTAZs[0] = TAZ;
            }
        } else if (newTAZs.at(0) != TAZ) {
            newTAZs = {TAZ, newTAZs.at(0)};
        }
    } else if (index == 1) {
        if (newTAZs.size() == 2) {
            if (newTAZs.at(0)->getID() == value) {
                newTAZs = {TAZ};
            } else {
                newTAZs[1] = TAZ;
            }
        } else if (newTAZs.at(0) != TAZ) {
            newTAZs = {newTAZs.at(0), TAZ};
        }
    } else {
        throw ProcessError(TL("Invalid index"));
    }
    GNEHierarchicalElement::updateParents(this, newTAZs);
}


std::string
GNEGenericData::getPartialID() const {
    return getDataIntervalParent()->getDataSetParent()->getID() + "[" +
           getDataIntervalParent()->getAttribute(SUMO_ATTR_BEGIN) + "," +
           getDataIntervalParent()->getAttribute(SUMO_ATTR_END) + "]:";
}

/****************************************************************************/
