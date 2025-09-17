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
/// @file    GNEAdditionalFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
///
// The Widget for add additional elements
/****************************************************************************/

#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/elements/additional/GNEAdditionalHandler.h>
#include <netedit/frames/GNEAttributesEditor.h>
#include <netedit/frames/GNEConsecutiveSelector.h>
#include <netedit/frames/GNEViewObjectSelector.h>
#include <netedit/frames/GNESelectorParent.h>
#include <netedit/frames/GNETagSelector.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEAdditionalFrame.h"

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::E2MultilaneLegendModule - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::E2MultilaneLegendModule::E2MultilaneLegendModule(GNEFrame* frameParent) :
    MFXGroupBoxModule(frameParent, TL("Legend")) {
    // declare label
    FXLabel* legendLabel = nullptr;
    // edge candidate
    legendLabel = new FXLabel(getCollapsableFrame(), TL(" edge candidate"), 0, GUIDesignLabel(JUSTIFY_LEFT));
    legendLabel->setBackColor(MFXUtils::getFXColor(frameParent->getViewNet()->getVisualisationSettings().candidateColorSettings.possible));
    legendLabel->setTextColor(MFXUtils::getFXColor(RGBColor::WHITE));
    // last edge selected
    legendLabel = new FXLabel(getCollapsableFrame(), TL(" last edge selected"), 0, GUIDesignLabel(JUSTIFY_LEFT));
    legendLabel->setBackColor(MFXUtils::getFXColor(frameParent->getViewNet()->getVisualisationSettings().candidateColorSettings.target));
    // edge selected
    legendLabel = new FXLabel(getCollapsableFrame(), TL(" edge selected"), 0, GUIDesignLabel(JUSTIFY_LEFT));
    legendLabel->setBackColor(MFXUtils::getFXColor(frameParent->getViewNet()->getVisualisationSettings().candidateColorSettings.source));
    // edge disconnected
    legendLabel = new FXLabel(getCollapsableFrame(), TL(" edge disconnected"), 0, GUIDesignLabel(JUSTIFY_LEFT));
    legendLabel->setBackColor(MFXUtils::getFXColor(frameParent->getViewNet()->getVisualisationSettings().candidateColorSettings.conflict));
}


GNEAdditionalFrame::E2MultilaneLegendModule::~E2MultilaneLegendModule() {}


void
GNEAdditionalFrame::E2MultilaneLegendModule::showE2MultilaneLegend() {
    show();
}


