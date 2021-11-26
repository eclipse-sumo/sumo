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
#include <netedit/changes/GNEChange_LaneType.h>
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
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_RESETEDGETYPE,      GNECreateEdgeFrame::EdgeTypeSelector::onCmdResetEdgeType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_SELECTEDGETYPE,     GNECreateEdgeFrame::EdgeTypeSelector::onCmdSelectEdgeType),
};

FXDEFMAP(GNECreateEdgeFrame::LaneTypeParameters) LaneTypeParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,                      GNECreateEdgeFrame::LaneTypeParameters::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,               GNECreateEdgeFrame::LaneTypeParameters::onCmdOpenAttributeDialog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_DELETELANETYPE,     GNECreateEdgeFrame::LaneTypeParameters::onCmdDeleteLaneType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_RESETLANETYPE,      GNECreateEdgeFrame::LaneTypeParameters::onCmdResetLaneType),
};

FXDEFMAP(GNECreateEdgeFrame::EdgeTypeParameters) EdgeTypeParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNECreateEdgeFrame::EdgeTypeParameters::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNECreateEdgeFrame::EdgeTypeParameters::onCmdOpenAttributeDialog),
};

// Object implementation
FXIMPLEMENT(GNECreateEdgeFrame::EdgeTypeSelector,       FXGroupBox,     EdgeTypeSelectorMap,    ARRAYNUMBER(EdgeTypeSelectorMap))
FXIMPLEMENT(GNECreateEdgeFrame::LaneTypeParameters,     FXGroupBox,     LaneTypeParametersMap,  ARRAYNUMBER(LaneTypeParametersMap))
FXIMPLEMENT(GNECreateEdgeFrame::EdgeTypeParameters,     FXGroupBox,     EdgeTypeParametersMap,  ARRAYNUMBER(EdgeTypeParametersMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNECreateEdgeFrame::EdgeTypeSelector - methods
// ---------------------------------------------------------------------------

GNECreateEdgeFrame::EdgeTypeSelector::EdgeTypeSelector(GNECreateEdgeFrame* createEdgeFrameParent) :
    FXGroupBox(createEdgeFrameParent->myContentFrame, "Template selector", GUIDesignGroupBoxFrame),
    myCreateEdgeFrameParent(createEdgeFrameParent),
    myEdgeTypeSelected(nullptr),
    myDefaultEdgeType(new GNEEdgeType(createEdgeFrameParent)) {
    // default edge radio button
    myUseDefaultEdgeType = new FXRadioButton(this,
            "Create default edge", this, MID_GNE_CREATEEDGEFRAME_SELECTRADIOBUTTON, GUIDesignRadioButton);
    // use custom edge radio button
    myUseCustomEdgeType = new FXRadioButton(this,
                                            "Use edgeType/template", this, MID_GNE_CREATEEDGEFRAME_SELECTRADIOBUTTON, GUIDesignRadioButton);
    // edge types combo box
    myEdgeTypesComboBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_CREATEEDGEFRAME_SELECTEDGETYPE, GUIDesignComboBoxAttribute);
    // create horizontal frame
    FXHorizontalFrame* horizontalFrameButtons = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // create new edge type button
    myAddEdgeTypeButton = new FXButton(horizontalFrameButtons,
                                       "add\t\add edge type", GUIIconSubSys::getIcon(GUIIcon::ADD), this, MID_GNE_CREATEEDGEFRAME_ADDEDGETYPE, GUIDesignButton);
    // create delete edge type button
    myDeleteEdgeTypeButton = new FXButton(horizontalFrameButtons,
                                          "delete\t\tdelete edge type", GUIIconSubSys::getIcon(GUIIcon::REMOVE), this, MID_GNE_CREATEEDGEFRAME_DELETEEDGETYPE, GUIDesignButton);
    // create reset edge type button
    myResetEdgeTypeButton = new FXButton(horizontalFrameButtons,
                                         "reset\t\treset edge type", GUIIconSubSys::getIcon(GUIIcon::RESET), this, MID_GNE_CREATEEDGEFRAME_RESETEDGETYPE, GUIDesignButton);
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
    myCreateEdgeFrameParent->myEdgeTypeParameters->showEdgeTypeParameters();
    // check conditions
    if (show && templateEditor->hasTemplate()) {
        // set buttons
        myUseDefaultEdgeType->setCheck(FALSE);
        myUseCustomEdgeType->setCheck(TRUE);
        // set template as current item
        myEdgeTypesComboBox->setCurrentItem(0);
        // update edge parameters (using template
        myCreateEdgeFrameParent->myEdgeTypeParameters->setTemplateValues();
        // disable delete edge type button (because templates cannot be removed)
        myDeleteEdgeTypeButton->disable();
        myResetEdgeTypeButton->disable();
        // disable parameter fields (because templates cannot be edited)
        myCreateEdgeFrameParent->myEdgeTypeParameters->disableEdgeTypeParameters();
    } else if (myUseDefaultEdgeType->getCheck() == TRUE) {
        // disable comboBox
        myEdgeTypesComboBox->disable();
        // disable buttons
        myAddEdgeTypeButton->disable();
        myDeleteEdgeTypeButton->disable();
        // enable parameter fields (because default edge can be edited)
        myCreateEdgeFrameParent->myEdgeTypeParameters->enableEdgeTypeParameters();
        // set default parameters
        myCreateEdgeFrameParent->myEdgeTypeParameters->setEdgeType(myDefaultEdgeType, false);
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
            // update edge parameters
            myCreateEdgeFrameParent->myEdgeTypeParameters->setEdgeType(myEdgeTypeSelected, true);
            // enable parameter fields
            myCreateEdgeFrameParent->myEdgeTypeParameters->enableEdgeTypeParameters();
        } else if (templateEditor->hasTemplate()) {
            // set template as current item
            myEdgeTypesComboBox->setCurrentItem(0);
            // update edge parameters (using template
            myCreateEdgeFrameParent->myEdgeTypeParameters->setTemplateValues();
            // disable delete edge type button (because templates cannot be removed)
            myDeleteEdgeTypeButton->disable();
            myResetEdgeTypeButton->disable();
            // disable parameter fields (because templates cannot be edited)
            myCreateEdgeFrameParent->myEdgeTypeParameters->disableEdgeTypeParameters();
        } else if (edgeTypes.size() > 0) {
            // set myEdgeTypeSelected
            myEdgeTypeSelected = edgeTypes.begin()->second;
            // set current item
            myEdgeTypesComboBox->setCurrentItem(0);
            // enable delete edge type button
            myDeleteEdgeTypeButton->enable();
            // update edge parameters
            myCreateEdgeFrameParent->myEdgeTypeParameters->setEdgeType(myEdgeTypeSelected, true);
            // enable parameter fields (because edgeTypes can be edited)
            myCreateEdgeFrameParent->myEdgeTypeParameters->enableEdgeTypeParameters();
        }
    } else {
        // hide parameter fields
        myCreateEdgeFrameParent->myEdgeTypeParameters->hideEdgeTypeParameters();
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
    return myEdgeTypeSelected;
}


void
GNECreateEdgeFrame::EdgeTypeSelector::clearEdgeTypeSelected() {
    myEdgeTypeSelected = nullptr;
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
    // also create a new laneType
    GNELaneType* laneType = new GNELaneType(edgeType);
    // add it using undoList
    myCreateEdgeFrameParent->getViewNet()->getUndoList()->begin(GUIIcon::EDGE, "create new edge type");
    myCreateEdgeFrameParent->getViewNet()->getUndoList()->add(new GNEChange_EdgeType(edgeType, true), true);
    myCreateEdgeFrameParent->getViewNet()->getUndoList()->add(new GNEChange_LaneType(laneType, 0, true), true);
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
    myCreateEdgeFrameParent->getViewNet()->getUndoList()->begin(GUIIcon::EDGE, "create new edge type");
    // iterate over all laneType
    for (int i = 0; i < (int)edgeType->getLaneTypes().size(); i++) {
        myCreateEdgeFrameParent->getViewNet()->getUndoList()->add(new GNEChange_LaneType(edgeType->getLaneTypes().at(i), i, false), true);
    }
    myCreateEdgeFrameParent->getViewNet()->getUndoList()->add(new GNEChange_EdgeType(edgeType, false), true);
    myCreateEdgeFrameParent->getViewNet()->getUndoList()->end();
    // refresh EdgeTypeSelector
    refreshEdgeTypeSelector();
    return 0;
}


long
GNECreateEdgeFrame::EdgeTypeSelector::onCmdResetEdgeType(FXObject*, FXSelector, void*) {
    // get options
    const OptionsCont& oc = OptionsCont::getOptions();
    // get undoList
    GNEUndoList* undoList = myCreateEdgeFrameParent->getViewNet()->getUndoList();
    // check if we're editing an existent edgeType
    if (useDefaultEdgeType()) {
        // reset speed
        myDefaultEdgeType->setAttribute(SUMO_ATTR_SPEED, toString(oc.getFloat("default.speed")));
        // reset lanes
        while (myDefaultEdgeType->getLaneTypes().size() > 1) {
            myDefaultEdgeType->removeLaneType(myDefaultEdgeType->getLaneTypes().back());
        }
        // reset disallow (and allow)
        myDefaultEdgeType->setAttribute(SUMO_ATTR_DISALLOW, oc.getString("default.disallow"));
        // reset spreadType
        myDefaultEdgeType->setAttribute(SUMO_ATTR_SPREADTYPE, "right");
        // reset width
        myDefaultEdgeType->setAttribute(SUMO_ATTR_WIDTH, toString(NBEdge::UNSPECIFIED_WIDTH));
        // reset parameters
        myDefaultEdgeType->setAttribute(GNE_ATTR_PARAMETERS, "");
    } else if (myEdgeTypeSelected) {
        // begin undoList
        undoList->begin(GUIIcon::EDGE, "reset edgeType '" + myDefaultEdgeType->getID() + "'");
        // reset lanes
        while (myEdgeTypeSelected->getLaneTypes().size() > 1) {
            myEdgeTypeSelected->removeLaneType((int)myEdgeTypeSelected->getLaneTypes().size() - 1, undoList);
        }
        // reset speed
        myEdgeTypeSelected->setAttribute(SUMO_ATTR_SPEED, toString(oc.getFloat("default.speed")), undoList);
        // reset disallow (and allow)
        myEdgeTypeSelected->setAttribute(SUMO_ATTR_DISALLOW, oc.getString("default.disallow"));
        // reset spreadType
        myEdgeTypeSelected->setAttribute(SUMO_ATTR_SPREADTYPE, "right");
        // reset width
        myEdgeTypeSelected->setAttribute(SUMO_ATTR_WIDTH, toString(NBEdge::UNSPECIFIED_WIDTH), undoList);
        // reset parameters
        myEdgeTypeSelected->setAttribute(GNE_ATTR_PARAMETERS, "", undoList);
        // end undoList
        undoList->end();
    }
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
        myCreateEdgeFrameParent->myEdgeTypeParameters->showEdgeTypeParameters();
        // disable parameter fields (because templantes cannot be edited)
        myCreateEdgeFrameParent->myEdgeTypeParameters->disableEdgeTypeParameters();
        // set edge template in myEdgeTypeParameters
        myCreateEdgeFrameParent->myEdgeTypeParameters->setTemplateValues();
    } else if (edgeTypes.count(myEdgeTypesComboBox->getText().text()) > 0) {
        // set valid color
        myEdgeTypesComboBox->setTextColor(FXRGB(0, 0, 0));
        myEdgeTypesComboBox->killFocus();
        // enable delete edge type button
        myDeleteEdgeTypeButton->enable();
        // show parameter fields
        myCreateEdgeFrameParent->myEdgeTypeParameters->showEdgeTypeParameters();
        // enable parameter fields
        myCreateEdgeFrameParent->myEdgeTypeParameters->enableEdgeTypeParameters();
        // set myEdgeTypeSelected
        myEdgeTypeSelected = myCreateEdgeFrameParent->myViewNet->getNet()->getAttributeCarriers()->retrieveEdgeType(myEdgeTypesComboBox->getText().text());
        // set edgeType in myEdgeTypeParameters
        myCreateEdgeFrameParent->myEdgeTypeParameters->setEdgeType(myEdgeTypeSelected, true);
    } else {
        // set invalid color
        myEdgeTypesComboBox->setTextColor(FXRGB(255, 0, 0));
        // disable delete edge type button
        myDeleteEdgeTypeButton->disable();
        // hide parameter fields
        myCreateEdgeFrameParent->myEdgeTypeParameters->hideEdgeTypeParameters();
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
    // set disallow
    myDefaultEdgeType->setAttribute(SUMO_ATTR_DISALLOW, "");
    // set spreadType
    myDefaultEdgeType->setAttribute(SUMO_ATTR_SPREADTYPE, "");
    // set width
    myDefaultEdgeType->setAttribute(SUMO_ATTR_WIDTH, "-1.00");
    // set parameters
    myDefaultEdgeType->setAttribute(GNE_ATTR_PARAMETERS, "");
}

// ---------------------------------------------------------------------------
// GNECreateEdgeFrame::LaneTypeParameters - methods
// ---------------------------------------------------------------------------

GNECreateEdgeFrame::LaneTypeParameters::LaneTypeParameters(GNECreateEdgeFrame* createEdgeFrameParent) :
    FXGroupBox(createEdgeFrameParent->myContentFrame, "Lane parameters", GUIDesignGroupBoxFrame),
    myCreateEdgeFrameParent(createEdgeFrameParent) {
    // declare horizontalFrameAttribute
    FXHorizontalFrame* horizontalFrameAttribute = nullptr;
    // create ComboBox for spread type
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(horizontalFrameAttribute, "Lane index", nullptr, GUIDesignLabelAttribute);
    myLaneIndex = new FXComboBox(horizontalFrameAttribute, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBoxAttribute);
    // create horizontalFrameAttribute for buttons
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // create delete edge type button
    myDeleteLaneTypeButton = new FXButton(horizontalFrameAttribute,
                                          "delete\t\tdelete lane type", GUIIconSubSys::getIcon(GUIIcon::REMOVE), this, MID_GNE_CREATEEDGEFRAME_DELETELANETYPE, GUIDesignButton);
    // create reset edge type button
    myResetLaneTypeButton = new FXButton(horizontalFrameAttribute,
                                         "reset\t\treset lane type", GUIIconSubSys::getIcon(GUIIcon::RESET), this, MID_GNE_CREATEEDGEFRAME_RESETLANETYPE, GUIDesignButton);
    // create textField for speed
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(horizontalFrameAttribute, toString(SUMO_ATTR_SPEED).c_str(), nullptr, GUIDesignLabelAttribute);
    mySpeed = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create Button for allow vehicles
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myAllowButton = new FXButton(horizontalFrameAttribute, toString(SUMO_ATTR_ALLOW).c_str(), nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myAllow = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create Button for disallow vehicles
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myDisallowButton = new FXButton(horizontalFrameAttribute, toString(SUMO_ATTR_DISALLOW).c_str(), nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myDisallow = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create textField for width
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(horizontalFrameAttribute, toString(SUMO_ATTR_WIDTH).c_str(), nullptr, GUIDesignLabelAttribute);
    myWidth = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create textField for parameters
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myParametersButton = new FXButton(horizontalFrameAttribute, "parameters", nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myParameters = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
}


GNECreateEdgeFrame::LaneTypeParameters::~LaneTypeParameters() {}


void
GNECreateEdgeFrame::LaneTypeParameters::refreshLaneTypeParameters() {
    // save previous laneIndex
    const int previousLaneIndex = (myLaneIndex->getNumItems() > 0) ? myLaneIndex->getCurrentItem() : 0;
    // first get GNEEdgeType
    GNEEdgeType* edgeType = nullptr;
    if (myCreateEdgeFrameParent->myEdgeTypeSelector->useDefaultEdgeType()) {
        edgeType = myCreateEdgeFrameParent->myEdgeTypeSelector->getDefaultEdgeType();
    } else {
        edgeType = myCreateEdgeFrameParent->myEdgeTypeSelector->getEdgeTypeSelected();
    }
    // check edge type
    if (edgeType) {
        // clear and refill myLaneIndex
        myLaneIndex->clearItems();
        for (int i = 0; i < (int)edgeType->getLaneTypes().size(); i++) {
            myLaneIndex->appendItem(toString(i).c_str());
        }
        // set visible items
        myLaneIndex->setNumVisible(myLaneIndex->getNumItems());
        // continue checking number of lanes
        if (previousLaneIndex < (int)edgeType->getLaneTypes().size()) {
            myLaneIndex->setCurrentItem(previousLaneIndex);
        } else {
            myLaneIndex->setCurrentItem(0);
        }
        // set speed
        mySpeed->setText(edgeType->getLaneTypes().at(myLaneIndex->getCurrentItem())->getAttribute(SUMO_ATTR_SPEED).c_str(), FALSE);
        mySpeed->setTextColor(FXRGB(0, 0, 0));
        // set allow
        myAllow->setText(edgeType->getLaneTypes().at(myLaneIndex->getCurrentItem())->getAttribute(SUMO_ATTR_ALLOW).c_str(), FALSE);
        myAllow->setTextColor(FXRGB(0, 0, 0));
        // set disallow
        myDisallow->setText(edgeType->getLaneTypes().at(myLaneIndex->getCurrentItem())->getAttribute(SUMO_ATTR_DISALLOW).c_str(), FALSE);
        myDisallow->setTextColor(FXRGB(0, 0, 0));
        // set width
        myWidth->setText(edgeType->getLaneTypes().at(myLaneIndex->getCurrentItem())->getAttribute(SUMO_ATTR_WIDTH).c_str(), FALSE);
        myWidth->setTextColor(FXRGB(0, 0, 0));
        // set parameters
        myParameters->setText(edgeType->getLaneTypes().at(myLaneIndex->getCurrentItem())->getAttribute(GNE_ATTR_PARAMETERS).c_str(), FALSE);
        myParameters->setTextColor(FXRGB(0, 0, 0));
    }
}


void
GNECreateEdgeFrame::LaneTypeParameters::showLaneTypeParameters() {
    show();
}


void
GNECreateEdgeFrame::LaneTypeParameters::hideLaneTypeParameters() {
    hide();
}


void
GNECreateEdgeFrame::LaneTypeParameters::enableLaneTypeParameters() {
    myLaneIndex->enable();
    mySpeed->enable();
    myAllowButton->enable();
    myAllow->enable();
    myDisallowButton->enable();
    myDisallow->enable();
    myWidth->enable();
}


void
GNECreateEdgeFrame::LaneTypeParameters::disableLaneTypeParameters() {
    myLaneIndex->disable();
    mySpeed->disable();
    myAllowButton->disable();
    myAllow->disable();
    myDisallowButton->disable();
    myDisallow->disable();
    myWidth->disable();
}


void
GNECreateEdgeFrame::LaneTypeParameters::setAttributes(GNEEdge* edge, GNEUndoList* undoList) const {
    // set speed
    edge->setAttribute(SUMO_ATTR_SPEED, toString(mySpeed->getText().text()), undoList);
    // set allow (no disallow)
    edge->setAttribute(SUMO_ATTR_ALLOW, toString(myAllow->getText().text()), undoList);
    // set witdth
    edge->setAttribute(SUMO_ATTR_WIDTH, toString(myWidth->getText().text()), undoList);
}


void
GNECreateEdgeFrame::LaneTypeParameters::updateNumLanes(int numLanes) {
    // first check if numLanes > 0
    if (numLanes > 0) {
        // get edgeTypeSelector
        GNECreateEdgeFrame::EdgeTypeSelector* edgeTypeSelector = myCreateEdgeFrameParent->getEdgeTypeSelector();
        // change numLanes depending of edited edgeType
        if (edgeTypeSelector->useDefaultEdgeType()) {
            // check if we have to add new lanes
            while (numLanes > (int)edgeTypeSelector->getDefaultEdgeType()->getLaneTypes().size()) {
                // create new GNELaneType
                GNELaneType* laneType = new GNELaneType(edgeTypeSelector->getDefaultEdgeType());
                // add it in the last position
                edgeTypeSelector->getDefaultEdgeType()->addLaneType(laneType, (int)edgeTypeSelector->getDefaultEdgeType()->getLaneTypes().size());
            }
            // check if we have to remove new lanes
            while (numLanes < (int)edgeTypeSelector->getDefaultEdgeType()->getLaneTypes().size()) {
                // remove it from the last position
                edgeTypeSelector->getDefaultEdgeType()->removeLaneType(edgeTypeSelector->getDefaultEdgeType()->getLaneTypes().back());
            }
        } else if (myCreateEdgeFrameParent->getEdgeTypeSelector()->getEdgeTypeSelected()) {
            // check if we have to add new lanes
            while (numLanes > (int)edgeTypeSelector->getEdgeTypeSelected()->getLaneTypes().size()) {
                // add it in the last position
                edgeTypeSelector->getEdgeTypeSelected()->addLaneType(myCreateEdgeFrameParent->getViewNet()->getUndoList());
            }
            // check if we have to remove new lanes
            while (numLanes < (int)edgeTypeSelector->getEdgeTypeSelected()->getLaneTypes().size()) {
                // remove it from the last position
                edgeTypeSelector->getEdgeTypeSelected()->removeLaneType((int)edgeTypeSelector->getEdgeTypeSelected()->getLaneTypes().size() - 1, myCreateEdgeFrameParent->getViewNet()->getUndoList());
            }

        }
        // refresh
        refreshLaneTypeParameters();
    }
}


long
GNECreateEdgeFrame::LaneTypeParameters::onCmdSetAttribute(FXObject* obj, FXSelector, void*) {
    if (obj == myLaneIndex) {
        // refresh
        refreshLaneTypeParameters();
    } else if (myCreateEdgeFrameParent->myEdgeTypeSelector->useDefaultEdgeType()) {
        setAttributeDefaultParameters(obj);
    } else {
        setAttributeExistentLaneType(obj);
    }
    return 1;
}


long
GNECreateEdgeFrame::LaneTypeParameters::onCmdOpenAttributeDialog(FXObject* obj, FXSelector, void*) {
    if (obj == myParametersButton) {
        // write debug information
        WRITE_DEBUG("Open parameters dialog");
        // check if use default edge
        if (myCreateEdgeFrameParent->myEdgeTypeSelector->useDefaultEdgeType()) {
            // get lane type
            GNELaneType* laneType = myCreateEdgeFrameParent->myEdgeTypeSelector->getDefaultEdgeType()->getLaneTypes().at(myLaneIndex->getCurrentItem());
            // edit parameters using dialog
            if (GNESingleParametersDialog(laneType).execute()) {
                // write debug information
                WRITE_DEBUG("Close parameters dialog");
                // update myParameters text field
                myParameters->setText(laneType->getAttribute(GNE_ATTR_PARAMETERS).c_str(), FALSE);
            } else {
                // write debug information
                WRITE_DEBUG("Cancel parameters dialog");
            }
        } else if (myCreateEdgeFrameParent->myEdgeTypeSelector->getEdgeTypeSelected()) {
            // get lane type
            GNELaneType* laneType = myCreateEdgeFrameParent->myEdgeTypeSelector->getEdgeTypeSelected()->getLaneTypes().at(myLaneIndex->getCurrentItem());
            // edit parameters using dialog
            if (GNESingleParametersDialog(laneType).execute()) {
                // write debug information
                WRITE_DEBUG("Close parameters dialog");
                // update myParameters text field
                myParameters->setText(laneType->getAttribute(GNE_ATTR_PARAMETERS).c_str(), FALSE);
            } else {
                // write debug information
                WRITE_DEBUG("Cancel parameters dialog");
            }
        }
    } else {
        // declare strings
        std::string allow = myAllow->getText().text();
        std::string disallow = myDisallow->getText().text();
        // declare accept changes
        bool acceptChanges = false;
        // open allow/disallow
        GNEAllowDisallow(myCreateEdgeFrameParent->getViewNet(), &allow, &disallow, &acceptChanges).execute();
        // continue depending of acceptChanges
        if (acceptChanges) {
            // update allow/disallow
            myAllow->setText(allow.c_str(), FALSE);
            myDisallow->setText(disallow.c_str(), FALSE);
            // set attribute
            if (myCreateEdgeFrameParent->myEdgeTypeSelector->useDefaultEdgeType()) {
                myCreateEdgeFrameParent->myEdgeTypeSelector->getDefaultEdgeType()->getLaneTypes().at(myLaneIndex->getCurrentItem())->setAttribute(SUMO_ATTR_ALLOW, allow.c_str());
            } else if (myCreateEdgeFrameParent->myEdgeTypeSelector->getEdgeTypeSelected()) {
                myCreateEdgeFrameParent->myEdgeTypeSelector->getEdgeTypeSelected()->getLaneTypes().at(myLaneIndex->getCurrentItem())->setAttribute(SUMO_ATTR_ALLOW, allow.c_str());
            }
        }
    }
    return 1;
}


long
GNECreateEdgeFrame::LaneTypeParameters::onCmdDeleteLaneType(FXObject*, FXSelector, void*) {
    /* */
    return 1;
}


long
GNECreateEdgeFrame::LaneTypeParameters::onCmdResetLaneType(FXObject*, FXSelector, void*) {
    /* */
    return 1;
}


void
GNECreateEdgeFrame::LaneTypeParameters::setAttributeDefaultParameters(FXObject* obj) {
    // get default edge type
    GNELaneType* defaultLaneType = myCreateEdgeFrameParent->myEdgeTypeSelector->getDefaultEdgeType()->getLaneTypes().at(myLaneIndex->getCurrentItem());
    // check what attribute was changed
    if (obj == mySpeed) {
        // check if is valid
        if (defaultLaneType->isValid(SUMO_ATTR_SPEED, mySpeed->getText().text())) {
            // set attribute (Without undoList)
            defaultLaneType->setAttribute(SUMO_ATTR_SPEED, mySpeed->getText().text());
            // reset color
            mySpeed->setTextColor(FXRGB(0, 0, 0));
            mySpeed->killFocus();
        } else {
            mySpeed->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == myAllow) {
        // check if is valid
        if (defaultLaneType->isValid(SUMO_ATTR_ALLOW, myAllow->getText().text())) {
            // set attribute (Without undoList)
            defaultLaneType->setAttribute(SUMO_ATTR_ALLOW, myAllow->getText().text());
            // reset color
            myAllow->setTextColor(FXRGB(0, 0, 0));
            myAllow->killFocus();
            // update disallow textField
            myDisallow->setText(defaultLaneType->getAttribute(SUMO_ATTR_DISALLOW).c_str(), FALSE);
        } else {
            myAllow->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == myDisallow) {
        // check if is valid
        if (defaultLaneType->isValid(SUMO_ATTR_DISALLOW, myDisallow->getText().text())) {
            // set attribute (Without undoList)
            defaultLaneType->setAttribute(SUMO_ATTR_DISALLOW, myDisallow->getText().text());
            // reset color
            myDisallow->setTextColor(FXRGB(0, 0, 0));
            myDisallow->killFocus();
            // update allow textField
            myAllow->setText(defaultLaneType->getAttribute(SUMO_ATTR_ALLOW).c_str(), FALSE);
        } else {
            myDisallow->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == myWidth) {
        // check if is valid
        if (defaultLaneType->isValid(SUMO_ATTR_WIDTH, myWidth->getText().text())) {
            // set attribute (Without undoList)
            defaultLaneType->setAttribute(SUMO_ATTR_WIDTH, myWidth->getText().text());
            // reset color
            myWidth->setTextColor(FXRGB(0, 0, 0));
            myWidth->killFocus();
        } else {
            myWidth->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == myParameters) {
        // check if is valid
        if (defaultLaneType->isValid(GNE_ATTR_PARAMETERS, myParameters->getText().text())) {
            // set attribute (Without undoList)
            defaultLaneType->setAttribute(GNE_ATTR_PARAMETERS, myParameters->getText().text());
            // reset color
            myParameters->setTextColor(FXRGB(0, 0, 0));
            myParameters->killFocus();
        } else {
            myParameters->setTextColor(FXRGB(255, 0, 0));
        }
    }
}


void
GNECreateEdgeFrame::LaneTypeParameters::setAttributeExistentLaneType(FXObject* obj) {
    // get undoList
    GNEUndoList* undoList = myCreateEdgeFrameParent->myViewNet->getUndoList();
    // get selected lane type
    GNELaneType* laneType = myCreateEdgeFrameParent->myEdgeTypeSelector->getEdgeTypeSelected()->getLaneTypes().at(myLaneIndex->getCurrentItem());
    // check what attribute was changed
    if (obj == mySpeed) {
        // check if is valid
        if (laneType->isValid(SUMO_ATTR_SPEED, mySpeed->getText().text())) {
            // set attribute
            laneType->setAttribute(SUMO_ATTR_SPEED, mySpeed->getText().text(), undoList);
            // reset color
            mySpeed->setTextColor(FXRGB(0, 0, 0));
            mySpeed->killFocus();
        } else {
            mySpeed->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == myAllow) {
        // check if is valid
        if (laneType->isValid(SUMO_ATTR_ALLOW, myAllow->getText().text())) {
            // set attribute
            laneType->setAttribute(SUMO_ATTR_ALLOW, myAllow->getText().text(), undoList);
            // reset color
            myAllow->setTextColor(FXRGB(0, 0, 0));
            myAllow->killFocus();
            // update disallow textField
            myDisallow->setText(laneType->getAttribute(SUMO_ATTR_DISALLOW).c_str(), FALSE);
        } else {
            myAllow->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == myDisallow) {
        // check if is valid
        if (laneType->isValid(SUMO_ATTR_DISALLOW, myDisallow->getText().text())) {
            // set attribute
            laneType->setAttribute(SUMO_ATTR_DISALLOW, myDisallow->getText().text(), undoList);
            // reset color
            myDisallow->setTextColor(FXRGB(0, 0, 0));
            myDisallow->killFocus();
            // update allow textField
            myAllow->setText(laneType->getAttribute(SUMO_ATTR_ALLOW).c_str(), FALSE);
        } else {
            myDisallow->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == myWidth) {
        // check if is valid
        if (laneType->isValid(SUMO_ATTR_WIDTH, myWidth->getText().text())) {
            // set attribute
            laneType->setAttribute(SUMO_ATTR_WIDTH, myWidth->getText().text(), undoList);
            // reset color
            myWidth->setTextColor(FXRGB(0, 0, 0));
            myWidth->killFocus();
        } else {
            myWidth->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == myParameters) {
        // check if is valid
        if (laneType->isValid(GNE_ATTR_PARAMETERS, myParameters->getText().text())) {
            // set attribute
            laneType->setAttribute(GNE_ATTR_PARAMETERS, myParameters->getText().text(), undoList);
            // reset color
            myParameters->setTextColor(FXRGB(0, 0, 0));
            myParameters->killFocus();
        } else {
            myParameters->setTextColor(FXRGB(255, 0, 0));
        }
    }
}

// ---------------------------------------------------------------------------
// GNECreateEdgeFrame::EdgeTypeParameters - methods
// ---------------------------------------------------------------------------

GNECreateEdgeFrame::EdgeTypeParameters::EdgeTypeParameters(GNECreateEdgeFrame* createEdgeFrameParent) :
    FXGroupBox(createEdgeFrameParent->myContentFrame, "Edge parameters", GUIDesignGroupBoxFrame),
    myCreateEdgeFrameParent(createEdgeFrameParent) {
    // declare horizontalFrameAttribute
    FXHorizontalFrame* horizontalFrameAttribute = nullptr;
    // create textField for ID
    myHorizontalFrameID = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myLabelID = new FXLabel(myHorizontalFrameID, "edgeType id", nullptr, GUIDesignLabelAttribute);
    myID = new FXTextField(myHorizontalFrameID, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create textField for speed
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(horizontalFrameAttribute, toString(SUMO_ATTR_SPEED).c_str(), nullptr, GUIDesignLabelAttribute);
    mySpeed = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create textField for numLanes
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(horizontalFrameAttribute, toString(SUMO_ATTR_NUMLANES).c_str(), nullptr, GUIDesignLabelAttribute);
    myNumLanes = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldInt);
    // create Button for allow vehicles
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myAllowButton = new FXButton(horizontalFrameAttribute, toString(SUMO_ATTR_ALLOW).c_str(), nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myAllow = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create Button for disallow vehicles
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myDisallowButton = new FXButton(horizontalFrameAttribute, toString(SUMO_ATTR_DISALLOW).c_str(), nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myDisallow = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create combo box for spread type
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(horizontalFrameAttribute, toString(SUMO_ATTR_SPREADTYPE).c_str(), nullptr, GUIDesignLabelAttribute);
    mySpreadType = new FXComboBox(horizontalFrameAttribute, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBoxAttribute);
    // fill spreadType
    mySpreadType->appendItem("right");
    mySpreadType->appendItem("center");
    mySpreadType->appendItem("roadCenter");
    mySpreadType->setNumVisible(3);
    // create textField for width
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(horizontalFrameAttribute, toString(SUMO_ATTR_WIDTH).c_str(), nullptr, GUIDesignLabelAttribute);
    myWidth = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create textField for priority
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(horizontalFrameAttribute, toString(SUMO_ATTR_PRIORITY).c_str(), nullptr, GUIDesignLabelAttribute);
    myPriority = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create textField for parameters
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myParametersButton = new FXButton(horizontalFrameAttribute, "parameters", nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myParameters = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create lane parameters
    myLaneTypeParameters = new LaneTypeParameters(createEdgeFrameParent);
}


GNECreateEdgeFrame::EdgeTypeParameters::~EdgeTypeParameters() {}


void
GNECreateEdgeFrame::EdgeTypeParameters::showEdgeTypeParameters() {
    // show EdgeTypeParameters
    show();
    // show LaneTypeParameters
    myLaneTypeParameters->showLaneTypeParameters();
}


void
GNECreateEdgeFrame::EdgeTypeParameters::hideEdgeTypeParameters() {
    // hide EdgeTypeParameters
    hide();
    // hide LaneTypeParameters
    myLaneTypeParameters->hideLaneTypeParameters();
}


void
GNECreateEdgeFrame::EdgeTypeParameters::enableEdgeTypeParameters() {
    // enable all elements
    myID->enable();
    myNumLanes->enable();
    mySpeed->enable();
    myAllowButton->enable();
    myAllow->enable();
    myDisallowButton->enable();
    myDisallow->enable();
    mySpreadType->enable();
    myWidth->enable();
    myPriority->enable();
    myParameters->enable();
    // enable lane parameters
    myLaneTypeParameters->enableLaneTypeParameters();
}


void
GNECreateEdgeFrame::EdgeTypeParameters::disableEdgeTypeParameters() {
    // disable all elements
    myID->disable();
    myNumLanes->disable();
    mySpeed->disable();
    myAllowButton->disable();
    myAllow->disable();
    myDisallowButton->disable();
    myDisallow->disable();
    mySpreadType->disable();
    myWidth->disable();
    myPriority->disable();
    myParameters->disable();
    // disable lane parameters
    myLaneTypeParameters->disableLaneTypeParameters();
}


void
GNECreateEdgeFrame::EdgeTypeParameters::setEdgeType(GNEEdgeType* edgeType, bool showID) {
    // set id
    if (showID) {
        myHorizontalFrameID->show();
        myLabelID->setText("edgeType ID");
        myID->setText(edgeType->getAttribute(SUMO_ATTR_ID).c_str(), FALSE);
        myID->setTextColor(FXRGB(0, 0, 0));
    } else {
        myHorizontalFrameID->hide();
    }
    // set numLanes
    myNumLanes->setText(edgeType->getAttribute(SUMO_ATTR_NUMLANES).c_str(), FALSE);
    myNumLanes->setTextColor(FXRGB(0, 0, 0));
    // set speed
    mySpeed->setText(edgeType->getAttribute(SUMO_ATTR_SPEED).c_str(), FALSE);
    mySpeed->setTextColor(FXRGB(0, 0, 0));
    // set allow
    myAllow->setText(edgeType->getAttribute(SUMO_ATTR_ALLOW).c_str(), FALSE);
    myAllow->setTextColor(FXRGB(0, 0, 0));
    // set disallow
    myDisallow->setText(edgeType->getAttribute(SUMO_ATTR_DISALLOW).c_str(), FALSE);
    myDisallow->setTextColor(FXRGB(0, 0, 0));
    // set spreadType
    if (edgeType->getAttribute(SUMO_ATTR_SPREADTYPE) == "right") {
        mySpreadType->setCurrentItem(0);
    } else if (edgeType->getAttribute(SUMO_ATTR_SPREADTYPE) == "center") {
        mySpreadType->setCurrentItem(1);
    } else {
        mySpreadType->setCurrentItem(2);
    }
    mySpreadType->setTextColor(FXRGB(0, 0, 0));
    // set width
    myWidth->setText(edgeType->getAttribute(SUMO_ATTR_WIDTH).c_str(), FALSE);
    myWidth->setTextColor(FXRGB(0, 0, 0));
    // set width
    myPriority->setText(edgeType->getAttribute(SUMO_ATTR_PRIORITY).c_str(), FALSE);
    myPriority->setTextColor(FXRGB(0, 0, 0));
    // set parameters
    myParameters->setText(edgeType->getAttribute(GNE_ATTR_PARAMETERS).c_str(), FALSE);
    myParameters->setTextColor(FXRGB(0, 0, 0));
    // now update lane parameters
    myLaneTypeParameters->refreshLaneTypeParameters();
    // recalc frame
    recalc();
}


void
GNECreateEdgeFrame::EdgeTypeParameters::setTemplateValues() {
    // get template editor
    const GNEInspectorFrame::TemplateEditor* templateEditor = myCreateEdgeFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->getTemplateEditor();
    // first check if there is a template
    if (templateEditor->hasTemplate()) {
        // set ID
        myLabelID->setText("template ID");
        myID->setText(templateEditor->getEdgeTemplate().edgeParameters.at(SUMO_ATTR_ID).c_str(), FALSE);
        myHorizontalFrameID->show();
        // set numLanes
        myNumLanes->setText(templateEditor->getEdgeTemplate().edgeParameters.at(SUMO_ATTR_NUMLANES).c_str(), FALSE);
        myNumLanes->setTextColor(FXRGB(0, 0, 0));
        // set speed
        mySpeed->setText(templateEditor->getEdgeTemplate().edgeParameters.at(SUMO_ATTR_SPEED).c_str(), FALSE);
        mySpeed->setTextColor(FXRGB(0, 0, 0));
        // set allow
        myAllow->setText(templateEditor->getEdgeTemplate().edgeParameters.at(SUMO_ATTR_ALLOW).c_str(), FALSE);
        myAllow->setTextColor(FXRGB(0, 0, 0));
        // set disallow
        myDisallow->setText(templateEditor->getEdgeTemplate().edgeParameters.at(SUMO_ATTR_DISALLOW).c_str(), FALSE);
        myDisallow->setTextColor(FXRGB(0, 0, 0));
        // set spreadType
        if (templateEditor->getEdgeTemplate().edgeParameters.at(SUMO_ATTR_SPREADTYPE) == "right") {
            mySpreadType->setCurrentItem(0);
        } else if (templateEditor->getEdgeTemplate().edgeParameters.at(SUMO_ATTR_SPREADTYPE) == "center") {
            mySpreadType->setCurrentItem(1);
        } else {
            mySpreadType->setCurrentItem(2);
        }
        mySpreadType->setTextColor(FXRGB(0, 0, 0));
        // set width
        myWidth->setText(templateEditor->getEdgeTemplate().edgeParameters.at(SUMO_ATTR_WIDTH).c_str(), FALSE);
        myWidth->setTextColor(FXRGB(0, 0, 0));
        // set priority
        myPriority->setText(templateEditor->getEdgeTemplate().edgeParameters.at(SUMO_ATTR_PRIORITY).c_str(), FALSE);
        myPriority->setTextColor(FXRGB(0, 0, 0));
        // set parameters
        myParameters->setText(templateEditor->getEdgeTemplate().edgeParameters.at(GNE_ATTR_PARAMETERS).c_str(), FALSE);
        myParameters->setTextColor(FXRGB(0, 0, 0));
        // recalc frame
        recalc();
    } else {
        throw ProcessError("no template");
    }
}


long
GNECreateEdgeFrame::EdgeTypeParameters::onCmdSetAttribute(FXObject* obj, FXSelector, void*) {
    // check if we're editing an existent edgeType
    if (myCreateEdgeFrameParent->myEdgeTypeSelector->useDefaultEdgeType()) {
        setAttributeDefaultParameters(obj);
    } else {
        setAttributeExistentEdgeType(obj);
    }
    return 1;
}


long
GNECreateEdgeFrame::EdgeTypeParameters::onCmdOpenAttributeDialog(FXObject* obj, FXSelector, void*) {
    if (obj == myParametersButton) {
        // write debug information
        WRITE_DEBUG("Open parameters dialog");
        // check if use default edge
        if (myCreateEdgeFrameParent->myEdgeTypeSelector->useDefaultEdgeType()) {
            // edit parameters using dialog
            if (GNESingleParametersDialog(myCreateEdgeFrameParent->myEdgeTypeSelector->getDefaultEdgeType()).execute()) {
                // write debug information
                WRITE_DEBUG("Close parameters dialog");
                // update myParameters text field
                myParameters->setText(myCreateEdgeFrameParent->myEdgeTypeSelector->getDefaultEdgeType()->getAttribute(GNE_ATTR_PARAMETERS).c_str(), FALSE);
            } else {
                // write debug information
                WRITE_DEBUG("Cancel parameters dialog");
            }
        } else if (myCreateEdgeFrameParent->myEdgeTypeSelector->getEdgeTypeSelected()) {
            // edit parameters using dialog
            if (GNESingleParametersDialog(myCreateEdgeFrameParent->myEdgeTypeSelector->getEdgeTypeSelected()).execute()) {
                // write debug information
                WRITE_DEBUG("Close parameters dialog");
                // update myParameters text field
                myParameters->setText(myCreateEdgeFrameParent->myEdgeTypeSelector->getEdgeTypeSelected()->getAttribute(GNE_ATTR_PARAMETERS).c_str(), FALSE);
            } else {
                // write debug information
                WRITE_DEBUG("Cancel parameters dialog");
            }
        }
    } else {
        // declare strings
        std::string allow = myAllow->getText().text();
        std::string disallow = myDisallow->getText().text();
        // declare accept changes
        bool acceptChanges = false;
        // open allow/disallow
        GNEAllowDisallow(myCreateEdgeFrameParent->getViewNet(), &allow, &disallow, &acceptChanges).execute();
        // continue depending of acceptChanges
        if (acceptChanges) {
            // update allow/disallow
            myAllow->setText(allow.c_str(), TRUE);
            myDisallow->setText(disallow.c_str(), TRUE);
            // set attribute
            if (myCreateEdgeFrameParent->myEdgeTypeSelector->useDefaultEdgeType()) {
                myCreateEdgeFrameParent->myEdgeTypeSelector->getDefaultEdgeType()->setAttribute(SUMO_ATTR_ALLOW, allow.c_str());
            } else if (myCreateEdgeFrameParent->myEdgeTypeSelector->getEdgeTypeSelected()) {
                myCreateEdgeFrameParent->myEdgeTypeSelector->getEdgeTypeSelected()->setAttribute(SUMO_ATTR_ALLOW, allow.c_str());
            }
        }
    }
    return 1;
}


void
GNECreateEdgeFrame::EdgeTypeParameters::setAttributeDefaultParameters(FXObject* obj) {
    // get default edge type
    GNEEdgeType* defaultEdgeType = myCreateEdgeFrameParent->myEdgeTypeSelector->getDefaultEdgeType();
    // check what attribute was changed
    if (obj == myID) {
        // check if is valid
        if (defaultEdgeType->isValid(SUMO_ATTR_ID, myID->getText().text())) {
            // set attribute (Without undoList)
            defaultEdgeType->setAttribute(SUMO_ATTR_ID, myID->getText().text());
            // reset color
            myID->setTextColor(FXRGB(0, 0, 0));
            myID->killFocus();
        } else {
            myID->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == myNumLanes) {
        // check if is valid
        if (defaultEdgeType->isValid(SUMO_ATTR_NUMLANES, myNumLanes->getText().text())) {
            // reset color
            myNumLanes->setTextColor(FXRGB(0, 0, 0));
            myNumLanes->killFocus();
            // update numLanes in laneTypeParameters
            myLaneTypeParameters->updateNumLanes(GNEAttributeCarrier::parse<int>(myNumLanes->getText().text()));
        } else {
            myNumLanes->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == mySpeed) {
        // check if is valid
        if (defaultEdgeType->isValid(SUMO_ATTR_SPEED, mySpeed->getText().text())) {
            // set attribute (Without undoList)
            defaultEdgeType->setAttribute(SUMO_ATTR_SPEED, mySpeed->getText().text());
            // reset color
            mySpeed->setTextColor(FXRGB(0, 0, 0));
            mySpeed->killFocus();
        } else {
            mySpeed->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == myAllow) {
        // check if is valid
        if (defaultEdgeType->isValid(SUMO_ATTR_ALLOW, myAllow->getText().text())) {
            // set attribute (Without undoList)
            defaultEdgeType->setAttribute(SUMO_ATTR_ALLOW, myAllow->getText().text());
            // reset color
            myAllow->setTextColor(FXRGB(0, 0, 0));
            myAllow->killFocus();
            // update disallow textField
            myDisallow->setText(defaultEdgeType->getAttribute(SUMO_ATTR_DISALLOW).c_str(), FALSE);
        } else {
            myAllow->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == myDisallow) {
        // check if is valid
        if (defaultEdgeType->isValid(SUMO_ATTR_DISALLOW, myDisallow->getText().text())) {
            // set attribute (Without undoList)
            defaultEdgeType->setAttribute(SUMO_ATTR_DISALLOW, myDisallow->getText().text());
            // reset color
            myDisallow->setTextColor(FXRGB(0, 0, 0));
            myDisallow->killFocus();
            // update allow textField
            myAllow->setText(defaultEdgeType->getAttribute(SUMO_ATTR_ALLOW).c_str(), FALSE);
        } else {
            myDisallow->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == mySpreadType) {
        // check if is valid
        if (defaultEdgeType->isValid(SUMO_ATTR_SPREADTYPE, mySpreadType->getText().text())) {
            // set attribute (Without undoList)
            defaultEdgeType->setAttribute(SUMO_ATTR_SPREADTYPE, mySpreadType->getText().text());
            // reset color
            mySpreadType->setTextColor(FXRGB(0, 0, 0));
            mySpreadType->killFocus();
        } else {
            mySpreadType->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == myWidth) {
        // check if is valid
        if (defaultEdgeType->isValid(SUMO_ATTR_WIDTH, myWidth->getText().text())) {
            // set attribute (Without undoList)
            defaultEdgeType->setAttribute(SUMO_ATTR_WIDTH, myWidth->getText().text());
            // reset color
            myWidth->setTextColor(FXRGB(0, 0, 0));
            myWidth->killFocus();
        } else {
            myWidth->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == myPriority) {
        // check if is valid
        if (defaultEdgeType->isValid(SUMO_ATTR_PRIORITY, myPriority->getText().text())) {
            // set attribute (Without undoList)
            defaultEdgeType->setAttribute(SUMO_ATTR_PRIORITY, myPriority->getText().text());
            // reset color
            myPriority->setTextColor(FXRGB(0, 0, 0));
            myPriority->killFocus();
        } else {
            myPriority->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == myParameters) {
        // check if is valid
        if (defaultEdgeType->isValid(GNE_ATTR_PARAMETERS, myParameters->getText().text())) {
            // set attribute (Without undoList)
            defaultEdgeType->setAttribute(GNE_ATTR_PARAMETERS, myParameters->getText().text());
            // reset color
            myParameters->setTextColor(FXRGB(0, 0, 0));
            myParameters->killFocus();
        } else {
            myParameters->setTextColor(FXRGB(255, 0, 0));
        }
    }
}


void
GNECreateEdgeFrame::EdgeTypeParameters::setAttributeExistentEdgeType(FXObject* obj) {
    // get undoList
    GNEUndoList* undoList = myCreateEdgeFrameParent->myViewNet->getUndoList();
    // get selected edge type
    GNEEdgeType* edgeType = myCreateEdgeFrameParent->myEdgeTypeSelector->getEdgeTypeSelected();
    // check what attribute was changed
    if (obj == myID) {
        // check if is valid
        if (edgeType->isValid(SUMO_ATTR_ID, myID->getText().text())) {
            // set attribute
            edgeType->setAttribute(SUMO_ATTR_ID, myID->getText().text(), undoList);
            // reset color
            myID->setTextColor(FXRGB(0, 0, 0));
            myID->killFocus();
        } else {
            myID->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == myNumLanes) {
        // check if is valid
        if (edgeType->isValid(SUMO_ATTR_NUMLANES, myNumLanes->getText().text())) {
            // reset color
            myNumLanes->setTextColor(FXRGB(0, 0, 0));
            myNumLanes->killFocus();
            // update numLanes in laneTypeParameters
            myLaneTypeParameters->updateNumLanes(GNEAttributeCarrier::parse<int>(myNumLanes->getText().text()));
        } else {
            myNumLanes->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == mySpeed) {
        // check if is valid
        if (edgeType->isValid(SUMO_ATTR_SPEED, mySpeed->getText().text())) {
            // set attribute
            edgeType->setAttribute(SUMO_ATTR_SPEED, mySpeed->getText().text(), undoList);
            // reset color
            mySpeed->setTextColor(FXRGB(0, 0, 0));
            mySpeed->killFocus();
        } else {
            mySpeed->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == myAllow) {
        // check if is valid
        if (edgeType->isValid(SUMO_ATTR_ALLOW, myAllow->getText().text())) {
            // set attribute
            edgeType->setAttribute(SUMO_ATTR_ALLOW, myAllow->getText().text(), undoList);
            // reset color
            myAllow->setTextColor(FXRGB(0, 0, 0));
            myAllow->killFocus();
            // update disallow textField
            myDisallow->setText(edgeType->getAttribute(SUMO_ATTR_DISALLOW).c_str(), FALSE);
        } else {
            myAllow->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == myDisallow) {
        // check if is valid
        if (edgeType->isValid(SUMO_ATTR_DISALLOW, myDisallow->getText().text())) {
            // set attribute
            edgeType->setAttribute(SUMO_ATTR_DISALLOW, myDisallow->getText().text(), undoList);
            // reset color
            myDisallow->setTextColor(FXRGB(0, 0, 0));
            myDisallow->killFocus();
            // update allow textField
            myAllow->setText(edgeType->getAttribute(SUMO_ATTR_ALLOW).c_str(), FALSE);
        } else {
            myDisallow->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == mySpreadType) {
        // check if is valid
        if (edgeType->isValid(SUMO_ATTR_SPREADTYPE, mySpreadType->getText().text())) {
            // set attribute
            edgeType->setAttribute(SUMO_ATTR_SPREADTYPE, mySpreadType->getText().text(), undoList);
            // reset color
            mySpreadType->setTextColor(FXRGB(0, 0, 0));
            mySpreadType->killFocus();
        } else {
            mySpreadType->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == myWidth) {
        // check if is valid
        if (edgeType->isValid(SUMO_ATTR_WIDTH, myWidth->getText().text())) {
            // set attribute
            edgeType->setAttribute(SUMO_ATTR_WIDTH, myWidth->getText().text(), undoList);
            // reset color
            myWidth->setTextColor(FXRGB(0, 0, 0));
            myWidth->killFocus();
        } else {
            myWidth->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == myPriority) {
        // check if is valid
        if (edgeType->isValid(SUMO_ATTR_PRIORITY, myPriority->getText().text())) {
            // set attribute (Without undoList)
            edgeType->setAttribute(SUMO_ATTR_PRIORITY, myPriority->getText().text());
            // reset color
            myPriority->setTextColor(FXRGB(0, 0, 0));
            myPriority->killFocus();
        } else {
            myPriority->setTextColor(FXRGB(255, 0, 0));
        }
    } else if (obj == myParameters) {
        // check if is valid
        if (edgeType->isValid(GNE_ATTR_PARAMETERS, myParameters->getText().text())) {
            // set attribute
            edgeType->setAttribute(GNE_ATTR_PARAMETERS, myParameters->getText().text(), undoList);
            // reset color
            myParameters->setTextColor(FXRGB(0, 0, 0));
            myParameters->killFocus();
        } else {
            myParameters->setTextColor(FXRGB(255, 0, 0));
        }
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
    // create edge parameters
    myEdgeTypeParameters = new EdgeTypeParameters(this);
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

void
GNECreateEdgeFrame::setUseEdgeTemplate() {
    myEdgeTypeSelector->useTemplate();
}

/****************************************************************************/
