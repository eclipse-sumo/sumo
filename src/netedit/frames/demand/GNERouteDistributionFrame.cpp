/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
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
/// @file    GNERouteDistributionFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2023
///
// The Widget for edit type distribution elements
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/changes/GNEChange_DemandElement.h>
#include <netedit/elements/demand/GNERouteDistribution.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNERouteDistributionFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNERouteDistributionFrame::RouteDistributionEditor) typeDistributionEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATE,    GNERouteDistributionFrame::RouteDistributionEditor::onCmdCreateRoute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_DELETE,    GNERouteDistributionFrame::RouteDistributionEditor::onCmdDeleteRoute),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_DELETE,    GNERouteDistributionFrame::RouteDistributionEditor::onUpdDeleteRoute),
};

FXDEFMAP(GNERouteDistributionFrame::RouteDistributionSelector) typeDistributionSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_TYPE,   GNERouteDistributionFrame::RouteDistributionSelector::onCmdSelectRouteDistribution),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_SET_TYPE,   GNERouteDistributionFrame::RouteDistributionSelector::onCmdUpdateRouteDistribution)
};

FXDEFMAP(GNERouteDistributionFrame::RouteDistributionAttributesEditorRow) RouteDistributionAttributesEditorRowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNERouteDistributionFrame::RouteDistributionAttributesEditorRow::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_REMOVE,  GNERouteDistributionFrame::RouteDistributionAttributesEditorRow::onCmdRemoveRow)
};

FXDEFMAP(GNERouteDistributionFrame::RouteDistributionAttributesEditor) RouteDistributionAttributesEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_BUTTON_ADD,   GNERouteDistributionFrame::RouteDistributionAttributesEditor::onCmdAddRow)
};

