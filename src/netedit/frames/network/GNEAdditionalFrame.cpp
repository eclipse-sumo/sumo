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
/// @file    GNEAdditionalFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
///
// The Widget for add additional elements
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/elements/additional/GNEAdditionalHandler.h>
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
    myAdditionalTagSelector = new GNETagSelector(this, GNETagProperties::TagType::ADDITIONALELEMENT, SUMO_TAG_BUS_STOP);

    // Create additional parameters
    myAdditionalAttributes = new GNEAttributesCreator(this);

    // Create netedit parameter
    myNeteditAttributes = new GNENeteditAttributes(this);

    // Create selector parent
    mySelectorAdditionalParent = new GNESelectorParent(this);

    // Create selector child edges
    myEdgesSelector = new GNENetworkSelector(this, GNENetworkSelector::Type::EDGE);

    // Create selector child lanes
    myLanesSelector = new GNENetworkSelector(this, GNENetworkSelector::Type::LANE);

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
    myViewNet->resetLastClickedPosition();
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
    // check if add or remove edge
    if (myEdgesSelector->isShown() && viewObjects.getEdgeFront()) {
        myEdgesSelector->toggleSelectedElement(viewObjects.getEdgeFront());
        return true;
    }
    // check if add or remove lane
    if (myLanesSelector->isShown() && viewObjects.getLaneFront()) {
        myLanesSelector->toggleSelectedElement(viewObjects.getLaneFront());
        return true;
    }
    // show warning dialogbox and stop check if input parameters are valid
    if (!myAdditionalAttributes->areValuesValid()) {
        myAdditionalAttributes->showWarningMessage();
        return false;
    }
    // obtain tagproperty (only for improve code legibility)
    const auto& tagProperties = myAdditionalTagSelector->getCurrentTemplateAC()->getTagProperty();
    // create base additional
    if (!createBaseAdditionalObject(tagProperties)) {
        return false;
    }
    // obtain attributes and values
    myAdditionalAttributes->getAttributesAndValues(myBaseAdditional, true);
    // fill netedit attributes
    if (!myNeteditAttributes->getNeteditAttributesAndValues(myBaseAdditional, viewObjects.getLaneFront())) {
        return false;
    }
    // If consecutive Lane Selector is enabled, it means that either we're selecting lanes or we're finished or we'rent started
    if (tagProperties.hasAttribute(SUMO_ATTR_EDGE) || (tagProperties.getTag() == SUMO_TAG_VAPORIZER)) {
        return buildAdditionalOverEdge(viewObjects.getLaneFront(), tagProperties);
    } else if (tagProperties.hasAttribute(SUMO_ATTR_LANE)) {
        return buildAdditionalOverLane(viewObjects.getLaneFront(), tagProperties);
    } else if (tagProperties.getTag() == GNE_TAG_MULTI_LANE_AREA_DETECTOR) {
        return myConsecutiveLaneSelector->addLane(viewObjects.getLaneFront());
    } else {
        return buildAdditionalOverView(tagProperties);
    }
}


GNENetworkSelector*
GNEAdditionalFrame::getEdgesSelector() const {
    return myEdgesSelector;
}


GNENetworkSelector*
GNEAdditionalFrame::getLanesSelector() const {
    return myLanesSelector;
}


GNEConsecutiveSelector*
GNEAdditionalFrame::getConsecutiveLaneSelector() const {
    return myConsecutiveLaneSelector;
}


GNENeteditAttributes*
GNEAdditionalFrame::getNeteditAttributes() const {
    return myNeteditAttributes;
}


