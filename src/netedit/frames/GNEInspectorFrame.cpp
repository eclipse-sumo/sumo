/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEInspectorFrame.cpp
/// @author  Jakob Erdmann
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2011
/// @version $Id$
///
// The Widget for modifying network-element attributes (i.e. lane speed)
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <iostream>
#include <cassert>
#include <cmath>
#include <regex>
#include <utils/common/MsgHandler.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/foxtools/fxexdefs.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <netedit/additionals/GNEAdditional.h>
#include <netedit/GNEAttributeCarrier.h>
#include <netedit/netelements/GNEConnection.h>
#include <netedit/netelements/GNECrossing.h>
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNEJunction.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/GNENet.h>
#include <netedit/additionals/GNEPOI.h>
#include <netedit/additionals/GNEPoly.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/dialogs/GNEDialog_AllowDisallow.h>

#include "GNEInspectorFrame.h"
#include "GNEDeleteFrame.h"
#include "GNEAdditionalFrame.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEInspectorFrame) GNEInspectorFrameMap[] = {
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_INSPECTORFRAME_GOBACK,          GNEInspectorFrame::onCmdGoBack),

};

FXDEFMAP(GNEInspectorFrame::AttributesEditor::AttributeInput) AttributeInputMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNEInspectorFrame::AttributesEditor::AttributeInput::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNEInspectorFrame::AttributesEditor::AttributeInput::onCmdOpenAttributeDialog)
};

FXDEFMAP(GNEInspectorFrame::AttributesEditor) AttributesEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,   GNEInspectorFrame::AttributesEditor::onCmdAttributeHelp),
};

FXDEFMAP(GNEInspectorFrame::NeteditAttributesEditor) NeteditAttributesEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEInspectorFrame::NeteditAttributesEditor::onCmdSetNeteditAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,               GNEInspectorFrame::NeteditAttributesEditor::onCmdNeteditAttributeHelp),
};

FXDEFMAP(GNEInspectorFrame::GEOAttributesEditor) GEOAttributesEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,  GNEInspectorFrame::GEOAttributesEditor::onCmdSetGEOAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,               GNEInspectorFrame::GEOAttributesEditor::onCmdGEOAttributeHelp),
};

FXDEFMAP(GNEInspectorFrame::TemplateEditor) TemplateEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_INSPECTORFRAME_COPYTEMPLATE,    GNEInspectorFrame::TemplateEditor::onCmdCopyTemplate),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_INSPECTORFRAME_SETTEMPLATE,     GNEInspectorFrame::TemplateEditor::onCmdSetTemplate),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_INSPECTORFRAME_COPYTEMPLATE,    GNEInspectorFrame::TemplateEditor::onUpdCopyTemplate),
};

// Object implementation
FXIMPLEMENT(GNEInspectorFrame,                                      FXVerticalFrame,    GNEInspectorFrameMap,       ARRAYNUMBER(GNEInspectorFrameMap))
FXIMPLEMENT(GNEInspectorFrame::AttributesEditor::AttributeInput,    FXHorizontalFrame,  AttributeInputMap,          ARRAYNUMBER(AttributeInputMap))
FXIMPLEMENT(GNEInspectorFrame::AttributesEditor,                    FXGroupBox,         AttributesEditorMap,        ARRAYNUMBER(AttributesEditorMap))
FXIMPLEMENT(GNEInspectorFrame::NeteditAttributesEditor,             FXGroupBox,         NeteditAttributesEditorMap, ARRAYNUMBER(NeteditAttributesEditorMap))
FXIMPLEMENT(GNEInspectorFrame::GEOAttributesEditor,                 FXGroupBox,         GEOAttributesEditorMap,     ARRAYNUMBER(GEOAttributesEditorMap))
FXIMPLEMENT(GNEInspectorFrame::TemplateEditor,                      FXGroupBox,         TemplateEditorMap,          ARRAYNUMBER(TemplateEditorMap))


// ===========================================================================
// method definitions
// ===========================================================================

GNEInspectorFrame::GNEInspectorFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet):
    GNEFrame(horizontalFrameParent, viewNet, "Inspector"),
    myPreviousElementInspect(nullptr),
    myPreviousElementDelete(nullptr) {

    // Create back button
    myBackButton = new FXButton(myHeaderLeftFrame, "", GUIIconSubSys::getIcon(ICON_NETEDITARROW), this, MID_GNE_INSPECTORFRAME_GOBACK, GUIDesignButtonIconRectangular);
    myHeaderLeftFrame->hide();
    myBackButton->hide();

    // Create Attributes Editor
    myAttributesEditor = new AttributesEditor(this);

    // Create GEO Parameters Editor
    myGEOAttributesEditor = new GEOAttributesEditor(this);

    // create Generic parameters editor
    myGenericParametersEditor = new GenericParametersEditor(this);

    // Create Netedit Attributes Editor
    myNeteditAttributesEditor = new NeteditAttributesEditor(this);

    // Create Template editor
    myTemplateEditor = new TemplateEditor(this);

    // Create groupbox and tree list
    myACHierarchy = new GNEFrame::ACHierarchy(this);
}


GNEInspectorFrame::~GNEInspectorFrame() {}


void
GNEInspectorFrame::show() {
    inspectElement(nullptr);
    GNEFrame::show();
}


void
GNEInspectorFrame::hide() {
    myInspectedACs.clear();
    myViewNet->setACUnderCursor(nullptr);
    GNEFrame::hide();
}


void
GNEInspectorFrame::inspectElement(GNEAttributeCarrier* AC) {
    // Use the implementation of inspect for multiple AttributeCarriers to avoid repetition of code
    std::vector<GNEAttributeCarrier*> itemsToInspect;
    if (AC != nullptr) {
        myViewNet->setACUnderCursor(AC);
        if (AC->isAttributeCarrierSelected()) {
            std::vector<GNEAttributeCarrier*> selectedACs = myViewNet->getNet()->getSelectedAttributeCarriers();
            for (auto i : selectedACs) {
                if (i->getTag() == AC->getTag()) {
                    itemsToInspect.push_back(i);
                }
            }
        } else {
            itemsToInspect.push_back(AC);
        }
    }
    inspectMultisection(itemsToInspect);
}


void
GNEInspectorFrame::inspectMultisection(const std::vector<GNEAttributeCarrier*>& ACs) {
    // hide back button
    myHeaderLeftFrame->hide();
    myBackButton->hide();
    // Assing ACs to myACs
    myInspectedACs = ACs;
    // Hide all elements
    myAttributesEditor->hideAttributesEditor();
    myNeteditAttributesEditor->hideNeteditAttributesEditor();
    myGEOAttributesEditor->hideGEOAttributesEditor();
    myGenericParametersEditor->hideGenericParametersEditor();
    myTemplateEditor->hideTemplateEditor();
    myACHierarchy->hideACHierarchy();
    // If vector of attribute Carriers contain data
    if (myInspectedACs.size() > 0) {
        // Set header
        std::string headerString;
        if (dynamic_cast<GNENetElement*>(myInspectedACs.front())) {
            headerString = "Net: ";
        } else if (dynamic_cast<GNEAdditional*>(myInspectedACs.front())) {
            headerString = "Additional: ";
        } else if (dynamic_cast<GNEShape*>(myInspectedACs.front())) {
            headerString = "Shape: ";
        }
        if (myInspectedACs.size() > 1) {
            headerString += toString(myInspectedACs.size()) + " ";
        }
        headerString += toString(myInspectedACs.front()->getTag());
        if (myInspectedACs.size() > 1) {
            headerString += "s";
        }
        // Set headerString into header label
        getFrameHeaderLabel()->setText(headerString.c_str());

        // Show attributes editor
        myAttributesEditor->showAttributeEditor();

        // show netedit attributes editor if  we're inspecting elements with Netedit Attributes
        myNeteditAttributesEditor->showNeteditAttributesEditor();

        // Show GEO Attributes Editor if we're inspecting elements with GEO Attributes
        myGEOAttributesEditor->showGEOAttributesEditor();

        // show generic attributes editor
        if (myInspectedACs.size() == 1) {
            myGenericParametersEditor->showGenericParametersEditor(myInspectedACs.front());
        } else {
            myGenericParametersEditor->showGenericParametersEditor(myInspectedACs);
        }

        // If attributes correspond to an Edge, show template editor
        myTemplateEditor->showTemplateEditor();

        // if we inspect a single Attribute carrier vector, show their childs
        if (myInspectedACs.size() == 1) {
            myACHierarchy->showACHierarchy(myInspectedACs.front());
        }
    } else {
        getFrameHeaderLabel()->setText("Inspect");
        myContentFrame->recalc();
    }
}