// Object implementation
FXIMPLEMENT(GNERouteDistributionFrame::RouteDistributionEditor,               MFXGroupBoxModule,  typeDistributionEditorMap,              ARRAYNUMBER(typeDistributionEditorMap))
FXIMPLEMENT(GNERouteDistributionFrame::RouteDistributionSelector,             MFXGroupBoxModule,  typeDistributionSelectorMap,            ARRAYNUMBER(typeDistributionSelectorMap))
FXIMPLEMENT(GNERouteDistributionFrame::RouteDistributionAttributesEditorRow,  FXHorizontalFrame,  RouteDistributionAttributesEditorRowMap, ARRAYNUMBER(RouteDistributionAttributesEditorRowMap))
FXIMPLEMENT(GNERouteDistributionFrame::RouteDistributionAttributesEditor,     MFXGroupBoxModule,  RouteDistributionAttributesEditorMap,    ARRAYNUMBER(RouteDistributionAttributesEditorMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNERouteFrame::RouteDistributionEditor - methods
// ---------------------------------------------------------------------------

GNERouteDistributionFrame::RouteDistributionEditor::RouteDistributionEditor(GNERouteDistributionFrame* typeDistributionFrameParent) :
    MFXGroupBoxModule(typeDistributionFrameParent, TL("Route Editor")),
    myRouteDistributionFrameParent(typeDistributionFrameParent) {
    // Create new route
    myCreateRouteButton = new FXButton(getCollapsableFrame(), TL("Create type distribution"), GUIIconSubSys::getIcon(GUIIcon::ROUTEDISTRIBUTION), this, MID_GNE_CREATE, GUIDesignButton);
    // Create delete/reset route
    myDeleteRouteButton = new FXButton(getCollapsableFrame(), TL("Delete type distribution"), GUIIconSubSys::getIcon(GUIIcon::MODEDELETE), this, MID_GNE_DELETE, GUIDesignButton);
    // show type editor
    show();
}


GNERouteDistributionFrame::RouteDistributionEditor::~RouteDistributionEditor() {}


long
GNERouteDistributionFrame::RouteDistributionEditor::onCmdCreateRoute(FXObject*, FXSelector, void*) {
    auto viewNet = myRouteDistributionFrameParent->myViewNet;
    // obtain a new valid Route ID
    const std::string routeDistributionID = viewNet->getNet()->getAttributeCarriers()->generateDemandElementID(SUMO_TAG_ROUTE_DISTRIBUTION);
    // create new route
    GNEDemandElement* routeDistribution = new GNERouteDistribution(viewNet->getNet(), routeDistributionID);
    // add it using undoList (to allow undo-redo)
    viewNet->getUndoList()->begin(routeDistribution->getTagProperty().getGUIIcon(), "create route distribution");
    viewNet->getUndoList()->add(new GNEChange_DemandElement(routeDistribution, true), true);
    viewNet->getUndoList()->end();
    return 1;
}


long
GNERouteDistributionFrame::RouteDistributionEditor::onCmdDeleteRoute(FXObject*, FXSelector, void*) {
    auto viewNet = myRouteDistributionFrameParent->myViewNet;
    auto currentRouteDistribution = myRouteDistributionFrameParent->myRouteDistributionSelector->getCurrentRouteDistribution();
    // begin undo list operation
    viewNet->getUndoList()->begin(currentRouteDistribution->getTagProperty().getGUIIcon(), "delete route distribution");
    // remove route (and all of their children)
    viewNet->getNet()->deleteDemandElement(currentRouteDistribution, viewNet->getUndoList());
    // end undo list operation
    viewNet->getUndoList()->end();
    return 1;
}


long
GNERouteDistributionFrame::RouteDistributionEditor::onUpdDeleteRoute(FXObject* sender, FXSelector, void*) {
    // first check if selected route is valid
    if (myRouteDistributionFrameParent->myRouteDistributionSelector->getCurrentRouteDistribution()) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
}

// ---------------------------------------------------------------------------
// GNERouteFrame::RouteDistributionSelector - methods
// ---------------------------------------------------------------------------

GNERouteDistributionFrame::RouteDistributionSelector::RouteDistributionSelector(GNERouteDistributionFrame* typeFrameParent) :
    MFXGroupBoxModule(typeFrameParent, TL("Current Route")),
    myRouteDistributionFrameParent(typeFrameParent) {
    // Create FXComboBox
    myRouteComboBox = new FXComboBox(getCollapsableFrame(), GUIDesignComboBoxNCol, this, MID_GNE_SET_TYPE, GUIDesignComboBox);
    // add default Routes (always first)
    for (const auto& route : myRouteDistributionFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE_DISTRIBUTION)) {
        myRouteComboBox->appendItem(route->getID().c_str(), route->getFXIcon());
    }
    // Set visible items
    if (myRouteComboBox->getNumItems() <= 20) {
        myRouteComboBox->setNumVisible((int)myRouteComboBox->getNumItems());
    } else {
        myRouteComboBox->setNumVisible(20);
    }
    // RouteDistributionSelector is always shown
    show();
}


GNERouteDistributionFrame::RouteDistributionSelector::~RouteDistributionSelector() {}


GNEDemandElement*
GNERouteDistributionFrame::RouteDistributionSelector::getCurrentRouteDistribution() const {
    return myRouteDistributionFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_ROUTE_DISTRIBUTION, myCurrentRouteDistribution, false);
}


void
GNERouteDistributionFrame::RouteDistributionSelector::setCurrentRouteDistribution(const GNEDemandElement* routeDistribution) {
    myCurrentRouteDistribution = routeDistribution->getID();
    refreshRouteDistributionSelector();
}


