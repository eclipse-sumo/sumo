/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNEMatchGenericDataAttribute.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2020
///
// The Widget for modifying selections of network-elements
/****************************************************************************/
#include <config.h>

#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/elements/data/GNEDataInterval.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

#include "GNEMatchGenericDataAttribute.h"
#include "GNEElementSet.h"

// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEMatchGenericDataAttribute) GNEMatchGenericDataAttributeMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECTORFRAME_SETINTERVAL,      GNEMatchGenericDataAttribute::onCmdSetInterval),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECTORFRAME_SETBEGIN,         GNEMatchGenericDataAttribute::onCmdSetBegin),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECTORFRAME_SETEND,           GNEMatchGenericDataAttribute::onCmdSetEnd),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECTORFRAME_SELECTTAG,        GNEMatchGenericDataAttribute::onCmdSelectTag),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECTORFRAME_SELECTATTRIBUTE,  GNEMatchGenericDataAttribute::onCmdSelectAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECTORFRAME_PROCESSSTRING,    GNEMatchGenericDataAttribute::onCmdProcessString),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,                               GNEMatchGenericDataAttribute::onCmdHelp)
};

// Object implementation
FXIMPLEMENT(GNEMatchGenericDataAttribute, FXGroupBoxModul, GNEMatchGenericDataAttributeMap, ARRAYNUMBER(GNEMatchGenericDataAttributeMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNEMatchGenericDataAttribute::GNEMatchGenericDataAttribute(GNEElementSet* elementSet, SumoXMLTag defaultTag, SumoXMLAttr defaultAttr, const std::string& defaultValue) :
    FXGroupBoxModul(elementSet->getSelectorFrameParent()->getContentFrame(), "Match GenericData Attribute"),
    myElementSet(elementSet),
    myIntervalSelector(nullptr),
    myBegin(nullptr),
    myEnd(nullptr),
    myMatchGenericDataTagComboBox(nullptr),
    myMatchGenericDataAttrComboBox(nullptr),
    myCurrentTag(defaultTag),
    myCurrentAttribute(toString(defaultAttr)),
    myMatchGenericDataString(nullptr) {
    // Create MFXIconComboBox for interval
    new FXLabel(this, "Interval [begin, end]", nullptr, GUIDesignLabelThick);
    myIntervalSelector = new MFXIconComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SELECTORFRAME_SETINTERVAL, GUIDesignComboBoxStaticExtended);
    // Create textfield for begin and end
    FXHorizontalFrame* myHorizontalFrameBeginEnd = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
    myBegin = new FXTextField(myHorizontalFrameBeginEnd, GUIDesignTextFieldNCol, this, MID_GNE_SELECTORFRAME_SETBEGIN, GUIDesignTextField);
    myEnd = new FXTextField(myHorizontalFrameBeginEnd, GUIDesignTextFieldNCol, this, MID_GNE_SELECTORFRAME_SETEND, GUIDesignTextField);
    // Create MFXIconComboBox for generic datas
    myMatchGenericDataTagComboBox = new MFXIconComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SELECTORFRAME_SELECTTAG, GUIDesignComboBox);
    // Create listBox for Attributes
    myMatchGenericDataAttrComboBox = new FXComboBox(this, GUIDesignComboBoxNCol, this, MID_GNE_SELECTORFRAME_SELECTATTRIBUTE, GUIDesignComboBox);
    // Create TextField for MatchGenericData string
    myMatchGenericDataString = new FXTextField(this, GUIDesignTextFieldNCol, this, MID_GNE_SELECTORFRAME_PROCESSSTRING, GUIDesignTextField);
    // Create help button
    new FXButton(this, "Help", nullptr, this, MID_HELP, GUIDesignButtonRectangular);
    // Fill list of sub-items (first element will be "edge")
    enableMatchGenericDataAttribute();
    // Set speed of edge as default attribute
    myMatchGenericDataAttrComboBox->setText("speed");
    myCurrentAttribute = SUMO_ATTR_SPEED;
    // Set default value for MatchGenericData string
    myMatchGenericDataString->setText(defaultValue.c_str());
}


