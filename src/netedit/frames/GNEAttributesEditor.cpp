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
/// @file    GNEAttributesEditor.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2024
///
// Row used for edit attributes
/****************************************************************************/
#include <config.h>

#include <netedit/dialogs/GNEMultipleParametersDialog.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/dialogs/GNEAllowVClassesDialog.h>
#include <netedit/dialogs/GNESingleParametersDialog.h>
#include <netedit/frames/common/GNEInspectorFrame.h>
#include <netedit/frames/demand/GNETypeFrame.h>
#include <utils/common/StringTokenizer.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/images/VClassIcons.h>
#include <utils/gui/images/POIIcons.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <netedit/elements/GNEAttributeCarrier.h>
#include <netedit/GNEViewNetHelper.h>
#include <utils/common/Parameterised.h>
#include <utils/foxtools/MFXToggleButtonTooltip.h>
#include <utils/foxtools/MFXTextFieldTooltip.h>
#include <utils/foxtools/MFXLabelTooltip.h>

#include "GNEAttributesEditor.h"
#include "GNEAttributesEditorRow.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

// temporal
#define MAX_ATTR 32

FXDEFMAP(GNEAttributesEditor) GNEAttributeTableMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITOR_FRONT,     GNEAttributesEditor::onCmdMarkAsFront),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITOR_DIALOG,    GNEAttributesEditor::onCmdOpenElementDialog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITOR_EXTENDED,  GNEAttributesEditor::onCmdOpenExtendedAttributesDialog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITOR_HELP,      GNEAttributesEditor::onCmdAttributesEditorHelp)
};