bool
GNEAdditionalFrame::createPath(const bool /* useLastRoute */) {
    // obtain tagproperty (only for improve code legibility)
    const auto& tagProperty = myAdditionalTagSelector->getCurrentTemplateAC()->getTagProperty();
    // first check that current tag is valid (currently only for E2 multilane detectors)
    if (tagProperty.getTag() == GNE_TAG_MULTI_LANE_AREA_DETECTOR) {
        // now check number of lanes
        if (myConsecutiveLaneSelector->getLanePath().size() < 2) {
            WRITE_WARNING(TL("E2 multilane detectors need at least two consecutive lanes"));
        } else if (createBaseAdditionalObject(tagProperty)) {
            // get attributes and values
            myAdditionalAttributes->getAttributesAndValues(myBaseAdditional, true);
            // fill netedit attributes
            if (myNeteditAttributes->getNeteditAttributesAndValues(myBaseAdditional, nullptr)) {
                // Check if ID has to be generated
                if (tagProperty.hasAttribute(SUMO_ATTR_ID)) {
                    myBaseAdditional->addStringAttribute(SUMO_ATTR_ID, myViewNet->getNet()->getAttributeCarriers()->generateAdditionalID(tagProperty.getTag()));
                }
                // add lane IDs
                myBaseAdditional->addStringListAttribute(SUMO_ATTR_LANES, myConsecutiveLaneSelector->getLaneIDPath());
                // set positions
                myBaseAdditional->addDoubleAttribute(SUMO_ATTR_POSITION, myConsecutiveLaneSelector->getLanePath().front().second);
                myBaseAdditional->addDoubleAttribute(SUMO_ATTR_ENDPOS, myConsecutiveLaneSelector->getLanePath().back().second);
                // parse common attributes
                if (buildAdditionalCommonAttributes(tagProperty)) {
                    // show warning dialogbox and stop check if input parameters are valid
                    if (!myAdditionalAttributes->areValuesValid()) {
                        myAdditionalAttributes->showWarningMessage();
                    } else {
                        // declare additional handler
                        GNEAdditionalHandler additionalHandler(getViewNet()->getNet(), true, false);
                        // build additional
                        additionalHandler.parseSumoBaseObject(myBaseAdditional);
                        // Refresh additional Parent Selector (For additionals that have a limited number of children)
                        mySelectorAdditionalParent->refreshSelectorParentModule();
                        // abort E2 creation
                        myConsecutiveLaneSelector->abortPathCreation();
                        // refresh additional attributes
                        myAdditionalAttributes->refreshAttributesCreator();
                        return true;
                    }
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
        // show additional attributes module
        myAdditionalAttributes->showAttributesCreatorModule(templateAC, {});
        // show netedit attributes
        myNeteditAttributes->showNeteditAttributesModule(templateAC);
        // Show myAdditionalFrameParent if we're adding an slave element
        if (templateAC->getTagProperty().isChild()) {
            mySelectorAdditionalParent->showSelectorParentModule(templateAC->getTagProperty().getParentTags());
        } else {
            mySelectorAdditionalParent->hideSelectorParentModule();
        }
        // Show EdgesSelector if we're adding an additional that own the attribute SUMO_ATTR_EDGES
        if (templateAC->getTagProperty().hasAttribute(SUMO_ATTR_EDGES)) {
            myEdgesSelector->showNetworkElementsSelector();
        } else {
            myEdgesSelector->hideNetworkElementsSelector();
        }
        // show help creation modul
        myHelpCreationModule->showHelpCreationModule(templateAC->getTagProperty().getTag());
        // check if we must show consecutive lane selector
        if (templateAC->getTagProperty().getTag() == GNE_TAG_MULTI_LANE_AREA_DETECTOR) {
            myConsecutiveLaneSelector->showConsecutiveLaneSelectorModule();
            myE2MultilaneLegendModule->showE2MultilaneLegend();
            myLanesSelector->hideNetworkElementsSelector();
            // recompute network
            myViewNet->getNet()->computeNetwork(myViewNet->getViewParent()->getGNEAppWindows());
        } else if (templateAC->getTagProperty().hasAttribute(SUMO_ATTR_LANES)) {
            myConsecutiveLaneSelector->hideConsecutiveLaneSelectorModule();
            myE2MultilaneLegendModule->hideE2MultilaneLegend();
            myLanesSelector->showNetworkElementsSelector();
        } else {
            myConsecutiveLaneSelector->hideConsecutiveLaneSelectorModule();
            myE2MultilaneLegendModule->hideE2MultilaneLegend();
            myLanesSelector->hideNetworkElementsSelector();
        }
        // reset last position
        myViewNet->resetLastClickedPosition();
    } else {
        // hide all modules if additional isn't valid
        myAdditionalAttributes->hideAttributesCreatorModule();
        myNeteditAttributes->hideNeteditAttributesModule();
        mySelectorAdditionalParent->hideSelectorParentModule();
        myEdgesSelector->hideNetworkElementsSelector();
        myLanesSelector->hideNetworkElementsSelector();
        myConsecutiveLaneSelector->hideConsecutiveLaneSelectorModule();
        myHelpCreationModule->hideHelpCreationModule();
        myE2MultilaneLegendModule->hideE2MultilaneLegend();
    }
}


bool
GNEAdditionalFrame::createBaseAdditionalObject(const GNETagProperties& tagProperty) {
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
    // declare tag for base additional
    SumoXMLTag baseAdditionalTag = tagProperty.getTag();
    // check if baseAdditionalTag has to be updated
    if (baseAdditionalTag == GNE_TAG_MULTI_LANE_AREA_DETECTOR) {
        baseAdditionalTag = SUMO_TAG_LANE_AREA_DETECTOR;
    } else if (baseAdditionalTag == GNE_TAG_CALIBRATOR_FLOW) {
        baseAdditionalTag = SUMO_TAG_FLOW;
    }
    // check if additional is child
    if (tagProperty.isChild()) {
        // get additional under cursor
        const GNEAdditional* additionalUnderCursor = myViewNet->getViewObjectsSelector().getAdditionalFront();
        // if user click over an additional element parent, mark int in ParentAdditionalSelector
        if (additionalUnderCursor && (additionalUnderCursor->getTagProperty().getTag() == tagProperty.getParentTags().front())) {
            // update parent additional selected
            mySelectorAdditionalParent->setIDSelected(additionalUnderCursor->getID());
        }
        // stop if currently there isn't a valid selected parent
        if (mySelectorAdditionalParent->getIdSelected().empty()) {
            myAdditionalAttributes->showWarningMessage(toString(tagProperty.getParentTags().front()) +
                    TL(" must be selected before insertion of ") + myAdditionalTagSelector->getCurrentTemplateAC()->getTagProperty().getTagStr() + ".");
            return false;
        } else {
            // create baseAdditional parent
            myBaseAdditional = new CommonXMLStructure::SumoBaseObject(nullptr);
            // set parent tag
            myBaseAdditional->setTag(tagProperty.getParentTags().front());
            // add ID
            myBaseAdditional->addStringAttribute(SUMO_ATTR_ID, mySelectorAdditionalParent->getIdSelected());
            // create baseAdditional again as child of current myBaseAdditional
            myBaseAdditional = new CommonXMLStructure::SumoBaseObject(myBaseAdditional);
        }
    } else {
        // just create a base additional
        myBaseAdditional = new CommonXMLStructure::SumoBaseObject(nullptr);
    }
    // set baseAdditional tag
    myBaseAdditional->setTag(baseAdditionalTag);
    // BaseAdditional created, then return true
    return true;
}


bool
GNEAdditionalFrame::buildAdditionalCommonAttributes(const GNETagProperties& tagProperties) {
    // If additional has a interval defined by a begin or end, check that is valid
    if (tagProperties.hasAttribute(SUMO_ATTR_STARTTIME) && tagProperties.hasAttribute(SUMO_ATTR_END)) {
        const double begin = myBaseAdditional->getDoubleAttribute(SUMO_ATTR_STARTTIME);
        const double end = myBaseAdditional->getDoubleAttribute(SUMO_ATTR_END);
        if (begin > end) {
            myAdditionalAttributes->showWarningMessage(TL("Attribute '") + toString(SUMO_ATTR_STARTTIME) + TL("' cannot be greater than attribute '") + toString(SUMO_ATTR_END) + "'.");
            return false;
        }
    }
    // If additional own the attribute SUMO_ATTR_FILE but was't defined, will defined as <ID>.xml
    if (tagProperties.hasAttribute(SUMO_ATTR_FILE) && myBaseAdditional->getStringAttribute(SUMO_ATTR_FILE).empty()) {
        if ((myAdditionalTagSelector->getCurrentTemplateAC()->getTagProperty().getTag() != SUMO_TAG_CALIBRATOR) && (myAdditionalTagSelector->getCurrentTemplateAC()->getTagProperty().getTag() != SUMO_TAG_REROUTER)) {
            // SUMO_ATTR_FILE is optional for calibrators and rerouters (fails to load in sumo when given and the file does not exist)
            myBaseAdditional->addStringAttribute(SUMO_ATTR_FILE, myBaseAdditional->getStringAttribute(SUMO_ATTR_ID) + ".xml");
        }
    }
    // check edge children
    if (tagProperties.hasAttribute(SUMO_ATTR_EDGES) && (!myBaseAdditional->hasStringListAttribute(SUMO_ATTR_EDGES) || myBaseAdditional->getStringListAttribute(SUMO_ATTR_EDGES).empty())) {
        // obtain edge IDs
        myBaseAdditional->addStringListAttribute(SUMO_ATTR_EDGES, myEdgesSelector->getSelectedIDs());
        // check if attribute has at least one edge
        if (myBaseAdditional->getStringListAttribute(SUMO_ATTR_EDGES).empty()) {
            myAdditionalAttributes->showWarningMessage(TL("List of edges cannot be empty"));
            return false;
        }
    }
    // check lane children
    if (tagProperties.hasAttribute(SUMO_ATTR_LANES) && (!myBaseAdditional->hasStringListAttribute(SUMO_ATTR_LANES) || myBaseAdditional->getStringListAttribute(SUMO_ATTR_LANES).empty())) {
        // obtain lane IDs
        myBaseAdditional->addStringListAttribute(SUMO_ATTR_LANES, myLanesSelector->getSelectedIDs());
        // check if attribute has at least one lane
        if (myBaseAdditional->getStringListAttribute(SUMO_ATTR_LANES).empty()) {
            myAdditionalAttributes->showWarningMessage(TL("List of lanes cannot be empty"));
            return false;
        }
    }
    // all ok, continue building additional
    return true;
}


bool
GNEAdditionalFrame::buildAdditionalOverEdge(GNELane* lane, const GNETagProperties& tagProperties) {
    // check that lane exist
    if (lane) {
        // Get attribute lane's edge
        myBaseAdditional->addStringAttribute(SUMO_ATTR_EDGE, lane->getParentEdge()->getID());
        // Check if ID has to be generated
        if (tagProperties.getTag() == SUMO_TAG_VAPORIZER) {
            myBaseAdditional->addStringAttribute(SUMO_ATTR_ID, lane->getParentEdge()->getID());
        } else if (tagProperties.hasAttribute(SUMO_ATTR_ID) && !myBaseAdditional->hasStringAttribute(SUMO_ATTR_ID)) {
            myBaseAdditional->addStringAttribute(SUMO_ATTR_ID, myViewNet->getNet()->getAttributeCarriers()->generateAdditionalID(tagProperties.getTag()));
        }
    } else {
        return false;
    }
    // parse common attributes
    if (!buildAdditionalCommonAttributes(tagProperties)) {
        return false;
    }
    // show warning dialogbox and stop check if input parameters are valid
    if (!myAdditionalAttributes->areValuesValid()) {
        myAdditionalAttributes->showWarningMessage();
        return false;
    } else {
        // declare additional handler
        GNEAdditionalHandler additionalHandler(myViewNet->getNet(), true, false);
        // build additional
        additionalHandler.parseSumoBaseObject(myBaseAdditional);
        // Refresh additional Parent Selector (For additionals that have a limited number of children)
        mySelectorAdditionalParent->refreshSelectorParentModule();
        // clear selected edges and lanes
        myEdgesSelector->onCmdClearSelection(nullptr, 0, nullptr);
        myLanesSelector->onCmdClearSelection(nullptr, 0, nullptr);
        // refresh additional attributes
        myAdditionalAttributes->refreshAttributesCreator();
        return true;
    }
}


bool
GNEAdditionalFrame::buildAdditionalOverLane(GNELane* lane, const GNETagProperties& tagProperties) {
    // check that lane exist
    if (lane != nullptr) {
        // Get attribute lane
        myBaseAdditional->addStringAttribute(SUMO_ATTR_LANE, lane->getID());
        // Check if ID has to be generated
        if (tagProperties.hasAttribute(SUMO_ATTR_ID) && !myBaseAdditional->hasStringAttribute(SUMO_ATTR_ID)) {
            myBaseAdditional->addStringAttribute(SUMO_ATTR_ID, myViewNet->getNet()->getAttributeCarriers()->generateAdditionalID(tagProperties.getTag()));
        }
    } else {
        return false;
    }
    // Obtain position of the mouse over lane (limited over grid)
    const double mousePositionOverLane = lane->getLaneShape().nearest_offset_to_point2D(myViewNet->snapToActiveGrid(myViewNet->getPositionInformation())) / lane->getLengthGeometryFactor();
    // set attribute position as mouse position over lane
    if (myBaseAdditional->getTag() == SUMO_TAG_ACCESS) {
        myBaseAdditional->addStringAttribute(SUMO_ATTR_POSITION, toString(mousePositionOverLane));
    } else {
        myBaseAdditional->addDoubleAttribute(SUMO_ATTR_POSITION, mousePositionOverLane);
    }
    // parse common attributes
    if (!buildAdditionalCommonAttributes(tagProperties)) {
        return false;
    }
    // show warning dialogbox and stop check if input parameters are valid
    if (!myAdditionalAttributes->areValuesValid()) {
        myAdditionalAttributes->showWarningMessage();
        return false;
    } else {
        // declare additional handler
        GNEAdditionalHandler additionalHandler(myViewNet->getNet(), true, false);
        // build additional
        additionalHandler.parseSumoBaseObject(myBaseAdditional);
        // Refresh additional Parent Selector (For additionals that have a limited number of children)
        mySelectorAdditionalParent->refreshSelectorParentModule();
        // clear selected edges and lanes
        myEdgesSelector->onCmdClearSelection(nullptr, 0, nullptr);
        myLanesSelector->onCmdClearSelection(nullptr, 0, nullptr);
        // refresh additional attributes
        myAdditionalAttributes->refreshAttributesCreator();
        return true;
    }
}


bool
GNEAdditionalFrame::buildAdditionalOverView(const GNETagProperties& tagProperties) {
    // disable intervals (temporal)
    if ((tagProperties.getTag() == SUMO_TAG_INTERVAL) ||
            (tagProperties.getTag() == SUMO_TAG_DEST_PROB_REROUTE) ||
            (tagProperties.getTag() == SUMO_TAG_CLOSING_REROUTE) ||
            (tagProperties.getTag() == SUMO_TAG_CLOSING_LANE_REROUTE) ||
            (tagProperties.getTag() == SUMO_TAG_ROUTE_PROB_REROUTE) ||
            (tagProperties.getTag() == SUMO_TAG_PARKING_AREA_REROUTE)) {
        WRITE_WARNING(TL("Currently unsupported. Create rerouter elements using rerouter dialog"));
        return false;
    }
    // disable steps (temporal)
    if (tagProperties.getTag() == SUMO_TAG_STEP) {
        WRITE_WARNING(TL("Currently unsupported. Create VSS steps using VSS dialog"));
        return false;
    }
    // disable flows (temporal)
    if (tagProperties.getTag() == GNE_TAG_CALIBRATOR_FLOW) {
        WRITE_WARNING(TL("Currently unsupported. Create calibratorFlows using calibrator dialog"));
        return false;
    }
    // Check if ID has to be generated
    if (tagProperties.hasAttribute(SUMO_ATTR_ID)) {
        myBaseAdditional->addStringAttribute(SUMO_ATTR_ID, myViewNet->getNet()->getAttributeCarriers()->generateAdditionalID(tagProperties.getTag()));
    }
    // Obtain position as the clicked position over view
    const Position viewPos = myViewNet->snapToActiveGrid(myViewNet->getPositionInformation());
    // add position and X-Y-Z attributes
    myBaseAdditional->addPositionAttribute(SUMO_ATTR_POSITION, viewPos);
    myBaseAdditional->addDoubleAttribute(SUMO_ATTR_X, viewPos.x());
    myBaseAdditional->addDoubleAttribute(SUMO_ATTR_Y, viewPos.y());
    myBaseAdditional->addDoubleAttribute(SUMO_ATTR_Z, viewPos.z());
    // parse common attributes
    if (!buildAdditionalCommonAttributes(tagProperties)) {
        return false;
    }
    // special case for VSS Steps
    if (myBaseAdditional->getTag() == SUMO_TAG_STEP) {
        // get VSS parent
        const auto VSSParent = myViewNet->getNet()->getAttributeCarriers()->retrieveAdditional(SUMO_TAG_VSS,
                               myBaseAdditional->getParentSumoBaseObject()->getStringAttribute(SUMO_ATTR_ID));
        // get last step
        GNEAdditional* step = nullptr;
        for (const auto& additionalChild : VSSParent->getChildAdditionals()) {
            if (!additionalChild->getTagProperty().isSymbol()) {
                step = additionalChild;
            }
        }
        // set time
        if (step) {
            myBaseAdditional->addTimeAttribute(SUMO_ATTR_TIME, string2time(step->getAttribute(SUMO_ATTR_TIME)) + TIME2STEPS(900));
        } else {
            myBaseAdditional->addTimeAttribute(SUMO_ATTR_TIME, 0);
        }
    }
    // show warning dialogbox and stop check if input parameters are valid
    if (!myAdditionalAttributes->areValuesValid()) {
        myAdditionalAttributes->showWarningMessage();
        return false;
    } else {
        // declare additional handler
        GNEAdditionalHandler additionalHandler(myViewNet->getNet(), true, false);
        // build additional
        additionalHandler.parseSumoBaseObject(myBaseAdditional);
        // Refresh additional Parent Selector (For additionals that have a limited number of children)
        mySelectorAdditionalParent->refreshSelectorParentModule();
        // clear selected edges and lanes
        myEdgesSelector->onCmdClearSelection(nullptr, 0, nullptr);
        myLanesSelector->onCmdClearSelection(nullptr, 0, nullptr);
        // refresh additional attributes
        myAdditionalAttributes->refreshAttributesCreator();
        return true;
    }
}

/****************************************************************************/