void
GNEInspectorFrame::inspectChild(GNEAttributeCarrier* AC, GNEAttributeCarrier* previousElement) {
    // Show back button if myPreviousElementInspect was defined
    myPreviousElementInspect = previousElement;
    if (myPreviousElementInspect != nullptr) {
        // disable myPreviousElementDelete to avoid inconsistences
        myPreviousElementDelete = nullptr;
        inspectElement(AC);
        myHeaderLeftFrame->show();
        myBackButton->show();
    }
}


void
GNEInspectorFrame::inspectFromDeleteFrame(GNEAttributeCarrier* AC, GNEAttributeCarrier* previousElement, bool previousElementWasMarked) {
    myPreviousElementDelete = previousElement;
    myPreviousElementDeleteWasMarked = previousElementWasMarked;
    // Show back button if myPreviousElementDelete is valid
    if (myPreviousElementDelete != nullptr) {
        // disable myPreviousElementInspect to avoid inconsistences
        myPreviousElementInspect = nullptr;
        inspectElement(AC);
        myHeaderLeftFrame->show();
        myBackButton->show();
    }
}

void
GNEInspectorFrame::removeInspectedAC(GNEAttributeCarrier* ac) {
    // Only remove if there is inspected ACs
    if (myInspectedACs.size() > 0) {
        // Try to find AC in myACs
        auto i = std::find(myInspectedACs.begin(), myInspectedACs.end(), ac);
        // if was found
        if (i != myInspectedACs.end()) {
            // erase AC from inspected ACs
            myInspectedACs.erase(i);
            // Write Warning in console if we're in testing mode
            WRITE_DEBUG("Removed inspected element from Inspected ACs. " + toString(myInspectedACs.size()) + " ACs remains.");
            // Inspect multi selection again
            inspectMultisection(myInspectedACs);
        }
    }
}


void
GNEInspectorFrame::clearInspectedAC() {
    // Only remove if there is inspected ACs
    if (myInspectedACs.size() > 0) {
        // clear ACs
        myInspectedACs.clear();
        myViewNet->setACUnderCursor(nullptr);
        // Inspect multi selection again (to hide all Editors)
        inspectMultisection(myInspectedACs);
    }
}


GNEFrame::ACHierarchy*
GNEInspectorFrame::getACHierarchy() const {
    return myACHierarchy;
}


GNEInspectorFrame::TemplateEditor*
GNEInspectorFrame::getTemplateEditor() const {
    return myTemplateEditor;
}


long
GNEInspectorFrame::onCmdGoBack(FXObject*, FXSelector, void*) {
    // Inspect previous element or go back to Delete Frame
    if (myPreviousElementInspect) {
        inspectElement(myPreviousElementInspect);
        myPreviousElementInspect = nullptr;
    } else if (myPreviousElementDelete != nullptr) {
        myPreviousElementDelete = nullptr;
        // Hide inspect frame and show delete frame
        hide();
        myViewNet->getViewParent()->getDeleteFrame()->show();
    }
    return 1;
}


const std::vector<GNEAttributeCarrier*>&
GNEInspectorFrame::getInspectedACs() const {
    return myInspectedACs;
}

// ===========================================================================
// GNEInspectorFrame::AttributesEditor::AttributeInput method definitions
// ===========================================================================

GNEInspectorFrame::AttributesEditor::AttributeInput::AttributeInput(GNEInspectorFrame::AttributesEditor* attributeEditorParent) :
    FXHorizontalFrame(attributeEditorParent, GUIDesignAuxiliarHorizontalFrame),
    myAttributesEditorParent(attributeEditorParent),
    myTag(SUMO_TAG_NOTHING),
    myAttr(SUMO_ATTR_NOTHING),
    myMultiple(false) {
    // Create and hide label
    myLabel = new FXLabel(this, "attributeLabel", 0, GUIDesignLabelAttribute);
    myLabel->hide();
    // Create and hide ButtonCombinableChoices
    myButtonCombinableChoices = new FXButton(this, "AttributeButton", 0, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myButtonCombinableChoices->hide();
    // create and hidde color editor
    myColorEditor = new FXButton(this, "ColorButton", 0, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myColorEditor->hide();
    // Create and hide textField for int attributes
    myTextFieldInt = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldInt);
    myTextFieldInt->hide();
    // Create and hide textField for real/time attributes
    myTextFieldReal = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextFieldReal);
    myTextFieldReal->hide();
    // Create and hide textField for string attributes
    myTextFieldStrings = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myTextFieldStrings->hide();
    // Create and hide ComboBox
    myChoicesCombo = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBoxAttribute);
    myChoicesCombo->hide();
    // Create and hide checkButton
    myBoolCheckButton = new FXCheckButton(this, "", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);
    myBoolCheckButton->hide();
}


void
GNEInspectorFrame::AttributesEditor::AttributeInput::showAttribute(SumoXMLTag ACTag, SumoXMLAttr ACAttr, const std::string& value) {
    // Set actual Tag, Attribute and multiple
    myTag = ACTag;
    myAttr = ACAttr;
    // set multiple
    myMultiple = GNEAttributeCarrier::parse<std::vector<std::string>>(value).size() > 1;
    // obtain attribute property (only for improve code legibility)
    const auto& attrValue = GNEAttributeCarrier::getTagProperties(myTag).getAttribute(myAttr);
    // enable all input values
    enableAttributeInputElements();
    if (attrValue.isColor()) {
        myColorEditor->setTextColor(FXRGB(0, 0, 0));
        myColorEditor->setText(toString(ACAttr).c_str());
        myColorEditor->show();
    } else {
        // Show attribute Label
        myLabel->setText(toString(myAttr).c_str());
        myLabel->show();
    }
    // Set field depending of the type of value
    if (attrValue.isBool()) {
        // set check button
        if (GNEAttributeCarrier::parseStringToANDBool(value)) {
            myBoolCheckButton->setCheck(true);
            myBoolCheckButton->setText("true");
        } else {
            myBoolCheckButton->setCheck(false);
            myBoolCheckButton->setText("false");
        }
        // show check button
        myBoolCheckButton->show();
        // enable or disable depending if attribute is editable
        if (attrValue.isNonEditable()) {
            myBoolCheckButton->disable();
        } else {
            myBoolCheckButton->enable();
        }
    } else if (attrValue.isDiscrete()) {
        // Check if are combinable choices
        if ((attrValue.getDiscreteValues().size() > 0) && attrValue.isCombinable()) {
            // hide label
            myLabel->hide();
            // Show button combinable choices
            myButtonCombinableChoices->setText(toString(myAttr).c_str());
            myButtonCombinableChoices->show();
            // Show string with the values
            myTextFieldStrings->setText(value.c_str());
            myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
            myTextFieldStrings->show();
        } else if (!myMultiple) {
            // fill comboBox
            myChoicesCombo->clearItems();
            for (auto it : attrValue.getDiscreteValues()) {
                myChoicesCombo->appendItem(it.c_str());
            }
            // show combo box with values
            myChoicesCombo->setNumVisible((int)attrValue.getDiscreteValues().size());
            myChoicesCombo->setCurrentItem(myChoicesCombo->findItem(value.c_str()));
            myChoicesCombo->setTextColor(FXRGB(0, 0, 0));
            myChoicesCombo->show();
            // enable or disable depending if attribute is editable
            if (attrValue.isNonEditable()) {
                myChoicesCombo->disable();
            } else {
                myChoicesCombo->enable();
            }
        } else {
            // represent combinable choices in multiple selections always with a textfield instead with a comboBox
            myTextFieldStrings->setText(value.c_str());
            myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
            myTextFieldStrings->show();
            // enable or disable depending if attribute is editable
            if (attrValue.isNonEditable()) {
                myTextFieldStrings->disable();
            } else {
                myTextFieldStrings->enable();
            }
        }
    } else if (attrValue.isFloat() || attrValue.isTime()) {
        // show TextField for real/time values
        myTextFieldReal->setText(value.c_str());
        myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
        myTextFieldReal->show();
        // enable or disable depending if attribute is editable
        if (attrValue.isNonEditable()) {
            myTextFieldReal->disable();
        } else {
            myTextFieldReal->enable();
        }
    } else if (attrValue.isInt()) {
        // Show textField for int attributes
        myTextFieldInt->setText(value.c_str());
        myTextFieldInt->setTextColor(FXRGB(0, 0, 0));
        myTextFieldInt->show();
        // enable or disable depending if attribute is editable
        if (attrValue.isNonEditable()) {
            myTextFieldInt->disable();
        } else {
            myTextFieldInt->enable();
        }
    } else {
        // In any other case (String, list, etc.), show value as String
        myTextFieldStrings->setText(value.c_str());
        myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
        myTextFieldStrings->show();
        // enable or disable depending if attribute is editable
        if (attrValue.isNonEditable()) {
            myTextFieldStrings->disable();
        } else {
            myTextFieldStrings->enable();
        }
    }
    // Show AttributeInput
    show();
    // recalc after show elements
    recalc();
}


