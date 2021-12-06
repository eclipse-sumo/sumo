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
#include <netedit/elements/network/GNEEdgeTemplate.h>
#include <netedit/elements/network/GNELaneTemplate.h>
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
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_ADD,                GNECreateEdgeFrame::EdgeTypeSelector::onCmdAddEdgeType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_DELETE,             GNECreateEdgeFrame::EdgeTypeSelector::onCmdDeleteEdgeType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_SELECT,             GNECreateEdgeFrame::EdgeTypeSelector::onCmdSelectEdgeType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_CREATEFROMTEMPLATE, GNECreateEdgeFrame::EdgeTypeSelector::onCmdCreateFromTemplate),
};

FXDEFMAP(GNECreateEdgeFrame::LaneTypeSelector) LaneTypeSelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_ADD,    GNECreateEdgeFrame::LaneTypeSelector::onCmdAddLaneType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_DELETE, GNECreateEdgeFrame::LaneTypeSelector::onCmdDeleteLaneType),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_CREATEEDGEFRAME_SELECT, GNECreateEdgeFrame::LaneTypeSelector::onCmdSelectLaneType),
};

// Object implementation
FXIMPLEMENT(GNECreateEdgeFrame::EdgeTypeSelector,       FXGroupBoxModul,     EdgeTypeSelectorMap,    ARRAYNUMBER(EdgeTypeSelectorMap))
FXIMPLEMENT(GNECreateEdgeFrame::LaneTypeSelector,       FXGroupBoxModul,     LaneTypeSelectorMap,    ARRAYNUMBER(LaneTypeSelectorMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNECreateEdgeFrame::EdgeTypeSelector - methods
// ---------------------------------------------------------------------------

GNECreateEdgeFrame::EdgeTypeSelector::EdgeTypeSelector(GNECreateEdgeFrame* createEdgeFrameParent) :
    FXGroupBoxModul(createEdgeFrameParent->myContentFrame, "Template selector"),
    myCreateEdgeFrameParent(createEdgeFrameParent),
    myDefaultEdgeType(new GNEEdgeType(createEdgeFrameParent)),
    myCurrentIndex(0) {
    // default edge radio button
    myUseDefaultEdgeType = new FXRadioButton(this, "Create default edge", this, MID_GNE_CREATEEDGEFRAME_SELECTRADIOBUTTON, GUIDesignRadioButton);
    // use custom edge radio button
    myUseCustomEdgeType = new FXRadioButton(this, "Use edgeType/template", this, MID_GNE_CREATEEDGEFRAME_SELECTRADIOBUTTON, GUIDesignRadioButton);
    // edge types combo box
    myEdgeTypesComboBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_CREATEEDGEFRAME_SELECT, GUIDesignComboBoxAttribute);
    // create horizontal frame
    FXHorizontalFrame* horizontalFrameButtons = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // create new edge type button
    myAddEdgeTypeButton = new FXButton(horizontalFrameButtons, "Add\t\tAdd edge type", GUIIconSubSys::getIcon(GUIIcon::ADD), 
                                       this, MID_GNE_CREATEEDGEFRAME_ADD, GUIDesignButton);
    // create delete edge type button
    myDeleteEdgeTypeButton = new FXButton(horizontalFrameButtons, "Delete\t\tDelete edge type", GUIIconSubSys::getIcon(GUIIcon::REMOVE), 
                                          this, MID_GNE_CREATEEDGEFRAME_DELETE, GUIDesignButton);

    // create delete edge type button
    myCreateFromTemplate = new FXButton(this, "Create from template\t\tCreate edgeType from template", GUIIconSubSys::getIcon(GUIIcon::EDGE), 
                                          this, MID_GNE_CREATEEDGEFRAME_CREATEFROMTEMPLATE, GUIDesignButton);
    // by default, create custom edge
    myUseDefaultEdgeType->setCheck(TRUE);
}


GNECreateEdgeFrame::EdgeTypeSelector::~EdgeTypeSelector() {
    delete myDefaultEdgeType;
}


void
GNECreateEdgeFrame::EdgeTypeSelector::refreshEdgeTypeSelector() {
    // get template editor
    const GNEInspectorFrame::TemplateEditor* templateEditor = myCreateEdgeFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->getTemplateEditor();
    // get current item
    const int index = myEdgeTypesComboBox->getCurrentItem();
    // fill combo box
    fillComboBox();
    // set default edgeType
    if (myUseDefaultEdgeType->getCheck()) {
        // disable create from template
        myCreateFromTemplate->disable();
        // disable comboBox
        myEdgeTypesComboBox->disable();
        // disable buttons
        myAddEdgeTypeButton->disable();
        myDeleteEdgeTypeButton->disable();
        // show default edgeType attributes
        myCreateEdgeFrameParent->myEdgeTypeAttributes->showAttributesCreatorModul(myDefaultEdgeType, {SUMO_ATTR_ID});
        // show lane attributes
        myCreateEdgeFrameParent->myLaneTypeSelector->showLaneTypeSelector();
    } else if (myUseCustomEdgeType->getCheck()) {
        // enable buttons
        myAddEdgeTypeButton->enable();
        myDeleteEdgeTypeButton->enable();
        // check conditions
        if (myEdgeTypesComboBox->getNumItems() == 0) {
            // disable comboBox and edgeType
            myEdgeTypesComboBox->disable();
            myDeleteEdgeTypeButton->disable();
            // hide attributes creators
            myCreateEdgeFrameParent->myEdgeTypeAttributes->hideAttributesCreatorModul();
            myCreateEdgeFrameParent->myLaneTypeSelector->hideLaneTypeSelector();
        } else if (templateEditor->getEdgeTemplate() && (index == 0)) {
            // enable create from template
            myCreateFromTemplate->enable();
            // enable comboBox
            myEdgeTypesComboBox->enable();
            // disable delete edge type button (because templates cannot be deleted)
            myDeleteEdgeTypeButton->disable();
            // show edgeType attributes and disable
            myCreateEdgeFrameParent->myEdgeTypeAttributes->showAttributesCreatorModul(templateEditor->getEdgeTemplate(), {SUMO_ATTR_ID});
            myCreateEdgeFrameParent->myEdgeTypeAttributes->disableAttributesCreator();
            // show lane attributes (will be automatic disabled)
            myCreateEdgeFrameParent->myLaneTypeSelector->showLaneTypeSelector();  
        } else {
            // disable create from template
            myCreateFromTemplate->disable();
            // enable comboBox
            myEdgeTypesComboBox->enable();
            // get edgeType
            myEdgeTypeSelected = myCreateEdgeFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveEdgeType(myEdgeTypesComboBox->getText().text(), false);
            // check if exist
            if (myEdgeTypeSelected) {
                // show edgeType attributes
                myCreateEdgeFrameParent->myEdgeTypeAttributes->showAttributesCreatorModul(myEdgeTypeSelected, {});
                // show lane attributes
                myCreateEdgeFrameParent->myLaneTypeSelector->showLaneTypeSelector();
            } else {
                // hide edgeType attributes
                myCreateEdgeFrameParent->myEdgeTypeAttributes->hideAttributesCreatorModul();
                // hide lane attributes
                myCreateEdgeFrameParent->myLaneTypeSelector->hideLaneTypeSelector();
                // set comboBox text
                myEdgeTypesComboBox->setTextColor(FXRGB(255, 0, 0));
            }
        }
    }
    // recalc
    recalc();
}


void
GNECreateEdgeFrame::EdgeTypeSelector::updateIDinComboBox(const std::string &oldID, const std::string &newID) {
    for (int i = 0; i < myEdgeTypesComboBox->getNumItems(); i++) {
        if (myEdgeTypesComboBox->getItem(i).text() == oldID) {
            myEdgeTypesComboBox->setItemText(i, newID.c_str());
        }
    }
}


bool
GNECreateEdgeFrame::EdgeTypeSelector::useEdgeTemplate() const {
    if (myCreateEdgeFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->getTemplateEditor()->getEdgeTemplate()) {
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
    myCurrentIndex--;
    if (myCurrentIndex < 0) {
        myCurrentIndex = 0;
    }
}


void
GNECreateEdgeFrame::EdgeTypeSelector::setCurrentEdgeType(const GNEEdgeType* edgeType) {
    for (int i = 0; i < myEdgeTypesComboBox->getNumItems(); i++) {
        if (myEdgeTypesComboBox->getItem(i).text() == edgeType->getID()) {
            // set current item and index
            myEdgeTypesComboBox->setCurrentItem(i);
            myCurrentIndex = i;
            // set buttons
            myUseDefaultEdgeType->setCheck(FALSE);
            myUseCustomEdgeType->setCheck(TRUE);
            // refresh
            refreshEdgeTypeSelector();
        }
    }
}


void
GNECreateEdgeFrame::EdgeTypeSelector::useTemplate() {
    myUseDefaultEdgeType->setCheck(FALSE);
    myUseCustomEdgeType->setCheck(TRUE);
    refreshEdgeTypeSelector();
}


long
GNECreateEdgeFrame::EdgeTypeSelector::onCmdRadioButton(FXObject* obj, FXSelector, void*) {
    // check what object was pressed
    if (obj == myUseDefaultEdgeType) {
        // update radio buttons
        myUseDefaultEdgeType->setCheck(TRUE);
        myUseCustomEdgeType->setCheck(FALSE);
    } else {
        // update radio buttons
        myUseDefaultEdgeType->setCheck(FALSE);
        myUseCustomEdgeType->setCheck(TRUE);
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
    // select last item
    myCurrentIndex = (myEdgeTypesComboBox->getNumItems() - 1);
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
    myCreateEdgeFrameParent->getViewNet()->getUndoList()->begin(GUIIcon::EDGE, "delete edge type");
    myCreateEdgeFrameParent->getViewNet()->getUndoList()->add(new GNEChange_EdgeType(edgeType, false), true);
    myCreateEdgeFrameParent->getViewNet()->getUndoList()->end();
    // refresh EdgeTypeSelector
    refreshEdgeTypeSelector();
    return 0;
}


long
GNECreateEdgeFrame::EdgeTypeSelector::onCmdSelectEdgeType(FXObject*, FXSelector, void*) {
    // update current index
    myCurrentIndex = myEdgeTypesComboBox->getCurrentItem();
    // refresh edgeType selector
    refreshEdgeTypeSelector();
    return 0;
}


long 
GNECreateEdgeFrame::EdgeTypeSelector::onCmdCreateFromTemplate(FXObject*, FXSelector, void*) {
    if (myCreateEdgeFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->getTemplateEditor()->getEdgeTemplate()) {
        // create new edge type
        GNEEdgeType* edgeType = new GNEEdgeType(myCreateEdgeFrameParent->getViewNet()->getNet());
        // copy all template values
        edgeType->copyTemplate(myCreateEdgeFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->getTemplateEditor()->getEdgeTemplate());
        // add it using undoList
        myCreateEdgeFrameParent->getViewNet()->getUndoList()->begin(GUIIcon::EDGE, "create new edge type");
        myCreateEdgeFrameParent->getViewNet()->getUndoList()->add(new GNEChange_EdgeType(edgeType, true), true);
        myCreateEdgeFrameParent->getViewNet()->getUndoList()->end();
        // update myEdgeTypeSelected
        myEdgeTypeSelected = edgeType;
        // select last item
        myCurrentIndex = (myEdgeTypesComboBox->getNumItems() - 1);
        // refresh EdgeTypeSelector
        refreshEdgeTypeSelector();
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
    // set comboBox text color
    myEdgeTypesComboBox->setTextColor(FXRGB(0, 0, 0));
    // add template
    if (templateEditor->getEdgeTemplate()) {
        myEdgeTypesComboBox->appendItem(("template: " + templateEditor->getEdgeTemplate()->getID()).c_str(), nullptr);
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
    // set current item
    if ((myCurrentIndex < 0) || (myCurrentIndex >= myEdgeTypesComboBox->getNumItems())) {
        myCurrentIndex = myEdgeTypesComboBox->getNumItems() - 1;
    }
    myEdgeTypesComboBox->setCurrentItem(myCurrentIndex);
}

// ---------------------------------------------------------------------------
// GNECreateEdgeFrame::LaneTypeSelector - methods
// ---------------------------------------------------------------------------

GNECreateEdgeFrame::LaneTypeSelector::LaneTypeSelector(GNECreateEdgeFrame* createEdgeFrameParent) :
    FXGroupBoxModul(createEdgeFrameParent->myContentFrame, "LaneType selector"),
    myCreateEdgeFrameParent(createEdgeFrameParent),
    myLaneIndex(0) {
    // lane types combo box
    myLaneTypesComboBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_CREATEEDGEFRAME_SELECT, GUIDesignComboBoxAttribute);
    // create horizontal frame
    FXHorizontalFrame* horizontalFrameButtons = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    // create new lane type button
    myAddLaneTypeButton = new FXButton(horizontalFrameButtons, "Add\t\tAdd lane type", GUIIconSubSys::getIcon(GUIIcon::ADD), 
                                       this, MID_GNE_CREATEEDGEFRAME_ADD, GUIDesignButton);
    // create delete lane type button
    myDeleteLaneTypeButton = new FXButton(horizontalFrameButtons, "Delete\t\tDelete lane type", GUIIconSubSys::getIcon(GUIIcon::REMOVE), 
                                          this, MID_GNE_CREATEEDGEFRAME_DELETE, GUIDesignButton);
}


GNECreateEdgeFrame::LaneTypeSelector::~LaneTypeSelector() {
}


void
GNECreateEdgeFrame::LaneTypeSelector::showLaneTypeSelector() {
    // start at first lane
    myLaneIndex = 0;
    // refresh laneTypeSelector
    refreshLaneTypeSelector();
    // show
    show();
}


void
GNECreateEdgeFrame::LaneTypeSelector::hideLaneTypeSelector() {
    // hide attributes creator modul
    myCreateEdgeFrameParent->myLaneTypeAttributes->hideAttributesCreatorModul();
    // hide
    hide();
}


void
GNECreateEdgeFrame::LaneTypeSelector::refreshLaneTypeSelector() {
    // clear lane types
    myLaneTypesComboBox->clearItems();
    // first check if use template 
    if (myCreateEdgeFrameParent->myEdgeTypeSelector->useEdgeTemplate()) {
        const GNEEdgeTemplate *edgeTemplate = myCreateEdgeFrameParent->getViewNet()->getViewParent()->getInspectorFrame()->getTemplateEditor()->getEdgeTemplate();
        // fill comboBox
        for (const auto& laneTemplate : edgeTemplate->getLaneTemplates()) {
            myLaneTypesComboBox->appendItem(laneTemplate->getAttribute(SUMO_ATTR_ID).c_str(), nullptr);
        }
        // update comboBox
        updateComboBox();
        // show laneTypeAttributes
        myCreateEdgeFrameParent->myLaneTypeAttributes->showAttributesCreatorModul(edgeTemplate->getLaneTemplates().at(myLaneIndex), {SUMO_ATTR_ID});
        // disable laneAttributes (because is a template)
        myCreateEdgeFrameParent->myLaneTypeAttributes->disableAttributesCreator();
        // disable add and remove buttons
        myAddLaneTypeButton->disable();
        myDeleteLaneTypeButton->disable();
    } else {
        // get edgeType
        const GNEEdgeType* edgeType = myCreateEdgeFrameParent->myEdgeTypeSelector->getEdgeTypeSelected();
        // fill comboBox
        for (const auto& laneType : edgeType->getLaneTypes()) {
            myLaneTypesComboBox->appendItem(laneType->getAttribute(SUMO_ATTR_ID).c_str(), nullptr);
        }
        // update comboBox
        updateComboBox();
        // show laneTypeAttributes
        myCreateEdgeFrameParent->myLaneTypeAttributes->showAttributesCreatorModul(edgeType->getLaneTypes().at(myLaneIndex), {});
        // enable add and remove buttons
        myAddLaneTypeButton->enable();
        // check if enable or disable remove lane button
        if (edgeType->getLaneTypes().size() > 1) {
            myDeleteLaneTypeButton->enable();
        } else {
            myDeleteLaneTypeButton->disable();
        }
    }
    // recalc
    recalc();
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
            // create laneTypes
            for (const auto &laneType : edgeType->getLaneTypes()) {
                newEdgeType->addLaneType(new GNELaneType(newEdgeType, laneType));
            }
            // add new lane
            newEdgeType->addLaneType(new GNELaneType(newEdgeType));
            // remove old edgeTyp und and newEdgeType
            myCreateEdgeFrameParent->getViewNet()->getUndoList()->begin(GUIIcon::LANE, "add laneType");
            myCreateEdgeFrameParent->getViewNet()->getUndoList()->add(new GNEChange_EdgeType(edgeType, false), true);
            myCreateEdgeFrameParent->getViewNet()->getUndoList()->add(new GNEChange_EdgeType(newEdgeType, true), true);
            myCreateEdgeFrameParent->getViewNet()->getUndoList()->end();
            // update index
            myLaneIndex = myLaneTypesComboBox->getNumItems() - 1;
            // set current edgeType in selector
            myCreateEdgeFrameParent->myEdgeTypeSelector->setCurrentEdgeType(newEdgeType);
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
GNECreateEdgeFrame::LaneTypeSelector::updateComboBox() {
    // check lane index
    if (myLaneIndex > myLaneTypesComboBox->getNumItems()) {
        myLaneIndex = 0;
    }
    // set current item
    myLaneTypesComboBox->setCurrentItem(myLaneIndex);
    // set num visible items
    if (myLaneTypesComboBox->getNumItems() <= 10) {
        myLaneTypesComboBox->setNumVisible(myLaneTypesComboBox->getNumItems());
    } else {
        myLaneTypesComboBox->setNumVisible(10);
    }
}

// ---------------------------------------------------------------------------
// GNECreateEdgeFrame::Legend - methods
// ---------------------------------------------------------------------------

GNECreateEdgeFrame::Legend::Legend(GNECreateEdgeFrame* createEdgeFrameParent) :
    FXGroupBoxModul(createEdgeFrameParent->myContentFrame, "Information") {
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
    new FXLabel(this, information.str().c_str(), 0, GUIDesignLabelFrameInformation);
}


GNECreateEdgeFrame::Legend::~Legend() {}

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
    myLegend = new Legend(this);
}


GNECreateEdgeFrame::~GNECreateEdgeFrame() {}


void
GNECreateEdgeFrame::processClick(const Position& clickedPosition, const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor,
                                 const bool oppositeEdge, const bool chainEdge) {
    // first check if there is an edge template, an edge type (default or custom)
    if (!myEdgeTypeSelector->useDefaultEdgeType() && !myEdgeTypeSelector->useEdgeTemplate() && (myEdgeTypeSelector->getEdgeTypeSelected() == nullptr)) {
        WRITE_WARNING("Select either default edgeType or a custom edgeType or template");
    } else {
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
    myEdgeTypeSelector->refreshEdgeTypeSelector();
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


GNECreateEdgeFrame::LaneTypeSelector* 
GNECreateEdgeFrame::getLaneTypeSelector() {
    return myLaneTypeSelector;
}


GNEFrameAttributesModuls::AttributesCreator* 
GNECreateEdgeFrame::getLaneTypeAttributes() const {
    return myLaneTypeAttributes;
}


void
GNECreateEdgeFrame::setUseEdgeTemplate() {
    myEdgeTypeSelector->useTemplate();
    myEdgeTypeSelector->refreshEdgeTypeSelector();
}

/****************************************************************************/