// Object implementation
FXIMPLEMENT(GNEAttributesEditor,  MFXGroupBoxModule,  GNEAttributeTableMap,   ARRAYNUMBER(GNEAttributeTableMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEAttributesEditor::GNEAttributesEditor(GNEFrame* frameParent, const std::string attributesEditorName, const int editorOptions) :
    MFXGroupBoxModule(frameParent, attributesEditorName.c_str()),
    myFrameParent(frameParent),
    myEditorOptions(editorOptions) {
    // create general buttons
    myFrontButton = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Front element"), "", "", GUIIconSubSys::getIcon(GUIIcon::FRONTELEMENT), this, MID_GNE_ATTRIBUTESEDITOR_FRONT, GUIDesignButton);
    myFrontButton->hide();
    myOpenDialogButton = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Open element dialog"), "", "", nullptr, this, MID_GNE_ATTRIBUTESEDITOR_DIALOG, GUIDesignButton);
    myOpenDialogButton->hide();
    myOpenExtendedAttributesButton = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Edit extended attributes"), "", "", nullptr, this, MID_GNE_ATTRIBUTESEDITOR_EXTENDED, GUIDesignButton);
    myOpenExtendedAttributesButton->hide();
    // resize myAttributesEditorRows and fill it with attribute rows
    myAttributesEditorRows.resize(MAX_ATTR);
    for (int i = 0; i < MAX_ATTR; i++) {
        myAttributesEditorRows[i] = new GNEAttributesEditorRow(this);
    }
    // Create help button
    myHelpButton = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Help"), "", "", nullptr, this, MID_GNE_ATTRIBUTESEDITOR_HELP, GUIDesignButtonRectangular);
}


GNEFrame*
GNEAttributesEditor::getFrameParent() const {
    return myFrameParent;
}


void
GNEAttributesEditor::showAttributesEditor(GNEAttributeCarrier* AC) {
    myEditedACs.clear();
    myEditedACs.push_back(AC);
    refreshAttributesEditor();
}


void
GNEAttributesEditor::showAttributesEditor(const std::unordered_set<GNEAttributeCarrier*>& ACs) {
    myEditedACs.clear();
    for (const auto& AC : ACs) {
        myEditedACs.push_back(AC);
    }
    refreshAttributesEditor();
}


void
GNEAttributesEditor::hideAttributesEditor() {
    myEditedACs.clear();
    // hide all rows before hidding table
    for (const auto& row : myAttributesEditorRows) {
        row->hideAttributeRow();
    }
    hide();
}


void
GNEAttributesEditor::refreshAttributesEditor() {
    if (myEditedACs.size() > 0) {
        const auto& tagProperty = myEditedACs.front()->getTagProperty();
        int itRows = 0;
        bool showButtons = false;
        // check if show netedit attributes (only for single edited ACs)
        if ((myEditorOptions & EditorOptions::NETEDIT_ATTRIBUTES) != 0) {
            // front button
            if (tagProperty.isDrawable()) {
                myFrontButton->show();
                showButtons = true;
            } else {
                myFrontButton->hide();
            }
            // specific for single edited attributes
            if (myEditedACs.size() == 1) {
                // edit dialog
                if (tagProperty.hasDialog()) {
                    // set text and icon
                    myOpenDialogButton->setText(TLF("Open % dialog", tagProperty.getTagStr()).c_str());
                    myOpenDialogButton->setIcon(GUIIconSubSys::getIcon(tagProperty.getGUIIcon()));
                    myOpenDialogButton->show();
                    showButtons = true;
                } else {
                    myOpenDialogButton->hide();
                }
                // extended attributes dialog
                if (tagProperty.hasExtendedAttributes()) {
                    myOpenExtendedAttributesButton->show();
                    showButtons = true;
                } else {
                    myOpenExtendedAttributesButton->hide();
                }
            }
        }
        // Iterate over tag property of first AC and show row for every attribute
        for (const auto& attrProperty : tagProperty) {
            bool showAttributeRow = true;
            // check show conditions
            if (attrProperty.isExtended()) {
                showAttributeRow = false;
            } else if (((myEditorOptions & EditorOptions::FLOW_ATTRIBUTES) == 0) && attrProperty.isFlow()) {
                showAttributeRow = false;
            } else if (((myEditorOptions & EditorOptions::FLOW_ATTRIBUTES) != 0) && !attrProperty.isFlow()) {
                showAttributeRow = false;
            } else if (((myEditorOptions & EditorOptions::GEO_ATTRIBUTES) == 0) && attrProperty.isGEO()) {
                showAttributeRow = false;
            } else if (((myEditorOptions & EditorOptions::GEO_ATTRIBUTES) != 0) && !attrProperty.isGEO()) {
                showAttributeRow = false;
            } else if (((myEditorOptions & EditorOptions::NETEDIT_ATTRIBUTES) == 0) && attrProperty.isNetedit()) {
                showAttributeRow = false;
            } else if (((myEditorOptions & EditorOptions::NETEDIT_ATTRIBUTES) != 0) && !attrProperty.isNetedit()) {
                showAttributeRow = false;
            } else if ((myEditorOptions & EditorOptions::BASIC_ATTRIBUTES) != 0) {
                showAttributeRow = true;
            }
            if (showAttributeRow && myAttributesEditorRows[itRows]->showAttributeRow(attrProperty)) {
                itRows++;
            }
        }
        // hide rest of rows before showing table
        for (int i = itRows; i < MAX_ATTR; i++) {
            myAttributesEditorRows[i]->hideAttributeRow();
        }
        // only show if at least one row or button was shown
        if ((itRows == 0) && !showButtons) {
            hideAttributesEditor();
        } else {
            // check if show help button
            if (itRows > 0) {
                myHelpButton->show();
            } else {
                myHelpButton->hide();
            }
            show();
        }
    } else {
        hideAttributesEditor();
    }
}


bool
GNEAttributesEditor::isSelectingParent() const {
    return false;
}


void
GNEAttributesEditor::setNewParent(const GNEAttributeCarrier* AC) {

}


void
GNEAttributesEditor::abortSelectingParent() const {

}


long
GNEAttributesEditor::onCmdMarkAsFront(FXObject*, FXSelector, void*) {
    // front all edited ACs
    for (auto& AC : myEditedACs) {
        AC->markForDrawingFront();
    }
    return 1;
}


long
GNEAttributesEditor::onCmdOpenElementDialog(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNEAttributesEditor::onCmdOpenExtendedAttributesDialog(FXObject*, FXSelector, void*) {
    // obtain edited AC (temporal), until unification of
    const auto demandElement = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(myEditedACs.front()->getTagProperty().getTag(), myEditedACs.front()->getID(), false);
    // open vehicle type dialog
    if (demandElement) {
        GNEVehicleTypeDialog(demandElement, true);  // NOSONAR, constructor returns after dialog has been closed
        refreshAttributesEditor();
    }
    return 1;
}


long
GNEAttributesEditor::onCmdAttributesEditorHelp(FXObject*, FXSelector, void*) {
    if (myEditedACs.size() > 0) {
        myFrameParent->openHelpAttributesDialog(myEditedACs.front());
    }
    return 1;
}


void
GNEAttributesEditor::setAttribute(SumoXMLAttr attr, const std::string& value) {
    const auto undoList = myFrameParent->getViewNet()->getUndoList();
    const auto& tagProperty = myEditedACs.front()->getTagProperty();
    // first check if we're editing a single attribute or an ID
    if (myEditedACs.size() > 1) {
        undoList->begin(tagProperty.getGUIIcon(), TLF("change multiple % attributes", tagProperty.getTagStr()));
    } else if (attr == SUMO_ATTR_ID) {
        // IDs attribute has to be encapsulated because implies multiple changes in different additionals (due references)
        undoList->begin(tagProperty.getGUIIcon(), TLF("change % attribute", tagProperty.getTagStr()));
    }
    // Set new value of attribute in all edited ACs
    for (const auto& editedAC : myEditedACs) {
        editedAC->setAttribute(attr, value, undoList);
    }
    // finish change multiple attributes or ID Attributes
    if ((myEditedACs.size() > 1) || (attr == SUMO_ATTR_ID)) {
        undoList->end();
    }
    refreshAttributesEditor();
    // update frame parent (needed to update other attribute tables)
    myFrameParent->attributeUpdated(attr);
}


void
GNEAttributesEditor::toggleEnableAttribute(SumoXMLAttr attr, const bool value) {
    const auto undoList = myFrameParent->getViewNet()->getUndoList();
    const auto& tagProperty = myEditedACs.front()->getTagProperty();
    // first check if we're editing a single attribute
    if (myEditedACs.size() > 1) {
        undoList->begin(tagProperty.getGUIIcon(), TLF("change multiple % attributes", tagProperty.getTagStr()));
    }
    // Set new value of attribute in all edited ACs
    for (const auto& editedAC : myEditedACs) {
        if (value) {
            editedAC->enableAttribute(attr, undoList);
        } else {
            editedAC->disableAttribute(attr, undoList);
        }
    }
    // finish change multiple attributes or ID Attributes
    if ((myEditedACs.size() > 1) || (attr == SUMO_ATTR_ID)) {
        undoList->end();
    }
    refreshAttributesEditor();
    // update frame parent (needed to update other attribute tables)
    myFrameParent->attributeUpdated(attr);
}


void
GNEAttributesEditor::inspectParent() {
    //myFrameParent->getViewNet()->getInspectedElements().inspectAC(
}


void
GNEAttributesEditor::moveLaneUp() {
    const auto lane = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveLane(myEditedACs.front()->getAttribute(SUMO_ATTR_LANE), false);
    if (lane) {
        // set next lane
        setAttribute(SUMO_ATTR_LANE, lane->getParentEdge()->getLanes().at(lane->getIndex() + 1)->getID());
    }
}


void
GNEAttributesEditor::moveLaneDown() {
    const auto lane = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveLane(myEditedACs.front()->getAttribute(SUMO_ATTR_LANE), false);
    if (lane) {
        // set previous lane
        setAttribute(SUMO_ATTR_LANE, lane->getParentEdge()->getLanes().at(lane->getIndex() - 1)->getID());
    }
}

/****************************************************************************/