void
GNEInspectorFrame::AttributesEditor::AttributeInput::hideAttribute() {
    // Hide all elements
    myLabel->hide();
    myTextFieldInt->hide();
    myTextFieldReal->hide();
    myTextFieldStrings->hide();
    myChoicesCombo->hide();
    myBoolCheckButton->hide();
    myButtonCombinableChoices->hide();
    myColorEditor->hide();
    // hide AttributeInput
    hide();
    // recalc after hide all elements
    recalc();
}


void
GNEInspectorFrame::AttributesEditor::AttributeInput::refreshAttributeInput(const std::string& value, bool forceRefresh) {
    if (myTextFieldInt->shown()) {
        // set last valid value and restore color if onlyValid is disabled
        if (myTextFieldInt->getTextColor() == FXRGB(0, 0, 0) || forceRefresh) {
            myTextFieldInt->setText(value.c_str());
            myTextFieldInt->setTextColor(FXRGB(0, 0, 0));
        }
    } else if (myTextFieldReal->shown()) {
        // set last valid value and restore color if onlyValid is disabled
        if (myTextFieldReal->getTextColor() == FXRGB(0, 0, 0) || forceRefresh) {
            myTextFieldReal->setText(value.c_str());
            myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
        }
    } else if (myTextFieldStrings->shown()) {
        // set last valid value and restore color if onlyValid is disabled
        if (myTextFieldStrings->getTextColor() == FXRGB(0, 0, 0) || forceRefresh) {
            myTextFieldStrings->setText(value.c_str());
            myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
        }
    } else if (myChoicesCombo->shown()) {
        // set last valid value and restore color if onlyValid is disabled
        if (myChoicesCombo->getTextColor() == FXRGB(0, 0, 0) || forceRefresh) {
            myChoicesCombo->setText(value.c_str());
            myChoicesCombo->setTextColor(FXRGB(0, 0, 0));
        }
    } else if (myBoolCheckButton->shown()) {
        myBoolCheckButton->setCheck(GNEAttributeCarrier::parse<bool>(value));
    }
}


bool
GNEInspectorFrame::AttributesEditor::AttributeInput::isCurrentAttributeValid() const {
    return ((myTextFieldInt->getTextColor() == FXRGB(0, 0, 0)) && (myTextFieldReal->getTextColor() == FXRGB(0, 0, 0)) &&
            (myTextFieldStrings->getTextColor() == FXRGB(0, 0, 0)) && (myChoicesCombo->getTextColor() == FXRGB(0, 0, 0)));
}


long
GNEInspectorFrame::AttributesEditor::AttributeInput::onCmdOpenAttributeDialog(FXObject* obj, FXSelector, void*) {
    if (obj == myColorEditor) {
        // create FXColorDialog
        FXColorDialog colordialog(this, tr("Color Dialog"));
        colordialog.setTarget(this);
        // If previous attribute wasn't correct, set black as default color
        if (GNEAttributeCarrier::canParse<RGBColor>(myTextFieldStrings->getText().text())) {
            colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::parseColor(myTextFieldStrings->getText().text())));
        } else {
            colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::parseColor(GNEAttributeCarrier::getTagProperties(myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs().front()->getTag()).getDefaultValue(myAttr))));
        }
        // execute dialog to get a new color
        if (colordialog.execute()) {
            std::string newValue = toString(MFXUtils::getRGBColor(colordialog.getRGBA()));
            myTextFieldStrings->setText(newValue.c_str());
            if (myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs().front()->isValid(myAttr, newValue)) {
                // if its valid for the first AC than its valid for all (of the same type)
                if (myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs().size() > 1) {
                    myAttributesEditorParent->getInspectorFrameParent()->getViewNet()->getUndoList()->p_begin("Change multiple attributes");
                }
                // Set new value of attribute in all selected ACs
                for (auto it_ac : myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs()) {
                    it_ac->setAttribute(myAttr, newValue, myAttributesEditorParent->getInspectorFrameParent()->getViewNet()->getUndoList());
                }
                // If previously value was incorrect, change font color to black
                myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
                myTextFieldStrings->killFocus();
            }
        }
        return 0;
    } else if (obj == myButtonCombinableChoices) {
        // if its valid for the first AC than its valid for all (of the same type)
        if (myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs().size() > 1) {
            myAttributesEditorParent->getInspectorFrameParent()->getViewNet()->getUndoList()->p_begin("Change multiple attributes");
        }
        // open GNEDialog_AllowDisallow
        GNEDialog_AllowDisallow(myAttributesEditorParent->getInspectorFrameParent()->getViewNet(), myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs().front()).execute();
        std::string allowed = myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs().front()->getAttribute(SUMO_ATTR_ALLOW);
        // Set new value of attribute in all selected ACs
        for (auto it_ac : myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs()) {
            it_ac->setAttribute(SUMO_ATTR_ALLOW, allowed, myAttributesEditorParent->getInspectorFrameParent()->getViewNet()->getUndoList());
        }
        // finish change multiple attributes
        if (myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs().size() > 1) {
            myAttributesEditorParent->getInspectorFrameParent()->getViewNet()->getUndoList()->p_end();
        }
        return 1;
    } else {
        throw ProcessError("Invalid call to onCmdOpenAttributeDialog");
    }
}


