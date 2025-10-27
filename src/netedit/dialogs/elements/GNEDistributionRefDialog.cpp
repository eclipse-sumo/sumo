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
/// @file    GNEDistributionRefDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// Dialog for edit attribute carriers
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/dialogs/basic/GNEWarningBasicDialog.h>
#include <netedit/dialogs/GNEColorDialog.h>
#include <netedit/dialogs/GNEVClassesDialog.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <utils/foxtools/MFXTextFieldIcon.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEDistributionRefDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEDistributionRefDialog) AttributeTextFieldMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITORROW_SETATTRIBUTE,       GNEDistributionRefDialog::AttributeTextField::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITORROW_OPENDIALOG_COLOR,   GNEDistributionRefDialog::AttributeTextField::onCmdOpenColorDialog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ATTRIBUTESEDITORROW_OPENDIALOG_ALLOW,   GNEDistributionRefDialog::AttributeTextField::onCmdOpenVClassDialog),
};

// Object implementation
FXIMPLEMENT(GNEDistributionRefDialog, FXHorizontalFrame, AttributeTextFieldMap, ARRAYNUMBER(AttributeTextFieldMap))

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDistributionRefDialog::GNEDistributionRefDialog(GNEAttributeCarrier* AC) :
    GNETemplateElementDialog<GNEAttributeCarrier>(AC, DialogType::ATTRIBUTECARRIER) {
    // Create auxiliar frames for rows
    FXHorizontalFrame* columns = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);
    FXVerticalFrame* columnLeft = new FXVerticalFrame(columns, GUIDesignAuxiliarFrameFixedWidth(250));
    FXVerticalFrame* columnRight = new FXVerticalFrame(columns, GUIDesignAuxiliarFrameFixedWidth(250));
    // calculate number of attributes
    std::vector<const GNEAttributeProperties*> attrProperties;
    for (const auto& attrProperty : myElement->getTagProperty()->getAttributeProperties()) {
        // check if this attribute can be edited in edit mode and in basic editor
        if (attrProperty->isEditMode() && attrProperty->isBasicEditor()) {
            attrProperties.push_back(attrProperty);
        }
    }
    // spread attributes in two columns
    for (size_t i = 0; i < attrProperties.size(); i++) {
        // create attribute text field
        auto attributeTextField = new AttributeTextField(this, (i % 2 == 0) ? columnLeft : columnRight, attrProperties[i]);
        // add to myAttributeTextFields vector
        myAttributeTextFields.push_back(attributeTextField);
    }
    // init commandGroup
    myElement->getNet()->getViewNet()->getUndoList()->begin(myElement, TLF("edit % '%'", AC->getTagStr(), AC->getID()));
    // open dialog
    openDialog();
}


GNEDistributionRefDialog::~GNEDistributionRefDialog() {}


void
GNEDistributionRefDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // nothing to do
}


long
GNEDistributionRefDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    if (false) {
        // open warning Box
        GNEWarningBasicDialog(myElement->getNet()->getViewNet()->getViewParent()->getGNEAppWindows(),
                              TLF("Error editing % '%'", myElement->getTagStr(), myElement->getID()),
                              TLF("The % '%' cannot be updated because there are invalid attributes.",
                                  myElement->getTagStr(), myElement->getID()));
        return 1;
    } else {
        // close dialog accepting changes
        return acceptElementDialog();
    }
}


long
GNEDistributionRefDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // reset changes
    resetChanges();
    return 1;
}

/****************************************************************************/
