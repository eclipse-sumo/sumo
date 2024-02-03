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
/// @file    GNEFrameAttributeModules.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2019
///
// Auxiliary class for GNEFrame Modules (only for attributes edition)
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

#include "GNEFrameAttributeModules.h"
#include "GNEFlowEditor.h"


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEFrameAttributeModules::AttributesEditorRow) AttributesEditorRowMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,                  GNEFrameAttributeModules::AttributesEditorRow::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_BOOL,             GNEFrameAttributeModules::AttributesEditorRow::onCmdSelectCheckButton),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_COLOR,            GNEFrameAttributeModules::AttributesEditorRow::onCmdOpenColorDialog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_ALLOW,            GNEFrameAttributeModules::AttributesEditorRow::onCmdOpenAllowDialog),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_INSPECTPARENT,    GNEFrameAttributeModules::AttributesEditorRow::onCmdInspectParent),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MOVEUP,                         GNEFrameAttributeModules::AttributesEditorRow::onCmdMoveElementLaneUp),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MOVEDOWN,                       GNEFrameAttributeModules::AttributesEditorRow::onCmdMoveElementLaneDown)
};

FXDEFMAP(GNEFrameAttributeModules::AttributesEditor) AttributesEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,   GNEFrameAttributeModules::AttributesEditor::onCmdAttributesEditorHelp)
};

FXDEFMAP(GNEFrameAttributeModules::AttributesEditorExtended) AttributesEditorExtendedMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE_DIALOG,   GNEFrameAttributeModules::AttributesEditorExtended::onCmdOpenDialog)
};

FXDEFMAP(GNEFrameAttributeModules::GenericDataAttributes) GenericDataAttributesMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_OPEN_PARAMETERS_DIALOG, GNEFrameAttributeModules::GenericDataAttributes::onCmdEditParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNEFrameAttributeModules::GenericDataAttributes::onCmdSetParameters)
};

FXDEFMAP(GNEFrameAttributeModules::ParametersEditor) ParametersEditorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_OPEN_PARAMETERS_DIALOG, GNEFrameAttributeModules::ParametersEditor::onCmdEditParameters),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,          GNEFrameAttributeModules::ParametersEditor::onCmdSetParameters)
};

// Object implementation
FXIMPLEMENT(GNEFrameAttributeModules::AttributesEditorRow,          FXHorizontalFrame,      AttributesEditorRowMap,         ARRAYNUMBER(AttributesEditorRowMap))
FXIMPLEMENT(GNEFrameAttributeModules::AttributesEditor,             MFXGroupBoxModule,      AttributesEditorMap,            ARRAYNUMBER(AttributesEditorMap))
FXIMPLEMENT(GNEFrameAttributeModules::AttributesEditorExtended,     MFXGroupBoxModule,      AttributesEditorExtendedMap,    ARRAYNUMBER(AttributesEditorExtendedMap))
FXIMPLEMENT(GNEFrameAttributeModules::GenericDataAttributes,        MFXGroupBoxModule,      GenericDataAttributesMap,       ARRAYNUMBER(GenericDataAttributesMap))
FXIMPLEMENT(GNEFrameAttributeModules::ParametersEditor,             MFXGroupBoxModule,      ParametersEditorMap,            ARRAYNUMBER(ParametersEditorMap))


// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEFrameAttributeModules::AttributesEditorRow - methods
// ---------------------------------------------------------------------------

GNEFrameAttributeModules::AttributesEditorRow::AttributesEditorRow(GNEFrameAttributeModules::AttributesEditor* attributeEditorParent, const GNEAttributeProperties& ACAttr,
        const std::string& value, const bool attributeEnabled, const bool computed, GNEAttributeCarrier* ACParent) :
    FXHorizontalFrame(attributeEditorParent->getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame),
    myAttributesEditorParent(attributeEditorParent),
    myACAttr(ACAttr),
    myACParent(ACParent) {
    // build attribute elements
    buildAttributeElements(attributeEnabled, computed);
    // build value elements
    buildValueElements(attributeEnabled, computed);
    // only create if parent was created
    if (getParent()->id()) {
        // create AttributesEditorRow
        FXHorizontalFrame::create();
        // Set field depending of the type of value
        if (myACAttr.isBool()) {
            // first we need to check if all boolean values are equal
            bool allBooleanValuesEqual = true;
            // declare  boolean vector
            std::vector<bool> booleanVector;
            // check if value can be parsed to a boolean vector
            if (GNEAttributeCarrier::canParse<std::vector<bool> >(value)) {
                booleanVector = GNEAttributeCarrier::parse<std::vector<bool> >(value);
            }
            // iterate over passed booleans comparing all element with the first
            for (const auto& booleanValue : booleanVector) {
                if (booleanValue != booleanVector.front()) {
                    allBooleanValuesEqual = false;
                }
            }
            // use checkbox or textfield depending if all booleans are equal
            if (allBooleanValuesEqual) {
                // set check button
                if ((booleanVector.size() > 0) && booleanVector.front()) {
                    myValueCheckButton->setCheck(true);
                    myValueCheckButton->setText("true");
                } else {
                    myValueCheckButton->setCheck(false);
                    myValueCheckButton->setText("false");
                }
                // show check button
                myValueCheckButton->show();
            } else {
                // show list of bools (0 1)
                myValueTextField->setText(value.c_str());
                myValueTextField->show();
            }
        } else if (myACAttr.isDiscrete()) {
            // Check if are VClasses
            if (myAttributeButton) {
                // Show string with the values
                myValueTextField->setText(value.c_str());
                myValueTextField->show();
            } else {
                // fill comboBox
                fillComboBox(value);
                myValueComboBox->show();
            }
        } else if (myACAttr.isVType()) {
            // fill comboBox
            fillComboBox(value);
            myValueComboBox->show();
        } else {
            // In any other case (String, list, etc.), show value as String
            myValueTextField->setText(value.c_str());
            myValueTextField->show();
        }
        // check if update lane buttons
        if (myValueLaneUpButton && myValueLaneDownButton) {
            updateMoveLaneButtons(value);
        }
        // Show AttributesEditorRow
        show();
    }
}


void
GNEFrameAttributeModules::AttributesEditorRow::destroy() {
    // only destroy if parent was created
    if (getParent()->id()) {
        FXHorizontalFrame::destroy();
    }
}


void
GNEFrameAttributeModules::AttributesEditorRow::refreshAttributesEditorRow(const std::string& value,
        const bool forceRefreshAttribute, const bool attributeEnabled, const bool computed, GNEAttributeCarrier* ACParent) {
    // update ACParent
    myACParent = ACParent;
    // refresh attribute elements
    refreshAttributeElements(value, attributeEnabled, computed);
    refreshValueElements(value, attributeEnabled, computed, forceRefreshAttribute);
}


bool
GNEFrameAttributeModules::AttributesEditorRow::isAttributesEditorRowValid() const {
    return ((myValueTextField->getTextColor() == FXRGB(0, 0, 0)) || (myValueTextField->getTextColor() == FXRGB(0, 0, 255))) &&
           ((myValueComboBox->getTextColor() == FXRGB(0, 0, 0)) || (myValueComboBox->getTextColor() == FXRGB(0, 0, 255)));
}


long
GNEFrameAttributeModules::AttributesEditorRow::onCmdOpenColorDialog(FXObject*, FXSelector, void*) {
    GNEViewNet* viewNet = myAttributesEditorParent->getFrameParent()->getViewNet();
    const auto& ACs = myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers();
    // create FXColorDialog
    FXColorDialog colordialog(this, TL("Color Dialog"));
    colordialog.setTarget(this);
    colordialog.setIcon(GUIIconSubSys::getIcon(GUIIcon::COLORWHEEL));
    // If previous attribute wasn't correct, set black as default color
    if (GNEAttributeCarrier::canParse<RGBColor>(myValueTextField->getText().text())) {
        colordialog.setRGBA(MFXUtils::getFXColor(GNEAttributeCarrier::parse<RGBColor>(myValueTextField->getText().text())));
    } else if (!myACAttr.getDefaultValue().empty()) {
        colordialog.setRGBA(MFXUtils::getFXColor(GNEAttributeCarrier::parse<RGBColor>(myACAttr.getDefaultValue())));
    } else {
        colordialog.setRGBA(MFXUtils::getFXColor(RGBColor::BLACK));
    }
    // execute dialog to get a new color
    if (colordialog.execute()) {
        std::string newValue = toString(MFXUtils::getRGBColor(colordialog.getRGBA()));
        myValueTextField->setText(newValue.c_str());
        if (viewNet->getInspectedAttributeCarriers().front()->isValid(myACAttr.getAttr(), newValue)) {
            // if its valid for the first AC than its valid for all (of the same type)
            if (ACs.size() > 1) {
                viewNet->getUndoList()->begin(ACs.front(), TL("change multiple attributes"));
            }
            // Set new value of attribute in all selected ACs
            for (const auto& inspectedAC : viewNet->getInspectedAttributeCarriers()) {
                inspectedAC->setAttribute(myACAttr.getAttr(), newValue, viewNet->getUndoList());
            }
            // finish change multiple attributes
            if (ACs.size() > 1) {
                viewNet->getUndoList()->end();
            }
            // If previously value was incorrect, change font color to black
            myValueTextField->setTextColor(FXRGB(0, 0, 0));
            myValueTextField->killFocus();
        }
    }
    return 1;
}