GNEMatchGenericDataAttribute::~GNEMatchGenericDataAttribute() {}


void
GNEMatchGenericDataAttribute::enableMatchGenericDataAttribute() {
    // first drop intervals
    myIntervals.clear();
    // iterate over all data sets
    for (const auto& dataSet : myElementSet->getSelectorFrameParent()->getViewNet()->getNet()->getAttributeCarriers()->getDataSets()) {
        for (const auto& dataInterval : dataSet->getDataIntervalChildren()) {
            myIntervals[std::make_pair(dataInterval.second->getAttributeDouble(SUMO_ATTR_BEGIN), dataInterval.second->getAttributeDouble(SUMO_ATTR_END))] = -1;
        }
    }
    // disable modul if there isn't intervals
    if (myIntervals.size() == 0) {
        disableMatchGenericDataAttribute();
    } else {
        // enable comboboxes and text field
        myIntervalSelector->enable();
        myBegin->enable();
        myEnd->enable();
        myMatchGenericDataTagComboBox->enable();
        myMatchGenericDataAttrComboBox->enable();
        myMatchGenericDataString->enable();
        // clear combo box interval selector
        myIntervalSelector->clearItems();
        // fill combo Box
        for (auto& interval : myIntervals) {
            interval.second = myIntervalSelector->appendIconItem((" [" + toString(interval.first.first) + "," + toString(interval.first.second) + "]").c_str(), GUIIconSubSys::getIcon(GUIIcon::DATAINTERVAL));
        }
        // set number of visible items
        if (myIntervalSelector->getNumItems() < 10) {
            myIntervalSelector->setNumVisible(myIntervalSelector->getNumItems());
        } else {
            myIntervalSelector->setNumVisible(10);
        }
        // Clear items of myMatchGenericDataTagComboBox
        myMatchGenericDataTagComboBox->clearItems();
        // update begin and end
        myBegin->setText(toString(myIntervals.begin()->first.first).c_str());
        myBegin->setTextColor(FXRGB(0, 0, 0));
        myEnd->setText(toString(myIntervals.begin()->first.second).c_str());
        myEnd->setTextColor(FXRGB(0, 0, 0));
        // get generic datas
        const auto genericDataTags = GNEAttributeCarrier::getTagPropertiesByType(GNETagProperties::TagType::GENERICDATA);
        // fill combo box (only with drawable elements)
        for (const auto& genericDataTag : genericDataTags) {
            if (genericDataTag.isDrawable()) {
                myMatchGenericDataTagComboBox->appendIconItem(genericDataTag.getFieldString().c_str(), GUIIconSubSys::getIcon(genericDataTag.getGUIIcon()));
            }
        }
        // set first item as current item
        myMatchGenericDataTagComboBox->setCurrentItem(0);
        myMatchGenericDataTagComboBox->setNumVisible(myMatchGenericDataTagComboBox->getNumItems());
        // call select tag
        onCmdSelectTag(nullptr, 0, nullptr);
    }
}


void
GNEMatchGenericDataAttribute::disableMatchGenericDataAttribute() {
    // disable comboboxes and text field
    myIntervalSelector->disable();
    myBegin->disable();
    myEnd->disable();
    myMatchGenericDataTagComboBox->disable();
    myMatchGenericDataAttrComboBox->disable();
    myMatchGenericDataString->disable();
    // change colors to black (even if there are invalid values)
    myMatchGenericDataTagComboBox->setTextColor(FXRGB(0, 0, 0));
    myMatchGenericDataAttrComboBox->setTextColor(FXRGB(0, 0, 0));
    myMatchGenericDataString->setTextColor(FXRGB(0, 0, 0));
}


void
GNEMatchGenericDataAttribute::showMatchGenericDataAttribute() {
    // just show Modul
    show();
}


void
GNEMatchGenericDataAttribute::hideMatchGenericDataAttribute() {
    // just hide modul
    hide();
}