long
GNEInspectorFrame::AttributesEditor::AttributeInput::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    // Declare changed value
    std::string newVal;
    bool refreshGEOAndNeteditEditors = false;
    // get Tag and attribute Values (only for improve efficiency)
    const auto& tagValues = GNEAttributeCarrier::getTagProperties(myTag);
    const auto& attrValues = tagValues.getAttribute(myAttr);
    // First, obtain the string value of the new attribute depending of their type
    if (attrValues.isBool()) {
        // Set true o false depending of the checkBox
        if (myBoolCheckButton->getCheck()) {
            myBoolCheckButton->setText("true");
            newVal = "true";
        } else {
            myBoolCheckButton->setText("false");
            newVal = "false";
        }
    } else if (attrValues.isDiscrete()) {
        // Check if are combinable choices (for example, Vehicle Types)
        if ((attrValues.getDiscreteValues().size() > 0) &&
                attrValues.isCombinable()) {
            // Get value obtained using AttributesEditor
            newVal = myTextFieldStrings->getText().text();
        } else if (!myMultiple) {
            // Get value of ComboBox
            newVal = myChoicesCombo->getText().text();
        } else {
            // due this is a multiple selection, obtain value of myTextFieldStrings instead of comboBox
            newVal = myTextFieldStrings->getText().text();
        }
    } else if (attrValues.isFloat() || attrValues.isTime()) {
        // Check if default value of attribute must be set
        if (myTextFieldReal->getText().empty() && attrValues.hasDefaultValue()) {
            newVal = tagValues.getDefaultValue(myAttr);
            myTextFieldReal->setText(newVal.c_str());
        } else {
            // obtain value of myTextFieldReal
            newVal = myTextFieldReal->getText().text();
        }
    } else if (attrValues.isInt()) {
        // Check if default value of attribute must be set
        if (myTextFieldInt->getText().empty() && attrValues.hasDefaultValue()) {
            newVal = tagValues.getDefaultValue(myAttr);
            myTextFieldInt->setText(newVal.c_str());
        } else {
            // obtain value of myTextFieldInt
            newVal = myTextFieldInt->getText().text();
        }
    } else if (attrValues.isString()) {
        // Check if default value of attribute must be set
        if (myTextFieldStrings->getText().empty() && attrValues.hasDefaultValue()) {
            newVal = tagValues.getDefaultValue(myAttr);
            myTextFieldStrings->setText(newVal.c_str());
        } else {
            // obtain value of myTextFieldStrings
            newVal = myTextFieldStrings->getText().text();
        }
    }

    // we need a extra check for Position and Shape Values, due #2658
    if ((myAttr == SUMO_ATTR_POSITION) || (myAttr == SUMO_ATTR_SHAPE)) {
        newVal = stripWhitespaceAfterComma(newVal);
        // due we're changing a Position and Shape attribute, GEO and Netedit Editors must be refresh
        refreshGEOAndNeteditEditors = true;
    }

    // Check if attribute must be changed
    if (myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs().front()->isValid(myAttr, newVal)) {
        // if its valid for the first AC than its valid for all (of the same type)
        if (myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs().size() > 1) {
            myAttributesEditorParent->getInspectorFrameParent()->getViewNet()->getUndoList()->p_begin("Change multiple attributes");
        } else if (myAttr == SUMO_ATTR_ID) {
            // IDs attribute has to be encapsulated
            myAttributesEditorParent->getInspectorFrameParent()->getViewNet()->getUndoList()->p_begin("change " + toString(myTag) + " attribute");
        }
        // Set new value of attribute in all selected ACs
        for (auto it_ac : myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs()) {
            it_ac->setAttribute(myAttr, newVal, myAttributesEditorParent->getInspectorFrameParent()->getViewNet()->getUndoList());
        }
        // finish change multiple attributes or ID Attributes
        if (myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs().size() > 1) {
            myAttributesEditorParent->getInspectorFrameParent()->getViewNet()->getUndoList()->p_end();
        } else if (myAttr == SUMO_ATTR_ID) {
            myAttributesEditorParent->getInspectorFrameParent()->getViewNet()->getUndoList()->p_end();
        }
        // If previously value was incorrect, change font color to black
        if (attrValues.isCombinable()) {
            myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
            myTextFieldStrings->killFocus();
            // in this case, we need to refresh the other values (For example, allow/Disallow objects)
            myAttributesEditorParent->refreshAttributeEditor(false, false);
        } else if (attrValues.isDiscrete()) {
            myChoicesCombo->setTextColor(FXRGB(0, 0, 0));
            myChoicesCombo->killFocus();
        } else if (attrValues.isFloat() || attrValues.isTime()) {
            myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
            myTextFieldReal->killFocus();
        } else if (attrValues.isInt() && myTextFieldStrings != 0) {
            myTextFieldInt->setTextColor(FXRGB(0, 0, 0));
            myTextFieldInt->killFocus();
        } else if (myTextFieldStrings != 0) {
            myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
            myTextFieldStrings->killFocus();
        }
        // Check if GEO and Netedit editors must be refresh
        if (refreshGEOAndNeteditEditors) {
            myAttributesEditorParent->getInspectorFrameParent()->myNeteditAttributesEditor->refreshNeteditAttributesEditor(true);
            myAttributesEditorParent->getInspectorFrameParent()->myGEOAttributesEditor->refreshGEOAttributesEditor(true);
        }
    } else {
        // If value of TextField isn't valid, change color to Red depending of type
        if (attrValues.isCombinable()) {
            myTextFieldStrings->setTextColor(FXRGB(255, 0, 0));
            myTextFieldStrings->killFocus();
        } else if (attrValues.isDiscrete()) {
            myChoicesCombo->setTextColor(FXRGB(255, 0, 0));
            myChoicesCombo->killFocus();
        } else if (attrValues.isFloat() || attrValues.isTime()) {
            myTextFieldReal->setTextColor(FXRGB(255, 0, 0));
        } else if (attrValues.isInt() && myTextFieldStrings != 0) {
            myTextFieldInt->setTextColor(FXRGB(255, 0, 0));
        } else if (myTextFieldStrings != 0) {
            myTextFieldStrings->setTextColor(FXRGB(255, 0, 0));
        }
        // Write Warning in console if we're in testing mode
        WRITE_DEBUG("Value '" + newVal + "' for attribute " + toString(myAttr) + " of " + toString(myTag) + " isn't valid");
    }
    return 1;
}


std::string
GNEInspectorFrame::AttributesEditor::AttributeInput::stripWhitespaceAfterComma(const std::string& stringValue) {
    std::string result(stringValue);
    while (result.find(", ") != std::string::npos) {
        result = StringUtils::replace(result, ", ", ",");
    }
    return result;
}


void
GNEInspectorFrame::AttributesEditor::AttributeInput::enableAttributeInputElements() {
    myTextFieldInt->enable();
    myTextFieldReal->enable();
    myTextFieldStrings->enable();
    myChoicesCombo->enable();
    myBoolCheckButton->enable();
}


void
GNEInspectorFrame::AttributesEditor::AttributeInput::disableAttributeInputElements() {
    myTextFieldInt->disable();
    myTextFieldReal->disable();
    myTextFieldStrings->disable();
    myChoicesCombo->disable();
    myBoolCheckButton->disable();
}

// ===========================================================================
// GNEInspectorFrame::AttributesEditor method definitions
// ===========================================================================

GNEInspectorFrame::AttributesEditor::AttributesEditor(GNEInspectorFrame* inspectorFrameParent) :
    FXGroupBox(inspectorFrameParent->myContentFrame, "Internal attributes", GUIDesignGroupBoxFrame),
    myInspectorFrameParent(inspectorFrameParent) {
    // Create sufficient AttributeInput for all types of AttributeCarriers
    for (int i = 0; i < (int)GNEAttributeCarrier::getHigherNumberOfAttributes(); i++) {
        myVectorOfAttributeInputs.push_back(new AttributeInput(this));
    }
    // Create help button
    myHelpButton = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
}


void
GNEInspectorFrame::AttributesEditor::showAttributeEditor() {
    if (myInspectorFrameParent->getInspectedACs().size() > 0) {
        // Gets tag (only for simplify code)
        SumoXMLTag ACFrontTag = myInspectorFrameParent->getInspectedACs().front()->getTag();
        //  check if current AC is a Junction without TLSs (needed to hidde TLS options)
        bool disableTLSinJunctions = (dynamic_cast<GNEJunction*>(myInspectorFrameParent->getInspectedACs().front()) && (dynamic_cast<GNEJunction*>(myInspectorFrameParent->getInspectedACs().front())->getNBNode()->getControllingTLS().empty()));
        // Iterate over attributes
        for (auto it : GNEAttributeCarrier::getTagProperties(ACFrontTag)) {
            // disable editing for unique attributes in case of multi-selection
            if ((myInspectorFrameParent->getInspectedACs().size() > 1) && it.second.isUnique()) {
                continue;
            }
            // Declare a set of occuring values and insert attribute's values of item
            std::set<std::string> occuringValues;
            for (auto it_ac : myInspectorFrameParent->getInspectedACs()) {
                occuringValues.insert(it_ac->getAttribute(it.first));
            }
            // get current value
            std::ostringstream oss;
            for (auto it_val = occuringValues.begin(); it_val != occuringValues.end(); it_val++) {
                if (it_val != occuringValues.begin()) {
                    oss << " ";
                }
                oss << *it_val;
            }
            // Show attribute
            if ((disableTLSinJunctions && (ACFrontTag == SUMO_TAG_JUNCTION) && ((it.first == SUMO_ATTR_TLTYPE) || (it.first == SUMO_ATTR_TLID))) == false) {
                // first show AttributesEditor
                show();
                // show attribute
                myVectorOfAttributeInputs[it.second.getPositionListed()]->showAttribute(ACFrontTag, it.first, oss.str());
            }
        }
    }
}


