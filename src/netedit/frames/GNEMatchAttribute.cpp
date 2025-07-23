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
#include <netedit/dialogs/GNEHelpDialog.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNEViewParent.h>
#include <netedit/GNEApplicationWindow.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEAttributeProperties.h>
#include <netedit/GNETagPropertiesDatabase.h>
#include <utils/foxtools/MFXComboBoxAttrProperty.h>
#include <utils/foxtools/MFXComboBoxTagProperty.h>
#include <utils/foxtools/MFXDialogBox.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEMatchAttribute.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEMatchAttribute) GNEMatchAttributeMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECTORFRAME_SELECTTAG,        GNEMatchAttribute::onCmdTagSelected),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECTORFRAME_SELECTATTRIBUTE,  GNEMatchAttribute::onCmdAttributeSelected),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECTORFRAME_TOGGLECOMMON,     GNEMatchAttribute::onCmdToogleOnlyCommon),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECTORFRAME_PROCESSSTRING,    GNEMatchAttribute::onCmdProcessString),
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
    myShowOnlyCommonAttributes = new FXCheckButton(getCollapsableFrame(), TL("Only common"), this, MID_GNE_SELECTORFRAME_TOGGLECOMMON, GUIDesignCheckButton);
    myShowOnlyCommonAttributes->setCheck(FALSE);
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
    auto parentHierarchy = myCurrentEditedProperties->getTagProperties()->getHierarchicalParentsRecuersively();
    // fill hierarchy
    for (size_t i = 0; i < parentHierarchy.size(); i++) {
        auto comboBox = myTagComboBoxVector.at(i);
        // clear previous elements
        comboBox->clearItems();
        // add <all> always as first element
        myTagComboBoxVector.at(i)->appendTagItem(myCurrentEditedProperties->getTagPropertiesAll());
        // add siblings (except for root)
        if (parentHierarchy.at(i)->getHierarchicalParent()) {
            for (const auto tagSibling : parentHierarchy.at(i)->getHierarchicalParent()->getHierarchicalChildren()) {
                if (tagSibling->isDrawable()) {
                    myTagComboBoxVector.at(i)->appendTagItem(tagSibling);
                }
            }
            // update tag
            if (myTagComboBoxVector.at(i)->hasTagProperty(parentHierarchy.at(i))) {
                myTagComboBoxVector.at(i)->setCurrentItem(parentHierarchy.at(i), FALSE);
                myTagComboBoxVector.at(i)->show();
            } else {
                myTagComboBoxVector.at(i)->hide();
            }
        }
    }
    // hide the two first combo boxes(root and supermode)
    myTagComboBoxVector.at(0)->hide();
    myTagComboBoxVector.at(1)->hide();
    // hide comboBox with only one element (+ <all>)
    if (myTagComboBoxVector.at(parentHierarchy.size() - 1)->getNumItems() == 2) {
        myTagComboBoxVector.at(parentHierarchy.size() - 1)->hide();
    }
    // check if show children
    auto comboBoxChildren = myTagComboBoxVector.at(parentHierarchy.size());
    if (parentHierarchy.back()->getHierarchicalChildren().size() > 0) {
        // clear previous elements
        comboBoxChildren->clearItems();
        // add <all> always as first element
        comboBoxChildren->appendTagItem(myCurrentEditedProperties->getTagPropertiesAll());
        for (const auto childTagProperty : parentHierarchy.back()->getHierarchicalChildren()) {
            comboBoxChildren->appendTagItem(childTagProperty);
        }
        comboBoxChildren->show();
    } else {
        comboBoxChildren->hide();
    }
    // hide rest of combo boxes
    for (size_t i = (parentHierarchy.size() + 1); i < myTagComboBoxVector.size(); i++) {
        myTagComboBoxVector.at(i)->hide();
    }
    // now fill attributes
    myAttributeComboBox->clearItems();
    // get all children recursivelly
    const auto attributes = myCurrentEditedProperties->getTagProperties()->getHierarchicalChildrenAttributesRecursively(myShowOnlyCommonAttributes->getCheck() == TRUE, true);
    for (const auto& attribute : attributes) {
        myAttributeComboBox->appendAttrItem(attribute.second);
    }
    // update tag
    if (myAttributeComboBox->getNumItems() == 0) {
        myAttributeComboBox->appendAttrItem(myCurrentEditedProperties->getAttributePropertiesNoCommon());
        myAttributeComboBox->disable();
        // set match string
        myMatchString->setText("", FALSE);
        myMatchString->disable();
    } else {
        myAttributeComboBox->enable();
        if (myAttributeComboBox->hasAttrProperty(myCurrentEditedProperties->getAttributeProperties())) {
            myAttributeComboBox->setCurrentItem(myCurrentEditedProperties->getAttributeProperties(), FALSE);
        } else {
            myAttributeComboBox->setCurrentItem(attributes.begin()->second, FALSE);
            myCurrentEditedProperties->setAttributeProperties(myAttributeComboBox->getCurrentAttrProperty());
        }
        // set match string
        myMatchString->setText(myCurrentEditedProperties->getMatchValue().c_str(), FALSE);
        myMatchString->enable();
    }
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
        const auto selectedTag = myTagComboBoxVector.at(tagComboBoxIndex)->getCurrentTagProperty();
        // if we select <all>, use parent tag
        if (selectedTag == myCurrentEditedProperties->getTagPropertiesAll()) {
            myCurrentEditedProperties->setTagProperties(myTagComboBoxVector.at(tagComboBoxIndex - 1)->getCurrentTagProperty());
        } else {
            myCurrentEditedProperties->setTagProperties(selectedTag);
        }
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
GNEMatchAttribute::onCmdToogleOnlyCommon(FXObject*, FXSelector, void*) {
    // simply refresh attribute
    refreshMatchAttribute();
    return 1;
}