long
GNEMatchGenericDataAttribute::onCmdSetInterval(FXObject*, FXSelector, void*) {
    // iterate over interval and update begin and end
    for (auto& interval : myIntervals) {
        if (interval.second == myIntervalSelector->getCurrentItem()) {
            // update begin
            myBegin->setTextColor(FXRGB(0, 0, 0));
            myBegin->setText(toString(interval.first.first).c_str());
            // update end
            myEnd->setTextColor(FXRGB(0, 0, 0));
            myEnd->setText(toString(interval.first.second).c_str());
        }
    }
    // call onCmdSelectTag
    onCmdSelectTag(0, 0, 0);
    return 1;
}


long
GNEMatchGenericDataAttribute::onCmdSetBegin(FXObject*, FXSelector, void*) {
    // check if can be parsed to double
    if (GNEAttributeCarrier::canParse<double>(myBegin->getText().text()) &&
            GNEAttributeCarrier::canParse<double>(myEnd->getText().text())) {
        // set valid color text and kill focus
        myBegin->setTextColor(FXRGB(0, 0, 0));
        myBegin->killFocus();
        // enable elements
        myMatchGenericDataTagComboBox->enable();
        myMatchGenericDataAttrComboBox->enable();
        myMatchGenericDataString->enable();
        // call onCmdSelectTag
        onCmdSelectTag(0, 0, 0);
    } else {
        // set invalid color text
        myBegin->setTextColor(FXRGB(255, 0, 0));
        // disable elements
        myMatchGenericDataTagComboBox->disable();
        myMatchGenericDataAttrComboBox->disable();
        myMatchGenericDataString->disable();
    }
    return 1;
}


long
GNEMatchGenericDataAttribute::onCmdSetEnd(FXObject*, FXSelector, void*) {
    // check if can be parsed to double
    if (GNEAttributeCarrier::canParse<double>(myBegin->getText().text()) &&
            GNEAttributeCarrier::canParse<double>(myEnd->getText().text())) {
        // set valid color text and kill focus
        myEnd->setTextColor(FXRGB(0, 0, 0));
        myEnd->killFocus();
        // enable elements
        myMatchGenericDataTagComboBox->enable();
        myMatchGenericDataAttrComboBox->enable();
        myMatchGenericDataString->enable();
        // call onCmdSelectTag
        onCmdSelectTag(0, 0, 0);
    } else {
        // set invalid color text
        myEnd->setTextColor(FXRGB(255, 0, 0));
        // disable elements
        myMatchGenericDataTagComboBox->disable();
        myMatchGenericDataAttrComboBox->disable();
        myMatchGenericDataString->disable();
    }
    return 1;
}


long
GNEMatchGenericDataAttribute::onCmdSelectTag(FXObject*, FXSelector, void*) {
    // check if net has proj
    const bool notProj = (GeoConvHelper::getFinal().getProjString() == "!");
    // First check what type of elementes is being selected
    myCurrentTag = SUMO_TAG_NOTHING;
    // get generic data tags
    const auto listOfTags = GNEAttributeCarrier::getTagPropertiesByType(GNETagProperties::TagType::GENERICDATA);
    // fill myMatchGenericDataTagComboBox
    for (const auto& genericDataTag : listOfTags) {
        if (genericDataTag.isDrawable() && (genericDataTag.getFieldString() == myMatchGenericDataTagComboBox->getText().text())) {
            myCurrentTag = genericDataTag.getTag();
        }
    }
    // check that typed-by-user value is correct
    if (myCurrentTag != SUMO_TAG_NOTHING) {
        // obtain begin and end
        const double begin = GNEAttributeCarrier::parse<double>(myBegin->getText().text());
        const double end = GNEAttributeCarrier::parse<double>(myEnd->getText().text());
        // obtain all Generic Data attributes for current generic tag
        const auto attributes = myElementSet->getSelectorFrameParent()->getViewNet()->getNet()->getAttributeCarriers()->retrieveGenericDataParameters(toString(myCurrentTag), begin, end);
        // set color and enable items
        myMatchGenericDataTagComboBox->setTextColor(FXRGB(0, 0, 0));
        myMatchGenericDataAttrComboBox->enable();
        myMatchGenericDataString->enable();
        myMatchGenericDataAttrComboBox->clearItems();
        // add data set parent
        myMatchGenericDataAttrComboBox->appendItem(toString(GNE_ATTR_DATASET).c_str());
        // fill attribute combo box
        for (const auto& attribute : attributes) {
            myMatchGenericDataAttrComboBox->appendItem(attribute.c_str());
        }
        myMatchGenericDataAttrComboBox->setNumVisible(myMatchGenericDataAttrComboBox->getNumItems());
        onCmdSelectAttribute(nullptr, 0, nullptr);
    } else {
        // change color to red and disable items
        myMatchGenericDataTagComboBox->setTextColor(FXRGB(255, 0, 0));
        myMatchGenericDataAttrComboBox->disable();
        myMatchGenericDataString->disable();
    }
    update();
    return 1;
}