long
GNEFrameAttributeModules::AttributesEditorRow::onCmdOpenAllowDialog(FXObject*, FXSelector, void*) {
    GNEViewNet* viewNet = myAttributesEditorParent->getFrameParent()->getViewNet();
    const auto& ACs = myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers();
    // if its valid for the first AC than its valid for all (of the same type)
    if (ACs.size() > 1) {
        viewNet->getUndoList()->begin(ACs.front(), TL("change multiple attributes"));
    }
    // declare accept changes
    bool acceptChanges = false;
    // open GNEAllowVClassesDialog (also used to modify SUMO_ATTR_CHANGE_LEFT etc
    GNEAllowVClassesDialog(viewNet, viewNet->getInspectedAttributeCarriers().front(),
                           myACAttr.getAttr(), &acceptChanges).execute();
    // continue depending of acceptChanges
    if (acceptChanges) {
        std::string allowed = viewNet->getInspectedAttributeCarriers().front()->getAttribute(myACAttr.getAttr());
        // Set new value of attribute in all selected ACs
        for (const auto& inspectedAC : viewNet->getInspectedAttributeCarriers()) {
            inspectedAC->setAttribute(myACAttr.getAttr(), allowed, viewNet->getUndoList());
        }
        // finish change multiple attributes
        if (ACs.size() > 1) {
            viewNet->getUndoList()->end();
        }
        // update frame parent after attribute successfully set
        myAttributesEditorParent->getFrameParent()->attributeUpdated(myACAttr.getAttr());
    }
    return 1;
}


long
GNEFrameAttributeModules::AttributesEditorRow::onCmdInspectParent(FXObject*, FXSelector, void*) {
    auto viewnet = myAttributesEditorParent->getFrameParent()->getViewNet();
    viewnet->getViewParent()->getInspectorFrame()->inspectChild(myACParent, viewnet->getInspectedAttributeCarriers().front());
    return 1;
}


long
GNEFrameAttributeModules::AttributesEditorRow::onCmdMoveElementLaneUp(FXObject*, FXSelector, void*) {
    // get view net
    auto viewNet = myAttributesEditorParent->getFrameParent()->getViewNet();
    // get inspected ACs (for code cleaning)
    auto AC = viewNet->getInspectedAttributeCarriers().front();
    // extract lane
    auto lane = viewNet->getNet()->getAttributeCarriers()->retrieveLane(AC->getAttribute(SUMO_ATTR_LANE));
    // set next lane
    AC->setAttribute(SUMO_ATTR_LANE, lane->getParentEdge()->getID() + "_" + toString(lane->getIndex() + 1), viewNet->getUndoList());
    // update frame parent after attribute successfully set
    myAttributesEditorParent->getFrameParent()->attributeUpdated(myACAttr.getAttr());
    return 1;
}


long
GNEFrameAttributeModules::AttributesEditorRow::onCmdMoveElementLaneDown(FXObject*, FXSelector, void*) {
    // get view net
    auto viewNet = myAttributesEditorParent->getFrameParent()->getViewNet();
    // get inspected ACs (for code cleaning)
    auto AC = viewNet->getInspectedAttributeCarriers().front();
    // extract lane
    auto lane = viewNet->getNet()->getAttributeCarriers()->retrieveLane(AC->getAttribute(SUMO_ATTR_LANE));
    // set next lane
    AC->setAttribute(SUMO_ATTR_LANE, lane->getParentEdge()->getID() + "_" + toString(lane->getIndex() - 1), viewNet->getUndoList());
    // update frame parent after attribute successfully set
    myAttributesEditorParent->getFrameParent()->attributeUpdated(myACAttr.getAttr());
    return 1;
}


long
GNEFrameAttributeModules::AttributesEditorRow::onCmdSetAttribute(FXObject*, FXSelector, void*) {
    // Declare changed value
    std::string newVal;
    // First, obtain the string value of the new attribute depending of their type
    if (myACAttr.isBool()) {
        // first check if we're editing boolean as a list of string or as a checkbox
        if (myValueCheckButton->shown()) {
            // Set true o false depending of the checkBox
            if (myValueCheckButton->getCheck()) {
                myValueCheckButton->setText("true");
                newVal = "true";
            } else {
                myValueCheckButton->setText("false");
                newVal = "false";
            }
        } else {
            // obtain boolean value of myValueTextField (because we're inspecting multiple attribute carriers with different values)
            newVal = myValueTextField->getText().text();
        }
    } else if (myACAttr.isDiscrete()) {
        // check if we have the new value in a text field or in the combo box
        if (myValueTextField->shown()) {
            // Get value obtained using AttributesEditor
            newVal = myValueTextField->getText().text();
        } else {
            // Get value of ComboBox
            newVal = myValueComboBox->getText().text();
        }
    } else if (myACAttr.isVType()) {
        // Get value of ComboBox
        newVal = myValueComboBox->getText().text();
    } else {
        // Check if default value of attribute must be set
        if (myValueTextField->getText().empty() && myACAttr.hasDefaultValue()) {
            newVal = myACAttr.getDefaultValue();
            myValueTextField->setText(newVal.c_str());
        } else if (myACAttr.isInt() && GNEAttributeCarrier::canParse<double>(myValueTextField->getText().text())) {
            // filter int attributes
            double doubleValue = GNEAttributeCarrier::parse<double>(myValueTextField->getText().text());
            // check if myValueTextField has to be updated
            if ((doubleValue - (int)doubleValue) == 0) {
                newVal = toString((int)doubleValue);
                myValueTextField->setText(newVal.c_str(), FALSE);
            }
        } else if ((myACAttr.getAttr() == SUMO_ATTR_ANGLE) && GNEAttributeCarrier::canParse<double>(myValueTextField->getText().text())) {
            // filter angle
            double angle = GNEAttributeCarrier::parse<double>(myValueTextField->getText().text());
            // filter if angle isn't between [0,360]
            if ((angle < 0) || (angle > 360)) {
                // apply module
                angle = fmod(angle, 360);
            }
            // set newVal
            newVal = toString(angle);
            // update Textfield
            myValueTextField->setText(newVal.c_str(), FALSE);
        } else {
            // obtain value of myValueTextField
            newVal = myValueTextField->getText().text();
        }
    }
    // we need a extra check for Position and Shape Values, due #2658
    if ((myACAttr.getAttr() == SUMO_ATTR_POSITION) || (myACAttr.getAttr() == SUMO_ATTR_SHAPE)) {
        newVal = stripWhitespaceAfterComma(newVal);
    }
    // get inspected ACs (for code cleaning)
    const auto& inspectedACs = myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers();
    // check if use default value
    const bool useDefaultValue = (newVal.empty() && myACAttr.hasDefaultValue());
    // Check if attribute must be changed
    if ((inspectedACs.size() > 0) && (inspectedACs.front()->isValid(myACAttr.getAttr(), newVal) || useDefaultValue)) {
        // check if we're merging junction
        if (!mergeJunction(myACAttr.getAttr(), inspectedACs, newVal)) {
            // if its valid for the first AC than its valid for all (of the same type)
            if (inspectedACs.size() > 1) {
                myAttributesEditorParent->getFrameParent()->getViewNet()->getUndoList()->begin(inspectedACs.front(), TL("change multiple attributes"));
            } else if (myACAttr.getAttr() == SUMO_ATTR_ID) {
                // IDs attribute has to be encapsulated
                myAttributesEditorParent->getFrameParent()->getViewNet()->getUndoList()->begin(inspectedACs.front(), TL("change ") + myACAttr.getTagPropertyParent().getTagStr() + TL(" attribute"));
            }
            // Set new value of attribute in all selected ACs
            for (const auto& inspectedAC : inspectedACs) {
                if (useDefaultValue) {
                    inspectedAC->setAttribute(myACAttr.getAttr(), myACAttr.getDefaultValue(), myAttributesEditorParent->getFrameParent()->getViewNet()->getUndoList());
                } else {
                    inspectedAC->setAttribute(myACAttr.getAttr(), newVal, myAttributesEditorParent->getFrameParent()->getViewNet()->getUndoList());
                }
            }
            // finish change multiple attributes or ID Attributes
            if (inspectedACs.size() > 1) {
                myAttributesEditorParent->getFrameParent()->getViewNet()->getUndoList()->end();
            } else if (myACAttr.getAttr() == SUMO_ATTR_ID) {
                myAttributesEditorParent->getFrameParent()->getViewNet()->getUndoList()->end();
            }
            // reset colors
            if (myValueTextField->shown()) {
                myValueTextField->setTextColor(FXRGB(0, 0, 0));
                myValueTextField->setBackColor(FXRGB(255, 255, 255));
                myValueTextField->killFocus();
            } else if (myValueComboBox->shown()) {
                myValueComboBox->setTextColor(FXRGB(0, 0, 0));
                myValueComboBox->setBackColor(FXRGB(255, 255, 255));
            }
            // in this case, we need to refresh the other values (For example, allow/Disallow objects)
            myAttributesEditorParent->refreshAttributeEditor(false, false);
            // update frame parent after attribute successfully set
            myAttributesEditorParent->getFrameParent()->attributeUpdated(myACAttr.getAttr());
        }
    } else {
        // If value of TextField isn't valid, change color to Red depending of type
        if (myValueTextField->shown()) {
            myValueTextField->setTextColor(FXRGB(255, 0, 0));
            if (newVal.empty()) {
                myValueTextField->setBackColor(FXRGBA(255, 213, 213, 255));
            }
        } else if (myValueComboBox->shown()) {
            myValueComboBox->setTextColor(FXRGB(255, 0, 0));
            if (newVal.empty()) {
                myValueComboBox->setBackColor(FXRGBA(255, 213, 213, 255));
            }
        }
        // Write Warning in console if we're in testing mode
        WRITE_DEBUG(TL("Value '") + newVal + TL("' for attribute ") + myACAttr.getAttrStr() + TL(" of ") + myACAttr.getTagPropertyParent().getTagStr() + TL(" isn't valid"));
    }
    return 1;
}