void
GNEInspectorFrame::AttributesEditor::hideAttributesEditor() {
    // hide al attributes
    for (auto i : myVectorOfAttributeInputs) {
        i->hideAttribute();
    }
    // hide also AttributesEditor
    hide();
}


void
GNEInspectorFrame::AttributesEditor::refreshAttributeEditor(bool forceRefreshShape, bool forceRefreshPosition) {
    if (myInspectorFrameParent->getInspectedACs().size() > 0) {
        // Declare pointer for allow/Disallow vehicles
        std::pair<GNEInspectorFrame::AttributesEditor::AttributeInput*, std::string> myAllowAttribute(nullptr, "");
        std::pair<GNEInspectorFrame::AttributesEditor::AttributeInput*, std::string> myDisallowAttribute(nullptr, "");
        // Gets tag (only for simplify code)
        SumoXMLTag ACFrontTag = myInspectorFrameParent->getInspectedACs().front()->getTag();
        //  check if current AC is a Junction without TLSs (needed to hidde TLS options)
        bool disableTLSinJunctions = (dynamic_cast<GNEJunction*>(myInspectorFrameParent->getInspectedACs().front()) && (dynamic_cast<GNEJunction*>(myInspectorFrameParent->getInspectedACs().front())->getNBNode()->getControllingTLS().empty()));
        // Iterate over attributes
        for (auto it : GNEAttributeCarrier::getTagProperties(ACFrontTag)) {
            // disable editing for unique attributes in case of multi-selection
            if ((myInspectorFrameParent->getInspectedACs().size() > 1) && it.second.isUnique()) {
                continue;
            }
            // Declare a set of occuring values and insert attribute's values of item
            std::set<std::string> occuringValues;
            for (auto it_ac : myInspectorFrameParent->getInspectedACs()) {
                occuringValues.insert(it_ac->getAttribute(it.first));
            }
            // get current value
            std::ostringstream oss;
            for (auto it_val = occuringValues.begin(); it_val != occuringValues.end(); it_val++) {
                if (it_val != occuringValues.begin()) {
                    oss << " ";
                }
                oss << *it_val;
            }
            // Show attribute
            if ((disableTLSinJunctions && (ACFrontTag == SUMO_TAG_JUNCTION) && ((it.first == SUMO_ATTR_TLTYPE) || (it.first == SUMO_ATTR_TLID))) == false) {
                // refresh attribute, with a special case for allow/disallow vehicles
                if (it.first  == SUMO_ATTR_ALLOW) {
                    myAllowAttribute.first = myVectorOfAttributeInputs[it.second.getPositionListed()];
                    myAllowAttribute.second = oss.str();
                } else if (it.first  == SUMO_ATTR_DISALLOW) {
                    myDisallowAttribute.first = myVectorOfAttributeInputs[it.second.getPositionListed()];
                    myDisallowAttribute.second = oss.str();
                } else {
                    // Check if refresh of Position or Shape has to be forced
                    if ((it.first  == SUMO_ATTR_SHAPE) && forceRefreshShape) {
                        myVectorOfAttributeInputs[it.second.getPositionListed()]->refreshAttributeInput(oss.str(), true);
                    } else if ((it.first  == SUMO_ATTR_POSITION) && forceRefreshPosition) {
                        // Refresh attributes maintain invalid values
                        myVectorOfAttributeInputs[it.second.getPositionListed()]->refreshAttributeInput(oss.str(), true);
                    } else {
                        // Refresh attributes maintain invalid values
                        myVectorOfAttributeInputs[it.second.getPositionListed()]->refreshAttributeInput(oss.str(), false);
                    }
                }
            }
        }
        // Check special case for Allow/Disallow attributes
        if (myAllowAttribute.first && myDisallowAttribute.first) {
            // if allow attribute is valid but disallow attribute is invalid
            if (myAllowAttribute.first->isCurrentAttributeValid() && !myDisallowAttribute.first->isCurrentAttributeValid()) {
                // force refresh of disallow attribute
                myDisallowAttribute.first->refreshAttributeInput(myDisallowAttribute.second, true);
            }
            // if disallow attribute is valid but allow attribute is invalid
            if (myDisallowAttribute.first->isCurrentAttributeValid() && !myAllowAttribute.first->isCurrentAttributeValid()) {
                // force refresh of disallow attribute
                myAllowAttribute.first->refreshAttributeInput(myAllowAttribute.second, true);
            }
        }
    }
}


GNEInspectorFrame*
GNEInspectorFrame::AttributesEditor::getInspectorFrameParent() const {
    return myInspectorFrameParent;
}


long
GNEInspectorFrame::AttributesEditor::onCmdAttributeHelp(FXObject*, FXSelector, void*) {
    // open Help attributes dialog if there is inspected ACs
    if (myInspectorFrameParent->getInspectedACs().size() > 0) {
        // open Help attributes dialog
        myInspectorFrameParent->openHelpAttributesDialog(myInspectorFrameParent->getInspectedACs().front()->getTag());
    }
    return 1;
}

// ===========================================================================
// GNEInspectorFrame::NeteditAttributesEditor method definitions
// ===========================================================================

GNEInspectorFrame::NeteditAttributesEditor::NeteditAttributesEditor(GNEInspectorFrame* inspectorFrameParent) :
    FXGroupBox(inspectorFrameParent->myContentFrame, "Netedit attributes", GUIDesignGroupBoxFrame),
    myInspectorFrameParent(inspectorFrameParent) {

    // Create elements for additional parent
    myHorizontalFrameAdditionalParent = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myLabelAdditionalParent = new FXLabel(myHorizontalFrameAdditionalParent, "Block move", 0, GUIDesignLabelAttribute);
    myTextFieldAdditionalParent = new FXTextField(myHorizontalFrameAdditionalParent, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);

    // Create elements for block movement
    myHorizontalFrameBlockMovement = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myLabelBlockMovement = new FXLabel(myHorizontalFrameBlockMovement, "Block move", 0, GUIDesignLabelAttribute);
    myCheckBoxBlockMovement = new FXCheckButton(myHorizontalFrameBlockMovement, "", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);

    // Create elements for block shape
    myHorizontalFrameBlockShape = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myLabelBlockShape = new FXLabel(myHorizontalFrameBlockShape, "Block shape", 0, GUIDesignLabelAttribute);
    myCheckBoxBlockShape = new FXCheckButton(myHorizontalFrameBlockShape, "", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);

    // Create elements for close shape
    myHorizontalFrameCloseShape = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myLabelCloseShape = new FXLabel(myHorizontalFrameCloseShape, "Close shape", 0, GUIDesignLabelAttribute);
    myCheckBoxCloseShape = new FXCheckButton(myHorizontalFrameCloseShape, "", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);

    // Create help button
    myHelpButton = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
}


GNEInspectorFrame::NeteditAttributesEditor::~NeteditAttributesEditor() {}