long
GNEMatchGenericDataAttribute::onCmdSelectAttribute(FXObject*, FXSelector, void*) {
    // obtain all Generic Data attributes for current generic tag
    auto attributes = myElementSet->getSelectorFrameParent()->getViewNet()->getNet()->getAttributeCarriers()->retrieveGenericDataParameters(
                          toString(myCurrentTag), GNEAttributeCarrier::parse<double>(myBegin->getText().text()),
                          GNEAttributeCarrier::parse<double>(myEnd->getText().text()));
    // add extra data set parent attribute
    attributes.insert(toString(GNE_ATTR_DATASET));
    // clear current attribute
    myCurrentAttribute.clear();
    // set current selected attribute
    for (const auto& attribute : attributes) {
        if (attribute == myMatchGenericDataAttrComboBox->getText().text()) {
            myCurrentAttribute = attribute;
        }
    }
    // check if selected attribute is valid
    if (myCurrentAttribute.empty()) {
        myMatchGenericDataAttrComboBox->setTextColor(FXRGB(255, 0, 0));
        myMatchGenericDataString->disable();
    } else {
        myMatchGenericDataAttrComboBox->setTextColor(FXRGB(0, 0, 0));
        myMatchGenericDataString->enable();
    }
    return 1;
}


long
GNEMatchGenericDataAttribute::onCmdProcessString(FXObject*, FXSelector, void*) {
    // obtain expression and tag value
    std::string expression = myMatchGenericDataString->getText().text();
    bool valid = true;
    // get all Generic datas
    const auto genericDatas = myElementSet->getSelectorFrameParent()->getViewNet()->getNet()->getAttributeCarriers()->retrieveGenericDatas(myCurrentTag,
                              GNEAttributeCarrier::parse<double>(myBegin->getText().text()),
                              GNEAttributeCarrier::parse<double>(myEnd->getText().text()));
    if (expression == "") {
        // the empty expression matches all objects
        myElementSet->getSelectorFrameParent()->handleIDs(myElementSet->getSelectorFrameParent()->getGenericMatches(genericDatas, myCurrentAttribute, '@', 0, expression));
    } else if (myCurrentAttribute != toString(GNE_ATTR_DATASET)) {
        // The expression must have the form
        //  <val matches if attr < val
        //  >val matches if attr > val
        //  =val matches if attr = val
        //  val matches if attr = val
        char compOp = expression[0];
        if ((compOp == '<') || (compOp == '>') || (compOp == '=')) {
            expression = expression.substr(1);
        } else {
            compOp = '=';
        }
        // check if value can be parsed to double
        if (GNEAttributeCarrier::canParse<double>(expression.c_str())) {
            myElementSet->getSelectorFrameParent()->handleIDs(myElementSet->getSelectorFrameParent()->getGenericMatches(genericDatas, myCurrentAttribute, compOp, GNEAttributeCarrier::parse<double>(expression.c_str()), expression));
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
        char compOp = expression[0];
        if ((compOp == '=') || (compOp == '!') || (compOp == '^')) {
            expression = expression.substr(1);
        } else {
            compOp = '@';
        }
        myElementSet->getSelectorFrameParent()->handleIDs(myElementSet->getSelectorFrameParent()->getGenericMatches(genericDatas, myCurrentAttribute, compOp, 0, expression));
    }
    // change color depending of flag "valid"
    if (valid) {
        myMatchGenericDataString->setTextColor(FXRGB(0, 0, 0));
        myMatchGenericDataString->killFocus();
    } else {
        myMatchGenericDataString->setTextColor(FXRGB(255, 0, 0));
    }
    return 1;
}


long
GNEMatchGenericDataAttribute::onCmdHelp(FXObject*, FXSelector, void*) {
    // Create dialog box
    FXDialogBox* additionalNeteditAttributesHelpDialog = new FXDialogBox(this, "Netedit Parameters Help", GUIDesignDialogBox);
    additionalNeteditAttributesHelpDialog->setIcon(GUIIconSubSys::getIcon(GUIIcon::MODEADDITIONAL));
    // set help text
    std::ostringstream help;
    help
            << "- The 'MatchGenericData Attribute' controls allow to specify a set of objects which are then applied to the current selection\n"
            << "  according to the current 'Modification Mode'.\n"
            << "     1. Select an object type from the first input box\n"
            << "     2. Select an attribute from the second input box\n"
            << "     3. Enter a 'match expression' in the third input box and press <return>\n"
            << "\n"
            << "- The empty expression matches all objects\n"
            << "- For numerical attributes the match expression must consist of a comparison operator ('<', '>', '=') and a number.\n"
            << "- An object matches if the comparison between its attribute and the given number by the given operator evaluates to 'true'\n"
            << "\n"
            << "- For string attributes the match expression must consist of a comparison operator ('', '=', '!', '^') and a string.\n"
            << "     '' (no operator) matches if string is a substring of that object'ts attribute.\n"
            << "     '=' matches if string is an exact match.\n"
            << "     '!' matches if string is not a substring.\n"
            << "     '^' matches if string is not an exact match.\n"
            << "\n"
            << "- Examples:\n"
            << "     junction; id; 'foo' -> match all junctions that have 'foo' in their id\n"
            << "     junction; type; '=priority' -> match all junctions of type 'priority', but not of type 'priority_stop'\n"
            << "     edge; speed; '>10' -> match all edges with a speed above 10\n";
    // Create label with the help text
    new FXLabel(additionalNeteditAttributesHelpDialog, help.str().c_str(), nullptr, GUIDesignLabelFrameInformation);
    // Create horizontal separator
    new FXHorizontalSeparator(additionalNeteditAttributesHelpDialog, GUIDesignHorizontalSeparator);
    // Create frame for OK Button
    FXHorizontalFrame* myHorizontalFrameOKButton = new FXHorizontalFrame(additionalNeteditAttributesHelpDialog, GUIDesignAuxiliarHorizontalFrame);
    // Create Button Close (And two more horizontal frames to center it)
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    new FXButton(myHorizontalFrameOKButton, "OK\t\tclose", GUIIconSubSys::getIcon(GUIIcon::ACCEPT), additionalNeteditAttributesHelpDialog, FXDialogBox::ID_ACCEPT, GUIDesignButtonOK);
    new FXHorizontalFrame(myHorizontalFrameOKButton, GUIDesignAuxiliarHorizontalFrame);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Opening help dialog of selector frame");
    // create Dialog
    additionalNeteditAttributesHelpDialog->create();
    // show in the given position
    additionalNeteditAttributesHelpDialog->show(PLACEMENT_CURSOR);
    // refresh APP
    getApp()->refresh();
    // open as modal dialog (will block all windows until stop() or stopModal() is called)
    getApp()->runModalFor(additionalNeteditAttributesHelpDialog);
    // Write Warning in console if we're in testing mode
    WRITE_DEBUG("Close help dialog of selector frame");
    return 1;
}

/****************************************************************************/