long
GNEMatchAttribute::onCmdProcessString(FXObject*, FXSelector, void*) {
    std::vector<GNEAttributeCarrier*> matches;
    // obtain expresion
    std::string expr = myMatchString->getText().text();
    bool valid = true;
    if (expr == "") {
        // the empty expression matches all objects
        matches = getMatches('@', 0, expr);
    } else if (myCurrentEditedProperties->getAttributeProperties()->isNumerical()) {
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
            matches = getMatches(compOp, GNEAttributeCarrier::parse<double>(expr.c_str()), expr);
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
        matches = getMatches(compOp, 0, expr);
    }
    if (valid) {
        mySelectorFrameParent->handleIDs(matches);
        myMatchString->setTextColor(FXRGB(0, 0, 0));
        myMatchString->killFocus();
    } else {
        myMatchString->setTextColor(FXRGB(255, 0, 0));
    }
    return 1;

}


long
GNEMatchAttribute::onCmdHelp(FXObject*, FXSelector, void*) {
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
    // create help dialog
    return GNEHelpDialog(mySelectorFrameParent->getViewNet()->getViewParent()->getGNEAppWindows(),
                         TL("Netedit parameters Help"), help.str()).openModal();
}


std::vector<GNEAttributeCarrier*>
GNEMatchAttribute::getMatches(const char compOp, const double val, const std::string& expr) {
    std::vector<GNEAttributeCarrier*> result;
    // first retrieve all ACs using ACTag
    const auto allACbyTag = mySelectorFrameParent->getViewNet()->getNet()->getAttributeCarriers()->retrieveAttributeCarriers(myCurrentEditedProperties->getTagProperties()->getTag());
    // iterate over all ACs
    for (const auto& AC : allACbyTag) {
        // first check if the attribute exist in the given tag
        if (AC->getTagProperty()->hasAttribute(myCurrentEditedProperties->getAttributeProperties()->getAttr())) {
            if (expr == "" && compOp == '@') {
                result.push_back(AC);
            } else if (myCurrentEditedProperties->getAttributeProperties()->isNumerical()) {
                double acVal;
                std::istringstream buf(AC->getAttribute(myCurrentEditedProperties->getAttributeProperties()->getAttr()));
                buf >> acVal;
                switch (compOp) {
                    case '<':
                        if (acVal < val) {
                            result.push_back(AC);
                        }
                        break;
                    case '>':
                        if (acVal > val) {
                            result.push_back(AC);
                        }
                        break;
                    case '=':
                        if (acVal == val) {
                            result.push_back(AC);
                        }
                        break;
                }
            } else {
                // string match
                std::string acVal = AC->getAttributeForSelection(myCurrentEditedProperties->getAttributeProperties()->getAttr());
                switch (compOp) {
                    case '@':
                        if (acVal.find(expr) != std::string::npos) {
                            result.push_back(AC);
                        }
                        break;
                    case '!':
                        if (acVal.find(expr) == std::string::npos) {
                            result.push_back(AC);
                        }
                        break;
                    case '=':
                        if (acVal == expr) {
                            result.push_back(AC);
                        }
                        break;
                    case '^':
                        if (acVal != expr) {
                            result.push_back(AC);
                        }
                        break;
                }
            }
        }
    }
    return result;
}