void
GNEInspectorFrame::NeteditAttributesEditor::showNeteditAttributesEditor() {
    if (myInspectorFrameParent->getInspectedACs().size() > 0) {
        // obtain tag property (only for improve code legibility)
        const auto& tagValue = GNEAttributeCarrier::getTagProperties(myInspectorFrameParent->getInspectedACs().front()->getTag());
        // Check if item can be moved
        if (tagValue.canBlockMovement()) {
            // show NeteditAttributesEditor
            show();
            // Iterate over AC to obtain values
            bool value = true;
            for (auto i : myInspectorFrameParent->getInspectedACs()) {
                value &= GNEAttributeCarrier::parse<bool>(i->getAttribute(GNE_ATTR_BLOCK_MOVEMENT));
            }
            // show block movement frame
            myHorizontalFrameBlockMovement->show();
            // set check box value and update label
            if (value) {
                myCheckBoxBlockMovement->setCheck(true);
                myCheckBoxBlockMovement->setText("true");
            } else {
                myCheckBoxBlockMovement->setCheck(false);
                myCheckBoxBlockMovement->setText("false");
            }
        }
        // check if item can block their shape
        if (tagValue.canBlockShape()) {
            // show NeteditAttributesEditor
            show();
            // Iterate over AC to obtain values
            bool value = true;
            for (auto i : myInspectorFrameParent->getInspectedACs()) {
                value &= GNEAttributeCarrier::parse<bool>(i->getAttribute(GNE_ATTR_BLOCK_SHAPE));
            }
            // show block shape frame
            myHorizontalFrameBlockShape->show();
            // set check box value and update label
            if (value) {
                myCheckBoxBlockShape->setCheck(true);
                myCheckBoxBlockShape->setText("true");
            } else {
                myCheckBoxBlockShape->setCheck(false);
                myCheckBoxBlockShape->setText("false");
            }
        }
        // check if item can block their shape
        if (tagValue.canCloseShape()) {
            // show NeteditAttributesEditor
            show();
            // Iterate over AC to obtain values
            bool value = true;
            for (auto i : myInspectorFrameParent->getInspectedACs()) {
                value &= GNEAttributeCarrier::parse<bool>(i->getAttribute(GNE_ATTR_CLOSE_SHAPE));
            }
            // show close shape frame
            myHorizontalFrameCloseShape->show();
            // set check box value and update label
            if (value) {
                myCheckBoxCloseShape->setCheck(true);
                myCheckBoxCloseShape->setText("true");
            } else {
                myCheckBoxCloseShape->setCheck(false);
                myCheckBoxCloseShape->setText("false");
            }
        }
        // Check if item has another item as parent and can be reparemt
        if (tagValue.hasParent() && tagValue.canBeReparent()) {
            // show NeteditAttributesEditor
            show();
            // obtain additional Parent
            GNEAdditional* additional = myInspectorFrameParent->getViewNet()->getNet()->retrieveAdditional(tagValue.getParentTag(), myInspectorFrameParent->getInspectedACs().front()->getAttribute(GNE_ATTR_PARENT));
            // show additional parent frame
            myHorizontalFrameAdditionalParent->show();
            // set Label and TextField with the Tag and ID of parent
            myLabelAdditionalParent->setText((toString(additional->getTag()) + " parent").c_str());
            myTextFieldAdditionalParent->setText(additional->getID().c_str());
        }
    }
}


void
GNEInspectorFrame::NeteditAttributesEditor::hideNeteditAttributesEditor() {
    // hide all elements of GroupBox
    myHorizontalFrameAdditionalParent->hide();
    myHorizontalFrameBlockMovement->hide();
    myHorizontalFrameBlockShape->hide();
    myHorizontalFrameCloseShape->hide();
    // hide groupbox
    hide();
}


void
GNEInspectorFrame::NeteditAttributesEditor::refreshNeteditAttributesEditor(bool forceRefresh) {
    if (myInspectorFrameParent->getInspectedACs().size() > 0) {
        // refresh block movement
        if (myHorizontalFrameBlockMovement->shown()) {
            // Iterate over AC to obtain values
            bool value = true;
            for (auto i : myInspectorFrameParent->getInspectedACs()) {
                value &= GNEAttributeCarrier::parse<bool>(i->getAttribute(GNE_ATTR_BLOCK_MOVEMENT));
            }
            // set check box value and update label
            if (value) {
                myCheckBoxBlockMovement->setCheck(true);
                myCheckBoxBlockMovement->setText("true");
            } else {
                myCheckBoxBlockMovement->setCheck(false);
                myCheckBoxBlockMovement->setText("false");
            }
        }
        // refresh block shape
        if (myHorizontalFrameBlockShape->shown()) {
            // Iterate over AC to obtain values
            bool value = true;
            for (auto i : myInspectorFrameParent->getInspectedACs()) {
                value &= GNEAttributeCarrier::parse<bool>(i->getAttribute(GNE_ATTR_BLOCK_SHAPE));
            }
            // set check box value and update label
            if (value) {
                myCheckBoxBlockShape->setCheck(true);
                myCheckBoxBlockShape->setText("true");
            } else {
                myCheckBoxBlockShape->setCheck(false);
                myCheckBoxBlockShape->setText("false");
            }
        }
        // refresh close shape
        if (myHorizontalFrameCloseShape->shown()) {
            // Iterate over AC to obtain values
            bool value = true;
            for (auto i : myInspectorFrameParent->getInspectedACs()) {
                value &= GNEAttributeCarrier::parse<bool>(i->getAttribute(GNE_ATTR_CLOSE_SHAPE));
            }
            // set check box value and update label
            if (value) {
                myCheckBoxCloseShape->setCheck(true);
                myCheckBoxCloseShape->setText("true");
            } else {
                myCheckBoxCloseShape->setCheck(false);
                myCheckBoxCloseShape->setText("false");
            }
        }
        // Check if item has another item as parent (Currently only for single Additionals)
        if (myHorizontalFrameAdditionalParent->shown() && ((myTextFieldAdditionalParent->getTextColor() == FXRGB(0, 0, 0)) || forceRefresh)) {
            // set Label and TextField with the Tag and ID of parent
            myLabelAdditionalParent->setText((toString(GNEAttributeCarrier::getTagProperties(myInspectorFrameParent->getInspectedACs().front()->getTag()).getParentTag()) + " parent").c_str());
            myTextFieldAdditionalParent->setText(myInspectorFrameParent->getInspectedACs().front()->getAttribute(GNE_ATTR_PARENT).c_str());
        }
    }
}


long
GNEInspectorFrame::NeteditAttributesEditor::onCmdSetNeteditAttribute(FXObject* obj, FXSelector, void*) {

    // make sure that ACs has elements
    if (myInspectorFrameParent->getInspectedACs().size() > 0) {
        // check if we're changing multiple attributes
        if (myInspectorFrameParent->getInspectedACs().size() > 1) {
            myInspectorFrameParent->getViewNet()->getUndoList()->p_begin("Change multiple attributes");
        }
        if (obj == myCheckBoxBlockMovement) {
            // set new values in all inspected Attribute Carriers
            for (auto i : myInspectorFrameParent->getInspectedACs()) {
                if (myCheckBoxBlockMovement->getCheck() == 1) {
                    i->setAttribute(GNE_ATTR_BLOCK_MOVEMENT, "true", myInspectorFrameParent->getViewNet()->getUndoList());
                    myCheckBoxBlockMovement->setText("true");
                } else {
                    i->setAttribute(GNE_ATTR_BLOCK_MOVEMENT, "false", myInspectorFrameParent->getViewNet()->getUndoList());
                    myCheckBoxBlockMovement->setText("false");
                }
            }
        } else if (obj == myCheckBoxBlockShape) {
            // set new values in all inspected Attribute Carriers
            for (auto i : myInspectorFrameParent->getInspectedACs()) {
                if (myCheckBoxBlockShape->getCheck() == 1) {
                    i->setAttribute(GNE_ATTR_BLOCK_SHAPE, "true", myInspectorFrameParent->getViewNet()->getUndoList());
                    myCheckBoxBlockShape->setText("true");
                } else {
                    i->setAttribute(GNE_ATTR_BLOCK_SHAPE, "false", myInspectorFrameParent->getViewNet()->getUndoList());
                    myCheckBoxBlockShape->setText("false");
                }
            }
        } else if (obj == myCheckBoxCloseShape) {
            // set new values in all inspected Attribute Carriers
            for (auto i : myInspectorFrameParent->getInspectedACs()) {
                if (myCheckBoxCloseShape->getCheck() == 1) {
                    i->setAttribute(GNE_ATTR_CLOSE_SHAPE, "true", myInspectorFrameParent->getViewNet()->getUndoList());
                    myCheckBoxCloseShape->setText("true");
                } else {
                    i->setAttribute(GNE_ATTR_CLOSE_SHAPE, "false", myInspectorFrameParent->getViewNet()->getUndoList());
                    myCheckBoxCloseShape->setText("false");
                }
            }
        } else if (obj == myTextFieldAdditionalParent) {
            if (myInspectorFrameParent->getInspectedACs().front()->isValid(GNE_ATTR_PARENT, myTextFieldAdditionalParent->getText().text())) {
                // change parent of all inspected elements
                for (auto i : myInspectorFrameParent->getInspectedACs()) {
                    i->setAttribute(GNE_ATTR_PARENT, myTextFieldAdditionalParent->getText().text(), myInspectorFrameParent->getViewNet()->getUndoList());
                }
                myTextFieldAdditionalParent->setTextColor(FXRGB(0, 0, 0));
            } else {
                myTextFieldAdditionalParent->setTextColor(FXRGB(255, 0, 0));
                myTextFieldAdditionalParent->killFocus();
            }
        }
        // finish change multiple attributes
        if (myInspectorFrameParent->getInspectedACs().size() > 1) {
            myInspectorFrameParent->getViewNet()->getUndoList()->p_end();
        }
        // force refresh values of AttributesEditor and GEOAttributesEditor
        myInspectorFrameParent->myAttributesEditor->refreshAttributeEditor(true, true);
        myInspectorFrameParent->myGEOAttributesEditor->refreshGEOAttributesEditor(true);
    }
    return 1;
}