long
GNEFrameAttributeModules::AttributesEditorRow::onCmdSelectCheckButton(FXObject*, FXSelector, void*) {
    const auto& ACs = myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers();
    // obtain undoList (To improve code legibly)
    GNEUndoList* undoList = myAttributesEditorParent->getFrameParent()->getViewNet()->getUndoList();
    // check if we have to enable or disable
    if (myAttributeCheckButton->getCheck()) {
        // enable input values
        myValueCheckButton->enable();
        myValueTextField->enable();
        // enable attribute
        undoList->begin(ACs.front(), TL("enable attribute '") + myACAttr.getAttrStr() + "'");
        ACs.front()->enableAttribute(myACAttr.getAttr(), undoList);
        undoList->end();
    } else {
        // disable input values
        myValueCheckButton->disable();
        myValueTextField->disable();
        // disable attribute
        undoList->begin(ACs.front(), TL("disable attribute '") + myACAttr.getAttrStr() + "'");
        ACs.front()->disableAttribute(myACAttr.getAttr(), undoList);
        undoList->end();
    }
    return 0;
}


GNEFrameAttributeModules::AttributesEditorRow::AttributesEditorRow() :
    myAttributesEditorParent(nullptr) {
}


std::string
GNEFrameAttributeModules::AttributesEditorRow::stripWhitespaceAfterComma(const std::string& stringValue) {
    std::string result(stringValue);
    while (result.find(", ") != std::string::npos) {
        result = StringUtils::replace(result, ", ", ",");
    }
    return result;
}


bool
GNEFrameAttributeModules::AttributesEditorRow::mergeJunction(SumoXMLAttr attr, const std::vector<GNEAttributeCarrier*>& inspectedACs, const std::string& newVal) const {
    // check if we're editing junction position
    if ((inspectedACs.size() == 1) && (inspectedACs.front()->getTagProperty().getTag() == SUMO_TAG_JUNCTION) && (attr == SUMO_ATTR_POSITION)) {
        // retrieve original junction
        GNEJunction* movedJunction = myAttributesEditorParent->getFrameParent()->getViewNet()->getNet()->getAttributeCarriers()->retrieveJunction(inspectedACs.front()->getID());
        // parse position
        const Position newPosition = GNEAttributeCarrier::parse<Position>(newVal);
        // iterate over network junction
        for (const auto& junction : myAttributesEditorParent->getFrameParent()->getViewNet()->getNet()->getAttributeCarriers()->getJunctions()) {
            // check distance position
            if ((junction.second.second->getPositionInView().distanceTo2D(newPosition) < POSITION_EPS) &&
                    myAttributesEditorParent->getFrameParent()->getViewNet()->mergeJunctions(movedJunction, junction.second.second)) {
                return true;
            }
        }
    }
    // nothing to merge
    return false;
}


void
GNEFrameAttributeModules::AttributesEditorRow::buildAttributeElements(const bool attributeEnabled, const bool computed) {
    // get static tooltip menu
    const auto tooltipMenu = myAttributesEditorParent->getFrameParent()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu();
    // check if disable element
    const bool disableRow = !isSupermodeValid(myAttributesEditorParent->getFrameParent()->getViewNet(), myACAttr);
    // continue depending of attribute
    if (myACParent) {
        myAttributeButton = new MFXButtonTooltip(this, tooltipMenu, myACAttr.getAttrStr().c_str(),
                myACParent->getACIcon(), this, MID_GNE_SET_ATTRIBUTE_INSPECTPARENT, GUIDesignButtonAttribute);
        // set color text depending of computed
        myAttributeButton->setTextColor(computed ? FXRGB(0, 0, 255) : FXRGB(0, 0, 0));
        // set tip text
        myAttributeButton->setTipText(TLF("Inspect % parent", myACAttr.getAttrStr()).c_str());
        myAttributeButton->setHelpText(TLF("Inspect % parent", myACAttr.getAttrStr()).c_str());
        // check if disable
        if (disableRow) {
            myAttributeButton->disable();
        }
    } else if (myACAttr.isActivatable()) {
        myAttributeCheckButton = new FXCheckButton(this, myACAttr.getAttrStr().c_str(), this, MID_GNE_SET_ATTRIBUTE_BOOL, GUIDesignCheckButtonAttribute);
        // check
        if (attributeEnabled) {
            myAttributeCheckButton->setCheck(TRUE);
        } else {
            myAttributeCheckButton->setCheck(FALSE);
        }
        // set color text depending of computed
        myAttributeCheckButton->setTextColor(computed ? FXRGB(0, 0, 255) : FXRGB(0, 0, 0));
        // check if disable
        if (disableRow) {
            myAttributeCheckButton->disable();
        }
    } else if (myACAttr.isSVCPermission() && (myACAttr.getAttr() != SUMO_ATTR_DISALLOW)) {
        myAttributeButton = new MFXButtonTooltip(this, tooltipMenu, myACAttr.getAttrStr().c_str(), nullptr, this, MID_GNE_SET_ATTRIBUTE_ALLOW, GUIDesignButtonAttribute);
        // set tip text
        myAttributeButton->setTipText(TL("Open dialog for editing vClasses"));
        myAttributeButton->setHelpText(TL("Open dialog for editing vClasses"));
        // check if disable
        if (!attributeEnabled || disableRow) {
            myAttributeButton->disable();
        }
    } else if (myACAttr.isColor()) {
        myAttributeButton = new MFXButtonTooltip(this, tooltipMenu, myACAttr.getAttrStr().c_str(), nullptr, this, MID_GNE_SET_ATTRIBUTE_COLOR, GUIDesignButtonAttribute);
        // set icon
        myAttributeButton->setIcon(GUIIconSubSys::getIcon(GUIIcon::COLORWHEEL));
        // set color text depending of computed
        myAttributeButton->setTextColor(computed ? FXRGB(0, 0, 255) : FXRGB(0, 0, 0));
        // set tip text
        myAttributeButton->setTipText(TL("Open dialog for editing color"));
        myAttributeButton->setHelpText(TL("Open dialog for editing color"));
        // check if disable
        if (disableRow) {
            myAttributeButton->disable();
        }
    } else {
        // Create label
        myAttributeLabel = new MFXLabelTooltip(this, tooltipMenu, myACAttr.getAttrStr().c_str(), nullptr, GUIDesignLabelThickedFixed(100));
        // set tip text
        myAttributeLabel->setTipText(myACAttr.getDefinition().c_str());
        myAttributeLabel->setHelpText(myACAttr.getDefinition().c_str());
    }
}


