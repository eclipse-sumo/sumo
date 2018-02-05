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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <cassert>
#include <cmath>
#include <regex>
#include <utils/common/MsgHandler.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/foxtools/fxexdefs.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>

#include "GNEAdditional.h"
#include "GNEAdditionalFrame.h"
#include "GNEAttributeCarrier.h"
#include "GNEChange_Selection.h"
#include "GNEConnection.h"
#include "GNECrossing.h"
#include "GNEDeleteFrame.h"
#include "GNEEdge.h"
#include "GNEInspectorFrame.h"
#include "GNEJunction.h"
#include "GNELane.h"
#include "GNENet.h"
#include "GNEPOI.h"
#include "GNEPoly.h"
#include "GNEUndoList.h"
#include "GNEViewNet.h"
#include "GNEViewParent.h"
#include "GNEDialog_AllowDisallow.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEInspectorFrame) GNEInspectorFrameMap[] = {
    FXMAPFUNC(SEL_COMMAND,              MID_GNE_INSPECTORFRAME_GOBACK,          GNEInspectorFrame::onCmdGoBack),

};

FXDEFMAP(GNEInspectorFrame::AttributesEditor::AttributeInput) AttributeInputMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNEInspectorFrame::AttributesEditor::AttributeInput::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNEInspectorFrame::AttributesEditor::AttributeInput::onCmdOpenAllowDisallowEditor)
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
    myPreviousElementInspect(NULL),
    myPreviousElementDelete(NULL) {

    // Create back button
    myBackButton = new FXButton(myHeaderLeftFrame, "", GUIIconSubSys::getIcon(ICON_NETEDITARROW), this, MID_GNE_INSPECTORFRAME_GOBACK, GUIDesignButtonIconRectangular);
    myHeaderLeftFrame->hide();
    myBackButton->hide();

    // Create Attributes Editor
    myAttributesEditor = new AttributesEditor(this);

    // Create GEO Parameters Editor
    myGEOAttributesEditor = new GEOAttributesEditor(this);

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
    inspectElement(NULL);
    GNEFrame::show();
}


void
GNEInspectorFrame::inspectElement(GNEAttributeCarrier* AC) {
    // Use the implementation of inspect for multiple AttributeCarriers to avoid repetition of code
    std::vector<GNEAttributeCarrier*> itemToInspect;
    if (AC != NULL) {
        itemToInspect.push_back(AC);
    }
    inspectMultisection(itemToInspect);
}


