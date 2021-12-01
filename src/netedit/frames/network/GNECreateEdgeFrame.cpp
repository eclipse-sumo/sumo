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
/// @file    GNECreateEdgeFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2018
///
// The Widget for create edges (and junctions)
/****************************************************************************/
#include <config.h>

#include <netbuild/NBNetBuilder.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/changes/GNEChange_EdgeType.h>
#include <netedit/dialogs/GNEAllowDisallow.h>
#include <netedit/dialogs/GNESingleParametersDialog.h>
#include <netedit/elements/network/GNEEdgeType.h>
#include <netedit/elements/network/GNELaneType.h>
#include <netimport/NITypeLoader.h>
#include <netimport/NIXMLTypesHandler.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/options/OptionsCont.h>


#include "GNECreateEdgeFrame.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNECreateEdgeFrame::EdgeTypeSelector) EdgeTypeSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_SELECTRADIOBUTTON,  GNECreateEdgeFrame::EdgeTypeSelector::onCmdRadioButton),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_ADDEDGETYPE,        GNECreateEdgeFrame::EdgeTypeSelector::onCmdAddEdgeType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_DELETEEDGETYPE,     GNECreateEdgeFrame::EdgeTypeSelector::onCmdDeleteEdgeType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_SELECTEDGETYPE,     GNECreateEdgeFrame::EdgeTypeSelector::onCmdSelectEdgeType),
};

FXDEFMAP(GNECreateEdgeFrame::LaneTypeSelector) LaneTypeSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_ADDEDGETYPE,        GNECreateEdgeFrame::LaneTypeSelector::onCmdAddLaneType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_DELETEEDGETYPE,     GNECreateEdgeFrame::LaneTypeSelector::onCmdDeleteLaneType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_SELECTEDGETYPE,     GNECreateEdgeFrame::LaneTypeSelector::onCmdSelectLaneType),
};