void
GNEFrameAttributeModules::AttributesEditorRow::buildValueElements(const bool attributeEnabled, const bool computed) {
    // Create and hide MFXTextFieldTooltip for string attributes
    myValueTextField = new MFXTextFieldTooltip(this,
            myAttributesEditorParent->getFrameParent()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu(),
            GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myValueTextField->hide();
    // set color text depending of computed
    myValueTextField->setTextColor(computed ? FXRGB(0, 0, 255) : FXRGB(0, 0, 0));
    // Create and hide ComboBox
    myValueComboBox = new MFXComboBoxIcon(this, GUIDesignComboBoxNCol, true, GUIDesignComboBoxVisibleItemsMedium,
                                          this, MID_GNE_SET_ATTRIBUTE, GUIDesignComboBoxAttribute);
    myValueComboBox->hide();
    // set color text depending of computed
    myValueComboBox->setTextColor(computed ? FXRGB(0, 0, 255) : FXRGB(0, 0, 0));
    // Create and hide checkButton
    myValueCheckButton = new FXCheckButton(this, "", this, MID_GNE_SET_ATTRIBUTE, GUIDesignCheckButton);
    myValueCheckButton->hide();
    // set color text depending of computed
    myValueCheckButton->setTextColor(computed ? FXRGB(0, 0, 255) : FXRGB(0, 0, 0));
    // check if create move up/down lanes
    if ((myACAttr.getAttr() == SUMO_ATTR_LANE) && !myACAttr.getTagPropertyParent().isNetworkElement()) {
        // get static tooltip menu
        const auto tooltipMenu = myAttributesEditorParent->getFrameParent()->getViewNet()->getViewParent()->getGNEAppWindows()->getStaticTooltipMenu();
        // create move lane up
        myValueLaneUpButton = new MFXButtonTooltip(this, tooltipMenu, "",
                GUIIconSubSys::getIcon(GUIIcon::ARROW_UP), this, MID_GNE_MOVEUP, GUIDesignButtonIcon);
        // set tip texts
        myValueLaneUpButton->setTipText(TL("Move element up one lane"));
        myValueLaneUpButton->setHelpText(TL("Move element up one lane"));
        // create move lane down
        myValueLaneDownButton = new MFXButtonTooltip(this, tooltipMenu, "",
                GUIIconSubSys::getIcon(GUIIcon::ARROW_DOWN), this, MID_GNE_MOVEDOWN, GUIDesignButtonIcon);
        // set tip texts
        myValueLaneDownButton->setTipText(TL("Move element down one lane"));
        myValueLaneDownButton->setHelpText(TL("Move element down one lane"));
    }
    // check if disable
    if (!attributeEnabled || !isSupermodeValid(myAttributesEditorParent->getFrameParent()->getViewNet(), myACAttr)) {
        myValueTextField->disable();
        myValueComboBox->disable();
        myValueCheckButton->disable();
        if (myValueLaneUpButton) {
            myValueLaneUpButton->disable();
        }
        if (myValueLaneDownButton) {
            myValueLaneDownButton->disable();
        }
    }
}


void
GNEFrameAttributeModules::AttributesEditorRow::refreshAttributeElements(const std::string& value, const bool attributeEnabled, const bool computed) {
    // check if disable element
    const bool disableElement = !isSupermodeValid(myAttributesEditorParent->getFrameParent()->getViewNet(), myACAttr);
    // continue depending of attribute element
    if (myAttributeCheckButton) {
        // set color text depending of computed
        myAttributeCheckButton->setTextColor(computed ? FXRGB(0, 0, 255) : FXRGB(0, 0, 0));
        // check
        if (attributeEnabled) {
            myAttributeCheckButton->setCheck(TRUE);
        } else {
            myAttributeCheckButton->setCheck(FALSE);
        }
        // check if disable
        if (disableElement) {
            myAttributeCheckButton->disable();
        } else {
            myAttributeCheckButton->enable();
        }
    } else if (myAttributeButton) {
        if (myAttributeButton->getSelector() == MID_GNE_SET_ATTRIBUTE_INSPECTPARENT) {
            // update text and icon
            if (myACParent->getTagProperty().getTag() == SUMO_TAG_VTYPE_DISTRIBUTION) {
                myAttributeButton->setText("vTypeDist.");
            } else {
                myAttributeButton->setText(myACParent->getTagStr().c_str());
            }
            myAttributeButton->setIcon(myACParent->getACIcon());
            // set tip text
            myAttributeButton->setTipText(TLF("Inspect % parent", myACAttr.getAttrStr()).c_str());
            myAttributeButton->setHelpText(TLF("Inspect % parent", myACAttr.getAttrStr()).c_str());
            // set color text depending of computed
            myAttributeButton->setTextColor(computed ? FXRGB(0, 0, 255) : FXRGB(0, 0, 0));
        } else {
            // set color text depending of computed
            myAttributeButton->setTextColor(computed ? FXRGB(0, 0, 255) : FXRGB(0, 0, 0));
        }
        // check if disable
        if (!attributeEnabled || disableElement) {
            myAttributeButton->disable();
        } else {
            myAttributeButton->enable();
        }
    }
    // check if update lane buttons
    if (myValueLaneUpButton && myValueLaneDownButton) {
        updateMoveLaneButtons(value);
    }
}


void
GNEFrameAttributeModules::AttributesEditorRow::refreshValueElements(const std::string& value, const bool attributeEnabled, const bool computed, const bool forceRefreshAttribute) {
    // check if disable element
    const bool disableElement = !attributeEnabled || !isSupermodeValid(myAttributesEditorParent->getFrameParent()->getViewNet(), myACAttr);
    // continue depending of shown element
    if (myValueTextField->shown()) {
        // set last valid value and restore color if onlyValid is disabled
        if (forceRefreshAttribute ||
                (myValueTextField->getTextColor() == FXRGB(0, 0, 0)) ||
                (myValueTextField->getTextColor() == FXRGB(0, 0, 255))) {
            myValueTextField->setText(value.c_str());
        }
        // set color text depending of computed
        myValueTextField->setTextColor(computed ? FXRGB(0, 0, 255) : FXRGB(0, 0, 0));
        // check if disable
        if (disableElement) {
            myValueTextField->disable();
        } else {
            myValueTextField->enable();
        }
    } else if (myValueComboBox->shown()) {
        // fill comboBox
        fillComboBox(value);
        // set color text depending of computed
        myValueComboBox->setTextColor(computed ? FXRGB(0, 0, 255) : FXRGB(0, 0, 0));
        // check if disable
        if (disableElement) {
            myValueComboBox->disable();
        } else {
            myValueComboBox->enable();
        }
    } else if (myValueCheckButton->shown()) {
        if (GNEAttributeCarrier::canParse<bool>(value)) {
            myValueCheckButton->setCheck(GNEAttributeCarrier::parse<bool>(value));
        } else {
            myValueCheckButton->setCheck(false);
        }
        // check if disable
        if (myValueCheckButton) {
            myValueComboBox->disable();
        } else {
            myValueComboBox->enable();
        }
    }
}


void
GNEFrameAttributeModules::AttributesEditorRow::fillComboBox(const std::string& value) {
    const auto inspectedACs = myAttributesEditorParent->getFrameParent()->getViewNet()->getInspectedAttributeCarriers();
    // clear and enable comboBox
    myValueComboBox->clearItems();
    // fill depeding of ACAttr
    if (myACAttr.getAttr() == SUMO_ATTR_VCLASS) {
        // add all vClasses with their icons
        for (const auto& vClassStr : SumoVehicleClassStrings.getStrings()) {
            myValueComboBox->appendIconItem(vClassStr.c_str(), VClassIcons::getVClassIcon(getVehicleClassID(vClassStr)));
        }
    } else if (myACAttr.isVType()) {
        // get ACs
        const auto& ACs = myAttributesEditorParent->getFrameParent()->getViewNet()->getNet()->getAttributeCarriers();
        // fill comboBox with all vTypes and vType distributions sorted by ID
        std::map<std::string, GNEDemandElement*> sortedTypes;
        for (const auto& type : ACs->getDemandElements().at(SUMO_TAG_VTYPE)) {
            sortedTypes[type.second->getID()] = type.second;
        }
        for (const auto& sortedType : sortedTypes) {
            myValueComboBox->appendIconItem(sortedType.first.c_str(), sortedType.second->getACIcon());
        }
        sortedTypes.clear();
        for (const auto& typeDistribution : ACs->getDemandElements().at(SUMO_TAG_VTYPE_DISTRIBUTION)) {
            sortedTypes[typeDistribution.second->getID()] = typeDistribution.second;
        }
        for (const auto& sortedType : sortedTypes) {
            myValueComboBox->appendIconItem(sortedType.first.c_str(), sortedType.second->getACIcon());
        }
    } else if (myACAttr.getAttr() == SUMO_ATTR_ICON) {
        // add all POIIcons with their icons
        for (const auto& POIIcon : SUMOXMLDefinitions::POIIcons.getValues()) {
            myValueComboBox->appendIconItem(SUMOXMLDefinitions::POIIcons.getString(POIIcon).c_str(), POIIcons::getPOIIcon(POIIcon));
        }
    } else if ((myACAttr.getAttr() == SUMO_ATTR_RIGHT_OF_WAY) && (inspectedACs.size() == 1) &&
               (inspectedACs.front()->getTagProperty().getTag() == SUMO_TAG_JUNCTION)) {
        // special case for junction types
        if (inspectedACs.front()->getAttribute(SUMO_ATTR_TYPE) == "priority") {
            myValueComboBox->appendIconItem(SUMOXMLDefinitions::RightOfWayValues.getString(RightOfWay::DEFAULT).c_str(), nullptr);
            myValueComboBox->appendIconItem(SUMOXMLDefinitions::RightOfWayValues.getString(RightOfWay::EDGEPRIORITY).c_str(), nullptr);
        } else if (inspectedACs.front()->getAttribute(SUMO_ATTR_TYPE) == "traffic_light") {
            myValueComboBox->appendIconItem(SUMOXMLDefinitions::RightOfWayValues.getString(RightOfWay::DEFAULT).c_str(), nullptr);
            myValueComboBox->appendIconItem(SUMOXMLDefinitions::RightOfWayValues.getString(RightOfWay::MIXEDPRIORITY).c_str(), nullptr);
            myValueComboBox->appendIconItem(SUMOXMLDefinitions::RightOfWayValues.getString(RightOfWay::ALLWAYSTOP).c_str(), nullptr);
        } else {
            myValueComboBox->disable();
        }
    } else {
        // fill comboBox with discrete values
        for (const auto& discreteValue : myACAttr.getDiscreteValues()) {
            myValueComboBox->appendIconItem(discreteValue.c_str(), nullptr);
        }
    }
    // set current value
    const auto index = myValueComboBox->findItem(value.c_str());
    if (index < 0) {
        if (myValueComboBox->getNumItems() > 0) {
            myValueComboBox->setCurrentItem(0);
        } else {
            myValueComboBox->disable();
        }
    } else {
        myValueComboBox->setCurrentItem(index);
    }
}


void
GNEFrameAttributeModules::AttributesEditorRow::updateMoveLaneButtons(const std::string& value) {
    // first extract lane
    const auto lane = myAttributesEditorParent->getFrameParent()->getViewNet()->getNet()->getAttributeCarriers()->retrieveLane(value, false);
    // check lane
    if (lane) {
        // check if disable move up
        if ((lane->getIndex() + 1) >= (int)lane->getParentEdge()->getLanes().size()) {
            myValueLaneUpButton->disable();
        } else {
            myValueLaneUpButton->enable();
        }
        // check if disable move down
        if ((lane->getIndex() - 1) < 0) {
            myValueLaneDownButton->disable();
        } else {
            myValueLaneDownButton->enable();
        }
    }
    if (!isSupermodeValid(myAttributesEditorParent->getFrameParent()->getViewNet(), myACAttr)) {
        myValueLaneUpButton->disable();
        myValueLaneDownButton->disable();
    }
}

// ---------------------------------------------------------------------------
// GNEFrameAttributeModules::AttributesEditor - methods
// ---------------------------------------------------------------------------

GNEFrameAttributeModules::AttributesEditor::AttributesEditor(GNEFrame* frameParent) :
    MFXGroupBoxModule(frameParent, TL("Internal attributes")),
    myFrameParent(frameParent),
    myIncludeExtended(true) {
    // resize myAttributesEditorRows
    myAttributesEditorRows.resize(GNEAttributeCarrier::MAXNUMBEROFATTRIBUTES, nullptr);
    // create myAttributesFlowEditor
    myAttributesEditorFlow = new GNEFlowEditor(frameParent->getViewNet(), frameParent);
    // leave it hidden
    myAttributesEditorFlow->hideFlowEditor();
    // Create help button
    myHelpButton = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Help"), "", "", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
}


void
GNEFrameAttributeModules::AttributesEditor::showAttributeEditorModule(bool includeExtended) {
    myIncludeExtended = includeExtended;
    // first remove all rows
    for (auto& row : myAttributesEditorRows) {
        // destroy and delete all rows
        if (row != nullptr) {
            row->destroy();
            delete row;
            row = nullptr;
        }
    }
    // get inspected ACs
    const auto& ACs = myFrameParent->getViewNet()->getInspectedAttributeCarriers();
    // declare flag to check if flow editor has to be shown
    bool showFlowEditor = false;
    if (ACs.size() > 0) {
        // Iterate over attributes
        for (const auto& attrProperty : ACs.front()->getTagProperty()) {
            // declare flag to show/hide attribute
            bool editAttribute = true;
            // disable editing for unique attributes in case of multi-selection
            if ((ACs.size() > 1) && attrProperty.isUnique()) {
                editAttribute = false;
            }
            // disable editing of extended attributes if includeExtended isn't enabled
            if (attrProperty.isExtended() && !includeExtended) {
                editAttribute = false;
            }
            // disable editing of flow definition attributes, but enable flow editor
            if (attrProperty.isFlowDefinition()) {
                editAttribute = false;
                showFlowEditor = true;
            }
            // continue if attribute is editable
            if (editAttribute) {
                // Declare a set of occurring values and insert attribute's values of item (note: We use a set to avoid repeated values)
                std::set<std::string> occurringValues;
                // iterate over edited attributes
                for (const auto& inspectedAC : ACs) {
                    occurringValues.insert(inspectedAC->getAttribute(attrProperty.getAttr()));
                }
                // get current value
                std::ostringstream oss;
                for (auto values = occurringValues.begin(); values != occurringValues.end(); values++) {
                    if (values != occurringValues.begin()) {
                        oss << " ";
                    }
                    oss << *values;
                }
                // obtain value to be shown in row
                std::string value = oss.str();
                // declare a flag for enabled attributes
                bool attributeEnabled = ACs.front()->isAttributeEnabled(attrProperty.getAttr());
                // overwrite value if attribute is disabled (used by LinkIndex)
                if (attributeEnabled == false) {
                    value = ACs.front()->getAlternativeValueForDisabledAttributes(attrProperty.getAttr());
                }
                // for types, the following attributes must be always enabled
                if (ACs.front()->getTagProperty().isType() &&
                        ((attrProperty.getAttr() == SUMO_ATTR_LENGTH) ||
                         (attrProperty.getAttr() == SUMO_ATTR_MINGAP) ||
                         (attrProperty.getAttr() == SUMO_ATTR_MAXSPEED) ||
                         (attrProperty.getAttr() == SUMO_ATTR_DESIRED_MAXSPEED))) {
                    attributeEnabled = true;
                }
                // extra check for Triggered and container Triggered
                if (ACs.front()->getTagProperty().isVehicleStop()) {
                    if ((attrProperty.getAttr() == SUMO_ATTR_EXPECTED) && (ACs.front()->isAttributeEnabled(SUMO_ATTR_TRIGGERED) == false)) {
                        attributeEnabled = false;
                    } else if ((attrProperty.getAttr() == SUMO_ATTR_EXPECTED_CONTAINERS) && (ACs.front()->isAttributeEnabled(SUMO_ATTR_CONTAINER_TRIGGERED) == false)) {
                        attributeEnabled = false;
                    }
                }
                // check if this attribute is computed
                const bool computed = (ACs.size() > 1) ? false : ACs.front()->isAttributeComputed(attrProperty.getAttr());
                // if is a Vtype, obtain icon
                GNEAttributeCarrier* ACParent = nullptr;
                if ((ACs.size() == 1) && attrProperty.isVType()) {
                    if (attrProperty.getAttr() == SUMO_ATTR_TYPE) {
                        ACParent = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, ACs.front()->getAttribute(SUMO_ATTR_TYPE), false);
                        if (ACParent == nullptr) {
                            ACParent = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE_DISTRIBUTION, ACs.front()->getAttribute(SUMO_ATTR_TYPE), false);
                        }
                    }
                }
                // create attribute editor row
                myAttributesEditorRows[attrProperty.getPositionListed()] = new AttributesEditorRow(this, attrProperty, value, attributeEnabled, computed, ACParent);
            }
        }
        // check if Flow editor has to be shown
        if (showFlowEditor) {
            myAttributesEditorFlow->showFlowEditor(ACs);
        } else {
            myAttributesEditorFlow->hideFlowEditor();
        }
        // show AttributesEditor
        show();
    } else {
        myAttributesEditorFlow->hideFlowEditor();
    }
    // reparent help button (to place it at bottom)
    myHelpButton->reparent(this);
}