void
GNEInspectorFrame::inspectMultisection(const std::vector<GNEAttributeCarrier*>& ACs) {
    // hide back button
    myHeaderLeftFrame->hide();
    myBackButton->hide();
    // Assing ACs to myACs
    myACs = ACs;
    // Hide all elements
    myAttributesEditor->hideAttributesEditor();
    myNeteditAttributesEditor->hideNeteditAttributesEditor();
    myGEOAttributesEditor->hideGEOAttributesEditor();
    myTemplateEditor->hideTemplateEditor();
    myACHierarchy->hideACHierarchy();
    // If vector of attribute Carriers contain data
    if (myACs.size() > 0) {
        // Set header
        std::string headerString;
        if (dynamic_cast<GNENetElement*>(myACs.front())) {
            headerString = "Net: ";
        } else if (dynamic_cast<GNEAdditional*>(myACs.front())) {
            headerString = "Additional: ";
        } else if (dynamic_cast<GNEShape*>(myACs.front())) {
            headerString = "Shape: ";
        }
        if (myACs.size() > 1) {
            headerString += toString(myACs.size()) + " ";
        }
        headerString += toString(myACs.front()->getTag());
        if (myACs.size() > 1) {
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

        // If attributes correspond to an Edge, show template editor
        myTemplateEditor->showTemplateEditor();

        // if we inspect a single Attribute carrier vector, show their childs
        if (myACs.size() == 1) {
            myACHierarchy->showACHierarchy(myACs.front());
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
    if (myPreviousElementInspect != NULL) {
        // disable myPreviousElementDelete to avoid inconsistences
        myPreviousElementDelete = NULL;
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
    if (myPreviousElementDelete != NULL) {
        // disable myPreviousElementInspect to avoid inconsistences
        myPreviousElementInspect = NULL;
        inspectElement(AC);
        myHeaderLeftFrame->show();
        myBackButton->show();
    }
}

void 
GNEInspectorFrame::removeInspectedAC(GNEAttributeCarrier *ac) {
    // Only remove if there is inspected ACs
    if(myACs.size() > 0) {
        // Try to find AC in myACs 
        auto i = std::find(myACs.begin(), myACs.end(), ac);
        // if was found
        if(i != myACs.end()){
            // erase AC from my AC
            myACs.erase(i);
            // Write Warning in console if we're in testing mode
            if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
                WRITE_WARNING("Removed inspected element from Inspected ACs. " + toString(myACs.size()) + " ACs remains.");
            }
            // Inspect multi selection again
            inspectMultisection(myACs);
        }
    }
}


void 
GNEInspectorFrame::clearInspectedAC() {
    // Only remove if there is inspected ACs
    if (myACs.size() > 0) {
        // clear ACs
        myACs.clear();
        // Inspect multi selection again (to hide all Editors)
        inspectMultisection(myACs);
    }
}


GNEInspectorFrame::AttributesEditor*
GNEInspectorFrame::getAttributesEditor() const {
    return myAttributesEditor;
}


GNEInspectorFrame::NeteditAttributesEditor*
GNEInspectorFrame::getNeteditAttributesEditor() const {
    return myNeteditAttributesEditor;
}


GNEInspectorFrame::GEOAttributesEditor*
GNEInspectorFrame::getGEOAttributesEditor() const {
    return myGEOAttributesEditor;
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
        myPreviousElementInspect = NULL;
    } else if (myPreviousElementDelete != NULL) {
        myViewNet->getViewParent()->getDeleteFrame()->showChildsOfMarkedAttributeCarrier();
        if (myPreviousElementDeleteWasMarked) {
            myViewNet->getViewParent()->getDeleteFrame()->markAttributeCarrier(myPreviousElementDelete);
        }
        myPreviousElementDelete = NULL;
        // Hide inspect frame and show delete frame
        hide();
        myViewNet->getViewParent()->getDeleteFrame()->show();
    }
    return 1;
}

const std::vector<GNEAttributeCarrier*>&
GNEInspectorFrame::getInspectedACs() const {
    return myACs;
}

// ===========================================================================
// GNEInspectorFrame::AttributesEditor::AttributeInput method definitions
// ===========================================================================

GNEInspectorFrame::AttributesEditor::AttributeInput::AttributeInput(GNEInspectorFrame::AttributesEditor* attributeEditorParent) :
    FXHorizontalFrame(attributeEditorParent, GUIDesignAuxiliarHorizontalFrame),
    myAttributesEditorParent(attributeEditorParent),
    myTag(SUMO_TAG_NOTHING),
    myAttr(SUMO_ATTR_NOTHING) {
    // Create and hide ButtonCombinableChoices
    myButtonCombinableChoices = new FXButton(this, "AttributeButton", 0, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButtonAttribute);
    myButtonCombinableChoices->hide();
    // Create and hide label
    myLabel = new FXLabel(this, "attributeLabel", 0, GUIDesignLabelAttribute);
    myLabel->hide();
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
    // Set actual Tag and attribute
    myTag = ACTag;
    myAttr = ACAttr;
    // Show attribute Label
    myLabel->setText(toString(myAttr).c_str());
    myLabel->show();
    // Set field depending of the type of value
    if (GNEAttributeCarrier::isBool(myTag, myAttr)) {
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
    } else if (GNEAttributeCarrier::isDiscrete(myTag, myAttr)) {
        // Obtain choices
        const std::vector<std::string> choices = GNEAttributeCarrier::discreteChoices(myTag, myAttr);
        // Check if are combinable coices
        if (choices.size() > 0 && GNEAttributeCarrier::discreteCombinableChoices(myTag, myAttr)) {
            // hide label
            myLabel->hide();
            // Show button combinable choices
            myButtonCombinableChoices->setText(toString(myAttr).c_str());
            myButtonCombinableChoices->show();
            // Show string with the values
            myTextFieldStrings->setText(value.c_str());
            myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
            myTextFieldStrings->show();
        } else {
            // fill comboBox
            myChoicesCombo->clearItems();
            for (auto it : choices) {
                myChoicesCombo->appendItem(it.c_str());
            }
            // show combo box with values
            myChoicesCombo->setNumVisible((int)choices.size());
            myChoicesCombo->setCurrentItem(myChoicesCombo->findItem(value.c_str()));
            myChoicesCombo->setTextColor(FXRGB(0, 0, 0));
            myChoicesCombo->show();
        }
    } else if (GNEAttributeCarrier::isFloat(myTag, myAttr) || GNEAttributeCarrier::isTime(myTag, myAttr)) {
        // show TextField for real/time values
        myTextFieldReal->setText(value.c_str());
        myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
        myTextFieldReal->show();
    } else if (GNEAttributeCarrier::isInt(myTag, myAttr)) {
        // Show textField for int attributes
        myTextFieldInt->setText(value.c_str());
        myTextFieldInt->setTextColor(FXRGB(0, 0, 0));
        myTextFieldInt->show();
    } else {
        // In any other case (String, list, etc.), show value as String
        myTextFieldStrings->setText(value.c_str());
        myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
        myTextFieldStrings->show();
    }
    // Show AttributeInput
    show();
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
    // hide AttributeInput
    hide();
}

 
void
GNEInspectorFrame::AttributesEditor::AttributeInput::refreshAttributeInput(const std::string &value, bool forceRefresh) {
    if(myTextFieldInt->shown()) {
        // set last valid value and restore color if onlyValid is disabled
        if(myTextFieldInt->getTextColor() == FXRGB(0, 0, 0) || forceRefresh) {
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
GNEInspectorFrame::AttributesEditor::AttributeInput::onCmdOpenAllowDisallowEditor(FXObject*, FXSelector, void*) {
    // obtain vehicles of text field and check if are valid
    std::string vehicles = myTextFieldStrings->getText().text();
    // check if values can parse
    if (canParseVehicleClasses(vehicles) == false) {
        if (myAttr == SUMO_ATTR_ALLOW) {
            vehicles = getVehicleClassNames(SVCAll, true);
        } else {
            vehicles = "";
        }
    }
    // open GNEDialog_AllowDisallow
    GNEDialog_AllowDisallow(getApp(), &vehicles).execute();
    // set obtained vehicles into TextField Strings
    myTextFieldStrings->setText((vehicles).c_str());
    onCmdSetAttribute(0, 0, 0);
    return 1;
}


long
GNEInspectorFrame::AttributesEditor::AttributeInput::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    // Declare changed value
    std::string newVal;
    bool refreshGEOAndNeteditEditors = false;
    // First, obtain the string value of the new attribute depending of their type
    if (GNEAttributeCarrier::isBool(myTag, myAttr)) {
        // Set true o false depending of the checBox
        if (myBoolCheckButton->getCheck()) {
            myBoolCheckButton->setText("true");
            newVal = "true";
        } else {
            myBoolCheckButton->setText("false");
            newVal = "false";
        }
    } else if (GNEAttributeCarrier::isDiscrete(myTag, myAttr)) {
        // Obtain choices
        const std::vector<std::string>& choices = GNEAttributeCarrier::discreteChoices(myTag, myAttr);
        // Check if are combinable choices (for example, Vehicle Types)
        if (choices.size() > 0 && GNEAttributeCarrier::discreteCombinableChoices(myTag, myAttr)) {
            // Get value obtained using AttributesEditor
            newVal = myTextFieldStrings->getText().text();
        } else {
            // Get value of ComboBox
            newVal = myChoicesCombo->getText().text();
        }
    } else if (GNEAttributeCarrier::isFloat(myTag, myAttr) || GNEAttributeCarrier::isTime(myTag, myAttr)) {
        // Check if default value of attribute must be set
        if (myTextFieldReal->getText().empty() && GNEAttributeCarrier::hasDefaultValue(myTag, myAttr)) {
            newVal = GNEAttributeCarrier::getDefaultValue<std::string>(myTag, myAttr);
            myTextFieldReal->setText(newVal.c_str());
        } else {
            // obtain value of myTextFieldReal
            newVal = myTextFieldReal->getText().text();
        }
    } else if (GNEAttributeCarrier::isInt(myTag, myAttr)) {
        // Check if default value of attribute must be set
        if (myTextFieldInt->getText().empty() && GNEAttributeCarrier::hasDefaultValue(myTag, myAttr)) {
            newVal = GNEAttributeCarrier::getDefaultValue<std::string>(myTag, myAttr);
            myTextFieldInt->setText(newVal.c_str());
        } else {
            // obtain value of myTextFieldInt
            newVal = myTextFieldInt->getText().text();
        }
    } else if (GNEAttributeCarrier::isString(myTag, myAttr)) {
        // Check if default value of attribute must be set
        if (myTextFieldStrings->getText().empty() && GNEAttributeCarrier::hasDefaultValue(myTag, myAttr)) {
            newVal = GNEAttributeCarrier::getDefaultValue<std::string>(myTag, myAttr);
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
        }
        // Set new value of attribute in all selected ACs
        for (auto it_ac : myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs()) {
            it_ac->setAttribute(myAttr, newVal, myAttributesEditorParent->getInspectorFrameParent()->getViewNet()->getUndoList());
        }
        // finish change multiple attributes
        if (myAttributesEditorParent->getInspectorFrameParent()->getInspectedACs().size() > 1) {
            myAttributesEditorParent->getInspectorFrameParent()->getViewNet()->getUndoList()->p_end();
        }
        // If previously value was incorrect, change font color to black
        if (GNEAttributeCarrier::discreteCombinableChoices(myTag, myAttr)) {
            myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
            myTextFieldStrings->killFocus();
            // in this case, we need to refresh the other values (For example, allow/Disallow objects)
            myAttributesEditorParent->refreshAttributeEditor(false, false);
        } else if (GNEAttributeCarrier::isDiscrete(myTag, myAttr)) {
            myChoicesCombo->setTextColor(FXRGB(0, 0, 0));
            myChoicesCombo->killFocus();
        } else if ((GNEAttributeCarrier::isFloat(myTag, myAttr) || GNEAttributeCarrier::isTime(myTag, myAttr))) {
            myTextFieldReal->setTextColor(FXRGB(0, 0, 0));
            myTextFieldReal->killFocus();
        } else if (GNEAttributeCarrier::isInt(myTag, myAttr) && myTextFieldStrings != 0) {
            myTextFieldInt->setTextColor(FXRGB(0, 0, 0));
            myTextFieldInt->killFocus();
        } else if (myTextFieldStrings != 0) {
            myTextFieldStrings->setTextColor(FXRGB(0, 0, 0));
            myTextFieldStrings->killFocus();
        }
        // Check if GEO and Netedit editors must be refresh
        if(refreshGEOAndNeteditEditors) {
            myAttributesEditorParent->getInspectorFrameParent()->getNeteditAttributesEditor()->refreshNeteditAttributesEditor(true);
            myAttributesEditorParent->getInspectorFrameParent()->getGEOAttributesEditor()->refreshGEOAttributesEditor(true);
        }
    } else {
        // If value of TextField isn't valid, change color to Red depending of type
        if (GNEAttributeCarrier::discreteCombinableChoices(myTag, myAttr)) {
            myTextFieldStrings->setTextColor(FXRGB(255, 0, 0));
            myTextFieldStrings->killFocus();
        } else if (GNEAttributeCarrier::isDiscrete(myTag, myAttr)) {
            myChoicesCombo->setTextColor(FXRGB(255, 0, 0));
            myChoicesCombo->killFocus();
        } else if ((GNEAttributeCarrier::isFloat(myTag, myAttr) || GNEAttributeCarrier::isTime(myTag, myAttr))) {
            myTextFieldReal->setTextColor(FXRGB(255, 0, 0));
        } else if (GNEAttributeCarrier::isInt(myTag, myAttr) && myTextFieldStrings != 0) {
            myTextFieldInt->setTextColor(FXRGB(255, 0, 0));
        } else if (myTextFieldStrings != 0) {
            myTextFieldStrings->setTextColor(FXRGB(255, 0, 0));
        }
        // Write Warning in console if we're in testing mode
        if (OptionsCont::getOptions().getBool("gui-testing-debug")) {
            WRITE_WARNING("Value '" + newVal + "' for attribute " + toString(myAttr) + " of " + toString(myTag) + " isn't valid");
        }
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

// ===========================================================================
// GNEInspectorFrame::AttributesEditor method definitions
// ===========================================================================

GNEInspectorFrame::AttributesEditor::AttributesEditor(GNEInspectorFrame* inspectorFrameParent) :
    FXGroupBox(inspectorFrameParent->myContentFrame, "Internal attributes", GUIDesignGroupBoxFrame),
    myInspectorFrameParent(inspectorFrameParent),
    myCurrentIndex(0) {
    // Create sufficient AttributeInput for all types of AttributeCarriers
    for (int i = 0; i < (int)GNEAttributeCarrier::getHigherNumberOfAttributes(); i++) {
        myVectorOfAttributeInputs.push_back(new AttributeInput(this));
    }
    // Create help button
    myHelpButton = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
}


void 
GNEInspectorFrame::AttributesEditor::showAttributeEditor() {
    if(myInspectorFrameParent->getInspectedACs().size() > 0) {
        // reset myCurrentIndex;
        myCurrentIndex = 0;
        // Gets tag and attributes of element
        SumoXMLTag ACFrontTag = myInspectorFrameParent->getInspectedACs().front()->getTag();
        const std::vector<SumoXMLAttr> &ACFrontAttrs = myInspectorFrameParent->getInspectedACs().front()->getAttrs();

        //  check if current AC is a Junction without TLSs (needed to hidde TLS options)
        bool disableTLSinJunctions = (dynamic_cast<GNEJunction*>(myInspectorFrameParent->getInspectedACs().front()) && (dynamic_cast<GNEJunction*>(myInspectorFrameParent->getInspectedACs().front())->getNBNode()->getControllingTLS().empty()));

        // Iterate over attributes
        for (auto it : ACFrontAttrs) {
            // disable editing for unique attributes in case of multi-selection
            if (myInspectorFrameParent->getInspectedACs().size() > 1 && GNEAttributeCarrier::isUnique(ACFrontTag, it)) {
                continue;
            }
            // Declare a set of occuring values and insert attribute's values of item
            std::set<std::string> occuringValues;
            for (auto it_ac : myInspectorFrameParent->getInspectedACs()) {
                occuringValues.insert(it_ac->getAttribute(it));
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
            if ((disableTLSinJunctions && (ACFrontTag == SUMO_TAG_JUNCTION) && ((it == SUMO_ATTR_TLTYPE) || (it == SUMO_ATTR_TLID))) == false) {
                if (myCurrentIndex < (int)myVectorOfAttributeInputs.size()) {
                    // first show AttributesEditor
                    show();
                    // show attribute
                    myVectorOfAttributeInputs[myCurrentIndex]->showAttribute(ACFrontTag, it, oss.str());
                    // update current index
                    myCurrentIndex++;
                }
                else {
                    throw ProcessError("myCurrentIndex greather than myVectorOfAttributeInputs");
                }
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
        // reset myCurrentIndex;
        myCurrentIndex = 0;
        // Declare pointer for allow/Disallow vehicles
        std::pair<GNEInspectorFrame::AttributesEditor::AttributeInput*, std::string> myAllowAttribute(NULL, "");
        std::pair<GNEInspectorFrame::AttributesEditor::AttributeInput*, std::string> myDisallowAttribute(NULL,"");
        // Gets tag and attributes of element
        SumoXMLTag ACFrontTag = myInspectorFrameParent->getInspectedACs().front()->getTag();
        const std::vector<SumoXMLAttr> &ACFrontAttrs = myInspectorFrameParent->getInspectedACs().front()->getAttrs();

        //  check if current AC is a Junction without TLSs (needed to hidde TLS options)
        bool disableTLSinJunctions = (dynamic_cast<GNEJunction*>(myInspectorFrameParent->getInspectedACs().front()) && (dynamic_cast<GNEJunction*>(myInspectorFrameParent->getInspectedACs().front())->getNBNode()->getControllingTLS().empty()));

        // Iterate over attributes
        for (auto it : ACFrontAttrs) {
            // disable editing for unique attributes in case of multi-selection
            if (myInspectorFrameParent->getInspectedACs().size() > 1 && GNEAttributeCarrier::isUnique(ACFrontTag, it)) {
                continue;
            }
            // Declare a set of occuring values and insert attribute's values of item
            std::set<std::string> occuringValues;
            for (auto it_ac : myInspectorFrameParent->getInspectedACs()) {
                occuringValues.insert(it_ac->getAttribute(it));
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
            if ((disableTLSinJunctions && (ACFrontTag == SUMO_TAG_JUNCTION) && ((it == SUMO_ATTR_TLTYPE) || (it == SUMO_ATTR_TLID))) == false) {
                if (myCurrentIndex < (int)myVectorOfAttributeInputs.size()) {
                    // refresh attribute, with a special case for allow/disallow vehicles
                    if(it == SUMO_ATTR_ALLOW) {
                        myAllowAttribute.first = myVectorOfAttributeInputs[myCurrentIndex];
                        myAllowAttribute.second = oss.str();
                    } else if (it == SUMO_ATTR_DISALLOW) {
                        myDisallowAttribute.first = myVectorOfAttributeInputs[myCurrentIndex];
                        myDisallowAttribute.second = oss.str();
                    } else {
                        // Check if refresh of Position or Shape has to be forced
                        if((it == SUMO_ATTR_SHAPE) && forceRefreshShape) {
                            myVectorOfAttributeInputs[myCurrentIndex]->refreshAttributeInput(oss.str(), true);
                        } else if ((it == SUMO_ATTR_POSITION) && forceRefreshPosition) {
                            // Refresh attributes maintain invalid values
                            myVectorOfAttributeInputs[myCurrentIndex]->refreshAttributeInput(oss.str(), true);
                        } else {
                            // Refresh attributes maintain invalid values
                            myVectorOfAttributeInputs[myCurrentIndex]->refreshAttributeInput(oss.str(), false);
                        }
                    }
                    // update current index
                    myCurrentIndex++;
                }
                else {
                    throw ProcessError("myCurrentIndex greather than myVectorOfAttributeInputs");
                }
            }
        }
        // Check special case for Allow/Disallow attributes
        if(myAllowAttribute.first && myDisallowAttribute.first) {
            // if allow attribute is valid but disallow attribute is invalid
            if(myAllowAttribute.first->isCurrentAttributeValid() && !myDisallowAttribute.first->isCurrentAttributeValid()) {
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
    if(myInspectorFrameParent->getInspectedACs().size() > 0) {
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
    if(myInspectorFrameParent->getInspectedACs().size() > 0) {
        // Check if item can be moved
        if (GNEAttributeCarrier::canBlockMovement(myInspectorFrameParent->getInspectedACs().front()->getTag())) {
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
        if (GNEAttributeCarrier::canBlockShape(myInspectorFrameParent->getInspectedACs().front()->getTag())) {
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
            }
            else {
                myCheckBoxBlockShape->setCheck(false);
                myCheckBoxBlockShape->setText("false");
            }
        }
        // check if item can block their shape
        if (GNEAttributeCarrier::canCloseShape(myInspectorFrameParent->getInspectedACs().front()->getTag())) {
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
            }
            else {
                myCheckBoxCloseShape->setCheck(false);
                myCheckBoxCloseShape->setText("false");
            }
        }
        // Check if item has another item as parent (Currently only for single Additionals)
        if (GNEAttributeCarrier::canHaveParent(myInspectorFrameParent->getInspectedACs().front()->getTag()) && (myInspectorFrameParent->getInspectedACs().size() ==1)) {
            // show NeteditAttributesEditor
            show();
            // obtain additional Parent
            GNEAdditional* additional = myInspectorFrameParent->getViewNet()->getNet()->retrieveAdditional(myInspectorFrameParent->getInspectedACs().front()->getAttribute(GNE_ATTR_PARENT));
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
            }
            else {
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
            }
            else {
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
            }
            else {
                myCheckBoxCloseShape->setCheck(false);
                myCheckBoxCloseShape->setText("false");
            }
        }
        // Check if item has another item as parent (Currently only for single Additionals)
        if (myHorizontalFrameAdditionalParent->shown() && ((myTextFieldAdditionalParent->getTextColor() == FXRGB(0, 0, 0)) || forceRefresh)) {
            // obtain additional Parent
            GNEAdditional* additional = myInspectorFrameParent->getViewNet()->getNet()->retrieveAdditional(myInspectorFrameParent->getInspectedACs().front()->getAttribute(GNE_ATTR_PARENT));
            // set Label and TextField with the Tag and ID of parent
            myLabelAdditionalParent->setText((toString(additional->getTag()) + " parent").c_str());
            myTextFieldAdditionalParent->setText(additional->getID().c_str());
        }
    }
}


long
GNEInspectorFrame::NeteditAttributesEditor::onCmdSetNeteditAttribute(FXObject* obj, FXSelector, void*) {
    // make sure that ACs has elements
    if (myInspectorFrameParent->getInspectedACs().size() > 0) {
        if(obj == myCheckBoxBlockMovement) {
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
        } else if(obj == myCheckBoxBlockShape) {
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
        } else if(obj == myTextFieldAdditionalParent) {
            if (myInspectorFrameParent->getInspectedACs().front()->isValid(GNE_ATTR_PARENT, myTextFieldAdditionalParent->getText().text())) {
                myInspectorFrameParent->getInspectedACs().front()->setAttribute(GNE_ATTR_PARENT, myTextFieldAdditionalParent->getText().text(), myInspectorFrameParent->getViewNet()->getUndoList());
                myTextFieldAdditionalParent->setTextColor(FXRGB(0, 0, 0));
            } else {
                myTextFieldAdditionalParent->setTextColor(FXRGB(255, 0, 0));
                myTextFieldAdditionalParent->killFocus();
            }
        }
        // force refresh values of AttributesEditor and GEOAttributesEditor
        myInspectorFrameParent->getAttributesEditor()->refreshAttributeEditor(true, true);
        myInspectorFrameParent->getGEOAttributesEditor()->refreshGEOAttributesEditor(true);
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
    myUseGEOLabel = new FXLabel(myUseGEOFrame, "Use GEO", 0, GUIDesignLabelAttribute);
    myUseGEOCheckButton = new FXCheckButton(myUseGEOFrame, "false", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButtonAttribute);

    // Create help button
    myHelpButton = new FXButton(this, "Help", 0, this, MID_HELP, GUIDesignButtonRectangular);
}


GNEInspectorFrame::GEOAttributesEditor::~GEOAttributesEditor() {}


void
GNEInspectorFrame::GEOAttributesEditor::showGEOAttributesEditor() {
    // make sure that ACs has elements
    if (myInspectorFrameParent->getInspectedACs().size() > 0) {
        // check if item can use a geo position
        if (GNEAttributeCarrier::canUseGeoPosition(myInspectorFrameParent->getInspectedACs().front()->getTag()) || GNEAttributeCarrier::canUseGeoShape(myInspectorFrameParent->getInspectedACs().front()->getTag())) {
            // show GEOAttributesEditor
            show();
            // Iterate over AC to obtain values
            bool value = true;
            for (auto i : myInspectorFrameParent->getInspectedACs()) {
                value &= GNEAttributeCarrier::parse<bool>(i->getAttribute(SUMO_ATTR_GEO));
            }
            // show use geo frame
            myUseGEOFrame->show();
            // set UseGEOCheckButton value of and update label
            if (value) {
                myUseGEOCheckButton->setCheck(true);
                myUseGEOCheckButton->setText("true");
            } else {
                myUseGEOCheckButton->setCheck(false);
                myUseGEOCheckButton->setText("false");
            }
            // now specify if a single position or an entire shape must be shown (note: cannot be shown both at the same time, and GEO Shape/Position only works for single selections)
            if(GNEAttributeCarrier::canUseGeoPosition(myInspectorFrameParent->getInspectedACs().front()->getTag()) && myInspectorFrameParent->getInspectedACs().size() == 1) {
                myGEOAttributeFrame->show();
                myGEOAttributeLabel->setText(toString(SUMO_ATTR_GEOPOSITION).c_str());
                myGEOAttributeTextField->setText(myInspectorFrameParent->getInspectedACs().front()->getAttribute(SUMO_ATTR_GEOPOSITION).c_str());
                myGEOAttributeTextField->setTextColor(FXRGB(0, 0, 0));
            } else if (GNEAttributeCarrier::canUseGeoShape(myInspectorFrameParent->getInspectedACs().front()->getTag()) && myInspectorFrameParent->getInspectedACs().size() == 1) {
                myGEOAttributeFrame->show();
                myGEOAttributeLabel->setText(toString(SUMO_ATTR_GEOSHAPE).c_str());
                myGEOAttributeTextField->setText(myInspectorFrameParent->getInspectedACs().front()->getAttribute(SUMO_ATTR_GEOSHAPE).c_str());
                myGEOAttributeTextField->setTextColor(FXRGB(0, 0, 0));
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
    // Check that myGEOAttributeFrame is shown
    if(myGEOAttributeFrame->shown() && ((myGEOAttributeTextField->getTextColor() == FXRGB(0, 0, 0)) || forceRefresh)) {
        if (GNEAttributeCarrier::canUseGeoPosition(myInspectorFrameParent->getInspectedACs().front()->getTag())) {
            myGEOAttributeTextField->setText(myInspectorFrameParent->getInspectedACs().front()->getAttribute(SUMO_ATTR_GEOPOSITION).c_str());
        } else if (GNEAttributeCarrier::canUseGeoShape(myInspectorFrameParent->getInspectedACs().front()->getTag())) {
            myGEOAttributeTextField->setText(myInspectorFrameParent->getInspectedACs().front()->getAttribute(SUMO_ATTR_GEOSHAPE).c_str());
        }
        myGEOAttributeTextField->setTextColor(FXRGB(0, 0, 0));
    }
}


long
GNEInspectorFrame::GEOAttributesEditor::onCmdSetGEOAttribute(FXObject* obj, FXSelector, void*) {
    // make sure that ACs has elements
    if (myInspectorFrameParent->getInspectedACs().size() > 0) {
        if (obj == myGEOAttributeTextField) {
            // Change GEO Attribute depending of type (Position or shape)
            if (GNEAttributeCarrier::canUseGeoPosition(myInspectorFrameParent->getInspectedACs().front()->getTag())) {
                if (myInspectorFrameParent->getInspectedACs().front()->isValid(SUMO_ATTR_GEOPOSITION, myGEOAttributeTextField->getText().text())) {
                    myInspectorFrameParent->getInspectedACs().front()->setAttribute(SUMO_ATTR_GEOPOSITION, myGEOAttributeTextField->getText().text(), myInspectorFrameParent->getViewNet()->getUndoList());
                    myGEOAttributeTextField->setTextColor(FXRGB(0, 0, 0));
                } else {
                    myGEOAttributeTextField->setTextColor(FXRGB(255, 0, 0));
                    myGEOAttributeTextField->killFocus();
                }
            } else if (GNEAttributeCarrier::canUseGeoShape(myInspectorFrameParent->getInspectedACs().front()->getTag())) {
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
                }
                else {
                    i->setAttribute(SUMO_ATTR_GEO, "false", myInspectorFrameParent->getViewNet()->getUndoList());
                    myUseGEOCheckButton->setText("false");
                }
            }
        }
        // force refresh values of Attributes editor and NeteditAttributesEditor
        myInspectorFrameParent->getAttributesEditor()->refreshAttributeEditor(true, true);
        myInspectorFrameParent->getNeteditAttributesEditor()->refreshNeteditAttributesEditor(true);
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
        << " - CheckBox 'use GEO' enables or disables saving position in GEO coordinates\n";
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
    myEdgeTemplate(NULL) {

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
        if(myInspectorFrameParent->getInspectedACs().size() == 1) {
            caption = ("Copy '" + myEdgeTemplate->getMicrosimID() + "' into edge '" + myInspectorFrameParent->getInspectedACs().front()->getID() + "'").c_str();
        } else {
            caption = ("Copy '" + myEdgeTemplate->getMicrosimID() + "' into " + toString(myInspectorFrameParent->getInspectedACs().size()) + " selected edges").c_str();
        }
        sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), NULL);
    }
    else {
        caption = "No edge Template Set";
        sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), NULL);
    }
    sender->handle(this, FXSEL(SEL_COMMAND, FXLabel::ID_SETSTRINGVALUE), (void*)&caption);
    return 1;
}

/****************************************************************************/
