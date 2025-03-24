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
/// @file    GNEMatchAttribute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2020
///
// The Widget for modifying selections of network-elements
/****************************************************************************/

#include <netedit/frames/common/GNESelectorFrame.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEAttributeProperties.h>
#include <netedit/GNETagPropertiesDatabase.h>
#include <utils/foxtools/MFXComboBoxAttrProperty.h>
#include <utils/foxtools/MFXComboBoxTagProperty.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEMatchAttribute.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEMatchAttribute) GNEMatchAttributeMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECTORFRAME_SELECTTAG,        GNEMatchAttribute::onCmdTagSelected),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECTORFRAME_SELECTATTRIBUTE,  GNEMatchAttribute::onCmdAttributeSelected),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECTORFRAME_PROCESSSTRING,    GNEMatchAttribute::onCmdSelMBString),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,                               GNEMatchAttribute::onCmdHelp)
};

// Object implementation
FXIMPLEMENT(GNEMatchAttribute, MFXGroupBoxModule, GNEMatchAttributeMap, ARRAYNUMBER(GNEMatchAttributeMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEMatchAttribute - methods
// ---------------------------------------------------------------------------
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4355) // mask warning about "this" in initializers
#endif
GNEMatchAttribute::GNEMatchAttribute(GNESelectorFrame* selectorFrameParent) :
    MFXGroupBoxModule(selectorFrameParent, TL("Match Attribute")),
    mySelectorFrameParent(selectorFrameParent),
    myCurrentEditedProperties(new CurrentEditedProperties(this)) {
    // Create MFXComboBoxIcons (sum 1 due children)
    for (int i = 0; i < selectorFrameParent->getViewNet()->getNet()->getTagPropertiesDatabase()->getHierarchyDepth() + 1; i++) {
        auto comboBoxIcon = new MFXComboBoxTagProperty(getCollapsableFrame(), GUIDesignComboBoxNCol, true, GUIDesignComboBoxVisibleItems,
                this, MID_GNE_SELECTORFRAME_SELECTTAG, GUIDesignComboBox);
        myTagComboBoxVector.push_back(comboBoxIcon);
    }
    // Create MFXComboBoxIcon for Attributes
    myAttributeComboBox = new MFXComboBoxAttrProperty(getCollapsableFrame(), GUIDesignComboBoxNCol, true, GUIDesignComboBoxVisibleItems,
            this, MID_GNE_SELECTORFRAME_SELECTATTRIBUTE, GUIDesignComboBox);
    // Create TextField for Match string
    myMatchString = new FXTextField(getCollapsableFrame(), GUIDesignTextFieldNCol, this, MID_GNE_SELECTORFRAME_PROCESSSTRING, GUIDesignTextField);
    // create button
    myMatchStringButton = GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Apply selection"), "", "", nullptr, this, MID_GNE_SELECTORFRAME_PROCESSSTRING, GUIDesignButton);
    // Create help button
    GUIDesigns::buildFXButton(getCollapsableFrame(), TL("Help"), "", "", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
    // refresh with the current tag and attr
    refreshMatchAttribute();
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

GNEMatchAttribute::~GNEMatchAttribute() {
    delete myCurrentEditedProperties;
}


void
GNEMatchAttribute::enableMatchAttribute() {
    for (const auto& tagComboBox : myTagComboBoxVector) {
        tagComboBox->enable();
    }
    myAttributeComboBox->enable();
    myMatchString->enable();
    myMatchStringButton->enable();
}


void
GNEMatchAttribute::disableMatchAttribute() {
    for (const auto& tagComboBox : myTagComboBoxVector) {
        tagComboBox->disable();
        tagComboBox->setTextColor(FXRGB(0, 0, 0));
    }
    myAttributeComboBox->disable();
    myMatchString->disable();
    myMatchStringButton->disable();
    // change colors to black (even if there are invalid values)
    myAttributeComboBox->setTextColor(FXRGB(0, 0, 0));
    myMatchString->setTextColor(FXRGB(0, 0, 0));
}


void
GNEMatchAttribute::showMatchAttribute() {
    // refresh before show
    refreshMatchAttribute();
    // show groupbox
    show();
}


void
GNEMatchAttribute::hideMatchAttribute() {
    // hide groupbox
    hide();
}


void
GNEMatchAttribute::refreshMatchAttribute() {
    // continue depending of current
    auto parentHierarchy = myCurrentEditedProperties->getTagProperties()->getParentHierarchy();
    const auto allTagProperty = mySelectorFrameParent->getViewNet()->getNet()->getTagPropertiesDatabase()->getTagPropertiesAll();
    // fill hierarchy
    for (size_t i = 0; i < parentHierarchy.size(); i++) {
        auto comboBox = myTagComboBoxVector.at(i);
        // clear previous elements
        comboBox->clearItems();
        // add <all> always as first element
        myTagComboBoxVector.at(i)->appendTagItem(allTagProperty);
        // add siblings (except for root)
        if (parentHierarchy.at(i)->getParent()) {
            for (const auto tagSibling : parentHierarchy.at(i)->getParent()->getChildren()) {
                myTagComboBoxVector.at(i)->appendTagItem(tagSibling);
            }
            // update tag
            myTagComboBoxVector.at(i)->setCurrentItem(parentHierarchy.at(i), FALSE);
        }
    }
    // hide the two first combo boxes(root and supermode)
    myTagComboBoxVector.at(0)->hide();
    myTagComboBoxVector.at(1)->hide();
    // check if show children
    if (parentHierarchy.back()->getChildren().size() > 0) {
        // clear previous elements
        myTagComboBoxVector.at(parentHierarchy.size())->clearItems();
        for (const auto childTagProperty : parentHierarchy.back()->getChildren()) {
            myTagComboBoxVector.at(parentHierarchy.size())->appendTagItem(childTagProperty);
        }
        myTagComboBoxVector.at(parentHierarchy.size())->show();
    } else {
        myTagComboBoxVector.at(parentHierarchy.size())->hide();
    }
    // hide rest of combo boxes
    for (size_t i = (parentHierarchy.size() + 1); i < myTagComboBoxVector.size(); i++) {
        myTagComboBoxVector.at(i)->hide();
    }
    // now fill attributes
    myAttributeComboBox->clearItems();
    // get ALL Children recursivelly)
    const auto attributes = myCurrentEditedProperties->getTagProperties()->getAllChildrenAttributes();
    for (const auto& attribute : attributes) {
        myAttributeComboBox->appendAttrItem(attribute.second);
    }
    // set match string
    myMatchString->setText(myCurrentEditedProperties->getMatchValue().c_str(), FALSE);
}


long
GNEMatchAttribute::onCmdTagSelected(FXObject* obj, FXSelector, void*) {
    // iterate over all comboBoxes
    int tagComboBoxIndex = 0;
    for (int i = 0; i < (int)myTagComboBoxVector.size(); i++) {
        if (myTagComboBoxVector.at(i) == obj) {
            tagComboBoxIndex = i;
        }
    }
    // check if tag property exist
    if (myTagComboBoxVector.at(tagComboBoxIndex)->getCurrentTagProperty()) {
        myCurrentEditedProperties->setTagProperties(myTagComboBoxVector.at(tagComboBoxIndex)->getCurrentTagProperty());
        refreshMatchAttribute();
    }
    return 0;
}


long
GNEMatchAttribute::onCmdAttributeSelected(FXObject*, FXSelector, void*) {
    myCurrentEditedProperties->setAttributeProperties(myAttributeComboBox->getCurrentAttrProperty());
    refreshMatchAttribute();
    return 1;
}


long
GNEMatchAttribute::onCmdSelMBString(FXObject*, FXSelector, void*) {
    /*
        // obtain expresion
        std::string expr(myMatchString->getText().text());
        const auto tagProperty = myElementSet->getSelectorFrameParent()->getViewNet()->getNet()->getTagPropertiesDatabase()->getTagProperty(myCurrentTag);
        bool valid = true;
        if (expr == "") {
            // the empty expression matches all objects
            myElementSet->getSelectorFrameParent()->handleIDs(myElementSet->getSelectorFrameParent()->getMatches(myCurrentTag, myCurrentAttribute, '@', 0, expr));
        } else if (tagProperty->hasAttribute(myCurrentAttribute) && tagProperty->getAttributeProperties(myCurrentAttribute)->isNumerical()) {
            // The expression must have the form
            //  <val matches if attr < val
            //  >val matches if attr > val
            //  =val matches if attr = val
            //  val matches if attr = val
            char compOp = expr[0];
            if (compOp == '<' || compOp == '>' || compOp == '=') {
                expr = expr.substr(1);
            } else {
                compOp = '=';
            }
            // check if value can be parsed to double
            if (GNEAttributeCarrier::canParse<double>(expr.c_str())) {
                myElementSet->getSelectorFrameParent()->handleIDs(myElementSet->getSelectorFrameParent()->getMatches(myCurrentTag, myCurrentAttribute, compOp, GNEAttributeCarrier::parse<double>(expr.c_str()), expr));
            } else {
                valid = false;
            }
        } else {
            // The expression must have the form
            //   =str: matches if <str> is an exact match
            //   !str: matches if <str> is not a substring
            //   ^str: matches if <str> is not an exact match
            //   str: matches if <str> is a substring (sends compOp '@')
            // Alternatively, if the expression is empty it matches all objects
            char compOp = expr[0];
            if (compOp == '=' || compOp == '!' || compOp == '^') {
                expr = expr.substr(1);
            } else {
                compOp = '@';
            }
            myElementSet->getSelectorFrameParent()->handleIDs(myElementSet->getSelectorFrameParent()->getMatches(myCurrentTag, myCurrentAttribute, compOp, 0, expr));
        }
        if (valid) {
            myMatchString->setTextColor(FXRGB(0, 0, 0));
            myMatchString->killFocus();
            myMatchStringButton->enable();
        } else {
            myMatchString->setTextColor(FXRGB(255, 0, 0));
            myMatchStringButton->disable();
        }
    */
    return 1;

}


long
GNEMatchAttribute::onCmdHelp(FXObject*, FXSelector, void*) {
    // Create dialog box
    FXDialogBox* additionalNeteditAttributesHelpDialog = new FXDialogBox(getCollapsableFrame(), TL("Netedit Parameters Help"), GUIDesignDialogBox);
    additionalNeteditAttributesHelpDialog->setIcon(GUIIconSubSys::getIcon(GUIIcon::MODEADDITIONAL));
    // set help text
    std::ostringstream help;
    help
            << TL("- The 'Match Attribute' controls allow to specify a set of objects which are then applied to the current selection\n")
            << TL("  according to the current 'Modification Mode'.\n")
            << TL("     1. Select an object type from the first input box\n")
            << TL("     2. Select an attribute from the second input box\n")
            << TL("     3. Enter a 'match expression' in the third input box and press <return>\n")
            << "\n"
            << TL("- The empty expression matches all objects\n")
            << TL("- For numerical attributes the match expression must consist of a comparison operator ('<', '>', '=') and a number.\n")
            << TL("- An object matches if the comparison between its attribute and the given number by the given operator evaluates to 'true'\n")
            << "\n"
            << TL("- For string attributes the match expression must consist of a comparison operator ('', '=', '!', '^') and a string.\n")
            << TL("     '' (no operator) matches if string is a substring of that object's attribute.\n")
            << TL("     '=' matches if string is an exact match.\n")
            << TL("     '!' matches if string is not a substring.\n")
            << TL("     '^' matches if string is not an exact match.\n")
            << "\n"
            << TL("- Examples:\n")
            << TL("     junction; id; 'foo' -> match all junctions that have 'foo' in their id\n")
            << TL("     junction; type; '=priority' -> match all junctions of type 'priority', but not of type 'priority_stop'\n")
            << TL("     edge; speed; '>10' -> match all edges with a speed above 10\n");
    // Create label with the help text
    new FXLabel(additionalNeteditAttributesHelpDialog, help.str().c_str(), nullptr, GUIDesignLabelFrameInformation);
    // Create horizontal separator
    new FXHorizontalSeparator(additionalNeteditAttributesHelpDialog, GUIDesignHorizontalSeparator);
    // Create frame for OK Button
    FXHorizontalFrame* myHorizontalFrameOKButton = new FXHorizontalFrame(additionalNeteditAttributesHelpDialog, GUIDesignAuxiliarHorizontalFrame);
    // Create Button Close (And two more horizontal frames to center it)
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    GUIDesigns::buildFXButton(myHorizontalFrameOKButton, TL("OK"), "", TL("close"), GUIIconSubSys::getIcon(GUIIcon::ACCEPT), additionalNeteditAttributesHelpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    // create Dialog
    additionalNeteditAttributesHelpDialog->create();
    // show in the given position
    additionalNeteditAttributesHelpDialog->show(PLACEMENT_CURSOR);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    getApp()->runModalFor(additionalNeteditAttributesHelpDialog);
    return 1;
}


void
GNEMatchAttribute::updateTag() {
    /*
    // declare tag index
    int tagIndex = -1;
    // fill combo box tags
    myMatchTagComboBox->clearItems();
    myMatchTagComboBox->setTextColor(FXRGB(0, 0, 0));
    // itreate over myTagProperties
    for (int i = 0; i < (int)myTagProperties.size(); i++) {
        // add tag in combo Box
        myMatchTagComboBox->appendIconItem(myTagProperties.at(i)->getSelectorText().c_str(), GUIIconSubSys::getIcon(myTagProperties.at(i)->getGUIIcon()));
        // check tag index
        if (myTagProperties.at(i)->getTag() == myCurrentTag) {
            tagIndex = i;
        }
    }
    // check tagIndex
    if (tagIndex == -1) {
        myMatchTagComboBox->setCurrentItem(0);
        myCurrentTag = myTagProperties.front()->getTag();
    } else {
        myMatchTagComboBox->setCurrentItem(tagIndex);
    }
    */
}


void
GNEMatchAttribute::updateAttribute() {
    /*
        // first check if tag is valid
        if (myCurrentTag != SUMO_TAG_NOTHING) {
            // now continue with attributes
            const auto tagProperty = myElementSet->getSelectorFrameParent()->getViewNet()->getNet()->getTagPropertiesDatabase()->getTagProperty(myCurrentTag);
            // set color and enable items
            myMatchAttrComboBox->enable();
            myMatchAttrComboBox->setTextColor(FXRGB(0, 0, 0));
            myMatchAttrComboBox->clearItems();
            // declare attr index
            int attrIndex = -1;
            // fill attributeProperty combo box
            for (int i = 0; i < (int)tagProperty->getNumberOfAttributes(); i++) {
                myMatchAttrComboBox->appendIconItem(tagProperty->at(i)->getAttrStr().c_str());
                // check attr index
                if (tagProperty->at(i)->getAttr() == myCurrentAttribute) {
                    attrIndex = i;
                }
            }
            // check attrIndex
            if (attrIndex == -1) {
                myMatchAttrComboBox->setCurrentItem(0);
                if (tagProperty->getNumberOfAttributes() > 0) {
                    myCurrentAttribute = tagProperty->getAttributeProperties().front()->getAttr();
                }
            } else {
                myMatchAttrComboBox->setCurrentItem(attrIndex);
            }
            // enable mach string
            myMatchString->enable();
            myMatchStringButton->enable();
        } else {
            // disable myMatchAttrComboBox
            myMatchAttrComboBox->disable();
            // disable mach string
            myMatchString->disable();
            myMatchStringButton->disable();
        }
    */
}

// ---------------------------------------------------------------------------
// GNEMatchAttribute::CurrentEditedProperties - methods
// ---------------------------------------------------------------------------

GNEMatchAttribute::CurrentEditedProperties::CurrentEditedProperties(const GNEMatchAttribute* matchAttributeParent) :
    myMatchAttributeParent(matchAttributeParent) {
    const auto database = myMatchAttributeParent->mySelectorFrameParent->getViewNet()->getNet()->getTagPropertiesDatabase();
    // set default tag and attribute for every property
    setTagProperties(database->getTagProperty(SUMO_TAG_EDGE, true));
    setAttributeProperties(myNetworkTagProperties.back()->getAttributeProperties(SUMO_ATTR_SPEED));
    myNetworkMatchValue = ">= 10";
    setTagProperties(database->getTagProperty(SUMO_TAG_VEHICLE, true));
    setAttributeProperties(myNetworkTagProperties.back()->getAttributeProperties(SUMO_ATTR_ID));
    setTagProperties(database->getTagProperty(SUMO_TAG_DATASET, true));
    setAttributeProperties(myNetworkTagProperties.back()->getAttributeProperties(SUMO_ATTR_ID));
}


const GNETagProperties*
GNEMatchAttribute::CurrentEditedProperties::getTagProperties() const {
    if (myMatchAttributeParent->mySelectorFrameParent->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) {
        return myNetworkTagProperties.back();
    } else if (myMatchAttributeParent->mySelectorFrameParent->getViewNet()->getEditModes().isCurrentSupermodeDemand()) {
        return myDemandTagProperties.back();
    } else if (myMatchAttributeParent->mySelectorFrameParent->getViewNet()->getEditModes().isCurrentSupermodeData()) {
        return myDataTagProperties.back();
    } else {
        throw ProcessError("Invalid supermode");
    }
}


const GNEAttributeProperties*
GNEMatchAttribute::CurrentEditedProperties::getAttributeProperties() const {
    if (myMatchAttributeParent->mySelectorFrameParent->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) {
        return myNetworkAttributeProperties;
    } else if (myMatchAttributeParent->mySelectorFrameParent->getViewNet()->getEditModes().isCurrentSupermodeDemand()) {
        return myDemandAttributeProperties;
    } else if (myMatchAttributeParent->mySelectorFrameParent->getViewNet()->getEditModes().isCurrentSupermodeData()) {
        return myDataAttributeProperties;
    } else {
        throw ProcessError("Invalid supermode");
    }
}


const std::string&
GNEMatchAttribute::CurrentEditedProperties::getMatchValue() const {
    if (myMatchAttributeParent->mySelectorFrameParent->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) {
        return myNetworkMatchValue;
    } else if (myMatchAttributeParent->mySelectorFrameParent->getViewNet()->getEditModes().isCurrentSupermodeDemand()) {
        return myDemandMatchValue;
    } else if (myMatchAttributeParent->mySelectorFrameParent->getViewNet()->getEditModes().isCurrentSupermodeData()) {
        return myDataMatchValue;
    } else {
        throw ProcessError("Invalid supermode");
    }
}


void
GNEMatchAttribute::CurrentEditedProperties::setTagProperties(const GNETagProperties* tagProperty) {
    if (tagProperty->getSupermode() == Supermode::NETWORK) {
        myNetworkTagProperties = tagProperty->getParentHierarchy();
    } else if (tagProperty->getSupermode() == Supermode::DEMAND) {
        myDemandTagProperties = tagProperty->getParentHierarchy();
    } else if (tagProperty->getSupermode() == Supermode::DATA) {
        myDataTagProperties = tagProperty->getParentHierarchy();
    }
}


void
GNEMatchAttribute::CurrentEditedProperties::setAttributeProperties(const GNEAttributeProperties* attrProperty) {
    if (attrProperty->getTagPropertyParent()->getSupermode() == Supermode::NETWORK) {
        myNetworkAttributeProperties = attrProperty;
    } else if (attrProperty->getTagPropertyParent()->getSupermode() == Supermode::DEMAND) {
        myDemandAttributeProperties = attrProperty;
    } else if (attrProperty->getTagPropertyParent()->getSupermode() == Supermode::DATA) {
        myDataAttributeProperties = attrProperty;
    }
}


void
GNEMatchAttribute::CurrentEditedProperties::setMatchValue(const std::string value) {
    if (myMatchAttributeParent->mySelectorFrameParent->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) {
        myNetworkMatchValue = value;
    } else if (myMatchAttributeParent->mySelectorFrameParent->getViewNet()->getEditModes().isCurrentSupermodeDemand()) {
        myDemandMatchValue = value;
    } else if (myMatchAttributeParent->mySelectorFrameParent->getViewNet()->getEditModes().isCurrentSupermodeData()) {
        myDataMatchValue = value;
    }
}

/****************************************************************************/
