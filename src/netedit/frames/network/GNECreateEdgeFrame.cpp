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
#include <netedit/elements/network/GNEEdgeType.h>
#include <netedit/elements/network/GNELaneType.h>
#include <netedit/dialogs/GNEAllowDisallow.h>
#include <netimport/NITypeLoader.h>
#include <netimport/NIXMLTypesHandler.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>


#include "GNECreateEdgeFrame.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNECreateEdgeFrame::EdgeSelector) EdgeSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_SELECTRADIOBUTTON,  GNECreateEdgeFrame::EdgeSelector::onCmdRadioButton),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_ADDEDGETYPE,        GNECreateEdgeFrame::EdgeSelector::onCmdAddEdgeType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_DELETEEDGETYPE,     GNECreateEdgeFrame::EdgeSelector::onCmdDeleteEdgeType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_SELECTEDGETYPE,     GNECreateEdgeFrame::EdgeSelector::onCmdSelectEdgeType),
};

FXDEFMAP(GNECreateEdgeFrame::EdgeTypeParameters) EdgeTypeParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNECreateEdgeFrame::EdgeTypeParameters::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNECreateEdgeFrame::EdgeTypeParameters::onCmdOpenAttributeDialog),
};

FXDEFMAP(GNECreateEdgeFrame::LaneTypeParameters) LaneTypeParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNECreateEdgeFrame::LaneTypeParameters::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNECreateEdgeFrame::LaneTypeParameters::onCmdOpenAttributeDialog),
};