// Object implementation
FXIMPLEMENT(GNECreateEdgeFrame::EdgeTypeSelector,       FXGroupBox,     EdgeTypeSelectorMap,    ARRAYNUMBER(EdgeTypeSelectorMap))
FXIMPLEMENT(GNECreateEdgeFrame::LaneTypeSelector,       FXGroupBox,     LaneTypeSelectorMap,    ARRAYNUMBER(LaneTypeSelectorMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNECreateEdgeFrame::EdgeTypeSelector - methods
// ---------------------------------------------------------------------------

GNECreateEdgeFrame::EdgeTypeSelector::EdgeTypeSelector(GNECreateEdgeFrame* createEdgeFrameParent) :
    FXGroupBox(createEdgeFrameParent->myContentFrame, "Template selector", GUIDesignGroupBoxFrame),
    myCreateEdgeFrameParent(createEdgeFrameParent),
    myDefaultEdgeType(new GNEEdgeType(createEdgeFrameParent)),
    myHiddenAttributes({SUMO_ATTR_ID, SUMO_ATTR_ONEWAY, SUMO_ATTR_DISCARD, SUMO_ATTR_MAXWIDTH, SUMO_ATTR_MINWIDTH, SUMO_ATTR_SIDEWALKWIDTH, SUMO_ATTR_BIKELANEWIDTH, SUMO_ATTR_WIDTHRESOLUTION}) {
    // default edge radio button
    myUseDefaultEdgeType = new FXRadioButton(this, "Create default edge", this, MID_GNE_CREATEEDGEFRAME_SELECTRADIOBUTTON, GUIDesignRadioButton);
    // use custom edge radio button
    myUseCustomEdgeType = new FXRadioButton(this, "Use edgeType/template", this, MID_GNE_CREATEEDGEFRAME_SELECTRADIOBUTTON, GUIDesignRadioButton);
    // edge types combo box
    myEdgeTypesComboBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_CREATEEDGEFRAME_SELECTEDGETYPE, GUIDesignComboBoxAttribute);
    // create horizontal frame
    FXHorizontalFrame* horizontalFrameButtons = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // create new edge type button
    myAddEdgeTypeButton = new FXButton(horizontalFrameButtons, "add\t\add edge type", GUIIconSubSys::getIcon(GUIIcon::ADD), 
                                       this, MID_GNE_CREATEEDGEFRAME_ADDEDGETYPE, GUIDesignButton);
    // create delete edge type button
    myDeleteEdgeTypeButton = new FXButton(horizontalFrameButtons, "delete\t\tdelete edge type", GUIIconSubSys::getIcon(GUIIcon::REMOVE), 
                                          this, MID_GNE_CREATEEDGEFRAME_DELETEEDGETYPE, GUIDesignButton);
    // by default, create custom edge
    myUseDefaultEdgeType->setCheck(TRUE);
}


GNECreateEdgeFrame::EdgeTypeSelector::~EdgeTypeSelector() {
    delete myDefaultEdgeType;
}


void
GNECreateEdgeFrame::EdgeTypeSelector::refreshEdgeTypeSelector(const bool show) {
    // get template editor
    const GNEInspectorFrame::TemplateEditor* templateEditor = myCreateEdgeFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->getTemplateEditor();
    // get edge types
    const auto& edgeTypes = myCreateEdgeFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getEdgeTypes();
    // get flag for number of items
    const bool thereAreItems = (templateEditor->hasTemplate() || (edgeTypes.size() > 0));
    // first fill combo box
    fillComboBox();
    // show parameter fields
/*
    myCreateEdgeFrameParent->myEdgeTypeParameters->showEdgeTypeParameters();
*/
    // check conditions
    if (show && templateEditor->hasTemplate()) {
        // set buttons
        myUseDefaultEdgeType->setCheck(FALSE);
        myUseCustomEdgeType->setCheck(TRUE);
        // set template as current item
        myEdgeTypesComboBox->setCurrentItem(0);
        // update edge parameters (using template
/*
        myCreateEdgeFrameParent->myEdgeTypeParameters->setTemplateValues();
*/
        // disable delete edge type button (because templates cannot be removed)
        myDeleteEdgeTypeButton->disable();
        // disable parameter fields (because templates cannot be edited)
/*
        myCreateEdgeFrameParent->myEdgeTypeParameters->disableEdgeTypeParameters();
*/
    } else if (myUseDefaultEdgeType->getCheck() == TRUE) {
        // disable comboBox
        myEdgeTypesComboBox->disable();
        // disable buttons
        myAddEdgeTypeButton->disable();
        myDeleteEdgeTypeButton->disable();
        // show edgeType attributes
        myCreateEdgeFrameParent->myEdgeTypeAttributes->showAttributesCreatorModul(myDefaultEdgeType, myHiddenAttributes);
        // show lane attributes
        myCreateEdgeFrameParent->myLaneTypeSelector->showLaneTypeSelector();
    } else if (thereAreItems) {
        // enable add button
        myAddEdgeTypeButton->enable();
        // enable combo box
        myEdgeTypesComboBox->enable();
        // myEdgeTypesComboBox
        if (myEdgeTypeSelected) {
            // declare index
            int index = 0;
            // search index of current selectedEdgeType
            for (int i = 0; i < myEdgeTypesComboBox->getNumItems(); i++) {
                if (myEdgeTypeSelected->getID() == myEdgeTypesComboBox->getItem(i).text()) {
                    index = i;
                }
            }
            // set current item
            myEdgeTypesComboBox->setCurrentItem(index);
            // enable delete edge type button
            myDeleteEdgeTypeButton->enable();
/*
            // update edge parameters
            myCreateEdgeFrameParent->myEdgeTypeParameters->setEdgeType(myEdgeTypeSelected, true);
            // enable parameter fields
            myCreateEdgeFrameParent->myEdgeTypeParameters->enableEdgeTypeParameters();
*/
        } else if (templateEditor->hasTemplate()) {
            // set template as current item
            myEdgeTypesComboBox->setCurrentItem(0);
            // update edge parameters (using template
/*
            myCreateEdgeFrameParent->myEdgeTypeParameters->setTemplateValues();
            // disable delete edge type button (because templates cannot be removed)
*/
            myDeleteEdgeTypeButton->disable();
/*
            // disable parameter fields (because templates cannot be edited)
            myCreateEdgeFrameParent->myEdgeTypeParameters->disableEdgeTypeParameters();
*/
        } else if (edgeTypes.size() > 0) {
            // set myEdgeTypeSelected
            myEdgeTypeSelected = edgeTypes.begin()->second;
            // set current item
            myEdgeTypesComboBox->setCurrentItem(0);
            // enable delete edge type button
            myDeleteEdgeTypeButton->enable();
            // update edge parameters
/*
            myCreateEdgeFrameParent->myEdgeTypeParameters->setEdgeType(myEdgeTypeSelected, true);
            // enable parameter fields (because edgeTypes can be edited)
            myCreateEdgeFrameParent->myEdgeTypeParameters->enableEdgeTypeParameters();
*/
        }
    } else {
        // hide parameter fields
/*
        myCreateEdgeFrameParent->myEdgeTypeParameters->hideEdgeTypeParameters();
*/
        // disable add and delete buttons
        myAddEdgeTypeButton->enable();
        myDeleteEdgeTypeButton->disable();
        // disable combo box
        myEdgeTypesComboBox->disable();
    }
    // recalc
    recalc();
}


bool
GNECreateEdgeFrame::EdgeTypeSelector::useEdgeTemplate() const {
    if (myCreateEdgeFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->getTemplateEditor()->hasTemplate()) {
        if ((myUseCustomEdgeType->getCheck() == TRUE) && (myEdgeTypesComboBox->getCurrentItem() == 0)) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}


bool
GNECreateEdgeFrame::EdgeTypeSelector::useDefaultEdgeType() const {
    return (myUseDefaultEdgeType->getCheck() == TRUE);
}


GNEEdgeType*
GNECreateEdgeFrame::EdgeTypeSelector::getDefaultEdgeType() const {
    return myDefaultEdgeType;
}


GNEEdgeType*
GNECreateEdgeFrame::EdgeTypeSelector::getEdgeTypeSelected() const {
    if (myUseDefaultEdgeType->getCheck() == TRUE) {
        return myDefaultEdgeType;
    } else {
        return myEdgeTypeSelected;
    }
}


void
GNECreateEdgeFrame::EdgeTypeSelector::clearEdgeTypeSelected() {
    myEdgeTypeSelected = nullptr;
}


void
GNECreateEdgeFrame::EdgeTypeSelector::setCurrentEdgeType(const GNEEdgeType* edgeType) {
    for (int i = 0; i < myEdgeTypesComboBox->getNumItems(); i++) {
        if (myEdgeTypesComboBox->getItem(i).text() == edgeType->getID()) {
            myEdgeTypesComboBox->setCurrentItem(i, TRUE);
        }
    }
}


void
GNECreateEdgeFrame::EdgeTypeSelector::useTemplate() {
    myUseCustomEdgeType->setCheck(TRUE, FALSE);
    refreshEdgeTypeSelector();
}

long
GNECreateEdgeFrame::EdgeTypeSelector::onCmdRadioButton(FXObject* obj, FXSelector, void*) {
    // check what object was pressed
    if (obj == myUseDefaultEdgeType) {
        // update radio buttons
        myUseDefaultEdgeType->setCheck(TRUE, FALSE);
        myUseCustomEdgeType->setCheck(FALSE, FALSE);
    } else {
        // update radio buttons
        myUseDefaultEdgeType->setCheck(FALSE, FALSE);
        myUseCustomEdgeType->setCheck(TRUE, FALSE);
    }
    // refresh template selector
    refreshEdgeTypeSelector();
    return 0;
}


long
GNECreateEdgeFrame::EdgeTypeSelector::onCmdAddEdgeType(FXObject*, FXSelector, void*) {
    // create new edge type
    GNEEdgeType* edgeType = new GNEEdgeType(myCreateEdgeFrameParent->getViewNet()->getNet());
    // add it using undoList
    myCreateEdgeFrameParent->getViewNet()->getUndoList()->begin(GUIIcon::EDGE, "create new edge type");
    myCreateEdgeFrameParent->getViewNet()->getUndoList()->add(new GNEChange_EdgeType(edgeType, true), true);
    myCreateEdgeFrameParent->getViewNet()->getUndoList()->end();
    // update myEdgeTypeSelected
    myEdgeTypeSelected = edgeType;
    // refresh EdgeTypeSelector
    refreshEdgeTypeSelector();
    return 0;
}


long
GNECreateEdgeFrame::EdgeTypeSelector::onCmdDeleteEdgeType(FXObject*, FXSelector, void*) {
    // first check if we have to reset myEdgeTypeSelected
    if (myEdgeTypeSelected && (myEdgeTypeSelected->getID() == myEdgeTypesComboBox->getText().text())) {
        myEdgeTypeSelected = nullptr;
    }
    // get edgeType to remove
    GNEEdgeType* edgeType = myCreateEdgeFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveEdgeType(myEdgeTypesComboBox->getText().text());
    // remove it using undoList
    myCreateEdgeFrameParent->getViewNet()->getUndoList()->begin(GUIIcon::EDGE, "delete edge edge type");
    myCreateEdgeFrameParent->getViewNet()->getUndoList()->add(new GNEChange_EdgeType(edgeType, false), true);
    myCreateEdgeFrameParent->getViewNet()->getUndoList()->end();
    // refresh EdgeTypeSelector
    refreshEdgeTypeSelector();
    return 0;
}


long
GNECreateEdgeFrame::EdgeTypeSelector::onCmdSelectEdgeType(FXObject*, FXSelector, void*) {
    // get template editor
    const GNEInspectorFrame::TemplateEditor* templateEditor = myCreateEdgeFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->getTemplateEditor();
    // get edge types
    const auto& edgeTypes = myCreateEdgeFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getEdgeTypes();
    // reset myEdgeTypeSelected
    myEdgeTypeSelected = nullptr;
    // check if we selected template
    if (templateEditor->hasTemplate() && myEdgeTypesComboBox->getCurrentItem() == 0) {
        // set valid color
        myEdgeTypesComboBox->setTextColor(FXRGB(0, 0, 0));
        myEdgeTypesComboBox->killFocus();
        // disable delete edge type button (because templates cannot be edited)
        myDeleteEdgeTypeButton->disable();
        // show parameter fields
/*
        myCreateEdgeFrameParent->myEdgeTypeParameters->showEdgeTypeParameters();
        // disable parameter fields (because templantes cannot be edited)
        myCreateEdgeFrameParent->myEdgeTypeParameters->disableEdgeTypeParameters();
        // set edge template in myEdgeTypeParameters
        myCreateEdgeFrameParent->myEdgeTypeParameters->setTemplateValues();
*/
    } else if (edgeTypes.count(myEdgeTypesComboBox->getText().text()) > 0) {
        // set valid color
        myEdgeTypesComboBox->setTextColor(FXRGB(0, 0, 0));
        myEdgeTypesComboBox->killFocus();
        // enable delete edge type button
        myDeleteEdgeTypeButton->enable();
        // show parameter fields
/*
        myCreateEdgeFrameParent->myEdgeTypeParameters->showEdgeTypeParameters();
        // enable parameter fields
        myCreateEdgeFrameParent->myEdgeTypeParameters->enableEdgeTypeParameters();
        // set myEdgeTypeSelected
        myEdgeTypeSelected = myCreateEdgeFrameParent->myViewNet->getNet()->getAttributeCarriers()->retrieveEdgeType(myEdgeTypesComboBox->getText().text());
        // set edgeType in myEdgeTypeParameters
        myCreateEdgeFrameParent->myEdgeTypeParameters->setEdgeType(myEdgeTypeSelected, true);
*/
    } else {
        // set invalid color
        myEdgeTypesComboBox->setTextColor(FXRGB(255, 0, 0));
        // disable delete edge type button
        myDeleteEdgeTypeButton->disable();
        // hide parameter fields
/*
        myCreateEdgeFrameParent->myEdgeTypeParameters->hideEdgeTypeParameters();
*/
    }
    return 0;
}


void
GNECreateEdgeFrame::EdgeTypeSelector::fillComboBox() {
    // get template editor
    const GNEInspectorFrame::TemplateEditor* templateEditor = myCreateEdgeFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->getTemplateEditor();
    // get edge types
    const auto& edgeTypes = myCreateEdgeFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getEdgeTypes();
    // clear edge types
    myEdgeTypesComboBox->clearItems();
    // add template
    if (templateEditor->hasTemplate()) {
        myEdgeTypesComboBox->appendItem(("template: " + templateEditor->getEdgeTemplate().edgeParameters.at(SUMO_ATTR_ID)).c_str(), nullptr);
    }
    // add edge types
    for (const auto& edgeType : edgeTypes) {
        myEdgeTypesComboBox->appendItem(edgeType.second->getID().c_str(), nullptr);
    }
    // set num visible antes
    if (myEdgeTypesComboBox->getNumItems() <= 10) {
        myEdgeTypesComboBox->setNumVisible(myEdgeTypesComboBox->getNumItems());
    } else {
        myEdgeTypesComboBox->setNumVisible(10);
    }
}


void
GNECreateEdgeFrame::EdgeTypeSelector::fillDefaultParameters() {
    // set numLanes
    myDefaultEdgeType->setAttribute(SUMO_ATTR_NUMLANES, "1");
    // set speed
    myDefaultEdgeType->setAttribute(SUMO_ATTR_SPEED, "13.89");
    // set allow
    myDefaultEdgeType->setAttribute(SUMO_ATTR_ALLOW, "all");
    // set spreadType
    myDefaultEdgeType->setAttribute(SUMO_ATTR_SPREADTYPE, "");
    // set width
    myDefaultEdgeType->setAttribute(SUMO_ATTR_WIDTH, "-1.00");
    // set parameters
    myDefaultEdgeType->setAttribute(GNE_ATTR_PARAMETERS, "");
}

// ---------------------------------------------------------------------------
// GNECreateEdgeFrame::LaneTypeSelector - methods
// ---------------------------------------------------------------------------

GNECreateEdgeFrame::LaneTypeSelector::LaneTypeSelector(GNECreateEdgeFrame* createEdgeFrameParent) :
    FXGroupBox(createEdgeFrameParent->myContentFrame, "LaneType selector", GUIDesignGroupBoxFrame),
    myCreateEdgeFrameParent(createEdgeFrameParent),
    myLaneIndex(0) {
    // lane types combo box
    myLaneTypesComboBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_CREATEEDGEFRAME_SELECTEDGETYPE, GUIDesignComboBoxAttribute);
    // create horizontal frame
    FXHorizontalFrame* horizontalFrameButtons = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // create new lane type button
    myAddLaneTypeButton = new FXButton(horizontalFrameButtons, "add\t\add lane type", GUIIconSubSys::getIcon(GUIIcon::ADD), 
                                       this, MID_GNE_CREATEEDGEFRAME_ADDEDGETYPE, GUIDesignButton);
    // create delete lane type button
    myDeleteLaneTypeButton = new FXButton(horizontalFrameButtons, "delete\t\tdelete lane type", GUIIconSubSys::getIcon(GUIIcon::REMOVE), 
                                          this, MID_GNE_CREATEEDGEFRAME_DELETEEDGETYPE, GUIDesignButton);
}


GNECreateEdgeFrame::LaneTypeSelector::~LaneTypeSelector() {
}


void
GNECreateEdgeFrame::LaneTypeSelector::showLaneTypeSelector() {
    // refresh laneTypeSelector
    refreshLaneTypeSelector();
    // show
    show();
}


void
GNECreateEdgeFrame::LaneTypeSelector::hideLaneTypeSelector() {
    hide();
}


long
GNECreateEdgeFrame::LaneTypeSelector::onCmdAddLaneType(FXObject*, FXSelector, void*) {
    // check what edgeType is being edited
    if (myCreateEdgeFrameParent->myEdgeTypeSelector->useDefaultEdgeType()) {
        // add new lane in default edge type
        myCreateEdgeFrameParent->myEdgeTypeSelector->getDefaultEdgeType()->addLaneType(new GNELaneType(myCreateEdgeFrameParent->myEdgeTypeSelector->getDefaultEdgeType()));
        // refresh laneTypeSelector
        refreshLaneTypeSelector();
        // set combo box
        myLaneTypesComboBox->setCurrentItem(myLaneTypesComboBox->getNumItems() - 1);
    } else if (!myCreateEdgeFrameParent->myEdgeTypeSelector->useEdgeTemplate()) { 
        // get selected
        const auto edgeType = myCreateEdgeFrameParent->myEdgeTypeSelector->getEdgeTypeSelected();
        if (edgeType) {
            // create new edgeType
            GNEEdgeType* newEdgeType = new GNEEdgeType(edgeType);
            // add new lane
            newEdgeType->addLaneType(new GNELaneType(newEdgeType));
            // remove old edgeTyp und and newEdgeType
            myCreateEdgeFrameParent->getViewNet()->getUndoList()->begin(GUIIcon::LANE, "add laneType");
            myCreateEdgeFrameParent->getViewNet()->getUndoList()->add(new GNEChange_EdgeType(edgeType, false), true);
            myCreateEdgeFrameParent->getViewNet()->getUndoList()->add(new GNEChange_EdgeType(newEdgeType, true), true);
            myCreateEdgeFrameParent->getViewNet()->getUndoList()->end();
            // set current edgeType in selector
            myCreateEdgeFrameParent->myEdgeTypeSelector->setCurrentEdgeType(newEdgeType);
            // refresh laneTypeSelector
            refreshLaneTypeSelector();
            // set combo box
            myLaneTypesComboBox->setCurrentItem(myLaneTypesComboBox->getNumItems() - 1);
        }
    }
    return 0;
}


long
GNECreateEdgeFrame::LaneTypeSelector::onCmdDeleteLaneType(FXObject*, FXSelector, void*) {
    return 0;
}


long
GNECreateEdgeFrame::LaneTypeSelector::onCmdSelectLaneType(FXObject*, FXSelector, void*) {
    // update index
    myLaneIndex = myLaneTypesComboBox->getCurrentItem();
    // refresh laneType selector
    refreshLaneTypeSelector();
    return 0;
}


void
GNECreateEdgeFrame::LaneTypeSelector::refreshLaneTypeSelector() {
    // clear lane types
    myLaneTypesComboBox->clearItems();
    // get edgeType
    const auto edgeType = myCreateEdgeFrameParent->myEdgeTypeSelector->getEdgeTypeSelected();
    if (edgeType) {
        if (myLaneIndex > ((int)edgeType->getLaneTypes().size() - 1)) {
            myLaneIndex = 0;
        }
        // add lane types
        myLaneTypesComboBox->enable();
        for (const auto& laneType : edgeType->getLaneTypes()) {
            myLaneTypesComboBox->appendItem(laneType->getAttribute(SUMO_ATTR_ID).c_str(), nullptr);
        }
        // set item
        myLaneTypesComboBox->setCurrentItem(myLaneIndex);
        // set num visible items
        if (myLaneTypesComboBox->getNumItems() <= 10) {
            myLaneTypesComboBox->setNumVisible(myLaneTypesComboBox->getNumItems());
        } else {
            myLaneTypesComboBox->setNumVisible(10);
        }
        // enable add button
        myAddLaneTypeButton->enable();
        // enable delete button
        if (edgeType->getLaneTypes().size() == 1) {
            myDeleteLaneTypeButton->disable();
        } else {
            myDeleteLaneTypeButton->enable();
        }
        // show laneTypeAttributes
        myCreateEdgeFrameParent->myLaneTypeAttributes->showAttributesCreatorModul(edgeType->getLaneTypes().at(myLaneIndex), {});
    } else {
        // disable items
        myAddLaneTypeButton->disable();
        myDeleteLaneTypeButton->disable();
        myLaneTypesComboBox->disable();
        // hide laneTypeAttributes
        myCreateEdgeFrameParent->myLaneTypeAttributes->hideAttributesCreatorModul();
    }
    // recalc
    recalc();
}


void
GNECreateEdgeFrame::LaneTypeSelector::fillDefaultParameters() {
    // get templateEdge
    const auto edgeType = myCreateEdgeFrameParent->myEdgeTypeSelector->getEdgeTypeSelected();
    if (edgeType && (myLaneIndex < (int)edgeType->getLaneTypes().size())) {
        // set speed
        edgeType->getLaneTypes().at(myLaneIndex)->setAttribute(SUMO_ATTR_SPEED, "13.89");
        // set allow
        edgeType->getLaneTypes().at(myLaneIndex)->setAttribute(SUMO_ATTR_ALLOW, "all");
        // set width
        edgeType->getLaneTypes().at(myLaneIndex)->setAttribute(SUMO_ATTR_WIDTH, "-1.00");
        // set parameters
        edgeType->getLaneTypes().at(myLaneIndex)->setAttribute(GNE_ATTR_PARAMETERS, "");
    }
}

// ---------------------------------------------------------------------------
// GNECreateEdgeFrame::Legend - methods
// ---------------------------------------------------------------------------

GNECreateEdgeFrame::EdgeTypeSelectorLegend::EdgeTypeSelectorLegend(GNECreateEdgeFrame* createEdgeFrameParent) :
    FXGroupBox(createEdgeFrameParent->myContentFrame, "Legend", GUIDesignGroupBoxFrame) {
    // crate information
    std::ostringstream information;
    // add label for shift+click
    information
            << "- ESC:" << "\n"
            << "  Deselect origin" << "\n"
            << "- Control+Click:" << "\n"
            << "  Move view" << "\n"
            << "- Shift+Click:" << "\n"
            << "  Splits edge in both directions" << "\n"
            << "- Alt+Shift+Click:" << "\n"
            << "  Splits edge in one direction";
    // create label
    new FXLabel(this, information.str().c_str(), 0, GUIDesignLabelFrameThicked);
}


GNECreateEdgeFrame::EdgeTypeSelectorLegend::~EdgeTypeSelectorLegend() {}

// ---------------------------------------------------------------------------
// GNECreateEdgeFrame - methods
// ---------------------------------------------------------------------------

GNECreateEdgeFrame::GNECreateEdgeFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Create Edge"),
    myObjectsUnderSnappedCursor(viewNet),
    myCreateEdgeSource(nullptr) {
    // create custom edge selector
    myEdgeTypeSelector = new EdgeTypeSelector(this);
    // Create edgeType parameters
    myEdgeTypeAttributes = new GNEFrameAttributesModuls::AttributesCreator(this);
    // lane type selector
    myLaneTypeSelector = new LaneTypeSelector(this);
    // Create laneType parameters
    myLaneTypeAttributes = new GNEFrameAttributesModuls::AttributesCreator(this);
    // create edge selector legend
    myEdgeTypeSelectorLegend = new EdgeTypeSelectorLegend(this);
}


GNECreateEdgeFrame::~GNECreateEdgeFrame() {}


void
GNECreateEdgeFrame::processClick(const Position& clickedPosition, const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor,
                                 const bool oppositeEdge, const bool chainEdge) {
    // obtain junction depending of gridEnabled
    GNEJunction* junction = nullptr;
    if (objectsUnderCursor.getJunctionFront()) {
        junction = objectsUnderCursor.getJunctionFront();
    } else if (myObjectsUnderSnappedCursor.getJunctionFront()) {
        junction = myObjectsUnderSnappedCursor.getJunctionFront();
    }
    // begin undo list
    if (!myViewNet->getUndoList()->hasCommandGroup()) {
        myViewNet->getUndoList()->begin(GUIIcon::EDGE, "create new " + toString(SUMO_TAG_EDGE));
    }
    // if we didn't clicked over another junction, then create a new
    if (junction == nullptr) {
        junction = myViewNet->getNet()->createJunction(myViewNet->snapToActiveGrid(clickedPosition), myViewNet->getUndoList());
    }
    // now check if we have to create a new edge
    if (myCreateEdgeSource == nullptr) {
        myCreateEdgeSource = junction;
        myCreateEdgeSource->markAsCreateEdgeSource();
        update();
    } else {
        // make sure that junctions source and destiny are different
        if (myCreateEdgeSource != junction) {
            // may fail to prevent double edges
            GNEEdge* newEdge = myViewNet->getNet()->createEdge(myCreateEdgeSource, junction, nullptr, myViewNet->getUndoList());
            // check if edge was sucesfully created
            if (newEdge) {
                // set parameters
                if (myEdgeTypeSelector->useEdgeTemplate()) {
                    newEdge->copyTemplate(myViewNet->getViewParent()->getInspectorFrame()->getTemplateEditor()->getEdgeTemplate(), myViewNet->getUndoList());
                } else if (myEdgeTypeSelector->useDefaultEdgeType()) {
                    newEdge->copyEdgeType(myEdgeTypeSelector->getDefaultEdgeType(), myViewNet->getUndoList());
                } else {
                    newEdge->copyEdgeType(myEdgeTypeSelector->getEdgeTypeSelected(), myViewNet->getUndoList());
                }
                // create another edge, if create opposite edge is enabled
                if (oppositeEdge && (myViewNet->getNet()->getAttributeCarriers()->retrieveEdge(junction, myCreateEdgeSource, false) == nullptr)) {
                    GNEEdge* newOppositeEdge = myViewNet->getNet()->createEdge(junction, myCreateEdgeSource, nullptr,
                                               myViewNet->getUndoList(), "-" + newEdge->getNBEdge()->getID());
                    // set parameters
                    if (myEdgeTypeSelector->useEdgeTemplate()) {
                        newOppositeEdge->copyTemplate(myViewNet->getViewParent()->getInspectorFrame()->getTemplateEditor()->getEdgeTemplate(), myViewNet->getUndoList());
                    } else if (myEdgeTypeSelector->useDefaultEdgeType()) {
                        newOppositeEdge->copyEdgeType(myEdgeTypeSelector->getDefaultEdgeType(), myViewNet->getUndoList());
                    } else {
                        newOppositeEdge->copyEdgeType(myEdgeTypeSelector->getEdgeTypeSelected(), myViewNet->getUndoList());
                    }
                }
                // edge created, then unmark as create edge source
                myCreateEdgeSource->unMarkAsCreateEdgeSource();
                // end undo list
                if (myViewNet->getUndoList()->hasCommandGroup()) {
                    myViewNet->getUndoList()->end();
                } else {
                    std::cout << "edge created without an open CommandGroup )-:\n";
                }
                // if we're creating edges in chain mode, mark junction as junction edge source
                if (chainEdge) {
                    myCreateEdgeSource = junction;
                    myCreateEdgeSource->markAsCreateEdgeSource();
                    myViewNet->getUndoList()->begin(GUIIcon::EDGE, "create new " + toString(SUMO_TAG_EDGE));
                } else {
                    myCreateEdgeSource = nullptr;
                }
            } else {
                myViewNet->setStatusBarText("An " + toString(SUMO_TAG_EDGE) + " with the same geometry already exists!");
            }
        } else {
            myViewNet->setStatusBarText("Start- and endpoint for an " + toString(SUMO_TAG_EDGE) + " must be distinct!");
        }
        update();
    }
}


void
GNECreateEdgeFrame::abortEdgeCreation() {
    // if myCreateEdgeSource exist, unmark ist as create edge source
    if (myCreateEdgeSource != nullptr) {
        // remove current created edge source
        myCreateEdgeSource->unMarkAsCreateEdgeSource();
        myCreateEdgeSource = nullptr;
    }
}


const GNEJunction*
GNECreateEdgeFrame::getJunctionSource() const {
    return myCreateEdgeSource;
}


void
GNECreateEdgeFrame::updateObjectsUnderSnappedCursor(const std::vector<GUIGlObject*>& GUIGlObjects) {
    myObjectsUnderSnappedCursor.updateObjectUnderCursor(GUIGlObjects);
}


void
GNECreateEdgeFrame::show() {
    // refresh template selector
    myEdgeTypeSelector->refreshEdgeTypeSelector(true);
    // show frame
    GNEFrame::show();
}


void
GNECreateEdgeFrame::hide() {
    // hide frame
    GNEFrame::hide();
}


GNECreateEdgeFrame::EdgeTypeSelector*
GNECreateEdgeFrame::getEdgeTypeSelector() const {
    return myEdgeTypeSelector;
}


GNEFrameAttributesModuls::AttributesCreator* 
GNECreateEdgeFrame::getEdgeTypeAttributes() const {
    return myEdgeTypeAttributes;
}


GNEFrameAttributesModuls::AttributesCreator* 
GNECreateEdgeFrame::getLaneTypeAttributes() const {
    return myLaneTypeAttributes;
}


void
GNECreateEdgeFrame::setUseEdgeTemplate() {
    myEdgeTypeSelector->useTemplate();
}

/****************************************************************************/