void
GNEFrameAttributeModules::AttributesEditor::hideAttributesEditorModule() {
    // hide AttributesEditorFlowModule
    myAttributesEditorFlow->hideFlowEditor();
    // hide also AttributesEditor
    hide();
}


void
GNEFrameAttributeModules::AttributesEditor::refreshAttributeEditor(bool forceRefreshShape, bool forceRefreshPosition) {
    // get inspected ACs
    const auto& ACs = myFrameParent->getViewNet()->getInspectedAttributeCarriers();
    // first check if there is inspected attribute carriers
    if (ACs.size() > 0) {
        // Iterate over inspected attribute carriers
        for (const auto& attrProperty : ACs.front()->getTagProperty()) {
            // declare flag to show/hide attribute
            bool editAttribute = true;
            // disable editing for unique attributes in case of multi-selection
            if ((ACs.size() > 1) && attrProperty.isUnique()) {
                editAttribute = false;
            }
            // disable editing of extended attributes if includeExtended isn't enabled
            if (attrProperty.isExtended() && !myIncludeExtended) {
                editAttribute = false;
            }
            // disable editing of flow definition attributes, but enable flow editor
            if (attrProperty.isFlowDefinition()) {
                editAttribute = false;
            }
            // continue if attribute is editable
            if (editAttribute) {
                // Declare a set of occurring values and insert attribute's values of item (note: We use a set to avoid repeated values)
                std::set<std::string> occurringValues;
                // iterate over edited attributes
                for (const auto& inspectedAC : ACs) {
                    occurringValues.insert(inspectedAC->getAttribute(attrProperty.getAttr()));
                }
                // get current value
                std::ostringstream oss;
                for (auto values = occurringValues.begin(); values != occurringValues.end(); values++) {
                    if (values != occurringValues.begin()) {
                        oss << " ";
                    }
                    oss << *values;
                }
                // obtain value to be shown in row
                std::string value = oss.str();
                // declare a flag for enabled attributes
                bool attributeEnabled = ACs.front()->isAttributeEnabled(attrProperty.getAttr());
                // for types, the following attributes must be always enabled
                if (ACs.front()->getTagProperty().isType() &&
                        ((attrProperty.getAttr() == SUMO_ATTR_LENGTH) ||
                         (attrProperty.getAttr() == SUMO_ATTR_MINGAP) ||
                         (attrProperty.getAttr() == SUMO_ATTR_MAXSPEED) ||
                         (attrProperty.getAttr() == SUMO_ATTR_DESIRED_MAXSPEED))) {
                    attributeEnabled = true;
                }
                // overwrite value if attribute is disabled (used by LinkIndex)
                if (attributeEnabled == false) {
                    value = ACs.front()->getAlternativeValueForDisabledAttributes(attrProperty.getAttr());
                }
                // extra check for Triggered and container Triggered
                if (ACs.front()->getTagProperty().isVehicleStop()) {
                    if ((attrProperty.getAttr() == SUMO_ATTR_EXPECTED) && (ACs.front()->isAttributeEnabled(SUMO_ATTR_TRIGGERED) == false)) {
                        attributeEnabled = false;
                    } else if ((attrProperty.getAttr() == SUMO_ATTR_EXPECTED_CONTAINERS) && (ACs.front()->isAttributeEnabled(SUMO_ATTR_CONTAINER_TRIGGERED) == false)) {
                        attributeEnabled = false;
                    }
                }
                // check if this attribute is computed
                const bool computed = (ACs.size() > 1) ? false : ACs.front()->isAttributeComputed(attrProperty.getAttr());
                // Check if Position or Shape refresh has to be forced
                if ((attrProperty.getAttr() == SUMO_ATTR_SHAPE) && forceRefreshShape) {
                    myAttributesEditorRows[attrProperty.getPositionListed()]->refreshAttributesEditorRow(value, true, attributeEnabled, computed, nullptr);
                } else if ((attrProperty.getAttr()  == SUMO_ATTR_POSITION) && forceRefreshPosition) {
                    myAttributesEditorRows[attrProperty.getPositionListed()]->refreshAttributesEditorRow(value, true, attributeEnabled, computed, nullptr);
                } else if (attrProperty.isVType() && (attrProperty.getTagPropertyParent().isVehicle() || attrProperty.getTagPropertyParent().isPerson() ||
                                                      attrProperty.getTagPropertyParent().isContainer())) {
                    // get type/distribution parent
                    auto typeParent = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE, ACs.front()->getAttribute(SUMO_ATTR_TYPE), false);
                    if (typeParent == nullptr) {
                        typeParent = myFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveDemandElement(SUMO_TAG_VTYPE_DISTRIBUTION, ACs.front()->getAttribute(SUMO_ATTR_TYPE), false);
                    }
                    myAttributesEditorRows[attrProperty.getPositionListed()]->refreshAttributesEditorRow(value, false, attributeEnabled, computed, typeParent);
                } else {
                    // Refresh attributes maintain invalid values
                    myAttributesEditorRows[attrProperty.getPositionListed()]->refreshAttributesEditorRow(value, false, attributeEnabled, computed, nullptr);
                }
            }
        }
        // check if flow editor has to be update
        if (myAttributesEditorFlow->shownFlowEditor()) {
            myAttributesEditorFlow->refreshFlowEditor();
        }
    }
}