std::vector<GNEAttributeCarrier*>
GNEMatchAttribute::getGenericMatches(const std::vector<GNEGenericData*>& genericDatas, const std::string& attr, const char compOp, const double val, const std::string& expr) {
    std::vector<GNEAttributeCarrier*> result;
    // iterate over generic datas
    for (const auto& genericData : genericDatas) {
        if (expr == "" && compOp == '@') {
            result.push_back(genericData);
        } else if (attr != toString(GNE_ATTR_PARENT)) {
            double acVal;
            std::istringstream buf(genericData->getParameter(attr, "0"));
            buf >> acVal;
            switch (compOp) {
                case '<':
                    if (acVal < val) {
                        result.push_back(genericData);
                    }
                    break;
                case '>':
                    if (acVal > val) {
                        result.push_back(genericData);
                    }
                    break;
                case '=':
                    if (acVal == val) {
                        result.push_back(genericData);
                    }
                    break;
            }
        } else {
            // string match
            std::string acVal = genericData->getAttributeForSelection(GNE_ATTR_PARENT);
            switch (compOp) {
                case '@':
                    if (acVal.find(expr) != std::string::npos) {
                        result.push_back(genericData);
                    }
                    break;
                case '!':
                    if (acVal.find(expr) == std::string::npos) {
                        result.push_back(genericData);
                    }
                    break;
                case '=':
                    if (acVal == expr) {
                        result.push_back(genericData);
                    }
                    break;
                case '^':
                    if (acVal != expr) {
                        result.push_back(genericData);
                    }
                    break;
            }
        }
    }
    return result;
}

// ---------------------------------------------------------------------1------
// GNEMatchAttribute::CurrentEditedProperties - methods
// ---------------------------------------------------------------------------

GNEMatchAttribute::CurrentEditedProperties::CurrentEditedProperties(const GNEMatchAttribute* matchAttributeParent) :
    myMatchAttributeParent(matchAttributeParent) {
    // build special attributes
    myTagPropertiesAllAttributes = new GNETagProperties(GNE_TAG_ATTRIBUTES_ALL,
            nullptr,
            GUIIcon::EMPTY,
            TL("Show all attributes"),
            FXRGBA(255, 255, 255, 255),
            TL("<all>"));
    myAttributePropertiesNoCommon = new GNEAttributeProperties(myTagPropertiesAllAttributes,
            GNE_ATTR_NOCOMMON,
            TL("No common attributes defined"));
    // set default tag and attribute for every property
    const auto database = myMatchAttributeParent->mySelectorFrameParent->getViewNet()->getNet()->getTagPropertiesDatabase();
    setTagProperties(database->getTagProperty(SUMO_TAG_EDGE, true));
    setAttributeProperties(myNetworkTagProperties.back()->getAttributeProperties(SUMO_ATTR_SPEED));
    myNetworkMatchValue = ">10";
    setTagProperties(database->getTagProperty(SUMO_TAG_VEHICLE, true));
    setAttributeProperties(myDemandTagProperties.back()->getAttributeProperties(SUMO_ATTR_ID));
    setTagProperties(database->getTagProperty(GNE_TAG_DATAS, true));
}


GNEMatchAttribute::CurrentEditedProperties::~CurrentEditedProperties() {
    delete myTagPropertiesAllAttributes;
}


const GNETagProperties*
GNEMatchAttribute::CurrentEditedProperties::getTagPropertiesAll() const {
    return myTagPropertiesAllAttributes;
}


const GNEAttributeProperties*
GNEMatchAttribute::CurrentEditedProperties::getAttributePropertiesNoCommon() const {
    return myAttributePropertiesNoCommon;
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
        myNetworkTagProperties = tagProperty->getHierarchicalParentsRecuersively();
    } else if (tagProperty->getSupermode() == Supermode::DEMAND) {
        myDemandTagProperties = tagProperty->getHierarchicalParentsRecuersively();
    } else if (tagProperty->getSupermode() == Supermode::DATA) {
        myDataTagProperties = tagProperty->getHierarchicalParentsRecuersively();
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