void
GNERouteDistributionFrame::RouteDistributionSelector::refreshRouteDistributionSelector() {
    // get ACs
    const auto& ACs = myRouteDistributionFrameParent->getViewNet()->getNet()->getAttributeCarriers();
    // clear items
    myRouteComboBox->clearItems();
    // fill myRouteMatchBox with list of type distributions sorted by ID
    std::map<std::string, GNEDemandElement*> typeDistributions;
    for (const auto& routeDistribution : ACs->getDemandElements().at(SUMO_TAG_ROUTE_DISTRIBUTION)) {
        typeDistributions[routeDistribution->getID()] = routeDistribution;
    }
    for (const auto& routeDistribution : typeDistributions) {
        myRouteComboBox->appendItem(routeDistribution.first.c_str(), routeDistribution.second->getFXIcon());
    }
    // Set visible items
    if (myRouteComboBox->getNumItems() <= 20) {
        myRouteComboBox->setNumVisible((int)myRouteComboBox->getNumItems());
    } else {
        myRouteComboBox->setNumVisible(20);
    }
    // check current type
    bool validCurrentRouteDistribution = false;
    for (int i = 0; i < (int)myRouteComboBox->getNumItems(); i++) {
        if (myRouteComboBox->getItem(i).text() == myCurrentRouteDistribution) {
            myRouteComboBox->setCurrentItem(i);
            validCurrentRouteDistribution = true;
        }
    }
    // Check that give route type is valid
    GNEDemandElement* routeDistribution = nullptr;
    if (validCurrentRouteDistribution) {
        routeDistribution = ACs->retrieveDemandElement(SUMO_TAG_ROUTE_DISTRIBUTION, myCurrentRouteDistribution);
    } else {
        routeDistribution = ACs->retrieveFirstDemandElement(SUMO_TAG_ROUTE_DISTRIBUTION);
    }
    // Check that give route type is valid
    if (routeDistribution) {
        myCurrentRouteDistribution = routeDistribution->getID();
        // set myCurrentRoute as inspected element
        myRouteDistributionFrameParent->getViewNet()->setInspectedAttributeCarriers({routeDistribution});
        // show modules
        myRouteDistributionFrameParent->myRouteDistributionAttributesEditor->showAttributeEditorModule();
    } else {
        myCurrentRouteDistribution.clear();
        // set myCurrentRoute as inspected element
        myRouteDistributionFrameParent->getViewNet()->setInspectedAttributeCarriers({});
        // hide modules
        myRouteDistributionFrameParent->myRouteDistributionAttributesEditor->hideRouteDistributionAttributesEditorModule();
    }
}


long
GNERouteDistributionFrame::RouteDistributionSelector::onCmdSelectRouteDistribution(FXObject*, FXSelector, void*) {
    const auto viewNet = myRouteDistributionFrameParent->getViewNet();
    const auto& routeDistributions = viewNet->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE_DISTRIBUTION);
    // Check if value of myRouteMatchBox correspond of an allowed additional tags
    for (const auto& routeDistribution : routeDistributions) {
        if (routeDistribution->getID() == myRouteComboBox->getText().text()) {
            // set pointer
            myCurrentRouteDistribution = routeDistribution->getID();
            // set color of myRouteMatchBox to black (valid)
            myRouteComboBox->setTextColor(FXRGB(0, 0, 0));
            // set myCurrentRoute as inspected element
            viewNet->setInspectedAttributeCarriers({routeDistribution});
            // show modules if selected item is valid
            myRouteDistributionFrameParent->myRouteDistributionAttributesEditor->showAttributeEditorModule();
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(("Selected item '" + myRouteComboBox->getText() + "' in RouteDistributionSelector").text());
            // update viewNet
            viewNet->updateViewNet();
            return 1;
        }
    }
    myCurrentRouteDistribution.clear();
    // hide all modules if selected item isn't valid
    myRouteDistributionFrameParent->myRouteDistributionAttributesEditor->hideRouteDistributionAttributesEditorModule();
    // set color of myRouteMatchBox to red (invalid)
    myRouteComboBox->setTextColor(FXRGB(255, 0, 0));
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Selected invalid item in RouteDistributionSelector");
    // update viewNet
    viewNet->updateViewNet();
    return 1;
}