void
GNEAdditionalFrame::E2MultilaneLegendModule::hideE2MultilaneLegend() {
    hide();
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::HelpCreationModule - methods
// ---------------------------------------------------------------------------

#define TLSX(string) std::string(gettext((string)))



GNEAdditionalFrame::HelpCreationModule::HelpCreationModule(GNEFrame* frameParent) :
    MFXGroupBoxModule(frameParent, TL("Help")) {
    // edge candidate
    myHelpLabel = new FXLabel(getCollapsableFrame(), "", 0, GUIDesignLabelFrameInformation);
    // fill map
    //addTLString(TL("-Requires EntryExitDetector\n")) +
    // E1
    myHelpMap[SUMO_TAG_INDUCTION_LOOP] = addTLString(TL("-Click over lane to create it"));
    // E1 Instant
    myHelpMap[SUMO_TAG_INSTANT_INDUCTION_LOOP] = addTLString(TL("-Click over lane to create it"));
    // E2
    myHelpMap[SUMO_TAG_LANE_AREA_DETECTOR] = addTLString(TL("-Click over lane to create it"));
    // E3
    myHelpMap[SUMO_TAG_ENTRY_EXIT_DETECTOR] = addTLString(TL("-Click over view to create it")) +
            addTLString(TL("-Requires at least one Entry\n and one Exit"));
    // E3 Entry
    myHelpMap[SUMO_TAG_DET_ENTRY] = addTLString(TL("-Requires EntryExitDetector\n parent\n")) +
                                    addTLString(TL("-Select EntryExitDetector\n before creating either\n clicking over one in view\n or by selecting from list"));
    // E3 Exit
    myHelpMap[SUMO_TAG_DET_EXIT] = addTLString(TL("-Requires EntryExitDetector\n parent\n")) +
                                   addTLString(TL("-Select EntryExitDetector\n before creating either\n clicking over one in view\n or by selecting from list"));
}


GNEAdditionalFrame::HelpCreationModule::~HelpCreationModule() {}


void
GNEAdditionalFrame::HelpCreationModule::showHelpCreationModule(SumoXMLTag XMLTag) {
    if (myHelpMap.count(XMLTag) > 0) {
        myHelpLabel->setText(myHelpMap.at(XMLTag).c_str());
        show();
    } else {
        hide();
    }
}


void
GNEAdditionalFrame::HelpCreationModule::hideHelpCreationModule() {
    hide();
}


std::string
GNEAdditionalFrame::HelpCreationModule::addTLString(const std::string& str) {
    return std::string(str.c_str());
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame: - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::GNEAdditionalFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, TL("Additionals")) {

    // create item Selector module for additionals
    myAdditionalTagSelector = new GNETagSelector(this, GNETagProperties::Type::ADDITIONALELEMENT, SUMO_TAG_BUS_STOP);

    // Create additional parameters
    myAdditionalAttributesEditor = new GNEAttributesEditor(this, GNEAttributesEditorType::EditorType::CREATOR);

    // Create selector parent
    mySelectorAdditionalParent = new GNESelectorParent(this);

    // Create selector child edges
    myViewObjetsSelector = new GNEViewObjectSelector(this);

    // Create list for E2Multilane lane selector
    myConsecutiveLaneSelector = new GNEConsecutiveSelector(this, false);

    // create help creation module
    myHelpCreationModule = new HelpCreationModule(this);

    // Create legend for E2 detector
    myE2MultilaneLegendModule = new E2MultilaneLegendModule(this);
}


GNEAdditionalFrame::~GNEAdditionalFrame() {
    // check if we have to delete base additional object
    if (myBaseAdditional) {
        delete myBaseAdditional;
    }
}


void
GNEAdditionalFrame::show() {
    // refresh tag selector
    myAdditionalTagSelector->refreshTagSelector();
    // reset last position
    myLastClickedPosition = Position::INVALID;
    // show frame
    GNEFrame::show();
}


bool
GNEAdditionalFrame::addAdditional(const GNEViewNetHelper::ViewObjectsSelector& viewObjects) {
    // first check that current selected additional is valid
    if (myAdditionalTagSelector->getCurrentTemplateAC() == nullptr) {
        myViewNet->setStatusBarText(TL("Current selected additional isn't valid."));
        return false;
    }
    // obtain tagproperty (only for improve code legibility)
    const auto& tagProperties = myAdditionalTagSelector->getCurrentTemplateAC()->getTagProperty();
    // check if toogle selection
    if ((viewObjects.getAttributeCarrierFront() == viewObjects.getLaneFront()) &&
            (myViewObjetsSelector->toggleSelectedLane(viewObjects.getLaneFront()))) {
        return true;
    }
    if (myViewObjetsSelector->toggleSelectedElement(viewObjects.getAttributeCarrierFront())) {
        return true;
    }
    // check if add a lane in consecutive lane selector
    if (tagProperties->getTag() == GNE_TAG_MULTI_LANE_AREA_DETECTOR) {
        return myConsecutiveLaneSelector->addLane(viewObjects.getLaneFront());
    }
    // disable rerouter elements (temporal)
    if ((tagProperties->getTag() == SUMO_TAG_INTERVAL) ||
            (tagProperties->getTag() == SUMO_TAG_DEST_PROB_REROUTE) ||
            (tagProperties->getTag() == SUMO_TAG_CLOSING_REROUTE) ||
            (tagProperties->getTag() == SUMO_TAG_CLOSING_LANE_REROUTE) ||
            (tagProperties->getTag() == SUMO_TAG_ROUTE_PROB_REROUTE) ||
            (tagProperties->getTag() == SUMO_TAG_PARKING_AREA_REROUTE)) {
        WRITE_WARNING(TL("Currently unsupported. Create rerouter elements using rerouter dialog"));
        return false;
    }
    // disable steps (temporal)
    if (tagProperties->getTag() == SUMO_TAG_STEP) {
        WRITE_WARNING(TL("Currently unsupported. Create VSS steps using VSS dialog"));
        return false;
    }
    // disable flows (temporal)
    if (tagProperties->getTag() == GNE_TAG_CALIBRATOR_FLOW) {
        WRITE_WARNING(TL("Currently unsupported. Create calibratorFlows using calibrator dialog"));
        return false;
    }
    // check last position
    if ((myViewNet->getPositionInformation() == myLastClickedPosition) && !myViewNet->getMouseButtonKeyPressed().shiftKeyPressed()) {
        WRITE_WARNING(TL("Shift + click to create two additionals in the same position"));
        return false;
    }
    // check if additional attributes are valid
    if (!myAdditionalAttributesEditor->checkAttributes(true)) {
        return false;
    }
    // reset base additional
    resetBaseAdditionalObject();
    // init base additional object
    if (!initBaseAdditionalObject(tagProperties, viewObjects)) {
        return false;
    }
    // parse common attributes
    if (!myViewObjetsSelector->fillSumoBaseObject(myBaseAdditional)) {
        return false;
    }
    // add basic attributes and values
    myAdditionalAttributesEditor->fillSumoBaseObject(myBaseAdditional);
    // declare additional handler
    GNEAdditionalHandler additionalHandler(myViewNet->getNet(), myBaseAdditional->hasStringAttribute(GNE_ATTR_ADDITIONAL_FILE) ?
                                           myBaseAdditional->getStringAttribute(GNE_ATTR_ADDITIONAL_FILE) : "",
                                           myViewNet->getViewParent()->getGNEAppWindows()->isUndoRedoAllowed());
    // build additional
    additionalHandler.parseSumoBaseObject(myBaseAdditional);
    // Refresh additional Parent Selector (For additionals that have a limited number of children)
    mySelectorAdditionalParent->refreshSelectorParentModule();
    // clear selected view objects
    myViewObjetsSelector->clearSelection();
    myAdditionalAttributesEditor->refreshAttributesEditor();
    return true;
}


GNETagSelector*
GNEAdditionalFrame::getAdditionalTagSelector() const {
    return myAdditionalTagSelector;
}


GNEConsecutiveSelector*
GNEAdditionalFrame::getConsecutiveLaneSelector() const {
    return myConsecutiveLaneSelector;
}


GNEAttributesEditor*
GNEAdditionalFrame::getAttributesEditor() const {
    return myAdditionalAttributesEditor;
}


GNEViewObjectSelector*
GNEAdditionalFrame::getViewObjetsSelector() const {
    return myViewObjetsSelector;
}


bool
GNEAdditionalFrame::createPath(const bool /* useLastRoute */) {
    // obtain tagproperty (only for improve code legibility)
    const auto tagProperty = myAdditionalTagSelector->getCurrentTemplateAC()->getTagProperty();
    // first check that current tag is valid (currently only for E2 multilane detectors)
    if (tagProperty->getTag() == GNE_TAG_MULTI_LANE_AREA_DETECTOR) {
        // now check number of lanes
        if (myConsecutiveLaneSelector->getLanePath().size() < 2) {
            WRITE_WARNING(TL("E2 multilane detectors need at least two consecutive lanes"));
        } else {
            // reset base object
            resetBaseAdditionalObject();
            // set tag
            myBaseAdditional->setTag(SUMO_TAG_LANE_AREA_DETECTOR);
            // get attributes and values
            myAdditionalAttributesEditor->fillSumoBaseObject(myBaseAdditional);
            // add lane IDs
            myBaseAdditional->addStringListAttribute(SUMO_ATTR_LANES, myConsecutiveLaneSelector->getLaneIDPath());
            // set positions
            myBaseAdditional->addDoubleAttribute(SUMO_ATTR_POSITION, myConsecutiveLaneSelector->getLanePath().front().second);
            myBaseAdditional->addDoubleAttribute(SUMO_ATTR_ENDPOS, myConsecutiveLaneSelector->getLanePath().back().second);
            // parse common attributes
            if (myViewObjetsSelector->fillSumoBaseObject(myBaseAdditional)) {
                // show warning dialogbox and stop check if input parameters are valid
                if (myAdditionalAttributesEditor->checkAttributes(true)) {
                    // declare additional handler
                    GNEAdditionalHandler additionalHandler(myViewNet->getNet(), myBaseAdditional->hasStringAttribute(GNE_ATTR_ADDITIONAL_FILE) ?
                                                           myBaseAdditional->getStringAttribute(GNE_ATTR_ADDITIONAL_FILE) : "",
                                                           myViewNet->getViewParent()->getGNEAppWindows()->isUndoRedoAllowed());
                    // build additional
                    additionalHandler.parseSumoBaseObject(myBaseAdditional);
                    // Refresh additional Parent Selector (For additionals that have a limited number of children)
                    mySelectorAdditionalParent->refreshSelectorParentModule();
                    // abort E2 creation
                    myConsecutiveLaneSelector->abortPathCreation();
                    return true;
                }
            }
        }
    }
    return false;
}


void
GNEAdditionalFrame::tagSelected() {
    // get template AC
    const auto templateAC = myAdditionalTagSelector->getCurrentTemplateAC();
    if (templateAC) {
        // show parameters
        myAdditionalAttributesEditor->showAttributesEditor(templateAC, true);
        // Show myAdditionalFrameParent if we're adding an slave element
        if (templateAC->getTagProperty()->isChild()) {
            mySelectorAdditionalParent->showSelectorParentModule(templateAC->getTagProperty()->getXMLParentTags());
        } else {
            mySelectorAdditionalParent->hideSelectorParentModule();
        }
        // Show EdgesSelector if we're adding an additional that own the attribute SUMO_ATTR_EDGES
        if (templateAC->getTagProperty()->hasAttribute(SUMO_ATTR_EDGES)) {
            myViewObjetsSelector->showNetworkElementsSelector(SUMO_TAG_EDGE, SUMO_ATTR_EDGES);
        } else {
            myViewObjetsSelector->hideNetworkElementsSelector();
        }
        // show help creation modul
        myHelpCreationModule->showHelpCreationModule(templateAC->getTagProperty()->getTag());
        // check if we must show consecutive lane selector
        if (templateAC->getTagProperty()->getTag() == GNE_TAG_MULTI_LANE_AREA_DETECTOR) {
            myConsecutiveLaneSelector->showConsecutiveLaneSelectorModule();
            myE2MultilaneLegendModule->showE2MultilaneLegend();
            myViewObjetsSelector->hideNetworkElementsSelector();
            // recompute network
            myViewNet->getNet()->computeNetwork(myViewNet->getViewParent()->getGNEAppWindows());
        } else if (templateAC->getTagProperty()->hasAttribute(SUMO_ATTR_LANES)) {
            myConsecutiveLaneSelector->hideConsecutiveLaneSelectorModule();
            myE2MultilaneLegendModule->hideE2MultilaneLegend();
            myViewObjetsSelector->showNetworkElementsSelector(SUMO_TAG_LANE, SUMO_ATTR_LANES);
        } else {
            myConsecutiveLaneSelector->hideConsecutiveLaneSelectorModule();
            myE2MultilaneLegendModule->hideE2MultilaneLegend();
        }
        // reset last position
        myLastClickedPosition = Position::INVALID;
    } else {
        // hide all modules if additional isn't valid
        myAdditionalAttributesEditor->hideAttributesEditor();
        mySelectorAdditionalParent->hideSelectorParentModule();
        myViewObjetsSelector->hideNetworkElementsSelector();
        myConsecutiveLaneSelector->hideConsecutiveLaneSelectorModule();
        myHelpCreationModule->hideHelpCreationModule();
        myE2MultilaneLegendModule->hideE2MultilaneLegend();
    }
}


void
GNEAdditionalFrame::resetBaseAdditionalObject() {
    // check if baseAdditional exist, and if yes, delete it
    if (myBaseAdditional) {
        // go to base additional root
        while (myBaseAdditional->getParentSumoBaseObject()) {
            myBaseAdditional = myBaseAdditional->getParentSumoBaseObject();
        }
        // delete baseAdditional (and all children)
        delete myBaseAdditional;
        // reset baseAdditional
        myBaseAdditional = nullptr;
    }
    // create an new base additional
    myBaseAdditional = new CommonXMLStructure::SumoBaseObject(nullptr);
}


bool
GNEAdditionalFrame::initBaseAdditionalObject(const GNETagProperties* tagProperty, const GNEViewNetHelper::ViewObjectsSelector& viewObjects) {
    // declare tag for base additional
    auto baseAdditionalTag = tagProperty->getTag();
    // check if tag has to be updated
    if (baseAdditionalTag == GNE_TAG_MULTI_LANE_AREA_DETECTOR) {
        baseAdditionalTag = SUMO_TAG_LANE_AREA_DETECTOR;
    } else if (baseAdditionalTag == GNE_TAG_CALIBRATOR_FLOW) {
        baseAdditionalTag = SUMO_TAG_FLOW;
    }
    // update view objects parents
    viewObjects.fillSumoBaseObject(myBaseAdditional);
    // check if additional is child of other element
    if (tagProperty->isChild()) {
        // check if we clicked over a parent
        SumoXMLTag parentTag = SUMO_TAG_NOTHING;
        for (const auto& pTag : tagProperty->getXMLParentTags()) {
            if (myBaseAdditional->hasParentID(pTag)) {
                parentTag = pTag;
            }
        }
        // update selector additional parent
        if (parentTag != SUMO_TAG_NOTHING) {
            // update parent additional selected
            mySelectorAdditionalParent->setIDSelected(myBaseAdditional->getParentID(parentTag));
        }
        // continue depending of parents
        if (mySelectorAdditionalParent->getIdSelected().empty()) {
            std::string messageError = toString(tagProperty->getXMLParentTags().front());
            if (tagProperty->getXMLParentTags().size() > 1) {
                const int numParents = (int)tagProperty->getXMLParentTags().size();
                messageError.clear();
                for (int i = 0; i < numParents; i++) {
                    messageError.append(toString(tagProperty->getXMLParentTags().at(i)));
                    if (i == numParents - 2) {
                        messageError.append(" or ");
                    } else if (i < (numParents - 2)) {
                        messageError.append(", ");
                    }
                }
            }
            WRITE_WARNING(TLF("A % must be selected before insertion of %.", messageError, tagProperty->getTagStr()));
            return false;
        } else {
            // set parent tag // POSSIBLE ERROR WITH ACCESS AND BUSSTOPS!
            myBaseAdditional->setTag(tagProperty->getXMLParentTags().front());
            // add ID
            myBaseAdditional->addStringAttribute(SUMO_ATTR_ID, mySelectorAdditionalParent->getIdSelected());
            // create base additional again as child of current base additional
            myBaseAdditional = new CommonXMLStructure::SumoBaseObject(myBaseAdditional);
        }
    }
    // set baseAdditional tag
    myBaseAdditional->setTag(baseAdditionalTag);
    // Obtain position as the clicked position over view
    const Position viewPosSnapped = myViewNet->snapToActiveGrid(myViewNet->getPositionInformation());
    // add position and X-Y-Z attributes
    myBaseAdditional->addPositionAttribute(SUMO_ATTR_POSITION, viewPosSnapped);
    myBaseAdditional->addDoubleAttribute(SUMO_ATTR_X, viewPosSnapped.x());
    myBaseAdditional->addDoubleAttribute(SUMO_ATTR_Y, viewPosSnapped.y());
    myBaseAdditional->addDoubleAttribute(SUMO_ATTR_Z, viewPosSnapped.z());
    // check if add edge attributes
    if (tagProperty->hasAttribute(SUMO_ATTR_EDGE)) {
        if (viewObjects.getEdgeFront() == nullptr) {
            return false;
        } else {
            myBaseAdditional->addStringAttribute(SUMO_ATTR_EDGE, viewObjects.getEdgeFront()->getID());
            // Obtain position of the mouse over lane (limited over grid)
            const auto firstLane = viewObjects.getEdgeFront()->getChildLanes().front();
            const double mousePositionOverLane = firstLane->getLaneShape().nearest_offset_to_point2D(viewPosSnapped) / firstLane->getLengthGeometryFactor();
            myBaseAdditional->addDoubleAttribute(SUMO_ATTR_POSITION, mousePositionOverLane);
        }
    } else if (tagProperty->getTag() == SUMO_TAG_VAPORIZER) {
        // special case for vaporizers
        if (viewObjects.getEdgeFront() == nullptr) {
            return false;
        } else {
            myBaseAdditional->addStringAttribute(SUMO_ATTR_ID, viewObjects.getEdgeFront()->getID());
        }
    }
    // check if add lane attributes
    if (tagProperty->hasAttribute(SUMO_ATTR_LANE)) {
        if (viewObjects.getLaneFront() == nullptr) {
            return false;
        } else {
            myBaseAdditional->addStringAttribute(SUMO_ATTR_LANE, viewObjects.getLaneFront()->getID());
            myBaseAdditional->addDoubleAttribute(GNE_ATTR_LANELENGTH, viewObjects.getLaneFront()->getLaneShapeLength() / viewObjects.getLaneFront()->getLengthGeometryFactor());
            // Obtain position of the mouse over lane (limited over grid)
            const double mousePositionOverLane = viewObjects.getLaneFront()->getLaneShape().nearest_offset_to_point2D(viewPosSnapped) / viewObjects.getLaneFront()->getLengthGeometryFactor();
            // special case for access
            if (tagProperty->getTag() == SUMO_TAG_ACCESS) {
                myBaseAdditional->addStringAttribute(SUMO_ATTR_POSITION, toString(mousePositionOverLane));
            } else {
                myBaseAdditional->addDoubleAttribute(SUMO_ATTR_POSITION, mousePositionOverLane);
            }
        }
    }
    // BaseAdditional created, then return true
    return true;
}

/****************************************************************************/
