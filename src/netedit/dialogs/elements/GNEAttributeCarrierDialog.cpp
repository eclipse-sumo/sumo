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
/// @file    GNEAttributeCarrierDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// Dialog for edit attribute carriers
/****************************************************************************/

#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/dialogs/basic/GNEWarningBasicDialog.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNENet.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <utils/foxtools/MFXTextFieldTooltip.h>
#include <utils/gui/div/GUIDesigns.h>

#include "GNEAttributeCarrierDialog.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEAttributeCarrierDialog::AttributeTextField) AttributeTextFieldMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEAttributeCarrierDialog::AttributeTextField::onCmdSetAttribute),
};

// Object implementation
FXIMPLEMENT(GNEAttributeCarrierDialog::AttributeTextField, FXHorizontalFrame, AttributeTextFieldMap, ARRAYNUMBER(AttributeTextFieldMap))

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEAttributeCarrierDialog::AttributeTextField - methods
// ---------------------------------------------------------------------------

GNEAttributeCarrierDialog::AttributeTextField::AttributeTextField(GNEAttributeCarrierDialog* ACDialog, FXVerticalFrame* verticalFrame, SumoXMLAttr attr) :
    FXHorizontalFrame(verticalFrame, GUIDesignAuxiliarHorizontalFrame),
    myACDialog(ACDialog),
    myAttr(attr) {
    // create label
    new FXLabel(this, toString(attr).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
    // create text field
    myTextField = new MFXTextFieldTooltip(this, ACDialog->getElement()->getNet()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(),
                                          GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
}


long
GNEAttributeCarrierDialog::AttributeTextField::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    return 1;
}

// ---------------------------------------------------------------------------
// GNEAttributeCarrierDialog - methods
// ---------------------------------------------------------------------------

GNEAttributeCarrierDialog::GNEAttributeCarrierDialog(GNEAttributeCarrier* AC) :
    GNETemplateElementDialog<GNEAttributeCarrier>(AC) {
    // Create auxiliar frames for rows
    FXHorizontalFrame* columns = new FXHorizontalFrame(myContentFrame, GUIDesignUniformHorizontalFrame);
    FXVerticalFrame* columnLeft = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    FXVerticalFrame* columnRight = new FXVerticalFrame(columns, GUIDesignAuxiliarFrame);
    // calculate number of attributes
    std::vector<SumoXMLAttr> attrs;
    for (const auto& attrProperty : myElement->getTagProperty()->getAttributeProperties()) {
        // check if this attribute can be edited in edit mode
        if (attrProperty->isEditMode()) {
            attrs.push_back(attrProperty->getAttr());
        }
    }
    // spread attributes in two columns
    for (size_t i = 0; i < attrs.size(); i++) {
        // create attribute text field
        auto attributeTextField = new AttributeTextField(this, (i % 2 == 0) ? columnLeft : columnRight, attrs[i]);
        // add to myAttributeTextFields vector
        myAttributeTextFields.push_back(attributeTextField);
    }
    // open dialog
    openDialog();
}


GNEAttributeCarrierDialog::~GNEAttributeCarrierDialog() {}


void
GNEAttributeCarrierDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // nothing to do
}


long
GNEAttributeCarrierDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    if (!false) {
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
GNEAttributeCarrierDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // reset changes
    resetChanges();
    return 1;
}

/****************************************************************************/
