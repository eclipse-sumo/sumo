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
/// @file    GNEAttributesEditorRow.cpp
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
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,   GNEAttributesEditor::onCmdAttributeTableHelp)
};

// Object implementation
FXIMPLEMENT(GNEAttributesEditor,  MFXGroupBoxModule,  GNEAttributeTableMap,   ARRAYNUMBER(GNEAttributeTableMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEAttributesEditor::GNEAttributesEditor(GNEFrame* frameParent, const int editorOptions) :
    MFXGroupBoxModule(frameParent, TL("Internal attributes")),
    myFrameParent(frameParent),
    myEditorOptions(editorOptions) {
    // resize myAttributesEditorRows and fill it with attribute rows
    myAttributesEditorRows.resize(MAX_ATTR);
    for (int i = 0; i < MAX_ATTR; i++) {
        myAttributesEditorRows[i] = new GNEAttributesEditorRow(this);
    }
    // Create help button
    myHelpButton = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Help"), "", "", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
}


void
GNEAttributesEditor::showAttributeTableModule(GNEAttributeCarrier* AC) {
    myEditedACs.clear();
    myEditedACs.push_back(AC);
    refreshAttributeTable();
}


void
GNEAttributesEditor::showAttributeTableModule(const std::unordered_set<GNEAttributeCarrier*>& ACs) {
    myEditedACs.clear();
    for (const auto& AC : ACs) {
        myEditedACs.push_back(AC);
    }
    refreshAttributeTable();
}


void
GNEAttributesEditor::hideAttributeTableModule() {
    myEditedACs.clear();
    // hide all rows before hidding table
    for (const auto& row : myAttributesEditorRows) {
        row->hideAttributeRow();
    }
    hide();
}


void
GNEAttributesEditor::refreshAttributeTable() {
    if (myEditedACs.size() > 0) {
        // Iterate over tag property of first AC and show row for every attribute
        int itRows = 0;
        for (const auto& attrProperty : myEditedACs.front()->getTagProperty()) {
            // check if avoid show extended attributes
            if (((myEditorOptions & EditorOptions::EXTENDED_ATTRIBUTES) == 0) && attrProperty.isExtended()) {
                continue;
            }
            // check if force show flow attributes
            if (((myEditorOptions & EditorOptions::FLOW_ATTRIBUTES) == 0) && attrProperty.isFlow()) {
                continue;
            } else if (((myEditorOptions & EditorOptions::FLOW_ATTRIBUTES) != 0) && !attrProperty.isFlow()) {
                continue;
            }
            // check if force show GEO attributes
            if (((myEditorOptions & EditorOptions::GEO_ATTRIBUTES) == 0) && attrProperty.isGEO()) {
                continue;
            } else if (((myEditorOptions & EditorOptions::GEO_ATTRIBUTES) != 0) && !attrProperty.isGEO()) {
                continue;
            }
            myAttributesEditorRows[itRows]->showAttributeRow(attrProperty);
            itRows++;
        }
        // hide rest of rows before showing table
        for (int i = itRows; i < MAX_ATTR; i++) {
            myAttributesEditorRows[i]->hideAttributeRow();
        }
        // only show if at least one row was shown
        if (itRows == 0) {
            hideAttributeTableModule();
        } else {
            show();
        }
    } else {
        hideAttributeTableModule();
    }
}


GNEFrame*
GNEAttributesEditor::getFrameParent() const {
    return myFrameParent;
}


long
GNEAttributesEditor::onCmdAttributeTableHelp(FXObject*, FXSelector, void*) {
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
    refreshAttributeTable();
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
    refreshAttributeTable();
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