GNEFrame*
GNEFrameAttributeModules::AttributesEditor::getFrameParent() const {
    return myFrameParent;
}


long
GNEFrameAttributeModules::AttributesEditor::onCmdAttributesEditorHelp(FXObject*, FXSelector, void*) {
    // open Help attributes dialog if there is inspected ACs
    if (myFrameParent->getViewNet()->getInspectedAttributeCarriers().size() > 0) {
        // open Help attributes dialog
        myFrameParent->openHelpAttributesDialog(myFrameParent->getViewNet()->getInspectedAttributeCarriers().front());
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrameAttributeModules::AttributesEditorExtended- methods
// ---------------------------------------------------------------------------

GNEFrameAttributeModules::AttributesEditorExtended::AttributesEditorExtended(GNEFrame* frameParent) :
    MFXGroupBoxModule(frameParent, TL("Extended attributes")),
    myFrameParent(frameParent) {
    // Create open dialog button
    GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Open attributes editor"), "", "", nullptr, this, MID_GNE_SET_ATTRIBUTE_DIALOG, GUIDesignButton);
}


GNEFrameAttributeModules::AttributesEditorExtended::~AttributesEditorExtended() {}


void
GNEFrameAttributeModules::AttributesEditorExtended::showAttributesEditorExtendedModule() {
    show();
}


void
GNEFrameAttributeModules::AttributesEditorExtended::hideAttributesEditorExtendedModule() {
    hide();
}


long
GNEFrameAttributeModules::AttributesEditorExtended::onCmdOpenDialog(FXObject*, FXSelector, void*) {
    // open GNEAttributesCreator extended dialog
    myFrameParent->attributesEditorExtendedDialogOpened();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrameAttributeModules::GenericDataAttributes - methods
// ---------------------------------------------------------------------------

GNEFrameAttributeModules::GenericDataAttributes::GenericDataAttributes(GNEFrame* frameParent) :
    MFXGroupBoxModule(frameParent, TL("Attributes")),
    myFrameParent(frameParent) {
    // create textfield and buttons
    myTextFieldParameters = new FXTextField(getCollapsableFrame(), GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myButtonEditParameters = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Edit attributes"), "", "", nullptr, this, MID_GNE_OPEN_PARAMETERS_DIALOG, GUIDesignButton);
}


GNEFrameAttributeModules::GenericDataAttributes::~GenericDataAttributes() {}


void
GNEFrameAttributeModules::GenericDataAttributes::showGenericDataAttributes() {
    // refresh GenericDataAttributes
    refreshGenericDataAttributes();
    // show groupbox
    show();
}


void
GNEFrameAttributeModules::GenericDataAttributes::hideGenericDataAttributes() {
    // hide groupbox
    hide();
}


void
GNEFrameAttributeModules::GenericDataAttributes::refreshGenericDataAttributes() {
    myTextFieldParameters->setText(getParametersStr().c_str());
    myTextFieldParameters->setTextColor(FXRGB(0, 0, 0));
    myTextFieldParameters->killFocus();
}


const Parameterised::Map&
GNEFrameAttributeModules::GenericDataAttributes::getParametersMap() const {
    return myParameters;
}


std::string
GNEFrameAttributeModules::GenericDataAttributes::getParametersStr() const {
    std::string result;
    // Generate an string using the following structure: "key1=value1|key2=value2|...
    for (const auto& parameter : myParameters) {
        result += parameter.first + "=" + parameter.second + "|";
    }
    // remove the last "|"
    if (!result.empty()) {
        result.pop_back();
    }
    return result;
}


std::vector<std::pair<std::string, std::string> >
GNEFrameAttributeModules::GenericDataAttributes::getParameters() const {
    std::vector<std::pair<std::string, std::string> > result;
    // Generate a vector string using the following structure: "<key1,value1>, <key2, value2>,...
    for (const auto& parameter : myParameters) {
        result.push_back(std::make_pair(parameter.first, parameter.second));
    }
    return result;
}


void
GNEFrameAttributeModules::GenericDataAttributes::setParameters(const std::vector<std::pair<std::string, std::string> >& parameters) {
    // declare result string
    std::string result;
    // Generate an string using the following structure: "key1=value1|key2=value2|...
    for (const auto& parameter : parameters) {
        result += parameter.first + "=" + parameter.second + "|";
    }
    // remove the last "|"
    if (!result.empty()) {
        result.pop_back();
    }
    // set result in textField (and call onCmdEditParameters)
    myTextFieldParameters->setText(result.c_str(), TRUE);
}


GNEFrame*
GNEFrameAttributeModules::GenericDataAttributes::getFrameParent() const {
    return myFrameParent;
}


bool
GNEFrameAttributeModules::GenericDataAttributes::areAttributesValid() const {
    if (myTextFieldParameters->getText().empty()) {
        return true;
    } else if (myTextFieldParameters->getTextColor() == FXRGB(255, 0, 0)) {
        return false;
    } else {
        return Parameterised::areAttributesValid(getParametersStr());
    }
}


long
GNEFrameAttributeModules::GenericDataAttributes::onCmdEditParameters(FXObject*, FXSelector, void*) {
    // write debug information
    WRITE_DEBUG("Open single parameters dialog");
    if (GNESingleParametersDialog(this).execute()) {
        // write debug information
        WRITE_DEBUG("Close single parameters dialog");
        // Refresh parameter EditorCreator
        refreshGenericDataAttributes();
    } else {
        // write debug information
        WRITE_DEBUG("Cancel single parameters dialog");
    }
    return 1;
}


long
GNEFrameAttributeModules::GenericDataAttributes::onCmdSetParameters(FXObject*, FXSelector, void*) {
    // clear current existent parameters
    myParameters.clear();
    // check if current given string is valid
    if (Parameterised::areParametersValid(myTextFieldParameters->getText().text(), true)) {
        // parsed parameters ok, then set text field black and continue
        myTextFieldParameters->setTextColor(FXRGB(0, 0, 0));
        myTextFieldParameters->killFocus();
        // obtain parameters "key=value"
        std::vector<std::string> parameters = StringTokenizer(myTextFieldParameters->getText().text(), "|", true).getVector();
        // iterate over parameters
        for (const auto& parameter : parameters) {
            // obtain key, value
            std::vector<std::string> keyParam = StringTokenizer(parameter, "=", true).getVector();
            // save it in myParameters
            myParameters[keyParam.front()] = keyParam.back();
        }
        // overwrite myTextFieldParameters (to remove duplicated parameters
        myTextFieldParameters->setText(getParametersStr().c_str(), FALSE);
    } else {
        myTextFieldParameters->setTextColor(FXRGB(255, 0, 0));
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEFrameAttributeModules::ParametersEditor - methods
// ---------------------------------------------------------------------------

GNEFrameAttributeModules::ParametersEditor::ParametersEditor(GNEInspectorFrame* inspectorFrameParent) :
    MFXGroupBoxModule(inspectorFrameParent, TL("Parameters")),
    myInspectorFrameParent(inspectorFrameParent) {
    // create textfield and buttons
    myTextFieldParameters = new FXTextField(getCollapsableFrame(), GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myButtonEditParameters = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Edit parameters"), "", "", nullptr, this, MID_GNE_OPEN_PARAMETERS_DIALOG, GUIDesignButton);
}


GNEFrameAttributeModules::ParametersEditor::ParametersEditor(GNETypeFrame* typeFrameParent) :
    MFXGroupBoxModule(typeFrameParent, TL("Parameters")),
    myTypeFrameParent(typeFrameParent) {
    // create textfield and buttons
    myTextFieldParameters = new FXTextField(getCollapsableFrame(), GUIDesignTextFieldNCol, this, MID_GNE_SET_ATTRIBUTE, GUIDesignTextField);
    myButtonEditParameters = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Edit parameters"), "", "", nullptr, this, MID_GNE_OPEN_PARAMETERS_DIALOG, GUIDesignButton);
}


GNEFrameAttributeModules::ParametersEditor::~ParametersEditor() {}


GNEViewNet*
GNEFrameAttributeModules::ParametersEditor::getViewNet() const {
    return myInspectorFrameParent ? myInspectorFrameParent->getViewNet() : myTypeFrameParent->getViewNet();
}


void
GNEFrameAttributeModules::ParametersEditor::showParametersEditor() {
    if (myInspectorFrameParent) {
        // get inspected ACs
        const auto& inspectedACs = myInspectorFrameParent->getViewNet()->getInspectedAttributeCarriers();
        if ((inspectedACs.size() > 0) && inspectedACs.front()->getTagProperty().hasParameters()) {
            // refresh ParametersEditor
            refreshParametersEditor();
            // show groupbox
            show();
        } else {
            hideParametersEditor();
        }
    } else if (myTypeFrameParent) {
        if (myTypeFrameParent->getTypeSelector()->getCurrentType() != nullptr) {
            /// refresh ParametersEditor
            refreshParametersEditor();
            // show groupbox
            show();
        } else {
            hideParametersEditor();
        }
    } else {
        hideParametersEditor();
    }
}


void
GNEFrameAttributeModules::ParametersEditor::hideParametersEditor() {
    // hide groupbox
    hide();
}


void
GNEFrameAttributeModules::ParametersEditor::refreshParametersEditor() {
    if (myInspectorFrameParent) {
        // get inspected ACs
        const auto& inspectedACs = myInspectorFrameParent->getViewNet()->getInspectedAttributeCarriers();
        // get front AC
        const GNEAttributeCarrier* frontAC = inspectedACs.size() > 0 ? inspectedACs.front() : nullptr;
        // continue depending of frontAC
        if (frontAC && frontAC->getTagProperty().hasParameters()) {
            // check if we're editing a single or a multiple AC
            if (inspectedACs.size() == 1) {
                // set text field parameters
                myTextFieldParameters->setText(frontAC->getAttribute(GNE_ATTR_PARAMETERS).c_str());
            } else if (inspectedACs.size() > 0) {
                // check if parameters of all inspected ACs are different
                std::string parameters = frontAC->getAttribute(GNE_ATTR_PARAMETERS);
                for (const auto& AC : inspectedACs) {
                    if (parameters != AC->getAttribute(GNE_ATTR_PARAMETERS)) {
                        parameters = "different parameters";
                    }
                }
                // set text field
                myTextFieldParameters->setText(parameters.c_str());
            }
            // reset color
            myTextFieldParameters->setTextColor(FXRGB(0, 0, 0));
            // disable myTextFieldParameters if Tag correspond to an network element but we're in demand mode (or vice versa), disable all elements
            if (GNEFrameAttributeModules::isSupermodeValid(myInspectorFrameParent->getViewNet(), frontAC)) {
                myTextFieldParameters->enable();
                myButtonEditParameters->enable();
            } else {
                myTextFieldParameters->disable();
                myButtonEditParameters->disable();
            }
        }
    } else if (myTypeFrameParent) {
        // get type
        GNEDemandElement* type = myTypeFrameParent->getTypeSelector()->getCurrentType();
        // continue depending of frontAC
        if (type) {
            // set text field parameters
            myTextFieldParameters->setText(type->getAttribute(GNE_ATTR_PARAMETERS).c_str());
            // reset color
            myTextFieldParameters->setTextColor(FXRGB(0, 0, 0));
            // disable myTextFieldParameters if Tag correspond to an network element but we're in demand mode (or vice versa), disable all elements
            if (GNEFrameAttributeModules::isSupermodeValid(myTypeFrameParent->getViewNet(), type)) {
                myTextFieldParameters->enable();
                myButtonEditParameters->enable();
            } else {
                myTextFieldParameters->disable();
                myButtonEditParameters->disable();
            }
        }
    }
}


GNEInspectorFrame*
GNEFrameAttributeModules::ParametersEditor::getInspectorFrameParent() const {
    return myInspectorFrameParent;
}


GNETypeFrame*
GNEFrameAttributeModules::ParametersEditor::getTypeFrameParent() const {
    return myTypeFrameParent;
}


long
GNEFrameAttributeModules::ParametersEditor::onCmdEditParameters(FXObject*, FXSelector, void*) {
    if (myInspectorFrameParent) {
        // get inspected ACs
        const auto& inspectedACs = myInspectorFrameParent->getViewNet()->getInspectedAttributeCarriers();
        // get front AC
        const GNEAttributeCarrier* frontAC = inspectedACs.size() > 0 ? inspectedACs.front() : nullptr;
        // continue depending of frontAC
        if (frontAC && frontAC->getTagProperty().hasParameters()) {
            if (inspectedACs.size() > 1) {
                // write debug information
                WRITE_DEBUG("Open multiple parameters dialog");
                // open multiple parameters dialog
                if (GNEMultipleParametersDialog(this).execute()) {
                    // write debug information
                    WRITE_DEBUG("Close multiple parameters dialog");
                    // update frame parent after attribute successfully set
                    myInspectorFrameParent->attributeUpdated(GNE_ATTR_PARAMETERS);
                    // Refresh parameter EditorInspector
                    refreshParametersEditor();
                } else {
                    // write debug information
                    WRITE_DEBUG("Cancel multiple parameters dialog");
                }
            } else {
                // write debug information
                WRITE_DEBUG("Open single parameters dialog");
                if (GNESingleParametersDialog(this).execute()) {
                    // write debug information
                    WRITE_DEBUG("Close single parameters dialog");
                    // update frame parent after attribute successfully set
                    myInspectorFrameParent->attributeUpdated(GNE_ATTR_PARAMETERS);
                    // Refresh parameter EditorInspector
                    refreshParametersEditor();
                } else {
                    // write debug information
                    WRITE_DEBUG("Cancel single parameters dialog");
                }
            }
        }
    } else if (myTypeFrameParent) {
        // get type
        GNEDemandElement* type = myTypeFrameParent->getTypeSelector()->getCurrentType();
        // continue depending of type
        if (type) {
            // write debug information
            WRITE_DEBUG("Open single parameters dialog");
            if (GNESingleParametersDialog(this).execute()) {
                // write debug information
                WRITE_DEBUG("Close single parameters dialog");
                // Refresh parameter EditorInspector
                refreshParametersEditor();
            } else {
                // write debug information
                WRITE_DEBUG("Cancel single parameters dialog");
            }
        }
    }
    return 1;
}


long
GNEFrameAttributeModules::ParametersEditor::onCmdSetParameters(FXObject*, FXSelector, void*) {
    if (myInspectorFrameParent) {
        const auto& inspectedACs = myInspectorFrameParent->getAttributesEditor()->getFrameParent()->getViewNet()->getInspectedAttributeCarriers();
        // get front AC
        GNEAttributeCarrier* frontAC = inspectedACs.size() > 0 ? inspectedACs.front() : nullptr;
        // continue depending of frontAC
        if (frontAC && frontAC->getTagProperty().hasParameters()) {
            // check if current given string is valid
            if (frontAC->isValid(GNE_ATTR_PARAMETERS, myTextFieldParameters->getText().text())) {
                // parsed parameters ok, then set text field black and continue
                myTextFieldParameters->setTextColor(FXRGB(0, 0, 0));
                myTextFieldParameters->killFocus();
                // check inspected parameters
                if (inspectedACs.size() == 1) {
                    // begin undo list
                    myInspectorFrameParent->getViewNet()->getUndoList()->begin(frontAC, "change parameters");
                    // set parameters
                    frontAC->setACParameters(myTextFieldParameters->getText().text(), myInspectorFrameParent->getViewNet()->getUndoList());
                    // end undo list
                    myInspectorFrameParent->getViewNet()->getUndoList()->end();
                } else if (inspectedACs.size() > 0) {
                    // begin undo list
                    myInspectorFrameParent->getViewNet()->getUndoList()->begin(frontAC, "change multiple parameters");
                    // set parameters in all ACs
                    for (const auto& inspectedAC : inspectedACs) {
                        inspectedAC->setACParameters(myTextFieldParameters->getText().text(), myInspectorFrameParent->getViewNet()->getUndoList());
                    }
                    // end undo list
                    myInspectorFrameParent->getViewNet()->getUndoList()->end();
                }
                // update frame parent after attribute successfully set
                myInspectorFrameParent->attributeUpdated(GNE_ATTR_PARAMETERS);
            } else {
                myTextFieldParameters->setTextColor(FXRGB(255, 0, 0));
            }
        }
    } else if (myTypeFrameParent) {
        // get type
        GNEDemandElement* type = myTypeFrameParent->getTypeSelector()->getCurrentType();
        // continue depending of frontAC
        if (type) {
            // check if current given string is valid
            if (type->isValid(GNE_ATTR_PARAMETERS, myTextFieldParameters->getText().text())) {
                // parsed parameters ok, then set text field black and continue
                myTextFieldParameters->setTextColor(FXRGB(0, 0, 0));
                myTextFieldParameters->killFocus();
                // begin undo list
                myTypeFrameParent->getViewNet()->getUndoList()->begin(type, "change parameters");
                // set parameters
                type->setACParameters(myTextFieldParameters->getText().text(), myTypeFrameParent->getViewNet()->getUndoList());
                // end undo list
                myTypeFrameParent->getViewNet()->getUndoList()->end();
            } else {
                myTextFieldParameters->setTextColor(FXRGB(255, 0, 0));
            }
        }
    }
    return 1;
}


bool
GNEFrameAttributeModules::isSupermodeValid(const GNEViewNet* viewNet, const GNEAttributeCarrier* AC) {
    if (viewNet->getEditModes().isCurrentSupermodeNetwork() &&
            (AC->getTagProperty().isNetworkElement() || AC->getTagProperty().isAdditionalElement())) {
        return true;
    } else if (viewNet->getEditModes().isCurrentSupermodeDemand() &&
               AC->getTagProperty().isDemandElement()) {
        return true;
    } else if (viewNet->getEditModes().isCurrentSupermodeData() &&
               (AC->getTagProperty().isDataElement() || AC->getTagProperty().isMeanData())) {
        return true;
    } else {
        return false;
    }
}


bool
GNEFrameAttributeModules::isSupermodeValid(const GNEViewNet* viewNet, const GNEAttributeProperties& ACAttr) {
    if (ACAttr.getTagPropertyParent().isNetworkElement() || ACAttr.getTagPropertyParent().isAdditionalElement()) {
        return (viewNet->getEditModes().isCurrentSupermodeNetwork());
    } else if (ACAttr.getTagPropertyParent().isDemandElement()) {
        return (viewNet->getEditModes().isCurrentSupermodeDemand());
    } else if (ACAttr.getTagPropertyParent().isDataElement() || ACAttr.getTagPropertyParent().isMeanData()) {
        return (viewNet->getEditModes().isCurrentSupermodeData());
    } else {
        return false;
    }
}

/****************************************************************************/
