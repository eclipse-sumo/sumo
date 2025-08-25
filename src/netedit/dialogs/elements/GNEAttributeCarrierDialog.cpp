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
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,      GNEAttributeCarrierDialog::AttributeTextField::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_BOOL, GNEAttributeCarrierDialog::AttributeTextField::onCmdSetBoolAttribute),
};

// Object implementation
FXIMPLEMENT(GNEAttributeCarrierDialog::AttributeTextField, FXHorizontalFrame, AttributeTextFieldMap, ARRAYNUMBER(AttributeTextFieldMap))

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEAttributeCarrierDialog::AttributeTextField - methods
// ---------------------------------------------------------------------------

GNEAttributeCarrierDialog::AttributeTextField::AttributeTextField(GNEAttributeCarrierDialog* ACDialog, FXVerticalFrame* verticalFrame,
        const GNEAttributeProperties* attrProperty) :
    FXHorizontalFrame(verticalFrame, GUIDesignAuxiliarHorizontalFrame),
    myACDialog(ACDialog),
    myAttrProperty(attrProperty) {
    // create label
    new FXLabel(this, attrProperty->getAttrStr().c_str(), nullptr, GUIDesignLabelThickedFixed(100));
    // continue depending of attr type
    if (attrProperty->isBool()) {
        // create lef boolean checkBox for enable/disable attributes
        myCheckButton = new FXCheckButton(this, "bool", this, MID_GNE_SET_ATTRIBUTE_BOOL, GUIDesignCheckButtonAttribute);
        // continue depending of current value
        if (ACDialog->getElement()->getAttribute(attrProperty->getAttr()) == GNEAttributeCarrier::TRUE_STR) {
            myCheckButton->setCheck(TRUE);
            myCheckButton->setText(TL("true"));
        } else {
            myCheckButton->setCheck(FALSE);
            myCheckButton->setText(TL("false"));
        }
    } else {
        // create text field
        myTextField = new MFXTextFieldTooltip(this, ACDialog->getElement()->getNet()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(),
                                              GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
        // set attribute
        myTextField->setText(ACDialog->getElement()->getAttribute(attrProperty->getAttr()).c_str());
    }
}


long
GNEAttributeCarrierDialog::AttributeTextField::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    if (myACDialog->getElement()->isValid(myAttrProperty->getAttr(), myTextField->getText().text())) {
        // set attribute
        myACDialog->getElement()->setAttribute(myAttrProperty->getAttr(), myTextField->getText().text(), myACDialog->getElement()->getNet()->getViewNet()->getUndoList());
        // set valid color and kill focus
        myTextField->setTextColor(GUIDesignTextColorBlack);
        myTextField->setBackColor(GUIDesignBackgroundColorWhite);
        myTextField->killFocus();
    } else {
        // set invalid color
        myTextField->setTextColor(GUIDesignTextColorRed);
        // set background color
        if (myTextField->getText().empty()) {
            myTextField->setTextColor(GUIDesignBackgroundColorRed);
        } else {
            myTextField->setBackColor(GUIDesignBackgroundColorWhite);
        }
    }
    return 1;
}


long
GNEAttributeCarrierDialog::AttributeTextField::onCmdSetBoolAttribute(FXObject*, FXSelector, void*) {
    if (myCheckButton->getCheck() == TRUE) {
        myACDialog->getElement()->setAttribute(myAttrProperty->getAttr(), GNEAttributeCarrier::TRUE_STR, myACDialog->getElement()->getNet()->getViewNet()->getUndoList());
        myCheckButton->setText(TL("true"));
    } else {
        myACDialog->getElement()->setAttribute(myAttrProperty->getAttr(), GNEAttributeCarrier::FALSE_STR, myACDialog->getElement()->getNet()->getViewNet()->getUndoList());
        myCheckButton->setText(TL("false"));
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEAttributeCarrierDialog - methods
// ---------------------------------------------------------------------------

GNEAttributeCarrierDialog::GNEAttributeCarrierDialog(GNEAttributeCarrier* AC) :
    GNETemplateElementDialog<GNEAttributeCarrier>(AC) {
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


GNEAttributeCarrierDialog::~GNEAttributeCarrierDialog() {}


void
GNEAttributeCarrierDialog::runInternalTest(const InternalTestStep::DialogArgument* /*dialogArgument*/) {
    // nothing to do
}


long
GNEAttributeCarrierDialog::onCmdAccept(FXObject*, FXSelector, void*) {
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
GNEAttributeCarrierDialog::onCmdReset(FXObject*, FXSelector, void*) {
    // reset changes
    resetChanges();
    return 1;
}

/****************************************************************************/