long
GNEInspectorFrame::NeteditAttributesEditor::onCmdNeteditAttributeHelp(FXObject*, FXSelector, void*) {
    return 0;
}

// ---------------------------------------------------------------------------
// GNEInspectorFrame::GEOAttributesEditor - methods
// ---------------------------------------------------------------------------

GNEInspectorFrame::GEOAttributesEditor::GEOAttributesEditor(GNEInspectorFrame* inspectorFrameParent) :
    FXGroupBox(inspectorFrameParent->myContentFrame, "GEO Attributes", GUIDesignGroupBoxFrame),
    myInspectorFrameParent(inspectorFrameParent) {

    // Create Frame for GEOAttribute
    myGEOAttributeFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myGEOAttributeLabel = new FXLabel(myGEOAttributeFrame, "Undefined GEO Attribute", 0, GUIDesignLabelAttribute);
    myGEOAttributeTextField = new FXTextField(myGEOAttributeFrame, GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);

    // Create Frame for use GEO
    myUseGEOFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myUseGEOLabel = new FXLabel(myUseGEOFrame, toString(SUMO_ATTR_GEO).c_str(), 0, GUIDesignLabelAttribute);
    myUseGEOCheckButton = new FXCheckButton(myUseGEOFrame, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);

    // Create help button
    myHelpButton = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
}


GNEInspectorFrame::GEOAttributesEditor::~GEOAttributesEditor() {}


void
GNEInspectorFrame::GEOAttributesEditor::showGEOAttributesEditor() {
    // make sure that ACs has elements
    if (myInspectorFrameParent->getInspectedACs().size() > 0) {
        // obtain tag property (only for improve code legibility)
        const auto& tagValue = GNEAttributeCarrier::getTagProperties(myInspectorFrameParent->getInspectedACs().front()->getTag());
        // check if item can use a geo position
        if (tagValue.hasGEOPosition() || tagValue.hasGEOShape()) {
            // show GEOAttributesEditor
            show();
            // Iterate over AC to obtain values
            bool value = true;
            for (auto i : myInspectorFrameParent->getInspectedACs()) {
                value &= GNEAttributeCarrier::parse<bool>(i->getAttribute(SUMO_ATTR_GEO));
            }
            // show use geo frame
            myUseGEOFrame->show();
            // set UseGEOCheckButton value of and update label (only if geo conversion is defined)
            if (GeoConvHelper::getFinal().getProjString() != "!") {
                myUseGEOCheckButton->enable();
                if (value) {
                    myUseGEOCheckButton->setCheck(true);
                    myUseGEOCheckButton->setText("true");
                } else {
                    myUseGEOCheckButton->setCheck(false);
                    myUseGEOCheckButton->setText("false");
                }
            } else {
                myUseGEOCheckButton->disable();
            }
            // now specify if a single position or an entire shape must be shown (note: cannot be shown both at the same time, and GEO Shape/Position only works for single selections)
            if (tagValue.hasGEOPosition() && myInspectorFrameParent->getInspectedACs().size() == 1) {
                myGEOAttributeFrame->show();
                myGEOAttributeLabel->setText(toString(SUMO_ATTR_GEOPOSITION).c_str());
                myGEOAttributeTextField->setTextColor(FXRGB(0, 0, 0));
                // only allow edit if geo conversion is defined
                if (GeoConvHelper::getFinal().getProjString() != "!") {
                    myGEOAttributeTextField->enable();
                    myGEOAttributeTextField->setText(myInspectorFrameParent->getInspectedACs().front()->getAttribute(SUMO_ATTR_GEOPOSITION).c_str());
                } else {
                    myGEOAttributeTextField->disable();
                    myGEOAttributeTextField->setText("No geo-conversion defined");
                }
            } else if (tagValue.hasGEOShape() && myInspectorFrameParent->getInspectedACs().size() == 1) {
                myGEOAttributeFrame->show();
                myGEOAttributeLabel->setText(toString(SUMO_ATTR_GEOSHAPE).c_str());
                myGEOAttributeTextField->setTextColor(FXRGB(0, 0, 0));
                // only allow edit if geo conversion is defined
                if (GeoConvHelper::getFinal().getProjString() != "!") {
                    myGEOAttributeTextField->enable();
                    myGEOAttributeTextField->setText(myInspectorFrameParent->getInspectedACs().front()->getAttribute(SUMO_ATTR_GEOSHAPE).c_str());
                } else {
                    myGEOAttributeTextField->disable();
                    myGEOAttributeTextField->setText("No geo-conversion defined");
                }
            }
        }
    }
}


void
GNEInspectorFrame::GEOAttributesEditor::hideGEOAttributesEditor() {
    // hide all elements of GroupBox
    myGEOAttributeFrame->hide();
    myUseGEOFrame->hide();
    // hide groupbox
    hide();
}


void
GNEInspectorFrame::GEOAttributesEditor::refreshGEOAttributesEditor(bool forceRefresh) {
    // obtain tag property (only for improve code legibility)
    const auto& tagValue = GNEAttributeCarrier::getTagProperties(myInspectorFrameParent->getInspectedACs().front()->getTag());
    // Check that myGEOAttributeFrame is shown
    if ((GeoConvHelper::getFinal().getProjString() != "!") && myGEOAttributeFrame->shown() && ((myGEOAttributeTextField->getTextColor() == FXRGB(0, 0, 0)) || forceRefresh)) {
        if (tagValue.hasGEOPosition()) {
            myGEOAttributeTextField->setText(myInspectorFrameParent->getInspectedACs().front()->getAttribute(SUMO_ATTR_GEOPOSITION).c_str());
        } else if (tagValue.hasGEOShape()) {
            myGEOAttributeTextField->setText(myInspectorFrameParent->getInspectedACs().front()->getAttribute(SUMO_ATTR_GEOSHAPE).c_str());
        }
        myGEOAttributeTextField->setTextColor(FXRGB(0, 0, 0));
    }
}