long
GNERouteDistributionFrame::RouteDistributionSelector::onCmdUpdateRouteDistribution(FXObject* sender, FXSelector, void*) {
    const auto& demandElements = myRouteDistributionFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements();
    if (demandElements.at(SUMO_TAG_ROUTE_DISTRIBUTION).size() > 0) {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        return sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
}

// ---------------------------------------------------------------------------
// GNERouteDistributionFrame::RouteDistributionAttributesEditorRow - methods
// ---------------------------------------------------------------------------

GNERouteDistributionFrame::RouteDistributionAttributesEditorRow::RouteDistributionAttributesEditorRow(
        GNERouteDistributionFrame::RouteDistributionAttributesEditor* attributeEditorParent,
        const GNEAttributeProperties& ACAttr, const std::string& id) :
    FXHorizontalFrame(attributeEditorParent->getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame),
    myRouteDistributionAttributesEditorParent(attributeEditorParent) {
    // get staticTooltip menu
    auto staticTooltipMenu = attributeEditorParent->getRouteDistributionFrameParent()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu();
    // Create attribute label (usually used only for ID)
    myIDLabel = new MFXLabelTooltip(this, staticTooltipMenu,
        ACAttr.getAttrStr().c_str(), nullptr, GUIDesignLabelThickedFixed(GUIDesignHeight));
    // Create and hide MFXTextFieldTooltip for string attributes
    myValueTextField = new MFXTextFieldTooltip(this, staticTooltipMenu,
        GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // only create if parent was created
    if (getParent()->id()) {
        // create RouteDistributionAttributesEditorRow
        FXHorizontalFrame::create();
        // Show attribute ACAttr.getAttrStr().c_str());
        myIDLabel->setTipText(ACAttr.getDefinition().c_str());
        // In any other case (String, list, etc.), show value as String
        myValueTextField->setText(id.c_str());
        myValueTextField->setTextColor(FXRGB(0, 0, 0));
        myValueTextField->killFocus();
        // Show RouteDistributionAttributesEditorRow
        show();
    }
}


GNERouteDistributionFrame::RouteDistributionAttributesEditorRow::RouteDistributionAttributesEditorRow(
        RouteDistributionAttributesEditor* attributeEditorParent, const std::string& type, const std::string& probability) :
    FXHorizontalFrame(attributeEditorParent->getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame),
    myRouteDistributionAttributesEditorParent(attributeEditorParent) {
    // get staticTooltip menu
    auto staticTooltipMenu = attributeEditorParent->getRouteDistributionFrameParent()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu();
    // create and hide color editor
    myDeleteRowButton = new MFXButtonTooltip(this, staticTooltipMenu,
        "", GUIIconSubSys::getIcon(GUIIcon::REMOVE), this, MID_GNE_BUTTON_REMOVE, GUIDesignButtonIcon);
    // Create and hide MFXTextFieldTooltip for string attributes
    myValueTextField = new MFXTextFieldTooltip(this, staticTooltipMenu,
        GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // Create and hide MFXTextFieldTooltip for string attributes
    myProbabilityTextField = new MFXTextFieldTooltip(this, staticTooltipMenu,
        GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldRestricted(TEXTFIELD_REAL));
    // only create if parent was created
    if (getParent()->id()) {
        // create RouteDistributionAttributesEditorRow
        FXHorizontalFrame::create();
        // set type
        myValueTextField->setText(type.c_str());
        myValueTextField->setTextColor(FXRGB(0, 0, 0));
        myValueTextField->killFocus();
        // set probability
        myProbabilityTextField->setText(probability.c_str());
        myProbabilityTextField->setTextColor(FXRGB(0, 0, 0));
        myProbabilityTextField->killFocus();
        // Show RouteDistributionAttributesEditorRow
        show();
    }
}


void
GNERouteDistributionFrame::RouteDistributionAttributesEditorRow::destroy() {
    // only destroy if parent was created
    if (getParent()->id()) {
        FXHorizontalFrame::destroy();
    }
}


void
GNERouteDistributionFrame::RouteDistributionAttributesEditorRow::refreshRouteDistributionAttributesEditorRow(const std::string& value) {
    // set last valid value and restore color if onlyValid is disabled
    myValueTextField->setText(value.c_str());
    // set blue color if is an computed value
    myValueTextField->setTextColor(FXRGB(0, 0, 0));
    myValueTextField->killFocus();
}


bool
GNERouteDistributionFrame::RouteDistributionAttributesEditorRow::isRouteDistributionAttributesEditorRowValid() const {
    return (myValueTextField->getTextColor() == FXRGB(0, 0, 0));
}


long
GNERouteDistributionFrame::RouteDistributionAttributesEditorRow::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    // Declare changed value
    std::string newVal;
    // obtain value of myValueTextField
    newVal = myValueTextField->getText().text();
    // get current distribution
    auto currentDistribution = myRouteDistributionAttributesEditorParent->getRouteDistributionFrameParent()->getRouteDistributionSelector()->getCurrentRouteDistribution();
    // continue if we have a distribution to edit
    if (currentDistribution) {
        // Check if attribute must be changed
        if (currentDistribution->isValid(SUMO_ATTR_ID, newVal)) {
            // set attribute
            currentDistribution->setAttribute(SUMO_ATTR_ID, newVal, myRouteDistributionAttributesEditorParent->getRouteDistributionFrameParent()->getViewNet()->getUndoList());
            // update text field
            myValueTextField->setTextColor(FXRGB(0, 0, 0));
            myValueTextField->setBackColor(FXRGB(255, 255, 255));
            myValueTextField->killFocus();
            // in this case, we need to refresh the other values (For example, allow/Disallow objects)
            myRouteDistributionAttributesEditorParent->refreshAttributeEditor();
            // update frame parent after attribute successfully set
            myRouteDistributionAttributesEditorParent->getRouteDistributionFrameParent()->attributeUpdated(SUMO_ATTR_ID);
        } else {
            myValueTextField->setTextColor(FXRGB(255, 0, 0));
            if (newVal.empty()) {
                myValueTextField->setBackColor(FXRGBA(255, 213, 213, 255));
            }
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG(TL("Value '") + newVal + TL("' for attribute ID of Distribution isn't valid"));
        }
    }
    return 1;
}


long
GNERouteDistributionFrame::RouteDistributionAttributesEditorRow::onCmdRemoveRow(FXObject*, FXSelector, void*) {

    return 1;
}


GNERouteDistributionFrame::RouteDistributionAttributesEditorRow::RouteDistributionAttributesEditorRow() :
    myRouteDistributionAttributesEditorParent(nullptr) {
}

// ---------------------------------------------------------------------------
// GNERouteDistributionFrame::RouteDistributionAttributesEditor - methods
// ---------------------------------------------------------------------------

GNERouteDistributionFrame::RouteDistributionAttributesEditor::RouteDistributionAttributesEditor(GNERouteDistributionFrame* typeDistributionFrameParent) :
    MFXGroupBoxModule(typeDistributionFrameParent, TL("Internal attributes")),
    myRouteDistributionFrameParent(typeDistributionFrameParent) {
    // resize myRouteDistributionAttributesEditorRows
    myRouteDistributionAttributesEditorRows.resize(GNEAttributeCarrier::MAXNUMBEROFATTRIBUTES, nullptr);
    // Create help button
    myAddButton = new FXButton(getCollapsableFrame(), "", GUIIconSubSys::getIcon(GUIIcon::ADD), this, MID_GNE_BUTTON_ADD, GUIDesignButtonIcon);
}


void
GNERouteDistributionFrame::RouteDistributionAttributesEditor::showAttributeEditorModule() {
    // first remove all rows
    for (auto& row : myRouteDistributionAttributesEditorRows) {
        // destroy and delete all rows
        if (row != nullptr) {
            row->destroy();
            delete row;
            row = nullptr;
        }
    }
    // get current distribution
    auto currentDistribution = myRouteDistributionFrameParent->getRouteDistributionSelector()->getCurrentRouteDistribution();
    // continue if we have a distribution to edit
    if (currentDistribution) {
        // get ID
        const auto distributionID = currentDistribution->getAttribute(SUMO_ATTR_ID);
        // first add ID
        myRouteDistributionAttributesEditorRows[0] = new RouteDistributionAttributesEditorRow(this,
                currentDistribution->getTagProperty().getAttributeProperties(SUMO_ATTR_ID), distributionID);
        // get all routes with the given current sorted by ID
        std::map<std::string, GNEDemandElement*> routes;
        for (const auto &route : myRouteDistributionFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getDemandElements().at(SUMO_TAG_ROUTE)) {
            if (route->getAttribute(GNE_ATTR_ROUTE_DISTRIBUTION) == distributionID) {
                routes[route->getID()] = route;
            }
        }
        int index = 1;
        for (const auto &route : routes) {
            myRouteDistributionAttributesEditorRows[index] = new RouteDistributionAttributesEditorRow(this,
                route.first, route.second->getAttribute(GNE_ATTR_ROUTE_DISTRIBUTION_PROBABILITY));
            index++;
        }
        // show RouteDistributionAttributesEditor
        show();
    }
    // reparent help button (to place it at bottom)
    myAddButton->reparent(this);
}


void
GNERouteDistributionFrame::RouteDistributionAttributesEditor::hideRouteDistributionAttributesEditorModule() {
    // hide also RouteDistributionAttributesEditor
    hide();
}


void
GNERouteDistributionFrame::RouteDistributionAttributesEditor::refreshAttributeEditor() {
    // get current distribution
    auto currentDistribution = myRouteDistributionFrameParent->getRouteDistributionSelector()->getCurrentRouteDistribution();
    // continue if we have a distribution to edit
    if (currentDistribution) {
        // Iterate over inspected attribute carriers
        for (const auto& attrProperty : currentDistribution->getTagProperty()) {
            // Refresh attributes
            myRouteDistributionAttributesEditorRows[attrProperty.getPositionListed()]->refreshRouteDistributionAttributesEditorRow(currentDistribution->getAttribute(attrProperty.getAttr()));
        }
    }
}


GNERouteDistributionFrame*
GNERouteDistributionFrame::RouteDistributionAttributesEditor::getRouteDistributionFrameParent() const {
    return myRouteDistributionFrameParent;
}


long
GNERouteDistributionFrame::RouteDistributionAttributesEditor::onCmdAddRow(FXObject*, FXSelector, void*) {
    return 1;
}

// ---------------------------------------------------------------------------
// GNERouteDistributionFrame - methods
// ---------------------------------------------------------------------------

GNERouteDistributionFrame::GNERouteDistributionFrame(GNEViewParent* viewParent, GNEViewNet* viewNet) :
    GNEFrame(viewParent, viewNet, "Route Distributions") {

    // create type editor
    myRouteDistributionEditor = new RouteDistributionEditor(this);

    // create type selector
    myRouteDistributionSelector = new RouteDistributionSelector(this);

    // Create type distribution attributes editor
    myRouteDistributionAttributesEditor = new GNERouteDistributionFrame::RouteDistributionAttributesEditor(this);
}


GNERouteDistributionFrame::~GNERouteDistributionFrame() {}


void
GNERouteDistributionFrame::show() {
    // refresh type selector
    myRouteDistributionSelector->refreshRouteDistributionSelector();
    // show frame
    GNEFrame::show();
}


GNERouteDistributionFrame::RouteDistributionSelector*
GNERouteDistributionFrame::getRouteDistributionSelector() const {
    return myRouteDistributionSelector;
}


void
GNERouteDistributionFrame::attributeUpdated(SumoXMLAttr /*attribute*/) {

}

/****************************************************************************/