// Object implementation
FXIMPLEMENT(GNECreateEdgeFrame::EdgeSelector,           FXGroupBox,     EdgeSelectorMap,        ARRAYNUMBER(EdgeSelectorMap))
FXIMPLEMENT(GNECreateEdgeFrame::EdgeTypeParameters,     FXGroupBox,     EdgeTypeParametersMap,  ARRAYNUMBER(EdgeTypeParametersMap))
FXIMPLEMENT(GNECreateEdgeFrame::LaneTypeParameters,     FXGroupBox,     LaneTypeParametersMap,  ARRAYNUMBER(LaneTypeParametersMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNECreateEdgeFrame::EdgeSelector - methods
// ---------------------------------------------------------------------------

GNECreateEdgeFrame::EdgeSelector::EdgeSelector(GNECreateEdgeFrame* createEdgeFrameParent) :
    FXGroupBox(createEdgeFrameParent->myContentFrame, "Template selector", GUIDesignGroupBoxFrame),
    myCreateEdgeFrameParent(createEdgeFrameParent),
    mySelectedEdgeType(nullptr),
    myDefaultEdgeType(new GNEEdgeType(createEdgeFrameParent->getViewNet()->getNet())) {
    // default edge radio button
    myCreateDefaultEdge = new FXRadioButton(this, 
        "Create default edge", this, MID_GNE_CREATEEDGEFRAME_SELECTRADIOBUTTON, GUIDesignRadioButton);
    // use custom edge radio button
    myUseCustomEdge = new FXRadioButton(this, 
        "Use edgeType/template", this, MID_GNE_CREATEEDGEFRAME_SELECTRADIOBUTTON, GUIDesignRadioButton);
    // edge types combo box
    myEdgeTypesComboBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_CREATEEDGEFRAME_SELECTEDGETYPE, GUIDesignComboBoxAttribute);
    // create horizontal frame
    FXHorizontalFrame* horizontalFrameNewSaveDelete = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // create new edge type button
    myNewEdgeTypeButton = new FXButton(horizontalFrameNewSaveDelete,
        "add\t\add edge type", GUIIconSubSys::getIcon(GUIIcon::ADD), this, MID_GNE_CREATEEDGEFRAME_ADDEDGETYPE, GUIDesignButton);
    // create delete edge type button
    myDeleteEdgeTypeButton = new FXButton(horizontalFrameNewSaveDelete,
        "delete\t\tdelete edge type", GUIIconSubSys::getIcon(GUIIcon::REMOVE), this, MID_GNE_CREATEEDGEFRAME_DELETEEDGETYPE, GUIDesignButton);
    // by default, create custom edge
    myCreateDefaultEdge->setCheck(TRUE);
}


GNECreateEdgeFrame::EdgeSelector::~EdgeSelector() {
    delete myDefaultEdgeType;
}


void
GNECreateEdgeFrame::EdgeSelector::refreshEdgeSelector() {
    // get template editor
    const GNEInspectorFrame::TemplateEditor* templateEditor = myCreateEdgeFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->getTemplateEditor();
    // get edge types
    const auto& edgeTypes = myCreateEdgeFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getEdgeTypes();
    // check if there is template
    if (templateEditor->hasTemplate() || (edgeTypes.size() > 0)) {
        // enable both buttons
        myCreateDefaultEdge->enable();
        myUseCustomEdge->enable();
        // enable combo box
        myEdgeTypesComboBox->enable();
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
        // myEdgeTypesComboBox 
        if (mySelectedEdgeType) {
            // declare index
            int index = 0;
            // search index of current selectedEdgeType
            for (int i = 0; i < myEdgeTypesComboBox->getNumItems(); i++) {
                if (mySelectedEdgeType->getID() == myEdgeTypesComboBox->getItem(i).text()) {
                    index = i;
                }
            }
            // set current item
            myEdgeTypesComboBox->setCurrentItem(index);
            // show parameter fields
            myCreateEdgeFrameParent->myEdgeTypeParameters->showEdgeTypeParameters();
            myCreateEdgeFrameParent->myLaneTypeParameters->showLaneTypeParameters();
            // enable parameter fields
            myCreateEdgeFrameParent->myEdgeTypeParameters->enableEdgeTypeParameters();
            myCreateEdgeFrameParent->myLaneTypeParameters->enableLaneTypeParameters();
            // update edge parameters
            myCreateEdgeFrameParent->myEdgeTypeParameters->setEdgeType(mySelectedEdgeType);
        } else if (templateEditor->hasTemplate()) {
            // set current item
            myEdgeTypesComboBox->setCurrentItem(0);
            // show parameter fields
            myCreateEdgeFrameParent->myEdgeTypeParameters->showEdgeTypeParameters();
            myCreateEdgeFrameParent->myLaneTypeParameters->showLaneTypeParameters();
            // disable parameter fields (because templates cannot be edited)
            myCreateEdgeFrameParent->myEdgeTypeParameters->enableEdgeTypeParameters();
            myCreateEdgeFrameParent->myLaneTypeParameters->enableLaneTypeParameters();
            // update edge parameters (using template
            myCreateEdgeFrameParent->myEdgeTypeParameters->setTemplateValues();
        } else if (edgeTypes.size() > 0) {
            // set current item
            myEdgeTypesComboBox->setCurrentItem(0);
            // set mySelectedEdgeType
            mySelectedEdgeType = edgeTypes.begin()->second;
            // show parameter fields
            myCreateEdgeFrameParent->myEdgeTypeParameters->showEdgeTypeParameters();
            myCreateEdgeFrameParent->myLaneTypeParameters->showLaneTypeParameters();
            // enable parameter fields
            myCreateEdgeFrameParent->myEdgeTypeParameters->enableEdgeTypeParameters();
            myCreateEdgeFrameParent->myLaneTypeParameters->enableLaneTypeParameters();
            // update edge parameters
            myCreateEdgeFrameParent->myEdgeTypeParameters->setEdgeType(mySelectedEdgeType);
        } else {
            // disable comboBox (nothing to select)
            myEdgeTypesComboBox->disable();
            // hide parameter fields
            myCreateEdgeFrameParent->myEdgeTypeParameters->hideEdgeTypeParameters();
            myCreateEdgeFrameParent->myLaneTypeParameters->hideLaneTypeParameters();
        }
    } else {
        // disable use custom edge
        myCreateDefaultEdge->enable();
        myUseCustomEdge->disable();
        // disable combo box
        myEdgeTypesComboBox->disable();
        // enable custom edge radio button
        myCreateDefaultEdge->setCheck(TRUE, FALSE);
    }
    // show editor parameter
    if (myUseCustomEdge->getCheck() == TRUE) {
        // enable combobox
        myEdgeTypesComboBox->enable();
        // enable buttons
        myNewEdgeTypeButton->enable();
        myDeleteEdgeTypeButton->enable();
    } else {
        // disable comboBox
        myEdgeTypesComboBox->disable();
        // disable buttons
        myNewEdgeTypeButton->disable();
        myDeleteEdgeTypeButton->disable();
        // set default parameters
        myCreateEdgeFrameParent->myEdgeTypeParameters->setEdgeType(myDefaultEdgeType);
    }
    // recalc
    recalc();
}


bool
GNECreateEdgeFrame::EdgeSelector::useEdgeTemplate() const {
    if (myCreateEdgeFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->getTemplateEditor()->hasTemplate()) {
        if ((myUseCustomEdge->getCheck() == TRUE) && (myEdgeTypesComboBox->getCurrentItem() == 0)){
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}


bool
GNECreateEdgeFrame::EdgeSelector::useDefaultEdge() const {
    return (myCreateDefaultEdge->getCheck() == TRUE);
}


GNEEdgeType* 
GNECreateEdgeFrame::EdgeSelector::getDefaultEdgeType() const {
    return myDefaultEdgeType;
}


GNEEdgeType* 
GNECreateEdgeFrame::EdgeSelector::getSelectedEdgeType() const {
    return mySelectedEdgeType;
}


void 
GNECreateEdgeFrame::EdgeSelector::clearSelectedEdgeType() {
    mySelectedEdgeType = nullptr;
}


long
GNECreateEdgeFrame::EdgeSelector::onCmdRadioButton(FXObject* obj, FXSelector, void*) {
    // check what object was pressed
    if (obj == myCreateDefaultEdge) {
        // update buttons
        myCreateDefaultEdge->setCheck(TRUE, FALSE);
        myUseCustomEdge->setCheck(FALSE, FALSE);
    } else {
        // update buttons
        myCreateDefaultEdge->setCheck(FALSE, FALSE);
        myUseCustomEdge->setCheck(TRUE, FALSE);
    }
    // refresh template selector
    refreshEdgeSelector();
    return 0;
}


long
GNECreateEdgeFrame::EdgeSelector::onCmdAddEdgeType(FXObject*, FXSelector, void*) {
    // create new edge type
    GNEEdgeType* edgeType = new GNEEdgeType(myCreateEdgeFrameParent->getViewNet()->getNet());
    // also create a new laneType
    GNELaneType* laneType = new GNELaneType(edgeType);
    // add it using undoList
    myCreateEdgeFrameParent->getViewNet()->getUndoList()->p_begin("create new edge type");
    myCreateEdgeFrameParent->getViewNet()->getUndoList()->add(new GNEChange_EdgeType(edgeType, true), true);
    myCreateEdgeFrameParent->getViewNet()->getUndoList()->add(new GNEChange_LaneType(laneType, true), true);
    myCreateEdgeFrameParent->getViewNet()->getUndoList()->p_end();
    // update mySelectedEdgeType
    mySelectedEdgeType = edgeType;
    // refresh edgeSelector
    refreshEdgeSelector();
    return 0;
}


long
GNECreateEdgeFrame::EdgeSelector::onCmdDeleteEdgeType(FXObject*, FXSelector, void*) {
    // first check number of elements
    if (myEdgeTypesComboBox->getNumItems() > 1) {
        // first check if we have to reset mySelectedEdgeType
        if (mySelectedEdgeType && (mySelectedEdgeType->getID() == myEdgeTypesComboBox->getText().text())) {
            mySelectedEdgeType = nullptr;
        }
        // get edgeType to remove
        GNEEdgeType* edgeType = myCreateEdgeFrameParent->getViewNet()->getNet()->retrieveEdgeType(myEdgeTypesComboBox->getText().text());
        // remove it using undoList
        myCreateEdgeFrameParent->getViewNet()->getUndoList()->p_begin("create new edge type");
        // iterate over all laneType
        for (const auto &laneType : edgeType->getLaneTypes()) {
            myCreateEdgeFrameParent->getViewNet()->getUndoList()->add(new GNEChange_LaneType(laneType, false), true);
        }
        myCreateEdgeFrameParent->getViewNet()->getUndoList()->add(new GNEChange_EdgeType(edgeType, false), true);
        myCreateEdgeFrameParent->getViewNet()->getUndoList()->p_end();
    }
    return 0;
}


long 
GNECreateEdgeFrame::EdgeSelector::onCmdSelectEdgeType(FXObject*, FXSelector, void*) {
    // get template editor
    const GNEInspectorFrame::TemplateEditor* templateEditor = myCreateEdgeFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->getTemplateEditor();
    // get edge types
    const auto& edgeTypes = myCreateEdgeFrameParent->getViewNet()->getNet()->getAttributeCarriers()->getEdgeTypes();
    // reset mySelectedEdgeType
    mySelectedEdgeType = nullptr;
    // check if we selected template
    if (templateEditor->hasTemplate() && myEdgeTypesComboBox->getCurrentItem() == 0) {
        // set valid color
        myEdgeTypesComboBox->setTextColor(FXRGB(0, 0, 0));
        myEdgeTypesComboBox->killFocus();
        // show parameter fields
        myCreateEdgeFrameParent->myEdgeTypeParameters->showEdgeTypeParameters();
        myCreateEdgeFrameParent->myLaneTypeParameters->showLaneTypeParameters();
        // disable parameter fields (because templantes cannot be edited)
        myCreateEdgeFrameParent->myEdgeTypeParameters->disableEdgeTypeParameters();
        myCreateEdgeFrameParent->myLaneTypeParameters->disableLaneTypeParameters();
        // set edge template in myEdgeTypeParameters
        myCreateEdgeFrameParent->myEdgeTypeParameters->setTemplateValues();
    } else if (edgeTypes.count(myEdgeTypesComboBox->getText().text()) > 0) {
        // set valid color
        myEdgeTypesComboBox->setTextColor(FXRGB(0, 0, 0));
        myEdgeTypesComboBox->killFocus();
        // show parameter fields
        myCreateEdgeFrameParent->myEdgeTypeParameters->showEdgeTypeParameters();
        myCreateEdgeFrameParent->myLaneTypeParameters->showLaneTypeParameters();
        // enable parameter fields
        myCreateEdgeFrameParent->myEdgeTypeParameters->enableEdgeTypeParameters();
        myCreateEdgeFrameParent->myLaneTypeParameters->enableLaneTypeParameters();
        // set mySelectedEdgeType
        mySelectedEdgeType = myCreateEdgeFrameParent->myViewNet->getNet()->retrieveEdgeType(myEdgeTypesComboBox->getText().text());
        // set edgeType in myEdgeTypeParameters
        myCreateEdgeFrameParent->myEdgeTypeParameters->setEdgeType(mySelectedEdgeType);
    } else {
        // set invalid color
        myEdgeTypesComboBox->setTextColor(FXRGB(255, 0, 0));
        // hide parameter fields
        myCreateEdgeFrameParent->myEdgeTypeParameters->hideEdgeTypeParameters();
        myCreateEdgeFrameParent->myLaneTypeParameters->hideLaneTypeParameters();
    }
    return 0;
}

void
GNECreateEdgeFrame::EdgeSelector::fillDefaultParameters() {
    // set numLanes
    myDefaultEdgeType->setAttribute(SUMO_ATTR_NUMLANES, "1");
    // set speed
    myDefaultEdgeType->setAttribute(SUMO_ATTR_SPEED, "13.89");
    // set allow
    myDefaultEdgeType->setAttribute(SUMO_ATTR_ALLOW, "all");
    // set disallow
    myDefaultEdgeType->setAttribute(SUMO_ATTR_DISALLOW, "");
    // set width
    myDefaultEdgeType->setAttribute(SUMO_ATTR_WIDTH, "-1.00");
    // set parameters
    myDefaultEdgeType->setAttribute(GNE_ATTR_PARAMETERS, "");
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
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myLabelID = new FXLabel(horizontalFrameAttribute, "edgeType id", nullptr, GUIDesignLabelAttribute);
    myID = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
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
    // create textField for width
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(horizontalFrameAttribute, toString(SUMO_ATTR_WIDTH).c_str(), nullptr, GUIDesignLabelAttribute);
    myWidth = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create textField for parameters
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    new FXLabel(horizontalFrameAttribute, "parameters", nullptr, GUIDesignLabelAttribute);
    myParameters = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
}


GNECreateEdgeFrame::EdgeTypeParameters::~EdgeTypeParameters() {}


void
GNECreateEdgeFrame::EdgeTypeParameters::showEdgeTypeParameters() {
    show();
}


void
GNECreateEdgeFrame::EdgeTypeParameters::hideEdgeTypeParameters() {
    hide();
}


void
GNECreateEdgeFrame::EdgeTypeParameters::enableEdgeTypeParameters() {
    myID->enable();
    myNumLanes->enable();
    mySpeed->enable();
    myAllowButton->enable();
    myAllow->enable();
    myDisallowButton->enable();
    myDisallow->enable();
    myWidth->enable();
    myParameters->enable();
}


void
GNECreateEdgeFrame::EdgeTypeParameters::disableEdgeTypeParameters() {
    myID->disable();
    myNumLanes->disable();
    mySpeed->disable();
    myAllowButton->disable();
    myAllow->disable();
    myDisallowButton->disable();
    myDisallow->disable();
    myWidth->disable();
    myParameters->disable();
}


void 
GNECreateEdgeFrame::EdgeTypeParameters::setEdgeType(GNEEdgeType* edgeType) {
    // set id
    myLabelID->setText("edgeType ID");
    myID->setText(edgeType->getAttribute(SUMO_ATTR_ID).c_str(), FALSE);
    myID->setTextColor(FXRGB(0, 0, 0));
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
    // set width
    myWidth->setText(edgeType->getAttribute(SUMO_ATTR_WIDTH).c_str(), FALSE);
    myWidth->setTextColor(FXRGB(0, 0, 0));
    // set parameters
    myParameters->setText(edgeType->getAttribute(GNE_ATTR_PARAMETERS).c_str(), FALSE);
    myParameters->setTextColor(FXRGB(0, 0, 0));
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
        // set numLanes
        myNumLanes->setText(templateEditor->getEdgeTemplate().edgeParameters.at(SUMO_ATTR_NUMLANES).c_str(), FALSE);
        // set speed
        mySpeed->setText(templateEditor->getEdgeTemplate().edgeParameters.at(SUMO_ATTR_SPEED).c_str(), FALSE);
        // set allow
        myAllow->setText(templateEditor->getEdgeTemplate().edgeParameters.at(SUMO_ATTR_ALLOW).c_str(), FALSE);
        // set disallow
        myDisallow->setText(templateEditor->getEdgeTemplate().edgeParameters.at(SUMO_ATTR_DISALLOW).c_str(), FALSE);
        // set width
        myWidth->setText(templateEditor->getEdgeTemplate().edgeParameters.at(SUMO_ATTR_WIDTH).c_str(), FALSE);
        // set parameters
        myParameters->setText(templateEditor->getEdgeTemplate().edgeParameters.at(GNE_ATTR_PARAMETERS).c_str(), FALSE);
    } else {
        throw ProcessError("no template");
    }
}


long
GNECreateEdgeFrame::EdgeTypeParameters::onCmdSetAttribute(FXObject* obj, FXSelector, void*) {
    // check if we're editing an existent edgeType
    if (myCreateEdgeFrameParent->myEdgeSelector->useDefaultEdge()) {
        setAttributeDefaultParameters(obj);
    } else {
        setAttributeExistentEdgeType(obj);
    }
    return 1;
}


long
GNECreateEdgeFrame::EdgeTypeParameters::onCmdOpenAttributeDialog(FXObject*, FXSelector, void*) {
    // declare strings
    std::string allow = myAllow->getText().text();
    std::string disallow = myDisallow->getText().text();
    // open dialog
    GNEAllowDisallow(myCreateEdgeFrameParent->getViewNet(), &allow, &disallow).execute();
    // update allow/disallow
    myAllow->setText(allow.c_str(), FALSE);
    myDisallow->setText(disallow.c_str(), FALSE);
    return 1;
}


void 
GNECreateEdgeFrame::EdgeTypeParameters::setAttributeDefaultParameters(FXObject* obj) {
    // get default edge type
    GNEEdgeType *defaultEdgeType = myCreateEdgeFrameParent->myEdgeSelector->getDefaultEdgeType();
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
            // set attribute (Without undoList)
            defaultEdgeType->setAttribute(SUMO_ATTR_NUMLANES, myNumLanes->getText().text());
            // reset color
            myNumLanes->setTextColor(FXRGB(0, 0, 0));
            myNumLanes->killFocus();
            // update numLanes in laneTypeParameters
            myCreateEdgeFrameParent->myLaneTypeParameters->updateNumLanes(GNEAttributeCarrier::parse<int>(myNumLanes->getText().text()));
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
    GNEUndoList *undoList = myCreateEdgeFrameParent->myViewNet->getUndoList();
    // get selected edge type
    GNEEdgeType *edgeType = myCreateEdgeFrameParent->myEdgeSelector->getSelectedEdgeType();
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
            // set attribute
            edgeType->setAttribute(SUMO_ATTR_NUMLANES, myNumLanes->getText().text(), undoList);
            // reset color
            myNumLanes->setTextColor(FXRGB(0, 0, 0));
            myNumLanes->killFocus();
            // update numLanes in laneTypeParameters
            myCreateEdgeFrameParent->myLaneTypeParameters->updateNumLanes(GNEAttributeCarrier::parse<int>(myNumLanes->getText().text()));
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
// GNECreateEdgeFrame::LaneTypeParameters - methods
// ---------------------------------------------------------------------------

GNECreateEdgeFrame::LaneTypeParameters::LaneTypeParameters(GNECreateEdgeFrame* createEdgeFrameParent) :
    FXGroupBox(createEdgeFrameParent->myContentFrame, "Lane parameters", GUIDesignGroupBoxFrame),
    myCreateEdgeFrameParent(createEdgeFrameParent) {
    // declare horizontalFrameAttribute
    FXHorizontalFrame* horizontalFrameAttribute = nullptr;
    // create ComboBox for spread type
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame),
    new FXLabel(horizontalFrameAttribute, "Lane index", nullptr, GUIDesignLabelAttribute);
    myLaneIndex = new FXComboBox(horizontalFrameAttribute, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBoxAttribute);
    // create textField for speed
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame),
    new FXLabel(horizontalFrameAttribute, toString(SUMO_ATTR_SPEED).c_str(), nullptr, GUIDesignLabelAttribute);
    mySpeed = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create Button for allow vehicles
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame),
    myAllowButton = new FXButton(horizontalFrameAttribute, toString(SUMO_ATTR_ALLOW).c_str(), nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myAllow = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create Button for disallow vehicles
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame),
    myDisallowButton = new FXButton(horizontalFrameAttribute, toString(SUMO_ATTR_DISALLOW).c_str(), nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myDisallow = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // create textField for width
    horizontalFrameAttribute = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame),
    new FXLabel(horizontalFrameAttribute, toString(SUMO_ATTR_WIDTH).c_str(), nullptr, GUIDesignLabelAttribute);
    myWidth = new FXTextField(horizontalFrameAttribute, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    // fill default parameters
    fillDefaultParameters(0);
}


GNECreateEdgeFrame::LaneTypeParameters::~LaneTypeParameters() {}


void
GNECreateEdgeFrame::LaneTypeParameters::showLaneTypeParameters() {
    show();
}


void 
GNECreateEdgeFrame::LaneTypeParameters::hideLaneTypeParameters(){
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
GNECreateEdgeFrame::LaneTypeParameters::setDefaultValues() {
    // set default lane
    myLaneIndex->setText("1");
    // set default speed
    mySpeed->setText("13.89");
    // set default allow
    myAllow->setText("all");
    // set default disallow
    myDisallow->setText("");
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
    // First remove extra lanes
    while (myLaneIndex->getNumItems() > numLanes) {
        myLaneIndex->removeItem(myLaneIndex->getNumItems() - 1);
    }
    // now add lanes
    while (myLaneIndex->getNumItems() < numLanes) {
        myLaneIndex->insertItem(myLaneIndex->getNumItems(), toString(myLaneIndex->getNumItems()).c_str());
    }
}

long
GNECreateEdgeFrame::LaneTypeParameters::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNECreateEdgeFrame::LaneTypeParameters::onCmdOpenAttributeDialog(FXObject*, FXSelector, void*) {
    // declare strings
    std::string allow = myAllow->getText().text();
    std::string disallow = myDisallow->getText().text();
    // open dialog
    GNEAllowDisallow(myCreateEdgeFrameParent->getViewNet(), &allow, &disallow).execute();
    // update allow/disallow
    myAllow->setText(allow.c_str(), FALSE);
    myDisallow->setText(disallow.c_str(), FALSE);
    return 1;
}


void
GNECreateEdgeFrame::LaneTypeParameters::fillDefaultParameters(int /* laneIndex */) {
    // set speed
    mySpeed->setText("13.89");
    // set allow
    myAllow->setText("all");
    // set disallow
    myDisallow->setText("");
    /*
    const OptionsCont& oc = OptionsCont::getOptions();
    double defaultSpeed = oc.getFloat("default.speed");
    std::string defaultType = oc.getString("default.type");
    int defaultNrLanes = oc.getInt("default.lanenumber");
    int defaultPriority = oc.getInt("default.priority");
    double defaultWidth = NBEdge::UNSPECIFIED_WIDTH;
    double defaultOffset = NBEdge::UNSPECIFIED_OFFSET;
    */
}

// ---------------------------------------------------------------------------
// GNECreateEdgeFrame::Legend - methods
// ---------------------------------------------------------------------------

GNECreateEdgeFrame::EdgeSelectorLegend::EdgeSelectorLegend(GNECreateEdgeFrame* createEdgeFrameParent) :
    FXGroupBox(createEdgeFrameParent->myContentFrame, "Legend", GUIDesignGroupBoxFrame) {
    // crate information
    std::ostringstream information;
    // add label for shift+click
    information
            << "- Control+Click:" << "\n"
            << "  Move view" << "\n"
            << "- Shift+Click:" << "\n"
            << "  Splits edge in both directions" << "\n"
            << "- Alt+Shift+Click:" << "\n"
            << "  Splits edge in one direction";
    // create label
    new FXLabel(this, information.str().c_str(), 0, GUIDesignLabelFrameThicked);
}


GNECreateEdgeFrame::EdgeSelectorLegend::~EdgeSelectorLegend() {}

// ---------------------------------------------------------------------------
// GNECreateEdgeFrame - methods
// ---------------------------------------------------------------------------

GNECreateEdgeFrame::GNECreateEdgeFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet) :
    GNEFrame(horizontalFrameParent, viewNet, "Create Edge"),
    myObjectsUnderSnappedCursor(viewNet),
    myCreateEdgeSource(nullptr) {
    // create custom edge selector
    myEdgeSelector = new EdgeSelector(this);
    // create edge parameters
    myEdgeTypeParameters = new EdgeTypeParameters(this);
    // create lane parameters
    myLaneTypeParameters = new LaneTypeParameters(this);
    // create edge selector legend
    myEdgeSelectorLegend = new EdgeSelectorLegend(this);
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
        myViewNet->getUndoList()->p_begin("create new " + toString(SUMO_TAG_EDGE));
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
                if (myEdgeSelector->useEdgeTemplate()) {
                    newEdge->copyTemplate(myViewNet->getViewParent()->getInspectorFrame()->getTemplateEditor()->getEdgeTemplate(), myViewNet->getUndoList());
                } else if (myEdgeSelector->useDefaultEdge()) { 
                    newEdge->copyEdgeType(myEdgeSelector->getDefaultEdgeType(), myViewNet->getUndoList());
                } else {
                    newEdge->copyEdgeType(myEdgeSelector->getSelectedEdgeType(), myViewNet->getUndoList());
                }
                // create another edge, if create opposite edge is enabled
                if (oppositeEdge) {
                    GNEEdge* newOppositeEdge = myViewNet->getNet()->createEdge(junction, myCreateEdgeSource, nullptr,
                                               myViewNet->getUndoList(), "-" + newEdge->getNBEdge()->getID());
                    // set parameters
                    if (myEdgeSelector->useEdgeTemplate()) {
                        newOppositeEdge->copyTemplate(myViewNet->getViewParent()->getInspectorFrame()->getTemplateEditor()->getEdgeTemplate(), myViewNet->getUndoList());
                    } else if (myEdgeSelector->useDefaultEdge()) { 
                        newOppositeEdge->copyEdgeType(myEdgeSelector->getDefaultEdgeType(), myViewNet->getUndoList());
                    } else {
                        newOppositeEdge->copyEdgeType(myEdgeSelector->getSelectedEdgeType(), myViewNet->getUndoList());
                    }
                }
                // edge created, then unmark as create edge source
                myCreateEdgeSource->unMarkAsCreateEdgeSource();
                // end undo list
                if (myViewNet->getUndoList()->hasCommandGroup()) {
                    myViewNet->getUndoList()->p_end();
                } else {
                    std::cout << "edge created without an open CommandGroup )-:\n";
                }
                // if we're creating edges in chain mode, mark junction as junction edge source
                if (chainEdge) {
                    myCreateEdgeSource = junction;
                    myCreateEdgeSource->markAsCreateEdgeSource();
                    myViewNet->getUndoList()->p_begin("create new " + toString(SUMO_TAG_EDGE));
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
    myEdgeSelector->refreshEdgeSelector();
    // show frame
    GNEFrame::show();
}


void
GNECreateEdgeFrame::hide() {
    // hide frame
    GNEFrame::hide();
}


GNECreateEdgeFrame::EdgeSelector*
GNECreateEdgeFrame::getEdgeSelector() const {
    return myEdgeSelector;
}

/****************************************************************************/