long
GNEInspectorFrame::GEOAttributesEditor::onCmdSetGEOAttribute(FXObject* obj, FXSelector, void*) {
    // make sure that ACs has elements
    if ((GeoConvHelper::getFinal().getProjString() != "!") && (myInspectorFrameParent->getInspectedACs().size() > 0)) {
        if (obj == myGEOAttributeTextField) {
            // obtain tag property (only for improve code legibility)
            const auto& tagValue = GNEAttributeCarrier::getTagProperties(myInspectorFrameParent->getInspectedACs().front()->getTag());
            // Change GEO Attribute depending of type (Position or shape)
            if (tagValue.hasGEOPosition()) {
                if (myInspectorFrameParent->getInspectedACs().front()->isValid(SUMO_ATTR_GEOPOSITION, myGEOAttributeTextField->getText().text())) {
                    myInspectorFrameParent->getInspectedACs().front()->setAttribute(SUMO_ATTR_GEOPOSITION, myGEOAttributeTextField->getText().text(), myInspectorFrameParent->getViewNet()->getUndoList());
                    myGEOAttributeTextField->setTextColor(FXRGB(0, 0, 0));
                } else {
                    myGEOAttributeTextField->setTextColor(FXRGB(255, 0, 0));
                    myGEOAttributeTextField->killFocus();
                }
            } else if (tagValue.hasGEOShape()) {
                if (myInspectorFrameParent->getInspectedACs().front()->isValid(SUMO_ATTR_GEOSHAPE, myGEOAttributeTextField->getText().text())) {
                    myInspectorFrameParent->getInspectedACs().front()->setAttribute(SUMO_ATTR_GEOSHAPE, myGEOAttributeTextField->getText().text(), myInspectorFrameParent->getViewNet()->getUndoList());
                    myGEOAttributeTextField->setTextColor(FXRGB(0, 0, 0));
                } else {
                    myGEOAttributeTextField->setTextColor(FXRGB(255, 0, 0));
                    myGEOAttributeTextField->killFocus();
                }
            } else {
                throw ProcessError("myGEOAttributeTextField must be hidden becaurse there isn't GEO Attribute to edit");
            }
        } else if (obj == myUseGEOCheckButton) {
            // update GEO Attribute of entire selection
            for (auto i : myInspectorFrameParent->getInspectedACs()) {
                if (myUseGEOCheckButton->getCheck() == 1) {
                    i->setAttribute(SUMO_ATTR_GEO, "true", myInspectorFrameParent->getViewNet()->getUndoList());
                    myUseGEOCheckButton->setText("true");
                } else {
                    i->setAttribute(SUMO_ATTR_GEO, "false", myInspectorFrameParent->getViewNet()->getUndoList());
                    myUseGEOCheckButton->setText("false");
                }
            }
        }
        // force refresh values of Attributes editor and NeteditAttributesEditor
        myInspectorFrameParent->myAttributesEditor->refreshAttributeEditor(true, true);
        myInspectorFrameParent->myNeteditAttributesEditor->refreshNeteditAttributesEditor(true);
    }
    return 1;
}


long
GNEInspectorFrame::GEOAttributesEditor::onCmdGEOAttributeHelp(FXObject*, FXSelector, void*) {
    FXDialogBox* helpDialog = new FXDialogBox(this, "GEO attributes Help", GUIDesignDialogBox);
    std::ostringstream help;
    help
            << " SUMO uses the World Geodetic System 84 (WGS84/UTM).\n"
            << " For a GEO-referenced network, geo coordinates are represented as pairs of Longitude and Latitude\n"
            << " in decimal degrees without extra symbols. (N,W..)\n"
            << " - Longitude: East-west position of a point on the Earth's surface.\n"
            << " - Latitude: North-south position of a point on the Earth's surface.\n"
            << " - CheckBox 'geo' enables or disables saving position in GEO coordinates\n";
    new FXLabel(helpDialog, help.str().c_str(), 0, GUIDesignLabelFrameInformation);
    // "OK"
    new FXButton(helpDialog, "OK\t\tclose", GUIIconSubSys::getIcon(ICON_ACCEPT), helpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    helpDialog->create();
    helpDialog->show();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEInspectorFrame::TemplateEditor - methods
// ---------------------------------------------------------------------------

GNEInspectorFrame::TemplateEditor::TemplateEditor(GNEInspectorFrame* inspectorFrameParent) :
    FXGroupBox(inspectorFrameParent->myContentFrame, "Templates", GUIDesignGroupBoxFrame),
    myInspectorFrameParent(inspectorFrameParent),
    myEdgeTemplate(nullptr) {

    // Create copy template button
    myCopyTemplateButton = new FXButton(this, "", 0, this, MID_GNE_INSPECTORFRAME_COPYTEMPLATE, GUIDesignButton);
    myCopyTemplateButton->hide();

    // Create set template button
    mySetTemplateButton = new FXButton(this, "Set as Template\t\t", 0, this, MID_GNE_INSPECTORFRAME_SETTEMPLATE, GUIDesignButton);
    mySetTemplateButton->hide();
}


GNEInspectorFrame::TemplateEditor::~TemplateEditor() {
    if (myEdgeTemplate) {
        // delete template
        myEdgeTemplate->decRef("GNEInspectorFrame::~GNEInspectorFrame");
        if (myEdgeTemplate->unreferenced()) {
            delete myEdgeTemplate;
        }
    }
}


void
GNEInspectorFrame::TemplateEditor::showTemplateEditor() {
    if (myInspectorFrameParent->getInspectedACs().front()->getTag() == SUMO_TAG_EDGE) {
        // show template editor
        show();
        // show "Copy Template" (caption supplied via onUpdate)
        myCopyTemplateButton->show();
        // show "Set As Template"
        if (myInspectorFrameParent->getInspectedACs().size() == 1) {
            mySetTemplateButton->show();
            mySetTemplateButton->setText(("Set edge '" + myInspectorFrameParent->getInspectedACs().front()->getID() + "' as Template").c_str());
        }
    }
}


void
GNEInspectorFrame::TemplateEditor::hideTemplateEditor() {
    // hide buttons
    myCopyTemplateButton->hide();
    mySetTemplateButton->hide();
    // hide template editor
    hide();
}


GNEEdge*
GNEInspectorFrame::TemplateEditor::getEdgeTemplate() const {
    return myEdgeTemplate;
}


void
GNEInspectorFrame::TemplateEditor::setEdgeTemplate(GNEEdge* tpl) {
    if (myEdgeTemplate) {
        myEdgeTemplate->decRef("GNEInspectorFrame::setEdgeTemplate");
        if (myEdgeTemplate->unreferenced()) {
            delete myEdgeTemplate;
        }
    }
    myEdgeTemplate = tpl;
    myEdgeTemplate->incRef("GNEInspectorFrame::setEdgeTemplate");
}


long
GNEInspectorFrame::TemplateEditor::onCmdCopyTemplate(FXObject*, FXSelector, void*) {
    for (auto it : myInspectorFrameParent->getInspectedACs()) {
        GNEEdge* edge = dynamic_cast<GNEEdge*>(it);
        assert(edge);
        edge->copyTemplate(myEdgeTemplate, myInspectorFrameParent->getViewNet()->getUndoList());
        myInspectorFrameParent->inspectMultisection(myInspectorFrameParent->getInspectedACs());
    }
    return 1;
}


long
GNEInspectorFrame::TemplateEditor::onCmdSetTemplate(FXObject*, FXSelector, void*) {
    assert(myInspectorFrameParent->getInspectedACs().size() == 1);
    GNEEdge* edge = dynamic_cast<GNEEdge*>(myInspectorFrameParent->getInspectedACs().front());
    assert(edge);
    setEdgeTemplate(edge);
    return 1;
}


long
GNEInspectorFrame::TemplateEditor::onUpdCopyTemplate(FXObject* sender, FXSelector, void*) {
    // declare caption for button
    FXString caption;
    if (myEdgeTemplate) {
        if (myInspectorFrameParent->getInspectedACs().size() == 1) {
            caption = ("Copy '" + myEdgeTemplate->getMicrosimID() + "' into edge '" + myInspectorFrameParent->getInspectedACs().front()->getID() + "'").c_str();
        } else {
            caption = ("Copy '" + myEdgeTemplate->getMicrosimID() + "' into " + toString(myInspectorFrameParent->getInspectedACs().size()) + " selected edges").c_str();
        }
        sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), nullptr);
    } else {
        caption = "No edge Template Set";
        sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), nullptr);
    }
    sender->handle(this, FXSEL(SEL_COMMAND, FXLabel::ID_SETSTRINGVALUE), (void*)&caption);
    return 1;
}

/****************************************************************************/
